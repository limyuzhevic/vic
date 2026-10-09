// Command parsing and validation for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <iomanip>
#include <sstream>
#include <regex>
#include <unordered_map>

namespace nlm {

class CommandParser {
public:
    struct ParsedCommand {
        std::string commandName;
        std::map<std::string, CommandArgument> arguments;
        std::vector<std::string> flags;
    };
    
    // Parse command line string into command structure
    static ParsedCommand parse(const std::string& commandLine) {
        ParsedCommand result;
        
        // Simple tokenization (could be enhanced)
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;
        
        for (char c : commandLine) {
            if (c == ' ' && !inQuotes) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (c == '"') {
                inQuotes = !inQuotes;
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) {
            return result;
        }
        
        result.commandName = tokens[0];
        
        // Parse arguments (simple key=value or --flag format)
        for (size_t i = 1; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];
            
            if (token.substr(0, 2) == "--") {
                // Flag (--verbose, --json)
                std::string flag = token.substr(2);
                result.flags.push_back(flag);
                result.arguments[flag] = true;
            } else if (token.find('=') != std::string::npos) {
                // Key=value argument
                size_t pos = token.find('=');
                std::string key = token.substr(0, pos);
                std::string value = token.substr(pos + 1);
                
                // Try to determine value type
                try {
                    // Try integer
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        result.arguments[key] = intVal;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try double
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        result.arguments[key] = doubleVal;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try boolean
                    if (value == "true" || value == "false") {
                        bool boolVal = (value == "true");
                        result.arguments[key] = boolVal;
                        continue;
                    }
                } catch (...) {}
                
                // Default to string
                result.arguments[key] = value;
            }
        }
        
        return result;
    }
};

class CommandValidator {
public:
    // Validate command arguments against command schema
    static bool validate(const std::shared_ptr<Command>& command,
                         const std::map<std::string, CommandArgument>& arguments) {
        if (!command) {
            return false;
        }
        
        // Get expected arguments from command
        auto expectedArgs = command->getArguments();
        
        // Check required arguments (none for now, could be extended)
        
        // Validate provided arguments against expected types
        for (const auto& arg : arguments) {
            const std::string& key = arg.first;
            const CommandArgument& value = arg.second;
            
            // Check if argument is expected
            bool isExpected = std::find(expectedArgs.begin(), expectedArgs.end(), key) != expectedArgs.end();
            
            if (!isExpected) {
                // Could be a flag or optional argument
                // For now, we'll accept any argument
            }
        }
        
        // Run command-specific validation
        if (command->validateArguments(arguments)) {
            return true;
        }
        
        return false;
    }
};

class CommandHistory {
private:
    struct HistoryEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string commandLine;
        CommandResult result;
        bool success;
    };
    
    std::vector<HistoryEntry> history_;
    size_t maxHistorySize_ = 1000;
    
public:
    // Add command to history
    void add(const std::string& commandLine, const CommandResult& result, bool success) {
        HistoryEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.commandLine = commandLine;
        entry.result = result;
        entry.success = success;
        
        history_.push_back(entry);
        
        if (history_.size() > maxHistorySize_) {
            history_.erase(history_.begin());
        }
    }
    
    // Get recent history
    std::vector<HistoryEntry> getRecent(size_t count = 10) const {
        if (count > history_.size()) {
            count = history_.size();
        }
        
        std::vector<HistoryEntry> result;
        result.reserve(count);
        
        auto start = history_.end() - count;
        for (auto it = start; it != history_.end(); ++it) {
            result.push_back(*it);
        }
        
        return result;
    }
    
    // Search history
    std::vector<HistoryEntry> search(const std::string& pattern) const {
        std::vector<HistoryEntry> result;
        
        std::regex regex(pattern, std::regex::icase);
        
        for (const auto& entry : history_) {
            if (std::regex_search(entry.commandLine, regex)) {
                result.push_back(entry);
            }
        }
        
        return result;
    }
    
    // Clear history
    void clear() {
        history_.clear();
    }
    
    // Get history size
    size_t size() const {
        return history_.size();
    }
};

class CommandCompleter {
public:
    // Generate command suggestions based on prefix
    static std::vector<std::string> suggest(const std::string& prefix, 
                                           const std::vector<std::shared_ptr<Command>>& availableCommands) {
        std::vector<std::string> suggestions;
        
        for (const auto& command : availableCommands) {
            const std::string& name = command->getName();
            
            if (name.find(prefix) == 0) {
                suggestions.push_back(name);
            }
        }
        
        return suggestions;
    }
    
    // Generate argument suggestions
    static std::vector<std::string> suggestArguments(const std::string& commandName,
                                                    const std::string& prefix,
                                                    const std::vector<std::shared_ptr<Command>>& availableCommands) {
        std::vector<std::string> suggestions;
        
        for (const auto& command : availableCommands) {
            if (command->getName() == commandName) {
                auto args = command->getArguments();
                for (const auto& arg : args) {
                    if (arg.find(prefix) == 0) {
                        suggestions.push_back(arg + "=<value>");
                    }
                }
                break;
            }
        }
        
        return suggestions;
    }
};

} // namespace nlm
