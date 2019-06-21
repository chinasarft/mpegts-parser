#include "bitreader.h"
#include <string.h>
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"{
#endif
static void fillReservoir(BitReader *bitReader) {
	bitReader->mReservoir = 0;
  size_t i;
  for (i = 0; bitReader->mSize > 0 && i < 4; ++i) {
    bitReader->mReservoir = (bitReader->mReservoir << 8) | *(bitReader->mData);

    ++bitReader->mData;
    --bitReader->mSize;
  }

  bitReader->mNumBitsLeft = 8 * i;
  // not enough for 4 byte should mv the lowbit to highbit
  bitReader->mReservoir <<= 32 - bitReader->mNumBitsLeft;
}

void InitBitReader(BitReader *bitReader, uint8_t *data, size_t size)
{
    memset(bitReader, 0, sizeof(BitReader));
	bitReader->mData = data;
	bitReader->mSize = size;
}

uint32_t GetBits(BitReader *bitReader, size_t n)
{
	uint32_t result = 0;
  while (n > 0) {
    if (bitReader->mNumBitsLeft == 0) {
      fillReservoir(bitReader);
    }
    if (bitReader->mNumBitsLeft == 0) {
      bitReader->isInsufficient = 1;
      return result;
    }

    size_t m = n;
    if (m > bitReader->mNumBitsLeft) {
      m = bitReader->mNumBitsLeft;
    }

    result = (result << m) | (bitReader->mReservoir >> (32 - m));
    bitReader->mReservoir <<= m;
    bitReader->mNumBitsLeft -= m;
    n -= m;
  }

  return result;
}

uint64_t GetBits64(BitReader *bitReader, size_t n) {
  if (n <= 32) {
    return (uint64_t)GetBits(bitReader, n);
  }
  uint64_t pre = (uint64_t)GetBits(bitReader, 32);
  uint64_t suf = (uint64_t)GetBits(bitReader, n-32);
  return pre<<(n-32) | suf;
}

void SkipBits(BitReader *bitReader, size_t n) {
	while (n > 32) {
    GetBits(bitReader, 32);
    n -= 32;
  }

  if (n > 0) {
    GetBits(bitReader, n);
  }
}

size_t NumBitsLeft(BitReader *bitReader) {
	return bitReader->mSize * 8 + bitReader->mNumBitsLeft;
}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
