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

#ifndef DisplayPropertiesH
#define DisplayPropertiesH

#include <QColor>
#include <QBrush>
#include <QPen>
#include <QString>

const QString LineEditStyleSheet("QLineEdit { border-width: 2px; border-style: solid; border-color: rgb(166,86,0);}");

enum DefaultPropertiesSet {Primitive, SelectedPrimitive, HoverPrimitive, Annotation, SelectedAnnotation, HoverAnnotation, PointPrimitive, SelectedPointPrimitive, HoverPointPrimitive};

class DisplayProperties
{
	public:
		DisplayProperties(DefaultPropertiesSet prop_set = Primitive);

		// annotation properties
		double GetLeaderGap() 			{return leader_gap_;}
		double GetLeaderExtension() 	{return leader_extension_;}
		double GetArrowHeadLength()		{return arrow_head_length_;}
		double GetArrowHeadWidth()		{return arrow_head_width_;}
		double GetZ() 					{return z_;}
		QColor GetPenColor() 			{return pen_color_;}
		double GetPenWidth() 			{return pen_width_;}
		QPen   GetPen(double scale=1.0)	{return QPen(pen_color_, pen_width_/scale);}
		QBrush GetBrush() 				{return brush_;}


	private:
		// annotation properties
		double leader_gap_;
		double leader_extension_;
		double arrow_head_length_;
		double arrow_head_width_;
		double z_;
		QColor pen_color_;
		double pen_width_;
		QBrush brush_;
};




#endif //DisplayPropertiesH
