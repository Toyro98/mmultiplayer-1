#pragma once

#include "../effect.h"

class Nothing : public Effect
{
private:
    std::vector<std::string> DisplayNames = {
        "Do Nothing",
        "Teleport To Current Location"
    };

public:
    Nothing(const std::string& name)
    {
        Name = name;
        DurationType = EffectDuration::Short;
    }

    void Start() override 
    {
        DisplayName = DisplayNames[RandomInt(DisplayNames.size())];
    }

    void Tick(const float deltaTime) override {}

    void Render(IDirect3DDevice9* device) override {}

    bool Shutdown() override 
    {
        return true;
    }

    std::string GetType() const override
    {
        return "Nothing";
    }
};

REGISTER_EFFECT(Nothing, "Does Nothing");
