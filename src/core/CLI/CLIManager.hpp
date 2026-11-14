// NLM Command Line Interface Manager
// Provides unified control over NLM phases, demos, and utilities

#ifndef NLM_CLI_MANAGER_HPP
#define NLM_CLI_MANAGER_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

struct CLICommand {
    std::string name;
    std::string description;
    std::string usage;
    std::function<bool(int argc, char** argv)> handler;
    std::string category;
    bool requiresBuild;
    std::map<std::string, std::string> options;
};

struct CLIContext {
    std::map<std::string, std::string> config;
    std::string logLevel;
    std::string outputFile;
    bool verbose;
    bool quiet;
    std::vector<std::string> args;
};

class CLIManager {
public:
    CLIManager();
    ~CLIManager();
    
    // Command registration
    void registerCommand(const CLICommand& cmd);
    void registerDemo(const std::string& name, const std::string& description, bool requiresBuild);
    void registerPhase(const std::string& phaseName, const std::string& description, bool requiresBuild);
    
    // Command execution
    bool execute(const std::string& command, int argc, char** argv);
    bool executeHelp(const std::string& command = "");
    
    // Configuration and options
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    
    // Utility functions
    bool parseOptions(int argc, char** argv, CLIContext& ctx);
    bool validateCommand(const std::string& command);
    
    // Demo and phase execution helpers
    bool runDemo(const std::string& demoName, int argc, char** argv);
    bool runPhase(const std::string& phaseName, int argc, char** argv);
    
    // Version and info
    std::string getVersion() const;
    std::string getBuildInfo() const;
    
    // Configuration file management
    bool loadConfigFile(const std::string& filename);
    bool saveConfigFile(const std::string& filename) const;
    
    // Logging and output management
    void configureLogging(const std::string& level);
    void redirectOutput(const std::string& filename);

private:
    std::map<std::string, CLICommand> commands;
    std::map<std::string, std::string> demos;
    std::map<std::string, std::string> phases;
    std::map<std::string, std::string> configOptions;
    
    CLIContext context;
    bool configLoaded;
    
    bool executeBuiltIn(const std::string& command, int argc, char** argv);
    bool executeExternal(const std::string& command, int argc, char** argv);
    bool buildRequired(const std::string& command) const;
    void printWelcome() const;
    void printCommandsByCategory() const;
    std::vector<std::string> getAvailableCommands() const;
    bool setupDemoEnvironment(const std::string& demoName);
    bool setupPhaseEnvironment(const std::string& phaseName);
};

} // namespace nlm

#endif // NLM_CLI_MANAGER_HPP
