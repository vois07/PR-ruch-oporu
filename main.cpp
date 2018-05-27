#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Conspirator.cpp"
#include "Functions.cpp"
#include "utils.h"

using namespace std;



int main (int argc, char* argv[])
{
  //clear log saved previously
  clearLogFile();
  srand(time(NULL));
  
  int rank, size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  Conspirator c(rank, size);

  printStatus("Process "+to_string(c.id)+": parent="+to_string(c.parentId)+" lCh="+to_string(c.leftChildId)+" rCh="+to_string(c.rightChildId)+" lN="
    +to_string(c.leftNeighbourId)+" rN="+to_string(c.rightNeighbourId)+"\n");

  int i = 100+c.id;
  int rnd = rand() % (100+c.id);

  // while(1){
    while(!c.wantsToMeet){
      printStatus("Process "+to_string(c.id)+": rand="+to_string(rnd)+", barrier="+to_string(i)+"\n");

      if(i<0)break;
      
      if(rnd >= i) {
        c.wantsToMeet = true;
        printStatus("Process "+to_string(c.id)+": I do want to meet. :)\n");
        //rzeczy
      
      } else {
        printStatus("Process "+to_string(c.id)+": I don't want to meet. :(\n");
        i--;
        rnd = rand() % (100+c.id);
      }
    }
    
  // }

  MPI_Finalize();
  return 0;
}
