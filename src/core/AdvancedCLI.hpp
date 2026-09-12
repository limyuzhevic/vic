#include "Core/AdvancedCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <map>
#include <vector>
#include <string>

namespace nlm {

// CLI result structure
struct CLIResult {
    bool success;
    std::string message;
    std::map<std::string, std::string> parameters;
    std::string configFile;
    std::string outputDir;
    bool saveCheckpoint;
    bool verbose;
    std::string mode;
    int seed;
    float timestep;
    int neurons;
    int regions;
    std::string plasticityMode;
    std::string rewardType;
    std::vector<std::string> neuromodulators;
    float developmentStage;
    std::string attentionMode;
    std::string metricsFormat;
    bool cpuProfiling;
    bool memoryProfiling;
    std::string experimentType;
    int duration;
    int targets;
};

// Advanced CLI implementation
class AdvancedCLI {
public:
    CLIResult parseArguments(int argc, char** argv) {
        CLIResult result;
        result.success = true;
        result.message = "Arguments parsed successfully";
        
        // Default values
        result.mode = "normal";
        result.seed = 42;
        result.timestep = 0.001f;
        result.neurons = 500;
        result.regions = 1;
        result.plasticityMode = "both";
        result.rewardType = "simple";
        result.developmentStage = 1.0f;
        result.attentionMode = "competitive";
        result.metricsFormat = "json";
        result.cpuProfiling = false;
        result.memoryProfiling = false;
        result.experimentType = "basic";
        result.duration = 5000;
        result.targets = 50;
        result.saveCheckpoint = false;
        result.verbose = false;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            } else if (arg == "--version" || arg == "-v") {
                printVersion();
                exit(0);
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    result.configFile = argv[++i];
                } else {
                    result.success = false;
                    result.message = "Missing configuration file path after --config";
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    result.outputDir = argv[++i];
                } else {
                    result.success = false;
                    result.message = "Missing output directory after --output";
                }
            } else if (arg == "--checkpoint") {
                if (i + 1 < argc) {
                    result.configFile = argv[++i];
                    result.saveCheckpoint = true;
                } else {
                    result.success = false;
                    result.message = "Missing checkpoint path after --checkpoint";
                }
            } else if (arg == "--mode") {
                if (i + 1 < argc) {
                    result.mode = argv[++i];
                    if (!isValidMode(result.mode)) {
                        result.success = false;
                        result.message = "Invalid mode. Valid modes: normal, replay, development, exploration";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing mode after --mode";
                }
            } else if (arg == "--seed") {
                if (i + 1 < argc) {
                    try {
                        result.seed = std::stoi(argv[++i]);
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid seed value";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing seed after --seed";
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    try {
                        result.timestep = std::stof(argv[++i]);
                        if (result.timestep <= 0.0f || result.timestep > 1.0f) {
                            result.success = false;
                            result.message = "Timestep must be positive and less than 1.0";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid timestep value";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing timestep after --timestep";
                }
            } else if (arg == "--neurons") {
                if (i + 1 < argc) {
                    try {
                        result.neurons = std::stoi(argv[++i]);
                        if (result.neurons <= 0) {
                            result.success = false;
                            result.message = "Neuron count must be positive";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid neuron count";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing neuron count after --neurons";
                }
            } else if (arg == "--regions") {
                if (i + 1 < argc) {
                    try {
                        result.regions = std::stoi(argv[++i]);
                        if (result.regions <= 0) {
                            result.success = false;
                            result.message = "Region count must be positive";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid region count";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing region count after --regions";
                }
            } else if (arg == "--plasticity") {
                if (i + 1 < argc) {
                    result.plasticityMode = argv[++i];
                    if (!isValidPlasticityMode(result.plasticityMode)) {
                        result.success = false;
                        result.message = "Invalid plasticity mode. Valid modes: stdp, hebbian, both, none";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing plasticity mode after --plasticity";
                }
            } else if (arg == "--reward") {
                if (i + 1 < argc) {
                    result.rewardType = argv[++i];
                    if (!isValidRewardType(result.rewardType)) {
                        result.success = false;
                        result.message = "Invalid reward type. Valid types: simple, advanced, none";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing reward type after --reward";
                }
            } else if (arg == "--neuromodulation") {
                if (i + 1 < argc) {
                    std::string mod = argv[++i];
                    if (mod == "all") {
                        result.neuromodulators = {"dopamine", "curiosity", "novelty"};
                    } else if (mod == "none") {
                        result.neuromodulators = {};
                    } else {
                        result.neuromodulators = {mod};
                    }
                } else {
                    result.success = false;
                    result.message = "Missing neuromodulator after --neuromodulation";
                }
            } else if (arg == "--development") {
                if (i + 1 < argc) {
                    try {
                        result.developmentStage = std::stof(argv[++i]);
                        if (result.developmentStage < 0.0f || result.developmentStage > 1.0f) {
                            result.success = false;
                            result.message = "Development stage must be between 0.0 and 1.0";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid development stage";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing development stage after --development";
                }
            } else if (arg == "--attention") {
                if (i + 1 < argc) {
                    result.attentionMode = argv[++i];
                    if (!isValidAttentionMode(result.attentionMode)) {
                        result.success = false;
                        result.message = "Invalid attention mode. Valid modes: competitive, sequential, parallel";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing attention mode after --attention";
                }
            } else if (arg == "--verbose" || arg == "-v") {
                result.verbose = true;
            } else if (arg == "--metrics") {
                if (i + 1 < argc) {
                    result.metricsFormat = argv[++i];
                    if (!isValidMetricsFormat(result.metricsFormat)) {
                        result.success = false;
                        result.message = "Invalid metrics format. Valid formats: json, csv, binary, compressed";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing metrics format after --metrics";
                }
            } else if (arg == "--profile") {
                if (i + 1 < argc) {
                    std::string profileType = argv[++i];
                    if (profileType == "cpu") {
                        result.cpuProfiling = true;
                    } else if (profileType == "memory") {
                        result.memoryProfiling = true;
                    } else if (profileType == "both") {
                        result.cpuProfiling = true;
                        result.memoryProfiling = true;
                    } else {
                        result.success = false;
                        result.message = "Invalid profile type. Valid types: cpu, memory, both";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing profile type after --profile";
                }
            } else if (arg == "--experiment") {
                if (i + 1 < argc) {
                    result.experimentType = argv[++i];
                    if (!isValidExperimentType(result.experimentType)) {
                        result.success = false;
                        result.message = "Invalid experiment type. Valid types: basic, learning, stdp, connectivity";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing experiment type after --experiment";
                }
            } else if (arg == "--duration") {
                if (i + 1 < argc) {
                    try {
                        result.duration = std::stoi(argv[++i]);
                        if (result.duration <= 0) {
                            result.success = false;
                            result.message = "Duration must be positive";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid duration value";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing duration after --duration";
                }
            } else if (arg == "--targets") {
                if (i + 1 < argc) {
                    try {
                        result.targets = std::stoi(argv[++i]);
                        if (result.targets <= 0) {
                            result.success = false;
                            result.message = "Target count must be positive";
                        }
                    } catch (...) {
                        result.success = false;
                        result.message = "Invalid target count";
                    }
                } else {
                    result.success = false;
                    result.message = "Missing target count after --targets";
                }
            } else if (arg == "--script") {
                if (i + 1 < argc) {
                    // Load script parameters
                    loadScriptParameters(argv[++i], result);
                } else {
                    result.success = false;
                    result.message = "Missing script file after --script";
                }
            } else if (arg == "--batch") {
                // Batch processing flag
                // Implementation would handle batch processing of multiple scripts
                result.success = true;
            } else if (arg == "--list-experiments") {
                // List available experiments
                listAvailableExperiments();
                exit(0);
            } else if (arg == "--show-config") {
                // Show current configuration
                showCurrentConfiguration(result);
                exit(0);
            } else if (arg == "--validate-config") {
                // Validate configuration
                validateConfiguration(result);
                exit(0);
            } else if (arg == "--batch") {
                // Batch mode flag
                // Implementation for batch processing
                result.success = true;
                result.message = "Batch processing mode enabled";
            } else if (arg.substr(0, 2) == "--") {
                result.success = false;
                result.message = "Unknown command line argument: " + arg;
            } else {
                result.success = false;
                result.message = "Unknown argument: " + arg;
            }
        }
        
        return result;
    }
    
private:
    bool isValidMode(const std::string& mode) {
        return mode == "normal" || mode == "replay" || mode == "development" || mode == "exploration";
    }
    
    bool isValidPlasticityMode(const std::string& mode) {
        return mode == "stdp" || mode == "hebbian" || mode == "both" || mode == "none";
    }
    
    bool isValidRewardType(const std::string& type) {
        return type == "simple" || type == "advanced" || type == "none";
    }
    
    bool isValidAttentionMode(const std::string& mode) {
        return mode == "competitive" || mode == "sequential" || mode == "parallel";
    }
    
    bool isValidMetricsFormat(const std::string& format) {
        return format == "json" || format == "csv" || format == "binary" || format == "compressed";
    }
    
    bool isValidExperimentType(const std::string& type) {
        return type == "basic" || type == "learning" || type == "stdp" || type == "connectivity";
    }
    
    void loadScriptParameters(const std::string& scriptFile, CLIResult& result) {
        // Parse script file for additional parameters
        // This is a simplified implementation
        std::ifstream file(scriptFile);
        if (!file.is_open()) {
            result.success = false;
            result.message = "Cannot open script file: " + scriptFile;
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Parse script file format (simplified)
            if (line.find("mode=") != std::string::npos) {
                result.mode = line.substr(5);
            } else if (line.find("seed=") != std::string::npos) {
                result.seed = std::stoi(line.substr(5));
            } else if (line.find("duration=") != std::string::npos) {
                result.duration = std::stoi(line.substr(8));
            }
            // Add more parsing as needed
        }
        file.close();
    }
    
    void printHelp() {
        std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  nlm [options]" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "General Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --version, -v           Show version information" << std::endl;
        std::cout << "  --config <file>         Load configuration from file" << std::endl;
        std::cout << "  --output <dir>          Set output directory" << std::endl;
        std::cout << "  --checkpoint <file>     Load from checkpoint" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Simulation Options:" << std::endl;
        std::cout << "  --mode <mode>           Mode: normal, replay, development, exploration" << std::endl;
        std::cout << "  --seed <number>         Random seed for reproducibility" << std::endl;
        std::cout << "  --timestep <ms>         Simulation timestep" << std::endl;
        std::cout << "  --neurons <count>       Number of neurons" << std::endl;
        std::cout << "  --regions <count>       Number of regions" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Plasticity Options:" << std::endl;
        std::cout << "  --plasticity <mode>     Plasticity mode: stdp, hebbian, both, none" << std::endl;
        std::cout << "  --reward <type>         Reward type: simple, advanced, none" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation Options:" << std::endl;
        std::cout << "  --neuromodulation <mod>  Neuromodulator(s): dopamine, curiosity, novelty, all, none" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Development Options:" << std::endl;
        std::cout << "  --development <stage>   Development stage: 0.0 to 1.0" << std::endl;
        std::cout << "  --attention <mode>      Attention mode: competitive, sequential, parallel" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --verbose, -v            Enable verbose output" << std::endl;
        std::cout << "  --metrics <format>      Metrics format: json, csv, binary, compressed" << std::endl;
        std::cout << "  --profile <type>        Profiling: cpu, memory, both" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Experiment Options:" << std::endl;
        std::cout << "  --experiment <type>     Experiment type: basic, learning, stdp, connectivity" << std::endl;
        std::cout << "  --duration <steps>      Simulation duration in steps" << std::endl;
        std::cout << "  --targets <count>       Number of target neurons" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Utility Options:" << std::endl;
        std::cout << "  --list-experiments       List available experiments" << std::endl;
        std::cout << "  --show-config            Show current configuration" << std::endl;
        std::cout << "  --validate-config        Validate configuration" << std::endl;
        std::cout << "  --script <file>         Load parameters from script file" << std::endl;
        std::cout << "  --batch                 Batch processing mode" << std::endl;
        std::cout << "" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Version 0.1.0 - Advanced Neural Learning Machine" << std::endl;
        std::cout << "Phase 2 Complete Neural Computation" << std::endl;
        std::cout << "Built with C++23 support" << std::endl;
    }
    
    void listAvailableExperiments() {
        std::cout << "=== Available Experiments ===" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Basic Experiments:" << std::endl;
        std::cout << "  basic          - Basic connectivity and neural dynamics" << std::endl;
        std::cout << "  learning       - Plasticity learning experiment" << std::endl;
        std::cout << "  stdp           - STDP verification" << std::endl;
        std::cout << "  connectivity   - Basic connectivity test" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Advanced Experiments:" << std::endl;
        std::cout << "  development    - Developmental plasticity study" << std::endl;
        std::cout << "  exploration    - Curiosity-driven exploration" << std::endl;
        std::cout << "  replay         - Checkpoint replay" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Research Experiments:" << std::endl;
        std::cout << "  neuromodulation - Neuromodulation integration" << std::endl;
        std::cout << "  prediction     - Predictive coding study" << std::endl;
        std::cout << "  memory        - Memory consolidation" << std::endl;
        std::cout << "" << std::endl;
    }
    
    void showCurrentConfiguration(const CLIResult& result) {
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "Mode: " << result.mode << std::endl;
        std::cout << "Seed: " << result.seed << std::endl;
        std::cout << "Timestep: " << result.timestep << std::endl;
        std::cout << "Neurons: " << result.neurons << std::endl;
        std::cout << "Regions: " << result.regions << std::endl;
        std::cout << "Plasticity: " << result.plasticityMode << std::endl;
        std::cout << "Reward: " << result.rewardType << std::endl;
        std::cout << "Neuromodulators: ";
        for (size_t i = 0; i < result.neuromodulators.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << result.neuromodulators[i];
        }
        std::cout << std::endl;
        std::cout << "Development Stage: " << result.developmentStage << std::endl;
        std::cout << "Attention Mode: " << result.attentionMode << std::endl;
        std::cout << "Metrics Format: " << result.metricsFormat << std::endl;
        std::cout << "CPU Profiling: " << (result.cpuProfiling ? "enabled" : "disabled") << std::endl;
        std::cout << "Memory Profiling: " << (result.memoryProfiling ? "enabled" : "disabled") << std::endl;
        std::cout << "Experiment Type: " << result.experimentType << std::endl;
        std::cout << "Duration: " << result.duration << std::endl;
        std::cout << "Targets: " << result.targets << std::endl;
        std::cout << "Verbose: " << (result.verbose ? "enabled" : "disabled") << std::endl;
    }
    
    void validateConfiguration(const CLIResult& result) {
        std::cout << "=== Configuration Validation ===" << std::endl;
        bool isValid = true;
        
        if (result.timestep <= 0.0f || result.timestep > 1.0f) {
            std::cout << "ERROR: Invalid timestep: " << result.timestep << std::endl;
            isValid = false;
        }
        
        if (result.neurons <= 0) {
            std::cout << "ERROR: Invalid neuron count: " << result.neurons << std::endl;
            isValid = false;
        }
        
        if (result.regions <= 0) {
            std::cout << "ERROR: Invalid region count: " << result.regions << std::endl;
            isValid = false;
        }
        
        if (!isValidMode(result.mode)) {
            std::cout << "ERROR: Invalid mode: " << result.mode << std::endl;
            isValid = false;
        }
        
        if (!isValidPlasticityMode(result.plasticityMode)) {
            std::cout << "ERROR: Invalid plasticity mode: " << result.plasticityMode << std::endl;
            isValid = false;
        }
        
        if (!isValidRewardType(result.rewardType)) {
            std::cout << "ERROR: Invalid reward type: " << result.rewardType << std::endl;
            isValid = false;
        }
        
        if (result.developmentStage < 0.0f || result.developmentStage > 1.0f) {
            std::cout << "ERROR: Invalid development stage: " << result.developmentStage << std::endl;
            isValid = false;
        }
        
        if (!isValidAttentionMode(result.attentionMode)) {
            std::cout << "ERROR: Invalid attention mode: " << result.attentionMode << std::endl;
            isValid = false;
        }
        
        if (!isValidMetricsFormat(result.metricsFormat)) {
            std::cout << "ERROR: Invalid metrics format: " << result.metricsFormat << std::endl;
            isValid = false;
        }
        
        if (!isValidExperimentType(result.experimentType)) {
            std::cout << "ERROR: Invalid experiment type: " << result.experimentType << std::endl;
            isValid = false;
        }
        
        if (result.duration <= 0) {
            std::cout << "ERROR: Invalid duration: " << result.duration << std::endl;
            isValid = false;
        }
        
        if (result.targets <= 0) {
            std::cout << "ERROR: Invalid target count: " << result.targets << std::endl;
            isValid = false;
        }
        
        if (isValid) {
            std::cout << "SUCCESS: Configuration is valid." << std::endl;
        } else {
            std::cout << "FAILURE: Configuration has errors." << std::endl;
        }
    }
};