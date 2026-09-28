#include "CLIManager.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace nlm {

CLIManager::CLIManager()
    : outputFormat_("table")
    , logLevel_("info")
    , verbose_(false) {
}

int CLIManager::run(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }
    
    std::string command;
    std::vector<std::string> args;
    
    parseArguments(argc, argv, command, args);
    
    // Find and execute subcommand
    for (const auto& subcommand : subcommands_) {
        if (subcommand.name == command) {
            return subcommand.handler(args);
        }
    }
    
    std::cerr << "Error: Unknown command: " << command << std::endl;
    std::cerr << "Use 'nlm --help' to see available commands." << std::endl;
    return 1;
}

void CLIManager::registerSubcommand(const std::string& name,
                                   const std::string& description,
                                   std::function<int(const std::vector<std::string>&)> handler) {
    subcommands_.push_back({name, description, handler});
}

void CLIManager::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Advanced Neural Simulation Framework" << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    
    for (const auto& subcommand : subcommands_) {
        std::cout << "  " << subcommand.name << " - " << subcommand.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Global options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --output-format FORMAT    Output format (json, yaml, table, verbose)" << std::endl;
    std::cout << "  --log-level LEVEL         Log level (debug, info, warn, error)" << std::endl;
    std::cout << std::endl;
}

void CLIManager::printSubcommandHelp(const std::string& name) const {
    for (const auto& subcommand : subcommands_) {
        if (subcommand.name == name) {
            std::cout << subcommand.description << std::endl;
            std::cout << std::endl;
            std::cout << "Usage: nlm " << name << " [options]" << std::endl;
            break;
        }
    }
}

int CLIManager::handleRun(const std::vector<std::string>& args) {
    std::cout << "=== Running NLM Simulation ===" << std::endl;
    
    Phase6Config config = parsePhase6Config(args);
    
    Phase6IntegratedExperiment experiment;
    
    // Show progress if verbose
    if (verbose_) {
        std::cout << "Starting integration verification..." << std::endl;
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "Error: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl;
    
    // Run the experiment
    std::cout << "Running integration experiment..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    printExperimentResult(result);
    
    if (verbose_) {
        std::cout << "Experiment completed in " << duration.count() << " seconds" << std::endl;
    }
    
    return 0;
}

int CLIManager::handleConfig(const std::vector<std::string>& args) {
    std::cout << "=== NLM Configuration Management ===" << std::endl;
    
    Phase6Config config = parsePhase6Config(args);
    
    printConfig(config);
    
    // Export configuration if requested
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--export" && i + 1 < args.size()) {
            std::string format = "json";
            std::string filename = args[i + 1];
            
            if (i + 2 < args.size() && args[i + 2].find("--format=") == 0) {
                format = args[i + 2].substr(9);
            }
            
            // Export configuration (simplified implementation)
            std::cout << "Exporting configuration to " << filename << " (format: " << format << ")" << std::endl;
            
            // In a full implementation, this would serialize the config
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "# NLM Configuration Export" << std::endl;
                file << "# Neuron Count: " << config.neuronCount << std::endl;
                file << "# Max Steps: " << config.maxSteps << std::endl;
                file << "# Enable Checkpointing: " << (config.enableCheckpointing ? "true" : "false") << std::endl;
                file << "# Enable Replay: " << (config.enableReplay ? "true" : "false") << std::endl;
                file << "# Enable Development: " << (config.enableDevelopment ? "true" : "false") << std::endl;
                file << "# Connection Probability: " << config.connectionProbability << std::endl;
                file << "# Region Count: " << config.regionCount << std::endl;
                file.close();
                std::cout << "Configuration exported successfully!" << std::endl;
            } else {
                std::cerr << "Error: Could not write to file " << filename << std::endl;
                return 1;
            }
        }
    }
    
    return 0;
}

int CLIManager::handleExperiment(const std::vector<std::string>& args) {
    std::cout << "=== NLM Experiment Management ===" << std::endl;
    std::cout << "Experiment management commands:" << std::endl;
    std::cout << "  --list                  List available experiments" << std::endl;
    std::cout << "  --run NAME              Run specific experiment" << std::endl;
    std::cout << "  --create NAME           Create new experiment" << std::endl;
    
    // Simplified implementation
    if (verbose_) {
        std::cout << "Available experiments: phase6_integration, phase4_demo, phase3_experiment" << std::endl;
    }
    
    return 0;
}

int CLIManager::handleBenchmark(const std::vector<std::string>& args) {
    std::cout << "=== NLM Benchmark ===" << std::endl;
    std::cout << "Running benchmarks..." << std::endl;
    
    // Simplified benchmark implementation
    std::cout << "Benchmarks not yet implemented in this version." << std::endl;
    std::cout << "For detailed benchmarking, use the benchmark tools included with the source." << std::endl;
    
    return 0;
}

int CLIManager::handleDebug(const std::vector<std::string>& args) {
    std::cout << "=== NLM Debug Tools ===" << std::endl;
    
    for (const auto& arg : args) {
        if (arg == "--brain-state") {
            std::cout << "Brain state debugging not yet implemented." << std::endl;
        } else if (arg == "--memory-stats") {
            std::cout << "Memory statistics not yet implemented." << std::endl;
        } else if (arg == "--performance") {
            std::cout << "Performance profiling not yet implemented." << std::endl;
        } else {
            std::cout << "Debug option: " << arg << std::endl;
        }
    }
    
    return 0;
}

int CLIManager::handleHelp(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
    } else {
        printSubcommandHelp(args[0]);
    }
    return 0;
}

void CLIManager::parseArguments(int argc, char* argv[],
                               std::string& command,
                               std::vector<std::string>& args) const {
    std::vector<std::string> allArgs;
    for (int i = 1; i < argc; ++i) {
        allArgs.push_back(argv[i]);
    }
    
    // Check for global options
    for (size_t i = 0; i < allArgs.size(); ++i) {
        if (allArgs[i] == "--help" || allArgs[i] == "-h") {
            printHelp();
            std::exit(0);
        } else if (allArgs[i] == "--version") {
            std::cout << "NLM Version 0.1.0 - Phase 6 Final Integration" << std::endl;
            std::exit(0);
        } else if (allArgs[i] == "--verbose") {
            verbose_ = true;
        } else if (allArgs[i].find("--output-format=") == 0) {
            outputFormat_ = allArgs[i].substr(17);
        } else if (allArgs[i].find("--log-level=") == 0) {
            logLevel_ = allArgs[i].substr(12);
        }
    }
    
    // Find command
    if (allArgs.empty()) {
        command = "help";
    } else {
        command = allArgs[0];
        
        // Remove global options from args
        for (const auto& arg : allArgs) {
            if (arg == "--help" || arg == "-h" || arg == "--version" ||
                arg == "--verbose" || arg.find("--output-format=") == 0 ||
                arg.find("--log-level=") == 0) {
                continue;
            }
            if (arg != command) {
                args.push_back(arg);
            }
        }
    }
}

Phase6Config CLIManager::parsePhase6Config(const std::vector<std::string>& args) const {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "/tmp/nlm_checkpoint.bin";
    config.connectionProbability = 0.15f;
    config.regionCount = 1;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg.find("--neuron-count=") == 0) {
            try {
                config.neuronCount = std::stoi(arg.substr(16));
            } catch (...) {
                std::cerr << "Warning: Invalid neuron count: " << arg.substr(16) << std::endl;
            }
        } else if (arg.find("--max-steps=") == 0) {
            try {
                config.maxSteps = std::stoi(arg.substr(11));
            } catch (...) {
                std::cerr << "Warning: Invalid max steps: " << arg.substr(11) << std::endl;
            }
        } else if (arg.find("--checkpoint-path=") == 0) {
            config.checkpointPath = arg.substr(19);
        } else if (arg.find("--connection-prob=") == 0) {
            try {
                config.connectionProbability = std::stof(arg.substr(20));
            } catch (...) {
                std::cerr << "Warning: Invalid connection probability: " << arg.substr(20) << std::endl;
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                config.regionCount = std::stoi(arg.substr(10));
            } catch (...) {
                std::cerr << "Warning: Invalid regions: " << arg.substr(10) << std::endl;
            }
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
    }
    
    return config;
}

std::shared_ptr<Config> CLIManager::parseConfigFile(const std::string& filepath) const {
    auto config = std::make_shared<Config>();
    
    // Simplified config file parsing
    // In a full implementation, this would use proper JSON/YAML parsing
    std::ifstream file(filepath);
    if (file.is_open()) {
        // Parse basic key=value pairs
        std::string line;
        while (std::getline(file, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config->set(key, value, ConfigSource::File);
            }
        }
        file.close();
    }
    
    return config;
}

void CLIManager::printConfig(const Phase6Config& config) const {
    std::cout << "NLM Configuration:" << std::endl;
    std::cout << "  Neuron Count: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Enable Checkpointing: " << (config.enableCheckpointing ? "YES" : "NO") << std::endl;
    std::cout << "  Enable Replay: " << (config.enableReplay ? "YES" : "NO") << std::endl;
    std::cout << "  Enable Development: " << (config.enableDevelopment ? "YES" : "NO") << std::endl;
    std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
    std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "  Region Count: " << config.regionCount << std::endl;
}

void CLIManager::printExperimentResult(const Phase6IntegrationResult& result) const {
    std::cout << "=== Experiment Results ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

void CLIManager::exportResult(const Phase6IntegrationResult& result,
                             const std::string& format,
                             const std::string& filename) const {
    // Simplified export implementation
    std::cout << "Exporting results to " << filename << " (format: " << format << ")" << std::endl;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        return;
    }
    
    if (format == "json") {
        file << "{\n";
        file << "  \"totalReward\": " << result.totalReward << ",\n";
        file << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
        file << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
        file << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
        file << "  \"noveltyLevel\": " << result.noveltyLevel << ",\n";
        file << "  \"curiosityLevel\": " << result.curiosityLevel << ",\n";
        file << "  \"memoryWorkingMemoryIntegrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        file << "  \"memoryEpisodicMemoryIntegrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        file << "  \"neuromodulationIntegrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
        file << "  \"predictionIntegrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
        file << "  \"developmentIntegrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
        file << "  \"checkpointingWorks\": " << (result.checkpointingWorks ? "true" : "false") << "\n";
        file << "}\n";
    } else {
        // Text format
        printExperimentResult(result);
    }
    
    file.close();
    std::cout << "Results exported successfully!" << std::endl;
}

std::string CLIManager::formatOutput(const std::string& format,
                                    const std::map<std::string, std::string>& data) const {
    if (format == "json") {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) ss << ",\n";
            ss << "  \"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        ss << "\n}\n";
        return ss.str();
    } else if (format == "yaml") {
        std::stringstream ss;
        for (const auto& pair : data) {
            ss << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    } else {
        // Table format
        std::stringstream ss;
        for (const auto& pair : data) {
            ss << pair.first << " = " << pair.second << "\n";
        }
        return ss.str();
    }
}

} // namespace nlm
