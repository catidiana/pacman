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

enum Pac_States {
    PAC_NONE,
    PAC_STAY,
    PAC_WALK_LEFT,
    PAC_WALK_RIGHT,
    PAC_WALK_DOWN,
    PAC_WALK_UP,
    PAC_DIES
};

class pacman
{
private:
    Image pacman_image;
    Image hellpacman_image;
    Sound dead_sound;
public:
    Image pacman_stay;
    V2 pac_coord;
    V2 matr_ceil;
    int hellmode = 0;
    Pac_States state;
    Pac_States awaiting_state;
    uint8_t pacman_lives;

    pacman() {
        pacman_image = load_image("res/pacman.png");
        hellpacman_image = load_image("res/hellpacman.png");
        dead_sound = load_sound("sound/death.wav");
        reset_pacman();
        pacman_lives = 3;
    }

    void reset_pacman () {
        pac_coord = {280, 190};
        define_matr_ceil();
        state = PAC_WALK_LEFT;
        awaiting_state = PAC_NONE;
    }

    void change_state (Pac_States new_state) {
        if (new_state != state) awaiting_state = new_state;
    }

    void define_matr_ceil() {
        matr_ceil.x = pac_coord.x/20;
        matr_ceil.y = (MAIN_WINDOW_INIT_HEIGHT - 1 - pac_coord.y)/20;
    }

    void action () {
        if (pac_coord.x%20 == 10 && pac_coord.y%20 == 10)
          {
            switch (awaiting_state) {
            case PAC_WALK_LEFT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0) {
                    state = awaiting_state;
                    awaiting_state = PAC_NONE;
                }
            } break;
            case PAC_WALK_RIGHT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28] > 0) {
                    state = awaiting_state;
                    awaiting_state = PAC_NONE;
                }
            } break;
            case PAC_WALK_DOWN: {
                if (WalkingMatr[matr_ceil.y + 1][matr_ceil.x] > 0) {
                    state = awaiting_state;
                    awaiting_state = PAC_NONE;
                }
            } break;
            case PAC_WALK_UP: {
                if (WalkingMatr[matr_ceil.y - 1][matr_ceil.x] > 0) {
                    state = awaiting_state;
                    awaiting_state = PAC_NONE;
                }
            } break;
            default:
                break;
            }
          }

        switch (state) {
        case PAC_WALK_LEFT: {
            if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] == 0 && pac_coord.x%20 == 10) state = PAC_STAY;
            else {pac_coord.x -= 2; if (pac_coord.x == 0) pac_coord.x = MAIN_WINDOW_INIT_WIDTH - 2;}
        } break;
        case PAC_WALK_RIGHT: {
            if (WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28] == 0 && pac_coord.x%20 == 10) state = PAC_STAY;
            else {pac_coord.x += 2; if (pac_coord.x == MAIN_WINDOW_INIT_WIDTH) pac_coord.x = 0;}
        } break;
        case PAC_WALK_DOWN: {
            if (WalkingMatr[matr_ceil.y+1][matr_ceil.x] == 0 && pac_coord.y%20 == 10) state = PAC_STAY;
            else {pac_coord.y -= 2;}
        } break;
        case PAC_WALK_UP: {
            if (WalkingMatr[matr_ceil.y-1][matr_ceil.x] == 0 && pac_coord.y%20 == 10) state = PAC_STAY;
            else {pac_coord.y += 2;}
        } break;
        case PAC_DIES:
            --pacman_lives;
            play_sound(dead_sound);
            break;
        case PAC_STAY:
        case PAC_NONE: break;
        }

        define_matr_ceil();
    }

    void draw (Image GameWindow, uint32_t frame) {
        switch (state) {
        case PAC_STAY: draw_image(GameWindow, pacman_image, pac_coord.x, pac_coord.y, 32, 32, 0 * 32, 0); break;
        case PAC_WALK_LEFT: draw_image(GameWindow, pacman_image, pac_coord.x, pac_coord.y, 32, 32, 1 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_RIGHT: draw_image(GameWindow, pacman_image, pac_coord.x, pac_coord.y, 32, 32, 2 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_DOWN: draw_image(GameWindow, pacman_image, pac_coord.x, pac_coord.y, 32, 32, 3 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_UP: draw_image(GameWindow, pacman_image, pac_coord.x, pac_coord.y, 32, 32, 4 * 32, (frame % 8 / 4) * 32); break;
        case PAC_DIES:
        case PAC_NONE: break;
        }

        for (int i = 0; i < pacman_lives - 1; ++i) {
            draw_image (GameWindow, pacman_image, 20 + 40 * i, 20, 32, 32, 2*32, 32);
        }
    }

    void helldraw (Image GameWindow, uint32_t frame) {
        switch (state) {
        case PAC_STAY:       draw_image(GameWindow, hellpacman_image, pac_coord.x, pac_coord.y, 32, 32, 0 * 32, 0); break;
        case PAC_WALK_LEFT:  draw_image(GameWindow, hellpacman_image, pac_coord.x, pac_coord.y, 32, 32, 1 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_RIGHT: draw_image(GameWindow, hellpacman_image, pac_coord.x, pac_coord.y, 32, 32, 2 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_DOWN:  draw_image(GameWindow, hellpacman_image, pac_coord.x, pac_coord.y, 32, 32, 3 * 32, (frame % 8 / 4) * 32); break;
        case PAC_WALK_UP:    draw_image(GameWindow, hellpacman_image, pac_coord.x, pac_coord.y, 32, 32, 4 * 32, (frame % 8 / 4) * 32); break;
        case PAC_DIES:
        case PAC_NONE: break;
        }

        for (int i = 0; i < pacman_lives - 1; ++i) {
            draw_image(GameWindow, pacman_image, 20 + 40 * i, 20, 32, 32, 2 * 32, 32);
        }
    }
};
