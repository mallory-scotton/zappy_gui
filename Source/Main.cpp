///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Args.hpp"
#include "Core/Application.hpp"
#include "Errors/Exception.hpp"
#include <string>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    std::string host = "localhost";
    int port = 4242;

    Zappy::Args& args = Zappy::Args::GetInstance();

    args.AddFlags("port", "Server port number", port, true);
    args.AddFlags("host", "Server host address", host, false);

    if (!args.Process(argc, argv))
    {
        return (args.GetExitCode());
    }

    if (host == "localhost")
    {
        host = "127.0.0.1";
    }

    try
    {
        Zappy::Application app(host, port);

        while (app.IsOpen())
        {
            app.Update();
        }
    }
    catch (const Zappy::Exception& error)
    {
        std::cerr << "Zappy Exception: " << error.what() << std::endl;
        return (84);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Unexpected exception: " << error.what() << std::endl;
        return (84);
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred." << std::endl;
        return (84);
    }

    return (0);
}
