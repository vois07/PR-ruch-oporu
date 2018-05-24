#include <stdio.h>
#include <vector>

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