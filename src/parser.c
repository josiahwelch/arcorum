/*
 * @file parser.c
 * @author Josiah Welch
 * @brief the C source file containing the parser for the Arcorum compiler.
 * @date October 13th, 2026
 */

#include <parser.h>

typedef struct {
    token_t *tokens;
    ssize_t pos;
} parser_t;

static node_t *parse_expr(parser_t *parser, uint8_t min_prec);

static uint8_t precedence(const token_t *token) {
    switch (token->type) {
        case TOK_PIPE_PIPE:                         return 1;
        case TOK_AMP_AMP:                           return 2;
        case TOK_EQUAL_EQUAL: case TOK_BANG_EQUAL:
        case TOK_LESS_EQUAL: case TOK_GREATER_EQUAL:
        case TOK_LESS: case TOK_GREATER:            return 3;
        case TOK_PLUS: case TOK_MINUS:
        case TOK_PIPE: case TOK_CARET:              return 4;
        case TOK_STAR: case TOK_SLASH:
        case TOK_SHIFT_LEFT: case TOK_SHIFT_RIGHT:
        case TOK_PERCENT: case TOK_AMP:             return 5;
        default:                                    return 0;
    }
}

static token_t *peek(const parser_t *parser) {
    return &parser->tokens[parser->pos];
}

static token_t *advance(parser_t *parser) {
    if (peek(parser)->type != TOK_EOF) parser->pos++;
    return peek(parser);
}

static node_t *new_node(node_type_t type, token_t *token) {
    node_t *node = calloc(1, sizeof(node_t));
    if (node == NULL)
        return NULL;
    node->token = token;
    node->type = type;
    return node;
}

static node_t *parse_primary(parser_t *parser) {
    token_t *current = peek(parser);
    advance(parser);
    if (current->type == TOK_LPAREN) {
        node_t *node = parse_expr(parser, 0);
        advance(parser);
        if (peek(parser)->type == TOK_RPAREN)
            return node;
        fprintf(stderr, "expected ')' at %ld:%ld!\n", peek(parser)->line, peek(parser)->col);
        return NULL;
    }
    if ((current->type >= TOK_IDENTIFIER && current->type <= TOK_STRING)
        || current->type == TOK_TRUE || current->type == TOK_FALSE) {
        // TOK_IDENTIFIER, TOK_INTEGER, TOK_FLOAT, TOK_CHAR, TOK_STRING
        node_t *node = new_node(NODE_LEAF, current);
        advance(parser);
        return node;
    }
    fprintf(stderr, "expected an expression, got ‘%s’ at %ld:%ld\n", current->value, current->line, current->col);
    return NULL;
}

static node_t *parse_unary(parser_t *parser) {
    token_t *current = peek(parser);
    if (current->type == TOK_MINUS || current->type == TOK_BANG || current->type == TOK_TILDE) {
        advance(parser);
        node_t *node = new_node(NODE_UNARY, current);
        node->left = parse_unary(parser);
        return node;
    }
    return parse_primary(parser);
}

static node_t *parse_expr(parser_t *parser, uint8_t min_prec) {
    node_t *left = parse_unary(parser);
    token_t *op = peek(parser);
    while (precedence(op) >= min_prec && precedence(op)) {
        advance(parser);
        node_t *right = parse_expr(parser, precedence(op) + 1);
        node_t *node = new_node(NODE_BINARY, op);
        node->left = left;
        node->right = right;
        left = node;
        op = peek(parser);
    }
    return left;
}

node_t *parse(token_t *tokens) {
    parser_t parser;
    parser.tokens = tokens;
    parser.pos = 0;
    node_t *node = parse_expr(&parser, 0);
    if (peek(&parser)->type != TOK_EOF) {
        fprintf(stderr, "Unrecognized token!\n");
        return NULL;
    }
    return node;
}