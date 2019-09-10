enum Food_States {
    FOOD_NONE,
    FOOD_ORDINARY,
    FOOD_ENERGIZER,
    FOOD_FRUIT
};

class food {
private:
    bool FoodMatrix[36][28];
    bool energizers[4];
    uint32_t energy_time;
    Image ordinary;
    Image energizer[2];
    Image fruit;
public:
    uint8_t energizer_mode;
    int32_t food_counter;

    food () {
        ordinary = load_image("res/ordinary.png");
        energizer[0] = load_image("res/energizer.png");
        energizer[1] = load_image("res/energizer_s.png");
        energizer_mode = 0;
        food_counter = 246;
        for (uint32_t y = 0; y < 36; y++) {
            for (uint32_t x = 0; x < 28; x++) {
                FoodMatrix[y][x] = FoodMatr[y][x];
            }
        }
        for (int i = 0; i < 4; i++)
            energizers[i] = true;
    }
    void refill_food() {
        for (uint32_t y = 0; y < 36; y++) {
            for (uint32_t x = 0; x < 28; x++) {
                FoodMatrix[y][x] = FoodMatr[y][x];
            }
        }
    }
    void draw_food (Image GameWindow, uint32_t s) {
        for (uint32_t y = 0; y < 36; y++) {
            for (uint32_t x = 0; x < 28; x++) {
                if (FoodMatrix[y][x] == true) {
                    draw_image(GameWindow, ordinary, x*20+10, (36 - y)*20 - 10);
                }
            }
        }
        if (energizers[0]) draw_image(GameWindow, energizer[(s%16)/8], 30, 190);
        if (energizers[1]) draw_image(GameWindow, energizer[(s%16)/8], 30, 590);
        if (energizers[2]) draw_image(GameWindow, energizer[(s%16)/8], 530, 190);
        if (energizers[3]) draw_image(GameWindow, energizer[(s%16)/8], 530, 590);
    }
    void eaten_energizer () {
        GAME_SCORE+=40;
        energizer_mode = 1;
        energy_time = SDL_GetTicks();
    }
    void eaten_food(pacman PacMan) {
        if(FoodMatrix[PacMan.matr_ceil.y][PacMan.matr_ceil.x]) {
            FoodMatrix[PacMan.matr_ceil.y][PacMan.matr_ceil.x] = false;
            GAME_SCORE += 10;
            food_counter--;
            if (PacMan.matr_ceil.y == 26 &&  PacMan.matr_ceil.x == 1) {
                energizers[0] = false;
                eaten_energizer();
            } else if (PacMan.matr_ceil.y == 6 &&  PacMan.matr_ceil.x == 1) {
                energizers[1] = false;
                eaten_energizer();
            } else if (PacMan.matr_ceil.y == 26 &&  PacMan.matr_ceil.x == 26) {
                energizers[2] = false;
                eaten_energizer();
            } else if (PacMan.matr_ceil.y == 6 &&  PacMan.matr_ceil.x == 26) {
                energizers[3] = false;
                eaten_energizer();
            }
        }
        if (energizer_mode > 0) {
            uint32_t current_time = SDL_GetTicks();
            if (current_time >= energy_time + 7000) energizer_mode = 2;
            if (current_time >= energy_time + 9000) energizer_mode = 0;
        }
    }
    };
