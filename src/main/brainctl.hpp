#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>
#include <unordered_map>
#include <algorithm>

// Include NLM brain headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {

// Command-line tool headers
class BrainCtlTool;
class Command;
class Subcommand;

// Tool types
enum class ToolType {
    BRAINCTL,
    NLMBRAIN,
    NLMDemo,
    NLMTrace,
    NLMStats,
    NLMExport,
    NLMSaveLoad,
    NLMCompare,
    NLMReplay
};

// Command execution modes
enum class ExecutionMode {
    INTERACTIVE,
    SCRIPTED,
    BATCH
};

// Command result structure
struct CommandResult {
    bool success;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::string outputFile;
    std::chrono::milliseconds executionTime;
    
    CommandResult(bool s, const std::string& msg) : 
        success(s), message(msg), executionTime(0) {}
};

// Base Command class
class Command {
protected:
    std::string name;
    std::string description;
    ToolType toolType;
    
public:
    Command(const std::string& cmdName, const std::string& cmdDesc, ToolType type)
        : name(cmdName), description(cmdDesc), toolType(type) {}
    
    virtual ~Command() = default;
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    ToolType getToolType() const { return toolType; }
    
    virtual CommandResult execute(const std::vector<std::string>& args,
                                ExecutionMode mode = ExecutionMode::INTERACTIVE,
                                std::shared_ptr<Brain> brain = nullptr,
                                std::shared_ptr<Environment> env = nullptr,
                                std::shared_ptr<Config> config = nullptr) = 0;
    
    virtual void displayHelp() const = 0;
};

// Subcommand class for tool organization
class Subcommand {
protected:
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<Command>> commands;
    
public:
    Subcommand(const std::string& subcmdName, const std::string& subcmdDesc)
        : name(subcmdName), description(subcmdDesc) {}
    
    void addCommand(std::shared_ptr<Command> cmd) {
        commands.push_back(cmd);
    }
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    
    bool executeCommand(const std::string& cmdName,
                       const std::vector<std::string>& args,
                       ExecutionMode mode,
                       std::shared_ptr<Brain> brain,
                       std::shared_ptr<Environment> env,
                       std::shared_ptr<Config> config,
                       std::map<std::string, std::string>& sessionState);
    
    void displayHelp() const;
    void displayCommands() const;
};

// BrainCtl Tool main class
class BrainCtlTool {
private:
    std::string version;
    std::vector<std::shared_ptr<Subcommand>> subcommands;
    std::map<std::string, std::string> config;
    std::shared_ptr<Logger> logger;
    bool running;
    std::thread inputThread;
    std::atomic<bool> stopRequested;
    
    // Session state
    std::map<std::string, std::string> sessionState;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    std::shared_ptr<Config> currentConfig;
    SimulationClock simulationClock;
    
public:
    BrainCtlTool(const std::string& ver = "1.0.0");
    ~BrainCtlTool();
    
    void initialize();
    int run(int argc, char** argv);
    
    // Configuration commands
    void setupConfigCommands();
    
    // Execution commands
    void setupExecutionCommands();
    
    // Analysis commands
    void setupAnalysisCommands();
    
    // Debug commands
    void setupDebugCommands();
    
    // Interactive mode
    void startInteractiveMode();
    void handleInteractiveInput();
    void displayPrompt() const;
    
    // Command parsing
    bool parseCommand(const std::string& input,
                     std::string& subcommandName,
                     std::string& commandName,
                     std::vector<std::string>& args);
    
    // File operations
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    bool exportBrainState(const std::string& filepath) const;
    
    // Help and documentation
    void displayMainHelp() const;
    void displaySubcommandHelp(const std::string& subcommandName) const;
    void displayWelcome() const;
    
    // Configuration management
    bool loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath) const;
    void displayConfig() const;
    
    // Session management
    void resetSession();
    void logCommand(const std::string& command, bool success, const std::string& result);
    
    // Tool access
    std::shared_ptr<Brain> getCurrentBrain() const { return currentBrain; }
    std::shared_ptr<Environment> getCurrentEnvironment() const { return currentEnvironment; }
    std::shared_ptr<Config> getCurrentConfig() const { return currentConfig; }
    
    bool isRunning() const { return running; }
};

// BrainCtl command implementations
class BrainCtlConfigCommand : public Command {
public:
    BrainCtlConfigCommand() : Command("config", "Manage NLM configuration", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlRunCommand : public Command {
public:
    BrainCtlRunCommand() : Command("run", "Run brain simulation", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlStepCommand : public Command {
public:
    BrainCtlStepCommand() : Command("step", "Execute single simulation step", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlPauseCommand : public Command {
public:
    BrainCtlPauseCommand() : Command("pause", "Pause brain simulation", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlResetCommand : public Command {
public:
    BrainCtlResetCommand() : Command("reset", "Reset brain state", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlExportCommand : public Command {
public:
    BrainCtlExportCommand() : Command("export", "Export brain data", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlAnalyzeCommand : public Command {
public:
    BrainCtlAnalyzeCommand() : Command("analyze", "Analyze brain state", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

class BrainCtlDebugCommand : public Command {
public:
    BrainCtlDebugCommand() : Command("debug", "Debug brain system", ToolType::BRAINCTL) {}
    
    CommandResult execute(const std::vector<std::string>& args,
                        ExecutionMode mode,
                        std::shared_ptr<Brain> brain,
                        std::shared_ptr<Environment> env,
                        std::shared_ptr<Config> config) override;
    
    void displayHelp() const override;
};

} // namespace nlm
