/*
 * @file arcorum.c
 * @author Josiah Welch
 * @brief the C source file containing the main routine for the Arcorum compiler.
 * @date August 28th, 2026
 */

#include <lexer.h>

int main() {
    char src[] = "import core.math;\n"
                 "enum Color { red, green, blue }\n"
                 "struct Point { x: i32, y: i32 }\n"
                 "fn add(a: i32, b: i32): i32 { return a + b; }\n"
                 "fn main(): i32 {\n"
                 "    let byte: u8 = 1;\n"
                 "    let word: u16 = 2;\n"
                 "    let dword: u32 = 3;\n"
                 "    let qword: u64 = 4;\n"
                 "    let tiny: i8 = 5;\n"
                 "    let small: i16 = 6;\n"
                 "    let number: i32 = 7;\n"
                 "    let large: i64 = 8;\n"
                 "    let enabled: bool = true;\n"
                 "    let letter = 'a';\n"
                 "    let message = \"yello\";\n"
                 "    let decimal = 1.25;\n"
                 "    let item = values[0];\n"
                 "    number = 1 + 2 - 3 * 4 / 5 % 6;\n"
                 "    number += 1;\n"
                 "    number -= 1;\n"
                 "    number *= 2;\n"
                 "    number /= 2;\n"
                 "    number++;\n"
                 "    number--;\n"
                 "    number = number << 1 >> 1;\n"
                 "    number = number & 7 | 8 ^ ~9;\n"
                 "    if (number == 1 && number != 2 || !false) {\n"
                 "        number = number < 10 + number <= 10;\n"
                 "    } elif (number > 0 && number >= 0) {\n"
                 "        const fixed: i32 = number;\n"
                 "    } else {\n"
                 "        while (number) { break; }\n"
                 "    }\n"
                 "    for (let i: i32 = 0; i < 10; i++) { continue; }\n"
                 "    let bad = @;\n"
                 "    return point.x;\n"
                 "}";
    token_t *tokens = lex(src, strlen(src));
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
}
