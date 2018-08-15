#include "ConspiratorLoad.h"
#include "Utils.h"
#include "Fifo.h"
#include "Lamport.h"
#include "Log.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stddef.h>

#define RESOURCE_OWNER_TAG 100
#define RESOURCE_TRANSFER_TAG 101
#define ORGANISE_MEETING_TAG 102
#define START_MEETING_TAG 103
#define END_MEETING_TAG 104
#define ACCEPTANCE_REQUEST_TAG 105
#define ASSIGNED_ACCEPTANCE_TAG 106
#define RESOURCE_REQUEST_TAG 107
#define RESOURCE_ASSIGN_TAG 108
#define BERSERK_TAG 666
#define KONOPNICKA_TAG 777

pthread_mutex_t resourceMutex;
pthread_mutex_t waitForResourceMutex;
pthread_mutex_t acceptanceFifoMutex;
pthread_mutex_t resourceFifoMutex;
pthread_mutex_t sendResourceMutex;
pthread_mutex_t acceptanceReceiveMutex;
pthread_cond_t acceptanceReceiveCv;
pthread_cond_t resourceCv;

int lamportTime;
#define MSG_DATA 2
MPI_Datatype mpiMsgType;
msg msgToSend;
ConspiratorState* currentConspiratorState;

void* resourceTransferListener(void* t) { //nasluchiwanie na przekazywanie zasobu w gore
	int resourceId;
	MPI_Status status;
	Conspirator* myConspirator = currentConspiratorState->conspiratorData;
	while(1) {
    	//MPI_Recv(&resourceId, 1, MPI_INT, MPI_ANY_SOURCE, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD, &status);
		resourceId = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		char buff[100];
		sprintf(buff, "[%d] Konspirator[%d] otrzymal zasob od Konspiratora[%d]\n", lamportTime, myConspirator->id, status.MPI_SOURCE);
    	printStatus(buff);
		
		if(myConspirator->id == 0) {
			currentConspiratorState->resourceOwner = 0;
		} else {//przekaz do rodzica
			sprintf(buff, "[%d] Konspirator[%d] przekazuje zasob do Konspiratora[%d]\n", lamportTime, myConspirator->id, myConspirator->parent);
			printStatus(buff);
			++lamportTime;
			msgToSend.data = resourceId;
			msgToSend.lamportTime = lamportTime;
			//MPI_Send(&resourceId, 1, MPI_INT, myConspirator->parent, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD);
			MPI_Send(&msgToSend, 1, mpiMsgType, myConspirator->parent, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD);
			currentConspiratorState->resourceOwner = myConspirator->parent;
		}
	}
	pthread_exit(NULL);
}

void* meetingAcceptaneListener(void* t) { //nasluchiwanie na potwierdzenie spotkania
	MPI_Status status;
	Conspirator* myConspirator = currentConspiratorState -> conspiratorData;
	int acceptanceCount = 0;
	int number = 1;
	char buff[100];
	while(acceptanceCount < myConspirator->siblingsCount) {
		sprintf(buff, "[%d] Konspirator[%d] oczekuje na jeszcze %d potwierdzen\n", lamportTime, myConspirator->id, myConspirator->siblingsCount-acceptanceCount);
		printStatus(buff);
    //MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, ORGANISE_MEETING_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, ORGANISE_MEETING_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		acceptanceCount++;
		sprintf(buff, "[%d] Konspirator[%d] otrzymal potwierdzenie od Konspiratora[%d]\n", lamportTime, myConspirator->id, status.MPI_SOURCE);
		printStatus(buff);
	}
	pthread_exit(NULL);
}

void* resourceAssignListener(void* t) { //nasluchiwanie na przypisanie zasobu
	int number;
	MPI_Status status;
	while(1) {
		//MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		pthread_mutex_lock(&sendResourceMutex);
		char buff[200];
		//printf("Process[%d] before pop\n", currentConspiratorState->conspiratorData->id);
		//printQueue(currentConspiratorState->resourceRequestFifo);
		int firstInQueue =  pop(currentConspiratorState->resourceRequestFifo);
		if(firstInQueue == currentConspiratorState->conspiratorData->id) {
			currentConspiratorState->resourceOwner = currentConspiratorState->conspiratorData->id;
			currentConspiratorState->sendResourceRequest = 0;
			currentConspiratorState->usingResources = 1;
			pthread_cond_signal(&resourceCv);
			sprintf(buff, "[%d] Konspirator[%d] otrzymal zasob\n", lamportTime, currentConspiratorState->conspiratorData->id);
			printStatus(buff);
		} else {
			sprintf(buff, "[%d] Konspirator[%d] otrzymal zasob od Konspiratora[%d] i przekazuje Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE, firstInQueue);
			printStatus(buff);
   		//MPI_Send(&number, 1, MPI_INT, firstInQueue, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = number;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, firstInQueue, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);

			currentConspiratorState->resourceOwner = firstInQueue;
			currentConspiratorState->sendResourceRequest = 0;
			if(isQueueEmpty(currentConspiratorState->resourceRequestFifo) == 0){
				sprintf(buff, "[%d] Konspirator[%d] przekazuje zadanie zasobu Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, currentConspiratorState->resourceOwner);
				printStatus(buff);
   			//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
				++lamportTime;
				msgToSend.data = number;
				msgToSend.lamportTime = lamportTime;
				MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
				currentConspiratorState->sendResourceRequest = 1;
			}
		}
		pthread_mutex_unlock(&sendResourceMutex);
	}
}

void* resourceRequestListener(void* t) { //nasluchiwanie na prosby o zasob
	int number;
	MPI_Status status;
	while(1) {
		//MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);

		char buff[200];
		sprintf(buff, "[%d] Konspirator[%d] otrzymal zadanie zasobu do Konspiratora[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
		printStatus(buff);
		pthread_mutex_lock(&sendResourceMutex);

		if(currentConspiratorState->conspiratorData->id == currentConspiratorState->resourceOwner) {
			if(currentConspiratorState->usingResources == 0 && isQueueEmpty(currentConspiratorState->resourceRequestFifo)){
				sprintf(buff, "[%d] Konspirator[%d] zdaje zasob na rzecz Konspiratora[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
				printStatus(buff);
				//MPI_Send(&number, 1, MPI_INT, status.MPI_SOURCE, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);
				++lamportTime;
				msgToSend.data = number;
				msgToSend.lamportTime = lamportTime;
				MPI_Send(&msgToSend, 1, mpiMsgType, status.MPI_SOURCE, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);

				currentConspiratorState->resourceOwner = status.MPI_SOURCE;
				currentConspiratorState->sendResourceRequest = 0;
			} else {
				sprintf(buff, "[%d] Konspirator[%d] blokuje zasob, zadanie Konspiratora[%d] w kolejce\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
				printStatus(buff);
				put(currentConspiratorState->resourceRequestFifo, status.MPI_SOURCE);
				//printf("Process[%d] after put\n", currentConspiratorState->conspiratorData->id);
				//printQueue(currentConspiratorState->resourceRequestFifo);
			}
		} else {
			sprintf(buff, "[%d] Konspirator[%d] umiescil zadanie Konspiratora[%d] w kolejce\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
			printStatus(buff);
			put(currentConspiratorState->resourceRequestFifo, status.MPI_SOURCE);
			//printf("Process[%d] after put\n", currentConspiratorState->conspiratorData->id);
			//printQueue(currentConspiratorState->resourceRequestFifo);
			if(currentConspiratorState->sendResourceRequest == 0) {
				sprintf(buff, "[%d] Konspirator[%d] przekazuje zasob Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, currentConspiratorState->resourceOwner);
				printStatus(buff);
	   		//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
				++lamportTime;
				msgToSend.data = number;
				msgToSend.lamportTime = lamportTime;
				MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);

				currentConspiratorState->sendResourceRequest = 1;
			}
		}
		pthread_mutex_unlock(&sendResourceMutex);
	}
}

void requestForResource() { //zadanie zasobu
	pthread_mutex_lock(&sendResourceMutex);
	char buff[200];

	if(currentConspiratorState->resourceOwner == currentConspiratorState->conspiratorData->id){
		int isEmpty;
		isEmpty = isQueueEmpty(currentConspiratorState->resourceRequestFifo);
		if(isEmpty == 1) {
			currentConspiratorState->usingResources = 1;
			currentConspiratorState->sendResourceRequest = 0;
		}
	} else {
		put(currentConspiratorState->resourceRequestFifo, currentConspiratorState->conspiratorData->id);
		//printf("Process[%d] after put\n", currentConspiratorState->conspiratorData->id);
		//printQueue(currentConspiratorState->resourceRequestFifo);
		if(currentConspiratorState->sendResourceRequest == 0) {
			sprintf(buff, "[RESOURCE_REQUEST][%d] process[%d] ask process[%d] for resource\n", lamportTime, currentConspiratorState->conspiratorData->id, currentConspiratorState->resourceOwner);
			printStatus(buff);
			int number = 1;
			//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = number;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);

			currentConspiratorState->sendResourceRequest = 1;
		}
	}

   	pthread_mutex_lock(&waitForResourceMutex);
   	while(currentConspiratorState->resourceOwner != currentConspiratorState->conspiratorData->id){
   		pthread_mutex_unlock(&sendResourceMutex);
   		pthread_cond_wait(&resourceCv, &waitForResourceMutex);
   	}
	pthread_mutex_unlock(&waitForResourceMutex);
	sprintf(buff, "[%d] Konspirator[%d] w posiadaniu zasobu\n", lamportTime, currentConspiratorState->conspiratorData->id);
	printStatus(buff);
}

void* acceptanceRequestListener(void* t) { //nasluchiwanie na prosby o zgode
	int number;
	MPI_Status status;
	while(1) {
		//MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		char buff[200];

		if(currentConspiratorState->conspiratorData->id == 0){
			sprintf(buff, "[%d] Akceptor[%d] otrzymuje zapytanie od Konspiratora[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
			printStatus(buff);
			//MPI_Send(&number, 1, MPI_INT, status.MPI_SOURCE, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = number;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, status.MPI_SOURCE, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD);
		} else {
			pthread_mutex_lock(&acceptanceFifoMutex);
			//printf("[start]put acceptanceRequestFifo\n");
			put(currentConspiratorState->acceptanceRequestFifo, status.MPI_SOURCE);
			//printf("[end]put acceptanceRequestFifo\n");
			pthread_mutex_unlock(&acceptanceFifoMutex);

			sprintf(buff, "[%d] Konspirator[%d] otrzymuje zapytanie od Konspiratora[%d] i przekazuje Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE, currentConspiratorState->conspiratorData->parent);
			printStatus(buff);
			//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->conspiratorData->parent, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = number;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->conspiratorData->parent, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD);
		}
	}
}

void* assignedAcceptanceListener(void* t) { //nasluchiwanie na uzyskane zgody
	int adress;
	MPI_Status status;
	while(1) {
		//MPI_Recv(&adress, 1, MPI_INT, MPI_ANY_SOURCE, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD, &status);
		adress = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		char buff[200];

		sprintf(buff, "[%d] Konspirator[%d] otrzymal zgode Akceptora[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, status.MPI_SOURCE);
		printStatus(buff);
		pthread_mutex_lock(&acceptanceFifoMutex);
		//printf("[start]pop acceptanceRequestFifo\n");
		sleep(3);
		//printArray(currentConspiratorState->acceptanceRequestFifo->elements, "request_fifo");
		int firstInQueue = pop(currentConspiratorState->acceptanceRequestFifo);
		//printf("[end]pop acceptanceRequestFifo\n");

		pthread_mutex_unlock(&acceptanceFifoMutex);

		if(firstInQueue == currentConspiratorState->conspiratorData->id){
			pthread_mutex_lock(&acceptanceReceiveMutex);
			currentConspiratorState->waitForAcceptance = 0;
			pthread_cond_signal(&acceptanceReceiveCv);
			pthread_mutex_unlock(&acceptanceReceiveMutex);
		} else {
			sprintf(buff, "[%d] Konspirator[%d] przekazuje zgode Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, firstInQueue);
			printStatus(buff);
			//MPI_Send(&adress, 1, MPI_INT, firstInQueue, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = adress;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, firstInQueue, ASSIGNED_ACCEPTANCE_TAG, MPI_COMM_WORLD);
		}
	}
}

void waitForAcceptorAcceptance() { //czekanie na zgode akceptora
	int number = 1;
	pthread_mutex_lock(&acceptanceFifoMutex);
   	put(currentConspiratorState->acceptanceRequestFifo, currentConspiratorState->conspiratorData->id);
	pthread_mutex_unlock(&acceptanceFifoMutex);
	char buff[200];
	sprintf(buff, "[%d] Konspirator[%d] oczekuje na zgode\n", lamportTime, currentConspiratorState->conspiratorData->id);
	printStatus(buff);
	//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->conspiratorData->id, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD);
	++lamportTime;
	msgToSend.data = number;
	msgToSend.lamportTime = lamportTime;
	MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->conspiratorData->id, ACCEPTANCE_REQUEST_TAG, MPI_COMM_WORLD);

	pthread_mutex_lock(&acceptanceReceiveMutex);
  	currentConspiratorState->waitForAcceptance = 1;
   	while(currentConspiratorState->waitForAcceptance ==1) {
   		pthread_cond_wait(&acceptanceReceiveCv, &acceptanceReceiveMutex);
   	}
	pthread_mutex_unlock(&acceptanceReceiveMutex);
	sprintf(buff, "[%d] Konspirator[%d] otrzymal zgode\n", lamportTime, currentConspiratorState->conspiratorData->id);
	printStatus(buff);
}

void waitForAllMeetingAck() {//czekanie na wszystkie potwierdzenia spotkania
	Conspirator* myConspirator = currentConspiratorState -> conspiratorData;
	int i;
	int number = -1;
	pthread_t transferListenerId  = createListener(meetingAcceptaneListener);
	char buff[200];

	for(i=0; i<myConspirator->siblingsCount; i++) {
		sprintf(buff, "[%d] Konspirator[%d] zaprasza Konspiratora[%d] na spotkanie\n", lamportTime, myConspirator->id, myConspirator->siblings[i]);
		printStatus(buff);
		//MPI_Send(&number, 1, MPI_INT, myConspirator->siblings[i], ORGANISE_MEETING_TAG, MPI_COMM_WORLD);
		++lamportTime;
		msgToSend.data = number;
		msgToSend.lamportTime = lamportTime;
		MPI_Send(&msgToSend, 1, mpiMsgType, myConspirator->siblings[i], ORGANISE_MEETING_TAG, MPI_COMM_WORLD);
	}

	void *status;
    pthread_join(transferListenerId, &status);
}

void takePartInMeeting() {//spotkanie wg uczestnika
		Conspirator* myConspirator = currentConspiratorState -> conspiratorData;
		int number = -1;
	   	MPI_Status status;
		char buff[200];
		sprintf(buff, "[%d] Konspirator[%d] czeka na poczatek spotkania\n", lamportTime, myConspirator->id);
   		printStatus(buff);
    	//MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, START_MEETING_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, START_MEETING_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);

		sprintf(buff, "[%d] Konspirator[%d] rozpoczyna spotkanie u Konspiratora[%d]\n", lamportTime, myConspirator->id, status.MPI_SOURCE);
		printStatus(buff);
   	 	//pal kuce
   		//MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, END_MEETING_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&msgToSend, 1, mpiMsgType, MPI_ANY_SOURCE, END_MEETING_TAG, MPI_COMM_WORLD, &status);
		number = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);

		sprintf(buff, "[%d] Konspirator[%d] konczy spotkanie u Konspiratora[%d]\n", lamportTime, myConspirator->id, status.MPI_SOURCE);
		printStatus(buff);
}

void organiseMeeting() {//spotkanie wg organizatora
	Conspirator* myConspirator = currentConspiratorState -> conspiratorData;
	int number = -1;
	int i;
	char buff[200];
	sprintf(buff, "[%d] Konspirator[%d] jest organizatorem\n", lamportTime, myConspirator->id);
   	printStatus(buff);

	waitForAcceptorAcceptance();
	requestForResource();
	//todo wait_for_resource

	for(i=0; i<myConspirator->siblingsCount; i++) {
		sprintf(buff, "[%d] Konspirator[%d] informuje o starcie spotkania Konspiratora[%d]\n", lamportTime, myConspirator->id, myConspirator->siblings[i]);
		printStatus(buff);
		//MPI_Send(&number, 1, MPI_INT, myConspirator->siblings[i], START_MEETING_TAG, MPI_COMM_WORLD);
		++lamportTime;
		msgToSend.data = number;
		msgToSend.lamportTime = lamportTime;
		MPI_Send(&msgToSend, 1, mpiMsgType, myConspirator->siblings[i], START_MEETING_TAG, MPI_COMM_WORLD);
	}
	//todo byc moze inne procesy musza potwierdzic start i czekamy na ich ack
	sleep(10); //pal kuce
	for(i=0; i<myConspirator->siblingsCount; i++){
		sprintf(buff, "[%d] Konspirator[%d] informuje o zakonczeniu spotkania Konspiratora[%d]\n", lamportTime, myConspirator->id, myConspirator->siblings[i]);
		printStatus(buff);
		//MPI_Send(&number, 1, MPI_INT, myConspirator->siblings[i], END_MEETING_TAG, MPI_COMM_WORLD);
		++lamportTime;
		msgToSend.data = number;
		msgToSend.lamportTime = lamportTime;
		MPI_Send(&msgToSend, 1, mpiMsgType, myConspirator->siblings[i], END_MEETING_TAG, MPI_COMM_WORLD);
	}
	// release
	int firstInQueue;
	pthread_mutex_lock(&sendResourceMutex);
	currentConspiratorState->usingResources = 0;
	//printf("Process[%d] befor pop\n", currentConspiratorState->conspiratorData->id);
	//printQueue(currentConspiratorState->resourceRequestFifo);
	firstInQueue = pop(currentConspiratorState->resourceRequestFifo);
	if(firstInQueue != -1) {
		sprintf(buff, "[%d] Konspirator[%d] zdaje zasob na rzecz Konspiratora[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, firstInQueue);
		printStatus(buff);
   		//MPI_Send(&number, 1, MPI_INT, firstInQueue, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);
		++lamportTime;
		msgToSend.data = number;
		msgToSend.lamportTime = lamportTime;
		MPI_Send(&msgToSend, 1, mpiMsgType, firstInQueue, RESOURCE_ASSIGN_TAG, MPI_COMM_WORLD);

		currentConspiratorState->resourceOwner = firstInQueue;
   		currentConspiratorState->sendResourceRequest = 0;
   		if(isQueueEmpty(currentConspiratorState->resourceRequestFifo) == 0) {
			sprintf(buff, "[%d] Konspirator[%d] przekazuje zadanie zasobu Konspiratorowi[%d]\n", lamportTime, currentConspiratorState->conspiratorData->id, currentConspiratorState->resourceOwner);
			printStatus(buff);
   			//MPI_Send(&number, 1, MPI_INT, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = number;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->resourceOwner, RESOURCE_REQUEST_TAG, MPI_COMM_WORLD);
   			currentConspiratorState->sendResourceRequest = 1;
		}
	}
	pthread_mutex_unlock(&sendResourceMutex);
}

void findMeeting() { //wybieranie aktywnego spotkania
	Conspirator* myConspirator = currentConspiratorState -> conspiratorData;
	char buff[200];
	if(myConspirator->siblingsCount == 0) {
		sprintf(buff, "[%d] Konspirator[%d] nie moze byc na spotkaniu sam :(\n", lamportTime, myConspirator->id);
		printStatus(buff);
		return;
	}
	int number = -1;
	int i;
	waitForAllMeetingAck();

	sprintf(buff, "[%d] Konspirator[%d] otrzymal wszystkie potwierdzenia\n", lamportTime, myConspirator->id);
   	printStatus(buff);
   	if(myConspirator->id < getMinFromArray(myConspirator->siblings, myConspirator->siblingsCount)) {
   		organiseMeeting();
   	} else {
   		takePartInMeeting();
   	}
	sprintf(buff, "[%d] Spotkanie Konspiratora[%d] sie konczy\n", lamportTime, myConspirator->id);
   	printStatus(buff);
}

void bCastResourceOwner(int worldSize) {//przypisywanie zasobu
	int ownerPid = rand1ToBound(worldSize);
	char buff[200];
	sprintf(buff, "[%d] Konspirator[%d] posiada zasob\n", lamportTime, ownerPid);
	printStatus(buff);
	int i=1;
	for(i=1;i<worldSize;i++) {
		//MPI_Send(&ownerPid, 1, MPI_INT, i, RESOURCE_OWNER_TAG, MPI_COMM_WORLD);
		++lamportTime;
		msgToSend.data = ownerPid;
		msgToSend.lamportTime = lamportTime;
		MPI_Send(&msgToSend, 1, mpiMsgType, i, RESOURCE_OWNER_TAG, MPI_COMM_WORLD);
	}
}

int main(int argc, char** argv) {
	clearLogFile();
	lamportTime = 0;
	int provided;
 	MPI_Init_thread( 0, 0, MPI_THREAD_MULTIPLE, &provided );
 	checkThreadSupport(provided);
 	int worldRank;
	int worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	checkWorldSize(worldSize);

	MPI_Aint offsets[MSG_DATA];
	MPI_Datatype types[MSG_DATA] = { MPI_INT, MPI_UNSIGNED_LONG };
	int lenmgths[MSG_DATA] = { 1, 1 };
	offsets[0] = offsetof(msg, data);
	offsets[1] = offsetof(msg, lamportTime);
	MPI_Type_create_struct(MSG_DATA, lenmgths, offsets, types, &mpiMsgType);
	MPI_Type_commit(&mpiMsgType);

	pthread_mutex_init(&resourceMutex, NULL);
	pthread_mutex_init(&acceptanceFifoMutex,NULL);
	pthread_mutex_init(&acceptanceReceiveMutex,NULL);
	pthread_mutex_init(&sendResourceMutex, NULL);
	pthread_mutex_init(&waitForResourceMutex, NULL);

	pthread_cond_init(&acceptanceReceiveCv, NULL);
	pthread_cond_init(&resourceCv, NULL);

	currentConspiratorState = initConspirator(worldRank, worldSize);
	pthread_t resourceTransferListenerId  = createListener(resourceTransferListener);
	pthread_t acceptanceRequestListenerId  = createListener(acceptanceRequestListener);
	pthread_t assignedAcceptanceListenerId  = createListener(assignedAcceptanceListener);
	pthread_t resourceRequestListenerId = createListener(resourceRequestListener);
	pthread_t resourceAssignListenerId = createListener(resourceAssignListener);


	if (worldRank == 0) {
		bCastResourceOwner(worldSize);
	} else {
		int ownerPid;
		//MPI_Recv(&ownerPid, 1, MPI_INT, 0, RESOURCE_OWNER_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&msgToSend, 1, mpiMsgType, 0, RESOURCE_OWNER_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		ownerPid = msgToSend.data;
		lamportTime = lamportClock(lamportTime, msgToSend.lamportTime);
		//printf("Process %d received recource owner = %d from process 0\n", worldRank, ownerPid);
		if(ownerPid == worldRank){
			int berserkId = BERSERK_TAG;
			char buff[100];
			sprintf(buff, "[%d] Konspirator[%d] przesyla zasob Konspiratorowi[%d]\n", lamportTime, worldRank, currentConspiratorState->conspiratorData->parent);
			printStatus(buff);
			//MPI_Send(&berserkId, 1, MPI_INT, currentConspiratorState->conspiratorData->parent, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD);
			++lamportTime;
			msgToSend.data = berserkId;
			msgToSend.lamportTime = lamportTime;
			MPI_Send(&msgToSend, 1, mpiMsgType, currentConspiratorState->conspiratorData->parent, RESOURCE_TRANSFER_TAG, MPI_COMM_WORLD);
		}
	}

	sleep(1); //just to pretty printing print
	sleep(rand1ToBound(5)); //just to pretty printing print

	int k = 0;
	while(1) {
		if(worldRank != 0) {
			findMeeting();
		}
		sleep(rand1ToBound(5));
		k++;
		char buff[100];
		sprintf(buff, "[%d] Spotkanie nr %d zakonczone!\n", lamportTime, k);
		printStatus(buff);
	}

	void *status;
    pthread_join(resourceTransferListenerId, &status);
    pthread_join(acceptanceRequestListenerId, &status);
    pthread_join(assignedAcceptanceListenerId, &status);

	pthread_mutex_destroy(&acceptanceFifoMutex);
	pthread_mutex_destroy(&resourceMutex);
	pthread_mutex_destroy(&acceptanceReceiveMutex);
	pthread_cond_destroy(&acceptanceReceiveCv);

	MPI_Finalize();
}
