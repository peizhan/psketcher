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
  \class dimeBlocksSection dime/sections/BlocksSection.h
  \brief The dimeBlocksSection class handles a BLOCKS \e section.
*/

#include <dime/sections/BlocksSection.h>
#include <dime/entities/Block.h>
#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/util/MemHandler.h>
#include <dime/Model.h>
#include <dime/util/Array.h>
#include <dime/Model.h>

static const char sectionName[] = "BLOCKS";

/*!
  Constructor which stores the sectioname.
*/

dimeBlocksSection::dimeBlocksSection(dimeMemHandler * const memhandler)
  : dimeSection(memhandler)
{
}

/*!
  Destructor. Should only be called when no memhandler is used.
*/

dimeBlocksSection::~dimeBlocksSection()
{
  if (!this->memHandler) {
    for (int i = 0; i < this->blocks.count(); i++)
      delete this->blocks[i];
  }
}

//!

dimeSection *
dimeBlocksSection::copy(dimeModel * const model) const
{
  dimeBlocksSection *bs = new dimeBlocksSection(model->getMemHandler());
  for (int i = 0; i < this->blocks.count(); i++) {
    bs->blocks.append((dimeBlock*)this->blocks[i]->copy(model));
  }
  return bs;
}

/*!
  This method reads a DXF BLOCKS section.
*/  

bool 
dimeBlocksSection::read(dimeInput * const file)
{
  int32 groupcode;
  const char *string;
  bool ok = true;
  dimeBlock *block = NULL;
  dimeMemHandler *memhandler = file->getMemHandler();

  while (true) {
    if (!file->readGroupCode(groupcode) || groupcode != 0) {
      fprintf( stderr, "Error reading groupcode: %d\n", groupcode);
      ok = false;
      break;
    }
    string = file->readString();
    if (!strcmp(string, "ENDSEC")) break;
    if (strcmp(string, "BLOCK")) {
      fprintf( stderr, "Unexpected string.\n");
      ok = false;
      break;
    }
    block = (dimeBlock*)dimeEntity::createEntity(string, memhandler);
    if (block == NULL) {
      fprintf(stderr, "error creating block: %s\n", string);
      ok = false;
      break;
    }
    if (!block->read(file)) {
      fprintf(stderr,"error reading block: %s.\n", string);
      ok = false;
      break;
    }
    this->blocks.append(block);
  }
  return ok;
}

/*!
  This method writes a DXF BLOCKS section.
*/

bool 
dimeBlocksSection::write(dimeOutput * const file)
{
  if (file->writeGroupCode(2) && file->writeString(sectionName)) {
    int i;
    for (i = 0; i < this->blocks.count(); i++) {
      if (!this->blocks[i]->write(file)) break;
    }
    if (i == this->blocks.count()) {
      return file->writeGroupCode(0) && file->writeString("ENDSEC");
    }
  }
  return false;
}

//!

int 
dimeBlocksSection::typeId() const
{
  return dimeBase::dimeBlocksSectionType;
}

/*!
  This function should be called after loading has ended, and will
  find all forward BLOCK references.
*/

void
dimeBlocksSection::fixReferences(dimeModel * const model)
{
  int i, n = this->blocks.count();
  for (i = 0; i < n; i++) {
    this->blocks[i]->fixReferences(model);
  }
}

//!

int
dimeBlocksSection::countRecords() const
{
  int cnt = 0;
  int i, n = this->blocks.count();
  for (i = 0; i < n; i++)
    cnt += this->blocks[i]->countRecords();
  return cnt + 2; // two records are written in write() 
}

//!

const char *
dimeBlocksSection::getSectionName() const
{
  return sectionName;
}

/*!
  Returns the number of blocks in this section. 
*/

int 
dimeBlocksSection::getNumBlocks() const
{
  return this->blocks.count();
}

/*!
  Returns the block at index \a idx.
*/

dimeBlock *
dimeBlocksSection::getBlock(const int idx)
{
  assert(idx >= 0 && idx < this->blocks.count());
  return this->blocks[idx];
}

/*!
  Removes (and deletes if no memhandler is used) the block at index \a idx.
*/

void 
dimeBlocksSection::removeBlock(const int idx)
{
  assert(idx >= 0 && idx < this->blocks.count());
  if (!this->memHandler) delete this->blocks[idx];
  this->blocks.removeElem(idx);
}

/*!
  Inserts a new block at index \a idx. If \a idx is negative, the
  block will be inserted at the end of the list of blocks.
*/

void 
dimeBlocksSection::insertBlock(dimeBlock * const block, const int idx)
{
  if (idx < 0) this->blocks.append(block);
  else {
    assert(idx <= this->blocks.count());
    this->blocks.insertElem(idx, block);
  }
}

