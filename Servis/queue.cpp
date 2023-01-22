#include <winsock2.h>
#include "queue.h"



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

bool IsEmpty(struct Queue* queue) {
	if (queue->size == 0){
		return true;
	}
	return false;

}

bool AddElem(struct Queue* queue, int data) {
	if (IsFull(queue))
		return false;

	queue->array[queue->tail] = data;
	queue->size = queue->size + 1;
	queue->tail = queue->tail + 1;
	if (queue->tail == queue->capactity)
		queue->tail = 0;

	return true;
}

int GetData(struct Queue* queue){

	if (IsEmpty(queue))
		return -1;

	int data = queue->array[queue->head];
	queue->size = queue->size - 1;
	queue->head = queue->head + 1;
	if (queue->head == queue->capactity)
		queue->head = 0;

	return data;
}

void FreeQueue(struct Queue* queue) {
	free(queue->array);
	free(queue);
}
