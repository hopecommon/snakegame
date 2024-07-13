#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h> //  替换 ncurses.h
#include <string>
#include <vector>
#include <queue>
#include <memory>

#include "snake.h"
#include "constants.h"
#include <SDL2/SDL_ttf.h> // 包含 SDL_ttf 头文件
#include <SDL2/SDL_mixer.h>

enum class MenuOption
{
  GameMode,
  Difficulty,
  MapType,
  StartGame
};

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
  // 开始菜单
  void renderStartMenu();
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
  void handleStartMenuEvents(const SDL_Event &e);
  void renderStartMenuOption(const std::string &text, float xPercent, float yPercent, bool isSelected, bool isCurrent);
  //  获取游戏模式字符串
  std::string getGameModeString(GameMode mode) const;

  //  获取游戏难度字符串
  std::string getDifficultyString(Difficulty diff) const;

  //  获取地图类型字符串
  std::string getMapTypeString(MapType type) const;
  // 开始游戏
  void startGame();
  // 渲染游戏结束界面，并询问玩家是否重新开始游戏
  bool renderRestartMenu();
  // 调整游戏延时
  void adjustDelay();

private:
  // 字体
  TTF_Font *font;
  // 音乐
  Mix_Music *mBackgroundMusic;
  SDL_Texture *staticElementsTexture;

  GameMode gameMode = GameMode::Bounded;    //  游戏模式，默认为有边界模式
  Difficulty difficulty = Difficulty::Easy; //  游戏难度，默认为简单模式
  MapType mapType = MapType::Empty;         //  地图类型，默认为无障碍地图
  bool isStartMenu = true;                  //  是否在开始菜单界面
  int selectedOption = 0;                   //  当前选中的选项
  std::vector<SnakeBody> obstacles;         //  障碍物列表

  std::queue<Direction> mDirectionQueue;
  const int MAX_QUEUE_SIZE = 3; // Maximum number of buffered inputs
  Direction mCurrentDirection;
  void addDirectionToQueue(Direction newDirection);
  bool isValidDirection(Direction newDirection);
  Direction getOppositeDirection(Direction dir);
  void togglePause();
  void updateSnakeDirection();

  float speedUpTimer = 0.0f;          // 加速计时器
  float speedUpOriginalSpeed = 0.0f;  // 加速前的原始速度
  float slowDownTimer = 0.0f;         // 减速计时器
  float slowDownOriginalSpeed = 0.0f; // 减速前的原始速度
  float doublePointsTimer = 0.0f;     // 得分翻倍计时器
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
  void renderCenteredText(const std::string &text, float xPercent, float yPercent, SDL_Color color) const;
  int getTextWidth(const std::string &text) const;
  void renderGameBoard() const;
  void renderInformationBoard() const;
  void renderInstructionBoard() const;
  void renderLeaderBoard() const;
  void renderFood() const;
  void renderSnake() const;
  void renderPoints() const;
  void renderDifficulty() const;
  void renderObstacles() const;

  // 处理 SDL 事件
  void handleEvents();
};

#endif