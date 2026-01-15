void setID(struct Token* token, const char* id);
void setValue(struct Token* token, const char* value);
char* getID(struct Token* token);
char* getValue(struct Token* token);
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
