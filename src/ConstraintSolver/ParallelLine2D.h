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


#ifndef ParallelLine2DH
#define ParallelLine2DH

#include "ConstraintEquationBase.h"
#include "Line2D.h"

const std::string SQL_parallel_line2d_database_table_name = "parallel_line2d_list";

const std::string SQL_parallel_line2d_database_schema = "CREATE TABLE " + SQL_parallel_line2d_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, line1 INTEGER NOT NULL, line2 INTEGER NOT NULL, marker_position_dof INTEGER NOT NULL, weight FLOAT NOT NULL, FOREIGN KEY(id) REFERENCES constraint_equation_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(line1) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(line2) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(marker_position_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

class ParallelLine2D : public ConstraintEquationBase
{
	public:
		ParallelLine2D(const Line2DPointer line1, const Line2DPointer line2);
		ParallelLine2D(unsigned id, pSketcherModel &psketcher_model); // Construct from database

		double GetMarkerPosition() const {return marker_position_->GetValue();}
		void SetMarkerPosition(const double marker_position,bool update_db=true) {marker_position_->SetValue(marker_position,update_db);} // @fixme need to maker sure that the new marker position is in the set [0.0,1.0]

		Line2DPointer GetLine1() const {return line1_;}
		Line2DPointer GetLine2() const {return line2_;}
	
		// method for adding this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddRemove(bool add_to_database); // Utility method used by AddToDatabase and RemoveFromDatabase
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

	protected:
		Line2DPointer line1_;
		Line2DPointer line2_;

		DOFPointer marker_position_; // value from 0.0 to 1.0 that determines where the parallel marker will be displayed along each constrained line
};
typedef boost::shared_ptr<ParallelLine2D> ParallelLine2DPointer;


#endif //ParallelLine2DH
