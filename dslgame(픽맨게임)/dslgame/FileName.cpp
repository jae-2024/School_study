#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_messagebox.h>
#include <time.h>
#include <stdlib.h>
#include <SDL_ttf.h>

#define WIDTH 640
#define HEIGHT 480

typedef struct {
    SDL_Rect rect;
    int dx, dy;
    SDL_Texture* texture;
} Game;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* imageSurface = NULL;
    SDL_Rect destRect;

    SDL_Surface* textSurface;
    SDL_Texture* texture;
    SDL_Color textColor = { 255, 255, 255 };
    TTF_Font* mFont;    //스코어를 만들기 위한 포인터

    // 게임속 좌표 설정
    int minX = 80;
    int minY = 60;
    int maxX = 550;
    int maxY = 410;

    // Pacman의 초기 위치
    int pacmanX = 80;
    int pacmanY = 60;
    int speed = 15;
    int angle = 0;

    // monster의 이동 속도
    int mon_speed = 10;

    // 보물 먹은 횟수
    int cnt = 0;

    // 몬스터 이동 시간 조정, 시간 측정을 위해 Unit32비트 부호 없는 정수를 사용
    Uint32 moveTime = 1000; // 1초마다 움직이도록 설정 (1000ms = 1초)

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return 1;
    }

    // 폰트 초기화
    if (TTF_Init() == -1)
    {
        return 0;
    }

    // 윈도우 창 생성
    window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN);


    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int score = 0;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    Game Game[5];

    SDL_Surface* backgroundSurface = IMG_Load("imgs/packbackground2.png");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_Surface* pacman = IMG_Load("imgs/pacman.png");
    SDL_Surface* monster1 = IMG_Load("imgs/monster1.png");
    SDL_Surface* monster2 = IMG_Load("imgs/monster2.png");
    SDL_Surface* monster3 = IMG_Load("imgs/monster3.png");
    SDL_Surface* treasure = IMG_Load("imgs/treasure.jpg");

    srand(time(NULL));

    Game[0].texture = SDL_CreateTextureFromSurface(renderer, pacman);
    Game[0].rect = { 80, 60, 20, 20 };

    Game[1].texture = SDL_CreateTextureFromSurface(renderer, monster1);
    Game[1].rect = { 100 + rand() % 380, 100 + rand() % 240, 60, 60 };

    Game[2].texture = SDL_CreateTextureFromSurface(renderer, monster2);
    Game[2].rect = { 100 + rand() % 410, 100 + rand() % 270, 30, 30 };

    Game[3].texture = SDL_CreateTextureFromSurface(renderer, monster3);
    Game[3].rect = { 100 + rand() % 420, 100 + rand() % 280, 20, 20 };

    Game[4].texture = SDL_CreateTextureFromSurface(renderer, treasure);
    Game[4].rect = { 200 + rand() % 320, 200 + rand() % 180, 15, 15 };
    /*540 400*/
    /*540 - monster크기*/

    Uint32 lastTime = SDL_GetTicks();   // 프로그램이 시작된 이후 경과한 시간을 밀리초 단위로 변환해서 lastTime에 초기화
    float fps = 0.f;

    // 메시지 루프
    SDL_Event event;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }

            // 키 입력 처리
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    if (pacmanY - speed >= minY) {
                        pacmanY -= speed;
                        angle = 270;
                    }
                    break;
                case SDLK_s:
                    if (pacmanY + speed <= maxY - 20) {
                        pacmanY += speed;
                        angle = 90;
                    }
                    break;
                case SDLK_a:
                    if (pacmanX - speed >= minX) {
                        pacmanX -= speed;
                        angle = 180;
                    }
                    break;
                case SDLK_d:
                    if (pacmanX + speed <= maxX - 20) {
                        pacmanX += speed;
                        angle = 0;
                    }
                    break;
                case SDLK_ESCAPE:
                    quit = 1;
                    break;
                }
            }
        }

        //몬스터와의 충돌 감지
        for (int i = 1; i < 4; i++) {
            if (Game[0].rect.x + 1< Game[i].rect.x + Game[i].rect.w &&
                Game[0].rect.x + Game[0].rect.w > Game[i].rect.x &&
                Game[0].rect.y < Game[i].rect.y + Game[i].rect.h &&
                Game[0].rect.y + Game[0].rect.h > Game[i].rect.y) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "Oops!", window);
                quit = 1; // 게임 종료
            }
        }

        // 보물과의 충돌 감지
        if (Game[0].rect.x < Game[4].rect.x + 15 &&
            Game[0].rect.x + 10 > Game[4].rect.x &&
            Game[0].rect.y < Game[4].rect.y + 15 &&
            Game[0].rect.y + 10 > Game[4].rect.y) {
            Game[4].rect = { 200 + rand() % 330, 200 + rand() % 190, 15, 15 };
            cnt++;
            mon_speed += 5;
            score++;
            if (moveTime > 200) {
                moveTime -= 200;
            }
            if (cnt == 5) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "Good!", window);
                quit = 1; // 게임 종료
            }
        }

        // 점수판 출력 코드
        char scoreText[50];
        sprintf(scoreText, "Score: %d", score);
        mFont = TTF_OpenFont("NGULIM.TTF", 30);
        textSurface = TTF_RenderText_Blended(mFont, scoreText, textColor);
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        destRect = { 10, 10, textSurface->w, textSurface->h };

        // 1초마다 몬스터가 움직이도록 설정
        Uint32 currentTime = SDL_GetTicks(); // 현재 시간 (밀리초)
        Uint32 elapsedTime = currentTime - lastTime;
        if (elapsedTime >= moveTime) { // 1초 경과 확인
            for (int i = 1; i < 4; ++i) {
                int randomDirection = rand() % 4;
                switch (randomDirection) {
                case 0:
                    if (Game[i].rect.y - mon_speed >= minY) {
                        Game[i].rect.y -= mon_speed;
                    }
                    break;
                case 1:
                    if (Game[i].rect.y + mon_speed <= maxY - 60) {
                        Game[i].rect.y += mon_speed;
                    }
                    break;
                case 2:
                    if (Game[i].rect.x - mon_speed >= minX) {
                        Game[i].rect.x -= mon_speed;
                    }
                    break;
                case 3:
                    if (Game[i].rect.x + mon_speed <= maxX - 60) {
                        Game[i].rect.x += mon_speed;
                    }
                    break;
                }
            }
            lastTime = currentTime;
        }

        SDL_Rect backgroundRect = { 80, 60, 470, 350 };
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
        

        Game[0].rect.x = pacmanX;
        Game[0].rect.y = pacmanY;

        SDL_RenderCopyEx(renderer, Game[0].texture, NULL, &Game[0].rect, angle, NULL, SDL_FLIP_NONE);
        SDL_RenderCopy(renderer, Game[1].texture, NULL, &Game[1].rect);
        SDL_RenderCopy(renderer, Game[2].texture, NULL, &Game[2].rect);
        SDL_RenderCopy(renderer, Game[3].texture, NULL, &Game[3].rect);
        SDL_RenderCopy(renderer, Game[4].texture, NULL, &Game[4].rect);

        SDL_RenderPresent(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(backgroundTexture);
    SDL_FreeSurface(backgroundSurface);

    SDL_DestroyTexture(Game[0].texture);
    SDL_FreeSurface(pacman);

    SDL_DestroyTexture(Game[1].texture);
    SDL_FreeSurface(monster1);

    SDL_DestroyTexture(Game[2].texture);
    SDL_FreeSurface(monster2);

    SDL_DestroyTexture(Game[3].texture);
    SDL_FreeSurface(monster3);

    SDL_DestroyTexture(Game[4].texture);
    SDL_FreeSurface(treasure);

    // 점수판 종료 코드
    TTF_CloseFont(mFont);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(textSurface);

    // 종료
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
