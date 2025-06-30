///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Args.hpp"
#include <string>

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

    return (0);
}
