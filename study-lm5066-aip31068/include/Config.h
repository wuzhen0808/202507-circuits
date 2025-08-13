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

    class Config : public ConfigItem
    {
    public:
        enum MissionType
        {
            READ_ALL,
            GATE_MASK_IIN_PFET_FAULT,
            CLEAN_PIN_PEAK,
            CLEAR_FAULTS,
        };

    private:
        EnumType<MissionType> missions;
        ConfigItem *startConfigItem;

    public:
        MissionType missionSelect; // -1 means not selected.

    public:
        Config()
        {
            missions.add(MissionType::READ_ALL, "Read-All");
            missions.add(MissionType::GATE_MASK_IIN_PFET_FAULT, "GATE_MASK:IIN_PFET_FAULT");
            missions.add(MissionType::CLEAN_PIN_PEAK, "CLEAN_PIN_PEAK");
            missions.add(MissionType::CLEAR_FAULTS, "CLEAR_FAULTS");

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
                    else if (this_->missionSelect == MissionType::GATE_MASK_IIN_PFET_FAULT)
                    {
                        title.set<String>("mission", "GATE_MASK:IIN/PFET-FAULT");
                    }
                    else if (this_->missionSelect == MissionType::CLEAN_PIN_PEAK)
                    {
                        title.set<String>("mission", "CLEAN_PIN_PEAK");
                    }
                    else if (this_->missionSelect == MissionType::CLEAR_FAULTS)
                    {
                        title.set<String>("mission", "CLEAR_FAULTS");
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
            if (this->missionSelect == MissionType::GATE_MASK_IIN_PFET_FAULT)
            {
                return true;
            }
            else if (this->missionSelect == MissionType::CLEAN_PIN_PEAK)
            {
                return true;
            }
            else if (this->missionSelect == MissionType::READ_ALL)
            {
                return true;
            }
            else if (this->missionSelect == MissionType::CLEAR_FAULTS)
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
