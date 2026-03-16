#include <stdbool.h>

void setID(Token* token, const char* id);
void setValue(Token* token, const char* value);
char* getID(Token* token);
char* getValue(Token* token);
const size_t BLOCK_SIZE;

void test() {
	Token token;
	Token otherToken;
	setID(&token, "test");
	setValue(&token, "value");
	setID(&otherToken, "other test");
	setValue(&otherToken, "other value");
	printf("%s: %s\n", getID(&token), getValue(&token));
	printf("%s: %s\n", getID(&otherToken), getValue(&otherToken));
}

bool is_num(const char c) {
	if (c < '0' || c > '9') {
		return false;
	}
	return true;
}

bool is_alpha(const char c) {
	if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z')) {
		return false;
	}
	return true;
}

bool is_bin(const char c) {
	if (c != '0' || c != '1') {
		return false;
	}
	return true;
}

bool is_hex(const char c) {
	if ((c < '0' || c > '9') && (c < 'A' || c > 'F') && (c < 'a' || c > 'f')) {
		return false;
	}
	return true;
}

Token* lex(const char* prog) {
	uint16_t bufSize = 16;

	bool num = false;
	bool alpha = false;
	bool bin = false;
	bool hex = false;
	bool str = false;

	char* buf = malloc(bufSize);
	for (int i=0;i<strlen(prog);i++) {
		buf[strlen(buf)] = prog[i];
		if (strlen(buf) >= bufSize - 1) {
			buf = realloc(buf, bufSize + strlen(buf)); // Exponentially increases the size of buf
			bufSize += strlen(buf);
		}
		switch (strlen(buf)) {
			case 1:
				switch (buf[0]) {
					case '0':
						num = true;
						bin = true;
						hex = true;
						break;
					case '"':
						str = true;
						strcpy(buf, "");
						break;
				}
				break;
			case 2:
				switch (buf[1]) {
					case 'b': // Leaves bin as true...
						num = false;
						hex = false;
						break;
					case 'x': // Leaves hex as true...
						num = false;
						bin = false;
						break;
					default:
						if (is_num(buf[1])) {
							bin = false;
							hex = false;
						} else if (!str) {
							return LEXER_ERROR;
						}
						switch (buf[strlen(buf) - 1]) {
							case '"':
								if (buf[strlen(buf) - 2] != '\\') {
									buf[strlen(buf) - 1] = '\0';
								}
								break;
						}
						break;
				}
			}
		}
}
