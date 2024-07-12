#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "constants.h"
// 蛇的移动方向枚举
enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
    None = 4,
};

// 蛇身体部位类
class SnakeBody
{
public:
    // 默认构造函数
    SnakeBody();
    // 构造函数，初始化蛇身体部位的坐标
    SnakeBody(int x, int y);
    // 获取蛇身体部位的横坐标
    int getX() const;
    // 获取蛇身体部位的纵坐标
    int getY() const;
    // 重载 == 运算符，用于比较两个蛇身体部位是否相同
    bool operator==(const SnakeBody &snakeBody) const;

private:
    // 蛇身体部位的横坐标
    int mX;
    // 蛇身体部位的纵坐标
    int mY;
};

// 蛇类，负责蛇的逻辑实现
class Snake
{
public:
    // 构造函数，初始化蛇的初始位置和方向
    // Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength);
    // 设置随机数种子
    void setRandomSeed();
    // 初始化蛇
    void initializeSnake();
    // 判断给定坐标点是否在蛇的身体上
    bool isPartOfSnake(int x, int y) const; //  添加 const
    // 让蛇感知到食物的位置
    void senseFood(const SnakeBody &food); //  参数改为 const 引用
    // 判断蛇是否接触到食物
    bool touchFood() const; //  添加 const
    // 判断蛇是否撞到墙壁
    bool hitWall() const; //  添加 const
    // 判断蛇是否撞到自身
    bool hitSelf() const; //  添加 const
    // 检查蛇是否发生碰撞
    bool checkCollision() const; //  添加 const

    // 改变蛇的移动方向
    bool changeDirection(Direction newDirection);
    // 获取蛇的身体部位列表
    const std::vector<SnakeBody> &getSnake() const; //  返回 const 引用
    // 获取蛇的长度
    int getLength() const; //  添加 const
    // 生成蛇头的下一个位置
    SnakeBody createNewHead() const; //  添加 const
    // 移动蛇
    bool moveFoward();
    // 更新蛇的位置 (根据时间)
    void update(float deltaTime);

    // 获取累积时间
    float getAccumulatedTime() const;
    // 获取速度
    float getSpeed() const;
    // 重置累积时间
    void resetAccumulatedTime();
    Direction getDirection() const;
    void setSpeed(float speed);

private:
    // 游戏区域宽度
    const int mGameBoardWidth;
    // 游戏区域高度
    const int mGameBoardHeight;
    // 蛇的初始长度
    const int mInitialSnakeLength;
    // 蛇的当前移动方向
    Direction mDirection;
    // 食物的位置
    SnakeBody mFood;
    // 蛇的身体部位列表
    std::vector<SnakeBody> mSnake;
    // 蛇的移动速度 (每个网格单位/秒)
    float mSpeed = 15.0f;

    // 累积时间 (用于控制蛇的移动)
    float mAccumulatedTime = 0.0f;
};

#endif