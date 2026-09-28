#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/Phase3Demo.hpp"
#include "../experiments/Phase4Demo.hpp"
#include "../experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace nlm {

class NLMCli {
public:
    struct CommandArgs {
        std::string command;
        std::map<std::string, std::string> options;
        std::vector<std::string> positionalArgs;
        bool helpRequested = false;
        bool versionRequested = false;
    };

    NLMCli() : helpText(generateHelpText()) {}
    ~NLMCli() {}
    
    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);
    
    // Execute the parsed command
    void executeCommand();
    
    // Generate help text
    std::string generateHelpText() const;

private:
    CommandArgs args;
    std::string helpText;
    
    // Command execution methods
    void executeRunCommand() const;
    void executeConfigCommand() const;
    void executeDemoCommand(const std::string& demoName) const;
    void executeListCommand() const;
    void executeHelpCommand() const;
    void executeVersionCommand() const;
    
    // Utility methods
    void printUsage(const std::string& command) const;
    std::shared_ptr<Config> createConfigFromArgs() const;
    std::string formatOptions(const std::map<std::string, std::string>& options) const;
};

} // namespace nlm