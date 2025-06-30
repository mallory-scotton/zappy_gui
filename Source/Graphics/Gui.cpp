///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Graphics/Gui.hpp"
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
}

///////////////////////////////////////////////////////////////////////////////
void Gui::Update(void)
{
    ImGui::SFML::Update(m_window, m_clock.restart());
}

///////////////////////////////////////////////////////////////////////////////
void Gui::Render(unsigned int viewport)
{
    PrepareDocking();

    RenderLogs();
    RenderCurrentGame();
    RenderTileInspector();
    RenderViewport(viewport);

    ImGui::SFML::Render(m_window);
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderViewport(unsigned int viewport)
{
    ImGui::Begin("Viewport");
    ImGui::Text("Rendering viewport: %d", viewport);
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderLogs(void)
{
    ImGui::Begin("Logs");
    ImGui::Text("Log messages will appear here.");
    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderCurrentGame(void)
{
    ImGui::Begin("Current Game");

    ImGui::Text("Current Frequency: %d", 0);
    ImGui::Text("Players Alive: %d", 0);
    ImGui::Text("Players Dead: %d", 0);

    //TODO: team names here

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Text("Total Resources:");
    ImGui::Text("Food: %d", 0);
    ImGui::Text("Linemate: %d", 0);
    ImGui::Text("Deraumere: %d", 0);
    ImGui::Text("Sibur: %d", 0);
    ImGui::Text("Mendiane: %d", 0);
    ImGui::Text("Phiras: %d", 0);
    ImGui::Text("Thystame: %d", 0);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    ImGui::Text("Players per Team:");
    //TODO: players alive here per team per level
    // use tree node ?

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Gui::RenderTileInspector(void)
{
    ImGui::Begin("Tile Inspector");

    //TODO: get the current tile and pos
    ImGui::Text("Current Tile: (%d, %d)", 0, 0);

    ImGui::Text("Resources:");
    ImGui::Text("Food: %d", 0);
    ImGui::Text("Linemate: %d", 0);
    ImGui::Text("Deraumere: %d", 0);
    ImGui::Text("Sibur: %d", 0);
    ImGui::Text("Mendiane: %d", 0);
    ImGui::Text("Phiras: %d", 0);
    ImGui::Text("Thystame: %d", 0);

    ImGui::Separator();
    //TODO: list players on tile with level and inv
    ImGui::Text("Players on Tile: %d", 0);

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

} // !namespace Zappy
