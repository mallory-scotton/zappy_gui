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
    , m_width(0)
    , m_height(0)
    , m_frequency(0)
    , m_livingPlayers(0)
    , m_deadPlayers(0)
    , m_hasChanged(false)
    , m_shouldStop(false)
    , m_hasWin(false)
    , m_winner("No Winner", sf::Color::White)
{
    m_totalResources.Reset();
}

///////////////////////////////////////////////////////////////////////////////
GameState::~GameState()
{
    StopNetworkThread();
    Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
bool GameState::Connect(const std::string& host, int port)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

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
        StartNetworkThread();
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
void GameState::Disconnect(void)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    if (m_isConnected)
    {
        m_socket.Close();
        m_isConnected = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::StartNetworkThread(void)
{
    if (m_networkThread.joinable())
    {
        StopNetworkThread();
    }

    m_shouldStop = false;
    m_networkThread = std::thread(&GameState::NetworkThreadFunction, this);
}


///////////////////////////////////////////////////////////////////////////////
void GameState::StopNetworkThread(void)
{
    m_shouldStop = true;
    m_cv.notify_all();

    if (m_networkThread.joinable())
    {
        m_networkThread.join();
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::NetworkThreadFunction(void)
{
    while (!m_shouldStop && m_isConnected)
    {
        try
        {
            ProcessNetworkMessages();

            // Small sleep to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Network thread error: " << e.what() << std::endl;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ProcessNetworkMessages(void)
{
    if (!m_isConnected)
    {
        return;
    }

    std::string msg;
    while (!(msg = m_socket.RecvLine(MSG_DONTWAIT)).empty() && !m_shouldStop)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);

        auto it = m_commands.find(msg.substr(0, 3));
        if (it != m_commands.end())
        {
            try
            {
                it->second(msg.substr(4));
            }
            catch (...) {}
        }
    }

    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        if (m_messages.size() >= 200)
        {
            size_t removed = 0;
            for (auto it = m_messages.begin(); it != m_messages.end() && removed < 50;)
            {
                if (!it->IsImportant())
                {
                    it = m_messages.erase(it);
                    ++removed;
                }
                else
                {
                    ++it;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::Lock(void) const
{
    m_mutex.lock();
}

///////////////////////////////////////////////////////////////////////////////
void GameState::Unlock(void) const
{
    m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////
std::tuple<unsigned int, unsigned int> GameState::GetDimensions(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (std::make_tuple(m_width, m_height));
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetWidth(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_width);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetHeight(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_height);
}

///////////////////////////////////////////////////////////////////////////////
const Inventory& GameState::GetTileAt(unsigned int x, unsigned int y) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (x >= m_width || y >= m_height)
    {
        throw Exception("Invalid tile coordinates");
    }
    return (m_tiles[y * m_width + x]);
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Inventory>& GameState::GetTiles(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_tiles);
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<Team>& GameState::GetTeams(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_teams);
}

///////////////////////////////////////////////////////////////////////////////
const std::deque<Message>& GameState::GetMessages(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_messages);
}

///////////////////////////////////////////////////////////////////////////////
const Inventory& GameState::GetTotalResources(void)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    m_totalResources.Reset();

    for (const auto& tile : m_tiles)
    {
        m_totalResources.Add(tile);
    }

    return (m_totalResources);
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ResetChanged(void)
{
    m_hasChanged = false;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetFrequency(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_frequency);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetLivingPlayers(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_livingPlayers);
}

///////////////////////////////////////////////////////////////////////////////
unsigned int GameState::GetDeadPlayers(void) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return (m_deadPlayers);
}

///////////////////////////////////////////////////////////////////////////////
std::vector<const Player*> GameState::GetPlayersAt(
    unsigned int x,
    unsigned int y
) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    std::vector<const Player*> players;

    for (const auto& team : m_teams)
    {
        for (const auto& player : team.GetPlayers())
        {
            auto [px, py] = player.GetPosition();

            if (px == x && py == y && player.IsAlive())
            {
                players.push_back(&player);
            }
        }
    }

    return (players);
}

///////////////////////////////////////////////////////////////////////////////
bool GameState::HasChanged(void) const
{
    return (m_hasChanged.load());
}

///////////////////////////////////////////////////////////////////////////////
bool GameState::HasWin(void) const
{
    return (m_hasWin);
}

///////////////////////////////////////////////////////////////////////////////
const Team& GameState::GetWinner(void) const
{
    return (m_winner);
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<GameState::AnimationEvent>& GameState::GetAnimationEvents(void) const
{
    return m_animationEvents;
}

///////////////////////////////////////////////////////////////////////////////

void GameState::ClearAnimationEvents(void)
{
    m_animationEvents.clear();
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseMSZ(const std::string& msg)
{
    std::istringstream iss(msg);

    iss >> m_width >> m_height;
    m_tiles.resize(m_width * m_height);
    m_hasChanged = true;
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

    m_tiles[index].ParseContent(iss);
    m_hasChanged = true;
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParseTNA(const std::string& msg)
{
    std::istringstream iss(msg);
    std::string name;

    iss >> name;

    if (!name.empty())
    {
        sf::Color color = m_teamColors[m_teams.size() % m_teamColors.size()];
        Team team(name, color);
        m_teams.push_back(team);
        m_hasChanged = true;
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
            m_livingPlayers++;
            m_hasChanged = true;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePPO(const std::string& msg)
{
    std::istringstream iss(msg);

    try {
        GetPlayerByID(iss).UpdatePosition(iss);
        m_hasChanged = true;
    }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePLV(const std::string& msg)
{
    std::istringstream iss(msg);

    try { GetPlayerByID(iss).UpdateLevel(iss); }
    catch (...) {}
}

///////////////////////////////////////////////////////////////////////////////
void GameState::ParsePIN(const std::string& msg)
{
    std::istringstream iss(msg);

    try { GetPlayerByID(iss).UpdateInventory(iss); }
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
            false
        );

        for (auto& team : m_teams)
        {
            if (team.GetName() == player.GetTeam())
            {
                team.RemovePlayer(player);
                m_livingPlayers--;
                m_deadPlayers++;
                m_hasChanged = true;
                break;
            }
        }
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
        m_hasChanged = true;

        for (auto& team : m_teams)
        {
            if (team.GetName() == player.GetTeam())
            {
                m_animationEvents.emplace_back(
                    AnimationType::Broadcast,
                    player.GetX(),
                    player.GetY(),
                    2.0f,
                    team
                );
                break;
            }
        }

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
    m_hasChanged = true;

    m_animationEvents.emplace_back(AnimationType::IncantationStart, x, y, 2.0f);
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
    m_hasChanged = true;

    m_animationEvents.emplace_back(
        result == "1" ? AnimationType::IncantationSuccess : AnimationType::IncantationFail,
        x, y,
        2.0f
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
            false
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
            false
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
            false
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
            false
        );

        for (auto& team : m_teams)
        {
            if (team.GetName() == player.GetTeam())
            {
                team.RemovePlayer(player);
                m_livingPlayers--;
                m_deadPlayers++;
                m_hasChanged = true;
                break;
            }
        }
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
            false
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
        false
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
        false
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
    m_hasChanged = true;

    m_hasWin = true;
    for (auto& team : m_teams)
    {
        if (team.GetName() == teamName)
        {
            m_winner = team;
            break;
        }
    }
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
        false
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
