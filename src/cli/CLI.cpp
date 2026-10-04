#include "CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unistd.h>
#include <getopt.h>

namespace nlm {

// CLI Implementation

bool CLI::parseArgs(int argc, char** argv) {
    try {
        // Initialize commands
        initializeCommands();
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle global options
            if (arg == "--help" || arg == "-h") {
                help_ = true;
            } else if (arg == "--version" || arg == "-v") {
                version_ = true;
            } else if (arg == "--verbose" || arg == "-V") {
                verbose_ = true;
            } else if (arg == "--quiet" || arg == "-q") {
                quiet_ = true;
            } else if (arg == "--log-file" && i + 1 < argc) {
                logFile_ = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                // Load configuration from file
                if (!config_->loadFromFile(arg)) {
                    std::cerr << "ERROR: Failed to load configuration from: " << arg << std::endl;
                }
            } else if (arg.find("--") == 0) {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    config_->set(key, value, ConfigSource::CommandLine);
                }
            } else {
                // It's a command
                args_.push_back(arg);
            }
        }
        
        // Setup logging if needed
        if (!logFile_.empty()) {
            setupLogging();
        }
        
        return true;
        
    } catch (const Error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected error: " << e.what() << std::endl;
        return false;
    }
}

int CLI::run() {
    try {
        // Handle global options
        if (help_) {
            showHelp();
            return 0;
        }
        
        if (version_) {
            showVersion();
            return 0;
        }
        
        // If no command provided, show help
        if (args_.empty()) {
            showHelp();
            return 1;
        }
        
        // Parse command and arguments
        std::string command = args_[0];
        std::vector<std::string> cmdArgs(args_.begin() + 1, args_.end());
        
        // Validate command
        if (!validateCommand(command)) {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
            showHelp();
            return 1;
        }
        
        // Execute command
        return executeCommand(command, cmdArgs);
        
    } catch (const Error& e) {
        handleError(e);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}

std::string CLI::getHelp() const {
    std::ostringstream oss;
    oss << "NLM - Neural Learning Machine CLI" << std::endl;
    oss << "Version: " << getVersion() << std::endl;
    oss << "\nUSAGE:" << std::endl;
    oss << "  nlm [OPTIONS] COMMAND [ARGS]..." << std::endl;
    oss << "\nGLOBAL OPTIONS:" << std::endl;
    oss << "  -h, --help          Show this help message" << std::endl;
    oss << "  -v, --version       Show version information" << std::endl;
    oss << "  -V, --verbose       Enable verbose output" << std::endl;
    oss << "  -q, --quiet         Suppress non-error output" << std::endl;
    oss << "  --log-file FILE     Log output to file" << std::endl;
    oss << "  --config FILE       Load configuration from file" << std::endl;
    oss << "  --key=value         Set configuration key" << std::endl;
    oss << "\nAVAILABLE COMMANDS:" << std::endl;
    
    for (const auto& cmd : commands_) {
        oss << "  " << cmd.name << " - " << cmd.description << std::endl;
    }
    
    oss << "\nFor more information on a specific command, use:" << std::endl;
    oss << "  nlm COMMAND --help" << std::endl;
    
    return oss.str();
}

std::string CLI::getVersion() const {
    return "NLM v0.1.0 - Neural Learning Machine";
}

int CLI::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    // Find command
    auto cmdIt = std::find_if(commands_.begin(), commands_.end(),
                             [&command](const Command& cmd) { return cmd.name == command; });
    
    if (cmdIt == commands_.end()) {
        std::cerr << "ERROR: Unknown command: " << command << std::endl;
        return 1;
    }
    
    try {
        return cmdIt->callback(args);
    } catch (const Error& e) {
        handleError(e);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Command execution failed: " << e.what() << std::endl;
        return 1;
    }
}

void CLI::initializeCommands() {
    // Core commands
    commands_.push_back({
        "run",
        "Run a simulation or experiment",
        "nlm run [OPTIONS]",
        std::bind(&CLI::cmdRun, this, std::placeholders::_1),
        {"nlm run --experiment phase6", "nlm run --benchmark"}
    });
    
    commands_.push_back({
        "config",
        "Manage configuration",
        "nlm config [COMMAND] [OPTIONS]",
        std::bind(&CLI::cmdConfig, this, std::placeholders::_1),
        {"nlm config list", "nlm config set key value"}
    });
    
    commands_.push_back({
        "experiment",
        "Manage experiments",
        "nlm experiment [COMMAND] [OPTIONS]",
        std::bind(&CLI::cmdExperiment, this, std::placeholders::_1),
        {"nlm experiment run", "nlm experiment list"}
    });
    
    commands_.push_back({
        "benchmark",
        "Run performance benchmarks",
        "nlm benchmark [OPTIONS]",
        std::bind(&CLI::cmdBenchmark, this, std::placeholders::_1),
        {"nlm benchmark --neurons 1000", "nlm benchmark --duration 60"}
    });
    
    commands_.push_back({
        "info",
        "Show system and configuration information",
        "nlm info [OPTIONS]",
        std::bind(&CLI::cmdInfo, this, std::placeholders::_1),
        {"nlm info system", "nlm info config"}
    });
    
    commands_.push_back({
        "test",
        "Run unit tests",
        "nlm test [OPTIONS]",
        std::bind(&CLI::cmdTest, this, std::placeholders::_1),
        {"nlm test all", "nlm test --neuron"}
    });
    
    commands_.push_back({
        "profile",
        "Run performance profiling",
        "nlm profile [OPTIONS]",
        std::bind(&CLI::cmdProfile, this, std::placeholders::_1),
        {"nlm profile --duration 30", "nlm profile --neurons 500"}
    });
    
    commands_.push_back({
        "replay",
        "Replay saved experiment results",
        "nlm replay [OPTIONS]",
        std::bind(&CLI::cmdReplay, this, std::placeholders::_1),
        {"nlm replay experiment.bench", "nlm replay --last"}
    });
    
    commands_.push_back({
        "save",
        "Save current state",
        "nlm save [OPTIONS]",
        std::bind(&CLI::cmdSave, this, std::placeholders::_1),
        {"nlm save brain_state", "nlm save --checkpoint"}
    });
    
    commands_.push_back({
        "load",
        "Load saved state",
        "nlm load [OPTIONS]",
        std::bind(&CLI::cmdLoad, this, std::placeholders::_1),
        {"nlm load brain_state", "nlm load --checkpoint"}
    });
}

int CLI::cmdRun(const std::vector<std::string>& args) {
    std::string experimentType = "phase6";
    std::string outputFile = "experiment_result.bench";
    std::chrono::seconds duration = std::chrono::seconds(60);
    size_t neuronCount = 500;
    
    // Parse command line arguments
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--experiment" && i + 1 < args.size()) {
            experimentType = args[++i];
        } else if (arg == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--duration" && i + 1 < args.size()) {
            try {
                duration = std::chrono::seconds(std::stoi(args[++i]));
            } catch (...) {
                std::cerr << "ERROR: Invalid duration value" << std::endl;
                return 1;
            }
        } else if (arg == "--neurons" && i + 1 < args.size()) {
            try {
                neuronCount = std::stoi(args[++i]);
            } catch (...) {
                std::cerr << "ERROR: Invalid neuron count value" << std::endl;
                return 1;
            }
        } else if (arg == "--help") {
            std::cout << "Usage: nlm run [OPTIONS]" << std::endl;
            std::cout << "  --experiment TYPE    Experiment type (default: phase6)" << std::endl;
            std::cout << "  --output FILE        Output file for results" << std::endl;
            std::cout << "  --duration SECONDS   Run duration in seconds" << std::endl;
            std::cout << "  --neurons N          Number of neurons" << std::endl;
            return 0;
        }
    }
    
    // Create experiment configuration
    std::map<std::string, std::string> experimentConfig;
    experimentConfig["experiment_type"] = experimentType;
    experimentConfig["neuron_count"] = std::to_string(neuronCount);
    experimentConfig["duration"] = std::to_string(duration.count());
    experimentConfig["output_file"] = outputFile;
    
    // Run experiment
    std::cout << "Running experiment: " << experimentType << std::endl;
    std::cout << "Neurons: " << neuronCount << ", Duration: " << duration.count() << " seconds" << std::endl;
    
    // TODO: Implement actual experiment running
    // For now, just create a placeholder output
    std::ofstream outfile(outputFile, std::ios::binary);
    if (!outfile) {
        throw Error("Failed to create output file: " + outputFile, ErrorCode::ConfigurationSaveFailed);
    }
    
    // Write simple binary output
    uint32_t placeholder = 0xDEADBEEF;
    outfile.write(reinterpret_cast<const char*>(&placeholder), sizeof(placeholder));
    outfile.close();
    
    std::cout << "Experiment completed. Results saved to: " << outputFile << std::endl;
    return 0;
}

int CLI::cmdConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: nlm config [COMMAND] [OPTIONS]" << std::endl;
        std::cout << "Available commands: list, get, set, reset, edit" << std::endl;
        return 0;
    }
    
    std::string command = args[0];
    std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
    
    if (command == "list") {
        return configList(cmdArgs);
    } else if (command == "get") {
        return configGet(cmdArgs);
    } else if (command == "set") {
        return configSet(cmdArgs);
    } else if (command == "reset") {
        return configReset(cmdArgs);
    } else {
        std::cerr << "ERROR: Unknown config command: " << command << std::endl;
        return 1;
    }
}

int CLI::cmdExperiment(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: nlm experiment [COMMAND] [OPTIONS]" << std::endl;
        std::cout << "Available commands: run, list, status, info" << std::endl;
        return 0;
    }
    
    std::string command = args[0];
    std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
    
    if (command == "run") {
        return experimentRun(cmdArgs);
    } else if (command == "list") {
        return experimentList(cmdArgs);
    } else if (command == "status") {
        return experimentStatus(cmdArgs);
    } else {
        std::cerr << "ERROR: Unknown experiment command: " << command << std::endl;
        return 1;
    }
}

int CLI::cmdBenchmark(const std::vector<std::string>& args) {
    // Parse benchmark options
    size_t neuronCount = 500;
    std::chrono::seconds duration = std::chrono::seconds(30);
    std::string outputFile = "benchmark_results.txt";
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--neurons" && i + 1 < args.size()) {
            try {
                neuronCount = std::stoi(args[++i]);
            } catch (...) {
                std::cerr << "ERROR: Invalid neuron count value" << std::endl;
                return 1;
            }
        } else if (arg == "--duration" && i + 1 < args.size()) {
            try {
                duration = std::chrono::seconds(std::stoi(args[++i]));
            } catch (...) {
                std::cerr << "ERROR: Invalid duration value" << std::endl;
                return 1;
            }
        } else if (arg == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: nlm benchmark [OPTIONS]" << std::endl;
            std::cout << "  --neurons N           Number of neurons (default: 500)" << std::endl;
            std::cout << "  --duration SECONDS    Benchmark duration in seconds" << std::endl;
            std::cout << "  --output FILE         Output file for results" << std::endl;
            return 0;
        }
    }
    
    std::cout << "Running benchmark with " << neuronCount << " neurons for " << duration.count() 
              << " seconds..." << std::endl;
    
    // TODO: Implement actual benchmarking
    // For now, just create a placeholder output
    std::ofstream outfile(outputFile);
    if (!outfile) {
        throw Error("Failed to create benchmark output file: " + outputFile, ErrorCode::ConfigurationSaveFailed);
    }
    
    outfile << "Benchmark Results" << std::endl;
    outfile << "=================" << std::endl;
    outfile << "Neurons: " << neuronCount << std::endl;
    outfile << "Duration: " << duration.count() << " seconds" << std::endl;
    outfile << "Placeholder benchmark completed successfully" << std::endl;
    outfile.close();
    
    std::cout << "Benchmark completed. Results saved to: " << outputFile << std::endl;
    return 0;
}

int CLI::cmdInfo(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: nlm info [OPTIONS]" << std::endl;
        std::cout << "  system              Show system information" << std::endl;
        std::cout << "  config              Show configuration" << std::endl;
        std::cout << "  version             Show version information" << std::endl;
        std::cout << "  --help              Show this help" << std::endl;
        return 0;
    }
    
    std::string infoType = args[0];
    
    if (infoType == "system") {
        auto systemInfo = CLIUtils::SystemUtils::getSystemInfo();
        CLIUtils::TableFormatter table(2, {"Property", "Value"});
        for (const auto& pair : systemInfo) {
            table.addRow({pair.first, pair.second});
        }
        std::cout << table.format() << std::endl;
    } else if (infoType == "config") {
        std::cout << config_->summary() << std::endl;
    } else if (infoType == "version") {
        std::cout << getVersion() << std::endl;
    } else if (infoType == "--help") {
        std::cout << "Usage: nlm info [OPTIONS]" << std::endl;
        std::cout << "  system              Show system information" << std::endl;
        std::cout << "  config              Show configuration" << std::endl;
        std::cout << "  version             Show version information" << std::endl;
    } else {
        std::cerr << "ERROR: Unknown info type: " << infoType << std::endl;
        return 1;
    }
    
    return 0;
}

int CLI::cmdTest(const std::vector<std::string>& args) {
    std::string testType = "all";
    
    if (!args.empty()) {
        testType = args[0];
    }
    
    if (args.size() > 0 && args[0] == "--help") {
        std::cout << "Usage: nlm test [OPTIONS]" << std::endl;
        std::cout << "  all                  Run all tests" << std::endl;
        std::cout << "  --neuron             Run neuron tests" << std::endl;
        std::cout << "  --synapse            Run synapse tests" << std::endl;
        std::cout << "  --brain              Run brain tests" << std::endl;
        std::cout << "  --config             Run configuration tests" << std::endl;
        return 0;
    }
    
    // TODO: Implement actual testing
    std::cout << "Running tests for: " << testType << std::endl;
    std::cout << "Note: Actual test implementation pending" << std::endl;
    
    return 0;
}

int CLI::cmdProfile(const std::vector<std::string>& args) {
    std::chrono::seconds duration = std::chrono::seconds(30);
    size_t neuronCount = 500;
    std::string outputFile = "profile_results.json";
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--duration" && i + 1 < args.size()) {
            try {
                duration = std::chrono::seconds(std::stoi(args[++i]));
            } catch (...) {
                std::cerr << "ERROR: Invalid duration value" << std::endl;
                return 1;
            }
        } else if (arg == "--neurons" && i + 1 < args.size()) {
            try {
                neuronCount = std::stoi(args[++i]);
            } catch (...) {
                std::cerr << "ERROR: Invalid neuron count value" << std::endl;
                return 1;
            }
        } else if (arg == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: nlm profile [OPTIONS]" << std::endl;
            std::cout << "  --duration SECONDS    Profile duration in seconds" << std::endl;
            std::cout << "  --neurons N           Number of neurons" << std::endl;
            std::cout << "  --output FILE         Output file for profiling results" << std::endl;
            return 0;
        }
    }
    
    std::cout << "Running performance profiling for " << duration.count() << " seconds..." << std::endl;
    
    // TODO: Implement actual profiling
    std::ofstream outfile(outputFile);
    if (!outfile) {
        throw Error("Failed to create profiling output file: " + outputFile, ErrorCode::ConfigurationSaveFailed);
    }
    
    outfile << "{\"profile\": \"placeholder\", \"duration\": " << duration.count() 
            << ", \"neurons\": " << neuronCount << "}" << std::endl;
    outfile.close();
    
    std::cout << "Profiling completed. Results saved to: " << outputFile << std::endl;
    return 0;
}

int CLI::cmdReplay(const std::vector<std::string>& args) {
    std::string inputFile = "experiment_result.bench";
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--file" && i + 1 < args.size()) {
            inputFile = args[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: nlm replay [OPTIONS]" << std::endl;
            std::cout << "  --file FILE           Input file to replay" << std::endl;
            return 0;
        }
    }
    
    std::cout << "Replaying experiment from: " << inputFile << std::endl;
    
    // TODO: Implement actual replay
    std::ifstream infile(inputFile, std::ios::binary);
    if (!infile) {
        std::cerr << "ERROR: Failed to open input file: " << inputFile << std::endl;
        return 1;
    }
    
    uint32_t placeholder;
    infile.read(reinterpret_cast<char*>(&placeholder), sizeof(placeholder));
    infile.close();
    
    std::cout << "Replay completed successfully" << std::endl;
    return 0;
}

int CLI::cmdSave(const std::vector<std::string>& args) {
    std::string outputFile = "brain_state.bin";
    bool checkpoint = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--file" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (arg == "--checkpoint") {
            checkpoint = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm save [OPTIONS]" << std::endl;
            std::cout << "  --file FILE           Output file for brain state" << std::endl;
            std::cout << "  --checkpoint          Save checkpoint (includes memory and development state)" << std::endl;
            return 0;
        }
    }
    
    std::cout << "Saving brain state to: " << outputFile << std::endl;
    if (checkpoint) {
        std::cout << "Saving checkpoint (includes memory and development state)" << std::endl;
    }
    
    // TODO: Implement actual saving
    std::ofstream outfile(outputFile, std::ios::binary);
    if (!outfile) {
        throw Error("Failed to create output file: " + outputFile, ErrorCode::ConfigurationSaveFailed);
    }
    
    // Write simple binary output
    uint32_t placeholder = 0xDEADBEEF;
    outfile.write(reinterpret_cast<const char*>(&placeholder), sizeof(placeholder));
    outfile.close();
    
    std::cout << "Brain state saved successfully" << std::endl;
    return 0;
}

int CLI::cmdLoad(const std::vector<std::string>& args) {
    std::string inputFile = "brain_state.bin";
    bool checkpoint = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--file" && i + 1 < args.size()) {
            inputFile = args[++i];
        } else if (arg == "--checkpoint") {
            checkpoint = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm load [OPTIONS]" << std::endl;
            std::cout << "  --file FILE           Input file for brain state" << std::endl;
            std::cout << "  --checkpoint          Load checkpoint (includes memory and development state)" << std::endl;
            return 0;
        }
    }
    
    std::cout << "Loading brain state from: " << inputFile << std::endl;
    if (checkpoint) {
        std::cout << "Loading checkpoint (includes memory and development state)" << std::endl;
    }
    
    // TODO: Implement actual loading
    std::ifstream infile(inputFile, std::ios::binary);
    if (!infile) {
        std::cerr << "ERROR: Failed to open input file: " << inputFile << std::endl;
        return 1;
    }
    
    uint32_t placeholder;
    infile.read(reinterpret_cast<char*>(&placeholder), sizeof(placeholder));
    infile.close();
    
    std::cout << "Brain state loaded successfully" << std::endl;
    return 0;
}

int CLI::configList(const std::vector<std::string>& args) {
    std::cout << "Configuration keys:" << std::endl;
    for (const auto& key : config_->getKeys()) {
        std::cout << "  " << key << std::endl;
    }
    return 0;
}

int CLI::configGet(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cerr << "ERROR: Need exactly one key argument" << std::endl;
        return 1;
    }
    
    std::string key = args[0];
    if (!config_->has(key)) {
        std::cerr << "ERROR: Key not found: " << key << std::endl;
        return 1;
    }
    
    // Try to get as string first, then as other types
    std::optional<std::string> strVal = config_->get<std::string>(key);
    if (strVal.has_value()) {
        std::cout << strVal.value() << std::endl;
        return 0;
    }
    
    std::optional<int> intVal = config_->get<int>(key);
    if (intVal.has_value()) {
        std::cout << intVal.value() << std::endl;
        return 0;
    }
    
    std::optional<double> doubleVal = config_->get<double>(key);
    if (doubleVal.has_value()) {
        std::cout << doubleVal.value() << std::endl;
        return 0;
    }
    
    std::optional<bool> boolVal = config_->get<bool>(key);
    if (boolVal.has_value()) {
        std::cout << (boolVal.value() ? "true" : "false") << std::endl;
        return 0;
    }
    
    std::cerr << "ERROR: Unable to determine type of value for key: " << key << std::endl;
    return 1;
}

int CLI::configSet(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "ERROR: Need key and value arguments" << std::endl;
        return 1;
    }
    
    std::string key = args[0];
    std::string value = args[1];
    
    // Try to parse as boolean
    if (value == "true" || value == "false") {
        bool boolValue = (value == "true");
        config_->set(key, boolValue);
        std::cout << "Set " << key << " = " << (boolValue ? "true" : "false") << std::endl;
        return 0;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(value, &pos);
        if (pos == value.size()) {
            config_->set(key, intValue);
            std::cout << "Set " << key << " = " << intValue << std::endl;
            return 0;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            config_->set(key, doubleValue);
            std::cout << "Set " << key << " = " << doubleValue << std::endl;
            return 0;
        }
    } catch (...) {}
    
    // Set as string
    config_->set(key, value);
    std::cout << "Set " << key << " = \"" << value << "\"" << std::endl;
    return 0;
}

int CLI::configReset(const std::vector<std::string>& args) {
    config_->clear();
    std::cout << "Configuration reset" << std::endl;
    return 0;
}

int CLI::experimentRun(const std::vector<std::string>& args) {
    // Delegate to run command
    return cmdRun(args);
}

int CLI::experimentList(const std::vector<std::string>& args) {
    std::cout << "Available experiments:" << std::endl;
    std::cout << "  phase6      - Phase 6 integration test" << std::endl;
    std::cout << "  phase4      - Phase 4 cognitive integration" << std::endl;
    std::cout << "  phase3      - Phase 3 neural dynamics" << std::endl;
    return 0;
}

int CLI::experimentStatus(const std::vector<std::string>& args) {
    std::cout << "Experiment status check (placeholder)" << std::endl;
    return 0;
}

void CLI::showHelp(const std::string& command) const {
    if (command.empty()) {
        std::cout << getHelp() << std::endl;
    } else {
        // Find command
        auto cmdIt = std::find_if(commands_.begin(), commands_.end(),
                                 [&command](const Command& cmd) { return cmd.name == command; });
        
        if (cmdIt == commands_.end()) {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
            return;
        }
        
        std::cout << "Usage: nlm " << cmdIt->usage << std::endl;
        std::cout << cmdIt->description << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        for (const auto& example : cmdIt->examples) {
            std::cout << "  " << example << std::endl;
        }
    }
}

void CLI::showVersion() const {
    std::cout << getVersion() << std::endl;
}

bool CLI::validateCommand(const std::string& command) const {
    return std::any_of(commands_.begin(), commands_.end(),
                      [&command](const Command& cmd) { return cmd.name == command; });
}

void CLI::handleError(const Error& error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
    if (verbose_) {
        std::cerr << "Details: " << error.getDetails() << std::endl;
        std::cerr << "Category: " << static_cast<int>(error.getCategory()) << std::endl;
        std::cerr << "Code: " << static_cast<int>(error.getCode()) << std::endl;
    }
}

void CLI::setupLogging() {
    if (!logFile_.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(logFile_);
        Logger::getGlobal().addLogger(fileLogger);
    }
}

void CLI::setupConfig() {
    // Set up default configuration
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
}

std::string CLI::formatTime(std::chrono::system_clock::time_point time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string CLI::formatBytes(size_t bytes) const {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
}

std::string CLI::formatDuration(std::chrono::milliseconds duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    
    std::stringstream ss;
    if (minutes.count() > 0) {
        ss << minutes.count() << "m ";
    }
    ss << (seconds.count() % 60) << "s";
    return ss.str();
}

bool CLI::validateArgs(const std::vector<std::string>& args) const {
    return !args.empty();
}

// CLIUtils Implementation

CLIUtils::ProgressBar::ProgressBar(size_t total, const std::string& label) 
    : total_(total), current_(0), label_(label), finished_(false) {
    showPercentage_ = true;
}

CLIUtils::ProgressBar::~ProgressBar() {
    if (!finished_) {
        std::cout << std::endl;
    }
}

void CLIUtils::ProgressBar::update(size_t current, const std::string& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    current_ = current;
    status_ = status;
    
    if (showPercentage_ && total_ > 0) {
        std::cout << "\r" << label_ << " " << current_ << "/" << total_ 
                  << " (" << (100 * current_ / total_) << "%)";
    } else {
        std::cout << "\r" << label_ << " " << current_ << "/" << total_;
    }
    
    if (status_.empty()) {
        std::cout << " - " << status_;
    }
    std::cout << std::flush;
}

void CLIUtils::ProgressBar::increment(const std::string& status) {
    update(current_ + 1, status);
}

void CLIUtils::ProgressBar::setStatus(const std::string& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = status;
}

void CLIUtils::ProgressBar::finish() {
    std::lock_guard<std::mutex> lock(mutex_);
    current_ = total_;
    finished_ = true;
    
    if (showPercentage_) {
        std::cout << "\r" << label_ << " " << total_ << "/" << total_ 
                  << " (100%)";
    } else {
        std::cout << "\r" << label_ << " " << total_ << "/" << total_;
    }
    
    if (!status_.empty()) {
        std::cout << " - " << status_;
    }
    std::cout << std::endl;
}

CLIUtils::TableFormatter::TableFormatter(size_t columns, const std::vector<std::string>& headers)
    : columns_(columns), headers_(headers) {
    calculateColumnWidths();
}

void CLIUtils::TableFormatter::addRow(const std::vector<std::string>& row) {
    if (row.size() == columns_) {
        rows_.push_back(row);
        calculateColumnWidths();
    }
}

void CLIUtils::TableFormatter::addRow(const std::map<std::string, std::string>& row) {
    std::vector<std::string> rowVec;
    for (const auto& pair : row) {
        rowVec.push_back(pair.second);
    }
    addRow(rowVec);
}

std::string CLIUtils::TableFormatter::format() const {
    std::ostringstream oss;
    
    // Header
    for (size_t i = 0; i < columns_; ++i) {
        oss << padString(headers_[i], columnWidths_[i]);
        if (i < columns_ - 1) {
            oss << "  ";
        }
    }
    oss << std::endl;
    
    // Separator
    for (size_t i = 0; i < columns_; ++i) {
        oss << std::string(columnWidths_[i], '-');
        if (i < columns_ - 1) {
            oss << "  ";
        }
    }
    oss << std::endl;
    
    // Rows
    for (const auto& row : rows_) {
        for (size_t i = 0; i < columns_; ++i) {
            if (i < row.size()) {
                oss << padString(row[i], columnWidths_[i]);
            } else {
                oss << padString("", columnWidths_[i]);
            }
            if (i < columns_ - 1) {
                oss << "  ";
            }
        }
        oss << std::endl;
    }
    
    return oss.str();
}

void CLIUtils::TableFormatter::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << format();
    }
}

void CLIUtils::TableFormatter::calculateColumnWidths() {
    columnWidths_.clear();
    for (size_t i = 0; i < columns_; ++i) {
        size_t maxWidth = headers_[i].size();
        for (const auto& row : rows_) {
            if (i < row.size()) {
                maxWidth = std::max(maxWidth, row[i].size());
            }
        }
        columnWidths_.push_back(maxWidth);
    }
}

std::string CLIUtils::TableFormatter::padString(const std::string& str, size_t width, bool rightAlign) const {
    if (str.size() >= width) {
        return str.substr(0, width);
    }
    
    if (rightAlign) {
        return std::string(width - str.size(), ' ') + str;
    } else {
        return str + std::string(width - str.size(), ' ');
    }
}

bool CLIUtils::ConfigFileUtils::generateSampleConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Sample Configuration File" << std::endl;
    file << "# Generated by NLM CLI" << std::endl;
    file << std::endl;
    file << "# Basic simulation settings" << std::endl;
    file << "random_seed = 42" << std::endl;
    file << "simulation_timestep = 0.001" << std::endl;
    file << "neuron_count = 500" << std::endl;
    file << "region_count = 1" << std::endl;
    file << "connection_probability = 0.15" << std::endl;
    file << std::endl;
    file << "# Plasticity settings" << std::endl;
    file << "stdp_ltp_weight = 0.02" << std::endl;
    file << "stdp_ltd_weight = 0.015" << std::endl;
    file << "stdp_tau = 20.0" << std::endl;
    file << "hebbian_learning_rate = 0.01" << std::endl;
    file << std::endl;
    file << "# Developmental settings" << std::endl;
    file << "synaptogenesis_rate = 0.0001" << std::endl;
    file << "pruning_rate = 0.00001" << std::endl;
    file << std::endl;
    file << "# Neuromodulation settings" << std::endl;
    file << "dopamine_influence = 0.5" << std::endl;
    file << "novelty_threshold = 0.1" << std::endl;
    file << "curiosity_weight = 0.3" << std::endl;
    
    file.close();
    return true;
}

bool CLIUtils::ConfigFileUtils::validateConfig(const std::string& filepath, std::string& errorMessage) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        errorMessage = "Cannot open configuration file: " + filepath;
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = CLIUtils::trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            errorMessage = "Invalid configuration line " + std::to_string(lineNumber) + ": " + line;
            return false;
        }
        
        std::string key = CLIUtils::trim(line.substr(0, pos));
        std::string value = CLIUtils::trim(line.substr(pos + 1));
        
        // Validate key (should not contain special characters except underscore)
        if (!std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '_' || c == '-';
        })) {
            errorMessage = "Invalid configuration key at line " + std::to_string(lineNumber) + ": " + key;
            return false;
        }
    }
    
    file.close();
    return true;
}

bool CLIUtils::ConfigFileUtils::mergeConfigs(const std::string& baseFile, const std::string& overrideFile,
                                           const std::string& outputFile) {
    // TODO: Implement configuration file merging
    // This would read both files, combine them with overrideFile taking precedence
    return false;
}

bool CLIUtils::ConfigFileUtils::exportConfig(const std::string& inputFile, const std::string& outputFile,
                                            const std::string& format) {
    // TODO: Implement configuration export
    return false;
}

bool CLIUtils::ExperimentUtils::validateExperimentParams(const std::map<std::string, std::string>& params,
                                                        std::string& errorMessage) {
    // Validate required parameters
    std::vector<std::string> requiredParams = {"experiment_type", "neuron_count", "duration"};
    for (const auto& param : requiredParams) {
        if (params.find(param) == params.end()) {
            errorMessage = "Missing required parameter: " + param;
            return false;
        }
    }
    
    // Validate parameter values
    try {
        size_t neuronCount = std::stoul(params.at("neuron_count"));
        if (neuronCount < 10 || neuronCount > 100000) {
            errorMessage = "Invalid neuron count: must be between 10 and 100000";
            return false;
        }
    } catch (...) {
        errorMessage = "Invalid neuron count value";
        return false;
    }
    
    try {
        size_t duration = std::stoul(params.at("duration"));
        if (duration < 1 || duration > 86400) { // Up to 24 hours
            errorMessage = "Invalid duration: must be between 1 and 86400 seconds";
            return false;
        }
    } catch (...) {
        errorMessage = "Invalid duration value";
        return false;
    }
    
    return true;
}

std::map<std::string, std::string> CLIUtils::ExperimentUtils::generateExperimentConfig(
    const std::string& experimentType, const std::string& name) {
    std::map<std::string, std::string> config;
    config["experiment_type"] = experimentType;
    config["name"] = name;
    config["neuron_count"] = "500";
    config["duration"] = "60";
    config["output_file"] = name + "_result.bench";
    return config;
}

std::vector<std::string> CLIUtils::ExperimentUtils::listExperiments() {
    return {"phase6", "phase4", "phase3"};
}

std::map<std::string, std::string> CLIUtils::ExperimentUtils::getExperimentMetadata(
    const std::string& experimentType) {
    std::map<std::string, std::string> metadata;
    
    if (experimentType == "phase6") {
        metadata["description"] = "Phase 6 integration test of all brain systems";
        metadata["neurons"] = "500-1000";
        metadata["duration"] = "60-180 seconds";
    } else if (experimentType == "phase4") {
        metadata["description"] = "Phase 4 cognitive integration test";
        metadata["neurons"] = "200-500";
        metadata["duration"] = "30-120 seconds";
    } else if (experimentType == "phase3") {
        metadata["description"] = "Phase 3 neural dynamics and plasticity";
        metadata["neurons"] = "100-300";
        metadata["duration"] = "15-60 seconds";
    }
    
    return metadata;
}

void CLIUtils::PerformanceUtils::startProfiling(const std::string& sessionName) {
    // TODO: Implement profiling
}

std::map<std::string, double> CLIUtils::PerformanceUtils::stopProfiling() {
    // TODO: Implement profiling results
    return {};
}

std::string CLIUtils::PerformanceUtils::generatePerformanceReport(const std::map<std::string, double>& metrics) {
    // TODO: Implement performance report generation
    return "Performance report placeholder";
}

std::map<std::string, std::string> CLIUtils::SystemUtils::getSystemInfo() {
    std::map<std::string, std::string> info;
    
    info["platform"] = "Linux";
    info["architecture"] = "x86_64";
    info["cpu_cores"] = std::to_string(getCPUCoreCount());
    info["memory_gb"] = std::to_string(getAvailableMemory() / (1024 * 1024 * 1024));
    info["compiler"] = "C++20";
    
    return info;
}

std::map<std::string, double> CLIUtils::SystemUtils::getSystemResources() {
    std::map<std::string, double> resources;
    
    resources["cpu_usage"] = 0.0;
    resources["memory_usage_gb"] = 0.0;
    resources["available_memory_gb"] = static_cast<double>(getAvailableMemory()) / (1024 * 1024 * 1024);
    
    return resources;
}

bool CLIUtils::SystemUtils::validateSystemRequirements(std::string& errorMessage) {
    if (getAvailableMemory() < 512 * 1024 * 1024) { // 512 MB minimum
        errorMessage = "Insufficient memory: minimum 512 MB required";
        return false;
    }
    
    if (getCPUCoreCount() < 1) {
        errorMessage = "Insufficient CPU cores: minimum 1 core required";
        return false;
    }
    
    return true;
}

size_t CLIUtils::SystemUtils::getAvailableMemory() {
    // TODO: Implement actual memory detection
    return 2 * 1024 * 1024 * 1024; // Return 2 GB for now
}

size_t CLIUtils::SystemUtils::getCPUCoreCount() {
    // TODO: Implement actual CPU core detection
    return std::thread::hardware_concurrency();
}

// Helper functions
std::string CLIUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// InteractiveCLI Implementation

InteractiveCLI::InteractiveCLI(std::shared_ptr<Config> config) : config_(config), historyIndex_(0), running_(true) {}

InteractiveCLI::~InteractiveCLI() = default;

void InteractiveCLI::run() {
    showWelcome();
    
    while (running_) {
        showPrompt();
        
        std::string command;
        std::getline(std::cin, command);
        
        if (!command.empty()) {
            addToHistory(command);
            historyIndex_ = history_.size();
            processCommand(command);
        }
    }
}

void InteractiveCLI::showWelcome() const {
    std::cout << "=== NLM Interactive CLI ===" << std::endl;
    std::cout << "Welcome to the Neural Learning Machine interactive environment." << std::endl;
    std::cout << "Type 'help' for a list of commands or 'quit' to exit." << std::endl;
    std::cout << std::endl;
}

void InteractiveCLI::showPrompt() const {
    std::cout << "nlm> ";
    std::cout.flush();
}

void InteractiveCLI::processCommand(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "help") {
        listCommands();
    } else if (cmd == "quit" || cmd == "exit") {
        running_ = false;
        std::cout << "Goodbye!" << std::endl;
    } else if (cmd == "history") {
        for (size_t i = 0; i < history_.size(); ++i) {
            std::cout << i + 1 << ": " << history_[i] << std::endl;
        }
    } else {
        // Execute as CLI command
        std::istringstream iss(command);
        std::string cmdName;
        std::vector<std::string> cmdArgs;
        
        iss >> cmdName;
        std::string arg;
        while (iss >> arg) {
            cmdArgs.push_back(arg);
        }
        
        // TODO: Execute command using CLI
        std::cout << "Command executed: " << command << std::endl;
    }
}

void InteractiveCLI::listCommands() const {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help           - Show this help" << std::endl;
    std::cout << "  quit/exit      - Exit the interactive session" << std::endl;
    std::cout << "  history        - Show command history" << std::endl;
    std::cout << std::endl;
    std::cout << "Additionally, you can use the standard NLM CLI commands:" << std::endl;
    std::cout << "  run [options]  - Run a simulation" << std::endl;
    std::cout << "  config [cmd]   - Manage configuration" << std::endl;
    std::cout << "  experiment [cmd] - Manage experiments" << std::endl;
    std::cout << "  benchmark [options] - Run benchmarks" << std::endl;
    std::cout << "  info [type]     - Show information" << std::endl;
    std::cout << "  test [type]     - Run tests" << std::endl;
}

void InteractiveCLI::addToHistory(const std::string& command) {
    history_.push_back(command);
}

// BatchCLI Implementation

BatchCLI::BatchCLI(std::shared_ptr<Config> config) : config_(config), verbose_(false) {}

BatchCLI::~BatchCLI() = default;

int BatchCLI::executeBatchFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw Error("Cannot open batch file: " + filepath, ErrorCode::ConfigurationLoadFailed);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return executeBatchString(content);
}

int BatchCLI::executeBatchString(const std::string& batchCommands) {
    // Parse batch commands
    std::vector<std::pair<std::string, std::vector<std::string>>> commands = 
        parseBatchCommands(batchCommands);
    
    int result = 0;
    for (const auto& command : commands) {
        results_.push_back("Executing: " + command.first);
        
        int cmdResult = executeBatchCommand(command.first, command.second);
        if (cmdResult != 0) {
            result = cmdResult;
            if (!verbose_) {
                results_.push_back("Command failed: " + command.first);
            }
        } else {
            results_.push_back("Command completed: " + command.first);
        }
    }
    
    return result;
}

std::vector<std::pair<std::string, std::vector<std::string>>> BatchCLI::parseBatchCommands(
    const std::string& batchCommands) {
    std::vector<std::pair<std::string, std::vector<std::string>>> commands;
    
    std::istringstream iss(batchCommands);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = CLIUtils::trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Split into command and arguments
        std::istringstream lineStream(line);
        std::string cmd;
        std::vector<std::string> args;
        
        lineStream >> cmd;
        std::string arg;
        while (lineStream >> arg) {
            args.push_back(arg);
        }
        
        commands.push_back({cmd, args});
    }
    
    return commands;
}

int BatchCLI::executeBatchCommand(const std::string& command, const std::vector<std::string>& args) {
    // Delegate to CLI for command execution
    // TODO: Implement actual command execution
    return 0;
}

} // namespace nlm
