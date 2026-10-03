// ====================================================================
// COMMAND LINE PARSER MODULE
// ====================================================================

#ifndef SRC_ADVANCED_COMMANDLINEPARSER_H
#define SRC_ADVANCED_COMMANDLINEPARSER_H

#include "core/Config/Config.hpp"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <regex>
#include <set>

namespace nlm {

class CommandLineParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string longName;
        std::string shortName;
        std::string argument;
        std::string defaultValue;
        bool hasArgument;
        bool required;
        std::function<bool(const std::string&)> validator;
        std::vector<std::string> choices;
        
        Command() : hasArgument(false), required(false) {}
    };
    
    struct ParsedCommand {
        std::string name;
        std::string value;
        bool present;
    };
    
    CommandLineParser() = default;
    
    // Command definition
    void addCommand(const std::string& name, const std::string& description,
                   const std::string& longName = "", const std::string& shortName = "",
                   const std::string& argument = "", const std::string& defaultValue = "",
                   bool required = false);
    
    void addCommand(const Command& cmd);
    
    // Command with validation
    template<typename Validator>
    void addCommand(const std::string& name, const std::string& description,
                   Validator validator, const std::string& longName = "", 
                   const std::string& shortName = "", const std::string& argument = "",
                   const std::string& defaultValue = "", bool required = false);
    
    // Command with choices
    void addCommandWithChoices(const std::string& name, const std::string& description,
                              const std::vector<std::string>& choices,
                              const std::string& longName = "", const std::string& shortName = "",
                              const std::string& argument = "", const std::string& defaultValue = "",
                              bool required = false);
    
    // Configuration generation
    std::shared_ptr<Config> parseToConfig(int argc, char** argv) const;
    
    // Command parsing
    std::vector<ParsedCommand> parseCommands(int argc, char** argv) const;
    
    // Validation
    std::vector<std::string> validateCommands(const std::vector<ParsedCommand>& commands) const;
    
    // Help generation
    std::string generateHelp() const;
    
    // Command listing
    std::vector<std::string> getCommandNames() const;
    
    Command getCommand(const std::string& name) const;
    
    // Configuration application
    bool applyToConfig(std::shared_ptr<Config> config, const std::vector<ParsedCommand>& commands) const;
    
    // Template implementations
    template<typename Validator>
    void addCommand(const std::string& name, const std::string& description,
                   Validator validator, const std::string& longName,
                   const std::string& shortName, const std::string& argument,
                   const std::string& defaultValue, bool required) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.longName = longName;
        cmd.shortName = shortName;
        cmd.argument = argument;
        cmd.defaultValue = defaultValue;
        cmd.required = required;
        cmd.hasArgument = !argument.empty();
        cmd.validator = validator;
        commands_.push_back(cmd);
    }
    
private:
    std::vector<Command> commands_;
    
    std::string sanitizeCommandName(const std::string& name) const;
    bool isValidCommandName(const std::string& name) const;
    std::string formatArgument(const std::string& arg) const;
    
    bool validateCommand(const Command& cmd, const std::string& value) const;
    
    std::vector<std::string> splitArguments(const std::string& argString) const;
    bool matchesPattern(const std::string& str, const std::string& pattern) const;
};

} // namespace nlm

#endif // SRC_ADVANCED_COMMANDLINEPARSER_H
