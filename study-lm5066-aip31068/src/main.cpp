
#include "Board.h"
#include "AIP31068.h"
#include "LM5066.h"
#include "PMBus.h"
#include "a8/util.h"
#include "Config.h"

namespace a9
{
    using a8::util::Buffer;
    using a8::util::ConfigContext;
    using a8::util::ConfigItem;
    using a8::util::Directory;
    using a8::util::ErrorListener;
    using a8::util::Lang;
    using a8::util::Logger;
    using a8::util::Output;
    using a8::util::Reader;
    using a8::util::Result;
    using a8::util::String;
    using a8::util::StringUtil;

    using a9::PMBus;

    
    void write(String opName, uint8_t cmdCode, Buffer<char> *buf);

    uint8_t aipAddress = 0x7C >> 1; //
    uint8_t lmAddress = 0x15;       //
    float mRsense = 1.0f;           // 1mR

    class ArduinoInput : public Reader
    {

    public:
        ArduinoInput()
        {
            while (Serial.available())
            {
                Serial.read();
            }
        }
        ~ArduinoInput() {}
        int read(char *buf, int bufLen, Result &res) override
        {
            while (!Serial.available())
            {
                delay(10);
            }
            buf[0] = Serial.read();
            return 1;
        }
    };
    class ArduinoOutput : public Output
    {

    public:
        void print(const String msg) override
        {
            // A8_TRACE2(">>ArduinoOutput::print,msg:", msg.text());
            Serial.print(msg.text());
            // A8_TRACE("<<ArduinoOutput::print");
        }
    };
    class EmptyLogger : public Logger
    {
    public:
        EmptyLogger() : Logger("Empty.Logger")
        {
        }
        void log(Level level, const String &msg) override
        {
        }
    };
    AIP31068 aip31068(aipAddress);
    PMBus pmbus;
    LM5066 lm5066(lmAddress, &aip31068, &pmbus);

    void CLEAN_PIN_PEAK()
    {
        write("CLEAN_PIN_PEAK", 0xD6, 0);
    }

    void GATE_MASK_IinPfetFault()
    {
        Buffer<char> buf;
        buf.add(0b00001000);
        write("GATE_MASK:IinPfetFault", 0xD7, &buf);
    }

    void write(String opName, uint8_t cmdCode, Buffer<char> *buf)
    {
        aip31068.ln().print(opName);
        PMBus::Command *cmd = pmbus.getCommand(cmdCode);
        if (cmd == 0)
        {
            aip31068.ln().print("No such command.");
            return;
        }
        int len = lm5066.write(*cmd, buf);
        if (len < 0)
        {
            aip31068.ln().print("Write failed.");
        }
        else
        {
            aip31068.ln().print("Write succ.");
        }
    }

    void readAll()
    {
        //
        Buffer<PMBus::Command *> readCommands;
        pmbus.getCommands(PMBus::Direction::READ, readCommands);
        Buffer<char> buf; // 读取数据缓冲区

        for (int i = 0; i < readCommands.len(); i++)
        {
            PMBus::Command *cmd = readCommands[i];
            char code = (char)cmd->code;

            String codeStr = StringUtil::toHexString(&code, 1);
            aip31068.ln().print("[").print(i).print("]").print(codeStr).print("=");
            buf.clear();
            int len = lm5066.read(*cmd, &buf);
            if (len > 0)
            {

                aip31068.print(StringUtil::toHexString(buf.buffer(), len));
                aip31068.print("(");

                String str;
                cmd->dataType->getAsString(buf, len, str);
                aip31068.print(str);

                aip31068.print(")");
            }
            else
            {
                aip31068.print("<RdErr:").print(len).print(">");
            }

            // aip31068.ln();
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

        // aip31068
        if (IS_FAIL(aip31068.init()))
        {
            Error_Handler();
            return -1;
        }
        aip31068.ln().print("LM5066 Test");

        // lm5066

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
        }
        else
        {
            aip31068.ln().print("LM5066 is not ready");
        }
        Config *fg = new Config();
        Directory<ConfigItem *> *fgDir = new Directory<ConfigItem *>(String() << "Foreground()", 0);
        fg->attach(fgDir);
        Reader *reader = new ArduinoInput();
        Output *out = new ArduinoOutput();
        Logger *logger = new EmptyLogger();
        Result res;
        ConfigContext cc(reader, out, logger, res);
        // Main loop
        while (1)
        {
            fg->enter(cc);
            if (fg->missionSelect == Config::MissionType::READ_ALL)
            {
                readAll();
            }
            else if (fg->missionSelect == Config::MissionType::GATE_MASK_IIN_PFET_FAULT)
            {
                GATE_MASK_IinPfetFault();
            }
            else if (fg->missionSelect == Config::MissionType::CLEAN_PIN_PEAK)
            {
                CLEAN_PIN_PEAK();
            }
            else
            {
                aip31068.ln().print("MissionSelect:").print(fg->missionSelect);
            }
        }
    }
}

int main(void)
{
    return a9::main();
}