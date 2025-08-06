
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

    String format(PMBus::DataType type, char *buf, int len)
    {

        switch (type)
        {
        case PMBus::DataType::TEMPERATURE:
            return String((int16_t)(buf[1] << 8 | buf[0]));
        case PMBus::DataType::ASCII:
            return String(buf + 1, (int)buf[0] - 1);
        default:
            return StringUtil::toHexString(buf, len);
        }
    }
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
            Buffer<PMBus::Command> readCommands;
            PMBus::getCommands(PMBus::READ, readCommands);
            char buf[12] = {0}; // 读取数据缓冲区

            for (int i = 0; i < readCommands.len(); i++)
            {
                PMBus::Command cmd = readCommands[i];
                char code = (char)cmd.code;

                String codeStr = StringUtil::toHexString(&code, 1);
                aip31068.ln().print("[").print(i).print("]").print(codeStr).print("=");
                int error = lm5066.read(code, cmd.len, buf);
                if (error > 0)
                {
                    aip31068.print(StringUtil::toHexString(buf, cmd.len))
                        .print("(")
                        .print(format(cmd.type, buf, cmd.len))
                        .print(")");
                }
                else
                {
                    aip31068.print("<RdErr>");
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