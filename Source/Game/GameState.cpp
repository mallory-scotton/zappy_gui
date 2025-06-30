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
#include <iostream>

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

    std::string msg = m_socket.RecvLine(MSG_DONTWAIT);

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
unsigned int GameState::GetWidth(void) const
{
    return (m_width);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetHeight(void) const
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
unsigned int GameState::GetFrequency(void) const
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
    unsigned int x, y;

    iss >> x >> y;

    if (x >= m_width || y >= m_height)
    {
        return;
    }

    unsigned int index = y * m_width + x;

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

    std::cout << "Team added: " << name << std::endl;
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
            "Event",
            "Server",
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
            "Broadcast",
            player.GetName(),
            false
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIC(const std::string& msg)
{
    std::istringstream iss(msg);
    unsigned int x, y, level;
    std::vector<unsigned int> ids;
    std::string idStr;

    iss >> x >> y >> level;

    while (iss >> idStr)
    {
        if (!idStr.empty())
        {
            unsigned int id = std::stoi(idStr.substr(1));
            ids.push_back(id);
        }
    }

    m_messages.emplace_back(
        "Incantation started at (" + std::to_string(x) +
        ", " + std::to_string(y) + ") for level " +
        std::to_string(level) + " by Player " + std::to_string(ids[0]),
        "Incantation",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIE(const std::string& msg)
{
    std::istringstream iss(msg);
    unsigned int x, y;
    std::string result;

    iss >> x >> y >> result;

    m_messages.emplace_back(
        "Incantation ended at (" + std::to_string(x) + ", " +
        std::to_string(y) + ") with result: " + result,
        "Incantation",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePFK(const std::string& msg)
{
    std::istringstream iss(msg);

    try
    {
        Player& player = GetPlayerByID(iss);
        m_messages.emplace_back(
            player.GetName() + " is laying an egg",
            "Egg",
            player.GetName(),
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePDR(const std::string& msg)
{
    std::istringstream iss(msg);
    unsigned int index;

    static const std::vector<std::string> resources =
    {
        "food",
        "linemate", "deraumere", "sibur",
        "mendiane", "phiras", "thystame"
    };

    try
    {
        Player& player = GetPlayerByID(iss);

        iss >> index;

        m_messages.emplace_back(
            player.GetName() + " has dropped a resource: " + resources[index],
            "Resource",
            player.GetName(),
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePGT(const std::string& msg)
{
    std::istringstream iss(msg);
    unsigned int index;

    static const std::vector<std::string> resources =
    {
        "food",
        "linemate", "deraumere", "sibur",
        "mendiane", "phiras", "thystame"
    };

    try
    {
        Player& player = GetPlayerByID(iss);

        iss >> index;

        m_messages.emplace_back(
            player.GetName() + " has taken a resource: " + resources[index],
            "Resource",
            player.GetName(),
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePDI(const std::string& msg)
{
    std::istringstream iss(msg);

    try
    {
        Player& player = GetPlayerByID(iss);

        player.SetAlive(false);

        m_messages.emplace_back(
            player.GetName() + " died",
            "Death",
            "Server",
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseENW(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string eggStr;
    unsigned int x, y;

    iss >> eggStr;

    try
    {
        Player& player = GetPlayerByID(iss);

        unsigned int id = std::stoi(eggStr.substr(1));
        iss >> x >> y;

        m_messages.emplace_back(
            "Egg " + std::to_string(id) + " laid by Player " +
            std::to_string(player.GetID()) + " at (" +
            std::to_string(x) + ", " + std::to_string(y) + ")",
            "Egg",
            player.GetName(),
            true
        );
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseEBO(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string eggStr;

    iss >> eggStr;

    unsigned int id = std::stoi(eggStr.substr(1));

    m_messages.emplace_back(
        "Egg " + std::to_string(id) + " has been hatched",
        "Egg",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseEDI(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string eggStr;

    iss >> eggStr;

    unsigned int id = std::stoi(eggStr.substr(1));

    m_messages.emplace_back(
        "Egg " + std::to_string(id) + " has been destroyed",
        "Egg",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSGT(const std::string& msg)
{
    std::istringstream iss(msg);

    iss >> m_frequency;
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSST(const std::string& msg)
{
    std::istringstream iss(msg);

    iss >> m_frequency;
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSEG(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string teamName;

    iss >> teamName;

    m_messages.emplace_back(
        "Team " + teamName + " has won the game!",
        "Victory",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSMG(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string content;

    std::getline(iss, content);
    content.erase(0, content.find_first_not_of(' '));

    m_messages.emplace_back(
        content,
        "Info",
        "Server",
        false
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSUC(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string command;

    std::getline(iss, command);
    command.erase(0, command.find_first_not_of(' '));

    m_messages.emplace_back(
        "Unknown command: " + command,
        "Error",
        "Server",
        true
    );
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseSBP(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string command, params;

    iss >> command;

    std::getline(iss, params);
    params.erase(0, params.find_first_not_of(' '));

    if (params.empty() || command.empty())
    {
        return;
    }
    m_messages.emplace_back(
        "Bad parameter for command: " + command + " " + params,
        "Error",
        "Server",
        false
    );
}

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
