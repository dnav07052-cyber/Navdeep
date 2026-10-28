#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <string>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

void setColor(int colorCode)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

struct Pipe
{
    int x;
    float gapY;
    int gapSize;
    bool isMoving;
    float speedY;
    bool passedP1;
    bool passedP2;

    Pipe(int _x, float _gapY, int _gapSize, bool _isMoving, float _speedY)
        : x(_x), gapY(_gapY), gapSize(_gapSize), isMoving(_isMoving), speedY(_speedY), passedP1(false), passedP2(false) {}
};

string getKeyName(int vkCode, char charCode)
{
    if (vkCode == VK_SPACE || charCode == ' ')
        return "SPACE";
    if (vkCode == VK_UP)
        return "UP ARROW";
    if (vkCode == VK_RETURN)
        return "ENTER";
    string s = "";
    s += (char)toupper((unsigned char)charCode);
    return s;
}

void clearInputBuffer()
{
    while (_kbhit())
    {
        _getch();
    }
}

int main()
{
    srand((unsigned)time(0));
    hideCursor();

    char p1KeyChar = 'w';
    int p1Vk = 'W';

    char p2KeyChar = ' ';
    int p2Vk = VK_SPACE;

    while (true)
    {
        system("cls");
        setColor(11);
        cout << "========================================\n";
        cout << "           FLAPPY BIRD CONSOLE          \n";
        cout << "========================================\n\n";

        setColor(14);
        cout << "  [1] Single Player (Jump: " << getKeyName(p1Vk, p1KeyChar) << " or SPACE)\n";
        setColor(13);
        cout << "  [2] Two Player Mode\n";
        cout << "      - P1: " << getKeyName(p1Vk, p1KeyChar) << "\n";
        cout << "      - P2: " << getKeyName(p2Vk, p2KeyChar) << "\n";
        setColor(10);
        cout << "  [3] Remap Keys\n";
        setColor(12);
        cout << "  [Q] Quit Game\n\n";

        setColor(15);
        cout << "Select an option (1, 2, 3, Q): " << flush;

        clearInputBuffer();
        char menuChoice = _getch();
        cout << menuChoice << "\n";

        if (menuChoice == 'q' || menuChoice == 'Q')
        {
            setColor(7);
            cout << "\nThanks for playing!\n";
            return 0;
        }

        if (menuChoice == '3')
        {
            system("cls");
            setColor(11);
            cout << "========================================\n";
            cout << "          KEY REMAPPING SETUP           \n";
            cout << "========================================\n\n";

            setColor(14);
            cout << "Press key for Player 1 Jump: " << flush;
            clearInputBuffer();
            p1KeyChar = _getch();
            if (p1KeyChar == ' ')
                p1Vk = VK_SPACE;
            else
                p1Vk = toupper((unsigned char)p1KeyChar);
            cout << "[" << getKeyName(p1Vk, p1KeyChar) << "]\n\n";

            setColor(13);
            cout << "Press key for Player 2 Jump: " << flush;
            clearInputBuffer();
            p2KeyChar = _getch();
            if (p2KeyChar == ' ')
                p2Vk = VK_SPACE;
            else
                p2Vk = toupper((unsigned char)p2KeyChar);
            cout << "[" << getKeyName(p2Vk, p2KeyChar) << "]\n\n";

            setColor(10);
            cout << "Keys remapped successfully! Press any key to continue..." << flush;
            _getch();
            continue;
        }

        bool twoPlayer = (menuChoice == '2');

        int gapSize = 6;
        int sleepTime = 40;
        float gravity = 0.32f;
        const float jump = -1.6f;

        float bird1Y = HEIGHT / 2.0f - 1;
        float vel1 = 0.0f;
        bool p1Alive = true;
        int p1Score = 0;
        const int bird1X = 8;

        float bird2Y = HEIGHT / 2.0f + 1;
        float vel2 = 0.0f;
        bool p2Alive = twoPlayer;
        int p2Score = 0;
        const int bird2X = 8;

        system("cls");

        vector<Pipe> pipes;
        pipes.push_back(Pipe(WIDTH - 1, (float)(rand() % (HEIGHT - gapSize - 4) + 2), gapSize, false, 0.0f));

        int frameCount = 0;
        bool gameOver = false;
        bool gameStarted = false;

        bool p1PrevState = false;
        bool p2PrevState = false;

        while (!gameOver)
        {
            bool p1KeyDown = (GetAsyncKeyState(p1Vk) & 0x8000) != 0;
            if (!twoPlayer && (GetAsyncKeyState(VK_SPACE) & 0x8000))
                p1KeyDown = true;

            bool p2KeyDown = twoPlayer && ((GetAsyncKeyState(p2Vk) & 0x8000) != 0 || (GetAsyncKeyState(VK_UP) & 0x8000) != 0);

            if (_kbhit())
            {
                char ch = _getch();
                if (ch == 27 || ch == 'q' || ch == 'Q')
                {
                    gameOver = true;
                    break;
                }
                if (tolower((unsigned char)ch) == tolower((unsigned char)p1KeyChar))
                    p1KeyDown = true;
                if (!twoPlayer && ch == ' ')
                    p1KeyDown = true;
                if (twoPlayer && tolower((unsigned char)ch) == tolower((unsigned char)p2KeyChar))
                    p2KeyDown = true;
            }

            if (!gameStarted)
            {
                if (p1KeyDown || p2KeyDown)
                {
                    gameStarted = true;
                    if (p1KeyDown && p1Alive)
                        vel1 = jump;
                    if (p2KeyDown && p2Alive)
                        vel2 = jump;
                }
            }
            else
            {
                if (p1KeyDown && !p1PrevState && p1Alive)
                {
                    vel1 = jump;
                }
                if (p2KeyDown && !p2PrevState && p2Alive)
                {
                    vel2 = jump;
                }
            }

            p1PrevState = p1KeyDown;
            p2PrevState = p2KeyDown;

            if (gameStarted)
            {
                if (p1Alive)
                {
                    vel1 += gravity;
                    bird1Y += vel1;
                    if (bird1Y >= HEIGHT - 1)
                    {
                        bird1Y = (float)(HEIGHT - 1);
                        p1Alive = false;
                    }
                    if (bird1Y < 0)
                    {
                        bird1Y = 0;
                        vel1 = 0;
                    }
                }

                if (twoPlayer && p2Alive)
                {
                    vel2 += gravity;
                    bird2Y += vel2;
                    if (bird2Y >= HEIGHT - 1)
                    {
                        bird2Y = (float)(HEIGHT - 1);
                        p2Alive = false;
                    }
                    if (bird2Y < 0)
                    {
                        bird2Y = 0;
                        vel2 = 0;
                    }
                }

                if (!p1Alive && (!twoPlayer || !p2Alive))
                {
                    gameOver = true;
                }

                frameCount++;
                if (frameCount % 18 == 0)
                {
                    bool isMoving = (rand() % 100 < 45);
                    float dir = (rand() % 2 == 0) ? 0.35f : -0.35f;
                    float startGapY = (float)(rand() % (HEIGHT - gapSize - 4) + 2);

                    pipes.push_back(Pipe(WIDTH - 1, startGapY, gapSize, isMoving, dir));
                }

                for (size_t i = 0; i < pipes.size(); i++)
                {
                    pipes[i].x--;

                    if (pipes[i].isMoving)
                    {
                        pipes[i].gapY += pipes[i].speedY;
                        if (pipes[i].gapY <= 1.0f || pipes[i].gapY >= HEIGHT - pipes[i].gapSize - 2)
                        {
                            pipes[i].speedY = -pipes[i].speedY;
                        }
                    }

                    int currentGapY = (int)(pipes[i].gapY);

                    if (p1Alive && pipes[i].x == bird1X)
                    {
                        int b1Y = (int)(bird1Y);
                        if (b1Y < currentGapY || b1Y >= currentGapY + pipes[i].gapSize)
                        {
                            p1Alive = false;
                        }
                        else if (!pipes[i].passedP1)
                        {
                            p1Score++;
                            pipes[i].passedP1 = true;
                        }
                    }

                    if (twoPlayer && p2Alive && pipes[i].x == bird2X)
                    {
                        int b2Y = (int)(bird2Y);
                        if (b2Y < currentGapY || b2Y >= currentGapY + pipes[i].gapSize)
                        {
                            p2Alive = false;
                        }
                        else if (!pipes[i].passedP2)
                        {
                            p2Score++;
                            pipes[i].passedP2 = true;
                        }
                    }
                }

                if (!pipes.empty() && pipes[0].x < 0)
                {
                    pipes.erase(pipes.begin());
                }
            }

            char screen[HEIGHT][WIDTH + 1];

            for (int y = 0; y < HEIGHT; y++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    if (y == HEIGHT - 1)
                        screen[y][x] = '=';
                    else
                        screen[y][x] = ' ';
                }
                screen[y][WIDTH] = '\0';
            }

            for (size_t i = 0; i < pipes.size(); i++)
            {
                if (pipes[i].x >= 0 && pipes[i].x < WIDTH)
                {
                    int gY = (int)(pipes[i].gapY);
                    for (int y = 0; y < HEIGHT - 1; y++)
                    {
                        if (y < gY || y >= gY + pipes[i].gapSize)
                        {
                            screen[y][pipes[i].x] = pipes[i].isMoving ? 'M' : '#';
                        }
                    }
                }
            }

            int b1Y = (int)(bird1Y);
            int b2Y = (int)(bird2Y);

            if (p1Alive && b1Y >= 0 && b1Y < HEIGHT - 1)
                screen[b1Y][bird1X] = '1';

            if (twoPlayer && p2Alive && b2Y >= 0 && b2Y < HEIGHT - 1)
            {
                if (p1Alive && b1Y == b2Y)
                    screen[b2Y][bird2X] = 'X';
                else
                    screen[b2Y][bird2X] = '2';
            }

            gotoxy(0, 0);
            int currentColor = -1;

            for (int y = 0; y < HEIGHT; y++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    char ch = screen[y][x];
                    int targetColor = 7;

                    if (ch == '1')
                        targetColor = 14;
                    else if (ch == '2')
                        targetColor = 13;
                    else if (ch == 'X')
                        targetColor = 12;
                    else if (ch == '#')
                        targetColor = 10;
                    else if (ch == 'M')
                        targetColor = 12;
                    else if (ch == '=')
                        targetColor = 6;

                    if (targetColor != currentColor)
                    {
                        setColor(targetColor);
                        currentColor = targetColor;
                    }
                    cout << (ch == 'M' ? '#' : ch);
                }
                cout << "\n";
            }

            gotoxy(0, HEIGHT);
            if (!twoPlayer)
            {
                setColor(14);
                cout << "Player 1 Score: " << p1Score << "    ";
                setColor(11);
                if (!gameStarted)
                    cout << "[Press JUMP to Start!]          \n";
                else
                    cout << "[ESC / Q: Quit]                 \n";
            }
            else
            {
                setColor(14);
                cout << "P1: " << p1Score << (p1Alive ? " (ALIVE) " : " (DEAD)  ");
                setColor(7);
                cout << "| ";
                setColor(13);
                cout << "P2: " << p2Score << (p2Alive ? " (ALIVE) " : " (DEAD)  ");
                setColor(11);
                if (!gameStarted)
                    cout << " [Press JUMP to Start!]\n";
                else
                    cout << " [ESC: Quit]           \n";
            }

            Sleep(sleepTime);
        }

        gotoxy(0, HEIGHT + 2);
        setColor(11);
        cout << "========================================\n";
        cout << "               GAME OVER!               \n";
        cout << "========================================\n";

        setColor(15);
        if (!twoPlayer)
        {
            cout << "  Final Score: " << p1Score << "\n\n";
        }
        else
        {
            cout << "  Scores -> P1: " << p1Score << "  |  P2: " << p2Score << "\n\n";
            if (p1Score > p2Score)
            {
                setColor(14);
                cout << "  >>> WINNER: PLAYER 1! <<<\n\n";
            }
            else if (p2Score > p1Score)
            {
                setColor(13);
                cout << "  >>> WINNER: PLAYER 2! <<<\n\n";
            }
            else
            {
                setColor(10);
                cout << "  >>> IT'S A TIE! <<<\n\n";
            }
        }

        setColor(14);
        cout << "  [R] Play Again    [M] Main Menu    [Q] Quit\n";
        setColor(15);
        cout << "  Select option: " << flush;

        clearInputBuffer();
        char postChoice = _getch();
        if (postChoice == 'q' || postChoice == 'Q')
        {
            setColor(7);
            cout << "\nGoodbye!\n";
            return 0;
        }
    }

    return 0;
}
