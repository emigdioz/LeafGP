// -----------------------------------------------------------------------------
// $Id$
//
//   Params.cc
// 
//   Genetic Programming in OpenCL (gpocl)
//
//   Copyright (C) 2010-2011 Douglas A. Augusto
// 
// This file is part of gpocl
// 
// GPOCL is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
// 
// GPOCL is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along
// with GPOCL; if not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------

#include "params.h"
#include "util.h"

using namespace std;

//---------------------------------------------------------------------
void
Params::ShowVersion() const // --version
{
   cout

   << "gpocl (GP in OpenCL) " << GPOCL_VERSION << " Copyright (C) 2010-2011\n"
   << "\n"
   << "This is free software. You may redistribute copies of it under the terms\n"
   << "of the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
   << "There is NO WARRANTY, to the extent permitted by law.\n"
   << "\n"
   << "Written by Douglas Adriano Augusto (daaugusto).\n";
}

//---------------------------------------------------------------------
void
Params::ShowUsage( const char* app = "gpocl" ) const // -h or --help
{
   cout  

   << "Genetic Programming in OpenCL (GPOCL)\n"
   << "\n"
   //<< "Usage: " << app << " [-cpu [n] | -gpu <fpi|fpc|ppcu|pppe>] [OPTION] <data points>\n"
   << "Usage: " << app << " [-cpu [n] | -gpu <fpi|ppcu|pppe>] [OPTION] <data points>\n"
   << "\n"    
   << "General options:\n"
   //<< "  -o file, --output-file file\n"
   //<< "     save result in 'file' [default = gpocl.out]\n"
   << "  -v, --verbose\n"
   << "     verbose\n"
   << "  --version\n"
   << "     print gpocl version and exit\n"
   << "  -h, --help\n"
   << "     print this help and exit\n"
   << "\n"
   << "Genetic Programming options:\n"
   << "  -p <p1,...,pn>, --primitives <p1,...,pn>\n"
   << "     GP primitives (operators/operands) [default = +,-,*,/,neg,ephemeral]\n"
   << "  -pp, --print-primitives\n"
   << "     Print all available GP primitives\n"
   << "  -g <n>, --generations <n>\n"
   << "     number of generations, n>0 [default = 1000]\n"
   << "  -s <n>, --seed <n>\n"
   << "     GP initialization seed, n>=0 [default = 0, random]\n"
   << "  -ps <n>, --population-size <n>\n"
   << "     number of individuals [default = 1024]\n"
   << "  -cp <f>, --crossover-probability <f>\n"
   << "     crossover probability, 0.0<=f<=1.0 [default = 0.95]\n"
   << "  -mp <f>, --mutation-probability <f>\n"
   << "     mutation probability, 0.0<=f<=1.0 [default = 0.10]\n"
   << "  -sp <n>, --seletion-pressure <n>\n"
   << "     selection pressure (tournament size), n>=1 [default = 3]\n"
   //<< "  -es <n>, --elitism-size <n>\n"
  // << "     elitism size [default = 1]\n"
   << "  -es <n>, --elitism-size <n>\n"
   << "     elitism size, 0<=n<=1 [default = 1]\n"
   << "  -max <n>, --maximum-size <n>\n"
   << "     maximum program size [default = 20]\n"
   << "  -min <n>, --minimum-size <n>\n"
   << "     minimum program size [default = 1]\n"
   << "  -et <n>, --error-tolerance <f>\n"
   << "     tolerance of error (stop criterion) [default = none]\n"
   << "\n"
   << "OpenCL options:\n"
   << "  -cl-mls <n>, --cl-maximum-local-size <n>\n";
}

//----------------------------------------------------------------------
bool 
Params::Initialize()
{
  m_verbose = true;
  m_print_primitives = false;
  m_primitives = "sin,cos,tan,sqrt,exp,+,-,*,/,ephemeral";
  m_number_of_generations = 100;
  m_population_size = 100;
  m_crossover_probability = 0.9;
  m_mutation_probability = 0.1;
  m_maximum_tree_size = 100;
  m_minimum_tree_size = 1;
  m_tournament_size = 3;
  //m_device = DEVICE_GPU_FPI;
  //m_device = DEVICE_CPU;
  m_elitism_size = 1;
  m_error_tolerance = -1;
  m_max_local_size = 0;
  m_output_file = "gpocl.out";
  m_seed = 0;
  m_number_of_runs = 5;
   // ---------------
   return true;
}

//---------------------------------------------------------------------
