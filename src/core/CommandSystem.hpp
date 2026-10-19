// Command system for NLM - enables easy extension and customization
// Provides a plugin-like architecture for adding new functionality

#ifndef NLM_COMMAND_SYSTEM_HPP
#define NLM_COMMAND_SYSTEM_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../core/Config/Config.hpp"

namespace nlm {

// Command execution result
enum class CommandResult {
    Success,
    Failure,
    InvalidArguments,
    NotImplemented,
    RuntimeError,
    PermissionDenied
};

// Command information
struct CommandInfo {
    std::string name;
    std::string description;
    std::string usage;
    std::string category;
    std::string author;
    std::string version;
    bool requiresBrain = false;
    bool isHidden = false;
    
    CommandInfo() = default;
    CommandInfo(const std::string& n, const std::string& desc, const std::string& usage = "",
                const std::string& cat = "General", const std::string& auth = "NLM",
                const std::string& ver = "1.0", bool reqBrain = false, bool hidden = false)
        : name(n), description(desc), usage(usage), category(cat), author(auth), version(ver),
          requiresBrain(reqBrain), isHidden(hidden) {}
};

// Command argument definition
struct CommandArgument {
    std::string name;
    std::string type; // "string", "int", "float", "bool", "double"
    std::string defaultValue;
    std::string description;
    bool required = false;
    std::vector<std::string> allowedValues;
    
    CommandArgument(const std::string& n, const std::string& t, const std::string& desc = "",
                   const std::string& defaultVal = "", bool req = false,
                   const std::vector<std::string>& allowed = {})
        : name(n), type(t), defaultValue(defaultVal), description(desc), required(req), allowedValues(allowed) {}
};

// Command execution context
struct CommandContext {
    std::shared_ptr<Config> config;
    std::vector<std::string> args;
    std::unordered_map<std::string, std::string> parsedArgs;
    CommandResult result = CommandResult::Success;
    std::string output;
    std::string errorMessage;
    
    CommandContext() = default;
    CommandContext(std::shared_ptr<Config> cfg) : config(cfg) {}
};

// Base command interface
class ICommand {
public:
    virtual ~ICommand() = default;
    
    virtual CommandInfo getInfo() const = 0;
    virtual CommandResult execute(CommandContext& context) = 0;
    virtual std::vector<CommandArgument> getArguments() const = 0;
    virtual bool validateArguments(const CommandContext& context) const = 0;
    
    virtual bool canExecute(const CommandContext& context) const {
        return !getInfo().requiresBrain || (context.config && context.config->has("neuron_count"));
    }
};

// Concrete command implementation template
class Command : public ICommand {
protected:
    CommandInfo info_;
    std::vector<CommandArgument> arguments_;
    std::function<CommandResult(CommandContext&)> executor_;
    std::function<bool(const CommandContext&)> validator_;
    
public:
    Command(const CommandInfo& info, const std::vector<CommandArgument>& args,
            const std::function<CommandResult(CommandContext&)>& executor,
            const std::function<bool(const CommandContext&)> validator = nullptr)
        : info_(info), arguments_(args), executor_(executor), validator_(validator) {}
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        if (!validateArguments(context)) {
            return CommandResult::InvalidArguments;
        }
        
        if (validator_ && !validator_(context)) {
            return CommandResult::Failure;
        }
        
        return executor_(context);
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        // Check required arguments
        for (const auto& arg : arguments_) {
            if (arg.required && context.parsedArgs.find(arg.name) == context.parsedArgs.end()) {
                return false;
            }
        }
        
        // Validate argument types
        for (const auto& pair : context.parsedArgs) {
            auto argIt = std::find_if(arguments_.begin(), arguments_.end(),
                                     [&pair](const CommandArgument& a) { return a.name == pair.first; });
            if (argIt != arguments_.end()) {
                if (!validateArgumentType(pair.second, argIt->type)) {
                    return false;
                }
                
                // Check allowed values
                if (!argIt->allowedValues.empty()) {
                    if (std::find(argIt->allowedValues.begin(), argIt->allowedValues.end(), pair.second) == argIt->allowedValues.end()) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
private:
    bool validateArgumentType(const std::string& value, const std::string& type) const {
        if (type == "string") return true;
        if (type == "int") {
            try { std::stoi(value); return true; } catch (...) { return false; }
        }
        if (type == "float") {
            try { std::stof(value); return true; } catch (...) { return false; }
        }
        if (type == "double") {
            try { std::stod(value); return true; } catch (...) { return false; }
        }
        if (type == "bool") {
            std::string lower = value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower == "true" || lower == "false" || lower == "1" || lower == "0" || lower == "yes" || lower == "no";
        }
        return false;
    }
};

// Command manager for registering and executing commands
class CommandManager {
public:
    CommandManager() = default;
    
    // Register a command
    void registerCommand(std::unique_ptr<ICommand> command) {
        if (command) {
            CommandInfo info = command->getInfo();
            if (!info.isHidden) {
                commands_[info.name] = std::move(command);
                
                // Register category
                if (!info.category.empty()) {
                    categories_.insert(info.category);
                }
            }
        }
    }
    
    // Get command by name
    ICommand* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // List all available commands
    std::vector<std::string> listCommands() const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    }
    
    // List commands by category
    std::vector<std::string> listCommands(const std::string& category) const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            if (pair.second->getInfo().category == category) {
                names.push_back(pair.first);
            }
        }
        std::sort(names.begin(), names.end());
        return names;
    }
    
    // Get all categories
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories(categories_.begin(), categories_.end());
        std::sort(categories.begin(), categories.end());
        return categories;
    }
    
    // Get command info
    CommandInfo getCommandInfo(const std::string& name) const {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second->getInfo();
        }
        return CommandInfo();
    }
    
    // Parse command line arguments
    bool parseCommandLine(const std::string& commandLine, CommandContext& context) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(commandLine);
        
        while (tokenStream >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) {
            return false;
        }
        
        context.args = tokens;
        
        // Parse arguments (--key=value or --key value)
        for (size_t i = 1; i < tokens.size(); ++i) {
            std::string arg = tokens[i];
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    // --key=value format
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    context.parsedArgs[key] = value;
                } else {
                    // --key value format (value is next token)
                    std::string key = arg.substr(2);
                    if (i + 1 < tokens.size() && tokens[i + 1][0] != '-') {
                        context.parsedArgs[key] = tokens[++i];
                    } else {
                        context.parsedArgs[key] = "true"; // boolean flag
                    }
                }
            } else {
                // Positional argument
                context.parsedArgs["arg" + std::to_string(i - 1)] = arg;
            }
        }
        
        return true;
    }
    
    // Execute command
    CommandResult executeCommand(const std::string& commandName, CommandContext& context) {
        ICommand* command = getCommand(commandName);
        if (!command) {
            context.errorMessage = "Unknown command: " + commandName;
            return CommandResult::NotImplemented;
        }
        
        if (!command->canExecute(context)) {
            context.errorMessage = "Command cannot be executed with current context";
            return CommandResult::Failure;
        }
        
        return command->execute(context);
    }
    
    // Get detailed help for a command
    std::string getCommandHelp(const std::string& commandName) {
        ICommand* command = getCommand(commandName);
        if (!command) {
            return "Command not found: " + commandName;
        }
        
        CommandInfo info = command->getInfo();
        std::ostringstream oss;
        
        oss << "Command: " << info.name << std::endl;
        oss << "Description: " << info.description << std::endl;
        if (!info.usage.empty()) {
            oss << "Usage: " << info.usage << std::endl;
        }
        oss << "Category: " << info.category << std::endl;
        oss << "Author: " << info.author << std::endl;
        oss << "Version: " << info.version << std::endl;
        
        std::vector<CommandArgument> args = command->getArguments();
        if (!args.empty()) {
            oss << "Arguments:" << std::endl;
            for (const auto& arg : args) {
                oss << "  --" << arg.name << " (<" << arg.type << ">)";
                if (!arg.description.empty()) {
                    oss << " - " << arg.description;
                }
                if (arg.required) {
                    oss << " [REQUIRED]";
                }
                if (!arg.defaultValue.empty()) {
                    oss << " [default: " << arg.defaultValue << "]";
                }
                oss << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Get help for all commands
    std::string getAllHelp() {
        std::ostringstream oss;
        oss << "=== NLM Command Help ===" << std::endl << std::endl;
        
        std::vector<std::string> categories = getCategories();
        for (const std::string& category : categories) {
            oss << "=== " << category << " ===" << std::endl;
            std::vector<std::string> commands = listCommands(category);
            for (const std::string& cmd : commands) {
                CommandInfo info = getCommandInfo(cmd);
                oss << "  " << cmd << " - " << info.description << std::endl;
            }
            oss << std::endl;
        }
        
        oss << "Use 'help <command>' for detailed help on a specific command." << std::endl;
        return oss.str();
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commands_;
    std::unordered_set<std::string> categories_;
};

// Command registration macro
#define REGISTER_COMMAND(manager, command) \
    do { \
        static bool registered = []() { \
            manager.registerCommand(std::make_unique<decltype(command)>(command)); \
            return true; \
        }(); \
    } while (false)

// Command helper functions
namespace CommandHelpers {
    // Convert string to various types
    template<typename T>
    T convertString(const std::string& value);
    
    // Format output with colors
    std::string formatOutput(const std::string& text, const std::string& color = "default");
    
    // Format error message
    std::string formatError(const std::string& message);
    
    // Validate file path
    bool validateFilePath(const std::string& path, bool mustExist = false);
    
    // Generate unique filename
    std::string generateUniqueFilename(const std::string& basePath, const std::string& prefix = "", const std::string& extension = "");
}

} // namespace nlm

#endif // NLM_COMMAND_SYSTEM_HPP
