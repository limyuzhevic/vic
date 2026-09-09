// Simple implementation of Args functionality for testing
#include "Args.hpp"
#include <iostream>

namespace nlm {
    std::string executionModeToString(ExecutionMode mode) {
        switch (mode) {
            case ExecutionMode::Standard: return "standard";
            case ExecutionMode::Visualization: return "visualization";
            case ExecutionMode::Experiment: return "experiment";
            case ExecutionMode::Benchmark: return "benchmark";
            case ExecutionMode::Test: return "test";
            default: return "unknown";
        }
    }

    ExecutionMode stringToExecutionMode(const std::string& mode) {
        std::string lower = mode;
        std::transform(lower.begin(), lower.end(), lower.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
        
        if (lower == "standard") return ExecutionMode::Standard;
        if (lower == "visualization") return ExecutionMode::Visualization;
        if (lower == "experiment") return ExecutionMode::Experiment;
        if (lower == "benchmark") return ExecutionMode::Benchmark;
        if (lower == "test") return ExecutionMode::Test;
        
        throw std::invalid_argument("Invalid execution mode: " + mode + ". "
                                    "Valid modes are: standard, visualization, experiment, benchmark, test");
    }

    // Simple implementation for testing
    std::string ArgumentParser::generateHelp() const {
        return "Usage: nlm [OPTIONS] [MODE]";
    }

    std::string ArgumentParser::generateVersion() {
        return "0.1.0 (Phase 2)";
    }

    std::string ArgumentParser::generateContactInfo() {
        return "NLM Development Team\nhttps://github.com/nml-team/nlm";
    }

    std::map<std::string, std::variant<std::string, bool, ExecutionMode>> ArgumentParser::getAllArgs() const {
        return {};
    }

    std::string ArgumentParser::normalize(const std::string& name) {
        std::string result = name;
        std::transform(result.begin(), result.end(), result.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    bool ArgumentParser::hasOption(const std::string& name) const {
        return false;
    }

    // Simplified constructor
    ArgumentParser::ArgumentParser() {
        // Simple initialization
        helpRequested_ = false;
        versionRequested_ = false;
    }

    bool ArgumentParser::parse(int argc, char** argv) {
        // Simple parsing for testing
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                helpRequested_ = true;
            } else if (arg == "--version" || arg == "-v") {
                versionRequested_ = true;
            } else if (arg.substr(0, 2) == "--mode") {
                // Simple mode parsing
                size_t eq = arg.find('=');
                if (eq != std::string::npos) {
                    std::string modeStr = arg.substr(eq + 1);
                    // Simple validation
                    if (modeStr == "standard" || modeStr == "visualization" || 
                        modeStr == "experiment" || modeStr == "benchmark" || 
                        modeStr == "test") {
                        // Could parse here
                    }
                }
            }
        }
        return true;
    }

    bool ArgumentParser::wasHelpRequested() const { return helpRequested_; }
    bool ArgumentParser::wasVersionRequested() const { return versionRequested_; }
    
    ExecutionMode ArgumentParser::getMode() const { 
        return ExecutionMode::Standard; 
    }
    
    bool ArgumentParser::getFlag(const std::string& name) const { 
        return false; 
    }
    
    std::string ArgumentParser::getValue(const std::string& name) const { 
        return ""; 
    }
    
    std::string ArgumentParser::getString(const std::string& name) const { 
        return getValue(name); 
    }
    
    std::optional<std::string> ArgumentParser::getOptionalString(const std::string& name) const { 
        return std::nullopt; 
    }

} // namespace nlm