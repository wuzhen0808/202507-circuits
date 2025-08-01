
#ifndef __BOARD_H_
#define __BOARD_H_
#include "stm32f4xx_hal.h"
#include "Lang.h"

class Board
{
private:


public:
    Board();
    ~Board();
    int init();
};

#endif