#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

const size_t BLOCK_SIZE = 16;

void* mem;
void* memBuf;
size_t memSize;
uint16_t offset;

void setID(struct Token* token, const char* id) {
	while (offset + strlen(id) >= memSize) {
		if ((memBuf = realloc(mem, memSize + BLOCK_SIZE)) != NULL) {mem = memBuf;}
		else {
			printf("MEMORY ERROR!\n");
		}
		memSize += BLOCK_SIZE;
	}
	token->id = offset;
	offset += strlen(id);
}

char* getID(struct Token* token) {
	return mem + token->id;
}

int main(int argc, char *argv[]) {
	mem = malloc(BLOCK_SIZE);
	memSize = BLOCK_SIZE;
	offset = 0;
	struct Token token;
	setID(&token, "I am testing this really really hard rn. I just need a long enough string to force a realloc...");
	printf("%s\n", getID(&token));
}


