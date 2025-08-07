
#include "Board.h"
#include "AIP31068.h"
#include "LM5066.h"
#include "PMBus.h"
#include "a8/util.h"

namespace a9
{
    using a8::util::Lang;
    using a8::util::StringUtil;
    using a9::PMBus;
    uint8_t aipAddress = 0x7C >> 1; //
    uint8_t lmAddress = 0x15;       //

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
        PMBus pmbus;

        // aip31068
        if (IS_FAIL(aip31068.init()))
        {
            Error_Handler();
            return -1;
        }
        aip31068.ln().print("LM5066 Test");

        // lm5066
        LM5066 lm5066(lmAddress, &aip31068, &pmbus);
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
            Buffer<PMBus::Command> readCommands;
            PMBus::getCommands(PMBus::Direction::READ, readCommands);
            Buffer<char> buf; // 读取数据缓冲区

            for (int i = 0; i < readCommands.len(); i++)
            {
                PMBus::Command cmd = readCommands[i];
                char code = (char)cmd.code;

                String codeStr = StringUtil::toHexString(&code, 1);
                aip31068.ln().print("[").print(i).print("]").print(codeStr).print("=");
                buf.clear();
                int len = lm5066.read(cmd, buf);
                if (len > 0)
                {

                    aip31068.print(StringUtil::toHexString(buf.buffer(), len));
                    aip31068.print("(");
                    String fStr;
                    cmd.formater(buf, len, fStr);
                    aip31068.print(fStr);
                    aip31068.print(")");
                }
                else
                {
                    aip31068.print("<RdErr:").print(len).print(">");
                }

                // aip31068.ln();
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