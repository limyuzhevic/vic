// ====================================================================
// COMMAND LINE PARSER IMPLEMENTATION
// ====================================================================

#include "advanced/CommandLineParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

void CommandLineParser::addCommand(const std::string& name, const std::string& description,
                                  const std::string& longName, const std::string& shortName,
                                  const std::string& argument, const std::string& defaultValue,
                                  bool required) {
    Command cmd;
    cmd.name = sanitizeCommandName(name);
    cmd.description = description;
    cmd.longName = longName.empty() ? "--" + cmd.name : longName;
    cmd.shortName = shortName.empty() ? "-" + (cmd.name.length() > 1 ? cmd.name.substr(0, 1) : cmd.name) : shortName;
    cmd.argument = argument;
    cmd.defaultValue = defaultValue;
    cmd.required = required;
    cmd.hasArgument = !argument.empty();
    
    commands_.push_back(cmd);
}

void CommandLineParser::addCommand(const Command& cmd) {
    Command sanitizedCmd = cmd;
    sanitizedCmd.name = sanitizeCommandName(cmd.name);
    commands_.push_back(sanitizedCmd);
}

std::shared_ptr<Config> CommandLineParser::parseToConfig(int argc, char** argv) const {
    auto config = std::make_shared<Config>();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            if (validateCommand(key, value)) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle --key value format
        else if (arg.find("--") == 0 && i + 1 < argc) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            
            if (validateCommand(key, value)) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -k value format
        else if (arg.find("-") == 0 && arg.length() > 1 && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (validateCommand(key, value)) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return config;
}

std::vector<CommandLineParser::ParsedCommand> CommandLineParser::parseCommands(int argc, char** argv) const {
    std::vector<ParsedCommand> parsedCommands;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        ParsedCommand parsed;
        
        if (arg.find("--") == 0) {
            if (arg.find("=") != std::string::npos) {
                size_t pos = arg.find("=");
                parsed.name = arg.substr(2, pos - 2);
                parsed.value = arg.substr(pos + 1);
            } else {
                parsed.name = arg.substr(2);
                if (i + 1 < argc) {
                    parsed.value = argv[++i];
                }
            }
            parsed.present = true;
        } else if (arg.find("-") == 0 && arg.length() > 1) {
            parsed.name = arg.substr(1);
            if (i + 1 < argc) {
                parsed.value = argv[++i];
            }
            parsed.present = true;
        }
        
        if (parsed.present) {
            parsedCommands.push_back(parsed);
        }
    }
    
    return parsedCommands;
}

std::vector<std::string> CommandLineParser::validateCommands(const std::vector<ParsedCommand>& commands) const {
    std::vector<std::string> errors;
    
    for (const auto& cmd : commands) {
        // Check if command exists
        bool commandExists = false;
        for (const auto& definedCmd : commands_) {
            if (definedCmd.name == cmd.name) {
                commandExists = true;
                
                // Validate command with its validator if it has one
                if (definedCmd.validator && !definedCmd.validator(cmd.value)) {
                    errors.push_back("Invalid value for command --" + cmd.name + ": '" + cmd.value + "'");
                }
                
                // Validate command choices
                if (!definedCmd.choices.empty()) {
                    bool validChoice = false;
                    for (const auto& choice : definedCmd.choices) {
                        if (choice == cmd.value) {
                            validChoice = true;
                            break;
                        }
                    }
                    if (!validChoice) {
                        errors.push_back("Invalid choice for command --" + cmd.name + ": '" + cmd.value + "'. Valid choices: " + 
                                        std::accumulate(definedCmd.choices.begin() + 1, definedCmd.choices.end(), 
                                                       definedCmd.choices[0], 
                                                       [](const std::string& a, const std::string& b) { return a + ", " + b; }));
                    }
                }
                
                break;
            }
        }
        
        if (!commandExists) {
            errors.push_back("Unknown command: --" + cmd.name);
        }
    }
    
    return errors;
}

std::string CommandLineParser::generateHelp() const {
    std::ostringstream oss;
    
    oss << "Available commands:" << std::endl;
    oss << std::endl;
    
    for (const auto& cmd : commands_) {
        oss << "  " << cmd.longName;
        if (!cmd.shortName.empty()) {
            oss << " (" << cmd.shortName << ")";
        }
        oss << std::endl;
        
        if (!cmd.description.empty()) {
            oss << "    " << cmd.description << std::endl;
        }
        
        if (cmd.hasArgument) {
            oss << "    " << cmd.argument << std::endl;
        }
        
        if (!cmd.defaultValue.empty()) {
            oss << "    Default: " << cmd.defaultValue << std::endl;
        }
        
        if (cmd.required) {
            oss << "    Required: yes" << std::endl;
        }
        
        if (!cmd.choices.empty()) {
            oss << "    Choices: ";
            for (size_t i = 0; i < cmd.choices.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << cmd.choices[i];
            }
            oss << std::endl;
        }
        
        oss << std::endl;
    }
    
    oss << "Usage: command [options]" << std::endl;
    oss << std::endl;
    oss << "Examples:" << std::endl;
    oss << "  --help                     Show this help message" << std::endl;
    oss << "  --config config.json       Load configuration from file" << std::endl;
    oss << "  --neuron_count 1000       Set number of neurons" << std::endl;
    oss << "  --learning_rate 0.01       Set learning rate" << std::endl;
    
    return oss.str();
}

std::vector<std::string> CommandLineParser::getCommandNames() const {
    std::vector<std::string> names;
    for (const auto& cmd : commands_) {
        names.push_back(cmd.name);
    }
    return names;
}

CommandLineParser::Command CommandLineParser::getCommand(const std::string& name) const {
    std::string sanitizedName = sanitizeCommandName(name);
    
    for (const auto& cmd : commands_) {
        if (cmd.name == sanitizedName) {
            return cmd;
        }
    }
    
    // Return empty command if not found
    return Command();
}

bool CommandLineParser::applyToConfig(std::shared_ptr<Config> config, const std::vector<ParsedCommand>& commands) const {
    bool success = true;
    
    for (const auto& cmd : commands) {
        // Find the command definition
        Command defCmd;
        bool found = false;
        for (const auto& definedCmd : commands_) {
            if (definedCmd.name == cmd.name) {
                defCmd = definedCmd;
                found = true;
                break;
            }
        }
        
        if (!found) {
            // Skip unknown commands
            continue;
        }
        
        // Apply to config
        if (defCmd.hasArgument) {
            if (!defCmd.validator || defCmd.validator(cmd.value)) {
                config->set(cmd.name, cmd.value, ConfigSource::CommandLine);
            } else {
                success = false;
            }
        } else {
            // Boolean flag
            config->set(cmd.name, true, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

std::string CommandLineParser::sanitizeCommandName(const std::string& name) const {
    std::string sanitized = name;
    
    // Remove leading -- or -
    if (sanitized.find("--") == 0) {
        sanitized = sanitized.substr(2);
    } else if (sanitized.find("-") == 0) {
        sanitized = sanitized.substr(1);
    }
    
    // Convert to lowercase
    for (char& c : sanitized) {
        c = std::tolower(c);
    }
    
    return sanitized;
}

bool CommandLineParser::isValidCommandName(const std::string& name) const {
    if (name.empty() || !std::isalpha(name[0])) {
        return false;
    }
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

std::string CommandLineParser::formatArgument(const std::string& arg) const {
    if (arg.empty()) {
        return "<value>";
    }
    
    return arg;
}

bool CommandLineParser::validateCommand(const Command& cmd, const std::string& value) const {
    if (cmd.validator) {
        return cmd.validator(value);
    }
    
    // Default validation: non-empty value if required
    if (cmd.required && value.empty()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> CommandLineParser::splitArguments(const std::string& argString) const {
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;
    char quoteChar = '\0';
    
    for (char c : argString) {
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else if (c == '"' || c == '\'') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = c;
            } else if (quoteChar == c) {
                inQuotes = false;
                quoteChar = '\0';
            }
            current += c;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        args.push_back(current);
    }
    
    return args;
}

bool CommandLineParser::matchesPattern(const std::string& str, const std::string& pattern) const {
    std::regex regex(pattern);
    return std::regex_match(str, regex);
}

} // namespace nlm
