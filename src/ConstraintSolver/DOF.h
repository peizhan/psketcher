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

#ifndef DOFH
#define DOFH

#include <string>
#include <boost/shared_ptr.hpp>
#include "../sqlite3/sqlite3.h"
class SolverFunctionsBase;
typedef boost::shared_ptr<SolverFunctionsBase> SolverFunctionsBasePointer;
class pSketcherModel;

// Abstract DOF base class
class DOF
{
	public:
		DOF (bool free, bool dependent);
		DOF (const char *name, bool free, bool dependent);
		DOF (unsigned id, bool dependent); // used when creating a DOF from the sqlite database

		virtual ~DOF() {;}

		//Accessor methods
		virtual void SetValue ( double value, bool update_db = true ) = 0;
		virtual double GetValue()const = 0;
		std::string GetName()const {return name_;}
        void SetName(std::string name) {name_ = name;}

		unsigned GetID()const {return id_number_;}
		void SetID(int id_number) {id_number_ = id_number;}
		static void SetNextID(int next_id) {next_id_number_ = next_id;}

        // Only used for DependentDOF's
        SolverFunctionsBasePointer GetSolverFunction() const {return solver_function_;}
        void SetSolverFunction(SolverFunctionsBasePointer solver_function) {solver_function_ = solver_function;}

		bool IsFree()const {return free_;}
		virtual void SetFree(bool free) {free_ = free;}

		bool IsDependent()const {return dependent_;}

		void FlagForDeletion() {delete_me_ = true;}
		void UnflagForDeletion() {delete_me_ = false;}
		bool IsFlaggedForDeletion() const {return delete_me_;}

		// method for adding this object to the SQLite3 database, needs to be implement by each child class
		virtual void AddToDatabase(sqlite3 *database) = 0;
		virtual void RemoveFromDatabase() = 0;

		// method to synchronize this object to the database, needs to be implemented by each child class
		// returns true on success, returns false if row does not exist in the database
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model) = 0;

	protected:
		// if not zero, this is the database where changes to the value of this DOF are stored
		sqlite3 *database_;

		std::string name_;
		bool free_;

	private:
		// deletion flag used when deleting primitives model
		bool delete_me_;

		bool dependent_;
		// static variable used to provide a unique ID number to each instance of this class
		static unsigned next_id_number_;

		// each instance of this class has a unique ID number
		unsigned id_number_;

        // Only used for DependentDOF's
        SolverFunctionsBasePointer solver_function_;
};
typedef boost::shared_ptr<DOF> DOFPointer;



#endif //DOFH
