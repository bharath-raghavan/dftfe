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
    , d_verbosity(verbosity)
  {
    init(parameter_file, useDevice);
  }

  void
  mimicRunClass::init(const std::string parameter_file,
                                  const bool        useDevice)
  {
    /*d_dftfeWrapper = std::make_unique<dftfeWrapper>(parameter_file,
                                                    d_mpiCommParent,
                                                    true,
                                                    true,
                                                    "GEOOPT",
                                                    d_restartFilesPath,
                                                    d_verbosity,
                                                    useDevice);
    d_dftPtr       = d_dftfeWrapper->getDftfeBasePtr();
    if (d_dftPtr->getParametersObject().optimizationMode == "ION")
      d_optMode = 0;
    else if (d_dftPtr->getParametersObject().optimizationMode == "CELL")
      d_optMode = 1;
    else if (d_dftPtr->getParametersObject().optimizationMode == "IONCELL")
      d_optMode = 2;
    if (dealii::Utilities::MPI::this_mpi_process(d_mpiCommParent) == 0)
      mkdir((d_restartFilesPath + "/optRestart").c_str(), ACCESSPERMS);
    std::vector<std::vector<double>> optData(2,
                                             std::vector<double>(1, 0.0));
    optData[0][0] = d_optMode;
    optData[1][0] = d_dftPtr->getParametersObject().periodicX ||
                        d_dftPtr->getParametersObject().periodicY ||
                        d_dftPtr->getParametersObject().periodicZ ?
                      1 :
                      0;
    if (!d_dftPtr->getParametersObject().reproducible_output)
      dftUtils::writeDataIntoFile(optData,
                                  d_restartFilesPath +
                                    "/optRestart/geometryOptimization.dat",
                                  d_mpiCommParent);
    d_cycle  = 0;
    d_status = d_optMode == 1 ? 1 : 0;
    d_geoOptIonPtr =
      std::make_unique<geoOptIon>(d_dftPtr, d_mpiCommParent, d_isRestart);
    d_geoOptCellPtr =
      std::make_unique<geoOptCell>(d_dftPtr, d_mpiCommParent, d_isRestart);*/
  }


  void
  mimicRunClass::runClient()
  {
    bool                             isLastStep = false;
	mimicCommunicator mimic_Communicator{};
	while (!isLastStep)
    {
        int request = -1;
        request =  mimic_Communicator.getRequest();
		std::cout << request << std::endl;
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
