#ifndef GUARD_RTC_H
#define GUARD_RTC_H

#include "global.h"

// The RTC time is stored in BCD format
struct SiiRtcInfo
{
    u8 year;
    u8 month;
    u8 day;
    u8 dayOfWeek;
    u8 hour;
    u8 minute;
    u8 second;
    u8 status;
};

extern struct Time gLocalTime;

void RtcInit(void);
void RtcGetInfo(struct SiiRtcInfo *rtc);
void RtcGetTime(struct SiiRtcInfo *rtc);
void RtcCalcLocalTime(void);
void RtcInitLocalTimeOffset(s32 hour, s32 minute);

#define DAY_MORNING 0
#define DAY_DAY     1
#define DAY_EVENING 2
#define DAY_NIGHT   3

u8 GetCurrentTimeOfDay(void);
void ApplyDayNightTint(u16 *pltt, u16 size);

#endif // GUARD_RTC_H
