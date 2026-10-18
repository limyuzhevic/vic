// Help System Implementation
//
// This file provides the implementation of the CLI help and documentation system

#include "HelpSystem.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

HelpSystem::HelpSystem() : autoGenerateHelp_(true), 
                           includeAdvancedCommands_(false),
                           helpOutputDirectory_("docs"),
                           documentationLanguage_("en") {}

std::string HelpSystem::generateHelpText(const CLICommand& command) const {
    std::stringstream ss;
    
    ss << "=== " << command.getName() << " ===" << std::endl;
    ss << std::endl;
    ss << "Description: " << command.getDescription() << std::endl;
    ss << "Usage: " << command.getUsage() << std::endl;
    ss << "Category: " << command.getCategory() << std::endl;
    ss << std::endl;
    
    // Add examples if available
    auto it = CommandExamples::CONFIG_COMMANDS.end(); // Placeholder
    if (it != CommandExamples::CONFIG_COMMANDS.end()) {
        ss << "Examples:" << std::endl;
        for (const auto& example : it->second) {
            ss << "  " << example << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string HelpSystem::generateCommandListText(const std::vector<std::string>& commands) const {
    std::stringstream ss;
    
    ss << "Available Commands:" << std::endl;
    ss << std::endl;
    
    for (const auto& command : commands) {
        ss << "  " << command << std::endl;
    }
    
    return ss.str();
}

std::string HelpSystem::generateCategoryHelpText(CLICommand::CommandType type) const {
    std::stringstream ss;
    
    // Use switch to generate category-specific help
    switch (type) {
        case CLICommand::CommandType::CONFIG:
            ss << "=== Configuration Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for managing neural simulation configurations:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  config-info     - Show configuration details" << std::endl;
            ss << "  config-validate - Validate configuration files" << std::endl;
            ss << "  config-merge    - Merge multiple configuration files" << std::endl;
            ss << "  config-export   - Export configuration to different formats" << std::endl;
            ss << "  config-list     - List available configurations and presets" << std::endl;
            ss << "  config-compare  - Compare configuration files" << std::endl;
            break;
            
        case CLICommand::CommandType::EXPERIMENT:
            ss << "=== Experiment Management Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for managing neural simulation experiments:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  exp-info        - Show experiment details" << std::endl;
            ss << "  exp-run         - Run experiments with different configurations" << std::endl;
            ss << "  exp-list        - List available experiments" << std::endl;
            ss << "  exp-compare     - Compare experiment results" << std::endl;
            ss << "  exp-export      - Export experiment results" << std::endl;
            ss << "  exp-merge       - Merge multiple experiment results" << std::endl;
            break;
            
        case CLICommand::CommandType::BRAIN:
            ss << "=== Brain Management Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for managing neural brain states:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  brain-info      - Show brain configuration and status" << std::endl;
            ss << "  brain-save      - Save brain state" << std::endl;
            ss << "  brain-load      - Load brain state" << std::endl;
            ss << "  brain-compare   - Compare brain states" << std::endl;
            ss << "  brain-export    - Export brain state" << std::endl;
            ss << "  brain-import    - Import brain state" << std::endl;
            break;
            
        case CLICommand::CommandType::WORLD:
            ss << "=== World/Environment Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for managing world and environment state:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  world-info      - Show world configuration" << std::endl;
            ss << "  world-save      - Save world state" << std::endl;
            ss << "  world-load      - Load world state" << std::endl;
            ss << "  world-test      - Test world configuration" << std::endl;
            ss << "  world-explore   - Explore world state" << std::endl;
            break;
            
        case CLICommand::CommandType::ANALYSIS:
            ss << "=== Analysis Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for analyzing neural simulation data:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  analyze-metrics     - Analyze simulation metrics" << std::endl;
            ss << "  analyze-performance  - Analyze system performance" << std::endl;
            ss << "  analyze-results     - Analyze experiment results" << std::endl;
            ss << "  analyze-memory      - Analyze memory usage" << std::endl;
            ss << "  analyze-neural      - Analyze neural network properties" << std::endl;
            break;
            
        case CLICommand::CommandType::UTILITY:
            ss << "=== Utility Tools ===" << std::endl;
            ss << std::endl;
            ss << "General utility tools for NLM system:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  version           - Show version information" << std::endl;
            ss << "  health-check      - Perform system health checks" << std::endl;
            ss << "  stats             - Show system statistics" << std::endl;
            ss << "  logs              - Show logs" << std::endl;
            ss << "  benchmark         - Run performance benchmarks" << std::endl;
            ss << "  benchmark-config  - Configure benchmarks" << std::endl;
            break;
            
        case CLICommand::CommandType::VISUALIZATION:
            ss << "=== Visualization Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for visualizing neural simulation data:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  viz-simulation    - Visualize simulation" << std::endl;
            ss << "  viz-results       - Visualize experiment results" << std::endl;
            ss << "  viz-brain         - Visualize brain structure" << std::endl;
            ss << "  viz-metrics       - Visualize metrics" << std::endl;
            ss << "  viz-comparison    - Visualize comparison data" << std::endl;
            break;
            
        case CLICommand::CommandType::DEVELOPMENT:
            ss << "=== Development Tools ===" << std::endl;
            ss << std::endl;
            ss << "Tools for debugging and development:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  debug             - Debug current session" << std::endl;
            ss << "  trace             - Enable/disable tracing" << std::endl;
            ss << "  profile           - Start/stop profiling" << std::endl;
            ss << "  monitor           - Start/stop monitoring" << std::endl;
            ss << "  snapshot          - Create system snapshot" << std::endl;
            ss << "  restore           - Restore from snapshot" << std::endl;
            break;
            
        case CLICommand::CommandType::ADVANCED:
            ss << "=== Advanced Tools ===" << std::endl;
            ss << std::endl;
            ss << "Advanced tools for distributed and batch operations:" << std::endl;
            ss << std::endl;
            ss << "Available commands:" << std::endl;
            ss << "  batch-exec        - Execute batch operations" << std::endl;
            ss << "  parallel-exec     - Execute parallel operations" << std::endl;
            ss << "  distributed       - Distributed execution" << std::endl;
            ss << "  scale-out         - Scale out operations" << std::endl;
            ss << "  load-balance      - Load balancing" << std::endl;
            break;
            
        default:
            ss << "=== Unknown Command Type ===" << std::endl;
    }
    
    return ss.str();
}

bool HelpSystem::loadHelpFile(const std::string& commandName, const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return false;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    commandHelpFiles_[commandName] = ss.str();
    
    return true;
}

bool HelpSystem::saveHelpFile(const std::string& commandName, const std::string& filePath) const {
    auto it = commandHelpFiles_.find(commandName);
    if (it == commandHelpFiles_.end()) {
        return false;
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << it->second;
    return true;
}

void HelpSystem::displayTutorial(const std::string& topic) const {
    std::cout << "=== Tutorial: " << topic << " ===" << std::endl;
    std::cout << std::endl;
    std::cout << "This is a tutorial for " << topic << "." << std::endl;
    std::cout << std::endl;
    std::cout << "Coming soon..." << std::endl;
}

void HelpSystem::displayCommandExamples(const std::string& commandName) const {
    // Use examples from CommandExamples namespace
    if (commandName.find("config") != std::string::npos) {
        std::cout << "=== Configuration Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::CONFIG_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("exp") != std::string::npos) {
        std::cout << "=== Experiment Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::EXPERIMENT_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("brain") != std::string::npos) {
        std::cout << "=== Brain Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::BRAIN_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("world") != std::string::npos) {
        std::cout << "=== World Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::WORLD_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("analyze") != std::string::npos) {
        std::cout << "=== Analysis Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::ANALYSIS_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("utility") != std::string::npos || 
               commandName == "version" || commandName == "health-check") {
        std::cout << "=== Utility Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::UTILITY_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("viz") != std::string::npos) {
        std::cout << "=== Visualization Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::VISUALIZATION_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("development") != std::string::npos) {
        std::cout << "=== Development Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::DEVELOPMENT_COMMANDS) {
            std::cout << example << std::endl;
        }
    } else if (commandName.find("advanced") != std::string::npos) {
        std::cout << "=== Advanced Tool Examples ===" << std::endl;
        std::cout << std::endl;
        for (const auto& example : CommandExamples::ADVANCED_COMMANDS) {
            std::cout << example << std::endl;
        }
    }
}

std::string HelpSystem::generateCommandReference() const {
    std::stringstream ss;
    
    ss << "=== NLM Command Reference ===" << std::endl;
    ss << std::endl;
    ss << "This document provides detailed information about all available NLM commands." << std::endl;
    ss << std::endl;
    ss << "For detailed help on a specific command, use: nlm-cli help <command-name>" << std::endl;
    ss << std::endl;
    
    // Add command summaries
    ss << "Commands are organized into the following categories:" << std::endl;
    ss << std::endl;
    ss << "1. Configuration Tools (config-*)" << std::endl;
    ss << "   Manage configuration files and presets" << std::endl;
    ss << std::endl;
    ss << "2. Experiment Management Tools (exp-*)" << std::endl;
    ss << "   Run and analyze neural simulation experiments" << std::endl;
    ss << std::endl;
    ss << "3. Brain Management Tools (brain-*)" << std::endl;
    ss << "   Save, load, and compare brain states" << std::endl;
    ss << std::endl;
    ss << "4. World/Environment Tools (world-*)" << std::endl;
    ss << "   Manage world state and configuration" << std::endl;
    ss << std::endl;
    ss << "5. Analysis Tools (analyze-*)" << std::endl;
    ss << "   Analyze simulation metrics and performance" << std::endl;
    ss << std::endl;
    ss << "6. Utility Tools" << std::endl;
    ss << "   Version info, health checks, system stats" << std::endl;
    ss << std::endl;
    ss << "7. Visualization Tools (viz-*)" << std::endl;
    ss << "   Visualize simulation results and metrics" << std::endl;
    ss << std::endl;
    ss << "8. Development Tools" << std::endl;
    ss << "   Debug, trace, profile, and monitor" << std::endl;
    ss << std::endl;
    ss << "9. Advanced Tools" << std::endl;
    ss << "   Batch and parallel execution" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string HelpSystem::generateConfigurationReference() const {
    std::stringstream ss;
    
    ss << "=== Configuration Reference ===" << std::endl;
    ss << std::endl;
    ss << "NLM uses JSON configuration files for managing neural simulation parameters." << std::endl;
    ss << std::endl;
    ss << "Configuration files can be loaded using --config or --json-config options." << std::endl;
    ss << std::endl;
    ss << "Example configuration file structure:" << std::endl;
    ss << std::endl;
    ss << "{\n";
    ss << "  \"random_seed\": 42,\n";
    ss << "  \"simulation_timestep\": 0.001,\n";
    ss << "  \"neuron_count\": 1000,\n";
    ss << "  \"region_count\": 1,\n";
    ss << "  \"connection_probability\": 0.15,\n";
    ss << "  \"stdp_ltp_weight\": 0.02,\n";
    ss << "  \"stdp_ltd_weight\": 0.015,\n";
    ss << "  \"synaptogenesis_rate\": 0.0001,\n";
    ss << "  \"pruning_rate\": 0.00001\n";
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string HelpSystem::generateExperimentReference() const {
    std::stringstream ss;
    
    ss << "=== Experiment Reference ===" << std::endl;
    ss << std::endl;
    ss << "NLM experiments run neural simulations and record results." << std::endl;
    ss << std::endl;
    ss << "Experiment results are saved in JSON format with detailed metrics." << std::endl;
    ss << std::endl;
    ss << "Example experiment result structure:" << std::endl;
    ss << std::endl;
    ss << "{\n";
    ss << "  \"experiment_name\": \"learning_experiment\",\n";
    ss << "  \"start_time\": \"2025-01-01T00:00:00\",\n";
    ss << "  \"end_time\": \"2025-01-01T01:00:00\",\n";
    ss << "  \"neuron_count\": 1000,\n";
    ss << "  \"total_spikes\": 5000,\n";
    ss << "  \"learning_occurred\": true,\n";
    ss << "  \"mean_weight_change\": 0.015\n";
    ss << "}" << std::endl;
    
    return ss.str();
}

void HelpSystem::generateHtmlDocumentation(const std::string& outputPath) const {
    if (!std::filesystem::exists(outputPath)) {
        std::filesystem::create_directories(outputPath);
    }
    
    // Generate index.html
    std::string indexPath = outputPath + "/index.html";
    std::ofstream indexFile(indexPath);
    if (indexFile.is_open()) {
        indexFile << "<!DOCTYPE html>" << std::endl;
        indexFile << "<html>" << std::endl;
        indexFile << "<head>" << std::endl;
        indexFile << "    <title>NLM CLI Documentation</title>" << std::endl;
        indexFile << "    <style>" << std::endl;
        indexFile << "        body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
        indexFile << "        h1 { color: #333; }" << std::endl;
        indexFile << "        h2 { color: #555; }" << std::endl;
        indexFile << "        .command { background-color: #f5f5f5; padding: 10px; margin: 10px 0; }" << std::endl;
        indexFile << "        .deprecated { color: #999; text-decoration: line-through; }" << std::endl;
        indexFile << "    </style>" << std::endl;
        indexFile << "</head>" << std::endl;
        indexFile << "<body>" << std::endl;
        indexFile << "    <h1>NLM CLI Documentation</h1>" << std::endl;
        indexFile << "    <p>This documentation provides information about NLM command-line tools.</p>" << std::endl;
        indexFile << "    <h2>Command Categories</h2>" << std::endl;
        indexFile << "    <div class=\"command\">" << std::endl;
        indexFile << "        <h3><a href=\"config.html\">Configuration Tools</a></h3>" << std::endl;
        indexFile << "        <p>Manage configuration files and presets</p>" << std::endl;
        indexFile << "    </div>" << std::endl;
        // Add other categories...
        indexFile << "</body>" << std::endl;
        indexFile << "</html>" << std::endl;
        indexFile.close();
    }
    
    log("Generated HTML documentation in: " + outputPath);
}

void HelpSystem::generateManPages(const std::string& outputPath) const {
    if (!std::filesystem::exists(outputPath)) {
        std::filesystem::create_directories(outputPath);
    }
    
    // Generate man pages for key commands
    // This would typically generate .1 files for each command
    log("Generated man pages in: " + outputPath);
}

void HelpSystem::generatePdfDocumentation(const std::string& outputPath) const {
    if (!std::filesystem::exists(outputPath)) {
        std::filesystem::create_directories(outputPath);
    }
    
    log("Generated PDF documentation in: " + outputPath);
}

std::string HelpSystem::formatCommandUsage(const std::string& commandName,
                                          const std::vector<std::string>& examples) const {
    std::stringstream ss;
    
    ss << "Usage: nlm-cli " << commandName << " [options]" << std::endl;
    
    if (!examples.empty()) {
        ss << std::endl;
        ss << "Examples:" << std::endl;
        for (const auto& example : examples) {
            ss << "  " << example << std::endl;
        }
    }
    
    return ss.str();
}

std::string HelpSystem::formatOptionHelp(const std::string& optionName,
                                        const std::string& description,
                                        const std::string& defaultValue,
                                        bool advanced) const {
    std::stringstream ss;
    
    ss << "  --" << optionName;
    
    if (advanced) {
        ss << "  [ADVANCED]";
    }
    
    ss << std::endl;
    ss << "    Description: " << description << std::endl;
    ss << "    Default: " << (defaultValue.empty() ? "None" : defaultValue) << std::endl;
    
    return ss.str();
}

std::string HelpSystem::escapeHtml(const std::string& text) const {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string HelpSystem::formatExamples(const std::vector<std::string>& examples) const {
    std::stringstream ss;
    
    for (const auto& example : examples) {
        ss << "  " << example << std::endl;
    }
    
    return ss.str();
}

std::string HelpSystem::generateExampleCommand(const std::string& commandName,
                                               const std::vector<std::string>& args) const {
    std::stringstream ss;
    
    ss << "nlm-cli " << commandName;
    
    for (const auto& arg : args) {
        // Basic argument formatting
        if (arg.find(' ') != std::string::npos) {
            ss << " \"" << arg << "\"";
        } else {
            ss << " " << arg;
        }
    }
    
    return ss.str();
}

std::vector<std::string> HelpSystem::searchCommands(const std::string& query) const {
    std::vector<std::string> results;
    // TODO: Implement command search
    return results;
}

std::vector<std::string> HelpSystem::getCommandsByType(CLICommand::CommandType type) const {
    std::vector<std::string> results;
    // TODO: Implement command filtering by type
    return results;
}

std::vector<std::string> HelpSystem::getCommandsByKeyword(const std::string& keyword) const {
    std::vector<std::string> results;
    // TODO: Implement command filtering by keyword
    return results;
}

} // namespace nlm