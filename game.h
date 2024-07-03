#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>

#include "snake.h"

// 游戏类，负责游戏逻辑的运行和控制
class Game
{
public:
  // 构造函数，初始化游戏参数
  Game();
  // 析构函数，释放资源
  ~Game();

  // 创建信息面板
  void createInformationBoard();
  // 渲染信息面板
  void renderInformationBoard() const;

  // 创建游戏区域
  void createGameBoard();
  // 渲染游戏区域
  void renderGameBoard() const;

  // 创建指令面板
  void createInstructionBoard();
  // 渲染指令面板
  void renderInstructionBoard() const;

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
  // 渲染排行榜
  void renderLeaderBoard() const;

  // 渲染所有游戏窗口
  void renderBoards() const;

  // 初始化游戏
  void initializeGame();
  // 运行游戏逻辑
  void runGame();
  // 渲染得分
  void renderPoints() const;
  // 渲染难度
  void renderDifficulty() const;

  // 创建随机食物
  void createRamdonFood();
  // 渲染食物
  void renderFood() const;
  // 渲染蛇
  void renderSnake() const;
  // 控制蛇的移动方向
  void controlSnake() const;

  // 开始游戏
  void startGame();
  // 渲染游戏结束界面，并询问玩家是否重新开始游戏
  bool renderRestartMenu() const;
  // 调整游戏延时
  void adjustDelay();

private:
  // 屏幕宽度和高度
  int mScreenWidth;
  int mScreenHeight;
  // 游戏区域宽度和高度
  int mGameBoardWidth;
  int mGameBoardHeight;
  // 信息面板高度
  const int mInformationHeight = 6;
  // 指令面板宽度
  const int mInstructionWidth = 18;
  // 游戏窗口列表
  std::vector<WINDOW *> mWindows;
  // 蛇的初始长度
  const int mInitialSnakeLength = 2;
  // 蛇的符号
  const char mSnakeSymbol = '@';
  // 蛇对象指针
  std::unique_ptr<Snake> mPtrSnake;
  // 食物信息
  SnakeBody mFood;
  // 食物符号
  const char mFoodSymbol = '#';
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
};

#endif