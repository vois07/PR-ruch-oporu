#ifndef RAND_UTILS_H
#define RAND_UTILS_H

#include <pthread.h>

int rand1ToBound(int bound); //losowanie
int getMinFromArray(int* arr, int size);
void printArray(int * arr, char* arr_name, int size);
pthread_t createListener(void* listener_function);
void checkWorldSize(int size);
void checkThreadSupport(int provided);
#endif
