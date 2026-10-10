// NLM Advanced CLI - Command Factory
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <typeinfo>

namespace nlm {

// Forward declarations
class ICLInterface;
class Config;
class Brain;
class terminal_renderer;
class debug_tools;

// Global state for CLI
struct CLIExecutionState {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<terminal_renderer> terminalRenderer;
    std::shared_ptr<debug_tools> debugTools;
    bool brainInitialized = false;
    uint64_t simulationStep = 0;
    double simulationTime = 0.0;
};

// Global execution state
CLIExecutionState globalState;

// Base command implementation
class BaseCommand : public ICLInterface {
public:
    BaseCommand(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    virtual ~BaseCommand() = default;
    
    std::string getName() const override { return name_; }
    std::string getDescription() const { return description_; }
    
    CLIResult execute(const CLICommand& cmd) override {
        return executeImpl(cmd.options);
    }
    
    void displayHelp() const override {
        displayHelpImpl();
    }
    
protected:
    virtual CLIResult executeImpl(const std::map<std::string, std::string>& options) = 0;
    virtual void displayHelpImpl() const = 0;
    
    std::string name_;
    std::string description_;
};

// Command factory for creating and managing CLI commands
class CommandFactory {
public:
    static CommandFactory& getInstance() {
        static CommandFactory instance;
        return instance;
    }
    
    // Register a command implementation
    template<typename CommandType>
    void registerCommand() {
        auto cmd = std::make_unique<CommandType>();
        commands_[cmd->getName()] = std::move(cmd);
    }
    
    // Execute a command by name
    CLIResult execute(const std::string& commandName, const std::map<std::string, std::string>& options) {
        auto it = commands_.find(commandName);
        if (it != commands_.end()) {
            return it->second->execute(CLICommand{commandName, "", {}, {}, options});
        }
        return CLIResult::Error;
    }
    
    // Get command description
    std::string getDescription(const std::string& commandName) const {
        auto it = commands_.find(commandName);
        if (it != commands_.end()) {
            return it->second->getDescription();
        }
        return "Unknown command";
    }
    
    // List all available commands
    std::vector<std::string> listCommands() const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Set global state dependencies
    void setConfig(std::shared_ptr<Config> config) { globalState.config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { 
        globalState.brain = brain; 
        globalState.brainInitialized = (brain != nullptr);
    }
    void setTerminalRenderer(std::shared_ptr<terminal_renderer> renderer) { globalState.terminalRenderer = renderer; }
    void setDebugTools(std::shared_ptr<debug_tools> debug) { globalState.debugTools = debug; }
    
    // Get global state
    static CLIExecutionState& getState() { return globalState; }
    
private:
    CommandFactory() = default;
    ~CommandFactory() = default;
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<ICLInterface>> commands_;
};

} // namespace nlm

