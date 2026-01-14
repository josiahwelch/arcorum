#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"

const uint8_t BLOCK_SIZE = 16;

char* mem = malloc(BLOCK_SIZE);
size_t memSize = BLOCK_SIZE;
uint16_t offset = 0;

void setID(struct* token, const char* id) {
	while (offset + strlen(id) => memSize) {
		realloc(mem, memSize + BLOCK_SIZE);
		memSize += BLOCK_SIZE;
	}
	token->id = offset;
	offset += strlen(id);
}

int main(int *argc, char *argv[]) {}
