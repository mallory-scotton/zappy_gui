///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Game/Inventory.hpp"
#include "Graphics/Gui.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
Inventory::Inventory(void)
    : food(0)
    , linemate(0)
    , deraumere(0)
    , sibur(0)
    , mendiane(0)
    , phiras(0)
    , thystame(0)
{}

///////////////////////////////////////////////////////////////////////////////
void Inventory::ParseContent(std::istringstream& content)
{
    content >> food
            >> linemate
            >> deraumere
            >> sibur
            >> mendiane
            >> phiras
            >> thystame;
}

///////////////////////////////////////////////////////////////////////////////
void Inventory::DrawInvText(void) const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 0.5f, 1.0f));
    ImGui::Text("Food:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", food);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    ImGui::Text("Linemate:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", linemate);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
    ImGui::Text("Deraumere:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", deraumere);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.3f, 1.0f));
    ImGui::Text("Sibur:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", sibur);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.5f, 0.8f, 1.0f));
    ImGui::Text("Mendiane:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", mendiane);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    ImGui::Text("Phiras:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", phiras);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
    ImGui::Text("Thystame:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", thystame);
}

///////////////////////////////////////////////////////////////////////////////
void Inventory::DrawInvNumb(void) const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.8f, 0.5f, 1.0f));
    ImGui::Text("%d ", food);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    ImGui::Text("%d", linemate);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
    ImGui::Text("%d", deraumere);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.6f, 0.3f, 1.0f));
    ImGui::Text("%d", sibur);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.5f, 0.8f, 1.0f));
    ImGui::Text("%d", mendiane);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.9f, 1.0f));
    ImGui::Text("%d", phiras);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
    ImGui::Text("%d", thystame);
    ImGui::PopStyleColor();
}

} // !namespace Zappy
