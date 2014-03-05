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


#include <string>
#include <sstream>
#include "IndependentDOF.h"
#include "PrimitiveBase.h"
#include "pSketcherModel.h"

using namespace std;

IndependentDOF ::IndependentDOF ( double value, bool free):
DOF(free,false /*dependent*/)
{
	value_=value;
}

IndependentDOF :: IndependentDOF ( const char *name, double value, bool free):
DOF(name,free,false /*dependent*/)
{
	value_ = value;
}

// the following constructor creates the DOF from the database stored in psketcher_model
IndependentDOF :: IndependentDOF ( unsigned id, pSketcherModel &psketcher_model ):
DOF(id,false /* bool dependent */)
{
	SetID(id);  bool exists = SyncToDatabase(psketcher_model);
	
	if(!exists) // this object does not exist in the table
	{
		stringstream error_description;
		error_description << "SQLite rowid " << id << " in table " << SQL_independent_dof_database_table_name << " does not exist";
		throw pSketcherException(error_description.str());
	}
}

bool IndependentDOF :: SyncToDatabase(pSketcherModel &psketcher_model)
{
	database_ = psketcher_model.GetDatabase();

	string table_name = SQL_independent_dof_database_table_name;

	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *statement;

	// "CREATE TABLE independent_dof_list (id INTEGER PRIMARY KEY, variable_name TEXT NOT NULL, bool_free INTEGER NOT NULL, value REAL NOT NULL);"
	
	stringstream sql_command;
	sql_command << "SELECT * FROM " << table_name << " WHERE id=" << GetID() << ";";

	rc = sqlite3_prepare(psketcher_model.GetDatabase(), sql_command.str().c_str(), -1, &statement, 0);
	if( rc!=SQLITE_OK ){
		stringstream error_description;
		error_description << "SQL error: " << sqlite3_errmsg(psketcher_model.GetDatabase());
		throw pSketcherException(error_description.str());
	}

	rc = sqlite3_step(statement);

	if(rc == SQLITE_ROW) {
		// row exist, store the values to initialize this object
		
		stringstream variable_name;
		variable_name << sqlite3_column_text(statement,1);
		name_ = variable_name.str();
		free_ = sqlite3_column_int(statement,2);
		value_ = sqlite3_column_double(statement,3);

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

	return true; // row existed in the database
}

void IndependentDOF::SetValue ( double value, bool update_db) 
{
    value_ = value;

	if(database_ != 0 && update_db ) // if this DOF is tied to a database then update the database
	{
		double old_value;

        // first retrieve the current value in the database so that the undo command can be set properly
        string table_name = SQL_independent_dof_database_table_name;
    
        char *zErrMsg = 0;
        int rc;
        sqlite3_stmt *statement;
        
        stringstream sql_command;
        sql_command << "SELECT * FROM " << table_name << " WHERE id=" << GetID() << ";";
    
        rc = sqlite3_prepare(database_, sql_command.str().c_str(), -1, &statement, 0);
        if( rc!=SQLITE_OK ){
            stringstream error_description;
            error_description << "SQL error: " << sqlite3_errmsg(database_);
            throw pSketcherException(error_description.str());
        }
    
        rc = sqlite3_step(statement);
    
        if(rc == SQLITE_ROW) {
            // row exist, retreive the previous value
            old_value = sqlite3_column_double(statement,3);
    
        } else {
            // the requested row does not exist in the database
            sqlite3_finalize(statement);    
            throw pSketcherException("DOF value is being updated for a DOF that is not currently stored in the database.");
        }
    
        rc = sqlite3_step(statement);
        if( rc!=SQLITE_DONE ){
            // sql statement didn't finish properly, some error must to have occured
            stringstream error_description;
            error_description << "SQL error: " << sqlite3_errmsg(database_);
            throw pSketcherException(error_description.str());
        }
        
        rc = sqlite3_finalize(statement);
        if( rc!=SQLITE_OK ){
            stringstream error_description;
            error_description << "SQL error: " << sqlite3_errmsg(database_);
            throw pSketcherException(error_description.str());
        }

        // Now that the prevoius value is know, update the database
		// define the update statement
		stringstream sql_stream;
		sql_stream.precision(__DBL_DIG__);
		sql_stream << "UPDATE " << SQL_independent_dof_database_table_name << " SET value=" 
					<< value_ << " WHERE id=" << GetID() << ";";

		string sql_update = sql_stream.str();
		
		// define the undo statement
		sql_stream.str("");
		sql_stream << "UPDATE " << SQL_independent_dof_database_table_name << " SET value=" 
					<< old_value << " WHERE id=" << GetID() << ";";
	
		string sql_undo = sql_stream.str();
		
		// do the database update
		rc = sqlite3_exec(database_, sql_update.c_str(), 0, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			std::string error_description = "SQL error: " + std::string(zErrMsg);
			sqlite3_free(zErrMsg);
			throw pSketcherException(error_description);
		}

		// store the undo/redo information in the database
		// need to use sqlite3_mprintf to make sure the single quotes in the sql statements get escaped where needed
		char *sql_undo_redo = sqlite3_mprintf("INSERT INTO undo_redo_list(undo,redo) VALUES('%q','%q');",sql_undo.c_str(),sql_update.c_str());
	
		rc = sqlite3_exec(database_, sql_undo_redo, 0, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			std::string error_description = "SQL error: " + std::string(zErrMsg);
			sqlite3_free(zErrMsg);
			throw pSketcherException(error_description);
		}

	} // if(database_ != 0 && update_db)
}

void IndependentDOF::SetFree(bool free)
{
	if(free != free_)
	{
		if(database_ != 0) // if this DOF is tied to a database then update the database
		{
			bool old_value = free_;
			free_ = free;
	
			// define the update statement
			stringstream sql_stream;
			sql_stream.precision(__DBL_DIG__);
			sql_stream << "UPDATE " << SQL_independent_dof_database_table_name << " SET bool_free=" 
						<< free_ << " WHERE id=" << GetID() << ";";
	
			string sql_update = sql_stream.str();
			
			// define the undo statement
			sql_stream.str("");
			sql_stream << "UPDATE " << SQL_independent_dof_database_table_name << " SET bool_free=" 
						<< old_value << " WHERE id=" << GetID() << ";";
		
			string sql_undo = sql_stream.str();
			
			// do the database update
			char *zErrMsg = 0;
			int rc = sqlite3_exec(database_, sql_update.c_str(), 0, 0, &zErrMsg);
			if( rc!=SQLITE_OK ){
				std::string error_description = "SQL error: " + std::string(zErrMsg);
				sqlite3_free(zErrMsg);
				throw pSketcherException(error_description);
			}
	
			// store the undo/redo information in the database
			// need to use sqlite3_mprintf to make sure the single quotes in the sql statements get escaped where needed
			char *sql_undo_redo = sqlite3_mprintf("INSERT INTO undo_redo_list(undo,redo) VALUES('%q','%q');",sql_undo.c_str(),sql_update.c_str());
		
			rc = sqlite3_exec(database_, sql_undo_redo, 0, 0, &zErrMsg);
			if( rc!=SQLITE_OK ){
				std::string error_description = "SQL error: " + std::string(zErrMsg);
				sqlite3_free(zErrMsg);
				throw pSketcherException(error_description);
			}

			sqlite3_free(sql_undo_redo);
	
		}else{
			// this is the case where there is not a database
			free_ = free; 
		} // if(database_ != 0)
	} // if(free != free_)
}

// method for adding this object to the SQLite3 database
void IndependentDOF::AddToDatabase(sqlite3 *database)
{
	// set the database for this object, in the future this database will be updated whenever this object is updated
	database_ = database;

	DatabaseAddDelete(true);
}

void IndependentDOF::RemoveFromDatabase()
{
	if(database_ != 0)
        DatabaseAddDelete(false);
    else
        throw pSketcherException("Attempt to remove a IndependentDOF from the database that was never added to the database.");
}

void IndependentDOF::DatabaseAddDelete(bool add_to_database) // utility method called by AddToDatabase and DeleteFromDatabase since they both do similar things
{	
	string sql_do, sql_undo;
	
	// First, create the sql statements to undo and redo this operation
	stringstream temp_stream;
	temp_stream.precision(__DBL_DIG__);
	temp_stream << "BEGIN; "
                << "INSERT INTO " << SQL_independent_dof_database_table_name << " VALUES(" 
                << GetID() << ",'" << name_ << "'," 
				<< free_ << "," << value_ <<"); "
                << "INSERT INTO dof_list VALUES("
                << GetID() << ",'" << SQL_independent_dof_database_table_name << "'); "
                << "COMMIT; ";

	if(add_to_database)
		sql_do = temp_stream.str();
	else
		sql_undo = temp_stream.str();

	temp_stream.str(""); // clears the string stream

	temp_stream << "BEGIN; "
				<< "DELETE FROM dof_list WHERE id=" << GetID() 
				<< "; DELETE FROM " << SQL_independent_dof_database_table_name << " WHERE id=" << GetID() 
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
			rc = sqlite3_exec(database_, ("ROLLBACK;"+SQL_independent_dof_database_schema).c_str(), 0, 0, &zErrMsg);  // need to add ROLLBACK since previous transaction failed
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
}
