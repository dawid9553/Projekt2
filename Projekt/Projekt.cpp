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
// RYSOWANIE ZAOKRĄGLONYCH PRZYCISKÓW
// ================================
void drawRoundedButton(SDL_Renderer* r, SDL_Rect rect, SDL_Color top, SDL_Color bottom, int radius)
{
    for (int y = 0; y < rect.h; y++) {
        float t = float(y) / rect.h;
        Uint8 R = top.r + t * (bottom.r - top.r);
        Uint8 G = top.g + t * (bottom.g - top.g);
        Uint8 B = top.b + t * (bottom.b - top.b);

        SDL_SetRenderDrawColor(r, R, G, B, 255);

        int x1 = rect.x;
        int x2 = rect.x + rect.w;

        if (y < radius) {
            int dx = radius - y;
            int cut = int(sqrt(radius * radius - dx * dx));
            x1 += cut;
            x2 -= cut;
        }
        if (y > rect.h - radius) {
            int dy = y - (rect.h - radius);
            int cut = int(sqrt(radius * radius - dy * dy));
            x1 += cut;
            x2 -= cut;
        }

        SDL_RenderDrawLine(r, x1, rect.y + y, x2, rect.y + y);
    }
}

// ================================
// PROSTY FONT BITMAPOWY (5x7)
// ================================
void drawChar(SDL_Renderer* r, char c, int x, int y, int scale)
{
    // 5x7 bitmapa, 1 = piksel, 0 = brak
    // tylko potrzebne litery: S,T,A,R,W,D,O,L,E,C,N
    static const int W = 5;
    static const int H = 7;
    int bitmap[H][W] = { 0 };

    switch (c) {
    case 'S': {
        int tmp[7][5] = {
            {1,1,1,1,1},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,1,1,1,1},
            {0,0,0,0,1},
            {0,0,0,0,1},
            {1,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'T': {
        int tmp[7][5] = {
            {1,1,1,1,1},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'A': {
        int tmp[7][5] = {
            {0,1,1,1,0},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,1,1,1,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'R': {
        int tmp[7][5] = {
            {1,1,1,1,0},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,1,1,1,0},
            {1,0,1,0,0},
            {1,0,0,1,0},
            {1,0,0,0,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'W': {
        int tmp[7][5] = {
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,1,0,1},
            {1,0,1,0,1},
            {1,1,0,1,1},
            {1,0,0,0,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'D': {
        int tmp[7][5] = {
            {1,1,1,0,0},
            {1,0,0,1,0},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,1,0},
            {1,1,1,0,0}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'O': {
        int tmp[7][5] = {
            {0,1,1,1,0},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {0,1,1,1,0}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'L': {
        int tmp[7][5] = {
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'E': {
        int tmp[7][5] = {
            {1,1,1,1,1},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,1,1,1,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'C': {
        int tmp[7][5] = {
            {0,1,1,1,1},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {0,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'N': {
        int tmp[7][5] = {
            {1,0,0,0,1},
            {1,1,0,0,1},
            {1,0,1,0,1},
            {1,0,0,1,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    default:
        return;
    }

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; i++) {
            if (bitmap[j][i]) {
                SDL_Rect px = {
                    x + i * scale,
                    y + j * scale,
                    scale,
                    scale
                };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

void drawText(SDL_Renderer* r, const string& text, int x, int y, int scale)
{
    int cursorX = x;
    for (char c : text) {
        if (c == ' ') {
            cursorX += 3 * scale;
            continue;
        }
        drawChar(r, c, cursorX, y, scale);
        cursorX += 6 * scale;
    }
}

//dodatkowa funkcja do obliczania szerokosci tekstu
int getTextWidth(const string& text, int scale) {
    int width = 0;
    for (char c : text) {
        if (c == ' ') {
            width += 3 * scale;
        }
        else {
            width += 6 * scale;
        }
    }
    return width;
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
    SDL_Rect startBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60, 240, 80 };
    SDL_Rect controlBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 50, 240, 70 };

    bool running = true;
    SDL_Event event;

    float boosterPulse = 0.0f;
    float boosterAngle = 0.0f;

    // ================================
    // PĘTLA GRY
    // ================================
    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN) {

                if (gameState == MENU && event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                    gameState = GAME;

                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    running = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && gameState == MENU) {

                int mx = event.button.x;
                int my = event.button.y;

                if (mx >= startBtn.x && mx <= startBtn.x + startBtn.w &&
                    my >= startBtn.y && my <= startBtn.y + startBtn.h) {
                    gameState = GAME;
                }

                if (mx >= controlBtn.x && mx <= controlBtn.x + controlBtn.w &&
                    my >= controlBtn.y && my <= controlBtn.y + controlBtn.h) {
                    useWASD = !useWASD;
                }
            }
        }

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

            player.x = max(player.size / 2, min(player.x, SCREEN_WIDTH - player.size / 2));
            player.y = max(player.size / 2, min(player.y, SCREEN_HEIGHT - player.size / 2));

            if (booster.active && checkBoosterCollision(player, booster)) {
                booster.active = false;
                boosterOwned = true;
            }

            boosterPulse += 0.1f;
            boosterAngle += 0.05f;

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

            if (!booster.active && !boosterOwned && rand() % 600 == 0) {
                booster.x = 50 + rand() % (SCREEN_WIDTH - 100);
                booster.y = 50 + rand() % (SCREEN_HEIGHT - 100);
                booster.active = true;
            }

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameState == MENU) {

            SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
            SDL_RenderClear(renderer);

            drawRoundedButton(renderer, startBtn,
                { 0, 255, 0, 255 },
                { 0, 180, 0, 255 },
                20);

            if (useWASD)
                drawRoundedButton(renderer, controlBtn,
                    { 255, 200, 0, 255 },
                    { 200, 150, 0, 255 },
                    20);
            else
                drawRoundedButton(renderer, controlBtn,
                    { 0, 200, 255, 255 },
                    { 0, 150, 200, 255 },
                    20);

            // Napisy na przyciskach
            // START button
int startTextWidth = getTextWidth("START", 3);
int startTextX = startBtn.x + (startBtn.w - startTextWidth) / 2;
drawText(renderer, "START", startTextX, startBtn.y + 25, 3);

// CONTROL button
if (useWASD) {
    int wasdTextWidth = getTextWidth("WASD", 3);
    int wasdTextX = controlBtn.x + (controlBtn.w - wasdTextWidth) / 2;
    drawText(renderer, "WASD", wasdTextX, controlBtn.y + 22, 3);
} else {
    int arrowsTextWidth = getTextWidth("ARROWS", 3);
    int arrowsTextX = controlBtn.x + (controlBtn.w - arrowsTextWidth) / 2;
    drawText(renderer, "ARROWS", arrowsTextX, controlBtn.y + 22, 3);
}

        }
        else {

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect pRect = {
                int(player.x - player.size / 2),
                int(player.y - player.size / 2),
                int(player.size),
                int(player.size)
            };
            SDL_RenderFillRect(renderer, &pRect);

            if (boosterOwned) {
                SDL_SetRenderDrawColor(renderer, 200, 0, 255, 255);
                for (int i = 0; i < 6; i++) {
                    SDL_Rect shieldRect = {
                        int(player.x - player.size - i),
                        int(player.y - player.size - i),
                        int(player.size * 2 + i * 2),
                        int(player.size * 2 + i * 2)
                    };
                    SDL_RenderDrawRect(renderer, &shieldRect);
                }
            }

            if (booster.active) {

                float pulse = 2.0f + sin(boosterPulse) * 3.0f;
                float radius = booster.radius + pulse;

                SDL_SetRenderDrawColor(renderer, 200, 0, 255, 120);
                for (int i = 0; i < 360; i += 20) {
                    float angle = i * 3.14159f / 180.0f + boosterAngle;
                    float x = booster.x + cos(angle) * (radius + 10);
                    float y = booster.y + sin(angle) * (radius + 10);
                    SDL_RenderDrawPoint(renderer, int(x), int(y));
                }

                SDL_SetRenderDrawColor(renderer, 180, 0, 255, 255);
                for (int w = -int(radius); w <= int(radius); w++) {
                    for (int h = -int(radius); h <= int(radius); h++) {
                        if (w * w + h * h <= int(radius * radius)) {
                            SDL_RenderDrawPoint(renderer,
                                int(booster.x + w),
                                int(booster.y + h));
                        }
                    }
                }
            }

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
