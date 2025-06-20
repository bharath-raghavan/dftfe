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

#ifndef mimicCommunicator_H_
#define mimicCommunicator_H_
#include <constants.h>
#include <headers.h>
#include <dftBase.h>

namespace dftfe
{
	
	class mimicCommunicator
	{

	public:
	    /*! \brief
	     * Finish communications and disconnect from the server
	     */
	    void finalize();

	    int getRequest();

	    void sendInt(int value);
		
		void sendVec(std::vector<dftfe::Int> value);

	    void sendClientProgramName();
		
	};

} // namespace dftfe
#endif
