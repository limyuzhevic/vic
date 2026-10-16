#pragma once

// Advanced command system and plugin architecture for NLM
// Provides extensible command framework, plugin management, and advanced features

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <set>
#include <any>
#include <typeindex>

namespace nlm {

// Command categories for organization
enum class CommandCategory {
    BASIC,
    PLOTTING,
    ANALYSIS,
    PLANNING,
    VISUALIZATION,
    DEBUG,
    DEVELOPMENT,
    PLUGINS
};

// Command argument types
enum class ArgumentType {
    FLAG,
    VALUE,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    FILEPATH,
    DIRECTORY,
    ENUM,
    VECTOR
};

// Command argument definition
struct CommandArgument {
    std::string name;
    std::string description;
    ArgumentType type;
    std::any defaultValue;
    bool required = false;
    std::vector<std::string> enumValues;
    std::string validationPattern;
    
    CommandArgument(const std::string& argName, const std::string& argDesc,
                   ArgumentType argType, bool argRequired = false)
        : name(argName), description(argDesc), type(argType), required(argRequired) {}
};

// Command result
struct CommandResult {
    bool success = false;
    std::string message;
    std::any data;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    CommandResult(bool succeed, const std::string& msg = "")
        : success(succeed), message(msg) {}
    
    CommandResult& addWarning(const std::string& warning) {
        warnings.push_back(warning);
        return *this;
    }
    
    CommandResult& addError(const std::string& error) {
        errors.push_back(error);
        success = false;
        return *this;
    }
    
    CommandResult& setData(const std::any& data) {
        this->data = data;
        return *this;
    }
};

// Command interface
class Command {
public:
    virtual ~Command() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual CommandCategory getCategory() const = 0;
    virtual std::vector<CommandArgument> getArguments() const = 0;
    virtual CommandResult execute(const std::map<std::string, std::any>& arguments) = 0;
    virtual bool isDeprecated() const { return false; }
    virtual std::string getDeprecatedMessage() const { return ""; }
    
    virtual bool requiresBrain() const { return false; }
    virtual bool isAsync() const { return false; }
    virtual double getTimeoutSeconds() const { return 30.0; }
    
    // Plugin support
    virtual std::vector<std::string> getDependencies() const { return {}; }
    virtual void setPluginContext(void* context) {}
};

// Command implementation helper
class CommandBuilder {
public:
    CommandBuilder& name(const std::string& name) {
        command_->name_ = name;
        return *this;
    }
    
    CommandBuilder& description(const std::string& desc) {
        command_->description_ = desc;
        return *this;
    }
    
    CommandBuilder& category(CommandCategory category) {
        command_->category_ = category;
        return *this;
    }
    
    CommandBuilder& requiresBrain(bool requires = true) {
        command_->requiresBrain_ = requires;
        return *this;
    }
    
    CommandBuilder& async(bool async = true) {
        command_->async_ = async;
        return *this;
    }
    
    CommandBuilder& timeout(double seconds) {
        command_->timeout_ = seconds;
        return *this;
    }
    
    CommandBuilder& addArgument(const CommandArgument& arg) {
        command_->arguments_.push_back(arg);
        return *this;
    }
    
    CommandBuilder& addFlag(const std::string& name, const std::string& desc) {
        return addArgument(CommandArgument(name, desc, ArgumentType::FLAG));
    }
    
    CommandBuilder& addValue(const std::string& name, const std::string& desc,
                           const std::any& defaultValue = std::any()) {
        CommandArgument arg(name, desc, ArgumentType::VALUE, false);
        arg.defaultValue = defaultValue;
        return addArgument(arg);
    }
    
    CommandBuilder& addString(const std::string& name, const std::string& desc,
                            bool required = false, const std::string& pattern = "") {
        CommandArgument arg(name, desc, ArgumentType::STRING, required);
        arg.validationPattern = pattern;
        return addArgument(arg);
    }
    
    CommandBuilder& addInteger(const std::string& name, const std::string& desc,
                             int64_t min = INT64_MIN, int64_t max = INT64_MAX) {
        CommandArgument arg(name, desc, ArgumentType::INTEGER, false);
        // Store min/max as custom validation
        arg.defaultValue = std::make_pair(min, max);
        return addArgument(arg);
    }
    
    CommandBuilder& addFloat(const std::string& name, const std::string& desc,
                           double min = -std::numeric_limits<double>::infinity(),
                           double max = std::numeric_limits<double>::infinity()) {
        CommandArgument arg(name, desc, ArgumentType::FLOAT, false);
        arg.defaultValue = std::make_pair(min, max);
        return addArgument(arg);
    }
    
    CommandBuilder& addFilepath(const std::string& name, const std::string& desc,
                              bool required = false, bool writable = true) {
        CommandArgument arg(name, desc, ArgumentType::FILEPATH, required);
        // Store writable as custom validation
        arg.defaultValue = writable;
        return addArgument(arg);
    }
    
    template<typename T>
    CommandBuilder& setHandler(std::function<CommandResult(const std::map<std::string, std::any>&)> handler) {
        command_->handler_ = [handler](Command* cmd, const std::map<std::string, std::any>& args) {
            return handler(args);
        };
        return *this;
    }
    
    std::unique_ptr<Command> build() {
        return std::move(command_);
    }
    
protected:
    CommandBuilder() {
        command_ = std::make_unique<BasicCommand>();
    }
    
    class BasicCommand : public Command {
    public:
        std::string name_;
        std::string description_;
        CommandCategory category_ = CommandCategory::BASIC;
        std::vector<CommandArgument> arguments_;
        bool requiresBrain_ = false;
        bool async_ = false;
        double timeout_ = 30.0;
        std::function<CommandResult(const std::map<std::string, std::any>&)> handler_;
        
        std::string getName() const override { return name_; }
        std::string getDescription() const override { return description_; }
        CommandCategory getCategory() const override { return category_; }
        std::vector<CommandArgument> getArguments() const override { return arguments_; }
        bool requiresBrain() const override { return requiresBrain_; }
        bool isAsync() const override { return async_; }
        double getTimeoutSeconds() const override { return timeout_; }
        
        CommandResult execute(const std::map<std::string, std::any>& arguments) override {
            if (handler_) {
                return handler_(arguments);
            }
            return CommandResult(false, "No handler implemented");
        }
    };
    
    std::unique_ptr<BasicCommand> command_;
};

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
    virtual bool initialize(void* context) = 0;
    virtual void shutdown() = 0;
    
    virtual std::vector<std::unique_ptr<Command>> getCommands() const { return {}; }
    virtual std::vector<std::unique_ptr<Plugin>> getSubPlugins() const { return {}; }
};

// Plugin manager
class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager() {
        shutdown();
    }
    
    bool loadPlugin(const std::string& pluginPath, void* context = nullptr) {
        // In a real implementation, this would dynamically load the plugin
        // For now, return false to indicate not implemented
        return false;
    }
    
    void unloadPlugin(const std::string& pluginName) {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            plugins_.erase(it);
        }
    }
    
    bool isPluginLoaded(const std::string& pluginName) const {
        return plugins_.find(pluginName) != plugins_.end();
    }
    
    std::vector<std::string> getLoadedPlugins() const {
        std::vector<std::string> names;
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::unique_ptr<Command>> getAllCommands() const {
        std::vector<std::unique_ptr<Command>> allCommands;
        
        for (const auto& pair : plugins_) {
            const auto& plugin = pair.second;
            auto commands = plugin->getCommands();
            allCommands.insert(allCommands.end(),
                             std::make_move_iterator(commands.begin()),
                             std::make_move_iterator(commands.end()));
        }
        
        return allCommands;
    }
    
    CommandResult executeCommand(const std::string& commandName,
                                const std::map<std::string, std::any>& arguments,
                                void* context = nullptr) {
        auto it = plugins_.find(commandName);
        if (it == plugins_.end()) {
            return CommandResult(false, "Plugin not found: " + commandName);
        }
        
        // For now, just return not implemented
        return CommandResult(false, "Command execution not implemented for plugin: " + commandName);
    }
    
    void initializeAll(void* context) {
        for (auto& pair : plugins_) {
            if (!pair.second->initialize(context)) {
                // Log initialization failure
            }
        }
    }
    
    void shutdownAll() {
        for (auto& pair : plugins_) {
            pair.second->shutdown();
        }
        plugins_.clear();
    }
    
protected:
    std::map<std::string, std::unique_ptr<Plugin>> plugins_;
};

// Command parser
class CommandParser {
public:
    CommandParser(std::vector<std::unique_ptr<Command>> commands)
        : commands_(std::move(commands)) {}
    
    CommandResult parseAndExecute(const std::string& commandLine,
                                 const std::map<std::string, std::any>& context = std::map<std::string, std::any>()) {
        // Parse command line into command name and arguments
        auto parsed = parseCommandLine(commandLine);
        if (!parsed.success) {
            return parsed;
        }
        
        // Find the command
        auto commandIt = findCommand(parsed.commandName);
        if (commandIt == commands_.end()) {
            return CommandResult(false, "Unknown command: " + parsed.commandName);
        }
        
        auto& command = *commandIt;
        
        // Validate arguments
        auto validationResult = validateArguments(command.get(), parsed.arguments);
        if (!validationResult.success) {
            return validationResult;
        }
        
        // Execute command
        try {
            return command->execute(parsed.arguments);
        } catch (const std::exception& e) {
            return CommandResult(false, "Command execution failed: " + std::string(e.what()));
        }
    }
    
    CommandResult parseCommandLine(const std::string& commandLine) {
        CommandResult result(true, "");
        
        // Simple command line parsing (in real implementation would be more robust)
        std::istringstream stream(commandLine);
        std::string token;
        std::vector<std::string> tokens;
        
        while (stream >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) {
            return CommandResult(false, "Empty command line");
        }
        
        ParsedCommand parsed;
        parsed.commandName = tokens[0];
        
        // Parse arguments
        for (size_t i = 1; i < tokens.size(); ++i) {
            std::string arg = tokens[i];
            
            // Check for flag arguments (start with --)
            if (arg.substr(0, 2) == "--") {
                parsed.arguments[arg] = true;  // Flag value
            }
            // Check for value arguments (contains =)
            else if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                parsed.arguments[key] = value;
            }
            else {
                // Positional argument
                parsed.arguments["__positional__"] = arg;
            }
        }
        
        return result;
    }
    
    std::vector<std::unique_ptr<Command>>::iterator findCommand(const std::string& name) {
        for (auto it = commands_.begin(); it != commands_.end(); ++it) {
            if ((*it)->getName() == name) {
                return it;
            }
        }
        return commands_.end();
    }
    
    CommandResult validateArguments(Command* command,
                                   const std::map<std::string, std::any>& arguments) {
        auto args = command->getArguments();
        
        for (const auto& arg : args) {
            auto it = arguments.find(arg.name);
            
            if (arg.required && it == arguments.end()) {
                return CommandResult(false, "Required argument missing: " + arg.name);
            }
            
            if (it != arguments.end()) {
                // Validate argument type
                if (!validateArgumentValue(arg, it->second)) {
                    return CommandResult(false, "Invalid argument value for: " + arg.name);
                }
            }
        }
        
        return CommandResult(true, "");
    }
    
    bool validateArgumentValue(const CommandArgument& arg, const std::any& value) const {
        if (!value.has_value()) {
            return !arg.required;
        }
        
        switch (arg.type) {
            case ArgumentType::FLAG:
                return std::any_cast<bool>(value);
            case ArgumentType::STRING:
                return std::any_cast<std::string>(value).size() > 0 || !arg.required;
            case ArgumentType::INTEGER:
                return true; // Type checking is implicit
            case ArgumentType::FLOAT:
                return true; // Type checking is implicit
            case ArgumentType::BOOLEAN:
                return true; // Type checking is implicit
            case ArgumentType::FILEPATH:
                return true; // Type checking is implicit
            case ArgumentType::DIRECTORY:
                return true; // Type checking is implicit
            case ArgumentType::ENUM:
                if (arg.enumValues.empty()) return true;
                return std::find(arg.enumValues.begin(), arg.enumValues.end(),
                               std::any_cast<std::string>(value)) != arg.enumValues.end();
            case ArgumentType::VECTOR:
                return true; // Type checking is implicit
            default:
                return true;
        }
    }
    
    std::string formatHelp() const {
        std::stringstream ss;
        ss << "=== Available Commands ===" << std::endl;
        
        std::map<CommandCategory, std::vector<Command*>> categorizedCommands;
        
        for (auto& command : commands_) {
            categorizedCommands[command->getCategory()].push_back(command.get());
        }
        
        for (const auto& pair : categorizedCommands) {
            ss << std::endl << "=== " << categoryToString(pair.first) << " ===" << std::endl;
            for (const auto* command : pair.second) {
                ss << command->getName() << ": " << command->getDescription() << std::endl;
                
                auto args = command->getArguments();
                for (const auto& arg : args) {
                    ss << "  --" << arg.name;
                    if (arg.type == ArgumentType::VALUE || arg.type == ArgumentType::STRING ||
                        arg.type == ArgumentType::INTEGER || arg.type == ArgumentType::FLOAT) {
                        ss << "=<value>";
                    }
                    ss << "\n    " << arg.description;
                    if (arg.required) {
                        ss << " (required)";
                    }
                    ss << std::endl;
                }
            }
        }
        
        return ss.str();
    }
    
private:
    std::string categoryToString(CommandCategory category) const {
        switch (category) {
            case CommandCategory::BASIC: return "Basic Commands";
            case CommandCategory::PLOTTING: return "Plotting Commands";
            case CommandCategory::ANALYSIS: return "Analysis Commands";
            case CommandCategory::PLANNING: return "Planning Commands";
            case CommandCategory::VISUALIZATION: return "Visualization Commands";
            case CommandCategory::DEBUG: return "Debug Commands";
            case CommandCategory::DEVELOPMENT: return "Development Commands";
            case CommandCategory::PLUGINS: return "Plugin Commands";
            default: return "Unknown";
        }
    }
    
    struct ParsedCommand {
        std::string commandName;
        std::map<std::string, std::any> arguments;
    };
    
    std::vector<std::unique_ptr<Command>> commands_;
};

// Advanced command system integration
class AdvancedCommandSystem {
public:
    AdvancedCommandSystem() = default;
    
    void registerCommand(std::unique_ptr<Command> command) {
        commands_.push_back(std::move(command));
        buildParser();
    }
    
    CommandResult executeCommandLine(const std::string& commandLine) {
        return parser_.parseAndExecute(commandLine);
    }
    
    std::string getHelp() const {
        return parser_.formatHelp();
    }
    
    void setPluginManager(PluginManager* manager) {
        pluginManager_ = manager;
        buildParser();
    }
    
    void integrateWithBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        buildParser();
    }
    
    // Specialized command builders for common NLM operations
    CommandBuilder createConnectivityCommand() {
        return CommandBuilder()
            .name("connectivity")
            .description("Test neural connectivity and propagation")
            .category(CommandCategory::BASIC)
            .requiresBrain(true)
            .addFlag("detailed", "Show detailed connectivity information")
            .addString("region", "Specific region to test", false)
            .addInteger("max_depth", "Maximum propagation depth", 10)
            .addFloat("threshold", "Connection strength threshold", 0.0f, 1.0f);
    }
    
    CommandBuilder createPlasticityCommand() {
        return CommandBuilder()
            .name("plasticity")
            .description("Apply plasticity rules and analyze changes")
            .category(CommandCategory::BASIC)
            .requiresBrain(true)
            .addFlag("stdp", "Apply STDP plasticity only")
            .addFlag("hebbian", "Apply Hebbian plasticity only")
            .addFloat("duration", "Plasticity duration in simulation steps", 100.0f)
            .addString("mode", "Plasticity mode (potentiate/depress)", false, "potentiate");
    }
    
    CommandBuilder createAnalysisCommand() {
        return CommandBuilder()
            .name("analysis")
            .description("Analyze brain state and statistics")
            .category(CommandCategory::ANALYSIS)
            .requiresBrain(true)
            .addFlag("stats", "Show comprehensive statistics")
            .addFlag("connectivity", "Analyze connectivity patterns")
            .addFlag("plasticity", "Analyze plasticity effects")
            .addString("output", "Output file for analysis results", false)
            .addEnum("format", "Output format", {"text", "csv", "json"}, "text");
    }
    
    CommandBuilder createVisualizationCommand() {
        return CommandBuilder()
            .name("visualize")
            .description("Create visualizations of brain state")
            .category(CommandCategory::VISUALIZATION)
            .requiresBrain(true)
            .addFlag("network", "Network visualization")
            .addFlag("activity", "Activity visualization")
            .addFlag("plasticity", "Plasticity changes visualization")
            .addString("output", "Output file for visualization", false)
            .addEnum("type", "Visualization type", {"graph", "heatmap", "3d"}, "graph");
    }
    
    CommandBuilder createDebugCommand() {
        return CommandBuilder()
            .name("debug")
            .description("Debug neural system and resolve issues")
            .category(CommandCategory::DEBUG)
            .requiresBrain(true)
            .addFlag("neurons", "Debug neuron states")
            .addFlag("synapses", "Debug synapse states")
            .addFlag("plasticity", "Debug plasticity rules")
            .addString("level", "Debug verbosity level", false, "info");
    }
    
    CommandBuilder createDevelopmentCommand() {
        return CommandBuilder()
            .name("develop")
            .description("Apply developmental changes to brain")
            .category(CommandCategory::DEVELOPMENT)
            .requiresBrain(true)
            .addFlag("full", "Apply full developmental sequence")
            .addEnum("stage", "Development stage to target", {"initial", "critical", "maturation", "adult"}, "initial")
            .addFloat("intensity", "Development intensity modifier", 1.0f);
    }
    
    CommandBuilder createSaveCommand() {
        return CommandBuilder()
            .name("save")
            .description("Save brain state to file")
            .category(CommandCategory::BASIC)
            .requiresBrain(true)
            .addString("filepath", "File path to save brain state", true)
            .addEnum("format", "Save format", {"binary", "json", "xml"}, "binary")
            .addFlag("compress", "Enable compression");
    }
    
    CommandBuilder createLoadCommand() {
        return CommandBuilder()
            .name("load")
            .description("Load brain state from file")
            .category(CommandCategory::BASIC)
            .requiresBrain(true)
            .addString("filepath", "File path to load brain state", true)
            .addFlag("validate", "Validate loaded state");
    }
    
    CommandBuilder createBenchmarkCommand() {
        return CommandBuilder()
            .name("benchmark")
            .description("Run performance benchmarks")
            .category(CommandCategory::ANALYSIS)
            .requiresBrain(true)
            .addEnum("mode", "Benchmark mode", {"quick", "comprehensive", "stress"}, "comprehensive")
            .addInteger("iterations", "Number of benchmark iterations", 10)
            .addString("output", "Output file for benchmark results", false);
    }
    
    CommandBuilder createExperimentCommand() {
        return CommandBuilder()
            .name("experiment")
            .description("Run scientific experiments")
            .category(CommandCategory::PLANNING)
            .requiresBrain(true)
            .addString("type", "Experiment type", true)
            .addString("parameters", "Experiment parameters (JSON)", false)
            .addInteger("duration", "Experiment duration in steps", 1000)
            .addFlag("record", "Record experimental data");
    }
    
    // Batch command execution
    CommandResult executeCommandBatch(const std::vector<std::string>& commandLines) {
        CommandResult overallResult(true, "");
        
        for (const auto& commandLine : commandLines) {
            auto result = executeCommandLine(commandLine);
            
            if (!result.success) {
                overallResult.success = false;
                overallResult.errors.push_back(result.message);
            }
            
            if (!result.warnings.empty()) {
                overallResult.warnings.insert(overallResult.warnings.end(),
                                             result.warnings.begin(), result.warnings.end());
            }
        }
        
        return overallResult;
    }
    
    // Command history and replay
    void addToHistory(const std::string& commandLine) {
        commandHistory_.push_back(commandLine);
        if (commandHistory_.size() > maxHistorySize_) {
            commandHistory_.erase(commandHistory_.begin());
        }
    }
    
    std::vector<std::string> getCommandHistory() const {
        return commandHistory_;
    }
    
    CommandResult replayCommand(const std::string& commandLine) {
        return executeCommandLine(commandLine);
    }
    
    void clearCommandHistory() {
        commandHistory_.clear();
    }
    
    // Command completion
    std::vector<std::string> completeCommand(const std::string& partialCommand) {
        std::vector<std::string> completions;
        
        for (const auto& command : commands_) {
            if (command->getName().find(partialCommand) == 0) {
                completions.push_back(command->getName());
            }
            
            // Also check command arguments
            for (const auto& arg : command->getArguments()) {
                if (arg.name.find(partialCommand) == 0) {
                    completions.push_back("--" + arg.name);
                }
            }
        }
        
        return completions;
    }
    
private:
    void buildParser() {
        parser_ = CommandParser(std::move(commands_));
        if (pluginManager_) {
            auto pluginCommands = pluginManager_->getAllCommands();
            parser_.commands_.insert(parser_.commands_.end(),
                                    std::make_move_iterator(pluginCommands.begin()),
                                    std::make_move_iterator(pluginCommands.end()));
        }
    }
    
    std::vector<std::unique_ptr<Command>> commands_;
    CommandParser parser_;
    PluginManager* pluginManager_ = nullptr;
    std::shared_ptr<Brain> brain_;
    
    const size_t maxHistorySize_ = 1000;
    std::vector<std::string> commandHistory_;
};

} // namespace nlm