/*
 * mpiUtils.h
 *
 *  Created on: Nov 1, 2013
 *      Author: ro
 */

#ifndef MPIUTILS_H_
#define MPIUTILS_H_

#include "dataflow.h"

namespace mpiUtils
{
  extern SgProject* project;

  //! initializes the mpiUtils module
  void initMPIUtils(SgProject* project_arg);
  //!
  SgProject* getProject();


//=======================================================================================
  //!
  bool isMPIInit(const DataflowNode& n);
  //!
  bool isMPIInit(const SgNode* node);
  //!
  bool isMPICommRank(const SgNode* node);
  //!
  bool isMPICommSize(const SgNode* node);
}

#endif /* MPIUTILS_H_ */
