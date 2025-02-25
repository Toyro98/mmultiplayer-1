#pragma once

#include <string>
#include "imgui/imgui.h"
#include "settings.h"
#include "engine.h"
#include "util.h"

template<typename T>
class Tracker
{
private:
    T Value;
    float TimeHit;
    float ResetAfterSeconds;

public:
    Tracker()
    {
        Value = std::numeric_limits<T>::lowest();
        TimeHit = 0.0f;
        ResetAfterSeconds = 2.75f; // 2.75f Comes from checking how long it took for the original speedometer to reset
    }

    T GetValue()
    {
        return Value;
    }

    void Reset()
    {
        Value = std::numeric_limits<T>::lowest();
        TimeHit = 0.0f;
    }

    void Update(const T current)
    {
        const auto pawn = Engine::GetPlayerPawn();
        if (!pawn)
        {
            return;
        }

        if (pawn->WorldInfo->RealTimeSeconds - TimeHit > ResetAfterSeconds)
        {
            Reset();
        }

        if (current > Value)
        {
            Value = current;
            TimeHit = pawn->WorldInfo->RealTimeSeconds;
        }
    }
};

class Item
{
public:
    short DrawIndex;
    short PositionIndex;
    float Factor;
    float Height;
    float ValueOffset;
    bool IsVisible;
    bool ModifyDisplayedValue;
    bool Multiply;
    bool AddSpaceBelow;
    std::string ItemName;
    std::string Label;
    std::string Format;
    ImVec4 LabelColor;
    ImVec4 ValueColor;

private:
    char LabelBuffer[32];
    char FormatBuffer[32];
    short DefaultPositionIndex;
    short PreviousPositionIndex;
    std::string DefaultLabel;
    std::string DefaultFormat;

public:
    template<typename T> void Draw(T value)
    {
        if (!IsVisible)
        {
            return;
        }

        ImGui::BeginWindow("Speedometer##trainer-speedometer");

        ImGui::TextColored(LabelColor, Label.c_str());
        ImGui::SameLine(ValueOffset);

        if (ModifyDisplayedValue)
        {
            if (Multiply)
            {
                value *= Factor;
            }
            else
            {
                value /= Factor == 0.0f ? 1.0f : Factor;
            }
        }

        ImGui::TextColored(ValueColor, Format.c_str(), value);

        if (AddSpaceBelow)
        {
            ImGui::DummyVertical(Height);
        }

        ImGui::End();
    }

    void Edit(bool& needsToReorderItems);
    void LoadSettings(const size_t index, const bool resetToDefaultPositionIndex = false, const bool resetToPreviousPositionIndex = false);
    void SaveSettings();
    void SetNameAndDefaults(const std::string &name, const std::string &label, int& positionIndex, const std::string &format = "%.2f");
};

class Speedometer
{
public:
    bool Enabled = false;
    bool HideCloseButton = false;
    bool ShowClassic = false;
    bool ShowTopHeightInfo = false;
    bool ShowExtraPlayerInfo = false;
    bool ShowEditWindow = false;
    bool ShowItemEditorWindow = false;
    bool WasEditWindowShown = false;
    bool WasEditItemShown = false;

// ImGui
private:
    // Instead of using ImGuiStyle using 1094 bytes, we create this with the styling we only use
    struct Style
    {
        ImVec4 TitleBg;
        ImVec4 TitleBgActive;
        ImVec4 WindowBg;
        ImVec4 Border;
        float WindowBorderSize;
        float WindowRounding;
    };

    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
    Style Style;

private:
    Item LocationX;
    Item LocationY;
    Item LocationZ;
    Item TopHeight;
    Item Speed;
    Item TopSpeed;
    Item Pitch;
    Item Yaw;
    Item Checkpoint;
    Item MovementState;
    Item Health;
    Item ReactionTime;
    Item LastJumpLocation;
    Item LastJumpLocationDelta;
    Tracker<float> TopHeightTracker;
    Tracker<float> TopSpeedTracker; 
    std::vector<Item*> Items;

public:
    bool HasCheckpoint = false;
    bool CheckpointUpdateTimer = false;
    float CheckpointTime = 0.0f;
    Classes::FVector CheckpointLocation = {};

public:
    void SortItemsOrder(); 
    void SaveWindowSettings(const bool saveItemColors);
    void LoadWindowSettings(const bool loadItemColors);
    void Initialize();
    void OnRender();

private:
    void DrawEditorWindow();
    void DrawItemEditorWindow();
};
