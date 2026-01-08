// Tetris.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Tetris.h"
#include "TetrisBlocks.h"
#include <time.h>
#include <array>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

Block* block; //方块父类指针
UINT timer_id; //定时器
int num; //方块的类型
int type; //方块的形态
const int kCellSize = 20; // 每个格子的像素大小
const int kCols = 40; // 水平格子数
const int kRows = 25; // 垂直格子数
int board[kRows + 1][kCols + 2] = { 0 }; // 包含左右和底部边界
int gridX = 0; // 方块在网格中的X位置
int gridY = 0; // 方块在网格中的Y位置

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 创建方块对象的工厂函数
Block* CreateBlock(int kind)
{
    switch (kind)
    {
    case 1: return new Block_O();
    case 2: return new Block_I();
    case 3: return new Block_S();
    case 4: return new Block_Z();
    case 5: return new Block_T();
    case 6: return new Block_J();
    case 7: return new Block_L();
    default: return new Block_O();
    }
}

// 获取方块在网格中的占用单元格
std::array<POINT, 4> GetCells(int blockKind, int shapeType)
{
    std::array<POINT, 4> cells{};
    switch (blockKind)
    {
    case 1: // O
        cells = { POINT{0,0}, POINT{1,0}, POINT{0,1}, POINT{1,1} };
        break;
    case 2: // I
        if (shapeType == 1 || shapeType == 3)
            cells = { POINT{0,0}, POINT{0,1}, POINT{0,2}, POINT{0,3} };
        else
            cells = { POINT{0,0}, POINT{1,0}, POINT{2,0}, POINT{3,0} };
        break;
    case 3: // S
        if (shapeType == 1 || shapeType == 3)
            cells = { POINT{1,0}, POINT{2,0}, POINT{0,1}, POINT{1,1} };
        else
            cells = { POINT{0,0}, POINT{0,1}, POINT{1,1}, POINT{1,2} };
        break;
    case 4: // Z
        if (shapeType == 1 || shapeType == 3)
            cells = { POINT{0,0}, POINT{1,0}, POINT{1,1}, POINT{2,1} };
        else
            cells = { POINT{1,0}, POINT{1,1}, POINT{0,1}, POINT{0,2} };
        break;
    case 5: // T
        if (shapeType == 1)
            cells = { POINT{0,0}, POINT{1,0}, POINT{2,0}, POINT{1,1} };
        else if (shapeType == 2)
            cells = { POINT{0,0}, POINT{0,1}, POINT{0,2}, POINT{1,1} };
        else if (shapeType == 3)
            cells = { POINT{1,0}, POINT{0,1}, POINT{1,1}, POINT{2,1} };
        else
            cells = { POINT{0,1}, POINT{1,0}, POINT{1,1}, POINT{1,2} };
        break;
    case 6: // J
        if (shapeType == 1)
            cells = { POINT{0,2}, POINT{1,0}, POINT{1,1}, POINT{1,2} };
        else if (shapeType == 2)
            cells = { POINT{0,0}, POINT{1,0}, POINT{2,0}, POINT{2,1} };
        else if (shapeType == 3)
            cells = { POINT{0,0}, POINT{0,1}, POINT{0,2}, POINT{1,0} };
        else
            cells = { POINT{0,0}, POINT{0,1}, POINT{1,1}, POINT{2,1} };
        break;
    case 7: // L
        if (shapeType == 1)
            cells = { POINT{0,0}, POINT{0,1}, POINT{0,2}, POINT{1,2} };
        else if (shapeType == 2)
            cells = { POINT{2,0}, POINT{0,1}, POINT{1,1}, POINT{2,1} };
        else if (shapeType == 3)
            cells = { POINT{0,0}, POINT{1,0}, POINT{1,1}, POINT{1,2} };
        else
            cells = { POINT{0,0}, POINT{1,0}, POINT{2,0}, POINT{0,1} };
        break;
    default:
        cells = { POINT{0,0}, POINT{1,0}, POINT{0,1}, POINT{1,1} };
        break;
    }
    return cells;
}

// 检查方块是否可以放置在指定位置
bool canPlace(int gx, int gy, int blockKind, int shapeType)
{
    auto cells = GetCells(blockKind, shapeType);
    for (auto cell : cells)
    {
        int x = gx + cell.x;
        int y = gy + cell.y;
        if (y < 0 || y > kRows || x < 0 || x > kCols + 1)
        {
            return false;
        }
        if (board[y][x])
        {
            return false;
        }
    }
    return true;
}

// 将方块锁定到游戏板上
void LockBlockToBoard()
{
    auto cells = GetCells(num, type);
    for (auto cell : cells)
    {
        board[gridY + cell.y][gridX + cell.x] = 1;
    }
}

// 初始化游戏板
void InitBoard()
{
    for (int r = 0; r <= kRows; ++r)
    {
        for (int c = 0; c <= kCols + 1; ++c)
        {
            if (r == kRows || c == 0 || c == kCols + 1)
            {
                board[r][c] = 1;
            }
            else
            {
                board[r][c] = 0;
            }
        }
    }
}

// 生成新的方块
void SpawnBlock()
{
    if (block)
    {
        delete block;
        block = nullptr;
    }

    num = rand() % 7 + 1;
    type = rand() % 4 + 1;
    block = CreateBlock(num);
    gridX = kCols / 2 - 1;
    gridY = 0;
    block->setPosition(gridX * kCellSize, gridY * kCellSize);
}

// 绘制方块
void DrawBoard(HDC hdc)
{
    HBRUSH hbr = CreateSolidBrush(RGB(80, 80, 80)); // 已停止方块颜色
    HGDIOBJ old = SelectObject(hdc, hbr); // 选择画刷
    for (int r = 0; r < kRows; ++r) // 绘制已停止的方块
    {
        for (int c = 1; c <= kCols; ++c)
        {
            if (board[r][c] == 0) // 空白单元格
                continue;
            int x = c * kCellSize; // 计算像素坐标
            int y = r * kCellSize; 
            Rectangle(hdc, x, y, x + kCellSize + 1, y + kCellSize + 1); // 绘制方块
        }
    }
    SelectObject(hdc, old); // 恢复原画刷
    DeleteObject(hbr); // 删除画刷
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR    lpCmdLine,
                    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TETRIS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_EX_LAYERED | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, 900, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            srand((unsigned)time(NULL));
            InitBoard(); // 初始化游戏板
            SpawnBlock(); // 生成第一个方块
        }
        timer_id = SetTimer(hWnd, 1, 500, NULL);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_LEFT:
            {
                if (canPlace(gridX - 1, gridY, num, type))
                {
                    block->toLeft();
                    gridX -= 1;
                }
            }
            break;
        case VK_RIGHT:
            {
                if (canPlace(gridX + 1, gridY, num, type))
                {
                    block->toRight();
                    gridX += 1;
                }
            }
            break;
        case VK_DOWN:
            {
                if (canPlace(gridX, gridY + 1, num, type))
                {
                    block->toDown();
                    gridY += 1;
                }
            }
            break;
        case VK_UP:
            {
                block->toPause();
            }
            break;
        case VK_RETURN:
            {
                int nextType = block->changeType(type);
                if (canPlace(gridX, gridY, num, nextType))
                {
                    type = nextType;
                }
            }
            break;
        default:
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }
    break;

    // 定时器设置位置和速度
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...

            DrawBoard(hdc);
            DrawBlock(hdc, block, type);

            EndPaint(hWnd, &ps);
        }
        break;

    // 定时消息处理，触发下一帧
    case WM_TIMER:
        {
            if (canPlace(gridX, gridY + 1, num, type))
            {
                block->falldown();
                gridY += 1;
            }
            else
            {
                LockBlockToBoard();
                SpawnBlock();
                if (!canPlace(gridX, gridY, num, type))
                {
                    KillTimer(hWnd, timer_id);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, timer_id);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
