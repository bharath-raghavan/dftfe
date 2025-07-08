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
  mimicCommunicator::sendInt(dftfe::uInt value)
  {
      auto value_ = (int) value;
	  MCL_Send(&value_, 1, MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendVec(std::vector<int> value) {
      MCL_Send(value.data(), value.size(), MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendVec(std::vector<double> value) {
      MCL_Send(value.data(), value.size(), MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendVec(std::vector<dftfe::uInt> value) {
  	  std::vector<int> value_(value.begin(), value.end());
      MCL_Send(value_.data(), value_.size(), MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendVec(std::vector<dftfe::Int> value) {
  	  std::vector<int> value_(value.begin(), value.end());
      MCL_Send(value_.data(), value_.size(), MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendVec(std::vector<std::string> value) {
  	  std::string  value_;
      
	  for (auto v : value)
	          value_ += v + std::string(",");
	  value_.pop_back();
	  int length = value_.length();
	  MCL_Send(&length, 1, MCL_LENGTH, 0);
	  MCL_Send(value_.data(), length, MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::send2DVec(std::vector<std::vector<double>> value) {	  
	  std::vector<double> value_;
	  for (int i = 0; i < value.size(); i++)
	     for (int j = 0; j < value[i].size(); j++)
	         value_.push_back(static_cast<double>(value[i][j]));
	   
	  sendVec(value_);
  }
  
  void
  mimicCommunicator::sendSet(std::set<dftfe::uInt> value) {
	  std::vector<int> value_(value.begin(), value.end());
	  sendVec(value_);
  }  

  void
  mimicCommunicator::sendClientProgramName()
  {
      std::string client_name = "DFTFE";
      int length = client_name.length();
      MCL_Send(&length, 1, MCL_LENGTH, 0);
      MCL_Send(client_name.data(), length, MCL_DATA, 0);
  }
  
  void
  mimicCommunicator::sendPos(std::vector<std::vector<double>> atomLoc, std::vector<double> mm_origin) {	  
	  std::vector<double> pos;
	  for (int i = 0; i < atomLoc.size(); i++)
	     for (int j = 0; j < 3; j++)
	         pos.push_back(atomLoc[i][j+2] + mm_origin[j]);
	  
	  sendVec(pos);
  }
  
  void
  mimicCommunicator::getPos(const int natoms)
  {
      std::vector<double> coords(3 * natoms);
      MCL_Receive(coords.data(), 3 * natoms, MCL_DATA, 0);
	  std::cout << "Recv pos: " << natoms << std::endl;
      for (int j = 0; j < natoms; ++j)
      {
          std::cout << coords[j * 3] << " " << coords[j * 3 + 1] << " " << coords[j * 3 + 2] << std::endl;
      }
  }
  
  

} // namespace dftfe
