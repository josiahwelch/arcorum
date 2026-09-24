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

typedef struct {} binary_t;

typedef struct {
    binary_t binary;
} expression_t;

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
