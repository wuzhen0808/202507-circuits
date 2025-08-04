
#include "Board.h"
#include "AIP31068.h"
#include "LM5066.h"

uint8_t aipAddress = 0x7C >> 1; //
uint8_t lmAddress = 0x15;       //

int main(void)
{
    Board board;
    if (IS_FAIL(board.init()))
    {
        Error_Handler();
        return -1;
    }

    AIP31068 aip31068(aipAddress);

    if (IS_FAIL(aip31068.init()))
    {
        Error_Handler();
        return -1;
    }
    aip31068.print("LM5066 Test");
    LM5066 lm5066(lmAddress);

    if (IS_FAIL(lm5066.init()))
    {
        aip31068.print("LM5066 init failed");
    }
    else
    {
        aip31068.print("LM5066 init success");
    }

    bool isReady = lm5066.isReady();

    if (isReady)
    {

        aip31068.print("LM5066 is ready");
        int error = lm5066.readTemperature();
        if (error < 0)
        {
            aip31068.print("Tempe read success");
        }
        else
        {
            aip31068.print("Tempe read success");
        }
    }
    else
    {
        aip31068.print("LM5066 is not ready");
    }

    // Main loop
    while (1)
    {
        delay(1000);
    }
}