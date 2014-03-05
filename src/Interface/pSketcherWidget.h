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

#ifndef PSKETCHERWIDGET_H
#define PSKETCHERWIDGET_H

/*
extern "C" {
#include <Python.h>
}
*/

#include <QGraphicsView>

#include "../QtBinding/QtSketch.h"
#include "../InteractiveConstructors/InteractiveConstructorBase.h"
#include "../InteractiveConstructors/Point2DConstructor.h"
#include "../InteractiveConstructors/Line2DConstructor.h"
#include "../InteractiveConstructors/Distance2DConstructor.h"
#include "../InteractiveConstructors/AngleLine2DConstructor.h"
#include "../InteractiveConstructors/ParallelLine2DConstructor.h"
#include "../InteractiveConstructors/TangentEdge2DConstructor.h"
#include "../InteractiveConstructors/Arc2DConstructor.h"
#include "../InteractiveConstructors/Circle2DConstructor.h"
#include "../InteractiveConstructors/HoriVertLine2DConstructor.h"

class QGraphicsScene;

class pSketcherWidget : public QGraphicsView
{
    Q_OBJECT

    public:
        pSketcherWidget(QGraphicsScene * scene, QWidget * parent = 0);
        ~pSketcherWidget();
    
        void GenerateDefaultSketch();

    
    signals:
        void sketchActionFinished();
    
    public slots:
        void GenerateTestSketch();
        void ExecutePythonScript();
        void SolveConstraints();

        void select(); // overides the select solot for QoccViewWidget

        // Interactive constructor slots
        void MakeLine();
        void MakeHorizontalConstraint();
        void MakeVerticalConstraint();
        void MakePolyLine();
        void MakeArc();
        void MakeCircle();
        void MakePoint();
        void MakeDistanceConstraint();
        void MakeAngleConstraint();
        void MakeTangentConstraint();
        void MakeParallelConstraint();

        void fitExtents();

        // slots for file handling
        void open();
        void newFile();
        bool save();
        bool saveAs();
        bool exportDXF();

        // slots for undo/redo
        void modelChanged(QString description);
        void undo();    
        void redo();
    
    protected: // methods
        virtual void paintEvent        ( QPaintEvent* e );
        virtual void resizeEvent       ( QResizeEvent* e );
        virtual void mousePressEvent   ( QMouseEvent* e );
        virtual void mouseReleaseEvent ( QMouseEvent* e );
        virtual void mouseMoveEvent    ( QMouseEvent* e );
        virtual void wheelEvent        ( QWheelEvent* e );
        virtual void leaveEvent        ( QEvent * e);
        virtual void mouseDoubleClickEvent ( QMouseEvent * event );
        virtual void drawBackground ( QPainter * painter, const QRectF & rect );
        virtual void keyReleaseEvent ( QKeyEvent * event );

        void SelectUnderMouse(QMouseEvent *e, bool multi_select = false);
    
    private: // members
        QtSketchPointer current_sketch_;
        InteractiveConstructorBase *interactive_primitive_;
    
    
    private: // methods
        QPoint previous_mouse_position_;
    
    
    public:

};

#endif // PSKETCHERWIDGET_H
