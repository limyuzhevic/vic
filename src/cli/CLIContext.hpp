#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include <iostream>
#include <iomanip>

#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"

namespace nlm {

struct CLIContext {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    CLIContext() = default;
    ~CLIContext() = default;
    
    void printUsage(const std::string& programName) {
        std::cout << "NLM CLI Tool - Version 0.1.0" << std::endl;
        std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  config list                    - List all configuration keys" << std::endl;
        std::cout << "  config show <key>             - Show value of specific key" << std::endl;
        std::cout << "  config set <key> <value>      - Set configuration value" << std::endl;
        std::cout << "  config create <filename>       - Create new config file" << std::endl;
        std::cout << "  config validate <filename>     - Validate config file format" << std::endl;
        std::cout << "  brain status                   - Show current brain statistics" << std::endl;
        std::cout << "  brain neurons --type <type> --count - List neurons by type" << std::endl;
        std::cout << "  brain synapses --min-weight <weight> - List synapses above weight threshold" << std::endl;
        std::cout << "  brain save <filename>          - Save brain state" << std::endl;
        std::cout << "  brain load <filename>          - Load brain state" << std::endl;
        std::cout << "  simulate --config <file> --steps <count> --headless - Run simulation" << std::endl;
        std::cout << "  version                        - Show version info" << std::endl;
        std::cout << "  help                           - Show help" << std::endl;
    }
    
    bool parseArgs(int argc, char** argv, std::string& command, 
                   std::map<std::string, std::string>& options,
                   std::vector<std::string>& args);
    
    void printConfigSummary() const;
    void printBrainStats() const;
    void printNeuronsByType(const std::string& typeFilter, size_t count) const;
    void printSynapsesAboveWeight(float minWeight) const;
};

} // namespace nlm
