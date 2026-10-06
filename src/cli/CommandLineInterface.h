// NLM CLI (Command Line Interface)
// Provides user-friendly access to NLM neural simulation capabilities

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Run CLI with command line arguments
    int run(int argc, char** argv);
    
    // Command registration for extensibility
    void registerCommand(const std::string& name, 
                        const std::string& description,
                        std::function<int(const std::vector<std::string>&)> handler);
    
    // Display help information
    void displayHelp() const;
    void displayVersion() const;
    
    // Configuration management
    bool loadConfig(const std::string& configPath, std::shared_ptr<Config>& config);
    void applyConfigOverrides(const std::vector<std::string>& args, std::shared_ptr<Config>& config);
    
private:
    // Command execution
    int executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Core commands
    int handleRun(const std::vector<std::string>& args);
    int handleCreate(const std::vector<std::string>& args);
    int handleSave(const std::vector<std::string>& args);
    int handleLoad(const std::vector<std::string>& args);
    int handleStatus(const std::vector<std::string>& args);
    int handleTest(const std::vector<std::string>& args);
    int handleDemo(const std::vector<std::string>& args);
    
    // Advanced commands
    int handleBenchmark(const std::vector<std::string>& args);
    int handleReplay(const std::vector<std::string>& args);
    int handleAnalyze(const std::vector<std::string>& args);
    int handleExport(const std::vector<std::string>& args);
    int handleMonitor(const std::vector<std::string>& args);
    
    // Utility commands
    int handleCheck(const std::vector<std::string>& args);
    int handleReset(const std::vector<std::string>& args);
    int handleInfo(const std::vector<std::string>& args);
    
    // Command definitions
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<int(const std::vector<std::string>&)> handler;
        bool requiresBrain;
        bool isAdvanced;
    };
    
    std::vector<Command> commands_;
    std::map<std::string, Command> commandMap_;
    
    // State management
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<Config> currentConfig_;
    std::shared_ptr<AgentBrain> currentAgent_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    
    // Configuration
    std::string defaultConfigPath_;
    bool verboseMode_;
    bool quietMode_;
    
    // Error handling
    void displayError(const std::string& message, bool isFatal = false) const;
    bool validateArguments(const std::string& command, const std::vector<std::string>& args) const;
};

} // namespace nlm

#endif // NLM_CLI_H
