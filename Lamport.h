#ifndef _LAMPORT_H
#define _LAMPORT_H

typedef struct msg_s { //struktura wiadomosci: dane i zegar
  int data;
  unsigned long lamportTime;
}msg;

int lamportClock(unsigned long a, unsigned long b) { //zwracanie kolejnego zegara
  if(a>b) return ++a;
  else return ++b;
}

#endif
