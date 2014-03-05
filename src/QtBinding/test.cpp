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


#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../QtBinding/QtPrimitives.h"
#include "../ConstraintSolver/pSketcherModel.h"
#include "../mmcMatrix/mmcMatrix.h"

using namespace std;

// test the constraint solver with the 2D point and line primitives
void constraint_solver_test_2(QGraphicsItem * parent)
{
	VectorPointer normal( new Vector(0.0,0.0,1.0));
	VectorPointer up( new Vector(0.0,1.0,0.0));
	PointPointer base( new Point(0.0,0.0,0.0));

	SketchPlanePointer my_sketch_plane(new SketchPlane(normal, up, base) );

	Point2DPointer point1(new QtPoint2D(ais_context,0.0,0.0,my_sketch_plane,false,false));  // none of the dof's can vary
	Point2DPointer point2(new QtPoint2D(ais_context,10.0,0.0,my_sketch_plane,true,false));  // only x dof can vary
	Point2DPointer point3(new QtPoint2D(ais_context,10.0,10.0,my_sketch_plane,true,true));  // x and y dof's can vary
	Point2DPointer point4(new QtPoint2D(ais_context,0.0,10.0,my_sketch_plane,true,true));  // x and y dof's can vary	

	Line2DPointer line1(new QtLine2D(ais_context,point1,point2,my_sketch_plane));
	Line2DPointer line2(new QtLine2D(ais_context,point2,point3,my_sketch_plane));
	Line2DPointer line3(new QtLine2D(ais_context,point3,point4,my_sketch_plane));
	Line2DPointer line4(new QtLine2D(ais_context,point4,point1,my_sketch_plane));

	// These 5 constraints will fully constrain the four free DOF's defined about
	ConstraintEquationBasePointer constraint1(new QtDistancePoint2D(ais_context,point1,point2,6.0));
	ConstraintEquationBasePointer constraint2(new QtDistancePoint2D(ais_context,point2,point3,12.0));
	ConstraintEquationBasePointer constraint3(new QtParallelLine2D(ais_context,line1,line3));
	ConstraintEquationBasePointer constraint4(new QtParallelLine2D(ais_context,line2,line4));
	ConstraintEquationBasePointer constraint5(new QtAngleLine2D(ais_context,line1,line2,mmcPI/2.0));

	// Create the ark 3d model object
	pSketcherModel my_model;
	
	// Add the primitives to the 3d model object
	my_model.AddPrimitive(point1);
	my_model.AddPrimitive(point2);
	my_model.AddPrimitive(point3);
	my_model.AddPrimitive(point4);
	my_model.AddPrimitive(line1);
	my_model.AddPrimitive(line2);
	my_model.AddPrimitive(line3);
	my_model.AddPrimitive(line4);

	// Add the constraints to the 3d model object
	my_model.AddConstraintEquation(constraint1);
	my_model.AddConstraintEquation(constraint2);
	my_model.AddConstraintEquation(constraint3);
	my_model.AddConstraintEquation(constraint4);
	my_model.AddConstraintEquation(constraint5);

	// check some of the DOF's before the solve step
	double x1, y1, z1;
	point1->Get3DLocation(x1, y1, z1);
	cout << "Point 1 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point2->Get3DLocation(x1, y1, z1);
	cout << "Point 2 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point3->Get3DLocation(x1, y1, z1);
	cout << "Point 3 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point4->Get3DLocation(x1, y1, z1);
	cout << "Point 4 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	
	// solve the constraint equations
	my_model.SolveConstraints();
	my_model.UpdateDisplay();

	// check some of the DOF's to see what happened during the solve step 
	point1->Get3DLocation(x1, y1, z1);
	cout << "Point 1 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point2->Get3DLocation(x1, y1, z1);
	cout << "Point 2 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point3->Get3DLocation(x1, y1, z1);
	cout << "Point 3 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
	point4->Get3DLocation(x1, y1, z1);
	cout << "Point 4 (x,y,z): (" << x1 << ", " << y1 << ", " << z1 << ")" << endl;
}
