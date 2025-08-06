
#include "Board.h"
#include "AIP31068.h"
#include "LM5066.h"
#include "PMBusCommand.h"
#include "a8/util.h"

namespace a9
{
    using a8::util::Lang;
    using a8::util::StringUtil;

    uint8_t aipAddress = 0x7C >> 1;             //
    uint8_t lmAddress = 0x15;                   //
    int CMD_LEN = 2;
    PMBusCommand COMMANDS[] = {
        {0x7D, 1}, // STATUS_TEMPERATURE
        {0x8D, 2}, // READ_TEMPERATURE_1
    };

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
        aip31068.ln().print("LM5066 Test");
        LM5066 lm5066(lmAddress, &aip31068);

        if (IS_FAIL(lm5066.init()))
        {
            aip31068.ln().print("LM5066 init failed");
        }
        else
        {
            aip31068.ln().print("LM5066 init success");
        }

        bool isReady = lm5066.isReady();

        if (isReady)
        {

            aip31068.ln().print("LM5066 is ready");

            // 读取温度

            char buf[2];
            for (int i = 0; i < CMD_LEN; i++)
            {
                char code = (char)COMMANDS[i].code;
                String codeStr = StringUtil::toHexString(&code, 1);
                aip31068.ln().print(codeStr).print("=");

                int error = lm5066.read(COMMANDS[i].code, COMMANDS[i].len, buf);
                if (error > 0)
                {
                    aip31068.print(StringUtil::toHexString(buf, COMMANDS[i].len));
                }
                else
                {
                    aip31068.print("<RdErr>");
                }
                //aip31068.ln();
            }
        }
        else
        {
            aip31068.ln().print("LM5066 is not ready");
        }

        // Main loop
        while (1)
        {
            delay(1000);
        }
    }
}

int main(void)
{
    return a9::main();
}