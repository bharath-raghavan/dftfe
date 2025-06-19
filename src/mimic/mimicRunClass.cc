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

#include <dftUtils.h>
#include <fileReaders.h>
#include <geometryOptimizationClass.h>
#include <sys/stat.h>
#include <iostream>
#include "mimicCommunicator.h"
#include "mimicRunClass.h"


namespace dftfe
{
  mimicRunClass::mimicRunClass(
    const std::string parameter_file,
    const std::string restartFilesPath,
    const MPI_Comm   &mpi_comm_parent,
    const dftfe::Int  verbosity,
    const bool        useDevice)
    : d_mpiCommParent(mpi_comm_parent)
    , pcout(std::cout,
            (dealii::Utilities::MPI::this_mpi_process(mpi_comm_parent) == 0))
	, d_restartFilesPath(restartFilesPath)
    , d_verbosity(verbosity)
  {
    init(parameter_file, useDevice);
  }

  void
  mimicRunClass::init(const std::string parameter_file,
                                  const bool        useDevice)
  {
  	pcout << "Init started" << std::endl;
    d_dftfeWrapper = std::make_unique<dftfeWrapper>(parameter_file,
                                                    d_mpiCommParent,
                                                    true,
                                                    true,
                                                    "GS",
                                                    d_restartFilesPath,
                                                    d_verbosity,
                                                    useDevice);
	pcout << "Init done" << std::endl;
  }


  void
  mimicRunClass::runClient()
  {
    bool                             isLastStep = false;
	mimicCommunicator mimic_Communicator{};
	pcout << "Starting client" << std::endl;
	while (!isLastStep)
    {
        int request = -1;
        request =  mimic_Communicator.getRequest();
		MPI_Bcast(&request, sizeof(int), MPI_BYTE, 0, d_mpiCommParent);
		std::cout << "MiMiC command name: " << MCL_GetRequestName(request) << " and number: " << request << std::endl;
        if (request == MCL_EXIT)
        {
            isLastStep = true;
        }
        else if (request == MCL_SEND_CLIENT_ID)
        {
             mimic_Communicator.sendClientId();
        }
        else if (request == MCL_SEND_CLIENT_NAME)
        {
            mimic_Communicator.sendClientProgramName();
        }
        else if (request == MCL_SEND_CLIENT_RUNTYPE)
        {
             mimic_Communicator.sendClientRunType();
        }
	}
    
  }

} // namespace dftfe
