/*
 * @file lexer.c
 * @author Josiah Welch
 * @brief the C source file containing the lexical analyzer for the Arcorum compiler.
 * @date September 4th, 2026
 */

#include <lexer.h>

// Start, end, and scan pointers
static char *tok_start;
static char *tok_end;
static char *tok_scan;

static int string() {
    // Base case
    if (*tok_start != '"')
        return 0;
    if (tok_scan == tok_start)
        tok_scan++;
    for (;*tok_scan != '"' && tok_scan != tok_end; tok_scan++) // To ensure that there isn't a \ before the end quotation mark
        if (*tok_scan == '\\')
            tok_scan++;
    return *tok_scan == '"';
}

/*
 * @param a pointer to the source char array
 * @param size of source char array
 * @return a pointer to the lexed token array
 */
token_t *lex(char *src, ssize_t len) {
    // Handling token array
    const ssize_t block = (len - len % 4) / 4; // ~1/4 of the source length
    ssize_t tok_len = block;
    ssize_t tok_n = 0;
    token_t *tokens = malloc(sizeof(token_t) * tok_len);

    tok_start = src;
    tok_end = src + len;

    for (tok_scan = src; tok_scan - src < len; tok_scan++) {
        while ((*tok_start == ' ' || *tok_start == '\t' || *tok_start == '\n') && (src - tok_start) < len)
            tok_start++;
        if (tok_scan < tok_start)
            tok_scan = tok_start;

        // String handling
        if (string()) {
            tokens[tok_n].value = malloc(tok_scan - tok_start + 1);
            strncpy(tokens[tok_n].value, tok_start, tok_scan - tok_start);
            tokens[tok_n].type = TOK_STRING;
            tok_start = tok_scan;
            tok_n++;
        }
    }

    return tokens;
}