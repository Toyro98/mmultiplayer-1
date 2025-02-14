#include "speedometer.h"

#pragma warning (push)
#pragma warning (disable: 4244)

void Item::Edit(bool& needsToReorderItems)
{
    ImGui::Text("Editing \"%s\"", ItemName.c_str());
    ImGui::Separator(5.0f);

    /*
    ImGui::Text("DrawIndex: %d", DrawIndex);
    ImGui::Text("PositionIndex: %d", PositionIndex);
    ImGui::Text("DefaultPositionIndex: %d", DefaultPositionIndex);
    ImGui::Text("PreviousPositionIndex: %d", PreviousPositionIndex);
    ImGui::Separator(5.0f);
    */

    ImGui::Checkbox("IsVisible", &IsVisible);
    ImGui::HelpMarker("If set to true, it will be hidden, otherwise it will be visible");

    // Text
    {
        ImGui::Separator(5.0f);
        if (ImGui::InputText("Label", LabelBuffer, sizeof(Label), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (Label != LabelBuffer)
            {
                bool empty = true;
                for (auto c : std::string(LabelBuffer))
                {
                    if (!isblank(c))
                    {
                        empty = false;
                        break;
                    }
                }

                if (!empty)
                {
                    Label = LabelBuffer;
                }
            }
        }
        ImGui::HelpMarker(("Change the label text to be whatever you want. Default Label: " + DefaultLabel).c_str());

        if (ImGui::InputText("Value Format", FormatBuffer, sizeof(Format), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (Format != FormatBuffer)
            {
                bool empty = true;
                for (auto c : std::string(FormatBuffer))
                {
                    if (!isblank(c))
                    {
                        empty = false;
                        break;
                    }
                }

                if (!empty)
                {
                    Format = FormatBuffer;
                }
            }
        }
        ImGui::HelpMarker(("All format values requires one \"%\". If you're not sure how to change this. Look up \"Format Specifiers\". You can add whatever you want before and after the specifier.\n\nDefault Format: " + DefaultFormat).c_str());
    }

    // Spacing
    {
        ImGui::Separator(5.0f);
        ImGui::Checkbox("Add Space Below", &AddSpaceBelow);

        if (!AddSpaceBelow)
            ImGui::BeginDisabled();

        ImGui::DragFloat("Height", &Height);

        if (!AddSpaceBelow)
            ImGui::EndDisabled();

        ImGui::DragFloat("Value Offset", &ValueOffset);
        ImGui::HelpMarker("Offset from the left side of the speedometer window");
    }

    // Value
    {
        ImGui::Separator(5.0f);
        ImGui::Checkbox("Modify Displayed Value", &ModifyDisplayedValue);

        if (!ModifyDisplayedValue)
            ImGui::BeginDisabled();

        ImGui::Checkbox("Multiply Value", &Multiply);
        ImGui::HelpMarker("If set to true, it will multiply the value with the factor. If set to false, it will divide instead");

        ImGui::InputFloat("Factor", &Factor);

        if (!ModifyDisplayedValue)
            ImGui::EndDisabled();
    }

    // Color
    {
        ImGui::Separator(5.0f);
        ImGui::ColorEdit4("Label Color", &LabelColor.x, ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Value Color", &ValueColor.x, ImGuiColorEditFlags_AlphaPreviewHalf);
    }

    // Buttons
    {
        ImGui::Separator(5.0f);
        if (ImGui::Button("Save##speedometer-edititem-save-button"))
        {
            SaveSettings();
        }
        ImGui::SameLine();

        if (ImGui::Button("Undo##speedometer-edititem-undo-button"))
        {
            LoadSettings(DrawIndex, false, true);
            needsToReorderItems = true;
        }
        ImGui::SameLine();

        if (ImGui::Button("Reset##speedometer-edititem-reset-button"))
        {
            Settings::SetSetting({ "Speedometer", "Item", ItemName }, json::object());

            LoadSettings(DrawIndex, true);
            SaveSettings();
        }
    }
}

void Item::LoadSettings(size_t index, bool resetToDefaultPositionIndex, bool resetToPreviousPositionIndex)
{
    DrawIndex = Settings::GetSetting({ "Speedometer", "Item", ItemName, "DrawIndex" }, index);

    int positionIndex = index;
    if (resetToDefaultPositionIndex)
    {
        positionIndex = DefaultPositionIndex;
    }
    else if (resetToPreviousPositionIndex)
    {
        positionIndex = PreviousPositionIndex;
    }

    PositionIndex = Settings::GetSetting({ "Speedometer", "Item", ItemName, "PositionIndex" }, positionIndex);
    IsVisible = Settings::GetSetting({ "Speedometer", "Item", ItemName, "IsVisible" }, true);
    ModifyDisplayedValue = Settings::GetSetting({ "Speedometer", "Item", ItemName, "ModifyDisplayedValue" }, false);
    Multiply = Settings::GetSetting({ "Speedometer", "Item", ItemName, "Multiply" }, false);
    Factor = Settings::GetSetting({ "Speedometer", "Item", ItemName, "Factor" }, 1.0f);
    AddSpaceBelow = Settings::GetSetting({ "Speedometer", "Item", ItemName, "AddSpaceBelow" }, false);
    Height = Settings::GetSetting({ "Speedometer", "Item", ItemName, "Height" }, 5.0f);
    ValueOffset = Settings::GetSetting({ "Speedometer", "Item", ItemName, "ValueOffset" }, 128.0f);

    Label = Settings::GetSetting({ "Speedometer", "Item", ItemName, "Label" }, DefaultLabel).get<std::string>();
    strncpy_s(LabelBuffer, sizeof(LabelBuffer) - 1, Label.c_str(), sizeof(LabelBuffer) - 1);

    Format = Settings::GetSetting({ "Speedometer", "Item", ItemName, "Format" }, DefaultFormat).get<std::string>();
    strncpy_s(FormatBuffer, sizeof(FormatBuffer) - 1, Format.c_str(), sizeof(FormatBuffer) - 1);

    LabelColor = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Item", ItemName, "LabelColor" }, ImVec4ToJson(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))));
    ValueColor = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Item", ItemName, "ValueColor" }, ImVec4ToJson(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))));
}

void Item::SaveSettings()
{
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "DrawIndex" }, DrawIndex);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "PositionIndex" }, PreviousPositionIndex = PositionIndex);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "IsVisible" }, IsVisible);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "ModifyDisplayedValue" }, ModifyDisplayedValue);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "Multiply" }, Multiply);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "Factor" }, Factor);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "AddSpaceBelow" }, AddSpaceBelow);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "Height" }, Height);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "ValueOffset" }, ValueOffset);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "Label" }, Label);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "Format" }, Format);
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "LabelColor" }, ImVec4ToJson(LabelColor));
    Settings::SetSetting({ "Speedometer", "Item", ItemName, "ValueColor" }, ImVec4ToJson(ValueColor));
}

void Item::SetNameAndDefaults(const std::string name, const std::string label, int& positionIndex, const std::string format)
{
    ItemName = name;
    DefaultLabel = Label = label;
    DefaultFormat = Format = format;
    DefaultPositionIndex = PreviousPositionIndex = PositionIndex = positionIndex;

    positionIndex++;
}

void Speedometer::SortItemsOrder()
{
    std::sort(Items.begin(), Items.end(), [](Item* a, Item* b)
    {
        return a->PositionIndex < b->PositionIndex;
    });
}

void Speedometer::SaveWindowSettings(bool saveItemColors)
{
    Settings::SetSetting({ "Speedometer", "Settings", "Show" }, Show);
    Settings::SetSetting({ "Speedometer", "Settings", "HideCloseButton" }, HideCloseButton);
    Settings::SetSetting({ "Speedometer", "Settings", "WindowFlags" }, WindowFlags);
    Settings::SetSetting({ "Speedometer", "Style", "WindowRounding" }, Style.WindowRounding);
    Settings::SetSetting({ "Speedometer", "Style", "WindowBorderSize" }, Style.WindowBorderSize);
    Settings::SetSetting({ "Speedometer", "Style", "BorderColor" }, ImVec4ToJson(Style.Colors[ImGuiCol_Border]));
    Settings::SetSetting({ "Speedometer", "Style", "TitleBackgroundColor" }, ImVec4ToJson(Style.Colors[ImGuiCol_TitleBg]));
    Settings::SetSetting({ "Speedometer", "Style", "WindowBackgroundColor" }, ImVec4ToJson(Style.Colors[ImGuiCol_WindowBg]));

    for (size_t i = 0; i < Items.size() && saveItemColors; i++)
    {
        Settings::SetSetting({ "Speedometer", "Item", Items[i]->ItemName, "LabelColor" }, ImVec4ToJson(Items[i]->LabelColor));
        Settings::SetSetting({ "Speedometer", "Item", Items[i]->ItemName, "ValueColor" }, ImVec4ToJson(Items[i]->ValueColor));
    }
}

void Speedometer::LoadWindowSettings(bool loadItemColors)
{
    ShowClassic = Settings::GetSetting({ "Speedometer", "Settings", "Classic", "Show" }, false);
    ShowTopHeightInfo = Settings::GetSetting({ "Speedometer", "Settings", "Classic", "ShowTopHeightInfo" }, false);
    ShowExtraPlayerInfo = Settings::GetSetting({ "Speedometer", "Settings", "Classic", "ShowExtraPlayerInfo" }, false);

    Show = Settings::GetSetting({ "Speedometer", "Settings", "Show" }, false);
    HideCloseButton = Settings::GetSetting({ "Speedometer", "Settings", "HideCloseButton" }, false);
    WindowFlags = Settings::GetSetting({ "Speedometer", "Settings", "WindowFlags" }, static_cast<ImGuiWindowFlags>(ImGuiWindowFlags_NoCollapse));
    Style.WindowRounding = Settings::GetSetting({ "Speedometer", "Style", "WindowRounding" }, 5.0f);
    Style.WindowBorderSize = Settings::GetSetting({ "Speedometer", "Style", "WindowBorderSize" }, 0.0f);

    // Default colors from the ImGui's Dark Theme
    Style.Colors[ImGuiCol_Border] = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Style", "BorderColor" }, ImVec4ToJson(ImVec4(0.43f, 0.43f, 0.50f, 0.50f))));
    Style.Colors[ImGuiCol_TitleBg] = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Style", "TitleBackgroundColor" }, ImVec4ToJson(ImVec4(0.16f, 0.29f, 0.48f, 0.7f))));
    Style.Colors[ImGuiCol_TitleBgActive] = Style.Colors[ImGuiCol_TitleBg];
    Style.Colors[ImGuiCol_WindowBg] = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Style", "WindowBackgroundColor" }, ImVec4ToJson(ImVec4(0.0f, 0.0f, 0.0f, 0.5f))));

    for (size_t i = 0; i < Items.size() && loadItemColors; i++)
    {
        Items[i]->LabelColor = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Item", Items[i]->ItemName, "LabelColor" }, ImVec4ToJson(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))));
        Items[i]->ValueColor = JsonToImVec4(Settings::GetSetting({ "Speedometer", "Item", Items[i]->ItemName, "ValueColor" }, ImVec4ToJson(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))));
    }
}

void Speedometer::Initialize()
{
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;

        Items.clear();
        Items.push_back(&LocationX);
        Items.push_back(&LocationY);
        Items.push_back(&LocationZ);
        Items.push_back(&TopHeight);
        Items.push_back(&Speed);
        Items.push_back(&TopSpeed);
        Items.push_back(&Pitch);
        Items.push_back(&Yaw);
        Items.push_back(&Checkpoint);
        Items.push_back(&MovementState);
        Items.push_back(&Health);
        Items.push_back(&ReactionTime);
        Items.push_back(&LastJumpLocation);
        Items.push_back(&LastJumpLocationDelta);

        int index = 0;
        LocationX.SetNameAndDefaults("Location X", "X", index);
        LocationY.SetNameAndDefaults("Location Y", "Y", index);
        LocationZ.SetNameAndDefaults("Location Z", "Z", index);
        TopHeight.SetNameAndDefaults("Top Height", "ZT", index);
        Speed.SetNameAndDefaults("Speed", "V", index);
        TopSpeed.SetNameAndDefaults("Top Speed", "VT", index);
        Pitch.SetNameAndDefaults("Pitch", "RX", index);
        Yaw.SetNameAndDefaults("Yaw", "RZ", index);
        Checkpoint.SetNameAndDefaults("Checkpoint", "T", index);
        MovementState.SetNameAndDefaults("Movement State", "S", index, "%d");
        Health.SetNameAndDefaults("Health", "H", index, "%d");
        ReactionTime.SetNameAndDefaults("Reaction Time", "RT", index);
        LastJumpLocation.SetNameAndDefaults("Last Jump Location", "SZ", index);
        LastJumpLocationDelta.SetNameAndDefaults("Last Jump Location Delta", "SZD", index);
    }

    // Check if there are any speedometer settings before loading from settings
    bool foundSettings = Settings::GetSetting({ "Speedometer", "Item" }, json::object()).is_null();

    for (size_t i = 0; i < Items.size(); i++)
    {
        // Pass the "i" for the DrawIndex default value
        Items[i]->LoadSettings(i, foundSettings);
    }

    // If no settings were found, apply default settings
    if (!foundSettings)
    {
        LocationX.ModifyDisplayedValue = true;
        LocationX.Factor = 100;
        LocationY.ModifyDisplayedValue = true;
        LocationY.Factor = 100;
        LocationZ.ModifyDisplayedValue = true;
        LocationZ.Factor = 100;
        TopHeight.AddSpaceBelow = true;
        TopHeight.ModifyDisplayedValue = true;
        TopHeight.Factor = 100;
        Speed.ModifyDisplayedValue = true;
        Speed.Multiply = true;
        Speed.Factor = 0.036f;
        TopSpeed.AddSpaceBelow = true;
        TopSpeed.ModifyDisplayedValue = true;
        TopSpeed.Multiply = true;
        TopSpeed.Factor = 0.036f;
        Yaw.AddSpaceBelow = true;
        LastJumpLocation.ModifyDisplayedValue = true;
        LastJumpLocation.Factor = 100;
        LastJumpLocationDelta.ModifyDisplayedValue = true;
        LastJumpLocationDelta.Factor = 100;

        for (size_t i = 0; i < Items.size(); i++)
        {
            Items[i]->SaveSettings();
        }
    }

    SortItemsOrder();
}

void Speedometer::OnRender()
{
    if (!Show)
    {
        return;
    }

    if (!ShowClassic)
    {
        if (ShowEditWindow)
        {
            DrawEditorWindow();
        }

        if (ShowItemEditorWindow)
        {
            DrawItemEditorWindow();
        }
    }

    Draw();
}

void Speedometer::Draw()
{
    const auto pawn = Engine::GetPlayerPawn();
    const auto controller = Engine::GetPlayerController();

    if (!pawn || !controller)
    {
        TopSpeedTracker.Reset();
        TopHeightTracker.Reset();
        return;
    }

    float speed = sqrtf(powf(pawn->Velocity.X, 2) + powf(pawn->Velocity.Y, 2));
    float pitch = static_cast<float>(controller->Rotation.Pitch % 0x10000) / static_cast<float>(0x10000) * 360.0f;
    float yaw = static_cast<float>(controller->Rotation.Yaw % 0x10000) / static_cast<float>(0x10000) * 360.0f;

    TopSpeedTracker.Update(speed);
    TopHeightTracker.Update(pawn->Location.Z);

    if (ShowClassic)
    {
        static const float padding = 5.0f;
        static const float rightPadding = 110.0f; 
        
        auto window = ImGui::BeginRawScene("##trainer-speedometer");
        auto& io = ImGui::GetIO();
        float width = io.DisplaySize.x - padding; 
        float yIncrement = ImGui::GetTextLineHeight();
        
        static int amountOfItems = Items.size() - 1;
        static int amountOfExtraPlayerInfoItems = 5;
        int count = (ShowTopHeightInfo ? 1 : 0) + (ShowExtraPlayerInfo ? amountOfItems : amountOfItems - amountOfExtraPlayerInfoItems);
        float y = io.DisplaySize.y - (count * yIncrement) - padding - ((yIncrement / 2) * (amountOfExtraPlayerInfoItems - (ShowExtraPlayerInfo ? 1 : 2)));

        window->DrawList->AddRectFilled(ImVec2(width - rightPadding - padding, y - (padding / 2)), io.DisplaySize, ImColor(ImVec4(0, 0, 0, 0.4f)));

        auto addTextToDrawList = [&](auto value, const char* label, const char* format, float yIncrement)
        {
            char buffer[0x1F]{};
            sprintf_s(buffer, format, value);

            window->DrawList->AddText(ImVec2(width - ImGui::CalcTextSize(buffer, nullptr, false).x, y), IM_COL32_WHITE, buffer);
            window->DrawList->AddText(ImVec2(width - rightPadding, y), IM_COL32_WHITE, label);

            y += yIncrement;
        };

        addTextToDrawList(pawn->Location.X / 100.0f, "X", "%.2f", yIncrement);
        addTextToDrawList(pawn->Location.Y / 100.0f, "Y", "%.2f", yIncrement);
        addTextToDrawList(pawn->Location.Z / 100.0f, "Z", "%.2f", yIncrement + (ShowTopHeightInfo ? 0 : yIncrement / 2));

        if (ShowTopHeightInfo) 
        {
            addTextToDrawList(TopHeightTracker.GetValue() / 100.0f, "ZT", "%.2f", yIncrement + yIncrement / 2);
        }

        addTextToDrawList(speed * 0.036f, "V", "%.2f", yIncrement);
        addTextToDrawList(TopSpeedTracker.GetValue(), "VT", "%.2f", yIncrement + yIncrement / 2);

        pitch = pitch == 0.0f ? pitch : pitch > 180.0f ? pitch - 360.0f + 0.01f : pitch + 0.01f;
        addTextToDrawList(pitch, "RX", "%.2f", yIncrement);
        addTextToDrawList(yaw, "RY", "%.2f", yIncrement + yIncrement / 2);
        addTextToDrawList(CheckpointTime, "T", "%.2f", yIncrement);

        if (ShowExtraPlayerInfo) 
        {
            addTextToDrawList(pawn->MovementState.GetValue(), "S", "%d", yIncrement);
            addTextToDrawList(pawn->Health, "H", "%d", yIncrement);
            addTextToDrawList(min(100.00f, controller->ReactionTimeEnergy), "RT", "%.2f", yIncrement + yIncrement / 2);
            addTextToDrawList(pawn->LastJumpLocation.Z / 100.0f, "SZ", "%.2f", yIncrement);
            addTextToDrawList((pawn->Location.Z - pawn->LastJumpLocation.Z) / 100.0f, "SZD", "%.2f", yIncrement);
        }

        ImGui::EndRawScene();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_TitleBg, Style.Colors[ImGuiCol_TitleBg]);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Style.Colors[ImGuiCol_TitleBg]);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Style.Colors[ImGuiCol_WindowBg]);
        ImGui::PushStyleColor(ImGuiCol_Border, Style.Colors[ImGuiCol_Border]);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, Style.WindowBorderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Style.WindowRounding);

        ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(190, 345), ImGuiCond_FirstUseEver);
        ImGui::BeginWindow("Speedometer##trainer-speedometer", HideCloseButton ? nullptr : &Show, WindowFlags);

        for (size_t i = 0; i < Items.size(); i++)
        {
            switch (Items[i]->DrawIndex)
            {
            case 0: Items[i]->Draw(pawn->Location.X); break;
            case 1: Items[i]->Draw(pawn->Location.Y); break;
            case 2: Items[i]->Draw(pawn->Location.Z); break;
            case 3: Items[i]->Draw(TopHeightTracker.GetValue()); break;
            case 4: Items[i]->Draw(speed); break;
            case 5: Items[i]->Draw(TopSpeedTracker.GetValue()); break;
            case 6: Items[i]->Draw(pitch); break;
            case 7: Items[i]->Draw(yaw); break;
            case 8: Items[i]->Draw(CheckpointTime); break;
            case 9: Items[i]->Draw(static_cast<int>(pawn->MovementState.GetValue())); break;
            case 10: Items[i]->Draw(pawn->Health); break;
            case 11: Items[i]->Draw(min(100.0f, controller->ReactionTimeEnergy)); break;
            case 12: Items[i]->Draw(pawn->LastJumpLocation.Z); break;
            case 13: Items[i]->Draw(pawn->Location.Z - pawn->LastJumpLocation.Z); break;
            default:
                printf("Unknown DrawIndex \"%d\" (Min = 0, Max = %d)\n", Items[i]->DrawIndex, Items.size());
                break;
            }
        }

        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }
}

void Speedometer::DrawEditorWindow()
{
    static float valueOffset = 128.0f;
    static ImVec4 valueColor(1.0f, 1.0f, 1.0f, 1.0f);
    static ImVec4 labelColor(1.0f, 1.0f, 1.0f, 1.0f);

    ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(470, 478), ImGuiCond_FirstUseEver);
    ImGui::BeginWindow("Edit Speedometer Window##speedometer-edit-window", &ShowEditWindow, ImGuiWindowFlags_NoCollapse);
    ImGui::SeparatorText("Flags");
    {
        ImGui::CheckboxFlags("No Title Bar", &WindowFlags, ImGuiWindowFlags_NoTitleBar);
        ImGui::CheckboxFlags("No Resize", &WindowFlags, ImGuiWindowFlags_NoResize);
        ImGui::Checkbox("No Close", &HideCloseButton);
    }

    ImGui::SeparatorText("Variables");
    {
        ImGui::SliderFloat("Rounding", &Style.WindowRounding, 0.0f, 16.0f, "%.2f");
        ImGui::SliderFloat("Border Size", &Style.WindowBorderSize, 0.0f, 2.0f, "%.2f");
    }

    ImGui::SeparatorText("Colors");
    {
        if (ImGui::ColorEdit4("Title Background", &Style.Colors[ImGuiCol_TitleBg].x, ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            Style.Colors[ImGuiCol_TitleBgActive] = Style.Colors[ImGuiCol_TitleBg];
        }
        ImGui::ColorEdit4("Window Background", &Style.Colors[ImGuiCol_WindowBg].x, ImGuiColorEditFlags_AlphaPreviewHalf);
        ImGui::ColorEdit4("Window Border", &Style.Colors[ImGuiCol_Border].x, ImGuiColorEditFlags_AlphaPreviewHalf);
    }

    ImGui::SeparatorText("Overrides");
    {
        if (ImGui::ColorEdit4("Label Color", &labelColor.x, ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                Items[i]->LabelColor = labelColor;
            }
        }

        if (ImGui::ColorEdit4("Value Color", &valueColor.x, ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                Items[i]->ValueColor = valueColor;
            }
        }

        if (ImGui::DragFloat("Value Offset", &valueOffset))
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                Items[i]->ValueOffset = valueOffset;
            }
        }
    }

    ImGui::SeparatorText("Buttons##Speedometer-Edit-Button-Separator");
    {
        if (ImGui::Button("Save##Speedometer-Edit-Save"))
        {
            SaveWindowSettings(true);
        }
        ImGui::SameLine();

        if (ImGui::Button("Undo##Speedometer-Edit-Undo"))
        {
            LoadWindowSettings(true);

            valueOffset = 128.0f;
            valueColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            labelColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        ImGui::SameLine();

        if (ImGui::Button("Reset##Speedometer-Edit-Reset"))
        {
            Settings::SetSetting({ "Speedometer", "Style" }, json::object());

            LoadWindowSettings(false);
            SaveWindowSettings(false);

            valueOffset = 128.0f;
            valueColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            labelColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    ImGui::End();
}

void Speedometer::DrawItemEditorWindow()
{
    static size_t selectedIndex = -1;

    ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(322, 407), ImGuiCond_FirstUseEver);
    ImGui::BeginWindow("Edit Speedometer Items##speedometer-edit-items", &ShowItemEditorWindow, ImGuiWindowFlags_NoCollapse);
    ImGui::SeparatorText("Items");
    {
        int itemIndexSelected = -1;
        int itemIndexToSwapWith = -1;

        // FIXME: The button size doesn't match perfectly and size 17.5 is close enough but would like it to be perfectly sized
        const auto selectableSize = ImVec2(256, 0);
        const auto buttonSize = ImVec2(17.5f, 17.5f);

        for (size_t i = 0; i < Items.size(); i++)
        {
            if (ImGui::Selectable((Items[i]->ItemName + "##speedometer-edit-item-" + std::to_string(i)).c_str(), i == selectedIndex, 0, selectableSize))
            {
                selectedIndex = i == selectedIndex ? -1 : i;
            }

            ImGui::SameLine();
            if (i == 0)
            {
                ImGui::BeginDisabled();
                ImGui::SmallArrowButton("", ImGuiDir_Up, buttonSize);
                ImGui::EndDisabled();
            }
            else if (ImGui::SmallArrowButton(("##speedometer-edit-item-arrow-up-" + std::to_string(i)).c_str(), ImGuiDir_Up, buttonSize))
            {
                itemIndexSelected = i;
                itemIndexToSwapWith = i - 1;
            }

            ImGui::SameLine();
            if (i == Items.size() - 1)
            {
                ImGui::BeginDisabled();
                ImGui::SmallArrowButton("", ImGuiDir_Down, buttonSize);
                ImGui::EndDisabled();
            }
            else if (ImGui::SmallArrowButton(("##speedometer-edit-item-arrow-down-" + std::to_string(i)).c_str(), ImGuiDir_Down, buttonSize))
            {
                itemIndexSelected = i;
                itemIndexToSwapWith = i + 1;
            }
        }

        if (itemIndexSelected != -1)
        {
            std::swap(Items[itemIndexSelected], Items[itemIndexToSwapWith]);

            // We swapped everything but we don't want to swap the position index
            int temp = Items[itemIndexToSwapWith]->PositionIndex;
            Items[itemIndexToSwapWith]->PositionIndex = Items[itemIndexSelected]->PositionIndex;
            Items[itemIndexSelected]->PositionIndex = temp;

            if (itemIndexSelected == selectedIndex || itemIndexToSwapWith == selectedIndex)
            {
                selectedIndex = itemIndexToSwapWith;
            }
        }

        if (selectedIndex != -1)
        {
            ImGui::BeginWindow("Edit Item##Speedometer-Edit-Item", nullptr, ImGuiWindowFlags_NoCollapse);
            {
                bool needsToReorderItems = false;
                Items[selectedIndex]->Edit(needsToReorderItems);

                if (needsToReorderItems)
                {
                    SortItemsOrder();
                }
            }

            ImGui::End();
        }
    }

    ImGui::SeparatorText("Buttons##Speedometer-Edit-Items-Separator");
    {
        if (ImGui::Button("Save All##Speedometer-Edit-SaveItems"))
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                Items[i]->SaveSettings();
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Undo All##Speedometer-Edit-UndoItems"))
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                Items[i]->LoadSettings(Items[i]->DrawIndex, false, true);
            }

            SortItemsOrder();
        }
        ImGui::SameLine();

        if (ImGui::Button("Reset All##Speedometer-Edit-ResetItems"))
        {
            Settings::SetSetting({ "Speedometer", "Item" }, json::object());

            selectedIndex = -1;
            Initialize();
        }
    }

    ImGui::End();
}

#pragma warning (pop)
