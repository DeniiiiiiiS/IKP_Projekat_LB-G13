#pragma once
#include <winsock2.h>

struct Queue {
    unsigned short capactity, size, head, tail;
    int* array;
};

//FUNCKIJA KOJA VRACA KREIRANI QUEUE
Queue* CreateQueue(unsigned short capacity);

//FUNKCIJA ZA DODAVANJE PODATKA U QUEUE
bool AddElem(struct Queue* queue, int data);

//FUNKCIJA KOJA VRACA PODATAK IZ QUEUE
int GetData(struct Queue *queue);

//FUNKCIJA ZA PROVERU DA LI JE QUEUE PRAZAN
bool IsEmpty(struct Queue* queue);

//FUNKCIJA ZA PROVERU DA LI JE QUEU POPUNJEN
bool IsFull(struct Queue* queue);

//FUNKCIJA ZA OSLOBADJENJE QUEUE
void FreeQueue(struct Queue* queue);

