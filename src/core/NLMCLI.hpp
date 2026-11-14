// NLM Phase 2 Simplified CLI
// Provides easy-to-use command-line interface for NLM demos

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace nlm {

struct NLMCommand {
    std::string name;
    std::string description;
    std::string usage;
    std::function<bool(const std::vector<std::string>&)> handler;
    std::string category;
    std::map<std::string, std::string> options;
};

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI() = default;
    
    // Command execution
    bool execute(const std::string& command, const std::vector<std::string>& args);
    bool execute(const std::string& command, const std::vector<std::string>& args, 
                 std::ostream& outputStream);
    
    // Command management
    void registerCommand(const NLMCommand& cmd);
    void registerDemo(const std::string& name, const std::string& description);
    void registerShortcut(const std::string& shortcut, const std::string& targetCommand);
    
    // Help and information
    bool showHelp(const std::string& command = "");
    bool showVersion();
    bool showInfo(const std::string& category = "");
    
    // Configuration and utilities
    bool parseArguments(int argc, char** argv, std::vector<std::string>& args);
    bool saveConfig(const std::string& filename) const;
    bool loadConfig(const std::string& filename);
    bool validateConfig(const std::string& filename);
    
    // Convenience commands
    bool runPhaseDemo(const std::string& phaseName);
    bool runBenchmark(const std::vector<std::string>& args);
    bool runInteractiveMode();
    bool runAnalysisMode(const std::vector<std::string>& args);

private:
    std::map<std::string, NLMCommand> commands;
    std::map<std::string, std::string> demos;
    std::map<std::string, std::string> shortcuts;
    std::map<std::string, std::string> config;
    
    std::ostream* outputStream;
    std::string defaultOutputFile;
    
    // Command execution helpers
    bool executeCommand(const NLMCommand& cmd, const std::vector<std::string>& args);
    bool executeDemo(const std::string& demoName, const std::vector<std::string>& args);
    bool executeShortcut(const std::string& shortcut, const std::vector<std::string>& args);
    
    // Configuration helpers
    bool applyDemoConfig(const std::string& demoName);
    bool applyPhaseConfig(const std::string& phaseName);
    bool validateAndLoadConfig(const std::string& filename);
    
    // Output helpers
    void printWelcome() const;
    void printCategories() const;
    void printCommandsInCategory(const std::string& category) const;
    std::vector<std::string> getAvailableCommands() const;
    std::vector<std::string> getAvailableCommandsInCategory(const std::string& category) const;
    
    // Quick execution helpers
    bool executeQuickPhase(int phase, const std::vector<std::string>& args);
    bool executeQuickDemo(const std::string& demo, const std::vector<std::string>& args);
    bool executeQuickBenchmark(const std::vector<std::string>& args);
};

} // namespace nlm
