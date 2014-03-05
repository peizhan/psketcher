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

#include <cmath>

#include "SolverFunctions.h"

using namespace std;


SolverFunctionsBasePointer SolverFunctionsFactory(std::string solver_function_name, std::vector<DOFPointer> dof_list)
{
    SolverFunctionsBasePointer new_solver_function;

    if(solver_function_name == "distance_point_2d")
        new_solver_function.reset(new distance_point_2d(dof_list));
    else if (solver_function_name == "angle_line_2d_interior")
        new_solver_function.reset(new angle_line_2d_interior(dof_list));
    else if (solver_function_name == "angle_line_2d_exterior")
        new_solver_function.reset(new angle_line_2d_exterior(dof_list));
    else if (solver_function_name == "tangent_edge_2d")
        new_solver_function.reset(new tangent_edge_2d(dof_list));
    else if (solver_function_name == "parallel_line_2d")
        new_solver_function.reset(new parallel_line_2d(dof_list));
    else if (solver_function_name == "arc2d_point_s")
        new_solver_function.reset(new arc2d_point_s(dof_list));
    else if (solver_function_name == "arc2d_point_t")
        new_solver_function.reset(new arc2d_point_t(dof_list));
    else if (solver_function_name == "arc2d_tangent_s")
        new_solver_function.reset(new arc2d_tangent_s(dof_list));
    else if (solver_function_name == "arc2d_tangent_t")
        new_solver_function.reset(new arc2d_tangent_t(dof_list));
    else if (solver_function_name == "point2d_tangent1_s")
        new_solver_function.reset(new point2d_tangent1_s(dof_list));
    else if (solver_function_name == "point2d_tangent1_t")
        new_solver_function.reset(new point2d_tangent1_t(dof_list));
    else if (solver_function_name == "point2d_tangent2_s")
        new_solver_function.reset(new point2d_tangent2_s(dof_list));
    else if (solver_function_name == "point2d_tangent2_t")
        new_solver_function.reset(new point2d_tangent2_t(dof_list));
    else if (solver_function_name == "distance_point_line_2d")
        new_solver_function.reset(new distance_point_line_2d(dof_list));
    else if (solver_function_name == "hori_vert_2d")
        new_solver_function.reset(new hori_vert_2d(dof_list));
    else
        throw SolverFunctionsException("SolverFunctionsFactory: Requested solver function name not found.");

    return new_solver_function;
}


distance_point_2d::distance_point_2d(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t, DOFPointer distance)
{
    AddDOF(point1s);
    AddDOF(point1t);
    AddDOF(point2s);
    AddDOF(point2t);
    AddDOF(distance);
}

distance_point_2d::distance_point_2d(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 5)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction distance_point_2d did not contain exactly 5 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double distance_point_2d::GetValue() const
{
    double point1s = GetDOF(0)->GetValue();
    double point1t = GetDOF(1)->GetValue();
    double point2s = GetDOF(2)->GetValue();
    double point2t = GetDOF(3)->GetValue();
    double distance = GetDOF(4)->GetValue();

    return -distance + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

double distance_point_2d::GetValueSelf(const mmcMatrix &params) const
{
    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);
    double distance = params(4,0);

    return -distance + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

mmcMatrix distance_point_2d::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);
    double distance = params(4,0);

    result(0,0) = (point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(1,0) = (point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(2,0) = (point2s - point1s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(3,0) = (point2t - point1t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(4,0) = -1;

    return result;
}

angle_line_2d_interior::angle_line_2d_interior(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t, DOFPointer angle)
{
    AddDOF(line1_point1s);
    AddDOF(line1_point1t);
    AddDOF(line1_point2s);
    AddDOF(line1_point2t);
    AddDOF(line2_point1s);
    AddDOF(line2_point1t);
    AddDOF(line2_point2s);
    AddDOF(line2_point2t);
    AddDOF(angle);
}

angle_line_2d_interior::angle_line_2d_interior(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 9)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction angle_line_2d_interior did not contain exactly 9 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double angle_line_2d_interior::GetValue() const
{
    double line1_point1s = GetDOF(0)->GetValue();
    double line1_point1t = GetDOF(1)->GetValue();
    double line1_point2s = GetDOF(2)->GetValue();
    double line1_point2t = GetDOF(3)->GetValue();
    double line2_point1s = GetDOF(4)->GetValue();
    double line2_point1t = GetDOF(5)->GetValue();
    double line2_point2s = GetDOF(6)->GetValue();
    double line2_point2t = GetDOF(7)->GetValue();
    double angle = GetDOF(8)->GetValue();

    return -cos(angle) + ((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
}

double angle_line_2d_interior::GetValueSelf(const mmcMatrix &params) const
{
    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);
    double angle = params(8,0);

    return -cos(angle) + ((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
}

mmcMatrix angle_line_2d_interior::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);
    double angle = params(8,0);

    result(0,0) = (line2_point1s - line2_point2s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point2s - line1_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(1,0) = (line2_point1t - line2_point2t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point2t - line1_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(2,0) = (line2_point2s - line2_point1s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point1s - line1_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(3,0) = (line2_point2t - line2_point1t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point1t - line1_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(4,0) = (line1_point1s - line1_point2s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point2s - line2_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(5,0) = (line1_point1t - line1_point2t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point2t - line2_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(6,0) = (line1_point2s - line1_point1s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point1s - line2_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(7,0) = (line1_point2t - line1_point1t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point1t - line2_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(8,0) = sin(angle);

    return result;
}

angle_line_2d_exterior::angle_line_2d_exterior(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t, DOFPointer angle)
{
    AddDOF(line1_point1s);
    AddDOF(line1_point1t);
    AddDOF(line1_point2s);
    AddDOF(line1_point2t);
    AddDOF(line2_point1s);
    AddDOF(line2_point1t);
    AddDOF(line2_point2s);
    AddDOF(line2_point2t);
    AddDOF(angle);
}

angle_line_2d_exterior::angle_line_2d_exterior(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 9)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction angle_line_2d_exterior did not contain exactly 9 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double angle_line_2d_exterior::GetValue() const
{
    double line1_point1s = GetDOF(0)->GetValue();
    double line1_point1t = GetDOF(1)->GetValue();
    double line1_point2s = GetDOF(2)->GetValue();
    double line1_point2t = GetDOF(3)->GetValue();
    double line2_point1s = GetDOF(4)->GetValue();
    double line2_point1t = GetDOF(5)->GetValue();
    double line2_point2s = GetDOF(6)->GetValue();
    double line2_point2t = GetDOF(7)->GetValue();
    double angle = GetDOF(8)->GetValue();

    return ((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + cos(angle);
}

double angle_line_2d_exterior::GetValueSelf(const mmcMatrix &params) const
{
    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);
    double angle = params(8,0);

    return ((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + cos(angle);
}

mmcMatrix angle_line_2d_exterior::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);
    double angle = params(8,0);

    result(0,0) = (line2_point1s - line2_point2s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point2s - line1_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(1,0) = (line2_point1t - line2_point2t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point2t - line1_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(2,0) = (line2_point2s - line2_point1s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point1s - line1_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(3,0) = (line2_point2t - line2_point1t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line1_point1t - line1_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(3.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0)));
    result(4,0) = (line1_point1s - line1_point2s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point2s - line2_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(5,0) = (line1_point1t - line1_point2t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point2t - line2_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(6,0) = (line1_point2s - line1_point1s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point1s - line2_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(7,0) = (line1_point2t - line1_point1t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(1.0/2.0))) + (line2_point1t - line2_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),(1.0/2.0))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),(3.0/2.0)));
    result(8,0) = -sin(angle);

    return result;
}

tangent_edge_2d::tangent_edge_2d(DOFPointer s1, DOFPointer t1, DOFPointer s2, DOFPointer t2)
{
    AddDOF(s1);
    AddDOF(t1);
    AddDOF(s2);
    AddDOF(t2);
}

tangent_edge_2d::tangent_edge_2d(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 4)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction tangent_edge_2d did not contain exactly 4 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double tangent_edge_2d::GetValue() const
{
    double s1 = GetDOF(0)->GetValue();
    double t1 = GetDOF(1)->GetValue();
    double s2 = GetDOF(2)->GetValue();
    double t2 = GetDOF(3)->GetValue();

    return -1 + pow((s1*s2 + t1*t2),2);
}

double tangent_edge_2d::GetValueSelf(const mmcMatrix &params) const
{
    double s1 = params(0,0);
    double t1 = params(1,0);
    double s2 = params(2,0);
    double t2 = params(3,0);

    return -1 + pow((s1*s2 + t1*t2),2);
}

mmcMatrix tangent_edge_2d::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double s1 = params(0,0);
    double t1 = params(1,0);
    double s2 = params(2,0);
    double t2 = params(3,0);

    result(0,0) = 2*s2*(s1*s2 + t1*t2);
    result(1,0) = 2*t2*(s1*s2 + t1*t2);
    result(2,0) = 2*s1*(s1*s2 + t1*t2);
    result(3,0) = 2*t1*(s1*s2 + t1*t2);

    return result;
}

parallel_line_2d::parallel_line_2d(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t)
{
    AddDOF(line1_point1s);
    AddDOF(line1_point1t);
    AddDOF(line1_point2s);
    AddDOF(line1_point2t);
    AddDOF(line2_point1s);
    AddDOF(line2_point1t);
    AddDOF(line2_point2s);
    AddDOF(line2_point2t);
}

parallel_line_2d::parallel_line_2d(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 8)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction parallel_line_2d did not contain exactly 8 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double parallel_line_2d::GetValue() const
{
    double line1_point1s = GetDOF(0)->GetValue();
    double line1_point1t = GetDOF(1)->GetValue();
    double line1_point2s = GetDOF(2)->GetValue();
    double line1_point2t = GetDOF(3)->GetValue();
    double line2_point1s = GetDOF(4)->GetValue();
    double line2_point1t = GetDOF(5)->GetValue();
    double line2_point2s = GetDOF(6)->GetValue();
    double line2_point2t = GetDOF(7)->GetValue();

    return -1 + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
}

double parallel_line_2d::GetValueSelf(const mmcMatrix &params) const
{
    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);

    return -1 + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
}

mmcMatrix parallel_line_2d::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double line1_point1s = params(0,0);
    double line1_point1t = params(1,0);
    double line1_point2s = params(2,0);
    double line1_point2t = params(3,0);
    double line2_point1s = params(4,0);
    double line2_point1t = params(5,0);
    double line2_point2s = params(6,0);
    double line2_point2t = params(7,0);

    result(0,0) = (-2*line2_point2s + 2*line2_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line1_point1s + 2*line1_point2s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),2)*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
    result(1,0) = (-2*line2_point2t + 2*line2_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line1_point1t + 2*line1_point2t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),2)*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
    result(2,0) = (-2*line2_point1s + 2*line2_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line1_point2s + 2*line1_point1s)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),2)*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
    result(3,0) = (-2*line2_point1t + 2*line2_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line1_point2t + 2*line1_point1t)/(pow((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2)),2)*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)));
    result(4,0) = (-2*line1_point2s + 2*line1_point1s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line2_point1s + 2*line2_point2s)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),2));
    result(5,0) = (-2*line1_point2t + 2*line1_point1t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line2_point1t + 2*line2_point2t)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),2));
    result(6,0) = (-2*line1_point1s + 2*line1_point2s)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line2_point2s + 2*line2_point1s)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),2));
    result(7,0) = (-2*line1_point1t + 2*line1_point2t)*((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t))/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*(pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2))) + pow(((line1_point1s - line1_point2s)*(line2_point1s - line2_point2s) + (line1_point1t - line1_point2t)*(line2_point1t - line2_point2t)),2)*(-2*line2_point2t + 2*line2_point1t)/((pow((line1_point1s - line1_point2s),2) + pow((line1_point1t - line1_point2t),2))*pow((pow((line2_point1s - line2_point2s),2) + pow((line2_point1t - line2_point2t),2)),2));

    return result;
}

arc2d_point_s::arc2d_point_s(DOFPointer s_center, DOFPointer radius, DOFPointer theta)
{
    AddDOF(s_center);
    AddDOF(radius);
    AddDOF(theta);
}

arc2d_point_s::arc2d_point_s(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 3)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction arc2d_point_s did not contain exactly 3 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double arc2d_point_s::GetValue() const
{
    double s_center = GetDOF(0)->GetValue();
    double radius = GetDOF(1)->GetValue();
    double theta = GetDOF(2)->GetValue();

    return s_center + radius*cos(theta);
}

double arc2d_point_s::GetValueSelf(const mmcMatrix &params) const
{
    double s_center = params(0,0);
    double radius = params(1,0);
    double theta = params(2,0);

    return s_center + radius*cos(theta);
}

mmcMatrix arc2d_point_s::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double s_center = params(0,0);
    double radius = params(1,0);
    double theta = params(2,0);

    result(0,0) = 1;
    result(1,0) = cos(theta);
    result(2,0) = -radius*sin(theta);

    return result;
}

arc2d_point_t::arc2d_point_t(DOFPointer t_center, DOFPointer radius, DOFPointer theta)
{
    AddDOF(t_center);
    AddDOF(radius);
    AddDOF(theta);
}

arc2d_point_t::arc2d_point_t(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 3)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction arc2d_point_t did not contain exactly 3 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double arc2d_point_t::GetValue() const
{
    double t_center = GetDOF(0)->GetValue();
    double radius = GetDOF(1)->GetValue();
    double theta = GetDOF(2)->GetValue();

    return t_center + radius*sin(theta);
}

double arc2d_point_t::GetValueSelf(const mmcMatrix &params) const
{
    double t_center = params(0,0);
    double radius = params(1,0);
    double theta = params(2,0);

    return t_center + radius*sin(theta);
}

mmcMatrix arc2d_point_t::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double t_center = params(0,0);
    double radius = params(1,0);
    double theta = params(2,0);

    result(0,0) = 1;
    result(1,0) = sin(theta);
    result(2,0) = radius*cos(theta);

    return result;
}

arc2d_tangent_s::arc2d_tangent_s(DOFPointer theta)
{
    AddDOF(theta);
}

arc2d_tangent_s::arc2d_tangent_s(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 1)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction arc2d_tangent_s did not contain exactly 1 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double arc2d_tangent_s::GetValue() const
{
    double theta = GetDOF(0)->GetValue();

    return sin(theta);
}

double arc2d_tangent_s::GetValueSelf(const mmcMatrix &params) const
{
    double theta = params(0,0);

    return sin(theta);
}

mmcMatrix arc2d_tangent_s::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double theta = params(0,0);

    result(0,0) = cos(theta);

    return result;
}

arc2d_tangent_t::arc2d_tangent_t(DOFPointer theta)
{
    AddDOF(theta);
}

arc2d_tangent_t::arc2d_tangent_t(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 1)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction arc2d_tangent_t did not contain exactly 1 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double arc2d_tangent_t::GetValue() const
{
    double theta = GetDOF(0)->GetValue();

    return -cos(theta);
}

double arc2d_tangent_t::GetValueSelf(const mmcMatrix &params) const
{
    double theta = params(0,0);

    return -cos(theta);
}

mmcMatrix arc2d_tangent_t::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double theta = params(0,0);

    result(0,0) = sin(theta);

    return result;
}

point2d_tangent1_s::point2d_tangent1_s(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t)
{
    AddDOF(point1s);
    AddDOF(point1t);
    AddDOF(point2s);
    AddDOF(point2t);
}

point2d_tangent1_s::point2d_tangent1_s(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 4)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction point2d_tangent1_s did not contain exactly 4 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double point2d_tangent1_s::GetValue() const
{
    double point1s = GetDOF(0)->GetValue();
    double point1t = GetDOF(1)->GetValue();
    double point2s = GetDOF(2)->GetValue();
    double point2t = GetDOF(3)->GetValue();

    return (point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

double point2d_tangent1_s::GetValueSelf(const mmcMatrix &params) const
{
    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    return (point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

mmcMatrix point2d_tangent1_s::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    result(0,0) = (point2s - point1s)*(point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(-1.0/2.0));
    result(1,0) = (point2t - point1t)*(point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(2,0) = pow((point1s - point2s),2)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) - 1/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(3,0) = (point1s - point2s)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));

    return result;
}

point2d_tangent1_t::point2d_tangent1_t(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t)
{
    AddDOF(point1s);
    AddDOF(point1t);
    AddDOF(point2s);
    AddDOF(point2t);
}

point2d_tangent1_t::point2d_tangent1_t(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 4)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction point2d_tangent1_t did not contain exactly 4 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double point2d_tangent1_t::GetValue() const
{
    double point1s = GetDOF(0)->GetValue();
    double point1t = GetDOF(1)->GetValue();
    double point2s = GetDOF(2)->GetValue();
    double point2t = GetDOF(3)->GetValue();

    return (point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

double point2d_tangent1_t::GetValueSelf(const mmcMatrix &params) const
{
    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    return (point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

mmcMatrix point2d_tangent1_t::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    result(0,0) = (point2s - point1s)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(1,0) = (point2t - point1t)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(-1.0/2.0));
    result(2,0) = (point1s - point2s)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(3,0) = pow((point1t - point2t),2)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) - 1/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));

    return result;
}

point2d_tangent2_s::point2d_tangent2_s(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t)
{
    AddDOF(point1s);
    AddDOF(point1t);
    AddDOF(point2s);
    AddDOF(point2t);
}

point2d_tangent2_s::point2d_tangent2_s(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 4)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction point2d_tangent2_s did not contain exactly 4 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double point2d_tangent2_s::GetValue() const
{
    double point1s = GetDOF(0)->GetValue();
    double point1t = GetDOF(1)->GetValue();
    double point2s = GetDOF(2)->GetValue();
    double point2t = GetDOF(3)->GetValue();

    return (point2s - point1s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

double point2d_tangent2_s::GetValueSelf(const mmcMatrix &params) const
{
    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    return (point2s - point1s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

mmcMatrix point2d_tangent2_s::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    result(0,0) = pow((point2s - point1s),2)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) - 1/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(1,0) = (point2s - point1s)*(point2t - point1t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(2,0) = (point2s - point1s)*(point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(-1.0/2.0));
    result(3,0) = (point2s - point1s)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));

    return result;
}

point2d_tangent2_t::point2d_tangent2_t(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t)
{
    AddDOF(point1s);
    AddDOF(point1t);
    AddDOF(point2s);
    AddDOF(point2t);
}

point2d_tangent2_t::point2d_tangent2_t(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 4)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction point2d_tangent2_t did not contain exactly 4 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double point2d_tangent2_t::GetValue() const
{
    double point1s = GetDOF(0)->GetValue();
    double point1t = GetDOF(1)->GetValue();
    double point2s = GetDOF(2)->GetValue();
    double point2t = GetDOF(3)->GetValue();

    return (point2t - point1t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

double point2d_tangent2_t::GetValueSelf(const mmcMatrix &params) const
{
    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    return (point2t - point1t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
}

mmcMatrix point2d_tangent2_t::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point1s = params(0,0);
    double point1t = params(1,0);
    double point2s = params(2,0);
    double point2t = params(3,0);

    result(0,0) = (point2s - point1s)*(point2t - point1t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(1,0) = pow((point2t - point1t),2)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) - 1/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(1.0/2.0));
    result(2,0) = (point2t - point1t)*(point1s - point2s)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0));
    result(3,0) = (point2t - point1t)*(point1t - point2t)/pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(3.0/2.0)) + pow((pow((point1s - point2s),2) + pow((point1t - point2t),2)),(-1.0/2.0));

    return result;
}

distance_point_line_2d::distance_point_line_2d(DOFPointer point_s, DOFPointer point_t, DOFPointer line_point1s, DOFPointer line_point1t, DOFPointer line_point2s, DOFPointer line_point2t, DOFPointer distance)
{
    AddDOF(point_s);
    AddDOF(point_t);
    AddDOF(line_point1s);
    AddDOF(line_point1t);
    AddDOF(line_point2s);
    AddDOF(line_point2t);
    AddDOF(distance);
}

distance_point_line_2d::distance_point_line_2d(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 7)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction distance_point_line_2d did not contain exactly 7 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double distance_point_line_2d::GetValue() const
{
    double point_s = GetDOF(0)->GetValue();
    double point_t = GetDOF(1)->GetValue();
    double line_point1s = GetDOF(2)->GetValue();
    double line_point1t = GetDOF(3)->GetValue();
    double line_point2s = GetDOF(4)->GetValue();
    double line_point2t = GetDOF(5)->GetValue();
    double distance = GetDOF(6)->GetValue();

    return -pow(distance,2) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2));
}

double distance_point_line_2d::GetValueSelf(const mmcMatrix &params) const
{
    double point_s = params(0,0);
    double point_t = params(1,0);
    double line_point1s = params(2,0);
    double line_point1t = params(3,0);
    double line_point2s = params(4,0);
    double line_point2t = params(5,0);
    double distance = params(6,0);

    return -pow(distance,2) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2));
}

mmcMatrix distance_point_line_2d::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double point_s = params(0,0);
    double point_t = params(1,0);
    double line_point1s = params(2,0);
    double line_point1t = params(3,0);
    double line_point2s = params(4,0);
    double line_point2t = params(5,0);
    double distance = params(6,0);

    result(0,0) = (-2*line_point1t + 2*line_point2t)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2));
    result(1,0) = (-2*line_point2s + 2*line_point1s)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2));
    result(2,0) = (-2*line_point2t + 2*point_t)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)*(-2*line_point1s + 2*line_point2s)/pow((pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)),2);
    result(3,0) = (-2*point_s + 2*line_point2s)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)*(-2*line_point1t + 2*line_point2t)/pow((pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)),2);
    result(4,0) = (-2*point_t + 2*line_point1t)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)*(-2*line_point2s + 2*line_point1s)/pow((pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)),2);
    result(5,0) = (-2*line_point1s + 2*point_s)*((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t))/(pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)) + pow(((line_point1t - point_t)*(line_point2s - line_point1s) - (line_point1s - point_s)*(line_point2t - line_point1t)),2)*(-2*line_point2t + 2*line_point1t)/pow((pow((line_point2s - line_point1s),2) + pow((line_point2t - line_point1t),2)),2);
    result(6,0) = -2*distance;

    return result;
}

hori_vert_2d::hori_vert_2d(DOFPointer dof1, DOFPointer dof2)
{
    AddDOF(dof1);
    AddDOF(dof2);
}

hori_vert_2d::hori_vert_2d(std::vector<DOFPointer> dof_list)
{
    // Check to make sure the correct number of parameters have been provided
    if(dof_list.size() != 2)
        throw SolverFunctionsException("The DOF vector for the constructor of SolverFunction hori_vert_2d did not contain exactly 2 DOF's");
    
    for(int i=0; i<dof_list.size(); i++)
        AddDOF(dof_list[i]);
}

double hori_vert_2d::GetValue() const
{
    double dof1 = GetDOF(0)->GetValue();
    double dof2 = GetDOF(1)->GetValue();

    return dof1 - dof2;
}

double hori_vert_2d::GetValueSelf(const mmcMatrix &params) const
{
    double dof1 = params(0,0);
    double dof2 = params(1,0);

    return dof1 - dof2;
}

mmcMatrix hori_vert_2d::GetGradientSelf(const mmcMatrix &params) const
{
    mmcMatrix result(GetNumDOFs(),1);

    double dof1 = params(0,0);
    double dof2 = params(1,0);

    result(0,0) = 1;
    result(1,0) = -1;

    return result;
}

