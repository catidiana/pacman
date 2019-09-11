
#define MAIN_WINDOW_INIT_WIDTH  560
#define MAIN_WINDOW_INIT_HEIGHT 720


#include <SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>



enum InputType {
    INPUT_NONE,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_DOWN,
    INPUT_UP
};

uint32_t GAME_SCORE = 0;
uint32_t LEVEL = 1;

#include "window_setup.cpp"
#include "walking_rules.cpp"
#include "pacman.cpp"
#include "food.cpp"
#include "ghost.cpp"



int
main (int argc, char **argv)
{
    printf("Setup Game... ");
    //**********SETUP**********
    SDL_Init (SDL_INIT_VIDEO);
    SDL_Window *main_window = SDL_CreateWindow ("Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                MAIN_WINDOW_INIT_WIDTH, MAIN_WINDOW_INIT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    assert (main_window);
    SDL_GLContext glcontext = SDL_GL_CreateContext (main_window);
    assert (glcontext);

    set_window_transform (MAIN_WINDOW_INIT_WIDTH, MAIN_WINDOW_INIT_HEIGHT);

    printf("success.\nCreate Game Window... ");
    glEnable (GL_TEXTURE_2D);
    glClearColor (0.20, 0.25, 0.30, 1.0);

    uint32_t window_w = MAIN_WINDOW_INIT_WIDTH;
    uint32_t window_h = MAIN_WINDOW_INIT_HEIGHT;
    Image GameWindow = new_image (window_w, window_h);
    uniform_fill(GameWindow, {0, 0, 240});
    printf("success.\nLoad images... ");

    Image background = load_image("res/background.png");
    Image level = load_image("res/level.png");
    Image high_score = load_image("res/high_score.png");
    Image digits[10];
    char t[9];
    strcpy(t,"res/0.png");
for (int i = 0; i < 10; i++) {
    *(t+4) = '0' +i;
    digits[i] = load_image(t);
}

    pacman PacMan;
    food Food;
    ghost_red Oikake;
    ghost_pink Machibuse;
    ghost_orange Otoboke;
    ghost_cyan Kimagure;

    printf("success.\nStart the main loop.\n ");
    for (int keep_running = 1; keep_running; )
    {
        uint32_t start_loop = SDL_GetTicks();
        static uint32_t s = 0;

        InputType input = INPUT_NONE;

        for (SDL_Event event; SDL_PollEvent (&event);)
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    window_w = event.window.data1;
                    window_h = event.window.data2;
                    set_window_transform (window_w, window_h);
                    break;
                }
            } break;
            case SDL_QUIT:
            {
                keep_running = 0;
                break;
            } break;
            case SDL_KEYDOWN:
            {
                if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_w:            input = INPUT_UP; break;
                    case SDLK_s:            input = INPUT_DOWN; break;
                    case SDLK_a:            input = INPUT_LEFT; break;
                    case SDLK_d:            input = INPUT_RIGHT; break;
                    case SDLK_UP:           input = INPUT_UP; break;
                    case SDLK_DOWN:         input = INPUT_DOWN; break;
                    case SDLK_LEFT:         input = INPUT_LEFT; break;
                    case SDLK_RIGHT:        input = INPUT_RIGHT; break;
                    }
                }
            } break;
            }
        }

        switch (input)
        {
        case INPUT_NONE: break;
        case INPUT_LEFT: PacMan.change_state(PAC_WALK_LEFT); break;
        case INPUT_RIGHT: PacMan.change_state(PAC_WALK_RIGHT); break;
        case INPUT_DOWN: PacMan.change_state(PAC_WALK_DOWN); break;
        case INPUT_UP: PacMan.change_state(PAC_WALK_UP); break;
        }

        glClear (GL_COLOR_BUFFER_BIT);

        //what happens here
        draw_image(GameWindow, background, MAIN_WINDOW_INIT_WIDTH/2, MAIN_WINDOW_INIT_HEIGHT/2);
        draw_image(GameWindow, level, 40, 700);
        draw_integer (GameWindow, digits, 100, 700, LEVEL);
        draw_integer (GameWindow, digits, 100, 680, GAME_SCORE);
        draw_image(GameWindow, high_score, 490, 700);

        Food.draw_food(GameWindow, s);
        PacMan.action(GameWindow, s);
        Food.eaten_food(PacMan);
       if (Food.energizer_mode == 1 && LEVEL < 5) {
            Oikake.awaiting_state = GHOST_FRIGHTENED;
            Machibuse.awaiting_state = GHOST_FRIGHTENED;
            Otoboke.awaiting_state = GHOST_FRIGHTENED;
            Kimagure.awaiting_state = GHOST_FRIGHTENED;
        }
        Oikake.action(GameWindow, s, PacMan);
        Machibuse.action(GameWindow, s, PacMan);
        Otoboke.action(GameWindow, s, PacMan);
        Kimagure.update_dependent(Oikake.matr_ceil);
        Kimagure.action(GameWindow, s, PacMan);


        ++s;
        update_image_texture (GameWindow);
        show_image           (GameWindow);


        SDL_GL_SwapWindow (main_window);
        uint32_t finish_loop = SDL_GetTicks();
        if (finish_loop < start_loop + 25) SDL_Delay(25 + start_loop - finish_loop);
    }

    return 0;
}
