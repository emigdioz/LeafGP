/*****************************************************************************
 * singlecpu.cpp
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
#include "singlecpu.h"

singleCPU::singleCPU(Params &p): GP(p, CL_DEVICE_TYPE_CPU)
{

}

void singleCPU::LoadPoints()
{
   std::vector<std::vector<cl_float> > tmp_X;
   GP::LoadPoints( tmp_X );

   // Allocate enough memory (linear) to hold the linear version
   m_X = new cl_float[ m_num_points * m_x_dim ];

   // Linearization
   unsigned pos = 0;
   for( unsigned i = 0; i < tmp_X.size(); ++i )
      for( unsigned j = 0; j < tmp_X[0].size(); ++j )
         m_X[pos++] = tmp_X[i][j];

}
