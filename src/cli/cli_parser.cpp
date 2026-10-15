// Command-line interface (CLI) for NLM (Neural Learning Machine)
// Implementation of CLI parser

#include "cli_parser.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace nlm;

// Initialize static members
const std::string CLIParser::version_ = "0.1.0";
const std::string CLIParser::buildInfo_ = "Built with C++20, optimized for NLM performance";

CLIParser::CLIParser() : verboseMode_(false), configFile_("") {
    // Add available commands
    commands_.push_back({"help", "Show help information", {}, {"--verbose"}, "nlm --help", nullptr});
    commands_.push_back({"version", "Display version information", {}, {}, "nlm version", nullptr});
    commands_.push_back({"--verbose", "Enable verbose logging", {}, {}, "nlm --verbose", nullptr});
    commands_.push_back({"--config", "Load configuration from file", {"file"}, {},
                        "nlm --config <file>", nullptr});
    commands_.push_back({"--set", "Set configuration parameter", {"key", "value"}, {},
                        "nlm --set <key> <value>", nullptr});
    commands_.push_back({"--show-config", "Display current configuration", {}, {},
                        "nlm --show-config", nullptr});
    commands_.push_back({"run", "Run simulation for specified steps", {"steps"}, {"--with-agent", "--brain"},
                        "nlm run <steps>", nullptr});
    commands_.push_back({"step", "Execute specified number of steps", {"n"}, {}, "nlm step <n>", nullptr});
    commands_.push_back({"save", "Save brain state to file", {"file"}, {}, "nlm save <file>", nullptr});
    commands_.push_back({"load", "Load brain state from file", {"file"}, {}, "nlm load <file>", nullptr});
    commands_.push_back({"export-stats", "Export simulation statistics", {"file"}, {}, "nlm export-stats <file>", nullptr});
    commands_.push_back({"print-status", "Display current brain status", {}, {}, "nlm print-status", nullptr});
    commands_.push_back({"experiment", "Run predefined experiment", {"name", "steps"}, {},
                        "nlm experiment <name> <steps>", nullptr});
    commands_.push_back({"benchmark", "Run performance benchmarks", {}, {"--name", "--steps"}, "nlm benchmark", nullptr});
    commands_.push_back({"validate", "Validate configuration and system integrity", {}, {}, "nlm validate", nullptr});
    commands_.push_back({"simulate", "Create and simulate world", {}, {"--world-size", "--vision"}, "nlm simulate --world-size <w> <h> --vision <w> <h>", nullptr});
    commands_.push_back({"episode", "Run a complete agent episode", {"steps"}, {}, "nlm episode <steps>", nullptr});
    commands_.push_back({"train", "Train agent for specified episodes", {"episodes"}, {}, "nlm train <episodes>", nullptr});
}

CLIParser::~CLIParser() {
}

bool CLIParser::parse(int argc, char** argv) {
    if (argc < 2) {
        return false; // No command provided
    }
    
    currentCommand_ = argv[1];
    
    // Parse flags and arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for flag with value (--key value)
        if (arg.substr(0, 2) == "--") {
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                flagValues_[key] = value;
            } else {
                flagValues_[arg] = "true"; // Boolean flag
            }
        } else {
            args_.push_back(arg);
        }
    }
    
    // Check for special flags
    if (hasFlag("--verbose")) {
        verboseMode_ = true;
    }
    
    if (hasFlag("--config")) {
        configFile_ = getFlagValue("--config");
    }
    
    return true;
}

const std::string& CLIParser::getArg(size_t index) const {
    if (index < args_.size()) {
        return args_[index];
    }
    static const std::string empty = "";
    return empty;
}

std::string CLIParser::getFlagValue(const std::string& flag) const {
    auto it = flagValues_.find(flag);
    if (it != flagValues_.end()) {
        return it->second;
    }
    return "";
}

bool CLIParser::hasFlag(const std::string& flag) const {
    return flagValues_.find(flag) != flagValues_.end();
}

void CLIParser::showHelp() const {
    std::cout << "NLM (Neural Learning Machine) CLI - Version " << version_ << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Commands:" << std::endl;
    std::cout << "  nlm --help                Show this help information" << std::endl;
    std::cout << "  nlm version               Display version information" << std::endl;
    std::cout << "  nlm --verbose             Enable verbose logging" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    
    for (const auto& cmd : commands_) {
        std::cout << "  " << cmd.usage << std::endl;
        std::cout << "    " << cmd.description << std::endl;
    }
}

void CLIParser::showVersion() const {
    std::cout << "NLM CLI Version " << version_ << std::endl;
    std::cout << "Build: " << buildInfo_ << std::endl;
}