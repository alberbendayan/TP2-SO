// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <linkedList.h>
#include <memoryManagement.h>
#include <stdlib.h>


typedef struct Linked_list_CDT {
    Node *first;
    Node *last;
    Node *current;
    int len;
} Linked_list_CDT;

Linked_list_ADT create_linked_list_ADT() {
    Linked_list_ADT list = (Linked_list_ADT) alloc_memory(sizeof(Linked_list_CDT));
    list->len = 0;
    list->first = NULL;
    list->last = NULL;
    list->current = NULL;
    return list;
}

Node *append_element(Linked_list_ADT list, void *data) {
    if (list == NULL)
        return NULL;
    Node *newNode = (Node *) alloc_memory(sizeof(Node));
    newNode->data = data;
    return append_node(list, newNode);
}

Node *append_node(Linked_list_ADT list, Node *node) {
    if (list == NULL)
        return NULL;
    node->next = NULL;
    if (list->len > 0)
        list->last->next = node;
    else
        list->first = node;
    node->prev = list->last;
    list->last = node;
    list->len++;
    return node;
}

Node *prepend_node(Linked_list_ADT list, Node *node) {
    if (list == NULL)
        return NULL;
    node->prev = NULL;
    if (list->len > 0)
        list->first->prev = node;
    else
        list->last = node;
    node->next = list->first;
    list->first = node;
    list->len++;
    return node;
}

Node *get_first(Linked_list_ADT list) {
    if (list == NULL)
        return NULL;
    return list->first;
}

int is_empty(Linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return !list->len;
}

int get_length(Linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return list->len;
}

void *remove_node(Linked_list_ADT list, Node *node) {
    if (list == NULL || node == NULL)
        return NULL;

    if (list->first == node)
        list->first = node->next;
    else
        node->prev->next = node->next;

    if (list->last == node)
        list->last = node->prev;
    else
        node->next->prev = node->prev;

    list->len--;
    void *data = node->data;
    node->next = NULL;
    node->prev = NULL;
    // free(node);
    return data;
}

// Atención: Usar funciones de agregado/borrado cuando se itera sobre la lista
// puede causar comportamiento indefinido.
void begin(Linked_list_ADT list) {
    if (list == NULL)
        return;
    list->current = list->first;
}

int has_next(Linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return list->current != NULL;
}

void *next(Linked_list_ADT list) {
    if (!has_next(list))
        return NULL;
    void *data = list->current->data;
    list->current = list->current->next;
    return data;
}

void free_linked_list_ADT_deep(Linked_list_ADT list) {
    Node *current = list->first;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free_linked_list_ADT(list);
}

void free_linked_list_ADT(Linked_list_ADT list) {
    free(list);
}
