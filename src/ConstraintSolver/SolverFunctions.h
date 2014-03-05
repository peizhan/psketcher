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

#ifndef SolverFunctionsH
#define SolverFunctionsH

#include <string>
#include <vector>
#include "SolverFunctionsBase.h"

SolverFunctionsBasePointer SolverFunctionsFactory(std::string solver_function_name, std::vector<DOFPointer> dof_list);


class distance_point_2d: public SolverFunctionsBase
{
    public:
        distance_point_2d(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t, DOFPointer distance);
        distance_point_2d(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "distance_point_2d";}
};

class angle_line_2d_interior: public SolverFunctionsBase
{
    public:
        angle_line_2d_interior(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t, DOFPointer angle);
        angle_line_2d_interior(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "angle_line_2d_interior";}
};

class angle_line_2d_exterior: public SolverFunctionsBase
{
    public:
        angle_line_2d_exterior(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t, DOFPointer angle);
        angle_line_2d_exterior(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "angle_line_2d_exterior";}
};

class tangent_edge_2d: public SolverFunctionsBase
{
    public:
        tangent_edge_2d(DOFPointer s1, DOFPointer t1, DOFPointer s2, DOFPointer t2);
        tangent_edge_2d(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "tangent_edge_2d";}
};

class parallel_line_2d: public SolverFunctionsBase
{
    public:
        parallel_line_2d(DOFPointer line1_point1s, DOFPointer line1_point1t, DOFPointer line1_point2s, DOFPointer line1_point2t, DOFPointer line2_point1s, DOFPointer line2_point1t, DOFPointer line2_point2s, DOFPointer line2_point2t);
        parallel_line_2d(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "parallel_line_2d";}
};

class arc2d_point_s: public SolverFunctionsBase
{
    public:
        arc2d_point_s(DOFPointer s_center, DOFPointer radius, DOFPointer theta);
        arc2d_point_s(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "arc2d_point_s";}
};

class arc2d_point_t: public SolverFunctionsBase
{
    public:
        arc2d_point_t(DOFPointer t_center, DOFPointer radius, DOFPointer theta);
        arc2d_point_t(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "arc2d_point_t";}
};

class arc2d_tangent_s: public SolverFunctionsBase
{
    public:
        arc2d_tangent_s(DOFPointer theta);
        arc2d_tangent_s(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "arc2d_tangent_s";}
};

class arc2d_tangent_t: public SolverFunctionsBase
{
    public:
        arc2d_tangent_t(DOFPointer theta);
        arc2d_tangent_t(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "arc2d_tangent_t";}
};

class point2d_tangent1_s: public SolverFunctionsBase
{
    public:
        point2d_tangent1_s(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t);
        point2d_tangent1_s(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "point2d_tangent1_s";}
};

class point2d_tangent1_t: public SolverFunctionsBase
{
    public:
        point2d_tangent1_t(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t);
        point2d_tangent1_t(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "point2d_tangent1_t";}
};

class point2d_tangent2_s: public SolverFunctionsBase
{
    public:
        point2d_tangent2_s(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t);
        point2d_tangent2_s(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "point2d_tangent2_s";}
};

class point2d_tangent2_t: public SolverFunctionsBase
{
    public:
        point2d_tangent2_t(DOFPointer point1s, DOFPointer point1t, DOFPointer point2s, DOFPointer point2t);
        point2d_tangent2_t(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "point2d_tangent2_t";}
};

class distance_point_line_2d: public SolverFunctionsBase
{
    public:
        distance_point_line_2d(DOFPointer point_s, DOFPointer point_t, DOFPointer line_point1s, DOFPointer line_point1t, DOFPointer line_point2s, DOFPointer line_point2t, DOFPointer distance);
        distance_point_line_2d(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "distance_point_line_2d";}
};

class hori_vert_2d: public SolverFunctionsBase
{
    public:
        hori_vert_2d(DOFPointer dof1, DOFPointer dof2);
        hori_vert_2d(std::vector<DOFPointer> dof_list);

        double GetValue() const;
        double GetValueSelf(const mmcMatrix &params) const;
        mmcMatrix GetGradientSelf(const mmcMatrix &params) const;
        std::string GetName() const {return "hori_vert_2d";}
};





#endif //SolverFunctionsH