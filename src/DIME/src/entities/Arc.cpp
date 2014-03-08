/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

/*!
  \class dimeArc dime/entities/Arc.h
  \brief The dimeArc class handles an ARC \e entity.
*/

#include <dime/entities/Arc.h>
#include <dime/records/Record.h>
#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/util/MemHandler.h>
#include <dime/Model.h>
#include <math.h>

#ifdef _WIN32
#define M_PI 3.14159265357989
#endif

#ifdef macintosh
#define M_PI 3.14159265357989
#endif

static char entityName[] = "ARC";

// FIXME: should be configurable

#define ARC_NUMPTS 20 // num pts for a 2PI arc

/*!
  Constructor.
*/

dimeArc::dimeArc() 
  : center( 0, 0, 0 ), radius( 0.0 ), startAngle( 0.0 ), endAngle( 2*M_PI )
{
}

//!

dimeEntity *
dimeArc::copy(dimeModel * const model) const
{
  dimeArc *a = new(model->getMemHandler()) dimeArc;
  if (!a) return NULL;
  
  if (!this->copyRecords(a, model)) {
    // check if allocated on heap.
    if (!model->getMemHandler()) delete a;
    a = NULL;
  }
  else {
    a->center = this->center;
    a->radius = this->radius;
    a->startAngle = this->startAngle;
    a->endAngle = this->endAngle;
    a->copyExtrusionData(this);
  }
  return a;  
}

//!

bool 
dimeArc::write(dimeOutput * const file)
{
  this->preWrite(file);
  
  file->writeGroupCode(10);
  file->writeDouble(this->center[0]);
  file->writeGroupCode(20);
  file->writeDouble(this->center[1]);
  file->writeGroupCode(30);
  file->writeDouble(this->center[2]);
  
  file->writeGroupCode(40);
  file->writeDouble(this->radius);

  // For some reason a 73 record needs a new subclass record.
  file->writeGroupCode(100);
  file->writeString("AcDbArc");

  file->writeGroupCode(50);
  file->writeDouble(this->startAngle);
  file->writeGroupCode(51);
  file->writeDouble(this->endAngle);

  return this->writeExtrusionData(file) && dimeEntity::write(file);
}

//!

int 
dimeArc::typeId() const
{
  return dimeBase::dimeArcType;
}

//!

bool 
dimeArc::handleRecord(const int groupcode,
		     const dimeParam &param,
		     dimeMemHandler * const memhandler)
{
  switch(groupcode) {
  case 10:
  case 20:
  case 30:
    this->center[groupcode / 10 - 1] = param.double_data;
    return true;
  case 40:
    this->radius = param.double_data;
    return true;
  case 50:
    this->startAngle = param.double_data;
    return true;
  case 51:
    this->endAngle = param.double_data;
    return true;
  case 100:
    // Eat AcDbArc records, leave others.
    if( strcmp( param.string_data, "AcDbArc" ) == 0 ) {
      return true;
    }
    else {
      return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
    }
  }
  return dimeExtrusionEntity::handleRecord(groupcode, param, memhandler);
}

//!

const char *
dimeArc::getEntityName() const
{
  return entityName;
}

//!

bool 
dimeArc::getRecord(const int groupcode,
		  dimeParam &param,
		  const int index) const
{
  switch(groupcode) {
  case 10:
  case 20:
  case 30:
    param.double_data = this->center[groupcode / 10 - 1];
    return true;
  case 40:
    param.double_data = this->radius;
    return true;
  case 50:
    param.double_data = this->startAngle;
    return true;
  case 51:
    param.double_data = this->endAngle;
    return true;
  }
  return dimeExtrusionEntity::getRecord(groupcode, param, index);
}

//!

void
dimeArc::print() const
{
  fprintf(stderr,"ARC:\n");
  fprintf(stderr, " center: %.3f %.3f %.3f\n", 
	  center[0], center[1], center[2]);
  fprintf(stderr, " radius: %f\n", radius);
  fprintf(stderr, " start: %f, end: %f\n", startAngle, endAngle);
  fprintf(stderr, " extrusionDir: %f %f %f\n", 
	  extrusionDir[0], extrusionDir[1], extrusionDir[2]);
  fprintf(stderr, " thickness: %f\n", thickness);
}

//!

dimeEntity::GeometryType 
dimeArc::extractGeometry(dimeArray <dimeVec3f> &verts,
			dimeArray <int> &indices,
			dimeVec3f &extrusionDir,
			dxfdouble &thickness)
{

  verts.setCount(0);
  indices.setCount(0);

  thickness = this->thickness;
  extrusionDir = this->extrusionDir;

  // split the arc into lines
  
  double end = this->endAngle;
  if (end < this->startAngle) end += 360.0;
  
  double delta = DXFDEG2RAD(end - this->startAngle);
  if (delta == 0.0) {
#ifndef NDEBUG
    fprintf(stderr,"ARC with startAngle == endAngle!\n");
#endif
    end += 2*M_PI;
    //return dimeEntity::NONE;
  }
  
  // find the number of this ARC that fits inside 2PI
  int parts = (int) DXFABS((2*M_PI)/delta);
  
  // find # pts to use for arc
  // add one to avoid arcs with 0 line segments
  int numpts = ARC_NUMPTS / parts + 1;
  if (numpts > ARC_NUMPTS) numpts = ARC_NUMPTS;
  
  double inc = delta / numpts;
  double rad = DXFDEG2RAD(this->startAngle);
  int i;
  for (i = 0; i < numpts; i++) {
    verts.append(dimeVec3f(this->center.x + this->radius * cos(rad),
			   this->center.y + this->radius * sin(rad),
			   this->center.z));
    rad += inc;
  }
  rad = DXFDEG2RAD(end);
  
  verts.append(dimeVec3f(this->center.x + this->radius * cos(rad),
			 this->center.y + this->radius * sin(rad),
			 this->center.z));

  return dimeEntity::LINES;
}

//!

int
dimeArc::countRecords() const
{
  int cnt = 1 + 3 + 1 + 2; // header + center point + radius + angles
  
  return cnt + dimeExtrusionEntity::countRecords();
}

