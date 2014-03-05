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


#ifndef QtAngleLine2DH
#define QtAngleLine2DH

#include <boost/enable_shared_from_this.hpp>

#include <QGraphicsProxyWidget>
#include "QtPrimitiveBase.h"

class QLineEdit;
class QtAngleLine2DWidget;

class QtAngleLine2D : public QtPrimitiveBase, public AngleLine2D, public boost::enable_shared_from_this<QtAngleLine2D>
{
	public:
		QtAngleLine2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model);
		QtAngleLine2D (QGraphicsItem * parent, const Line2DPointer line1, const Line2DPointer line2, double angle, bool interior_angle);

		void Display() {return QtPrimitiveBase::Display();}
		bool IsSelected() {return QtPrimitiveBase::IsSelected();}
		void SetSelectable(bool input_selectable) {PrimitiveBase::SetSelectable(input_selectable); QtPrimitiveBase::SetSelectable(input_selectable);}

		void UpdateDisplay();

		// Implement some of QGraphicsItem's methods
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

		virtual QPainterPath shape() const {return current_shape_;}

		virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

	private:
        bool pending_db_save_;
		QtAngleLine2DWidget *angle_widget_;
		
		QPainterPath current_shape_;
};
typedef boost::shared_ptr<QtAngleLine2D> QtAngleLine2DPointer;


// class defining the line edit graphics item used by QtDistancePoint2D
class QtAngleLine2DWidget : public QGraphicsProxyWidget
{
	Q_OBJECT

	public:
		QtAngleLine2DWidget(QtAngleLine2DPointer arc_primitive, QGraphicsItem *parent = 0);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget);
		virtual bool event(QEvent *event);

		void UpdateGeometry(double text_s, double text_t, double scale);


	signals:
		void modelChanged();  // emitted when changes are applied

	private slots:
		void textChanged();
		void applyChanges();

	private:
		// methods

		// Parameters
		QtAngleLine2DPointer angle_constraint_primitive_; 

		QLineEdit *angle_line_edit_;
};


#endif //QtAngleLine2DH
