#pragma once

#include <string.h>
#include <vector>

#include "../../imgui/imgui.h"

#pragma warning (push)
#pragma warning (disable: 26812)

// Using typedef to not having to create custom binary opartions
typedef int EGroup;

// Group flags
enum EGroup_
{
    EGroup_None             = 1 << 0,
    EGroup_Disable          = 1 << 1,   // Effect disables something. Use another group to specify
    EGroup_Teleport         = 1 << 2,   // Effect involves teleporting
    EGroup_Input            = 1 << 3,   // Effect modifies input
    EGroup_Mouse            = 1 << 4,   // Effect modifies mouse
    EGroup_Camera           = 1 << 5,   // Effect modifies camera
    EGroup_GameSpeed        = 1 << 6,   // Effect modifies gamespeed
    EGroup_Gravity          = 1 << 7,   // Effect modifies gravity
    EGroup_Hud              = 1 << 8,   // Effect modifies hud
    EGroup_AI               = 1 << 9,   // Effect modifies AI 
    EGroup_Player           = 1 << 10,  // Effect modifies player 
    EGroup_Checkpoint       = 1 << 11,  // Effect involves checkpoint
    EGroup_Weapon           = 1 << 12,  // Effect modifies weapon
    EGroup_Force            = 1 << 13,  // Effect forces something. The displayname should make it clear
    EGroup_Health           = 1 << 14,  // Effect modifies health
    EGroup_Level            = 1 << 15,  // Effect modifies something in the level
};

static const char* GroupNames[] = {
    "None",
    "Disable",
    "Teleport",
    "Input",
    "Mouse",
    "Camera",
    "GameSpeed",
    "Gravity",
    "Hud",
    "AI",
    "Player",
    "Checkpoint",
    "Weapon",
    "Force",
    "Health",
    "Level",
};

static std::string GetNamesFromGroup(EGroup_ group)
{
    if (group == EGroup_None)
    {
        return GroupNames[0];
    }

    std::vector<std::string> names;
    for (int i = 0; i < IM_ARRAYSIZE(GroupNames); i++)
    {
        if (group & 1 << i)
        {
            names.push_back(GroupNames[i]);
        }
    }

    std::string result;
    for (size_t i = 0; i < names.size(); i++)
    {
        result += names[i];
        if (i < names.size() - 1)
        {
            result += ", ";
        }
    }

    return result;
}

#pragma warning (pop)
