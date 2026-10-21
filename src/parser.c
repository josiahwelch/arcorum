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

// Forward definitions
static node_t *parse_expr(parser_t *parser, uint8_t min_prec);
static node_t *parse_statement(parser_t *parser);

// Helper functions
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

static void err_msg(token_t *token, const char *exp_msg, bool got_msg) {
    if (got_msg)
        if (token->type == TOK_EOF)
            fprintf(stderr, "expected %s, got end of file at %zd:%zd!\n", exp_msg, token->line, token->col);
        else
            fprintf(stderr, "expected %s, got '%s' at %zd:%zd!\n", exp_msg, token->value, token->line, token->col);
    else
        fprintf(stderr, "expected %s at %zd:%zd!\n", exp_msg, token->line, token->col);
}

static token_t *expect(parser_t *parser, ttype_t type, const char *exp_msg) {
    token_t *token = peek(parser);
    advance(parser);
    if (token->type == type)
        return token;
    err_msg(token, exp_msg, true);
    exit(1);
    return NULL;
}

static node_t *new_node(node_type_t type, token_t *token) {
    node_t *node = calloc(1, sizeof(node_t));
    if (node == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
        return NULL;
    }
    node->token = token;
    node->type = type;
    return node;
}

// Parser functions
static node_t *parse_primary(parser_t *parser) {
    token_t *current = peek(parser);
    advance(parser);
    if (current->type == TOK_LPAREN) {
        node_t *node = parse_expr(parser, 0);
        expect(parser, TOK_RPAREN, "')'");
        return node;
    }
    if ((current->type >= TOK_IDENTIFIER && current->type <= TOK_STRING)
        || current->type == TOK_TRUE || current->type == TOK_FALSE) {
        // TOK_IDENTIFIER, TOK_INTEGER, TOK_FLOAT, TOK_CHAR, TOK_STRING
        node_t *node = new_node(NODE_LEAF, current);
        return node;
    }
    err_msg(current, "an expression", true);
    exit(1);
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
    node_t *node = parse_primary(parser);
    node_t *new;
    for (token_t *token = peek(parser); token->type == TOK_LBRACKET || token->type == TOK_DOT || token->type == TOK_LPAREN; token = peek(parser)) {
        advance(parser);
        switch (token->type) {
            case TOK_LBRACKET: {
                node_t *index = parse_expr(parser, 0);
                expect(parser, TOK_RBRACKET, "']'");
                new = new_node(NODE_INDEX, token);
                new->left = node;
                new->right = index;
                node = new;
                break;
            }
            case TOK_DOT: {
                token_t *id = expect(parser, TOK_IDENTIFIER, "an identifier");
                new = new_node(NODE_MEMBER, token);
                new->left = node;
                new->right = new_node(NODE_LEAF, id);
                node = new;
                break;
            }
            case TOK_LPAREN: {
                new = new_node(NODE_CALL, token);
                new->left = node;
                if (peek(parser)->type == TOK_RPAREN)
                    advance(parser);
                else {
                    size_t size = 1;
                    while (1) {
                        node_t *arg = parse_expr(parser, 0);
                        new->argc++;
                        if (new->argc >= size) {
                            size *= 2;
                            new->args = realloc(new->args, sizeof(node_t *) * size);
                            if (new->args == NULL) {
                                fprintf(stderr, "Ran out of memory!\n");
                                exit(1);
                            }
                        }
                        new->args[new->argc - 1] = arg;
                        if (peek(parser)->type != TOK_COMMA)
                            break;
                        advance(parser);
                        if (peek(parser)->type == TOK_RPAREN)
                            break;
                    }
                    expect(parser, TOK_RPAREN, "')'");
                }
                node = new;
                break;
            }
            default: break;
        }
    }
    return node;
}

static node_t *parse_expr(parser_t *parser, uint8_t min_prec) {
    node_t *left = parse_unary(parser);
    token_t *op = peek(parser);
    while (precedence(op) >= min_prec && precedence(op)) {
        advance(parser);
        node_t *right = parse_expr(parser, precedence(op) + 1);
        node_t *node = new_node(precedence(op) == 3 && precedence(peek(parser)) == 3 ? NODE_CHAIN : NODE_BINARY, op);
        if (node->type == NODE_CHAIN) { //comparison
            size_t size = 4;
            node->args = calloc(1, sizeof(node_t *) * size);
            node->ops  = calloc(1, sizeof(token_t *) * size);
            if (node->args == NULL || node->ops == NULL) {
                fprintf(stderr, "Ran out of memory!\n");
                exit(1);
            }
            // First pair with operator
            node->args[0] = left;
            node->args[1] = right;
            node->ops[0]  = op;
            node->argc    = 2;
            while (precedence(peek(parser)) == 3) {
                token_t *next_op = peek(parser);
                advance(parser);
                node_t *arg = parse_expr(parser, 4);
                if (node->argc >= size) {
                    size *= 2;
                    node->args = realloc(node->args, sizeof(node_t *) * size);
                    node->ops = realloc(node->ops, sizeof(token_t *) * size);
                    if (node->args == NULL || node->ops == NULL) {
                        fprintf(stderr, "Ran out of memory!\n");
                        exit(1);
                    }
                }
                node->ops[node->argc - 1] = next_op;
                node->args[node->argc] = arg;
                node->argc++;
            }
        } else {
            node->left = left;
            node->right = right;
        }
        left = node;
        op = peek(parser);
    }
    return left;
}

static node_t *parse_expr_statement(parser_t *parser) {
    node_t *node = new_node(NODE_EXPR_STATEMENT, peek(parser));
    if (node->token->type == TOK_BREAK || node->token->type == TOK_CONTINUE) {
        advance(parser);
        expect(parser, TOK_SEMICOLON, "';'");
    } else if (node->token->type == TOK_RETURN) {
        advance(parser);
        if (peek(parser)->type != TOK_SEMICOLON && peek(parser)->type != TOK_EOF && peek(parser)->type != TOK_RBRACE)
            node->left = parse_expr(parser, 0);
        expect(parser, TOK_SEMICOLON, "';'");
    } else {
        node->left = parse_expr(parser, 0);
        expect(parser, TOK_SEMICOLON, "';'");
    }
    return node;
}

static node_t *parse_block(parser_t *parser) {
    node_t *node = new_node(NODE_BLOCK, peek(parser));
    size_t size = 1;
    expect(parser, TOK_LBRACE, "'{'");
    while (peek(parser)->type != TOK_RBRACE && peek(parser)->type != TOK_EOF) {
        node->argc++;
        if (node->argc >= size) {
            size *= 2;
            node->args = realloc(node->args, sizeof(node_t *) * size);
            if (node->args == NULL) {
                fprintf(stderr, "Ran out of memory!\n");
                exit(1);
            }
        }
        if (peek(parser)->type == TOK_LBRACE)
            node->args[node->argc - 1] = parse_block(parser);
        else
            node->args[node->argc - 1] = parse_statement(parser);
    }
    expect(parser, TOK_RBRACE, "'}'");
    return node;
}

static node_t *parse_statement(parser_t *parser) {
    switch (peek(parser)->type) {
        case TOK_LBRACE:
            return parse_block(parser);
        default:
            return parse_expr_statement(parser);
    }
}

node_t *parse(token_t *tokens) {
    parser_t parser;
    parser.tokens = tokens;
    parser.pos = 0;
    node_t *node = parse_statement(&parser);
    expect(&parser, TOK_EOF, "end of file");
    return node;
}