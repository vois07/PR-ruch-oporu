#include "utils.h"
#include <string>
#include <random>

namespace utils
{
	int getMeetingChannel(int leader)
	{
		string ans = std::to_string(leader) + std::to_string(rand())%(INT_MAX/1000);
		return std::stoi(ans);
	}
	
	typedef struct Update_s
	{
		int meetingID;
		int newParticipant;
	}Update;
	
	void makeMPI_UPDATE()
	{
		const int nitems = 2;
		int blocklengths[2]={1,1};
		MPI_Datatype types[2]={MPI_INT, MPI_INT};
		MPI_Datatype MPI_UPDATE;
		MPI_Aint offsets[2];

		offsets[0] = offsetof(Update,meetingID);
		offsets[1] = offsetof(Update,newParticipant);

		MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_UPDATE);
		MPI_Type_commit(&MPI_UPDATE);
	}
}
