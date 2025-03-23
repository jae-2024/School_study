#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memory_game.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* imageSurface[MAX_CARDS];
    Pl card4Game[MAX_CARDS];

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL Memory Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window) {
        printf("SDL Initialization Fail: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    char cardImage[14][22] = { "img/character_1.png", "img/character_2.png", "img/character_3.png", "img/character_4.png",
                               "img/character_5.png", "img/character_6.png", "img/character_7.png", "img/character_8.png",
                               "img/character_9.png", "img/character_10.png", "img/character_11.png", "img/character_12.png",
                               "img/character_13.png", "img/character_14.png" };

    int card[14] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    int i, j;
    Pl gamefunc;

    srand(time(NULL));
    SDL_Rect placedRects[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++) {
        imageSurface[i] = IMG_Load(cardImage[card[i]]);
        card4Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
    }
    gamefunc.loadHintButtonTexture(renderer);

    int cardCount = 6;
    int lifeCount = 5;

    for (int gameRound = 0; cardCount <= MAX_CARDS; gameRound++) {
        gamefunc.initializeLevel(card4Game, placedRects, renderer, cardImage, cardCount);

        SDL_RenderClear(renderer);
        for (i = 0; i < cardCount; i++) SDL_RenderCopy(renderer, card4Game[i].texture, NULL, &card4Game[i].rect);

        // ��Ʈ ��ư ������
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // ��ư ��� ��
        SDL_RenderFillRect(renderer, &hintButton);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // �׵θ� ��
        SDL_RenderDrawRect(renderer, &hintButton);

        // ��Ʈ ��ư �̹��� �ε�
        gamefunc.loadHintButtonTexture(renderer);
        // ��Ʈ ��ư ������
        gamefunc.renderHintButton(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(4000);

        for (i = 0; i < cardCount; i++) {
            imageSurface[i] = IMG_Load("img/hidepic.png");
            card4Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
            card4Game[i].isCovered = true;
        }
        // ���ο� ī�� �̹��� ǥ��
        for (i = 0; i < cardCount; i++) SDL_RenderCopy(renderer, card4Game[i].texture, NULL, &card4Game[i].rect);
        SDL_RenderPresent(renderer);

        // �޽��� ����
        int currentCardIndex = 0;
        bool quit = false;
        SDL_Event e;
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    // ���콺 Ŭ�� ��ǥ Ȯ��
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    // ��Ʈ ��ư Ŭ�� Ȯ��
                    if (gamefunc.isMouseOverButton(x, y)) {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Hint", "Find different!", window);
                        // ������ ������ �ϴ� ī���� ���� �̹����� ����
                        if (currentCardIndex < cardCount) {
                            // ���� ������ �� ī���� ���� �̹����� �ٸ� �̹����� ��ü
                            if (card4Game[currentCardIndex].isCovered) {
                                imageSurface[currentCardIndex] = IMG_Load("img/hint.png");
                                card4Game[currentCardIndex].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[currentCardIndex]);
                            }
                        }

                        // ȭ�� ����
                        SDL_RenderClear(renderer);
                        for (i = 0; i < cardCount; i++) {
                            SDL_RenderCopy(renderer, card4Game[i].texture, NULL, &card4Game[i].rect);
                        }

                        // ��Ʈ ��ư ������
                        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // ��ư ��� ��
                        SDL_RenderFillRect(renderer, &hintButton);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // �׵θ� ��
                        SDL_RenderDrawRect(renderer, &hintButton);

                        // ��Ʈ ��ư ������
                        gamefunc.renderHintButton(renderer);
                        SDL_RenderPresent(renderer);
                    }

                    // Ŭ���� ī�� Ȯ��
                    for (i = 0; i < cardCount; i++) {
                        // ī���� ��� �ȿ��� Ŭ���Ǿ����� Ȯ��
                        if (x >= card4Game[i].rect.x && x <= card4Game[i].rect.x + card4Game[i].rect.w &&
                            y >= card4Game[i].rect.y && y <= card4Game[i].rect.y + card4Game[i].rect.h) {

                            // Ŭ���� ī�尡 ���� �ִ� �������� Ȯ��
                            if (card4Game[i].isCovered) {
                                // �� �̹��� �ε�
                                imageSurface[i] = IMG_Load(cardImage[card[i]]);
                                card4Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                                card4Game[i].isCovered = false;

                                // Ŭ���� ī�� ������ �̰� �����ָ� ������ �ƴ� ī�尡 �������� ����
                                SDL_RenderClear(renderer);
                                for (int j = 0; j < cardCount; j++) {
                                    SDL_RenderCopy(renderer, card4Game[j].texture, NULL, &card4Game[j].rect);
                                }
                                // ��Ʈ ��ư ������
                                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // ��ư ��� ��
                                SDL_RenderFillRect(renderer, &hintButton);
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // �׵θ� ��
                                SDL_RenderDrawRect(renderer, &hintButton);

                                // ��Ʈ ��ư ������
                                gamefunc.renderHintButton(renderer);
                                SDL_RenderPresent(renderer);
                                SDL_RenderPresent(renderer);

                                // ������ �´��� Ȯ��
                                if (card[i] == currentCardIndex) {
                                    currentCardIndex++;
                                    if (currentCardIndex == cardCount) {
                                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "GOOD!", window);
                                        quit = true;
                                    }
                                }
                                else {
                                    lifeCount--;
                                    // ����� �� ������ ���� ����
                                    if (lifeCount == 0) {
                                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", "OOPS!", window);
                                        cardCount = MAX_CARDS;
                                        quit = true;
                                    }

                                    SDL_Delay(300);

                                    // ���� �̹����� �ٽ� ����
                                    if (card4Game[i].texture != NULL) SDL_DestroyTexture(card4Game[i].texture);
                                    if (imageSurface[i] != NULL) SDL_FreeSurface(imageSurface[i]);

                                    imageSurface[i] = IMG_Load("img/hidepic.png");
                                    card4Game[i].texture = SDL_CreateTextureFromSurface(renderer, imageSurface[i]);
                                    card4Game[i].isCovered = true;
                                }

                                // ȭ�� ����
                                SDL_RenderClear(renderer);
                                for (int j = 0; j < cardCount; j++) {
                                    SDL_RenderCopy(renderer, card4Game[j].texture, NULL, &card4Game[j].rect);
                                }

                                // ��Ʈ ��ư ������
                                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // ��ư ��� ��
                                SDL_RenderFillRect(renderer, &hintButton);
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // �׵θ� ��
                                SDL_RenderDrawRect(renderer, &hintButton);

                                // ��Ʈ ��ư ������
                                gamefunc.renderHintButton(renderer);
                                SDL_RenderPresent(renderer);
                            }
                        }
                    }
                }
            }
        }

        cardCount += 4;
    }

    for (int i = 0; i < MAX_CARDS; i++) {
        SDL_DestroyTexture(card4Game[i].texture);
        SDL_FreeSurface(imageSurface[i]);
    }

    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
