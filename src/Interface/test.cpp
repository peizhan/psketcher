
#include <iostream>
#include "../ConstraintSolver/Sketch.h"

int main(int argc, char *argv[])
{
    // create the current Ark3D sketch
    VectorPointer normal( new Vector(0.0,0.0,1.0));
    VectorPointer up( new Vector(0.0,1.0,0.0));
    PointPointer base( new Point(0.0,0.0,0.0));
    Sketch *current_sketch = new Sketch(normal, up, base);

    for(int i = 0; i < 10; i++)
    { 
        Point2DPointer point1 = current_sketch->AddPoint2D(0.0,0.0,false,false);  // none of the dof's can vary
        Point2DPointer point2 = current_sketch->AddPoint2D(10.0,0.0,true,false);  // only x dof can vary
        Point2DPointer point3 = current_sketch->AddPoint2D(10.0,10.0,true,true);  // x and y dof's can vary
    
        Arc2DPointer arc1 = current_sketch->AddArc2D(1.5,6.0,(mmcPI/2.0)*.8,(mmcPI)*1.2,2.0,true,true,true,true,false);
    
        Line2DPointer line1 = current_sketch->AddLine2D(point1,point2);
        Line2DPointer line2 = current_sketch->AddLine2D(point2,point3);
        Line2DPointer line3 = current_sketch->AddLine2D(point3,arc1->GetPoint1());
        Line2DPointer line4 = current_sketch->AddLine2D(arc1->GetPoint2(),point1);
    
        // These 5 constraints will fully constrain the four free DOF's defined about
        ConstraintEquationBasePointer constraint1 = current_sketch->AddDistancePoint2D(point1,point2,6.0);
        ConstraintEquationBasePointer constraint2 = current_sketch->AddDistancePoint2D(point2,point3,12.0);
        ConstraintEquationBasePointer constraint3 = current_sketch->AddParallelLine2D(line1,line3);
        ConstraintEquationBasePointer constraint4 = current_sketch->AddParallelLine2D(line2,line4);
        ConstraintEquationBasePointer constraint5 = current_sketch->AddAngleLine2D(line1,line2,mmcPI/2.0,false);
        //ConstraintEquationBasePointer constraint8 = current_sketch->AddAngleLine2D(line2,line3,mmcPI/2.0,false);
    
        ConstraintEquationBasePointer constraint6 = current_sketch->AddTangentEdge2D(line3,Point2,arc1,Point1);
        ConstraintEquationBasePointer constraint7 = current_sketch->AddTangentEdge2D(line4,Point1,arc1,Point2);
    }
    
    std::cout << "Starting constraint solver..." << std::endl;
    current_sketch->SolveConstraints();
}