#include "tree.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum state {
  NON_TERMINAL,
  TERMINAL
};

Node *json_to_tree(cJSON *json){
  // fnd symbol
  // search json key
  cJSON *keys = json->child;
  cJSON *values = NULL;
  while (keys != NULL) {
    if (!cJSON_IsArray(keys)){
      fprintf(stderr, "Error: grammar should be an array of strings");
      exit(1);
    }

    values = keys->child;
    while(values != NULL){
      if (!cJSON_IsString(values)){
        fprintf(stderr, "Error: grammar array should have only strings");
        exit(1);
      }

      fprintf(stderr, "%s\n", values->valuestring);
      values = values->next;
    }

    // fprintf(stderr, "DEBUGPRINT[1]: parser.c:15: keys=%s\n", keys->string);
    keys = keys->next;
  }
}

// takes a raw string and returns the list of non-terms in it
char **find_non_terms(char *str){
  // what should -<expr> generate?
  // node { "-", children: [] }
  // node { "<expr>", children: [ lookup on the json ] }

  // what should (<expr>) generate?
  // node { "(<expr>)", children: [ expr children ] }
  enum state s;
  int cursor = 0;
  int value_start = cursor;
  int symbols_capacity = 4;
  int symbols_length = 0;
  char **symbols = calloc(symbols_capacity, sizeof(char *));

  while(1) {
    switch (str[cursor]){
      case '<':
        // handle <.......\0
        if (!strchr(&str[cursor], '>')) {
          fprintf(stderr, "Error: No matching closing tag on non-term");
          exit(1);
        }

        // handle <>
        if (str[cursor + 1] == '>') {
          fprintf(stderr, "Error: Non-term can't be empty");
          exit(1);
        }

        value_start = cursor;
        while (str[cursor] != '>') {
          cursor++;
        }
        // include >
        cursor++;
        int buf_size = cursor - value_start;
        char *symbol = malloc(buf_size + 1);
        strncpy(symbol, &str[value_start], buf_size);
        symbol[buf_size + 1] = '\0';

        if (symbols_length >= symbols_capacity) {
          char **tmp = calloc(symbols_capacity * 2, sizeof(char *));
          memcpy(symbols, tmp, sizeof(char *) * symbols_capacity);
          symbols = tmp;
          symbols_capacity = symbols_capacity * 2;
        };

        symbols[symbols_length] = symbol;
        symbols_length++;
      case '\0':
        for (int i = 0; i < symbols_length; i++) 
             fprintf(stderr, "DEBUGPRINT[7]: parser.c:64:=%s\n", symbols[i]);
        return symbols;
      default:
        cursor++;
        continue;
    }
  }
}

void parse_children(cJSON *child) {
  if(!cJSON_IsString(child)){
    fprintf(stderr, "Invalid grammar, expecting string");
    exit(1);
  }

  char *curr = child->valuestring;
  find_non_terms(curr);
  return;
  while (child != NULL) {
    // actual parser logic
    // fprintf(stderr, "DEBUGPRINT[1]: parser.c:15: curr=%s\n", curr);

    child = child->next;
  }
};
