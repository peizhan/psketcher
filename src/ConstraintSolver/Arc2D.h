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

#ifndef Arc2DH
#define Arc2DH

#include "Edge2DBase.h"

const std::string SQL_arc2d_database_table_name = "arc2d_list";

const std::string SQL_arc2d_database_schema = "CREATE TABLE " + SQL_arc2d_database_table_name + " (id INTEGER PRIMARY KEY, dof_table_name TEXT NOT NULL, primitive_table_name TEXT NOT NULL, sketch_plane INTEGER NOT NULL, center_point INTEGER NOT NULL, radius_dof INTEGER NOT NULL, s_center_dof INTEGER NOT NULL, t_center_dof INTEGER NOT NULL, theta_1_dof INTEGER NOT NULL, theta_2_dof INTEGER NOT NULL, end1_point INTEGER NOT NULL, end2_point INTEGER NOT NULL, text_angle_dof INTEGER NOT NULL, text_radius_dof INTEGER NOT NULL, FOREIGN KEY(id) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(sketch_plane) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(center_point) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(radius_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(s_center_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(t_center_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(theta_1_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(theta_2_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(end1_point) REFERENCES primitive_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(end2_point) REFERENCES primitive_list(id), FOREIGN KEY(text_angle_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, FOREIGN KEY(text_radius_dof) REFERENCES dof_list(id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED);";

// Line2D class
class Arc2D : public Edge2DBase
{
	public:
		Arc2D (double s_center, double t_center, double theta_1, double theta_2, double radius, SketchPlanePointer sketch_plane,
               bool s_center_free = false, bool t_center_free = false, bool theta_1_free = false, bool theta_2_free = false, bool radius_free = false);
		Arc2D (double s1, double t1, double s2, double t2, double s3, double t3, SketchPlanePointer sketch_plane,
               bool s_center_free = false, bool t_center_free = false, bool theta_1_free = false, bool theta_2_free = false, bool radius_free = false);
		Arc2D (DOFPointer s_center, DOFPointer t_center, DOFPointer theta_1, DOFPointer theta_2, DOFPointer radius, SketchPlanePointer sketch_plane);
		Arc2D (unsigned id, pSketcherModel &psketcher_model); // Construct from database

		DOFPointer GetSCenter()const {return s_center_;}
		DOFPointer GetTCenter()const {return t_center_;}

		double GetSCenterValue()const {return s_center_->GetValue();}
		double GetTCenterValue()const {return t_center_->GetValue();}

		DOFPointer GetTheta1()const {return theta_1_;}
		DOFPointer GetTheta2()const {return theta_2_;}

		double GetTheta1Value()const {return theta_1_->GetValue();}
		double GetTheta2Value()const {return theta_2_->GetValue();}

		DOFPointer GetRadius()const {return radius_;}
		double GetRadiusValue()const {return radius_->GetValue();}
		void SetRadiusValue(double radius) {radius_->SetValue(radius);}

		void Get3DLocations(double & x_center, double & y_center, double & z_center) const;

		Point2DPointer GetPoint1(){return point1_;}
		Point2DPointer GetPoint2(){return point2_;}
		Point2DPointer GetCenterPoint() {return center_point_;}

		Point2DPointer GeneratePoint1();
		Point2DPointer GeneratePoint2();
		Point2DPointer GenerateCenterPoint();

		void GetTangent1(DOFPointer & s_component, DOFPointer & t_component);  // returns a dependent that defines tangent vector for each endpoint of the edge
		void GetTangent2(DOFPointer & s_component, DOFPointer & t_component);

		void GetTangent1(double & s_component, double & t_component);
		void GetTangent2(double & s_component, double & t_component);

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

		DOFPointer theta_1_;	// start angle
		DOFPointer theta_2_;	// end angle

		DOFPointer radius_;

		// Points based on dependent DOF's that provide the location of the arc endpoints
		// It is important that these points be maintained here so that Edge2DCBase point coincident checks can be performed by comparing DOF pointers
		// If these points are generated for each call to GetPoint1() then, for every call to GetPoint*(), the pointers to the S and T DOF's will be unique
		Point2DPointer point1_;
		Point2DPointer point2_;
		Point2DPointer center_point_;

		DOFPointer text_radius_;
		DOFPointer text_angle_;
};
typedef boost::shared_ptr<Arc2D> Arc2DPointer;



#endif //Arc2DH
