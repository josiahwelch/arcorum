#include <stdbool.h>

void setID(struct Token* token, const char* id);
void setValue(struct Token* token, const char* value);
char* getID(struct Token* token);
char* getValue(struct Token* token);
const size_t BLOCK_SIZE;

void test() {
	struct Token token;
	struct Token otherToken;
	setID(&token, "test");
	setValue(&token, "value");
	setID(&otherToken, "other test");
	setValue(&otherToken, "other value");
	printf("%s: %s\n", getID(&token), getValue(&token));
	printf("%s: %s\n", getID(&otherToken), getValue(&otherToken));
}



Token* lex(const char* prog) {
	uint16_t bufSize = 16;
	char* buf = malloc(bufSize);
	for (int i=0;i<strlen(prog);i++) {
		buf[strlen(buf)] = prog[i];
		if (strlen(buf) >= bufSize - 1) {
			buf = realloc(bufSize + strlen(buf)* 2);
		}
