#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 200 * 4
#define HEIGHT 140 * 4
#define MAX_CARDS 14

class Card4Memory{
public:
    SDL_Rect rect;
    SDL_Texture* texture;
    int dx, dy;
    bool isCovered;
};

SDL_Rect hintButton = { WIDTH - 100, HEIGHT - 50, 80, 40 };
SDL_Texture* hintButtonTexture = NULL;

class Pl : public Card4Memory{
public:
    bool isMouseOverButton(int mouseX, int mouseY);
    void loadHintButtonTexture(SDL_Renderer* renderer);
    void renderHintButton(SDL_Renderer* renderer);
    bool isOverlapping(SDL_Rect newRect, SDL_Rect existingRects[], int count);
    void initializeLevel(Card4Memory* cards, SDL_Rect* placedRects, SDL_Renderer* renderer, char cardImage[][22], int cardCount);
};

bool Pl::isMouseOverButton(int mouseX, int mouseY) {
    return mouseX >= hintButton.x && mouseX <= hintButton.x + hintButton.w &&
        mouseY >= hintButton.y && mouseY <= hintButton.y + hintButton.h;
}

void Pl::loadHintButtonTexture(SDL_Renderer* renderer) {
    SDL_Surface* buttonSurface = IMG_Load("img/hintbutton.jpg");
    hintButtonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);
}

void Pl::renderHintButton(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, hintButtonTexture, NULL, &hintButton);
}

bool Pl::isOverlapping(SDL_Rect newRect, SDL_Rect existingRects[], int count) {
    for (int i = 0; i < count; i++) {
        SDL_Rect rect = existingRects[i];
        if (newRect.x < rect.x + rect.w &&
            newRect.x + newRect.w > rect.x &&
            newRect.y < rect.y + rect.h &&
            newRect.y + newRect.h > rect.y) {
            return true;
        }
    }
    return newRect.x < hintButton.x + hintButton.w &&
        newRect.x + newRect.w > hintButton.x &&
        newRect.y < hintButton.y + hintButton.h &&
        newRect.y + newRect.h > hintButton.y;
}

void Pl::initializeLevel(Card4Memory* cards, SDL_Rect* placedRects, SDL_Renderer* renderer, char cardImage[][22], int cardCount) {
    for (int i = 0; i < cardCount; i++) {
        SDL_Rect newRect;
        bool validPosition = false;

        while (!validPosition) {
            newRect.x = rand() % (WIDTH - 70);
            newRect.y = rand() % (HEIGHT - 70);
            newRect.w = 80;
            newRect.h = 80;

            if (!isOverlapping(newRect, placedRects, i)) {
                validPosition = true;
                placedRects[i] = newRect;
            }
        }
        cards[i].rect = newRect;
        cards[i].isCovered = true;
    }

    SDL_RenderClear(renderer);
    for (int i = 0; i < cardCount; i++) {
        SDL_RenderCopy(renderer, cards[i].texture, NULL, &cards[i].rect);
    }
    renderHintButton(renderer);
    SDL_RenderPresent(renderer);
}