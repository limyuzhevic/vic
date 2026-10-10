// NLM Advanced CLI - Command Line Parser
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

class CLICommand;
class ICLInterface;

class CLIParser {
public:
    CLIParser() = default;
    ~CLIParser() = default;
    
    // Parse command line arguments
    void parse(int argc, char** argv) {
        commands.clear();
        options.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle options (--option=value or --option value)
            if (arg.substr(0, 2) == "--") {
                size_t equalsPos = arg.find('=');
                if (equalsPos != std::string::npos) {
                    std::string key = arg.substr(2, equalsPos - 2);
                    std::string value = arg.substr(equalsPos + 1);
                    options[key] = value;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // Option with separate value
                    options[arg.substr(2)] = argv[++i];
                } else {
                    // Flag option
                    options[arg.substr(2)] = "true";
                }
            } else if (arg.substr(0, 1) == "-") {
                // Short option
                std::string key = arg.substr(1);
                options[key] = "true";
            } else {
                // Command
                std::string cmdName = arg;
                CLICommand cmd;
                cmd.name = cmdName;
                cmd.options = currentOptions;
                commands.push_back(cmd);
                currentOptions.clear();
            }
        }
        
        // Apply global options to all commands
        for (auto& cmd : commands) {
            cmd.options = options;
        }
    }
    
    // Check if batch mode is enabled
    bool isBatchMode() const {
        return options.find("batch") != options.end() && options.at("batch") == "true";
    }
    
    // Get all commands
    const std::vector<CLICommand>& getCommands() const { return commands; }
    
    // Get global options
    const std::map<std::string, std::string>& getOptions() const { return options; }
    
    // Get option value
    std::string getOption(const std::string& key, const std::string& defaultValue = "") const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
    
    // Check if option exists
    bool hasOption(const std::string& key) const {
        return options.find(key) != options.end();
    }
    
    // Display help for commands
    void displayHelp(const std::string& command = "") const {
        if (command.empty()) {
            std::cout << "\n=== NLM Advanced CLI Commands ===\n" << std::endl;
            for (const auto& cmd : commands) {
                std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            }
        } else {
            // Find and display help for specific command
            for (const auto& cmd : commands) {
                if (cmd.name == command) {
                    std::cout << "\n=== Help for command: " << cmd.name << " ===\n" << std::endl;
                    std::cout << cmd.description << std::endl;
                    if (!cmd.arguments.empty()) {
                        std::cout << "\nArguments:" << std::endl;
                        for (const auto& arg : cmd.arguments) {
                            std::cout << "  " << arg << std::endl;
                        }
                    }
                    break;
                }
            }
        }
    }
    
private:
    std::vector<CLICommand> commands;
    std::map<std::string, std::string> options;
    std::map<std::string, std::string> currentOptions;
};

} // namespace nlm

