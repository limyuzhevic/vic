// NLM Advanced CLI - Main CLI Interface Header
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

// Command structure for CLI parsing
struct CLICommand {
    std::string name;
    std::string description;
    std::vector<std::string> aliases;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> options;
    bool requiresBrain = false;
    bool isInteractive = false;
    
    CLICommand() = default;
    
    CLICommand(const std::string& name, const std::string& desc)
        : name(name), description(desc) {}
    
    std::string toString() const {
        std::string result = "nlm " + name;
        for (const auto& opt : options) {
            result += " --" + opt.first + "=" + opt.second;
        }
        return result;
    }
};

// CLI Result type
enum class CLIResult {
    Success,
    Error,
    Warning,
    Quit,
    HelpDisplayed
};

// Command callback function type
using CommandCallback = std::function<CLIResult(const std::map<std::string, std::string>& options)>;

// Base CLI interface
class ICLInterface {
public:
    virtual ~ICLInterface() = default;
    virtual CLIResult execute(const CLICommand& cmd) = 0;
    virtual void displayHelp() const = 0;
    virtual std::string getName() const = 0;
};

// Global command registry for CLI factory
class CommandRegistry {
private:
    std::map<std::string, std::pair<CommandCallback, std::string>> commands;
    
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(const std::string& name, CommandCallback callback, const std::string& description) {
        commands[name] = {callback, description};
    }
    
    bool hasCommand(const std::string& name) const {
        return commands.find(name) != commands.end();
    }
    
    CLIResult executeCommand(const std::string& name, const std::map<std::string, std::string>& options) {
        if (hasCommand(name)) {
            return commands[name].first(options);
        }
        return CLIResult::Error;
    }
    
    const std::string& getDescription(const std::string& name) const {
        static const std::string empty = "";
        auto it = commands.find(name);
        return it != commands.end() ? it->second.second : empty;
    }
    
    std::vector<std::string> getAllCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commands) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// Command decorator for help text generation
class CommandHelpBuilder {
public:
    static CLICommand build(const std::string& name, const std::string& description,
                           const std::vector<std::string>& args = {}) {
        CLICommand cmd(name, description);
        for (const auto& arg : args) {
            cmd.arguments.push_back(arg);
        }
        return cmd;
    }
};

} // namespace nlm

