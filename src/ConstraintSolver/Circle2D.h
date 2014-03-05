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

#ifndef Circle2DH
#define Circle2DH

#include "Edge2DBase.h"

const std::string SQL_circle2d_database_table_name = "circle2d_list";

const std::string SQL_circle2d_database_schema = "CREATE TABLE " + SQL_circle2d_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, sketch_plane INTEGER NOT NULL, center_point INTEGER NOT NULL, radius_dof INTEGER NOT NULL, s_center_dof INTEGER NOT NULL, t_center_dof INTEGER NOT NULL, text_angle_dof INTEGER NOT NULL, text_radius_dof INTEGER NOT NULL, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(sketch_plane) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(center_point) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(radius_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(s_center_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(t_center_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(text_angle_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(text_radius_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

// Line2D class
class Circle2D : public Primitive2DBase
{
	public:
		Circle2D (double s_center, double t_center, double radius, SketchPlanePointer sketch_plane,
               bool s_center_free = false, bool t_center_free = false, bool radius_free = false);
		Circle2D (double s1, double t1, double s2, double t2, double s3, double t3, SketchPlanePointer sketch_plane,
               bool s_center_free = false, bool t_center_free = false, bool radius_free = false);
		Circle2D (DOFPointer s_center, DOFPointer t_center, DOFPointer radius, SketchPlanePointer sketch_plane);
		Circle2D (unsigned id, pSketcherModel &psketcher_model); // Construct from database

		DOFPointer GetSCenter()const {return s_center_;}
		DOFPointer GetTCenter()const {return t_center_;}

		double GetSCenterValue()const {return s_center_->GetValue();}
		double GetTCenterValue()const {return t_center_->GetValue();}

		DOFPointer GetRadius()const {return radius_;}
		double GetRadiusValue()const {return radius_->GetValue();}
		void SetRadiusValue(double radius, bool update_db=true) {radius_->SetValue(radius,update_db);}

		void Get3DLocations(double & x_center, double & y_center, double & z_center) const;

		Point2DPointer GetCenterPoint() {return center_point_;}

		Point2DPointer GenerateCenterPoint();

		void ApplySelectionMask(SelectionMask mask);

		void SetTextLocation(double text_radius, double text_angle) {text_radius_->SetValue(text_radius); text_angle_->SetValue(text_angle);}
		void SetSTTextLocation(double text_s_, double text_t_, bool update_db=true);
		void SetDefaultTextLocation();
		double GetTextRadius() const {return text_radius_->GetValue();}
		double GetTextAngle() const {return text_angle_->GetValue();}
	
		// method for adding this object to the SQLite3 database
		virtual void AddToDatabase(sqlite3 *database);
		virtual void RemoveFromDatabase();
		void DatabaseAddRemove(bool add_to_database); // Utility method used by AddToDatabase and RemoveFromDatabase
		virtual bool SyncToDatabase(pSketcherModel &psketcher_model);

		dimeEntity *GenerateDimeEntity() const; // used for DXF export

	protected:
		// parameters that define the arc
		DOFPointer s_center_;
		DOFPointer t_center_;

		DOFPointer radius_;

		Point2DPointer center_point_;

		DOFPointer text_radius_;
		DOFPointer text_angle_;
};
typedef boost::shared_ptr<Circle2D> Circle2DPointer;



#endif //Circle2DH
