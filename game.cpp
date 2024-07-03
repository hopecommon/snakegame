#include <string>
#include <iostream>
#include <cmath>

// 用于终端延时
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

#include "game.h"

// 游戏类，负责游戏逻辑的运行和控制
Game::Game()
{
    // 将屏幕分成三个窗口
    this->mWindows.resize(3);
    // 初始化 ncurses 库
    initscr();
    // 如果没有按键按下，则不等待按键输入
    nodelay(stdscr, true);
    // 打开键盘控制
    keypad(stdscr, true);
    // 关闭字符回显
    noecho();
    // 隐藏光标
    curs_set(0);
    // 获取屏幕和游戏区域参数
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    // 创建信息面板、游戏区域和指令面板
    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // 初始化排行榜为全零
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    // 释放窗口资源
    for (int i = 0; i < this->mWindows.size(); i++)
    {
        delwin(this->mWindows[i]);
    }
    // 关闭 ncurses 库
    endwin();
}

// 创建信息面板
void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    // 创建信息面板窗口
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

// 渲染信息面板
void Game::renderInformationBoard() const
{
    // 在信息面板上输出欢迎语和游戏说明
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Please fill in the blanks to make it work properly!!");
    mvwprintw(this->mWindows[0], 4, 1, "Implemented using C++ and libncurses library.");
    // 刷新信息面板窗口
    wrefresh(this->mWindows[0]);
}

// 创建游戏区域
void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    // 创建游戏区域窗口
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

// 渲染游戏区域
void Game::renderGameBoard() const
{
    // 刷新游戏区域窗口
    wrefresh(this->mWindows[1]);
}

// 创建指令面板
void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    // 创建指令面板窗口
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

// 渲染指令面板
void Game::renderInstructionBoard() const
{
    // 在指令面板上输出游戏操作说明
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 11, 1, "Points");

    // 刷新指令面板窗口
    wrefresh(this->mWindows[2]);
}

// 渲染排行榜
void Game::renderLeaderBoard() const
{
    // 如果没有足够的空间，则跳过渲染排行榜
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 * 2)
    {
        return;
    }
    // 在指令面板上输出排行榜标题
    mvwprintw(this->mWindows[2], 14, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    // 渲染排行榜数据
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 14 - 2); i++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 14 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 14 + (i + 1), 5, pointString.c_str());
    }
    // 刷新指令面板窗口
    wrefresh(this->mWindows[2]);
}

// 渲染游戏结束界面，并询问玩家是否重新开始游戏
bool Game::renderRestartMenu() const
{
    // 创建一个菜单窗口
    WINDOW *menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    // 在菜单窗口上绘制边框
    box(menu, 0, 0);
    // 定义菜单选项
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    // 初始化菜单选项索引
    int index = 0;
    int offset = 4;
    // 在菜单窗口上输出玩家最终得分
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    // 设置当前选项为高亮显示
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    // 输出其他菜单选项
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    // 刷新菜单窗口
    wrefresh(menu);

    // 监听玩家键盘输入，处理菜单选择
    int key;
    while (true)
    {
        key = getch();
        switch (key)
        {
        case 'W':
        case 'w':
        case KEY_UP:
        {
            // 移除上一选项的高亮显示
            mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
            // 调整选项索引
            index--;
            index = (index < 0) ? menuItems.size() - 1 : index;
            // 设置当前选项为高亮显示
            wattron(menu, A_STANDOUT);
            mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
            wattroff(menu, A_STANDOUT);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
            // 移除上一选项的高亮显示
            mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
            // 调整选项索引
            index++;
            index = (index > menuItems.size() - 1) ? 0 : index;
            // 设置当前选项为高亮显示
            wattron(menu, A_STANDOUT);
            mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
            wattroff(menu, A_STANDOUT);
            break;
        }
        }
        // 刷新菜单窗口
        wrefresh(menu);
        // 如果玩家按下空格键或回车键，则退出循环
        if (key == ' ' || key == 10)
        {
            break;
        }
        // 延迟100毫秒，避免菜单响应过于灵敏
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // 释放菜单窗口资源
    delwin(menu);

    // 根据玩家选择的选项返回相应的结果
    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 渲染得分
void Game::renderPoints() const
{
    // 将得分转换为字符串，并输出到指令面板
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    // 刷新指令面板窗口
    wrefresh(this->mWindows[2]);
}

// 渲染难度
void Game::renderDifficulty() const
{
    // 将难度转换为字符串，并输出到指令面板
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 9, 1, difficultyString.c_str());
    // 刷新指令面板窗口
    wrefresh(this->mWindows[2]);
}

// 初始化游戏
void Game::initializeGame()
{
    // 分配内存创建新的蛇对象
    this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));
    this->mPtrSnake->initializeSnake(); // 初始化蛇
    // 初始化游戏得分
    this->mPoints = 0;
    this->renderPoints(); // 渲染得分
    // 在随机位置生成食物
    this->createRamdonFood();
    this->renderFood(); // 渲染食物
    // 让蛇感知到食物
    this->mPtrSnake->senseFood(this->mFood);
    // 其他初始化操作
    this->mDelay = this->mBaseDelay;
    this->renderDifficulty(); // 渲染难度
}

// 创建随机食物
void Game::createRamdonFood()
{
    // TODO 在随机位置生成食物
    // TODO 确保食物不与蛇重叠
    int foodX, foodY;
    // 生成随机食物的横坐标和纵坐标
    do
    {
        // 随机生成食物的坐标
        foodX = rand() % (this->mGameBoardWidth - 2) + 1;
        foodY = rand() % (this->mGameBoardHeight - 2) + 1;
    } while (this->mPtrSnake->isPartOfSnake(foodX, foodY)); // 确保食物不与蛇重叠

    this->mFood = SnakeBody(foodX, foodY);
}

// 渲染食物
void Game::renderFood() const
{
    // 在游戏区域窗口中渲染食物
    mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    // 刷新游戏区域窗口
    wrefresh(this->mWindows[1]);
}

// 渲染蛇
void Game::renderSnake() const
{
    // 获取蛇的长度
    int snakeLength = this->mPtrSnake->getLength();
    // 获取蛇的身体部位列表
    std::vector<SnakeBody> &snake = this->mPtrSnake->getSnake();
    // 渲染蛇的身体部位
    for (int i = 0; i < snakeLength; i++)
    {
        mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    // 刷新游戏区域窗口
    wrefresh(this->mWindows[1]);
}

// 控制蛇的移动方向
void Game::controlSnake() const
{
    int key;
    // 获取用户键盘输入
    key = getch();
    // 根据按键处理蛇的移动方向
    switch (key)
    {
    case 'W':
    case 'w':
    case KEY_UP:
    {
        // 改变蛇的移动方向为向上
        this->mPtrSnake->changeDirection(Direction::Up);
        break;
    }
    case 'S':
    case 's':
    case KEY_DOWN:
    {
        // 改变蛇的移动方向为向下
        this->mPtrSnake->changeDirection(Direction::Down);
        break;
    }
    case 'A':
    case 'a':
    case KEY_LEFT:
    {
        // 改变蛇的移动方向为向左
        this->mPtrSnake->changeDirection(Direction::Left);
        break;
    }
    case 'D':
    case 'd':
    case KEY_RIGHT:
    {
        // 改变蛇的移动方向为向右
        this->mPtrSnake->changeDirection(Direction::Right);
        break;
    }
    default:
    {
        break;
    }
    }
}

// 渲染所有游戏窗口
void Game::renderBoards() const
{
    // 清空所有窗口内容
    for (int i = 0; i < this->mWindows.size(); i++)
    {
        werase(this->mWindows[i]);
    }
    // 渲染信息面板、游戏区域和指令面板
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    // 重新绘制所有窗口边框
    for (int i = 0; i < this->mWindows.size(); i++)
    {
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    // 渲染排行榜
    this->renderLeaderBoard();
}

// 调整游戏延时
void Game::adjustDelay()
{
    // 根据得分调整难度
    this->mDifficulty = this->mPoints / 5;
    // 如果得分是5的倍数，则更新游戏延时
    if (mPoints % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
    }
}

// 运行游戏逻辑
void Game::runGame()
{
    bool moveSuccess;
    int key;
    // 游戏循环
    while (true)
    {
        /* TODO
         * 这是游戏主循环
         * 它一直运行，并执行以下操作：
         * 	1. 处理键盘输入
         * 	2. 清空窗口
         * 	3. 移动蛇
         * 	4. 检查蛇是否吃到了食物
         * 	5. 检查蛇是否撞到障碍物
         * 	6. 根据步骤3和4的结果执行相应的操作
         *   7. 渲染食物和蛇的当前位置
         *   8. 更新游戏状态并刷新窗口
         */
        // 处理键盘输入
        this->controlSnake();

        // 清空游戏区域窗口
        werase(this->mWindows[1]);
        // 移动蛇
        moveSuccess = this->mPtrSnake->moveFoward();
        // 渲染蛇
        this->renderSnake();

        // 检查蛇是否吃到了食物
        if (moveSuccess)
        {
            // 如果蛇吃到了食物，则更新得分并生成新的食物
            this->mPoints++;
            this->renderPoints();
            this->adjustDelay();
            this->createRamdonFood();
            this->renderFood();
        }
        // 检查蛇是否撞到障碍物
        if (this->mPtrSnake->checkCollision())
        {
            // 如果蛇撞到障碍物，则游戏结束
            break;
        }
        // 蛇感知食物
        this->mPtrSnake->senseFood(this->mFood);
        this->renderFood();
        this->renderSnake();
        // 更新游戏状态并刷新窗口
        this->renderPoints();
        this->renderDifficulty();
        wrefresh(this->mWindows[1]);
        // 延时
        std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));

        // 刷新屏幕
        refresh();
    }
}

// 开始游戏
void Game::startGame()
{
    // 刷新屏幕
    refresh();
    bool choice;
    // 游戏主循环
    while (true)
    {
        // 加载排行榜信息
        this->readLeaderBoard();
        // 渲染游戏界面
        this->renderBoards();
        // 初始化游戏
        this->initializeGame();
        // 运行游戏逻辑
        this->runGame();
        // 更新排行榜
        this->updateLeaderBoard();
        // 保存排行榜信息
        this->writeLeaderBoard();
        // 显示游戏结束界面，询问玩家是否重新开始
        choice = this->renderRestartMenu();
        // 如果玩家选择退出游戏，则结束游戏循环
        if (choice == false)
        {
            break;
        }
    }
}

// 从文件加载排行榜信息
bool Game::readLeaderBoard()
{
    // 以二进制读模式打开排行榜文件
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    // 如果打开文件失败，则返回false
    if (!fhand.is_open())
    {
        return false;
    }
    // 临时变量，用于读取排行榜数据
    int temp;
    int i = 0;
    // 读取排行榜文件中的数据，直到文件结束或读取到指定数量的数据
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char *>(&temp), sizeof(temp));
        // 将读取到的数据保存到排行榜数据数组中
        this->mLeaderBoard[i] = temp;
        i++;
    }
    // 关闭文件
    fhand.close();
    // 返回true表示读取成功
    return true;
}

// 更新排行榜信息
bool Game::updateLeaderBoard()
{
    // 初始化更新标志为false
    bool updated = false;
    // 获取玩家当前得分
    int newScore = this->mPoints;
    // 遍历排行榜数据
    for (int i = 0; i < this->mNumLeaders; i++)
    {
        // 如果当前排行榜数据大于或等于玩家得分，则跳过
        if (this->mLeaderBoard[i] >= this->mPoints)
        {
            continue;
        }
        // 将玩家得分插入到排行榜中，并将原有数据向下移动
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        // 设置更新标志为true
        updated = true;
    }
    // 返回更新标志
    return updated;
}

// 将排行榜信息写入文件
bool Game::writeLeaderBoard()
{
    // 以二进制写模式打开排行榜文件，并清空文件内容
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    // 如果打开文件失败，则返回false
    if (!fhand.is_open())
    {
        return false;
    }
    // 将排行榜数据写入文件
    for (int i = 0; i < this->mNumLeaders; i++)
    {
        fhand.write(reinterpret_cast<char *>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));
        ;
    }
    // 关闭文件
    fhand.close();
    // 返回true表示写入成功
    return true;
}