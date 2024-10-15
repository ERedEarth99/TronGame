#include <Windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

bool running = true;
void* buffer_memory;
int buffer_width = 1280;
int buffer_height = 720;
BITMAPINFO buffer_bitmap_info;

const int WIDTH = 40; // Width of the game grid
const int HEIGHT = 20; // Height of the game grid
const int PIXEL_SIZE = 20; // Size of each cell in pixels
const char EMPTY = ' ';
const char PLAYER = 'P';
const char AI = 'A';
const char WALL = '#';

struct Position {
    int x, y;
};

std::vector<std::vector<char>> grid(WIDTH, std::vector<char>(HEIGHT, EMPTY));
Position playerPos = { WIDTH / 4, HEIGHT / 2 };
Position aiPos = { 3 * WIDTH / 4, HEIGHT / 2 };
Position playerDirection = { 1, 0 }; // Moving right
Position aiDirection = { -1, 0 }; // Moving left
bool gameOver = false;

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY: {
        running = false;
    } break;

    case WM_SIZE: {
        // Handle window resizing if needed
    } break;

    default: {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
}

void InitializeGrid() {
    for (int i = 0; i < WIDTH; i++) {
        grid[i][0] = WALL; // Top wall
        grid[i][HEIGHT - 1] = WALL; // Bottom wall
    }
    for (int i = 0; i < HEIGHT; i++) {
        grid[0][i] = WALL; // Left wall
        grid[WIDTH - 1][i] = WALL; // Right wall
    }
}

void UpdateGrid() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            grid[i][j] = EMPTY;
        }
    }
    grid[playerPos.x][playerPos.y] = PLAYER;
    grid[aiPos.x][aiPos.y] = AI;
}

bool CheckCollision(const Position& pos) {
    return (pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT ||
        grid[pos.x][pos.y] == PLAYER || grid[pos.x][pos.y] == AI);
}

void UpdatePosition(Position& pos, const Position& direction) {
    pos.x += direction.x;
    pos.y += direction.y;
}

void Render(HDC hdc) {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            COLORREF color;
            switch (grid[x][y]) {
            case PLAYER: color = RGB(0, 255, 0); break; // Green for player
            case AI: color = RGB(255, 0, 0); break; // Red for AI
            case WALL: color = RGB(255, 255, 255); break; // White for walls
            default: color = RGB(0, 0, 0); break; // Black for empty
            }
            HBRUSH brush = CreateSolidBrush(color);
            RECT rect = { x * PIXEL_SIZE, y * PIXEL_SIZE, (x + 1) * PIXEL_SIZE, (y + 1) * PIXEL_SIZE };
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
        }
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Create window class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Game Window";
    window_class.lpfnWndProc = window_callback;

    // Register Class
    RegisterClass(&window_class);
    // Create Window
    HWND window = CreateWindow(window_class.lpszClassName, L"Tron Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, buffer_width, buffer_height, 0, 0, hInstance, 0);

    HDC hdc = GetDC(window);

    InitializeGrid();
    std::srand(static_cast<unsigned>(std::time(0))); // Seed random for AI movement

    while (running) {
        // Handle inputs
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if (!gameOver) {
            // Player controls
            if (GetAsyncKeyState('W')) playerDirection = { 0, -1 }; // Up
            if (GetAsyncKeyState('S')) playerDirection = { 0, 1 }; // Down
            if (GetAsyncKeyState('A')) playerDirection = { -1, 0 }; // Left
            if (GetAsyncKeyState('D')) playerDirection = { 1, 0 }; // Right

            UpdatePosition(playerPos, playerDirection);
            if (CheckCollision(playerPos)) {
                gameOver = true;
            }

            // Simple AI movement
            aiDirection = { rand() % 3 - 1, rand() % 3 - 1 }; // Random direction
            UpdatePosition(aiPos, aiDirection);
            if (CheckCollision(aiPos)) {
                gameOver = true;
            }

            UpdateGrid();

            // Render
            Render(hdc);
            Sleep(100); // Control game speed
        }
    }

    ReleaseDC(window, hdc);
    return 0;
}