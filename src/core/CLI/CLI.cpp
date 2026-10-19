#include "CLI.h"
#include "CommonArgs.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <fstream>

namespace nlm {

struct CLI::Impl {
    ProgramInfo programInfo;
    std::vector<Argument> arguments;
    std::map<std::string, std::string> parsedArgs;
    std::map<std::string, std::string> defaultArgs;
    std::vector<std::string> extraArgs;
    bool helpRequested = false;
    bool versionRequested = false;
    bool validateRequested = false;
    std::string argString;
    
    // Categories for help organization
    std::set<CommandCategory> categoriesFound;
};

CLI::CLI() : pImpl(std::make_unique<Impl>()) {
    setupDefaultArguments();
}

void CLI::setProgramInfo(const ProgramInfo& info) {
    pImpl->programInfo = info;
}

void CLI::addArgument(const std::string& name, char shortName, 
                     const std::string& description, const std::string& defaultValue,
                     bool hasValue, const std::function<void(const std::string&)>& callback,
                     CommandCategory category) {
    Argument arg(name, shortName, description, defaultValue, hasValue, callback);
    pImpl->arguments.push_back(arg);
    
    if (!defaultValue.empty()) {
        pImpl->defaultArgs[name] = defaultValue;
    }
    
    pImpl->categoriesFound.insert(category);
}

bool CLI::parse(int argc, char* argv[]) {
    pImpl->argString = "";
    for (int i = 1; i < argc; ++i) {
        pImpl->argString += std::string(argv[i]) + " ";
    }
    
    // Check for special flags first
    std::vector<std::string> args(argv + 1, argv + argc);
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h" || arg == "/help") {
            pImpl->helpRequested = true;
            continue;
        }
        
        if (arg == "--version" || arg == "-v" || arg == "/version") {
            pImpl->versionRequested = true;
            continue;
        }
        
        if (arg == "--validate" || arg == "-V") {
            pImpl->validateRequested = true;
            continue;
        }
        
        // Parse regular arguments
        if (parseArgument(arg, pImpl->parsedArgs)) {
            // Successfully parsed
            if (i + 1 < args.size() && !startsWith(args[i + 1], "-")) {
                // Move past the value for next iteration
                i++;
            }
        } else {
            pImpl->extraArgs.push_back(arg);
        }
    }
    
    // Apply defaults for any arguments that weren't specified
    for (const auto& arg : pImpl->arguments) {
        if (pImpl->parsedArgs.find(arg.name) == pImpl->parsedArgs.end() && 
            !arg.defaultValue.empty()) {
            pImpl->parsedArgs[arg.name] = arg.defaultValue;
            if (arg.callback) {
                arg.callback(arg.defaultValue);
            }
        }
    }
    
    return true;
}

bool CLI::parseArgument(const std::string& arg, std::map<std::string, std::string>& parsedArgs) {
    // Check if it starts with --
    if (arg.substr(0, 2) == "--") {
        std::string name = arg.substr(2);
        size_t eqPos = name.find('=');
        
        if (eqPos != std::string::npos) {
            // --name=value format
            std::string value = name.substr(eqPos + 1);
            name = name.substr(0, eqPos);
            
            // Find argument definition
            for (const auto& argDef : pImpl->arguments) {
                if (argDef.name == name) {
                    parsedArgs[name] = value;
                    if (argDef.callback) {
                        argDef.callback(value);
                    }
                    return true;
                }
            }
        } else {
            // --name format (boolean flag)
            for (const auto& argDef : pImpl->arguments) {
                if (argDef.name == name) {
                    // For boolean flags, if no value, set to "true"
                    if (!argDef.hasValue) {
                        parsedArgs[name] = "true";
                        return true;
                    }
                    return false; // This argument requires a value
                }
            }
        }
    } else if (arg.size() == 1 && arg[0] >= 'a' && arg[0] <= 'z') {
        // Single character short argument
        char shortChar = arg[0];
        
        for (const auto& argDef : pImpl->arguments) {
            if (argDef.shortName == shortChar) {
                if (argDef.hasValue) {
                    return false; // This argument requires a value
                } else {
                    parsedArgs[argDef.name] = "true";
                    return true;
                }
            }
        }
    }
    
    return false;
}

std::string CLI::getString(const std::string& name, const std::string& defaultValue) const {
    auto it = pImpl->parsedArgs.find(name);
    if (it != pImpl->parsedArgs.end()) {
        return it->second;
    }
    
    // Check defaults
    auto defaultIt = pImpl->defaultArgs.find(name);
    if (defaultIt != pImpl->defaultArgs.end()) {
        return defaultIt->second;
    }
    
    return defaultValue;
}

int CLI::getInt(const std::string& name, int defaultValue) const {
    std::string value = getString(name);
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

float CLI::getFloat(const std::string& name, float defaultValue) const {
    std::string value = getString(name);
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stof(value);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool CLI::getBool(const std::string& name) const {
    std::string value = getString(name);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

bool CLI::hasArgument(const std::string& name) const {
    return pImpl->parsedArgs.find(name) != pImpl->parsedArgs.end();
}

const std::vector<std::string>& CLI::getExtraArgs() const {
    return pImpl->extraArgs;
}

void CLI::printHelp() const {
    std::cout << "\n";
    
    if (!pImpl->programInfo.name.empty()) {
        std::cout << pImpl->programInfo.name << " " << pImpl->programInfo.version << "\n";
        std::cout << std::string(pImpl->programInfo.name.length() + pImpl->programInfo.version.length(), '=') << "\n\n";
        
        std::cout << pImpl->programInfo.description << "\n\n";
        
        if (!pImpl->programInfo.author.empty()) {
            std::cout << "Author: " << pImpl->programInfo.author << "\n";
        }
        
        if (!pImpl->programInfo.website.empty()) {
            std::cout << "Website: " << pImpl->programInfo.website << "\n";
        }
        
        std::cout << "\n";
    }
    
    printArgumentHelp(CommandCategory::BASIC);
    printArgumentHelp(CommandCategory::CONFIG);
    printArgumentHelp(CommandCategory::OUTPUT);
    printArgumentHelp(CommandCategory::DEBUG);
    printArgumentHelp(CommandCategory::ADVANCED);
    
    std::cout << "\nExamples:\n";
    std::cout << "  " << pImpl->programInfo.name << " " << CommonArgs::EPISODES_LONG << " 10 " << CommonArgs::STEPS_LONG << " 500\n";
    std::cout << "  " << pImpl->programInfo.name << " " << CommonArgs::CONFIG_LONG << " my_config.cfg " << CommonArgs::DEBUG_LONG << "\n";
    std::cout << "  " << pImpl->programInfo.name << " --help\n";
    
    std::cout << "\nFor more information, visit: " << (pImpl->programInfo.website.empty() ? 
        "https://github.com/nlm-project" : pImpl->programInfo.website) << "\n";
}

void CLI::printVersion() const {
    if (pImpl->programInfo.name.empty()) {
        std::cout << "NLM " << pImpl->programInfo.version << "\n";
        return;
    }
    
    std::cout << pImpl->programInfo.name << " " << pImpl->programInfo.version << "\n";
    std::cout << "Phase " << getVersionNumber(pImpl->programInfo.version) << " Demo Program\n";
    std::cout << "Author: " << pImpl->programInfo.author << "\n";
    
    if (!pImpl->programInfo.website.empty()) {
        std::cout << "Website: " << pImpl->programInfo.website << "\n";
    }
}

void CLI::printConfig() const {
    std::cout << "Current Configuration:\n";
    std::cout << "====================\n\n";
    
    for (const auto& arg : pImpl->arguments) {
        auto it = pImpl->parsedArgs.find(arg.name);
        if (it != pImpl->parsedArgs.end()) {
            std::cout << "  " << std::left << std::setw(25) << arg.name << ": " << it->second << "\n";
        } else if (!arg.defaultValue.empty()) {
            std::cout << "  " << std::left << std::setw(25) << arg.name << ": " << arg.defaultValue << " (default)\n";
        }
    }
}

bool CLI::isHelpRequested() const {
    return pImpl->helpRequested;
}

bool CLI::isVersionRequested() const {
    return pImpl->versionRequested;
}

const std::string& CLI::getArgString() const {
    return pImpl->argString;
}

void CLI::setupDefaultArguments() {
    // Add common arguments
    addArgument(CommonArgs::EPISODES_LONG, CommonArgs::EPISODES_SHORT,
               "Number of episodes to run", "5",
               true, nullptr, CommandCategory::BASIC);
    
    addArgument(CommonArgs::STEPS_LONG, CommonArgs::STEPS_SHORT,
               "Number of steps per episode", "200",
               true, nullptr, CommandCategory::BASIC);
    
    addArgument(CommonArgs::CONFIG_LONG, CommonArgs::CONFIG_SHORT,
               "Configuration file path", "",
               true, nullptr, CommandCategory::CONFIG);
    
    addArgument(CommonArgs::SEED_LONG, CommonArgs::SEED_SHORT,
               "Random seed for reproducible results", "42",
               true, nullptr, CommandCategory::CONFIG);
    
    addArgument(CommonArgs::LOG_LEVEL_LONG, CommonArgs::LOG_LEVEL_SHORT,
               "Logging level (DEBUG, INFO, WARNING, ERROR)", "INFO",
               true, nullptr, CommandCategory::OUTPUT);
    
    addArgument(CommonArgs::QUIET_LONG, CommonArgs::QUIET_SHORT,
               "Suppress most output (quiet mode)", "false",
               false, nullptr, CommandCategory::OUTPUT);
    
    addArgument(CommonArgs::VERBOSE_LONG, CommonArgs::VERBOSE_SHORT,
               "Enable verbose output", "false",
               false, nullptr, CommandCategory::OUTPUT);
    
    addArgument(CommonArgs::DEBUG_LONG, CommonArgs::DEBUG_SHORT,
               "Enable debug mode with additional diagnostics", "false",
               false, nullptr, CommandCategory::DEBUG);
    
    addArgument(CommonArgs::VALIDATE_LONG, CommonArgs::VALIDATE_SHORT,
               "Validate configuration parameters before running", "false",
               false, nullptr, CommandCategory::DEBUG);
    
    addArgument(CommonArgs::PROFILE_LONG, CommonArgs::PROFILE_SHORT,
               "Enable performance profiling", "false",
               false, nullptr, CommandCategory::ADVANCED);
    
    addArgument(CommonArgs::MODE_LONG, CommonArgs::MODE_SHORT,
               "Execution mode (standard, fast, thorough)", "standard",
               true, nullptr, CommandCategory::ADVANCED);
    
    addArgument(CommonArgs::INTERACTIVE_LONG, CommonArgs::INTERACTIVE_SHORT,
               "Enable interactive mode (if supported)", "false",
               false, nullptr, CommandCategory::ADVANCED);
}

void CLI::printArgumentHelp(CommandCategory category) const {
    if (pImpl->categoriesFound.find(category) == pImpl->categoriesFound.end()) {
        return;
    }
    
    std::string categoryName;
    switch (category) {
        case CommandCategory::BASIC: categoryName = "Basic Options"; break;
        case CommandCategory::CONFIG: categoryName = "Configuration Options"; break;
        case CommandCategory::OUTPUT: categoryName = "Output Options"; break;
        case CommandCategory::DEBUG: categoryName = "Debug Options"; break;
        case CommandCategory::ADVANCED: categoryName = "Advanced Options"; break;
    }
    
    std::cout << categoryName << "\n";
    std::cout << std::string(categoryName.length(), '-') << "\n\n";
    
    for (const auto& arg : pImpl->arguments) {
        // Check if this argument belongs to the current category
        bool isInCategory = false;
        switch (category) {
            case CommandCategory::BASIC:
                isInCategory = (arg.name == CommonArgs::EPISODES_LONG || 
                              arg.name == CommonArgs::STEPS_LONG);
                break;
            case CommandCategory::CONFIG:
                isInCategory = (arg.name == CommonArgs::CONFIG_LONG || 
                              arg.name == CommonArgs::SEED_LONG);
                break;
            case CommandCategory::OUTPUT:
                isInCategory = (arg.name == CommonArgs::LOG_LEVEL_LONG || 
                              arg.name == CommonArgs::QUIET_LONG ||
                              arg.name == CommonArgs::VERBOSE_LONG);
                break;
            case CommandCategory::DEBUG:
                isInCategory = (arg.name == CommonArgs::DEBUG_LONG || 
                              arg.name == CommonArgs::VALIDATE_LONG);
                break;
            case CommandCategory::ADVANCED:
                isInCategory = (arg.name == CommonArgs::PROFILE_LONG || 
                              arg.name == CommonArgs::MODE_LONG ||
                              arg.name == CommonArgs::INTERACTIVE_LONG);
                break;
        }
        
        if (isInCategory) {
            std::string shortHelp;
            if (arg.shortName != 0) {
                shortHelp = "  -" + std::string(1, arg.shortName) + ", ";
            } else {
                shortHelp = "      ";
            }
            
            shortHelp += "  --" + arg.name;
            if (!arg.defaultValue.empty()) {
                shortHelp += "=<value>";
            }
            
            std::cout << shortHelp << std::setw(40 - shortHelp.length()) << std::setfill(' ') << "";
            std::cout << arg.description;
            if (!arg.defaultValue.empty()) {
                std::cout << " (default: " << arg.defaultValue << ")";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\n";
}

std::string CLI::getVersionNumber(const std::string& versionString) const {
    // Extract version number from version string
    size_t pos = versionString.find_first_of("0123456789");
    if (pos == std::string::npos) {
        return "2";
    }
    
    size_t endPos = versionString.find_first_not_of("0123456789", pos);
    return versionString.substr(pos, endPos - pos);
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

} // namespace nlm
