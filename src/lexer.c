/*
 * @file lexer.c
 * @author Josiah Welch
 * @brief the C source file containing the lexical analyzer for the Arcorum compiler.
 * @date September 4th, 2026
 */

#include <lexer.h>

// Lexer token variables
static ssize_t tok_len;
static ssize_t tok_n = 0;
static token_t *tokens;

// Start, end, and scan pointers
static char *tok_start;
static char *tok_end;
static char *tok_scan;

// Helper functions
static void add_token(const ttype_t type, const char *end) {
    const ssize_t value_len = end - tok_start;

    tokens[tok_n].value = malloc(value_len + 1);
    memcpy(tokens[tok_n].value, tok_start, value_len);
    tokens[tok_n].value[value_len] = '\0';
    tokens[tok_n].value_len = value_len;
    tokens[tok_n].type = type;

    tok_start = (char *)end;
    tok_n++;
}

static int is_terminator(const char *ptr) {
    return *ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r' || *ptr == '\0';
}

static ttype_t quoted_literal() {
    // Base case
    const char quote = *tok_start;
    if (quote != '"' && quote != '\'')
        return TOK_INVALID;

    ssize_t character_count = 0;
    if (tok_scan == tok_start)
        tok_scan++;

    while (tok_scan != tok_end && *tok_scan != quote) {
        if (*tok_scan == '\\' && tok_scan + 1 != tok_end)
            tok_scan++;
        tok_scan++;
        character_count++;
    }

    if (tok_scan == tok_end)
        return TOK_INVALID;
    tok_scan++;

    if (quote == '\'' && character_count != 1)
        return TOK_INVALID;
    return quote == '"' ? TOK_STRING : TOK_CHAR;
}

static ttype_t identifier() {
    // Base case
    if ((*tok_start < 'a' || *tok_start > 'z') && (*tok_start < 'A' || *tok_start > 'Z')) // First character must be alphabetical
        return TOK_INVALID;

    while (tok_scan != tok_end && ((*tok_scan >= 'a' && *tok_scan <= 'z') || (*tok_scan >= 'A' && *tok_scan <= 'Z') || (*tok_scan >= '0' && *tok_scan <= '9')))
        tok_scan++;

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

static ttype_t operator(ssize_t *len) {
    // Base case
    if (tok_scan - tok_start > 0)
        return TOK_INVALID;

    const char next = tok_scan + 1 < tok_end ? *(tok_scan + 1) : '\0';
    *len = 1;

    switch (*tok_scan) {
        case '+':
            if (next == '+' || next == '=')
                *len = 2;
            return next == '+' ? TOK_INC : next == '=' ? TOK_PLUS_EQUAL : TOK_PLUS;
        case '-':
            if (next == '-' || next == '=')
                *len = 2;
            return next == '-' ? TOK_DEC : next == '=' ? TOK_MINUS_EQUAL : TOK_MINUS;
        case '*':
            if (next == '=')
                *len = 2;
            return next == '=' ? TOK_STAR_EQUAL : TOK_STAR;
        case '/':
            if (next == '=')
                *len = 2;
            return next == '=' ? TOK_SLASH_EQUAL : TOK_SLASH;
        case '=':
            if (next == '=')
                *len = 2;
            return next == '=' ? TOK_EQUAL_EQUAL : TOK_EQUAL;
        case '!':
            if (next == '=')
                *len = 2;
            return next == '=' ? TOK_BANG_EQUAL : TOK_BANG;
        case '<':
            if (next == '=' || next == '<')
                *len = 2;
            return next == '=' ? TOK_LESS_EQUAL : next == '<' ? TOK_SHIFT_LEFT : TOK_LESS;
        case '>':
            if (next == '=' || next == '>')
                *len = 2;
            return next == '=' ? TOK_GREATER_EQUAL : next == '>' ? TOK_SHIFT_RIGHT : TOK_GREATER;
        case '&':
            if (next == '&')
                *len = 2;
            return next == '&' ? TOK_AMP_AMP : TOK_AMP;
        case '|':
            if (next == '|')
                *len = 2;
            return next == '|' ? TOK_PIPE_PIPE : TOK_PIPE;
        case '%':
            return TOK_PERCENT;
        case '^':
            return TOK_CARET;
        case '~':
            return TOK_TILDE;
        default:
            return TOK_INVALID;
    }
}

static ttype_t punctuation() {
    if (tok_scan - tok_start > 0)
        return TOK_INVALID;

    switch (*tok_scan) {
        case '(':
            return TOK_LPAREN;
        case ')':
            return TOK_RPAREN;
        case '{':
            return TOK_LBRACE;
        case '}':
            return TOK_RBRACE;
        case '[':
            return TOK_LBRACKET;
        case ']':
            return TOK_RBRACKET;
        case ':':
            return TOK_COLON;
        case ';':
            return TOK_SEMICOLON;
        case ',':
            return TOK_COMMA;
        case '.':
            return TOK_DOT;
        default:
            return TOK_INVALID;
    }
}

static ttype_t number() {
    bool is_float = false;
    if (*tok_scan < '0' || *tok_scan > '9')
        return TOK_INVALID;
    for (;tok_scan != tok_end && ((*tok_scan >= '0' && *tok_scan <= '9') || *tok_scan == '.'); tok_scan++)
        if (*tok_scan == '.') {
            if (is_float)
                return TOK_INVALID;
            is_float = true;
        }
    return is_float ? TOK_FLOAT : TOK_INTEGER;
}

/*
 * @param a pointer to the source char array
 * @param size of source char array
 * @return a pointer to the lexed token array
 */
token_t *lex(char *src, ssize_t len) {
    tok_len = len;
    tokens = calloc(tok_len, sizeof(token_t));
    tok_n = 0;
    tok_start = src;
    tok_end = src + len;

    // Handling token array

    for (tok_scan = src; tok_scan - src < len; tok_scan++) {
        // Whitespace, newline, and tab handling
        while (tok_start < tok_end && is_terminator(tok_start))
            tok_start++;
        if (tok_scan < tok_end && is_terminator(tok_scan)) // So that tok_scan is checked too
            tok_start = tok_scan + 1;

        if (tok_scan < tok_start)
            tok_scan = tok_start;
        if (tok_start == tok_end)
            break;

        // String and character literal handling
        const ttype_t literal_type = quoted_literal();
        if (literal_type != TOK_INVALID) {
            add_token(literal_type, tok_scan);
            tok_scan--;
            continue;
        }

        // Identifier handling
        const ttype_t id = identifier();
        if (id != TOK_INVALID) {
            add_token(id, tok_scan);
            tok_scan--;
            continue;
        }

        // Operator handling
        ssize_t operator_len;
        const ttype_t operator_type = operator(&operator_len);
        if (operator_type != TOK_INVALID) {
            const char *end = tok_scan + operator_len;
            add_token(operator_type, end);
            tok_scan = (char *)end - 1;
            continue;
        }

        // Punctuation handling
        const ttype_t punctuation_type = punctuation();
        if (punctuation_type != TOK_INVALID)
            add_token(punctuation_type, tok_scan + 1);

        // Number handling
        const ttype_t number_type = number();
        if (number_type != TOK_INVALID) {
            add_token(number_type, tok_scan);
            tok_scan--;
        }

        // Reallocates memory
        for (;tok_n >= tok_len; tok_len += len) {
            if (realloc(tokens, tok_len * sizeof(token_t)) == NULL)
                return NULL;
        }
    }

    tok_start = tok_end;
    add_token(TOK_EOF, tok_end);

    return tokens;
}
