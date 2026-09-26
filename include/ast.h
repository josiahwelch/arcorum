/*
 * @file arcorum.c
 * @author Josiah Welch
 * @brief the C header file containing the ASTs for the parser of the Arcorum compiler.
 * @date September 24th, 2026
 */

#ifndef AST_H
#define AST_H
#include <lexer.h>

typedef struct {
    token_t *identifier;
    uint16_t id_count;
} import_decl_t;

typedef struct {
    char *type;
    token_t *ud_type;
    bool is_user_defined;
} type_t;

typedef struct {
    token_t *identifier;
    type_t *type;
} field_param_t;

typedef struct {
    field_param_t param;
    uint16_t param_count;
} field_params_t;

typedef struct {
    token_t *fn_id;
    field_params_t *params;
} fn_decl_t;

typedef struct {
    token_t *struct_id;
    field_params_t *fields;
} struct_decl_t;

typedef struct {
    token_t *enum_id;
    token_t *enums;
} enum_decl_t;

typedef struct expression_t expression_t;
typedef struct unary_t unary_t;

typedef struct {
    token_t *token;
    bool is_expr;
    expression_t *expression;
} primary_t;

typedef enum {
    PAREN,
    SQUARE,
    SUBMEM
} postfix_type_t;

typedef struct {
    postfix_type_t kind;
    union {
        field_params_t args;      /* PAREN:  "(" [ args ] ")" */
        expression_t *index;      /* SQUARE: "[" expression "]" */
        token_t *field;           /* SUBMEM: "." IDENTIFIER */
    };
} postfix_op_t;

typedef struct {
    primary_t *primary;
    postfix_op_t *ops;
    uint16_t count;
} postfix_t;

struct unary_t {
    postfix_t *postfix;
    ttype_t prefix;
    bool is_unary;
    unary_t *unary;
};

typedef struct {
    unary_t *unary;
    ttype_t type;
} unary_pairs_t;

typedef struct {
    unary_t *unary;
    unary_pairs_t *pairs;
    uint16_t count;
} mul_div_t;

typedef struct {
    mul_div_t *mul_div;
    bool is_sub;
} mul_div_pair_t;

typedef struct {
    mul_div_t *mul_div;
    mul_div_pair_t *pairs;
    uint16_t count;
} add_sub_t;

typedef struct {
    add_sub_t *add_sub;
    bool is_left;
} add_sub_pair_t;

typedef struct {
    add_sub_t add_sub;
    add_sub_pair_t *pairs;
    uint16_t count;
} shift_t;

typedef struct {
    shift_t *shifts;
    uint16_t count;
} bit_and_t;

typedef struct {
    bit_and_t *bit_ands;
    uint16_t count;
} bit_xor_t;

typedef struct {
    bit_xor_t *bit_xors;
    uint16_t count;
} bit_or_t;

typedef struct {
    bit_or_t *bit_or;
    ttype_t type;
} comp_pair_t;

typedef struct {
    bit_or_t *bit_or;
    comp_pair_t *pairs;
    uint16_t count;
} comp_t;

typedef struct {
    comp_t *comp;
    bool is_not_equal;
} eq_pair_t;

typedef struct {
    comp_t *comp;
    eq_pair_t *pairs;
    uint16_t count;
} eq_t;

typedef struct {
    eq_t *eqs;
    uint16_t count;
} log_and_t;

typedef struct {
    log_and_t *log_ands;
    uint16_t count;
} log_or_t;

typedef struct {
    log_or_t *log_or;
} binary_t;

struct expression_t {
    binary_t binary;
};

typedef struct {
    field_params_t vars;
    expression_t expression;
    bool is_const;
} var_decl_t;

typedef struct {
    import_decl_t *import_decl;
    fn_decl_t *fn_decl;
    struct_decl_t *struct_decl;
    enum_decl_t *enum_decl;
    var_decl_t *var_decl;
} declaration_t;

typedef struct {
    declaration_t *declaration;
} program_t;

#endif //AST_H
