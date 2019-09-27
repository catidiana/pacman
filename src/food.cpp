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
    bool fruits;
    bool bonus;
    Image ordinary;
    Image energizer[2];
    Image fruit;
    Sound chomp;
    Sound fruit_b;
    uint32_t fruit_time;
    uint32_t fruit_bonus;
public:
    bool energizer_mode;
    int32_t food_counter;

    food () {
        ordinary = load_image("res/ordinary.png");
        energizer[0] = load_image("res/energizer.png");
        energizer[1] = load_image("res/energizer_s.png");
        fruit = load_image("res/cherry.png");
        chomp = load_sound("sound/food.wav");
        fruit_b = load_sound("sound/bonus.wav");
        refill_food();

    }
    void refill_food() {
        food_counter = 244;
        for (uint32_t y = 0; y < 36; y++) {
            for (uint32_t x = 0; x < 28; x++) {
                FoodMatrix[y][x] = FoodMatr[y][x];
            }
        }
        for (int i = 0; i < 4; i++)
            energizers[i] = true;
        energizer_mode = false;
        fruits = false;
        bonus = false;
    }
    void draw_food (Image GameWindow, uint32_t s) {
        for (uint32_t y = 0; y < 36; y++) {
            for (uint32_t x = 0; x < 28; x++) {
                if (FoodMatrix[y][x]) {
                    draw_image(GameWindow, ordinary, x*20+10, (36 - y)*20 - 10);
                }
            }
        }
        if (energizers[0]) draw_image(GameWindow, energizer[(s%16)/8], 30, 190);
        if (energizers[1]) draw_image(GameWindow, energizer[(s%16)/8], 30, 590);
        if (energizers[2]) draw_image(GameWindow, energizer[(s%16)/8], 530, 190);
        if (energizers[3]) draw_image(GameWindow, energizer[(s%16)/8], 530, 590);
        if ((food_counter == 160 || food_counter == 60) && fruits == false && bonus == false) {
            fruits = true;
            fruit_time = SDL_GetTicks();
        }
        if (fruits) {
            draw_image(GameWindow, fruit, 280, 310);
            uint32_t current_time = SDL_GetTicks();
            if (current_time >= fruit_time + 9000) fruits = false;
        }
    }
    void eaten_energizer () {
        GAME_SCORE+=40;
        energizer_mode = true;
    }
    void eaten_fruit (pacman PacMan) {
        if (PacMan.matr_ceil.y == 20 && PacMan.matr_ceil.x == 13) {
            play_sound(fruit_b);
            fruits = false;
            srand (time(NULL));
            fruit_bonus = (rand()%(5 + LEVEL))*100 + LEVEL*100;
            GAME_SCORE += fruit_bonus;
            bonus = true;
        }
    }
    void show_bonus (Image GameWindow) {
        uint32_t current_time = SDL_GetTicks();
        if (current_time >= fruit_time + 11000) bonus = false;
        else draw_bonus (GameWindow, fruit_bonus);
    }
    void eaten_food(pacman PacMan, Image GameWindow) {
        energizer_mode = false;
        if (fruits) eaten_fruit(PacMan);
        if (bonus) show_bonus (GameWindow);
        if(FoodMatrix[PacMan.matr_ceil.y][PacMan.matr_ceil.x]) {
            FoodMatrix[PacMan.matr_ceil.y][PacMan.matr_ceil.x] = false;
            GAME_SCORE += 10;
            food_counter--;
            play_sound(chomp);
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
    }
};
