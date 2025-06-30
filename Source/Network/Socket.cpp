///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Network/Socket.hpp"
#include "Errors/NetworkException.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
Socket::Socket(void)
    : m_fd(-1)
{}

///////////////////////////////////////////////////////////////////////////////
Socket::Socket(int domain, int type, int protocol)
    : m_fd(socket(domain, type, protocol))
{
    if (m_fd < 0)
    {
        throw NetworkException("Failed to create socket");
    }
}

///////////////////////////////////////////////////////////////////////////////
Socket::Socket(Socket&& other) noexcept
    : m_fd(other.m_fd)
{
    other.m_fd = -1;
}

///////////////////////////////////////////////////////////////////////////////
Socket::~Socket()
{
    Close();
}

///////////////////////////////////////////////////////////////////////////////
Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other)
    {
        Close();
        m_fd = other.m_fd;
        other.m_fd = -1;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
void Socket::Close(void)
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
int Socket::Get(void) const
{
    return (m_fd);
}

///////////////////////////////////////////////////////////////////////////////
bool Socket::IsValid(void) const
{
    return (m_fd >= 0);
}

///////////////////////////////////////////////////////////////////////////////
void Socket::Connect(const std::string& host, int port)
{
    if (!IsValid())
    {
        throw NetworkException("Invalid socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
    {
        throw NetworkException("Invalid address");
    }

    if (::connect(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw NetworkException("Connection failed");
    }
}

///////////////////////////////////////////////////////////////////////////////
ssize_t Socket::Send(const void* data, size_t size, int flags)
{
    if (!IsValid())
    {
        return (-1);
    }
    return (::send(m_fd, data, size, flags));
}

///////////////////////////////////////////////////////////////////////////////
ssize_t Socket::Send(const std::string& message, int flags)
{
    if (!IsValid())
    {
        return (-1);
    }
    return (::send(m_fd, message.c_str(), message.length(), flags));
}

///////////////////////////////////////////////////////////////////////////////
ssize_t Socket::Recv(void* buffer, size_t size, int flags)
{
    if (!IsValid())
    {
        return (-1);
    }
    return (::recv(m_fd, buffer, size, flags));
}

///////////////////////////////////////////////////////////////////////////////
std::string Socket::Recv(size_t size, int flags)
{
    if (!IsValid())
    {
        return ("");
    }

    std::string buffer(size, '\0');
    ssize_t received = ::recv(m_fd, &buffer[0], size, flags);

    if (received == 0)
    {
        Close();
        return ("");
    }
    else if (received < 0)
    {
        if (errno == ECONNRESET || errno == ENOTCONN || errno == EPIPE)
        {
            Close();
        }
        return ("");
    }

    buffer.resize(received);
    return (buffer);
}

///////////////////////////////////////////////////////////////////////////////
std::string Socket::RecvLine(int flags)
{
    if (!IsValid())
    {
        return ("");
    }

    std::string line;
    char ch;
    ssize_t received;

    while ((received = ::recv(m_fd, &ch, 1, flags)) > 0)
    {
        if (ch == '\n')
        {
            break;
        }
        if (ch != '\r')  // Skip carriage return characters
        {
            line += ch;
        }
    }

    if (received == 0)
    {
        Close();
    }
    else if (received < 0)
    {
        if (errno == ECONNRESET || errno == ENOTCONN || errno == EPIPE)
        {
            Close();
        }
    }

    return (line);
}

} // !namespace Zappy
