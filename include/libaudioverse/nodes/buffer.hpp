/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "../privatre/node.hpp"
#include <memory>

namespace libaudioverse_implementation {


class Simulation;

class BufferNode: public Node {
	public:
	BufferNode(std::shared_ptr<Simulation> simulation);
	void setBuffer(std::shared_ptr<Buffer> buff);
	virtual void positionChanged();
	virtual void process();
	int frame = 0;
	int buffer_length=0;
	double offset=0.0;
};

}