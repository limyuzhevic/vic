#include "cli/CLI.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <cstring>
#include <cmath>
#include <filesystem>
#include <chrono>

namespace nlm {

CLIArgs::CLIArgs(int argc, char** argv) {
    args_.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        args_.push_back(argv[i]);
    }
    helpRequested_ = false;
    parseArgs();
}

void CLIArgs::registerCommand(const std::string& name, const std::string& description,
                            const std::string& usage, const std::function<bool(const CLIArgs&)>& executor,
                            const std::vector<std::string>& options) {
    Command cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.usage = usage;
    cmd.examples.clear(); // Can be populated later if needed
    cmd.executor = executor;
    cmd.options = options;
    commands_.push_back(cmd);
}

void CLIArgs::addOption(const std::string& name, const std::string& value) {
    options_[name] = value;
}

bool CLIArgs::hasOption(const std::string& name) const {
    return options_.find(name) != options_.end();
}

std::string CLIArgs::getOption(const std::string& name) const {
    auto it = options_.find(name);
    if (it != options_.end()) {
        return it->second;
    }
    return "";
}

std::string CLIArgs::getOptionOrDefault(const std::string& name, const std::string& defaultValue) const {
    if (hasOption(name)) {
        return getOption(name);
    }
    return defaultValue;
}

int CLIArgs::getOptionAsInt(const std::string& name, int defaultValue) const {
    if (!hasOption(name)) return defaultValue;
    try {
        return std::stoi(getOption(name));
    } catch (...) {
        return defaultValue;
    }
}

double CLIArgs::getOptionAsDouble(const std::string& name, double defaultValue) const {
    if (!hasOption(name)) return defaultValue;
    try {
        return std::stod(getOption(name));
    } catch (...) {
        return defaultValue;
    }
}

bool CLIArgs::getOptionAsBool(const std::string& name, bool defaultValue) const {
    if (!hasOption(name)) return defaultValue;
    std::string value = getOption(name);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (value.empty()) return true; // Flag present, value is true
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

bool CLIArgs::executeCommand() const {
    if (commands_.empty() || command_.empty()) {
        return showHelp();
    }
    
    for (const auto& cmd : commands_) {
        if (cmd.name == command_) {
            return cmd.executor(*this);
        }
    }
    
    std::cerr << "Error: Unknown command: " << command_ << std::endl;
    showHelp();
    return false;
}

bool CLIArgs::showHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Enhanced CLI" << std::endl;
    std::cout << "===============================================" << std::endl << std::endl;
    
    if (!command_.empty()) {
        std::cout << "Error: Command not found: " << command_ << std::endl << std::endl;
    }
    
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands_) {
        std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
        std::cout << "    Usage: " << cmd.usage << std::endl;
        if (!cmd.examples.empty()) {
            std::cout << "    Examples:" << std::endl;
            for (const auto& example : cmd.examples) {
                std::cout << "      " << example << std::endl;
            }
        }
        if (!cmd.options.empty()) {
            std::cout << "    Options:" << std::endl;
            for (const auto& opt : cmd.options) {
                std::cout << "      " << opt << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << "Global options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --verbose, -V                 Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                   Suppress non-essential output" << std::endl;
    std::cout << "  --config FILE                 Load configuration from file" << std::endl;
    std::cout << "  --output DIR                  Set output directory" << std::endl;
    std::cout << "  --profile                     Enable profiling" << std::endl;
    std::cout << "  --profile-interval SECONDS    Set profiling interval" << std::endl;
    std::cout << "  --no-checkpointing            Disable checkpointing" << std::endl;
    std::cout << "  --checkpoint-interval SECONDS  Set checkpoint interval" << std::endl;
    std::cout << std::endl;
    
    return true;
}

bool CLIArgs::validate() const {
    validationErrors_.clear();
    return validateOptions();
}

std::string CLIArgs::getValidationErrors() const {
    return validationErrors_;
}

void CLIArgs::startTimer(const std::string& operation) {
    timers_[operation] = std::chrono::high_resolution_clock::now();
}

void CLIArgs::stopTimer(const std::string& operation) {
    auto it = timers_.find(operation);
    if (it != timers_.end()) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - it->second);
        // Store duration for later retrieval
        timers_[operation + "_duration"] = std::chrono::high_resolution_clock::time_point(
            std::chrono::duration<double>(duration.count()));
    }
}

double CLIArgs::getElapsedTime(const std::string& operation) const {
    auto it = timers_.find(operation);
    if (it == timers_.end()) return 0.0;
    
    auto it2 = timers_.find(operation + "_duration");
    if (it2 != timers_.end()) {
        return std::chrono::duration<double>(it2->second.time_since_epoch()).count();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - it->second);
    return duration.count();
}

std::string CLIArgs::getFormattedDuration(double seconds) const {
    if (seconds < 0.001) {
        return std::to_string(seconds * 1000000) + " μs";
    } else if (seconds < 1.0) {
        return std::to_string(seconds * 1000) + " ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + " s";
    } else if (seconds < 3600.0) {
        return std::to_string(seconds / 60) + " min";
    } else {
        return std::to_string(seconds / 3600) + " h";
    }
}

void CLIArgs::parseArgs() {
    helpRequested_ = false;
    command_ = "";
    options_.clear();
    
    // First pass: find the first non-option argument as the command
    bool inOptions = true;
    for (size_t i = 1; i < args_.size(); ++i) {
        const std::string& arg = args_[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested_ = true;
            break;
        } else if (arg == "--version" || arg == "-v") {
            command_ = "version";
            break;
        } else if (arg == "--verbose" || arg == "-V") {
            options_["verbose"] = "";
        } else if (arg == "--quiet" || arg == "-q") {
            options_["quiet"] = "";
        } else if (arg.find("--") == 0) {
            // Long option
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string name = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options_[name] = value;
            } else {
                options_[arg.substr(2)] = "";
            }
        } else if (arg.find('-') == 0 && arg.length() > 1 && !isdigit(arg[1])) {
            // Short option
            if (arg.length() > 2) {
                // Multiple short options like -abc
                for (size_t j = 1; j < arg.length(); ++j) {
                    options_[std::string(1, arg[j])] = "";
                }
            } else {
                options_[arg.substr(1)] = "";
            }
        } else {
            // This is the command
            inOptions = false;
            command_ = arg;
        }
    }
}

void CLIArgs::parseOptions() {
    // Options are already parsed in parseArgs, this method is for consistency
}

bool CLIArgs::validateOptions() const {
    validationErrors_.clear();
    
    // Basic validation
    if (hasOption("checkpoint-interval") && hasOption("no-checkpointing")) {
        validationErrors_ += "Error: Cannot specify --checkpoint-interval without --no-checkpointing\n";
        return false;
    }
    
    if (hasOption("profile-interval") && hasOption("profile")) {
        validationErrors_ += "Error: Cannot specify --profile-interval without --profile\n";
        return false;
    }
    
    // Validate numeric ranges
    if (hasOption("checkpoint-interval")) {
        try {
            double interval = std::stod(getOption("checkpoint-interval"));
            if (interval <= 0) {
                validationErrors_ += "Error: --checkpoint-interval must be positive\n";
                return false;
            }
        } catch (...) {
            validationErrors_ += "Error: --checkpoint-interval must be a valid number\n";
            return false;
        }
    }
    
    if (hasOption("profile-interval")) {
        try {
            double interval = std::stod(getOption("profile-interval"));
            if (interval <= 0) {
                validationErrors_ += "Error: --profile-interval must be positive\n";
                return false;
            }
        } catch (...) {
            validationErrors_ += "Error: --profile-interval must be a valid number\n";
            return false;
        }
    }
    
    return validationErrors_.empty();
}

std::string CLIArgs::formatOptionValue(const std::string& value) const {
    return "[" + value + "]";
}

} // namespace nlm
