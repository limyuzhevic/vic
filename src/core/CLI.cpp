#include "CLI.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ctime>

namespace nlm {

struct CLIParser::Impl {
    struct Option {
        CLIConfig config;
        std::any value;
        bool present = false;
        bool processed = false;
    };
    
    struct Argument {
        std::string name;
        std::string description;
        bool required = false;
        std::any value;
        bool processed = false;
    };
    
    std::vector<Option> options;
    std::vector<Argument> arguments;
    
    // Help and version
    bool helpRequested = false;
    bool versionRequested = false;
    std::string versionInfo;
    
    // Error handling
    bool parseError = false;
    std::string errorMessage;
    
    // Parsing state
    std::string currentArg;
    size_t currentArgIndex = 0;
    
    // Output
    bool helpPrinted = false;
};

CLIParser::CLIParser() : pImpl(std::make_unique<Impl>()) {}

void CLIParser::addOption(const CLIConfig& config) {
    Impl::Option option;
    option.config = config;
    options.push_back(option);
}

void CLIParser::addArgument(const std::string& name, const std::string& description,
                           bool required) {
    Impl::Argument arg;
    arg.name = name;
    arg.description = description;
    arg.required = required;
    arguments.push_back(arg);
}

void CLIParser::addHelpOption(bool& helpEnabled) {
    pImpl->helpRequested = true;
    helpEnabled = false; // Will be set to true if --help is found
}

void CLIParser::addVersionOption(const std::string& versionInfo) {
    pImpl->versionRequested = true;
    pImpl->versionInfo = versionInfo;
}

bool CLIParser::parse(int argc, char** argv) {
    // Reset parsing state
    pImpl->parseError = false;
    pImpl->errorMessage.clear();
    pImpl->helpPrinted = false;
    
    // Initialize option values
    for (auto& option : pImpl->options) {
        option.present = false;
        option.processed = false;
        
        // Set default value if option has one
        if (!option.config.defaultValue.empty()) {
            option.value = option.config.defaultValue;
        }
    }
    
    // Initialize argument values
    for (auto& arg : pImpl->arguments) {
        arg.processed = false;
        arg.value = "";
    }
    
    // Parse arguments
    pImpl->currentArgIndex = 1; // Skip program name
    while (pImpl->currentArgIndex < argc) {
        pImpl->currentArg = argv[pImpl->currentArgIndex];
        
        // Check for help/version flags
        if (pImpl->currentArg == "--help" || pImpl->currentArg == "-h") {
            pImpl->helpRequested = true;
            pImpl->currentArgIndex++;
            continue;
        }
        
        if (pImpl->currentArg == "--version" || pImpl->currentArg == "-v") {
            pImpl->versionRequested = true;
            pImpl->currentArgIndex++;
            continue;
        }
        
        // Check for long option (--name=value or --name value)
        if (pImpl->currentArg.substr(0, 2) == "--") {
            std::string optionName = pImpl->currentArg.substr(2);
            std::string optionValue;
            
            size_t equalsPos = optionName.find('=');
            if (equalsPos != std::string::npos) {
                optionValue = optionName.substr(equalsPos + 1);
                optionName = optionName.substr(0, equalsPos);
            }
            
            // Find and process option
            bool found = false;
            for (auto& option : pImpl->options) {
                if (option.config.name == optionName) {
                    option.present = true;
                    option.processed = true;
                    
                    if (optionValue.empty() && pImpl->currentArgIndex + 1 < argc) {
                        // Try next argument as value
                        std::string nextArg = argv[pImpl->currentArgIndex + 1];
                        if (nextArg.substr(0, 2) != "--" && nextArg.substr(0, 1) != "-") {
                            optionValue = nextArg;
                            pImpl->currentArgIndex++;
                        }
                    }
                    
                    if (!optionValue.empty()) {
                        option.value = optionValue;
                    } else if (option.config.type == CLIType::Flag) {
                        option.value = "true";
                    } else {
                        option.value = option.config.defaultValue;
                    }
                    
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                pImpl->parseError = true;
                pImpl->errorMessage = "Unknown option: " + pImpl->currentArg;
                return false;
            }
            
            pImpl->currentArgIndex++;
            continue;
        }
        
        // Check for short option (-x or -xyz)
        if (pImpl->currentArg.substr(0, 1) == "-") {
            std::string optionName = pImpl->currentArg.substr(1);
            
            // Find and process option
            bool found = false;
            for (auto& option : pImpl->options) {
                if (option.config.shortName == optionName) {
                    option.present = true;
                    option.processed = true;
                    option.value = "true";
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                pImpl->parseError = true;
                pImpl->errorMessage = "Unknown option: " + pImpl->currentArg;
                return false;
            }
            
            pImpl->currentArgIndex++;
            continue;
        }
        
        // Positional argument
        if (pImpl->arguments.empty() || 
            pImpl->currentArgIndex - 1 < pImpl->arguments.size()) {
            
            if (pImpl->arguments.empty()) {
                pImpl->parseError = true;
                pImpl->errorMessage = "Unexpected argument: " + pImpl->currentArg;
                return false;
            }
            
            // Find first unprocessed argument
            size_t argIndex = 0;
            for (; argIndex < pImpl->arguments.size(); ++argIndex) {
                if (!pImpl->arguments[argIndex].processed) {
                    break;
                }
            }
            
            if (argIndex < pImpl->arguments.size()) {
                pImpl->arguments[argIndex].value = pImpl->currentArg;
                pImpl->arguments[argIndex].processed = true;
            }
            
            pImpl->currentArgIndex++;
        }
    }
    
    // Check for required options and arguments
    for (const auto& option : pImpl->options) {
        if (option.config.required && !option.present) {
            pImpl->parseError = true;
            pImpl->errorMessage = "Required option missing: " + option.config.name;
            return false;
        }
    }
    
    for (const auto& arg : pImpl->arguments) {
        if (arg.required && !arg.processed) {
            pImpl->parseError = true;
            pImpl->errorMessage = "Required argument missing: " + arg.name;
            return false;
        }
    }
    
    return true;
}

template<typename T>
T CLIParser::getValue(const std::string& name, const T& defaultValue) const {
    for (const auto& option : pImpl->options) {
        if (option.config.name == name && option.present) {
            try {
                return std::any_cast<T>(option.value);
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
    }
    return defaultValue;
}

std::string CLIParser::getStringValue(const std::string& name,
                                     const std::string& defaultValue) const {
    return getValue<std::string>(name, defaultValue);
}

bool CLIParser::getBoolValue(const std::string& name, bool defaultValue) const {
    for (const auto& option : pImpl->options) {
        if (option.config.name == name && option.present) {
            if (option.value.type() == typeid(bool)) {
                return std::any_cast<bool>(option.value);
            } else if (option.value.type() == typeid(std::string)) {
                std::string val = std::any_cast<std::string>(option.value);
                return val == "true" || val == "1" || val == "yes" || val == "on";
            }
        }
    }
    return defaultValue;
}

int CLIParser::getIntValue(const std::string& name, int defaultValue) const {
    try {
        return getValue<int>(name, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

double CLIParser::getDoubleValue(const std::string& name, double defaultValue) const {
    try {
        return getValue<double>(name, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

template<typename T>
T CLIParser::getArgument(size_t index, const T& defaultValue) const {
    if (index < pImpl->arguments.size() && pImpl->arguments[index].processed) {
        try {
            return std::any_cast<T>(pImpl->arguments[index].value);
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string CLIParser::getArgumentString(size_t index,
                                         const std::string& defaultValue) const {
    return getArgument<std::string>(index, defaultValue);
}

bool CLIParser::hasOption(const std::string& name) const {
    for (const auto& option : pImpl->options) {
        if (option.config.name == name && option.present) {
            return true;
        }
    }
    return false;
}

bool CLIParser::wasHelpRequested() const {
    return pImpl->helpRequested;
}

bool CLIParser::wasVersionRequested() const {
    return pImpl->versionRequested;
}

std::string CLIParser::getErrorMessage() const {
    return pImpl->errorMessage;
}

void CLIParser::printHelp() const {
    if (pImpl->helpPrinted) {
        return;
    }
    
    pImpl->helpPrinted = true;
    
    // Print program name and description (should be added by caller)
    std::cout << "Usage: nlm [OPTIONS] [ARGUMENTS]" << std::endl << std::endl;
    
    // Group options by category
    std::map<std::string, std::vector<const Impl::Option*>> optionGroups;
    
    for (const auto& option : pImpl->options) {
        optionGroups[option.config.category].push_back(&option);
    }
    
    // Print each option group
    for (const auto& group : optionGroups) {
        std::cout << "  " + group.first + ":" << std::endl;
        
        for (const auto* option : group.second) {
            // Print option name
            std::cout << "    ";
            
            if (!option->config.shortName.empty()) {
                std::cout << "-" + option->config.shortName << ", ";
            }
            
            if (!option->config.name.empty()) {
                std::cout << "--" + option->config.name;
                
                if (option->config.type == CLIType::Value) {
                    std::cout << "=" << option->config.defaultValue;
                }
            }
            
            std::cout << "\n      " << option->config.description;
            
            // Print default value if not empty
            if (!option->config.defaultValue.empty() && 
                (option->config.type == CLIType::Value || option->config.type == CLIType::Flag)) {
                std::cout << " (default: " + option->config.defaultValue + ")";
            }
            
            // Print if required
            if (option->config.required) {
                std::cout << " (required)";
            }
            
            std::cout << std::endl;
        }
    }
    
    // Print arguments
    if (!pImpl->arguments.empty()) {
        std::cout << "\n  Arguments:" << std::endl;
        for (const auto& arg : pImpl->arguments) {
            std::cout << "    " << arg.name;
            if (arg.required) {
                std::cout << " (required)";
            }
            std::cout << "\n      " << arg.description << std::endl;
        }
    }
    
    // Print help/version options
    if (pImpl->helpRequested) {
        std::cout << "\n  --help, -h\n      Print this help message and exit" << std::endl;
    }
    
    if (pImpl->versionRequested) {
        std::cout << "\n  --version, -v\n      Print version information and exit" << std::endl;
    }
}

void CLIParser::printVersion() const {
    if (pImpl->versionRequested) {
        std::cout << "NLM " + pImpl->versionInfo << std::endl;
    }
}

void CLIParser::applyToConfig(Config& config, ConfigSource configSource) const {
    for (const auto& option : pImpl->options) {
        if (option.present) {
            std::string key = option.config.name;
            
            // Convert value based on option type
            if (option.value.type() == typeid(int)) {
                config.set(key, std::any_cast<int>(option.value), configSource);
            } else if (option.value.type() == typeid(double)) {
                config.set(key, std::any_cast<double>(option.value), configSource);
            } else if (option.value.type() == typeid(bool)) {
                config.set(key, std::any_cast<bool>(option.value), configSource);
            } else if (option.value.type() == typeid(std::string)) {
                config.set(key, std::any_cast<std::string>(option.value), configSource);
            }
        }
    }
}

bool CLIParser::validateAll() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cerr << "Error: " + error << std::endl;
        }
        return false;
    }
    return true;
}

std::vector<std::string> CLIParser::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& option : pImpl->options) {
        if (option.config.validator) {
            try {
                // Try to get value from option
                std::string valueStr;
                if (option.value.type() == typeid(int)) {
                    valueStr = std::to_string(std::any_cast<int>(option.value));
                } else if (option.value.type() == typeid(double)) {
                    valueStr = std::to_string(std::any_cast<double>(option.value));
                } else if (option.value.type() == typeid(bool)) {
                    valueStr = std::any_cast<bool>(option.value) ? "true" : "false";
                } else if (option.value.type() == typeid(std::string)) {
                    valueStr = std::any_cast<std::string>(option.value);
                }
                
                if (!option.config.validator(valueStr)) {
                    errors.push_back("Invalid value for option " + option.config.name + ": " + valueStr);
                }
            } catch (...) {
                errors.push_back("Error validating option " + option.config.name);
            }
        }
    }
    
    return errors;
}

} // namespace nlm
