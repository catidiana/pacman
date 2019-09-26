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
    Image pacman_go_left[2];
    Image pacman_go_right[2];
    Image pacman_go_down[2];
    Image pacman_go_up[2];
    Image pacman_lifes_count[2];
    Sound dead_pac;
public:
    Image pacman_stay;
    V2 pac_coord;
    V2 matr_ceil;
    Pac_States state;
    Pac_States awaiting_state;
    uint8_t pacman_lifes;


    pacman() {
        pacman_stay           = load_image("res/pacman.png");
        pacman_go_left[0]     = load_image("res/pacman_45_left.png");
        pacman_go_left[1]     = load_image("res/pacman_90_left.png");
        pacman_go_right[0]    = load_image("res/pacman_45_right.png");
        pacman_go_right[1]    = load_image("res/pacman_90_right.png");
        pacman_go_down[0]     = load_image("res/pacman_45_down.png");
        pacman_go_down[1]     = load_image("res/pacman_90_down.png");
        pacman_go_up[0]       = load_image("res/pacman_45_up.png");
        pacman_go_up[1]       = load_image("res/pacman_90_up.png");
        pacman_lifes_count[1] = load_image("res/lifes_2.png");
        pacman_lifes_count[0] = load_image("res/lifes_1.png");
        dead_pac              = load_sound("sound/death.wav");
        reset_pacman();
        pacman_lifes = 3;
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
            --pacman_lifes;
            play_sound(dead_pac);
            break;
        case PAC_STAY:
        case PAC_NONE: break;
        }

        define_matr_ceil();
    }

    void draw (Image GameWindow, uint32_t frame) {
        switch (state) {
        case PAC_STAY:       draw_image (GameWindow, pacman_stay                 , pac_coord.x, pac_coord.y); break;
        case PAC_WALK_LEFT:  draw_image (GameWindow, pacman_go_left [(frame%8)/4], pac_coord.x, pac_coord.y); break;
        case PAC_WALK_RIGHT: draw_image (GameWindow, pacman_go_right[(frame%8)/4], pac_coord.x, pac_coord.y); break;
        case PAC_WALK_DOWN:  draw_image (GameWindow, pacman_go_down [(frame%8)/4], pac_coord.x, pac_coord.y); break;
        case PAC_WALK_UP:    draw_image (GameWindow, pacman_go_up   [(frame%8)/4], pac_coord.x, pac_coord.y); break;
        case PAC_DIES:
        case PAC_NONE: break;
        }

        if (pacman_lifes > 1) draw_image(GameWindow, pacman_lifes_count[pacman_lifes - 2], 40, 20);
    }
};
