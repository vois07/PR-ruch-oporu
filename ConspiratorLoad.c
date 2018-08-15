#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ConspiratorStructure.h"
#include "ConspiratorState.h"
#include "Log.h"
#include "Utils.h"
#include <unistd.h>

void findSiblings(int id, int count, Conspirator* myConspirator){
    int i=0, le=0, re, leftNeighbourId, rightNeighbourId;
    while(le + pow(2,i) <= id) {
        le+=pow(2,i);
        i++;
    }
    re = le+pow(2, i)-1;

    leftNeighbourId = id-1;
    rightNeighbourId = id+1;
    if(id == le) {
        leftNeighbourId=-1;
    }
    if(id == re) {
        rightNeighbourId=-1;
    }

    if(rightNeighbourId >= count){
        rightNeighbourId=-1;
        if(leftNeighbourId >= count){
            leftNeighbourId=-1;
        }
    }

    if(leftNeighbourId == -1 && rightNeighbourId == -1) {
        myConspirator->siblings = NULL;
        myConspirator->siblingsCount = 0;
    } else if(leftNeighbourId != -1 && rightNeighbourId != -1) {
        int* siblings = malloc(2*sizeof(int));
        siblings[0]=leftNeighbourId; siblings[1]=rightNeighbourId;
        myConspirator->siblings = siblings;
        myConspirator->siblingsCount = 2;
    } else if(leftNeighbourId != -1 || rightNeighbourId != -1) {
        int sibling = leftNeighbourId +  rightNeighbourId + 1;  //id rodzenstwa o id != -1
        int* siblings = malloc(1*sizeof(int));
        siblings[0]=sibling;
        myConspirator->siblings = siblings;
        myConspirator->siblingsCount = 1;
    }    
}

void findParent(int id, Conspirator* myConspirator){
    if (id > 0) {
        if (id % 2 == 0) {
            myConspirator->parent = (id - 2)/2;
        } else {
            myConspirator->parent = (id - 1)/2;
        }
    } else myConspirator->parent = -1;
}

void findChildren(int id, int count, Conspirator* myConspirator){
    int leftChildId = -1, rightChildId = -1;
    if (2*id + 1 < count) {
        leftChildId = 2*id + 1;
    }
    if (2*id + 2 < count) {
        rightChildId = 2*id + 2;
    }

    if(leftChildId == -1 && rightChildId == -1) {
        myConspirator->children = NULL;
        myConspirator->childrenCount = 0;
    } else if(leftChildId != -1 && rightChildId != -1) { 
        int* children = malloc(2*sizeof(int));
        children[0]=leftChildId; children[1]=rightChildId;
        myConspirator->children = children;
        myConspirator->childrenCount = 2;
    } else if(leftChildId != -1 || rightChildId != -1) {
        int child = leftChildId + rightChildId + 1; //id dziecka o id != -1
        int* children = malloc(1*sizeof(int));
        children[0] = child;
        myConspirator->children = children;
        myConspirator->childrenCount = 1;
    }
    // printf("FOUND id[%d] left[%d] right[%d]\n", id, leftChildId, rightChildId);
}


void printConspirator(Conspirator* c){
    char siblings[100];
    if(c->siblingsCount == 0) sprintf(siblings, "%s", "none");
    else if(c->siblingsCount == 1) sprintf(siblings, "{%d}", c->siblings[0]);
    else if(c->siblingsCount == 2) sprintf(siblings, "{%d, %d}", c->siblings[0], c->siblings[1]);

    char children[100];
    if(c->childrenCount == 0) sprintf(children, "%s", "none");
    else if(c->childrenCount == 1) sprintf(children, "{%d}", c->children[0]);
    else if(c->childrenCount == 2) sprintf(children, "{%d, %d}", c->children[0], c->children[1]);

    char buff[200];
    sprintf(buff, "id = %d, parent = %d, siblings = %s, children = %s\n", c->id, c->parent, siblings, children);
    printStatus(buff);
}

Conspirator* loadConspirator(int id, int count) {
    Conspirator* myConspirator = malloc(sizeof(Conspirator));
    
    findChildren(id, count, myConspirator);
    findParent(id, myConspirator);
    findSiblings(id, count, myConspirator);
    myConspirator->id = id;

    return myConspirator;
}

ConspiratorState* initConspirator(int conspiratorId, int conspiratorCount) {
    Conspirator* myConspirator = loadConspirator(conspiratorId, conspiratorCount);
    printConspirator(myConspirator);
    ConspiratorState* myConspiratorState = malloc(sizeof(ConspiratorState));
    myConspiratorState->conspiratorData=myConspirator;
    myConspiratorState->waitForAcceptance = 0;
    myConspiratorState->sendResourceRequest = 0;
    myConspiratorState->usingResources = 0;
    myConspiratorState->resourceOwner=myConspirator->parent;
    myConspiratorState->resourceRequestFifo = malloc(sizeof(fifo));
    myConspiratorState->resourceRequestFifo->start = 0;
    myConspiratorState->resourceRequestFifo->end = 0;

    myConspiratorState->acceptanceRequestFifo = malloc(sizeof(fifo));
    myConspiratorState->acceptanceRequestFifo->start = 0;
    myConspiratorState->acceptanceRequestFifo->end = 0;
    sleep(1); //do printowania
    return myConspiratorState;
}
