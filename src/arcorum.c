/*
 * @file arcorum.c
 * @author Josiah Welch
 * @brief the C source file containing the main routine for the Arcorum compiler.
 * @date August 28th, 2026
 */

#include <lexer.h>

int main() {
    token_t *tokens;
    const char src[] = "\"yeller\"";
    tokens = lex(src, strlen(src));
    printf("%s\n", tokens[0].value);
}
