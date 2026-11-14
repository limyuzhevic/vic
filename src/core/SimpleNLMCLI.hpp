// NLM Phase 2: Easy Command Line Interface
// Simplified interface for NLM Phase 2 operations

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace nlm {

// Simple configuration manager
class SimpleConfig {
public:
    SimpleConfig() = default;
    ~SimpleConfig() = default;
    
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    bool validate() const;
    
    // Configuration validation
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const std::string& value) const;
    std::vector<std::string> getErrorMessages() const { return errorMessages; }

private:
    std::map<std::string, std::string> configValues;
    std::vector<std::string> errorMessages;
    
    // Valid configuration keys
    static const std::vector<std::string> validKeys;
    static const std::map<std::string, std::string> keyDescriptions;
};

// Command structure for easy CLI
struct SimpleCommand {
    std::string name;
    std::string description;
    std::string usage;
    std::function<bool(const std::vector<std::string>&)> handler;
};

// Easy-to-use NLM CLI
class SimpleNLMCLI {
public:
    SimpleNLMCLI();
    ~SimpleNLMCLI() = default;
    
    // Main command execution
    bool execute(int argc, char** argv);
    bool execute(const std::string& command, const std::vector<std::string>& args);
    
    // Demo and phase execution
    bool runPhase2(const std::vector<std::string>& args);
    bool runPhase3(const std::vector<std::string>& args);
    bool runPhase4(const std::vector<std::string>& args);
    bool runPhase6(const std::vector<std::string>& args);
    bool runBenchmark(const std::vector<std::string>& args);
    
    // Convenience commands
    bool runQuick(const std::vector<std::string>& args);
    bool runQuickTest(const std::vector<std::string>& args);
    bool runBenchmarkQuick(const std::vector<std::string>& args);
    
    // Configuration commands
    bool configure(const std::vector<std::string>& args);
    bool saveConfig(const std::vector<std::string>& args);
    bool loadConfig(const std::vector<std::string>& args);
    
    // Help and information
    bool help(const std::vector<std::string>& args);
    bool version(const std::vector<std::string>& args);
    bool info(const std::vector<std::string>& args);
    
    // Utility commands
    bool setEnv(const std::vector<std::string>& args);
    bool showEnv(const std::vector<std::string>& args);
    bool validateConfig(const std::vector<std::string>& args);

private:
    SimpleConfig config;
    bool verboseMode;
    bool quietMode;
    
    // Registered commands
    std::map<std::string, SimpleCommand> commands;
    
    // Command registration
    void registerCommands();
    void registerDemoCommands();
    void registerPhaseCommands();
    void registerUtilityCommands();
    
    // Command execution
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    bool executeDemo(const std::string& demo, const std::vector<std::string>& args);
    bool executePhase(const std::string& phase, const std::vector<std::string>& args);
    
    // Configuration helpers
    bool applyDemoConfig(const std::string& demo);
    bool applyPhaseConfig(const std::string& phase);
    bool validateConfigFile(const std::string& filename);
    
    // Output formatting
    void printWelcome() const;
    void printHelp() const;
    void printDemoHelp() const;
    void printPhaseHelp() const;
    void printQuickHelp() const;
    void printConfigurationHelp() const;
    
    // Quick execution helpers
    bool executeQuickPhase(int phase, const std::vector<std::string>& args);
    bool executeQuickDemo(const std::string& demo, const std::vector<std::string>& args);
    
    // Error handling
    void printError(const std::string& message);
    void printWarning(const std::string& message);
    void printSuccess(const std::string& message);
    void printInfo(const std::string& message);
};

} // namespace nlm
