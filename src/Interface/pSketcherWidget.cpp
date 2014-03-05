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

#include <QMouseEvent>

#include <cmath>
#include <QtGui>

#include "pSketcherWidget.h"
#include "../QtBinding/QtPrimitives.h"
#include "../InteractiveConstructors/QtEventProperties.h"
#include "Point2DEditDialog.h"

using namespace std;

pSketcherWidget::pSketcherWidget(QGraphicsScene * scene, QWidget * parent) :
QGraphicsView(scene, parent)
{
	//setOptimizationFlag( QGraphicsView::IndirectPainting );
	
	interactive_primitive_ = 0;

	GenerateDefaultSketch();

	setInteractive(true);  // allow selections and other mouse interactions
	setDragMode(QGraphicsView::RubberBandDrag);	
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // update whole scene at a time
	setRenderHints(QPainter::Antialiasing); // enable antialiasing
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse); 

	// turn off scroll bars (the mouse will be used to pan the scene)
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);	

	//setMouseTracking(true);

/*
	// create the python interpretor instance
	PyImport_AppendInittab( "psketcher_module", &initpsketcher_module );
	Py_Initialize();
	//init_psketcher_module();
	PyRun_SimpleString("import psketcher_module \n");
	PyRun_SimpleString("import sys \n");
	PyRun_SimpleString("sys.path.append('./src/PythonBinding/') \n");
	PyRun_SimpleString("import psketcher_module \n");
	
	PyRun_SimpleString("point1 =  psketcher_module.CreatePoint(0.0,1.0,2.0) \n");
	PyRun_SimpleString("print point1.GetYDOF().GetValue() \n");
*/
}

pSketcherWidget::~pSketcherWidget()
{
	if(interactive_primitive_ != 0)
		delete interactive_primitive_;

	if(current_sketch_ != 0)
		delete current_sketch_;	
}

void pSketcherWidget::select()
{
	if(current_sketch_ != 0)
		current_sketch_->ApplySelectionMask(All);

	// delete any interactive primitives that were in the process of being constructed
	delete interactive_primitive_;
	interactive_primitive_ = 0;
}

void pSketcherWidget::paintEvent        ( QPaintEvent* e )
{
	QGraphicsView::paintEvent(e);
}

void pSketcherWidget::resizeEvent       ( QResizeEvent* e )
{
	QGraphicsView::resizeEvent(e);
}

void pSketcherWidget::mousePressEvent   ( QMouseEvent* e )
{
	if(interactive_primitive_ != 0 && !(e->button() & Qt::MidButton))
	{
		MouseEventPropertiesPointer event_props(new QtMouseEventProperties((MouseButtonEventType)ButtonPress,e,this));
		bool finished = false;

		switch (e->button() && !(e->button() & Qt::MidButton)) {
			case Qt::LeftButton:
				QGraphicsView::mousePressEvent(e);
				finished = interactive_primitive_->LeftButtonDown(event_props);
				break;
			case Qt::MidButton:   finished = interactive_primitive_->MiddleButtonDown(event_props); break;
			case Qt::RightButton: finished = interactive_primitive_->RightButtonDown(event_props); break;
			default:              finished = false; 
		}

		if(finished)
		{
			// The interactive primitive is finished, so allow it to create its object and then clean it up
			interactive_primitive_->CreateObject();
			delete interactive_primitive_;
			interactive_primitive_ = 0;
			// @fixme redraw();
			emit sketchActionFinished();
			modelChanged(tr("Create primitive"));
		}
	} else if (e->button() & Qt::MidButton) {
		// begin pan event and change cursor to a hand to convey this to the user
		QGraphicsView::mousePressEvent(e);
		setCursor(Qt::SizeAllCursor);
	} else {
		QGraphicsView::mousePressEvent(e);
	}

	previous_mouse_position_ = e->pos();
}

void pSketcherWidget::mouseReleaseEvent ( QMouseEvent* e )
{
	
	if(interactive_primitive_ != 0 && !((e->button() & Qt::MidButton)))
	{
		MouseEventPropertiesPointer event_props(new QtMouseEventProperties((MouseButtonEventType)ButtonRelease,e,this));
		bool finished = false;

		switch (e->button()) {
			case Qt::LeftButton:
				QGraphicsView::mouseReleaseEvent(e); // allow the user to select an object
				finished = interactive_primitive_->LeftButtonUp(event_props); 
				break;
			case Qt::MidButton:   finished = interactive_primitive_->MiddleButtonUp(event_props); break;
			case Qt::RightButton: finished = interactive_primitive_->RightButtonUp(event_props); break;
			default:              finished = false;
		}

		if(finished)
		{
			// The interactive primitive is finished, so allow it to create its object and then clean it up
			interactive_primitive_->CreateObject();
			delete interactive_primitive_;
			interactive_primitive_ = 0;
			// @fixme redraw();
			emit sketchActionFinished();
			modelChanged(tr("Create primitive"));
		}
	} else if (e->button() & Qt::MidButton) {
		// pan event has ended to reset cursor to its default
		unsetCursor();
	} else {
		QGraphicsView::mouseReleaseEvent(e);
		modelChanged(tr("Modify primitive"));
	}

}

void pSketcherWidget::mouseMoveEvent    ( QMouseEvent* e )
{	
	if(interactive_primitive_ != 0 && !(e->buttons() & Qt::MidButton))
	{
		if(e->buttons() & Qt::LeftButton)
			QGraphicsView::mouseMoveEvent(e); // allows selection hinting to occur

		QGraphicsView::mouseMoveEvent(e);
		MotionEventPropertiesPointer event_props(new QtMotionEventProperties(e,this));
		if(interactive_primitive_->MouseMove(event_props))
		{
			// The interactive primitive is finished, so allow it to create its object and then clean it up
			interactive_primitive_->CreateObject();
			delete interactive_primitive_;
			interactive_primitive_ = 0;
			// @fixme redraw();
			emit sketchActionFinished();
			modelChanged(tr("Create primitive"));
		}
	} else if(e->buttons() & Qt::MidButton) {
		// translate the scene rectangle when the middle mouse button is pressed and moved
		//QPointF delta = mapToScene(e->pos()) - mapToScene(previous_mouse_position_);
		
		QPointF delta = (e->pos()) - (previous_mouse_position_);
		
		//std::cout << "(x,y) = (" << delta.x() << ", " << delta.y() << ")"<< std::endl;
		
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

	} else {
		QGraphicsView::mouseMoveEvent(e);
	}
	previous_mouse_position_ = e->pos();
}

// Wheel event causes the scene to zoon in or out
void pSketcherWidget::wheelEvent        ( QWheelEvent* e )
{
	double numDegrees = -e->delta()/8.0;
	double numSteps = numDegrees / 15.0;
	double factor = std::pow(1.125, numSteps);
	scale(factor, factor);
}

void pSketcherWidget::leaveEvent		   ( QEvent * e)
{
	QGraphicsView::leaveEvent(e);
}

void pSketcherWidget::GenerateDefaultSketch()
{
	// create the current pSketcher sketch
	VectorPointer normal( new Vector(0.0,0.0,1.0));
	VectorPointer up( new Vector(0.0,1.0,0.0));
	PointPointer base( new Point(0.0,0.0,0.0));
	current_sketch_ = QtSketchPointer(new QtSketch(scene(),normal, up, base));

	modelChanged(tr("Initialize Sketch"));
}

void pSketcherWidget::GenerateTestSketch()
{
	string description;
	bool temp_bool;

	Point2DPointer point1 = current_sketch_->AddPoint2D(0.0,0.0,false,false);  // none of the dof's can vary
	Point2DPointer point2 = current_sketch_->AddPoint2D(400.0,0.0,true,false);  // only x dof can vary
	Point2DPointer point3 = current_sketch_->AddPoint2D(400.0,400.0,true,true);  // x and y dof's can vary

	Arc2DPointer arc1 = current_sketch_->AddArc2D(60.0,240.0,(mmcPI/2.0)*.8,(mmcPI)*1.2,80.0,true,true,true,true,false);

	Line2DPointer line1 = current_sketch_->AddLine2D(point1,point2);
	Line2DPointer line2 = current_sketch_->AddLine2D(point2,point3);
	Line2DPointer line3 = current_sketch_->AddLine2D(point3,arc1->GetPoint1());
	Line2DPointer line4 = current_sketch_->AddLine2D(arc1->GetPoint2(),point1);

    Circle2DPointer circle1 = current_sketch_->AddCircle2D(arc1->GetCenterPoint()->GetSDOF(),arc1->GetCenterPoint()->GetTDOF(),40.0,false);

	// These 5 constraints will fully constrain the four free DOF's defined about
	ConstraintEquationBasePointer constraint1 = current_sketch_->AddDistancePoint2D(point1,point2,240.0);
	ConstraintEquationBasePointer constraint2 = current_sketch_->AddDistancePoint2D(point2,point3,480.0);
	ConstraintEquationBasePointer constraint3 = current_sketch_->AddParallelLine2D(line1,line3);
	ConstraintEquationBasePointer constraint4 = current_sketch_->AddParallelLine2D(line2,line4);
	ConstraintEquationBasePointer constraint5 = current_sketch_->AddAngleLine2D(line1,line2,mmcPI/2.0,false);
	//ConstraintEquationBasePointer constraint8 = current_sketch_->AddAngleLine2D(line2,line3,mmcPI/2.0,false);

	ConstraintEquationBasePointer constraint6 = current_sketch_->AddTangentEdge2D(line3,Point2,arc1,Point1);
	ConstraintEquationBasePointer constraint7 = current_sketch_->AddTangentEdge2D(line4,Point1,arc1,Point2);

	// create an edge loop
	EdgeLoop2DPointer edge_loop1(new EdgeLoop2D());
	edge_loop1->AddEdge(line1);
	edge_loop1->AddEdge(line2);
	edge_loop1->AddEdge(line3);
	edge_loop1->AddEdge(arc1);
	edge_loop1->AddEdge(line4);
	//std::cout << "Is loop valid: " << edge_loop1->IsLoopValid() << std::endl;

	//current_sketch_->ApplySelectionMask(Points);
	//current_sketch_->ApplySelectionMask(Edges);
	
	/*
	DOFPointer test_dof_1 = current_sketch_->DOFFactory(15);
	std::cout << "Name for DependentDOF 15 (from database)= " << test_dof_1->GetVariable().get_name() << ", value = " << test_dof_1->GetValue() << ", free = " << test_dof_1->IsFree() << ", dependent " << test_dof_1->IsDependent() << std::endl;

	DOFPointer test_dof_4 = current_sketch_->FetchDOF(15);
	std::cout << "Name for DependentDOF 15 (from memory)= " << test_dof_4->GetVariable().get_name() << ", value = " << test_dof_4->GetValue() << ", free = " << test_dof_4->IsFree() << ", dependent " << test_dof_4->IsDependent() << std::endl;

	DependentDOFPointer test_dof_2 = boost::dynamic_pointer_cast<DependentDOF>(current_sketch_->DOFFactory(22));
	std::cout << "Expression for DependentDOF 22 (from database)= " << test_dof_2->GetExpression() << ", value = " << test_dof_2->GetValue() << ", free = " << test_dof_2->IsFree() << ", dependent = " << test_dof_2->IsDependent() << ", name = " << test_dof_2->GetVariable().get_name() << std::endl;

	DependentDOFPointer test_dof_3 = boost::dynamic_pointer_cast<DependentDOF>(current_sketch_->FetchDOF(22));
	std::cout << "Expression for DependentDOF 22 (from memory)= " << test_dof_3->GetExpression() << ", value = " << test_dof_3->GetValue() << ", free = " << test_dof_3->IsFree() << ", dependent = " << test_dof_3->IsDependent() << ", name = " << test_dof_3->GetVariable().get_name() << std::endl;

	Point2DPointer test_point = current_sketch_->FetchPrimitive<Point2D>(10);
	test_point->SyncListsToDatabase("dof_table_10","primitive_table_10",*current_sketch_);	

	DistancePoint2DPointer test_constraint = current_sketch_->FetchConstraint<DistancePoint2D>(19);
	//test_constraint->SyncListsToDatabase("dof_table_19","primitive_table_19",*current_sketch_);
	//test_constraint->SyncConstraintListToDatabase("constraint_table_19",*current_sketch_);
	*/

	/*
	point1->SyncToDatabase(*current_sketch_);
	point2->SyncToDatabase(*current_sketch_);
	point3->SyncToDatabase(*current_sketch_);
	arc1->SyncToDatabase(*current_sketch_);
	line1->SyncToDatabase(*current_sketch_);	
	line2->SyncToDatabase(*current_sketch_);
	line3->SyncToDatabase(*current_sketch_);
	line4->SyncToDatabase(*current_sketch_);
	constraint1->SyncToDatabase(*current_sketch_); // distance
	constraint2->SyncToDatabase(*current_sketch_); // distance
	constraint3->SyncToDatabase(*current_sketch_); // parallel
	constraint4->SyncToDatabase(*current_sketch_); // parallel
	constraint5->SyncToDatabase(*current_sketch_); // angle
	constraint6->SyncToDatabase(*current_sketch_); // tangent
	constraint7->SyncToDatabase(*current_sketch_); // tangent
	*/

	//current_sketch_->SyncToDatabase();
    
    /*
    Point2DPointer point4 = current_sketch_->AddPoint2D(0.0,-10.0,true,false);  // only x dof can vary
    Point2DPointer point5 = current_sketch_->AddPoint2D(10.0,-10.0,true,true);  // x and y dof's can vary
    Point2DPointer point6 = current_sketch_->AddPoint2D(12.0,-10.0,true,true);  // x and y dof's can vary
    Point2DPointer point7 = current_sketch_->AddPoint2D(10.0,-2.0,true,true);  // x and y dof's can vary
    Line2DPointer line5 = current_sketch_->AddLine2D(point4,point5);
    Line2DPointer line6 = current_sketch_->AddLine2D(point6,point7);
    */

    modelChanged("Create test sketch");

    /*
    current_sketch_->ReplaceDOF(point5->GetSDOF(),point6->GetSDOF());
    current_sketch_->ReplaceDOF(point5->GetTDOF(),point6->GetTDOF());

    modelChanged("Merge points");
    */

	fitExtents();
}

void pSketcherWidget::SolveConstraints() 
{
	if(current_sketch_ != 0)
	{
		current_sketch_->SolveConstraints();
		current_sketch_->UpdateDisplay();

		modelChanged(tr("Solve constraints"));
	}
}

void pSketcherWidget::ExecutePythonScript()
{
/*
    FILE *fp = fopen ("./src/PythonScripts/test_sketch.py", "r+");
    PyRun_SimpleFile (fp, "./src/PythonScripts/test_sketch.py");
		fclose(fp);
*/
}

void pSketcherWidget::MakeLine() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new Line2DConstructor(current_sketch_);
}

void pSketcherWidget::MakePolyLine() {;}

void pSketcherWidget::MakeArc()
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new Arc2DConstructor(current_sketch_);
}

void pSketcherWidget::MakeCircle()
{
    if(interactive_primitive_ != 0) delete interactive_primitive_;
    interactive_primitive_ = new Circle2DConstructor(current_sketch_);
}

void pSketcherWidget::MakePoint() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new Point2DConstructor(current_sketch_);
}

void pSketcherWidget::MakeVerticalConstraint() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new HoriVertLine2DConstructor(current_sketch_,true);
}

void pSketcherWidget::MakeHorizontalConstraint() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new HoriVertLine2DConstructor(current_sketch_,false);
}

void pSketcherWidget::MakeDistanceConstraint() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new Distance2DConstructor(current_sketch_);
}

void pSketcherWidget::MakeAngleConstraint() 
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new AngleLine2DConstructor(current_sketch_);
}

void pSketcherWidget::MakeTangentConstraint()
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new TangentEdge2DConstructor(current_sketch_);
}


void pSketcherWidget::MakeParallelConstraint()
{
	if(interactive_primitive_ != 0) delete interactive_primitive_;
	interactive_primitive_ = new ParallelLine2DConstructor(current_sketch_);
}

// double clicking causes the currently selected item to span its edit dialog
void pSketcherWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if(interactive_primitive_ == 0 && !(event->modifiers() & Qt::ShiftModifier))
	{
		// edit the selected item (only edit if not in multi-select mode

		// first, make sure that a primitive is selected
		std::vector<PrimitiveBasePointer> primitive_list = current_sketch_->GetSelectedPrimitives();
		if(primitive_list.size() >= 1 )
		{
			if(dynamic_cast<QtPoint2D*>(primitive_list[0].get()) != 0)
			{
				QtPoint2DPointer selected_point = boost::dynamic_pointer_cast<QtPoint2D>(primitive_list[0]);
			
				// create the point edit dialog
				Point2DEditDialog *point_edit_dialog = new Point2DEditDialog(selected_point, this);
				connect(point_edit_dialog, SIGNAL(modelChanged(QString)), this, SLOT(modelChanged(QString)));
				point_edit_dialog->show();
			} 
		}

	} else {
		QGraphicsView::mouseDoubleClickEvent(event);
	}
}

// This slot is called whenever the model changes
// This slot will handle updating the display and the undo history
void pSketcherWidget::modelChanged(QString description)
{
	current_sketch_->MarkStablePoint(description.toStdString());
	current_sketch_->UpdateDisplay();
}

void pSketcherWidget::undo()
{
	current_sketch_->Undo();
	current_sketch_->UpdateDisplay();
}

void pSketcherWidget::redo()
{
	current_sketch_->Redo();
	current_sketch_->UpdateDisplay();
}

void pSketcherWidget::fitExtents()
{
	QRectF rect = scene()->itemsBoundingRect();
	//std::cout << "bounding rect " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << std::endl;
	fitInView(rect,Qt::KeepAspectRatio);
}

void pSketcherWidget::drawBackground ( QPainter * painter, const QRectF & rect )
{
	// resize the scene rectangle to twice the visible rectangle which is passed to this function
	QRectF scene_rect;
	
	QPointF center_point = rect.center(); // current centerpoint of rectangle
	QSizeF size = rect.size(); // current size of rectangle
	size.setWidth(size.width()*2.0);
	size.setHeight(size.height()*2.0);
	scene_rect.setSize(size);  // scale rectangle, top left corner will not move
	scene_rect.moveCenter(center_point); // move center of rectangle back to the original center
	
	// finally, set the scene rectangle
	setSceneRect(scene_rect);

	// let the base class do its thing
	QGraphicsView::drawBackground(painter, rect);
}

void pSketcherWidget::SelectUnderMouse(QMouseEvent *e, bool multi_select)
{


}

void pSketcherWidget::keyReleaseEvent ( QKeyEvent * event )
{
	if(event->key() == Qt::Key_Delete)
	{
		current_sketch_->DeleteSelected();
		modelChanged(tr("Delete primitive(s)"));

	} else if(event->key() == Qt::Key_Escape) {
		// cancel the current primitive
		if(interactive_primitive_ != 0)
		{
			delete interactive_primitive_;
			interactive_primitive_ = 0;
		}

		emit sketchActionFinished();
	} else {
		// not handling the event, call the base class implementation
		QGraphicsView::keyReleaseEvent(event);
	}
}

void pSketcherWidget::open()
{
    // delete any interactive constructors in use
    if(interactive_primitive_ != 0)
    {
        delete interactive_primitive_;
        interactive_primitive_ = 0;
    }

	// @fixme Need to add a check to make sure that there are no unsaved changes before loading another file
	QString file_name = QFileDialog::getOpenFileName(this,tr("Open Sketch"),".",tr("pSketcher sketch files (*.pSketch)"));
	
	if(!file_name.isEmpty())
	{
		delete current_sketch_;
		current_sketch_ = new QtSketch(scene(),file_name.toStdString());
	}
}

void pSketcherWidget::newFile()
{
	// @fixme Need to add a check to make sure that there are no unsaved changes before loading a new file

    // delete any interactive constructors in use
    if(interactive_primitive_ != 0)
    {
        delete interactive_primitive_;
        interactive_primitive_ = 0;
    }

	delete current_sketch_;

	// create a new pSketcher sketch
	VectorPointer normal( new Vector(0.0,0.0,1.0));
	VectorPointer up( new Vector(0.0,1.0,0.0));
	PointPointer base( new Point(0.0,0.0,0.0));
	current_sketch_ = QtSketchPointer(new QtSketch(scene(),normal, up, base));

	modelChanged(tr("Intialize sketch"));
}

bool pSketcherWidget::save()
{
    // delete any interactive constructors in use
    if(interactive_primitive_ != 0)
    {
        delete interactive_primitive_;
        interactive_primitive_ = 0;
    }

	if(current_sketch_->GetFileName() == "")
	{	
		return saveAs();
	} else {
		return current_sketch_->Save();
	}
}

bool pSketcherWidget::saveAs()
{
	QString file_name = QFileDialog::getSaveFileName(this,tr("Save Sketch"), ".", tr("pSketcher sketch files (*.pSketch)"));

	if (file_name.isEmpty())
		return false;

	return current_sketch_->Save(file_name.toStdString());
}

bool pSketcherWidget::exportDXF()
{
	QString file_name = QFileDialog::getSaveFileName(this,tr("Export DXF File"), ".", tr("dxf files (*.dxf)"));

	if (file_name.isEmpty())
		return false;

	return current_sketch_->ExportDXF(file_name.toStdString());
}
