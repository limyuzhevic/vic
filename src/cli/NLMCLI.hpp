#pragma once

#include "core/Config/Config.hpp"

namespace nlm {

class NLMCLI {
public:
    NLMCLI(int argc, char** argv);
    ~NLMCLI();

    // Disable copying, enable moving
    NLMCLI(const NLMCLI&) = delete;
    NLMCLI& operator=(const NLMCLI&) = delete;
    NLMCLI(NLMCLI&&) noexcept;
    NLMCLI& operator=(NLMCLI&&) noexcept;

    // Run the CLI
    bool run();

private:
    void parseArgs();
    void printHelp();
    void printVersion();

    bool runCommand();
    bool configCommand();
    bool experimentCommand();
    bool checkpointCommand();
    bool visualizeCommand();
    bool benchmarkCommand();
    bool profileCommand();

    // Member variables
    int argc;
    char** argv;
    std::string command;
    std::vector<std::string> args;
    std::map<std::string, ConfigValue> globalConfigData;
    Config globalConfig;
    bool helpCommand = false;
    bool versionCommand = false;
    bool verbose = false;
    bool quiet = false;
};

} // namespace nlm