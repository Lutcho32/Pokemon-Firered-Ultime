#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "constants/opponents.h"
#include "main.h"
#include "overworld.h"
#include "rtc.h"
#include "gflib.h"
#include "util.h"
#include "decompress.h"
#include "task.h"

enum
{
    NORMAL_FADE,
    FAST_FADE,
    HARDWARE_FADE,
};

// These are structs for some unused palette system.
// The full functionality of this system is unknown.

#define NUM_PALETTE_STRUCTS 16

struct PaletteStruct
{
    u16 colors[16];
    u32 unused;
};

static EWRAM_DATA struct PaletteStruct *sPaletteStructs = NULL;
static EWRAM_DATA u16 sUnused_02038444 = 0;
static EWRAM_DATA u16 sPlttBufferTransferPending = 0;
static EWRAM_DATA u8 sFiller[2] = {0};

void DoNothing(void)
{
}

void LoadPalette(const void *src, u16 offset, u16 size)
{
    CpuCopy16(src, &gPlttBufferUnfaded[offset], size);
    CpuCopy16(src, &gPlttBufferFaded[offset], size);
}

void TransferPlttBuffer(void)
{
    if (!gPaletteFade.bufferTransferDisabled)
    {
        void *src = gPlttBufferFaded;
        void *dest = (void *)PLTT;
        if (gMain.callback2 == CB2_Overworld || gMain.callback2 == CB2_OverworldBasic || gMain.callback2 == BattleMainCB2)
        {
            u16 tintedPltt[PLTT_BUFFER_SIZE];
            CpuCopy16(src, tintedPltt, PLTT_SIZE);
            ApplyDayNightTint(tintedPltt, PLTT_SIZE);
            if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) && (gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_SQUIRTLE || gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_BULBASAUR || gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_CHARMANDER || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_SQUIRTLE || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_BULBASAUR || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_CHARMANDER))
            {
                ApplyTypeTint(tintedPltt, PLTT_SIZE, gBattleMons[B_POSITION_OPPONENT_LEFT].type1);
            }
            DmaCopy16(3, tintedPltt, dest, PLTT_SIZE);
        }
        else
            DmaCopy16(3, src, dest, PLTT_SIZE);
        sPlttBufferTransferPending = FALSE;
        if (gPaletteFade.mode == HARDWARE_FADE && gPaletteFade.active)
            UpdateBlendRegisters();
    }
}

u8 UpdatePaletteFade(void)
{
    u8 result;
    u8 dummy = 0;

    if (sPlttBufferTransferPending)
        return PALETTE_FADE_STATUS_LOADING;
    if (gPaletteFade.mode == NORMAL_FADE)
        result = UpdateNormalPaletteFade();
    else if (gPaletteFade.mode == FAST_FADE)
        result = UpdateFastPaletteFade();
    else
        result = UpdateHardwarePaletteFade();

    if (result != PALETTE_FADE_STATUS_ACTIVE)
        return result;

    if (gPaletteFade.hardwareFadeFinishing)
    {
        gPaletteFade.hardwareFadeFinishing = FALSE;
        gPaletteFade.mode = NORMAL_FADE;
        gPaletteFade.active = FALSE;
    }

    return PALETTE_FADE_STATUS_ACTIVE;
}

void FillPalette(u16 color, u16 offset, u16 size)
{
    u16 i;
    u16 *dest = &gPlttBufferUnfaded[offset];
    for (i = 0; i < size / 2; i++)
    {
        dest[i] = color;
    }
    dest = &gPlttBufferFaded[offset];
    for (i = 0; i < size / 2; i++)
    {
        dest[i] = color;
    }
}

void SetPlttBufferTransferPending(void)
{
    sPlttBufferTransferPending = TRUE;
}

bool8 IsPlttBufferTransferPending(void)
{
    return sPlttBufferTransferPending;
}

bool8 UpdateNormalPaletteFade(void)
{
    u16 i;
    u16 color;
    u8 r, g, b;
    u8 targetR, targetG, targetB;
    bool8 temp;

    if (!gPaletteFade.active)
        return FALSE;

    if (gPaletteFade.delayCounter < gPaletteFade.delay)
    {
        gPaletteFade.delayCounter++;
        return TRUE;
    }

    gPaletteFade.delayCounter = 0;

    if (gPaletteFade.y < gPaletteFade.targetY)
    {
        gPaletteFade.y++;
    }
    else if (gPaletteFade.y > gPaletteFade.targetY)
    {
        gPaletteFade.y--;
    }
    else
    {
        gPaletteFade.active = FALSE;
        temp = gPaletteFade.bufferTransferDisabled;
        gPaletteFade.bufferTransferDisabled = FALSE;
        if (gMain.callback2 == CB2_Overworld || gMain.callback2 == CB2_OverworldBasic || gMain.callback2 == BattleMainCB2)
        {
            u16 tintedPltt[PLTT_BUFFER_SIZE];
            CpuCopy16(gPlttBufferFaded, tintedPltt, PLTT_SIZE);
            ApplyDayNightTint(tintedPltt, PLTT_SIZE);
            if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) && (gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_SQUIRTLE || gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_BULBASAUR || gTrainerBattleOpponent_A == TRAINER_CHAMPION_FIRST_CHARMANDER || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_SQUIRTLE || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_BULBASAUR || gTrainerBattleOpponent_A == TRAINER_CHAMPION_REMATCH_CHARMANDER))
            {
                ApplyTypeTint(tintedPltt, PLTT_SIZE, gBattleMons[B_POSITION_OPPONENT_LEFT].type1);
            }
            CpuCopy32(tintedPltt, (void *)PLTT, PLTT_SIZE);
        }
        else
            CpuCopy32(gPlttBufferFaded, (void *)PLTT, PLTT_SIZE);
        sPlttBufferTransferPending = FALSE;
        if (gPaletteFade.mode == HARDWARE_FADE && gPaletteFade.active)
            UpdateBlendRegisters();
        gPaletteFade.bufferTransferDisabled = temp;
        return TRUE;
    }
    return TRUE;
}
