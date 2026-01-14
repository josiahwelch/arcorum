#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

#define BLOCK_SIZE 16

char* mem;
size_t memSize;
uint16_t offset;

void setID(struct Token* token, const char* id) {
	while (offset + strlen(id) >= memSize) {
		mem = (char*) realloc(mem, memSize + BLOCK_SIZE);
		memSize += BLOCK_SIZE;
	}
	token->id = offset;
	offset += strlen(id);
}

char* getID(struct Token* token) {
	return mem + token->id;
}

int main(int argc, char *argv[]) {
	mem = (char*) malloc(BLOCK_SIZE);
	memSize = BLOCK_SIZE;
	offset = 0;
	struct Token token;
	setID(&token, "I am testing this really really hard rn. I just need a long enough string to force a realloc...");
	printf("%s\n", getID(&token));
}


