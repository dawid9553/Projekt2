#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

// Sterowanie: true = WASD, false = strzałki
bool useWASD = true;
int bestScore = 0;

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
    string move;
};

struct Fish fishtab[11] = {
    {0,0,15,1.3,5,1,false,"horizontal"},
    {0,0,25,1.25,15,5,false,"vertical"},
    {0,0,40,1.25,50,10,false,"wave"},
    {0,0,50,1,150,15,false,"horizontal"},
    {0,0,70,1.1,400,20,false,"vertical"},
    {0,0,90,1.2,800,25,false,"wave"},
    {0,0,100,1.5,1500,30,false,"horizontal"},
    {0,0,125,1,2250,40,false,"vertical"},
    {0,0,150,.75,3500,50,false,"horizontal"},
    {0,0,175,.5,5000,100,false,"wave"},
    {0,0,200,1.2,2147483646,0,false,"wave"},



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
    case 'B': {
    int tmp[7][5] = {
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}

case '0': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,1,1},
        {1,0,1,0,1},
        {1,1,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '1': {
    int tmp[7][5] = {
        {0,0,1,0,0},
        {0,1,1,0,0},
        {1,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '2': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,1,1},
        {0,0,1,1,0},
        {0,1,1,0,0},
        {1,1,0,0,0},
        {1,1,1,1,1}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '3': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,1,1},
        {0,0,1,1,0},
        {0,0,0,1,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '4': {
    int tmp[7][5] = {
        {1,0,0,1,0},
        {1,0,0,1,0},
        {1,0,0,1,0},
        {1,1,1,1,1},
        {0,0,0,1,0},
        {0,0,0,1,0},
        {0,0,0,1,0}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '5': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '6': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,0}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '7': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,1,0},
        {0,0,1,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '8': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    };
    memcpy(bitmap, tmp, sizeof(bitmap));
    break;
}
case '9': {
    int tmp[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
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
    {
    ifstream in("highscore.txt");
    if (in) {
        in >> bestScore;
    }
}
    GameState gameState = MENU;
    int score = 0;
    
    // ================================
    // GRACZ I RYBY
    // ================================
    Fish player{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 15, 3, 5, 5, true };

    //TWORZENIE RYB POCZATKOWYCH I ICH POCZATKOWE POZYCJE
    //TYP RYBY 1
    vector<Fish> fish;
    for (int i = 0; i < 25+(rand()%15); i++) {
        fish.push_back({fishtab[0]});
        fish[i].x = float(rand() % SCREEN_WIDTH);
        fish[i].y = float(rand() % SCREEN_HEIGHT);
    }
    
    //TYP RYBY 2
    for (int i = 0; i < 10 + (rand() % 5); i++) {
            fish.push_back({ fishtab[1] });

            //Sprawdzanie czy ryby jest odpowiednio daleko od gracza w chwili rozpoczecia
            float x = fish.size() - 1;
            float safeDistance = 150.0f;
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
    // TYP RYBY 3
    for (int i = 0; i < 5+(rand()%10); i++) {
        fish.push_back({ fishtab[2] });

        //Sprawdzanie czy ryby jest odpowiednio daleko od gracza w chwili rozpoczecia
        float x = fish.size() - 1;
        float safeDistance = 200.0f;
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

            //Sterowanie w menu
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
            //ustawienia
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
            //sterowanie w grze
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

            //Granice ruchu gracza
            player.x = max(player.size / 2, min(player.x, SCREEN_WIDTH - player.size / 2));
            player.y = max(player.size / 2, min(player.y, SCREEN_HEIGHT - player.size / 2));
            score = int(player.mass);
            
            if (booster.active && checkBoosterCollision(player, booster)) {
                booster.active = false;
                boosterOwned = true;
            }

            boosterPulse += 0.1f;
            boosterAngle += 0.05f;


            //typy ruchow ryb
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
                    if (f.mass == fishtab[9].mass || f.mass == fishtab[10].mass) { //mina(fishtab[10]) i najwieksza(fishtab[9]) ryba znikaja po ukonczeniu swojego ruchu, i pojawia sie ryba z parametrami fishtab[4]
                        f.mass = fishtab[4].mass;
                        f.massgain = fishtab[4].massgain;
                        f.speed = fishtab[4].speed;
                        f.size = fishtab[4].size;
                    }
                }
            }

                //granice ruchu ryb
                f.x = max(f.size / 2, min(f.x, SCREEN_WIDTH - f.size / 2));
                f.y = max(f.size / 2, min(f.y, SCREEN_HEIGHT - f.size / 2));
            }

            if (!booster.active && !boosterOwned && rand() % 600 == 0) {
                booster.x = 50 + rand() % (SCREEN_WIDTH - 100);
                booster.y = 50 + rand() % (SCREEN_HEIGHT - 100);
                booster.active = true;
            }


            //kolizje z rybami
            for (int i = 0; i < (int)fish.size(); i++) {
                if (checkCollision(player, fish[i])) {

                    if (player.mass >= fish[i].mass) {
                        player.mass += fish[i].massgain;
                        //przyrost masy po zjedzeniu
                        float enemymass,enemysize, dmass,dsize, eatcount;
                        int gamestage;
                        for (int i = 0; i < 10; i++) {
                            if (player.mass <= fishtab[i].mass) {
                                enemymass = fishtab[i].mass;
                                enemysize = fishtab[i].size;
                                gamestage = i-1;
                                break;
                            }
                            else if (player.mass > fishtab[9].mass) {
                                enemysize = 300;
                                enemymass = 50000;
                            }
                        }
                        dmass = enemymass - fish[i].mass;
                        eatcount = dmass / fish[i].massgain; //ilosc ile gracz musi zjesc ryb z ta sama masa co zjedzona, zeby moc zjesc nastepna wieksza rybe
                        dsize = enemysize - fish[i].size;
                        player.size += dsize / eatcount;



                        //respawn nowych ryb po zjedzeniu
                        fish[i] = fish.back();
                        fish.pop_back();
                        int amount;
                        int range[10] = {4,5,5,5,6,6,7,7,6,5};
                        int add[10] = {0,0,0,1,1,1,2,3,4,5};
                        float x, y;
                        if (fish.size() < 10)
                            amount = 7;
                        else
                            amount = rand() % 3 ;
                        for (int j = 0; j < amount; j++) {
                            int a = rand()% range[gamestage] + add[gamestage];
                            if(fishtab[a].move=="vertical") {
                            x = float(rand() % SCREEN_WIDTH);
                            y = 0;
                            }
                            else if(fishtab[a].move == "horizontal"){
                                x = 0;
                                y = float(rand() % SCREEN_HEIGHT);
                            }
                            else{
                                x = SCREEN_WIDTH;
                                y = float(rand() % SCREEN_HEIGHT);
                            }
                            fish.push_back({x,y,fishtab[a].size,fishtab[a].speed,fishtab[a].mass,fishtab[a].massgain,false,fishtab[a].move });
                        }
                    }
                    else if (boosterOwned) {
                        fish[i] = fish.back();
                        fish.pop_back();
                        boosterOwned = false;
                    }
                    else {
    if (score > bestScore) {
        bestScore = score;
        ofstream out("highscore.txt");
        if (out) {
            out << bestScore;
        }
    }
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
            // WYSWIETLENIE WYNIKU
string scoreText = "SCORE " + to_string(score);
int scoreWidth = getTextWidth(scoreText, 3);
drawText(renderer, scoreText, SCREEN_WIDTH - scoreWidth - 20, 20, 3);
            
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

                if (f.mass <= player.mass)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                else if (f.move == "vertical")
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else if (f.move == "wave")
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);

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
            // FINALOWY WYNIK
string finalScore = "SCORE " + to_string(score);
int fsWidth = getTextWidth(finalScore, 3);
int fsX = (SCREEN_WIDTH - fsWidth) / 2;
drawText(renderer, finalScore, fsX, y + 80, 3);

// NAJLEPSZY WYNIK
string bestText = "BEST SCORE " + to_string(bestScore);
int bestWidth = getTextWidth(bestText, 3);
int bestX = (SCREEN_WIDTH - bestWidth) / 2;
drawText(renderer, bestText, bestX, y + 140, 3);
// NOWY REKORD
if (score == bestScore) {
    string newRec = "NEW RECORD";
    int nrWidth = getTextWidth(newRec, 3);
    int nrX = (SCREEN_WIDTH - nrWidth) / 2;
    drawText(renderer, newRec, nrX, y + 200, 3);
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
