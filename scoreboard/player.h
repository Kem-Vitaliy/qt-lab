#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player();

    void plus();
    void minus();
    void reset();
    int getpoints();

private:
    int points;

};

#endif // PLAYER_H
