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


#ifndef QtSketchH
#define QtSketchH

#include <QGraphicsScene>

#include "../ConstraintSolver/Sketch.h"
#include "QtPrimitives.h"

//class QGraphicsScene;

class QtSketch : public Sketch
{
	public:
		// constructor
		QtSketch(QGraphicsScene *scene, VectorPointer normal, VectorPointer up, PointPointer base, bool grid_snap = false);
		QtSketch(QGraphicsScene *scene, const std::string &file_name, bool grid_snap = false);

		// accessor methods
		bool GetGridSnap() {return grid_snap_;}
		void SetGridSnap(bool grid_snap) {grid_snap_ = grid_snap;}

		// override some of the pSketcherModel methods
		void ClearSelected() {scene_->clearSelection(); pSketcherModel::ClearSelected();}
		virtual void AddConstraintEquation(const ConstraintEquationBasePointer &new_constraint_equation, bool update_database = true);
		virtual void AddPrimitive(const PrimitiveBasePointer &new_primitive, bool update_database = true);

		// methods for adding primitives to the sketch
		QtPoint2DPointer AddPoint2D ( double s, double t, bool s_free, bool t_free);
		QtArc2DPointer AddArc2D (double s_center, double t_center, double theta_1, double theta_2, double radius, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free);
		QtArc2DPointer AddArc2D (double s1, double t1, double s2, double t2, double s3, double t3, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free);
        QtCircle2DPointer AddCircle2D (double s_center, double t_center, double radius, bool s_center_free, bool t_center_free, bool radius_free);
        QtCircle2DPointer AddCircle2D (DOFPointer s_center, DOFPointer t_center, double radius, bool radius_free);
		QtLine2DPointer AddLine2D (const Point2DPointer point1, const Point2DPointer point2);
		QtDistancePoint2DPointer AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2, double distance);
		QtDistancePoint2DPointer AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2);
        QtDistancePointLine2DPointer AddDistancePointLine2D(const Point2DPointer point, const Line2DPointer line);
		QtParallelLine2DPointer AddParallelLine2D(const Line2DPointer line1, const Line2DPointer line2);
		QtHoriVertLine2DPointer AddHoriVertLine2D(const Line2DPointer line, bool vertical_constraint);
		QtAngleLine2DPointer AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, double angle, bool interior_angle);
		QtAngleLine2DPointer AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, bool interior_angle);
		QtTangentEdge2DPointer AddTangentEdge2D(Edge2DBasePointer edge1, EdgePointNumber point_num_1, Edge2DBasePointer edge2, EdgePointNumber point_num_2);

	private:
		// methods for generating objects directly from the database
		// These methods are private since the Fetch methods should be used to access the DOF's primitives and constraints and they will call these methods if necessary
		static PrimitiveBasePointer PrimitiveFactory(unsigned id, pSketcherModel &psketcher_model);
		static ConstraintEquationBasePointer ConstraintFactory(unsigned id, pSketcherModel &psketcher_model);

		QGraphicsScene *scene_;
		bool grid_snap_;
};
//typedef boost::shared_ptr<QtSketch> QtSketchPointer;
typedef QtSketch* QtSketchPointer;


#endif //QtSketchH
