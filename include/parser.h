/*
 * @file parser.h
 * @author Josiah Welch
 * @brief the C header file containing the methods for the parser of the Arcorum compiler.
 * @date October 13th, 2026
 */

#ifndef ARCORUM_PARSER_H
#define ARCORUM_PARSER_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <lexer.h>

typedef struct node_t node_t;

typedef enum {
    NODE_BINARY,
    NODE_UNARY,
    NODE_LEAF
} node_type_t;

struct node_t {
    node_type_t type;
    token_t *token;
    node_t *left;
    node_t *right;
};

#endif //ARCORUM_PARSER_H
