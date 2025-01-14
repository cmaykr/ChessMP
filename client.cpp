#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>

#include "piece.hpp"

SDL_Texture* getTexture(SDL_Renderer *renderer, std::string const& texturePath)
{
    SDL_Surface *surface = IMG_Load(texturePath.c_str());
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return tex;
}

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

    Piece pawn {"models/pawn.png", PieceType::Pawn, true};
    Piece rook {"models/rook.png", PieceType::Rook, true};
    Piece knight {"models/knight.png", PieceType::Knight, true};
    Piece bishop {"models/bishop.png", PieceType::Bishop, true};
    Piece queen {"models/queen.png", PieceType::Queen, true};
    Piece king {"models/king.png", PieceType::King, true};
    Piece pawnB {"models/pawn.png", PieceType::Pawn, false};
    Piece rookB {"models/rook.png", PieceType::Rook, false};
    Piece knightB {"models/knight.png", PieceType::Knight, false};
    Piece bishopB {"models/bishop.png", PieceType::Bishop, false};
    Piece queenB {"models/queen.png", PieceType::Queen, false};
    Piece kingB {"models/king.png", PieceType::King, false};
    SDL_Texture *p = getTexture(renderer, pawn.getTexture());
    SDL_Texture *r = getTexture(renderer, rook.getTexture());
    SDL_Texture *k = getTexture(renderer, knight.getTexture());
    SDL_Texture *b = getTexture(renderer, bishop.getTexture());
    SDL_Texture *q = getTexture(renderer, queen.getTexture());
    SDL_Texture *ki = getTexture(renderer, king.getTexture());
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
    std::array<std::array<Piece*, 8>, 8> board{};
    board[0][7] = &rook;
    board[1][7] = &knight;
    board[2][7] = &bishop;
    board[3][7] = &queen;
    board[4][7] = &king;
    board[5][7] = &bishop;
    board[6][7] = &knight;
    board[7][7] = &rook;

    for (int i{}; i < 8; i++)
    {
        board[i][6] = &pawn;
    }

    board[0][0] = &rookB;
    board[1][0] = &knightB;
    board[2][0] = &bishopB;
    board[3][0] = &queenB;
    board[4][0] = &kingB;
    board[5][0] = &bishopB;
    board[6][0] = &knightB;
    board[7][0] = &rookB;

    for (int i{}; i < 8; i++)
    {
        board[i][1] = &pawnB;
    }

    int x, y;

    int iterations{};
    std::cout << "Start" << std::endl;
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) == 1)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&x, &y);
                    int boardX = (x - w / 2 + size*4) / 40;
                    int boardY = (y - h / 2 + size*4) / 40;
                    std::cout << "X: " << boardX << " Y: " << boardY << std::endl;

                    Piece* piece = board[boardX][boardY];
                    if (boardY + 1 < 8)
                    {
                        board[boardX][boardY+1] = piece;
                        board[boardX][boardY] = nullptr;
                    }
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

        int piecesDrawn{};
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

                if (board[i][j] == nullptr)
                {
                    continue;;
                }
                switch (board[i][j]->getType())
                {
                    case PieceType::Pawn:
                        temp = p;
                        break;
                    case PieceType::Rook:
                        temp = r;
                        break;
                    case PieceType::Bishop:
                        temp = b;
                        break;
                    case PieceType::King:
                        temp = ki;
                        break;
                    case PieceType::Queen:
                        temp = q;
                        break;
                    case PieceType::Knight:
                        temp = k;
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