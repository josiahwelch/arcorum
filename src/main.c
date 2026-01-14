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
		realloc(mem, memSize + BLOCK_SIZE);
		memSize += BLOCK_SIZE;
	}
	token->id = offset;
	offset += strlen(id);
}

int main(int argc, char *argv[]) {
	mem = malloc(BLOCK_SIZE);
	memSize = BLOCK_SIZE;
	offset = 0;
}


