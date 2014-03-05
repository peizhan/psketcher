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


#ifndef QtDistancePointLine2DH
#define QtDistancePointLine2DH

#include <boost/enable_shared_from_this.hpp>

#include <QGraphicsProxyWidget>
#include "QtPrimitiveBase.h"

class QLineEdit;
class QtDistancePointLine2DWidget;

class QtDistancePointLine2D : public QtPrimitiveBase, public DistancePointLine2D, public boost::enable_shared_from_this<QtDistancePointLine2D>
{
	public:
		QtDistancePointLine2D (QGraphicsItem * parent, unsigned id, pSketcherModel &psketcher_model);
		QtDistancePointLine2D (QGraphicsItem * parent, const Point2DPointer point, const Line2DPointer line, 
								double distance);

		void Display() {return QtPrimitiveBase::Display();}
		bool IsSelected() {return QtPrimitiveBase::IsSelected();}
		void SetSelectable(bool input_selectable) {PrimitiveBase::SetSelectable(input_selectable); QtPrimitiveBase::SetSelectable(input_selectable);}

		void UpdateDisplay();

		virtual QRectF boundingRect() const;
		virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
		virtual QPainterPath shape() const {return current_shape_;}

		virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

	private:
        bool pending_db_save_;
		QtDistancePointLine2DWidget *distance_widget_;
		QPainterPath current_shape_;

};
typedef boost::shared_ptr<QtDistancePointLine2D> QtDistancePointLine2DPointer;


// class defining the line edit graphics item used by QtDistancePointLine2D
class QtDistancePointLine2DWidget : public QGraphicsProxyWidget
{
	Q_OBJECT

	public:
		QtDistancePointLine2DWidget(QtDistancePointLine2DPointer distance_constraint, QGraphicsItem *parent = 0);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget);
		virtual bool event(QEvent *event);

		void UpdateGeometry(double scale);


	signals:
		void modelChanged();  // emitted when changes are applied

	private slots:
		void textChanged();
		void applyChanges();

	private:
		// methods

		// Parameters
		QtDistancePointLine2DPointer distance_constraint_; 

		QLineEdit *distance_line_edit_;
};


#endif //QtDistancePointLine2DH
