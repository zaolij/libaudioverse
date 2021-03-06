/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "../private/node.hpp"
#include <memory>
#include <vector>

namespace libaudioverse_implementation {

class Simulation;
class HrtfData;

class PannerBankNode: public SubgraphNode {
	public:
	PannerBankNode(std::shared_ptr<Simulation> sim, int pannerCount, std::shared_ptr<HrtfData> hrtf);
	std::shared_ptr<Node> input_gain, output_gain;
	std::vector<std::shared_ptr<Node>> panners;
	void configureForwardedProperties();
	void strategyChanged();
	void needsRepositioning(); //called when the cone's parameters change.
	void willTick() override;
};

std::shared_ptr<Node> createPannerBankNode(std::shared_ptr<Simulation> simulation, int pannerCount, std::shared_ptr<HrtfData> hrtf);
}