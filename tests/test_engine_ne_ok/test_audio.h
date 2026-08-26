#ifndef AUDIO_TEST_H
#define AUDIO_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
#include <font/sysfont.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SDL_Surface *logic;
extern Cmixer mixer;

#define LIST_VISIBLE 9
#define MAX_NAME_LEN 64
#define MAX_VOLUME   128

// ============================
// UI COLORS
// ============================

#define UI_BG            color_rgb(10, 15, 30)
#define UI_BG2           color_rgb(20, 30, 50)

#define UI_WHITE         color_rgb(255,255,255)
#define UI_GRAY          color_rgb(130,130,140)

#define UI_CYAN          color_rgb(0,255,255)
#define UI_YELLOW        color_rgb(255,200,0)
#define UI_GREEN         color_rgb(0,255,0)

#define UI_SELECT_BLUE   color_rgb(40,60,120)
#define UI_SELECT_GOLD   color_rgb(130,90,20)

#define UI_SCROLL_BG     color_rgb(30,35,60)
#define UI_PANEL_DARK    color_rgb(5,5,10)

// ============================
// AUDIO LIST
// ============================

struct AudioList
{
    char (*names)[MAX_NAME_LEN];

    int count;
    int selected;
    int scroll;
};

extern AudioList musicList;
extern AudioList sfxList;

extern int global_volume;

// Banco precargado de SFX
extern CSample* sfxPreloadedBank;

// ============================
// AUDIO TEST
// ============================

inline void run_audio_test()
{
    int activeCol = 0;

    bool exiting = false;

    float marqueeTimer = 0.0f;
    int charOffset = 0;

    bool u_l = true;
    bool d_l = true;
    bool l_l = true;
    bool r_l = true;
    bool a_l = true;
    bool b_l = true;

    static char safePath[256];

    while (!exiting)
    {
        // ============================
        // INPUT
        // ============================

        Input::update();

        bool u = Input::isDown(0, BUTTON_UP);
        bool d = Input::isDown(0, BUTTON_DOWN);
        bool l = Input::isDown(0, BUTTON_LEFT);
        bool r = Input::isDown(0, BUTTON_RIGHT);

        bool a = Input::isDown(0, BUTTON_A);
        bool b = Input::isDown(0, BUTTON_B);

        AudioList* cur =
            (activeCol == 0) ? &musicList : &sfxList;

        // ============================
        // EXIT
        // ============================

        if (b && !b_l)
        {
            exiting = true;
        }

        // ============================
        // CHANGE TAB
        // ============================

        if ((l && !l_l) || (r && !r_l))
        {
            activeCol = (activeCol == 0) ? 1 : 0;

            marqueeTimer = 0.0f;
            charOffset = 0;

            cur =
                (activeCol == 0) ? &musicList : &sfxList;
        }

        // ============================
        // LIST NAVIGATION
        // ============================

        if (d && !d_l && cur->count > 0)
        {
            cur->selected =
                (cur->selected + 1) % cur->count;

            marqueeTimer = 0.0f;
            charOffset = 0;
        }

        if (u && !u_l && cur->count > 0)
        {
            cur->selected =
                (cur->selected - 1 + cur->count) % cur->count;

            marqueeTimer = 0.0f;
            charOffset = 0;
        }

        // ============================
        // VOLUME
        // ============================

        if (Input::isDown(0, BUTTON_L1) &&
            global_volume > 0)
        {
            global_volume -= 2;
        }

        if (Input::isDown(0, BUTTON_R1) &&
            global_volume < MAX_VOLUME)
        {
            global_volume += 2;
        }

        mixer.setMasterVolume(global_volume);

        // ============================
        // SCROLL
        // ============================

        if (cur->selected >= cur->scroll + LIST_VISIBLE)
        {
            cur->scroll =
                cur->selected - LIST_VISIBLE + 1;
        }

        if (cur->selected < cur->scroll)
        {
            cur->scroll = cur->selected;
        }

        // ============================
        // PLAY AUDIO
        // ============================

        if (a &&
            !a_l &&
            cur->count > 0 &&
            cur->names != NULL)
        {
            if (activeCol == 0)
            {
                snprintf(
                    safePath,
                    sizeof(safePath),
                    "music/%s",
                    cur->names[cur->selected]
                );

                mixer.stopMusic();

                mixer.playMusic(
                    safePath,
                    true
                );
            }
            else
            {
                if (sfxPreloadedBank != NULL)
                {
                    mixer.playChannel(
                        &sfxPreloadedBank[cur->selected],
                        false,
                        global_volume
                    );
                }
            }
        }

        // ============================
        // DRAW BACKGROUND
        // ============================

        fill_vertical_gradient(
            logic,
            UI_BG,
            UI_BG2
        );

        // ============================
        // HEADER
        // ============================

        if (activeCol == 0)
        {
            sysfontDrawString(
                15,
                10,
                "AUDIO EXPLORER -> [ BACKGROUND MUSIC ]",
                UI_CYAN
            );
        }
        else
        {
            sysfontDrawString(
                15,
                10,
                "AUDIO EXPLORER -> [ SOUND EFFECTS ]",
                UI_YELLOW
            );
        }

        fill_rect(
            logic,
            15,
            22,
            290,
            1,
            color_rgb(60,60,80)
        );

        // ============================
        // AUDIO LIST
        // ============================

        int startX = 20;

        for (int i = 0; i < LIST_VISIBLE; i++)
        {
            int idx = cur->scroll + i;

            if (idx < cur->count &&
                cur->names != NULL)
            {
                int py = 42 + (i * 18);

                // ====================
                // SELECTED ITEM
                // ====================

                if (idx == cur->selected)
                {
                    unsigned int barColor =
                        (activeCol == 0)
                        ? UI_SELECT_BLUE
                        : UI_SELECT_GOLD;

                    fill_rect(
                        logic,
                        startX - 4,
                        py - 2,
                        265,
                        14,
                        barColor
                    );

                    int nLen =
                        strlen(cur->names[idx]);

                    // Marquee scrolling
                    if (nLen > 28)
                    {
                        marqueeTimer += 0.15f;

                        if (marqueeTimer > 1.0f)
                        {
                            marqueeTimer = 0.0f;

                            charOffset++;

                            if (charOffset > nLen - 10)
                            {
                                charOffset = 0;
                            }
                        }

                        char db[32];

                        strncpy(
                            db,
                            &cur->names[idx][charOffset],
                            29
                        );

                        db[29] = '\0';

                        sysfontDrawString(
                            startX,
                            py,
                            db,
                            UI_WHITE
                        );
                    }
                    else
                    {
                        sysfontDrawString(
                            startX,
                            py,
                            cur->names[idx],
                            UI_WHITE
                        );
                    }
                }
                else
                {
                    char sn[32];

                    if (strlen(cur->names[idx]) > 28)
                    {
                        strncpy(
                            sn,
                            cur->names[idx],
                            25
                        );

                        sn[25] = '\0';

                        strcat(sn, "...");

                        sysfontDrawString(
                            startX,
                            py,
                            sn,
                            UI_GRAY
                        );
                    }
                    else
                    {
                        sysfontDrawString(
                            startX,
                            py,
                            cur->names[idx],
                            UI_GRAY
                        );
                    }
                }
            }
        }

        // ============================
        // SCROLLBAR
        // ============================

        int scrollAreaY = 40;
        int scrollAreaH = 155;

        int scrollbarX = 295;

        fill_rect(
            logic,
            scrollbarX,
            scrollAreaY,
            4,
            scrollAreaH,
            UI_SCROLL_BG
        );

        if (cur->count > 0)
        {
            int barHeight =
                (cur->count > LIST_VISIBLE)
                ? (scrollAreaH * LIST_VISIBLE) / cur->count
                : scrollAreaH;

            if (barHeight < 15)
            {
                barHeight = 15;
            }

            int maxScrollIdx =
                (cur->count > LIST_VISIBLE)
                ? (cur->count - LIST_VISIBLE)
                : 1;

            int sY =
                scrollAreaY +
                (cur->scroll *
                (scrollAreaH - barHeight) /
                maxScrollIdx);

            unsigned int scrollColor =
                (activeCol == 0)
                ? UI_CYAN
                : UI_YELLOW;

            fill_rect(
                logic,
                scrollbarX,
                sY,
                4,
                barHeight,
                scrollColor
            );
        }

        // ============================
        // TAB INDICATORS
        // ============================

        fill_rect(
            logic,
            145,
            202,
            10,
            4,
            (activeCol == 0)
            ? UI_CYAN
            : color_rgb(40,40,50)
        );

        fill_rect(
            logic,
            165,
            202,
            10,
            4,
            (activeCol == 1)
            ? UI_YELLOW
            : color_rgb(40,40,50)
        );

        // ============================
        // BOTTOM PANEL
        // ============================

        fill_rect(
            logic,
            0,
            212,
            320,
            28,
            UI_PANEL_DARK
        );

        int porcentaje =
            (global_volume * 100) / MAX_VOLUME;

        sysfontPrintF(
            12,
            220,
            UI_GREEN,
            "VOLUME: %d%%",
            porcentaje
        );

        // Volume Bar BG
        fill_rect(
            logic,
            115,
            223,
            50,
            6,
            color_rgb(40,40,40)
        );

        // Volume Fill
        fill_rect(
            logic,
            115,
            223,
            (global_volume * 50 / MAX_VOLUME),
            6,
            UI_GREEN
        );

        sysfontDrawString(
            175,
            223,
            "A:PLAY L1/R1:VOL L/R:TABS",
            UI_GRAY
        );

        // ============================
        // UPDATE INPUT STATES
        // ============================

        u_l = u;
        d_l = d;

        l_l = l;
        r_l = r;

        a_l = a;
        b_l = b;

        // ============================
        // RENDER
        // ============================

        Render();

        Fps_sincronizar(60);
    }
}

#endif