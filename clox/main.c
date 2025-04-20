#include "common.h"
#include "chunk.h"
#include "debug.h"


int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    int constant = addConstant(&chunk, 1.2);
    printf("cc %f \n", constant);
    int constant1 = addConstant(&chunk, 1.3);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_RETURN, 123);
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    
    return 0;
}
