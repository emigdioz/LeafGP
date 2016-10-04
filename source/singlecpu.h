/*****************************************************************************
 * singlecpu.h
 *
 * Created: 10/3/2016 2016 by emigdio
 *
 * Copyright 2016 emigdio. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef SINGLECPU_H
#define SINGLECPU_H

#include "GP.h"

class singleCPU: public GP
{
public:
  singleCPU(Params& );
  virtual ~singleCPU() {}
  void LoadPoints();
  void CalculateNDRanges() {}
  void PrintStrategy() const { std::cout << "Single core CPU"; }
  unsigned DeviceFission() {};
};

#endif // SINGLECPU_H
