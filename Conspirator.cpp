#include <stdio.h>
#include <vector>
#include <cmath>
#include "Conspirator.h"

using namespace std;

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