#include "gameClient.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unistd.h>
#include <poll.h>

#include "resourceManager.hpp"

GameClient::GameClient(std::ostream & output, std::string const& serverAddress, std::string const& serverPort)
    : output(output)
{
    struct addrinfo hints{};
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;

    int err;
    if (err = getaddrinfo(serverAddress.c_str(), serverPort.c_str(), &hints, &result) < 0)
    {
        exit(1);
    }

    for (rp = result; rp; rp = rp->ai_next)
    {
        serverFD = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (serverFD == -1)
        {
            continue;
        }
        if (connect(serverFD, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            break;
        }
        
        closeSocket();
    }

    freeaddrinfo(result);

    std::string response = sendAndReceiveToServer("Request: Connect");
    std::stringstream ss{};
    ss << response;
    std::string text;
    while (getline(ss, text, '\n'))
    {
        output << text << std::endl;
        if (text == "Status: 1")
        {
            output << "Connection to server succeeded" << std::endl;
        }
        else if (text == "Status: 0")
        {
            output << "Connection to server failed, exiting." << std::endl;
            closeSocket();
            exit(1);
        }
        else if (text == "Player: White")
        {
            output << "Player is white" << std::endl;
            clientIsPlayerWhite = true;
            break;
        }
        else if (text == "Player: Black")
        {
            output << "Player is black" << std::endl;
            clientIsPlayerWhite = false;
            break;
        }
    }
    response = sendAndReceiveToServer("Requst: Full board");
    output << response << std::endl;

    ss = std::stringstream{response};
    std::string oneLineResponse{};
    getline(ss, oneLineResponse, '\n');
    if (oneLineResponse == "Status: 0")
    {
        output << "Request to get full board from server failed, exiting." << std::endl;
        exit(1);
    }

    for (int y{}; y < 8; y++)
    {
        for (int x{}; x < 8; x++)
        {
            std::string pieceString;
            ss >> pieceString;
            output << pieceString << std::endl;
            if (pieceString == "EE")
            {
                localBoard[x][y] = Piece{};
            }
            else
            {
                bool isWhite = (pieceString[0] == 'W') ? true : false;
                PieceType type{};
                std::string texturePath{};
                switch (pieceString[1])
                {
                    case 'P':
                        type = PieceType::Pawn;
                        texturePath = "models/pawn.png";
                        break;
                    case 'R':
                        type = PieceType::Rook;
                        texturePath = "models/rook.png";
                        break;
                    case 'N':
                        type = PieceType::Knight;
                        texturePath = "models/knight.png";
                        break;
                    case 'B':
                        type = PieceType::Bishop;
                        texturePath = "models/bishop.png";
                        break;
                    case 'Q':
                        type = PieceType::Queen;
                        texturePath = "models/queen.png";
                        break;
                    case 'K':
                        type = PieceType::King;
                        texturePath = "models/king.png";
                        break;
                    default:
                        type = PieceType::UNKNOWN;
                        break;
                }
                localBoard[x][y] = Piece(texturePath, type, isWhite);
            }
        }
    }

    output << "Client connected" << std::endl;
}

GameClient::~GameClient()
{
    closeSocket();
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
    bool clickMove = true;
    Piece chosenPiece{};

    output << "Starting game loop" << std::endl;
    while (running)
    {
        std::string message {receiveMessage(20)};
        if (!message.empty())
        {
            output << "Received message from server: " << std::endl << "{" << std::endl << message << "}" << std::endl;

            if (message.substr(message.find("Status: ")+8, 1) == "1")
            {
                
                int startX{}, startY{};
                int targetX{}, targetY{};
                std::stringstream ss{message.substr(message.find(':') + 1, message.size())};
                std::string text;
                while (getline(ss, text, '}'))
                {
                    std::stringstream ss2{text};
                    int num;
                    if (text.find("from") != std::string::npos)
                    {
                        std::string from = text.substr(text.find('{') + 2, text.find('}') - text.find('{'));
                        std::string x = from.substr(0, from.find(' '));
                        std::string y = from.substr(from.find(' ') + 1, 1);
                        startX = std::stoi(x);
                        startY = std::stoi(y);
                    }
                    else if (text.find("to") != std::string::npos)
                    {
                        std::string to = text.substr(text.find('{') + 2, text.find('}') - text.find('{') - 1);
                        std::string x = to.substr(0, to.find(' '));
                        std::string y = to.substr(to.find(' ') + 1, 1);
                        targetX = std::stoi(x);
                        targetY = std::stoi(y);
                    }
                }

                output << "Moving piece" << std::endl;
                output << "From: " << startX << " " << startY << " To: " << targetX << " " << targetY << std::endl;
                localBoard[targetX][targetY] = localBoard[startX][startY];
                localBoard[startX][startY] = Piece{};
                isPlayerWhitesTurn = !isPlayerWhitesTurn;

                output << "Move successful" << std::endl;
            }
            else
            {
                output << "Move failed" << std::endl;
            }
        }

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

                    if (chosenX == boardX && chosenY == boardY)
                        clickMove = false;
                    
                    if (chosenPiece.isEmpty() && !localBoard[boardX][boardY].isEmpty() && localBoard[boardX][boardY].isPieceWhite() == clientIsPlayerWhite)
                    {
                        chosenPiece = localBoard[boardX][boardY];
                        chosenX = boardX;
                        chosenY = boardY;
                    }
                    else if (!chosenPiece.isEmpty())
                    {
                        if (boardX == chosenX && boardY == chosenY)
                            clickMove = false;  

                        if (!localBoard[boardX][boardY].isEmpty() && localBoard[boardX][boardY].isPieceWhite() == clientIsPlayerWhite)
                        {
                            chosenPiece = localBoard[boardX][boardY];
                            chosenX = boardX;
                            chosenY = boardY;
                        }
                        if (tryMove(chosenX, chosenY, boardX, boardY))
                        {
                            localBoard[boardX][boardY] = chosenPiece;
                            localBoard[chosenX][chosenY] = Piece{};
                            chosenX = -1;
                            chosenY = -1;
                            clickMove = true;
                            chosenPiece = Piece{};
                            isPlayerWhitesTurn = !isPlayerWhitesTurn;
                        }
                    }
                    initX = boardX;
                    initY = boardY;
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    clickMove = true;

                    if (!chosenPiece.isEmpty())
                    {
                        SDL_GetMouseState(&x, &y);
                        int boardX = (x - w / 2 + size*4) / 40;
                        int boardY = (y - h / 2 + size*4) / 40;

                        if (!localBoard[boardX][boardY].isEmpty() && localBoard[boardX][boardY].isPieceWhite() == clientIsPlayerWhite)
                        {
                            chosenPiece = localBoard[boardX][boardY];
                            chosenX = boardX;
                            chosenY = boardY;
                        }
                        else if (tryMove(chosenX, chosenY, boardX, boardY))
                        {
                            localBoard[boardX][boardY] = chosenPiece;
                            localBoard[chosenX][chosenY] = Piece{};
                            chosenX = -1;
                            chosenY = -1;
                            chosenPiece = Piece{};
                            isPlayerWhitesTurn = !isPlayerWhitesTurn;
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

                if (!chosenPiece.isEmpty() && i == chosenY && j == chosenX)
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
                    // if (game->isMoveValid(chosenX, chosenY, x, y, localBoard))
                    // {
                    //     circle.x = xStart + x * size;
                    //     circle.y = yStart + y * size;
                    //     SDL_SetTextureColorMod(dot, 0, 255, 0);
                    //     SDL_RenderCopy(renderer.get(), dot, NULL, &circle);
                    // }
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

std::string GameClient::receiveMessage(int timeout)
{
    if (timeout < -1 || timeout > 1000)
    {
        output << "Invalid timeout value, exiting." << std::endl;
        exit(1);
    }
    char buf[1024];
    struct pollfd fds[1];
    fds[0].fd = serverFD;
    fds[0].events = POLLIN;
    long unsigned int N = {};
    int polls = poll(fds, 1, timeout);
    if (polls > 0)
    {
        N = recv(serverFD, buf, sizeof(buf), 0);
        if (N == -1)
        {
            std::cerr << "Socket failed when receiving message" << std::endl;
            closeSocket();
            exit(1);
        }
        if (N == 0)
        {
            std::cerr << "Peer socket not available, closing connection" << std::endl;
            closeSocket();
            exit(2);
        }
        std::string message {buf, N};

        return message;

    }
    else if (polls < 0)
    {
        std::cerr << "Nothing to receive" << std::endl;
        closeSocket();
        exit(1);
    }

    return "";
}

std::string GameClient::sendAndReceiveToServer(std::string const& message)
{
    int N = send(serverFD, message.c_str(), message.size(), 0);
    if (N == -1)
    {
        output << "Nothing sent, exiting." << std::endl;
        exit(1);
    }
    
    return receiveMessage(-1);
}

void GameClient::closeSocket()
{
    close(serverFD);
}

bool GameClient::tryMove(int chosenX, int chosenY, int boardX, int boardY)
{
    std::string move = "Request: MOVE\nfrom{ " + std::to_string(chosenX) + " " + std::to_string(chosenY) + " } to{ " + std::to_string(boardX) + " " + std::to_string(boardY) + " }";
    output << "Sending request to server: [" << move << "]" << std::endl;
    std::string response = sendAndReceiveToServer(move);
    output << "Response from server: " << std::endl << "{" << response << "}" << std::endl;

    if (response.substr(response.find("Status: ")+8, 1) == "1")
    {
        return true;
    }

    return false;
}