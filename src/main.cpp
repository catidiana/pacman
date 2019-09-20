
#define MAIN_WINDOW_INIT_WIDTH  560
#define MAIN_WINDOW_INIT_HEIGHT 720


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

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
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    }
    if (Mix_OpenAudio (44100, AUDIO_S16SYS, 2, 1024) < 0)
    {
        fprintf (stderr, "Error: Initializing SDL_mixer: %s\n", Mix_GetError ());
    }
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
    uint32_t record = 0;
    FILE *f = fopen("record.txt", "r");
    if (f != NULL) fscanf(f, "%d", &record);
    Image GameWindow = new_image (window_w, window_h);
    uniform_fill(GameWindow, {0, 0, 240});
    printf("success.\nLoad sounds... ");
    Sound scared_sound = load_sound("sound/scared.wav");
    Sound start = load_sound("sound/start.wav");
    Sound win = load_sound("sound/win.wav");
    printf("success.\nLoad images... ");

    Image background = load_image("res/background.png");
    Image level = load_image("res/level.png");
    Image high_score = load_image("res/high_score.png");
    Image ready = load_image("res/ready.png");
    Image you_win = load_image("res/you_win.png");
    Image game_over = load_image("res/game_over.png");
    Image pac_dies = load_image("res/pac_die.png");
    Image digits[10];
    char t[10];
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
        static uint32_t frame = 0;


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
                    case SDLK_w:            PacMan.change_state(PAC_WALK_UP); break;
                    case SDLK_s:            PacMan.change_state(PAC_WALK_DOWN); break;
                    case SDLK_a:            PacMan.change_state(PAC_WALK_LEFT); break;
                    case SDLK_d:            PacMan.change_state(PAC_WALK_RIGHT); break;
                    case SDLK_UP:           PacMan.change_state(PAC_WALK_UP); break;
                    case SDLK_DOWN:         PacMan.change_state(PAC_WALK_DOWN); break;
                    case SDLK_LEFT:         PacMan.change_state(PAC_WALK_LEFT); break;
                    case SDLK_RIGHT:        PacMan.change_state(PAC_WALK_RIGHT); break;
                    }
                }
            } break;
            }
        }

        glClear (GL_COLOR_BUFFER_BIT);

        //what happens here
        if (Food.food_counter == 0) {
            SDL_Delay(5000);
            LEVEL++;
            PacMan.pacman_lifes = 3;
            PacMan.reset_pacman();
            Food.refill_food();
            Oikake.reset_ghost();
            Machibuse.reset_ghost();
            Otoboke.reset_ghost();
            Kimagure.reset_ghost();
        }

        if (frame == 1) {
            SDL_Delay(4000);
        }

        if(PacMan.state == PAC_DIES) {
            SDL_Delay(2000);
            PacMan.reset_pacman();
            Oikake.reset_ghost();
            Machibuse.reset_ghost();
            Otoboke.reset_ghost();
            Kimagure.reset_ghost();
            if (PacMan.pacman_lifes == 0) {
                frame = 0;
                PacMan.pacman_lifes = 3;
                Food.refill_food();
            }
        }


        draw_image(GameWindow, background, MAIN_WINDOW_INIT_WIDTH/2, MAIN_WINDOW_INIT_HEIGHT/2);
        draw_image(GameWindow, level, 40, 700);
        draw_integer (GameWindow, digits, 100, 700, LEVEL);
        draw_integer (GameWindow, digits, 556, 680, record);
        draw_integer (GameWindow, digits, 100, 680, GAME_SCORE);
        draw_image(GameWindow, high_score, 490, 700);

        Food.draw_food(GameWindow, frame);
        PacMan.action(GameWindow, frame);
        Food.eaten_food(PacMan, GameWindow, digits);
        if (Food.energizer_mode == 1 && LEVEL < 10) {
            dead_bonus_count = 0;
            play_sound(scared_sound);
            Oikake.awaiting_state = GHOST_FRIGHTENED;
            Machibuse.awaiting_state = GHOST_FRIGHTENED;
            Otoboke.awaiting_state = GHOST_FRIGHTENED;
            Kimagure.awaiting_state = GHOST_FRIGHTENED;
        }

        check_pacman_life (GameWindow, frame, PacMan, Oikake, Machibuse, Otoboke, Kimagure);

        Oikake.action(GameWindow, digits, frame, PacMan);
        Machibuse.action(GameWindow, digits, frame, PacMan);
        Otoboke.action(GameWindow, digits, frame, PacMan);
        Kimagure.update_dependent(Oikake.matr_ceil);
        Kimagure.action(GameWindow, digits, frame, PacMan);


        if (PacMan.state != PAC_DIES) check_pacman_life (GameWindow, frame, PacMan, Oikake, Machibuse, Otoboke, Kimagure);

        if (PacMan.pacman_lifes == 0)  {
            draw_image(GameWindow, game_over, 280, 310);
            draw_image(GameWindow, pac_dies, PacMan.pac_coord.x, PacMan.pac_coord.y);
            if (GAME_SCORE > record) {
                record = GAME_SCORE;
                FILE *f = fopen("record.txt", "w");
                fprintf(f, "%d", record);
            }
            LEVEL = 1;
            GAME_SCORE = 0;
        }


        if (Food.food_counter == 0) {
            draw_image(GameWindow, you_win, MAIN_WINDOW_INIT_WIDTH/2, MAIN_WINDOW_INIT_HEIGHT/2);
            play_sound(win);
            draw_image(GameWindow, PacMan.pacman_stay, PacMan.pac_coord.x, PacMan.pac_coord.y);
        }

        if (frame == 0) {
            play_sound(start);
            FILE *f = fopen("record.txt", "r");
            if (f != NULL) fscanf(f, "%d", &record);
            draw_image(GameWindow, ready, 280, 310);
        }

        ++frame;

        update_image_texture (GameWindow);
        show_image           (GameWindow);


        SDL_GL_SwapWindow (main_window);
        uint32_t finish_loop = SDL_GetTicks();
        if (finish_loop < start_loop + 22) SDL_Delay(22 + start_loop - finish_loop);
    }

    return 0;
}
