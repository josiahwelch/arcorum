/*
 * @file arcorum.c
 * @author Josiah Welch
 * @brief the C source file containing the main routine for the Arcorum compiler.
 * @date August 28th, 2026
 */

#include <lexer.h>
#include <parser.h>

void print_ast(node_t *node) {
    // Corresponds to node_type_t enum
    char *type_to_str[] = {
        "NODE_BINARY",
        "NODE_UNARY",
        "NODE_LEAF",
        "NODE_INDEX",
        "NODE_MEMBER",
        "NODE_CALL",
        "NODE_CHAIN",
        "NODE_EXPR_STATEMENT",
        "NODE_BLOCK",
        "NODE_IF",
        "NODE_ELIF",
        "NODE_ELSE",
        "NODE_WHILE"
    };
    if (node->type == NODE_CALL) {
        printf("token(%u): \"%s\", type: %s, args:", node->token->type, node->token->value, type_to_str[node->type]);
        for (size_t i = 0; i < node->argc; i++)
            print_ast(node->args[i]);
    }
    printf("token(%u): \"%s\", type: %s\n", node->token->type, node->token->value, type_to_str[node->type]);
    if (node->left != NULL)
        print_ast(node->left);
    if (node->right != NULL)
        print_ast(node->right);
}

int main() {
    char src[] = "import core.math;\n"
                 "enum Color { red, green, blue }\n"
                 "struct Point { x: i32, y: i32 }\n"
                 "fn add(a: i32, b: i32): i32 {\n"
                 "\treturn a + b;\n"
                 "}\n"
                 "fn main(): i32 {\n"
                 "\tlet byte: u8 = 1;\n"
                 "\tlet word: u16 = 2;\n"
                 "\tlet dword: u32 = 3;\n"
                 "\tlet qword: u64 = 4;\n"
                 "\tlet tiny: i8 = 5;\n"
                 "\tlet small: i16 = 6;\n"
                 "\tlet number: i32 = 7;\n"
                 "\tlet large: i64 = 8;\n"
                 "\tlet enabled: u8 = 1;\n"
                 "\tlet letter: u8 = 'a';\n"
                 "\tlet message: *u8 = \"yello\";\n"
                 "\tlet item: i8 = values[0];\n"
                 "\tnumber = 1 + 2 - 3 * 4 / 5 % 6;\n"
                 "\tnumber += 1;\n"
                 "\tnumber -= 1;\n"
                 "\tnumber *= 2;\n"
                 "\tnumber /= 2;\n"
                 "\tnumber++;\n"
                 "\tnumber--;\n"
                 "\tnumber = number << 1 >> 1;\n"
                 "\tnumber = number & 7 | 8 ^ ~9;\n"
                 "\tif (number == 1 && number != 2 || !0) {\n"
                 "\tnumber = number < 10 + number <= 10;\n"
                 "\t} elif (number > 0 && number >= 0) {\n"
                 "\tconst fixed: i32 = number;\n"
                 "\t} else {\n"
                 "\twhile (number) { break; }\n"
                 "\t}\n"
                 "\tfor (let i: i32 = 0; i < 10; i++) { continue; }\n"
                 "\tlet nothing: *u8 = 0;\n"
                 "\tlet ptr: *i32 = &number;\n"
                 "\t*ptr = 9;\n"
                 "\tlet first: u8, second: u16, third: i32;\n"
                 "\treturn point.x;\n"
                 "}";
    token_t *tokens = lex(src, strlen(src));
    printf("LEXER OUTPUT:\n");
    for (int i = 0; tokens[i].value != NULL; i++) {
        printf("%zd:%zd ", tokens[i].line, tokens[i].col);
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
                printf("%u: %s\n", tokens[i].type, tokens[i].value);
                break;
        }
    }
    print_ast(parse(tokens));
}
