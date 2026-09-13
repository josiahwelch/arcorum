/*
 * @file arcorum.c
 * @author Josiah Welch
 * @brief the C source file containing the main routine for the Arcorum compiler.
 * @date August 28th, 2026
 */

#include <lexer.h>

int main() {
    const char src[] = "if \"yeller\"\n"
                       "let yeller = \"yel\" + \"ler\"\n"
                       "elif \"feller\"\n"
                       "const yello << fellow";
    token_t *tokens = lex(src, strlen(src));
    for (int i = 0; tokens[i].value != NULL; i++) {
        switch (tokens[i].type) {
            case TOK_EOF:
                printf("EOF: %s\n", tokens[i].value);
                break;
            case TOK_INVALID:
                printf("INVALID: %s\n", tokens[i].value);
                break;
            case TOK_IDENTIFIER:
                printf("IDENTIFIER: %s\n", tokens[i].value);
                break;
            case TOK_INTEGER:
                printf("INTEGER: %s\n", tokens[i].value);
                break;
            case TOK_FLOAT:
                printf("FLOAT: %s\n", tokens[i].value);
                break;
            case TOK_STRING:
                printf("STRING: %s\n", tokens[i].value);
                break;
            default:
                printf("MISC: %s\n", tokens[i].value);
                break;
        }
    }
}
