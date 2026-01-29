#include "player.h"

Player::Player()
{
    points = 0;
}

void Player::plus()
{
    points++;
}

void Player::minus()
{
    if(points > 0)
        points--;
}

int Player::getpoints()
{
    return points;
}

void Player::reset()
{
    points = 0;
}
