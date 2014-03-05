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

#include <math.h>
#include "ConstraintSolver.h"

using namespace std;

// Constructor
ConstraintSolver::ConstraintSolver(const std::vector<SolverFunctionsBasePointer> &constraints, const std::vector<double> & weights, const std::vector<DOFPointer> & free_parameters, const std::vector<DOFPointer> & fixed_parameters, const std::vector<double> & fixed_values):
MeritFunction(free_parameters.size())
{
	if(constraints.size() < 1)
		throw MeritFunctionException();
	
	if(constraints.size() != weights.size())
		throw MeritFunctionException();
		
	if(free_parameters.size() < 1)
		throw MeritFunctionException();
		
	if(fixed_parameters.size() != fixed_values.size())
		throw MeritFunctionException(); 
	
	free_parameters_ = free_parameters;
	fixed_parameters_ = fixed_parameters;
	weights_ = weights;
	constraints_ = constraints;
	
    fixed_values_.SetSize(fixed_values.size(),1);
    for(int i=0; i < fixed_values.size(); i++)
        fixed_values_(i,0) = fixed_values[i];

    // Define the dof map, used by each solver function to map the global parameter list to their local parameter list.
    map<unsigned,unsigned> dof_map;
    unsigned location = 0;
    for(int i=0; i < free_parameters_.size(); i++)
    {
        dof_map.insert(pair<unsigned,unsigned>(free_parameters_[i]->GetID(),location++));
    }
    for(int i=0; i < fixed_parameters_.size(); i++)
    {
        dof_map.insert(pair<unsigned,unsigned>(fixed_parameters_[i]->GetID(),location++));
    }

    // let each constraint create its own transform from the global parameter list to their local parameter list
    // each constraint also takes care of any dependent DOFs that also neet to define a transform
    for(int i=0; i < constraints_.size(); i++)
        constraints_[i]->DefineInputMap(dof_map);

}

double ConstraintSolver::GetMeritValue(const mmcMatrix & x)
{
	double result = 0;

    mmcMatrix full_input_vector = x.CombineAsColumn(fixed_values_);

    for(int i=0; i < constraints_.size(); i++)
        result += weights_[i]*pow(constraints_[i]->GetValue(full_input_vector),2);

	return result;
}

mmcMatrix ConstraintSolver::GetMeritGradient(const mmcMatrix & x)
{
    mmcMatrix full_input_vector = x.CombineAsColumn(fixed_values_);

    mmcMatrix gradient(full_input_vector.GetNumRows(),1,0.0);

    for(int i=0; i < constraints_.size(); i++)
        gradient += weights_[i]*2.0*constraints_[i]->GetValue(full_input_vector)*constraints_[i]->GetGradient(full_input_vector);

    return gradient.GetSubMatrix(0,0,free_parameters_.size()-1,0);
}


