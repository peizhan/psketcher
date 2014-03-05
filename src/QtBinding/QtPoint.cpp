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


#include "QtPoint.h"

QtPoint::QtPoint (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
Point(id,psketcher_model)
{
	SetProperties(PointPrimitive);
	SetSelectedProperties(SelectedPointPrimitive);
	SetMouseHoverProperties(HoverPointPrimitive);

	setZValue(GetProperties().GetZ());

	// Display the newly create ais_object
	Display();
}


QtPoint::QtPoint (QGraphicsItem * parent, double x, double y, double z, bool x_free, bool y_free, bool z_free) :
QtPrimitiveBase(parent),
Point(x,y,z,x_free,y_free,z_free)
{
	SetProperties(PointPrimitive);
	SetSelectedProperties(SelectedPointPrimitive);
	SetMouseHoverProperties(HoverPointPrimitive);

	setZValue(GetProperties().GetZ());

	// Display the newly create ais_object
	Display();
}

void QtPoint::UpdateDisplay()
{

	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtPoint::boundingRect() const
{ 
	return QRectF(1,1,1,1);
}

void QtPoint::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) 
{
	;
}


