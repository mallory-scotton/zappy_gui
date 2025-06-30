///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Game/GameState.hpp"
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
GameState::GameState(const std::string& host, int port)
    : m_socket(AF_INET, SOCK_STREAM)
    , m_isConnected(false)
    , m_host(host)
    , m_port(port)
{}

///////////////////////////////////////////////////////////////////////////////
bool GameState::Connect(void)
{
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

} // !namespace Zappy
