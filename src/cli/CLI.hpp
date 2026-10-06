// Command Line Interface Framework for NLM
// Provides modern CLI with help text, argument validation, and subcommands

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace nlm {

struct CLICommand {
    std::string name;
    std::string description;
    std::string usage;
    std::function<void(const std::vector<std::string>&)> handler;
};

class CLI {
private:
    std::vector<CLICommand> commands;
    std::string programName;
    bool helpRequested;
    bool versionRequested;
    std::string configFile;
    std::string outputFile;
    bool verbose;
    std::vector<std::string> extraArgs;
    
public:
    CLI(int argc, char* argv[]);
    
    void addCommand(const CLICommand& cmd);
    void showHelp() const;
    void showVersion() const;
    bool parseArguments();
    void run() const;
    
    // Getters
    bool isHelpRequested() const { return helpRequested; }
    bool isVersionRequested() const { return versionRequested; }
    const std::string& getConfigFile() const { return configFile; }
    const std::string& getOutputFile() const { return outputFile; }
    bool isVerbose() const { return verbose; }
    const std::vector<std::string>& getExtraArgs() const { return extraArgs; }
    
private:
    void setupDefaultCommands();
    void setupPhase2Commands();
    void setupExperimentCommands();
    void setupDebugCommands();
    void setupOutputCommands();
    std::string formatUsage(const CLICommand& cmd) const;
};

} // namespace nlm

#endif // NLM_CLI_HPP