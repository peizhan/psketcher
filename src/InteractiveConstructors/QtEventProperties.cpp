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


#include <QMouseEvent>
#include <QGraphicsView>
#include "QtEventProperties.h"

// Constructor
QtMouseEventProperties::QtMouseEventProperties(MouseButtonEventType event_type, QMouseEvent  *event, QGraphicsView *view)
{
	screen_x_position_ = event->x();
	screen_y_position_ = event->y();
	
	// translate the screen coordinates to world coordinates
	QPointF scene_location = view->mapToScene(screen_x_position_,screen_y_position_);
	global_x_position_ = scene_location.x();
	global_y_position_ = -scene_location.y();
	global_z_position_ = 0.0;
	
	control_ = event->modifiers() & Qt::ControlModifier;
	shift_ = event->modifiers() & Qt::ShiftModifier;
	alt_ = event->modifiers() & Qt::AltModifier;
	
	switch (event->button()) {
		case Qt::LeftButton:  mouse_button_ = LeftButton; break;
		case Qt::MidButton:   mouse_button_ = MiddleButton; break;
		case Qt::RightButton: mouse_button_ = RightButton; break;
		case Qt::XButton1:    mouse_button_ = Button4; break;
		case Qt::XButton2:    mouse_button_ = Button5; break;
		default:              mouse_button_ = OtherButton; 
	}

	mouse_event_type_ = event_type;
}

QtMotionEventProperties::QtMotionEventProperties(QMouseEvent  *event, QGraphicsView *view)
{
	screen_x_position_ = event->x();
	screen_y_position_ = event->y();

	// translate the screen coordinates to world coordinates
	QPointF scene_location = view->mapToScene(screen_x_position_,screen_y_position_);
	global_x_position_ = scene_location.x();
	global_y_position_ = -scene_location.y();
	global_z_position_ = 0.0;

	control_ = event->modifiers() & Qt::ControlModifier;
	shift_ = event->modifiers() & Qt::ShiftModifier;
	alt_ = event->modifiers() & Qt::AltModifier;
	
	left_button_ = event->buttons() & Qt::LeftButton;
	middle_button_ = event->buttons() & Qt::MidButton;
	right_button_ = event->buttons() & Qt::RightButton;
	button_4_ = event->buttons() & Qt::XButton1;
	button_5_ = event->buttons() & Qt::XButton2;
}
