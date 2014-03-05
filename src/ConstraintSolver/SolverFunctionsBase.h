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


#ifndef SolverFunctionsBaseH
#define SolverFunctionsBaseH

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

#include "DOF.h"
#include "../mmcMatrix/mmcMatrix.h"

//Exception class
class SolverFunctionsException
{
    public:
        SolverFunctionsException() {std::cerr << "SolverFunctions exception thrown: No description available." << std::endl;}
        SolverFunctionsException(std::string error_description) {std::cerr << "SolverFunctions exception thrown: " << error_description << std::endl;}
};

// Abstract Solver Function base class
class SolverFunctionsBase
{
    public:
        virtual ~SolverFunctionsBase();

        // methods implemented by the base class
        void AddDOF(DOFPointer new_pointer);
        double GetValue(const mmcMatrix &x) const;
        mmcMatrix GetGradient(const mmcMatrix &x) const;
        void DefineInputMap(const std::map<unsigned,unsigned> &input_dof_map);
        DOFPointer GetDOF(unsigned index) const {return dof_list_[index];}
        unsigned GetNumDOFs() const {return dof_list_.size();}
        const std::vector<DOFPointer> & GetDOFList() const {return dof_list_;}

        // pure abstract methods
        virtual double GetValue() const = 0;
        virtual double GetValueSelf(const mmcMatrix &params) const = 0;
        virtual mmcMatrix GetGradientSelf(const mmcMatrix &params) const = 0;
        virtual std::string GetName() const = 0;

    private:
        std::vector<DOFPointer> dof_list_;
        mmcMatrix transform_;

};
typedef boost::shared_ptr<SolverFunctionsBase> SolverFunctionsBasePointer;

#endif //SolverFunctionsBaseH