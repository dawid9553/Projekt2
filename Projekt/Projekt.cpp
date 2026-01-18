// ================================
// 1. INCLUDE + STAŁE
// ================================
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

// ================================
// 2. STRUKTURA RYBY
// ================================
struct Fish {
    float x, y;
    float size;
    float speed;
    bool isPlayer;
};

// ================================
// 3. KOLIZJE
// ================================
bool checkCollision(const Fish& a, const Fish& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (a.size / 2 + b.size / 2);
}

// ================================
// 4. STANY GRY
// ================================
enum GameState {
    MENU,
    GAME
};

// ================================
// 5. MAIN
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
    Fish player{ SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 30, 5, true };

    vector<Fish> fish;
    for (int i = 0; i < 10; i++) {
        fish.push_back({
            float(rand() % SCREEN_WIDTH),
            float(rand() % SCREEN_HEIGHT),
            float(20),
            float(2),
            false
            });
    }
    for (int i = 0; i < 10; i++) {
        fish.push_back({
            float(rand() % SCREEN_WIDTH),
            float(rand() % SCREEN_HEIGHT),
            float(40),
            float(3),
            false
            });
    }

    // Przycisk START
    SDL_Rect startBtn = { SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-40, 200, 80 };

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

            // Myszka - START
            if (event.type == SDL_MOUSEBUTTONDOWN && gameState == MENU) {
                int mx = event.button.x;
                int my = event.button.y;

                if (mx >= startBtn.x && mx <= startBtn.x + startBtn.w &&
                    my >= startBtn.y && my <= startBtn.y + startBtn.h) {
                    gameState = GAME;
                }
            }
        }

        // ================================
        // LOGIKA GRY
        // ================================
        if (gameState == GAME) {
            const Uint8* state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_W]) player.y -= player.speed;
            if (state[SDL_SCANCODE_S]) player.y += player.speed;
            if (state[SDL_SCANCODE_A]) player.x -= player.speed;
            if (state[SDL_SCANCODE_D]) player.x += player.speed;

            // Granice gracza
            player.x = max(player.size / 2,
                min(player.x, SCREEN_WIDTH - player.size / 2));
            player.y = max(player.size / 2,
                min(player.y, SCREEN_HEIGHT - player.size / 2));
            // Ruch ryb + granice
            for (auto& f : fish) {
                if (f.size == 20) {
                    if (f.x == SCREEN_WIDTH - f.size/2)
                        f.x = f.size/2;

					f.x += 2 * f.speed;
                        
                }
                else if(f.size== 40){
                        if (f.y == SCREEN_HEIGHT - f.size / 2)
                            f.y = f.size / 2;

                        f.y += 1 * f.speed;

                    }


                f.x = max(f.size / 2,
                    min(f.x, SCREEN_WIDTH - f.size / 2));
                f.y = max(f.size / 2,
                    min(f.y, SCREEN_HEIGHT - f.size / 2));
            }

            // Kolizje
            for (int i = 0; i < fish.size(); i++) {
                if (checkCollision(player, fish[i])) {
                    if (player.size >= fish[i].size) {
                        player.size += fish[i].size *0.2f;
                        fish[i] = fish.back();
                        fish.pop_back();
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

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &startBtn);
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

            // Ryby
            for (auto& f : fish) {
                if (f.size <= player.size)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

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

    // ================================
    // ZAMYKANIE
    // ================================
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
