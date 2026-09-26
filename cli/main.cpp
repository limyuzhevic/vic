// NLM Command Line Interface
// CLI for NLM configuration and simulation control

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// Include NLM headers
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

namespace cli {

class NLMCommandLine {
public:
    NLMCommandLine() = default;
    ~NLMCommandLine() = default();
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Help and version information
    void printHelp(const std::string& programName);
    void printVersion();
    
    // Configuration file parsing
    bool loadConfigFromFile(const std::string& filename, nlm::Config& config);
    
    // Command line argument parsing
    bool parseArguments(int argc, char** argv, std::string& configFile, 
                       int& numSteps, std::string& outputFile, bool& verbose);
    
    // Simulation execution
    int executeSimulation(const nlm::Config& config, int numSteps, 
                         const std::string& outputFile, bool verbose);
    
    // Configuration file generation
    void generateSampleConfig(const std::string& filename);
    
    // Error handling
    void printError(const std::string& message);
    
    // Progress reporting
    void printProgress(int step, int totalSteps);
};

int NLMCommandLine::run(int argc, char** argv) {
    std::string configFile;
    int numSteps = 100;
    std::string outputFile;
    bool verbose = false;
    
    // Parse command line arguments
    if (!parseArguments(argc, argv, configFile, numSteps, outputFile, verbose)) {
        return 1;
    }
    
    // Create default configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        if (!loadConfigFromFile(configFile, *config)) {
            printError("Failed to load configuration file");
            return 1;
        }
    }
    
    // Set default configuration if no file loaded
    if (config->getKeys().empty()) {
        std::cout << "Using default configuration" << std::endl;
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
    }
    
    // Execute simulation
    return executeSimulation(*config, numSteps, outputFile, verbose);
}

void NLMCommandLine::printHelp(const std::string& programName) {
    std::cout << "NLM (Neural Learning Machine) Command Line Interface" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -c, --config FILE       Configuration file (JSON or key=value format)" << std::endl;
    std::cout << "  -n, --steps N           Number of simulation steps (default: 100)" << std::endl;
    std::cout << "  -o, --output FILE       Output file for brain checkpoint" << std::endl;
    std::cout << "  -V, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -g, --generate-config FILE Generate sample configuration file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                          Run simulation with default config" << std::endl;
    std::cout << "  " << programName << " -c config.json -n 500 -o brain.bin" << std::endl;
    std::cout << "  " << programName << " -c config.txt -V -o output.bin" << std::endl;
    std::cout << "  " << programName << " -g sample_config.txt" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file formats:" << std::endl;
    std::cout << "  - JSON format: brain.neuron_count = 1000" << std::endl;
    std::cout << "  - Simple format: brain.neuron_count=1000" << std::endl;
    std::cout << "  - Comments start with #" << std::endl;
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM Command Line Interface" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
}

bool NLMCommandLine::loadConfigFromFile(const std::string& filename, nlm::Config& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printError("Cannot open configuration file: " + filename);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasContent = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Remove leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        
        if (start == std::string::npos) {
            continue; // Empty line
        }
        
        line = line.substr(start, end - start);
        
        // Skip comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            printError("Invalid configuration format in " + filename + ":" + 
                      std::to_string(lineNumber) + " - missing '='");
            return false;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Remove quotes from value if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Trim whitespace from key and value
        size_t keyStart = key.find_first_not_of(" \t");
        size_t keyEnd = key.find_last_not_of(" \t");
        if (keyStart == std::string::npos) {
            continue;
        }
        key = key.substr(keyStart, keyEnd - keyStart);
        
        size_t valueStart = value.find_first_not_of(" \t");
        size_t valueEnd = value.find_last_not_of(" \t");
        if (valueStart == std::string::npos) {
            continue;
        }
        value = value.substr(valueStart, valueEnd - valueStart);
        
        // Parse different value types
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            config.set(key, boolValue, nlm::ConfigSource::File);
            hasContent = true;
        } else if (value.find('.') != std::string::npos || value == "inf" || value == "nan") {
            try {
                double doubleValue = std::stod(value);
                config.set(key, doubleValue, nlm::ConfigSource::File);
                hasContent = true;
            } catch (...) {
                printError("Invalid numeric value in " + filename + ":" + 
                          std::to_string(lineNumber) + ": " + value);
                return false;
            }
        } else {
            try {
                // Try integer first
                size_t pos;
                long long intValue = std::stoll(value, &pos);
                if (pos == value.size()) {
                    config.set(key, intValue, nlm::ConfigSource::File);
                    hasContent = true;
                } else {
                    // Fall back to string
                    config.set(key, value, nlm::ConfigSource::File);
                    hasContent = true;
                }
            } catch (...) {
                // Use as string
                config.set(key, value, nlm::ConfigSource::File);
                hasContent = true;
            }
        }
    }
    
    file.close();
    return hasContent;
}

bool NLMCommandLine::parseArguments(int argc, char** argv, std::string& configFile, 
                                   int& numSteps, std::string& outputFile, bool& verbose) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return false;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return false;
        } else if (arg == "-g" || arg == "--generate-config") {
            if (i + 1 < argc) {
                generateSampleConfig(argv[++i]);
                return false;
            } else {
                printError("Missing filename for --generate-config");
                return false;
            }
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                printError("Missing filename for --config");
                return false;
            }
        } else if (arg == "-n" || arg == "--steps") {
            if (i + 1 < argc) {
                try {
                    numSteps = std::stoi(argv[++i]);
                    if (numSteps <= 0) {
                        printError("Number of steps must be positive");
                        return false;
                    }
                } catch (...) {
                    printError("Invalid number of steps");
                    return false;
                }
            } else {
                printError("Missing number for --steps");
                return false;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                printError("Missing filename for --output");
                return false;
            }
        } else if (arg == "-V" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] == '-') {
            printError("Unknown option: " + arg);
            return false;
        } else {
            printError("Unknown argument: " + arg);
            return false;
        }
    }
    
    return true;
}

int NLMCommandLine::executeSimulation(const nlm::Config& config, int numSteps, 
                                     const std::string& outputFile, bool verbose) {
    std::cout << "Starting NLM simulation..." << std::endl;
    std::cout << "Configuration keys: ";
    auto keys = config.getKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << keys[i];
    }
    std::cout << std::endl;
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(std::const_pointer_cast<nlm::Config>(std::shared_ptr<const nlm::Config>(&config, [](const nlm::Config*) {})));
    
    // Initialize brain
    if (verbose) {
        std::cout << "Initializing brain..." << std::endl;
    }
    
    if (!brain->initialize()) {
        printError("Failed to initialize brain");
        return 1;
    }
    
    // Run simulation
    std::cout << "Running " << numSteps << " simulation steps..." << std::endl;
    
    if (verbose) {
        std::cout << "Step | Neurons | Firing | Spikes | Rate (Hz) | Curiosity" << std::endl;
        std::cout << "-----|---------|--------|--------|----------|----------" << std::endl;
    }
    
    for (int step = 0; step < numSteps; ++step) {
        brain->step(step);
        
        if (verbose) {
            printProgress(step, numSteps);
        }
        
        // Print statistics periodically
        if (step % 100 == 0 || step == numSteps - 1) {
            size_t neurons = brain->getTotalNeuronCount();
            size_t firing = brain->getFiringNeuronCount();
            size_t spikes = brain->getTotalSpikeCount();
            float rate = brain->getAverageFiringRate();
            
            std::cout << "Step " << step << ": "
                      << neurons << " neurons, "
                      << firing << " firing, "
                      << spikes << " total spikes, "
                      << rate << " Hz" << std::endl;
        }
    }
    
    // Get final statistics
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalFiring = brain->getFiringNeuronCount();
    size_t finalSpikes = brain->getTotalSpikeCount();
    float finalRate = brain->getAverageFiringRate();
    
    std::cout << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Final Statistics:" << std::endl;
    std::cout << "  Total Neurons: " << finalNeurons << std::endl;
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Firing Neurons: " << finalFiring << std::endl;
    std::cout << "  Total Spikes: " << finalSpikes << std::endl;
    std::cout << "  Average Firing Rate: " << finalRate << " Hz" << std::endl;
    std::cout << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    // Save brain checkpoint if output file specified
    if (!outputFile.empty()) {
        std::cout << std::endl << "Saving brain checkpoint to: " << outputFile << std::endl;
        if (brain->save(outputFile)) {
            std::cout << "✓ Brain checkpoint saved successfully!" << std::endl;
        } else {
            printError("Failed to save brain checkpoint");
            return 1;
        }
    }
    
    return 0;
}

void NLMCommandLine::generateSampleConfig(const std::string& filename) {
    std::cout << "Generating sample configuration file: " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Cannot create configuration file: " + filename);
        return;
    }
    
    file << "# NLM Sample Configuration File" << std::endl;
    file << "# This file contains example configuration settings for NLM" << std::endl;
    file << "# Use with: nlm --config " << filename << std::endl;
    file << std::endl;
    file << "# Brain Configuration" << std::endl;
    file << "brain.neuron_count = 1000" << std::endl;
    file << "brain.synapse_density = 0.1" << std::endl;
    file << "brain.connection_probability = 0.05" << std::endl;
    file << "brain.initial_weight_mean = 0.5" << std::endl;
    file << "brain.initial_weight_std = 0.1" << std::endl;
    file << std::endl;
    file << "# Plasticity Configuration" << std::endl;
    file << "plasticity.stdp.enable = true" << std::endl;
    file << "plasticity.stdp.learning_rate = 0.001" << std::endl;
    file << "plasticity.stdp.tau_plus = 20.0" << std::endl;
    file << "plasticity.stdp.tau_minus = 20.0" << std::endl;
    file << "plasticity.hebbian.enable = true" << std::endl;
    file << "plasticity.structural.enable = true" << std::endl;
    file << std::endl;
    file << "# Neuromodulation Configuration" << std::endl;
    file << "neuromod.dopamine.scale = 1.0" << std::endl;
    file << "neuromod.curiosity.enable = true" << std::endl;
    file << "neuromod.novelty.enable = true" << std::endl;
    file << std::endl;
    file << "# Developmental Configuration" << std::endl;
    file << "development.enabled = true" << std::endl;
    file << "development.init_plasticity = 1.0" << std::endl;
    file << "development.synaptogenesis_rate = 0.0001" << std::endl;
    file << "development.pruning_rate = 0.00001" << std::endl;
    
    file.close();
    std::cout << "✓ Sample configuration file generated successfully!" << std::endl;
}

void NLMCommandLine::printError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

void NLMCommandLine::printProgress(int step, int totalSteps) {
    int progress = (step * 100) / totalSteps;
    std::cout << "\r[";
    for (int i = 0; i < 50; ++i) {
        if (i < (progress * 50) / 100) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << progress << "% (Step " << step << "/" << totalSteps << ")";
    std::cout.flush();
}

} // namespace cli

// Main function
int main(int argc, char** argv) {
    cli::NLMCommandLine cli;
    return cli.run(argc, argv);
}
