// NLM CLI Framework Implementation

#include "core/CLI/CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// ArgumentParser implementation
ArgumentParser::ArgumentParser(const std::string& programName, const std::string& version,
                               const std::string& description)
    : programName_(programName), version_(version), description_(description) {
    setupBuiltInArguments();
}

void ArgumentParser::setupBuiltInArguments() {
    // Help argument
    addArgument('h', "help", "Print help message and exit", "",
                false, "");

    // Version argument
    addArgument('v', "version", "Print version information and exit", "",
                false, "");

    // Log level argument
    addArgument('l', "log-level", "Set logging level (debug, info, warning, error, critical)", "info",
                true, "log-level");

    // Log file argument
    addArgument('f', "log-file", "Write logs to file in addition to console", "",
                true, "log-file");

    // No colors flag
    addArgument('', "no-colors", "Disable colored output", "",
                false, "");
}

void ArgumentParser::addArgument(char shortOpt, const std::string& longOpt,
                                 const std::string& description,
                                 const std::string& defaultValue,
                                 bool hasValue,
                                 const std::string& validatorName) {
    ArgumentInfo info;
    info.name = longOpt;
    info.shortOpt = shortOpt;
    info.longOpt = longOpt;
    info.description = description;
    info.defaultValue = defaultValue;
    info.hasValue = hasValue;
    info.isSet = false;

    // Set up validator if needed
    if (validatorName == "positive-int") {
        info.validator = [](const std::string& value) {
            try {
                int val = std::stoi(value);
                if (val <= 0) {
                    throw CLIError("Value must be positive");
                }
            } catch (const std::exception& e) {
                throw CLIError(std::string("Invalid positive integer: ") + e.what());
            }
        };
    } else if (validatorName == "positive-double") {
        info.validator = [](const std::string& value) {
            try {
                double val = std::stod(value);
                if (val <= 0.0) {
                    throw CLIError("Value must be positive");
                }
            } catch (const std::exception& e) {
                throw CLIError(std::string("Invalid positive double: ") + e.what());
            }
        };
    } else if (validatorName == "log-level") {
        info.validator = [](const std::string& value) {
            static const std::vector<std::string> validLevels = {
                "debug", "info", "warning", "error", "critical"
            };
            if (std::find(validLevels.begin(), validLevels.end(), value) == validLevels.end()) {
                throw CLIError("Invalid log level. Must be one of: debug, info, warning, error, critical");
            }
        };
    } else if (validatorName == "file-path") {
        info.validator = [](const std::string& value) {
            std::ifstream file(value);
            if (!file.good()) {
                // Don't throw for file not existing, it might be created later
            }
        };
    }

    arguments_.push_back(info);
    argumentMap_[longOpt] = &arguments_.back();
}

void ArgumentParser::parse(int argc, char* argv[]) {
    processArguments(argc, argv);
    validateArguments();
}

void ArgumentParser::processArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check for --help or -h
        if (arg == "--help" || arg == "-h") {
            if (argumentMap_["help"]) {
                argumentMap_["help"]->isSet = true;
            }
            continue;
        }

        // Check for --version or -v
        if (arg == "--version" || arg == "-v") {
            if (argumentMap_["version"]) {
                argumentMap_["version"]->isSet = true;
            }
            continue;
        }

        // Check for --config or -c (if we add it)
        if (arg.find("--config") == 0 || arg.find("-c") == 0) {
            // Handle config file argument
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else {
                if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            }
            if (argumentMap_["config"]) {
                argumentMap_["config"]->isSet = true;
                argumentMap_["config"]->value = configFile;
            }
            continue;
        }

        // Handle regular arguments (key=value or --key value)
        if (arg.find('=') != std::string::npos) {
            // Key=value format
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            key = trim(key);
            value = trim(value);

            if (argumentMap_.find(key) != argumentMap_.end()) {
                auto* info = argumentMap_[key];
                if (!info->hasValue) {
                    throw CLIError("Argument --" + key + " does not take a value");
                }
                info->isSet = true;
                info->value = value;
                if (info->validator) {
                    info->validator(value);
                }
            } else {
                // Unknown argument
                throw CLIError("Unknown argument: " + key);
            }
        } else if (arg.find("--") == 0) {
            // Long option without =, check if it's a flag
            std::string key = arg.substr(2);
            if (argumentMap_.find(key) != argumentMap_.end()) {
                auto* info = argumentMap_[key];
                if (info->hasValue) {
                    // Need to get next argument as value
                    if (i + 1 < argc) {
                        info->isSet = true;
                        info->value = argv[++i];
                        if (info->validator) {
                            info->validator(info->value);
                        }
                    } else {
                        throw CLIError("Argument --" + key + " requires a value");
                    }
                } else {
                    info->isSet = true;
                }
            } else {
                throw CLIError("Unknown argument: --" + key);
            }
        } else if (arg.find("-") == 0 && arg.length() > 1) {
            // Short option (-x)
            std::string shortOpt = arg.substr(1);
            bool found = false;
            for (auto& info : arguments_) {
                if (info.shortOpt == shortOpt[0]) {
                    found = true;
                    if (info.hasValue) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            info->isSet = true;
                            info->value = argv[++i];
                            if (info->validator) {
                                info->validator(info->value);
                            }
                        } else {
                            throw CLIError("Argument -" + shortOpt + " requires a value");
                        }
                    } else {
                        info->isSet = true;
                    }
                    break;
                }
            }
            if (!found) {
                throw CLIError("Unknown short option: -" + shortOpt);
            }
        } else {
            // Positional argument (treat as config file)
            unknownArgValue = arg;
        }
    }
}

void ArgumentParser::validateArguments() const {
    // Check for help/version first (these don't require other arguments)
    if (hasArgument("help") || hasArgument("version")) {
        return;
    }

    // Validate required arguments
    for (const auto& info : arguments_) {
        if (info.isSet && info.validator) {
            info.validator(info.value);
        }
    }
}

std::string ArgumentParser::getValue(const std::string& longOpt) const {
    auto it = argumentMap_.find(longOpt);
    if (it != argumentMap_.end() && it->second->isSet) {
        return it->second->value;
    }
    return it != argumentMap_.end() ? it->second->defaultValue : "";
}

int ArgumentParser::getInt(const std::string& longOpt, int defaultValue) const {
    std::string value = getValue(longOpt);
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stoi(value);
    } catch (const std::exception& e) {
        throw CLIError(std::string("Invalid integer value for --") + longOpt + ": " + e.what());
    }
}

double ArgumentParser::getDouble(const std::string& longOpt, double defaultValue) const {
    std::string value = getValue(longOpt);
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stod(value);
    } catch (const std::exception& e) {
        throw CLIError(std::string("Invalid double value for --") + longOpt + ": " + e.what());
    }
}

bool ArgumentParser::getBool(const std::string& longOpt, bool defaultValue) const {
    std::string value = getValue(longOpt);
    if (value.empty()) {
        return defaultValue;
    }
    // Convert string to bool (true/false, yes/no, on/off, 1/0)
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lower == "true" || lower == "yes" || lower == "on" || lower == "1") {
        return true;
    } else if (lower == "false" || lower == "no" || lower == "off" || lower == "0") {
        return false;
    } else {
        throw CLIError(std::string("Invalid boolean value for --") + longOpt + ": " + value);
    }
}

bool ArgumentParser::hasArgument(const std::string& longOpt) const {
    auto it = argumentMap_.find(longOpt);
    return it != argumentMap_.end() && it->second->isSet;
}

void ArgumentParser::printHelp() const {
    std::cout << std::endl;
    std::cout << "  " << programName_ << ": " << description_ << std::endl;
    std::cout << std::endl;
    std::cout << "  Usage: " << programName_ << " [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << std::endl;

    // Print arguments grouped by category
    std::cout << "  Arguments:" << std::endl;
    for (const auto& info : arguments_) {
        if (!info.hasValue) {
            // Flags
            if (info.shortOpt) {
                std::cout << "    -" << info.shortOpt << ", --" << info.name << "\n";
                std::cout << "        " << info.description << std::endl;
            } else {
                std::cout << "    --" << info.name << "\n";
                std::cout << "        " << info.description << std::endl;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "  Options with values:" << std::endl;
    for (const auto& info : arguments_) {
        if (info.hasValue) {
            if (info.shortOpt) {
                std::cout << "    -" << info.shortOpt << " " << info.name << "=VALUE, --" << info.name << "=VALUE\n";
                std::cout << "        " << info.description << " (default: " << info.defaultValue << ")" << std::endl;
            } else {
                std::cout << "    --" << info.name << "=VALUE\n";
                std::cout << "        " << info.description << " (default: " << info.defaultValue << ")" << std::endl;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "  Examples:" << std::endl;
    std::cout << "    " << programName_ << " --help" << std::endl;
    std::cout << "    " << programName_ << " --version" << std::endl;
    std::cout << "    " << programName_ << " --config=experiments/my_config.cfg" << std::endl;
    std::cout << "    " << programName_ << " --log-level=debug --log-file=nlm.log" << std::endl;
    std::cout << std::endl;
}

void ArgumentParser::printVersion() const {
    std::cout << programName_ << " " << version_ << std::endl;
    std::cout << description_ << std::endl;
}

std::string ArgumentParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool ArgumentParser::isFlag(const std::string& arg) {
    return arg.find('=') == std::string::npos;
}

// LoggerController implementation
struct LoggerController::Impl {
    LogLevel currentLevel = LogLevel::Info;
    bool useColors = true;
    std::ofstream logFile;
};

LoggerController::LoggerController() {
    pImpl = std::make_unique<Impl>();
}

void LoggerController::configure(const std::string& logLevel, const std::string& logFile,
                                 bool enableColors) {
    // Set log level
    if (logLevel == "debug") pImpl->currentLevel = LogLevel::Debug;
    else if (logLevel == "warning") pImpl->currentLevel = LogLevel::Warning;
    else if (logLevel == "error") pImpl->currentLevel = LogLevel::Error;
    else if (logLevel == "critical") pImpl->currentLevel = LogLevel::Critical;
    else pImpl->currentLevel = LogLevel::Info;

    pImpl->useColors = enableColors;

    // Open log file if specified
    if (!logFile.empty()) {
        pImpl->logFile.open(logFile, std::ios::app);
    }

    // Set global logger level
    Logger::getGlobal().setLevel(pImpl->currentLevel);
}

std::string LoggerController::getLogLevel() const {
    switch (pImpl->currentLevel) {
        case LogLevel::Debug: return "debug";
        case LogLevel::Info: return "info";
        case LogLevel::Warning: return "warning";
        case LogLevel::Error: return "error";
        case LogLevel::Critical: return "critical";
        default: return "info";
    }
}

// CLIConfigHelper implementation
struct CLIConfigHelper::Impl {
    bool validateConfig(const Config& config) const {
        // Basic validation - can be extended
        bool isValid = true;

        // Check for required keys
        std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count"
        };

        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                std::cerr << "WARNING: Missing required configuration key: " << key << std::endl;
                isValid = false;
            }
        }

        return isValid;
    }
};

CLIConfigHelper::CLIConfigHelper() {
    pImpl = std::make_unique<Impl>();
}

bool CLIConfigHelper::loadConfig(std::shared_ptr<Config> config, int argc, char* argv[]) {
    // Try to load from command line arguments
    if (config->loadFromArgs(argc, argv)) {
        NLM_LOG_INFO("Configuration loaded from command line arguments");
    }

    // Set default values for critical keys
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    return true;
}

bool CLIConfigHelper::validateConfig(const Config& config) const {
    return pImpl->validateConfig(config);
}

// Validation functions implementation
void CLI::validatePositiveInt(const std::string& value) {
    try {
        int val = std::stoi(value);
        if (val <= 0) {
            throw CLIError("Value must be positive");
        }
    } catch (const std::exception& e) {
        throw CLIError(std::string("Invalid positive integer: ") + e.what());
    }
}

void CLI::validatePositiveDouble(const std::string& value) {
    try {
        double val = std::stod(value);
        if (val <= 0.0) {
            throw CLIError("Value must be positive");
        }
    } catch (const std::exception& e) {
        throw CLIError(std::string("Invalid positive double: ") + e.what());
    }
}

void CLI::validateFilePath(const std::string& value) {
    std::ifstream file(value);
    if (!file.good()) {
        // Don't throw for file not existing, it might be created later
    }
}

void CLI::validateLogLevel(const std::string& value) {
    static const std::vector<std::string> validLevels = {
        "debug", "info", "warning", "error", "critical"
    };
    if (std::find(validLevels.begin(), validLevels.end(), value) == validLevels.end()) {
        throw CLIError("Invalid log level. Must be one of: debug, info, warning, error, critical");
    }
}

} // namespace nlm
