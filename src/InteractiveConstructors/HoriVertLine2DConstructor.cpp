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

#include "HoriVertLine2DConstructor.h"

HoriVertLine2DConstructor::HoriVertLine2DConstructor(QtSketchPointer parent_sketch, bool vertical_constraint):
InteractiveConstructorBase(parent_sketch),
vertical_constraint_(vertical_constraint),
primitive_finished_(false)
{
	parent_sketch_->ApplySelectionMask(Lines);
	parent_sketch_->ClearSelected();
}

void HoriVertLine2DConstructor::CreateObject()
{
	if(primitive_finished_)
	{		
		// @fixme need to determine if user intended an interior angle or an exterior angle
		parent_sketch_->AddHoriVertLine2D(line_, vertical_constraint_);
	}
}

bool HoriVertLine2DConstructor::MouseMove(MotionEventPropertiesPointer event_props)
{

	return false;
}

bool HoriVertLine2DConstructor::LeftButtonUp(MouseEventPropertiesPointer event_props)
{
	// perform selection operation
	//parent_sketch_->GetAISContext()->Select();

	// first, make sure a point is selected
	std::vector<PrimitiveBasePointer> primitive_list = parent_sketch_->GetSelectedPrimitives();

	if(primitive_list.size() < 1 )
		return false; 							// no primitive selected

	if(dynamic_cast<QtLine2D*>(primitive_list[0].get()) != 0){
		line_ = boost::dynamic_pointer_cast<QtLine2D>(primitive_list[0]);
		
		primitive_finished_ = true;
		
		return true;

	} else {
		return false; // line not selected
	}
}
