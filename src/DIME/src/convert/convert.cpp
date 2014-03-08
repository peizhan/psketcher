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

#include <dime/convert/convert.h>
#include <dime/convert/layerdata.h>
#include "convert_funcs.h"

#include <dime/entities/Insert.h>
#include <dime/sections/HeaderSection.h>
#include <dime/Model.h>
#include <dime/State.h>
#include <dime/Layer.h>


/*!
  \class dxfConverter convert.h
  \brief The dxfConverter class offers a simple interface for dxf converting.
  It makes it possible to extract all geometry from dxf files, and store
  it in internal geometry sturctures, which again can be exported as 
  vrml.
*/


/*!
  \fn void dxfConverter::setNumSub(const int num)
  Sets the number of subdivisions for a circle or ellipse. 
  This overrides the value set in dxfConverter::setMaxerr() and 
  should normally not be used

  \sa dxfConverter::getNumSub()
*/

/*!
  \fn int dxfConverter::getNumSub() const
  Returns the numner of subdivisions set by dxfConverter::setNumSub()
*/

/*!
  \fn void dxfConverter::setMaxerr(const dxfdouble maxerr)
  Sets the maximum error allowed when converting circles, arcs
  and ellipses into lines/polygons.
*/

/*!
  \fn dxfdouble dxfConverter::getMaxerr() const
  Returns the maximum allowed error when converting circles, arcs
  ellipses.
*/
  
/*!
  \fn void dxfConverter::setFillmode(const bool fill)
  Sets whether polylines with width and SOLID and TRACE should be filled.
*/

/*!
  \fn bool dxfConverter::getFillmode() const
  Returns whether polylines with width and SOLID and TRACE should be filled.
*/

/*!
  \fn bool dxfConverter::getLayercol() const
  Returns whether only layers should be used (and not color index) when
  converting
*/
  
/*!
  \fn void dxfConverter::setLayercol(const bool v)
  Sets whether only layer (and not color index) should be used when converting.
  This method should normally no be used.
*/

/*!
  \fn int dxfConverter::getCurrentInsertColorIndex() const
  Returns the color index of the current INSERT entity. If no INSERT
  entity is current, the color index 7 (white) will be returned.
*/


/*!
  Constructor
 */
dxfConverter::dxfConverter()
{
  this->maxerr = 0.1f;
  this->numsub = -1;
  this->fillmode = true;
  this->layercol = false;
  this->currentInsertColorIndex =  7;
  this->currentPolyline = NULL;
  for (int i = 0; i < 255; i++) layerData[i] = NULL;
}

/*!
  Destructor
*/
dxfConverter::~dxfConverter()
{
  for (int i = 0; i < 255; i++) {
    delete layerData[i];
  }
}

/*!
  Returns a dxfLayerData instance for the color with color index \a colidx.
*/
dxfLayerData *
dxfConverter::getLayerData(const int colidx)
{
  assert(colidx >= 1 && colidx <= 255);
  if (layerData[colidx-1] == NULL) {
    layerData[colidx-1] = new dxfLayerData(colidx);
  }
  return layerData[colidx-1];
}

/*!
  Finds the color index for \a entity, and returns the dxfLayerData for it.
*/
dxfLayerData *
dxfConverter::getLayerData(const dimeEntity *entity)
{
  // special case for VERTEX
  if (this->currentPolyline && entity->typeId() == dimeBase::dimeVertexType) {
    if (!(entity->getEntityFlags() & FLAG_COLOR_NUMBER))
      return getLayerData(this->currentPolyline);
  }

  int colidx = getColorIndex(entity);
  if (colidx == 0) { // BYBLOCK
    colidx = this->currentInsertColorIndex;
  }
  // we don't care if layer is turned off (negative color)
  if (colidx < 0) colidx = -colidx;
    
  if (colidx < 1 || colidx > 255) { // just in case
    fprintf(stderr,"Illegal color number %d. Changed to 7 (white)\n",
	    colidx);
    colidx = 7;
  }
  return getLayerData(colidx);
}

/*!
  Returns a pointer to the dxfLayerData array.
*/
dxfLayerData **
dxfConverter::getLayerData()
{
  return layerData;
}


/*!
  Converts \a model to the internal geometry structures.
  \sa dxfConverter::writeWrl()
*/
bool 
dxfConverter::doConvert(dimeModel &model)
{  
  //
  // remove these 6 lines, and you may merge several dxf
  // files into a single vrml file by calling doConvert() several
  // times before calling writeVrml
  //
  for (int i = 0; i < 255; i++) {
    if (layerData[i]) {
      delete layerData[i];
      layerData[i] = NULL;
    }
  }

  return model.traverseEntities(dime_callback, this, 
				false, true, false);
}

/*!
  Writes the internal geometry structures to \a filename.
*/
bool
dxfConverter::writeVrml(const char * filename, const bool vrml1,
                        const bool only2d)
{
  FILE * f;
  if (fopen(filename, "wb")) {
    bool ret = this->writeVrml(f, vrml1, only2d);
    fclose(f);
    return ret;
  } else {
    return false;
  }
}

/*!
  Writes the internal geometry structures to \a out.
  Warning: This function is not CRT safe.
*/
bool
dxfConverter::writeVrml(FILE *out, const bool vrml1,
                        const bool only2d)
{
#ifndef NOWRLEXPORT
  //
  // write header
  //
  
  if (vrml1) {
    fprintf(out, 
            "#VRML V1.0 ascii\n\n");    
  }
  else {
    fprintf(out, 
            "#VRML V2.0 utf8\n\n");
  }

  //
  // write each used layer/color
  //
  for (int i = 0; i < 255; i++) {
    if (layerData[i] != NULL) {
      layerData[i]->writeWrl(out, 0, vrml1, only2d);
      delete layerData[i]; layerData[i] = NULL;
    }
  }
#endif // NOWRLEXPORT
  return true;
}

/*!
  Finds the correct color index for \a entity. Handles the BYLAYER case.
*/
int
dxfConverter::getColorIndex(const dimeEntity *entity)
{
  int colnum = entity->getColorNumber();
  if (this->layercol || colnum == 256) {
    const dimeLayer *layer = entity->getLayer();
    colnum = layer->getColorNumber();
  }
  return colnum;
}

//
// forward the call to the correct class instance
//
bool 
dxfConverter::dime_callback(const dimeState * const state, 
			    dimeEntity *entity, void *data)
{
  return ((dxfConverter*)data)->private_callback(state, entity);
}

//
// handles the callback from the dime-library
//
bool 
dxfConverter::private_callback(const dimeState * const state, 
			       dimeEntity *entity)
{ 
  if (entity->typeId() == dimeBase::dimePolylineType) {
    this->currentPolyline = entity;
  }

  if (state->getCurrentInsert()) {
    this->currentInsertColorIndex = 
      getColorIndex((dimeEntity*)state->getCurrentInsert());
  }
  else {
    this->currentInsertColorIndex = 7;
  }

  dxfLayerData *ld = getLayerData(entity);

  // fillmode on by default. entities which will not fill its polygons
  // should turn it off (layerData::addQuad() will create polygons,
  // not lines)
  //
  ld->setFillmode(true);
  
  switch (entity->typeId()) { 
  case dimeBase::dime3DFaceType:
    convert_3dface(entity, state, ld, this);
    break;
  case dimeBase::dimeSolidType:
    convert_solid(entity, state, ld, this);
    break;
  case dimeBase::dimeTraceType:
    convert_solid(entity, state, ld, this);
    break;
  case dimeBase::dimeArcType:
    convert_arc(entity, state, ld, this);
    break;
  case dimeBase::dimeCircleType:
    convert_circle(entity, state, ld, this);
    break;
  case dimeBase::dimeEllipseType:
    convert_ellipse(entity, state, ld, this);
    break;
  case dimeBase::dimeInsertType:
    // handled in traverseEntities
    break;
  case dimeBase::dimeBlockType:
    // handled in traverseEntities
    break;
  case dimeBase::dimeLineType:
    convert_line(entity, state, ld, this);
    break;
  case dimeBase::dimeLWPolylineType:
    convert_lwpolyline(entity, state, ld, this);
    break;
  case dimeBase::dimePointType:
    convert_point(entity, state, ld, this);
    break;
  case dimeBase::dimePolylineType:
    convert_polyline(entity, state, ld, this);
    break;
  case dimeBase::dimeSplineType:
    // go for it Raphael! :-)
    break;
  default:
    break;
  }
  return true;
}

/*!
  Finds the state of supported header variables in \a model. This
  method should be called before dxfxConverter::doConvert()
*/
void 
dxfConverter::findHeaderVariables(dimeModel &model)
{
  dimeHeaderSection *hs = (dimeHeaderSection*)
    model.findSection("HEADER");

  if (hs) {
    dimeParam param;
    int groupcode;

    if (hs->getVariable("$FILLMODE", &groupcode, &param, 1) == 1) {
      if (groupcode == 70)
	this->fillmode = (bool) param.int16_data;
    }
  }
}

