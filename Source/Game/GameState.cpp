///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Game/GameState.hpp"
#include "Errors/Exception.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <random>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
GameState::GameState()
    : m_socket(AF_INET, SOCK_STREAM)
    , m_isConnected(false)
    , m_commands({
        {"msz", std::bind(&GameState::ParseMSZ, this, std::placeholders::_1)},
        {"bct", std::bind(&GameState::ParseBCT, this, std::placeholders::_1)},
        {"tna", std::bind(&GameState::ParseTNA, this, std::placeholders::_1)},
        {"pnw", std::bind(&GameState::ParsePNW, this, std::placeholders::_1)},
        {"ppo", std::bind(&GameState::ParsePPO, this, std::placeholders::_1)},
        {"plv", std::bind(&GameState::ParsePLV, this, std::placeholders::_1)},
        {"pin", std::bind(&GameState::ParsePIN, this, std::placeholders::_1)},
        {"pex", std::bind(&GameState::ParsePEX, this, std::placeholders::_1)},
        {"pbc", std::bind(&GameState::ParsePBC, this, std::placeholders::_1)},
        {"pic", std::bind(&GameState::ParsePIC, this, std::placeholders::_1)},
        {"pie", std::bind(&GameState::ParsePIE, this, std::placeholders::_1)},
        {"pfk", std::bind(&GameState::ParsePFK, this, std::placeholders::_1)},
        {"pdr", std::bind(&GameState::ParsePDR, this, std::placeholders::_1)},
        {"pgt", std::bind(&GameState::ParsePGT, this, std::placeholders::_1)},
        {"pdi", std::bind(&GameState::ParsePDI, this, std::placeholders::_1)},
        {"enw", std::bind(&GameState::ParseENW, this, std::placeholders::_1)},
        {"ebo", std::bind(&GameState::ParseEBO, this, std::placeholders::_1)},
        {"edi", std::bind(&GameState::ParseEDI, this, std::placeholders::_1)},
        {"sgt", std::bind(&GameState::ParseSGT, this, std::placeholders::_1)},
        {"sst", std::bind(&GameState::ParseSST, this, std::placeholders::_1)},
        {"seg", std::bind(&GameState::ParseSEG, this, std::placeholders::_1)},
        {"smg", std::bind(&GameState::ParseSMG, this, std::placeholders::_1)},
        {"suc", std::bind(&GameState::ParseSUC, this, std::placeholders::_1)},
        {"sbp", std::bind(&GameState::ParseSBP, this, std::placeholders::_1)}
    })
{}

///////////////////////////////////////////////////////////////////////////////
bool GameState::Connect(const std::string& host, int port)
{
    m_host = host;
    m_port = port;

    try
    {
        if (!m_socket.IsValid())
        {
            m_socket = Socket(AF_INET, SOCK_STREAM);
        }

        m_socket.Connect(m_host, m_port);
        m_isConnected = true;

        std::string welcome = m_socket.RecvLine();

        if (welcome.empty() || welcome != "WELCOME")
        {
            m_socket.Close();
            m_isConnected = false;
            return (false);
        }

        m_socket.Send("GRAPHIC\n");
        return (true);
    }
    catch (const std::exception& e)
    {
        m_socket.Close();
        m_isConnected = false;
        return (false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::Update(void)
{
    if (!m_isConnected)
    {
        return;
    }

    std::string msg = m_socket.RecvLine();

    if (msg.empty())
    {
        return;
    }

    auto it = m_commands.find(msg.substr(0, 3));
    if (it != m_commands.end())
    {
        it->second(msg.substr(4));
    }
}

///////////////////////////////////////////////////////////////////////////////
std::tuple<int, int> GameState::GetDimensions(void) const
{
    return (std::make_tuple(m_width, m_height));
}

///////////////////////////////////////////////////////////////////////////////
int GameState::GetWidth(void) const
{
    return (m_width);
}

///////////////////////////////////////////////////////////////////////////////
int GameState::GetHeight(void) const
{
    return (m_height);
}

///////////////////////////////////////////////////////////////////////////////
const Inventory& GameState::GetTileAt(unsigned int x, unsigned int y) const
{
    if (x >= m_width || y >= m_height)
    {
        throw Exception("Invalid tile coordinates");
    }
    return (m_tiles[y * m_width + x]);
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Inventory>& GameState::GetTiles(void) const
{
    return (m_tiles);
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Team>& GameState::GetTeams(void) const
{
    return (m_teams);
}

const std::vector<Message>& GameState::GetMessages(void) const
{
    return (m_messages);
}

///////////////////////////////////////////////////////////////////////////////
int GameState::GetFrequency(void) const
{
    return (m_frequency);
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseMSZ(const std::string& msg)
{
    std::istringstream iss(msg);

    iss >> m_width >> m_height;
    m_tiles.resize(m_width * m_height);
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseBCT(const std::string& msg)
{
    std::istringstream iss(msg);
    int x, y;

    iss >> x >> y;

    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return;
    }

    int index = y * m_width + x;

    m_tiles[index].ParseContent(iss.str());
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseTNA(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string name;

    iss >> name;

    if (!name.empty())
    {
        Team team(name);
        m_teams.push_back(team);
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePNW(const std::string& msg)
{
    Player player(msg);

    const std::string& teamName = player.GetTeam();

    for (auto& team : m_teams)
    {
        if (team.GetName() == teamName)
        {
            team.AddPlayer(player);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePPO(const std::string& msg)
{
    std::istringstream iss(msg);

    try { GetPlayerByID(iss).UpdatePosition(iss.str()); }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePLV(const std::string& msg)
{
    std::istringstream iss(msg);

    try { GetPlayerByID(iss).UpdateLevel(iss.str()); }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIN(const std::string& msg)
{
    std::istringstream iss(msg);

    try { GetPlayerByID(iss).UpdateInventory(iss.str()); }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePEX(const std::string& msg)
{
    std::istringstream iss(msg);

    try
    {
        Player& player = GetPlayerByID(iss);

        m_messages.emplace_back(
            player.GetName() + " has left the game.",
            "EVENT",
            "SERVER",
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePBC(const std::string& msg)
{
    std::istringstream iss(msg);

    try
    {
        Player& player = GetPlayerByID(iss);
        std::string content;

        std::getline(iss, content);
        content.erase(0, content.find_first_not_of(' '));

        m_messages.emplace_back(
            player.GetName() + ": " + content,
            "BROADCAST",
            player.GetName(),
            false
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIC(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIE(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePFK(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePDR(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePGT(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePDI(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseENW(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseEBO(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseEDI(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSGT(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSST(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSEG(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSMG(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSUC(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSBP(const std::string& msg)
{}

///////////////////////////////////////////////////////////////////////////////
Player& GameState::GetPlayerByID(std::istringstream& iss)
{
    std::string idStr;

    iss >> idStr;

    if (!idStr.empty())
    {
        unsigned int id = std::stoi(idStr.substr(1));

        for (auto& team : m_teams)
        {
            for (auto& player : team.GetPlayers())
            {
                if (player.GetID() == id)
                {
                    return (player);
                }
            }
        }
    }

    throw Exception("Player not found");
}

} // !namespace Zappy
