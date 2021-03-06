/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/private/simulation.hpp>
#include <libaudioverse/private/node.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/implementations/delayline.hpp>
#include <libaudioverse/implementations/feedback_delay_network.hpp>
#include <libaudioverse/implementations/one_pole_filter.hpp>
#include <libaudioverse/nodes/feedback_delay_network.hpp>
#include <memory>
#include <algorithm>
#include <limits> //FDN uses infinity and -infinity in the constructor.

namespace libaudioverse_implementation {

FeedbackDelayNetworkNode::FeedbackDelayNetworkNode(std::shared_ptr<Simulation> simulation, float maxDelay, int channels):
Node(Lav_OBJTYPE_FEEDBACK_DELAY_NETWORK_NODE, simulation, channels, channels) {
	max_delay = maxDelay;
	this->channels = channels;
	network = new FeedbackDelayNetwork<InterpolatedDelayLine>(channels, maxDelay, simulation->getSr());
	last_output = allocArray<float>(channels);
	next_input=allocArray<float>(channels);
	gains = allocArray<float>(channels);
	for(int i = 0; i < channels; i++) gains[i] = 1.0f;
	getProperty(Lav_FDN_MAX_DELAY).setFloatValue(maxDelay);
	for(int i= 0; i < channels; i++) {
		appendInputConnection(i, 1);
		appendOutputConnection(i, 1);
	}

	//Allocate and configure the filters.
	filters = new OnePoleFilter*[channels];
	for(int i = 0; i < channels; i++) filters[i] = new OnePoleFilter(simulation->getSr());
	
	
	std::vector<float> default(channels, 0.0f);
	//Set up the properties.
	getProperty(Lav_FDN_DELAYS).setArrayLengthRange(channels, channels);
	getProperty(Lav_FDN_DELAYS).setFloatRange(0.0, maxDelay);
	getProperty(Lav_FDN_DELAYS).replaceFloatArray(channels, &default[0]);	
	getProperty(Lav_FDN_DELAYS).setFloatArrayDefault(default);
	
	getProperty(Lav_FDN_OUTPUT_GAINS).setArrayLengthRange(channels, channels);
	getProperty(Lav_FDN_OUTPUT_GAINS).setFloatRange(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	default.clear();
	default.resize(channels, 1.0f);
	getProperty(Lav_FDN_OUTPUT_GAINS).replaceFloatArray(channels, &default[0]);
	getProperty(Lav_FDN_OUTPUT_GAINS).setFloatArrayDefault(default);
	//Identity matrix.
	default.clear();
	default.resize(channels*channels, 0.0f);
	//Build an identity matrix.
	for(int i = 0; i < channels; i++) default[i*channels+i] = 0.0f;
	getProperty(Lav_FDN_MATRIX).setArrayLengthRange(channels*channels, channels*channels);
	getProperty(Lav_FDN_MATRIX).setFloatRange(-std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	getProperty(Lav_FDN_MATRIX).replaceFloatArray(channels*channels, &default[0]);
	getProperty(Lav_FDN_MATRIX).setFloatArrayDefault(default);
	
	//The filters.
	getProperty(Lav_FDN_FILTER_TYPES).setArrayLengthRange(channels, channels);
	getProperty(Lav_FDN_FILTER_TYPES).zeroArray(channels);
	getProperty(Lav_FDN_FILTER_FREQUENCIES).setArrayLengthRange(channels, channels);
	getProperty(Lav_FDN_FILTER_FREQUENCIES).zeroArray(channels);
}

FeedbackDelayNetworkNode::~FeedbackDelayNetworkNode() {
	delete network;
	freeArray(last_output);
	freeArray(next_input);
	freeArray(gains);
}

std::shared_ptr<Node> createFeedbackDelayNetworkNode(std::shared_ptr<Simulation> simulation, float maxDelay, int channels) {
	return standardNodeCreation<FeedbackDelayNetworkNode>(simulation, maxDelay, channels);
}

void FeedbackDelayNetworkNode::process() {
	//First, check an d set properties.
	//We just do this inline because it's trivial.
	if(werePropertiesModified(this, Lav_FDN_DELAYS)) {
		setDelays(getProperty(Lav_FDN_DELAYS).getFloatArrayPtr());
	}
	if(werePropertiesModified(this, Lav_FDN_MATRIX)) {
		setMatrix(getProperty(Lav_FDN_MATRIX).getFloatArrayPtr());
	}
	if(werePropertiesModified(this, Lav_FDN_OUTPUT_GAINS)) {
		setOutputGains(getProperty(Lav_FDN_OUTPUT_GAINS).getFloatArrayPtr());
	}
	if(werePropertiesModified(this, Lav_FDN_FILTER_TYPES, Lav_FDN_FILTER_FREQUENCIES)) {
		configureFilters(
		getProperty(Lav_FDN_FILTER_TYPES).getIntArrayPtr(),
		getProperty(Lav_FDN_FILTER_FREQUENCIES).getFloatArrayPtr());
	}
	for(int i = 0	; i < block_size; i++) {
		network->computeFrame(last_output);
		for(int j = 0; j < num_output_buffers; j++) { 	
			output_buffers[j][i] = last_output[j]*gains[j];
			next_input[j] = input_buffers[j][i];
			//Apply the filter.
			last_output[j] = filters[j]->tick(last_output[j]);
		}
		network->advance(next_input, last_output);
	}
}

void FeedbackDelayNetworkNode::setMatrix(float* values) {
	network->setMatrix(values);
}

void FeedbackDelayNetworkNode::setOutputGains(float* values) {
	std::copy(values, values+channels, gains);
}

void FeedbackDelayNetworkNode::setDelays(float* values) {
	network->setDelays(values);
}

void FeedbackDelayNetworkNode::configureFilters(int* types, float* frequencies) {
	for(int i = 0; i < channels; i++) {
		if(types[i] == Lav_FDN_FILTER_TYPE_DISABLED) {
			//Configure a passthrough filter.
			filters[i]->setCoefficients(1.0, 0.0);
		}
		else {
			filters[i]->setPoleFromFrequency(frequencies[i], types[i] == Lav_FDN_FILTER_TYPE_HIGHPASS);
		}
	}
}

//begin public api.

Lav_PUBLIC_FUNCTION LavError Lav_createFeedbackDelayNetworkNode(LavHandle simulationHandle, float maxDelay, int channels, LavHandle* destination) {
	PUB_BEGIN
	auto simulation =incomingObject<Simulation>(simulationHandle);
	LOCK(*simulation);
	*destination = outgoingObject<Node>(createFeedbackDelayNetworkNode(
	simulation, maxDelay, channels));
	PUB_END
}

}