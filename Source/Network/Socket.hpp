///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Socket
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private member properties
    ///////////////////////////////////////////////////////////////////////////
    int m_fd;   //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Socket(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param domain
    /// \param type
    /// \param protocol
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit Socket(int domain, int type, int protocol = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Socket(const Socket&) = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param other
    ///
    ///////////////////////////////////////////////////////////////////////////
    Socket(Socket&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~Socket();

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Socket& operator=(const Socket&) = delete;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param other
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Socket& operator=(Socket&& other) noexcept;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Close(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    int Get(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsValid(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param host
    /// \param port
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Connect(const std::string& host, int port);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param data
    /// \param size
    /// \param flags
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    ssize_t Send(const void* data, size_t size, int flags = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Send a string over the socket
    ///
    /// \param message The string message to send
    /// \param flags Optional flags for send operation
    ///
    /// \return Number of bytes sent, or -1 on error
    ///
    ///////////////////////////////////////////////////////////////////////////
    ssize_t Send(const std::string& message, int flags = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param buffer
    /// \param size
    /// \param flags
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    ssize_t Recv(void* buffer, size_t size, int flags = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Receive data as a string from the socket
    ///
    /// \param size Maximum number of bytes to receive
    /// \param flags Optional flags for recv operation
    ///
    /// \return The received string (may be empty if no data or error)
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::string Recv(size_t size, int flags = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Receive a line (until newline character) from the socket
    ///
    /// \param flags Optional flags for recv operation
    ///
    /// \return The received line without the newline character
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::string RecvLine(int flags = 0);
};

} // !namespace Zappy
