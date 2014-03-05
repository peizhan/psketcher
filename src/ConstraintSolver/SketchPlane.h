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


#ifndef SketchPlaneH
#define SketchPlaneH

#include "PrimitiveBase.h"
#include "Vector.h"
#include "Point.h"

const std::string SQL_sketch_plane_database_table_name = "sketch_plane_list";

const std::string SQL_sketch_plane_database_schema = "CREATE TABLE " + SQL_sketch_plane_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, base_point INTEGER NOT NULL, normal_vector INTEGER NOT NULL, up_vector INTEGER NOT NULL, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(base_point) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(normal_vector) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(up_vector) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

// sketch plane class (includes up vector)
class SketchPlane : virtual public PrimitiveBase
{
	public:
		SketchPlane ( VectorPointer normal, VectorPointer up, PointPointer base);
		SketchPlane (unsigned id, pSketcherModel &psketcher_model); // Construct from database

		void Get3DLocation ( double s, double t, double & x, double & y, double & z);
		double GetSTLocation( double x, double y, double z, double &s, double &t);
		void GetABCD ( double & coef_a, double & coef_b, double & coef_c, double & coef_d);
		
		void ApplySelectionMask(SelectionMask mask);

		VectorPointer GetNormal() {return normal_;}
		VectorPointer GetUp() {return up_;}
		PointPointer GetBase() {return base_;}

		// method for adding this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddRemove(bool add_to_database); // Utility method used by AddToDatabase and RemoveFromDatabase
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

	private:
		VectorPointer normal_;
		VectorPointer up_;
		PointPointer base_;
};
typedef boost::shared_ptr<SketchPlane> SketchPlanePointer;


#endif //SketchPlaneH
