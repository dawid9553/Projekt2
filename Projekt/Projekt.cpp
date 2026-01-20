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
    float massgain;
    bool isPlayer;
    string move; //zmienna uniwersalna do okreslania ruchow
};


//tablica rodzaji ryb
struct Fish Fishtab[14] = {
    {0,0,20,1.5,2,1,false,"horizontal"},
    {0,0,40,1.5,10,5,false,"vertical"},
    {0,0,50,1,50,10,false,"wave"},
    {0,0,80,150,15,false,"horizontal"},
    {0,0,100,250,20,false,"wave"},
    {0,0,100,500,25,false,"wave"},
    {0,0,100,750,30,false,"horizontal"},
    {0,0,100,1000,50,false,"vertical"},
    {0,0,100,1500,75,false,"horizontal"},
    {0,0,100,2500,100,false,"wave"},
    {0,0,100,5000,150,false,"horizontal"},
    {0,0,100,10000,200,false,"horizontal"},
    {0,0,100,20000,250,false,"wave"},
    {0,0,200,999999999999,0,false,"wave"},



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
    case 'I': {
        int tmp[7][5] = {
            {1,1,1,1,1},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {1,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'G': {
        int tmp[7][5] = {
            {0,1,1,1,1},
            {1,0,0,0,0},
            {1,0,0,0,0},
            {1,0,1,1,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {0,1,1,1,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'M': {
        int tmp[7][5] = {
            {1,0,0,0,1},
            {1,1,0,1,1},
            {1,0,1,0,1},
            {1,0,1,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1}
        };
        memcpy(bitmap, tmp, sizeof(bitmap));
        break;
    }
    case 'V': {
        int tmp[7][5] = {
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {1,0,0,0,1},
            {0,1,0,1,0},
            {0,1,0,1,0},
            {0,0,1,0,0}
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
    SETTINGS,
    GAME,
    GAME_OVER
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
    Fish player{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, 5, 5, 5, true };

    vector<Fish> fish;
    for (int i = 0; i < 30 + (rand() % 20); i++) {
        fish.push_back({Fishtab[0]});
        fish[i].x = float(rand() % SCREEN_WIDTH);
        fish[i].y = float(rand() % SCREEN_HEIGHT);
    }
//przerobiony kod, pod nowa tablice z rybami
    for (int i = 0; i < 10+(rand() % 5); i++) {
           fish.push_back({Fishtab[1]});
               float x = fish.size() - 1;
               float safeDistance = 150.0f ;   // czerwone nie mogą być zbyt blisko gracza
               bool ok = false;
        while (!ok) {
           fish[x].x = float(rand() % SCREEN_WIDTH);
           fish[x].y = float(rand() % SCREEN_HEIGHT);

            float dx = fish[x].x - player.x;
            float dy = fish[x].y - player.y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > safeDistance)
                ok = true;
        }
    }

    for (int i = 0; i < 5+(rand()%10); i++) {
        fish.push_back({ Fishtab[2] });
        float x = fish.size() - 1;
        float safeDistance = 200.0f; //niebieskie nie moga byc blisko gracza
        bool ok = false;

        while (!ok) {
            fish[x].x = SCREEN_WIDTH - 50 - (rand() % 600);
            fish[x].y = SCREEN_HEIGHT - 50 - (rand() % 600);

            float dx = fish[x].x - player.x;
            float dy = fish[x].y - player.y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > safeDistance) ok = true;
        }

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
    SDL_Rect startBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60, 240, 70 };
    SDL_Rect controlBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 100, 240, 70 };
    SDL_Rect wasdBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60, 240, 70 };
    SDL_Rect arrowsBtn = { SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 20, 240, 70 };

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
                    gameState = SETTINGS;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && gameState == SETTINGS) {

                int mx = event.button.x;
                int my = event.button.y;

                // WASD button
                if (mx >= wasdBtn.x && mx <= wasdBtn.x + wasdBtn.w &&
                    my >= wasdBtn.y && my <= wasdBtn.y + wasdBtn.h) {
                    useWASD = true;
                    gameState = MENU;
                }

                // ARROWS button
                if (mx >= arrowsBtn.x && mx <= arrowsBtn.x + arrowsBtn.w &&
                    my >= arrowsBtn.y && my <= arrowsBtn.y + arrowsBtn.h) {
                    useWASD = false;
                    gameState = MENU;
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

                if (f.move == "horizontal") {
                    if (f.x == SCREEN_WIDTH - f.size / 2) {
                        f.x = f.size / 2;
                        f.y = rand() % SCREEN_HEIGHT;
                    }
                    f.x += 2 * f.speed;
                }
                else if (f.move == "vertical") {
                    if (f.y == SCREEN_HEIGHT - f.size / 2) {
                        f.y = f.size / 2;
                        f.x = rand() % SCREEN_WIDTH;
                    }
                    f.y += 1 * f.speed;
                }
                else if (f.move == "wave") {
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
            player.mass += fish[i].massgain;
            player.size += fish[i].massgain;
            fish[i] = fish.back();
            fish.pop_back();
        }

                    else if (boosterOwned) {
                        fish[i] = fish.back();
                        fish.pop_back();
                        boosterOwned = false;
                    }
                    else {
                        gameState = GAME_OVER;
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
            drawRoundedButton(renderer, controlBtn,
                { 0, 200, 255, 255 },
                { 0, 150, 200, 255 },
                20);

            // START text
            int startTextWidth = getTextWidth("START", 3);
            int startTextX = startBtn.x + (startBtn.w - startTextWidth) / 2;
            drawText(renderer, "START", startTextX, startBtn.y + 25, 3);

            // SETTINGS text
            int settingsTextWidth = getTextWidth("SETTINGS", 3);
            int settingsTextX = controlBtn.x + (controlBtn.w - settingsTextWidth) / 2;
            drawText(renderer, "SETTINGS", settingsTextX, controlBtn.y + 22, 3);

        }

        else if (gameState == SETTINGS) {

            SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
            SDL_RenderClear(renderer);

            drawRoundedButton(renderer, wasdBtn,
                { 255, 200, 0, 255 },
                { 200, 150, 0, 255 },
                20);

            drawRoundedButton(renderer, arrowsBtn,
                { 0, 200, 255, 255 },
                { 0, 150, 200, 255 },
                20);

            int wasdTextWidth = getTextWidth("WASD", 3);
            int wasdTextX = wasdBtn.x + (wasdBtn.w - wasdTextWidth) / 2;
            drawText(renderer, "WASD", wasdTextX, wasdBtn.y + 22, 3);

            int arrowsTextWidth = getTextWidth("ARROWS", 3);
            int arrowsTextX = arrowsBtn.x + (arrowsBtn.w - arrowsTextWidth) / 2;
            drawText(renderer, "ARROWS", arrowsTextX, arrowsBtn.y + 22, 3);
        }
        else if (gameState == GAME)
        {

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

    // jadalne, jeśli jesteś wizualnie większy lub równy
    bool edible = (player.size >= f.size);

    if (f.move == "wave") {
        // niebieskie zawsze niebieskie, ale dalej podlegają logice zjadania
        if (edible)
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // żółte jadalne 
        else
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);   // ciemny niebieski = groźny
    }
    else {
        if (edible)
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // żółte = jadalne
        else
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   // czerwone = groźne
    }

    SDL_Rect rect = {
        int(f.x - f.size / 2),
        int(f.y - f.size / 2),
        int(f.size),
        int(f.size)
    };
    SDL_RenderFillRect(renderer, &rect);
}

        }
        else if (gameState == GAME_OVER)
        {
            SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
            SDL_RenderClear(renderer);

            string msg = "GAME OVER";
            int scale = 4;
            int textWidth = getTextWidth(msg, scale);

            int x = (SCREEN_WIDTH - textWidth) / 2;
            int y = SCREEN_HEIGHT / 2 - 50;

            drawText(renderer, msg, x, y, scale);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
