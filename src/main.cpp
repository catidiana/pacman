/* PacMan Game
 *
 * Copyright (C) 2019 Martin & Diana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

typedef uint32_t u32;

u32 GAME_SCORE = 0;
u32 LEVEL = 1;

#include "window_setup.cpp"

struct Assets {
  Sound scared_sound;
  Sound start_sound;
  Sound win_sound;

  Image background_image;
  Image ready_image;
  Image you_win_image;
  Image game_over_image;
  Image pac_dies_image;
  Image cherries_image;
  Image font_image;
} assets;


static void
draw_text (Image des, Image font, uint32_t x, uint32_t y, const char *text)
{
  uint32_t glyph_w = 14;
  uint32_t glyph_h = font.h;

  for (char c = text[0]; c; c = (++text)[0])
    {
      if (c >= 'A' && c <= 'Z')
        {
          c = c - 'A' + 10;
          draw_image (des, font, x, y, glyph_w, glyph_h, c * glyph_w, 0);
        }
      else if (c >= '0' && c <= '9')
        {
          c = c - '0';
          draw_image (des, font, x, y, glyph_w, glyph_h, c * glyph_w, 0);
        }

      x += glyph_w;
    }
}


static void
draw_integer (Image des, Image font, uint32_t x_right_corner, uint32_t y_center, uint32_t number)
{
    uint32_t glyph_w = 14;
    uint32_t glyph_h = font.h;
    uint32_t x = x_right_corner - glyph_w / 2;
    uint32_t y = y_center;

    if (number == 0)
    {
      draw_image (des, font, x, y, glyph_w, glyph_h, 0, 0);
    }
    else
    {
        do
        {
            uint32_t digit = number % 10;
            number /= 10;
            draw_image (des, font, x, y, glyph_w, glyph_h, digit * glyph_w, 0);
            x -= glyph_w;
        }
        while (number);
    }
}


void draw_bonus (Image image_des, uint32_t number) {
    draw_text    (image_des, assets.font_image, 493, 20, "BONUS");
    draw_integer (image_des, assets.font_image, 480, 20, number);
}


#include "walking_rules.cpp"
#include "pacman.cpp"
#include "food.cpp"
#include "ghost.cpp"


int
main (int argc, char **argv)
{
    printf ("Setup Game...");
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
    {
        fprintf (stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio (44100, AUDIO_S16SYS, 2, 1024) < 0)
    {
        fprintf (stderr, "Error: Initializing SDL_mixer: %s\n", Mix_GetError ());
        return 1;
    }

    SDL_Window *main_window = SDL_CreateWindow ("Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                MAIN_WINDOW_INIT_WIDTH, MAIN_WINDOW_INIT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    assert (main_window);

    SDL_GLContext glcontext = SDL_GL_CreateContext (main_window);
    assert (glcontext);

    set_window_transform (MAIN_WINDOW_INIT_WIDTH, MAIN_WINDOW_INIT_HEIGHT);

    printf ("success.\n");
    printf ("Create Game Window...");

    glEnable (GL_TEXTURE_2D);
    glClearColor (0.20, 0.25, 0.30, 1.0);

    uint32_t window_w = MAIN_WINDOW_INIT_WIDTH;
    uint32_t window_h = MAIN_WINDOW_INIT_HEIGHT;
    uint32_t record = 0;
    FILE *f = fopen("record.txt", "r");
    if (f != NULL) fscanf(f, "%d", &record);
    Image GameWindow = new_image (window_w, window_h);
    uniform_fill(GameWindow, {0, 0, 240});

    printf ("success.\n");
    printf ("Load sounds...");

    assets.scared_sound = load_sound("sound/scared.wav");
    assets.start_sound  = load_sound("sound/start.wav");
    assets.win_sound    = load_sound("sound/win.wav");

    printf ("success.\n");
    printf ("Load images...");

    assets.background_image = load_image("res/background.png");
    assets.ready_image      = load_image("res/ready.png");
    assets.you_win_image    = load_image("res/you_win.png");
    assets.game_over_image  = load_image("res/game_over.png");
    assets.pac_dies_image   = load_image("res/pac_die.png");
    assets.font_image       = load_image("res/font.png");

    pacman PacMan;
    food Food;
    ghost_red Oikake;
    ghost_pink Machibuse;
    ghost_orange Otoboke;
    ghost_cyan Kimagure;

    printf ("success.\n");
    printf ("Start the main loop.\n");

    for (int keep_running = 1; keep_running; )
    {
        uint32_t loop_start_time = SDL_GetTicks ();
        static uint32_t frame = 0;

        for (SDL_Event event; SDL_PollEvent (&event); )
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
                    case SDLK_UP:
                    case SDLK_w:
                      PacMan.change_state(PAC_WALK_UP); break;
                    case SDLK_DOWN:
                    case SDLK_s:
                      PacMan.change_state(PAC_WALK_DOWN); break;
                    case SDLK_LEFT:
                    case SDLK_a:
                      PacMan.change_state(PAC_WALK_LEFT); break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                      PacMan.change_state(PAC_WALK_RIGHT); break;
                    }
                }
            } break;
            }
        }

        glClear (GL_COLOR_BUFFER_BIT);
      
        if (Food.food_counter == 0) {
            SDL_Delay(5000);
            LEVEL++;
            PacMan.pacman_lives = 3;
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
            if (PacMan.pacman_lives == 0) {
                frame = 0;
                PacMan.pacman_lives = 3;
                Food.refill_food();
            }
        }

        draw_image   (GameWindow, assets.background_image, MAIN_WINDOW_INIT_WIDTH/2, MAIN_WINDOW_INIT_HEIGHT/2);
        draw_text    (GameWindow, assets.font_image,  11, 700, "LEVEL");
        draw_integer (GameWindow, assets.font_image, 100, 700, LEVEL);
        draw_integer (GameWindow, assets.font_image, 556, 680, record);
        draw_integer (GameWindow, assets.font_image, 100, 680, GAME_SCORE);
        draw_text    (GameWindow, assets.font_image, 423, 700, "HIGH SCORE");

        Food.draw_food(GameWindow, frame);
        PacMan.action();
        Food.eaten_food(PacMan, GameWindow);
        if (Food.energizer_mode == 1 && LEVEL < 10) {
            dead_bonus_count = 0;
            play_sound(assets.scared_sound);
            Oikake.awaiting_state    = GHOST_FRIGHTENED;
            Machibuse.awaiting_state = GHOST_FRIGHTENED;
            Otoboke.awaiting_state   = GHOST_FRIGHTENED;
            Kimagure.awaiting_state  = GHOST_FRIGHTENED;
        }

        check_pacman_life (PacMan, Oikake, Machibuse, Otoboke, Kimagure);

        Oikake.action   (GameWindow, frame, PacMan);
        Machibuse.action(GameWindow, frame, PacMan);
        Otoboke.action  (GameWindow, frame, PacMan);
        Kimagure.update_dependent(Oikake.matr_ceil);
        Kimagure.action (GameWindow, frame, PacMan);

        if (PacMan.state != PAC_DIES) check_pacman_life (PacMan, Oikake, Machibuse, Otoboke, Kimagure);

        if (PacMan.pacman_lives == 0)  {
            draw_image(GameWindow, assets.game_over_image, 280, 310);
            draw_image(GameWindow, assets.pac_dies_image, PacMan.pac_coord.x, PacMan.pac_coord.y);
            if (GAME_SCORE > record) {
                record = GAME_SCORE;
                FILE *f = fopen("record.txt", "w");
                fprintf(f, "%d", record);
            }
            LEVEL = 1;
            GAME_SCORE = 0;
        }


        if (Food.food_counter == 0) {
            draw_image(GameWindow, assets.you_win_image, MAIN_WINDOW_INIT_WIDTH/2, MAIN_WINDOW_INIT_HEIGHT/2);
            play_sound(assets.win_sound);
            draw_image(GameWindow, PacMan.pacman_stay, PacMan.pac_coord.x, PacMan.pac_coord.y);
        }

        if (frame == 0) {
          play_sound (assets.start_sound);
          draw_image (GameWindow, assets.ready_image, 280, 310);
        }

        ++frame;

        PacMan.draw (GameWindow, frame);
        update_image_texture (GameWindow);
        show_image           (GameWindow);

        SDL_GL_SwapWindow (main_window);
        uint32_t loop_end_time = SDL_GetTicks();
        if (loop_end_time < loop_start_time + 22) SDL_Delay (22 + loop_start_time - loop_end_time);
    }

    return 0;
}
