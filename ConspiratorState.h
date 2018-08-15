#ifndef CONSPIRATOR_STATE_STRUCTURE_H
#define CONSPIRATOR_STATE_STRUCTURE_H
#include "ConspiratorStructure.h"
#include "Fifo.h"
// strutkura pelniaca role pamieci globalnej pomiedzy watkami, tutaj chyba kolejki, wskaznik na wlasciciela tokena, itd
typedef struct{
    Conspirator* conspiratorData;
    int resourceOwner;
    int sendResourceRequest;
    int waitForAcceptance;
    fifo* resourceRequestFifo;
    fifo* acceptanceRequestFifo;
    int usingResources;
}ConspiratorState;

#endif