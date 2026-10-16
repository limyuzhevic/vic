#include "CLIParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iterator>

namespace nlm {

struct CLIParser::Impl {
    std::vector<Command> commands;
    std::vector<Option> options;
    std::map<std::string, std::string> parsedOptions;
    std::string currentCommand;
    bool interactive = false;
    std::ofstream historyFile;
    std::vector<std::string> commandHistory;
    std::vector<std::string> tabCompletions;
    std::string historyFilepath;
    std::string outputFilepath;
    std::string error;
};

CLIParser::CLIParser() : pImpl(std::make_unique<Impl>()) {
}

CLIParser::~CLIParser() {
    if (pImpl->historyFile.is_open()) {
        pImpl->historyFile.close();
    }
}

void CLIParser::addOption(const std::string& name, char shortName, const std::string& description,
                         const std::string& defaultValue, const std::string& type,
                         bool required, const std::vector<std::string>& validValues) {
    Option option;
    option.name = name;
    option.shortName = shortName;
    option.description = description;
    option.defaultValue = defaultValue;
    option.type = type;
    option.required = required;
    option.validValues = validValues;
    
    pImpl->options.push_back(option);
}

void CLIParser::addOptionValidator(const std::string& name, std::function<bool(const std::string&)> validator) {
    for (auto& option : pImpl->options) {
        if (option.name == name) {
            option.validator = validator;
            break;
        }
    }
}

void CLIParser::addCommand(const std::string& name, const std::string& description,
                          const std::vector<Option>& commandOptions) {
    Command command;
    command.name = name;
    command.description = description;
    command.options = commandOptions;
    pImpl->commands.push_back(command);
}

bool CLIParser::parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

bool CLIParser::parse(const std::vector<std::string>& args) {
    pImpl->error.clear();
    pImpl->parsedOptions.clear();
    
    std::string currentCommand;
    
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            if (currentCommand.empty()) {
                printHelp();
                return false;
            }
            printHelp(currentCommand);
            return false;
        } 
        else if (arg == "--version") {
            printVersion();
            return false;
        }
        
        else if (arg == "--interactive") {
            setInteractive(true);
        }
        
        else if (arg == "--output" && i + 1 < args.size()) {
            setOutputFile(args[++i]);
        }
        
        else if (arg.find("--") == 0 && !arg.empty()) {
            std::string optionName = arg.substr(1);
            std::string value = "";
            
            size_t equalPos = optionName.find('=');
            if (equalPos != std::string::npos) {
                value = optionName.substr(equalPos + 1);
                optionName = optionName.substr(0, equalPos);
            } else if (i + 1 < args.size() && args[i + 1][0] != '-') {
                value = args[++i];
            }
            
            bool optionFound = false;
            for (const auto& option : pImpl->options) {
                if (option.name == optionName || (option.shortName && optionName.size() == 1 && optionName[0] == option.shortName)) {
                    optionFound = true;
                    
                    if (!value.empty()) {
                        if (option.validator && !option.validator(value)) {
                            pImpl->error = "Invalid value for option '--" + option.name + "': " + value;
                            return false;
                        }
                        
                        if (!option.validValues.empty()) {
                            if (std::find(option.validValues.begin(), option.validValues.end(), value) == option.validValues.end()) {
                                pImpl->error = "Invalid value for option '--" + option.name + "': " + value + ". Valid values: ";
                                for (const auto& validValue : option.validValues) {
                                    pImpl->error += validValue + " ";
                                }
                                return false;
                            }
                        }
                        
                        pImpl->parsedOptions[option.name] = value;
                    }
                    
                    break;
                }
            }
            
            if (!optionFound) {
                pImpl->error = "Unknown option: --" + optionName;
                return false;
            }
        }
        
        else if (arg.size() == 1 && arg[0] == '-') {
            for (size_t j = 1; j < arg.size(); ++j) {
                char shortOpt = arg[j];
                bool optionFound = false;
                
                for (auto& option : pImpl->options) {
                    if (option.shortName == shortOpt) {
                        optionFound = true;
                        
                        if (i + 1 < args.size() && args[i + 1][0] != '-') {
                            std::string value = args[++i];
                            if (option.validator && !option.validator(value)) {
                                pImpl->error = "Invalid value for option '-" + std::string(1, shortOpt) + "': " + value;
                                return false;
                            }
                            pImpl->parsedOptions[option.name] = value;
                        }
                        
                        break;
                    }
                }
                
                if (!optionFound) {
                    pImpl->error = "Unknown option: '-" + std::string(1, shortOpt) + "'";
                    return false;
                }
            }
        }
        
        else if (currentCommand.empty()) {
            currentCommand = arg;
        }
    }
    
    return true;
}

std::string CLIParser::getOptionValue(const std::string& name, const std::string& defaultValue) const {
    auto it = pImpl->parsedOptions.find(name);
    if (it != pImpl->parsedOptions.end()) {
        return it->second;
    }
    return defaultValue;
}

bool CLIParser::hasOption(const std::string& name) const {
    return pImpl->parsedOptions.find(name) != pImpl->parsedOptions.end();
}

bool CLIParser::executeCommand(const std::string& name) {
    for (const auto& command : pImpl->commands) {
        if (command.name == name) {
            if (command.action) {
                // Execute the command action
                return true;
            }
            break;
        }
    }
    return false;
}

void CLIParser::printHelp(const std::string& commandName) const {
    std::stringstream output;
    
    if (commandName.empty()) {
        output << "NLM - Neural Learning Machine CLI\n\n";
        output << "Usage: nlm [OPTIONS] [COMMAND] [ARGS...]\n\n";
        output << "Global Options:\n";
        
        for (const auto& option : pImpl->options) {
            std::string optStr;
            if (option.shortName) {
                optStr += "-" + std::string(1, option.shortName);
                if (!option.name.empty()) {
                    optStr += ", ";
                }
            }
            if (!option.name.empty()) {
                optStr += "--" + option.name;
            }
            
            output << "  " << std::left << std::setw(20) << optStr;
            output << " " << option.description;
            if (!option.defaultValue.empty()) {
                output << " (default: " << option.defaultValue << ")";
            }
            output << "\n";
        }
        
        output << "\nAvailable Commands:\n";
        for (const auto& command : pImpl->commands) {
            output << "  " << std::left << std::setw(20) << command.name << " " << command.description << "\n";
        }
        
        output << "\nExamples:\n";
        output << "  nlm run --config my_config.json\n";
        output << "  nlm interactive\n";
        output << "  nlm run --neurons 1000 --steps 5000\n";
        
    } else {
        for (const auto& command : pImpl->commands) {
            if (command.name == commandName) {
                output << "Command: " << command.name << "\n";
                output << "Description: " << command.description << "\n\n";
                output << "Options:\n";
                
                for (const auto& option : command.options) {
                    std::string optStr;
                    if (option.shortName) {
                        optStr += "-" + std::string(1, option.shortName);
                        if (!option.name.empty()) {
                            optStr += ", ";
                        }
                    }
                    if (!option.name.empty()) {
                        optStr += "--" + option.name;
                    }
                    
                    output << "  " << std::left << std::setw(20) << optStr;
                    output << " " << option.description;
                    if (!option.defaultValue.empty()) {
                        output << " (default: " << option.defaultValue << ")";
                    }
                    output << "\n";
                }
                break;
            }
        }
    }
    
    std::cout << output.str();
}

void CLIParser::printVersion() const {
    std::cout << "NLM Version 2.0.0 (Phase 2: Real Neural Computation)\n";
    std::cout << "Build: Oct 16, 2026\n";
    std::cout << "\nFeatures:\n";
    std::cout << "  - Real LIF neuron dynamics\n";
    std::cout << "  - Event-driven spike propagation with delays\n";
    std::cout << "  " STDP and Hebbian plasticity rules\n";
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)"};

void CLIParser::setInteractive(bool interactive) {
    pImpl->interactive = interactive;
}

bool CLIParser::isInteractive() const {
    return pImpl->interactive;
}

void CLIParser::setOutputFile(const std::string& filepath) {
    pImpl->outputFilepath = filepath;
    if (!filepath.empty()) {
        pImpl->historyFile.open(filepath, std::ios::app);
    }
}

void CLIParser::enableCommandHistory(const std::string& historyFile) {
    if (!historyFile.empty()) {
        pImpl->historyFilepath = historyFile;
    }
    pImpl->historyFile.open(historyFile, std::ios::app);
}

void CLIParser::enableTabCompletion(const std::vector<std::string>& completions) {
    pImpl->tabCompletions = completions;
}

std::vector<std::string> CLIParser::getCommandHistory() const {
    return pImpl->commandHistory;
}

void CLIParser::clearCommandHistory() {
    pImpl->commandHistory.clear();
}

std::string CLIParser::getError() const {
    return pImpl->error;
}

bool CLIParser::saveHistory() const {
    if (pImpl->historyFilepath.empty()) return false;
    
    std::ofstream file(pImpl->historyFilepath, std::ios::trunc);
    if (!file.is_open()) return false;
    
    for (const auto& entry : pImpl->commandHistory) {
        file << entry << "\n";
    }
    
    file.close();
    return true;
}

bool CLIParser::loadHistory() {
    if (pImpl->historyFilepath.empty()) return false;
    
    std::ifstream file(pImpl->historyFilepath);
    if (!file.is_open()) return false;
    
    pImpl->commandHistory.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            pImpl->commandHistory.push_back(line);
        }
    }
    
    file.close();
    return true;
}

std::vector<std::string> CLIParser::getTabCompletions(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    for (const auto& command : pImpl->commands) {
        if (command.name.find(partialCommand) == 0) {
            completions.push_back(command.name);
        }
    }
    
    for (const auto& option : pImpl->options) {
        if (option.name.find(partialCommand) == 0) {
            completions.push_back("--" + option.name);
            if (option.shortName) {
                completions.push_back(std::string("-") + option.shortName);
            }
        }
    }
    
    return completions;
}

} // namespace nlm