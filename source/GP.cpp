// -----------------------------------------------------------------------------
// $Id$
//
//   GP.cc
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

#include "GP.h"
#include "util.h"
#include "random.h"

#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <limits>
#include <iomanip>
#include <ctime>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QVector>

using namespace std;

Params* GP::m_params = 0;

#ifdef USE_OPENCL
// -----------------------------------------------------------------------------
GP::GP(Params& p, cl_device_type device_type ): m_device_type( device_type ),
                                                 m_X( 0 ),
                                                 m_E( 0 ),
                                                 m_num_points( 0 ),
                                                 m_y_dim( 1 ), 
                                                 m_x_dim( 0 ),
#ifdef PROFILING
                                                 // Ensure that the performance counters are empty
                                                 m_kernel_time( 0 ),
                                                 m_launch_time( 0 ),
                                                 m_kernel_calls( 0 ),
                                                 m_node_evaluations( 0 ),
#endif
                                                 m_best_error( std::numeric_limits<cl_float>::max() )
#else
GP::GP(Params& p):                               m_X( 0 ),
                                                 m_E( 0 ),
                                                 m_num_points( 0 ),
                                                 m_y_dim( 1 ),
                                                 m_x_dim( 0 ),
                                                 m_best_error( std::numeric_limits<cl_float>::max() )
#endif

{
   m_params = &p;

   // Random seed
   Random::Seed( (p.m_seed == 0 ? time( NULL ) : p.m_seed) );

   // Create room for the best individual so far
   m_best_program = new cl_uint[MaximumProgramSize()];
   // Set its size as zero
   SetProgramSize( m_best_program, 0 );
}

// -----------------------------------------------------------------------------
void GP::Evolve()
{
  treeStruct thisTree;
  QString bestIndividual;
  QString anyIndividual;
  basicInfo currentInfo;
  std::vector<double> exp;
  std::vector<double> act;
  QVector<double> expQ;
  QVector<double> actQ;
  QVector<double> partialValues;
  math_stats mathTool;

  /*

      Pseudo-code for Evolve:

   1: Create (randomly) the initial population P
   2: Evaluate all individuals (programs) of P
   3: for generation ← 1 to NG do
      4: Copy the best (elitism) individuals of P to the temporary population Ptmp
      5: while |Ptmp| < |P| do
         6: Select and copy from P two fit individuals, p1 and p2
         7: if [probabilistically] crossover then
            8: Recombine p1 and p2, creating the children p1' and p2'
            9: p1 ← p1' ; p2 ← p2'
         10: end if
         11: if [probabilistically] mutation then
            12: Apply mutation operators in p1 and p2, generating p1' and p2'
            13: p1 ← p1' ; p2 ← p2'
         14: end if
         15: Insert p1 and p2 into Ptmp
      16: end while
      17: Evaluate all individuals (programs) of Ptmp
      18: P ← Ptmp; then discard Ptmp
   19: end for
   20: return the best individual so far
   */
  int progress_run;

  std::clock_t start = std::clock();

  for(int curr_run = 0;curr_run < m_params->m_number_of_runs;curr_run++)
  {

    // Kernel has to be loaded each run since data can change
    if( m_params->m_print_primitives ) { m_primitives.ShowAvailablePrimitives(); return; }
    randomlySplitData(original_input_data_matrix,m_params->m_trainingRatio);
    // copy training data to input_matrix
    input_data_matrix = training_data;
    LoadPoints();
    m_primitives.m_primitives.clear();
    m_primitives.Load( m_x_dim, m_params->m_maximum_tree_size, m_params->m_primitives );
    if(engineType != 2) {
        OpenCLInit();
        CalculateNDRanges();
      #ifndef NDEBUG
        std::cout << "NDRanges: [local: " << m_local_size << ", global: " << m_global_size << ", work-groups: " << m_global_size/m_local_size << "]\n";
      #endif
        CreateBuffers();
        BuildKernel();
        SetKernelArgs();
      #ifndef NDEBUG
        std::cout << "Total local memory/CU (bytes): " << m_device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()
                  << ", used by the kernel: " << m_kernel.getWorkGroupInfo<CL_KERNEL_LOCAL_MEM_SIZE>( m_device )
                 // [OCL 1.1 only] << ", private memory used by each work-item: " << m_kernel.getWorkGroupInfo<CL_KERNEL_PRIVATE_MEM_SIZE>( m_device )
                  << ", actual work-group size: " << m_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>( m_device )
                  << std::endl;
      #endif
    }

    m_E = new cl_float[ m_params->m_population_size ];
    cl_uint* pop_a = new cl_uint[ m_params->m_population_size * MaximumProgramSize() ];
    cl_uint* pop_b = new cl_uint[ m_params->m_population_size * MaximumProgramSize() ];

    cl_uint* cur_pop = pop_a;
    cl_uint* tmp_pop = pop_b;
    //cl_uint* cur_pop = new cl_uint[ m_params->m_population_size * MaximumProgramSize() ];
    //cl_uint* tmp_pop = new cl_uint[ m_params->m_population_size * MaximumProgramSize() ];

    // 1:
    //   std::cout << "\n[Gen 1 of " << m_params->m_number_of_generations << "]...\n";
    InitializePopulation( cur_pop );
    // 2:
    ///EvaluatePopulation( cur_pop, errors );
    EvaluatePopulation( cur_pop );

    // ---------
    std::cout << "[Gen " << 1 << " of " << m_params->m_number_of_generations  << "] (Error: " << std::setprecision(10) << m_best_error << ", size: " << ProgramSize( m_best_program ) << ")... ";
#ifdef PROFILING
    std::cout << std::setprecision(2) << std::fixed << "| GPop/s: " << m_node_evaluations / (m_kernel_time/1.0E9) << std::setprecision(4) << " | Node evals: " << m_node_evaluations << " | Avg. KET(ms): " << m_kernel_time / (m_kernel_calls * 1.0E6) << " | Avg. KLT(ms): " << m_launch_time / (m_kernel_calls * 1.0E6) << " | Acc. KET(s): " << m_kernel_time/1.0E+9 << " | Acc. KLT(s): " << m_launch_time/1.0E+9 << " | Kernel calls: " << m_kernel_calls;
#endif
    std::cout << " | ET(s): " << ( std::clock() - start ) / double(CLOCKS_PER_SEC) << std::endl;
    // ---------
    float avgSize;
	 float maxError = sqrt(m_best_error/m_num_points);
	 float testError;
	 float maxAvgSize;
	 float normalizedFitness;
	 float realFitness;
	 float avgFitnessTraining;
    // 3:
	 avgSize = 0.0f;
	 avgFitnessTraining = 0.0f;
	 partialValues.clear();

	 // Generation 1 population is always same
	 thisPop.trees.clear();
	 thisPop.id.clear();
	 thisPop.normFitness.clear();
	 thisPop.realFitness.clear();
	 thisPop.operatorT.clear();
	 thisPop.parents[0].clear();
	 thisPop.parents[1].clear();
	 thisPop.size.clear();
	 for(int ind = 0;ind < m_params->m_population_size;ind++)
	 {
			avgSize += ProgramSize(Program( cur_pop, ind ));
			thisPop.id.push_back(ind + 1);
			thisPop.operatorT.push_back(1); // Random operators
			realFitness = sqrt(m_E[ind]/m_num_points);
			normalizedFitness = 1.0/(1 + realFitness);
			thisPop.normFitness.push_back(normalizedFitness);
			thisPop.realFitness.push_back(realFitness);
			thisPop.size.push_back(ProgramSize(Program(cur_pop,ind)));
			thisPop.parents[0].push_back(0);
			thisPop.parents[1].push_back(0);
			convertProgramToTreeStruct(thisTree,Program(cur_pop,ind));
			convertProgramString(Program(cur_pop,ind),anyIndividual);
			thisTree.syntaxPrefix = anyIndividual;
			thisPop.trees.push_back(thisTree);
			partialValues.push_back(realFitness);
			avgFitnessTraining += realFitness;
	 }
	 thisPop.currentGen = 1;
	 thisPop.currentRun = curr_run;

	 avgSize /= m_params->m_population_size;
	 maxAvgSize = avgSize;

	 locallyEvaluateTrainingCompressed(m_best_program,act,exp);
	 input_data_matrix = testing_data;
	 testError = locallyEvaluateTesting(m_best_program);
	 input_data_matrix = training_data;
	 currentInfo.currentRun = curr_run;
	 currentInfo.maxError = maxError;
	 currentInfo.bestTrainError = sqrt(m_best_error/m_num_points);
	 currentInfo.bestTestError = testError;
	 currentInfo.minError = currentInfo.bestTrainError;
	 currentInfo.bestSize = ProgramSize(m_best_program);
	 currentInfo.avgSize = avgSize;
	 currentInfo.maxAvgSize = maxAvgSize;
	 currentInfo.currentGeneration = 1;
	 currentInfo.currentNodesExecutions = m_node_evaluations;
	 currentInfo.avgTrainError = avgFitnessTraining/m_params->m_population_size;
	 currentInfo.medianTrainError = mathTool.computeMedian(partialValues);
	 expQ = QVector<double>::fromStdVector(exp);
	 actQ = QVector<double>::fromStdVector(act);
	 currentInfo.actual = actQ;
	 currentInfo.expected = expQ;
	 emit GP_send_basic_info(currentInfo);
	 thisPop.bestNormalizedTrainingFitness = 1.0/(1 + currentInfo.bestTrainError);
	 thisPop.bestRealTrainingFitness = currentInfo.bestTrainError;
	 thisPop.bestSize = currentInfo.bestSize;
	 thisPop.bestNormalizedTestingFitness = 1.0/(1 + currentInfo.bestTestError);
	 thisPop.bestRealTestingFitness = currentInfo.bestTestError;
	 thisPop.avgSize = avgSize;
	 thisPop.actualOutput = actQ;
	 thisPop.expectedOutput = expQ;
	 emit GP_send_pop_info(thisPop);
	 lastPop = thisPop;

   for( unsigned gen = 2; gen <= m_params->m_number_of_generations; ++gen )
   {
     // 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16:
     ///Breed( cur_pop, tmp_pop, errors );
     Breed( cur_pop, tmp_pop );
     // 17:
     ///if( EvaluatePopulation( tmp_pop, errors ) ) break;
     if( EvaluatePopulation( tmp_pop ) ) break;

     // 18:
     std::swap( cur_pop, tmp_pop );

     // ---------
     std::cout << "[Gen " << gen << " of " << m_params->m_number_of_generations  << "] (Error: " << std::setprecision(10) << sqrt(m_best_error/m_num_points) << ", size: " << ProgramSize( m_best_program ) << ")... ";
#ifdef PROFILING
     std::cout << std::setprecision(2) << std::fixed << "| GPop/s: " << m_node_evaluations / (m_kernel_time/1.0E9) << std::setprecision(4) << " | Node evals: " << m_node_evaluations << " | Avg. KET(ms): " << m_kernel_time / (m_kernel_calls * 1.0E6) << " | Avg. KLT(ms): " << m_launch_time / (m_kernel_calls * 1.0E6) << " | Acc. KET(s): " << m_kernel_time/1.0E+9 << " | Acc. KLT(s): " << m_launch_time/1.0E+9 << " | Kernel calls: " << m_kernel_calls;
#endif
     std::cout << " | ET(s): " << ( std::clock() - start ) / double(CLOCKS_PER_SEC) << std::endl;
     // ---------
     avgSize = 0.0f;
     avgFitnessTraining = 0.0f;
     partialValues.clear();

     for(int ind = 0;ind < m_params->m_population_size;ind++)
       avgSize += ProgramSize(Program( cur_pop, ind ));

     avgSize /= m_params->m_population_size;
     if(avgSize > maxAvgSize) maxAvgSize = avgSize;
     //************* temporal ***************************************************************************
     convertProgramToTreeStruct(thisTree,m_best_program);
     convertProgramString(m_best_program,bestIndividual);
     thisTree.syntaxPrefix = bestIndividual;
     emit GP_send_single_tree(thisTree);

     progress_run = ((float)gen/m_params->m_number_of_generations) * 1000;
     emit GP_send_run_progress(progress_run,curr_run);
     locallyEvaluateTrainingCompressed(m_best_program,act,exp);
     input_data_matrix = testing_data;
     testError = locallyEvaluateTesting(m_best_program);
     input_data_matrix = training_data;
     currentInfo.currentRun = curr_run;
     currentInfo.maxError = maxError;
     currentInfo.bestTrainError = sqrt(m_best_error/m_num_points);
     currentInfo.bestTestError = testError;
     currentInfo.minError = currentInfo.bestTrainError;
     currentInfo.bestSize = ProgramSize(m_best_program);
     currentInfo.avgSize = avgSize;
     currentInfo.maxAvgSize = maxAvgSize;
     currentInfo.currentGeneration = gen;
     currentInfo.currentNodesExecutions = m_node_evaluations;

     expQ = QVector<double>::fromStdVector(exp);
     actQ = QVector<double>::fromStdVector(act);
     currentInfo.actual = actQ;
     currentInfo.expected = expQ;

     for(int ind = 0;ind < m_params->m_population_size;ind++)
     {
       thisPop.id[ind] = (m_params->m_population_size * (gen - 1)) + ind + 1;
       realFitness = sqrt(m_E[ind]/m_num_points);
       normalizedFitness = 1.0/(1 + realFitness);
       thisPop.normFitness[ind] = normalizedFitness;
       thisPop.realFitness[ind] = realFitness;
       thisPop.size[ind] = ProgramSize(Program(cur_pop,ind));
       convertProgramToTreeStruct(thisTree,Program(cur_pop,ind));
       convertProgramString(Program(cur_pop,ind),anyIndividual);
       thisTree.syntaxPrefix = anyIndividual;
       thisPop.trees[ind] = thisTree;
       avgFitnessTraining += realFitness;
       partialValues.push_back(realFitness);
     }
     currentInfo.avgTrainError = avgFitnessTraining/m_params->m_population_size;
     currentInfo.medianTrainError = mathTool.computeMedian(partialValues);
     emit GP_send_basic_info(currentInfo);

     thisPop.currentGen = gen;
     thisPop.currentRun = curr_run;
     thisPop.bestNormalizedTrainingFitness = 1.0/(1 + currentInfo.bestTrainError);
     thisPop.bestRealTrainingFitness = currentInfo.bestTrainError;
     thisPop.bestSize = currentInfo.bestSize;
     thisPop.bestNormalizedTestingFitness = 1.0/(1 + currentInfo.bestTestError);
     thisPop.bestRealTestingFitness = currentInfo.bestTestError;
     thisPop.avgSize = avgSize;
     thisPop.actualOutput = actQ;
     thisPop.expectedOutput = expQ;
     emit GP_send_pop_info(thisPop);
     lastPop = thisPop;

   } // 19
   // 20:
   std::cout << "\n> Best: [" << std::setprecision(16) << sqrt(m_best_error/m_num_points) << "]\t{"
             << ProgramSize( m_best_program ) << "}\t";
   PrintProgramPretty( m_best_program );
   std::cout << std::endl;

// ************ Test code for CMA-ES LS approach *****************************************

   //executeLS_CMAES(m_best_program);
   //executeLS_LM(m_best_program);


// ***************************************************************************************

   // Clean up
   for(int i = 0; i < (m_params->m_population_size * MaximumProgramSize());++i)
   {
     pop_a[i] = NULL;
     pop_b[i] = NULL;
   }
   delete[] pop_a;
   delete[] pop_b;
   SetProgramSize( m_best_program, 0 );
   m_best_error = std::numeric_limits<cl_float>::max();
  }
}

// -----------------------------------------------------------------------------
///void GP::Breed( cl_uint* old_pop, cl_uint* new_pop, const cl_float* errors )
void GP::Breed( cl_uint* old_pop, cl_uint* new_pop )
{
  int Parent1;
  int Parent2;
   // Elitism
   for( unsigned i = 0; i < m_params->m_elitism_size; ++i )
   {
      // FIXME: (use the vector of best individuals)
      Clone( m_best_program, Program( new_pop, i ) );

#ifdef PROFILING
      // Update the total number of nodes that are going to be evaluated (to be
      // used to calculate how many GPop/s we could achieve).
      m_node_evaluations += ProgramSize( new_pop, i ) * m_num_points;
      thisPop.parents[0][i] = lastPop.id.at(i);
      thisPop.parents[1][i] = 0;
      thisPop.operatorT[i] = 5; //Elistism
#endif
   }

//   // Tournament:
//   for( unsigned i = m_params->m_elitism_size; i < m_params->m_population_size; ++i )
//   {
//      // Genetic operations
//      if( Random::Probability( m_params->m_crossover_probability ) )
//         // Respectively: mom, dad, and child
//         Crossover( Program( old_pop, Tournament( old_pop ) ),
//                    Program( old_pop, Tournament( old_pop ) ),
//                    Program( new_pop, i ) );
//      else
//         Clone( Program( old_pop, Tournament( old_pop ) ), Program( new_pop, i ) );

//      // Every genetic operator have a chance to go wrong
//      if( Random::Probability( m_params->m_mutation_probability ) )
//      {
//         if( Random::Probability( 2.0/3.0 ) ) // 66%
//            NodeMutate( Program( new_pop, i ) ); // node mutate / neighbor mutate
//         else
//            SubTreeMutate( Program( new_pop, i ) );
//      }

   // Tournament:
   for( unsigned i = m_params->m_elitism_size; i < m_params->m_population_size; ++i )
   {
      // Genetic operations
      if( Random::Probability( m_params->m_crossover_probability ) )
      {
         // Respectively: mom, dad, and child
        Parent1 = Tournament( old_pop );
        Parent2 = Tournament( old_pop );
        Crossover( Program( old_pop, Parent1 ),
                    Program( old_pop, Parent2 ),
                    Program( new_pop, i ) );
        thisPop.parents[0][i] = lastPop.id.at(Parent1);
        thisPop.parents[1][i] = lastPop.id.at(Parent2);
        thisPop.operatorT[i] = 2;
      }
      else if ( Random::Probability( m_params->m_mutation_probability ) )
      {
         Clone( Program( old_pop, i ), Program( new_pop, i ) );
         if( Random::Probability( 2.0/3.0 ) ) // 66%
            NodeMutate( Program( new_pop, i ) ); // node mutate / neighbor mutate
         else
            SubTreeMutate( Program( new_pop, i ) );
         thisPop.parents[0][i] = lastPop.id.at(i);
         thisPop.parents[1][i] = 0;
         thisPop.operatorT[i] = 3;
      }
      else
      {
        Clone( Program( old_pop, i ), Program( new_pop, i ) );
        thisPop.parents[0][i] = lastPop.id.at(i);
        thisPop.parents[1][i] = 0;
        thisPop.operatorT[i] = 4;
      }


#ifdef PROFILING
      // Update the total number of nodes that are going to be evaluated (to be
      // used to calculate how many GPop/s we could achieve).
      m_node_evaluations += ProgramSize( new_pop, i ) * m_num_points;
#endif
   }
}

// -----------------------------------------------------------------------------
void GP::Crossover( const cl_uint* mom, const cl_uint* dad, cl_uint* child ) const
{
   assert( mom != NULL && dad != NULL && child != NULL && child != mom && child != dad );

   unsigned pt_mom;
   unsigned pt_dad;
   unsigned mom_subtree_size;
   unsigned dad_subtree_size;
   unsigned child_program_size;

   // Choose cut points (mom/dad) that don't go beyond the maximum tree size
   do
   {
      pt_mom = Random::Int( 1, ProgramSize( mom ) );
      pt_dad = Random::Int( 1, ProgramSize( dad ) );
      mom_subtree_size = TreeSize( mom + pt_mom );
      dad_subtree_size = TreeSize( dad + pt_dad );

      child_program_size = ProgramSize( mom ) - mom_subtree_size + dad_subtree_size;
   } while( child_program_size > MaximumTreeSize() || 
            child_program_size < MinimumTreeSize() ); // TODO: check how many loops it is
                                                      // performing here to satisfy the conditions.

   SetProgramSize( child, child_program_size );

   // Actual crossover
   unsigned i, j;
   for( i = 1; i < pt_mom; i++ )
   {
      *(child + i) = *(mom + i);
   }
   for( j = pt_dad; j < pt_dad + dad_subtree_size; j++ )
   {
      *(child + i) = *(dad + j);

      i++;
   }
   for( j = pt_mom + mom_subtree_size; j < ProgramSize( mom ) + 1; j++ )
   {
      *(child + i) = *(mom + j);

      i++;
   }

   assert( TreeSize( child + 1 ) == ProgramSize( child ) );
}

// -----------------------------------------------------------------------------
void GP::SubTreeMutate( cl_uint* program ) const
{
   assert( program != NULL );
   assert( ProgramSize( program ) <= MaximumTreeSize() );
   assert( ProgramSize( program ) >= MinimumTreeSize() );

   // Pos 0 is the program size; pos 1 is the first node and 'program size + 1'
   // is the last node.
   unsigned mutation_pt = Random::Int( 1, ProgramSize( program ) ); // [1, size] (inclusive)

   unsigned subtree_size = TreeSize( program + mutation_pt );
   unsigned new_subtree_size = Random::Int( 
           std::max( 1, int( MinimumTreeSize() - ( ProgramSize( program ) - subtree_size ) ) ), 
                             MaximumTreeSize() - ( ProgramSize( program ) - subtree_size ) );
  
   // Set the resulting tree size to the newly generated program
   SetProgramSize( program, ProgramSize( program ) + (new_subtree_size - subtree_size) );

   // Move the second fragment (if necessary)
   if( new_subtree_size != subtree_size )
   {
      if( new_subtree_size < subtree_size )
      {
         for( unsigned i = mutation_pt + new_subtree_size; i < ProgramSize( program ) + 1; ++i )
         {
            program[i] = program[i + (subtree_size - new_subtree_size)];
         }
      }
      else
      {
         for( unsigned i = ProgramSize( program ); i >= mutation_pt + new_subtree_size; --i )
         {
            program[i] = program[i - (new_subtree_size - subtree_size)];
         }
      }
   }

   // Actually create the random subtree starting at mutation_pt
   CreateLinearTree( program + mutation_pt, new_subtree_size );
}

// -----------------------------------------------------------------------------
/**
  This function mutates a terminal's value to one in its neighborhood. It can
  handle GPT_EPHEMERAL, GPT_CLASS, and GPT_VAR terminals.
 */
void GP::NeighborTerminalMutate( cl_uint& terminal ) const
{
   const cl_float precision = SCALE_FACTOR / (cl_float) COMPACT_RANGE;

   switch( INDEX( terminal ) )
   {
      case Primitives::GPT_EPHEMERAL:
         if( Random::Probability( 0.5 ) ) // same probability for increment/decrement
         {
            if( AS_FLOAT( terminal ) < COMPACT_RANGE - precision )
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_FLOAT( terminal ) + precision );
            else if( AS_FLOAT( terminal ) > precision )
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_FLOAT( terminal ) - precision );
         }
         else // the opposite
         {
            if( AS_FLOAT( terminal ) > precision )
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_FLOAT( terminal ) - precision );
            else if( AS_FLOAT( terminal ) < COMPACT_RANGE - precision )
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_FLOAT( terminal ) + precision );
         }
         break;

      case Primitives::GPT_CLASS:
         if( Random::Probability( 0.5 ) ) // same probability for increment/decrement
         {
            if( AS_INT( terminal ) < m_primitives.m_max_Y )
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) + 1 );
            else if( AS_INT( terminal ) > m_primitives.m_min_Y ) // handle one-class problem
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) - 1 );
         }
         else // the opposite
         {
            if( AS_INT( terminal ) > m_primitives.m_min_Y )
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) - 1 );
            else if( AS_INT( terminal ) < m_primitives.m_max_Y ) // handle one-class problem
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) + 1 );
         }
         break;
      case Primitives::GPT_VAR:
         if( Random::Probability( 0.5 ) ) // same probability for increment/decrement
         {
            if( AS_INT( terminal ) < m_x_dim - 1 )
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) + 1 );
            else if( AS_INT( terminal ) > 0 ) // handle one-dimensional problem
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) - 1 );
         }
         else // the opposite
         {
            if( AS_INT( terminal ) > 0 )
               // decrement a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) - 1 );
            else if( AS_INT( terminal ) < m_x_dim - 1 ) // handle one-dimensional problem
               // increment a little bit
               Primitives::RepackNodeValue( terminal, AS_INT( terminal ) + 1 );
         }
         break;
      default: // an unrecognized terminal (possibly a constant, lets pick another
               // terminal).
         terminal = m_primitives.RandomNode( 0, 0 );
   }
}

// -----------------------------------------------------------------------------
void GP::NodeMutate( cl_uint* program ) const
{
   assert( program != NULL );
   assert( ProgramSize( program ) <= MaximumTreeSize() );
   assert( ProgramSize( program ) >= MinimumTreeSize() );

   // Pos 0 is the program size; pos 1 is the first node and 'program size + 1'
   // is the last node.
   unsigned mutation_pt = Random::Int( 1, ProgramSize( program ) ); // [1, size] (inclusive)

   // Mutate the node by a random node of the same arity (remember, this is *node*
   // mutation!).
   if( ARITY( program[mutation_pt] ) == 0 && Random::Probability( 0.5 ) )
      NeighborTerminalMutate( program[mutation_pt] );
   else
      program[mutation_pt] = m_primitives.RandomNode( ARITY( program[mutation_pt] ),
                                                      ARITY( program[mutation_pt] ) );
}

// -----------------------------------------------------------------------------
#if 0
void GP::CopySubTreeMutate( const cl_uint* program_orig, cl_uint* program_dest ) const
{
#define CAN_CHANGE_ORIGINAL_SIZE 1

   assert( program_orig != NULL && program_dest != NULL && program_orig != program_dest );
   assert( ProgramSize( program_orig ) <= MaximumTreeSize() );

   // Pos 0 is the program size; pos 1 is the first node and 'program size + 1'
   // is the last node.
   unsigned mutation_pt = Random::Int( 1, ProgramSize( program_orig ) ); // [1, size] (inclusive)

   //                   (mutation point)
   //                          v
   // [ ]     [ ]     [ ]     [*]     [*]     [*]     [ ]    [ ]
   // |      first      |     | mutated subtree |     | second |

   // Copy the first fragment but not the size (for now).
   for( unsigned i = 1; i < mutation_pt; ++i )
      program_dest[i] = program_orig[i];

   // Create a new random subtree of same size of the original one and put it
   // in the corresponding place in program_dest
   unsigned subtree_size = TreeSize( &program_orig[mutation_pt] );
#ifdef CAN_CHANGE_ORIGINAL_SIZE
   unsigned new_subtree_size = Random::Int( 1, 
         MaximumTreeSize() - ( ProgramSize( program_orig ) - subtree_size ) );
#else
   unsigned new_subtree_size = subtree_size;
#endif

   CreateLinearTree( &program_dest[mutation_pt], new_subtree_size );

   // Continue to copy the second fragment
   for( unsigned i = mutation_pt + subtree_size; i < ProgramSize( program_orig ) + 1; ++i )
      program_dest[i + (new_subtree_size - subtree_size)] = program_orig[i];

   // Finally, set the resulting tree size to the newly generated program
   SetProgramSize( program_dest, ProgramSize( program_orig ) + 
                                (new_subtree_size - subtree_size ) );
}

// -----------------------------------------------------------------------------
void GP::CopyNodeMutate( const cl_uint* program_orig, cl_uint* program_dest ) const
{
   // Copy the size (CopyNodeMutate, differently from CopySubTreeMutate doesn't
   // change the actual size of the program)
   assert( program_orig != NULL && program_dest != NULL && program_orig != program_dest );
   assert( ProgramSize( program_orig ) <= MaximumTreeSize() );

   unsigned size = *program_orig;
   // Pos 0 is the program size; pos 1 is the first node and 'program size + 1'
   // is the last node.
   unsigned mutation_pt = Random::Int( 1, size ); // [1, size] (inclusive)

   //                      (mutation point)
   //                             v
   // [ ]     [ ]     [ ]        [*]         [ ]     [ ]     [ ]
   // |      first      |   | mutated pt |   | second          |

   // Copy the size (pos 0) and then the first fragment (until just before the mutation point) 
   for( unsigned i = 0; i < mutation_pt; ++i )
      program_dest[i] = program_orig[i];

   // Mutate the node by a random node of the same arity (remember, this is *node*
   // mutation!).
   program_dest[mutation_pt] = m_primitives.RandomNode( ARITY( program_orig[mutation_pt] ),
                                                          ARITY( program_orig[mutation_pt] ) );

   // Continue to copy the second fragment
   for( unsigned i = mutation_pt + 1; i < size + 1; ++i )
      program_dest[i] = program_orig[i];
}
#endif

// -----------------------------------------------------------------------------
void GP::Clone( const cl_uint* program_orig, cl_uint* program_dest ) const
{
   assert( program_orig != NULL && program_dest != NULL );
   assert( ProgramSize( program_orig ) <= MaximumTreeSize() );
   assert( ProgramSize( program_orig ) >= MinimumTreeSize() );

   // The size is the first element
   for( unsigned i = *program_orig + 1; i-- ; ) *program_dest++ = *program_orig++;
}

// -----------------------------------------------------------------------------
///bool GP::EvaluatePopulation( cl_uint* pop, cl_float* errors )
bool GP::EvaluatePopulation( const cl_uint* pop )
{
  if(engineType != 2) {
    KernelLaunch( pop );
    CalculateErrors( pop );
  }

   // We should stop the evolution if an error below the specified tolerance is found
   return (m_best_error <= m_params->m_error_tolerance);
}


// -----------------------------------------------------------------------------
void GP::CalculateErrors( const cl_uint* pop )
{
   // Read the errors
   m_queue.enqueueReadBuffer( m_buf_E, CL_TRUE, 0, m_params->m_population_size * 
                              sizeof(cl_float), m_E, NULL, NULL );

   // --- Error calculation -----------------
   for( unsigned p = 0; p < m_params->m_population_size; ++p )
      UpdateBestProgram( Program( pop, p ), m_E[p] );
      // TODO: Pick the best and fill the elitism vector (if any)
}

// -----------------------------------------------------------------------------
bool GP::KernelLaunch( const cl_uint* pop )
{
   // Write data to buffer (TODO: can we use mapbuffer here?)
   /* 
      What about creating two mapbuffers (to write directly to the device,
      using CL_MEM_ALLOC_HOST_PTR), one for each population (cur/tmp), and then
      passing them (alternately) when enqueueing the kernel? If mapbuffers mean
      that we can save a copy by writing directly to the device, then could we
      use efficiently this buffer in the host to access the populations, i.e.,
      does mapbuffer keep a copy (synchronized) in the host?
      */
   m_queue.enqueueWriteBuffer( m_buf_pop, CL_TRUE, 0, sizeof( cl_uint ) *
         ( m_params->m_population_size * MaximumProgramSize() ), pop, NULL, NULL);

#ifdef PROFILING
   cl::Event e_time;
#endif
   
   // ---------- begin kernel execution
   m_queue.enqueueNDRangeKernel( m_kernel, cl::NDRange(), 
                                 cl::NDRange( m_global_size ), cl::NDRange( m_local_size )
#ifdef PROFILING
                                 , NULL, &e_time
#endif
         );
   // ---------- end kernel execution

   // Wait until the kernel has finished
   m_queue.finish();

#ifdef PROFILING
   cl_ulong started, ended, enqueued;
   e_time.getProfilingInfo( CL_PROFILING_COMMAND_START, &started );
   e_time.getProfilingInfo( CL_PROFILING_COMMAND_END, &ended );
   e_time.getProfilingInfo( CL_PROFILING_COMMAND_QUEUED, &enqueued );

   ++m_kernel_calls;

   m_kernel_time += ended - started;
   m_launch_time += started - enqueued;
#endif
}

// -----------------------------------------------------------------------------
void GP::UpdateBestProgram( const cl_uint* program, cl_float error )
{
   if( error < m_best_error  || ( util::AlmostEqual( error, m_best_error ) && 
                                  ProgramSize( program ) < ProgramSize( m_best_program ) ) )
   {
      m_best_error = error; Clone( program, m_best_program );

      if( m_params->m_verbose )
      {
         std::cout << "\nEvolved: [" << std::setprecision(12) << m_best_error << "]\t{" 
            << ProgramSize( m_best_program ) << "}\t";
         PrintProgramPretty( m_best_program );
         std::cout << "\n--------------------------------------------------------------------------------\n";
      }
   }
}

// -----------------------------------------------------------------------------
///unsigned GP::Tournament( const cl_uint* pop, const cl_float* errors ) const
unsigned GP::Tournament( const cl_uint* pop ) const
{
   unsigned winner = Random::Int( 0, m_params->m_population_size - 1 );
   for( unsigned t = 1; t < m_params->m_tournament_size; ++t ) 
   {
      unsigned competitor = Random::Int( 0, m_params->m_population_size - 1 );
      if( m_E[competitor] < m_E[winner]
            || ( util::AlmostEqual( m_E[competitor], m_E[winner] ) &&
               ProgramSize( pop, competitor ) < ProgramSize( pop, winner ) ) )
      {
         winner = competitor;
      }
   }

   return winner;
}

// -----------------------------------------------------------------------------
void GP::OpenCLInit()
{
   std::vector<cl::Platform> platforms;
   std::vector<cl::Device> devices;

   /* Iterate over the available platforms and pick the list of compatible devices
      from the first platform that offers the device type we are querying. */
   cl::Platform::get( &platforms );
   for( int i = 0; i < platforms.size() && devices.empty(); i++ )
      try {
         platforms[i].getDevices( m_device_type, &devices );
      } catch( cl::Error ) { }

   //if( devices.empty() )
   //  qDebug()<<"Not a single compatible device found.";
      //throw Error( "Not a single compatible device found." );

   // TODO: Pick the best device from *all* platforms
   m_device = devices.front();
   m_context = cl::Context( devices );

   // Change m_device properties (number of cores) if necessary (CPU only);
   // also, return the actual number of compute units
   m_max_cu = DeviceFission();

   m_max_local_size = std::min( m_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>(),
                                m_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0] );

   // Check if the user gave us a desirable maximum local size. If his/her proposed
   // size is within the hardware limits lets pick it:
   if( m_params->m_max_local_size > 0 && m_params->m_max_local_size < m_max_local_size )
      m_max_local_size = m_params->m_max_local_size;

   std::cout << "\nDevice: " << m_device.getInfo<CL_DEVICE_NAME>() << 
     ", Compute units: " << m_max_cu << ", Local size: " << m_max_local_size <<  std::endl;

   // Check whether our Y array (expected values) cannot be stored in the
   // __constant address space
   if( m_num_points * sizeof(cl_float) > m_device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>() )
     m_compile_flags += " -D Y_DOES_NOT_FIT_IN_CONSTANT_BUFFER";

#ifdef PROFILING
   m_queue = cl::CommandQueue( m_context, m_device, CL_QUEUE_PROFILING_ENABLE );
#else
   m_queue = cl::CommandQueue( m_context, m_device, 0 );
#endif

}

// -----------------------------------------------------------------------------
void GP::CreateBuffers()
{
   CreateBufferDataPoints();
   CreateBufferErrors();
   CreateBufferPopulation();
}

// -----------------------------------------------------------------------------
void GP::CreateBufferDataPoints()
{
   //TODO: Optimize for CPU (USE_HOST_PTR?)

   // Buffer (memory on the device) of training points
   // TODO: I think m_X can be freed right after cl::Buffer returns. Check that!
#ifndef NDEBUG
   std::cout << "\nTrying to allocate " << sizeof( cl_float ) * m_num_points * m_x_dim << " bytes for the input values (X)\n";
#endif
   m_buf_X = cl::Buffer( m_context,
                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                         sizeof( cl_float ) * m_num_points * m_x_dim,
                         m_X );
#ifndef NDEBUG
   std::cout << "Trying to allocate " << sizeof( cl_float ) * m_num_points << " bytes for the expected output values (Y)\n";
#endif
   m_buf_Y = cl::Buffer( m_context,
                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                         sizeof( cl_float ) * m_num_points, &m_Y[0] );
}

// -----------------------------------------------------------------------------
void GP::CreateBufferErrors()
{
   // Buffer (memory on the device) of prediction errors (one per program)
#ifndef NDEBUG
   std::cout << "Trying to allocate " << sizeof( cl_float ) * m_params->m_population_size << " bytes for the prediction errors\n";
#endif
   m_buf_E = cl::Buffer( m_context, CL_MEM_WRITE_ONLY,
                         m_params->m_population_size * sizeof(cl_float) );
}

// -----------------------------------------------------------------------------
void GP::CreateBufferPopulation()
{
  /* 
   Structure of a program (individual)

     |                         |
+----+-----+----+--------------+-------------
|size|arity|type| index/value  |  ...
| 32 |  3  | 7  |     22       |
+----+-----+----+--------------+-------------
     |    first element        | second ...
  */
   // Buffer (memory on the device) of the population
#ifndef NDEBUG
   std::cout << "Trying to allocate " << sizeof( cl_uint ) * ( m_params->m_population_size * MaximumProgramSize() )  << " bytes for the population of programs\n";
#endif
   m_buf_pop = cl::Buffer( m_context, CL_MEM_READ_ONLY,
         sizeof( cl_uint ) * ( m_params->m_population_size * MaximumProgramSize() ) );
}

// -----------------------------------------------------------------------------
void GP::BuildKernel()
{
   std::vector<bool> already_added( m_primitives.m_primitives.size(), false );

   /* To avoid redundant switch cases in the kernel, we will add only those cases clauses 
      that correspond to the subset of primitives given by the user. */
   std::string interpreter = "#define INTERPRETER_CORE" + (m_primitives.m_need_identity ? 
            " case " + util::ToString( (unsigned) Primitives::GPF_IDENTITY ) + ": PUSH_1(ARG(0)) break;" : " ");

   for( unsigned i = 0; i < m_primitives.m_primitives.size(); ++i )
      if( INDEX( m_primitives.m_primitives[i] ) != Primitives::GPT_VAR ) 
      {
         /* Checking for duplicates. The user may have given duplicated
            primitives (they are allowed to do that). */
         if( ! already_added[INDEX( m_primitives.m_primitives[i] )] )
         {
            unsigned arity = m_primitives.DB[INDEX( m_primitives.m_primitives[i] )].arity; 

            interpreter += " case " + util::ToString( INDEX( m_primitives.m_primitives[i] ) )
               + ": PUSH" + ( arity <= 3 ? "_" : "(" ) + util::ToString( arity ) + (arity <= 3 ? "(" : ",") + 
#ifdef FAST_PRIMITIVES
            ( m_primitives.DB[INDEX( m_primitives.m_primitives[i] )].fastcode.empty() ?
              m_primitives.DB[INDEX( m_primitives.m_primitives[i] )].code :
              m_primitives.DB[INDEX( m_primitives.m_primitives[i] )].fastcode )
#else
              m_primitives.DB[INDEX( m_primitives.m_primitives[i] )].code 
#endif
               + ") break;";

            // Make this primitive as already added to the interpreter
            already_added[INDEX( m_primitives.m_primitives[i] )] = true;
         }
      }
   interpreter += "\n";

   /*
      In practice we don't need 'max_stack_size' to be equal to 'MaximumTreeSize()',
      we can lower this and then save memory on the device. The formula below give us
      the upper bound for stack use:

                             /          |            MTS             | \
            stack size = max|  1, MTS - |  ------------------------  |  |
                            |           |       /               \    |  |
                            |           |   min|  arity   , MTS  |   |  |
                             \          |_      \      max      /   _| /

            where MTS is MaximumTreeSize().
    */

   unsigned max_stack_size = std::max( 1U, static_cast<unsigned>( MaximumTreeSize() - 
                                           std::floor(MaximumTreeSize() / 
                                           (float) std::min( m_primitives.m_max_arity, MaximumTreeSize() ) ) ) );

   // program_src = header + kernel
   std::string program_src = 
      "#define LOCAL_SIZE " + util::ToString( m_local_size ) + "\n" +
      "#define POP_SIZE " + util::ToString( m_params->m_population_size ) + "\n" +
      "#define NUM_POINTS " + util::ToString( m_num_points ) + "\n"
      "#define X_DIM " + util::ToString( m_x_dim ) + "\n"
      "#define MAX_TREE_SIZE " + util::ToString( MaximumTreeSize() ) + "\n" +
      "#define MAX_FLOAT " + util::ToString( std::numeric_limits<cl_float>::max() ) + "f\n" +
      "#define TOP       ( stack[stack_top] )\n"
      "#define POP       ( stack[stack_top--] )\n"
 //     "#define PUSH( i ) ( stack[++stack_top] = (i) )\n"
      "#define PUSH(arity, exp) stack[stack_top + 1 - arity] = (exp); stack_top += 1 - arity;\n"
      "#define PUSH_0( value ) stack[++stack_top] = (value);\n"
      "#define PUSH_1( exp ) stack[stack_top] = (exp);\n"
      "#define PUSH_2( exp ) stack[stack_top - 1] = (exp); --stack_top;\n"
      "#define PUSH_3( exp ) stack[stack_top - 2] = (exp); stack_top -= 2;\n"
      "#define ARG(n) (stack[stack_top - n])\n"
      "#define STACK_SIZE " + util::ToString( max_stack_size ) + "\n" +
      "#define CREATE_STACK float stack[STACK_SIZE]; int stack_top = -1;\n"
      "#define NODE program[op]\n"
      + interpreter + m_kernel_src;

#ifndef NDEBUG
   std::cout << "\nActual OpenCL kernel:\n\n" << program_src << std::endl;
#endif

   //--------------------

   cl::Program::Sources sources( 1, std::make_pair(  program_src.c_str(), program_src.size() ));

   cl::Program program( m_context, sources );

   // TODO: currently not elegant!
   std::vector<cl::Device> devices; devices.push_back( m_device );
   try {
      program.build( devices, m_compile_flags.c_str() );
   } catch( cl::Error& e ) {
      if( e.err() == CL_BUILD_PROGRAM_FAILURE )
      {
         std::string str =
            program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( m_device );
         std::cerr << "Program Info: " << str << std::endl;
      }

      throw;
   }

#ifndef NDEBUG
	std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
	std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
	std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
#endif

   m_kernel = cl::Kernel( program, "evaluate" );
}

// -----------------------------------------------------------------------------
void GP::LoadKernel( const char* kernel_file )
{
   std::ifstream file( kernel_file );
   //m_kernel_src = "";
   m_kernel_src.append( std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()) );
}

//////////////////////////// Genetic Operations ///////////////////////////////
// -----------------------------------------------------------------------------
void GP::InitializePopulation( cl_uint* pop )
{
   for( unsigned i = 0; i < m_params->m_population_size; ++i )
   {
      cl_uint tree_size = Random::Int( MinimumTreeSize(), MaximumTreeSize() );

      cl_uint* program = Program( pop, i );

      // The first "node" is the program's size
      SetProgramSize( program, tree_size );

      CreateLinearTree( ++program, tree_size );
#ifdef PROFILING
      // Update the total number of nodes that are going to be evaluated (to be
      // used to calculate how many GPop/s we could achieve).
      m_node_evaluations += tree_size * m_num_points;
#endif
   }
}

// -----------------------------------------------------------------------------
void GP::PrintProgram( const cl_uint* program ) const
{
   PrintTree( program + 1 );
}

// -----------------------------------------------------------------------------
void GP::PrintProgramPretty( const cl_uint* program, int start, int end ) const
{
   if( (start == -1) || (end == -1) ) { start = 0; end = ProgramSize( program++ ) - 1; }
   
   if( ARITY( *(program + start) ) == 0 )
   {
      PrintNode( program + start );
      return;
   }
   else
   {
      PrintNode( program + start ); std::cout << "( ";
   }

   int i;
   start++;
   while( start <= end )
   {
      i = TreeSize( program + start );
      PrintProgramPretty( program, start, ( i > 1 ) ? start + i - 1 : end );
      start += i; 
      
      /* Put the trailing ")" */
      if( start <= end ) std::cout << ", "; else std::cout << " )";
   }
   return;
}

// -----------------------------------------------------------------------------
void GP::PrintNode( const cl_uint* node ) const
{
   switch( INDEX( *node ) )
   {
      case Primitives::GPT_VAR:
         std::cout << "X" << AS_INT( *node ) << "";
         break;
      case Primitives::GPT_EPHEMERAL:
         std::cout << AS_FLOAT( *node ) << "";
         break;
      case Primitives::GPT_CLASS:
         std::cout << "class(" << AS_INT( *node ) << ")";
         break;
      case Primitives::GPF_IDENTITY:
         std::cout << "I";
         break;
      default:
         std::cout << m_primitives.DB[INDEX(*node)].name << "";
   }
}

void GP::convertProgramString(const cl_uint *program, QString &output, int start, int end)
{
  if( (start == -1) || (end == -1) ) { start = 0; end = ProgramSize( program++ ) - 1; bufferString = "";}

  if( ARITY( *(program + start) ) == 0 )
  {
     switch( INDEX( *( program + start ) ) )
     {
        case Primitives::GPT_VAR:
           bufferString += "X";
           bufferString += QString::number(AS_INT( *(program + start) ));
           break;
        case Primitives::GPT_EPHEMERAL:
           bufferString += QString::number(AS_FLOAT( *(program + start) ));
           break;
        case Primitives::GPT_CLASS:
           bufferString += "class(";
           bufferString += QString::number(AS_INT( *(program + start) ));
           bufferString += ")";
           break;
        case Primitives::GPF_IDENTITY:
           bufferString += "I";
           break;
        default:
           bufferString += QString::fromStdString(m_primitives.DB[INDEX(*(program + start))].name);
     }
     return;
  }
  else
  {
     switch( INDEX( *( program + start ) ) )
     {
       case Primitives::GPT_VAR:
          bufferString += "X";
          bufferString += QString::number(AS_INT( *(program + start) ));
          break;
       case Primitives::GPT_EPHEMERAL:
          bufferString += QString::number(AS_FLOAT( *(program + start) ));
          break;
       case Primitives::GPT_CLASS:
          bufferString += "class(";
          bufferString += QString::number(AS_INT( *(program + start) ));
          bufferString += ")";
          break;
       case Primitives::GPF_IDENTITY:
          bufferString += "I";
          break;
       default:
          bufferString += QString::fromStdString(m_primitives.DB[INDEX(*(program + start))].name);

     }
     bufferString += "( ";
  }

  int i;
  start++;
  while( start <= end )
  {
     i = TreeSize( program + start );
     convertProgramString( program, output, start, ( i > 1 ) ? start + i - 1 : end );
     start += i;

     /* Put the trailing ")" */
     if( start <= end )
      bufferString += ", ";
     else
       bufferString += " )";
  }
  output = bufferString;
  return;
}
void GP::convertProgramToTreeStruct(treeStruct &tree, const cl_uint *program)
{
  tree.name.clear();
  tree.arity.clear();
  tree.subTreeSize.clear();
  tree.posX.clear();
  tree.posY.clear();
  tree.index.clear();
  int size = ProgramSize(program++);
  for(int i = 0;i < size;i++)
  {
    switch( INDEX( *(program + i) ) )
    {
      case Primitives::GPT_VAR:
        tree.name.append("X" + QString::number(AS_INT( *(program + i) )));
        break;
      case Primitives::GPT_EPHEMERAL:
        tree.name.append(QString::number(AS_FLOAT( *(program + i) )));
        break;
      case Primitives::GPT_CLASS:
        tree.name.append(QString::number(AS_INT( *(program + i) )));
        break;
      case Primitives::GPF_IDENTITY:
        tree.name.append("I");
        break;
      default:
        tree.name.append(QString::fromStdString(m_primitives.DB[INDEX(*(program + i))].name));
    }
    tree.arity.append(ARITY( *(program + i)));
    tree.subTreeSize.append(TreeSize( program + i ));
    tree.posX.append(0);
    tree.posY.append(0);
    tree.index.append(-1);
  }
}

// -----------------------------------------------------------------------------
void GP::PrintTree( const cl_uint* node ) const
{
   int sum = 0;

   do {
      PrintNode( node );
      sum += ARITY( *node++ ) - 1;
   } while( sum != -1 );
}

// -----------------------------------------------------------------------------
void GP::CreateLinearTree( cl_uint* node, unsigned size ) const
{
   assert( size >= 1 );
   assert( node != 0 );

   unsigned open = 1;

   do {
      if( open == size || open > 1 )
         /* 
            [open == size] When the number of open arguments is equal the
            number of left nodes, then the only valid choice is a terminal
            (RandomNode( 0, 0 )), otherwise we would end up with a program
            greater than its size.

            [open > 1] When the number of open arguments is greater than one,
            then we can allow terminals to be chosen because they will not
            prematurely end the program.
          */
         *node = m_primitives.RandomNode( 0, size - open );
      else
         /* This means that 'open == 1' and 'size > 1', so we cannot choose
            a terminal here because we would end up with a shorter program. */
         *node = m_primitives.RandomNode( 1, size - open );

      /* Whenever we put a new operator/operand, the number of open arguments
         decreases. However, if the new operator requires more than one
         argument (arity >= 2) then we end up increasing the current number of
         open arguments.
       */
      open += ARITY( *node++ ) - 1;
   } while( --size );
}

void GP::insertData(std::vector<std::vector<float> > data)
{
  input_data_matrix = data;
  original_input_data_matrix = data;
}

// -----------------------------------------------------------------------------
void GP::LoadPoints( std::vector<std::vector<cl_float> > & out_x )
{
   using namespace util;
  int nRows = input_data_matrix.size();
  int nCols = input_data_matrix.at(0).size();
  m_Y.clear();
	for(int j = 0;j < nRows; j++)
	{
		std::vector<float> line_data;
		for(int i = 0;i < nCols-1; i++)
		{
			line_data.push_back(input_data_matrix[j][i]);
		}
		out_x.push_back(line_data);
		m_Y.push_back(input_data_matrix[j][nCols-1]);
		if( m_Y.back() < m_primitives.m_min_Y && m_Y.back() > 0 ) m_primitives.m_min_Y = m_Y.back();
		if( m_Y.back() > m_primitives.m_max_Y && m_Y.back() <= MAX_INT_VALUE ) m_primitives.m_max_Y = m_Y.back();
	}
	m_num_points = nRows;
	m_x_dim = nCols - 1;
}

void GP::randomlySplitData(std::vector<std::vector<float> > original, int ratio)
{
	int nRows = original.size();
	int nCols = original.at(0).size();
	int trainingSize = (nRows * ratio)/100;
	int testingSize = nRows - trainingSize;
	std::vector<float> instanceTemp;
	training_data.clear();
	testing_data.clear();
	std::vector<int> index;
	for(int i = 0; i < nRows; ++i)
		index.push_back(i);
	std::random_shuffle(index.begin(), index.end());
	for(int ind = 0;ind < trainingSize;ind++)
	{
		instanceTemp = original.at(index.at(ind));
		training_data.push_back(instanceTemp);
	}
	for(int ind = 0;ind < testingSize;ind++)
	{
		instanceTemp = original.at(index.at(ind + trainingSize));
		testing_data.push_back(instanceTemp);
	}
	m_num_points = trainingSize;
	m_num_points_testing = testingSize;
}

// -----------------------------------------------------------------------------

float GP::locallyEvaluateTrainingCompressed(const cl_uint *program, std::vector<double> &act_compress, std::vector<double> &exp_compress)
{
	float partial_error = 0.0f;
	float partial_semantic;
	float rmse;
	std::vector<float> actual;
	std::vector<float> expected;

	for( uint iter = 0; iter < m_num_points; iter++ )
	{
		partial_semantic = evaluateInstance(program,iter);
		partial_error += pow( input_data_matrix.at(iter).at(m_x_dim) - partial_semantic, 2 );
		actual.push_back(partial_semantic);
		expected.push_back(input_data_matrix.at(iter).at(m_x_dim));
	}
	rmse = sqrt(partial_error/m_num_points);
	//qDebug()<<"RMSE: "<<rmse;
	compressOutputPairs(actual,expected,act_compress,exp_compress);
	return rmse;

}

float GP::locallyEvaluateTraining(const cl_uint *program)
{
	float partial_error = 0.0f;
	float partial_semantic;
	float rmse;
	std::vector<float> actual;
	std::vector<float> expected;

	for( uint iter = 0; iter < m_num_points; iter++ )
	{
		partial_semantic = evaluateInstance(program,iter);
		partial_error += pow( input_data_matrix.at(iter).at(m_x_dim) - partial_semantic, 2 );
		actual.push_back(partial_semantic);
		expected.push_back(input_data_matrix.at(iter).at(m_x_dim));
	}
	rmse = sqrt(partial_error/m_num_points);
	return rmse;

}

float GP::locallyEvaluateTesting(const cl_uint *program)
{
	float partial_error = 0.0f;
	float partial_semantic;
	float rmse;
	std::vector<float> actual;
	std::vector<float> expected;

	for( uint iter = 0; iter < m_num_points_testing; iter++ )
	{
		partial_semantic = evaluateInstance(program,iter);
		partial_error += pow( input_data_matrix.at(iter).at(m_x_dim) - partial_semantic, 2 );
		actual.push_back(partial_semantic);
		expected.push_back(input_data_matrix.at(iter).at(m_x_dim));
	}
	rmse = sqrt(partial_error/m_num_points_testing);
	return rmse;
}

float GP::evaluateInstance(const cl_uint *program, int iter)
{
  unsigned max_stack_size = std::max( 1U, static_cast<unsigned>( MaximumTreeSize() -
                                          std::floor(MaximumTreeSize() /
                                          (float) std::min( m_primitives.m_max_arity, MaximumTreeSize() ) ) ) );
  int index;
  #define X_DIM m_x_dim
  #define POP       ( stack[stack_top--] )
  //#define PUSH(arity, exp) stack[stack_top + 1 - arity] = (exp); stack_top += 1 - arity;
  #define PUSH_0( value ) stack[++stack_top] = value;
  #define PUSH_1( exp ) stack[stack_top] = exp;
  #define PUSH_2( exp ) stack[stack_top - 1] = exp; --stack_top;
  #define PUSH_3( exp ) stack[stack_top - 2] = exp; stack_top -= 2;
  #define ARG(n) (stack[stack_top - n])
  #define STACK_SIZE max_stack_size
  #define CREATE_STACK float stack[STACK_SIZE]; int stack_top = -1;
  #define NODE program[op]

	CREATE_STACK
	float error = 0.0f;
	for(int op = ProgramSize(program); op > 0; op--)
	{
		index = INDEX(program[op]);
		switch(index) {
			case 0: PUSH_0(AS_FLOAT(NODE)) break;
			case 1: PUSH_0(AS_INT( NODE )) break;
			case 2: PUSH_3((int)ARG(0) ? ARG(1) : ARG(2)) break;
			case 3: PUSH_2(ARG(0) + ARG(1)) break;
			case 4: PUSH_2(ARG(0) && ARG(1)) break;
			case 5: PUSH_2((ARG(1) == 0.0f ? 1.0f : ARG(0)/ARG(1))) break;
			case 6: PUSH_2(ARG(0) == ARG(1)) break;
			case 7: PUSH_2(fmod(ARG(0), ARG(1))) break;
			case 8: PUSH_2(ARG(0) > ARG(1)) break;
			case 9: PUSH_2(ARG(0) >= ARG(1)) break;
			case 10: PUSH_2(ARG(0) < ARG(1)) break;
			case 11: PUSH_2(ARG(0) <= ARG(1)) break;
			case 12: PUSH_2(max(ARG(0), ARG(1))) break;
			case 13: PUSH_2((ARG(0) + ARG(1))/2.0f) break;
			case 14: PUSH_2(min(ARG(0), ARG(1))) break;
			case 15: PUSH_2(ARG(0) - ARG(1)) break;
			case 16: PUSH_2(ARG(0) * ARG(1)) break;
			case 17: PUSH_2(ARG(0) != ARG(1)) break;
			case 18: PUSH_2(ARG(0) || ARG(1)) break;
			case 19: PUSH_2(pow(ARG(0), ARG(1))) break;
			case 20: PUSH_2(((ARG(0) <= 0.0f && ARG(1) > 0.0f) || (ARG(0) > 0.0f && ARG(1) <= 0.0f))) break;
			case 21: PUSH_1(fabs(ARG(0))) break;
			case 22: PUSH_1(ceil(ARG(0))) break;
			case 23: PUSH_1(cos(ARG(0))) break;
			case 24: PUSH_1(exp(ARG(0))) break;
			case 25: PUSH_1(exp10(ARG(0))) break;
			case 26: PUSH_1(exp2(ARG(0))) break;
			case 27: PUSH_1(floor(ARG(0))) break;
			case 28: PUSH_1(ARG(0) * ARG(0)) break;
			case 29: PUSH_1(ARG(0) * ARG(0) * ARG(0)) break;
			case 30: PUSH_1(ARG(0) * ARG(0) * ARG(0) * ARG(0)) break;
			case 31: PUSH_1((ARG(0) < 1.0f ? 1.0f : log(ARG(0)))) break;
			case 32: PUSH_1((ARG(0) < 1.0f ? 1.0f : log10(ARG(0)))) break;
			case 33: PUSH_1((ARG(0) < 1.0f ? 1.0f : log2(ARG(0)))) break;
			case 34: PUSH_1(-ARG(0)) break;
			case 35: PUSH_1(!(int)ARG(0)) break;
			case 36: PUSH_1(round(ARG(0))) break;
			case 37: PUSH_1(sin(ARG(0))) break;
			case 38: PUSH_1((ARG(0) < 0.0f ? 1.0f : sqrt(ARG(0)))) break;
			case 39: PUSH_1(tan(ARG(0))) break;
			case 40: PUSH_1((ARG(0) >= 0.0f)) break;
			case 41: PUSH_1((ARG(0) > 0.0f ? 1.0f : (ARG(0) < 0.0f ? -1.0f : 0.0f))) break;
			case 42: PUSH_1((1.0f/(1.0f + exp(-ARG(0))))) break;
			case 43: PUSH_1((ARG(0)*ARG(0)/(1.0f + ARG(0)*ARG(0)))) break;
			case 44: PUSH_1(pow((ARG(0)/2.71828174591064f)*sqrt(ARG(0)*sinh(1/ARG(0))),ARG(0))*sqrt(2*3.14159274101257f/ARG(0))) break;
			case 45: PUSH_1(exp(-ARG(0)*ARG(0))) break;
			case 46: PUSH_0(-1.0f) break;
			case 47: PUSH_0(-2.0f) break;
			case 48: PUSH_0(-3.0f) break;
			case 49: PUSH_0(0.0f) break;
			case 50: PUSH_0(1.0f) break;
			case 51: PUSH_0(0.31830987334251f) break;
			case 52: PUSH_0(2.0f) break;
			case 53: PUSH_0(0.63661974668503f) break;
			case 54: PUSH_0(1.12837922573090f) break;
			case 55: PUSH_0(3.0f) break;
			case 56: PUSH_0(1.202056903159594f) break;
			case 57: PUSH_0(0.915965594177219f) break;
			case 58: PUSH_0(2.71828174591064f) break;
			case 59: PUSH_0(0.5772156649015329f) break;
			case 60: PUSH_0(1.618033988749895f) break;
			case 61: PUSH_0(2.30258512496948f) break;
			case 62: PUSH_0(0.69314718246460f) break;
			case 63: PUSH_0(0.43429449200630f) break;
			case 64: PUSH_0(1.44269502162933f) break;
			case 65: PUSH_0(0.5671432904097839f) break;
			case 66: PUSH_0(3.14159274101257f) break;
			case 67: PUSH_0(1.57079637050629f) break;
			case 68: PUSH_0(0.78539818525314f) break;
			case 69: PUSH_0(0.70710676908493f) break;
			case 70: PUSH_0(1.41421353816986f) break;
			case 127: PUSH_0(input_data_matrix.at(iter).at(AS_INT(program[op]))) break;
		}
	}
	return POP;
}

void GP::compressOutputPairs(std::vector<float> actual, std::vector<float> expected, std::vector<double> &actual_compressed, std::vector<double> &expected_compressed)
{
	int outSamples = 20;
	int length = actual.size();
	int sliceLength = length/outSamples;
	int start,end;
	float partial_act,partial_exp,avg_act,avg_exp;
	actual_compressed.clear();
	expected_compressed.clear();
	for(int i = 0;i < (outSamples-1);i++)
	{
		// gather slice data
		start = i * sliceLength;
		end = ((i + 1) * sliceLength) - 1;
		partial_act = 0.0f;
		partial_exp = 0.0f;
		for(int j = start;j <= end;j++)
		{
			partial_act += actual.at(j);
			partial_exp += expected.at(j);
		}
		avg_act = partial_act / sliceLength;
		avg_exp = partial_exp / sliceLength;
		actual_compressed.push_back(avg_act);
		expected_compressed.push_back(avg_exp);
	}
	// last slice could be larger
	start = (outSamples - 1) * sliceLength;
	end = length - 1;
	partial_act = 0.0f;
	partial_exp = 0.0f;
	for(int j = start;j <= end;j++)
	{
		partial_act += actual.at(j);
		partial_exp += expected.at(j);
	}
	avg_act = partial_act / sliceLength;
	avg_exp = partial_exp / sliceLength;
	actual_compressed.push_back(avg_act);
	expected_compressed.push_back(avg_exp);
}

float GP::executeLS_CMAES(const cl_uint *program)
{
	CMAES<double> evo;
	double *arFunvals, *const*pop, *xfinal;
	double fmean;
	int programSize = ProgramSize(program);
	// Initialize everything
	const int dim = programSize;
	double xstart[dim];
	for(int i=0; i<dim; i++) xstart[i] = 1;
	double stddev[dim];
	for(int i=0; i<dim; i++) stddev[i] = 0.3;
	Parameters<double> parameters;
	parameters.lambda = 20;
	parameters.stopMaxIter = 300;
	parameters.init(dim, xstart, stddev);
	arFunvals = evo.init(parameters);

  while(!evo.testForTermination())
  {
    pop = evo.samplePopulation();
    for (int i = 0; i < evo.get(CMAES<double>::Lambda); ++i)
      arFunvals[i] = fitnessCMAES(pop[i],program,programSize);
    evo.updateDistribution(arFunvals);
  }
  xfinal = evo.getNew(CMAES<double>::XMean);
  fmean = fitnessCMAES(evo.getPtr(CMAES<double>::XMean),program,programSize);
  qDebug()<<(int) evo.get(CMAES<double>::Generation)<<" generations";
  qDebug()<<"CMA-ES best fitness = "<<evo.get(CMAES<double>::FBestEver);
  qDebug()<<"CMA-ES xmean fitness = "<<fmean;
}

double GP::fitnessCMAES(const double *x, const cl_uint *program, int N)
{
	float expectedOutput;
	float actualOutput;
	float partial_error = 0.0f;
//	for(int k = 0;k < N;k++)
//		qDebug()<<"par["<<k<<"]="<<x[k];
	for( uint iter = 0; iter < m_num_points; iter++ )
	{
		expectedOutput = input_data_matrix.at(iter).at(m_x_dim);
		actualOutput = evaluateInstanceCMAES(program,iter,x);
		partial_error += pow( expectedOutput - actualOutput, 2 );
	}
	return sqrt(partial_error/m_num_points);
}

float GP::evaluateInstanceCMAES(const cl_uint *program, int iter, const double *v)
{
  unsigned max_stack_size = std::max( 1U, static_cast<unsigned>( MaximumTreeSize() -
                                          std::floor(MaximumTreeSize() /
                                          (float) std::min( m_primitives.m_max_arity, MaximumTreeSize() ) ) ) );
  int index;
  #define X_DIM m_x_dim
  #define POP       ( stack[stack_top--] )
  //#define PUSH(arity, exp) stack[stack_top + 1 - arity] = (exp); stack_top += 1 - arity;
  #define PUSH_0( value ) stack[++stack_top] = value;
  #define PUSH_1( exp ) stack[stack_top] = exp;
  #define PUSH_2( exp ) stack[stack_top - 1] = exp; --stack_top;
  #define PUSH_3( exp ) stack[stack_top - 2] = exp; stack_top -= 2;
  #define ARG(n) (stack[stack_top - n])
  #define STACK_SIZE max_stack_size
  #define CREATE_STACK float stack[STACK_SIZE]; int stack_top = -1;
  #define NODE program[op]

	CREATE_STACK
	float error = 0.0f;
	for(int op = ProgramSize(program); op > 0; op--)
	{
		index = INDEX(program[op]);
		switch(index) {
			case 0: PUSH_0(v[op]*AS_FLOAT(NODE)) break;
			case 1: PUSH_0(AS_INT( NODE )) break;
			case 2: PUSH_3((int)ARG(0) ? ARG(1) : ARG(2)) break;
			case 3: PUSH_2(v[op]*(ARG(0) + ARG(1))) break;
			case 4: PUSH_2(ARG(0) && ARG(1)) break;
			case 5: PUSH_2((ARG(1) == 0.0f ? v[op]*1.0f : v[op]*(ARG(0)/ARG(1)))) break;
			case 6: PUSH_2(ARG(0) == ARG(1)) break;
			case 7: PUSH_2(fmod(ARG(0), ARG(1))) break;
			case 8: PUSH_2(ARG(0) > ARG(1)) break;
			case 9: PUSH_2(ARG(0) >= ARG(1)) break;
			case 10: PUSH_2(ARG(0) < ARG(1)) break;
			case 11: PUSH_2(ARG(0) <= ARG(1)) break;
			case 12: PUSH_2(max(ARG(0), ARG(1))) break;
			case 13: PUSH_2((ARG(0) + ARG(1))/2.0f) break;
			case 14: PUSH_2(min(ARG(0), ARG(1))) break;
			case 15: PUSH_2(v[op]*(ARG(0) - ARG(1))) break;
			case 16: PUSH_2(v[op]*(ARG(0) * ARG(1))) break;
			case 17: PUSH_2(ARG(0) != ARG(1)) break;
			case 18: PUSH_2(ARG(0) || ARG(1)) break;
			case 19: PUSH_2(pow(ARG(0), ARG(1))) break;
			case 20: PUSH_2(((ARG(0) <= 0.0f && ARG(1) > 0.0f) || (ARG(0) > 0.0f && ARG(1) <= 0.0f))) break;
			case 21: PUSH_1(fabs(ARG(0))) break;
			case 22: PUSH_1(ceil(ARG(0))) break;
			case 23: PUSH_1(v[op]*cos(ARG(0))) break;
			case 24: PUSH_1(v[op]*exp(ARG(0))) break;
			case 25: PUSH_1(exp10(ARG(0))) break;
			case 26: PUSH_1(exp2(ARG(0))) break;
			case 27: PUSH_1(floor(ARG(0))) break;
			case 28: PUSH_1(ARG(0) * ARG(0)) break;
			case 29: PUSH_1(ARG(0) * ARG(0) * ARG(0)) break;
			case 30: PUSH_1(ARG(0) * ARG(0) * ARG(0) * ARG(0)) break;
			case 31: PUSH_1((ARG(0) < 1.0f ? 1.0f : log(ARG(0)))) break;
			case 32: PUSH_1((ARG(0) < 1.0f ? 1.0f : log10(ARG(0)))) break;
			case 33: PUSH_1((ARG(0) < 1.0f ? 1.0f : log2(ARG(0)))) break;
			case 34: PUSH_1(-ARG(0)) break;
			case 35: PUSH_1(!(int)ARG(0)) break;
			case 36: PUSH_1(round(ARG(0))) break;
			case 37: PUSH_1(v[op]*sin(ARG(0))) break;
			case 38: PUSH_1((ARG(0) < 0.0f ? v[op]*1.0f : v[op]*sqrt(ARG(0)))) break;
			case 39: PUSH_1(v[op]*tan(ARG(0))) break;
			case 40: PUSH_1((ARG(0) >= 0.0f)) break;
			case 41: PUSH_1((ARG(0) > 0.0f ? 1.0f : (ARG(0) < 0.0f ? -1.0f : 0.0f))) break;
			case 42: PUSH_1((1.0f/(1.0f + exp(-ARG(0))))) break;
			case 43: PUSH_1((ARG(0)*ARG(0)/(1.0f + ARG(0)*ARG(0)))) break;
			case 44: PUSH_1(pow((ARG(0)/2.71828174591064f)*sqrt(ARG(0)*sinh(1/ARG(0))),ARG(0))*sqrt(2*3.14159274101257f/ARG(0))) break;
			case 45: PUSH_1(exp(-ARG(0)*ARG(0))) break;
			case 46: PUSH_0(-1.0f) break;
			case 47: PUSH_0(-2.0f) break;
			case 48: PUSH_0(-3.0f) break;
			case 49: PUSH_0(0.0f) break;
			case 50: PUSH_0(1.0f) break;
			case 51: PUSH_0(0.31830987334251f) break;
			case 52: PUSH_0(2.0f) break;
			case 53: PUSH_0(0.63661974668503f) break;
			case 54: PUSH_0(1.12837922573090f) break;
			case 55: PUSH_0(3.0f) break;
			case 56: PUSH_0(1.202056903159594f) break;
			case 57: PUSH_0(0.915965594177219f) break;
			case 58: PUSH_0(2.71828174591064f) break;
			case 59: PUSH_0(0.5772156649015329f) break;
			case 60: PUSH_0(1.618033988749895f) break;
			case 61: PUSH_0(2.30258512496948f) break;
			case 62: PUSH_0(0.69314718246460f) break;
			case 63: PUSH_0(0.43429449200630f) break;
			case 64: PUSH_0(1.44269502162933f) break;
			case 65: PUSH_0(0.5671432904097839f) break;
			case 66: PUSH_0(3.14159274101257f) break;
			case 67: PUSH_0(1.57079637050629f) break;
			case 68: PUSH_0(0.78539818525314f) break;
			case 69: PUSH_0(0.70710676908493f) break;
			case 70: PUSH_0(1.41421353816986f) break;
			case 127: PUSH_0(input_data_matrix.at(iter).at(AS_INT(program[op]))) break;
		}
	}
	return POP;
}

float GP::executeLS_LM(const cl_uint *program)
{
	int nNodes = ProgramSize(program);
	alglib::real_1d_array x = "[0,0]";
	double epsg = 0.0000000001;
	double epsf = 0;
	double epsx = 0;
	alglib::ae_int_t maxits = 0;
	alglib::minlmstate state;
	alglib::minlmreport rep;

	x.setlength(nNodes);
	for(int i = 0;i < nNodes;i++)
		x[i] = 1; // Initial states
//	program_GLOBAL = program;
//	m_num_points_GLOBAL = m_num_points;
//	input_data_matrix_GLOBAL = input_data_matrix;
//	m_x_dim_GLOBAL = m_x_dim;
//	programSize_GLOBAL = nNodes;

	alglib::minlmcreatev(nNodes, x, 0.0001, state);
	alglib::minlmsetcond(state, epsg, epsf, epsx, maxits);
	//alglib::minlmoptimize(state, fitnessLM);
	alglib::minlmresults(state, x, rep);

	qDebug()<<int(rep.terminationtype);
	qDebug()<<x.tostring(2).c_str();
}
