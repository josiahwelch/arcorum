# Arcus Style

The C style used across Arcus projects (Arcorum, ArcusOS, and others).

Layout is enforced by `.clang-format` (run `clang-format -i src/*.c include/*.h`).
Everything else here is kept by hand.

## Layout

- Indent with tabs, 8 wide. Continuation lines get 4 extra spaces.
- 80-column limit.
- K&R braces everywhere, including functions:

  ```c
  static node_t *peek(const parser_t *parser) {
  	return &parser->tokens[parser->pos];
  }
  ```

- `case` labels line up with their `switch`.
- `*` binds to the name: `token_t *token`.

## Types

- Structs and enums are `typedef`'d and end in `_t`: `token_t`, `ttype_t`,
  `node_t`, `node_type_t`.
- Self-referencing structs get a forward typedef:
  `typedef struct node_t node_t;` then `struct node_t { ... };`.

## Functions

- Every `static` function has a prototype (with parameter names) at the top
  of its `.c` file.
- Empty parameter lists are written `(void)`.
- `return x;`, no parentheses.

## Variables

- Declare all locals at the top of the function, followed by one blank line.
- Don't call functions in declaration initializers; assign afterwards.

## Comments

- `/* */` only, written as sentences.
- Multi-line comments:

  ```c
  /*
   * First line.
   * Second line.
   */
  ```

## Includes

- Public headers live in `include/`, sources in `src/`.
- `include/` is on the compiler's search path (`-Iinclude`), which is why
  project headers use angle brackets: `#include <lexer.h>`.
- Headers include whatever they need.
