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

#include "QtAngleLine2D.h"

QtAngleLine2D::QtAngleLine2D(QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model):
QtPrimitiveBase(parent),
AngleLine2D(id,psketcher_model),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	angle_widget_ = 0;
	
	// Display the newly create ais_object
	Display();
}

QtAngleLine2D::QtAngleLine2D(QGraphicsItem * parent, const Line2DPointer line1, const Line2DPointer line2, double angle, bool interior_angle):
QtPrimitiveBase(parent),
AngleLine2D(line1,line2,angle,interior_angle),
pending_db_save_(false)
{
	SetProperties(Annotation);
	SetSelectedProperties(SelectedAnnotation);
	SetMouseHoverProperties(HoverAnnotation);

	setZValue(GetProperties().GetZ());

	angle_widget_ = 0;
	
	// Display the newly create ais_object
	Display();
}


void QtAngleLine2D::UpdateDisplay()
{
	Display();

	QtPrimitiveBase::UpdateDisplay();
}

QRectF QtAngleLine2D::boundingRect() const
{ 
	QRectF bounding_rect;

	// first determine the intersection point of the two lines
	double x1 = GetLine1()->GetPoint1()->GetSValue();
	double x2 = GetLine1()->GetPoint2()->GetSValue();
	double x3 = GetLine2()->GetPoint1()->GetSValue();
	double x4 = GetLine2()->GetPoint2()->GetSValue();

	double y1 = GetLine1()->GetPoint1()->GetTValue();
	double y2 = GetLine1()->GetPoint2()->GetTValue();
	double y3 = GetLine2()->GetPoint1()->GetTValue();
	double y4 = GetLine2()->GetPoint2()->GetTValue();

	double denominator = (x1-x2)*(y3-y4)-(x3-x4)*(y1-y2);

	double text_x, text_y;
	double x_center, y_center;
	bool lines_parallel = false;
	double arrow_tip_x1;
	double arrow_tip_x2;
	double arrow_tip_y1;
	double arrow_tip_y2;

	if(denominator == 0.0)
	{
		// Lines are parallel, use the extrema of the lines to define the bounding box

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
		
		bounding_rect = QRectF(QPointF(min_s, -min_t),QPointF(max_s,-max_t));

	} else {
		// lines do intersect
		// finish calculating the intersection point
		double temp1 = x1*y2-y1*x2;
		double temp2 = x3*y4-x4*y3;
		
		x_center = (temp1*(x3-x4)-temp2*(x1-x2))/denominator;
		y_center = (temp1*(y3-y4)-temp2*(y1-y2))/denominator;

		double radius = GetTextRadius() + GetBoundingRectPad();

		bounding_rect = QRectF(QPointF(x_center-radius, -(y_center-radius)),QPointF(x_center+radius,-(y_center+radius)));
	}

	return bounding_rect;
}

void QtAngleLine2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */) 
{
	// @fixme need to handle the case where one of the lines has zero length (make sure there are no divide by zeros)

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
	
	double leader_gap = current_properties.GetLeaderGap()/level_of_detail;
	double leader_extension = current_properties.GetLeaderExtension()/level_of_detail;

	// first determine the intersection point of the two lines
	double x1 = GetLine1()->GetPoint1()->GetSValue();
	double x2 = GetLine1()->GetPoint2()->GetSValue();
	double x3 = GetLine2()->GetPoint1()->GetSValue();
	double x4 = GetLine2()->GetPoint2()->GetSValue();

	double y1 = GetLine1()->GetPoint1()->GetTValue();
	double y2 = GetLine1()->GetPoint2()->GetTValue();
	double y3 = GetLine2()->GetPoint1()->GetTValue();
	double y4 = GetLine2()->GetPoint2()->GetTValue();

	double denominator = (x1-x2)*(y3-y4)-(x3-x4)*(y1-y2);

	double text_x, text_y;
	double x_center, y_center;
	bool lines_parallel = false;
	double arrow_tip_x1;
	double arrow_tip_x2;
	double arrow_tip_y1;
	double arrow_tip_y2;

	if(denominator == 0.0)
	{
		// Lines are parallel
		lines_parallel = true;

		text_x = text_s_->GetValue();
		text_y = text_t_->GetValue();
	} else {
		// lines do intersect
		// finish calculating the intersection point
		double temp1 = x1*y2-y1*x2;
		double temp2 = x3*y4-x4*y3;
		
		x_center = (temp1*(x3-x4)-temp2*(x1-x2))/denominator;
		y_center = (temp1*(y3-y4)-temp2*(y1-y2))/denominator;

		text_x = x_center + GetTextRadius()*cos(GetTextAngle());
		text_y = y_center + GetTextRadius()*sin(GetTextAngle());
	}

	if(!lines_parallel)
	{	
		// normal case where lines are not parallel
		double line1_theta = atan2(y2-y1, x2-x1);
		double line2_theta = atan2(y4-y3, x4-x3);
		if(line2_theta < line1_theta)
			line2_theta += 2.0*mmcPI;

		double theta;
		bool theta_is_interior;	

		if(line2_theta - line1_theta > mmcPI)
		{
			theta_is_interior = false;
			theta = line2_theta - line1_theta - mmcPI;
		} else {
			theta_is_interior = true;
			theta = line2_theta - line1_theta;
		}

		double alpha1 = line1_theta;

		double alpha2 = alpha1 + theta;
		double alpha3 = alpha1 + mmcPI;
		double alpha4 = alpha1 + mmcPI + theta;
		double alpha5 = alpha1 + 2.0*mmcPI;

		double text_theta = GetTextAngle();

		// determine which sector the text is nearest too
		// first increment text_theta so that it is greater then alpha1 and less then alpha5	
		if(text_theta < alpha1)
		{
			while (text_theta < alpha1)
				text_theta += 2.0*mmcPI;
		} else if (text_theta > alpha5) {
			while (text_theta > alpha5)
				text_theta -= 2.0*mmcPI;
		}
		
		double arrow_arc_theta1;
		double arrow_arc_theta2;
		if(IsInteriorAngle() && theta_is_interior || !IsInteriorAngle() && !theta_is_interior)
		{
			// either sector 1 or sector 3 for an interior angle
			double sector_1_delta = fabs((alpha1+alpha2)*0.5-text_theta);
			double sector_3_delta = fabs((alpha3+alpha4)*0.5-text_theta);

			if(sector_1_delta < sector_3_delta)
			{
				arrow_arc_theta1 = alpha1;
				arrow_arc_theta2 = alpha2;
			} else {
				arrow_arc_theta1 = alpha3;
				arrow_arc_theta2 = alpha4;
			}

			// theta1 correspnds to line1 
			arrow_tip_x1 = x_center + GetTextRadius()*cos(arrow_arc_theta1);
			arrow_tip_y1 = y_center + GetTextRadius()*sin(arrow_arc_theta1);

			arrow_tip_x2 = x_center + GetTextRadius()*cos(arrow_arc_theta2);
			arrow_tip_y2 = y_center + GetTextRadius()*sin(arrow_arc_theta2);

		} else {
			// either sector 2 or sector 4 for an exterior angle
			double sector_2_delta = fabs((alpha2+alpha3)*0.5-text_theta);
			double sector_4_delta = fabs((alpha4+alpha5)*0.5-text_theta);

			if(sector_2_delta < sector_4_delta)
			{
				arrow_arc_theta1 = alpha2;
				arrow_arc_theta2 = alpha3;
			} else {
				arrow_arc_theta1 = alpha4;
				arrow_arc_theta2 = alpha5;
			}

			// theta1 correspnds to line2 
			arrow_tip_x2 = x_center + GetTextRadius()*cos(arrow_arc_theta1);
			arrow_tip_y2 = y_center + GetTextRadius()*sin(arrow_arc_theta1);

			arrow_tip_x1 = x_center + GetTextRadius()*cos(arrow_arc_theta2);
			arrow_tip_y1 = y_center + GetTextRadius()*sin(arrow_arc_theta2);
		}

		// display the arrow arc
		QPainterPath selection_path;
		QPainterPath arrow_arc = GetArcArrowPathAndSelectionPath(x_center, -y_center, GetTextRadius(),arrow_arc_theta1,arrow_arc_theta2,15.0/level_of_detail,12.0/level_of_detail,selection_path,level_of_detail);
		painter->drawPath(arrow_arc);
		current_shape_ = selection_path;
		
		// draw an additional arc to the text if text is outside of the arrow arc
		QRectF rect(QPointF(x_center-GetTextRadius(),-y_center-GetTextRadius()),
					QPointF(x_center+GetTextRadius(),-y_center+GetTextRadius()));
		if(text_theta < arrow_arc_theta1)
			painter->drawArc(rect,(text_theta)*(180.0/mmcPI)*16.0,(arrow_arc_theta1-text_theta)*(180.0/mmcPI)*16.0);
		else if (text_theta > arrow_arc_theta2) 
			painter->drawArc(rect,(arrow_arc_theta2)*(180.0/mmcPI)*16.0,((text_theta)-(arrow_arc_theta2))*(180.0/mmcPI)*16.0);

		// display the editable text
		// create the line edit widget graphics item
		if(angle_widget_ == 0)
		{
			// @fixme need to make sure the following dyname_cast won't create a pointer that is need used even if this shared_ptr class is freed from memory
			angle_widget_ = new QtAngleLine2DWidget(shared_from_this(),dynamic_cast<QGraphicsItem*>(const_cast<QtAngleLine2D*>(this)));
		}
		angle_widget_->UpdateGeometry(text_x, text_y, level_of_detail);

		// create leader for line1 if necessary
		double delta1, delta2;
		double line1_length = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
		delta1 = sqrt((arrow_tip_x1-x1)*(arrow_tip_x1-x1)+(arrow_tip_y1-y1)*(arrow_tip_y1-y1));
		delta2 = sqrt((arrow_tip_x1-x2)*(arrow_tip_x1-x2)+(arrow_tip_y1-y2)*(arrow_tip_y1-y2));
		if(delta1 > line1_length && delta1 > delta2)
		{
			double direction_x = arrow_tip_x1-x1;
			double direction_y = arrow_tip_y1-y1;
			double direction_length = sqrt(direction_x*direction_x + direction_y*direction_y);
			direction_x = direction_x / direction_length;
			direction_y = direction_y / direction_length;
	
			painter->drawLine(QPointF(x2+leader_gap*direction_x,-(y2+leader_gap*direction_y)),
							QPointF(x1+(delta1+leader_extension)*direction_x,-(y1+(delta1+leader_extension)*direction_y)));
		} else if (delta2 > line1_length) {
			double direction_x = arrow_tip_x1-x2;
			double direction_y = arrow_tip_y1-y2;
			double direction_length = sqrt(direction_x*direction_x + direction_y*direction_y);
			direction_x = direction_x / direction_length;
			direction_y = direction_y / direction_length;
	
			painter->drawLine(QPointF(x1+leader_gap*direction_x,-(y1+leader_gap*direction_y)),
							QPointF(x2+(delta2+leader_extension)*direction_x,-(y2+(delta2+leader_extension)*direction_y)));
		}
	
		// create leader for line2 if necessary
		double line2_length = sqrt((x3-x4)*(x3-x4)+(y3-y4)*(y3-y4));
		delta1 = sqrt((arrow_tip_x2-x3)*(arrow_tip_x2-x3)+(arrow_tip_y2-y3)*(arrow_tip_y2-y3));
		delta2 = sqrt((arrow_tip_x2-x4)*(arrow_tip_x2-x4)+(arrow_tip_y2-y4)*(arrow_tip_y2-y4));
		if(delta1 > line2_length && delta1 > delta2)
		{
			double direction_x = arrow_tip_x2-x3;
			double direction_y = arrow_tip_y2-y3;
			double direction_length = sqrt(direction_x*direction_x + direction_y*direction_y);
			direction_x = direction_x / direction_length;
			direction_y = direction_y / direction_length;
	
			painter->drawLine(QPointF(x4+leader_gap*direction_x,-(y4+leader_gap*direction_y)),
							QPointF(x3+(delta1+leader_extension)*direction_x,-(y3+(delta1+leader_extension)*direction_y)));
		} else if (delta2 > line2_length) {
			double direction_x = arrow_tip_x2-x4;
			double direction_y = arrow_tip_y2-y4;
			double direction_length = sqrt(direction_x*direction_x + direction_y*direction_y);
			direction_x = direction_x / direction_length;
			direction_y = direction_y / direction_length;
	
			painter->drawLine(QPointF(x3+leader_gap*direction_x,-(y3+leader_gap*direction_y)),
							QPointF(x4+(delta2+leader_extension)*direction_x,-(y4+(delta2+leader_extension)*direction_y)));
		}

	} else {
		// the case where the lines are parallel

		// display an arrow to the center point of each of the lines
		QPainterPath selection_path;

		QPolygonF arrow1 = GetArrowPolygonAndSelectionPath(text_x, -text_y, 0.5*(x1+x2), -0.5*(y1+y2),15.0/level_of_detail,12.0/level_of_detail,selection_path,level_of_detail);
		painter->drawPolygon(arrow1);
		current_shape_ = selection_path;

		QPolygonF arrow2 = GetArrowPolygonAndSelectionPath(text_x, -text_y, 0.5*(x3+x4), -0.5*(y3+y4),15.0/level_of_detail,12.0/level_of_detail,selection_path,level_of_detail);
		painter->drawPolygon(arrow2);
		current_shape_.addPath(selection_path);

		// display the editable text
		// create the line edit widget graphics item
		if(angle_widget_ == 0)
		{
			// @fixme need to make sure the following dyname_cast won't create a pointer that is need used even if this shared_ptr class is freed from memory
			angle_widget_ = new QtAngleLine2DWidget(shared_from_this(),dynamic_cast<QGraphicsItem*>(const_cast<QtAngleLine2D*>(this)));
		}
		angle_widget_->UpdateGeometry(text_x, text_y, level_of_detail);
		
	} // if(!lines_parallel)
	
}


void QtAngleLine2D::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
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

void QtAngleLine2D::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
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

QtAngleLine2DWidget::QtAngleLine2DWidget(QtAngleLine2DPointer arc_primitive, QGraphicsItem *parent) :
angle_constraint_primitive_(arc_primitive), QGraphicsProxyWidget(parent)
{
	//setFlags(ItemIgnoresTransformations);

	// create widget
	angle_line_edit_ = new QLineEdit;
	angle_line_edit_->setStyleSheet(LineEditStyleSheet);
	angle_line_edit_->setValidator(new QDoubleValidator(this));
	angle_line_edit_->setAlignment(Qt::AlignCenter);
	angle_line_edit_->setText(QString("%1").arg(angle_constraint_primitive_->GetAngleValue()*(180.0/mmcPI)));
	textChanged();
	//angle_line_edit_->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
	angle_line_edit_->resize(angle_line_edit_->minimumSizeHint());
	connect(angle_line_edit_, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
	connect(angle_line_edit_, SIGNAL(returnPressed()), this, SLOT(applyChanges()));

	// package widget
	setWidget(angle_line_edit_);
}



// apply the changes if valid values have been entered
void QtAngleLine2DWidget::applyChanges()
{
	if(angle_line_edit_->hasAcceptableInput())
	{
		angle_constraint_primitive_->SetAngleValue(angle_line_edit_->text().toDouble()*(mmcPI/180.0));
		clearFocus();
		emit modelChanged();
	}
}


void QtAngleLine2DWidget::textChanged()
{
	bool acceptable_input;

	acceptable_input = angle_line_edit_->hasAcceptableInput();

	// resize the dialog to automaticall fit all of the text displayed
	QFontMetrics fm(font());
	angle_line_edit_->setFixedWidth(fm.width(angle_line_edit_->text() + "  "));
}

bool QtAngleLine2DWidget::event(QEvent *event)
{
	if(event->type() == QEvent::FocusOut)
	{
		angle_line_edit_->setText(QString("%1").arg(angle_constraint_primitive_->GetAngleValue()*(180.0/mmcPI)));
		textChanged();
	}
	
	return QGraphicsProxyWidget::event(event);
}

void QtAngleLine2DWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
	QGraphicsProxyWidget::paint(painter, option,widget);	
}

void QtAngleLine2DWidget::UpdateGeometry(double text_s, double text_t, double scale)
{
	QTransform transform;
	transform.translate(text_s,-text_t);
	transform.scale(1.0/scale, 1.0/scale);
	
	transform.translate(-angle_line_edit_->width()*0.5,-angle_line_edit_->height()*0.5);

	setTransform(transform);
}

