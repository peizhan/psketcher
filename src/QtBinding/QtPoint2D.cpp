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

#include "QtPoint2D.h"

QtPoint2D::QtPoint2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
Point2D(id,psketcher_model),
pending_db_save_(false)
{
	SetProperties(PointPrimitive);
	SetSelectedProperties(SelectedPointPrimitive);
	SetMouseHoverProperties(HoverPointPrimitive);

	setZValue(GetProperties().GetZ());

	double x_position, y_position, z_position;

	Get3DLocation(x_position, y_position, z_position);

	// Display the newly create ais_object
	Display();
}


QtPoint2D::QtPoint2D (QGraphicsItem * parent,double s, double t, SketchPlanePointer sketch_plane, bool s_free, bool t_free) :
QtPrimitiveBase(parent),
Point2D(s,t,sketch_plane,s_free,t_free),
pending_db_save_(false)
{
	SetProperties(PointPrimitive);
	SetSelectedProperties(SelectedPointPrimitive);
	SetMouseHoverProperties(HoverPointPrimitive);

	setZValue(GetProperties().GetZ());

	double x_position, y_position, z_position;

	Get3DLocation(x_position, y_position, z_position);

	// Display the newly create ais_object
	Display();
}

QtPoint2D::QtPoint2D (QGraphicsItem * parent, DOFPointer s, DOFPointer t, SketchPlanePointer sketch_plane) :
Point2D(s,t,sketch_plane),
QtPrimitiveBase(parent),
pending_db_save_(false)
{
	SetProperties(PointPrimitive);
	SetSelectedProperties(SelectedPointPrimitive);
	SetMouseHoverProperties(HoverPointPrimitive);

	setZValue(GetProperties().GetZ());

	double x_position, y_position, z_position;

	Get3DLocation(x_position, y_position, z_position);

	// Display the newly create ais_object
	Display();
}

void QtPoint2D::UpdateDisplay()
{
	double x_position, y_position, z_position;

	Get3DLocation(x_position, y_position, z_position);


	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtPoint2D::boundingRect() const
{
	QRectF rect(QPointF(GetSValue(),-GetTValue()),
 				QPointF(GetSValue(),-GetTValue()));
	
	// increase the size of the box to account for the actual radius of the point that represents the circle
	// @fixme if the user zooms out or if the model has a large length scale, the bounding box wont contain the whole point
	rect.adjust(-GetBoundingRectPad(),-GetBoundingRectPad(),GetBoundingRectPad(),GetBoundingRectPad());
	
	return rect;
}

void QtPoint2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */) 
{
	double level_of_detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	
	DisplayProperties current_properties;

	// @fixme the way constraint_properties is defined in the following if statement block will prevent the user from changing the display properties of the point constraints at run time since the DisplayProperties constructor is used to set these properties
	DisplayProperties constraint_properties;

	if(option->state & QStyle::State_MouseOver && IsSelectable())
	{
		current_properties = GetMouseHoverProperties();
		constraint_properties = DisplayProperties(HoverAnnotation);
	} else if (option->state & QStyle::State_Selected) {
		current_properties = GetSelectedProperties();
		constraint_properties = DisplayProperties(SelectedAnnotation);
	} else {
		current_properties = GetProperties();
		constraint_properties = DisplayProperties(Annotation);
	}

	painter->setPen(current_properties.GetPen(level_of_detail));
	painter->setBrush(current_properties.GetBrush());	

	QPainterPath point_path;
	PaintPointAndSelectionPath(painter, option, GetSValue(), -GetTValue(),point_path);

	// if point is constrained, draw the constraints
	if(! SIsFree() || ! TIsFree())
	{
		painter->setPen(constraint_properties.GetPen(level_of_detail));
		painter->setBrush(constraint_properties.GetBrush());
		
		double radius = 5.0/level_of_detail;

		if((! SIsFree() && ! GetSDOF()->IsDependent() )  && (! TIsFree() && ! GetTDOF()->IsDependent() ))
		{
			QRectF rect(QPointF(GetSValue()-radius,-(GetTValue()-radius)),
			QPointF(GetSValue()+radius,-(GetTValue()+radius)));
			painter->setBrush(Qt::NoBrush);
			painter->drawEllipse(rect);

		} else if((! SIsFree() && ! GetSDOF()->IsDependent() )) {

			painter->drawLine(QPointF(GetSValue()-radius,-(GetTValue()-radius)),
			QPointF(GetSValue()-radius,-(GetTValue()+radius)));

			painter->drawLine(QPointF(GetSValue()+radius,-(GetTValue()-radius)),
			QPointF(GetSValue()+radius,-(GetTValue()+radius)));

		} else if((! TIsFree() && ! GetTDOF()->IsDependent() )) {

			painter->drawLine(QPointF(GetSValue()+radius,-(GetTValue()+radius)),
			QPointF(GetSValue()-radius,-(GetTValue()+radius)));

			painter->drawLine(QPointF(GetSValue()-radius,-(GetTValue()-radius)),
			QPointF(GetSValue()+radius,-(GetTValue()-radius)));
		}
	}
	

	current_shape_ = point_path;
}

void QtPoint2D::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(event->buttons() & Qt::LeftButton)
	{
		pending_db_save_ = true;

		// move the point to the new global position
		SetSValue(event->scenePos().x(),false /*update_db*/);
		SetTValue(-event->scenePos().y(),false /*update_db*/);

		// force a update of the display so that the drag event is seen interactively
		scene()->update();

		//@fixme After drag operation is finished, need to trigger QtSketch's modelChanged() slot

	} else {
		// not handling this event, let the base class do its thing
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void QtPoint2D::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
	if (event->button() & Qt::LeftButton && pending_db_save_) 
	{
		// if there is a pending db save, do the save now (this happens at the end of a drag event)
		SetSValue(GetSValue());
		SetTValue(GetTValue());

		pending_db_save_ = false;
	}

	// let the base class do it's thing
	QGraphicsItem::mouseReleaseEvent(event);
}


