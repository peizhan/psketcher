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


#ifndef QtEventPropertiesH
#define QtEventPropertiesH

#include "InteractiveConstructorBase.h"

class QMouseEvent;

// This is the GTK binding for the mouse and motion events

// Get binding for the mouse event properties (only the constructor needs to be defined) 
class QtMouseEventProperties : public MouseEventProperties
{
	public:
		// Constructor
		QtMouseEventProperties(MouseButtonEventType event_type, QMouseEvent *event, QGraphicsView *view);

	private:
};
typedef std::auto_ptr<QtMouseEventProperties> QtMouseEventPropertiesPointer;



// Get binding for the mouse event properties (only the constructor needs to be defined) 
class QtMotionEventProperties : public MotionEventProperties
{
	public:
		// Constructor
		QtMotionEventProperties(QMouseEvent  *event, QGraphicsView *view);

	private:
};
typedef std::auto_ptr<QtMotionEventProperties> QtMotionEventPropertiesPointer;

#endif //QtEventPropertiesH
