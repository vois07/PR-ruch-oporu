#ifndef FIFO_H
#define FIFO_H

typedef struct {
    int elements[10]; //kolejka pomiesci n-1 
    int start;
    int end;
}fifo;

int put(fifo* queue, int elem); //dodawanie na koncu
int pop(fifo* queue); //usuwanie ostatniego
int isQueueEmpty(fifo* queue);
void printQueue(fifo *queue); //pomocnicza

#endif