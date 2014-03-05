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

#ifndef Edge2DBaseH
#define Edge2DBaseH

#include <vector>

#include "Primitive2DBase.h"
#include "Point2D.h"

// Abstract base class for 2D edges
enum EdgePointNumber {Point1 = 1, Point2 = 2};
class Edge2DBase : public Primitive2DBase
{
	public:
		Edge2DBase(SketchPlanePointer sketch_plane);
		Edge2DBase();
		virtual ~Edge2DBase() {;}
		
		virtual void ApplySelectionMask(SelectionMask mask);

		// virtual methods that must be implemented by child classes
		virtual Point2DPointer GetPoint1() = 0;		// returns end point of edge (these may include dependentDOF's as is the case of the arc primitive)
		virtual Point2DPointer GetPoint2() = 0;
		virtual void GetTangent1(DOFPointer & s_component, DOFPointer & t_component) = 0;  // returns dependent dof's that define the tangent vector for each endpoint of the edge
		virtual void GetTangent2(DOFPointer & s_component, DOFPointer & t_component) = 0;

		virtual void GetTangent1(double & s_component, double & t_component) = 0;
		virtual void GetTangent2(double & s_component, double & t_component) = 0;

		// methods implemented by this base class
		bool IsPointCoincident(EdgePointNumber my_point_number, boost::shared_ptr<Edge2DBase> other_edge, EdgePointNumber other_point_number);
	protected:
};
typedef boost::shared_ptr<Edge2DBase> Edge2DBasePointer;


#endif //Edge2DBaseH
