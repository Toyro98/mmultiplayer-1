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
        if (pawn)
        {
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
    }
};

class Item
{
public:
    int DrawIndex;
    int PositionIndex;
    bool IsVisible;
    bool ModifyDisplayedValue;
    bool Multiply;
    float Factor;
    bool AddSpaceBelow;
    float Height;
    float ValueOffset;

    std::string ItemName;
    std::string Label;
    std::string Format;

    ImVec4 LabelColor;
    ImVec4 ValueColor;

private:
    char LabelBuffer[32];
    char FormatBuffer[32];
    int DefaultPositionIndex;
    int PreviousPositionIndex;
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
    void LoadSettings(size_t index, bool resetToDefaultPositionIndex = false, bool resetToPreviousPositionIndex = false);
    void SaveSettings();
    void SetNameAndDefaults(const std::string name, const std::string label, int& positionIndex, const std::string format = "%.2f");
};

class Speedometer
{
public:
    bool Show = false;
    bool HideCloseButton = false;
    bool ShowEditWindow = false;
    bool ShowItemEditorWindow = false;
    bool WasEditWindowShown = false;
    bool WasEditItemShown = false;
    ImGuiWindowFlags WindowFlags;
    ImGuiStyle Style;
    bool ShowClassic = false;
    bool ShowTopHeightInfo = false;
    bool ShowExtraPlayerInfo = false;

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
    std::vector<Item*> Items;
    Tracker<float> TopHeightTracker;
    Tracker<float> TopSpeedTracker; 

public:
    bool HasCheckpoint = false;
    float CheckpointTime = 0.0f;
    bool CheckpointUpdateTimer = false;
    Classes::FVector CheckpointLocation;

public:
    void SortItemsOrder(); 
    void SaveWindowSettings(bool saveItemColors); 
    void LoadWindowSettings(bool loadItemColors);
    void Initialize();
    void OnRender();

private:
    void Draw();
    void DrawEditorWindow();
    void DrawItemEditorWindow();
};
