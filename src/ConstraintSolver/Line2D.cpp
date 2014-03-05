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
#include <dime/entities/Line.h>
// End of includes related to libdime

#include "Line2D.h"

#include "pSketcherModel.h"

using namespace std;

void Line2D::GetTangent1(DOFPointer & s_component, DOFPointer & t_component)
{
    SolverFunctionsBasePointer s_function(new point2d_tangent1_s(GetS1(),GetT1(),GetS2(),GetT2()));
    SolverFunctionsBasePointer t_function(new point2d_tangent1_t(GetS1(),GetT1(),GetS2(),GetT2()));

	s_component.reset(new DependentDOF(s_function));
	t_component.reset(new DependentDOF(t_function));
}

void Line2D::GetTangent2(DOFPointer & s_component, DOFPointer & t_component)
{
    SolverFunctionsBasePointer s_function(new point2d_tangent2_s(GetS1(),GetT1(),GetS2(),GetT2()));
    SolverFunctionsBasePointer t_function(new point2d_tangent2_t(GetS1(),GetT1(),GetS2(),GetT2()));

    s_component.reset(new DependentDOF(s_function));
    t_component.reset(new DependentDOF(t_function));
}

void Line2D::GetTangent1(double & s_component, double & t_component)
{
	double length = sqrt(pow(GetS1()->GetValue()-GetS2()->GetValue(),2)+pow(GetT1()->GetValue()-GetT2()->GetValue(),2));
	
	s_component = (GetS1()->GetValue() - GetS2()->GetValue())/length;
	t_component = (GetT1()->GetValue() - GetT2()->GetValue())/length;
}

void Line2D::GetTangent2(double & s_component, double & t_component)
{
	double length = sqrt(pow(GetS1()->GetValue()-GetS2()->GetValue(),2)+pow(GetT1()->GetValue()-GetT2()->GetValue(),2));
	
	s_component = (GetS2()->GetValue() - GetS1()->GetValue())/length;
	t_component = (GetT2()->GetValue() - GetT1()->GetValue())/length;
}

// returns global coordinates of line end points
void Line2D :: Get3DLocations(double & x1, double & y1, double & z1,
															double & x2, double & y2, double & z2) const
{
	sketch_plane_->Get3DLocation(s1_->GetValue(), t1_->GetValue(), x1, y1, z1);
	sketch_plane_->Get3DLocation(s2_->GetValue(), t2_->GetValue(), x2, y2, z2);
}

void Line2D::ApplySelectionMask(SelectionMask mask)
{
	if(mask == All || mask == Edges || mask == Lines || mask == PointsAndLines)
		SetSelectable(true);
	else
		SetSelectable(false);
}

Line2D :: Line2D(const Point2DPointer point1, const Point2DPointer point2, SketchPlanePointer sketch_plane):
Edge2DBase(sketch_plane)
{
	AddPrimitive(point1);
	AddPrimitive(point2);
	AddPrimitive(sketch_plane);

	s1_ = point1->GetSDOF();
	t1_ = point1->GetTDOF();

	s2_ = point2->GetSDOF();
	t2_ = point2->GetTDOF();

	AddDOF(s1_);
	AddDOF(t1_);

	AddDOF(s2_);
	AddDOF(t2_);
}

Point2DPointer Line2D::GetPoint1()
{
	if(point1_.get() == 0)
	{
		// point object does not exist yet so create it
		point1_.reset(new Point2D(GetS1(), GetT1(), GetSketchPlane()));
	}

	return point1_;
}

Point2DPointer Line2D::GetPoint2()
{
	if(point2_.get() == 0)
	{
		// create the actual point object
		point2_.reset(new Point2D(GetS2(), GetT2(), GetSketchPlane()));
	}
	
	return point2_;
}

// Construct from database
Line2D::Line2D(unsigned id, pSketcherModel &psketcher_model)
{
	SetID(id);  bool exists = SyncToDatabase(psketcher_model);
	
	if(!exists) // this object does not exist in the table
	{
		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << SQL_line2d_database_table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}
}

void Line2D::AddToDatabase(sqlite3 *database)
{
	database_ = database;
	DatabaseAddRemove(true);
}

void Line2D::RemoveFromDatabase()
{
	DatabaseAddRemove(false);
}

void Line2D::DatabaseAddRemove(bool add_to_database) // Utility method used by AddToDatabase and RemoveFromDatabase
{
	string sql_do, sql_undo;

	stringstream dof_list_table_name;
	dof_list_table_name << "dof_table_" << GetID();
	stringstream primitive_list_table_name;
	primitive_list_table_name << "primitive_table_" << GetID();

	// First, create the sql statements to undo and redo this operation
	stringstream temp_stream;
	temp_stream.precision(__DBL_DIG__);
	temp_stream << "BEGIN; "
                << "INSERT INTO " << SQL_line2d_database_table_name << " VALUES(" 
                << GetID() << ",'" << dof_list_table_name.str() << "','" 
				<< primitive_list_table_name.str() << "'," << GetSketchPlane()->GetID() 
				<< "," << s1_->GetID() << "," << t1_->GetID()
				<< "," << s2_->GetID() << "," << t2_->GetID() << "); "
                << "INSERT INTO primitive_list VALUES("
                << GetID() << ",'" << SQL_line2d_database_table_name << "'); "
                << "COMMIT; ";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str(""); // clears the string stream

	temp_stream << "BEGIN; "
				<< "DELETE FROM primitive_list WHERE id=" << GetID() 
				<< "; DELETE FROM " << SQL_line2d_database_table_name << " WHERE id=" << GetID() 
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
			rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_line2d_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
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


bool Line2D::SyncToDatabase(pSketcherModel &psketcher_model)
{
	database_ = psketcher_model.GetDatabase();

	string table_name = SQL_line2d_database_table_name;

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
		s1_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,4));
		t1_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,5));
		s2_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,6));
		t2_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,7));

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

dimeEntity *Line2D::GenerateDimeEntity() const
{
	dimeLine *output;

	// get the 3d coordinates for the line endpoints
	double x1,y1,z1,x2,y2,z2;
	Get3DLocations(x1,y1,z1,x2,y2,z2);
	
	// define the endpoints in the dime format
	dimeVec3f point1(x1,y1,z1);
	dimeVec3f point2(x2,y2,z2);

	// create the actual line
	output = new dimeLine();

	output->setCoords(0,point1);
	output->setCoords(1,point2);	

	return output;
}
