#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"
#include "lexer.h"

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
	strcpy((char*)(mem + offset), id);
	token->id = offset;
	offset += strlen(id);
}

void setValue(struct Token* token, const char* value) {
	while (offset + strlen(value) >= memSize) {
		if ((memBuf = realloc(mem, memSize + BLOCK_SIZE)) != NULL) {mem = memBuf;}
		else {
			printf("MEMORY ERROR!\n");
		}
		memSize += BLOCK_SIZE;
	}
	strcpy((char*)(mem + offset), value);
	token->value = offset;
	offset += strlen(value);
}

char* getID(struct Token* token) {
	return mem + token->id;
}

char* getValue(struct Token* token) {
	return mem + token->value;
}

int main(int argc, char *argv[]) {
	mem = malloc(BLOCK_SIZE);
	memSize = BLOCK_SIZE;
	offset = 0;
	test();
}
