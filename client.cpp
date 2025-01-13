#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>

#include "piece.hpp"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        std::cerr << "SDL_init error: " << SDL_GetError() << std::endl;

    SDL_Window *window = SDL_CreateWindow("ChessMP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (window == NULL)
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Rect box{};
    box.x = 20;
    box.y = 20;

    Piece piece {box, "models/pawn.png", PieceType::Pawn};
    SDL_Surface *surface = IMG_Load("models/pawn.png");
    SDL_Texture *pawn = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("models/rook.png");
    SDL_Texture *rook = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("models/knight.png");
    SDL_Texture *knight = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("models/bishop.png");
    SDL_Texture *bishop = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("models/queen.png");
    SDL_Texture *queen = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load("models/king.png");
    SDL_Texture *king = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Rect pos;
    pos.h = 40;
    pos.w = 40;

    int size = 40;
    
    box.w = size;
    box.h = size;

    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);
    int xStart{w / 2 - size*4};
    int yStart{h / 2 - size*4};

    std::cout << "Init board" << std::endl;
    std::array<std::array<PieceType, 8>, 8> board{};
    board[0][7] = PieceType::Rook;
    board[1][7] = PieceType::Knight;
    board[2][7] = PieceType::Bishop;
    board[3][7] = PieceType::Queen;
    board[4][7] = PieceType::King;
    board[5][7] = PieceType::Bishop;
    board[6][7] = PieceType::Knight;
    board[7][7] = PieceType::Rook;

    for (int i{}; i < 8; i++)
    {
        board[i][6] = PieceType::Pawn;
    }

    std::cout << "Start" << std::endl;
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        SDL_RenderClear(renderer);
        box.x = xStart;
        box.y = yStart;

        for (int i{}; i < 8; i++)
        {
            for (int j{}; j < 8; j++)
            {
                if ((i + j) % 2 == 0)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(renderer, &box);

                box.x += size;
            }
            box.y += size;
            box.x = xStart;
        }

        for (int i{}; i < 8; i++)
        {
            for (int j{}; j < 8; j++)
            {
                SDL_Rect position{};
                position.h = size;
                position.w = size;
                position.x = xStart + i*size;
                position.y = yStart + j*size;
                SDL_Texture *temp;
                switch (board[i][j])
                {
                    case PieceType::Pawn:
                        temp = pawn;
                        break;
                    case PieceType::Rook:
                        temp = rook;
                        break;
                    case PieceType::Bishop:
                        temp = bishop;
                        break;
                    case PieceType::King:
                        temp = king;
                        break;
                    case PieceType::Queen:
                        temp = queen;
                        break;
                    case PieceType::Knight:
                        temp = knight;
                        break;
                    default:
                        temp = nullptr;
                        break;
                }

                if (temp != nullptr)
                {
                    SDL_SetTextureColorMod(temp, 125, 200, 200);
                    SDL_RenderCopy(renderer, temp, NULL, &position);
                }
            }
        }


        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //SDL_DestroyTexture(tex);
    SDL_Quit();
    
    return 0;
}