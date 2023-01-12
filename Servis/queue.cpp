#include <winsock2.h>
#include "queue.h"


//TO DO
//IMPLEMENTIRATI FUNKCIJE ZA QUEUE



Queue* CreateQueue(unsigned short capacity) {
	struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));

	queue->capactity = capacity;
	queue->size = 0;
	queue->head = 0;
	queue->tail = 0;
	queue->array = (int*)malloc(sizeof(int) * capacity);

	return queue;
}

bool IsFull(struct Queue* queue) {
	if (queue->size == queue->capactity)
		return true;
	return false;
}

//TO DO
//ISTO KAO I IS FULL SAMO DA LI JE SIZE 0
bool IsEmpty(struct Queue* queue) {}

bool AddElem(struct Queue* queue, int data) {
	if (IsFull(queue))
		return false;

	queue->array[queue->tail] = data;
	queue->size = queue->size + 1;
	queue->tail = queue->tail + 1;
	if (queue->size == queue->capactity)
		queue->size = 0;

	return true;
}


//TO DO
//FUNKCIJA KOJA VRACA ZADNJI PODATAK IZ QUEUE
int GetData(struct Queue* queue);

