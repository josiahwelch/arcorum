# Arcorum Grammar

EBNF for the Arcorum parser. `{ x }` means zero or more, `[ x ]` means optional,
quoted strings are literal tokens, and UPPERCASE names are token classes from the lexer.

## Program

```ebnf
program     = { declaration } EOF ;

declaration = import_decl
            | fn_decl
            | struct_decl
            | enum_decl
            | var_decl ";" ;                     (* global let/const *)

import_decl = "import" IDENTIFIER { "." IDENTIFIER } ";" ;

fn_decl     = "fn" IDENTIFIER "(" [ params ] ")" [ ":" type ] block ;
params      = param { "," param } [ "," ] ;
param       = IDENTIFIER ":" type ;

struct_decl = "struct" IDENTIFIER "{" [ field { "," field } [ "," ] ] "}" ;
field       = IDENTIFIER ":" type ;

enum_decl   = "enum" IDENTIFIER "{" [ IDENTIFIER { "," IDENTIFIER } [ "," ] ] "}" ;

type        = "u8" | "u16" | "u32" | "u64"
            | "i8" | "i16" | "i32" | "i64"
            | "bool"
            | IDENTIFIER ;                       (* struct or enum name *)
```

A function with no `: type` returns nothing.

## Statements

Assignment, `++` and `--` are **statements**, not expressions, so `if (x = 1)` and
`a = b = c` are syntax errors.

```ebnf
block       = "{" { statement } "}" ;

statement   = block
            | if_stmt
            | while_stmt
            | for_stmt
            | "return" [ expression ] ";"
            | "break" ";"
            | "continue" ";"
            | simple_stmt ";" ;

simple_stmt = var_decl
            | assignment
            | expression ;                       (* e.g. a bare call: f(x); *)

var_decl    = ( "let" | "const" ) IDENTIFIER [ ":" type ] [ "=" expression ] ;
assignment  = place ( "=" | "+=" | "-=" | "*=" | "/=" ) expression
            | place ( "++" | "--" ) ;
place       = postfix ;                          (* checked later: must be ident, a.b, or a[i] *)

if_stmt     = "if" "(" expression ")" block
              { "elif" "(" expression ")" block }
              [ "else" block ] ;

while_stmt  = "while" "(" expression ")" block ;

for_stmt    = "for" "(" [ simple_stmt ] ";" [ expression ] ";" [ assignment ] ")" block ;
```

Rules the parser does not enforce (they belong in the semantic pass):

- `const` requires an initializer.
- `let` needs a type or an initializer, so the type can be inferred.
- An assignment target must be a real location.

## Expressions

The expressions are parsed with a Pratt (precedence-climbing) loop. All binary operators
are left-associative. The bitwise operators bind **tighter** than the comparisons,
as in Rust and Go, so `x & 7 == 0` means `(x & 7) == 0`. In C it means `x & (7 == 0)`.

| Prec | Operators              | Kind          |
|------|------------------------|---------------|
| 1    | `\|\|`                 | logical or    |
| 2    | `&&`                   | logical and   |
| 3    | `==` `!=`              | equality      |
| 4    | `<` `<=` `>` `>=`      | comparison    |
| 5    | `\|`                   | bitwise or    |
| 6    | `^`                    | bitwise xor   |
| 7    | `&`                    | bitwise and   |
| 8    | `<<` `>>`              | shift         |
| 9    | `+` `-`                | additive      |
| 10   | `*` `/` `%`            | multiplicative|
| 11   | `!` `~` `-` (prefix)   | unary         |
| 12   | `()` `[]` `.`          | postfix       |

```ebnf
expression  = binary ;                           (* driven by the table above *)
unary       = ( "!" | "~" | "-" ) unary | postfix ;
postfix     = primary { "(" [ args ] ")" | "[" expression "]" | "." IDENTIFIER } ;
args        = expression { "," expression } [ "," ] ;
primary     = INTEGER | FLOAT | CHAR | STRING
            | "true" | "false"
            | IDENTIFIER
            | "(" expression ")" ;
```

## Open questions

These are not decided yet. The grammar above leaves them out.

- **Pointers:** a `*T` type plus `&x`/`*p`. An OS language will need these soon.
- **Arrays:** a type syntax such as `[N]T`. Indexing `a[i]` already parses.
- **Struct literals:** e.g. `Point { x: 1, y: 2 }`.
- **Casts:** e.g. `x as u8`.
- **Comments:** the lexer does not handle `//` yet.
