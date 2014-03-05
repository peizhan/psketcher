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


#ifndef InteractiveConstructorBaseH
#define InteractiveConstructorBaseH

#include <boost/shared_ptr.hpp>

#include "../QtBinding/QtSketch.h"

enum MouseButton {LeftButton, MiddleButton, RightButton, Button4, Button5, OtherButton};
enum MouseButtonEventType {ButtonPress, ButtonRelease};

class MouseEventProperties
{
	public:
		// Accessor methods used to retrieve values (the constructor will define all of the values)
		double GetXPosition() {return global_x_position_;}
		double GetYPosition() {return global_y_position_;}
		double GetZPosition() {return global_z_position_;}

		double GetXScreenPosition() {return screen_x_position_;}
		double GetYScreenPosition() {return screen_y_position_;}
		
		bool ControlPressed() {return control_;}
		bool ShiftPressed() {return shift_;}
		bool AltPressed() {return alt_;}

		MouseButton GetButton() {return mouse_button_;}
		MouseButtonEventType GetEventType() {return mouse_event_type_;}

	protected:
		double screen_x_position_;
		double screen_y_position_;

		double global_x_position_;
		double global_y_position_;
		double global_z_position_;
		
		bool control_;
		bool shift_;
		bool alt_;

		MouseButton mouse_button_;
		MouseButtonEventType mouse_event_type_;
};
typedef std::auto_ptr<MouseEventProperties> MouseEventPropertiesPointer;



class MotionEventProperties
{
	public:
		// Accessor methods used to retrieve values (the constructor will define all of the values)
		double GetXPosition() {return global_x_position_;}
		double GetYPosition() {return global_y_position_;}
		double GetZPosition() {return global_z_position_;}

		double GetXScreenPosition() {return screen_x_position_;}
		double GetYScreenPosition() {return screen_y_position_;}
		
		bool ControlPressed() {return control_;}
		bool ShiftPressed() {return shift_;}
		bool AltPressed() {return alt_;}

		bool LeftButtonPressed() {return left_button_;}
		bool MiddleButtonPressed() {return middle_button_;}
		bool RightButtonPressed() {return right_button_;}
		bool Button4Pressed() {return button_4_;}
		bool Button5Pressed() {return button_5_;}

	protected:
		double screen_x_position_;
		double screen_y_position_;
		
		double global_x_position_;
		double global_y_position_;
		double global_z_position_;

		bool control_;
		bool shift_;
		bool alt_;

		bool left_button_;
		bool middle_button_;
		bool right_button_;
		bool button_4_;
		bool button_5_;
};
typedef std::auto_ptr<MotionEventProperties> MotionEventPropertiesPointer;

class InteractiveConstructorBase
{
	public:
		// Constructors and destructor
		InteractiveConstructorBase(QtSketchPointer parent_sketch);
		virtual ~InteractiveConstructorBase() {;}
		
		// all derived classes must implement this method
		virtual void CreateObject()=0; // Adds the created object to parent_sketch_

		// GUI event methods
		// each method returns true if the object being created is completely defined and false otherwise
		virtual bool LeftButtonDown(MouseEventPropertiesPointer event_props) {return false;}
		virtual bool MiddleButtonDown(MouseEventPropertiesPointer event_props) {return false;}
		virtual bool RightButtonDown(MouseEventPropertiesPointer event_props) {return false;}

		virtual bool LeftButtonUp(MouseEventPropertiesPointer event_props) {return false;}
		virtual bool MiddleButtonUp(MouseEventPropertiesPointer event_props) {return false;}
		virtual bool RightButtonUp(MouseEventPropertiesPointer event_props) {return false;}

		virtual bool MouseMove(MotionEventPropertiesPointer event_props) {return false;}

	protected:
		QtSketchPointer parent_sketch_;
};


#endif //InteractiveConstructorBaseH
