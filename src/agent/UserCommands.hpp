// UserCommands.hpp - Command-line interface for NLM users
#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

class UserCommands {
public:
    // Define command types
    enum class CommandType {
        HELP,
        CONFIG,
        CREATE,
        RUN,
        DEBUG,
        SAVE,
        LOAD,
        EXPORT,
        ANALYZE,
        VALIDATE,
        VERSION,
        UNKNOWN
    };
    
    // Define command structure
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> parameters;
        CommandType type;
        bool requiresBrain = false;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    UserCommands();
    ~UserCommands() = default;
    
    // Parse and execute command line arguments
    bool execute(int argc, char* argv[], std::shared_ptr<class Brain> brain = nullptr);
    
    // Display help information
    void displayHelp() const;
    void displayConfigHelp() const;
    void displayCreateHelp() const;
    void displayRunHelp() const;
    
    // Command handlers
    void handleHelp(const std::vector<std::string>& args);
    void handleConfig(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleCreate(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleRun(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleDebug(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleSave(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleLoad(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleExport(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleAnalyze(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleValidate(const std::vector<std::string>& args, std::shared_ptr<class Brain> brain);
    void handleVersion(const std::vector<std::string>& args);
    
    // Command validation
    bool validateConfigFile(const std::string& configFile) const;
    bool validateBrainExists(std::shared_ptr<class Brain> brain) const;
    
    // Configuration file management
    void saveConfig(const std::string& filename, const std::map<std::string, std::string>& config) const;
    bool loadConfig(const std::string& filename, std::map<std::string, std::string>& config) const;
    
    // Brain setup utilities
    void createDefaultBrain(std::shared_ptr<class Brain> brain, const std::map<std::string, std::string>& params);
    void applyConfigToBrain(std::shared_ptr<class Brain> brain, const std::map<std::string, std::string>& config);
    
    // File export utilities
    void exportBrainState(std::shared_ptr<class Brain> brain, const std::string& filename, const std::string& format = "checkpoint");
    void exportNeuralData(std::shared_ptr<class Brain> brain, const std::string& filename);
    void exportActivityData(std::shared_ptr<class Brain> brain, const std::string& filename);
    
    // Analysis and validation
    void analyzeBrainState(std::shared_ptr<class Brain> brain);
    void validateBrainHealth(std::shared_ptr<class Brain> brain);
    
private:
    std::vector<Command> commands_;
    std::map<std::string, Command> commandMap_;
    
    // Initialize commands
    void initializeCommands();
    
    // Parse command line
    std::pair<CommandType, std::vector<std::string>> parseCommandLine(int argc, char* argv[]) const;
    
    // Format error messages
    void displayError(const std::string& message) const;
    void displayWarning(const std::string& message) const;
    
    // Get command type from string
    CommandType getCommandType(const std::string& command) const;
};

// Convenience functions for common operations

// Quick analysis command
void quickAnalyze(std::shared_ptr<class Brain> brain);

// Validate brain configuration
bool validateBrainConfiguration(std::shared_ptr<class Brain> brain);

// Export brain to file with automatic format detection
void exportBrain(std::shared_ptr<class Brain> brain, const std::string& filename);

// Load and validate configuration file
bool loadAndValidateConfig(const std::string& filename, std::shared_ptr<class Brain> brain);

// Create and initialize brain with custom parameters
std::shared_ptr<class Brain> createCustomBrain(const std::map<std::string, std::string>& parameters);

} // namespace nlm
