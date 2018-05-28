#include <iostream>
#include <mpi.h>
#define ROOT 0
#define MSG_TAG 100
#include<cstdlib>
#include<cstddef>
#include<cstring>


using namespace std;

typedef struct x_s 
{
	int id; char name[10];
}x;


int main(int argc, char **argv)
{
	int rank, size;
    MPI_Status status;

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	const int nitems = 2;
	int blocklengths[2]={1,10};
	MPI_Datatype types[2]={MPI_INT, MPI_CHAR};
	MPI_Datatype mpi_x;
	MPI_Aint offsets[2];

	offsets[0] = offsetof(x,id);
	offsets[1] = offsetof(x,name);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_x);
	MPI_Type_commit(&mpi_x);

	if (rank==0)
	{
		x sendx; string s = "asdafsc";
		sendx.id = 42; strcpy(sendx.name, s.c_str());
		const int dest = 1;
		MPI_Send (&sendx,1,mpi_x, dest, MSG_TAG, MPI_COMM_WORLD);
		cout<<"RANK: "<<rank<<" send x"<<endl;

	}

	if (rank==1)
	{
		MPI_Status status;

		const int src = 0;
		
		x recvx;
		MPI_Recv(&recvx,1,mpi_x, src, MSG_TAG, MPI_COMM_WORLD, &status);
		cout<< "RECEIVED!!!! "<<recvx.id<<" "<<recvx.name<<endl;
	}

	

	MPI_Type_free(&mpi_x);
	MPI_Finalize();
}

