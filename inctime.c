#include "inctime.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

void InitLinkIncTime(LinkIncTime* pInc) {
    if (!pInc)
        return;
    memset(pInc, 0, sizeof(LinkIncTime));
    return;
}

void SetSeekFlag(LinkIncTime* pInc) {
    if (pInc->nIncAduioPts > pInc->nIncVideoPts)
        pInc->nSeekSyncATime = pInc->nIncAduioPts + 40;
    else 
        pInc->nSeekSyncATime = pInc->nIncVideoPts + 40;
    pInc->nSeekSyncVTime = pInc->nSeekSyncATime;
}

static void getPts(LinkIncTime* pInc, int64_t nOriginPts, int64_t *pIncPts, int64_t *pLastPts, int64_t *pSyncTime) {
    int64_t nIncPts = *pIncPts;
    int64_t nLastPts = *pLastPts;
    int64_t nSyncTime = *pSyncTime;

    *pLastPts = nOriginPts;
    if (pInc->bFisrt) {
        *pIncPts = 0;
        pInc->bFisrt = 0;
        return;
    }

    if (nSyncTime != 0) {
        *pIncPts = nSyncTime;
        *pSyncTime = 0;
        return;
    }

    //  认为时间戳，1. 翻转 2.不连续(可能是正常的，比如只是单纯的拼接两个ts)
    if (llabs(nLastPts - nOriginPts) > 1000) {
         printf("logwarn: pts abnormal:%"PRId64" %"PRId64"\n", nOriginPts, nLastPts);
        SetSeekFlag(pInc);
        *pIncPts = pInc->nSeekSyncATime;
        *pSyncTime = 0;
        return;
    }
    if (nOriginPts - nLastPts >= 0) {
        *pIncPts = nIncPts + (nOriginPts - nLastPts);
    } else {
        // 1000以内的非单调递增，pts保持不变, 但是更新了lastpts
        printf("logwarn: pts abnormal:%"PRId64" %"PRId64"\n", nOriginPts, nLastPts);
    }
    
    return;
}

int64_t GetAudioPts(LinkIncTime* pInc, int64_t nOriginPts) {
    getPts(pInc, nOriginPts, &pInc->nIncAduioPts, &pInc->nLastAudioPts, &pInc->nSeekSyncATime);
    pInc->nIncAduioPts &= (int64_t)0x0FFFFFFFF;
    return pInc->nIncAduioPts;
}

int64_t GetVideoPts(LinkIncTime* pInc, int64_t nOriginPts) {
    getPts(pInc, nOriginPts, &pInc->nIncVideoPts, &pInc->nLastVideoPts, &pInc->nSeekSyncVTime);
    pInc->nIncVideoPts &= (int64_t)0x0FFFFFFFF;
    return pInc->nIncVideoPts;
}