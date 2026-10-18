// Demonstration program for Enhanced Argument Parser
//
// This program demonstrates the new ArgumentParser functionality
// including configuration presets, argument parsing, and configuration loading

#include "core/ArgumentParser.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

void demonstrateArgumentParser() {
    std::cout << "=== Enhanced Argument Parser Demonstration ===" << std::endl << std::endl;
    
    // Create an argument parser
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Example 1: Parsing help
    std::cout << "1. Help Option:" << std::endl;
    parser.displayHelp();
    std::cout << std::endl;
    
    // Example 2: Version information
    std::cout << "2. Version Information:" << std::endl;
    parser.displayVersion();
    std::cout << std::endl;
    
    // Example 3: Preset demonstration
    std::cout << "3. Preset Configuration Examples:" << std::endl;
    std::cout << "   Available presets: research, performance, education, debug, production" << std::endl;
    std::cout << "   Each preset provides optimized settings for different use cases:" << std::endl;
    std::cout << "     - research: Detailed logging, validation enabled" << std::endl;
    std::cout << "     - performance: Optimized for speed, reduced logging" << std::endl;
    std::cout << "     - education: Simplified configuration, verbose output" << std::endl;
    std::cout << "     - debug: Maximum verbosity, all debugging enabled" << std::endl;
    std::cout << "     - production: Minimal output, validation enabled" << std::endl;
    std::cout << std::endl;
    
    // Example 4: Argument types
    std::cout << "4. Argument Types Supported:" << std::endl;
    std::cout << "   - --help, -h : Display help information" << std::endl;
    std::cout << "   - --version : Display version information" << std::endl;
    std::cout << "   - --verbose, -v : Enable verbose logging" << std::endl;
    std::cout << "   - --quiet, -q : Suppress all output" << std::endl;
    std::cout << "   - --config, -c <file> : Load configuration file" << std::endl;
    std::cout << "   - --json-config, -j <file> : Load JSON configuration file" << std::endl;
    std::cout << "   - --config-value, -C <key=val> : Override single configuration value" << std::endl;
    std::cout << "   - --config-object, -O <key=val> : Override nested configuration object" << std::endl;
    std::cout << "   - --list-configs : List available configuration presets" << std::endl;
    std::cout << "   - --show-config : Show current configuration" << std::endl;
    std::cout << "   - --validate-config : Validate configuration against schema" << std::endl;
    std::cout << "   - --export-config <file> : Export configuration to file" << std::endl;
    std::cout << "   - --merge-configs <files> : Merge multiple configuration files" << std::endl;
    std::cout << "   - --dry-run : Validate configuration without running simulation" << std::endl;
    std::cout << "   - --preset <name> : Apply a configuration preset" << std::endl;
    std::cout << "   - --log-level <level> : Set log level (DEBUG, INFO, WARN, ERROR, FATAL)" << std::endl;
    std::cout << "   - --simulation-steps <steps> : Set simulation steps" << std::endl;
    std::cout << std::endl;
    
    // Example 5: Configuration file example
    std::cout << "5. Configuration File Format:" << std::endl;
    std::cout << "   Traditional format (key=value):" << std::endl;
    std::cout << "     random_seed = 42" << std::endl;
    std::cout << "     neuron_count = 1000" << std::endl;
    std::cout << "     simulation_timestep = 0.001" << std::endl;
    std::cout << "   JSON format:" << std::endl;
    std::cout << "     {\"random_seed\": 42, \"neuron_count\": 1000, \"simulation_timestep\": 0.001}" << std::endl;
    std::cout << std::endl;
    
    // Example 6: Preset files
    std::cout << "6. Preset Configuration Files:" << std::endl;
    std::cout << "   All presets are available in configs/presets/:" << std::endl;
    std::cout << "     - research.json : Detailed logging for research" << std::endl;
    std::cout << "     - performance.json : Optimized for speed" << std::endl;
    std::cout << "     - education.json : Simplified for education" << std::endl;
    std::cout << "     - debug.json : Maximum verbosity for debugging" << std::endl;
    std::cout << "     - production.json : Production-ready configuration" << std::endl;
    std::cout << std::endl;
    
    // Example 7: Usage examples
    std::cout << "7. Usage Examples:" << std::endl;
    std::cout << "   Basic usage:" << std::endl;
    std::cout << "     nlm --config myconfig.cfg" << std::endl;
    std::cout << "     nlm --json-config config.json" << std::endl;
    std::cout << "     nlm --preset research --verbose" << std::endl;
    std::cout << "     nlm --preset debug --show-config" << std::endl;
    std::cout << "     nlm --config-value neuron_count=2000 --config-object brain.region=0.1" << std::endl;
    std::cout << "     nlm --validate-config --dry-run" << std::endl;
    std::cout << "     nlm --list-configs" << std::endl;
    std::cout << "     nlm --export-config production_config.json" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
}

int main(int argc, char** argv) {
    // Demonstrate the enhanced argument parser
    demonstrateArgumentParser();
    
    // Try to parse command line arguments if any are provided
    if (argc > 1) {
        std::cout << "\n=== Parsing Command Line Arguments ===" << std::endl;
        
        nlm::ArgumentParser parser;
        nlm::Config config;
        
        if (parser.parse(argc, argv, config)) {
            std::cout << "Command line arguments parsed successfully" << std::endl;
            
            // Display parsed options
            const auto& parsedOptions = parser.getParsedOptions();
            if (parsedOptions.count("--verbose")) {
                std::cout << "Verbose mode enabled" << std::endl;
            }
            
            if (parsedOptions.count("--quiet")) {
                std::cout << "Quiet mode enabled" << std::endl;
            }
            
            if (parsedOptions.count("--preset") && parser.getParsedValues().count("--preset")) {
                std::cout << "Preset: " << parser.getParsedValues()["--preset"] << std::endl;
            }
            
            if (parsedOptions.count("--config") && parser.getParsedValues().count("--config")) {
                std::cout << "Config file: " << parser.getParsedValues()["--config"] << std::endl;
            }
            
            if (parsedOptions.count("--json-config") && parser.getParsedValues().count("--json-config")) {
                std::cout << "JSON config file: " << parser.getParsedValues()["--json-config"] << std::endl;
            }
            
            if (parsedOptions.count("--show-config")) {
                std::cout << "\nCurrent Configuration:" << std::endl;
                std::cout << config.summary() << std::endl;
            }
            
        } else {
            std::cout << "Error parsing command line arguments" << std::endl;
        }
    }
    
    return 0;
}
