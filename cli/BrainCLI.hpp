#ifndef BRAINCLI_HPP
#define BRAINCLI_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <chrono>
#include <thread>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace nlm {
namespace cli {

class Command;
class CLIApplication;

// Command argument definition
struct Argument {
    std::string name;
    std::string description;
    std::string type;
    std::string defaultValue;
    bool required;
    std::vector<std::string> choices;
    std::function<bool(const std::string&)> validator;
};

// Command definition
class Command {
public:
    std::string name;
    std::string description;
    std::string category;
    std::vector<Argument> arguments;
    std::function<int(const std::map<std::string, std::string>&)> execute;
    std::function<void()> help;
    
    Command(const std::string& name, const std::string& description, 
            const std::string& category = "General")
        : name(name), description(description), category(category) {}
    
    virtual ~Command() = default;
    
    virtual int executeCommand(const std::map<std::string, std::string>& args) {
        return execute(args);
    }
    
    virtual void displayHelp() const {
        if (help) help();
    }
};

// Subcommand for hierarchical command structure
class SubCommand : public Command {
public:
    std::vector<Command*> subCommands;
    
    SubCommand(const std::string& name, const std::string& description,
               const std::string& category = "General")
        : Command(name, description, category) {}
    
    void addSubCommand(Command* cmd) {
        subCommands.push_back(cmd);
    }
    
    Command* findSubCommand(const std::string& name) {
        for (auto* cmd : subCommands) {
            if (cmd->name == name) return cmd;
        }
        return nullptr;
    }
};

// CLI Application with modern features
class CLIApplication {
public:
    CLIApplication(const std::string& name, const std::string& version);
    ~CLIApplication();
    
    void setVersion(const std::string& version) { version_ = version; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    // Command management
    void addCommand(Command* command);
    void addSubCommand(SubCommand* subCommand);
    
    // Interactive and batch mode support
    bool runInteractive();
    bool runBatch(const std::string& script);
    
    // Configuration
    void loadConfig(const std::string& configFile);
    void saveConfig(const std::string& configFile);
    
    // Plugin system
    void loadPlugin(const std::string& pluginPath);
    void listPlugins() const;
    
    // Tab completion
    std::vector<std::string> getCompletions(const std::string& input) const;
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(const std::string& commandName) const;
    void showVersion() const;
    
    // Logging and reporting
    void enableLogging(bool enable) { loggingEnabled_ = enable; }
    void setLogLevel(LogLevel level) { logLevel_ = level; }
    
    // Error handling
    void setErrorHandling(bool strict) { strictErrorHandling_ = strict; }
    
    // State management
    void saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> loadBrainState(const std::string& filename);
    
    // Resource management
    void setMaxMemoryUsage(size_t maxBytes) { maxMemoryUsage_ = maxBytes; }
    void setCpuAffinity(const std::vector<int>& cores) { cpuAffinity_ = cores; }
    
protected:
    std::string name_;
    std::string version_;
    std::string description_;
    bool loggingEnabled_;
    LogLevel logLevel_;
    bool strictErrorHandling_;
    size_t maxMemoryUsage_;
    std::vector<int> cpuAffinity_;
    
    std::vector<Command*> commands_;
    std::vector<SubCommand*> subCommands_;
    std::map<std::string, std::string> config_;
    
    // Parser state
    std::string currentInput_;
    std::vector<std::string> commandHistory_;
    
    // Plugin system
    std::vector<std::string> loadedPlugins_;
    
    // Help display
    void displayWelcome() const;
    void displayPrompt() const;
    std::string parseCommandLine(const std::string& input) const;
    bool executeParsedCommand(const std::string& commandLine);
    
    // Configuration helpers
    void initDefaultConfig();
    void applyConfig() const;
};

// Template for creating commands easily
class CommandFactory {
public:
    static Command* createBrainCommand(const std::string& operation);
    static Command* createVisualizationCommand(const std::string& type);
    static Command* createAnalysisCommand(const std::string& analysis);
    static Command* createPluginCommand(const std::string& pluginOp);
};

} // namespace cli
} // namespace nlm

#endif // BRAINCLI_HPP
