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

#include <string>
#include <sstream>
#include <iostream>

#include "DOF.h"

using namespace std;

// Initialize private static variables for DOF and PrimitiveBase classes
unsigned DOF::next_id_number_ = 1;

DOF::DOF (bool free, bool dependent) :
id_number_(next_id_number_++),free_(free), dependent_(dependent),
database_(0),
delete_me_(false)
{
	// by default, name variable using id_number_
	stringstream variable_name;
	variable_name << "dof" << id_number_;

	name_ = variable_name.str();
}

DOF::DOF ( const char *name, bool free, bool dependent) :
id_number_(next_id_number_++),free_(free), dependent_(dependent),
database_(0),
delete_me_(false)
{
	name_ = name;
}

DOF::DOF (unsigned id, bool dependent) :
id_number_(id),free_(false), dependent_(dependent),
database_(0),
delete_me_(false)
{

}