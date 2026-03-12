typedef enum {
	KEYWORD,
	IDENTIFIER,
	CONSTANT,
	OPERATOR,
	BRACKET,
	SEPARATOR
} TokenCat;

typedef enum {
	STR,
	INT,
	FLT,
	CHR,
	LOG,
	CMP,
	MAT
} TokenType;

typedef struct {
	enum TokenCat category;
	enum TokenType type;
	uint16_t id; // Storing as offsets to improve memory efficiency
	uint16_t value; // Storing as offsets to improve memory efficiency
} Token;
