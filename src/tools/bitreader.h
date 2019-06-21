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
    uint8_t isInsufficient; // set if GetBits overflow
}BitReader;

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"{
#endif
void InitBitReader(BitReader *bitReader, uint8_t *data, size_t size);
uint32_t GetBits(BitReader *bitReader, size_t n);
void SkipBits(BitReader *bitReader, size_t n);
size_t NumBitsLeft(BitReader *bitReader);
inline uint8_t IsOverFlow(BitReader *p){return p->isInsufficient;};
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


#endif  // A_BIT_READER_H_
