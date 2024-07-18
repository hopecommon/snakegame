#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"

SnakeBody::SnakeBody()
{
}

SnakeBody::SnakeBody(int x, int y) : mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator==(const SnakeBody &snakeBody)
{
    // TODO overload the == operator for SnakeBody comparision.
    return (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY());
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength) : mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
    // TODO check if a given point with axis x, y is on the body of the snake.
    SnakeBody point = SnakeBody(x, y);
    for (int i = 0; i < this->mSnake.size(); i++)
    {
        if (point == this->mSnake[i])
            return true;
    }
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
    // TODO check if the snake has hit the wall
    int headx = this->mSnake[0].getX();
    int heady = this->mSnake[0].getY();
    if (headx <= 0 || headx >= this->mGameBoardWidth - 1)
        return true;
    if (heady <= 0 || heady >= this->mGameBoardHeight - 1)
        return true;
    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
    // TODO check if the snake has hit itself.
    for (int i = 1; i < this->mSnake.size(); i++)
    {
        if (this->mSnake[0] == this->mSnake[i])
            return true;
    }
    return false;
}

bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Snake::senseFood(SnakeBody food)
{
    this->mFood = food;
}

std::vector<SnakeBody> &Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)
{
    switch (this->mDirection)
    {
    case Direction::Up:
    {
        // what you need to do when the current direction of the snake is Up
        // and the user inputs a new direction?  TODO
        if (newDirection == Direction::Left || newDirection == Direction::Right)
        {
            this->mDirection = newDirection;
            return true;
        }
        else
            return false;
    }
    case Direction::Down:
    {
        // what you need to do when the current direction of the snake is Down
        // and the user inputs a new direction? TODO
        if (newDirection == Direction::Left || newDirection == Direction::Right)
        {
            this->mDirection = newDirection;
            return true;
        }
        else
            return false;
    }
    case Direction::Left:
    {
        // what you need to do when the current direction of the snake is Left
        // and the user inputs a new direction? TODO
        if (newDirection == Direction::Up || newDirection == Direction::Down)
        {
            this->mDirection = newDirection;
            return true;
        }
        else
            return false;
    }
    case Direction::Right:
    {
        // what you need to do when the current direction of the snake is Right
        // and the user inputs a new direction? TODO
        if (newDirection == Direction::Up || newDirection == Direction::Down)
        {
            this->mDirection = newDirection;
            return true;
        }
        else
            return false;
    }
    }
    return false;
}

SnakeBody Snake::createNewHead()
{
    /* TODO
     * read the position of the current head
     * read the current heading direction
     * add the new head according to the direction
     * return the new snake
     */
    int headx = this->mSnake[0].getX();
    int heady = this->mSnake[0].getY();
    int next_headx, next_heady;

    switch (this->mDirection)
    {
    case Direction::Up:
    {
        next_headx = headx;
        next_heady = heady - 1;
        break;
    }
    case Direction::Down:
    {
        next_headx = headx;
        next_heady = heady + 1;
        break;
    }
    case Direction::Left:
    {
        next_headx = headx - 1;
        next_heady = heady;
        break;
    }
    case Direction::Right:
    {
        next_headx = headx + 1;
        next_heady = heady;
        break;
    }
    }
    SnakeBody newhead = SnakeBody(next_headx, next_heady);
    return newhead;
}

/*
 * If eat food, return true, otherwise return false
 */
bool Snake::moveFoward()
{
    /*
     * TODO
     * move the snake forward.
     * If eat food, return true, otherwise return false
     */
    if (this->touchFood())
    {
        SnakeBody newhead = this->mFood;
        this->mSnake.insert(this->mSnake.begin(), newhead);
        return true;
    }
    else
    {
        this->mSnake.pop_back();
        SnakeBody newhead = this->createNewHead();
        this->mSnake.insert(this->mSnake.begin(), newhead);
        return false;
    }
}

bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Snake::getLength()
{
    return this->mSnake.size();
}
