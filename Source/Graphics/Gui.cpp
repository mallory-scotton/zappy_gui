///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Graphics/Gui.hpp"
#include "Game/GameState.hpp"
#include "Errors/ImGuiException.hpp"
#include "Libraries/imgui.h"
#include "Libraries/imgui-SFML.h"
#include "Libraries/imgui_internal.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
Gui::Gui(sf::RenderWindow& window)
    : m_window(window)
    , m_currentX(0)
    , m_currentY(0)
    , m_debug(false)
{
    if (!ImGui::SFML::Init(m_window))
    {
        throw ImGuiException("Failed to initialize ImGui-SFML");
    }

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    SetupImGuiStyle();
}

///////////////////////////////////////////////////////////////////////////////
Gui::~Gui()
{
    ImGui::SFML::Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::ProcessEvent(const sf::Event& event)
{
    ImGui::SFML::ProcessEvent(m_window, event);

    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::F1)
        {
            m_debug = !m_debug;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Gui::Update(void)
{
    ImGui::SFML::Update(m_window, m_clock.restart());
}

///////////////////////////////////////////////////////////////////////////////
void Gui::Render(Viewport& viewport)
{
    PrepareDocking();

    // SMall font
    RenderLogs();

    // Big font
    RenderCurrentGame();
    RenderTileInspector(viewport);
    RenderViewport(viewport);

    if (m_debug)
    {
        ImGui::Begin(
            "App Stats", nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings
        );
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", ImGui::GetIO().DeltaTime * 1000.0f);
        ImGui::End();
    }

    ImGui::SFML::Render(m_window);
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderViewport(Viewport& viewport)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr,
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse
    );
    ImGui::PopStyleVar();

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 position = ImGui::GetWindowPos();

    viewport.SetViewportPosition(position.x, position.y);

    viewport.Resize(
        static_cast<unsigned int>(size.x),
        static_cast<unsigned int>(size.y)
    );

    ImGui::Image(viewport.GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderLogs(void)
{
    GameState& gs = GameState::GetInstance();

    ImGui::Begin("Logs");

    ImGui::Text("Game Logs:");
    GameState::ScopedLock lock(gs);
    const auto& logs = gs.GetMessages();

    if (ImGui::TreeNode("Filter Options"))
    {
        ImGui::Checkbox("Broadcast", &m_BroadcastLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Egg", &m_EggLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Event", &m_EventLogs);

        ImGui::Checkbox("Incantation", &m_IncantationLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Resource", &m_ResourceLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Death", &m_DeathLogs);

        ImGui::Checkbox("Victory", &m_VictoryLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Info", &m_InfoLogs);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &m_ErrorLogs);
        ImGui::TreePop();
    }

    ImGui::Separator();

    static float rainbowTime = 0.0f;
    rainbowTime += ImGui::GetIO().DeltaTime;

    for (auto it = logs.rbegin(); it != logs.rend(); ++it)
    {
        const auto& log = *it;
        const std::string& type = log.GetType();

        // Filter based on log type
        bool showLog = false;

        if (type == "Broadcast" && m_BroadcastLogs)
            showLog = true;
        else if (type == "Egg" && m_EggLogs)
            showLog = true;
        else if (type == "Event" && m_EventLogs)
            showLog = true;
        else if (type == "Incantation" && m_IncantationLogs)
            showLog = true;
        else if (type == "Resource" && m_ResourceLogs)
            showLog = true;
        else if (type == "Death" && m_DeathLogs)
            showLog = true;
        else if (type == "Victory" && m_VictoryLogs)
            showLog = true;
        else if (type == "Info" && m_InfoLogs)
            showLog = true;
        else if (type == "Error" && m_ErrorLogs)
            showLog = true;

        if (showLog)
        {
            if (type == "Victory") {
                ImVec4 origColor = ImGui::GetStyle().Colors[ImGuiCol_Text];

                const std::string& content = log.GetContent();
                for (size_t i = 0; i < content.size(); i++) {
                    float hue = rainbowTime * 2.0f + i * 0.02f;
                    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                    ImGui::ColorConvertHSVtoRGB(fmodf(hue, 1.0f), 0.8f, 1.0f,
                                                color.x, color.y, color.z);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::Text("%c", content[i]);
                    ImGui::PopStyleColor();
                    ImGui::SameLine(0.0f, 0.0f);
                }
                ImGui::NewLine();

                ImGui::PushStyleColor(ImGuiCol_Text, origColor);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%s", log.GetContent().c_str());
            }
        }
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderCurrentGame(void)
{
    GameState& gs = GameState::GetInstance();

    ImGui::Begin("Current Game");

    ImGui::SetWindowFontScale(2.f);

    ImGui::Text("Current Frequency: %d", gs.GetFrequency());
    ImGui::Text("Map Size: %d x %d", gs.GetWidth(), gs.GetHeight());
    ImGui::Text("Players Alive: %d", gs.GetLivingPlayers());
    ImGui::Text("Players Dead: %d", gs.GetDeadPlayers());

    const auto& teams = gs.GetTeams();
    ImGui::Text("Teams: %d", static_cast<int>(teams.size()));
    ImGui::SameLine();
    for (const auto& team : teams)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ConvertColor(team.GetColor()));
        ImGui::Text("%s", team.GetName().c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Text("Total Resources:");

    const Inventory& totalResources = gs.GetTotalResources();

    totalResources.DrawInvText();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Text("Players per Team:");
    for (const auto& team : teams)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ConvertColor(team.GetColor()));

        const auto& players = team.GetPlayers();

        std::array<int, 9> levelCount = {};

        for (const auto& player : players)
        {
            levelCount[player.GetLevel()]++;
        }

        int maxLevel = 1;
        for (int i = 8; i >= 1; i--) {
            if (levelCount[i] > 0) {
                maxLevel = i;
                break;
            }
        }

        std::string displayName = team.GetName() + " Level " + std::to_string(team.GetMaxLevel());
        bool open = ImGui::TreeNode(team.GetName().c_str(), "%s", displayName.c_str());
        ImGui::PopStyleColor();

        if (open)
        {
            ImGui::Text("Players: %d | Max Level: %d", team.GetLivingPlayers(), maxLevel);

            for (unsigned int level = 8; level > 0; level--) {
                ImGui::Separator();
                ImGui::Text("Level %d: %d players", level, levelCount[level]);

                std::string levelStr = "Level " + std::to_string(level);
                if (ImGui::TreeNode(levelStr.c_str())) {
                    for (const auto& player : players)
                    {
                        if (player.GetLevel() == level)
                        {
                            ImGui::Text("%s (ID: %d)", player.GetName().c_str(),
                                        player.GetID());
                            ImGui::SameLine();
                            player.GetInventory().DrawInvNumb();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderTileInspector(Viewport& viewport)
{
    GameState& gs = GameState::GetInstance();

    ImGui::Begin("Tile Inspector");

    ImGui::Text("Current Tile: (%d, %d)", viewport.m_indexX, viewport.m_indexY);

    const Inventory& inv = gs.GetTileAt(viewport.m_indexX, viewport.m_indexY);

    inv.DrawInvText();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Text("Players on Tile: %d", 0);
    for (const auto& team : gs.GetTeams())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ConvertColor(team.GetColor()));

        for (const auto& player : team.GetPlayers())
        {
            if (player.GetX() == viewport.m_indexX && player.GetY() == viewport.m_indexY)
            {
                ImGui::Text("%s (ID: %d, Level: %d)", player.GetName().c_str(),
                            player.GetID(), player.GetLevel());
                player.GetInventory().DrawInvNumb();
            }
        }

        ImGui::PopStyleColor();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::PrepareDocking(void)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpaceHost", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_PassthruCentralNode
    );

    static bool dockspace_initialized = false;
    if (!dockspace_initialized)
    {
        dockspace_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);

        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id,
            ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id,
            ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);

        ImGuiID dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right,
            ImGuiDir_Up, 0.5f, nullptr, &dock_id_right);

        ImGui::DockBuilderDockWindow("Logs", dock_id_left);
        ImGui::DockBuilderDockWindow("Viewport", dockspace_id);
        ImGui::DockBuilderDockWindow("Current Game", dock_id_right_top);
        ImGui::DockBuilderDockWindow("Tile Inspector", dock_id_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::SetupImGuiStyle(void)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
ImVec4 Gui::ConvertColor(const sf::Color& color) const
{
    return ImVec4(
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    );
}

} // !namespace Zappy
