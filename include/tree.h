#ifndef NODE_H
#define NODE_H

typedef struct node {
    char *symbol;
    struct node **children;
    int length;
    int capacity;
} Node;

typedef struct node_manager {
    int index;
    Node pool[100];
} NodeManager;

Node *init_node(char *val);
void add_child(Node *parent, Node *child);

#endif
