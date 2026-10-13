/*
 * @file parser.c
 * @author Josiah Welch
 * @brief the C source file containing the parser for the Arcorum compiler.
 * @date October 13th, 2026
 */

#include <parser.h>

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