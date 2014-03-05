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


#include <QtGui>
#include "DisplayProperties.h"

DisplayProperties::DisplayProperties(DefaultPropertiesSet prop_set) 
{
	arrow_head_length_ = 15.0;
	arrow_head_width_ = 12.0;

	switch (prop_set)
	{
		case Primitive:
			// geometry properties
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 1.0;
			pen_color_ = Qt::black;
			pen_width_ = 1.0;
			brush_ = QBrush(Qt::lightGray,Qt::SolidPattern);
	
			break;
	
		case SelectedPrimitive:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 1.0;
			pen_color_ = QColor(180,180,180);
			pen_width_ = 1.0;
			brush_ = QBrush(QColor(70,70,70),Qt::SolidPattern);
	
			break;

		case HoverPrimitive:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 1.0;
			pen_color_ = QColor(180,180,180);
			pen_width_ = 1.0;
			brush_ = QBrush(QColor(70,70,70),Qt::SolidPattern);
	
			break;
	
		case Annotation:
			
			// annotation properties
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 10.0;
			pen_color_ = QColor(166,86,0);
			pen_width_ = 2.0;
			brush_ = QBrush(pen_color_,Qt::SolidPattern);
	
			break;
	
		case SelectedAnnotation:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 10.0;
			pen_color_ = QColor(180,180,180);
			pen_width_ = 2.0;
			brush_ = QBrush(pen_color_,Qt::SolidPattern);
	
			break;

		case HoverAnnotation:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 10.0;
			pen_color_ = QColor(180,180,180);
			pen_width_ = 2.0;
			brush_ = QBrush(pen_color_,Qt::SolidPattern);
	
			break;
	
		case PointPrimitive:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 2.0;
			pen_color_ = Qt::black;
			pen_width_ = 1.0;
			brush_ = QBrush(Qt::lightGray,Qt::SolidPattern);
	
			break;
	
		case SelectedPointPrimitive:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 2.0;
			pen_color_ = Qt::lightGray;
			pen_width_ = 1.0;
			brush_ = QBrush(Qt::black,Qt::SolidPattern);
	
			break;

		case HoverPointPrimitive:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 2.0;
			pen_color_ = Qt::lightGray;
			pen_width_ = 1.0;
			brush_ = QBrush(Qt::black,Qt::SolidPattern);
	
			break;

		default:
			leader_gap_ = 10.0;
			leader_extension_ = 15.0;
			z_ = 1.0;
			pen_color_ = Qt::black;
			pen_width_ = 2.0;
			brush_ = QBrush(Qt::lightGray,Qt::SolidPattern);
	}
}