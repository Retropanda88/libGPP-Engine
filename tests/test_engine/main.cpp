#include <engine/Engine.h>
#include <input/Input.h>
#include <input/inputTypes.h>

#define MAX_PLAYERS 2
#define MAX_BUTTONS BUTTON_COUNT

void draw_input_state()
{
    u32 white  = color_rgb(255,255,255);
    u32 green  = color_rgb(0,255,0);
    u32 red    = color_rgb(255,0,0);
    u32 yellow = color_rgb(255,255,0);

    int y = 30;

    print(20, 10, "Input Test Suite", white);

    for (int p = 0; p < MAX_PLAYERS; p++)
    {
        if (!Input::isConnected(p))
            continue;

        char title[32];
        snprintf(title, sizeof(title), "Player %d", p);
        print(20, y, title, yellow);
        y += 15;

        for (int b = 0; b < MAX_BUTTONS; b++)
        {
            int down     = Input::isDown(p, (InputButton)b);
            int pressed  = Input::isPressed(p, (InputButton)b);
            int released = Input::isReleased(p, (InputButton)b);

            if (down || pressed || released)
            {
                char line[64];

                snprintf(line, sizeof(line),
                         "Btn %d  D:%d P:%d R:%d",
                         b, down, pressed, released);

                u32 color = white;

                if (pressed)
                    color = green;
                else if (released)
                    color = red;

                print(40, y, line, color);
                y += 15;
            }
        }

        y += 10;
    }
}

int main(int argc, char **argv)
{
    if (Init_Sistem("libGPP-Engine Input Test") != 0)
        return 1;

    if (Set_Video() != 0)
        return 1;

    Input::init();

    while (1)
    {
        fill_radial_gradient(
            logic,
            color_rgb(25,77,255),
            color_rgb(22,25,160)
        );

        Input::update();

        draw_input_state();

        Render();

#if defined(ANDROID_BUILD) || defined(PC_BUILD)
        SDL_Delay(16);
#endif
    }

    off_video();
    shoutdown_sistem();
    return 0;
}