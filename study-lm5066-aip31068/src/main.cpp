
#include "Board.h"
#include "AIP31068.h"
#include "LM5066.h"
#include "PMBusCommand.h"


namespace a9
{

    uint8_t aipAddress = 0x7C >> 1;                 //
    uint8_t lmAddress = 0x15;                       //
    PMBusCommand pmcStatusTemperature(0x7D, 1); // PMBus 命令
    PMBusCommand pmcTemperature(0x8D, 1);       // PMBus 命令
    PMBusCommand pmcVoltage(0x8B, 2);           // PMBus 命令
    PMBusCommand pmcCurrent(0x8C, 2);           // PMBus 命令
    int CMD_LEN = 4;
    PMBusCommand COMMANDS[] = {pmcStatusTemperature, pmcTemperature, pmcVoltage, pmcCurrent};

    int main(void)
    {
        using namespace a9;
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
        aip31068.lnPrint("LM5066 Test");
        LM5066 lm5066(lmAddress, &aip31068);

        if (IS_FAIL(lm5066.init()))
        {
            aip31068.lnPrint("LM5066 init failed");
        }
        else
        {
            aip31068.lnPrint("LM5066 init success");
        }

        bool isReady = lm5066.isReady();

        if (isReady)
        {

            aip31068.lnPrint("LM5066 is ready");

            // 读取温度

            char buf[2];
            for (int i = 0; i < CMD_LEN; i++)
            {
                int error = lm5066.read(COMMANDS[i].code,COMMANDS[i].len, buf);
                if (error == 0)
                {
                    aip31068.lnPrint(COMMANDS[i].code);
                    aip31068.print(":");
                    aip31068.printLn();
                    for (int j = 0; j < COMMANDS[i].len; j++)
                    {
                        aip31068.print(buf[j]);
                    }
                }
                else
                {
                    aip31068.lnPrint("Tempe read failed");
                }
            }
        }
        else
        {
            aip31068.lnPrint("LM5066 is not ready");
        }

        // Main loop
        while (1)
        {
            delay(1000);
        }
    }
}

int main(void){
    return a9::main();
}