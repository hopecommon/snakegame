#include <string>
#include <iostream>
#include <cmath>

// 用于时间控制
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

#include "game.h"

// 构造函数
Game::Game() : font(nullptr), mScreenWidth(WINDOW_WIDTH), mScreenHeight(WINDOW_HEIGHT) // 设置窗口高度
{
    // 初始化 SDL
    if (!initSDL())
    {
        std::cerr << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL 初始化失败");
    }

    // 加载字体
    font = TTF_OpenFont("arial.ttf", 20);
    if (font == nullptr)
    {
        std::cerr << "字体加载失败: " << TTF_GetError() << std::endl;
        // 清理 SDL
        closeSDL();
        throw std::runtime_error("字体加载失败");
    }
    // 加载音乐
    mBackgroundMusic = Mix_LoadMUS("bgm.mp3");
    if (mBackgroundMusic == nullptr)
    {
        std::cerr << "背景音乐加载失败: " << Mix_GetError() << std::endl;
        // 处理错误
        closeSDL();
        throw std::runtime_error("音乐加载失败");
    }
    // 计算游戏区域大小
    mGameBoardWidth = mScreenWidth - mInstructionWidth;
    mGameBoardHeight = mScreenHeight - mInformationHeight;

    // 初始化排行榜
    mLeaderBoard.assign(mNumLeaders, 0);
}

// 析构函数
Game::~Game()
{
    closeSDL();
}
bool Game::initSDL()
{
    //  初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        return false;
    }

    //  初始化 SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf 初始化失败: " << TTF_GetError() << std::endl;
        SDL_Quit(); //  如果 SDL_ttf 初始化失败，则需要关闭 SDL
        return false;
    }
    // 初始化 SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer 初始化失败: " << Mix_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    //  创建窗口
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              mScreenWidth, mScreenHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        TTF_Quit(); //  如果窗口创建失败，则需要关闭 SDL_ttf
        SDL_Quit(); //  如果窗口创建失败，则需要关闭 SDL
        return false;
    }

    //  创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "渲染器创建失败: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

// 关闭 SDL
void Game::closeSDL()
{

    // 释放字体资源
    if (font != nullptr)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }

    // 销毁渲染器
    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // 销毁窗口
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // 退出 SDL_ttf
    TTF_Quit();
    // Stop the music
    Mix_HaltMusic();

    // Free the music
    Mix_FreeMusic(mBackgroundMusic);
    mBackgroundMusic = nullptr;

    // Quit SDL_mixer
    Mix_Quit();
    // 退出 SDL
    SDL_Quit();
}
// 函数用于渲染文字
void Game::renderText(const std::string &text, int x, int y, SDL_Color color) const
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr)
    {
        std::cerr << "Failed to create text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
    {
        std::cerr << "Failed to create text texture! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
// 渲染游戏区域
void Game::renderGameBoard() const
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // 设置边框颜色 (白色)

    // 绘制上边框
    SDL_RenderDrawLine(renderer, 0, 0, mGameBoardWidth, 0);
    // 绘制下边框
    SDL_RenderDrawLine(renderer, 0, mGameBoardHeight - 1, mGameBoardWidth, mGameBoardHeight - 1);
    // 绘制左边框
    SDL_RenderDrawLine(renderer, 0, 0, 0, mGameBoardHeight);
    // 绘制右边框
    SDL_RenderDrawLine(renderer, mGameBoardWidth - 1, 0, mGameBoardWidth - 1, mGameBoardHeight);
}
// 渲染信息面板
void Game::renderInformationBoard() const
{

    // 设置文字颜色 (例如，白色)
    SDL_Color textColor = {255, 255, 255, 255};
    // 使用百分比计算文本位置
    int x = 0.05 * mScreenWidth;                          // 距离游戏区域左侧 5% 的位置
    int y = mGameBoardHeight + 0.05 * mInformationHeight; // 距离屏幕顶部 5% 的位置
    // 渲染文字
    renderText("Welcome to The Snake Game!", x, y, textColor);
}
// 渲染指令面板
void Game::renderInstructionBoard() const
{
    SDL_Color textColor = {255, 255, 255, 255};

    // 使用百分比计算文本位置
    int x = mGameBoardWidth + 0.05 * mScreenWidth; // 距离游戏区域右侧 5% 的位置
    int y = 0.05 * mScreenHeight;                  // 距离屏幕顶部 5% 的位置
    int ySpacing = 0.04 * mScreenHeight;           // 行间距为屏幕高度的 5%

    renderText("Manual", x, y, textColor);
    y += ySpacing;

    renderText("Up / W", x, y, textColor);
    y += ySpacing;

    renderText("Down / S", x, y, textColor);
    y += ySpacing;

    renderText("Left / A", x, y, textColor);
    y += ySpacing;

    renderText("Right / D", x, y, textColor);
    y += ySpacing;
    renderText("Pause: Space", x, y, textColor);
    y += ySpacing;
}
//  辅助函数：获取文字宽度
int Game::getTextWidth(const std::string &text) const
{
    int w;
    TTF_SizeText(font, text.c_str(), &w, nullptr);
    return w;
}
// 渲染排行榜
void Game::renderLeaderBoard() const
{
    SDL_Color textColor = {255, 255, 255, 255};

    // 使用百分比计算文本位置
    int x = mGameBoardWidth + 0.05 * mScreenWidth; // 距离游戏区域右侧 5% 的位置
    int y = 0.4 * mScreenHeight;                   // 距离屏幕顶部 40% 的位置
    int ySpacing = 0.04 * mScreenHeight;           // 行间距为屏幕高度的 5%

    renderText("Leader Board", x, y, textColor);
    y += ySpacing;

    // 渲染排行榜数据
    for (int i = 0; i < mNumLeaders; i++)
    {
        std::string rankText = "#" + std::to_string(i + 1) + ": " + std::to_string(mLeaderBoard[i]);
        renderText(rankText, x, y, textColor);
        y += ySpacing;
    }
}
// 处理开始菜单按键事件
void Game::handleStartMenuEvents(const SDL_Event &e)
{
    switch (e.key.keysym.sym)
    {
    case SDLK_UP:
        selectedOption = (selectedOption - 1 + 4) % 4; // 循环选择选项，包括 "Start Game"
        break;
    case SDLK_DOWN:
        selectedOption = (selectedOption + 1) % 4; // 循环选择选项，包括 "Start Game"
        break;
    case SDLK_LEFT:
        switch (selectedOption)
        {
        case 0: // 游戏模式
            gameMode = GameMode::Bounded;
            break;
        case 1: // 游戏难度
            difficulty = Difficulty::Easy;
            break;
        case 2:                       // 地图类型
            mapType = MapType::Empty; //  直接设置为 Empty
            break;
        }
        break;
    case SDLK_RIGHT:
        switch (selectedOption)
        {
        case 0: // 游戏模式
            gameMode = GameMode::Unbounded;
            break;
        case 1: // 游戏难度
            difficulty = Difficulty::Hard;
            break;
        case 2:                           // 地图类型
            mapType = MapType::Obstacles; //  直接设置为 Obstacles
            break;
        }
        break;
    case SDLK_RETURN:
        if (selectedOption == 3)
        { // 开始游戏
            isStartMenu = false;
            initializeGame(); // 在开始游戏时才初始化游戏
        }
        break;
    }
    renderStartMenu(); // 重新渲染菜单界面
}

void Game::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDLK_ESCAPE:
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (isStartMenu)
            {
                handleStartMenuEvents(e); // 处理开始菜单按键事件
            }
            else
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_w:
                    addDirectionToQueue(Direction::Up);
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    addDirectionToQueue(Direction::Down);
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    addDirectionToQueue(Direction::Left);
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    addDirectionToQueue(Direction::Right);
                    break;
                case SDLK_SPACE:
                    togglePause();
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}
//  获取游戏模式字符串
std::string Game::getGameModeString(GameMode mode) const
{
    switch (mode)
    {
    case GameMode::Bounded:
        return "Bounded";
    case GameMode::Unbounded:
        return "Unbounded";
    default:
        return "Unknown";
    }
}

//  获取游戏难度字符串
std::string Game::getDifficultyString(Difficulty diff) const
{
    switch (diff)
    {
    case Difficulty::Easy:
        return "Easy";
    case Difficulty::Hard:
        return "Hard";
    default:
        return "Unknown";
    }
}

//  获取地图类型字符串
std::string Game::getMapTypeString(MapType type) const
{
    switch (type)
    {
    case MapType::Empty:
        return "Empty";
    case MapType::Obstacles:
        return "Obstacles"; //  修改地图类型名称
    default:
        return "Unknown";
    }
}

// 渲染居中显示的文本
void Game::renderCenteredText(const std::string &text, float xPercent, float yPercent, SDL_Color color) const
{
    int x = static_cast<int>(mScreenWidth * xPercent);  //  将百分比转换为像素坐标
    int y = static_cast<int>(mScreenHeight * yPercent); //  将百分比转换为像素坐标

    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr)
    {
        std::cerr << "无法创建文本曲面: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
    {
        std::cerr << "无法创建文本纹理: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dstRect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h}; // 居中
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
// 颜色定义
SDL_Color textColor = {255, 255, 255, 255};      // 白色
SDL_Color selectedColor = {0, 255, 0, 255};      // 绿色
SDL_Color highlightColor = {255, 255, 0, 255};   // 黄色
SDL_Color selectedRowBgColor = {0, 100, 0, 255}; // 深绿色
void Game::renderStartMenu()
{
    // 1. 渲染背景
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // 设置背景颜色 (黑色)
    SDL_RenderClear(renderer);                                // 清空渲染器

    // 2. 渲染标题
    renderCenteredText("Snake Game", 0.5f, 0.25f, textColor); // 水平垂直居中

    // 3. 渲染菜单选项
    float optionY = 0.5f;         // 第一个选项的纵坐标 (屏幕高度的 50%)
    float optionSpacing = 0.075f; // 选项间距 (屏幕高度的 7.5%)

    // 游戏模式
    renderText("Game Mode:", 0.25f * mScreenWidth, optionY * mScreenHeight, textColor);
    renderStartMenuOption("Bounded", 0.5f, optionY, gameMode == GameMode::Bounded, selectedOption == 0);
    renderStartMenuOption("Unbounded", 0.75f, optionY, gameMode == GameMode::Unbounded, selectedOption == 0);
    optionY += optionSpacing;

    // 游戏难度
    renderText("Difficulty:", 0.25f * mScreenWidth, optionY * mScreenHeight, textColor);
    renderStartMenuOption("Easy", 0.5f, optionY, difficulty == Difficulty::Easy, selectedOption == 1);
    renderStartMenuOption("Hard", 0.75f, optionY, difficulty == Difficulty::Hard, selectedOption == 1);
    optionY += optionSpacing;

    // 地图类型
    renderText("Map Type:", 0.25f * mScreenWidth, optionY * mScreenHeight, textColor);
    renderStartMenuOption("Empty", 0.5f, optionY, mapType == MapType::Empty, selectedOption == 2);
    renderStartMenuOption("Obstacles", 0.75f, optionY, mapType == MapType::Obstacles, selectedOption == 2);
    optionY += optionSpacing;

    // 开始游戏
    renderCenteredText("Start Game", 0.5f, optionY, (selectedOption == 3) ? highlightColor : textColor);

    // 4. 更新屏幕
    SDL_RenderPresent(renderer);
}

// 渲染单个菜单选项
void Game::renderStartMenuOption(const std::string &text, float xPercent, float yPercent, bool isSelected, bool isCurrent)
{
    int x = static_cast<int>(mScreenWidth * xPercent);
    int y = static_cast<int>(mScreenHeight * yPercent);

    SDL_Color color = (isSelected) ? highlightColor : textColor; //  已选选项为黄色，未选选项为白色

    if (isCurrent)
    { //  当前选中的选项，添加箭头
        renderCenteredText(">>" + text + "<<", xPercent, yPercent, color);
    }
    else
    {
        renderCenteredText(text, xPercent, yPercent, color);
    }
}

// 渲染游戏结束界面，并询问玩家是否重新开始游戏
bool Game::renderRestartMenu()
{
    // 定义菜单选项
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    // 初始化菜单选项索引
    int selectedIndex = 0;

    // 颜色定义
    SDL_Color textColor = {255, 255, 255, 255};    // 白色
    SDL_Color highlightColor = {255, 255, 0, 255}; // 黄色

    // 处理玩家输入
    SDL_Event e;
    bool keepWaiting = true;
    while (keepWaiting)
    {
        // 处理事件
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                isRunning = false;
                return false;
            }
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_w:
                    selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    selectedIndex = (selectedIndex + 1) % menuItems.size();
                    break;
                case SDLK_RETURN: //  使用回车键确认选择
                    keepWaiting = false;
                    isRunning = (selectedIndex == 0); //  选择 "Restart" 则继续游戏
                    return (selectedIndex == 0);
                default:
                    break;
                }
            }
        }

        // 渲染游戏结束界面
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xCC);
        SDL_Rect overlayRect = {0, 0, mScreenWidth, mScreenHeight};
        SDL_RenderFillRect(renderer, &overlayRect);

        // 使用百分比计算文本位置
        int centerX = mScreenWidth / 2;
        int centerY = mScreenHeight / 2;
        int ySpacing = 0.05 * mScreenHeight;

        // 渲染 "Game Over"
        renderText("Game Over", centerX - getTextWidth("Game Over") / 2, centerY - 0.1 * mScreenHeight, textColor);

        // 渲染最终得分
        std::string scoreText = "Your Final Score: " + std::to_string(mPoints);
        renderText(scoreText, centerX - getTextWidth(scoreText) / 2, centerY, textColor);

        // 渲染菜单选项
        for (size_t i = 0; i < menuItems.size(); ++i)
        {
            SDL_Color currentColor = (i == selectedIndex) ? highlightColor : textColor;
            renderText(menuItems[i], centerX - getTextWidth(menuItems[i]) / 2, centerY + 0.1 * mScreenHeight + i * ySpacing, currentColor);
        }

        // 更新屏幕以显示菜单
        SDL_RenderPresent(renderer);

        SDL_Delay(10); // 防止 CPU 占用过高
    }

    return false;
}

// 渲染得分
void Game::renderPoints() const
{
    SDL_Color textColor = {255, 255, 255, 255};
    std::string pointsText = "Points: " + std::to_string(mPoints);

    // 使用百分比计算文本位置
    int x = mGameBoardWidth + 0.05 * mScreenWidth; // 距离游戏区域右侧 5% 的位置
    int y = 0.3 * mScreenHeight;                   // 距离屏幕顶部 15% 的位置

    renderText(pointsText, x, y, textColor);
}

// 渲染难度
void Game::renderDifficulty() const
{
    SDL_Color textColor = {255, 255, 255, 255};
    std::string difficultyText = "Difficulty: " + std::to_string(mDifficulty);

    // 使用百分比计算文本位置
    int x = mGameBoardWidth + 0.05 * mScreenWidth; // 距离游戏区域右侧 5% 的位置
    int y = 0.35 * mScreenHeight;                  // 距离屏幕顶部 10% 的位置

    renderText(difficultyText, x, y, textColor);
}
// 初始化游戏
void Game::initializeGame()
{
    // // 清空障碍物列表
    obstacles.clear();

    // 分配内存创建新的蛇对象
    this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, this->gameMode));
    this->mPtrSnake->initializeSnake(); // 初始化蛇

    // 根据难度设置蛇的初始速度
    switch (difficulty)
    {
    case Difficulty::Easy:
        mPtrSnake->setSpeed(15.0f);
        break;
    case Difficulty::Hard:
        mPtrSnake->setSpeed(30.0f);
        break;
    }
    // 根据地图类型设置障碍物

    switch (mapType)
    {
    case MapType::Empty:
        // obstacles.clear();
        break;
    case MapType::Obstacles:
        //  添加障碍物
        for (int i = 0; i < 5; i++)
        {
            obstacles.push_back(SnakeBody(5, i));
        }
        obstacles.push_back(SnakeBody(10, 15));
        // ... 添加更多障碍物
        break;
    }
    // 初始化游戏得分
    this->mPoints = 0;
    // 在随机位置生成食物
    this->createRamdomFood();
    // 让蛇感知到食物
    this->mPtrSnake->senseFood(this->mFood);
    // 其他初始化操作
    this->mDelay = this->mBaseDelay;
    this->renderDifficulty(); // 渲染难度
    this->renderPoints();     // 渲染得分
    this->renderFood();       // 渲染食物
    renderObstacles();
}

// 创建随机食物
void Game::createRamdomFood()
{
    int foodX, foodY;
    do
    {
        foodX = rand() % (mGameBoardWidth / GRID_SIZE - 2) + 1;
        foodY = rand() % (mGameBoardHeight / GRID_SIZE - 2) + 1;
    } while (mPtrSnake->isPartOfSnake(foodX, foodY));

    mFood = SnakeBody(foodX, foodY);

    // 随机选择食物类型
    int foodType = rand() % 4; //  生成 0 到 3 之间的随机数
    switch (foodType)
    {
    case 0:
        mFood.setFoodType(FoodType::Normal);
        break;
    case 1:
        mFood.setFoodType(FoodType::SpeedUp);
        break;
    case 2:
        mFood.setFoodType(FoodType::SlowDown);
        break;
    case 3:
        mFood.setFoodType(FoodType::DoublePoints);
        break;
    }
}
void Game::renderObstacles() const
{
    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF); // 设置障碍物颜色 (灰色)
    for (const auto &obstacle : obstacles)
    {
        SDL_Rect obstacleRect = {
            obstacle.getX() * GRID_SIZE,
            obstacle.getY() * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE};
        SDL_RenderFillRect(renderer, &obstacleRect);
    }
}
// 渲染食物
void Game::renderFood() const
{
    SDL_Rect foodRect = {
        mFood.getX() * GRID_SIZE,
        mFood.getY() * GRID_SIZE,
        GRID_SIZE,
        GRID_SIZE};

    // 根据食物类型设置颜色
    switch (mFood.getFoodType())
    {
    case FoodType::Normal:
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); //  红色
        break;
    case FoodType::SpeedUp:
        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // 天蓝色
        break;
    case FoodType::SlowDown:
        SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255); // 亮紫色
        break;
    case FoodType::DoublePoints:
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF); //  黄色
        break;
    }

    SDL_RenderFillRect(renderer, &foodRect);
}
// 渲染蛇
void Game::renderSnake() const
{
    // 获取蛇身信息
    const std::vector<SnakeBody> &snake = mPtrSnake->getSnake();

    // 使用常量 GRID_SIZE 渲染蛇身
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF); // 绿色
    for (const auto &snakePart : snake)
    {
        SDL_Rect snakePartRect = {
            snakePart.getX() * GRID_SIZE,
            snakePart.getY() * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE};
        SDL_RenderFillRect(renderer, &snakePartRect);
    }
}

void Game::addDirectionToQueue(Direction newDirection)
{
    if (mDirectionQueue.size() < MAX_QUEUE_SIZE && isValidDirection(newDirection))
    {
        mDirectionQueue.push(newDirection);
    }
}

bool Game::isValidDirection(Direction newDirection)
{
    if (mDirectionQueue.empty())
    {
        return newDirection != getOppositeDirection(mCurrentDirection);
    }
    return newDirection != getOppositeDirection(mDirectionQueue.back());
}

Direction Game::getOppositeDirection(Direction dir)
{
    switch (dir)
    {
    case Direction::Up:
        return Direction::Down;
    case Direction::Down:
        return Direction::Up;
    case Direction::Left:
        return Direction::Right;
    case Direction::Right:
        return Direction::Left;
    default:
        return Direction::None;
    }
}

void Game::togglePause()
{
    if (mPtrSnake->getDirection() == Direction::None)
    {
        mPtrSnake->changeDirection(mCurrentDirection);
    }
    else
    {
        mCurrentDirection = mPtrSnake->getDirection();
        mPtrSnake->changeDirection(Direction::None);
    }
}

void Game::updateSnakeDirection()
{
    if (!mDirectionQueue.empty())
    {
        Direction newDirection = mDirectionQueue.front();
        mDirectionQueue.pop();

        if (isValidDirection(newDirection))
        {
            mPtrSnake->changeDirection(newDirection);
            mCurrentDirection = newDirection;
        }
    }
}
// 调整游戏延时
void Game::adjustDelay()
{
    mDifficulty = mPoints / 5;
    if (mPoints % 5 == 0)
    {
        mPtrSnake->setSpeed(mPtrSnake->getSpeed() + 0.5f); //  每增加 5 分，蛇的速度增加 0.5
    }
}

// 运行游戏逻辑

void Game::runGame()
{
    // 初始化计时器
    using clock = std::chrono::steady_clock;
    auto lastFrameTime = clock::now();
    auto lastLogicUpdateTime = lastFrameTime;

    // 创建静态元素的纹理
    SDL_Texture *staticElementsTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mScreenWidth, mScreenHeight);

    // 渲染静态元素到纹理
    SDL_SetRenderTarget(renderer, staticElementsTexture);
    renderGameBoard();
    renderInformationBoard();
    renderInstructionBoard();
    renderLeaderBoard();
    SDL_SetRenderTarget(renderer, nullptr);
    // Start playing background music
    if (Mix_PlayMusic(mBackgroundMusic, -1) == -1)
    {
        SDL_Log("Failed to play background music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    // 显示开始菜单
    isStartMenu = true;
    renderStartMenu();
    // 游戏主循环
    while (isRunning)
    {
        // 1. 计算帧时间
        auto currentFrameTime = clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        // 2. 处理键盘输入
        handleEvents();
        // 3. 如果在开始菜单界面，则不进行游戏逻辑更新和渲染
        if (isStartMenu)
        {
            continue; //  直接进入下一轮循环
        }
        // 4. 更新游戏逻辑 (每秒 20 次)
        if (std::chrono::duration<float>(currentFrameTime - lastLogicUpdateTime).count() >= 0.05f)
        {
            lastLogicUpdateTime = currentFrameTime;
            updateSnakeDirection(); // Add this line to update the snake's direction
            mPtrSnake->update(deltaTime);

            // 检查是否需要移动蛇
            if (mPtrSnake->getAccumulatedTime() >= 1.0f / mPtrSnake->getSpeed())
            {
                mPtrSnake->resetAccumulatedTime(); // 重置累积时间
                                                   // 检查蛇是否处于暂停状态
                if (mPtrSnake->getDirection() != Direction::None)
                {
                    // 移动蛇
                    bool moveSuccess = mPtrSnake->moveFoward();

                    // 检查蛇是否吃到了食物
                    if (moveSuccess)
                    {
                        switch (mFood.getFoodType())
                        {
                        case FoodType::Normal:
                            mPoints++;
                            break;
                        case FoodType::SpeedUp:
                        {
                            float originalSpeed = mPtrSnake->getSpeed();
                            mPtrSnake->setSpeed(originalSpeed + 5.0f); //  增加速度 5.0f
                            speedUpTimer = 10.0f;                      //  设置加速持续时间为 10 秒
                            speedUpOriginalSpeed = originalSpeed;      //  保存原始速度
                            break;
                        }
                        case FoodType::SlowDown:
                        {
                            float originalSpeed = mPtrSnake->getSpeed();
                            mPtrSnake->setSpeed(originalSpeed * 0.8f); //  降低速度为原来的 0.8 倍
                            slowDownTimer = 10.0f;                     //  设置减速持续时间为 10 秒
                            slowDownOriginalSpeed = originalSpeed;     //  保存原始速度
                            break;
                        }
                        case FoodType::DoublePoints:
                            doublePointsTimer = 10.0f; //  设置得分翻倍持续时间为 10 秒
                            break;
                        }

                        //  如果得分翻倍，则获得 2 分
                        if (doublePointsTimer > 0.0f)
                        {
                            mPoints += 2;
                        }
                        else
                        {
                            mPoints++;
                        }

                        adjustDelay();
                        createRamdomFood();
                        // 蛇感知食物
                        this->mPtrSnake->senseFood(this->mFood);
                    }

                    // 检查蛇是否撞到障碍物
                    if (mPtrSnake->checkCollision())
                    {
                        isRunning = false;
                        break; // 游戏结束
                    }
                    // 检查与障碍物的碰撞
                    for (const auto &obstacle : obstacles)
                    {
                        if (mPtrSnake->getSnakebody()[0] == obstacle)
                        {
                            isRunning = false;
                            break; // 游戏结束
                        }
                    }
                }
            }
        }

        // 5. 渲染游戏画面
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // 设置背景颜色 (黑色)
        SDL_RenderClear(renderer);                                // 清空渲染器

        // 渲染静态元素
        SDL_RenderCopy(renderer, staticElementsTexture, nullptr, nullptr);

        // 只渲染动态元素
        renderObstacles();
        renderSnake();
        renderFood();
        renderPoints();
        renderDifficulty();

        // 6. 更新屏幕
        SDL_RenderPresent(renderer);

        // 7. 控制游戏速度 (目标帧率 30 FPS)
        float targetFrameTime = 1.0f / 30.0f;
        float sleepTime = targetFrameTime - deltaTime;
        if (sleepTime > 0)
        {
            SDL_Delay(static_cast<Uint32>(sleepTime * 1000.0f));
        }
        // 更新加速计时器
        if (speedUpTimer > 0.0f)
        {
            speedUpTimer -= deltaTime;
            if (speedUpTimer <= 0.0f)
            {
                // 加速效果结束，恢复原始速度
                mPtrSnake->setSpeed(speedUpOriginalSpeed);
            }
        }

        // 更新减速计时器
        if (slowDownTimer > 0.0f)
        {
            slowDownTimer -= deltaTime;
            if (slowDownTimer <= 0.0f)
            {
                // 减速效果结束，恢复原始速度
                mPtrSnake->setSpeed(slowDownOriginalSpeed);
            }
        }

        // 更新得分翻倍计时器
        if (doublePointsTimer > 0.0f)
        {
            doublePointsTimer -= deltaTime;
        }
    }

    // 清理资源
    SDL_DestroyTexture(staticElementsTexture);
}
// 开始游戏
void Game::startGame()
{
    while (isRunning)
    {
        // 加载排行榜
        readLeaderBoard();

        // 运行游戏
        runGame();

        // 更新排行榜
        updateLeaderBoard();

        // 保存排行榜
        writeLeaderBoard();

        // 显示重新开始菜单
        if (!renderRestartMenu())
        {
            break; // 退出游戏
        }
    }
}

// 从文件加载排行榜信息
bool Game::readLeaderBoard()
{
    std::fstream fhand(mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }

    int temp;
    int i = 0;
    while (!fhand.eof() && i < mNumLeaders)
    {
        fhand.read(reinterpret_cast<char *>(&temp), sizeof(temp));
        mLeaderBoard[i] = temp;
        i++;
    }
    fhand.close();
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