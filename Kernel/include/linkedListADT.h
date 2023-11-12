#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list_CDT *linked_list_ADT;

typedef struct node {
    void *data;
    struct node *prev;
    struct node *next;
} node;

linked_list_ADT create_linked_list_ADT();
node *append_element(linked_list_ADT list, void *data);
node *append_node(linked_list_ADT list, node *node);
node *prepend_node(linked_list_ADT list, node *node);
void *remove_node(linked_list_ADT list, node *node);
node *get_first(linked_list_ADT list);
int is_empty(linked_list_ADT list);
void begin(linked_list_ADT list);
int has_next(linked_list_ADT list);
void *next(linked_list_ADT list);
void free_linked_list_ADT_deep(linked_list_ADT list);
int get_length(linked_list_ADT list);
void *remove_node_by_data(linked_list_ADT list, void *data);

#endif
