#include <defs.h>
#include <linkedListADT.h>
#include <memoryManagement.h>
#include <stdlib.h>


typedef struct linked_list_CDT {
    node *first;
    node *last;
    node *current;
    int len;
} linked_list_CDT;

linked_list_ADT create_linked_list_ADT() {
    linked_list_ADT list = (linked_list_ADT) mm_malloc(sizeof(linked_list_CDT));
    list->len = 0;
    list->first = NULL;
    list->last = NULL;
    list->current = NULL;
    return list;
}

node *append_element(linked_list_ADT list, void *data) {
    if (list == NULL)
        return NULL;
    node *new_node = (node *) mm_malloc(sizeof(node));
    new_node->data = data;
    return append_node(list, new_node);
}

node *append_node(linked_list_ADT list, node *node) {
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

node *prepend_node(linked_list_ADT list, node *node) {
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

node *get_first(linked_list_ADT list) {
    if (list == NULL)
        return NULL;
    return list->first;
}

int is_empty(linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return !list->len;
}

int get_length(linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return list->len;
}

void *remove_node(linked_list_ADT list, node *node) {
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
    //mm_free(node);//?
    return data;
}

void *remove_node_by_data(linked_list_ADT list, void *data) {
    if (list == NULL || data == NULL)
        return NULL;

    node *current = list->first;

    while (current != NULL) {
        if (current->data == data) {
            void *removed_data = remove_node(list, current);
            mm_free(current);
            return removed_data;
        }
        current = current->next;
    }

    return NULL; // El dato no fue encontrado en la lista
}


void begin(linked_list_ADT list) {
    if (list == NULL)
        return;
    list->current = list->first;
}

int has_next(linked_list_ADT list) {
    if (list == NULL)
        return -1;
    return list->current != NULL;
}

void *next(linked_list_ADT list) {
    if (!has_next(list))
        return NULL;
    void *data = list->current->data;
    list->current = list->current->next;
    return data;
}

void free_linked_list_ADT_deep(linked_list_ADT list) {
    node *current = list->first;
    node *next;
    while (current != NULL) {
        next = current->next;
        mm_free(current);
        current = next;
    }
    mm_free(list);
}
