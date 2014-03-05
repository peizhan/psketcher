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

#include "QtPrimitiveBase.h"

QtPrimitiveBase::QtPrimitiveBase(QGraphicsItem * parent ) : 
QGraphicsItem(parent),
selection_diameter_(6.0),
bounding_rect_pad_(10.0)
{
	// by default use primitive display properties
	SetProperties(Primitive);
	SetSelectedProperties(SelectedPrimitive);
	SetMouseHoverProperties(HoverPrimitive);

	// accept mouse hover events (false by default)
	setAcceptHoverEvents(true);
}

void QtPrimitiveBase::Display()
{

}

void QtPrimitiveBase::UpdateDisplay()
{
	// if this method is called, it is likely that the geometry of the primitives have changed
	// it is called after the constraint network is solved
	prepareGeometryChange();  // let the QGraphicsView know that bound rectangles and selection shapes could be changing
	
	// go ahead and redraw everything using its QGraphicsItem method
	update();
}

bool QtPrimitiveBase::IsSelected()
{
	return isSelected();
}

void QtPrimitiveBase::SetSelectable(bool selectable)
{
	setFlag(QGraphicsItem::ItemIsSelectable, selectable);
}

// Erase current ais_object from the ais_context
void QtPrimitiveBase::Erase()
{
	QGraphicsScene *current_scene = scene();

	if(current_scene != 0)
	{
		current_scene->removeItem(this);
	}
}

void QtPrimitiveBase::PaintPoint(QPainter *painter, const QStyleOptionGraphicsItem *option, double x, double y)
{
	double level_of_detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	
	double radius = 3.0/level_of_detail;
	QRectF rect(QPointF(x-radius,y-radius),
 				QPointF(x+radius,y+radius));
	painter->drawEllipse(rect);
}

void QtPrimitiveBase::PaintPointAndSelectionPath(QPainter *painter, const QStyleOptionGraphicsItem *option, double x, double y,QPainterPath &selection_path)
{
	double level_of_detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
  
	selection_path.addEllipse(QPointF(x,y),1.0*selection_diameter_/level_of_detail,1.0*selection_diameter_/level_of_detail);

	PaintPoint(painter, option, x, y);
}

QPolygonF QtPrimitiveBase::GetArrowPolygon(double x1, double y1, double x2, double y2, double arrow_head_length, double arrow_head_width, bool double_arrow) const
{
	mmcMatrix start_point(2,1);
	start_point(0,0) = x1;
	start_point(1,0) = y1;

	mmcMatrix end_point(2,1);
	end_point(0,0) = x2;
	end_point(1,0) = y2;
	
    if(end_point.GetDistanceTo(start_point) == 0)
    {
        QPolygonF polygon;
        return polygon;
    }

	mmcMatrix line_vector = (end_point - start_point).GetNormalized();
	
	mmcMatrix normal_vector(2,1);
	normal_vector(0,0) = line_vector(1,0);
	normal_vector(1,0) = -line_vector(0,0);

	mmcMatrix arrow_base = end_point - arrow_head_length*line_vector;
	mmcMatrix corner_1 = arrow_base + 0.5*arrow_head_width*normal_vector;
	mmcMatrix corner_2 = arrow_base - 0.5*arrow_head_width*normal_vector;

	QPolygonF polygon;

	if(!double_arrow)
	{
		// single ended arrow

		polygon << QPointF(x1,y1) 
				<< QPointF(arrow_base(0,0),arrow_base(1,0)) 
				<< QPointF(corner_1(0,0),corner_1(1,0))
				<< QPointF(end_point(0,0),end_point(1,0))
				<< QPointF(corner_2(0,0),corner_2(1,0))
				<< QPointF(arrow_base(0,0),arrow_base(1,0))
				<< QPointF(x1,y1);
	} else {
		// double ended arrow

		mmcMatrix start_arrow_base = start_point + arrow_head_length*line_vector;
		mmcMatrix start_corner_1 = start_arrow_base + 0.5*arrow_head_width*normal_vector;
		mmcMatrix start_corner_2 = start_arrow_base - 0.5*arrow_head_width*normal_vector;

		polygon << QPointF(start_arrow_base(0,0),start_arrow_base(1,0)) 
				<< QPointF(start_corner_1(0,0),start_corner_1(1,0))
				<< QPointF(start_point(0,0),start_point(1,0))
				<< QPointF(start_corner_2(0,0),start_corner_2(1,0))
				<< QPointF(start_arrow_base(0,0),start_arrow_base(1,0))
				<< QPointF(arrow_base(0,0),arrow_base(1,0)) 
				<< QPointF(corner_1(0,0),corner_1(1,0))
				<< QPointF(end_point(0,0),end_point(1,0))
				<< QPointF(corner_2(0,0),corner_2(1,0))
				<< QPointF(arrow_base(0,0),arrow_base(1,0))
				<< QPointF(start_arrow_base(0,0),start_arrow_base(1,0));
	}

	return polygon;
}

QPainterPath QtPrimitiveBase::GetArcArrowPath(double x_center, double y_center, double radius, double theta1, double theta2, double arrow_head_length, double arrow_head_width) const
{
	// theta1 and theta2 are in radians
	QPainterPath path;

	// define everyting that will be needed to create the arc arrow
	QRectF rect(QPointF(x_center-radius,y_center-radius),
 				QPointF(x_center+radius,y_center+radius));
	
	double arrow_base_theta1;
	double arrow_base_theta2;

	if(theta2 >= theta1)
	{
		arrow_base_theta1 = theta1 + arrow_head_length/radius;
		arrow_base_theta2 = theta2 - arrow_head_length/radius;
	} else {
		arrow_base_theta1 = theta1 - arrow_head_length/radius;
		arrow_base_theta2 = theta2 + arrow_head_length/radius;
	}

	mmcMatrix normal_1(2,1);
	mmcMatrix normal_2(2,1);

	mmcMatrix base_point_1;
	mmcMatrix base_point_2;
	mmcMatrix tip_point_1(2,1);
	mmcMatrix tip_point_2(2,1);

	mmcMatrix center(2,1);
	center(0,0) = x_center;
	center(1,0) = y_center;

	tip_point_1(0,0) = radius*cos(theta1);
	tip_point_1(1,0) = -radius*sin(theta1);
	tip_point_1 = tip_point_1 + center;

	normal_1(0,0) = cos(arrow_base_theta1);
	normal_1(1,0) = -sin(arrow_base_theta1);

	base_point_1 = radius * normal_1 + center;

	tip_point_2(0,0) = radius*cos(theta2);
	tip_point_2(1,0) = -radius*sin(theta2);
	tip_point_2 = tip_point_2 + center;

	normal_2(0,0) = cos(arrow_base_theta2);
	normal_2(1,0) = -sin(arrow_base_theta2);

	base_point_2 = radius * normal_2 + center;

	mmcMatrix arrow_1_corner_1 = base_point_1 + 0.5*arrow_head_width*normal_1;
	mmcMatrix arrow_1_corner_2 = base_point_1 - 0.5*arrow_head_width*normal_1;

	mmcMatrix arrow_2_corner_1 = base_point_2 + 0.5*arrow_head_width*normal_2;
	mmcMatrix arrow_2_corner_2 = base_point_2 - 0.5*arrow_head_width*normal_2;

	// create the arc arrow
	path.moveTo(base_point_1(0,0),base_point_1(1,0));
	path.lineTo(arrow_1_corner_1(0,0), arrow_1_corner_1(1,0));
	path.lineTo(tip_point_1(0,0), tip_point_1(1,0));
	path.lineTo(arrow_1_corner_2(0,0), arrow_1_corner_2(1,0));
	//path.moveTo(base_point_1(0,0),base_point_1(1,0));
	path.arcTo(rect,arrow_base_theta1*(180.0/mmcPI),(arrow_base_theta2-arrow_base_theta1)*(180.0/mmcPI));
	path.lineTo(arrow_2_corner_1(0,0), arrow_2_corner_1(1,0));
	path.lineTo(tip_point_2(0,0), tip_point_2(1,0));
	path.lineTo(arrow_2_corner_2(0,0), arrow_2_corner_2(1,0));

	path.arcTo(rect,arrow_base_theta2*(180.0/mmcPI),(arrow_base_theta1-arrow_base_theta2)*(180.0/mmcPI));

	return path;
}

QLineF QtPrimitiveBase::GetLineAndSelectionPath(mmcMatrix point1, mmcMatrix point2, QPainterPath &selection_path, double scale) const
{
	// create selection area polygon
	mmcMatrix tangent = point2 - point1;
	mmcMatrix normal(2,1);
	
	normal(0,0) = -tangent(1,0); 
	normal(1,0) = tangent(0,0);

	double length = normal.GetMagnitude();
	double radius = 0.5*selection_diameter_/scale;

	if(length > 0.0){
		// line has non-zero length 
		normal = (1.0/length)*normal;
		tangent = (1.0/length)*tangent;		

		if(2.0*radius > length)
			tangent = 0.0*tangent;

		mmcMatrix corner1 = point1+radius*tangent + radius*normal;
		mmcMatrix corner2 = point2-radius*tangent + radius*normal;
		mmcMatrix corner3 = point2-radius*tangent - radius*normal;
		mmcMatrix corner4 = point1+radius*tangent - radius*normal;

		QPolygonF line_selection_polygon;

		line_selection_polygon << QPointF(corner1(0,0),corner1(1,0));
		line_selection_polygon << QPointF(corner2(0,0),corner2(1,0));
		line_selection_polygon << QPointF(corner3(0,0),corner3(1,0));
		line_selection_polygon << QPointF(corner4(0,0),corner4(1,0));

		selection_path.addPolygon(line_selection_polygon);
		selection_path.closeSubpath();

	} else {
		// this line has zero length, so make a selection circle rather than a rectangle
		selection_path.addEllipse(QPointF(point1(0,0),point1(1,0)),0.5*(selection_diameter_/scale),0.5*(selection_diameter_/scale));
	}

	return QLineF(QPointF(point1(0,0),point1(1,0)),QPointF(point2(0,0),point2(1,0)));
}

QLineF QtPrimitiveBase::GetLineAndSelectionPath(double x1, double y1, double x2, double y2, QPainterPath &selection_path, double scale) const
{
	mmcMatrix point1(2,1);
	mmcMatrix point2(2,1);

	point1(0,0) = x1;
	point1(1,0) = y1;
	
	point2(0,0) = x2;
	point2(1,0) = y2;

	return GetLineAndSelectionPath(point1, point2, selection_path, scale);
}

QPainterPath QtPrimitiveBase::GetArcAndSelectionPath(double center_x, double center_y, double radius, double theta1, double theta2, QPainterPath &selection_path, double scale) const
{
	if(theta1 > theta2)
	{
		double temp;
		temp = theta1;
		theta1 = theta2;
		theta2 = temp;
	}

	QPainterPath arc_path;
	
	QRectF rect(QPointF(center_x-radius,center_y-radius),QPointF(center_x+radius,center_y+radius));

	arc_path.moveTo(center_x + radius*cos(theta1), center_y - radius*sin(theta1));
	arc_path.arcTo(rect,theta1*(180.0/mmcPI),(theta2-theta1)*(180.0/mmcPI));
	arc_path.arcTo(rect,theta2*(180.0/mmcPI),(theta1-theta2)*(180.0/mmcPI));

	// create selection path
	double gap_angle = (((0.5*selection_diameter_)/radius))/scale;
	if(2.0*gap_angle > theta2-theta1)
		gap_angle = 0.0;

	double current_radius = (radius+0.5*selection_diameter_/scale);

	rect = QRectF(QPointF(center_x-current_radius,center_y-current_radius),QPointF(center_x+current_radius,center_y+current_radius));
	selection_path.moveTo(center_x + current_radius*cos(theta1+gap_angle), center_y - current_radius*sin(theta1+gap_angle));
	selection_path.arcTo(rect,(theta1+gap_angle)*(180.0/mmcPI),(theta2-theta1-2.0*gap_angle)*(180.0/mmcPI));
	current_radius = (radius-0.5*selection_diameter_/scale);
	rect = QRectF(QPointF(center_x-current_radius,center_y-current_radius),QPointF(center_x+current_radius,center_y+current_radius));
	selection_path.lineTo(center_x + current_radius*cos(theta2-gap_angle), center_y - current_radius*sin(theta2-gap_angle));
	selection_path.arcTo(rect,(theta2-gap_angle)*(180.0/mmcPI),(theta1-theta2+2.0*gap_angle)*(180.0/mmcPI));
	selection_path.closeSubpath();

	return arc_path;
}


QPolygonF QtPrimitiveBase::GetArrowPolygonAndSelectionPath(double x1, double y1, double x2, double y2, double arrow_head_length, double arrow_head_width, QPainterPath &selection_path, double scale) const
{
	mmcMatrix start_point(2,1);
	start_point(0,0) = x1;
	start_point(1,0) = y1;

	mmcMatrix end_point(2,1);
	end_point(0,0) = x2;
	end_point(1,0) = y2;
	
    if(end_point.GetDistanceTo(start_point) == 0)
    {
        // degenerate case where the arrow has zero length
        selection_path.addEllipse(QPointF(x1,y1),1.0*selection_diameter_/scale,1.0*selection_diameter_/scale);
        
        QPolygonF polygon;
        return polygon;
    }

	mmcMatrix line_vector = (end_point - start_point).GetNormalized();
	
	mmcMatrix normal_vector(2,1);
	normal_vector(0,0) = line_vector(1,0);
	normal_vector(1,0) = -line_vector(0,0);

	mmcMatrix arrow_base = end_point - arrow_head_length*line_vector;
	mmcMatrix corner_1 = arrow_base + 0.5*arrow_head_width*normal_vector;
	mmcMatrix corner_2 = arrow_base - 0.5*arrow_head_width*normal_vector;

	QPolygonF polygon;

	mmcMatrix start_arrow_base = start_point + arrow_head_length*line_vector;
	mmcMatrix start_corner_1 = start_arrow_base + 0.5*arrow_head_width*normal_vector;
	mmcMatrix start_corner_2 = start_arrow_base - 0.5*arrow_head_width*normal_vector;

	polygon << QPointF(start_arrow_base(0,0),start_arrow_base(1,0)) 
			<< QPointF(start_corner_1(0,0),start_corner_1(1,0))
			<< QPointF(start_point(0,0),start_point(1,0))
			<< QPointF(start_corner_2(0,0),start_corner_2(1,0))
			<< QPointF(start_arrow_base(0,0),start_arrow_base(1,0))
			<< QPointF(arrow_base(0,0),arrow_base(1,0)) 
			<< QPointF(corner_1(0,0),corner_1(1,0))
			<< QPointF(end_point(0,0),end_point(1,0))
			<< QPointF(corner_2(0,0),corner_2(1,0))
			<< QPointF(arrow_base(0,0),arrow_base(1,0))
			<< QPointF(start_arrow_base(0,0),start_arrow_base(1,0));

	QPolygonF selection_polygon;
	mmcMatrix selection_corner_1 = arrow_base + 0.5*(selection_diameter_/scale)*normal_vector;
	mmcMatrix selection_corner_2 = arrow_base - 0.5*(selection_diameter_/scale)*normal_vector;
	mmcMatrix selection_start_corner_1 = start_arrow_base + 0.5*(selection_diameter_/scale)*normal_vector;
	mmcMatrix selection_start_corner_2 = start_arrow_base - 0.5*(selection_diameter_/scale)*normal_vector;

	selection_path.moveTo(QPointF(selection_start_corner_1(0,0),selection_start_corner_1(1,0)));
	selection_path.lineTo(QPointF(start_corner_1(0,0),start_corner_1(1,0)));
	selection_path.lineTo(QPointF(start_point(0,0),start_point(1,0)));
	selection_path.lineTo(QPointF(start_corner_2(0,0),start_corner_2(1,0)));
	selection_path.lineTo(QPointF(selection_start_corner_2(0,0),selection_start_corner_2(1,0)));
	selection_path.lineTo(QPointF(selection_corner_2(0,0),selection_corner_2(1,0)));
	selection_path.lineTo(QPointF(corner_2(0,0),corner_2(1,0)));
	selection_path.lineTo(QPointF(end_point(0,0),end_point(1,0)));
	selection_path.lineTo(QPointF(corner_1(0,0),corner_1(1,0)));
	selection_path.lineTo(QPointF(selection_corner_1(0,0),selection_corner_1(1,0)));
	selection_path.closeSubpath();

	return polygon;
}


QPainterPath QtPrimitiveBase::GetArcArrowPathAndSelectionPath(double x_center, double y_center, double radius, double theta1, double theta2, double arrow_head_length, double arrow_head_width, QPainterPath &selection_path, double scale) const
{

	// theta1 and theta2 are in radians
	QPainterPath path;

	// define everyting that will be needed to create the arc arrow
	QRectF rect(QPointF(x_center-radius,y_center-radius),
 				QPointF(x_center+radius,y_center+radius));
	
	double arrow_base_theta1;
	double arrow_base_theta2;

	if(theta2 >= theta1)
	{
		arrow_base_theta1 = theta1 + arrow_head_length/radius;
		arrow_base_theta2 = theta2 - arrow_head_length/radius;
	} else {
		arrow_base_theta1 = theta1 - arrow_head_length/radius;
		arrow_base_theta2 = theta2 + arrow_head_length/radius;
	}

	mmcMatrix normal_1(2,1);
	mmcMatrix normal_2(2,1);

	mmcMatrix base_point_1;
	mmcMatrix base_point_2;
	mmcMatrix tip_point_1(2,1);
	mmcMatrix tip_point_2(2,1);

	mmcMatrix center(2,1);
	center(0,0) = x_center;
	center(1,0) = y_center;

	tip_point_1(0,0) = radius*cos(theta1);
	tip_point_1(1,0) = -radius*sin(theta1);
	tip_point_1 = tip_point_1 + center;

	normal_1(0,0) = cos(arrow_base_theta1);
	normal_1(1,0) = -sin(arrow_base_theta1);

	base_point_1 = radius * normal_1 + center;

	tip_point_2(0,0) = radius*cos(theta2);
	tip_point_2(1,0) = -radius*sin(theta2);
	tip_point_2 = tip_point_2 + center;

	normal_2(0,0) = cos(arrow_base_theta2);
	normal_2(1,0) = -sin(arrow_base_theta2);

	base_point_2 = radius * normal_2 + center;

	mmcMatrix arrow_1_corner_1 = base_point_1 + 0.5*arrow_head_width*normal_1;
	mmcMatrix arrow_1_corner_2 = base_point_1 - 0.5*arrow_head_width*normal_1;

	mmcMatrix arrow_2_corner_1 = base_point_2 + 0.5*arrow_head_width*normal_2;
	mmcMatrix arrow_2_corner_2 = base_point_2 - 0.5*arrow_head_width*normal_2;

	// create the arc arrow
	path.moveTo(base_point_1(0,0),base_point_1(1,0));
	path.lineTo(arrow_1_corner_1(0,0), arrow_1_corner_1(1,0));
	path.lineTo(tip_point_1(0,0), tip_point_1(1,0));
	path.lineTo(arrow_1_corner_2(0,0), arrow_1_corner_2(1,0));
	path.arcTo(rect,arrow_base_theta1*(180.0/mmcPI),(arrow_base_theta2-arrow_base_theta1)*(180.0/mmcPI));
	path.lineTo(arrow_2_corner_1(0,0), arrow_2_corner_1(1,0));
	path.lineTo(tip_point_2(0,0), tip_point_2(1,0));
	path.lineTo(arrow_2_corner_2(0,0), arrow_2_corner_2(1,0));
	path.arcTo(rect,arrow_base_theta2*(180.0/mmcPI),(arrow_base_theta1-arrow_base_theta2)*(180.0/mmcPI));


	// create the arc arrow selection path
	mmcMatrix arrow_1_corner_1b = base_point_1 + 0.5*(selection_diameter_/scale)*normal_1;
	mmcMatrix arrow_1_corner_2b = base_point_1 - 0.5*(selection_diameter_/scale)*normal_1;

	mmcMatrix arrow_2_corner_1b = base_point_2 + 0.5*(selection_diameter_/scale)*normal_2;
	mmcMatrix arrow_2_corner_2b = base_point_2 - 0.5*(selection_diameter_/scale)*normal_2;

	double radius1 = radius+0.5*(selection_diameter_/scale);
	double radius2 = radius-0.5*(selection_diameter_/scale);

	QRectF rect1(QPointF(x_center-radius1,y_center-radius1),
 				QPointF(x_center+radius1,y_center+radius1));

	QRectF rect2(QPointF(x_center-radius2,y_center-radius2),
 				QPointF(x_center+radius2,y_center+radius2));

	selection_path.moveTo(arrow_1_corner_1b(0,0),arrow_1_corner_1b(1,0));
	selection_path.lineTo(arrow_1_corner_1(0,0), arrow_1_corner_1(1,0));
	selection_path.lineTo(tip_point_1(0,0), tip_point_1(1,0));
	selection_path.lineTo(arrow_1_corner_2(0,0), arrow_1_corner_2(1,0));
	selection_path.arcTo(rect1,arrow_base_theta1*(180.0/mmcPI),(arrow_base_theta2-arrow_base_theta1)*(180.0/mmcPI));
	selection_path.lineTo(arrow_2_corner_2(0,0), arrow_2_corner_2(1,0));
	selection_path.lineTo(tip_point_2(0,0), tip_point_2(1,0));
	selection_path.lineTo(arrow_2_corner_1(0,0), arrow_2_corner_1(1,0));
	selection_path.arcTo(rect2,arrow_base_theta2*(180.0/mmcPI),(arrow_base_theta1-arrow_base_theta2)*(180.0/mmcPI));

	return path;
}

