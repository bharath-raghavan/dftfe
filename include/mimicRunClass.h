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

#ifndef mimicRunClass_H_
#define mimicRunClass_H_
#include "constants.h"
#include "headers.h"
#include "dftBase.h"
#include "dftfeWrapper.h"

namespace dftfe
{
  class mimicRunClass
  {
  public:
    /**
     * @brief mimicRunClass constructor: copy data from dftparameters to the members of mimicRunClass
     *
     *
     *  @param[in] dftBase *_dftBasePtr pointer to base class of dftClass
     *  @param[in] mpi_comm_parent parent mpi communicator
     */
    mimicRunClass(const std::string parameter_file,
                              const std::string restartFilesPath,
                              const MPI_Comm   &mpi_comm_parent,
                              const dftfe::Int  verbosity,
                              const bool        useDevice);


    void
    init(const std::string parameter_file, const bool useDevice);

    /**
     * @brief runClient:
     *
     *
     */
    void
    runClient();

  private:
    // pointers to dft class and optimization classes
    std::unique_ptr<dftfeWrapper> d_dftfeWrapper;
    dftBase                      *d_dftPtr;
    dftParameters                *d_dftParamsPtr;
		
    const std::string d_restartFilesPath;
    const dftfe::Int  d_verbosity;
    // status parameters
    dftfe::Int d_status, d_cycle, d_optMode;
    // parallel communication objects
    const MPI_Comm d_mpiCommParent;

    // conditional stream object
    dealii::ConditionalOStream pcout;
  };
} // namespace dftfe
#endif
