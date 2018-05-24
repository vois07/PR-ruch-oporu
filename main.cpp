#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include "Conspirator.cpp"
#include "Functions.cpp"

using namespace std;

int main (int argc, char* argv[])
{
  int rank, size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  //printf( "Hello world from process %d of %d\n", rank, size );

  Conspirator c(rank, size);
  //printf("Process %d: parent=%d lCh=%d rCh=%d lN=%d rN=%d\n", c.id, c.parentId, c.leftChildId, c.rightChildId, c.leftNeighbourId, c.rightNeighbourId);
  printStatus("Process "+to_string(c.id)+": parent="+to_string(c.parentId)+" lCh="+to_string(c.leftChildId)+" rCh="+to_string(c.rightChildId)+" lN="+to_string(c.leftNeighbourId)+" rN="+to_string(c.rightNeighbourId)+"\n");

  MPI_Finalize();
  return 0;
}
