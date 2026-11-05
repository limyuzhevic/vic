// UnifiedCLI.cpp - Implementation of the unified CLI framework

#include "core/CLI/UnifiedCLI.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Implementation of CLI class methods

void CLI::showHelp() const {
    std::cout << "Usage: " << programName_ << " [OPTIONS]" << std::endl << std::endl;
    std::cout << description_ << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& option : options_) {
        std::cout << "  ";
        if (option.shortName != '\0') {
            std::cout << "-" << option.shortName << ", ";
        }
        std::cout << "--" << option.longName;
        
        if (option.hasArgument) {
            std::cout << "=" << option.type;
        }
        
        std::cout << std::endl;
        std::cout << "    " << option.description << std::endl;
        if (!option.defaultValue.empty()) {
            std::cout << "    Default: " << option.defaultValue << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "Commonly used options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  --config=FILE                 Load configuration from file" << std::endl;
    std::cout << "  --help-all                    Show all available options" << std::endl;
    std::cout << std::endl;
    
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void CLI::showVersion() const {
    std::cout << programName_ << " version " << version_ << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
}

// Implementation of DemoApp class methods

void DemoApp::configureFromConfig() {
    // Apply common configuration settings
    try {
        // Set random seed
        if (config_->hasKey("random_seed")) {
            config_->set("random_seed", config_->get<int64_t>("random_seed"), "demo_config");
        }
        
        // Set simulation timestep
        if (config_->hasKey("simulation_timestep")) {
            config_->set("simulation_timestep", config_->get<double>("simulation_timestep"), "demo_config");
        }
        
        // Set log level
        if (config_->hasKey("log_level")) {
            std::string logLevel = config_->get<std::string>("log_level");
            printInfo("Log level set to: " + logLevel);
        }
        
    } catch (const std::exception& e) {
        printWarning("Configuration error: " + std::string(e.what()));
    }
}

void DemoApp::print(const std::string& message) {
    if (quiet_) return;
    std::cout << message << std::endl;
}

void DemoApp::printVerbose(const std::string& message) {
    if (verbose_) {
        std::cout << "[VERBOSE] " << message << std::endl;
    }
}

void DemoApp::printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void DemoApp::printWarning(const std::string& message) {
    std::cerr << "[WARNING] " << message << std::endl;
}

void DemoApp::printInfo(const std::string& message) {
    if (!quiet_) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

std::string DemoApp::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm