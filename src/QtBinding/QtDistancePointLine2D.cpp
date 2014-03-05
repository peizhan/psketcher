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

#include "QtDistancePointLine2D.h"

QtDistancePointLine2D::QtDistancePointLine2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
DistancePointLine2D(id,psketcher_model),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	distance_widget_ = 0;

	// Display the newly created object
	Display();
}

QtDistancePointLine2D::QtDistancePointLine2D(QGraphicsItem * parent,const Point2DPointer point, const Line2DPointer line,double distance):
QtPrimitiveBase(parent),
DistancePointLine2D(point,line,distance),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	distance_widget_ = 0;

	// Display the newly create object
	Display();
}


void QtDistancePointLine2D::UpdateDisplay()
{
	Display();

	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtDistancePointLine2D::boundingRect() const
{ 
	mmcMatrix point1 = line_->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = line_->GetPoint2()->GetmmcMatrix();
	mmcMatrix point3 = point_->GetmmcMatrix();
	mmcMatrix tangent = point2-point1;

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

	mmcMatrix arrow_end_1 = point1 + text_position_->GetValue()*tangent;
	mmcMatrix arrow_end_2 = point1 + text_position_->GetValue()*tangent + normal*GetActualDistance();

	double max_x = qMax(qMax(qMax(point1(0,0),point2(0,0)),qMax(arrow_end_1(0,0),arrow_end_2(0,0))),point3(0,0));
	double max_y = qMax(qMax(qMax(point1(1,0),point2(1,0)),qMax(arrow_end_1(1,0),arrow_end_2(1,0))),point3(1,0));

	double min_x = qMin(qMin(qMin(point1(0,0),point2(0,0)),qMin(arrow_end_1(0,0),arrow_end_2(0,0))),point3(0,0));
	double min_y = qMin(qMin(qMin(point1(1,0),point2(1,0)),qMin(arrow_end_1(1,0),arrow_end_2(1,0))),point3(1,0));

	return QRectF(QPointF(min_x-GetBoundingRectPad(),-(min_y-GetBoundingRectPad())),QPointF(max_x+GetBoundingRectPad(),-(max_y+GetBoundingRectPad())));
}

void QtDistancePointLine2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */)
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

	mmcMatrix point1 = line_->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = line_->GetPoint2()->GetmmcMatrix();
	mmcMatrix point3 = point_->GetmmcMatrix();

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

	double offset_direction = normal.DotProduct(point3-point2) > 0 ? 1.0 : -1.0;
	
	mmcMatrix text_location = point1 + tangent*text_position_->GetValue() + normal*text_offset_->GetValue();
	
	mmcMatrix arrow_end_1 = point1 + text_position_->GetValue()*tangent;
	mmcMatrix arrow_end_2 = point1 + text_position_->GetValue()*tangent + offset_direction*normal*GetActualDistance();

	// define leader 1 (this is the leader at the point end of the distance constraint)
	double leader_one_direction = (arrow_end_2-point3).DotProduct(tangent) >= 0.0 ? 1.0 : -1.0;

	mmcMatrix leader_start_1 = point3 + leader_one_direction*leader_gap*tangent;
	mmcMatrix leader_end_1 = arrow_end_2 + (leader_one_direction*leader_extension)*tangent;

	double leader_two_direction = (arrow_end_1-point1).DotProduct(tangent) >= 0.0 ? 1.0 : -1.0;
	double leader_two_needed = false;

	// define leader 2 if needed (this is the leader at the line end of the distance constraint)
	mmcMatrix leader_start_2,leader_end_2;
	if(leader_two_direction > 0)
	{
		if(text_position_->GetValue() > leader_gap + (point2-point1).GetMagnitude())
		{
			leader_two_needed = true;
			leader_start_2 = point2 + leader_two_direction*leader_gap*tangent;
			leader_end_2 = arrow_end_1 + (leader_two_direction*leader_extension)*tangent;
		}
	} else {
		if(text_position_->GetValue() < -leader_gap)
		{
			leader_two_needed = true;
			leader_start_2 = point1 + leader_two_direction*leader_gap*tangent;
			leader_end_2 = arrow_end_1 + (leader_two_direction*leader_extension)*tangent;
		}		
	}

	// draw leader lines
	painter->drawLine(QPointF(leader_start_1(0,0),-leader_start_1(1,0)),QPointF(leader_end_1(0,0),-leader_end_1(1,0)));
	if(leader_two_needed)
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
		distance_widget_ = new QtDistancePointLine2DWidget(shared_from_this(),dynamic_cast<QGraphicsItem*>(const_cast<QtDistancePointLine2D*>(this)));
	}

	distance_widget_->UpdateGeometry(level_of_detail);
}


void QtDistancePointLine2D::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
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

void QtDistancePointLine2D::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
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


QtDistancePointLine2DWidget::QtDistancePointLine2DWidget(QtDistancePointLine2DPointer distance_constraint, QGraphicsItem *parent) :
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
void QtDistancePointLine2DWidget::applyChanges()
{
	if(distance_line_edit_->hasAcceptableInput())
	{
		distance_constraint_->SetValue(distance_line_edit_->text().toDouble());
		clearFocus();
		emit modelChanged();
	}
}


void QtDistancePointLine2DWidget::textChanged()
{
	bool acceptable_input;

	acceptable_input = distance_line_edit_->hasAcceptableInput();

	// resize the dialog to automaticall fit all of the text displayed
	QFontMetrics fm(font());
	distance_line_edit_->setFixedWidth(fm.width(distance_line_edit_->text() + "  "));
}

bool QtDistancePointLine2DWidget::event(QEvent *event)
{
	if(event->type() == QEvent::FocusOut)
	{
		distance_line_edit_->setText(QString("%1").arg(distance_constraint_->GetValue()));
		textChanged();
	}
	
	return QGraphicsProxyWidget::event(event);
}

void QtDistancePointLine2DWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{

	QGraphicsProxyWidget::paint(painter, option,widget);	
}

void QtDistancePointLine2DWidget::UpdateGeometry(double scale)
{
	mmcMatrix point1 = distance_constraint_->GetLine()->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = distance_constraint_->GetLine()->GetPoint2()->GetmmcMatrix();
	mmcMatrix tangent = point2-point1;

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

	mmcMatrix text_position = point1 + tangent*distance_constraint_->GetTextPosition() + normal*distance_constraint_->GetTextOffset();	

	QTransform transform;
	transform.translate(text_position(0,0),-text_position(1,0));
	transform.scale(1.0/scale, 1.0/scale);
	
	transform.translate(-distance_line_edit_->width()*0.5,-distance_line_edit_->height()*0.5);

	setTransform(transform);
}
