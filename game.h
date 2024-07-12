#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h> //  替换 ncurses.h
#include <string>
#include <vector>
#include <memory>

#include "snake.h"
#include "constants.h"
#include <SDL2/SDL_ttf.h> // 包含 SDL_ttf 头文件

// 游戏类，负责游戏逻辑的运行和控制
class Game
{
public:
  // 构造函数，初始化游戏参数
  Game();
  // 析构函数，释放资源
  ~Game();

  // 初始化 SDL
  bool initSDL();
  // 关闭 SDL
  void closeSDL();

  // 加载排行榜信息
  void loadLeadBoard();
  // 更新排行榜信息
  void updateLeadBoard();
  // 从文件加载排行榜信息
  bool readLeaderBoard();
  // 更新排行榜信息
  bool updateLeaderBoard();
  // 将排行榜信息写入文件
  bool writeLeaderBoard();

  // 初始化游戏
  void initializeGame();
  // 运行游戏逻辑
  void runGame();

  // 创建随机食物
  void createRamdomFood();

  // 开始游戏
  void startGame();
  // 渲染游戏结束界面，并询问玩家是否重新开始游戏
  bool renderRestartMenu();
  // 调整游戏延时
  void adjustDelay();

private:
  // 字体
  TTF_Font *font;
  // 屏幕宽度和高度
  int mScreenWidth;
  int mScreenHeight;
  // 游戏区域宽度和高度
  int mGameBoardWidth;
  int mGameBoardHeight;
  // 信息面板高度
  const int mInformationHeight = 2 * GRID_SIZE;
  // 指令面板宽度
  const int mInstructionWidth = 10 * GRID_SIZE;
  // SDL 窗口和渲染器
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  // 蛇的初始长度
  const int mInitialSnakeLength = 2;
  // 蛇对象指针
  std::unique_ptr<Snake> mPtrSnake;
  // 食物信息
  SnakeBody mFood;
  // 玩家得分
  int mPoints = 0;
  // 游戏难度
  int mDifficulty = 0;
  // 游戏延时的基本值
  int mBaseDelay = 100;
  // 游戏延时
  int mDelay;
  // 排行榜文件路径
  const std::string mRecordBoardFilePath = "record.dat";
  // 排行榜数据
  std::vector<int> mLeaderBoard;
  // 排行榜最大记录数量
  const int mNumLeaders = 3;
  bool isRunning = true;                   //  控制游戏循环的标志变量
  bool keyPressed = false;                 //  记录按键是否已经被按下的标志变量
  Direction lastDirection = Direction::Up; //  记录蛇暂停前的移动方向，默认为 Up
  //  禁止拷贝构造函数和赋值运算符，防止资源重复释放
  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

  // 渲染函数声明
  void renderText(const std::string &text, int x, int y, SDL_Color color) const;
  int getTextWidth(const std::string &text) const;
  void renderGameBoard() const;
  void renderInformationBoard() const;
  void renderInstructionBoard() const;
  void renderLeaderBoard() const;
  void renderFood() const;
  void renderSnake() const;
  void renderPoints() const;
  void renderDifficulty() const;

  // 处理 SDL 事件
  void handleEvents();
};

#endif