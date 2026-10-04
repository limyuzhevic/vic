#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

namespace nlm {

class HelpCommand : public SimpleCLICommand {
public:
    HelpCommand() : SimpleCLICommand(
        "help",
        "Show help information",
        "nlm help [command] [--format json|text|html] [--examples]",
        "Show help information for NLM commands.\n\n"
        "Options:\n"
        "  --format json|text|html    Output format (default: text)\n"
        "  --examples                Show usage examples\n"
        "  --all                     Show help for all commands\n"
        "  --group <name>            Show help for a specific command group\n"
        "  --search <query>          Search for help topics\n\n"
        "Examples:\n"
        "  nlm help\n"
        "  nlm help run\n"
        "  nlm help --format json\n"
        "  nlm help --search configuration"
    ) {
        options = {
            {"format", "", "Output format", OptionType::Choice, false, "text", "json|text|html"},
            {"examples", "", "Show usage examples", OptionType::Flag},
            {"all", "", "Show help for all commands", OptionType::Flag},
            {"group", "", "Show help for a specific command group", OptionType::OptionalValue},
            {"search", "", "Search for help topics", OptionType::OptionalValue}
        };
        
        parameters = {
            {"command", "Command to get help for", false, "string", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get options
        std::string format = getOptionValue(args, "format");
        bool examples = hasOption(args, "examples");
        bool all = hasOption(args, "all");
        std::string group = getOptionValue(args, "group");
        std::string search = getOptionValue(args, "search");
        
        // Get command parameter
        std::string command;
        if (!args.empty() && args[0] != "--verbose" && args[0] != "--format" &&
            args[0] != "--examples" && args[0] != "--all" && args[0] != "--group" &&
            args[0] != "--search") {
            command = args[0];
        }
        
        // Determine help topic
        std::string helpTopic;
        if (!search.empty()) {
            helpTopic = "search:" + search;
        } else if (!group.empty()) {
            helpTopic = "group:" + group;
        } else if (!command.empty()) {
            helpTopic = command;
        } else if (all) {
            helpTopic = "all";
        } else {
            helpTopic = "general";
        }
        
        // Generate and display help
        std::string helpText = generateHelp(helpTopic, examples);
        
        if (format == "json") {
            displayJsonHelp(helpText);
        } else if (format == "html") {
            displayHtmlHelp(helpText);
        } else {
            std::cout << helpText;
        }
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        // Basic validation
        for (const auto& arg : args) {
            if (arg.find("--format") != std::string::npos && arg != "--format") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            } else if (arg.find("--group") != std::string::npos && arg != "--group") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            } else if (arg.find("--search") != std::string::npos && arg != "--search") {
                size_t pos = arg.find(" ");
                if (pos == std::string::npos) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    std::string generateHelp(const std::string& topic, bool showExamples) {
        std::stringstream helpStream;
        
        if (topic == "general") {
            helpStream << generateGeneralHelp(showExamples);
        } else if (topic == "all") {
            helpStream << generateAllCommandsHelp(showExamples);
        } else if (topic.find("search:") == 0) {
            std::string query = topic.substr(7);
            helpStream << generateSearchResultsHelp(query);
        } else if (topic.find("group:") == 0) {
            std::string groupName = topic.substr(6);
            helpStream << generateGroupHelp(groupName, showExamples);
        } else {
            // Specific command help
            helpStream << generateCommandHelp(topic, showExamples);
        }
        
        return helpStream.str();
    }
    
    std::string generateGeneralHelp(bool showExamples) {
        std::stringstream help;
        
        help << "NLM - Neural Learning Machine" << std::endl;
        help << "=========================" << std::endl << std::endl;
        
        help << "Usage: nlm <command> [options]" << std::endl << std::endl;
        
        help << "Description:" << std::endl;
        help << "  NLM (熙然) - Neural Learning Machine" << std::endl;
        help << "  An experimental artificial developmental brain." << std::endl;
        help << "  Phase 2: Real Neural Computation with:" << std::endl;
        help << "    - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
        help << "    - Event-driven spike propagation with delays" << std::endl;
        help << "    - STDP and Hebbian plasticity rules" << std::endl;
        help << "    - Structural plasticity (synaptogenesis/pruning)" << std::endl;
        help << std::endl;
        
        help << "Available Commands:" << std::endl;
        
        // Get available commands from command groups
        auto commandGroups = getAvailableCommandGroups();
        
        size_t maxCommandNameLength = 0;
        for (const auto& group : commandGroups) {
            for (const auto& command : group->getCommands()) {
                if (command->getName().length() > maxCommandNameLength) {
                    maxCommandNameLength = command->getName().length();
                }
            }
        }
        
        for (const auto& group : commandGroups) {
            help << std "\n" << group->getName() << " Commands:" << std::endl;
            for (const auto& command : group->getCommands()) {
                help << "  " << std::left << std::setw(25) << command->getName()
                     << "- " << command->getDescription() << std::endl;
            }
        }
        
        help << std::endl;
        help << "For more help on a specific command, use: nlm help <command>" << std::endl;
        help << "For help on a command group, use: nlm help --group <group>" << std::endl;
        
        if (showExamples) {
            help << std::endl << "\nExamples:" << std::endl;
            help << "  nlm help                   - Show general help" << std::endl;
            help << "  nlm help run              - Show help for run command" << std::endl;
            help << "  nlm help --format json    - Show help in JSON format" << std::endl;
            help << "  nlm help --search config  - Search for configuration help" << std::endl;
            help << "  nlm help --all            - Show help for all commands" << std::endl;
        }
        
        return help.str();
    }
    
    std::string generateAllCommandsHelp(bool showExamples) {
        std::stringstream help;
        
        help << "All NLM Commands" << std::endl;
        help << "===============" << std::endl << std::endl;
        
        auto commandGroups = getAvailableCommandGroups();
        
        for (const auto& group : commandGroups) {
            help << group->getName() << " Commands:" << std::endl;
            help << group->getDescription() << std::endl;
            help << std::endl;
            
            for (const auto& command : group->getCommands()) {
                help << "Command: " << command->getName() << std::endl;
                help << "Description: " << command->getDescription() << std::endl;
                help << "Usage: " << command->getUsage() << std::endl;
                
                if (showExamples) {
                    help << "Examples: nlm " << command->getUsage() << std::endl;
                }
                
                help << std::endl;
            }
        }
        
        return help.str();
    }
    
    std::string generateCommandHelp(const std::string& commandName, bool showExamples) {
        std::stringstream help;
        
        // Find command
        auto commandGroups = getAvailableCommandGroups();
        std::shared_ptr<CLICommand> command = nullptr;
        
        for (const auto& group : commandGroups) {
            if (group->hasCommand(commandName)) {
                command = group->getCommand(commandName);
                break;
            }
        }
        
        if (!command) {
            return "Error: Command not found: " + commandName + "\n";
        }
        
        help << "Command: " << command->getName() << std::endl;
        help << "Description: " << command->getDescription() << std::endl;
        help << "Usage: " << command->getUsage() << std::endl;
        help << std::endl;
        
        help << "Options:" << std::endl;
        auto options = command->getOptions();
        for (const auto& option : options) {
            std::string flag = "  --" + option.name;
            if (!option.shortName.empty()) {
                flag += " (-" + option.shortName + ")";
            }
            
            help << flag << std::endl;
            help << "    " << option.description << std::endl;
            if (!option.defaultValue.empty()) {
                help << "    Default: " << option.defaultValue << std::endl;
            }
            if (!option.valueDescription.empty()) {
                help << "    Type: " << option.valueDescription << std::endl;
            }
            help << std::endl;
        }
        
        auto parameters = command->getParameters();
        if (!parameters.empty()) {
            help << "Parameters:" << std::endl;
            for (const auto& parameter : parameters) {
                std::string flag = "  " + parameter.name;
                help << flag << std::endl;
                help << "    " << parameter.description << std::endl;
                if (!parameter.defaultValue.empty()) {
                    help << "    Default: " << parameter.defaultValue << std::endl;
                }
                if (!parameter.type.empty()) {
                    help << "    Type: " << parameter.type << std::endl;
                }
                help << std::endl;
            }
        }
        
        if (showExamples) {
            help << "Examples:" << std::endl;
            help << "  nlm " << command->getUsage() << std::endl;
            help << std::endl;
        }
        
        return help.str();
    }
    
    std::string generateGroupHelp(const std::string& groupName, bool showExamples) {
        std::stringstream help;
        
        auto commandGroups = getAvailableCommandGroups();
        std::shared_ptr<CLIGroup> group = nullptr;
        
        for (const auto& g : commandGroups) {
            if (g->getName() == groupName) {
                group = g;
                break;
            }
        }
        
        if (!group) {
            return "Error: Command group not found: " + groupName + "\n";
        }
        
        help << group->getName() << " Command Group" << std::endl;
        help << "============================" << std::endl << std::endl;
        
        help << group->getDescription() << std::endl;
        help << std::endl;
        
        help << "Commands in this group:" << std::endl;
        for (const auto& command : group->getCommands()) {
            help << "  " << command->getName() << " - " << command->getDescription() << std::endl;
        }
        
        if (showExamples) {
            help << std::endl << "Examples:" << std::endl;
            for (const auto& command : group->getCommands()) {
                help << "  nlm " << command->getUsage() << std::endl;
            }
        }
        
        return help.str();
    }
    
    std::string generateSearchResultsHelp(const std::string& query) {
        std::stringstream help;
        
        help << "Search Results for: " << query << std::endl;
        help << "=========================" << std::endl << std::endl;
        
        // Search for commands, options, and parameters containing the query
        auto commandGroups = getAvailableCommandGroups();
        
        bool found = false;
        std::string lowerQuery = toLower(query);
        
        for (const auto& group : commandGroups) {
            for (const auto& command : group->getCommands()) {
                if (contains(command->getName(), lowerQuery) ||
                    contains(command->getDescription(), lowerQuery) ||
                    contains(command->getUsage(), lowerQuery)) {
                    
                    if (!found) {
                        help << "Commands:" << std::endl;
                        found = true;
                    }
                    
                    help << "  " << command->getName() << " - " << command->getDescription() << std::endl;
                }
            }
        }
        
        if (!found) {
            help << "No matches found for: " << query << std::endl;
        }
        
        return help.str();
    }
    
    void displayJsonHelp(const std::string& helpText) {
        std::cout << "{\n";
        std::cout << "  \"help\": \"" << escapeJson(helpText) << "\"\n";
        std::cout << "}\n";
    }
    
    void displayHtmlHelp(const std::string& helpText) {
        std::cout << "<!DOCTYPE html>\n";
        std::cout << "<html>\n";
        std::cout << "<head><title>NLM Help</title></head>\n";
        std::cout << "<body>\n";
        std::cout << "<pre>" << helpText << "</pre>\n";
        std::cout << "</body>\n";
        std::cout << "</html>\n";
    }
    
    // Helper methods
    bool contains(const std::string& text, const std::string& query) {
        std::string lowerText = toLower(text);
        return lowerText.find(query) != std::string::npos;
    }
    
    std::string toLower(const std::string& text) {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string escapeJson(const std::string& text) {
        std::string result;
        for (char c : text) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    std::vector<std::shared_ptr<CLIGroup>> getAvailableCommandGroups() {
        // Get available command groups
        // This should be implemented based on how command groups are managed
        static std::vector<std::shared_ptr<CLIGroup>> commandGroups;
        return commandGroups;
    }
};

} // namespace nlm
