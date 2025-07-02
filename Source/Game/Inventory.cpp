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
    // Food: Pastel yellow (255, 255, 153)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255/255.0f, 255/255.0f, 153/255.0f, 1.0f));
    ImGui::Text("Food:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", food);

    // Linemate: Gray (179, 179, 179)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(179/255.0f, 179/255.0f, 179/255.0f, 1.0f));
    ImGui::Text("Linemate:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", linemate);

    // Deraumere: Deep emerald green (0, 128, 75)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0/255.0f, 128/255.0f, 75/255.0f, 1.0f));
    ImGui::Text("Deraumere:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", deraumere);

    // Sibur: Red ruby (224, 17, 95)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(224/255.0f, 17/255.0f, 95/255.0f, 1.0f));
    ImGui::Text("Sibur:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", sibur);

    // Mendiane: White pastel (230, 230, 245)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(230/255.0f, 230/255.0f, 245/255.0f, 1.0f));
    ImGui::Text("Mendiane:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", mendiane);

    // Phiras: Pastel cyan (175, 238, 238)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(175/255.0f, 238/255.0f, 238/255.0f, 1.0f));
    ImGui::Text("Phiras:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", phiras);

    // Thystame: Purple (147, 112, 219)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(147/255.0f, 112/255.0f, 219/255.0f, 1.0f));
    ImGui::Text("Thystame:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%d", thystame);
}

///////////////////////////////////////////////////////////////////////////////
void Inventory::DrawInvNumb(void) const
{
    // Food: Pastel yellow (255, 255, 153)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255/255.0f, 255/255.0f, 153/255.0f, 1.0f));
    ImGui::Text("%d ", food);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Linemate: Gray (179, 179, 179)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(179/255.0f, 179/255.0f, 179/255.0f, 1.0f));
    ImGui::Text("%d", linemate);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Deraumere: Deep emerald green (0, 128, 75)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0/255.0f, 128/255.0f, 75/255.0f, 1.0f));
    ImGui::Text("%d", deraumere);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Sibur: Red ruby (224, 17, 95)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(224/255.0f, 17/255.0f, 95/255.0f, 1.0f));
    ImGui::Text("%d", sibur);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Mendiane: White pastel (230, 230, 245)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(230/255.0f, 230/255.0f, 245/255.0f, 1.0f));
    ImGui::Text("%d", mendiane);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Phiras: Pastel cyan (175, 238, 238)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(175/255.0f, 238/255.0f, 238/255.0f, 1.0f));
    ImGui::Text("%d", phiras);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Thystame: Purple (147, 112, 219)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(147/255.0f, 112/255.0f, 219/255.0f, 1.0f));
    ImGui::Text("%d", thystame);
    ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////
void Inventory::Reset(void)
{
    food = 0;
    linemate = 0;
    deraumere = 0;
    sibur = 0;
    mendiane = 0;
    phiras = 0;
    thystame = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Inventory::Add(const Inventory& other)
{
    food += other.food;
    linemate += other.linemate;
    deraumere += other.deraumere;
    sibur += other.sibur;
    mendiane += other.mendiane;
    phiras += other.phiras;
    thystame += other.thystame;
}

} // !namespace Zappy
