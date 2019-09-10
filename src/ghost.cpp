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

class ghost
{
protected:
    Ghost_Type type;
    Image ghost_fright[4];
    int shy_time;
    uint32_t start_time;
    V2 walk_target_ceil;

public:
    Ghost_States state;
    Ghost_States awaiting_state;
    Ghost_Direction direction;
    V2 gh_coord;
    V2 matr_ceil;
    uint8_t wave;
    uint32_t walking_time[4];
    uint32_t hunting_time[4];

    ghost (Ghost_Type type_, V2 target_) : type(type_), walk_target_ceil(target_) {
        shy_time = 1000*type;
        wave = 0;
        for (int i = 0; i < 4; i++) {
            walking_time[i] = 7000;
            if (i > 2) walking_time[i] -= 2000;
            hunting_time[i] = 20000;
        }
        if (type == OIKAKE_RED) {
            state = GHOST_WALK;
            awaiting_state = GHOST_HUNT;
            direction = DIR_RIGHT;
        } else {
            state = GHOST_SHY;
            awaiting_state = GHOST_TRANSFER;
            direction = DIR_LEFT;
        }
        ghost_fright[0]  = load_image("res/ghost_fr_1.png");
        ghost_fright[1]  = load_image("res/ghost_fr_2.png");
        ghost_fright[2]  = load_image("res/ghost_fr_3.png");
        ghost_fright[3]  = load_image("res/ghost_fr_4.png");
        start_time = SDL_GetTicks();
    }
    void define_matr_ceil() {
        matr_ceil.x = (gh_coord.x/10)/2;
        matr_ceil.y = ((MAIN_WINDOW_INIT_HEIGHT - gh_coord.y)/10)/2;
    }
    void shy_mode (uint32_t s) {
        if (s%40 < 20) {
            gh_coord.x--;
            direction = DIR_LEFT;
        } else { gh_coord.x++;
            direction = DIR_RIGHT;
        }
    }
    void transfer_mode () {
        if (gh_coord.x != 290) {
            if (gh_coord.x < 290) {
                gh_coord.x++;
                direction = DIR_RIGHT;
            } else {
                gh_coord.x--;
                direction = DIR_LEFT;
            }
        }
        else if (gh_coord.y != 430) {
            if (gh_coord.y < 430) {
                gh_coord.y++;
                direction = DIR_UP;
            } else {
                gh_coord.y--;
                direction = DIR_DOWN;
            }
        }
    }
    void walk_to_left() {
        --gh_coord.x;
        if (gh_coord.x == 0) gh_coord.x = MAIN_WINDOW_INIT_WIDTH - 1;
        direction = DIR_LEFT;
    }
    void walk_to_right() {
        ++gh_coord.x;
        if (gh_coord.x == MAIN_WINDOW_INIT_WIDTH) gh_coord.x = 0;
        direction = DIR_RIGHT;
    }
    void walk_to_down() {
        --gh_coord.y;
        direction = DIR_DOWN;
    }
    void walk_to_up() {
        ++gh_coord.y;
        direction = DIR_UP;
    }
    void continue_walk () {
        switch (direction) {
        case DIR_UP: walk_to_up();
            break;
        case DIR_LEFT: walk_to_left();
            break;
        case DIR_DOWN: walk_to_down();
            break;

        case DIR_RIGHT: walk_to_right();
            break;
        }
    }
    Ghost_Direction find_awailable_direction() {
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
        return find_awailable_direction();
    }
    void walk_to_target(V2 matr_target) {
        if (gh_coord.x%20 != 10 || gh_coord.y%20 != 10) {
            continue_walk();
            return;
        }
        if (WalkingMatr[matr_ceil.y][matr_ceil.x] == 1) {
            switch (direction) {
            case DIR_UP: {
                if (WalkingMatr[matr_ceil.y - 1][matr_ceil.x] > 0) {
                    continue_walk();
                    return;
                }
            } break;
            case DIR_LEFT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28] > 0) {
                    continue_walk();
                    return;
                }
            } break;
            case DIR_DOWN: {
                if (WalkingMatr[matr_ceil.y + 1][matr_ceil.x] > 0) {
                    continue_walk();
                    return;
                }
            } break;
            case DIR_RIGHT: {
                if (WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28] > 0) {
                    continue_walk();
                    return;
                }
            } break;
            }
            direction = find_awailable_direction();
            continue_walk();
        } else {
            bool awailable_ceils[4];
            int target_distance[4];
            //up - left - down - right;
            awailable_ceils[0] = WalkingMatr[matr_ceil.y - 1][matr_ceil.x];
            awailable_ceils[1] = WalkingMatr[matr_ceil.y][(matr_ceil.x-1)%28];
            awailable_ceils[2] = WalkingMatr[matr_ceil.y + 1][matr_ceil.x];
            awailable_ceils[3] = WalkingMatr[matr_ceil.y][(matr_ceil.x+1)%28];
            int x_dir = (int)matr_ceil.x - (int)matr_target.x;
            int y_dir = (int)matr_ceil.y - (int)matr_target.y;
            target_distance[0] = x_dir*x_dir + (y_dir - 1)*(y_dir - 1);
            target_distance[1] = (x_dir - 1)*(x_dir - 1) + y_dir*y_dir;
            target_distance[2] = x_dir*x_dir +(y_dir + 1)*(y_dir + 1);
            target_distance[3] = (x_dir + 1)*(x_dir + 1) + y_dir*y_dir;
            for (int i = 0; i < 4; i++) {
                if (awailable_ceils[i] == false) target_distance[i] = 3000;
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
            continue_walk();
        }
    }
    virtual void hunt (pacman PacMan) {
        walk_to_target(PacMan.matr_ceil);
    }
    void calculating_path(uint32_t s, pacman PacMan, food Food) {
        if (Food.energizer_mode > 0) {}
        switch (awaiting_state) {
        case GHOST_SHY:
            break;
        case GHOST_TRANSFER: {
            uint32_t current_time = SDL_GetTicks();
            if (current_time  >= start_time + shy_time) {
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
        } break;
        case GHOST_HUNT: {
            uint32_t current_time = SDL_GetTicks();
            if (current_time >= start_time + walking_time[wave] && gh_coord.x%20 == 10 && gh_coord.y%20 == 10) {
                state = GHOST_HUNT;
                if (wave < 3) awaiting_state = GHOST_WALK;
                else awaiting_state = GHOST_NONE;
                direction = find_direction_to_pacman(PacMan);
                start_time = SDL_GetTicks();
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
        default:
            break;
        }
        define_matr_ceil();
    }
    virtual void action(Image GameWindow, uint32_t s, pacman PacMan, food Food) = 0;
};

class ghost_red : public ghost
{
private:
    Image ghost_red_mask[4][2];
public:
    ghost_red () : ghost(OIKAKE_RED, {33, 0}) {
        ghost_red_mask[0][0]  = load_image("res/ghost_red_up_1.png");
        ghost_red_mask[0][1]  = load_image("res/ghost_red_up_2.png");
        ghost_red_mask[1][0]  = load_image("res/ghost_red_left_1.png");
        ghost_red_mask[1][1]  = load_image("res/ghost_red_left_2.png");
        ghost_red_mask[2][0]  = load_image("res/ghost_red_down_1.png");
        ghost_red_mask[2][1]  = load_image("res/ghost_red_down_2.png");
        ghost_red_mask[3][0]  = load_image("res/ghost_red_right_1.png");
        ghost_red_mask[3][1]  = load_image("res/ghost_red_right_2.png");
        gh_coord = {290, 430};
        define_matr_ceil();

    }
    void action (Image GameWindow, uint32_t s, pacman PacMan, food Food) override {
        calculating_path(s, PacMan, Food);
        draw_image(GameWindow, ghost_red_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};

class ghost_pink : public ghost
{
private:
    Image ghost_pink_mask[4][2];
public:
    ghost_pink () : ghost(MACHIBUSE_PINK, {3, 0}) {
        ghost_pink_mask[0][0]  = load_image("res/ghost_pink_up_1.png");
        ghost_pink_mask[0][1]  = load_image("res/ghost_pink_up_2.png");
        ghost_pink_mask[1][0]  = load_image("res/ghost_pink_left_1.png");
        ghost_pink_mask[1][1]  = load_image("res/ghost_pink_left_2.png");
        ghost_pink_mask[2][0]  = load_image("res/ghost_pink_down_1.png");
        ghost_pink_mask[2][1]  = load_image("res/ghost_pink_down_2.png");
        ghost_pink_mask[3][0]  = load_image("res/ghost_pink_right_1.png");
        ghost_pink_mask[3][1]  = load_image("res/ghost_pink_right_2.png");
        gh_coord = {290, 370};
        define_matr_ceil();

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
    void action (Image GameWindow, uint32_t s, pacman PacMan, food Food) override {
        calculating_path(s, PacMan, Food);
        draw_image(GameWindow, ghost_pink_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};


class ghost_orange : public ghost
{
private:
    Image ghost_orange_mask[4][2];
public:
    ghost_orange () : ghost(OTOBOKE_ORANGE, {0, 25}) {
        ghost_orange_mask[0][0]  = load_image("res/ghost_orange_up_1.png");
        ghost_orange_mask[0][1]  = load_image("res/ghost_orange_up_2.png");
        ghost_orange_mask[1][0]  = load_image("res/ghost_orange_left_1.png");
        ghost_orange_mask[1][1]  = load_image("res/ghost_orange_left_2.png");
        ghost_orange_mask[2][0]  = load_image("res/ghost_orange_down_1.png");
        ghost_orange_mask[2][1]  = load_image("res/ghost_orange_down_2.png");
        ghost_orange_mask[3][0]  = load_image("res/ghost_orange_right_1.png");
        ghost_orange_mask[3][1]  = load_image("res/ghost_orange_right_2.png");
        gh_coord = {330, 370};
        define_matr_ceil();
    }
    void hunt (pacman PacMan) override {
        if (abs((int)PacMan.matr_ceil.x - (int)matr_ceil.x) > 8 && abs((int)PacMan.matr_ceil.y - (int)matr_ceil.y) > 8) walk_to_target(walk_target_ceil);
        else walk_to_target(PacMan.matr_ceil);
    }
    void action (Image GameWindow, uint32_t s, pacman PacMan, food Food) override {
        calculating_path(s, PacMan, Food);
        draw_image(GameWindow, ghost_orange_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};


class ghost_cyan : public ghost
{
private:
    Image ghost_cyan_mask[4][2];
    V2 dependent;
public:
    ghost_cyan () : ghost(KIMAGURE_CYAN, {33, 25}) {
        ghost_cyan_mask[0][0]  = load_image("res/ghost_cyan_up_1.png");
        ghost_cyan_mask[0][1]  = load_image("res/ghost_cyan_up_2.png");
        ghost_cyan_mask[1][0]  = load_image("res/ghost_cyan_left_1.png");
        ghost_cyan_mask[1][1]  = load_image("res/ghost_cyan_left_2.png");
        ghost_cyan_mask[2][0]  = load_image("res/ghost_cyan_down_1.png");
        ghost_cyan_mask[2][1]  = load_image("res/ghost_cyan_down_2.png");
        ghost_cyan_mask[3][0]  = load_image("res/ghost_cyan_right_1.png");
        ghost_cyan_mask[3][1]  = load_image("res/ghost_cyan_right_2.png");
        gh_coord = {250, 370};
        define_matr_ceil();
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
    void update_dependent (V2 new_dep) {
        dependent.x = new_dep.x;
        dependent.y = new_dep.y;
    }
    void action (Image GameWindow, uint32_t s, pacman PacMan, food Food) override {
        calculating_path(s, PacMan, Food);
        draw_image(GameWindow, ghost_cyan_mask[direction][(s%8)/4], gh_coord.x, gh_coord.y);
    }
};
