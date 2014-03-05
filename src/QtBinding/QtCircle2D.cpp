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

#include "QtCircle2D.h"

QtCircle2D::QtCircle2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
Circle2D(id,psketcher_model),
pending_db_save_(false)
{
	SetProperties(Primitive);
	SetSelectedProperties(SelectedPrimitive);
	SetMouseHoverProperties(HoverPrimitive);

	setZValue(GetProperties().GetZ());

	radius_widget_ = 0;

	// Display the newly create ais_object
	Display();
}

QtCircle2D::QtCircle2D (QGraphicsItem * parent, double s_center, double t_center, double radius, 
					SketchPlanePointer sketch_plane, bool s_center_free, bool t_center_free, bool radius_free):
QtPrimitiveBase(parent),
Circle2D(s_center,t_center,radius,sketch_plane, s_center_free, t_center_free,radius_free),
pending_db_save_(false)
{
	SetProperties(Primitive);
	SetSelectedProperties(SelectedPrimitive);
	SetMouseHoverProperties(HoverPrimitive);

	setZValue(GetProperties().GetZ());

	radius_widget_ = 0;

	// Display the newly create ais_object
	Display();
}

QtCircle2D::QtCircle2D (QGraphicsItem * parent, double s1, double t1, double s2, double t2, double s3, double t3,
			SketchPlanePointer sketch_plane, bool s_center_free, bool t_center_free, bool radius_free):
QtPrimitiveBase(parent),
Circle2D(s1,t1,s2,t2,s3,t3, sketch_plane, s_center_free, t_center_free,radius_free),
pending_db_save_(false)
{
	SetProperties(Primitive);
	SetSelectedProperties(SelectedPrimitive);
	SetMouseHoverProperties(HoverPrimitive);

	setZValue(GetProperties().GetZ());

	radius_widget_ = 0;

	// Display the newly create ais_object
	Display();
}


QtCircle2D::QtCircle2D (QGraphicsItem * parent,DOFPointer s_center, DOFPointer t_center, DOFPointer radius, SketchPlanePointer sketch_plane):
Circle2D(s_center,t_center,radius,sketch_plane),
QtPrimitiveBase(parent),
pending_db_save_(false)
{
	SetProperties(Primitive);
	SetSelectedProperties(SelectedPrimitive);
	SetMouseHoverProperties(HoverPrimitive);

	setZValue(GetProperties().GetZ());

    radius_widget_ = 0;

	// Display the newly create ais_object
	Display();
}


void QtCircle2D::UpdateDisplay()
{
	Display();

	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtCircle2D::boundingRect() const
{
	double radius = qMax(GetRadius()->GetValue(),GetTextRadius()) + GetBoundingRectPad();
	QRectF rect(QPointF(GetSCenter()->GetValue()-radius,-GetTCenter()->GetValue()-radius),
 				QPointF(GetSCenter()->GetValue()+radius,-GetTCenter()->GetValue()+radius));
	return rect;
}

void QtCircle2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */) 
{
  	double level_of_detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	
	DisplayProperties current_properties;

	// @fixme the way radius_properties is defined in the following if statement block will prevent the user from changing the display properties of the radius dimension or the points at run time since the DisplayProperties constructor is used to set these properties
	DisplayProperties radius_properties; 

	if(option->state & QStyle::State_MouseOver && IsSelectable())
	{
		current_properties = GetMouseHoverProperties();
		radius_properties = DisplayProperties(HoverAnnotation);
	} else if (option->state & QStyle::State_Selected) {
		current_properties = GetSelectedProperties();
		radius_properties = DisplayProperties(SelectedAnnotation);
	} else {
		current_properties = GetProperties();
		radius_properties = DisplayProperties(Annotation);
	}
	
	double leader_gap = current_properties.GetLeaderGap()/level_of_detail;
	double leader_extension = current_properties.GetLeaderExtension()/level_of_detail;

	double leader_extension_angle = ((leader_extension/GetRadius()->GetValue())*(180.0/mmcPI))/level_of_detail;
	double leader_gap_angle = ((leader_gap/GetRadius()->GetValue())*(180.0/mmcPI))/level_of_detail;

	double radius = GetRadius()->GetValue();
	QRectF rect(QPointF(GetSCenter()->GetValue()-radius,-GetTCenter()->GetValue()-radius),
 				QPointF(GetSCenter()->GetValue()+radius,-GetTCenter()->GetValue()+radius));

	double text_angle = GetTextAngle()*((180.0)/(mmcPI));

	// create the radius dimension if necessary
	// Only display the radius if it is not a free parameter
	// If it is a free parameter, it is not really a constraint and should not be displayed as such
	if( ! radius_->IsFree())
	{
		painter->setPen(radius_properties.GetPen(level_of_detail));
		painter->setBrush(radius_properties.GetBrush());

		QPolygonF radius_arrow = GetArrowPolygon(s_center_->GetValue(),-t_center_->GetValue(),s_center_->GetValue()+radius_->GetValue()*cos(GetTextAngle()),
							   -(t_center_->GetValue()+radius_->GetValue()*sin(GetTextAngle())), 15.0/level_of_detail,12.0/level_of_detail);
		painter->drawPolygon(radius_arrow);

		// draw a line from the circle center point to the text location in case the text is outside of the circle
		painter->drawLine(QPointF(s_center_->GetValue(),-t_center_->GetValue()),
					      QPointF(s_center_->GetValue()+GetTextRadius()*cos(GetTextAngle()),-(t_center_->GetValue()+GetTextRadius()*sin(GetTextAngle()))));

		// create the line edit widget graphics item
		if(radius_widget_ == 0)
		{
			// @fixme need to make sure the following dyname_cast won't create a pointer that is need used even if this shared_ptr class is freed from memory
			radius_widget_ = new QtCircle2DWidget(shared_from_this(),dynamic_cast<QGraphicsItem*>(const_cast<QtCircle2D*>(this)));
		}
		radius_widget_->UpdateGeometry(level_of_detail);
	}

	painter->setPen(current_properties.GetPen(level_of_detail));
	painter->setBrush(current_properties.GetBrush());

	// paint the actual circl
	QPainterPath circle_selection_path;
	painter->drawPath(GetArcAndSelectionPath(GetSCenter()->GetValue(), -GetTCenter()->GetValue(), radius, 0.0*(mmcPI/180.0), 360.0*(mmcPI/180.0), circle_selection_path, level_of_detail));
	current_shape_ = circle_selection_path;
}

void QtCircle2D::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(event->buttons() & Qt::LeftButton)
	{		
        pending_db_save_ = true;
        
		// move the point to the new global position
		SetSTTextLocation(event->scenePos().x(),-event->scenePos().y(), false /*update_db*/);

		// force a update of the display so that the drag event is seen interactively
		scene()->update();

		//@fixme After drag operation is finished, need to trigger QtSketch's modelChanged() slot

	} else {
		// not handling this event, let the base class do its thing
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void QtCircle2D::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    if (event->button() & Qt::LeftButton && pending_db_save_) 
    {
        // if there is a pending db save, do the save now (this happens at the end of a drag event)
        SetTextLocation(GetTextRadius(),GetTextAngle());

        pending_db_save_ = false;
    }

    // let the base class do it's thing
    QGraphicsItem::mouseReleaseEvent(event);
}



QtCircle2DWidget::QtCircle2DWidget(QtCircle2DPointer circle_primitive, QGraphicsItem *parent) :
circle_primitive_(circle_primitive), QGraphicsProxyWidget(parent)
{
	//setFlags(ItemIgnoresTransformations);
    
	// create widget
	radius_line_edit_ = new QLineEdit;
	radius_line_edit_->setStyleSheet(LineEditStyleSheet);
	radius_line_edit_->setValidator(new QDoubleValidator(this));
	radius_line_edit_->setAlignment(Qt::AlignCenter);
	radius_line_edit_->setText(QString("%1").arg(circle_primitive_->GetRadiusValue()));
	textChanged();
	//radius_line_edit_->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
	radius_line_edit_->resize(radius_line_edit_->minimumSizeHint());
	connect(radius_line_edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
	connect(radius_line_edit_, SIGNAL(returnPressed()), this, SLOT(applyChanges()));

	// package widget
	setWidget(radius_line_edit_);
}



// apply the changes if valid values have been entered
void QtCircle2DWidget::applyChanges()
{
	if(radius_line_edit_->hasAcceptableInput())
	{
		circle_primitive_->SetRadiusValue(radius_line_edit_->text().toDouble());
		clearFocus();
		emit modelChanged();
	}
}


void QtCircle2DWidget::textChanged()
{
	bool acceptable_input;

	acceptable_input = radius_line_edit_->hasAcceptableInput();

	// resize the dialog to automaticall fit all of the text displayed
	QFontMetrics fm(font());
	radius_line_edit_->setFixedWidth(fm.width(radius_line_edit_->text() + "  "));
}

bool QtCircle2DWidget::event(QEvent *event)
{
	if(event->type() == QEvent::FocusOut)
	{
		radius_line_edit_->setText(QString("%1").arg(circle_primitive_->GetRadiusValue()));
		textChanged();
	}
	
	return QGraphicsProxyWidget::event(event);
}

void QtCircle2DWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{

	QGraphicsProxyWidget::paint(painter, option,widget);	
}

void QtCircle2DWidget::UpdateGeometry(double scale)
{
	double text_s;
	double text_t;

	text_s = circle_primitive_->GetSCenterValue() + circle_primitive_->GetTextRadius()*cos(circle_primitive_->GetTextAngle());
	text_t = circle_primitive_->GetTCenterValue() + circle_primitive_->GetTextRadius()*sin(circle_primitive_->GetTextAngle());

	QTransform transform;
	transform.translate(text_s,-text_t);
	transform.scale(1.0/scale, 1.0/scale);
	
	transform.translate(-radius_line_edit_->width()*0.5,-radius_line_edit_->height()*0.5);

	setTransform(transform);
}


