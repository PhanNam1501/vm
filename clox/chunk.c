#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineNumber = NULL;
    chunk->lineReplay = NULL;
    chunk->lineCapacity = 0;
    chunk->lineCount = 0;
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
    }
    printf("line: %4d \n", line);
    chunk->code[chunk->count] = byte;
    chunk->count++;
    if (chunk->lineCount > 0 && chunk->lineNumber[chunk->lineCount - 1] == line) {
        chunk->lineReplay[chunk->lineCount]++;
    } else {
        if (chunk->lineCapacity < chunk->lineCount + 1) {
            int oldLineCapacity = chunk->lineCapacity;
            chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
            chunk->lineNumber = GROW_ARRAY(uint8_t, chunk->lineNumber,
                oldLineCapacity, chunk->lineCapacity);
            chunk->lineReplay = GROW_ARRAY(uint8_t, chunk->lineReplay,
                oldLineCapacity, chunk->lineCapacity);
        } 
        chunk->lineNumber[chunk->lineCount] = line;
        chunk->lineReplay[chunk->lineCount] = 1;
        chunk->lineCount++;
    }

}

int getLine(Chunk* chunk,  int instructionIndex) {
    int count = 0;
    for (int i = 0; i < chunk->lineCount; i++) {
        count += chunk->lineReplay[i];
        if (count - 1 >= instructionIndex) {
            return chunk->lineNumber[i];
        }
    }
}

int addConstant(Chunk* chunk, Value value) {
    printf("%f\n", value);
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;//index
}

void writeConstant(Chunk* chunk, Value value, int line) {
    int constant = addConstant(chunk, value);

    if (value <= UINT8_MAX) {
        writeChunk(chunk, OP_CONSTANT, value);
        writeChunk(chunk, constant, line);
    } else {
        writeChunk(chunk,  OP_CONSTANT_LONG, value);

        writeChunk(chunk, (constant >> 0) & 0xFF, line);
        writeChunk(chunk, (constant >> 8) & 0xFF, line);
        writeChunk(chunk, (constant >> 16) & 0xFF, line);
    }
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lineNumber, chunk->lineCapacity);
    FREE_ARRAY(int, chunk->lineReplay, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}