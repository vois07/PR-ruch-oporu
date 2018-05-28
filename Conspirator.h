#include <stdio.h>
#include <vector>
#include "Resource.h"
#include "Meeting.h"

using namespace std;

class Conspirator {
    public:
        bool isAcceptor;
        bool isLeader;

        int parentId;
        int id;
        int leftNeighbourId;
        int rightNeighbourId;
        int leftChildId;
        int rightChildId;		

        Conspirator(int myId, int processAmount);

	//conspirator
	void wait(int);
	Resource res;
	Meeting meet;
	int haveIBeenInvited();
	void answerInvite();
	void joinMeeting();
	int getUpdate();
	void leaveMeeting();
	
	//acceptor
	void checkPermission();
	void grantPermission();
	
	//leader
	void invite(int);
	void askPermission();
	void getPermission();
	void askDVD();
	Resource getDVD();
	void updatePatricipants(int);
	void startMeeting();
	void endMeeting();
	
	//resource owner
	void answerDVD();


};
