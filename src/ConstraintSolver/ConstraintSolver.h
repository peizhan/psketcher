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

#ifndef ConstraintSolverH
#define ConstraintSolverH

#include <map>
#include "SolverFunctions.h"
#include "../mmcMatrix/mmcMatrix.h"
#include "../NumOptimization/bfgs.h"

/* Now will define the merit function derived class used in the template matching */
class ConstraintSolver : public MeritFunction
{
public:
	ConstraintSolver(const std::vector<SolverFunctionsBasePointer> &constraints, const std::vector<double> & weights, const std::vector<DOFPointer> & free_parameters, const std::vector<DOFPointer> & fixed_parameters, const std::vector<double> & fixed_values);
	virtual ~ConstraintSolver() {;}

	virtual double GetMeritValue(const mmcMatrix & x);
	virtual mmcMatrix GetMeritGradient(const mmcMatrix & x);

private:
	std::vector<DOFPointer> free_parameters_;
	std::vector<DOFPointer> fixed_parameters_;
	mmcMatrix fixed_values_;
	std::vector<double> weights_;
	std::vector<SolverFunctionsBasePointer> constraints_;
};

#endif //ConstraintSolverH

