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

#include "QtLine2D.h"

QtLine2D::QtLine2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
Line2D(id,psketcher_model)
{
	double x1, y1, z1, x2, y2, z2;	

	Get3DLocations(x1, y1, z1, x2, y2, z2);

	// Display the newly create ais_object
	Display();
}

QtLine2D::QtLine2D (QGraphicsItem * parent, const Point2DPointer point1, const Point2DPointer point2, SketchPlanePointer sketch_plane) :
QtPrimitiveBase(parent),
Line2D(point1, point2, sketch_plane)
{
	double x1, y1, z1, x2, y2, z2;	

	Get3DLocations(x1, y1, z1, x2, y2, z2);

	// Display the newly create ais_object
	Display();
}

void QtLine2D::UpdateDisplay()
{
	double x1, y1, z1, x2, y2, z2;	

	Get3DLocations(x1, y1, z1, x2, y2, z2);


	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtLine2D::boundingRect() const
{
	double min_x = qMin(GetS1()->GetValue(),GetS2()->GetValue());
	double max_x = qMax(GetS1()->GetValue(),GetS2()->GetValue());
	double min_y = qMin(GetT1()->GetValue(),GetT2()->GetValue());
	double max_y = qMax(GetT1()->GetValue(),GetT2()->GetValue());

	return QRectF(QPointF(min_x-GetBoundingRectPad(),-(min_y-GetBoundingRectPad())),QPointF(max_x+GetBoundingRectPad(),-(max_y+GetBoundingRectPad())));
}

void QtLine2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */)
{
	double level_of_detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
  
	DisplayProperties current_properties;

	if(option->state & QStyle::State_MouseOver && IsSelectable())
	{
		current_properties = GetMouseHoverProperties();
	} else if (option->state & QStyle::State_Selected) {
		current_properties = GetSelectedProperties();
	} else {
		current_properties = GetProperties();
	}

	painter->setPen(current_properties.GetPen(level_of_detail));
	painter->setBrush(current_properties.GetBrush());

	// draw the line
	QPainterPath line_path;
	painter->drawLine(GetLineAndSelectionPath(GetS1()->GetValue(),-GetT1()->GetValue(),GetS2()->GetValue(),-GetT2()->GetValue(),line_path, level_of_detail));

	// update the shape that will be used for mouse selection
	current_shape_ = line_path;
}
