#include "gameClient.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "resourceManager.hpp"

GameClient::GameClient(std::array<std::array<Piece, 8>, 8> &board)
    : localBoard{board}
{

}

struct RendererDeleter
{
    void operator()(SDL_Renderer *renderer) const
    {
        SDL_DestroyRenderer(renderer);
    }
};

/// @brief Attempts to move a piece on the board. Use this method to move pieces on the board, because a move needs to be checked for several rules before moving.
/// @return Returns true if move is valid according to all checked rules, false otherwise.
bool GameClient::tryMove(int startX, int startY, int targetX, int targetY)
{
    Piece piece = localBoard[startX][startY];
    
    bool validMove = false;
    switch (piece.getType())
    {
        case Pawn:
        {
            if (targetX == startX && (targetY) == startY + 1)
            {
                validMove = true;
            }
            break;
        }
        case Rook:
        {
            if (targetX == startX || targetY == startY)
            {
                validMove = true;
            }
            break;
        }
        case King:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);

            bool validMoveXAxis = xDiff == 1 || xDiff == 0;
            bool validMoveYAxis = yDiff == 1 || yDiff == 0;
            if (validMoveXAxis && validMoveYAxis)
            {
                validMove = true;
            }
            break;
        }
        case Bishop:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff)
            {
                validMove = true;
            }
            break;
        }
        case Queen:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == yDiff || targetX == startX || targetY == startY)
            {
                validMove = true;
            }
            break;
        }
        case Knight:
        {
            int xDiff = abs(startX - targetX);
            int yDiff = abs(startY - targetY);
            if (xDiff == 2 && yDiff == 1)
            {
                validMove = true;
            }
            else if (yDiff == 2 && xDiff == 1)
            {
                
                validMove = true;
            }
            break;
        }
        default:
            break;
    }

    if (validMove)
        move(startX, startY, targetX, targetY);
    return validMove;
}

void GameClient::move(int initX, int initY, int boardX, int boardY)
{
    Piece piece = localBoard[initX][initY];
    if (localBoard[boardX][boardY].isEmpty())
    {
        localBoard[boardX][boardY] = piece;
        localBoard[initX][initY] = Piece{};
    }
}

void GameClient::run()
{
    ResourceManager& instance = ResourceManager::getInstance();
    std::cout << "Initializing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        std::cerr << "SDL_init error: " << SDL_GetError() << std::endl;

    std::cout << "Creating SDL window" << std::endl;
    SDL_Window *window = SDL_CreateWindow("ChessMP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (window == NULL)
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;

    std::cout << "Creating SDL renderer" << std::endl;
    std::shared_ptr<SDL_Renderer> renderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(window, -1, 0), RendererDeleter());
    if (renderer == nullptr)
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;

    instance.setRenderer(renderer);
    SDL_RenderClear(renderer.get());
    SDL_RenderPresent(renderer.get());

    SDL_Rect box{};
    box.x = 20;
    box.y = 20;

    std::cout << "Loading resources" << std::endl;
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
    instance.addTexture(pawn.getTexture());
    instance.addTexture(rook.getTexture());
    instance.addTexture(knight.getTexture());
    instance.addTexture(bishop.getTexture());
    instance.addTexture(queen.getTexture());
    instance.addTexture(king.getTexture());
    SDL_Texture *p = instance.getTexture(pawn.getTexture());
    SDL_Texture *r = instance.getTexture(rook.getTexture());
    SDL_Texture *k = instance.getTexture(knight.getTexture());
    SDL_Texture *b = instance.getTexture(bishop.getTexture());
    SDL_Texture *q = instance.getTexture(queen.getTexture());
    SDL_Texture *ki = instance.getTexture(king.getTexture());
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
    
    int x, y;

    bool running = true;
    bool mouseDown = false;
    bool lastMouseDown = false;
    int initX{};
    int initY{};
    bool clickMove = false;
    Piece chosenPiece{};

    std::cout << "Starting game loop" << std::endl;
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
                case SDL_MOUSEBUTTONDOWN:
                {
                    SDL_GetMouseState(&x, &y);
                    int boardX = (x - w / 2 + size*4) / 40;
                    int boardY = (y - h / 2 + size*4) / 40;

                    lastMouseDown = mouseDown;
                    mouseDown = true;

                    if (clickMove)
                    {
                        tryMove(initX, initY, boardX, boardY);

                        clickMove = false;
                    }
                    else
                    {
                        initX = boardX;
                        initY = boardY;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    SDL_GetMouseState(&x, &y);
                    int boardX = (x - w / 2 + size*4) / 40;
                    int boardY = (y - h / 2 + size*4) / 40;
                    
                    if (initX == boardX && initY == boardY)
                    {
                        if (!clickMove && !localBoard[boardX][boardY].isEmpty())
                        {
                            clickMove = true;
                            chosenPiece = localBoard[boardX][initY];
                            initX = boardX;
                            initY = boardY;
                        } else
                        {
                            clickMove = false;
                            chosenPiece = Piece{};
                        }
                    }
                    else if (tryMove(initX, initY, boardX, boardY))
                    {
                        chosenPiece = Piece{};
                    }

                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer.get(), 255, 100, 100, 255);
        SDL_RenderClear(renderer.get());
        box.x = xStart;
        box.y = yStart;

        for (int i{}; i < 8; i++)
        {
            for (int j{}; j < 8; j++)
            {
                if ((i + j) % 2 == 0)
                    SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
                SDL_RenderFillRect(renderer.get(), &box);

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

                if (localBoard[i][j].isEmpty())
                {
                    continue;;
                }
                switch (localBoard[i][j].getType())
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
                    SDL_RenderCopy(renderer.get(), temp, NULL, &position);
                }
            }
        }


        SDL_RenderPresent(renderer.get());
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}