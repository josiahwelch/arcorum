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
	uint16_t id; // Storing as offsets to improve memory efficiency
	uint16_t value; // Storing as offsets to improve memory efficiency
};
