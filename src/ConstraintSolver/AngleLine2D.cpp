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

#include "AngleLine2D.h"
#include "IndependentDOF.h"

#include "pSketcherModel.h"

using namespace std;

// Create an angle constraint between two lines
AngleLine2D::AngleLine2D(const Line2DPointer line1, const Line2DPointer line2, double angle /* radians */, bool interior_angle):
line1_(line1),
line2_(line2),
interior_angle_(interior_angle),
text_angle_(new IndependentDOF(0.0,false)),
text_radius_(new IndependentDOF(0.0,false)),
text_s_(new IndependentDOF(0.0,false)),
text_t_(new IndependentDOF(0.0,false))
{
	// store the primitives that this primitive depends on
	AddPrimitive(line1);
	AddPrimitive(line2);

	AddDOF(line1_->GetS1());
	AddDOF(line1_->GetS2());
	AddDOF(line1_->GetT1());
	AddDOF(line1_->GetT2());

	AddDOF(line2_->GetS1());
	AddDOF(line2_->GetS2());
	AddDOF(line2_->GetT1());
	AddDOF(line2_->GetT2());

	AddDOF(text_angle_);
	AddDOF(text_radius_);
	AddDOF(text_s_);
	AddDOF(text_t_);

	SetDefaultTextLocation();

	// Create a DOF for the angle parameter
	DOFPointer new_dof(new IndependentDOF(angle,false));
	angle_ = new_dof;	

	AddDOF(angle_);

	// define the constraint equation
	if(interior_angle_)
    {
        solver_function_.reset(new  angle_line_2d_interior(line1_->GetS1(),line1_->GetT1(),line1_->GetS2(),line1_->GetT2(),line2_->GetS1(),line2_->GetT1(),line2_->GetS2(),line2_->GetT2(),angle_));
	} else {
        solver_function_.reset(new  angle_line_2d_exterior(line1_->GetS1(),line1_->GetT1(),line1_->GetS2(),line1_->GetT2(),line2_->GetS1(),line2_->GetT1(),line2_->GetS2(),line2_->GetT2(),angle_));
    }

    weight_ = 1.0;
}

// Construct from database
AngleLine2D::AngleLine2D(unsigned id, pSketcherModel &psketcher_model)
{
	SetID(id);  bool exists = SyncToDatabase(psketcher_model);
	
	if(!exists) // this object does not exist in the table
	{
		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << SQL_angle_line2d_database_table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}
}

void AngleLine2D::SetDefaultTextLocation()
{
	// first determine the intersection point of the two lines
	double x1 = line1_->GetPoint1()->GetSValue();
	double x2 = line1_->GetPoint2()->GetSValue();
	double x3 = line2_->GetPoint1()->GetSValue();
	double x4 = line2_->GetPoint2()->GetSValue();

	double y1 = line1_->GetPoint1()->GetTValue();
	double y2 = line1_->GetPoint2()->GetTValue();
	double y3 = line2_->GetPoint1()->GetTValue();
	double y4 = line2_->GetPoint2()->GetTValue();

	double denominator = (x1-x2)*(y3-y4)-(x3-x4)*(y1-y2);

	if(denominator == 0.0)
	{
		// so use the length of the lines to set a reasonable radius and set angle to 0.0
		text_radius_->SetValue(0.25*(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)) + sqrt((x3-x4)*(x3-x4)+(y3-y4)*(y3-y4))));
		text_angle_->SetValue(0.0);

		// use the following parameters to locate the text instead
		text_s_->SetValue(0.25*(x1+x2+x3+x4));
		text_t_->SetValue(0.25*(y1+y2+y3+y4));
	} else {
		// lines do intersect
		// finish calculating the intersection point
		double temp1 = x1*y2-y1*x2;
		double temp2 = x3*y4-x4*y3;
		
		double x_center = (temp1*(x3-x4)-temp2*(x1-x2))/denominator;
		double y_center = (temp1*(y3-y4)-temp2*(y1-y2))/denominator;
		
		// calculate the average radius and the average angle of all of the line endpoints
		double ave_radius = 0.0;
		double ave_angle = 0.0;
		
		ave_radius += sqrt((x1-x_center)*(x1-x_center)+(y1-y_center)*(y1-y_center));
		ave_radius += sqrt((x2-x_center)*(x2-x_center)+(y2-y_center)*(y2-y_center));
		ave_radius += sqrt((x3-x_center)*(x3-x_center)+(y3-y_center)*(y3-y_center));
		ave_radius += sqrt((x4-x_center)*(x4-x_center)+(y4-y_center)*(y4-y_center));
		ave_radius = ave_radius/4.0;

		int angle_counter = 0;
		if((y1-y_center)*(y1-y_center)+(x1-x_center)*(x1-x_center) > 0)
		{
			ave_angle += atan2(y1-y_center,x1-x_center) + 2.0*mmcPI;
			angle_counter++;
		}

		if((y2-y_center)*(y2-y_center)+(x2-x_center)*(x2-x_center) > 0)
		{
			ave_angle += atan2(y2-y_center,x2-x_center) + 2.0*mmcPI;
			angle_counter++;
		}

		if((y3-y_center)*(y3-y_center)+(x3-x_center)*(x3-x_center) > 0)
		{
			ave_angle += atan2(y3-y_center,x3-x_center) + 2.0*mmcPI;
			angle_counter++;
		}

		if((y4-y_center)*(y4-y_center)+(x4-x_center)*(x4-x_center) > 0)
		{
			ave_angle += atan2(y4-y_center,x4-x_center) + 2.0*mmcPI;
			angle_counter++;
		}

		if(angle_counter > 0)
			ave_angle = ave_angle / (double)angle_counter;
		else
			ave_angle = 0.0;

		text_radius_->SetValue(ave_radius*0.75);
		text_angle_->SetValue(ave_angle);
	}
}

void AngleLine2D::SetSTTextLocation(double text_s, double text_t, bool update_db)
{
	// first determine the intersection point of the two lines
	double x1 = line1_->GetPoint1()->GetSValue();
	double x2 = line1_->GetPoint2()->GetSValue();
	double x3 = line2_->GetPoint1()->GetSValue();
	double x4 = line2_->GetPoint2()->GetSValue();

	double y1 = line1_->GetPoint1()->GetTValue();
	double y2 = line1_->GetPoint2()->GetTValue();
	double y3 = line2_->GetPoint1()->GetTValue();
	double y4 = line2_->GetPoint2()->GetTValue();

	double denominator = (x1-x2)*(y3-y4)-(x3-x4)*(y1-y2);

	if(denominator == 0.0)
	{
		// the lines are parallel

		// use the following parameters to locate the text instead of text_radius_ and text_angle_
		text_s_->SetValue(text_s, update_db);
		text_t_->SetValue(text_t, update_db);
	} else {
		// lines do intersect
		// finish calculating the intersection point
		double temp1 = x1*y2-y1*x2;
		double temp2 = x3*y4-x4*y3;
		
		double x_center = (temp1*(x3-x4)-temp2*(x1-x2))/denominator;
		double y_center = (temp1*(y3-y4)-temp2*(y1-y2))/denominator;

		text_radius_->SetValue(sqrt((x_center - text_s)*(x_center - text_s) + (y_center - text_t)*(y_center - text_t)), update_db);
		text_angle_->SetValue(atan2(text_t-y_center, text_s-x_center), update_db);
	}
}

double AngleLine2D::GetActualAngle() const
{	
	double line1_ds = line1_->GetS2()->GetValue() - line1_->GetS1()->GetValue();
	double line1_dt = line1_->GetT2()->GetValue() - line1_->GetT1()->GetValue();
	double line1_length = sqrt(line1_ds*line1_ds+line1_dt*line1_dt);

	double line2_ds = line2_->GetS2()->GetValue() - line2_->GetS1()->GetValue();
	double line2_dt = line2_->GetT2()->GetValue() - line2_->GetT1()->GetValue();
	double line2_length = sqrt(line2_ds*line2_ds+line2_dt*line2_dt);

	double actual_angle;
	if(interior_angle_)
		actual_angle = acos((1/(line1_length*line2_length))*(line1_ds*line2_ds + line1_dt*line2_dt));
	else
		actual_angle = mmcPI - acos((1/(line1_length*line2_length))*(line1_ds*line2_ds + line1_dt*line2_dt));


	return actual_angle;
}

void AngleLine2D::AddToDatabase(sqlite3 *database)
{
	database_ = database;
	DatabaseAddRemove(true);
}

void AngleLine2D::RemoveFromDatabase()
{
	DatabaseAddRemove(false);
}

void AngleLine2D::DatabaseAddRemove(bool add_to_database) // Utility method used by AddToDatabase and RemoveFromDatabase
{
	string sql_do, sql_undo;

	stringstream dof_list_table_name;
	dof_list_table_name << "dof_table_" << GetID();
	stringstream primitive_list_table_name;
	primitive_list_table_name << "primitive_table_" << GetID();
	stringstream constraint_list_table_name;

	// First, create the sql statements to undo and redo this operation
	stringstream temp_stream;
	temp_stream.precision(__DBL_DIG__);
	temp_stream << "BEGIN; "
                << "INSERT INTO " << SQL_angle_line2d_database_table_name << " VALUES(" 
                << GetID() << ",'" << dof_list_table_name.str() << "','" 
				<< primitive_list_table_name.str() 
				<< "'," << line1_->GetID() << "," << line2_->GetID()
				<< "," << angle_->GetID() << "," << interior_angle_
				<< "," << text_angle_->GetID() << "," << text_radius_->GetID()
				<< "," << text_s_->GetID() << "," << text_t_->GetID()
                << "," << weight_
				<< "); "
                << "INSERT INTO constraint_equation_list VALUES("
                << GetID() << ",'" << SQL_angle_line2d_database_table_name << "'); "
                << "COMMIT; ";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str(""); // clears the string stream

	temp_stream << "BEGIN; "
				<< "DELETE FROM constraint_equation_list WHERE id=" << GetID() 
				<< "; DELETE FROM " << SQL_angle_line2d_database_table_name << " WHERE id=" << GetID() 
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
			rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_angle_line2d_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
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

	// Now use the methods provided by PrimitiveBase and ConstraintEquationBase to create the tables listing the DOF's, the other Primitives that this primitive depends on, and the constraint equations
	DatabaseAddDeleteLists(add_to_database,dof_list_table_name.str(),primitive_list_table_name.str());
}

bool AngleLine2D::SyncToDatabase(pSketcherModel &psketcher_model)
{
	database_ = psketcher_model.GetDatabase();

	string table_name = SQL_angle_line2d_database_table_name;

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
		line1_ = psketcher_model.FetchPrimitive<Line2D>(sqlite3_column_int(statement,3));
		line2_ = psketcher_model.FetchPrimitive<Line2D>(sqlite3_column_int(statement,4));
		angle_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,5));
		interior_angle_ = sqlite3_column_int(statement,6);
		text_angle_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,7));
		text_radius_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,8));
		text_s_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,9));
		text_t_ = psketcher_model.FetchDOF(sqlite3_column_int(statement,10));
        weight_ = sqlite3_column_double(statement,11);

        if(interior_angle_)
        {
            solver_function_.reset(new  angle_line_2d_interior(line1_->GetS1(),line1_->GetT1(),line1_->GetS2(),line1_->GetT2(),line2_->GetS1(),line2_->GetT1(),line2_->GetS2(),line2_->GetT2(),angle_));
        } else {
            solver_function_.reset(new  angle_line_2d_exterior(line1_->GetS1(),line1_->GetT1(),line1_->GetS2(),line1_->GetT2(),line2_->GetS1(),line2_->GetT1(),line2_->GetS2(),line2_->GetT2(),angle_));
        }

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
