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

#include <sstream>

#include "QtSketch.h"

using namespace std;

QtSketch::QtSketch(QGraphicsScene *scene, VectorPointer normal, VectorPointer up, PointPointer base, bool grid_snap):
Sketch(normal,up,base, QtSketch::PrimitiveFactory, QtSketch::ConstraintFactory),
grid_snap_(grid_snap),
scene_(scene)
{

}

QtSketch::QtSketch(QGraphicsScene *scene, const std::string &file_name, bool grid_snap):
Sketch(file_name,QtSketch::PrimitiveFactory, QtSketch::ConstraintFactory),
scene_(scene),
grid_snap_(grid_snap)
{
	// now that primitives and constraints have been defined from the file, loop through all of the primitives and constraints and display any that are derived from QGraphicsItem
	// loop over the primitives
	for(map<unsigned,PrimitiveBasePointer>::iterator primitive_it=primitive_list_.begin() ; primitive_it != primitive_list_.end(); primitive_it++ )
	{
		// Normally this type of cast is dangerous since it does the increment share_ptr's reference count
		// It is safe to pass this pointer to QGraphicsScene since QGraphicsItem's destructor removes itself from the QGraphicsScene it is a member of before deleting itself. Becuase of this, scene_ will not contain a nonexistant temp_graphics_item
		QGraphicsItem *temp_graphics_item = dynamic_cast<QGraphicsItem*>(primitive_it->second.get());
	
		if(temp_graphics_item != 0)
		{
			scene_->addItem(temp_graphics_item);
		}
	}

	// loop over the constraints
	for(map<unsigned,ConstraintEquationBasePointer>::iterator constraint_it=constraint_equation_list_.begin() ; constraint_it != constraint_equation_list_.end(); constraint_it++ )
	{
		// Normally this type of cast is dangerous since it does the increment share_ptr's reference count
		// It is safe to pass this pointer to QGraphicsScene since QGraphicsItem's destructor removes itself from the QGraphicsScene it is a member of before deleting itself. Becuase of this, scene_ will not contain a nonexistant temp_graphics_item
		QGraphicsItem *temp_graphics_item = dynamic_cast<QGraphicsItem*>(constraint_it->second.get());
	
		if(temp_graphics_item != 0)
		{
			scene_->addItem(temp_graphics_item);
		}
	}

}

QtPoint2DPointer QtSketch::AddPoint2D ( double s, double t, bool s_free, bool t_free)
{
	QtPoint2DPointer new_point(new QtPoint2D(0,s,t,GetSketchPlane(),s_free,t_free));
	AddPrimitive(new_point);
	return new_point;
}


QtArc2DPointer QtSketch::AddArc2D (double s_center, double t_center, double theta_1, double theta_2, double radius, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free)
{
	QtArc2DPointer new_arc(new QtArc2D(0,s_center, t_center, theta_1, theta_2, radius, GetSketchPlane(),s_center_free, t_center_free, theta_1_free, theta_2_free, radius_free));
	AddPrimitive(new_arc);

	// now add the end points and the center of the arc as seperate primitives so that they can be selected by the user for constructing lines and other primitives
	// @fixme these points need to be removed fro the sketch if new_arc is ever deleted from the scene otherwise the arc will still drive the points but will not be visible

	Point2DPointer point1 = new_arc->GetPoint1();
	Point2DPointer point2 = new_arc->GetPoint2();
	Point2DPointer center_point = new_arc->GetCenterPoint();

	QtPoint2DPointer qt_point1(new QtPoint2D(0,point1->GetSDOF(), point1->GetTDOF(), GetSketchPlane()));
	QtPoint2DPointer qt_point2(new QtPoint2D(0,point2->GetSDOF(), point2->GetTDOF(), GetSketchPlane()));
	QtPoint2DPointer qt_center_point(new QtPoint2D(0,center_point->GetSDOF(), center_point->GetTDOF(), GetSketchPlane()));

	// need to explicitly make these points dependent on the arc primitive so that if the arc primitive is ever deleted from the scene, these primitives will be deleted also
	qt_point1->AddPrimitive(new_arc);
	qt_point2->AddPrimitive(new_arc);
	qt_center_point->AddPrimitive(new_arc);

	AddPrimitive(qt_point1);
	AddPrimitive(qt_point2);
	AddPrimitive(qt_center_point);

	return new_arc;
}

QtCircle2DPointer QtSketch::AddCircle2D (double s_center, double t_center, double radius, bool s_center_free, bool t_center_free, bool radius_free)
{
    QtCircle2DPointer new_circle(new QtCircle2D(0,s_center, t_center, radius, GetSketchPlane(),s_center_free, t_center_free, radius_free));
    AddPrimitive(new_circle);

    Point2DPointer center_point = new_circle->GetCenterPoint();

    QtPoint2DPointer qt_center_point(new QtPoint2D(0,center_point->GetSDOF(), center_point->GetTDOF(), GetSketchPlane()));

    // need to explicitly make the center point dependent on the circle primitive so that if the circle primitive is ever deleted from the scene, the primitives will be deleted also
    qt_center_point->AddPrimitive(new_circle);

    AddPrimitive(qt_center_point);

    return new_circle;
}

QtCircle2DPointer QtSketch::AddCircle2D (DOFPointer s_center, DOFPointer t_center, double radius, bool radius_free)
{
    DOFPointer radius_dof(new IndependentDOF(radius,radius_free));
    QtCircle2DPointer new_circle(new QtCircle2D(0,s_center,t_center,radius_dof,GetSketchPlane()));
    AddPrimitive(new_circle);
    return new_circle;
}

QtArc2DPointer QtSketch::AddArc2D (double s1, double t1, double s2, double t2, double s3, double t3, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free)
{
	bool success = true;
	
	QtArc2DPointer new_arc;

	try{
		new_arc.reset(new QtArc2D(0,s1,t1,s2,t2,s3,t3, GetSketchPlane(),s_center_free, t_center_free, theta_1_free, theta_2_free, radius_free));
	}
	catch (pSketcherException e)
	{
		// all three points were on a straight line so no arc could be made
		success = false;
	}
	
	if(success)
	{
		AddPrimitive(new_arc);
	
		// now add the end points and the center of the arc as seperate primitives so that they can be selected by the user for constructing lines and other primitives
		// @fixme these points need to be removed fro the sketch if new_arc is ever deleted from the scene otherwise the arc will still drive the points but will not be visible
	
		Point2DPointer point1 = new_arc->GetPoint1();
		Point2DPointer point2 = new_arc->GetPoint2();
		Point2DPointer center_point = new_arc->GetCenterPoint();
	
		QtPoint2DPointer qt_point1(new QtPoint2D(0,point1->GetSDOF(), point1->GetTDOF(), GetSketchPlane()));
		QtPoint2DPointer qt_point2(new QtPoint2D(0,point2->GetSDOF(), point2->GetTDOF(), GetSketchPlane()));
		QtPoint2DPointer qt_center_point(new QtPoint2D(0,center_point->GetSDOF(), center_point->GetTDOF(), GetSketchPlane()));
	
		// need to explicitly make these points dependent on the arc primitive so that if the arc primitive is ever deleted from the scene, these primitives will be deleted also
		qt_point1->AddPrimitive(new_arc);
		qt_point2->AddPrimitive(new_arc);
		qt_center_point->AddPrimitive(new_arc);
	
		AddPrimitive(qt_point1);
		AddPrimitive(qt_point2);
		AddPrimitive(qt_center_point);
	}


	return new_arc;

}


QtLine2DPointer QtSketch::AddLine2D (const Point2DPointer point1, const Point2DPointer point2)
{
	QtLine2DPointer new_line(new QtLine2D(0,point1, point2, GetSketchPlane()));
	AddPrimitive(new_line);
	return new_line;
}


QtDistancePoint2DPointer QtSketch::AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2, double distance)
{
	QtDistancePoint2DPointer new_constraint(new QtDistancePoint2D(0,point1,point2,distance));
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

// Add a distance constraint using the current distance
QtDistancePoint2DPointer QtSketch::AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2)
{
	QtDistancePoint2DPointer new_constraint(new QtDistancePoint2D(0,point1,point2,1.0));
	new_constraint->SetValue(new_constraint->GetActualDistance());
	new_constraint->UpdateDisplay();
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

// Add a distance constraint using the current distance
QtDistancePointLine2DPointer QtSketch::AddDistancePointLine2D(const Point2DPointer point, const Line2DPointer line)
{
    QtDistancePointLine2DPointer new_constraint(new QtDistancePointLine2D(0,point,line,1.0));
    new_constraint->SetValue(new_constraint->GetActualDistance());
    new_constraint->UpdateDisplay();
    AddConstraintEquation(new_constraint);
    return new_constraint;
}

QtParallelLine2DPointer QtSketch::AddParallelLine2D(const Line2DPointer line1, const Line2DPointer line2)
{
	QtParallelLine2DPointer new_constraint (new QtParallelLine2D(0,line1, line2));
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

QtHoriVertLine2DPointer QtSketch::AddHoriVertLine2D(const Line2DPointer line, bool vertical_constraint)
{
	QtHoriVertLine2DPointer new_constraint (new QtHoriVertLine2D(0,line, vertical_constraint));
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

QtAngleLine2DPointer QtSketch::AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, double angle, bool interior_angle)
{
	QtAngleLine2DPointer new_constraint(new QtAngleLine2D(0,line1,line2,angle,interior_angle));
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

QtAngleLine2DPointer QtSketch::AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, bool interior_angle)
{
	QtAngleLine2DPointer new_constraint(new QtAngleLine2D(0,line1,line2,1.0,interior_angle)); // using a temp angle of 1.0, will be replaced by the current angle next
	new_constraint->SetAngleValue(new_constraint->GetActualAngle());
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

QtTangentEdge2DPointer QtSketch::AddTangentEdge2D(Edge2DBasePointer edge1, EdgePointNumber point_num_1, Edge2DBasePointer edge2, EdgePointNumber point_num_2)
{
	QtTangentEdge2DPointer new_constraint(new QtTangentEdge2D(0,edge1, point_num_1, edge2, point_num_2));
	AddConstraintEquation(new_constraint);
	return new_constraint;
}

void QtSketch::AddConstraintEquation(const ConstraintEquationBasePointer &new_constraint_equation, bool update_database)
{
	// if this constraint equation is derived from QtPrimitiveBase add it to the scene
	
	// Normally this type of cast is dangerous since it does the increment share_ptr's reference count
	// It is safe to pass this pointer to QGraphicsScene since QGraphicsItem's destructor removes itself from the QGraphicsScene it is a member of before deleting itself. Becuase of this, scene_ will not contain a nonexistant temp_graphics_item
	QGraphicsItem *temp_graphics_item = dynamic_cast<QGraphicsItem*>(new_constraint_equation.get());

	if(temp_graphics_item != 0)
	{
		scene_->addItem(temp_graphics_item);
		//std::cout << "adding constraint equation to the scene" << std::endl;
	}

	// now call the base class version of this method
	pSketcherModel::AddConstraintEquation(new_constraint_equation,update_database);
}

void QtSketch::AddPrimitive(const PrimitiveBasePointer &new_primitive, bool update_database)
{
	// if this primitive is derived from QtPrimitiveBase add it to the scene
	
	// Normally this type of cast is dangerous since it does the increment share_ptr's reference count
	// It is safe to pass this pointer to QGraphicsScene since QGraphicsItem's destructor removes itself from the QGraphicsScene it is a member of before deleting itself. Becuase of this, scene_ will not contain a nonexistant temp_graphics_item
	QGraphicsItem *temp_graphics_item = dynamic_cast<QGraphicsItem*>(new_primitive.get());

	if(temp_graphics_item != 0)
	{
		scene_->addItem(temp_graphics_item);
		//std::cout << "adding primitive to the scene" << std::endl;
	}

	// now call the base class version of this method
	pSketcherModel::AddPrimitive(new_primitive, update_database);
}

PrimitiveBasePointer QtSketch::PrimitiveFactory(unsigned id, pSketcherModel &psketcher_model)
{
	// grab the table name from the database so we now exactly which class needs to be created
	int rc;
	sqlite3_stmt *statement;
	stringstream table_name_stream;
	string table_name;

	stringstream sql_command;
	sql_command << "SELECT * FROM primitive_list WHERE id=" << id << ";";

	rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);

	if(rc == SQLITE_ROW) {
		// row exist, store the values to initialize this object
		table_name_stream << sqlite3_column_text(statement,1);
		table_name = table_name_stream.str();
	} else {
		// the requested row does not exist in the database
		sqlite3_finalize(statement);

		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);
	if( rc!=SQLITE_DONE ){
		// sql statement didn't finish properly, some error must to have occured
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}
	
	rc = sqlite3_finalize(statement);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	// now generate the object based on the table name
	PrimitiveBasePointer result;

	if(table_name == SQL_arc2d_database_table_name)
	{
		result.reset(new QtArc2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_line2d_database_table_name){
		result.reset(new QtLine2D(0,id,psketcher_model));
	}
    else if(table_name == SQL_circle2d_database_table_name){
        result.reset(new QtCircle2D(0,id,psketcher_model));
    }
	else if(table_name == SQL_point_database_table_name){
		result.reset(new Point(id,psketcher_model));
	}
	else if(table_name == SQL_point2d_database_table_name){
		result.reset(new QtPoint2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_sketch_plane_database_table_name){
		result.reset(new SketchPlane(id,psketcher_model));
	}
	else if(table_name == SQL_vector_database_table_name){
		result.reset(new Vector(id,psketcher_model));
	}
	else {
		throw pSketcherException("pSketcher::PrimitiveFactory: Unable to determine type based on database table name " + table_name);	
	}

	return result;
}

ConstraintEquationBasePointer QtSketch::ConstraintFactory(unsigned id, pSketcherModel &psketcher_model)
{

	// grab the table name from the database so we now exactly which class needs to be created
	int rc;
	sqlite3_stmt *statement;
	stringstream table_name_stream;
	string table_name;

	stringstream sql_command;
	sql_command << "SELECT * FROM constraint_equation_list WHERE id=" << id << ";";

	rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);

	if(rc == SQLITE_ROW) {
		// row exist, store the values to initialize this object
		table_name_stream << sqlite3_column_text(statement,1);
		table_name = table_name_stream.str();
	} else {
		// the requested row does not exist in the database
		sqlite3_finalize(statement);

		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);
	if( rc!=SQLITE_DONE ){
		// sql statement didn't finish properly, some error must to have occured
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}
	
	rc = sqlite3_finalize(statement);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	// now generate the object based on the table name
	ConstraintEquationBasePointer result;

	if(table_name == SQL_angle_line2d_database_table_name){
		result.reset(new QtAngleLine2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_distance_point2d_database_table_name){
		result.reset(new QtDistancePoint2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_parallel_line2d_database_table_name){
		result.reset(new QtParallelLine2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_horivert_line2d_database_table_name){
		result.reset(new QtHoriVertLine2D(0,id,psketcher_model));
	}
	else if(table_name == SQL_tangent_edge2d_database_table_name){
		result.reset(new QtTangentEdge2D(0,id,psketcher_model));
	}
    else if(table_name == SQL_distance_pointline2d_database_table_name){
        result.reset(new QtDistancePointLine2D(0,id,psketcher_model));
    }
	else {
		throw pSketcherException("pSketcher::ConstraintFactory: Unable to determine type based on database table name " + table_name);
	}

	return result;
}




