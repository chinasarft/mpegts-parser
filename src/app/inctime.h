#include <stdint.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif

// origin表示原始时间戳，比如从ts文件中读取的，因为ts的时间戳只有33位，所以int64_t也不会溢出
typedef struct {
    int64_t nIncAduioPts;
    int64_t nIncVideoPts;
    int64_t nLastAudioPts;
    int64_t nLastVideoPts;
    int64_t nSeekSyncATime;
    int64_t nSeekSyncVTime;
    uint8_t bFisrt;
}LinkIncTime;

void InitLinkIncTime(LinkIncTime*);
int64_t GetAudioPts(LinkIncTime* pInc, int64_t nOriginPts);
int64_t GetVideoPts(LinkIncTime* pInc, int64_t nOriginPts);
void SetSeekFlag(LinkIncTime* pInc);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
