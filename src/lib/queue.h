#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct Node {
  char* message;
  struct Node* next;
  struct Node* prev;
} Node;

typedef struct Queue {
  struct Node* head;
  struct Node* tail;
  int size;
} Queue;

void enqueue(Queue*, char*);

void dequeue(Queue*);

void print_queue(Queue);

void clear_queue(Queue*);

Queue* init_queue();

void destroy_queue(Queue*);

#endif
