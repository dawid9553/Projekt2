#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

// Sterowanie: true = WASD, false = strzałki
bool useWASD = true;

// ================================
// STRUKTURY
// ================================
struct Fish {
    float x, y;
    float size;
    float speed;
    float mass;
    bool isPlayer;
};

struct Booster {
    float x, y;
    float radius;
    bool active;
};

// ================================
// KOLIZJE
// ================================
bool checkCollision(const Fish& a, const Fish& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (a.size / 2 + b.size / 2);
}

bool checkBoosterCollision(const Fish& p, const Booster& b) {
    float dx = p.x - b.x;
    float dy = p.y - b.y;
    float dist = sqrt(dx * dx + dy * dy);
    return dist < (p.size / 2 + b.radius);
}

// ================================
// STANY GRY
// ================================
enum GameState {
    MENU,
    GAME
};

// ================================
// MAIN
// ================================
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Fish Eat Get Big",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(nullptr));

    GameState gameState = MENU;

    // ================================
    // GRACZ I RYBY
    // ================================
    Fish player{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, 5, 5, true };

    vector<Fish> fish;
    for (int i = 0; i < 20; i++) {
        fish.push_back({ float(rand() % SCREEN_WIDTH), float(rand() % SCREEN_HEIGHT), 20, 1.5f, 1, false });
    }
    for (int i = 0; i < 10; i++) {
        fish.push_back({ float(rand() % SCREEN_WIDTH), float(rand() % SCREEN_HEIGHT), 40, 1.5f, 8, false });
    }
    for (int i = 0; i < 15; i++) {

        float x, y;
        float safeDistance = 200.0f;
        bool ok = false;

        while (!ok) {
            x = SCREEN_WIDTH - 50 - (rand() % 600);
            y = SCREEN_HEIGHT - 50 - (rand() % 600);

            float dx = x - player.x;
            float dy = y - player.y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > safeDistance) ok = true;
        }

        fish.push_back({ x, y, 60, 1.5f, 12, false });
    }

    // BOOSTER
    Booster booster{
        float(50 + rand() % (SCREEN_WIDTH - 100)),
        float(50 + rand() % (SCREEN_HEIGHT - 100)),
        20.0f,
        true
    };

    bool boosterOwned = false;

    // ================================
    // PRZYCISKI MENU
    // ================================
    SDL_Rect startBtn = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 200, 80 };
    SDL_Rect controlBtn = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 60 };

    bool running = true;
    SDL_Event event;

    // ================================
    // PĘTLA GRY
    // ================================
    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                running = false;

            // Klawiatura
            if (event.type == SDL_KEYDOWN) {

                if (gameState == MENU && event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                    gameState = GAME;

                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    running = false;
            }

            // Kliknięcia w MENU
            if (event.type == SDL_MOUSEBUTTONDOWN && gameState == MENU) {

                int mx = event.button.x;
                int my = event.button.y;

                // START
                if (mx >= startBtn.x && mx <= startBtn.x + startBtn.w &&
                    my >= startBtn.y && my <= startBtn.y + startBtn.h) {
                    gameState = GAME;
                }

                // ZMIANA STEROWANIA
                if (mx >= controlBtn.x && mx <= controlBtn.x + controlBtn.w &&
                    my >= controlBtn.y && my <= controlBtn.y + controlBtn.h) {
                    useWASD = !useWASD;
                }
            }
        }

        // ================================
        // LOGIKA GRY
        // ================================
        if (gameState == GAME) {

            const Uint8* state = SDL_GetKeyboardState(NULL);

            if (useWASD) {
                if (state[SDL_SCANCODE_W]) player.y -= player.speed;
                if (state[SDL_SCANCODE_S]) player.y += player.speed;
                if (state[SDL_SCANCODE_A]) player.x -= player.speed;
                if (state[SDL_SCANCODE_D]) player.x += player.speed;
            }
            else {
                if (state[SDL_SCANCODE_UP]) player.y -= player.speed;
                if (state[SDL_SCANCODE_DOWN]) player.y += player.speed;
                if (state[SDL_SCANCODE_LEFT]) player.x -= player.speed;
                if (state[SDL_SCANCODE_RIGHT]) player.x += player.speed;
            }

            // Granice gracza
            player.x = max(player.size / 2, min(player.x, SCREEN_WIDTH - player.size / 2));
            player.y = max(player.size / 2, min(player.y, SCREEN_HEIGHT - player.size / 2));

            // Booster
            if (booster.active && checkBoosterCollision(player, booster)) {
                booster.active = false;
                boosterOwned = true;
            }

            // Ruch ryb
            for (auto& f : fish) {

                if (f.size == 20) {
                    if (f.x == SCREEN_WIDTH - f.size / 2)
                        f.x = f.size / 2;
                    f.x += 2 * f.speed;
                }
                else if (f.size == 40) {
                    if (f.y == SCREEN_HEIGHT - f.size / 2)
                        f.y = f.size / 2;
                    f.y += 1 * f.speed;
                }
                else if (f.size == 60) {
                    f.x -= f.speed;
                    f.y += (rand() % 3 - 1) * f.speed;

                    if (f.x < f.size / 2 || f.y < f.size / 2 || f.y > SCREEN_HEIGHT - f.size / 2) {
                        f.x = SCREEN_WIDTH - f.size / 2;
                        f.y = float(rand() % SCREEN_HEIGHT);
                    }
                }

                f.x = max(f.size / 2, min(f.x, SCREEN_WIDTH - f.size / 2));
                f.y = max(f.size / 2, min(f.y, SCREEN_HEIGHT - f.size / 2));
            }

            // Respawn boostera
            if (!booster.active && !boosterOwned && rand() % 600 == 0) {
                booster.x = 50 + rand() % (SCREEN_WIDTH - 100);
                booster.y = 50 + rand() % (SCREEN_HEIGHT - 100);
                booster.active = true;
            }

            // Kolizje
            for (int i = 0; i < (int)fish.size(); i++) {
                if (checkCollision(player, fish[i])) {

                    if (player.size >= fish[i].size) {
                        player.size += fish[i].mass / 2;
                        fish[i] = fish.back();
                        fish.pop_back();
                    }
                    else if (boosterOwned) {
                        fish[i] = fish.back();
                        fish.pop_back();
                        boosterOwned = false;
                    }
                    else {
                        cout << "GAME OVER!" << endl;
                        running = false;
                    }
                    break;
                }
            }
        }

        // ================================
        // RENDER
        // ================================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameState == MENU) {

            SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
            SDL_RenderClear(renderer);

            // START
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &startBtn);

            // PRZYCISK ZMIANY STEROWANIA
            if (useWASD)
                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255); // żółty = WASD
            else
                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255); // niebieski = strzałki

            SDL_RenderFillRect(renderer, &controlBtn);
        }
        else {

            // Gracz
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect pRect = {
                int(player.x - player.size / 2),
                int(player.y - player.size / 2),
                int(player.size),
                int(player.size)
            };
            SDL_RenderFillRect(renderer, &pRect);

            // Booster
            if (booster.active) {
                SDL_SetRenderDrawColor(renderer, 180, 0, 255, 255);
                for (int w = -int(booster.radius); w <= int(booster.radius); w++) {
                    for (int h = -int(booster.radius); h <= int(booster.radius); h++) {
                        if (w * w + h * h <= int(booster.radius * booster.radius)) {
                            SDL_RenderDrawPoint(renderer, int(booster.x + w), int(booster.y + h));
                        }
                    }
                }
            }

            // Ryby
            for (auto& f : fish) {

                if (f.size <= player.size)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                else if (f.size == 20)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                else if (f.size == 40)
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

                SDL_Rect rect = {
                    int(f.x - f.size / 2),
                    int(f.y - f.size / 2),
                    int(f.size),
                    int(f.size)
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
