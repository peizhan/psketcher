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


#include "Circle2DConstructor.h"

Circle2DConstructor::Circle2DConstructor(QtSketchPointer parent_sketch):
InteractiveConstructorBase(parent_sketch),
primitive_finished_(false)
{
	parent_sketch_->ApplySelectionMask(Points);
	parent_sketch_->ClearSelected();
}

Circle2DConstructor::~Circle2DConstructor() 
{
    // Delete temp circle if it was defined
    if(temp_circle_.get() != 0)
        parent_sketch_->DeletePrimitive(temp_circle_);

    parent_sketch_->ApplySelectionMask(All); 
    parent_sketch_->ClearSelected();
}  

void Circle2DConstructor::CreateObject()
{
	if(primitive_finished_)
	{
        if(center_point_.get() == 0)
		  parent_sketch_->AddCircle2D(center_s_,center_t_,current_radius_,true,true,false);
        else
          parent_sketch_->AddCircle2D(center_point_->GetSDOF(),center_point_->GetTDOF(),current_radius_,false);
	}
}

bool Circle2DConstructor::MouseMove(MotionEventPropertiesPointer event_props)
{
    if(temp_circle_.get() != 0)
    {
        double x = event_props->GetXPosition();
        double y = event_props->GetYPosition();
        double z = event_props->GetZPosition();

        double motion_s, motion_t;

        // project x,y,z coordinates onto sketch plane
        parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,motion_s,motion_t);

        current_radius_ = sqrt(pow(motion_s-center_s_,2) + pow(motion_t-center_t_,2));

        temp_circle_->SetRadiusValue(current_radius_,false /*update_db*/);

        parent_sketch_->UpdateDisplay();
    }

	return false;
}

bool Circle2DConstructor::LeftButtonUp(MouseEventPropertiesPointer event_props)
{ 
    if(temp_circle_.get() == 0)
    {
        // first mouse click

        // first, check to see if a point was created
        std::vector<PrimitiveBasePointer> primitive_list = parent_sketch_->GetSelectedPrimitives();

        if(primitive_list.size() >= 1 && dynamic_cast<QtPoint2D*>(primitive_list[0].get()) != 0)
        {
            center_point_ = boost::dynamic_pointer_cast<QtPoint2D>(primitive_list[0]);
            center_s_ = center_point_->GetSValue();
            center_t_ = center_point_->GetTValue();
        } else {
            // no point was selected
            double x = event_props->GetXPosition();
            double y = event_props->GetYPosition();
            double z = event_props->GetZPosition();

            // project x,y,z coordinates onto sketch plane
            parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,center_s_,center_t_);
        }

        temp_circle_ = parent_sketch_->AddCircle2D(center_s_,center_t_,0.0,true,true,true);

        return false;
    } else {
        // second mouse click 
        double x = event_props->GetXPosition();
        double y = event_props->GetYPosition();
        double z = event_props->GetZPosition();

        double motion_s, motion_t;

        // project x,y,z coordinates onto sketch plane
        parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,motion_s,motion_t);

        current_radius_ = sqrt(pow(motion_s-center_s_,2) + pow(motion_t-center_t_,2));

        primitive_finished_ = true;

        return true;
    }
}




