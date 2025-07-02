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
    d_dftfeWrapper = std::make_unique<dftfeWrapper>(parameter_file,
                                                    d_mpiCommParent,
                                                    true,
                                                    true,
                                                    "GS",
                                                    d_restartFilesPath,
                                                    d_verbosity,
                                                    useDevice);
													
	d_dftPtr       = d_dftfeWrapper->getDftfeBasePtr();
	d_dftParamsPtr = d_dftfeWrapper->getDftfeParamsPtr();
	
	populateMassKind();
	
	std::vector<std::vector<double>> d_mm_origin;
	dftUtils::readFile(3, d_mm_origin, "MM BOX ORIGIN"); // TODO: read filename from params
	mm_origin = d_mm_origin[0];
  }

  void
  mimicRunClass::populateMassKind(){
  	atomTypes = d_dftPtr->getAtomTypes();
	
	std::map<dftfe::uInt, dftfe::uInt> atomTypesMassesUnorder;
	std::map<dftfe::uInt, std::string> atomKindsUnorder;
	std::map<dftfe::uInt, dftfe::uInt> atomKindIDMap;
	
	dftUtils::readMassKindFile(atomTypesMassesUnorder, atomKindsUnorder, "ATOMIC MASSES FILE"); // TODO: read filename from params
	
	dftfe::uInt i = 1;
	
	// reorder according to atomTypes
	for (const auto& at : atomTypes) {
		atomMasses.push_back(atomTypesMassesUnorder[at]);
		atomKinds.push_back(atomKindsUnorder[at]);
		atomKindIDMap.insert({at, i});
		i += 1;
	}
	
	for (auto a : d_dftfeWrapper->getAtomicNumbers())
		atomKindIDs.push_back(atomKindIDMap[a]);
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
             mimic_Communicator.sendInt(MCL_GetProgramID());
        }
        else if (request == MCL_SEND_CLIENT_NAME)
        {
            mimic_Communicator.sendClientProgramName();
        }
        else if (request == MCL_SEND_CLIENT_RUNTYPE)
        {
             mimic_Communicator.sendInt(MCL_RUNTYPE_QM_PW);
        }
		else if (request == MCL_SEND_NUM_PARTICLES)
        {
            mimic_Communicator.sendInt(d_dftParamsPtr->natoms);
        }
		else if (request == MCL_SEND_NUM_PARTICLE_SPECIES)
        {
            mimic_Communicator.sendInt(d_dftParamsPtr->natomTypes);
        }
		else if (request == MCL_SEND_SPECIES_ELEMENTS)
        {
            mimic_Communicator.sendSet(atomTypes); //d_dftfeWrapper->getAtomicNumbers()
        }
		else if (request == MCL_SEND_SPECIES_MASSES)
        {
            mimic_Communicator.sendVec(atomMasses);
        }
		else if (request == MCL_SEND_SPECIES_LABELS)
        {
            mimic_Communicator.sendVec(atomKinds);
        }
		else if (request == MCL_SEND_NUM_CONSTR_BONDS)
        {
            mimic_Communicator.sendInt(0); // replace with num of constraints
        }
		else if (request == MCL_SEND_NUM_CONSTR_ANGLES)
        {
            mimic_Communicator.sendInt(0); // replace with num of constraints
        }
		else if (request == MCL_SEND_BOX_VECTORS)
        {
            mimic_Communicator.send2DVec(d_dftfeWrapper->getCell());
        }
		else if (request == MCL_SEND_BOX_NUM_GRIDPOINTS)
        {	
			std::vector<int> dummy_n_grid_pts;
	  	  	for (int i = 0; i < 3; i++)
	  	    	dummy_n_grid_pts.push_back(5);
            mimic_Communicator.sendVec(dummy_n_grid_pts); // TODO: dummy vec
        }
		else if (request == MCL_SEND_BOX_ORIGIN)
        {
            mimic_Communicator.sendVec(mm_origin); // TODO: check if this approach is correct
        }
		else if (request == MCL_SEND_BOX_GRIDPOINT_COORDS)
        {	
			std::vector<double> dummy_grid_pts;
	  	  	for (int i = 0; i < (3*5*5*5); i++)
	  	    	dummy_grid_pts.push_back(static_cast<double>(0));
            mimic_Communicator.sendVec(dummy_grid_pts); // TODO: dummy vec
        }
		else if (request == MCL_SEND_PARTICLE_SPECIES_IDS)
        {	
			 mimic_Communicator.sendVec(atomKindIDs);
        }
		else if (request == MCL_SEND_NUCLEAR_CHARGES)
        {
            mimic_Communicator.sendVec(atomKinds); // TODO: replace with actual value
        }
		else if (request == MCL_SEND_PARTICLE_POSITIONS)
        {
            mimic_Communicator.sendPos(d_dftPtr->getAtomLocationsCart(), mm_origin); // TODO: shift by mm_origin
        }
	}
    
  }

} // namespace dftfe
