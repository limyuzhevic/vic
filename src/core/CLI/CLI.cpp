#include "main.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>

using namespace nlm;

namespace {
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
}

struct CLI::Impl {
    std::string programName;
    std::string currentCommand;
    std::vector<std::string> args;
    
    struct Option {
        std::string name;
        std::string description;
        char shortOpt;
        bool required;
        std::string value;
        bool provided;
    };
    
    std::vector<Option> options;
    std::unordered_map<std::string, CommandHandler> commands;
    
    std::string error;
    bool helpRequested;
    bool versionRequested;
};

CLI::CLI(const std::string& programName) : pImpl(std::make_unique<Impl>()), helpRequested(false), versionRequested(false) {
    pImpl->programName = programName;
}

CLI::~CLI() = default;

void CLI::addCommand(const std::string& name, const std::string& description,
                     std::function<void(const CLI&)> handler) {
    pImpl->commands[name] = handler;
}

void CLI::addOption(const std::string& name, const std::string& description,
                    char shortOpt, bool required) {
    Impl::Option option;
    option.name = name;
    option.description = description;
    option.shortOpt = shortOpt;
    option.required = required;
    option.value = "";
    option.provided = false;
    pImpl->options.push_back(option);
}

void CLI::setOption(const std::string& name, const std::string& value) {
    for (auto& option : pImpl->options) {
        if (toLower(option.name) == toLower(name)) {
            option.value = value;
            option.provided = true;
            return;
        }
    }
    pImpl->options.push_back({name, "", 0, false, value, true});
}

std::string CLI::getOption(const std::string& name) const {
    for (const auto& option : pImpl->options) {
        if (toLower(option.name) == toLower(name)) {
            return option.value;
        }
    }
    return "";
}

bool CLI::hasOption(const std::string& name) const {
    for (const auto& option : pImpl->options) {
        if (toLower(option.name) == toLower(name)) {
            return option.provided;
        }
    }
    return false;
}

bool CLI::parse(int argc, char** argv) {
    pImpl->args.clear();
    pImpl->currentCommand = "";
    
    // Reset flags
    helpRequested = false;
    versionRequested = false;
    
    bool inCommand = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for help/version
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            continue;
        }
        if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            continue;
        }
        
        // Check for command
        if (!inCommand && pImpl->commands.find(arg) != pImpl->commands.end()) {
            pImpl->currentCommand = arg;
            inCommand = true;
            continue;
        }
        
        // Check for option (--name value or --name=value)
        if (arg.find("--") == 0) {
            size_t equals = arg.find("=", 2);
            if (equals != std::string::npos) {
                std::string name = arg.substr(2, equals - 2);
                std::string value = arg.substr(equals + 1);
                setOption(name, value);
            } else {
                // Check if next argument is a value (not starting with --)
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string name = arg.substr(2);
                    std::string value = argv[++i];
                    setOption(name, value);
                } else {
                    // Option without value (boolean)
                    std::string name = arg.substr(2);
                    setOption(name, "true");
                }
            }
        } else if (arg.find("-") == 0 && arg.length() == 2) {
            // Short option (-v)
            char shortOpt = arg[1];
            for (auto& option : pImpl->options) {
                if (option.shortOpt == shortOpt) {
                    setOption(option.name, "true");
                    break;
                }
            }
        } else {
            // Positional argument
            pImpl->args.push_back(arg);
            inCommand = false;
        }
    }
    
    return !helpRequested && !versionRequested;
}

std::string CLI::getCommand() const {
    return pImpl->currentCommand;
}

std::vector<std::string> CLI::getArgs() const {
    return pImpl->args;
}

void CLI::printHelp() const {
    std::cout << "Usage: " << pImpl->programName << " [OPTIONS] COMMAND [ARGS...]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Global Options:" << std::endl;
    for (const auto& option : pImpl->options) {
        if (option.shortOpt) {
            std::cout << "  -" << option.shortOpt << ", ";
        } else {
            std::cout << "      ";
        }
        std::cout << "--" << option.name;
        if (option.required) std::cout << " [required]";
        std::cout << "\n        " << option.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    for (const auto& cmd : pImpl->commands) {
        std::cout << "  " << cmd.first << "\n        " << cmd.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "For more information about a specific command, use:" << std::endl;
    std::cout << "  " << pImpl->programName << " COMMAND --help" << std::endl;
}

void CLI::printVersion() const {
    std::cout << pImpl->programName << " version 0.1.0" << std::endl;
}

std::string CLI::getProgramName() const {
    return pImpl->programName;
}

bool CLI::validate() const {
    pImpl->error.clear();
    
    for (const auto& option : pImpl->options) {
        if (option.required && !option.provided) {
            pImpl->error = "Missing required option: " + option.name;
            return false;
        }
    }
    
    return true;
}

std::string CLI::getError() const {
    return pImpl->error;
}

struct CLIConfig::Impl {
    std::unordered_map<std::string, std::string> configValues;
    std::unordered_map<std::string, std::string> cliValues;
};

CLIConfig::CLIConfig() : pImpl(std::make_unique<Impl>()) {}
CLIConfig::~CLIConfig() = default;

bool CLIConfig::load(const std::string& configFile, const CLI& cli) {
    pImpl->configValues.clear();
    pImpl->cliValues.clear();
    
    // Load from file
    auto config = std::make_shared<Config>();
    if (!configFile.empty() && config->loadFromFile(configFile)) {
        // Extract string values from config
        for (const auto& key : config->getKeys()) {
            auto value = config->get<std::string>(key);
            if (value) {
                pImpl->configValues[key] = *value;
            }
        }
    }
    
    // Override with CLI values
    for (const auto& option : cli.getCLI().options) {
        if (option.provided) {
            pImpl->cliValues[option.name] = option.value;
        }
    }
    
    return true;
}

template<typename T>
T CLIConfig::getValue(const std::string& key, const T& defaultValue) const {
    // First check CLI override
    auto cliIt = pImpl->cliValues.find(key);
    if (cliIt != pImpl->cliValues.end()) {
        // Convert string to T (simplified implementation)
        return defaultValue; // TODO: Implement proper conversion
    }
    
    // Then check file
    auto fileIt = pImpl->configValues.find(key);
    if (fileIt != pImpl->configValues.end()) {
        return defaultValue; // TODO: Implement proper conversion
    }
    
    return defaultValue;
}

bool CLIConfig::save(const std::string& filepath) const {
    auto config = std::make_shared<Config>();
    
    // Save CLI overrides first
    for (const auto& pair : pImpl->cliValues) {
        config->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Save file values
    for (const auto& pair : pImpl->configValues) {
        auto cliIt = pImpl->cliValues.find(pair.first);
        if (cliIt == pImpl->cliValues.end()) {
            config->set(pair.first, pair.second, ConfigSource::File);
        }
    }
    
    return config->saveToFile(filepath);
}

struct NLMApplication::Impl {
    std::string name;
    std::string version;
    CLI cli;
    std::unordered_map<std::string, CommandHandler> commands;
};

NLMApplication::NLMApplication(const std::string& name, const std::string& version)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->version = version;
    pImpl->cli = CLI(name);
}

NLMApplication::~NLMApplication() = default;

int NLMApplication::run(int argc, char** argv) {
    // Register commands and options
    registerCommands();
    registerOptions();
    
    // Parse arguments
    if (!pImpl->cli.parse(argc, argv)) {
        if (pImpl->cli.hasHelp()) {
            handleHelp();
        } else if (pImpl->cli.hasVersion()) {
            handleVersion();
        }
        return 1;
    }
    
    // Validate
    if (!pImpl->cli.validate()) {
        std::cerr << "Error: " << pImpl->cli.getError() << std::endl;
        return 1;
    }
    
    // Execute command
    std::string command = pImpl->cli.getCommand();
    if (command.empty()) {
        std::cerr << "Error: No command specified" << std::endl;
        pImpl->cli.printHelp();
        return 1;
    }
    
    auto it = pImpl->commands.find(command);
    if (it == pImpl->commands.end()) {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        pImpl->cli.printHelp();
        return 1;
    }
    
    it->second(pImpl->cli);
    return 0;
}

CLI& NLMApplication::getCLI() {
    return pImpl->cli;
}

std::string NLMApplication::getName() const {
    return pImpl->name;
}

std::string NLMApplication::getVersion() const {
    return pImpl->version;
}

void NLMApplication::registerCommands() {}
void NLMApplication::registerOptions() {}

void NLMApplication::handleHelp() const {
    std::cout << "Help requested for " << getName() << std::endl;
}

void NLMApplication::handleVersion() const {
    std::cout << getName() << " version " << getVersion() << std::endl;
}

void NLMApplication::handleConfig() const {
    // Default implementation
}
