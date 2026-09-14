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

// Helper functions
static int is_terminator(const char *ptr) {
    return *ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r' || *ptr == '\0';
}

static int string() {
    // Base case
    if (*tok_start != '"')
        return TOK_INVALID;
    if (tok_scan == tok_start)
        tok_scan++;
    for (;*tok_scan != '"' && tok_scan != tok_end; tok_scan++) // To ensure that there isn't a \ before the end quotation mark
        if (*tok_scan == '\\')
            tok_scan++;
    return *tok_scan++ == '"'; // Increments so that it includes the closing quotation mark
}

static ttype_t identifier() {
    // Base case
    if ((*tok_start < 'a' || *tok_start > 'z') && (*tok_start < 'A' || *tok_start > 'Z')) // First character must be alphabetical
        return TOK_INVALID;

    for (;!is_terminator(tok_scan); tok_scan++)
        if ((*tok_scan < 'a' || *tok_scan > 'z') && (*tok_scan < 'A' || *tok_scan > 'Z') && (*tok_scan < '0' || *tok_scan > '9')) // Rest of them must be alphanumeric
            return TOK_INVALID;

    switch (tok_scan - tok_start) {
        case 2:
            if (strncmp(tok_start, "fn", 2) == 0)
                return TOK_FN;
            if (strncmp(tok_start, "if", 2) == 0)
                return TOK_IF;
            if (strncmp(tok_start, "u8", 2) == 0)
                return TOK_U8;
            if (strncmp(tok_start, "i8", 2) == 0)
                return TOK_I8;
            break;
        case 3:
            if (strncmp(tok_start, "let", 3) == 0)
                return TOK_LET;
            if (strncmp(tok_start, "for", 3) == 0)
                return TOK_FOR;
            if (strncmp(tok_start, "u16", 3) == 0)
                return TOK_U16;
            if (strncmp(tok_start, "u32", 3) == 0)
                return TOK_U32;
            if (strncmp(tok_start, "u64", 3) == 0)
                return TOK_U64;
            if (strncmp(tok_start, "i16", 3) == 0)
                return TOK_I16;
            if (strncmp(tok_start, "i32", 3) == 0)
                return TOK_I32;
            if (strncmp(tok_start, "i64", 3) == 0)
                return TOK_I64;
            break;
        case 4:
            if (strncmp(tok_start, "else", 4) == 0)
                return TOK_ELSE;
            if (strncmp(tok_start, "elif", 4) == 0)
                return TOK_ELIF;
            if (strncmp(tok_start, "enum", 4) == 0)
                return TOK_ENUM;
            if (strncmp(tok_start, "true", 4) == 0)
                return TOK_TRUE;
            if (strncmp(tok_start, "bool", 4) == 0)
                return TOK_BOOL;
            break;
        case 5:
            if (strncmp(tok_start, "const", 5) == 0)
                return TOK_CONST;
            if (strncmp(tok_start, "while", 5) == 0)
                return TOK_WHILE;
            if (strncmp(tok_start, "break", 5) == 0)
                return TOK_BREAK;
            if (strncmp(tok_start, "false", 5) == 0)
                return TOK_FALSE;
            break;
        case 6:
            if (strncmp(tok_start, "return", 6) == 0)
                return TOK_RETURN;
            if (strncmp(tok_start, "struct", 6) == 0)
                return TOK_STRUCT;
            if (strncmp(tok_start, "import", 6) == 0)
                return TOK_IMPORT;
            break;
        case 8:
            if (strncmp(tok_start, "continue", 8) == 0)
                return TOK_CONTINUE;
            break;
        default:
            break;
    }
            return TOK_IDENTIFIER;
}

static ttype_t math() {
    // Base case
    if (tok_scan - tok_start > 0)
        return TOK_INVALID;

    switch (*(tok_scan + 1)) {
        case '+':
            return *tok_scan == '+' ? TOK_INC : TOK_INVALID;
            break;
        case '-':
            return *tok_scan == '-' ? TOK_DEC : TOK_INVALID;
            break;
        case '>':
            return *tok_scan == '>' ? TOK_SHIFT_RIGHT : TOK_INVALID;
            break;
        case '<':
            return *tok_scan == '<' ? TOK_SHIFT_LEFT : TOK_INVALID;
            break;
        default:
            switch (*tok_scan) {
                case '+':
                    return TOK_PLUS;
                    break;
                case '-':
                    return TOK_MINUS;
                    break;
                case '*':
                    return TOK_AMP;
                    break;
                case '/':
                    return TOK_SLASH;
                    break;
                case '^':
                    return TOK_CARET;
                    break;
                case '%':
                    return TOK_PERCENT;
                    break;
                    // Bit-wise functions
                case '&':
                    return TOK_AMP;
                    break;
                case '|':
                    return TOK_PIPE;
                    break;
                case '~':
                    return TOK_TILDE;
                    break;
                default:
                    break;
            }
            break;
    }
    return TOK_INVALID;
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
        // Whitespace, newline, and tab handling
        while ((is_terminator(tok_start)) && (src - tok_start) < len)
            tok_start++;
        if ((is_terminator(tok_scan)) && (src - tok_scan) < len) // So that tok_scan is checked too
            tok_start = tok_scan + 1;

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

        // Identifier handling
        const ttype_t id = identifier();
        if (!!id) {
            tokens[tok_n].value = malloc(tok_scan - tok_start + 1);
            strncpy(tokens[tok_n].value, tok_start, tok_scan - tok_start);
            tokens[tok_n].type = id;
            tok_start = tok_scan;
            tok_n++;
        }

        // Math symbol handling
        const ttype_t sym = math();
        if (!!sym) {
            tokens[tok_n].value = malloc(tok_scan - tok_start + 1);
            strncpy(tokens[tok_n].value, tok_start, tok_scan - tok_start + 2);
            tokens[tok_n].type = sym;
            tok_start = tok_scan;
            tok_n++;
        }
    }

    return tokens;
}
