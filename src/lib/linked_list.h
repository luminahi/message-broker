#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

typedef struct ListNode {
    int fd;
    struct ListNode* next;
} ListNode;

typedef struct LinkedList {
    ListNode* HEAD;
    int length;
} LinkedList;

void insert_one(LinkedList* list, int fd);

ListNode* remove_one(LinkedList* list, int fd);

void print_list(LinkedList* list);

void destroy_list(LinkedList* list);

LinkedList* init_list();

#endif
