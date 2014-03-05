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

#include "QtParallelLine2D.h"

QtParallelLine2D::QtParallelLine2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
ParallelLine2D(id,psketcher_model)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	// Display the newly create ais_object
	Display();
}

QtParallelLine2D::QtParallelLine2D(QGraphicsItem * parent, const Line2DPointer line1, const Line2DPointer line2):
QtPrimitiveBase(parent),
ParallelLine2D(line1,line2)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	// Display the newly create ais_object
	Display();
}


void QtParallelLine2D::UpdateDisplay()
{
	Display();

	QtPrimitiveBase::UpdateDisplay();
}


QRectF QtParallelLine2D::boundingRect() const
{ 
	double min_s, max_s, min_t, max_t;
	max_s = min_s = GetLine1()->GetS1()->GetValue();
	max_t= min_t = GetLine1()->GetT1()->GetValue();
	
	double current_s;
	double current_t;	

	for(int i = 0; i<3; i++)
	{
		if (i == 0)
		{
			current_s = GetLine1()->GetS2()->GetValue();
			current_t = GetLine1()->GetT2()->GetValue();	
		} else if (i == 1) {
			current_s = GetLine2()->GetS1()->GetValue();
			current_t = GetLine2()->GetT1()->GetValue();	
		} else if (i == 2) {
			current_s = GetLine2()->GetS2()->GetValue();
			current_t = GetLine2()->GetT2()->GetValue();	
		}	
		
		if(max_s < current_s)
			max_s = current_s;
		else if(min_s > current_s)
			min_s = current_s;
	
		if(max_t < current_t)
			max_t = current_t;
		else if(min_t > current_t)
			min_t = current_t;
	}
	
	QRectF bounding_rect = QRectF(QPointF((min_s-GetBoundingRectPad()), -(min_t-GetBoundingRectPad())),QPointF(max_s+GetBoundingRectPad(),-(max_t+GetBoundingRectPad())));

	return bounding_rect;
}


void QtParallelLine2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */)
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

	QFont current_font;
	current_font.setStyleHint(QFont::SansSerif);
	current_font.setFamily("Helvetica");
	current_font.setPointSizeF(12.0);

	QString marker_text("//");

	// calculate the text position for each line
	mmcMatrix line1_point1 = GetLine1()->GetPoint1()->GetmmcMatrix();
	mmcMatrix line1_point2 = GetLine1()->GetPoint2()->GetmmcMatrix();

	mmcMatrix line2_point1 = GetLine2()->GetPoint1()->GetmmcMatrix();
	mmcMatrix line2_point2 = GetLine2()->GetPoint2()->GetmmcMatrix();

	mmcMatrix line1_text_position = (line1_point2 - line1_point1)*GetMarkerPosition()+line1_point1;
	mmcMatrix line2_text_position = (line2_point2 - line2_point1)*GetMarkerPosition()+line2_point1;

	// display the actual text as two painter paths	
	QPainterPath text_path1;
	text_path1.addText (0.0, 0.0, current_font, marker_text);
	QTransform text_transform;
	text_path1 = text_transform.scale(1.0/level_of_detail,1.0/level_of_detail).map(text_path1);
	text_transform.reset();
	QRectF bounding_box_1 = text_path1.boundingRect();
	text_transform.reset();
	text_path1 = text_transform.translate(-bounding_box_1.center().x(),-bounding_box_1.center().y()).map(text_path1);
	text_transform.reset();
	text_path1 = text_transform.translate(line1_text_position(0,0),-line1_text_position(1,0)).map(text_path1);

	QPainterPath text_path2; 
	text_path2.addText (0.0, 0.0, current_font, marker_text);
	text_transform.reset();
	text_path2 = text_transform.scale(1.0/level_of_detail,1.0/level_of_detail).map(text_path2);
	text_transform.reset();
	QRectF bounding_box_2 = text_path2.boundingRect();
	text_transform.reset();
	text_path2 = text_transform.translate(-bounding_box_2.center().x(),-bounding_box_2.center().y()).map(text_path2);
	text_transform.reset();
	text_path2 = text_transform.translate(line2_text_position(0,0),-line2_text_position(1,0)).map(text_path2);

	painter->drawPath(text_path1);
	painter->drawPath(text_path2);

	current_shape_ = text_path1;
	current_shape_.addPath(text_path2);
}
