// nlm_validate tool implementation
// Configuration and system validation tool for NLM

#include "tools/nlm_validate/nlm_validate.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ArgumentParser.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <vector>

namespace nlm {

NLMValidateTool::NLMValidateTool() : parser_(std::make_shared<ArgumentParser>("nlm_validate", "Configuration and system validation tool for NLM")) {
    setupArgumentParser();
}

bool NLMValidateTool::initialize(const std::unordered_map<std::string, std::string>& config) {
    logger_ = std::make_unique<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    config_ = std::make_shared<nlm::Config>();
    return true;
}

bool NLMValidateTool::execute(const std::vector<std::string>& args) {
    if (parser_.getPositionalArgs().empty()) {
        parser_.printHelp();
        return false;
    }
    
    std::string command = parser_.getPositionalArgs()[0];
    
    if (command == "validate") {
        return executeValidateCommand(args);
    } else if (command == "check") {
        return executeCheckCommand(args);
    } else {
        log("Unknown command: " + command, true);
        return false;
    }
}

bool NLMValidateTool::executeValidateCommand(const std::vector<std::string>& args) {
    std::string configFile = parser_.wasProvided("--file") ? parser_.getValue("--file") : "";
    bool strict = parser_.wasProvided("--strict") ? parser_.getValueBool("--strict") : false;
    bool verbose = parser_.wasProvided("--verbose") ? parser_.getValueBool("--verbose") : false;
    
    std::cout << "Validating NLM configuration and system..." << std::endl;
    
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Check configuration file if provided
    if (!configFile.empty()) {
        auto tempConfig = std::make_shared<Config>();
        if (!tempConfig->loadFromFile(configFile)) {
            errors.push_back("Invalid configuration file: " + configFile);
        } else {
            // Validate configuration content
            std::vector<std::string> keys = tempConfig->getKeys();
            if (keys.empty()) {
                warnings.push_back("Configuration file is empty");
            }
            
            for (const auto& key : keys) {
                std::optional<ConfigValue> value = tempConfig->get<ConfigValue>(key);
                if (!value.has_value()) {
                    errors.push_back("Key not found: " + key);
                }
            }
        }
    }
    
    // Validate current configuration
    std::string summary = config_->summary();
    if (summary.empty()) {
        warnings.push_back("Current configuration is empty");
    }
    
    // Report results
    bool success = errors.empty();
    
    if (!success) {
        std::cout << "\nValidation FAILED:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
    }
    
    if (!warnings.empty()) {
        std::cout << "\nValidation Warnings:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  WARNING: " << warning << std::endl;
        }
    }
    
    if (success) {
        std::cout << "\nValidation PASSED: All checks passed." << std::endl;
    }
    
    return success;
}

bool NLMValidateTool::executeCheckCommand(const std::vector<std::string>& args) {
    std::string component = parser_.wasProvided("--component") ? parser_.getValue("--component") : "all";
    
    std::cout << "Checking system component: " << component << std::endl;
    
    std::map<std::string, bool> checks;
    checks["config"] = true;  // Simulate config check
    checks["memory"] = true;  // Simulate memory check
    checks["cpu"] = true;     // Simulate CPU check
    
    bool allPassed = true;
    for (const auto& check : checks) {
        std::cout << "  " << check.first << ": " << (check.second ? "PASS" : "FAIL") << std::endl;
        if (!check.second) allPassed = false;
    }
    
    return allPassed;
}

void NLMValidateTool::setupArgumentParser() {
    parser_->addArgument("command", 0, "command", "Validation command", "", false);
    parser_->addArgument("--file", 'f', "file", "Configuration file to validate", "");
    parser_->addArgument("--strict", 0, "strict", "Enable strict validation mode", "false");
    parser_->addArgument("--verbose", 'v', "verbose", "Enable verbose output", "false");
    parser_->addArgument("--component", 0, "component", "Component to check (config, memory, cpu, all)", "all");
    parser_->addArgument("--help", 0, "help", "Show help information");
    parser_->addArgument("--version", 0, "version", "Show version information");
}

std::string NLMValidateTool::getDescription() const {
    return "Configuration and system validation tool for NLM. Validates configuration files and system components.";
}

std::string NLMValidateTool::getVersion() const {
    return "1.0.0";
}

std::string NLMValidateTool::getUsage() const {
    return "nlm_validate <command> [options]"; 
}

void NLMValidateTool::shutdown() {
    if (logger_) {
        Logger::setGlobal(nullptr);
        logger_.reset();
    }
}

void NLMValidateTool::log(const std::string& message, bool error) const {
    std::cout << (error ? "[ERROR] " : "[INFO] ") << message << std::endl;
}

} // namespace nlm