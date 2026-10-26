// NLM Configuration Generator with improved error handling and validation
// Phase 6: Enhanced configuration management

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace nlm;

class ImprovedConfigGenerator {
public:
    bool generateConfig(const std::string& filename, const std::string& mode = "default");
    bool generateExamples(const std::string& outputDir);
    bool validateConfig(const std::string& filename, std::string& errorMsg);
    bool createTemplate(const std::string& filename);
    
private:
    std::string generateSection(const std::string& sectionName, 
                               const std::map<std::string, std::string>& values);
    bool saveToFile(const std::string& filename, const std::string& content);
    std::string getCurrentTimestamp();
    std::string trim(const std::string& str);
    bool isValidMode(const std::string& mode);
    bool isValidFilename(const std::string& filename);
    std::string getConfigModeDescription(const std::string& mode);
};

/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    try {
        std::string mode = "default";
        std::string outputFile = "nlm_config.cfg";
        std::string outputDir = "./examples";
        bool createExamples = false;
        bool createTemplate = false;
        bool showHelp = false;
        bool showVersion = false;
        std::string validateFile;
        
        // Parse command line arguments with better error handling
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg == "--version" || arg == "-v") {
                showVersion = true;
            } else if (arg == "--mode" && i + 1 < argc) {
                mode = argv[++i];
                if (!isValidMode(mode)) {
                    std::cerr << "Error: Invalid mode '" << mode << "'. Valid modes: default, demo, experiment, production, debug" << std::endl;
                    return 1;
                }
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
                if (!isValidFilename(outputFile)) {
                    std::cerr << "Error: Invalid output filename: " << outputFile << std::endl;
                    return 1;
                }
            } else if (arg == "--output-dir" && i + 1 < argc) {
                outputDir = argv[++i];
                if (!isValidFilename(outputDir)) {
                    std::cerr << "Error: Invalid output directory: " << outputDir << std::endl;
                    return 1;
                }
            } else if (arg == "--examples") {
                createExamples = true;
            } else if (arg == "--template") {
                createTemplate = true;
            } else if (arg == "--validate" && i + 1 < argc) {
                validateFile = argv[++i];
            } else if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                size_t pos = key.find('=');
                if (pos != std::string::npos) {
                    std::string value = key.substr(pos + 1);
                    key = key.substr(0, pos);
                    std::cout << "Custom option: " << key << " = " << value << std::endl;
                }
            }
        }
        
        if (showHelp) {
            printHelp();
            return 0;
        }
        
        if (showVersion) {
            printVersion();
            return 0;
        }
        
        ImprovedConfigGenerator generator;
        
        if (!validateFile.empty()) {
            std::cout << "Validating configuration file: " << validateFile << std::endl;
            std::string errorMsg;
            if (!generator.validateConfig(validateFile, errorMsg)) {
                std::cerr << "Configuration validation failed: " << errorMsg << std::endl;
                return 1;
            }
            std::cout << "Configuration validation passed!" << std::endl;
            return 0;
        }
        
        if (createTemplate) {
            std::cout << "Creating configuration template..." << std::endl;
            if (!generator.createTemplate(outputFile)) {
                std::cerr << "Failed to create configuration template" << std::endl;
                return 1;
            }
        } else if (createExamples) {
            std::cout << "Generating example configurations..." << std::endl;
            if (!generator.generateExamples(outputDir)) {
                std::cerr << "Failed to generate example configurations" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Generating configuration: " << outputFile << " (mode: " << mode << ")" << std::endl;
            if (!generator.generateConfig(outputFile, mode)) {
                std::cerr << "Failed to generate configuration" << std::endl;
                return 1;
            }
        }
        
        std::cout << "Configuration generation completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error" << std::endl;
        return 1;
    }
}

void printHelp() {
    std::cout << R"(
=== NLM Configuration Generator ===

Usage: nlm-config [OPTIONS]

Options:
  -h, --help              Show this help message
  -v, --version           Show version information
  --mode MODE             Configuration mode: default, demo, experiment, production, debug (default: default)
  --output FILE           Output configuration file (default: nlm_config.cfg)
  --output-dir DIR        Directory for example configurations (default: ./examples)
  --examples              Generate example configurations in directory
  --template              Create configuration template file
  --validate FILE         Validate existing configuration file

Modes:
  default                 Basic configuration for learning experiments
  demo                    Configuration optimized for demos and tutorials
  experiment              Configuration for comprehensive experiments
  production              Production-ready configuration
  debug                   Configuration for debugging and development

Examples:
  nlm-config                Generate default configuration
  nlm-config --mode demo     Generate demo configuration
  nlm-config --examples      Generate all example configurations
  nlm-config --template      Create configuration template file
  nlm-config --validate my_config.cfg  Validate configuration file

Generated files:
  - nlm_config.cfg              Default configuration
  - demo_config.cfg             Demo configuration
  - experiment_config.cfg       Experiment configuration
  - production_config.cfg       Production configuration
  - debug_config.cfg            Debug configuration

Example configurations directory:
  - examples/                  All example files
  - examples/simple_brain.cfg   Basic brain setup
  - examples/advanced_brain.cfg  Advanced features enabled
  - examples/performance.cfg     Performance optimized
  - examples/debug.cfg          Debug and development
  - examples/batch.cfg          Batch processing setup
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM Configuration Generator v6.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Phase 6: Final Integration Configuration Management" << std::endl;
    std::cout << "Enhanced with comprehensive error handling and validation" << std::endl;
}

bool isValidMode(const std::string& mode) {
    static const std::vector<std::string> validModes = {
        "default", "demo", "experiment", "production", "debug"
    };
    return std::find(validModes.begin(), validModes.end(), mode) != validModes.end();
}

bool isValidFilename(const std::string& filename) {
    if (filename.empty()) return false;
    // Check for invalid characters
    static const std::string invalidChars = "<>:\"/\\|?*";
    return filename.find_first_of(invalidChars) == std::string::npos;
}

// ConfigGenerator implementation
bool ImprovedConfigGenerator::generateConfig(const std::string& filename, const std::string& mode) {
    std::string content;
    
    try {
        if (mode == "demo") {
            content = generateSection("Demo Configuration", {
                {"brain.neuron_count", "500"},
                {"brain.region_count", "1"},
                {"brain.connection_probability", "0.1"},
                {"brain.v_thresh", "-50.0"},
                {"brain.v_rest", "-70.0"},
                {"brain.v_reset", "-75.0"},
                {"brain.tau_mem", "20.0"},
                {"brain.tau_ref", "2.0"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.001"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"experiment.max_steps", "1000"},
                {"experiment.checkpoint", "true"},
                {"experiment.replay", "true"},
                {"experiment.development", "true"}
            });
        } else if (mode == "experiment") {
            content = generateSection("Experiment Configuration", {
                {"brain.neuron_count", "2000"},
                {"brain.region_count", "2"},
                {"brain.connection_probability", "0.05"},
                {"brain.initial_weight_mean", "0.5"},
                {"brain.initial_weight_std", "0.1"},
                {"brain.v_thresh", "-50.0"},
                {"brain.v_rest", "-70.0"},
                {"brain.v_reset", "-75.0"},
                {"brain.tau_mem", "20.0"},
                {"brain.tau_ref", "2.0"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.001"},
                {"plasticity.stdp.tau_plus", "20.0"},
                {"plasticity.stdp.tau_minus", "20.0"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.hebbian.threshold", "0.5"},
                {"plasticity.structural.enable", "true"},
                {"plasticity.structural.rate", "0.0001"},
                {"neuromod.dopamine.scale", "1.5"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"experiment.max_steps", "5000"},
                {"experiment.checkpoint", "true"},
                {"experiment.checkpoint_path", "./checkpoints/"},
                {"experiment.replay", "true"},
                {"experiment.development", "true"},
                {"experiment.save_metrics", "true"},
                {"experiment.parallel", "true"}
            });
        } else if (mode == "production") {
            content = generateSection("Production Configuration", {
                {"brain.neuron_count", "5000"},
                {"brain.region_count", "3"},
                {"brain.connection_probability", "0.02"},
                {"brain.initial_weight_mean", "0.4"},
                {"brain.initial_weight_std", "0.15"},
                {"brain.v_thresh", "-45.0"},
                {"brain.v_rest", "-65.0"},
                {"brain.v_reset", "-70.0"},
                {"brain.tau_mem", "25.0"},
                {"brain.tau_ref", "3.0"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.0005"},
                {"plasticity.stdp.tau_plus", "25.0"},
                {"plasticity.stdp.tau_minus", "25.0"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.hebbian.threshold", "0.6"},
                {"plasticity.structural.enable", "true"},
                {"plasticity.structural.rate", "0.00005"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"neuromod.serotonin.scale", "0.8"},
                {"experiment.max_steps", "20000"},
                {"experiment.checkpoint", "true"},
                {"experiment.checkpoint_path", "./production_checkpoints/"},
                {"experiment.replay", "true"},
                {"experiment.development", "true"},
                {"experiment.save_metrics", "true"},
                {"experiment.parallel", "true"},
                {"experiment.garbage_collection", "true"},
                {"performance.optimization_level", "high"},
                {"performance.memory_pooling", "true"},
                {"performance.simd_optimization", "true"},
                {"performance.parallel_processing", "true"}
            });
        } else if (mode == "debug") {
            content = generateSection("Debug Configuration", {
                {"brain.neuron_count", "100"},
                {"brain.region_count", "1"},
                {"brain.connection_probability", "0.2"},
                {"brain.v_thresh", "-55.0"},
                {"brain.v_rest", "-75.0"},
                {"brain.v_reset", "-80.0"},
                {"brain.tau_mem", "15.0"},
                {"brain.tau_ref", "1.0"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.01"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.hebbian.threshold", "0.3"},
                {"plasticity.structural.enable", "true"},
                {"plasticity.structural.rate", "0.001"},
                {"neuromod.dopamine.scale", "2.0"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"experiment.max_steps", "500"},
                {"experiment.checkpoint", "true"},
                {"experiment.checkpoint_path", "./debug_checkpoints/"},
                {"experiment.replay", "true"},
                {"experiment.development", "true"},
                {"experiment.save_metrics", "true"},
                {"performance.optimization_level", "low"},
                {"performance.memory_pooling", "false"},
                {"performance.simd_optimization", "false"},
                {"performance.parallel_processing", "false"},
                {"logging.level", "debug"},
                {"logging.file", "nlm_debug.log"},
                {"logging.max_size", "52428800"},
                {"logging.max_files", "10"}
            });
        } else {
            // Default mode
            content = generateSection("Default NLM Configuration", {
                {"brain.neuron_count", "1000"},
                {"brain.region_count", "1"},
                {"brain.connection_probability", "0.1"},
                {"brain.v_thresh", "-50.0"},
                {"brain.v_rest", "-70.0"},
                {"brain.v_reset", "-75.0"},
                {"brain.tau_mem", "20.0"},
                {"brain.tau_ref", "2.0"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.001"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"experiment.max_steps", "2000"},
                {"experiment.checkpoint", "false"},
                {"experiment.replay", "false"},
                {"experiment.development", "false"}
            });
        }
        
        return saveToFile(filename, content);
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating configuration (mode: " << mode << "): " << e.what() << std::endl;
        return false;
    }
}

bool ImprovedConfigGenerator::generateExamples(const std::string& outputDir) {
    try {
        // Create directory
        std::string mkdirCmd = "mkdir -p " + outputDir;
        system(mkdirCmd.c_str());
        
        std::vector<std::pair<std::string, std::string>> examples = {
            {"simple_brain.cfg", "default"},
            {"advanced_brain.cfg", "demo"},
            {"performance.cfg", "production"},
            {"debug.cfg", "debug"},
            {"batch.cfg", "experiment"}
        };
        
        bool allSuccess = true;
        for (const auto& example : examples) {
            std::string filepath = outputDir + "/" + example.first;
            if (!generateConfig(filepath, example.second)) {
                std::cerr << "Failed to generate " << example.first << std::endl;
                allSuccess = false;
            } else {
                std::cout << "Generated " << example.first << std::endl;
            }
        }
        
        return allSuccess;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating example configurations: " << e.what() << std::endl;
        return false;
    }
}

bool ImprovedConfigGenerator::validateConfig(const std::string& filename, std::string& errorMsg) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        errorMsg = "Cannot open file: " + filename;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int lineCount = 0;
    
    try {
        while (std::getline(file, line)) {
            lineNumber++;
            lineCount++;
            
            // Check for extremely long lines (potential corruption)
            if (line.length() > 10000) {
                errorMsg = "Line " + std::to_string(lineNumber) + " is too long (potential corruption)";
                return false;
            }
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate format: key = value
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                errorMsg = "Invalid format at line " + std::to_string(lineNumber) + ": missing '='";
                return false;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                errorMsg = "Empty key at line " + std::to_string(lineNumber);
                return false;
            }
            
            if (value.empty()) {
                errorMsg = "Empty value at line " + std::to_string(lineNumber);
                return false;
            }
            
            // Validate key format (allow dots for nested keys)
            if (key.find_first_of(" \t\n\r") != std::string::npos) {
                errorMsg = "Key contains whitespace at line " + std::to_string(lineNumber);
                return false;
            }
        }
        
        if (lineCount == 0) {
            errorMsg = "File is empty";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        errorMsg = "Exception during validation: " + std::string(e.what());
        return false;
    }
}

bool ImprovedConfigGenerator::createTemplate(const std::string& filename) {
    try {
        std::string content = R"(
# NLM Configuration Template
# Generated on " + getCurrentTimestamp() + R"(
# This file contains all available configuration options

# ============================================================================
# BRAIN CONFIGURATION
# ============================================================================
# Basic brain parameters
brain.neuron_count = 1000                  # Total number of neurons
brain.region_count = 1                     # Number of neural regions
brain.connection_probability = 0.1         # Probability of connections between neurons
brain.initial_weight_mean = 0.5            # Mean initial synaptic weight
brain.initial_weight_std = 0.1             # Standard deviation of initial weights
brain.v_thresh = -50.0                     # Neuron threshold potential (mV)
brain.v_rest = -70.0                      # Resting potential (mV)
brain.v_reset = -75.0                      # Reset potential after spike (mV)
brain.tau_mem = 20.0                       # Membrane time constant (ms)
brain.tau_ref = 2.0                        # Refractory period (ms)

# ============================================================================
# PLASTICITY CONFIGURATION
# ============================================================================
# STDP (Spike-Timing-Dependent Plasticity)
plasticity.stdp.enable = true              # Enable STDP
plasticity.stdp.learning_rate = 0.001      # STDP learning rate
plasticity.stdp.tau_plus = 20.0            # STDP time constant for potentiation
plasticity.stdp.tau_minus = 20.0           # STDP time constant for depression

# Hebbian plasticity
plasticity.hebbian.enable = true           # Enable Hebbian learning
plasticity.hebbian.threshold = 0.5         # Threshold for Hebbian updates

# Structural plasticity
plasticity.structural.enable = true        # Enable structural plasticity
plasticity.structural.rate = 0.0001        # Rate of synaptogenesis/pruning

# ============================================================================
# NEUROMODULATION CONFIGURATION
# ============================================================================
# Dopamine system
neuromod.dopamine.scale = 1.0              # Dopamine modulation scale

# Curiosity system
neuromod.curiosity.enable = true           # Enable curiosity-driven exploration

# Novelty detection
neuromod.novelty.enable = true              # Enable novelty detection

# Acetylcholine
neuromod.acetylcholine.scale = 1.0         # Acetylcholine modulation scale

# Norepinephrine
neuromod.norepinephrine.scale = 1.0        # Norepinephrine modulation scale

# Serotonin
neuromod.serotonin.scale = 1.0             # Serotonin modulation scale

# ============================================================================
# EXPERIMENT CONFIGURATION
# ============================================================================
experiment.max_steps = 2000                # Maximum simulation steps
experiment.checkpoint = false              # Enable checkpointing
experiment.checkpoint_path = ./checkpoints/ # Checkpoint directory
experiment.replay = false                   # Enable memory replay
experiment.development = false              # Enable developmental stages
experiment.save_metrics = true              # Save experiment metrics
experiment.parallel = false                 # Enable parallel processing

# ============================================================================
# PERFORMANCE CONFIGURATION
# ============================================================================
performance.optimization_level = medium    # Optimization level: low, medium, high
performance.memory_pooling = false         # Enable memory pooling
performance.simd_optimization = false      # Enable SIMD instructions
performance.parallel_processing = false     # Enable parallel processing
garbage_collection = false                  # Enable garbage collection

# ============================================================================
# LOGGING CONFIGURATION
# ============================================================================
logging.level = info                       # Log level: debug, info, warn, error
logging.file = nlm.log                     # Log file path
logging.max_size = 10485760                # Maximum log file size (10MB)
logging.max_files = 5                      # Maximum number of log files

# ============================================================================
# ADVANCED OPTIONS
# ============================================================================
# Add your custom options here
# custom.option_name = value
)" + getCurrentTimestamp() + R"(

# ============================================================================
# USAGE NOTES
# ============================================================================
# 1. Comments start with # or /
# 2. Empty lines are ignored
# 3. Values can be quoted with double or single quotes
# 4. Boolean values: true, false
# 5. Numeric values: int, double
# 6. String values: enclosed in quotes

# ============================================================================
# EXAMPLES
# ============================================================================
# Simple learning experiment:
# brain.neuron_count = 500
# plasticity.stdp.enable = true
# experiment.max_steps = 1000

# Complex research setup:
# brain.neuron_count = 5000
# brain.region_count = 3
# plasticity.stdp.enable = true
# plasticity.hebbian.enable = true
# plasticity.structural.enable = true
# neuromod.curiosity.enable = true
# neuromod.novelty.enable = true
# experiment.max_steps = 20000
# experiment.checkpoint = true
# experiment.checkpoint_path = ./research_checkpoints/
)""";
        
        return saveToFile(filename, content);
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating configuration template: " << e.what() << std::endl;
        return false;
    }
}

std::string ImprovedConfigGenerator::generateSection(const std::string& sectionName,
                                            const std::map<std::string, std::string>& values) {
    std::string content = "# " + sectionName + " Configuration\n";
    content += "# Generated by Improved NLM Configuration Generator\n";
    content += "# " + getCurrentTimestamp() + "\n\n";
    
    for (const auto& pair : values) {
        content += pair.first + " = " + pair.second + "\n";
    }
    
    return content;
}

bool ImprovedConfigGenerator::saveToFile(const std::string& filename, const std::string& content) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot write to file: " << filename << std::endl;
            return false;
        }
        
        // Check content size before writing
        if (content.length() > 10000000) { // 10MB limit
            std::cerr << "Content too large to write to file" << std::endl;
            return false;
        }
        
        file << content;
        file.close();
        
        // Verify file was written
        std::ifstream verifyFile(filename);
        if (!verifyFile.is_open()) {
            std::cerr << "File was not created properly: " << filename << std::endl;
            return false;
        }
        verifyFile.close();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error writing to file " << filename << ": " << e.what() << std::endl;
        return false;
    }
}

std::string ImprovedConfigGenerator::getCurrentTimestamp() {
    try {
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    } catch (...) {
        return "Unknown time";
    }
}

std::string ImprovedConfigGenerator::trim(const std::string& str) {
    try {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    } catch (...) {
        return str;
    }
}
