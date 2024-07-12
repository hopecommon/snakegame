#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include "snake.h"
using namespace std;

SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
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

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
    return (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY());
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->initializeMap();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeMap()
{
    string filename("map.txt") ;
    string line;

    ifstream input_file(filename);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '" << filename << "'" << endl;
    }

    while (getline(input_file, line))
    {
        int X = 0;
        int Y = 0;
        int index = line.find(':');
        X = stoi(line.substr(0,index));
        Y = stoi(line.substr(index+1));
        this->mMap.push_back(SnakeBody(X,Y));
    }
    input_file.close();
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isOneOfBlocks(int x, int y)
{
    SnakeBody temp = SnakeBody(x, y);
    for (int i = 0; i < this->mMap.size(); i ++)
    {
        if (this->mMap[i] == temp)
        {
            return true;
        }
    }
    return false;
}

bool Snake::isPartOfSnake(int x, int y)
{
    SnakeBody temp = SnakeBody(x, y);
    for (int i = 0; i < this->mSnake.size(); i ++)
    {
        if (this->mSnake[i] == temp)
        {
            return true;
        }
    }
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
    SnakeBody& head = this->mSnake[0];
    int headX = head.getX();
    int headY = head.getY();
    if (headX <= 0 || headX >= this->mGameBoardWidth - 1)
    {
        return true;
    }
    if (headY <= 0 || headY >= this->mGameBoardHeight - 1)
    {
        return true;
    }
    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
    SnakeBody& head = this->mSnake[0];
    // Exclude the snake head
    for (int i = 1; i < this->mSnake.size(); i ++)
    {
        if (this->mSnake[i] == head)
        {
            return true;
        }
    }
    return false;
}

bool Snake::hitBlock()
{
    SnakeBody& head = this->mSnake[0];
    int headX = head.getX();
    int headY = head.getY();
    for (int i = 0; i < this->mMap.size(); i ++)
    {
        if (this->mMap[i] == head)
        {
            return true;
        }
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

std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

std::vector<SnakeBody>&Snake::getMap()
{
    return this->mMap;
}

bool Snake::changeDirection(Direction newDirection)
{
    switch (this->mDirection)
    {
        case Direction::Up:
        {
            if (newDirection == Direction::Left || newDirection == Direction::Right)
            {
                this->mDirection = newDirection;
                return true;
            }
            else
            {
                return false;
            }
        }
        case Direction::Down:
        {
            if (newDirection == Direction::Left || newDirection == Direction::Right)
            {
                this->mDirection = newDirection;
                return true;
            }
            else
            {
                return false;
            }
        }
        case Direction::Left:
        {
            if (newDirection == Direction::Up || newDirection == Direction::Down)
            {
                this->mDirection = newDirection;
                return true;
            }
            else
            {
                return false;
            }
        }
        case Direction::Right:
        {
            if (newDirection == Direction::Up || newDirection == Direction::Down)
            {
                this->mDirection = newDirection;
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}


SnakeBody Snake::createNewHead()
{
    SnakeBody& head = this->mSnake[0];
    int headX = head.getX();
    int headY = head.getY();
    int headXNext;
    int headYNext;

    switch (this->mDirection)
    {
        case Direction::Up:
        {
            headXNext = headX;
            headYNext = headY - 1;
            break;
        }
        case Direction::Down:
        {
            headXNext = headX;
            headYNext = headY + 1;
            break;
        }
        case Direction::Left:
        {
            headXNext = headX - 1;
            headYNext = headY;
            break;
        }
        case Direction::Right:
        {
            headXNext = headX + 1;
            headYNext = headY;
            break;
        }
    }

    SnakeBody newHead = SnakeBody(headXNext, headYNext);

    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
bool Snake::moveFoward()
{
    if (this->touchFood())
    {
        SnakeBody newHead = this->mFood;
        this->mSnake.insert(this->mSnake.begin(), newHead);
        return true;
    }
    else
    {
        this->mSnake.pop_back();
        SnakeBody newHead = this->createNewHead();
        this->mSnake.insert(this->mSnake.begin(), newHead);
        return false;
    }
}

bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf() || this->hitBlock())
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

