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

// Begining of includes related to libdime (used for dxf import and export)
#include <dime/entities/Arc.h>
// End of includes related to libdime

#include "Arc2D.h"
#include "IndependentDOF.h"
#include "DependentDOF.h"
#include "SolverFunctions.h"
#include "pSketcherModel.h"

using namespace std;

// create an arc
Arc2D::Arc2D (double s_center, double t_center, double theta_1, double theta_2, double radius, SketchPlanePointer sketch_plane,
              bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free):
s_center_(new IndependentDOF(s_center,s_center_free)),
t_center_(new IndependentDOF(t_center,t_center_free)),
theta_1_(new IndependentDOF(theta_1,theta_1_free)),
theta_2_(new IndependentDOF(theta_2,theta_2_free)),
radius_(new IndependentDOF(radius,radius_free)),
Edge2DBase(sketch_plane)
{
    AddPrimitive(sketch_plane);

    AddDOF(s_center_);
    AddDOF(t_center_);
    AddDOF(theta_1_);
    AddDOF(theta_2_);
    AddDOF(radius_);

    point1_ = GeneratePoint1(); 
    AddPrimitive(point1_);
    point2_ = GeneratePoint2();
    AddPrimitive(point2_);
    center_point_ = GenerateCenterPoint();
    AddPrimitive(center_point_);

    SetDefaultTextLocation();
}

// construct 2D arc from three points on the sketch plane
Arc2D::Arc2D (double s1, double t1, double s2, double t2, double s3, double t3, SketchPlanePointer sketch_plane,
              bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free):
Edge2DBase(sketch_plane)
{
    double m11 = s1*(t2-t3)-t1*(s2-s3)+(s2*t3-t2*s3);
    double mag1 = s1*s1+t1*t1;
    double mag2 = s2*s2+t2*t2;
    double mag3 = s3*s3+t3*t3;
    double m12 = mag1*(t2-t3)-t1*(mag2-mag3)+(mag2*t3-mag3*t2);
    double m13 = mag1*(s2-s3) - s1*(mag2-mag3) + (mag2*s3-mag3*s2);
    double m14 = mag1*(s2*t3-t2*s3) - s1*(mag2*t3-t2*mag3) + t1*(mag2*s3-s2*mag3);

    // check to insure that the lines are not colinears1
    if(m11 == 0.0)
    {   
        // lines are colinear, throw an exception
        throw pSketcherException();
    } else {
        double s_center = 0.5*(m12/m11);
        double t_center = -0.5*(m13/m11);
        
        double radius = sqrt(s_center*s_center + t_center*t_center + m14/m11);
    
        double theta_1 = atan2(t1-t_center, s1-s_center);
        double theta_2 = atan2(t2-t_center, s2-s_center);
        double theta_3 = atan2(t3-t_center, s3-s_center);

        if(!((theta_2 <= theta_3 && theta_2 >= theta_1) || (theta_2 <= theta_1 && theta_2 >= theta_3)))
        {
            if(theta_3 < theta_1)
                theta_3 += 2.0*mmcPI;
            else
                theta_1 += 2.0*mmcPI;   
        }

        s_center_.reset(new IndependentDOF(s_center,s_center_free));
        t_center_.reset(new IndependentDOF(t_center,t_center_free));
        theta_1_.reset(new IndependentDOF(theta_1,theta_1_free));
        theta_2_.reset(new IndependentDOF(theta_3,theta_2_free));
        radius_.reset(new IndependentDOF(radius,radius_free));

        AddPrimitive(sketch_plane);
    
        AddDOF(s_center_);
        AddDOF(t_center_);
        AddDOF(theta_1_);
        AddDOF(theta_2_);
        AddDOF(radius_);
        
        point1_ = GeneratePoint1();
        AddPrimitive(point1_); 
        point2_ = GeneratePoint2();
        AddPrimitive(point2_);
        center_point_ = GenerateCenterPoint();
        AddPrimitive(center_point_);
    
        SetDefaultTextLocation();
    }
}


Arc2D::Arc2D (DOFPointer s_center, DOFPointer t_center, DOFPointer theta_1, DOFPointer theta_2, DOFPointer radius, SketchPlanePointer sketch_plane):
s_center_(s_center),
t_center_(t_center),
theta_1_(theta_1),
theta_2_(theta_2),
radius_(radius),
Edge2DBase(sketch_plane)
{
    AddPrimitive(sketch_plane);

    AddDOF(s_center_);
    AddDOF(t_center_);
    AddDOF(theta_1_);
    AddDOF(theta_2_);
    AddDOF(radius_);

    point1_ = GeneratePoint1();
    AddPrimitive(point1_);
    point2_ = GeneratePoint2();
    AddPrimitive(point2_);
    center_point_ = GenerateCenterPoint();
    AddPrimitive(center_point_);

    SetDefaultTextLocation();
}

// Construct from database
Arc2D::Arc2D(unsigned id, pSketcherModel &psketcher_model)
{
    SetID(id);  bool exists = SyncToDatabase(psketcher_model);
    
    if(!exists) // this object does not exist in the table
    {
        stringstream error_description;
        error_description << "SQLite rowid " << id << " in table " << SQL_arc2d_database_table_name << " does not exist";
        throw pSketcherException(error_description.str());
    }
}

void Arc2D::Get3DLocations(double & x_center, double & y_center, double & z_center) const
{
    sketch_plane_->Get3DLocation(s_center_->GetValue(), t_center_->GetValue(), x_center, y_center, z_center);
}


void Arc2D::ApplySelectionMask(SelectionMask mask)
{
    if(mask == All || mask == Edges || mask == Arcs)
        SetSelectable(true);
    else
        SetSelectable(false);
}


// Return a point that will follow the first endpoint of the arc
// This point will use dependent DOF's to define its location
Point2DPointer Arc2D::GeneratePoint1()
{
    SolverFunctionsBasePointer s_1(new arc2d_point_s(s_center_,radius_,theta_1_));
    SolverFunctionsBasePointer t_1(new arc2d_point_t(t_center_,radius_,theta_1_));

    // create dependent DOF's based on the above expressions
    DOFPointer s_dof(new DependentDOF(s_1));
    DOFPointer t_dof(new DependentDOF(t_1));

    // create the actual point object
    Point2DPointer result(new Point2D(s_dof, t_dof, sketch_plane_));
    return result;
}

// Return a point that will follow the second endpoint of the arc
// This point will use dependent DOF's to define its location
Point2DPointer Arc2D::GeneratePoint2()
{
    SolverFunctionsBasePointer s_2(new arc2d_point_s(s_center_,radius_,theta_2_));
    SolverFunctionsBasePointer t_2(new arc2d_point_t(t_center_,radius_,theta_2_));

    // create dependent DOF's based on the above expressions
    DOFPointer s_dof(new DependentDOF(s_2));
    DOFPointer t_dof(new DependentDOF(t_2));

    // create the actual point object
    Point2DPointer result(new Point2D(s_dof, t_dof, sketch_plane_));
    return result;
}

// Returns a point which will follow the center point of the arc
Point2DPointer Arc2D::GenerateCenterPoint()
{
    Point2DPointer result(new Point2D(s_center_, t_center_, sketch_plane_));
    return result;
}

void Arc2D::GetTangent1(DOFPointer & s_component, DOFPointer & t_component)
{
    SolverFunctionsBasePointer s_function(new arc2d_tangent_s(GetTheta1()));
    SolverFunctionsBasePointer t_function(new arc2d_tangent_t(GetTheta1())); 

    s_component.reset(new DependentDOF(s_function));
    t_component.reset(new DependentDOF(t_function));
}

void Arc2D::GetTangent2(DOFPointer & s_component, DOFPointer & t_component)
{
    SolverFunctionsBasePointer s_function(new arc2d_tangent_s(GetTheta2()));
    SolverFunctionsBasePointer t_function(new arc2d_tangent_t(GetTheta2())); 

    s_component.reset(new DependentDOF(s_function));
    t_component.reset(new DependentDOF(t_function));
}


void Arc2D::GetTangent1(double & s_component, double & t_component)
{
    s_component = sin(GetTheta1()->GetValue());
    t_component = -cos(GetTheta1()->GetValue());
}

void Arc2D::GetTangent2(double & s_component, double & t_component)
{
    s_component = -sin(GetTheta2()->GetValue());
    t_component = cos(GetTheta2()->GetValue());
}


void Arc2D::SetDefaultTextLocation()
{
    double text_angle = (theta_1_->GetValue() + theta_2_->GetValue())*0.5;
    text_angle_.reset(new IndependentDOF(text_angle,false));
    AddDOF(text_angle_);

    double text_radius = 0.5*radius_->GetValue();
    text_radius_.reset(new IndependentDOF(text_radius,false));
    AddDOF(text_radius_);
}

void Arc2D::SetSTTextLocation(double text_s, double text_t, bool update_db)
{
    text_radius_->SetValue(sqrt((s_center_->GetValue() - text_s)*(s_center_->GetValue() - text_s) + (t_center_->GetValue() - text_t)*(t_center_->GetValue() - text_t)),update_db);
    text_angle_->SetValue(atan2(text_t-t_center_->GetValue(), text_s-s_center_->GetValue()),update_db);
}


void Arc2D::AddToDatabase(sqlite3 *database)
{
    database_ = database;
    DatabaseAddRemove(true);
}

void Arc2D::RemoveFromDatabase()
{
    DatabaseAddRemove(false);
}

void Arc2D::DatabaseAddRemove(bool add_to_database) // Utility method used by AddToDatabase and RemoveFromDatabase
{
    string sql_do, sql_undo;

    stringstream dof_list_table_name;
    dof_list_table_name << "dof_table_" << GetID();
    stringstream primitive_list_table_name;
    primitive_list_table_name << "primitive_table_" << GetID();

    //"CREATE TABLE arc2d_list (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, sketch_plane INTEGER NOT NULL, center_point INTEGER NOT NULL, radius_dof INTEGER NOT NULL, s_center_dof INTEGER NOT NULL, t_center_dof INTEGER NOT NULL, theta_1_dof INTEGER NOT NULL, theta_2_dof INTEGER NOT NULL, end1_point INTEGER NOT NULL, end2_point INTEGER NOT NULL, text_angle_dof INTEGER NOT NULL, text_radius_dof INTEGER NOT NULL);";

    // First, create the sql statements to undo and redo this operation
    stringstream temp_stream;
    temp_stream.precision(__DBL_DIG__);
    temp_stream << "BEGIN; "
                << "INSERT INTO " << SQL_arc2d_database_table_name << " VALUES(" 
                << GetID() << ",'" << dof_list_table_name.str() << "','" 
                << primitive_list_table_name.str() << "'," << GetSketchPlane()->GetID() 
                << "," << center_point_->GetID() << "," << radius_->GetID()
                << "," << s_center_->GetID() << "," << t_center_->GetID() 
                << "," << theta_1_->GetID() << "," << theta_2_->GetID()
                << "," << point1_->GetID() << "," << point2_->GetID()
                << "," << text_angle_->GetID() << "," << text_radius_->GetID()
                << "); "
                << "INSERT INTO primitive_list VALUES("
                << GetID() << ",'" << SQL_arc2d_database_table_name << "'); "
                << "COMMIT; ";

    if(add_to_database)
        sql_do = temp_stream.str();
    else
        sql_undo = temp_stream.str();

    temp_stream.str(""); // clears the string stream

    temp_stream << "BEGIN; "
                << "DELETE FROM primitive_list WHERE id=" << GetID() 
                << "; DELETE FROM " << SQL_arc2d_database_table_name << " WHERE id=" << GetID() 
                << "; COMMIT;";

    if(add_to_database)
        sql_undo = temp_stream.str();
    else
        sql_do = temp_stream.str();

    // add this object to the appropriate tables by executing the SQL command sql_insert 
    char *zErrMsg = 0;
    int rc = sqlite3_exec(database_, sql_do.c_str(), 0, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        if(add_to_database)
        {
            //std::cerr << "SQL error: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
            
            // the table "independent_dof_list" may not exist, attempt to create
            rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_arc2d_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
            if( rc!=SQLITE_OK ){
                std::string error_description = "SQL error: " + std::string(zErrMsg);
                sqlite3_free(zErrMsg);
                throw pSketcherException(error_description);
            }
    
            // now that the table has been created, attempt the insert one last time
            rc = sqlite3_exec(database_, sql_do.c_str(), 0, 0, &zErrMsg);
            if( rc!=SQLITE_OK ){
                std::string error_description = "SQL error: " + std::string(zErrMsg);
                sqlite3_free(zErrMsg);
                throw pSketcherException(error_description);
            }
        } else {
            std::string error_description = "SQL error: " + std::string(zErrMsg);
            sqlite3_free(zErrMsg);
            throw pSketcherException(error_description);
        }
    }

    // finally, update the undo_redo_list in the database with the database changes that have just been made
    // need to use sqlite3_mprintf to make sure the single quotes in the sql statements get escaped where needed
    char *sql_undo_redo = sqlite3_mprintf("INSERT INTO undo_redo_list(undo,redo) VALUES('%q','%q')",sql_undo.c_str(),sql_do.c_str());

    rc = sqlite3_exec(database_, sql_undo_redo, 0, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        std::string error_description = "SQL error: " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        throw pSketcherException(error_description);
    }

    sqlite3_free(sql_undo_redo);

    // Now use the method provided by PrimitiveBase to create the tables listing the DOF's and the other Primitives that this primitive depends on
    DatabaseAddDeleteLists(add_to_database,dof_list_table_name.str(),primitive_list_table_name.str());
}

bool Arc2D::SyncToDatabase(pSketcherModel &psketcher_model)
{
    database_ = psketcher_model.GetDatabase();

    string table_name = SQL_arc2d_database_table_name;

    char *zErrMsg = 0;
    int rc;
    sqlite3_stmt *statement;
    
    stringstream sql_command;
    sql_command << "SELECT * FROM " << table_name << " WHERE id=" << GetID() << ";";

    rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
    if( rc!=SQLITE_OK ){
        stringstream error_description;
        error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
        throw pSketcherException(error_description.str());
    }

    rc = sqlite3_step(statement);

    stringstream dof_table_name, primitive_table_name;

    if(rc == SQLITE_ROW) {
        // row exists, store the values to initialize this object
        
        dof_table_name << sqlite3_column_text(statement,1);
        primitive_table_name << sqlite3_column_text(statement,2);
        SetSketchPlane(psketcher_model.FetchPrimitive<SketchPlane>(sqlite3_column_int(statement,3)));
        center_point_ = psketcher_model.FetchPrimitive<Point2D>(sqlite3_column_int(statement,4));
        radius_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,5));
        s_center_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,6));
        t_center_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,7));
        theta_1_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,8));
        theta_2_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,9));
        point1_ = psketcher_model.FetchPrimitive<Point2D>(sqlite3_column_int(statement,10));
        point2_ = psketcher_model.FetchPrimitive<Point2D>(sqlite3_column_int(statement,11));
        text_angle_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,12));
        text_radius_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,13));

    } else {
        // the requested row does not exist in the database
        sqlite3_finalize(statement);    

        return false; // row does not exist in the database, exit method and return false
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

    // now sync the lists store in the base classes
    SyncListsToDatabase(dof_table_name.str(),primitive_table_name.str(),psketcher_model); // PrimitiveBase

    return true; // row existed in the database
}

dimeEntity *Arc2D::GenerateDimeEntity() const
{
    dimeArc *output;

    // get the 3d coordinates for the center point
    double x_center, y_center, z_center;
    Get3DLocations(x_center,y_center,z_center);

    // get the normal vector for the current sketch plane which defines the axis direction of the arc
    double x_normal = sketch_plane_->GetNormal()->GetXValue();
    double y_normal = sketch_plane_->GetNormal()->GetYValue();
    double z_normal = sketch_plane_->GetNormal()->GetZValue();
    
    // define the center point and normal vector in the dime format
    dimeVec3f center(x_center,y_center,z_center);
    dimeVec3f normal(x_normal,y_normal,z_normal);

    // create the actual arc
    output = new dimeArc();

    double theta1 = theta_1_->GetValue()*(180.0/mmcPI);
    double theta2 = theta_2_->GetValue()*(180.0/mmcPI);

    if(theta1 > theta2)
    {
        double temp;
        temp = theta1;
        theta1 = theta2;
        theta2 = temp;
    }

    output->setExtrusionDir(normal);
    output->setCenter(center);
    output->setStartAngle(theta1);
    output->setEndAngle(theta2);
    output->setRadius(radius_->GetValue());

    return output;
}
