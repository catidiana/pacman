enum Ghost_States {
    GHOST_NONE,
    GHOST_SHY,
    GHOST_TRANSFER,
    GHOST_WALK,
    GHOST_HUNT,
    GHOST_FRIGHTENED,
    GHOST_EATEN
};

enum Ghost_Type {
    OIKAKE_RED,
    MACHIBUSE_PINK,
    OTOBOKE_ORANGE,
    KIMAGURE_CYAN
};

enum Ghost_Direction {
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
    DIR_RIGHT
};

uint8_t dead_bonus_count = 0;

class ghost
{
protected:
    Ghost_Type type;
    Image ghost_fright[4];
    Image ghost_dead[4];
    Image bonus_;
    Sound ghost_b;
    int shy_time;
    uint32_t start_time;
    uint32_t scared_time;
    uint32_t scared_interval;
    V2 walk_target_ceil;
    uint8_t wave;
    uint32_t walking_time[4];
    uint32_t hunting_time[4];
    bool bonus;

public:
    Ghost_States state;
    Ghost_States awaiting_state;
    Ghost_Direction direction;
    V2 gh_coord;
    V2 reset_coord;
    V2 matr_ceil;


    ghost (Ghost_Type type_, V2 target_, V2 res_coord_) : type(type_), walk_target_ceil(target_), reset_coord(res_coord_) {
        reset_ghost();
        bonus_ = load_image("res/bonus.png");
        ghost_b = load_sound("sound/bonus.wav");

        ghost_fright[0]  = load_image("res/ghost_fr_1.png");
        ghost_fright[1]  = load_image("res/ghost_fr_2.png");
        ghost_fright[2]  = load_image("res/ghost_fr_3.png");
        ghost_fright[3]  = load_image("res/ghost_fr_4.png");

        ghost_dead[0]  = load_image("res/dead_ghost_up.png");
        ghost_dead[1]  = load_image("res/dead_ghost_left.png");
        ghost_dead[2]  = load_image("res/dead_ghost_down.png");
        ghost_dead[3]  = load_image("res/dead_ghost_right.png");
    }
    void reset_ghost() {
        shy_time = 1000*type;
        if (LEVEL > 5) shy_time = 1000;
        wave = 0;
        gh_coord.x = reset_coord.x;
        gh_coord.y = reset_coord.y;
        define_matr_ceil();
        bonus = false;
        for (int i = 0; i < 4; i++) {
            walking_time[i] = 8000;
            if (i > 2) walking_time[i] -= 2000;
            if (LEVEL < 5) walking_time[i] -=1000*LEVEL;
            hunting_time[i] = 15000 + LEVEL*5000;
        }
        if (type == OIKAKE_RED) {
            state = GHOST_TRANSFER;
            awaiting_state = GHOST_WALK;
            direction = DIR_RIGHT;
        } else {
            state = GHOST_SHY;
            awaiting_state = GHOST_TRANSFER;
            direction = DIR_LEFT;
        }
        start_time = SDL_GetTicks();
        scared_time = SDL_GetTicks();
        if (LEVEL < 8) scared_interval = 10000 - LEVEL*1000;
        else scared_interval = 3000;
    }
    void define_matr_ceil() {
        matr_ceil.x = gh_coord.x/20;
        matr_ceil.y = (MAIN_WINDOW_INIT_HEIGHT - 1 - gh_coord.y)/20;
    }
    void shy_mode (uint32_t s) {
        if (s%40 < 20) {
            --gh_coord.x;
            direction = DIR_LEFT;
        } else { ++gh_coord.x;
            direction = DIR_RIGHT;
        }
    }
    void transfer_mode () {
        if (gh_coord.x != 290) {
            if (gh_coord.x < 290) {
                gh_coord.x +=2;
                direction = DIR_RIGHT;
            } else {
                gh_coord.x-=2;
                direction = DIR_LEFT;
            }
        }
        else if (gh_coord.y != 430) {
            if (gh_coord.y < 430) {
                gh_coord.y+= 2;
                direction = DIR_UP;
            } else {
                gh_coord.y-= 2;
                direction = DIR_DOWN;
            }
        }
    }
    void walk_to_left(uint8_t speed = 2) {
        gh_coord.x -= speed;
        if (gh_coord.x == 0) gh_coord.x = MAIN_WINDOW_INIT_WIDTH - 2;
        direction = DIR_LEFT;
    }
    void walk_to_right(uint8_t speed = 2) {
        gh_coord.x += speed;
        if (gh_coord.x == MAIN_WINDOW_INIT_WIDTH) gh_coord.x = 0;
        direction = DIR_RIGHT;
    }
    void walk_to_down(uint8_t speed = 2) {
        gh_coord.y -= speed;
        direction = DIR_DOWN;
    }
    void walk_to_up(uint8_t speed = 2) {
        gh_coord.y += speed;
        direction = DIR_UP;
    }
    void continue_walk (uint8_t speed = 2) {
        switch (direction) {
        case DIR_UP: walk_to_up(speed);
            break;
        case DIR_LEFT: walk_to_left(speed);
            break;
        case DIR_DOWN: walk_to_down(speed);
            break;

        case DIR_RIGHT: walk_to_right(speed);
            break;
        }
    }
    Ghost_Direction find_available_direction() {
        if (WalkingMatr[matr_ceil.y - 1][matr_ceil.x] > 0 && direction != DIR_DOWN) return DIR_UP;
        if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0 && direction != DIR_RIGHT) return DIR_LEFT;
        if (WalkingMatr[matr_ceil.y + 1][matr_ceil.x] > 0 && direction != DIR_UP) return DIR_DOWN;
        return DIR_RIGHT;
    }
    Ghost_Direction find_direction_to_pacman(pacman PacMan) {
        if (WalkingMatr[matr_ceil.y - 1][matr_ceil.x] > 0 && PacMan.matr_ceil.y < matr_ceil.y) return DIR_UP;
        if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0 && PacMan.matr_ceil.x > matr_ceil.x) return DIR_LEFT;
        if (WalkingMatr[matr_ceil.y + 1][matr_ceil.x] > 0 && PacMan.matr_ceil.y > matr_ceil.y) return DIR_DOWN;
        if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0 && PacMan.matr_ceil.x < matr_ceil.x)return DIR_RIGHT;
        return find_available_direction();
    }
    void walk_to_target(V2 matr_target, uint8_t speed = 2) {
        if (gh_coord.x%20 != 10 || gh_coord.y%20 != 10) {
            continue_walk(speed);
            return;
        }
        if (WalkingMatr[matr_ceil.y][matr_ceil.x] == 1) {
            switch (direction) {
            case DIR_UP: {
                if (WalkingMatr[matr_ceil.y - 1][matr_ceil.x] > 0) {
                    continue_walk(speed);
                    return;
                }
            } break;
            case DIR_LEFT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0) {
                    continue_walk(speed);
                    return;
                }
            } break;
            case DIR_DOWN: {
                if (WalkingMatr[matr_ceil.y + 1][matr_ceil.x] > 0) {
                    continue_walk(speed);
                    return;
                }
            } break;
            case DIR_RIGHT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28] > 0) {
                    continue_walk(speed);
                    return;
                }
            } break;
            }
            direction = find_available_direction();
            continue_walk(speed);
        } else {
            bool available_ceils[4];
            int target_distance[4];
            //up - left - down - right;
            available_ceils[0] = WalkingMatr[matr_ceil.y - 1][matr_ceil.x];
            available_ceils[1] = WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28];
            available_ceils[2] = WalkingMatr[matr_ceil.y + 1][matr_ceil.x];
            available_ceils[3] = WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28];
            int x_dir = (int)matr_ceil.x - (int)matr_target.x;
            int y_dir = (int)matr_ceil.y - (int)matr_target.y;
            target_distance[0] = x_dir*x_dir + (y_dir - 1)*(y_dir - 1);
            target_distance[1] = (x_dir - 1)*(x_dir - 1) + y_dir*y_dir;
            target_distance[2] = x_dir*x_dir +(y_dir + 1)*(y_dir + 1);
            target_distance[3] = (x_dir + 1)*(x_dir + 1) + y_dir*y_dir;
            for (int i = 0; i < 4; i++) {
                if (available_ceils[i] == false) target_distance[i] = 3000;
            }
            target_distance[(direction + 2)%4] = 3000;
            int selected_direction = DIR_UP;
            int min_distance = target_distance[DIR_UP];
            for (int i = 1; i < 4; i++) {
                if (target_distance[i] < min_distance) {
                    selected_direction = i;
                    min_distance = target_distance[i];
                }
            }
            switch (selected_direction) {
            case 0: direction = DIR_UP;
                break;
            case 1: direction = DIR_LEFT;
                break;
            case 2: direction = DIR_DOWN;
                break;
            default: direction = DIR_RIGHT;
                break;
            }
            continue_walk(speed);
        }
    }

    virtual void hunt (pacman PacMan) {
        walk_to_target(PacMan.matr_ceil);
    }
    virtual void draw_avatar (Image GameWindow, uint32_t s) {
        draw_image(GameWindow, ghost_fright[(s%16)/4], gh_coord.x, gh_coord.y);
    }    
    void ghost_eaten_check(pacman PacMan) {
        if (matr_ceil.y == PacMan.matr_ceil.y && matr_ceil.x == PacMan.matr_ceil.x) {
            awaiting_state = GHOST_EATEN;
            if (bonus == false) {
                bonus = true;
                play_sound(ghost_b);
                ++dead_bonus_count;
                GAME_SCORE +=200*dead_bonus_count;
            }
        }
    }
    void calculating_path(Image GameWindow, Image* image_digits, uint32_t s, pacman PacMan) {
        switch (awaiting_state) {

        case GHOST_SHY: {
            if (matr_ceil.x == 14 && matr_ceil.y == 14) {
                gh_coord.x = 290 + 40*(rand()%3 - 1);
                gh_coord.y = 370;
                direction = DIR_LEFT;
                shy_time += 1000;
                start_time = SDL_GetTicks();
                define_matr_ceil();
                state = GHOST_SHY;
                awaiting_state = GHOST_TRANSFER;
            }
            draw_bonus (GameWindow, bonus_, 200*dead_bonus_count);
        }
            break;
        case GHOST_TRANSFER: {
            uint32_t current_time = SDL_GetTicks();
            if (current_time  >= start_time + shy_time && gh_coord.x%2 == 0 && gh_coord.y%2 == 0) {
                state = GHOST_TRANSFER;
                awaiting_state = GHOST_WALK;
            }
        } break;

        case GHOST_WALK: {
            if (state == GHOST_HUNT) {
                uint32_t current_time = SDL_GetTicks();
                if (current_time >= start_time + walking_time[wave] && gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                    state = GHOST_WALK;
                    awaiting_state = GHOST_HUNT;
                    start_time = SDL_GetTicks();
                    wave++;
                }
            } else if (state == GHOST_TRANSFER && gh_coord.x == 290 && gh_coord.y == 430) {
                state = GHOST_WALK;
                awaiting_state = GHOST_HUNT;
                start_time = SDL_GetTicks();
            }
            else if (state == GHOST_FRIGHTENED) {
                uint32_t current_time = SDL_GetTicks();
                if (current_time >= scared_time + scared_interval && gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                    state = GHOST_WALK;
                    awaiting_state = GHOST_HUNT;
                    start_time += scared_interval;
                }
            }
        } break;

        case GHOST_HUNT: {
            if (state == GHOST_WALK) {
                uint32_t current_time = SDL_GetTicks();
                if (current_time >= start_time + walking_time[wave] && gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                    state = GHOST_HUNT;
                    if (wave < 3) awaiting_state = GHOST_WALK;
                    else awaiting_state = GHOST_NONE;
                    direction = find_direction_to_pacman(PacMan);
                    start_time = SDL_GetTicks();
                }
            } else if (state == GHOST_FRIGHTENED) {
                uint32_t current_time = SDL_GetTicks();
                if (current_time >= scared_time + scared_interval && gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                    state = GHOST_HUNT;
                    awaiting_state = GHOST_WALK;
                    start_time -= scared_interval;
                }
            }
        } break;

        case GHOST_FRIGHTENED: {

            switch (state) {
            case GHOST_TRANSFER: {
                gh_coord.x = 290;
                gh_coord.y = 430;
                define_matr_ceil();
                awaiting_state = GHOST_WALK;
                state = GHOST_FRIGHTENED;
                scared_time = SDL_GetTicks();
            }
                break;
            case GHOST_SHY: {
                start_time = start_time + scared_interval;
                awaiting_state = GHOST_TRANSFER;
            }
                break;
            case GHOST_FRIGHTENED: {
                awaiting_state = GHOST_WALK;
                scared_time = SDL_GetTicks();
            }
                break;
            default: {
                awaiting_state = state;
                state = GHOST_FRIGHTENED;
                scared_time = SDL_GetTicks();
            }
                break;
            }

        } break;
        case GHOST_EATEN: {
            if (gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                state = GHOST_EATEN;
                awaiting_state = GHOST_SHY;
            }
        } break;
        default:
            break;
        }
        switch (state) {
        case GHOST_SHY: shy_mode(s);
            break;
        case GHOST_TRANSFER: transfer_mode();
            break;
        case GHOST_WALK: walk_to_target(walk_target_ceil);
            break;
        case GHOST_HUNT: hunt(PacMan);
            break;
        case GHOST_FRIGHTENED: {
            ghost_eaten_check(PacMan);
            walk_to_target({35 - PacMan.matr_ceil.y, 27 - PacMan.matr_ceil.x}, 1);
            define_matr_ceil();
            if (awaiting_state != GHOST_EATEN) ghost_eaten_check(PacMan);
            uint32_t current_time = SDL_GetTicks();
            if (current_time + 2000 < scared_time + scared_interval) draw_image(GameWindow, ghost_fright[(s%8)/4], gh_coord.x, gh_coord.y);
            else draw_image(GameWindow, ghost_fright[(s%16)/4], gh_coord.x, gh_coord.y);

        } break;
        case GHOST_EATEN: {
            walk_to_target({14, 14}, 5);
            bonus = false;
            draw_image(GameWindow, ghost_dead[direction], gh_coord.x, gh_coord.y);
            draw_bonus (GameWindow, bonus_, 200*dead_bonus_count);
        }
            break;
        default:
            break;
        }
    }
    void action (Image GameWindow, Image* image_digits, uint32_t s, pacman PacMan) {
        calculating_path(GameWindow, image_digits, s, PacMan);
        define_matr_ceil();
        draw_avatar (GameWindow, s);
    }
};

class ghost_red : public ghost
{
private:
    Image ghost_red_mask[4][2];
public:
    ghost_red () : ghost(OIKAKE_RED, {33, 0}, {280, 430}) {
        ghost_red_mask[0][0]  = load_image("res/ghost_red_up_1.png");
        ghost_red_mask[0][1]  = load_image("res/ghost_red_up_2.png");
        ghost_red_mask[1][0]  = load_image("res/ghost_red_left_1.png");
        ghost_red_mask[1][1]  = load_image("res/ghost_red_left_2.png");
        ghost_red_mask[2][0]  = load_image("res/ghost_red_down_1.png");
        ghost_red_mask[2][1]  = load_image("res/ghost_red_down_2.png");
        ghost_red_mask[3][0]  = load_image("res/ghost_red_right_1.png");
        ghost_red_mask[3][1]  = load_image("res/ghost_red_right_2.png");
    }
    void draw_avatar (Image GameWindow, uint32_t s) override {
        if (state != GHOST_FRIGHTENED && state != GHOST_EATEN)
            draw_image(GameWindow, ghost_red_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};

class ghost_pink : public ghost
{
private:
    Image ghost_pink_mask[4][2];
public:
    ghost_pink () : ghost(MACHIBUSE_PINK, {3, 0}, {290, 370}) {
        ghost_pink_mask[0][0]  = load_image("res/ghost_pink_up_1.png");
        ghost_pink_mask[0][1]  = load_image("res/ghost_pink_up_2.png");
        ghost_pink_mask[1][0]  = load_image("res/ghost_pink_left_1.png");
        ghost_pink_mask[1][1]  = load_image("res/ghost_pink_left_2.png");
        ghost_pink_mask[2][0]  = load_image("res/ghost_pink_down_1.png");
        ghost_pink_mask[2][1]  = load_image("res/ghost_pink_down_2.png");
        ghost_pink_mask[3][0]  = load_image("res/ghost_pink_right_1.png");
        ghost_pink_mask[3][1]  = load_image("res/ghost_pink_right_2.png");

    }
    void hunt (pacman PacMan) override {
        switch (PacMan.state) {
        case PAC_WALK_UP: walk_to_target({PacMan.matr_ceil.y - 4, PacMan.matr_ceil.x});
            break;
        case PAC_WALK_LEFT: walk_to_target({PacMan.matr_ceil.y, PacMan.matr_ceil.x-4});
            break;
        case PAC_WALK_DOWN: walk_to_target({PacMan.matr_ceil.y + 4, PacMan.matr_ceil.x});
            break;
        case PAC_WALK_RIGHT: walk_to_target({PacMan.matr_ceil.y, PacMan.matr_ceil.x+4});
            break;
        default: walk_to_target(PacMan.matr_ceil);
            break;
        }
    }
    void draw_avatar (Image GameWindow, uint32_t s) override {
        if (state != GHOST_FRIGHTENED && state != GHOST_EATEN)
            draw_image(GameWindow, ghost_pink_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};


class ghost_orange : public ghost
{
private:
    Image ghost_orange_mask[4][2];
public:
    ghost_orange () : ghost(OTOBOKE_ORANGE, {0, 25}, {330, 370}) {
        ghost_orange_mask[0][0]  = load_image("res/ghost_orange_up_1.png");
        ghost_orange_mask[0][1]  = load_image("res/ghost_orange_up_2.png");
        ghost_orange_mask[1][0]  = load_image("res/ghost_orange_left_1.png");
        ghost_orange_mask[1][1]  = load_image("res/ghost_orange_left_2.png");
        ghost_orange_mask[2][0]  = load_image("res/ghost_orange_down_1.png");
        ghost_orange_mask[2][1]  = load_image("res/ghost_orange_down_2.png");
        ghost_orange_mask[3][0]  = load_image("res/ghost_orange_right_1.png");
        ghost_orange_mask[3][1]  = load_image("res/ghost_orange_right_2.png");
    }
    void hunt (pacman PacMan) override {
        if (abs((int)PacMan.matr_ceil.x - (int)matr_ceil.x) > 8 && abs((int)PacMan.matr_ceil.y - (int)matr_ceil.y) > 8) walk_to_target(walk_target_ceil);
        else walk_to_target(PacMan.matr_ceil);
    }
    void draw_avatar (Image GameWindow, uint32_t s) override {
        if (state != GHOST_FRIGHTENED && state != GHOST_EATEN)
            draw_image(GameWindow, ghost_orange_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};


class ghost_cyan : public ghost
{
private:
    Image ghost_cyan_mask[4][2];
    V2 dependent;
public:
    ghost_cyan () : ghost(KIMAGURE_CYAN, {33, 25}, {250, 370}) {
        ghost_cyan_mask[0][0]  = load_image("res/ghost_cyan_up_1.png");
        ghost_cyan_mask[0][1]  = load_image("res/ghost_cyan_up_2.png");
        ghost_cyan_mask[1][0]  = load_image("res/ghost_cyan_left_1.png");
        ghost_cyan_mask[1][1]  = load_image("res/ghost_cyan_left_2.png");
        ghost_cyan_mask[2][0]  = load_image("res/ghost_cyan_down_1.png");
        ghost_cyan_mask[2][1]  = load_image("res/ghost_cyan_down_2.png");
        ghost_cyan_mask[3][0]  = load_image("res/ghost_cyan_right_1.png");
        ghost_cyan_mask[3][1]  = load_image("res/ghost_cyan_right_2.png");
    }
    void hunt (pacman PacMan) override {
        switch (PacMan.state) {
        case PAC_WALK_UP: walk_to_target({dependent.y + (PacMan.matr_ceil.y - 2 - dependent.y)*2, dependent.x + (PacMan.matr_ceil.x - dependent.x)*2});
            break;
        case PAC_WALK_LEFT: walk_to_target({dependent.y + (PacMan.matr_ceil.y - dependent.y)*2, dependent.x + (PacMan.matr_ceil.x - 2 - dependent.x)*2});
            break;
        case PAC_WALK_DOWN: walk_to_target({dependent.y + (PacMan.matr_ceil.y + 2 - dependent.y)*2, dependent.x + (dependent.x - dependent.x)*2});
            break;
        case PAC_WALK_RIGHT: walk_to_target({dependent.y + (PacMan.matr_ceil.y - dependent.y)*2, dependent.x + (PacMan.matr_ceil.x + 2 - dependent.x)*2});
            break;
        default: walk_to_target(PacMan.matr_ceil);
            break;
        }
    }
    void draw_avatar (Image GameWindow, uint32_t s) override {
        if (state != GHOST_FRIGHTENED && state != GHOST_EATEN)
            draw_image(GameWindow, ghost_cyan_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
    void update_dependent (V2 new_dep) {
        dependent.x = new_dep.x;
        dependent.y = new_dep.y;
    }
};



static void check_pacman_life (pacman &PacMan, ghost Oikake, ghost Machibuse, ghost Otoboke, ghost Kimagure)
{
  if ((Oikake.matr_ceil.x    == PacMan.matr_ceil.x && Oikake.matr_ceil.y    == PacMan.matr_ceil.y && Oikake.state    != GHOST_FRIGHTENED && Oikake.state    != GHOST_EATEN) ||
      (Machibuse.matr_ceil.x == PacMan.matr_ceil.x && Machibuse.matr_ceil.y == PacMan.matr_ceil.y && Machibuse.state != GHOST_FRIGHTENED && Machibuse.state != GHOST_EATEN) ||
      (Otoboke.matr_ceil.x   == PacMan.matr_ceil.x && Otoboke.matr_ceil.y   == PacMan.matr_ceil.y && Otoboke.state   != GHOST_FRIGHTENED && Otoboke.state   != GHOST_EATEN) ||
      (Kimagure.matr_ceil.x  == PacMan.matr_ceil.x && Kimagure.matr_ceil.y  == PacMan.matr_ceil.y && Kimagure.state  != GHOST_FRIGHTENED && Kimagure.state  != GHOST_EATEN)) {
      PacMan.state = PAC_DIES;
      PacMan.awaiting_state = PAC_NONE;
      PacMan.action();
  }
}
