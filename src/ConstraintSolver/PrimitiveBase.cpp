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

#include "PrimitiveBase.h"
#include "DependentDOF.h"
#include "pSketcherModel.h"

using namespace std;

// Initialize private static variables for DOF and PrimitiveBase classes
unsigned PrimitiveBase::next_id_number_ = 1;

PrimitiveBase::PrimitiveBase():
id_number_(next_id_number_++),
selected_(false),
selectable_(true),
delete_me_(false),
database_(0)
{
	//cout << "In PrimitiveBase constructor" << endl;
}

void PrimitiveBase::AddPrimitive(boost::shared_ptr<PrimitiveBase> new_primitive) 
{
	primitive_list_.push_back(new_primitive);

	// remove any duplicate primitives from the primitive list
	sort( primitive_list_.begin(), primitive_list_.end());
	primitive_list_.erase( unique( primitive_list_.begin(), primitive_list_.end()), primitive_list_.end());
}

void PrimitiveBase::SetSelectable(bool selectable)
{
	selectable_ = selectable;
	
	// if not selectable, need to make sure selected flag is false
	if(!selectable_)
		selected_ = false;
}

// Default selection mask implementation, each derived class should override this method
void PrimitiveBase::ApplySelectionMask(SelectionMask mask)
{
	if(mask == All)
		SetSelectable(true);
	else
		SetSelectable(false);
}

void PrimitiveBase::AddDOF(DOFPointer new_dof)
{
	// add the new dof to the list
	dof_list_.push_back(new_dof);

	// check to see if the dof to be added is a dependent, if so at the dof's that it dependends on as well
	if(dynamic_cast<DependentDOF*>(new_dof.get()) != 0)
	{
		DependentDOFPointer dependent_dof = boost::dynamic_pointer_cast<DependentDOF>(new_dof);
		
		std::vector<DOFPointer> temp_dof_list = dependent_dof->GetDOFList();

		for(unsigned int current_dof = 0; current_dof < temp_dof_list.size(); current_dof++)
			dof_list_.push_back(temp_dof_list[current_dof]);
	}
	
	// lastly, remove any duplicate degrees of freedom in the list
	sort( dof_list_.begin(), dof_list_.end());
	dof_list_.erase( unique( dof_list_.begin(), dof_list_.end()), dof_list_.end());
}

// returns true if deletion flag changes, otherwise returns false
bool PrimitiveBase::FlagForDeletionIfDependent(boost::shared_ptr<PrimitiveBase> input_primitive)
{
	// short circuit if already flagged for deletion
	if(IsFlaggedForDeletion())
		return false;

	// check to see if this primitive depends on the input primitive
	if (find(primitive_list_.begin(), primitive_list_.end(), input_primitive) != primitive_list_.end())
	{
		FlagForDeletion();
		return true;
	} else {
		return false;
	}
}

// utility method used to add the dof_list_ and the primitive_list_ to the database
void PrimitiveBase::DatabaseAddDeleteLists(bool add_to_database, const string &dof_list_table_name, const string &primitive_list_table_name)
{
	string sql_do, sql_undo;

	stringstream temp_stream;

	temp_stream << "BEGIN;"
				<< "CREATE TABLE " << dof_list_table_name << " (id INTEGER PRIMARY KEY, FOREIGN KEY(id) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);"
				<< "CREATE TABLE " << primitive_list_table_name << " (id INTEGER PRIMARY KEY, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

	// add the insert statements for each dof in dof_list_	
	for(unsigned int current_dof = 0; current_dof < dof_list_.size(); current_dof++)
	{
		temp_stream << "INSERT INTO " << dof_list_table_name << " VALUES(" << dof_list_[current_dof]->GetID() << "); ";
	}

	// add the insert statements for each primitive in primitive_list_	
	for(unsigned int current_primitive = 0; current_primitive < primitive_list_.size(); current_primitive++)
	{
		temp_stream << "INSERT INTO " << primitive_list_table_name << " VALUES(" << primitive_list_[current_primitive]->GetID() << "); ";
	}

	temp_stream << "COMMIT;";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str("");

	// define the undo sql statement
	temp_stream << "BEGIN; "
 				<< "DROP TABLE " << dof_list_table_name
				<< "; DROP TABLE " << primitive_list_table_name
				<< "; COMMIT;";

	if(add_to_database)
		sql_undo = temp_stream.str();
	else
		sql_do = temp_stream.str();

	// go ahead and create the sql tables
	char *zErrMsg = 0;
	int rc = sqlite3_exec(database_, sql_do.c_str(), 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		std::string error_description = "SQL error: " + std::string(zErrMsg);
		sqlite3_free(zErrMsg);
		throw pSketcherException(error_description);
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
}

// Utility method to sync dof_list_ and primitive_list_ to the database
void PrimitiveBase::SyncListsToDatabase(const std::string &dof_list_table_name, const std::string &primitive_list_table_name, pSketcherModel &psketcher_model)
{
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *statement;

	// synchronize the dof_list_ vector to the database
	
	// clear the contents of the vector, will be recreated from the database
	dof_list_.clear();

	stringstream sql_command;
	sql_command << "SELECT * FROM " << dof_list_table_name << ";";

	rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);
	
	int current_dof_id;
	DOFPointer current_dof;
	while(rc == SQLITE_ROW) {
		current_dof_id = sqlite3_column_int(statement,0);

		// get the dof (it will be automatically created from the database if it doesn't already exist)
		current_dof = psketcher_model.FetchDOF(current_dof_id);

		dof_list_.push_back(current_dof);

		rc = sqlite3_step(statement);
	}

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



	// synchronize the primitive_list_ to the database

	// clear the contents of the vector, will be recreated from the database
	primitive_list_.clear();

	sql_command.str(""); // clear the contents of the string stream
	sql_command << "SELECT * FROM " << primitive_list_table_name << ";";

	rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);
	
	int current_primitive_id;
	PrimitiveBasePointer current_primitive;
	while(rc == SQLITE_ROW) {
		current_primitive_id = sqlite3_column_int(statement,0);

		// get the dof (it will be automatically created from the database if it doesn't already exist)
		current_primitive = psketcher_model.FetchPrimitive<PrimitiveBase>(current_primitive_id);

		primitive_list_.push_back(current_primitive);

		rc = sqlite3_step(statement);
	}

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

}
