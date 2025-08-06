#include "AIP31068.h"
#include "a8/util/StringUtil.h"

namespace a9
{

    using a8::util::Buffer;
    using a8::util::String;
    using a8::util::StringUtil;

    AIP31068::AIP31068(uint8_t address)
    {
        this->lcd = 0;
        this->address = address;
    }

    AIP31068::~AIP31068()
    {
        if (this->lcd != 0)
        {
            delete this->lcd;
            this->lcd = 0;
        }
    }

    int AIP31068::init()
    {
        if (IS_FAIL(Device::init()))
        {
            return -1;
        }

        this->lcd = new LiquidCrystal_AIP31068_I2C(this->address, 16, 2);

        Wire.setSCL(PB8);
        Wire.setSDA(PB9);

        this->lcd->init(); // initialize the lcd
        return OK;
    }

    AIP31068& AIP31068::print(const char* text)
    {   
        //
        Serial.print(text);
        //
        Buffer<String> texts = StringUtil::split(text, '\n');

        String *lastLine = this->lines.get(this->lines.size() - 1, 0);

        if (lastLine == 0)
        {
            lastLine = new String(16);
            this->lines.append(lastLine);
        }

        lastLine->append(texts[0]);

        for (int i = 1; i < texts.len(); i++)
        {
            this->lines.append(new String(texts[i]));
        }

        while (lines.size() > 2)
        {
            String *str = this->lines[0];
            this->lines.remove(0);
            delete str;
        }

        this->lcd->clear();
        int totalRows = this->lines.size();
        int window = 2;
        int fromIdx = totalRows - window;

        for (int i = 0; i < window; i++)
        {
            
            this->lcd->setCursor(0, i);
            int rowIndex = fromIdx + i;
            if(rowIndex < 0 )
            {
                continue; // Skip if the row index is out of bounds
            }

            String *str = this->lines[rowIndex];
            char* line = str->text();
            
            this->lcd->print(line);
        }

        return *this;
        
    }
}