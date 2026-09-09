/*
 * @file lexer.c
 * @author Josiah Welch
 * @brief the C source file containing the lexical analyzer for the Arcorum compiler.
 * @date September 4th, 2026
 */

#include <lexer.h>

/*
 * @param a pointer to the source char array
 * @param size of source char array
 * @return a pointer to the lexed token array
 */
token_t *lex(char *src, ssize_t len) {
    // Handling token array
    ssize_t const block = (len - len % 4) / 4; // ~1/4 of the source length
    ssize_t tok_len = block;
    ssize_t tok_n = 0;
    token_t *tokens = malloc(sizeof(token_t) * tok_len);

    // Start and scan pointers
    char *tok_start = src;
    char *tok_scan = src;

    for (int i = 0; i < tok_len; i++) {}

    return tokens;
}