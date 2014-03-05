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


#ifndef QtPrimitiveBaseH
#define QtPrimitiveBaseH

#include "DisplayProperties.h"

#include <QGraphicsItem>
#include <QRectF>
#include <QLineF>
#include "../ConstraintSolver/Primitives.h"

// QtPrimitiveBase class
class QtPrimitiveBase : public QGraphicsItem, virtual public PrimitiveBase
{
	public:
		QtPrimitiveBase(QGraphicsItem * parent = 0);

		virtual void Display();
		virtual void Erase();

		DisplayProperties GetProperties()           {return properties_;}
		DisplayProperties GetSelectedProperties()   {return selected_properties_;}
		DisplayProperties GetMouseHoverProperties() {return mouse_hover_properties_;}
	
		void SetProperties(DefaultPropertiesSet prop_set) {properties_ = DisplayProperties(prop_set);}
		void SetSelectedProperties(DefaultPropertiesSet prop_set) {selected_properties_ = DisplayProperties(prop_set);}
		void SetMouseHoverProperties(DefaultPropertiesSet prop_set) {mouse_hover_properties_ = DisplayProperties(prop_set);}

		double GetBoundingRectPad() const {return bounding_rect_pad_;}

		// update coordinates of primitive
		virtual void UpdateDisplay();

		virtual bool IsSelected();
		virtual void SetSelectable(bool selectable_);

		// some utility methods used to paint primitives
		void PaintPoint(QPainter *painter, const QStyleOptionGraphicsItem *option, double x, double y);
		void PaintPointAndSelectionPath(QPainter *painter, const QStyleOptionGraphicsItem *option, double x, double y,QPainterPath &selection_path);

		QPolygonF GetArrowPolygon(double x1, double y1, double x2, double y2, double arrow_head_length, double arrow_head_width, bool double_arrow = false) const;
		QPainterPath GetArcArrowPath(double x_center, double y_center, double radius, double theta1, double theta2, double arrow_head_length, double arrow_head_width) const;

		QLineF GetLineAndSelectionPath(double x1, double y1, double x2, double y2, QPainterPath &selection_path, double scale) const;
		QLineF GetLineAndSelectionPath(mmcMatrix point1, mmcMatrix point2, QPainterPath &selection_path, double scale) const;
		
		QPainterPath GetArcAndSelectionPath(double center_x, double center_y, double radius, double theta1, double theta2, QPainterPath &selection_path, double scale) const;

		QPolygonF GetArrowPolygonAndSelectionPath(double x1, double y1, double x2, double y2, double arrow_head_length, double arrow_head_width, QPainterPath &selection_path, double scale) const;
		QPainterPath GetArcArrowPathAndSelectionPath(double x_center, double y_center, double radius, double theta1, double theta2, double arrow_head_length, double arrow_head_width, QPainterPath &selection_path, double scale) const;

	protected:
		// member class that stores display properties for the primitives
		DisplayProperties properties_;
		DisplayProperties selected_properties_;
		DisplayProperties mouse_hover_properties_;

		double selection_diameter_;

		double bounding_rect_pad_;
};


#endif //QtPrimitiveBaseH
