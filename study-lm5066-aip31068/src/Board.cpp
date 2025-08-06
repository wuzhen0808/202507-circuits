#include "Board.h"
#include "Wire.h"

namespace a9
{

    Board::Board()
    {
        // Constructor implementation
    }
    Board::~Board()
    {
        // Destructor implementation
    }

    int Board::init()
    {
        // Initialization code for the board
        HAL_Init();
        // Additional setup can be added here
        SystemClock_Config();
        
        // Gpio initialization
        __HAL_RCC_GPIOB_CLK_ENABLE();
        
        Serial.setTx(PB6);
        Serial.setRx(PB7);
        Serial.begin(9600); // Initialize serial communication at 9600 baud rate

        // SMBus init
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 启用 DWT
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // 启用周期计数器
        DWT->CYCCNT = 0;

        Wire.setSCL(PB8);
        Wire.setSDA(PB9);

        return OK;
    }
}