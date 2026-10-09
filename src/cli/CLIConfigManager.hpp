#pragma once

#include <string>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

class CLIConfigManager {
public:
    CLIConfigManager();
    ~CLIConfigManager();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleListCommand(int argc, char** argv);
    bool handleSetCommand(int argc, char** argv);
    bool handleGetCommand(int argc, char** argv);
    bool handleSaveCommand(int argc, char** argv);
    bool handleLoadCommand(int argc, char** argv);

    // Print help messages
    void printHelp();
    void printSetHelp();
    void printGetHelp();
    void printSaveHelp();
    void printLoadHelp();

private:
    std::shared_ptr<Config> config_;
};

} // namespace nlm