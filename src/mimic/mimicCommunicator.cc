// ---------------------------------------------------------------------
//
// Copyright (c) 2017-2025 The Regents of the University of Michigan and DFT-FE
// authors.
//
// This file is part of the DFT-FE code.
//
// The DFT-FE code is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the DFT-FE distribution.
//
// ---------------------------------------------------------------------
//
// @author Bharath Raghavan
//

#if defined(DFTFE_WITH_MIMIC)
#  include <mcl.h>
#endif

#include <TypeConfig.h>
#include <constants.h>
#include <headers.h>
#include <dftBase.h>
#include <mimicCommunicator.h>
#include "dftfeWrapper.h"

#if !defined(DFTFE_WITH_MIMIC)
/// Called in case DFT-FE is compiled without MiMiC
static void MCL_Send(void*, int, int, int)
{
    // throw error
}
/// Called in case DFT-FE is compiled without MiMiC
static void MCL_Receive(void*, int, int, int)
{
    // throw error
}
/// Called in case DFT-FE is compiled without MiMiC
static void MCL_Finalize()
{
    // throw error
}
#endif

namespace dftfe
{

  void
  mimicCommunicator::finalize()
  {
     MCL_Finalize();
  }
  
  int
  mimicCommunicator::getRequest()
  {
      int request;
      MCL_Receive(&request, 1, MCL_REQUEST, 0);
      return request;
  }
  
  void
  mimicCommunicator::sendInt(int value)
  {
	  MCL_Send(&value, 1, MCL_DATA, 0);
  }

  void
  mimicCommunicator::sendClientProgramName()
  {
      std::string client_name = "DFTFE";
      int length = client_name.length();
      MCL_Send(&length, 1, MCL_LENGTH, 0);
      MCL_Send(client_name.data(), length, MCL_DATA, 0);
  }
  

} // namespace dftfe
