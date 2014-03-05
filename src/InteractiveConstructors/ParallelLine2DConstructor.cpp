/*
Copyright (c) 2006-2014, Michael Greminger
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF A
DVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ParallelLine2DConstructor.h"

ParallelLine2DConstructor::ParallelLine2DConstructor(QtSketchPointer parent_sketch):
InteractiveConstructorBase(parent_sketch),
primitive_finished_(false),
line1_defined_(false)
{
	parent_sketch_->ApplySelectionMask(Lines);
	parent_sketch_->ClearSelected();
}

void ParallelLine2DConstructor::CreateObject()
{
	if(primitive_finished_)
	{		
		parent_sketch_->AddParallelLine2D(line1_, line2_);
	}
}

bool ParallelLine2DConstructor::MouseMove(MotionEventPropertiesPointer event_props)
{
	// parent_sketch_->GetAISContext()->MoveTo((int)event_props->GetXPosition(), (int)event_props->GetYPosition(), current_view_);

	return false;
}

bool ParallelLine2DConstructor::LeftButtonUp(MouseEventPropertiesPointer event_props)
{
	// perform selection operation
	//parent_sketch_->GetAISContext()->Select();

	// first, make sure a point is selected
	std::vector<PrimitiveBasePointer> primitive_list = parent_sketch_->GetSelectedPrimitives();

	if(primitive_list.size() < 1 )
		return false; 							// no primitive selected

	if(dynamic_cast<QtLine2D*>(primitive_list[0].get()) != 0){
		QtLine2DPointer new_line = boost::dynamic_pointer_cast<QtLine2D>(primitive_list[0]);

		if(line1_defined_)
		{
			line2_ = new_line;
			
			if(!(line1_ == line2_))
			{
				primitive_finished_ = true;
				return true;
			} else {
				return false;  // line2_ is the same as line1_ so line cannot be created
			}
		} else {
			line1_ = new_line;
			line1_defined_ = true;
			return false;
		}

	} else {
		return false; // point not selected
	}
}
