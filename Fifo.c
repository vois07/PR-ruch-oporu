#include <stdio.h>
#include <stdlib.h>
#include "Fifo.h"
#include "Utils.h"

int put(fifo* queue, int elem) {
	if(queue->end == queue->start) {//gdy pusta
		queue->elements[queue->end] = elem;
		queue->end = (queue->end +1)%10;
	} else {
		if((queue->end+1)%10 == queue->start) {
			return -1; //gdy pelna
		} else {
			queue->elements[queue->end] = elem;
			queue->end = (queue->end +1)%10;
		}
	}
	return 1;
}

int pop(fifo* queue) {
	if(queue->start == queue->end) {
		return -1; //gdy pusta
	}
	int first = queue->elements[queue->start];
	queue->start = (queue->start +1)%10;
	return first;
}

int isQueueEmpty(fifo* queue) {
	if(queue->end == queue->start){
		return 1; //gdy pusta
	} else {
		return 0;
	}
}

void printQueue(fifo* queue){
	int i = queue->start;
	int j = queue->end;
	int elem;
	while(i != j) {
		elem = queue->elements[i];
		i = (i+1)%10;
		printf("%d, ", elem);
	}
	printf("\n");
} 
