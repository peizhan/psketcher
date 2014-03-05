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

#ifndef DependentDOFH
#define DependentDOFH

#include <vector>

#include "DOF.h"
#include "SolverFunctionsBase.h"

const std::string SQL_dependent_dof_database_table_name = "dependent_dof_list";

const std::string SQL_dependent_dof_database_schema = "CREATE TABLE " + SQL_dependent_dof_database_table_name + " (id INTEGER PRIMARY KEY, variable_name TEXT NOT NULL, solver_function TEXT NOT NULL, source_dof_table_name TEXT NOT NULL);";

// DependentDOF class
class DependentDOF : public DOF
{
	public:
		DependentDOF ( SolverFunctionsBasePointer solver_function);
		DependentDOF ( const char *name, SolverFunctionsBasePointer solver_function);
		// the following constructor creates the DOF from the database stored in psketcher_model
		DependentDOF ( unsigned id, pSketcherModel &psketcher_model );
		
		//Accessor methods
		void SetValue ( double value, bool update_db = true ) { /* @fixme warn user about an attempt to modify a dependent DOF */;}
		void SetFree(bool free) { /* @fixme warn user about an attempt to modify a dependent DOF */;}
		double GetValue()const;
		const std::vector<DOFPointer> & GetDOFList() {return GetSolverFunction()->GetDOFList();}

		// methods for adding and removing this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddDelete(bool add_to_database); // utility method called by AddToDatabase and DeleteFromDatabase since they both do similar things

		// method to synchronize this object to the database
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

	private:

};
typedef boost::shared_ptr<DependentDOF> DependentDOFPointer;



#endif //DependentDOFH
