#ifndef __BIT_READER_H__
#define __BIT_READER_H__

#include <sys/types.h>
#include <stdint.h>

typedef struct BitReader
{
    uint8_t *mData;
    size_t mSize;

    uint32_t mReservoir;  // left-aligned bits
    size_t mNumBitsLeft;
}BitReader;


void InitBitReader(BitReader *bitReader, uint8_t *data, size_t size);
uint32_t GetBits(BitReader *bitReader, size_t n);
void SkipBits(BitReader *bitReader, size_t n);
size_t numBitsLeft(BitReader *bitReader);


#endif  // A_BIT_READER_H_
