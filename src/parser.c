/*
 * @file parser.c
 * @author Josiah Welch
 * @brief the C source file containing the parser for the Arcorum compiler.
 * @date October 13th, 2026
 */

#include <parser.h>

typedef struct {
	token_t *tokens;
	ssize_t pos;
} parser_t;

/* Helper functions. */
static uint8_t precedence(const token_t *token);
static token_t *peek(const parser_t *parser);
static token_t *advance(parser_t *parser);
static void err_msg(const token_t *token, const char *exp_msg, bool got_msg);
static token_t *expect(parser_t *parser, ttype_t type, const char *exp_msg);
static node_t *new_node(node_type_t type, token_t *token);

/* Parser functions. */
static node_t *parse_primary(parser_t *parser);
static node_t *parse_unary(parser_t *parser);
static node_t *parse_expr(parser_t *parser, uint8_t min_prec);
static node_t *parse_expr_statement(parser_t *parser);
static node_t *parse_block(parser_t *parser);
static node_t *parse_else(parser_t *parser);
static node_t *parse_elif(parser_t *parser);
static node_t *parse_if(parser_t *parser);
static node_t *parse_while(parser_t *parser);
static node_t *parse_type(parser_t *parser);
static node_t *parse_var_type(parser_t *parser);
static node_t *parse_var_decl(parser_t *parser);
static node_t *parse_for(parser_t *parser);
static node_t *parse_statement(parser_t *parser);
static node_t *parse_fn(parser_t *parser);
static node_t *parse_struct(parser_t *parser);
static node_t *parse_enum(parser_t *parser);
static node_t *parse_import_src(parser_t *parser);
static node_t *parse_import(parser_t *parser);
static node_t *parse_program(parser_t *parser);

/* Helper functions. */
static uint8_t precedence(const token_t *token) {
	switch (token->type) {
	case TOK_PIPE_PIPE:
		return 1;
	case TOK_AMP_AMP:
		return 2;
	case TOK_EQUAL_EQUAL:
	case TOK_BANG_EQUAL:
	case TOK_LESS_EQUAL:
	case TOK_GREATER_EQUAL:
	case TOK_LESS:
	case TOK_GREATER:
		return 3;
	case TOK_PLUS:
	case TOK_MINUS:
	case TOK_PIPE:
	case TOK_CARET:
		return 4;
	case TOK_STAR:
	case TOK_SLASH:
	case TOK_SHIFT_LEFT:
	case TOK_SHIFT_RIGHT:
	case TOK_PERCENT:
	case TOK_AMP:
		return 5;
	default:
		return 0;
	}
}

static token_t *peek(const parser_t *parser) {
	return &parser->tokens[parser->pos];
}

static token_t *advance(parser_t *parser) {
	if (peek(parser)->type != TOK_EOF)
		parser->pos++;
	return peek(parser);
}

static void err_msg(const token_t *token, const char *exp_msg, bool got_msg) {
	if (got_msg) {
		if (token->type == TOK_EOF)
			fprintf(stderr,
			    "expected %s, got end of file at %zd:%zd!\n",
			    exp_msg, token->line, token->col);
		else
			fprintf(stderr, "expected %s, got '%s' at %zd:%zd!\n",
			    exp_msg, token->value, token->line, token->col);
	} else
		fprintf(stderr, "expected %s at %zd:%zd!\n", exp_msg,
		    token->line, token->col);
}

static token_t *expect(parser_t *parser, ttype_t type, const char *exp_msg) {
	token_t *token;

	token = peek(parser);
	advance(parser);
	if (token->type == type)
		return token;
	err_msg(token, exp_msg, true);
	exit(1);
}

static node_t *new_node(node_type_t type, token_t *token) {
	node_t *node;

	node = calloc(1, sizeof(*node));
	if (node == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
	node->token = token;
	node->type = type;
	return node;
}

/* Parser functions. */
static node_t *parse_primary(parser_t *parser) {
	node_t *node;
	token_t *current;

	current = peek(parser);
	advance(parser);
	if (current->type == TOK_LPAREN) {
		node = parse_expr(parser, 0);
		expect(parser, TOK_RPAREN, "')'");
		return node;
	}
	/* TOK_IDENTIFIER through TOK_STRING covers all literals. */
	if (current->type >= TOK_IDENTIFIER && current->type <= TOK_STRING)
		return new_node(NODE_LEAF, current);
	err_msg(current, "an expression", true);
	exit(1);
}

static node_t *parse_unary(parser_t *parser) {
	node_t *arg, *index, *new, *node;
	token_t *current, *id, *token;
	size_t size;

	current = peek(parser);
	if (current->type == TOK_MINUS || current->type == TOK_BANG ||
	    current->type == TOK_TILDE || current->type == TOK_AMP ||
	    current->type == TOK_STAR) {
		advance(parser);
		node = new_node(NODE_UNARY, current);
		node->left = parse_unary(parser);
		return node;
	}
	node = parse_primary(parser);
	for (token = peek(parser); token->type == TOK_LBRACKET ||
	    token->type == TOK_DOT || token->type == TOK_LPAREN;
	    token = peek(parser)) {
		advance(parser);
		switch (token->type) {
		case TOK_LBRACKET:
			index = parse_expr(parser, 0);
			expect(parser, TOK_RBRACKET, "']'");
			new = new_node(NODE_INDEX, token);
			new->left = node;
			new->right = index;
			node = new;
			break;
		case TOK_DOT:
			id = expect(parser, TOK_IDENTIFIER, "an identifier");
			new = new_node(NODE_MEMBER, token);
			new->left = node;
			new->right = new_node(NODE_LEAF, id);
			node = new;
			break;
		case TOK_LPAREN:
			new = new_node(NODE_CALL, token);
			new->left = node;
			if (peek(parser)->type == TOK_RPAREN)
				advance(parser);
			else {
				size = 1;
				for (;;) {
					arg = parse_expr(parser, 0);
					new->argc++;
					if (new->argc >= size) {
						size *= 2;
						new->args = realloc(new->args,
						    sizeof(node_t *) * size);
						if (new->args == NULL) {
							fprintf(stderr,
							    "Ran out of memory!\n");
							exit(1);
						}
					}
					new->args[new->argc - 1] = arg;
					if (peek(parser)->type != TOK_COMMA)
						break;
					advance(parser);
					if (peek(parser)->type == TOK_RPAREN)
						break;
				}
				expect(parser, TOK_RPAREN, "')'");
			}
			node = new;
			break;
		default:
			break;
		}
	}
	return node;
}

static node_t *parse_expr(parser_t *parser, uint8_t min_prec) {
	node_t *arg, *left, *node, *right;
	token_t *next_op, *op;
	size_t size;
	node_type_t type;

	left = parse_unary(parser);
	op = peek(parser);
	while (precedence(op) >= min_prec && precedence(op)) {
		advance(parser);
		right = parse_expr(parser, precedence(op) + 1);
		if (precedence(op) == 3 && precedence(peek(parser)) == 3)
			type = NODE_CHAIN;
		else
			type = NODE_BINARY;
		node = new_node(type, op);
		if (node->type == NODE_CHAIN) {
			/* Comparison chain. */
			size = 4;
			node->args = calloc(1, sizeof(node_t *) * size);
			node->ops = calloc(1, sizeof(token_t *) * size);
			if (node->args == NULL || node->ops == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
			/* First pair with operator. */
			node->args[0] = left;
			node->args[1] = right;
			node->ops[0] = op;
			node->argc = 2;
			while (precedence(peek(parser)) == 3) {
				next_op = peek(parser);
				advance(parser);
				arg = parse_expr(parser, 4);
				if (node->argc >= size) {
					size *= 2;
					node->args = realloc(node->args,
					    sizeof(node_t *) * size);
					node->ops = realloc(node->ops,
					    sizeof(token_t *) * size);
					if (node->args == NULL ||
					    node->ops == NULL) {
						fprintf(stderr,
						    "Ran out of memory!\n");
						exit(1);
					}
				}
				node->ops[node->argc - 1] = next_op;
				node->args[node->argc] = arg;
				node->argc++;
			}
		} else {
			node->left = left;
			node->right = right;
		}
		left = node;
		op = peek(parser);
	}
	return left;
}

static node_t *parse_expr_statement(parser_t *parser) {
	node_t *node;

	node = new_node(NODE_EXPR_STATEMENT, peek(parser));
	if (node->token->type == TOK_BREAK ||
	    node->token->type == TOK_CONTINUE) {
		advance(parser);
		expect(parser, TOK_SEMICOLON, "';'");
	} else if (node->token->type == TOK_RETURN) {
		advance(parser);
		if (peek(parser)->type != TOK_SEMICOLON &&
		    peek(parser)->type != TOK_EOF &&
		    peek(parser)->type != TOK_RBRACE)
			node->left = parse_expr(parser, 0);
		expect(parser, TOK_SEMICOLON, "';'");
	} else {
		node->left = parse_expr(parser, 0);
		if (peek(parser)->type >= TOK_EQUAL &&
		    peek(parser)->type <= TOK_SLASH_EQUAL) {
			node->type = NODE_ASSIGN;
			node->token = peek(parser);
			advance(parser);
			node->right = parse_expr(parser, 0);
		} else if (peek(parser)->type == TOK_INC ||
		    peek(parser)->type == TOK_DEC) {
			node->type = NODE_ASSIGN;
			node->token = peek(parser);
			advance(parser);
		}
		expect(parser, TOK_SEMICOLON, "';'");
	}
	return node;
}

static node_t *parse_block(parser_t *parser) {
	node_t *node;
	size_t size;

	node = new_node(NODE_BLOCK, peek(parser));
	size = 1;
	expect(parser, TOK_LBRACE, "'{'");
	while (
	    peek(parser)->type != TOK_RBRACE && peek(parser)->type != TOK_EOF) {
		node->argc++;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		if (peek(parser)->type == TOK_LBRACE)
			node->args[node->argc - 1] = parse_block(parser);
		else
			node->args[node->argc - 1] = parse_statement(parser);
	}
	expect(parser, TOK_RBRACE, "'}'");
	return node;
}

static node_t *parse_else(parser_t *parser) {
	node_t *node;

	node = new_node(NODE_ELSE, peek(parser));
	expect(parser, TOK_ELSE, "'else'");
	node->right = parse_block(parser);
	return node;
}

static node_t *parse_elif(parser_t *parser) {
	node_t *node;

	node = new_node(NODE_ELIF, peek(parser));
	expect(parser, TOK_ELIF, "'elif'");
	expect(parser, TOK_LPAREN, "'('");
	node->left = parse_expr(parser, 0);
	expect(parser, TOK_RPAREN, "')'");
	node->right = parse_block(parser);
	return node;
}

static node_t *parse_if(parser_t *parser) {
	node_t *node;
	size_t size;

	node = new_node(NODE_IF, peek(parser));
	size = 1;
	expect(parser, TOK_IF, "'if'");
	expect(parser, TOK_LPAREN, "'('");
	node->left = parse_expr(parser, 0);
	expect(parser, TOK_RPAREN, "')'");
	node->right = parse_block(parser);
	while (
	    peek(parser)->type == TOK_ELIF || peek(parser)->type == TOK_ELSE) {
		node->argc++;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		if (peek(parser)->type == TOK_ELIF)
			node->args[node->argc - 1] = parse_elif(parser);
		else if (peek(parser)->type == TOK_ELSE) {
			node->args[node->argc - 1] = parse_else(parser);
			if (peek(parser)->type == TOK_ELIF ||
			    peek(parser)->type == TOK_ELSE) {
				err_msg(peek(parser), "end of if-else chain",
				    true);
				exit(1);
			}
		}
	}
	return node;
}

static node_t *parse_while(parser_t *parser) {
	node_t *node;

	node = new_node(NODE_WHILE, peek(parser));
	expect(parser, TOK_WHILE, "'while'");
	expect(parser, TOK_LPAREN, "'('");
	node->left = parse_expr(parser, 0);
	expect(parser, TOK_RPAREN, "')'");
	node->right = parse_block(parser);
	return node;
}

static node_t *parse_type(parser_t *parser) {
	node_t *node;
	token_t *token;
	size_t ptr_depth;

	for (ptr_depth = 0; peek(parser)->type == TOK_STAR; advance(parser))
		ptr_depth++;
	token = peek(parser);
	/* TOK_U8 through TOK_I64 covers all built-in types. */
	if ((token->type < TOK_U8 || token->type > TOK_I64) &&
	    token->type != TOK_IDENTIFIER) {
		err_msg(token, "a type", true);
		exit(1);
	}
	node = new_node(NODE_TYPE, token);
	node->argc = ptr_depth;
	advance(parser);
	return node;
}

static node_t *parse_var_type(parser_t *parser) {
	node_t *node;
	token_t *name;

	name = expect(parser, TOK_IDENTIFIER, "variable name");
	node = new_node(NODE_VAR_TYPE, name);
	expect(parser, TOK_COLON, "':'");
	node->left = parse_type(parser);
	return node;
}

static node_t *parse_var_decl(parser_t *parser) {
	node_t *node;
	size_t size;
	bool done;

	node = new_node(NODE_VAR, peek(parser));
	advance(parser);
	size = 1;
	done = false;
	while (!done) {
		node->argc += 1;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		node->args[node->argc - 1] = parse_var_type(parser);
		switch (peek(parser)->type) {
		case TOK_COMMA:
			advance(parser);
			break;
		case TOK_EQUAL:
			done = true;
			if (node->argc > 1)
				expect(parser, TOK_SEMICOLON, "';'");
			advance(parser);
			break;
		case TOK_SEMICOLON:
			advance(parser);
			return node;
		default:
			expect(parser, TOK_SEMICOLON,
			    "'=', (',' type), or ';'");
			break;
		}
	}
	node->left = parse_expr(parser, 0);
	expect(parser, TOK_SEMICOLON, "';'");
	return node;
}

static node_t *parse_for(parser_t *parser) {
	node_t *node;
	node_t *node_mod;
	int i;

	node = new_node(NODE_FOR, peek(parser));
	expect(parser, TOK_FOR, "'for'");
	expect(parser, TOK_LPAREN, "'('");
	node->args = calloc(3, sizeof(node_t *));
	node->argc = 3;
	if (node->args == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
		return NULL;
	}
	for (i = 0; i < 2; i++) {
		if (peek(parser)->type == TOK_SEMICOLON)
			advance(parser);
		else
			node->args[i] = parse_statement(parser);
	}
	/* See parse_expr_statement. */
	if (peek(parser)->type != TOK_RPAREN) {
		node_mod = new_node(NODE_EXPR_STATEMENT, peek(parser));
		node_mod->left = parse_expr(parser, 0);
		if (peek(parser)->type >= TOK_EQUAL &&
		    peek(parser)->type <= TOK_SLASH_EQUAL) {
			node_mod->type = NODE_ASSIGN;
			node_mod->token = peek(parser);
			advance(parser);
			node_mod->right = parse_expr(parser, 0);
		} else if (peek(parser)->type == TOK_INC ||
		    peek(parser)->type == TOK_DEC) {
			node_mod->type = NODE_ASSIGN;
			node_mod->token = peek(parser);
			advance(parser);
		}
		node->args[2] = node_mod;
	}
	expect(parser, TOK_RPAREN, "')'");
	node->left = parse_block(parser);
	return node;
}

static node_t *parse_statement(parser_t *parser) {
	switch (peek(parser)->type) {
	case TOK_LBRACE:
		return parse_block(parser);
	case TOK_IF:
		return parse_if(parser);
	case TOK_WHILE:
		return parse_while(parser);
	case TOK_LET:
	case TOK_CONST:
		return parse_var_decl(parser);
	case TOK_FOR:
		return parse_for(parser);
	default:
		return parse_expr_statement(parser);
	}
}

static node_t *parse_fn(parser_t *parser) {
	node_t *node;
	size_t size;
	bool done;

	node = new_node(NODE_FN, advance(parser));
	expect(parser, TOK_IDENTIFIER, "an identifier");
	expect(parser, TOK_LPAREN, "'('");
	size = 1;
	done = false;
	if (peek(parser)->type == TOK_RPAREN)
		advance(parser);
	else
		while (!done) {
			node->argc += 1;
			if (node->argc >= size) {
				size *= 2;
				node->args = realloc(node->args,
				    sizeof(node_t *) * size);
				if (node->args == NULL) {
					fprintf(stderr, "Ran out of memory!\n");
					exit(1);
				}
			}
			node->args[node->argc - 1] = parse_var_type(parser);
			if (peek(parser)->type == TOK_COMMA)
				advance(parser);
			else {
				expect(parser, TOK_RPAREN, "')'");
				break;
			}
		}
	if (peek(parser)->type == TOK_COLON) {
		advance(parser);
		node->left = parse_type(parser);
	}
	node->right = parse_block(parser);
	return node;
}

static node_t *parse_struct(parser_t *parser) {
	node_t *node;
	size_t size;
	bool done;

	node = new_node(NODE_STRUCT, advance(parser));
	expect(parser, TOK_IDENTIFIER, "an identifier");
	expect(parser, TOK_LBRACE, "'{'");
	size = 1;
	done = false;
	while (!done) {
		node->argc += 1;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		node->args[node->argc - 1] = parse_var_type(parser);
		if (peek(parser)->type == TOK_COMMA)
			advance(parser);
		else {
			expect(parser, TOK_RBRACE, "'}'");
			break;
		}
	}
	return node;
}

static node_t *parse_enum(parser_t *parser) {
	node_t *node;
	size_t size;

	node = new_node(NODE_ENUM, advance(parser));
	expect(parser, TOK_IDENTIFIER, "an identifier");
	expect(parser, TOK_LBRACE, "'{'");
	size = 1;
	for (;;) {
		node->argc += 1;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		node->args[node->argc - 1] = new_node(NODE_LEAF,
		    expect(parser, TOK_IDENTIFIER, "an identifier"));
		if (peek(parser)->type == TOK_COMMA)
			advance(parser);
		else {
			expect(parser, TOK_RBRACE, "'}'");
			break;
		}
	}
	return node;
}

static node_t *parse_import_src(parser_t *parser) {
	node_t *node;
	size_t size;

	node = new_node(NODE_IMPORT_SRC, peek(parser));
	size = 1;
	if (peek(parser)->type == TOK_STRING) {
		node->args = calloc(1, sizeof(node_t *));
		if (node->args == NULL) {
			fprintf(stderr, "Ran out of memory!\n");
			exit(1);
		}
		node->argc = 1;
		node->args[0] = new_node(NODE_LEAF, peek(parser));
		advance(parser);
	} else
		for (;;) {
			node->argc += 1;
			if (node->argc >= size) {
				size *= 2;
				node->args = realloc(node->args,
				    sizeof(node_t *) * size);
				if (node->args == NULL) {
					fprintf(stderr, "Ran out of memory!\n");
					exit(1);
				}
			}
			node->args[node->argc - 1] = new_node(NODE_LEAF,
			    expect(parser, TOK_IDENTIFIER, "an identifier"));
			if (peek(parser)->type == TOK_DOT)
				advance(parser);
			else
				break;
		}
	return node;
}

static node_t *parse_import(parser_t *parser) {
	node_t *node;
	size_t size;

	node = new_node(NODE_IMPORT, advance(parser));
	size = 1;
	for (;;) {
		node->argc += 1;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		node->args[node->argc - 1] = parse_import_src(parser);
		if (peek(parser)->type == TOK_COMMA)
			advance(parser);
		else {
			expect(parser, TOK_SEMICOLON, "';'");
			break;
		}
	}
	return node;
}

static node_t *parse_program(parser_t *parser) {
	node_t *node;
	size_t size;
	bool done;

	node = new_node(NODE_PROGRAM, peek(parser));
	size = 1;
	done = false;
	while (!done) {
		node->argc += 1;
		if (node->argc >= size) {
			size *= 2;
			node->args =
			    realloc(node->args, sizeof(node_t *) * size);
			if (node->args == NULL) {
				fprintf(stderr, "Ran out of memory!\n");
				exit(1);
			}
		}
		switch (peek(parser)->type) {
		case TOK_FN:
			node->args[node->argc - 1] = parse_fn(parser);
			break;
		case TOK_STRUCT:
			node->args[node->argc - 1] = parse_struct(parser);
			break;
		case TOK_ENUM:
			node->args[node->argc - 1] = parse_enum(parser);
			break;
		case TOK_IMPORT:
			node->args[node->argc - 1] = parse_import(parser);
			break;
		case TOK_LET:
		case TOK_CONST:
			node->args[node->argc - 1] = parse_var_decl(parser);
			break;
		case TOK_EOF:
			node->argc--; /* Since it gets incremented anyways. */
			done = true;
			break;
		default:
			err_msg(peek(parser),
			    "an import statement or an enumerated type, struct, function, or variable declaration",
			    true);
			exit(1);
			return NULL;
		}
	}
	return node;
}

node_t *parse(token_t *tokens) {
	parser_t parser;
	node_t *node;

	parser.tokens = tokens;
	parser.pos = 0;
	node = parse_program(&parser);
	expect(&parser, TOK_EOF, "end of file");
	return node;
}
