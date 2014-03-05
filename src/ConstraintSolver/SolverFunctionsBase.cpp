#include <sstream>

#include "SolverFunctionsBase.h"
#include "pSketcherModel.h"

using namespace std;

SolverFunctionsBase::~SolverFunctionsBase()
{
    dof_list_.clear();
}

void SolverFunctionsBase::AddDOF(DOFPointer new_pointer)
{
    dof_list_.push_back(new_pointer);
}

double SolverFunctionsBase::GetValue(const mmcMatrix &x) const
{
    mmcMatrix local_x = transform_*x;


    for(int i = 0; i < dof_list_.size(); i++)
    {
        if (dof_list_[i]->IsDependent())
        {
            local_x(i,0) = dof_list_[i]->GetSolverFunction()->GetValue(x);
        }
    }

    return GetValueSelf(local_x);
}

mmcMatrix SolverFunctionsBase::GetGradient(const mmcMatrix &x) const
{
    mmcMatrix local_x = transform_*x;
    mmcMatrix temp(dof_list_.size(),dof_list_.size());
    temp.SetIdentity();
    mmcMatrix jacobian = transform_.GetTranspose()*temp;

    for(int i = 0; i < dof_list_.size(); i++)
    {
        if (dof_list_[i]->IsDependent())
        {
            local_x(i,0) = dof_list_[i]->GetSolverFunction()->GetValue(x);
            jacobian.SetSubmatrix(0,i,dof_list_[i]->GetSolverFunction()->GetGradient(x));
        }
    }

    return jacobian*GetGradientSelf(local_x);
}

void SolverFunctionsBase::DefineInputMap(const std::map<unsigned,unsigned> &input_dof_map)
{
    mmcMatrix new_transform(dof_list_.size(),input_dof_map.size(),0.0);

    map<unsigned,unsigned>::const_iterator map_it;

    for(int i = 0; i < dof_list_.size(); i++)
    {
        if (!dof_list_[i]->IsDependent())
        {
            map_it = input_dof_map.find(dof_list_[i]->GetID());
            if(map_it != input_dof_map.end())
            {
                // dof found in map
                new_transform(i,map_it->second) = 1.0;
            } else {
                // dof not found in map, need to throw an exception
                stringstream error_message;
                error_message << "DOF with the ID " << dof_list_[i]->GetID() << " not found in input map while defining transform_ for a SolverFunctionsBase instance.";
                throw pSketcherException(error_message.str());
            }
        }
    }

    transform_ = new_transform;

    // let each dependent dof create its own transform_
    for(int i=0; i < dof_list_.size(); i++)
        if(dof_list_[i]->IsDependent())
            dof_list_[i]->GetSolverFunction()->DefineInputMap(input_dof_map);
}
