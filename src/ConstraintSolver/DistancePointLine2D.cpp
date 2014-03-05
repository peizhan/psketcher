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

#include "DistancePointLine2D.h"
#include "IndependentDOF.h"

#include "pSketcherModel.h"

using namespace std;


// Create a constraint that defines the distance between two points confined to a sketch plane
DistancePointLine2D::DistancePointLine2D(const Point2DPointer point, const Line2DPointer line, double distance):
point_(point),
line_(line)
{
	AddPrimitive(point);
	AddPrimitive(line);

	AddDOF(point->GetSDOF());
	AddDOF(point->GetTDOF());
	AddDOF(line->GetS1());
	AddDOF(line->GetT1());
	AddDOF(line->GetS2());
	AddDOF(line->GetT2());

	// Create a DOF for the distance parameter
	DOFPointer new_dof(new IndependentDOF(distance,false));
	distance_ = new_dof;

	AddDOF(distance_);

	// create DOF's for the text location
	text_offset_.reset(new IndependentDOF(0.0,false));
	text_position_.reset(new IndependentDOF(0.0,false));
	AddDOF(text_offset_);
	AddDOF(text_position_);	
	
	// Define the constraint function
    solver_function_.reset(new distance_point_line_2d(point_->GetSDOF(),point_->GetTDOF(),line_->GetS1(),line_->GetT1(),line_->GetS2(),line_->GetT2(),distance_));

	weight_ = 1.0;

	// text location was not specified so provide a reasonable default
	SetDefaultTextLocation();
}

// Construct from database
DistancePointLine2D::DistancePointLine2D(unsigned id, pSketcherModel &psketcher_model)
{
	SetID(id);  bool exists = SyncToDatabase(psketcher_model);
	
	if(!exists) // this object does not exist in the table
	{
		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << SQL_distance_pointline2d_database_table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}
}

// Calculate the current actual distance between the point and the line
// This method is used to set the distance when graphically generating a distance constraint
double DistancePointLine2D::GetActualDistance() const
{
	// calculate the distance between point1_ and point2_
	double term1 = (line_->GetS2()->GetValue() - line_->GetS1()->GetValue());
	double term1_sq = term1*term1;
	double term2 = (line_->GetT2()->GetValue() - line_->GetT1()->GetValue());
	double term2_sq = term2*term2;
	
	double den = sqrt(term1_sq+term2_sq);

	double term3 = (line_->GetT1()->GetValue() - point_->GetTDOF()->GetValue());
	double term4 = (line_->GetS1()->GetValue() - point_->GetSDOF()->GetValue());

	double num = fabs(term1*term3 - term2*term4);

	return num/den;
}


void DistancePointLine2D::SetDefaultTextLocation()
{
	mmcMatrix point1 = line_->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = line_->GetPoint2()->GetmmcMatrix();
	mmcMatrix point3 = point_->GetmmcMatrix();

	mmcMatrix tangent = (point2-point1);
	double tangent_magnitude = tangent.GetMagnitude();
	if (tangent_magnitude > 0.0)
	{
		tangent = tangent.GetScaled(1.0/tangent_magnitude);
	} else {
		// tangent vector has zero length, define an arbitrary tangent vector to avoid divide by zero
		tangent(0,0) = 1.0;
		tangent(1,0) = 0.0;	
	}

	mmcMatrix normal(2,1);
	normal(0,0) = -tangent(1,0);
	normal(1,0) = tangent(0,0);

	double dot_product = normal.DotProduct(point3-point1);
	
	if(dot_product >= 0.0)
		text_offset_->SetValue(0.5*GetActualDistance());
	else
		text_offset_->SetValue(-0.5*GetActualDistance());	

	if((point3 - point2).GetMagnitude() > (point3 - point1).GetMagnitude())
		text_position_->SetValue(-0.1*tangent_magnitude);
	else
		text_position_->SetValue(1.1*tangent_magnitude);
}

void DistancePointLine2D::SetSTTextLocation(double s, double t, bool update_db)
{
	mmcMatrix point1 = line_->GetPoint1()->GetmmcMatrix();
	mmcMatrix point2 = line_->GetPoint2()->GetmmcMatrix();

	mmcMatrix tangent = (point2-point1);
	double tangent_magnitude = tangent.GetMagnitude();
	if (tangent_magnitude > 0.0)
	{
		tangent = tangent.GetScaled(1.0/tangent_magnitude);
	} else {
		// tangent vector has zero length, define an arbitrary tangent vector to avoid divide by zero
		tangent(0,0) = 1.0;
		tangent(1,0) = 0.0;	
	}

	mmcMatrix normal(2,1);
	normal(0,0) = -tangent(1,0);
	normal(1,0) = tangent(0,0);

	mmcMatrix text_location(2,1);
	text_location(0,0) = s;
	text_location(1,0) = t;

	mmcMatrix inverse(2,2);
	inverse(0,0) = tangent(1,0);
	inverse(0,1) = -tangent(0,0);
	inverse(1,0) = -normal(1,0);
	inverse(1,1) = normal(0,0);
	inverse = (1.0/(normal(0,0)*tangent(1,0) - tangent(0,0)*normal(1,0)))*inverse;

	mmcMatrix solution = inverse*(text_location - point1);
	text_offset_->SetValue(solution(0,0),update_db);
	text_position_->SetValue(solution(1,0),update_db);
}

void DistancePointLine2D::AddToDatabase(sqlite3 *database)
{
	database_ = database;
	DatabaseAddRemove(true);
}

void DistancePointLine2D::RemoveFromDatabase()
{
	DatabaseAddRemove(false);
}

void DistancePointLine2D::DatabaseAddRemove(bool add_to_database) // Utility method used by AddToDatabase and RemoveFromDatabase
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
                << "INSERT INTO " << SQL_distance_pointline2d_database_table_name << " VALUES(" 
                << GetID() << ",'" << dof_list_table_name.str() << "','" 
				<< primitive_list_table_name.str() 
				<< "'," << distance_->GetID() << "," << point_->GetID()
				<< "," << line_->GetID() << "," << text_offset_->GetID() 
				<< "," << text_position_->GetID() 
                << "," << weight_ << "); "
                << "INSERT INTO constraint_equation_list VALUES("
                << GetID() << ",'" << SQL_distance_pointline2d_database_table_name << "'); "
                << "COMMIT; ";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str(""); // clears the string stream

	temp_stream << "BEGIN; "
				<< "DELETE FROM constraint_equation_list WHERE id=" << GetID() 
				<< "; DELETE FROM " << SQL_distance_pointline2d_database_table_name << " WHERE id=" << GetID() 
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
			rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_distance_pointline2d_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
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

	// Now use the method provided by PrimitiveBase to create the tables listing the DOF's and the other Primitives that this primitive depends on.
	DatabaseAddDeleteLists(add_to_database,dof_list_table_name.str(),primitive_list_table_name.str());
}


bool DistancePointLine2D::SyncToDatabase(pSketcherModel &psketcher_model)
{
	database_ = psketcher_model.GetDatabase();

	string table_name = SQL_distance_pointline2d_database_table_name;

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
		distance_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,3));
		point_ = psketcher_model.FetchPrimitive<Point2D>(sqlite3_column_int(statement,4));
		line_ = psketcher_model.FetchPrimitive<Line2D>(sqlite3_column_int(statement,5));
		text_offset_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,6));
		text_position_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,7));
        weight_ = sqlite3_column_double(statement,8);

        // Define the constraint function
        solver_function_.reset(new distance_point_line_2d(point_->GetSDOF(),point_->GetTDOF(),line_->GetS1(),line_->GetT1(),line_->GetS2(),line_->GetT2(),distance_));

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
