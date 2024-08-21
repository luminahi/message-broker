#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "queue.h"

static Node* create_node(char* message) {
  Node* node = (Node*) malloc(sizeof(Node));
  node->message = (char*) malloc(sizeof(char) * 128);
  
  strcpy(node->message, message);
  node->prev = NULL;
  node->next = NULL;

  return node;
}

void enqueue(Queue* queue, char* name) {
  Node* node = create_node(name);

  if (queue->size == 0) {
    queue->head = node;
  } else {
    queue->tail->prev = node;
    node->next = queue->tail;
  }

  queue->tail = node;
  queue->size++;
}

void dequeue(Queue* queue) {
  if (queue->size <= 0)
    return;
  Node* temp = queue->head;

  if (queue->size == 1) {
    queue->head = NULL;
    queue->tail = NULL;
  } else {
    queue->head->prev->next = NULL;
    queue->head = queue->head->prev;
  }

  queue->size--;
  free(temp);
}

void clear_queue(Queue* queue) {
  while (queue->head) {
    dequeue(queue);
  }
}

void print_queue(Queue queue) {
  Node* temp = queue.tail;

  while (temp) {
    printf("[%s] -> ", temp->message);
    temp = temp->next;
  }

  putchar('\n');
}

Queue* init_queue() {
  Queue* queue = (Queue*)malloc(sizeof(Queue));

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  return queue;
}

void destroy_queue(Queue* queue) {
  if (queue->size > 0) clear_queue(queue);
  free(queue);
}
