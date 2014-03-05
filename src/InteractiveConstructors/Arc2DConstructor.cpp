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

#include "Arc2DConstructor.h"

Arc2DConstructor::Arc2DConstructor(QtSketchPointer parent_sketch):
InteractiveConstructorBase(parent_sketch),
primitive_finished_(false)
{
	current_point_number_ = 0;
}

void Arc2DConstructor::CreateObject()
{
	if(primitive_finished_)
	{
		parent_sketch_->AddArc2D(point1_s_,point1_t_,point2_s_,point2_t_,point3_s_,point3_t_,true,true,true,true,false);
	}
}

bool Arc2DConstructor::LeftButtonUp(MouseEventPropertiesPointer event_props)
{
	double x = event_props->GetXPosition();
	double y = event_props->GetYPosition();
	double z = event_props->GetZPosition();
	
	current_point_number_++;

	if(current_point_number_ == 1)
	{
		// project x,y,z coordinates onto sketch plane
		parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,point1_s_,point1_t_);
	} else if(current_point_number_ == 2) {
		// project x,y,z coordinates onto sketch plane
		parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,point2_s_,point2_t_);
	} else if(current_point_number_ == 3) {
		// project x,y,z coordinates onto sketch plane
		parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,point3_s_,point3_t_);
	}
	
	
	if(current_point_number_ == 3)
		primitive_finished_ = true;
	
	return primitive_finished_;
}
