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


#include "Line2DConstructor.h"

Line2DConstructor::Line2DConstructor(QtSketchPointer parent_sketch):
InteractiveConstructorBase(parent_sketch),
primitive_finished_(false),
point1_defined_(false),
delete_point1_on_cancel_(false),
delete_point2_on_cancel_(false),
temp_point_defined_(false),
temp_line_defined_(false)
{
	parent_sketch_->ApplySelectionMask(Points);
	parent_sketch_->ClearSelected();
}

Line2DConstructor::~Line2DConstructor() 
{
    // Delete any primitives that where not used because this object was killed before the line
    // could be completed.
    if(delete_point1_on_cancel_)
        parent_sketch_->DeletePrimitive(point1_);

    if(delete_point2_on_cancel_)
        parent_sketch_->DeletePrimitive(point2_);

    if(temp_point_defined_)
        parent_sketch_->DeletePrimitive(temp_point_);

    if(temp_line_defined_)
        parent_sketch_->DeletePrimitive(temp_line_);

    parent_sketch_->ApplySelectionMask(All); 
    parent_sketch_->ClearSelected();
}

void Line2DConstructor::CreateObject()
{
	if(primitive_finished_)
	{
		parent_sketch_->AddLine2D(point1_, point2_);

        // any temporary objects created can now be kept
        delete_point1_on_cancel_ = false;
        delete_point2_on_cancel_ = false;
	}
}

bool Line2DConstructor::MouseMove(MotionEventPropertiesPointer event_props)
{
    if(temp_point_defined_)
    {
        double x = event_props->GetXPosition();
        double y = event_props->GetYPosition();
        double z = event_props->GetZPosition();

        double motion_s, motion_t;
        
        // project x,y,z coordinates onto sketch plane
        parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,motion_s,motion_t);

        temp_point_->SetSValue(motion_s, false /*update_db*/);
        temp_point_->SetTValue(motion_t, false /*update_db*/);

        parent_sketch_->UpdateDisplay();
    }

	return false;
}

bool Line2DConstructor::LeftButtonUp(MouseEventPropertiesPointer event_props)
{
    bool new_point_created = false;
    QtPoint2DPointer new_point;
    
	// first, check to see if a point was created
	std::vector<PrimitiveBasePointer> primitive_list = parent_sketch_->GetSelectedPrimitives();

    if(primitive_list.size() >= 1 && dynamic_cast<QtPoint2D*>(primitive_list[0].get()) != 0)
    {
        new_point = boost::dynamic_pointer_cast<QtPoint2D>(primitive_list[0]);
    } else {
        // no point was selected so create a point at the selection point
        
        double x = event_props->GetXPosition();
        double y = event_props->GetYPosition();
        double z = event_props->GetZPosition();

        double click_s, click_t;
        
        // project x,y,z coordinates onto sketch plane
        parent_sketch_->GetSketchPlane()->GetSTLocation(x,y,z,click_s,click_t);

        new_point = parent_sketch_->AddPoint2D(click_s,click_t,true,true);

		new_point_created = true;
    }

    if(point1_defined_)
    {
        point2_ = new_point;
        if(new_point_created)
            delete_point2_on_cancel_ = true;
        
        if(!(point2_->GetSDOF() == point1_->GetSDOF() && point2_->GetTDOF() == point1_->GetTDOF()))
        {
            primitive_finished_ = true;
            return true;
        } else {
            return false;  // point2_ is the same as point1_ so line cannot be created
        }
    } else {
        point1_ = new_point;
        point1_defined_ = true;
        if(new_point_created)
            delete_point1_on_cancel_ = true;

        // create a temp line to provide feedback to the user as the line is created
        temp_point_ = parent_sketch_->AddPoint2D(point1_->GetSValue(), point1_->GetTValue(),true,true);
        temp_point_defined_ = true;
        temp_line_ = parent_sketch_->AddLine2D(point1_,temp_point_); // @fixme Need to add a method to add a temp line without having to define a point, will solve selection issue with the second point of an interactive line
        temp_line_defined_ = true;
        temp_point_->SetSelectable(false); // cannot select this temp point otherwise other points will not be selected

        return false;
    }

}




