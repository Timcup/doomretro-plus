#include "discord_rpc.h"
#include "discord.h"
#include "doomdef.h"       // gamestate, gameskill, gamemap, gameepisode
#include "d_player.h"      // players[]
#include "doomstat.h"
#include "g_game.h"
#include "p_setup.h"
#include <stdio.h>
#include <time.h>

#define DISCORD_APP_ID "1510639781397139626"

static int64_t startTimestamp;

static void HandleReady(const DiscordUser *user)
{
    // Connected — optional logging
}

static void HandleDisconnected(int errcode, const char *message) {}
static void HandleError(int errcode, const char *message) {}

void DISCORD_Init(void)
{
    SDL_Delay(3000);
    DiscordEventHandlers handlers = { 0 };
    handlers.ready = HandleReady;
    handlers.disconnected = HandleDisconnected;
    handlers.errored = HandleError;

    Discord_Initialize(DISCORD_APP_ID, &handlers, 1, NULL);

    DiscordRichPresence presence = { 0 };
    presence.details = "Searching for IWAD...";
    presence.largeImageKey = "icon_512x512";
    presence.largeImageText = "DOOM Retro+";
    Discord_UpdatePresence(&presence);

    startTimestamp = (int64_t)time(NULL);
}

void DISCORD_Update(void)
{
    DiscordRichPresence presence = { 0 };
    char details[128];
    char state[128];
    char titledetails[128];

    // Build details string based on game state
    if (gamestate == GS_LEVEL)
    {
        // Show map name
        snprintf(details, sizeof(details), "%s - %s", mapnum, maptitle);

        // Show skill level
        const char* skills[5];

        if (FREEDOOM)
        {
            skills[0] = "Please don't kill me";
            skills[1] = "Will this hurt?";
            skills[2] = "Bring on the pain";
            skills[3] = "Extreme Carnage";
            skills[4] = "Mayhem!";
        }
        else if (chex)
        {
            skills[0] = "Easy Does It";
            skills[1] = "Not So Sticky";
            skills[2] = "Gobs of God";
            skills[3] = "Extreme Ooze";
            skills[4] = "Super Slimey!";
        }
        else
        {
            skills[0] = "I'm Too Young to Die";
            skills[1] = "Hey, Not Too Rough";
            skills[2] = "Hurt Me Plenty";
            skills[3] = "Ultra-Violence";
            skills[4] = "Nightmare!";
        }

        snprintf(state, sizeof(state), "%s", skills[gameskill < 5 ? gameskill : 4]);

        presence.details       = details;
        presence.state         = state;
        presence.largeImageKey = "icon_512x512";
        presence.largeImageText = "DOOM Retro";
        presence.startTimestamp = startTimestamp;
    }
    else if (gamestate == GS_TITLESCREEN)
    {
        snprintf(titledetails, sizeof(titledetails), "%s - At the Title Screen", gamedescription);
        presence.details = titledetails;
        presence.largeImageKey = "icon_512x512";
        presence.largeImageText = "DOOM Retro";
    }
    else if (gamestate == GS_INTERMISSION)
    {
        snprintf(titledetails, sizeof(titledetails), "%s - Intermission", gamedescription);
        presence.details = titledetails;
        presence.largeImageKey = "icon_512x512";
    }

    Discord_UpdatePresence(&presence);
    Discord_RunCallbacks();
}

void DISCORD_Shutdown(void)
{
    Discord_Shutdown();
}