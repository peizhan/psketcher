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

#include "EdgeLoop2D.h"

using namespace std;

EdgeLoop2D::EdgeLoop2D(std::vector<Edge2DBasePointer> edge_list):
edge_list_(edge_list)
{
}

// method to check to see if the loop is valid
// each consecutive edge must have a coincident node and the first and last edge must have a coincident node
bool EdgeLoop2D::IsLoopValid()
{
	// check to insure that there is more than one edge
	if(edge_list_.size() < 2 )
		return false;
	
	// check to insure that there are no duplicate edges 
	for(unsigned int current_edge = 0; current_edge < edge_list_.size()-1; current_edge++)
		for(unsigned int other_edge = current_edge+1; other_edge < edge_list_.size(); other_edge++)
		{
			if(edge_list_[current_edge] == edge_list_[other_edge])
				return false;
		}

	// check to insure that each edge shares an endpoint with the next edge in the list
	for(unsigned int current_edge = 0; current_edge < edge_list_.size(); current_edge++)
	{
		unsigned int next_edge;

		if(current_edge < edge_list_.size()-1)
			next_edge = current_edge + 1;
		else
			next_edge = 0;  // this is the case of the last edge in the list, its next edge will be the first edge
		
		bool test1 = edge_list_[current_edge]->IsPointCoincident(Point1, edge_list_[next_edge],Point1);
		bool test2 = edge_list_[current_edge]->IsPointCoincident(Point1, edge_list_[next_edge],Point2);
		bool test3 = edge_list_[current_edge]->IsPointCoincident(Point2, edge_list_[next_edge],Point1);
		bool test4 = edge_list_[current_edge]->IsPointCoincident(Point2, edge_list_[next_edge],Point2);
		
		if( ! (test1 || test2 || test3 || test4))
			return false;
	}
	
	// no problems found
	return true;
}

// Add an edge to the edge loop
void EdgeLoop2D::AddEdge(Edge2DBasePointer new_edge)
{
	edge_list_.push_back(new_edge);
}
