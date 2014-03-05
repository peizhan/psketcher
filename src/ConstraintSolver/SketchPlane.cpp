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

#include "SketchPlane.h"

#include "pSketcherModel.h"

using namespace std;

// Construct from database
SketchPlane::SketchPlane(unsigned id, pSketcherModel &psketcher_model)
{
	SetID(id);  bool exists = SyncToDatabase(psketcher_model);
	
	if(!exists) // this object does not exist in the table
	{
		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << SQL_sketch_plane_database_table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}
}

// Constructor for SketchPlane class
SketchPlane::SketchPlane ( VectorPointer normal, VectorPointer up, PointPointer base):
normal_(normal),
up_(up),
base_(base)
{
	AddPrimitive(normal);
	AddPrimitive(up);
	AddPrimitive(base);

	// @TODO need to check to insure that the normal vector and the up vector are normalized, calculations later on will assume this

	// @TODO need to make sure that up vector lies in plane defined by normal vector (up vector must be orthogonal to normal vector)

	// Populate the primitve base classes DOF list for each of the primitives referenced by this SketchPlane
	std::vector<DOFPointer> current_dof_list = normal_->GetDOFList();
	for(unsigned int current_dof = 0; current_dof < current_dof_list.size(); current_dof++)
	{
		AddDOF(current_dof_list[current_dof]);
	}

	current_dof_list = up_->GetDOFList();
	for(unsigned int current_dof = 0; current_dof < current_dof_list.size(); current_dof++)
	{
		AddDOF(current_dof_list[current_dof]);
	}

	current_dof_list = base_->GetDOFList();
	for(unsigned int current_dof = 0; current_dof < current_dof_list.size(); current_dof++)
	{
		AddDOF(current_dof_list[current_dof]);
	}
}

void SketchPlane::GetABCD ( double & coef_a, double & coef_b, double & coef_c, double & coef_d)
{
	mmcMatrix normal_vector = normal_->GetmmcMatrix();
	mmcMatrix base_vector = base_->GetmmcMatrix();

	coef_a = normal_vector(0,0);
	coef_b = normal_vector(1,0);
	coef_c = normal_vector(2,0);
	coef_d = normal_vector.DotProduct(base_vector);
}

// Return the global coordinates of a point on the sketch plane
void SketchPlane::Get3DLocation ( double s, double t, double & x, double & y, double & z)
{
	mmcMatrix j_vector = up_->GetmmcMatrix();  // t axis direction vector in sketch plane
	mmcMatrix i_vector = j_vector.CrossProduct(normal_->GetmmcMatrix()); // s axis direction vector in sketch plane

	mmcMatrix global_position = base_->GetmmcMatrix() + s*i_vector + t*j_vector;

	x = global_position(0,0);
	y = global_position(1,0);
	z = global_position(2,0);
}

// Find the closest point on the sketch plane to the global coordinates x,y, and z and return its s and t coordinates
// the return value is the distance of the global point to the sketch plane (if the sketch plane normal vector is not normalized than the distance will be scaled)
double SketchPlane::GetSTLocation( double x, double y, double z, double &s, double &t)
{
	mmcMatrix global_point(3,1);
	global_point(0,0) = x;
	global_point(1,0) = y;
	global_point(2,0) = z;

	mmcMatrix normal_vector = normal_->GetmmcMatrix();
	mmcMatrix j_vector = up_->GetmmcMatrix();  // t axis direction vector in sketch plane
	mmcMatrix i_vector = j_vector.CrossProduct(normal_vector); // s axis direction vector in sketch plane

	mmcMatrix a_matrix = i_vector;
	a_matrix = a_matrix.CombineAsRow(j_vector);
	a_matrix = a_matrix.CombineAsRow(normal_vector);

	mmcMatrix result = a_matrix.GetInverse3By3()*(global_point - base_->GetmmcMatrix());

	s = result(0,0);
	t = result(1,0);

	return result(2,0);
}

void SketchPlane::ApplySelectionMask(SelectionMask mask)
{
	if(mask == All)
		SetSelectable(true);
	else
		SetSelectable(false);
}

// method for adding this object to the SQLite3 database, needs to be implement by each child class
void SketchPlane::AddToDatabase(sqlite3 *database)
{
	database_ = database;
	DatabaseAddRemove(true);
}

void SketchPlane::RemoveFromDatabase()
{
	DatabaseAddRemove(false);
}

void SketchPlane::DatabaseAddRemove(bool add_to_database) // Utility method used by AddToDatabase and RemoveFromDatabase
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
                << "INSERT INTO " << SQL_sketch_plane_database_table_name << " VALUES(" 
                << GetID() << ",'" << dof_list_table_name.str() << "','" 
				<< primitive_list_table_name.str() << "'," << base_->GetID() 
				<< "," << normal_->GetID() << "," << up_->GetID() << "); "
                << "INSERT INTO primitive_list VALUES("
                << GetID() << ",'" << SQL_sketch_plane_database_table_name << "'); "
                << "COMMIT; ";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str(""); // clears the string stream

	temp_stream << "BEGIN; "
				<< "DELETE FROM primitive_list WHERE id=" << GetID() 
				<< "; DELETE FROM " << SQL_sketch_plane_database_table_name << " WHERE id=" << GetID() 
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
			rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_sketch_plane_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
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



bool SketchPlane::SyncToDatabase(pSketcherModel &psketcher_model)
{
	database_ = psketcher_model.GetDatabase();

	string table_name = SQL_sketch_plane_database_table_name;

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
		base_ = psketcher_model.FetchPrimitive<Point>(sqlite3_column_int(statement,3));
		normal_ = psketcher_model.FetchPrimitive<Vector>(sqlite3_column_int(statement,4));
		up_ = psketcher_model.FetchPrimitive<Vector>(sqlite3_column_int(statement,5));

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
