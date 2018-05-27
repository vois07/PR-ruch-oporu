#include <stdio.h>
#include <vector>
#include "Resource.h"
#include "Meeting.h"

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

		

        Conspirator(int myId, int processAmount);

		void Wait(int);
		Resource res;
		Meeting meet;
		int haveIBeenInvited();
		int meetingID;
		void Invite(int);
		void SendInvites();
		void askForPermission();
		Resource askForDVD();
		void grantDVD();
		void handleDVDReqs();
		void endMeeting();
		void StartMeeting();
		void UpdatePatricipants();
		void participateMeeting();
		void makeMeeting();
		void getUpdate();
};
