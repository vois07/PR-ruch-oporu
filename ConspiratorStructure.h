#ifndef CONSPIRATOR_STRUCTURE
#define CONSPIRATOR_STRUCTURE

typedef struct {
    int id;
    int parent;
    int* children;
    int* siblings;
    int childrenCount;
    int siblingsCount;
}Conspirator;

#endif
