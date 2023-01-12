#pragma once
#include <winsock2.h>

struct Queue {
    unsigned short capactity, size, head, tail;
    int* array;
};

//TO DO
//FUNCKIJA KOJA VRACA KREIRANI QUEUE
Queue* CreateQueue(unsigned short capacity);


//TO DO
//UBACI PODATAK U QUEUE NA POCETAK
bool AddElem(struct Queue* queue, int data);


//TO DO
//FUNKCIJA KOJA VRACA ZADNJI PODATAK IZ QUEUE
int GetData(struct Queue *queue);

bool IsEmpty(struct Queue* queue);

bool IsFull(struct Queue* queue);

void FreeQueue(struct Queue* queue);

