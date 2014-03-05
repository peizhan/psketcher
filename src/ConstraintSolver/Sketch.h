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

#ifndef SketchH
#define SketchH

#include "pSketcherModel.h"

class Sketch : public pSketcherModel
{
	public:
		// constructor
		Sketch(VectorPointer normal, VectorPointer up, PointPointer base,PrimitiveBasePointer (*current_primitive_factory)(unsigned, pSketcherModel &) = pSketcherModel::PrimitiveFactory, ConstraintEquationBasePointer (*current_constraint_factory)(unsigned, pSketcherModel &) = pSketcherModel::ConstraintFactory);

		Sketch(const std::string &file_name,PrimitiveBasePointer (*current_primitive_factory)(unsigned, pSketcherModel &) = pSketcherModel::PrimitiveFactory, ConstraintEquationBasePointer (*current_constraint_factory)(unsigned, pSketcherModel &) = pSketcherModel::ConstraintFactory);
	
		void AddToDatabase();

		// Accessor methods
		SketchPlanePointer GetSketchPlane() {return sketch_plane_;}

		// methods for adding primitives to the sketch
		Point2DPointer AddPoint2D ( double s, double t, bool s_free, bool t_free);
		Arc2DPointer AddArc2D (double s_center, double t_center, double theta_1, double theta_2, double radius, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free);
        Circle2DPointer AddCircle2D (double s_center, double t_center, double radius, bool s_center_free, bool t_center_free, bool radius_free);
		Arc2DPointer AddArc2D (double s1, double t1, double s2, double t2, double s3, double t3, bool s_center_free, bool t_center_free, bool theta_1_free, bool theta_2_free, bool radius_free);
		Line2DPointer AddLine2D (const Point2DPointer point1, const Point2DPointer point2);
		DistancePoint2DPointer AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2, double distance);
		DistancePoint2DPointer AddDistancePoint2D(const Point2DPointer point1, const Point2DPointer point2);
        DistancePointLine2DPointer AddDistancePointLine2D(const Point2DPointer point, const Line2DPointer line);
		ParallelLine2DPointer AddParallelLine2D(const Line2DPointer line1, const Line2DPointer line2);
		HoriVertLine2DPointer AddHoriVertLine2D(const Line2DPointer line, bool vertical_constraint);
		AngleLine2DPointer AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, double angle, bool interior_angle);
		AngleLine2DPointer AddAngleLine2D(const Line2DPointer line1, const Line2DPointer line2, bool interior_angle);
		TangentEdge2DPointer AddTangentEdge2D(Edge2DBasePointer edge1, EdgePointNumber point_num_1, Edge2DBasePointer edge2, EdgePointNumber point_num_2);

	protected:
		SketchPlanePointer sketch_plane_;
};


#endif //SketchH


