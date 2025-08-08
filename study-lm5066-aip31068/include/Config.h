#pragma once
#include "a8/util.h"
namespace a9
{
    using a8::util::Buffer;
    using a8::util::ConfigContext;
    using a8::util::ConfigItem;
    using a8::util::ConfigItems;
    using a8::util::Directory;
    using a8::util::EnumType;
    using a8::util::String;
    enum MissionType
    {
        READ_ALL,
        DISABLE_OT_LIMIT_FAULT
    };

    
    class Config : public ConfigItem
    {
        
        EnumType<MissionType> missions;
        ConfigItem *startConfigItem;
        
    public:    
        MissionType missionSelect; // -1 means not selected.

    public:
        Config()
        {
            missions.add(MissionType::READ_ALL,"Read-All");
            missions.add(MissionType::DISABLE_OT_LIMIT_FAULT,"Disable-OT-Limit-Fault");
            missionSelect = MissionType::READ_ALL;
        }
        void onAttached() override
        {
            {
                ConfigItem *ci = ConfigItems::addReturn(this, "Start");
                this->startConfigItem = ci;
                ci->onEnter = [](ConfigContext &cc)
                {
                    cc.navigator->stop(1);
                };
                ci->onBuildTitle = [](TitleBuilder &title)
                {
                    Config *this_ = title.configItem->getRoot<Config>();

                    if (this_->missionSelect == MissionType::READ_ALL)
                    {
                        title.set<String>("mission", "Read-All");
                    }
                    else if (this_->missionSelect == MissionType::DISABLE_OT_LIMIT_FAULT)
                    {
                        title.set<String>("mission", "Dis-OT-Limit-Fault");
                    }
                    else
                    {
                        title.set<String>("mission", "<Please-select>");
                    }
                };
            }
            {
                ConfigItem *this_ = (ConfigItem *)this;
                ConfigItems::addSelectInput<MissionType>(this_, String() << "Select-Mission", this->missionSelect, missions.options);
            }
        }

        bool isValid() override
        {
            if (this->missionSelect == MissionType::DISABLE_OT_LIMIT_FAULT)
            {
                return true;
            }
            else if (this->missionSelect == MissionType::READ_ALL)
            {
                return true;
            }
            else
            {
                // no such mission type.
                return false;
            }
        }
        void onLeftFailure(ConfigContext &cc) override
        {
            cc.navigator->to(this->startConfigItem->getDirectory());
        }

        void enter(ConfigContext &cc) override
        {
            ConfigItems::runNav(this->dir, cc);
        }
    };
}
