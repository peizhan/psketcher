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


#ifndef Line2DH
#define Line2DH

#include "Edge2DBase.h"
#include "Point2D.h"

const std::string SQL_line2d_database_table_name = "line2d_list";

const std::string SQL_line2d_database_schema = "CREATE TABLE " + SQL_line2d_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, sketch_plane INTEGER NOT NULL, s1_dof INTEGER NOT NULL, t1_dof INTEGER NOT NULL, s2_dof INTEGER NOT NULL, t2_dof INTEGER NOT NULL, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(sketch_plane) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(s1_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(t1_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(s2_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(t2_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

// Line2D class
class Line2D : public Edge2DBase
{
	public:
		Line2D (const Point2DPointer point1, const Point2DPointer point2, SketchPlanePointer sketch_plane);
		Line2D (unsigned id, pSketcherModel &psketcher_model); // Construct from database

		DOFPointer GetS1()const {return s1_;}
		DOFPointer GetT1()const {return t1_;}

		DOFPointer GetS2()const {return s2_;}
		DOFPointer GetT2()const {return t2_;}

		void Get3DLocations(double & x1, double & y1, double & z1,
												double & x2, double & y2, double & z2)const ;

		Point2DPointer GetPoint1();
		Point2DPointer GetPoint2();

		void GetTangent1(DOFPointer & s_component, DOFPointer & t_component);  // returns a dependent dof that defines tangent vector for each endpoint of the edge
		void GetTangent2(DOFPointer & s_component, DOFPointer & t_component);

		void GetTangent1(double & s_component, double & t_component);
		void GetTangent2(double & s_component, double & t_component);

		void ApplySelectionMask(SelectionMask mask);

		// method for adding this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddRemove(bool add_to_database); // Utility method used by AddToDatabase and RemoveFromDatabase
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

		virtual dimeEntity *GenerateDimeEntity() const;

	private:
		DOFPointer s1_;
		DOFPointer t1_;

		DOFPointer s2_;
		DOFPointer t2_;

		// point1 and point2 do not need to be put into the database since they are created on the fly as needed
		Point2DPointer point1_;  // point1 and point2 depend on s1 through t2 and are included just so they don't have to be generated for every call to GetPoint*()
		Point2DPointer point2_;
};
typedef boost::shared_ptr<Line2D> Line2DPointer;


#endif //Line2DH
