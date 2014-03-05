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


#ifndef Point2DH
#define Point2DH

#include "Primitive2DBase.h"

const std::string SQL_point2d_database_table_name = "point2d_list";

const std::string SQL_point2d_database_schema = "CREATE TABLE " + SQL_point2d_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, sketch_plane INTEGER NOT NULL, s_dof INTEGER NOT NULL, t_dof INTEGER NOT NULL, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(sketch_plane) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(s_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(t_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

// Point2D class (a point constrained to a sketch plane)
class Point2D : public Primitive2DBase
{
	public:
		void ApplySelectionMask(SelectionMask mask);

		Point2D ( double s, double t, SketchPlanePointer sketch_plane, bool s_free = false, bool t_free = false);
		Point2D ( DOFPointer s, DOFPointer t, SketchPlanePointer sketch_plane);
		Point2D (unsigned id, pSketcherModel &psketcher_model); // Construct from database

		DOFPointer GetSDOF()const {return s_;}
		DOFPointer GetTDOF()const {return t_;}
		double GetSValue()const {return s_->GetValue();}
		double GetTValue()const {return t_->GetValue();}
		bool SIsFree() const {return s_->IsFree();}
		bool TIsFree() const {return t_->IsFree();}

		void SetSValue(double value, bool update_db = true){s_->SetValue(value,update_db);}
		void SetTValue(double value, bool update_db = true) {t_->SetValue(value,update_db);}
		void SetSFree(bool free) {s_->SetFree(free);}
		void SetTFree(bool free) {t_->SetFree(free);}

		void Get3DLocation(double & x_location, double & y_location, double & z_location)const;

		mmcMatrix GetmmcMatrix()const;

		// method for adding this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddRemove(bool add_to_database); // Utility method used by AddToDatabase and RemoveFromDatabase
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

		dimeEntity *GenerateDimeEntity() const;

	private:
		DOFPointer s_;
		DOFPointer t_;
};
typedef boost::shared_ptr<Point2D> Point2DPointer;


#endif //Point2DH
