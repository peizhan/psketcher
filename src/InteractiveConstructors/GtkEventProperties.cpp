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


#include "GtkEventProperties.h"

// Constructor
GtkMouseEventProperties::GtkMouseEventProperties(MouseButtonEventType event_type, GdkEventButton  *event)
{
	x_position_ = event->x;
	y_position_ = event->y;
	

	control_ = event->state & GDK_CONTROL_MASK;
	shift_ = event->state & GDK_SHIFT_MASK;
	alt_ = event->state &GDK_MOD1_MASK;
	
	switch (event->button) {
		case 1: mouse_button_ = LeftButton; break;
		case 2: mouse_button_ = MiddleButton; break;
		case 3: mouse_button_ = RightButton; break;
		case 4: mouse_button_ = Button4; break;
		case 5: mouse_button_ = Button5; break;
		default: mouse_button_ = OtherButton;
	}

	mouse_event_type_ = event_type;
}

GtkMotionEventProperties::GtkMotionEventProperties(GdkEventMotion  *event)
{
	x_position_ = event->x;
	y_position_ = event->y;
	
	control_ = event->state & GDK_CONTROL_MASK;
	shift_ = event->state & GDK_SHIFT_MASK;
	alt_ = event->state &GDK_MOD1_MASK;
	
	left_button_ = event->state & GDK_BUTTON1_MASK;
	middle_button_ = event->state & GDK_BUTTON2_MASK;
	right_button_ = event->state & GDK_BUTTON3_MASK;
	button_4_ = event->state & GDK_BUTTON4_MASK;
	button_5_ = event->state & GDK_BUTTON5_MASK;
}
