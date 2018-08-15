#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Utils.h"
#include <pthread.h>
#include <mpi.h>

int rand1ToBound(int bound) {
	time_t tt;
	srand(time(&tt));
	return (rand() % (bound-1))+1; // <1,n)
}

void checkWorldSize(int size) {//wymagamy min 2 procesów
  if (size < 2) {
    fprintf(stderr, "World size must be greater than 1!\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
}

void checkThreadSupport(int provided) { //sprawdzamy wielowatkowosc
 	if (provided != MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "NO THREAD SUPPORT!!\n");
        exit(-1);
	}
}

pthread_t createListener(void* listener_function) {
	pthread_t t;
    int rc = pthread_create(&t, NULL, listener_function, (void *)t);
	if (rc){
	    printf("ERROR; return code from pthread_create() is %d\n", rc);
	    exit(-1);
	}
    return t;
}

void printArray(int * arr, char* arr_name, int size) {
    printf("%s = ", arr_name);
    if(arr == NULL) {
        printf("none ");
        return;
    }

    int len = size;
    int i = 0;
    for(i=0; i<len; i++) {
        printf("%d, ", arr[i]);
    }
}

int getMinFromArray(int* arr, int size) {
	if(arr == NULL) return -1;

	int i=0;
	int min = arr[0];
	for(i=1; i<size; i++) {
		if(arr[i] < min) {
			min = arr[i];
		}
	}
	return min;
}
