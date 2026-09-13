/*
 * @file lexer.c
 * @author Josiah Welch
 * @brief the C header file containing the methods of the lexical analyzer for the Arcorum compiler.
 * @date August 28th, 2026
 */

#ifndef LEXER_H
#define LEXER_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    TOK_INVALID = 0,
    TOK_EOF,

    TOK_IDENTIFIER,
    TOK_INTEGER,
    TOK_FLOAT,
    TOK_CHAR,
    TOK_STRING,

    TOK_FN,
    TOK_LET,
    TOK_CONST,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_ELIF,
    TOK_WHILE,
    TOK_FOR,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_STRUCT,
    TOK_ENUM,
    TOK_IMPORT,
    TOK_TRUE,
    TOK_FALSE,

    TOK_U8,
    TOK_U16,
    TOK_U32,
    TOK_U64,
    TOK_I8,
    TOK_I16,
    TOK_I32,
    TOK_I64,
    TOK_BOOL,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,

    TOK_EQUAL,
    TOK_PLUS_EQUAL,
    TOK_MINUS_EQUAL,
    TOK_STAR_EQUAL,
    TOK_SLASH_EQUAL,

    TOK_EQUAL_EQUAL,
    TOK_BANG_EQUAL,
    TOK_LESS,
    TOK_LESS_EQUAL,
    TOK_GREATER,
    TOK_GREATER_EQUAL,

    TOK_BANG,
    TOK_AMP,
    TOK_AMP_AMP,
    TOK_PIPE,
    TOK_PIPE_PIPE,
    TOK_CARET,
    TOK_TILDE,
    TOK_SHIFT_LEFT,
    TOK_SHIFT_RIGHT,
    TOK_INC,
    TOK_DEC,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_COMMA,
    TOK_DOT
} ttype_t;

typedef struct {
    char *value;
    ssize_t value_len;
    ttype_t type;
} token_t;

/* Returns an owned TOK_EOF-terminated array, or NULL on failure. */
token_t *lex(char *src, ssize_t len);

#endif
