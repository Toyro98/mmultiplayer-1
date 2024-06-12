#include "chaos.h"

#include "../chaos/effect.h"
#include "../../menu.h"
#include "../../settings.h"
#include "../../util.h"

static bool IsEnabled = false;
static bool IsPaused = false;
static bool IsChaosActive = false;

std::mt19937 rng;
static int Seed = 0;
static bool DoRandomizeNewSeed = true;

static float DurationTime[static_cast<int>(EDuration::COUNT)] = { 5.0f, 15.0f, 45.0f, 90.0f };
static const char* DurationTimeStrings[] = { "Brief", "Short", "Normal", "Long" };

static float TimeUntilNewEffect = 20.0f;
static int MaxAttemptsForNewEffect = 100;

static float TimerInSeconds = 0.0f;
static float TimerHeight = 18.0f;
static ImVec4 TimerBackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static ImVec4 TimerColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

struct ActiveEffectInfo {
    float TimeRemaining;
    Effect* Effect;
};

static std::vector<ActiveEffectInfo> ActiveEffects;

static void SetNewSeed()
{
    if (DoRandomizeNewSeed)
    {
        std::random_device rd;
        std::mt19937 rngEngine(rd());
        std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

        Seed = dist(rngEngine);
        rng.seed(Seed);
    }

    for (auto effect : Effects())
    {
        effect->SetSeed(Seed);
    }
}

static void Restart()
{
    for (auto active : ActiveEffects)
    {
        active.Effect->Shutdown();
    }

    ActiveEffects.clear();
    ActiveEffects.shrink_to_fit();

    TimerInSeconds = 0.0f;
    IsChaosActive = false;
}

static void ChaosTab()
{
    if (ImGui::Checkbox("Enabled##Chaos-EnabledCheckbox", &IsEnabled) && !IsEnabled)
    {
        Restart();
    }

    if (!IsEnabled)
    {
        return;
    }

    ImGui::Checkbox("Randomize Seed##Chaos-DoRandomizeNewSeed", &DoRandomizeNewSeed);
    ImGui::Separator(2.5f);

    if (IsChaosActive)
    {
        const char* buttonLabel = IsPaused ? "Resume##Chaos-ResumeButton" : "Pause##Chaos-PauseButton";
        if (ImGui::Button(buttonLabel))
        {
            IsPaused = !IsPaused;
        }

        ImGui::SameLine();
        if (ImGui::Button("Restart##Chaos-RestartButton"))
        {
            Restart();
        }
    }
    else
    {
        if (ImGui::Button("Start##Chaos-StartButton"))
        {
            SetNewSeed();
            Restart();

            IsPaused = false;
            IsChaosActive = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Start New Game##Chaos-StartNewGameButton"))
        {
            SetNewSeed();
        
            const auto world = Engine::GetWorld();

            if (world)
            {
                const auto gameInfo = static_cast<Classes::ATdGameInfo*>(world->Game);
                gameInfo->TdGameData->StartNewGameWithTutorial(true);
            
                Restart();
                IsChaosActive = true;
            }
        }
    }

    if (DoRandomizeNewSeed)
    {
        ImGui::BeginDisabled();
        ImGui::InputInt("Seed##Chaos-Seed", &Seed, 0, 0);
        ImGui::EndDisabled();
    }
    else
    {
        if (ImGui::InputInt("Seed##Chaos-Seed", &Seed, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            SetNewSeed();
        }
    }

    ImGui::Separator(2.5f);

    if (ImGui::SliderFloat("Time Until New Effect##Chaos-TimeUntilNewEffect", &TimeUntilNewEffect, 5.0f, 60.0f, "%.0f sec"))
    {
        TimerInSeconds = 0.0f;
    }

    const auto& io = ImGui::GetIO();

    ImGui::SliderFloat("Timer Height##Chaos-TimerHeight", &TimerHeight, 1.0f, 30.0f, "%.1f");
    ImGui::ColorEdit4("Timer Color##Chaos-TimerColor", &TimerColor.x);
    ImGui::ColorEdit4("Background Color##Chaos-TimerBackgroundColor", &TimerBackgroundColor.x);
    ImGui::Separator(2.5f);

    ImGui::Text("Duration Time");
    for (int i = 0; i < static_cast<int>(EDuration::COUNT); i++)
    {
        ImGui::SliderFloat((std::string(DurationTimeStrings[i]) + "##Chaos-" + DurationTimeStrings[i] + "-DurationTime").c_str(),
            &DurationTime[i], 5.0f, 120.0f, "%.0f sec"
        );
    }
    ImGui::Separator(2.5f);

    // TODO: Disable all of the same type of effect if there are multiple ones

    for (auto effect : Effects())
    {
        if (!effect->IsEnabled)
        {
            ImGui::BeginDisabled();
        }

        int durationType = (int)effect->DurationType;
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo(("##Chaos-DurationType-" + effect->Name).c_str(), &durationType, DurationTimeStrings, IM_ARRAYSIZE(DurationTimeStrings));

        effect->DurationType = static_cast<EDuration>(durationType);
        effect->DurationTime = DurationTime[(int)effect->DurationType];

        if (!effect->IsEnabled)
        {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        ImGui::Checkbox(effect->Name.c_str(), &effect->IsEnabled);
    }
}

static void OnRender(IDirect3DDevice9* device) 
{
    if (!IsEnabled)
    {
        return;
    }

    for (auto active : ActiveEffects)
    {
        if (active.TimeRemaining >= 0.0f)
        {
            active.Effect->Render(device);
        }
    }

    const auto window = ImGui::BeginRawScene("##Chaos-TimeCountdownRender");
    const auto& io = ImGui::GetIO();

    window->DrawList->AddRectFilled(ImVec2(), ImVec2(io.DisplaySize.x, TimerHeight), ImColor(TimerBackgroundColor));
    window->DrawList->AddRectFilled(ImVec2(), ImVec2(io.DisplaySize.x * TimerInSeconds / TimeUntilNewEffect, TimerHeight), ImColor(TimerColor));

    ImGui::EndRawScene();

    // Temp until proper UI
    ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 165), ImGuiCond_FirstUseEver);
    ImGui::BeginWindow("Active Effects##Chaos-ActiveEffectsTemp");

    for (auto it = ActiveEffects.rbegin(); it != ActiveEffects.rend(); ++it)
    {
        const auto& active = *it;

        if (active.Effect->IsDone)
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.75f), active.Effect->DisplayName.c_str());
        }
        else
        {
            if (active.Effect->GetType() == "GoToMainMenu")
            {
                ImGui::Text("%s", active.Effect->DisplayName.c_str());
            }
            else
            {
                ImGui::Text("%s (%.1f)", active.Effect->DisplayName.c_str(), active.TimeRemaining);
            }
        }
    }

    ImGui::End();
}

static void AddRandomEffect()
{
    std::uniform_int_distribution<int> dist(0, Effects().size() - 1);

    for (int i = 0; i < MaxAttemptsForNewEffect; i++)
    {
        auto effect = Effects()[dist(rng)];

        if (!effect->IsEnabled)
        {
            continue;
        }

        bool isDuplicate = false;
        for (const auto& active : ActiveEffects)
        {
            if (effect->GetType() == active.Effect->GetType())
            {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate)
        {
            continue;
        }

        ActiveEffectInfo newActiveEffectInfo;
        newActiveEffectInfo.TimeRemaining = DurationTime[(int)effect->DurationType];
        newActiveEffectInfo.Effect = effect;

        effect->DurationTime = newActiveEffectInfo.TimeRemaining;
        effect->Start();

        ActiveEffects.push_back(newActiveEffectInfo);
        break;
    }
}

static void OnTick(float deltaTime) 
{
    if (!IsEnabled || !IsChaosActive || IsPaused)
    {
        return;
    }

    const auto pawn = Engine::GetPlayerPawn();
    const auto controller = Engine::GetPlayerController();

    if (!pawn || !controller)
    {
        return;
    }

    const float timeSinceSpawned = pawn->WorldInfo->TimeSeconds - pawn->SpawnTime;
    const float timeSinceCreated = pawn->WorldInfo->TimeSeconds - pawn->CreationTime;
    
    const auto engine = Engine::GetEngine();
    const float maxDeltaTime = 1.0f / (engine ? engine->MinSmoothedFrameRate : 22.0f);

    if (timeSinceSpawned == 0.0f || timeSinceCreated == 0.0f || deltaTime >= maxDeltaTime)
    {
        return;
    }

    TimerInSeconds += deltaTime;

    if (TimerInSeconds >= TimeUntilNewEffect)
    {
        AddRandomEffect();
        TimerInSeconds = 0.0f;
    }

    for (auto it = ActiveEffects.rbegin(); it != ActiveEffects.rend(); ++it)
    {
        auto& active = *it;
        active.TimeRemaining -= deltaTime;

        if (active.TimeRemaining >= 0.0f)
        {
            active.Effect->Tick(deltaTime);
            continue;
        }

        if (active.Effect->Shutdown())
        {
            ActiveEffects.erase((it + 1).base());
        }
    }

    ActiveEffects.shrink_to_fit();
}

bool Chaos::Initialize() 
{
    Menu::AddTab("Chaos", ChaosTab);

    Engine::OnTick(OnTick);
    Engine::OnRenderScene(OnRender);

    return true;
}

std::string Chaos::GetName() 
{ 
    return "Chaos"; 
}