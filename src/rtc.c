#include "global.h"
#include "rtc.h"
#include "constants/pokemon.h"
#include "gba/flash_internal.h"

struct Time gLocalTime;

static void RtcReset(void);

void RtcInit(void)
{
    struct SiiRtcInfo rtc;
    gLocalTime.days = 0;
    gLocalTime.hours = 0;
    gLocalTime.minutes = 0;
    gLocalTime.seconds = 0;

    SiiRtcInit();
    RtcGetInfo(&rtc);

    if (rtc.status & (SIIRTCINFO_POWER | SIIRTCINFO_ERROR))
        RtcReset();
}

void RtcGetInfo(struct SiiRtcInfo *rtc)
{
    SiiRtcGetInfo(rtc);
}

void RtcGetTime(struct SiiRtcInfo *rtc)
{
    SiiRtcGetTime(rtc);
}

static void RtcReset(void)
{
    struct SiiRtcInfo rtc;
    rtc.year = 0;
    rtc.month = 1;
    rtc.day = 1;
    rtc.dayOfWeek = 0;
    rtc.hour = 0;
    rtc.minute = 0;
    rtc.second = 0;
    rtc.status = 0;
    SiiRtcSetDateTime(&rtc);
}

void RtcCalcLocalTime(void)
{
    struct SiiRtcInfo rtc;
    RtcGetTime(&rtc);

    s32 hours = ((rtc.hour & 0xF0) >> 4) * 10 + (rtc.hour & 0x0F);
    s32 minutes = ((rtc.minute & 0xF0) >> 4) * 10 + (rtc.minute & 0x0F);
    s32 seconds = ((rtc.second & 0xF0) >> 4) * 10 + (rtc.second & 0x0F);

    hours += gSaveBlock2Ptr->localTimeOffset.hours;
    minutes += gSaveBlock2Ptr->localTimeOffset.minutes;
    seconds += gSaveBlock2Ptr->localTimeOffset.seconds;

    if (seconds >= 60) { minutes++; seconds -= 60; }
    if (seconds < 0) { minutes--; seconds += 60; }
    if (minutes >= 60) { hours++; minutes -= 60; }
    if (minutes < 0) { hours--; minutes += 60; }
    if (hours >= 24) { hours -= 24; }
    if (hours < 0) { hours += 24; }

    gLocalTime.hours = hours;
    gLocalTime.minutes = minutes;
    gLocalTime.seconds = seconds;
}

void RtcInitLocalTimeOffset(s32 hour, s32 minute)
{
    // Placeholder
}

u8 GetCurrentTimeOfDay(void)
{
    if (gLocalTime.hours >= 5 && gLocalTime.hours < 10)
        return DAY_MORNING;
    if (gLocalTime.hours >= 10 && gLocalTime.hours < 17)
        return DAY_DAY;
    if (gLocalTime.hours >= 17 && gLocalTime.hours < 20)
        return DAY_EVENING;
    return DAY_NIGHT;
}

void ApplyDayNightTint(u16 *pltt, u16 size)
{
    u8 timeOfDay = GetCurrentTimeOfDay();
    u16 i;
    u32 r, g, b;
    u16 color;

    if (timeOfDay == DAY_DAY)
        return;

    for (i = 0; i < size / 2; i++)
    {
        color = pltt[i];

        r = color & 0x1F;
        g = (color >> 5) & 0x1F;
        b = (color >> 10) & 0x1F;

                // Skip white/very bright colors
        if (r > 28 && g > 28 && b > 28) continue;
        // Skip window yellows/oranges
        if (r > 25 && g > 20 && b < 15) continue;

        switch (timeOfDay)
        {
        case DAY_MORNING:
            r = (r * 22) / 31;
            g = (g * 22) / 31;
            b = (b * 28) / 31;
            break;
        case DAY_EVENING:
            r = (r * 28) / 31;
            g = (g * 20) / 31;
            b = (b * 20) / 31;
            break;
        case DAY_NIGHT:
            r = (r * 12) / 31;
            g = (g * 14) / 31;
            b = (b * 24) / 31;
            break;
        }

        pltt[i] = (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10);
    }
}

void ApplyTypeTint(u16 *pltt, u16 size, u8 type)
{
    u16 i;
    u32 r, g, b;
    u16 color;
    u8 tr = 31, tg = 31, tb = 31;

    switch (type)
    {
    case TYPE_NORMAL:
        tr = 25; tg = 25; tb = 22; break;
    case TYPE_FIGHTING:
        tr = 30; tg = 18; tb = 15; break;
    case TYPE_FLYING:
        tr = 22; tg = 25; tb = 30; break;
    case TYPE_POISON:
        tr = 25; tg = 15; tb = 30; break;
    case TYPE_GROUND:
        tr = 28; tg = 24; tb = 15; break;
    case TYPE_ROCK:
        tr = 22; tg = 20; tb = 15; break;
    case TYPE_BUG:
        tr = 22; tg = 28; tb = 15; break;
    case TYPE_GHOST:
        tr = 18; tg = 15; tb = 25; break;
    case TYPE_STEEL:
        tr = 22; tg = 25; tb = 28; break;
    case TYPE_FIRE:
        tr = 31; tg = 18; tb = 12; break;
    case TYPE_WATER:
        tr = 15; tg = 22; tb = 31; break;
    case TYPE_GRASS:
        tr = 15; tg = 28; tb = 15; break;
    case TYPE_ELECTRIC:
        tr = 30; tg = 30; tb = 12; break;
    case TYPE_PSYCHIC:
        tr = 31; tg = 15; tb = 25; break;
    case TYPE_ICE:
        tr = 18; tg = 28; tb = 31; break;
    case TYPE_DRAGON:
        tr = 18; tg = 15; tb = 31; break;
    case TYPE_DARK:
        tr = 15; tg = 12; tb = 18; break;
    }

    for (i = 0; i < size / 2; i++)
    {
        color = pltt[i];
        r = color & 0x1F;
        g = (color >> 5) & 0x1F;
        b = (color >> 10) & 0x1F;

        r = (r * tr) / 31;
        g = (g * tg) / 31;
        b = (b * tb) / 31;

        pltt[i] = (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10);
    }
}
