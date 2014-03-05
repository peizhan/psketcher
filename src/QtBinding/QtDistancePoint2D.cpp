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

#include "QtDistancePoint2D.h"

QtDistancePoint2D::QtDistancePoint2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
DistancePoint2D(id,psketcher_model),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	distance_widget_ = 0;

	// Display the newly create ais_object
	Display();
}

QtDistancePoint2D::QtDistancePoint2D(QGraphicsItem * parent,const Point2DPointer point1, const Point2DPointer point2,double distance):
QtPrimitiveBase(parent),
DistancePoint2D(point1,point2,distance),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	distance_widget_ = 0;

	// Display the newly create ais_object
	Display();
}


void QtDistancePoint2D::UpdateDisplay()
{
	Display();

	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtDistancePoint2D::boundingRect() const
{ 
	mmcMatrix point1 = point1_->GetmmcMatrix();
	mmcMatrix point2 = point2_->GetmmcMatrix();
	mmcMatrix tangent = (point2-point1).GetNormalized();

	mmcMatrix normal(2,1);
	normal(0,0) = -tangent(1,0);
	normal(1,0) = tangent(0,0);

	mmcMatrix text_location = point1 + tangent*text_position_->GetValue() + normal*text_offset_->GetValue();	

	double offset = (text_location - point1).DotProduct(normal);

	mmcMatrix arrow_end_1 = point1 + offset*normal;
	mmcMatrix arrow_end_2 = point2 + offset*normal;

	double max_x = qMax(qMax(point1(0,0),point2(0,0)),qMax(arrow_end_1(0,0),arrow_end_2(0,0)));
	double max_y = qMax(qMax(point1(1,0),point2(1,0)),qMax(arrow_end_1(1,0),arrow_end_2(1,0)));

	double min_x = qMin(qMin(point1(0,0),point2(0,0)),qMin(arrow_end_1(0,0),arrow_end_2(0,0)));
	double min_y = qMin(qMin(point1(1,0),point2(1,0)),qMin(arrow_end_1(1,0),arrow_end_2(1,0)));

	return QRectF(QPointF(min_x-GetBoundingRectPad(),-(min_y-GetBoundingRectPad())),QPointF(max_x+GetBoundingRectPad(),-(max_y+GetBoundingRectPad())));
}

void QtDistancePoint2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */)
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
	
	double leader_gap = current_properties.GetLeaderGap()/level_of_detail;
	double leader_extension = current_properties.GetLeaderExtension()/level_of_detail;

	double arrow_head_length = current_properties.GetArrowHeadLength()/level_of_detail;
	double arrow_head_width = current_properties.GetArrowHeadWidth()/level_of_detail;

	painter->setPen(current_properties.GetPen(level_of_detail));
	painter->setBrush(current_properties.GetBrush());

	mmcMatrix point1 = point1_->GetmmcMatrix();
	mmcMatrix point2 = point2_->GetmmcMatrix();

	mmcMatrix tangent = (point2-point1);
	double tangent_magnitude = tangent.GetMagnitude();
	if (tangent_magnitude > 0.0)
	{
		tangent = tangent.GetScaled(1.0/tangent_magnitude);
	} else {
		// tangent vector has zero length, define an arbitrary tangent vector to avoid divide by zero
		tangent(0,0) = 1.0;
		tangent(1,0) = 0.0;	
	}

	mmcMatrix normal(2,1);
	normal(0,0) = -tangent(1,0);
	normal(1,0) = tangent(0,0);
	
	mmcMatrix text_location = point1 + tangent*text_position_->GetValue() + normal*text_offset_->GetValue();
	
	double offset = (text_location - point1).DotProduct(normal);
	double offset_sign = offset >= 0.0 ? 1.0 : -1.0;

	
	mmcMatrix arrow_end_1 = point1 + offset*normal;
	mmcMatrix arrow_end_2 = point2 + offset*normal;

	mmcMatrix leader_start_1 = point1 + offset_sign*leader_gap*normal;
	mmcMatrix leader_start_2 = point2 + offset_sign*leader_gap*normal;
	mmcMatrix leader_end_1 = point1 + (offset+offset_sign*leader_extension)*normal;
	mmcMatrix leader_end_2 = point2 + (offset+offset_sign*leader_extension)*normal;

	// draw leader lines
	painter->drawLine(QPointF(leader_start_1(0,0),-leader_start_1(1,0)),QPointF(leader_end_1(0,0),-leader_end_1(1,0)));
	painter->drawLine(QPointF(leader_start_2(0,0),-leader_start_2(1,0)),QPointF(leader_end_2(0,0),-leader_end_2(1,0)));

	// draw line from arrow to text
	painter->drawLine(QPointF(arrow_end_1(0,0),-arrow_end_1(1,0)),QPointF(text_location(0,0),-text_location(1,0)));

	QPainterPath arrow_selection_path;
	QPolygonF arrow = GetArrowPolygonAndSelectionPath(arrow_end_1(0,0),-arrow_end_1(1,0),arrow_end_2(0,0),-arrow_end_2(1,0),arrow_head_length,arrow_head_width,arrow_selection_path,level_of_detail);
	current_shape_ = arrow_selection_path;

	painter->drawPolygon(arrow);
	//painter->drawPath(arrow_selection_path);

	// create the line edit widget graphics item
	if(distance_widget_ == 0)
	{
		// @fixme need to make sure the following dyname_cast won't create a pointer that is need used even if this shared_ptr class is freed from memory
		distance_widget_ = new QtDistancePoint2DWidget(shared_from_this(),dynamic_cast<QGraphicsItem*>(const_cast<QtDistancePoint2D*>(this)));
	}

	distance_widget_->UpdateGeometry(level_of_detail);
}


void QtDistancePoint2D::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(event->buttons() & Qt::LeftButton)
	{	
        pending_db_save_ = true;
        
		// move the point to the new global position
		SetSTTextLocation(event->scenePos().x(),-event->scenePos().y(),false /*update_db*/);

		// force a update of the display so that the drag event is seen interactively
		scene()->update();

		//@fixme After drag operation is finished, need to trigger QtSketch's modelChanged() slot

	} else {
		// not handling this event, let the base class do its thing
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void QtDistancePoint2D::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    if (event->button() & Qt::LeftButton && pending_db_save_) 
    {
        // if there is a pending db save, do the save now (this happens at the end of a drag event)
        SetTextLocation(GetTextPosition(),GetTextOffset());

        pending_db_save_ = false;
    }

    // let the base class do it's thing
    QGraphicsItem::mouseReleaseEvent(event);
}


QtDistancePoint2DWidget::QtDistancePoint2DWidget(QtDistancePoint2DPointer distance_constraint, QGraphicsItem *parent) :
distance_constraint_(distance_constraint), QGraphicsProxyWidget(parent)
{
	//setFlags(ItemIgnoresTransformations);

	// create widget
	distance_line_edit_ = new QLineEdit;
	distance_line_edit_->setStyleSheet(LineEditStyleSheet);
	distance_line_edit_->setValidator(new QDoubleValidator(this));
	distance_line_edit_->setAlignment(Qt::AlignCenter);
	distance_line_edit_->setText(QString("%1").arg(distance_constraint_->GetValue()));
	textChanged();
	//distance_line_edit_->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
	distance_line_edit_->resize(distance_line_edit_->minimumSizeHint());
	connect(distance_line_edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
	connect(distance_line_edit_, SIGNAL(returnPressed()), this, SLOT(applyChanges()));

	// package widget
	setWidget(distance_line_edit_);
}



// apply the changes if valid values have been entered
void QtDistancePoint2DWidget::applyChanges()
{
	if(distance_line_edit_->hasAcceptableInput())
	{
		distance_constraint_->SetValue(distance_line_edit_->text().toDouble());
		clearFocus();
		emit modelChanged(tr("Distance constraint changed"));
	}
}


void QtDistancePoint2DWidget::textChanged()
{
	// resize the dialog to automaticall fit all of the text displayed
	QFontMetrics fm(font());
	distance_line_edit_->setFixedWidth(fm.width(distance_line_edit_->text() + "  "));
}

bool QtDistancePoint2DWidget::event(QEvent *event)
{
	if(event->type() == QEvent::FocusOut)
	{
		distance_line_edit_->setText(QString("%1").arg(distance_constraint_->GetValue()));
		textChanged();
	}
	
	return QGraphicsProxyWidget::event(event);
}

void QtDistancePoint2DWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{

	QGraphicsProxyWidget::paint(painter, option,widget);	
}

void QtDistancePoint2DWidget::UpdateGeometry(double scale)
{
	mmcMatrix point1 = distance_constraint_->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = distance_constraint_->GetPoint2()->GetmmcMatrix();
	mmcMatrix tangent = (point2-point1).GetNormalized();

	mmcMatrix normal(2,1);
	normal(0,0) = -tangent(1,0);
	normal(1,0) = tangent(0,0);

	mmcMatrix text_position = point1 + tangent*distance_constraint_->GetTextPosition() + normal*distance_constraint_->GetTextOffset();

	QTransform transform;
	transform.translate(text_position(0,0),-text_position(1,0));
	transform.scale(1.0/scale, 1.0/scale);
	
	transform.translate(-distance_line_edit_->width()*0.5,-distance_line_edit_->height()*0.5);

	setTransform(transform);
}
