#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

static ListNode* create_node(int fd) {
    ListNode* node = (ListNode*) malloc(sizeof(ListNode));
    node->fd = fd;
    node->next = NULL;
    return node;
}

void insert_one(LinkedList* list, int fd) {
    ListNode* node = create_node(fd);
    node->next = list->HEAD;
    list->HEAD = node;
    list->length++;
}

ListNode* remove_one(LinkedList* list, int fd) {
    if (!fd || list->length < 1) return NULL;

    ListNode* previous = NULL;
    ListNode* current = list->HEAD;

    while (current) {
        if (current->fd == fd) {
            ListNode* aux = current;
            if (previous) previous->next = current->next;
            else list->HEAD = list->HEAD->next;
            list->length--;

            return aux;
        }

        previous = current;
        current = current->next;
    }

    return NULL;
}

void destroy_list(LinkedList* list) {
    ListNode* aux = NULL;

    while (list->HEAD) {
        aux = list->HEAD;
        list->HEAD = list->HEAD->next;
        free(aux);
        list->length = 0;
    }
}

void print_list(LinkedList* list) {
    ListNode* aux = list->HEAD;
    
    while (aux) {
        printf("[%d]-> ", aux->fd);
        aux = aux->next;
    }
    printf("NULL\n");
}

void iterate_list(LinkedList* list, void (*fn)()) {
    ListNode* aux = list->HEAD;

    while (aux) {

        aux = aux->next;        
    }
}

LinkedList* init_list() {
    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));
    list->HEAD = NULL;
    list->length = 0;

    return list;
}
