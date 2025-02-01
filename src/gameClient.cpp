#include "gameClient.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "resourceManager.hpp"

GameClient::GameClient(Game* game, std::ostream & output)
    : localBoard{game->getBoard()}, output(output), game(game)
{
    if (game == nullptr)
    {
        output << "Error: Game is not initialized in constructor." << std::endl;
        exit(1);
        return;
    }
    //localBoard = game->getBoard();
}

struct RendererDeleter
{
    void operator()(SDL_Renderer *renderer) const
    {
        SDL_DestroyRenderer(renderer);
    }
};

void GameClient::run()
{
    if (game == nullptr)
    {
        output << "Error: Game is not initialized." << std::endl;
        exit(1);
        return;
    }

    ResourceManager& instance = ResourceManager::getInstance();
    output << "Initializing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        output << "SDL_init error: " << SDL_GetError() << std::endl;

    output << "Creating SDL window" << std::endl;
    SDL_Window *window = SDL_CreateWindow("ChessMP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (window == NULL)
        output << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;

    output << "Creating SDL renderer" << std::endl;
    std::shared_ptr<SDL_Renderer> renderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(window, -1, 0), RendererDeleter());
    if (renderer == nullptr)
        output << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;

    instance.setRenderer(renderer);
    SDL_RenderClear(renderer.get());
    SDL_RenderPresent(renderer.get());

    SDL_Rect box{};
    box.x = 20;
    box.y = 20;

    output << "Loading resources" << std::endl;
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

    instance.loadTexturesFromFile("resources/pieceTextures.json");
    SDL_Texture *pawnTex = instance.getTexture(pawn.getTexture());
    SDL_Texture *rookTex = instance.getTexture(rook.getTexture());
    SDL_Texture *knightTex = instance.getTexture(knight.getTexture());
    SDL_Texture *bishopTex = instance.getTexture(bishop.getTexture());
    SDL_Texture *queenTex = instance.getTexture(queen.getTexture());
    SDL_Texture *kingTex = instance.getTexture(king.getTexture());
    instance.loadTexture("models/Small_Dot.png");
    SDL_Texture *dot = instance.getTexture("models/Small_Dot.png");
    instance.loadTexture("models/SquareDot.png");
    SDL_Texture *square = instance.getTexture("models/SquareDot.png");
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
    int initX{};
    int initY{};
    int chosenX{};
    int chosenY{};
    bool clickMove = false;
    Piece chosenPiece{};

    output << "Starting game loop" << std::endl;
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

                    if (chosenPiece.isEmpty() && !localBoard[boardX][boardY].isEmpty() && localBoard[boardX][boardY].isPieceWhite() == game->isPlayerWhitesTurn())
                    {
                        chosenPiece = localBoard[boardX][boardY];
                        chosenX = boardX;
                        chosenY = boardY;
                    }
                    else if (!chosenPiece.isEmpty() && boardX == chosenX && boardY == chosenY)
                    {
                        clickMove = false;
                    }
                    initX = boardX;
                    initY = boardY;
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    SDL_GetMouseState(&x, &y);
                    int boardX = (x - w / 2 + size*4) / 40;
                    int boardY = (y - h / 2 + size*4) / 40;
                    
                    if (boardX == chosenX && boardY == chosenY)
                    {
                        if (!chosenPiece.isEmpty())
                            clickMove = true;
                    }
                    else if (!chosenPiece.isEmpty())
                    {
                        if (!localBoard[boardX][boardY].isEmpty() && localBoard[boardX][boardY].isPieceWhite() == chosenPiece.isPieceWhite()) // Why does checking if spot is empty needed twice?
                        {
                            chosenPiece = localBoard[boardX][boardY];
                            chosenX = boardX;
                            chosenY = boardY;
                        }
                        else
                        {
                            game->tryMove(chosenX, chosenY, boardX, boardY, localBoard);
                            chosenPiece = Piece{};
                            chosenX = -1;
                            chosenY = -1;
                            clickMove = false;
                        }
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

                if (!chosenPiece.isEmpty() && i == initY && j == initX)
                {
                    SDL_SetTextureColorMod(square, 0, 255, 0);
                    SDL_RenderCopy(renderer.get(), square, NULL, &box);
                }
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
                    continue;
                }
                switch (localBoard[i][j].getType())
                {
                    case PieceType::Pawn:
                        temp = pawnTex;
                        break;
                    case PieceType::Rook:
                        temp = rookTex;
                        break;
                    case PieceType::Bishop:
                        temp = bishopTex;
                        break;
                    case PieceType::King:
                        temp = kingTex;
                        break;
                    case PieceType::Queen:
                        temp = queenTex;
                        break;
                    case PieceType::Knight:
                        temp = knightTex;
                        break;
                    default:
                        temp = nullptr;
                        break;
                }

                if (temp != nullptr)
                {
                    (localBoard[i][j].isPieceWhite()) ? SDL_SetTextureColorMod(temp, 125, 200, 200) : SDL_SetTextureColorMod(temp, 200, 200, 200);
                    SDL_RenderCopy(renderer.get(), temp, NULL, &position);
                }
            }
        }

        if (!chosenPiece.isEmpty())
        {
            SDL_Rect circle;
            circle.w = size;
            circle.h = size;
            for (int x{}; x < 8; x++)
            {
                for (int y{}; y < 8; y++)
                {
                    if (game->isMoveValid(chosenX, chosenY, x, y, localBoard))
                    {
                        circle.x = xStart + x * size;
                        circle.y = yStart + y * size;
                        SDL_SetTextureColorMod(dot, 0, 255, 0);
                        SDL_RenderCopy(renderer.get(), dot, NULL, &circle);
                    }
                }
            }
        }

        if (!clickMove && !chosenPiece.isEmpty())
        {
            SDL_GetMouseState(&x, &y);
            SDL_Rect position{};
            position.h = size;
            position.w = size;
            position.x = x - size / 2;
            position.y = y - size / 2;
            SDL_Texture *temp = instance.getTexture(chosenPiece.getTexture());

            (chosenPiece.isPieceWhite()) ? SDL_SetTextureColorMod(temp, 125, 200, 200) : SDL_SetTextureColorMod(temp, 200, 200, 200);
            SDL_RenderCopy(renderer.get(), temp, NULL, &position);
        }

        SDL_RenderPresent(renderer.get());
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}