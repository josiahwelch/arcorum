enum TokenCat {
	KEYWORD,
	IDENTIFIER,
	CONSTANT,
	OPERATOR,
	BRACKET,
	SEPARATOR
};

enum TokenType {
	STR,
	INT,
	FLT,
	CHR,
	LOG,
	CMP,
	MAT
};

struct Token {
	enum TokenCat category;
	enum TokenType type;
	char 
