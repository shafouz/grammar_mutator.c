#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <graphviz/gvcext.h>
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <time.h>
#include "tree.h"
#include "parser.h"

void read_file(char **buf, const char *filename){
  FILE *p = fopen(filename, "r");
  if (p == NULL) {
    fprintf(stderr, "Error opening file %s\n", filename);
    exit(1);
  }

  fseek(p, 0L, SEEK_END);
  long size = ftell(p);
  rewind(p);

  *buf = (char *)malloc(size + 1);

  if (buf == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }

  fread(*buf, 1, size, p);
  (*buf)[size] = '\0';
  fclose(p);
}


struct node_manager nm = {
  .index = 0,
  .pool = {}
};

Node *init_node(char *val){
  Node expr = {
    .symbol = val,
    .children = NULL,
    .length = 0,
    .capacity = 4
  };

  // init arr
  expr.children = calloc(expr.capacity, sizeof(Node));

  if (nm.index >= 99) {
    fprintf(stderr, "Too many nodes ( 100 )");
    exit(1);
  }

  nm.pool[nm.index] = expr;
  Node *ret = &nm.pool[nm.index];
  nm.index++;

  return ret;
}

// alloc n amount of nodes at first
// check every time and reallocate if it doesnt fit
void add_child(Node *parent, Node *child){
  if (parent->length >= parent->capacity) {
    int new_capacity = parent->capacity * 2;
    Node **tmp = calloc(new_capacity, sizeof(Node));
    memcpy(tmp, parent->children, sizeof(Node) * parent->capacity);

    parent->children = tmp;
    parent->capacity = new_capacity;
  }


  parent->children[parent->length] = child;
  parent->length++;
}

void random_str(char *buf, size_t size){
  const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            buf[n] = charset[key];
        }
        buf[size] = '\0';
    }
}

void build_graph(Agraph_t *graph, Agnode_t *parent, Node *node){
  // generate random id
  char buf[8];
  random_str(buf, 8);

  // create node
  Agnode_t *curr = agnode(graph, buf, 1);
  agsafeset(curr, "label", node->symbol, "");

  // link to parent
  if (parent != NULL) {
    agedge(graph, parent, curr, 0, 1);
  }

  // do the same for every children
  Node *child = NULL;
  for (int i = 0; i < node->length; i++) {
    child = node->children[i];
    if (child->children == NULL) {
      continue;
    }
    build_graph(graph, curr, child);
  }
}

int display_tree(Node *tree){
  GVC_t *gvc = gvContext();
  Agraph_t *graph = agopen(tree->symbol, Agdirected, 0);
  build_graph(graph, NULL, tree);
  gvLayout(gvc, graph, "dot");
  gvRenderFilename(gvc, graph, "png", "out.png");
  gvFreeLayout(gvc, graph);
  agclose(graph);
  return gvFreeContext(gvc);
}

void example_tree(){
  Node *start = init_node("start");
  Node *expr = init_node("expr");
  Node *term = init_node("2");
  Node *plus = init_node("+");
  Node *term2 = init_node("2");

  add_child(start, expr);
  add_child(expr, term);
  add_child(expr, plus);
  add_child(expr, term2);

  display_tree(start);
}

int main(int argc, char *argv[])
{
  char *buf;
  read_file(&buf, "./grammar.json");
  cJSON *json = cJSON_Parse(buf);
  Node *tree = json_to_tree(json);


  // if(!cJSON_IsString(keys->child)){
  //   fprintf(stderr, "Invalid grammar, expecting string");
  //   exit(1);
  // }

  // Node *root = init_node("root");
  //
  // cJSON *curr = json->child;
  // cJSON *tmp1 = cJSON_GetObjectItem(json, "<expr>");
  // parse_children(curr->child);
  // while (curr != NULL) {
  //   if (!cJSON_IsArray(curr)){
  //     fprintf(stderr, "Invalid grammar, expecting array");
  //     exit(1);
  //   }
  //
  //   // Node *node = init_node(curr->string);
  //   parse_children(curr->child);
  //
  //
  //   // add_child(root, init_node(curr->string));
  //   curr = curr->next;
  // }

  // display_tree(root);

  return 0;
}
