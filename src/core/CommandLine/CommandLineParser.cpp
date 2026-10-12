#include "CommandLine/CommandLineParser.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm {

struct CommandLineParser::Impl {
    std::string programName;
    std::string description;
    std::vector<Option> options;
    std::map<std::string, std::string> parsedStringValues;
    std::map<std::string, int> parsedIntValues;
    std::map<std::string, float> parsedFloatValues;
    std::map<std::string, bool> parsedBoolValues;
    std::vector<std::string> positionalArgs;
    std::unordered_set<std::string> encounteredOptions;
    bool helpRequested;
    bool versionRequested;
    bool parsingError;
    std::string errorMessage;
    
    Impl(const std::string& progName, const std::string& desc)
        : programName(progName), description(desc), helpRequested(false),
          versionRequested(false), parsingError(false) {}
    
    ~Impl() = default;
};

CommandLineParser::CommandLineParser(const std::string& programName, const std::string& description)
    : pImpl(std::make_unique<Impl>(programName, description)) {
    setupDefaultOptions();
}

CommandLineParser::~CommandLineParser() = default;

CommandLineParser::CommandLineParser(CommandLineParser&&) noexcept = default;
CommandLineParser& CommandLineParser::operator=(CommandLineParser&&) noexcept = default;

void CommandLineParser::setupDefaultOptions() {
    // Add version and help flags
    addFlag("help", 'h', "Show this help message and exit");
    addFlag("version", 'v', "Show version information and exit");
}

void CommandLineParser::addOption(const std::string& longName, char shortName,
                                const std::string& description, const std::string& defaultValue,
                                const std::string& type, bool required,
                                const std::function<void(const std::string&)>& callback) {
    Option option(longName, shortName, description, defaultValue, type, required, callback);
    pImpl->options.push_back(option);
}

void CommandLineParser::addFlag(const std::string& longName, char shortName,
                               const std::string& description, bool defaultValue) {
    Option option(longName, shortName, description, defaultValue ? "true" : "false",
                  "bool", false, nullptr);
    pImpl->options.push_back(option);
    pImpl->parsedBoolValues[longName] = defaultValue;
}

bool CommandLineParser::parse(int argc, char** argv) {
    pImpl->parsedStringValues.clear();
    pImpl->parsedIntValues.clear();
    pImpl->parsedFloatValues.clear();
    pImpl->parsedBoolValues.clear();
    pImpl->positionalArgs.clear();
    pImpl->encounteredOptions.clear();
    pImpl->helpRequested = false;
    pImpl->versionRequested = false;
    pImpl->parsingError = false;
    pImpl->errorMessage.clear();
    
    // Initialize all options with their default values
    for (const auto& option : pImpl->options) {
        if (option.type == "string") {
            pImpl->parsedStringValues[option.longName] = option.defaultValue;
        } else if (option.type == "int") {
            try {
                pImpl->parsedIntValues[option.longName] = std::stoi(option.defaultValue);
            } catch (...) {
                pImpl->parsedIntValues[option.longName] = 0;
            }
        } else if (option.type == "float") {
            try {
                pImpl->parsedFloatValues[option.longName] = std::stof(option.defaultValue);
            } catch (...) {
                pImpl->parsedFloatValues[option.longName] = 0.0f;
            }
        } else if (option.type == "bool") {
            pImpl->parsedBoolValues[option.longName] = (option.defaultValue == "true" || 
                                                        option.defaultValue == "1" ||
                                                        option.defaultValue == "yes");
        }
    }
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        processArgument(arg, i, argc, argv);
    }
    
    // Check for required options
    if (!validateOptions()) {
        return false;
    }
    
    return !pImpl->parsingError;
}

void CommandLineParser::processArgument(const std::string& arg, size_t index, int argc, char** argv) {
    if (arg == "--help" || arg == "-h") {
        pImpl->helpRequested = true;
        return;
    }
    
    if (arg == "--version" || arg == "-v") {
        pImpl->versionRequested = true;
        return;
    }
    
    // Check for long option (--option=value or --option value)
    if (arg.find("--") == 0) {
        std::string optName = arg.substr(2);
        std::string optValue;
        
        size_t eqPos = optName.find('=');
        if (eqPos != std::string::npos) {
            optName = optName.substr(0, eqPos);
            optValue = optName.substr(eqPos + 1);
            optValue = optValue.substr(eqPos + 1); // Remove the = part
        } else if (index + 1 < (size_t)argc) {
            // Next argument might be the value
            std::string nextArg = argv[index + 1];
            if (nextArg.find("--") != 0 && nextArg.find("-") != 0) {
                optValue = nextArg;
                index++; // Skip the value argument
            }
        }
        
        // Find the option
        for (const auto& option : pImpl->options) {
            if (option.longName == optName) {
                if (option.callback) {
                    option.callback(optValue.empty() ? "" : optValue);
                }
                
                if (!optValue.empty()) {
                    // Set the value
                    if (option.type == "string") {
                        pImpl->parsedStringValues[optName] = optValue;
                    } else if (option.type == "int") {
                        try {
                            pImpl->parsedIntValues[optName] = std::stoi(optValue);
                        } catch (...) {
                            printError("Invalid integer value for option --" + optName + ": " + optValue);
                            pImpl->parsingError = true;
                        }
                    } else if (option.type == "float") {
                        try {
                            pImpl->parsedFloatValues[optName] = std::stof(optValue);
                        } catch (...) {
                            printError("Invalid float value for option --" + optName + ": " + optValue);
                            pImpl->parsingError = true;
                        }
                    } else if (option.type == "bool") {
                        pImpl->parsedBoolValues[optName] = (optValue == "true" || optValue == "1" || 
                                                            optValue == "yes");
                    }
                }
                
                pImpl->encounteredOptions.insert(optName);
                return;
            }
        }
        
        // Option not found
        printError("Unknown option --" + optName);
        pImpl->parsingError = true;
        
    } else if (arg.find("-") == 0 && arg.length() > 1 && !std::isalpha(arg[1])) {
        // Short option (-v, -f value)
        std::string shortOpt = arg.substr(1);
        if (shortOpt.length() == 1) {
            char optChar = shortOpt[0];
            for (const auto& option : pImpl->options) {
                if (option.shortName == optChar) {
                    if (option.callback) {
                        option.callback("");
                    }
                    
                    pImpl->encounteredOptions.insert(option.longName);
                    if (option.type == "bool") {
                        pImpl->parsedBoolValues[option.longName] = !pImpl->parsedBoolValues[option.longName];
                    }
                    return;
                }
            }
        } else {
            // Multiple short options (-abc)
            for (char c : shortOpt) {
                for (const auto& option : pImpl->options) {
                    if (option.shortName == c) {
                        if (option.callback) {
                            option.callback("");
                        }
                        
                        pImpl->encounteredOptions.insert(option.longName);
                        if (option.type == "bool") {
                            pImpl->parsedBoolValues[option.longName] = !pImpl->parsedBoolValues[option.longName];
                        }
                    }
                }
            }
        }
        
    } else {
        // Positional argument
        pImpl->positionalArgs.push_back(arg);
    }
}

bool CommandLineParser::validateOptions() const {
    for (const auto& option : pImpl->options) {
        if (option.required && pImpl->encounteredOptions.find(option.longName) == pImpl->encounteredOptions.end()) {
            printError("Required option --" + option.longName + " is missing");
            return false;
        }
    }
    
    return !pImpl->parsingError;
}

void CommandLineParser::printError(const std::string& message) const {
    pImpl->parsingError = true;
    pImpl->errorMessage = message;
    NLM_LOG_ERROR("Error: " + message);
}

std::string CommandLineParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CommandLineParser::getString(const std::string& option) const {
    auto it = pImpl->parsedStringValues.find(option);
    if (it != pImpl->parsedStringValues.end()) {
        return it->second;
    }
    return "";
}

int CommandLineParser::getInt(const std::string& option) const {
    auto it = pImpl->parsedIntValues.find(option);
    if (it != pImpl->parsedIntValues.end()) {
        return it->second;
    }
    return 0;
}

float CommandLineParser::getFloat(const std::string& option) const {
    auto it = pImpl->parsedFloatValues.find(option);
    if (it != pImpl->parsedFloatValues.end()) {
        return it->second;
    }
    return 0.0f;
}

bool CommandLineParser::getBool(const std::string& option) const {
    auto it = pImpl->parsedBoolValues.find(option);
    if (it != pImpl->parsedBoolValues.end()) {
        return it->second;
    }
    return false;
}

bool CommandLineParser::hasOption(const std::string& option) const {
    return pImpl->encounteredOptions.find(option) != pImpl->encounteredOptions.end();
}

const std::vector<std::string>& CommandLineParser::getPositionalArgs() const {
    return pImpl->positionalArgs;
}

void CommandLineParser::printHelp() const {
    std::cout << "Usage: " << pImpl->programName << " [options] [arguments]" << std::endl;
    std::cout << std::endl;
    
    if (!pImpl->description.empty()) {
        std::cout << "Description:" << std::endl;
        std::cout << "  " << pImpl->description << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Options:" << std::endl;
    
    // Group options by type
    std::vector<std::pair<std::string, std::vector<Option>>> optionGroups;
    
    for (const auto& option : pImpl->options) {
        std::string group = "General";
        if (option.longName == "help" || option.longName == "version") {
            group = "Help";
        } else if (option.type == "bool") {
            group = "Flags";
        } else if (option.type == "string") {
            group = "String Options";
        } else if (option.type == "int") {
            group = "Integer Options";
        } else if (option.type == "float") {
            group = "Float Options";
        }
        
        bool found = false;
        for (auto& groupPair : optionGroups) {
            if (groupPair.first == group) {
                groupPair.second.push_back(option);
                found = true;
                break;
            }
        }
        
        if (!found) {
            optionGroups.push_back({group, std::vector<Option>{option}});
        }
    }
    
    for (const auto& groupPair : optionGroups) {
        std::cout << "  " << groupPair.first << ":" << std::endl;
        for (const auto& option : groupPair.second) {
            std::string shortOpt = (option.shortName != 0) ? "-" + std::string(1, option.shortName) : "";
            std::string longOpt = "--" + option.longName;
            std::string optStr = "    ";
            if (!shortOpt.empty() && !longOpt.empty()) {
                optStr = "    " + shortOpt + ", " + longOpt;
            } else if (!shortOpt.empty()) {
                optStr = "    " + shortOpt;
            } else {
                optStr = "    " + longOpt;
            }
            
            std::cout << optStr << std::setw(30) << std::left << "  " + option.description;
            
            if (!option.defaultValue.empty()) {
                std::cout << " (default: " << option.defaultValue << ")";
            }
            
            if (option.required) {
                std::cout << " (required)";
            }
            
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << pImpl->programName << " --config=myconfig.cfg" << std::endl;
    std::cout << "  " << pImpl->programName << " --neuron-count 1000 --simulation-steps 5000" << std::endl;
    std::cout << "  " << pImpl->programName << " --enable-plasticity --debug" << std::endl;
}

bool CommandLineParser::wasHelpRequested() const {
    return pImpl->helpRequested;
}

bool CommandLineParser::wasVersionRequested() const {
    return pImpl->versionRequested;
}

const std::string& CommandLineParser::getProgramName() const {
    return pImpl->programName;
}

const std::string& CommandLineParser::getDescription() const {
    return pImpl->description;
}

const std::string& CommandLineParser::getVersion() {
    static const std::string version = "1.0.0";
    return version;
}

const std::string& CommandLineParser::getBuildInfo() {
    static const std::string buildInfo = "Built with C++20, CMake 3.16+";
    return buildInfo;
}

} // namespace nlm
