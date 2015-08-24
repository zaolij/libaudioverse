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
#include <libaudioverse/implementations/biquad.hpp>
#include <libaudioverse/private/multichannel_filter_bank.hpp>
#include <memory>


namespace libaudioverse_implementation {

class BiquadNode: public Node {
	public:
	BiquadNode(std::shared_ptr<Simulation> sim, unsigned int channels);
	void process();
	void reconfigure();
	void reset() override;
	private:
	MultichannelFilterBank<BiquadFilter> bank;
	int prev_type;
};

BiquadNode::BiquadNode(std::shared_ptr<Simulation> sim, unsigned int channels): Node(Lav_OBJTYPE_BIQUAD_NODE, sim, channels, channels),
bank(simulation->getSr()) {
	if(channels < 1) ERROR(Lav_ERROR_RANGE, "Cannot filter 0 or fewer channels.");
	bank.setChannelCount(channels);
	prev_type = getProperty(Lav_BIQUAD_FILTER_TYPE).getIntValue();
	appendInputConnection(0, channels);
	appendOutputConnection(0, channels);
}

std::shared_ptr<Node> createBiquadNode(std::shared_ptr<Simulation> simulation, unsigned int channels) {
	auto retval = std::shared_ptr<BiquadNode>(new BiquadNode(simulation, channels), ObjectDeleter(simulation));
	simulation->associateNode(retval);
	return retval;
}

void BiquadNode::reconfigure() {
	int type = getProperty(Lav_BIQUAD_FILTER_TYPE).getIntValue();
	float sr = simulation->getSr();
	float frequency = getProperty(Lav_BIQUAD_FREQUENCY).getFloatValue();
	float q = getProperty(Lav_BIQUAD_Q).getFloatValue();
	float dbgain= getProperty(Lav_BIQUAD_DBGAIN).getFloatValue();
	bank->configure(type, frequency, dbgain, q);
	if(type != prev_type) bank.reset();
	prev_type = type;
}

void BiquadNode::process() {
	if(werePropertiesModified(this, Lav_BIQUAD_FILTER_TYPE, Lav_BIQUAD_DBGAIN, Lav_BIQUAD_FREQUENCY, Lav_BIQUAD_Q)) reconfigure();
	bank.process(block_size, &input_buffers[0], &output_buffers[0]);
}

void BiquadNode::reset() {
	bank.reset();
}

Lav_PUBLIC_FUNCTION LavError Lav_createBiquadNode(LavHandle simulationHandle, unsigned int channels, LavHandle* destination) {
	PUB_BEGIN
	auto simulation =incomingObject<Simulation>(simulationHandle);
	LOCK(*simulation);
	*destination = outgoingObject<Node>(createBiquadNode(simulation, channels));
	PUB_END
}

}