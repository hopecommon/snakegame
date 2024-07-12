#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h> //  添加 SDL 头文件
#include "snake.h"
#include "constants.h"
// 蛇身体部位类
SnakeBody::SnakeBody()
{
}

// 蛇身体部位类构造函数，初始化蛇身体部位的坐标
SnakeBody::SnakeBody(int x, int y) : mX(x), mY(y)
{
}

// 获取蛇身体部位的横坐标
int SnakeBody::getX() const
{
    return mX;
}

// 获取蛇身体部位的纵坐标
int SnakeBody::getY() const
{
    return mY;
}

// 重载 == 运算符，用于比较两个蛇身体部位是否相同
bool SnakeBody::operator==(const SnakeBody &snakeBody) const //  添加 const
{
    // 比较两个 SnakeBody 对象的横坐标和纵坐标是否相同
    return (this->mX == snakeBody.mX) && (this->mY == snakeBody.mY);
}

// 蛇类构造函数，初始化蛇的初始位置和方向
Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength)
    : mGameBoardWidth(gameBoardWidth / GRID_SIZE),
      mGameBoardHeight(gameBoardHeight / GRID_SIZE),
      mInitialSnakeLength(initialSnakeLength)
{
    // 初始化蛇
    this->initializeSnake();
    // 设置随机数种子
    this->setRandomSeed();
}

// 设置随机数种子
void Snake::setRandomSeed()
{
    // 使用当前时间作为随机数种子
    std::srand(std::time(nullptr));
}
// 初始化蛇
void Snake::initializeSnake()
{
    // 将蛇初始位置设置在游戏区域的中心
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    // 初始化蛇的身体部位
    this->mSnake.resize(this->mInitialSnakeLength);
    for (int i = 0; i < this->mInitialSnakeLength; i++)
    {
        this->mSnake[i] = SnakeBody(centerX, centerY + i);
    }
    // 设置蛇的初始方向为向上
    this->mDirection = Direction::Up;
}

// 判断给定坐标点是否在蛇的身体上
bool Snake::isPartOfSnake(int x, int y) const //  添加 const
{
    // 遍历蛇的身体部分
    for (const auto &part : mSnake)
    {
        // 如果找到匹配的坐标，返回 true
        if (part.getX() == x && part.getY() == y)
        {
            return true;
        }
    }
    // 如果没有找到匹配的坐标，返回 false
    return false;
}

/*
 *
 * 假设：
 * 只有蛇头会撞到墙壁
 */
// 判断蛇是否撞到墙壁
bool Snake::hitWall() const
{
    // 获取蛇头的坐标
    int headX = mSnake[0].getX();
    int headY = mSnake[0].getY();

    // 检查蛇头是否超出游戏板的边界
    if (headX < 0 || headX >= mGameBoardWidth || headY < 0 || headY >= mGameBoardHeight)
    {
        return true; // 蛇头撞到墙壁
    }

    return false; // 蛇头没有撞到墙壁
}

/*
 * 蛇头与蛇身体重叠
 */
// 判断蛇是否撞到自身
bool Snake::hitSelf() const //  添加 const
{
    // TODO 判断蛇是否撞到自身
    // 获取蛇头的坐标
    int headX = mSnake[0].getX();
    int headY = mSnake[0].getY();
    // 遍历蛇的身体部分（从第二个开始）
    for (size_t i = 1; i < mSnake.size(); ++i)
    {
        // 如果蛇头的坐标与身体的某个部分的坐标重叠，返回 true
        if (headX == mSnake[i].getX() && headY == mSnake[i].getY())
        {
            return true;
        }
    }

    // 如果没有重叠，返回 false
    return false;
}

// 判断蛇是否接触到食物
bool Snake::touchFood() const //  添加 const
{
    // 获取蛇头的下一个位置
    SnakeBody newHead = this->createNewHead();
    // 判断蛇头下一个位置是否与食物重合
    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 让蛇感知到食物的位置
void Snake::senseFood(const SnakeBody &food) //  参数改为 const 引用
{
    this->mFood = food;
}

// 获取蛇的身体部位列表
const std::vector<SnakeBody> &Snake::getSnake() const //  返回 const 引用，添加 const
{
    return this->mSnake;
}

// 改变蛇的移动方向
bool Snake::changeDirection(Direction newDirection)
{
    // 根据当前方向和新的方向判断是否可以改变方向
    switch (this->mDirection)
    {
    case Direction::Up:
    {
        // 如果蛇当前方向为向上，则判断新的方向不能为向下
        if (newDirection != Direction::Down)
        {
            this->mDirection = newDirection;
            return true;
        }
        break;
    }
    case Direction::Down:
    {

        // 如果蛇当前方向为向下，则判断新的方向不能为向上
        if (newDirection != Direction::Up)
        {
            this->mDirection = newDirection;
            return true;
        }
        break;
    }
    case Direction::Left:
    {
        // 如果蛇当前方向为向左，则判断新的方向不能为向右
        if (newDirection != Direction::Right)
        {
            this->mDirection = newDirection;
            return true;
        }
        break;
    }
    case Direction::Right:
    {
        // 如果蛇当前方向为向右，则判断新的方向不能为向左
        if (newDirection != Direction::Left)
        {
            this->mDirection = newDirection;
            return true;
        }
        break;
    }
    case Direction::None:
    {
        this->mDirection = newDirection;
        return true;
        break;
    }
    }

    // 返回false表示未改变方向，因为蛇不能直接转180度
    return false;
}

// 生成蛇头的下一个位置
SnakeBody Snake::createNewHead() const //  添加 const
{
    int headX = mSnake[0].getX();
    int headY = mSnake[0].getY();

    switch (mDirection)
    {
    case Direction::Up:
        headY--; //  每次移动一个网格单位
        break;
    case Direction::Down:
        headY++;
        break;
    case Direction::Left:
        headX--;
        break;
    case Direction::Right:
        headX++;
        break;
    case Direction::None:
        break;
    }

    return SnakeBody(headX, headY);
}
/*
 * 如果吃到食物，返回true，否则返回false
 */
// 移动蛇
bool Snake::moveFoward()
{
    // 获取蛇头的下一个位置
    SnakeBody newHead = this->createNewHead();
    bool eatFood = false;

    if (this->touchFood())
    {
        // 蛇吃到了食物
        eatFood = true;
        // 不删除蛇尾，而是增加蛇头，实现蛇的增长
        this->mSnake.insert(this->mSnake.begin(), newHead);
    }
    else
    {
        // 蛇没有吃到食物
        // 将蛇头插入到蛇的身体部位列表的最前面
        this->mSnake.insert(this->mSnake.begin(), newHead);
        // 删除蛇的尾部，实现蛇的移动
        this->mSnake.pop_back();
    }
    return eatFood;
}

// 检查蛇是否发生碰撞
bool Snake::checkCollision() const //  添加 const
{
    // 如果蛇撞到墙壁或自身，则返回true，否则返回false
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 获取蛇的长度
int Snake::getLength() const //  添加 const
{
    return this->mSnake.size();
}

void Snake::update(float deltaTime)
{
    deltaTime = std::min(deltaTime, 1.0f / 30.0f); // 限制最大帧时间
    mAccumulatedTime += deltaTime;
}
// 获取累积时间
float Snake::getAccumulatedTime() const
{
    return mAccumulatedTime;
}

// 获取速度
float Snake::getSpeed() const
{
    return mSpeed;
}

// 重置累积时间
void Snake::resetAccumulatedTime()
{
    mAccumulatedTime = 0.0f;
}

Direction Snake::getDirection() const
{
    return mDirection;
}
void Snake::setSpeed(float speed)
{
    mSpeed = speed;
}