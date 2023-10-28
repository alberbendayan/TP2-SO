#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Linked_list_CDT *Linked_list_ADT;

typedef struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

Linked_list_ADT create_linked_list_ADT();
Node *append_element(Linked_list_ADT list, void *data);
Node *append_node(Linked_list_ADT list, Node *node);
Node *prepend_node(Linked_list_ADT list, Node *node);
void *remove_node(Linked_list_ADT list, Node *node);
Node *get_first(Linked_list_ADT list);
int is_empty(Linked_list_ADT list);
void begin(Linked_list_ADT list);
int has_next(Linked_list_ADT list);
void *next(Linked_list_ADT list);
void free_linked_list_ADT_deep(Linked_list_ADT list);
void free_linked_list_ADT(Linked_list_ADT list);
int get_length(Linked_list_ADT list);

#endif
