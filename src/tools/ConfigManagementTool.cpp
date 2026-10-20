// NLM Configuration Management Tool
// Provides utilities for configuration file generation, validation, and management

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace nlm;

void printUsage() {
    std::cout << "=== NLM Configuration Management Tool ===\n\n";
    std::cout << "Usage: nlm-config [COMMAND] [OPTIONS]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  generate [FORMAT] <output_file> [config_file]\n";
    std::cout << "      Generate configuration file in specified format\n";
    std::cout << "\n      FORMAT: json, yaml, yml, cfg (default)\n";
    std::cout << "\n      Example: nlm-config generate json configs/my_config.json\n";
    std::cout << "\n  validate [config_file]\n";
    std::cout << "      Validate configuration file and report issues\n";
    std::cout << "\n      Example: nlm-config validate configs/default.cfg\n";
    std::cout << "\n  merge [source1] [source2] <output_file>\n";
    std::cout << "      Merge multiple configuration files into one\n";
    std::cout << "\n  export [config_file] [export_file]\n";
    std::cout << "      Export configuration to human-readable format\n";
    std::cout << "\n  help                         Show this help message\n\n";
    std::cout << "Options:\n";
    std::cout << "  --verbose, -v               Enable verbose output\n";
    std::cout << "  --force                      Overwrite output file without confirmation\n";
    std::cout << "  --format <format>            Specify output format (default: cfg)\n\n";
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
}

void printVersion() {
    std::cout << "=== NLM Configuration Management Tool ===\n\n";
    std::cout << "Version: 0.1.0\n";
    std::cout << "Description: Configuration management utilities for NLM\n";
    std::cout << "Supported formats: JSON, YAML, key-value (.cfg)\n\n";
    std::cout << "Features:\n";
    std::cout << "  - Configuration file generation\n";
    std::cout << "  - Format validation\n";
    std::cout << "  - Configuration merging\n";
    std::cout << "  - Human-readable export\n";
    std::cout << "  - Batch processing\n\n";
}

bool generateConfig(const std::string& outputPath, const std::string& format, 
                   const std::string& inputPath = "") {
    try {
        auto config = std::make_shared<Config>();
        
        // Load existing config if provided
        if (!inputPath.empty()) {
            if (!config->loadFromFile(inputPath)) {
                std::cerr << "Error: Failed to load input configuration file: " << inputPath << std::endl;
                return false;
            }
            std::cout << "Loaded configuration from: " << inputPath << std::endl;
        }
        
        // Validate configuration
        if (!config->validateConfig()) {
            std::cerr << "Warning: Configuration validation failed, but proceeding with generation." << std::endl;
        }
        
        // Generate output file
        if (!config->generateConfigFile(outputPath, format)) {
            std::cerr << "Error: Failed to generate configuration file: " << outputPath << std::endl;
            return false;
        }
        
        std::cout << "Configuration file generated successfully: " << outputPath << std::endl;
        
        // Show summary
        std::cout << "\nConfiguration Summary:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception during configuration generation: " << e.what() << std::endl;
        return false;
    }
}

bool validateConfig(const std::string& configPath) {
    try {
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(configPath)) {
            std::cerr << "Error: Failed to load configuration file: " << configPath << std::endl;
            return false;
        }
        
        std::cout << "Configuration file loaded: " << configPath << std::endl;
        std::cout << "\nConfiguration Summary:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        // Validate configuration
        if (config->validateConfig()) {
            std::cout << "\n✓ Configuration validation PASSED" << std::endl;
            return true;
        } else {
            std::cout << "\n✗ Configuration validation FAILED" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception during configuration validation: " << e.what() << std::endl;
        return false;
    }
}

bool mergeConfigs(const std::vector<std::string>& inputPaths, const std::string& outputPath) {
    try {
        auto config = std::make_shared<Config>();
        
        for (const auto& inputPath : inputPaths) {
            std::cout << "Merging: " << inputPath << std::endl;
            if (!config->loadFromFile(inputPath)) {
                std::cerr << "Error: Failed to load configuration file: " << inputPath << std::endl;
                return false;
            }
        }
        
        // Save merged configuration
        if (!config->saveToFile(outputPath)) {
            std::cerr << "Error: Failed to save merged configuration to: " << outputPath << std::endl;
            return false;
        }
        
        std::cout << "\nMerged configuration saved to: " << outputPath << std::endl;
        std::cout << config->summary() << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception during configuration merging: " << e.what() << std::endl;
        return false;
    }
}

void printAvailableFormats() {
    std::cout << "\n=== Supported Configuration Formats ===\n\n";
    std::cout << "1. JSON (.json)" << std::endl;
    std::cout << "   - Human-readable, web-friendly" << std::endl;
    std::cout << "   - Widely supported by tools and libraries" << std::endl;
    std::cout << "   - Excellent for version control" << std::endl;
    std::cout << "   - Nested object support\n";
    
    std::cout << "2. YAML (.yaml, .yml)" << std::endl;
    std::cout << "   - Clean, readable syntax" << std::endl;
    std::cout << "   - Supports complex data structures" << std::endl;
    std::cout << "   - Excellent for configuration files" << std::endl;
    std::cout << "   - Good for CI/CD pipelines\n";
    
    std::cout << "3. Key-Value (.cfg, .txt, .ini)" << std::endl;
    std::cout << "   - Simple, traditional format" << std::endl;
    std::cout << "   - Easy to edit manually" << std::endl;
    std::cout << "   - Backward compatible with existing configs\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "--help" || command == "-h" || command == "help") {
        printUsage();
        return 0;
    }
    
    if (command == "--version" || command == "version") {
        printVersion();
        return 0;
    }
    
    if (command == "generate") {
        std::string format = "cfg";
        std::string outputPath;
        std::string inputPath;
        bool verbose = false;
        bool force = false;
        
        // Parse options
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--force") {
                force = true;
            } else if (arg == "--format" && i + 1 < argc) {
                format = argv[++i];
                std::transform(format.begin(), format.end(), format.begin(), ::tolower);
            } else if (outputPath.empty()) {
                outputPath = arg;
            } else if (inputPath.empty() && !outputPath.empty()) {
                inputPath = arg;
            }
        }
        
        if (outputPath.empty()) {
            std::cerr << "Error: Missing output file path" << std::endl;
            printUsage();
            return 1;
        }
        
        std::cout << "=== NLM Configuration Generator ===\n" << std::endl;
        
        if (!force) {
            std::ifstream checkFile(outputPath);
            if (checkFile.good()) {
                std::cout << "Output file already exists: " << outputPath << std::endl;
                std::cout << "Use --force to overwrite without confirmation\n";
                std::cout << "Continue? (y/n): ";
                char response;
                std::cin >> response;
                if (response != 'y' && response != 'Y') {
                    std::cout << "Operation cancelled.\n";
                    return 0;
                }
            }
        }
        
        return generateConfig(outputPath, format, inputPath) ? 0 : 1;
        
    } else if (command == "validate") {
        std::string configPath;
        
        if (argc >= 3) {
            configPath = argv[2];
        }
        
        if (configPath.empty()) {
            std::cerr << "Error: Missing configuration file path" << std::endl;
            printUsage();
            return 1;
        }
        
        std::cout << "=== NLM Configuration Validator ===\n" << std::endl;
        
        return validateConfig(configPath) ? 0 : 1;
        
    } else if (command == "merge") {
        std::vector<std::string> inputPaths;
        std::string outputPath;
        bool verbose = false;
        
        // Parse options
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (outputPath.empty() && inputPaths.size() > 1) {
                outputPath = arg;
            } else if (inputPaths.size() == 0 || (inputPaths.size() == 1 && outputPath.empty())) {
                inputPaths.push_back(arg);
            }
        }
        
        if (inputPaths.size() < 2) {
            std::cerr << "Error: Need at least 2 input configuration files to merge" << std::endl;
            printUsage();
            return 1;
        }
        
        if (outputPath.empty()) {
            std::cerr << "Error: Missing output file path" << std::endl;
            printUsage();
            return 1;
        }
        
        std::cout << "=== NLM Configuration Merger ===\n" << std::endl;
        
        return mergeConfigs(inputPaths, outputPath) ? 0 : 1;
        
    } else {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        printUsage();
        return 1;
    }
}
