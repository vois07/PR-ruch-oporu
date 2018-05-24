#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <cmath>

using namespace std;

class Conspirator {
    public:
        bool isAcceptor;
        bool isMeetingLeader;
        bool wantsToMeet;

        int parentId;
        int id;
        int leftNeighbourId;
        int rightNeighbourId;
        int leftChildId;
        int rightChildId;

        vector<int> team;
        bool isDvdTaken;

        Conspirator(int myId, int processAmount);
};

Conspirator::Conspirator(int myId, int processAmount){

            this->id = myId;
            this->isAcceptor = false;

            this->isMeetingLeader = false;
            this->wantsToMeet = false;
            this->parentId = -1;

            if (myId > 0) {
                if (myId % 2 == 0) {
                    this->parentId = (myId - 2)/2;
                } else {
                    this->parentId = (myId - 1)/2;
                }
            }

            int i=0, le=0, re;
            while(le + pow(2,i) <= myId) {
                le+=pow(2,i);
                i++;
            }
            re = le+pow(2, i)-1;

            this->leftNeighbourId=myId-1;
            this->rightNeighbourId=myId+1;
            if(myId == le) {
                this->leftNeighbourId=-1;
            }
            if(myId == re) {
                this->rightNeighbourId=-1;
            }

            if(this->rightNeighbourId>=processAmount){
                this->rightNeighbourId=-1;
                if(this->leftNeighbourId>=processAmount){
                    this->leftNeighbourId=-1;
                }
            }

            this->leftChildId = this->rightChildId = -1;
            if (2*myId + 1 < processAmount) {
                this->leftChildId = 2*myId + 1;
            }

            if (2*myId + 2 < processAmount) {
                this->rightChildId = 2*myId + 2;
            }

            this->isDvdTaken = false;

}


int main (int argc, char* argv[])
{
  int rank, size;

  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  //printf( "Hello world from process %d of %d\n", rank, size );

  Conspirator c(rank, size);
  printf("Process %d: parent=%d lCh=%d rCh=%d lN=%d rN=%d\n", c.id, c.parentId, c.leftChildId, c.rightChildId, c.leftNeighbourId, c.rightNeighbourId);

  MPI_Finalize();
  return 0;
}
