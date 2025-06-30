///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils/Args.hpp"
#include <iostream>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
// Namespace Zappy
///////////////////////////////////////////////////////////////////////////////
namespace Zappy
{

///////////////////////////////////////////////////////////////////////////////
Args::Args(void)
    : m_exitCode(84)
{}

///////////////////////////////////////////////////////////////////////////////
void Args::PrintUsage(char *prog) const
{
    std::cout << "Usage: " << prog;

    for (const auto& flag : m_flags)
    {
        std::cout << " " << (flag.mandatory ? '<' : '[');
        std::cout << flag.vlong.substr(2) << (flag.mandatory ? '>' : ']');
    }
    std::cout << "\n\n";

    std::cout << "Options:\n";

    for (const auto& flag : m_flags)
    {
        std::cout << "  " << std::setw(4) << flag.vshort
                  << ", " << std::setw(12) << flag.vlong
                  << "  " << flag.description;
        if (flag.mandatory)
        {
            std::cout << " (required)";
        }
        else
        {
            std::cout << " (default: " << flag.getter() << ")";
        }
        std::cout << "\n";
    }

    std::cout << "\nExample:\n  ";
    std::cout << prog <<  " --host 127.0.0.1 --port 4242\n";
}

///////////////////////////////////////////////////////////////////////////////
bool Args::Process(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-help")
        {
            PrintUsage(argv[0]);
            m_exitCode = 0;
            return (false);
        }

        auto it = m_flagMap.find(arg);
        if (it == m_flagMap.end())
        {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            PrintUsage(argv[0]);
            return (false);
        }

        size_t flagIndex = it->second;

        if (i + 1 >= argc)
        {
            std::cerr << "Error: Flag '" << arg << "' requires a value\n";
            PrintUsage(argv[0]);
            return (false);
        }

        // Get the value and call the setter
        std::string value = argv[++i];
        try
        {
            m_flags[flagIndex].setter(value);
            m_flags[flagIndex].found = true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: Invalid value '" << value
                      << "' for flag '" << arg << "': " << e.what() << "\n";
            PrintUsage(argv[0]);
            return (false);
        }
    }

    if (!ValidateMandatoryFlags())
    {
        PrintUsage(argv[0]);
        return (false);
    }

    m_exitCode = 0;

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
bool Args::ValidateMandatoryFlags(void) const
{
    bool allMandatoryFound = true;

    for (const auto& flag : m_flags)
    {
        if (flag.mandatory && !flag.found)
        {
            std::cerr << "Error: Required flag '"
                      << flag.vlong << "' not provided\n";
            allMandatoryFound = false;
        }
    }

    return (allMandatoryFound);
}

///////////////////////////////////////////////////////////////////////////////
int Args::GetExitCode(void) const
{
    return (m_exitCode);
}

} // !namespace Zappy
