// NLM Demo Framework Main Entry Point
// Unified Command-Line Interface for All NLM Demos
//
// This is the main entry point for the enhanced NLM demo system.
// It provides a unified interface for running all NLM demonstrations,
// tests, and benchmarks with advanced command-line options.

#include "core/DemoFramework.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Global demo runner instance
DemoRunner& getDemoRunner() {
    static DemoRunner runner;
    return runner;
}

// Print detailed help information
void printDetailedHelp() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          NLM Demo Framework Help                            ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Basic Usage:                                                                    ║" << std::endl;
    std::cout << "║    nlm_demo [OPTIONS] [DEMO]                                                    ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Available Demos:                                                               ║" << std::endl;
    std::cout << "║    --demo=basic         Basic connectivity and spike propagation test          ║" << std::endl;
    std::cout << "║    --demo=plasticity    Plasticity learning and synaptic weight changes        ║" << std::endl;
    std::cout << "║    --demo=stdp          STDP (spike-timing dependent plasticity) verification    ║" << std::endl;
    std::cout << "║    --demo=phase6        Phase 6 integration test (all systems connected)         ║" << std::endl;
    std::cout << "║    --demo=text          Text teaching and concept formation demo               ║" << std::endl;
    std::cout << "║    --demo=benchmark     Performance benchmark for different network sizes       ║" << std::endl;
    std::cout << "║    --demo=all           Run all available demos sequentially                     ║" << std::endl;
    std::cout << "║    --demo=custom        Run custom simulation with specified parameters           ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Configuration Options:                                                          ║" << std::endl;
    std::cout << "║    --config=<file>      Configuration file (default: configs/default.cfg)       ║" << std::endl;
    std::cout << "║    --output=<file>      Output file for results (default: demo_results_<timestamp>.txt) ║" << std::endl;
    std::cout << "║    --checkpoint=<path>  Checkpoint path for save/load operations                   ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Simulation Options:                                                             ║" << std::endl;
    std::cout << "║    --steps=<count>      Number of simulation steps (default: 1000)             ║" << std::endl;
    std::cout << "║    --neuron-count=<n>   Number of neurons (default: 500)                       ║" << std::endl;
    std::cout << "║    --region-count=<n>   Number of regions (default: 1)                        ║" << std::endl;
    std::cout << "║    --connection-prob=<p>Connection probability (0.0-1.0, default: 0.1)          ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Output and Control Options:                                                      ║" << std::endl;
    std::cout << "║    --verbose, -v        Enable verbose output                                    ║" << std::endl;
    std::cout << "║    --quiet, -q          Suppress non-essential output                            ║" << std::endl;
    std::cout << "║    --progress           Show progress indicators (for long simulations)          ║" << std::endl;
    std::cout << "║    --no-colors          Disable colored output                                  ║" << std::endl;
    std::cout << "║    --log-level=<level>  Log level: debug, info, warning, error, critical           ║" << std::endl;
    std::cout << "║    --profile=<file>     Save performance profiling data                        ║" << std::endl;
    std::cout << "║    --continuous         Run continuously until interrupted (with --runtime)       ║" << std::endl;
    std::cout << "║    --runtime=<seconds>  Run time in seconds (for continuous mode)              ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Advanced Options:                                                               ║" << std::endl;
    std::cout << "║    --advanced           Enable advanced features and commands                    ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Quick Examples:                                                                 ║" << std::endl;
    std::cout << "║    nlm_demo --demo=basic --steps=1000 --verbose                                 ║" << std::endl;
    std::cout << "║    nlm_demo --demo=plasticity --config=myconfig.cfg                              ║" << std::endl;
    std::cout << "║    nlm_demo --demo=all --progress --output=results.txt                        ║" << std::endl;
    std::cout << "║    nlm_demo --demo=benchmark --steps=100 --neuron-count=100                    ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Configuration File Format:                                                        ║" << std::endl;
    std::cout << "║    # NLM Configuration File                                                        ║" << std::endl;
    std::cout << "║    # comment lines start with #                                                   ║" << std::endl;
    std::cout << "║    neuron_count = 1000                                                            ║" << std::endl;
    std::cout << "║    region_count = 2                                                               ║" << std::endl;
    std::cout << "║    connection_probability = 0.1                                                    ║" << std::endl;
    std::cout << "║    simulation_timestep = 0.001                                                    ║" << std::endl;
    std::cout << "║    stdp_ltp_weight = 0.01                                                         ║" << std::endl;
    std::cout << "║    stdp_ltd_weight = 0.012                                                        ║" << std::endl;
    std::cout << "║    random_seed = 42                                                                ║" << std::endl;
    std::cout << "║    log_level = INFO                                                                ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Available Config Files:                                                          ║" << std::endl;
    
    // List available config files
    std::vector<std::string> configs = DemoRunner::getAvailableConfigs();
    if (configs.empty()) {
        std::cout << "║    No config files found in configs/ directory" << std::endl;
    } else {
        for (const auto& config : configs) {
            std::cout << "║    - " << config << std::endl;
        }
    }
    
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Output Formats:                                                                 ║" << std::endl;
    std::cout << "║    Text results saved to: demo_results_YYYYMMDD_HHMMSS.txt                         ║" << std::endl;
    std::cout << "║    Performance profiling: demo_profile_YYYYMMDD_HHMMSS.json                      ║" << std::endl;
    std::cout << "║    Progress logs: demo_progress_YYYYMMDD_HHMMSS.log                               ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Advanced Features:                                                              ║" << std::endl;
    std::cout << "║    --advanced enables:                                                            ║" << std::endl;
    std::cout << "║      • Enhanced error handling and recovery                                     ║" << std::endl;
    std::cout << "║      • Performance profiling and monitoring                                      ║" << std::endl;
    std::cout << "║      • Automatic checkpoint saving and loading                                   ║" << std::endl;
    std::cout << "║      • Progress monitoring for long simulations                                  ║" << std::endl;
    std::cout << "║      • Detailed result aggregation and reporting                                  ║" << std::endl;
    std::cout << "║      • Advanced configuration validation                                         ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "║  Troubleshooting:                                                                 ║" << std::endl;
    std::cout << "║    • Use --verbose to see detailed execution information                           ║" << std::endl;
    std::cout << "║    • Use --progress to monitor long simulations                                   ║" << std::endl;
    std::cout << "║    • Check demo_results_*.txt for detailed output                                 ║" << std::endl;
    std::cout << "║    • Use --debug for maximum logging detail                                       ║" << std::endl;
    std::cout << "║                                                                                  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Demo Framework v2.0                     ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  Neural Learning Machine (熙然)                              ║" << std::endl;
    std::cout << "║  Enhanced Demo Framework                                     ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  Version: 2.0.0                                            ║" << std::endl;
    std::cout << "║  Build: Enhanced with modular architecture                  ║" << std::endl;
    std::cout << "║  Features:                                                 ║" << std::endl;
    std::cout << "║    • Unified demo/test framework                            ║" << std::endl;
    std::cout << "║    • Advanced command-line interface                        ║" << std::endl;
    std::cout << "║    • Comprehensive configuration system                      ║" << std::endl;
    std::cout << "║    • Enhanced error handling and recovery                  ║" << std::endl;
    std::cout << "║    • Performance monitoring and profiling                   ║" << std::endl;
    std::cout << "║    • Progress reporting for long simulations                ║" << std::endl;
    std::cout << "║    • Modular, maintainable code architecture                ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  Phase 6 Integration Ready:                                ║" << std::endl;
    std::cout << "║    ✓ Memory systems connected                              ║" << std::endl;
    std::cout << "║    ✓ Neuromodulation integrated                             ║" << std::endl;
    std::cout << "║    ✓ Prediction system working                             ║" << std::endl;
    std::cout << "║    ✓ Development integration active                        ║" << std::endl;
    std::cout << "║    ✓ Checkpoint save/load functional                        ║" << std::endl;
    std::cout << "║    ✓ Replay and consolidation working                       ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Parse enhanced command-line arguments
bool parseEnhancedArguments(int argc, char** argv, DemoOptions& options) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Help and version options
            if (arg == "--help" || arg == "-h") {
                printDetailedHelp();
                return false;
            }
            else if (arg == "--version") {
                printVersion();
                return false;
            }
            else if (arg == "--list") {
                DemoRunner::listAvailableDemos();
                return false;
            }
            else if (arg == "--help-detailed") {
                printDetailedHelp();
                return false;
            }
            
            // Demo selection
            else if (arg.substr(0, 9) == "--demo=") {
                std::string demoName = arg.substr(9);
                static const std::map<std::string, DemoMode> demoMap = {
                    {"basic", DemoMode::BASIC_CONNECTIVITY},
                    {"plasticity", DemoMode::PLASTICITY_LEARNING},
                    {"stdp", DemoMode::STDP_VERIFICATION},
                    {"phase6", DemoMode::PHASE6_INTEGRATION},
                    {"text", DemoMode::TEXT_DEMO},
                    {"benchmark", DemoMode::BENCHMARK},
                    {"all", DemoMode::ALL_TESTS},
                    {"custom", DemoMode::CUSTOM_CONFIG},
                    {"enhanced", DemoMode::BASIC_CONNECTIVITY}, // Alias for basic with enhanced features
                    {"learning", DemoMode::PLASTICITY_LEARNING}, // Alias for plasticity
                    {"neuromod", DemoMode::STDP_VERIFICATION}, // Alias for STDP
                    {"integration", DemoMode::PHASE6_INTEGRATION}, // Alias for phase6
                };
                
                auto it = demoMap.find(demoName);
                if (it != demoMap.end()) {
                    options.mode = it->second;
                } else {
                    std::cerr << "Error: Unknown demo: " << demoName << std::endl;
                    std::cerr << "Use --list to see available demos." << std::endl;
                    return false;
                }
            }
            
            // Configuration file
            else if (arg.substr(0, 12) == "--config=") {
                options.configFile = arg.substr(12);
            }
            
            // Output file
            else if (arg.substr(0, 11) == "--output=") {
                options.outputFile = arg.substr(11);
            }
            
            // Checkpoint path
            else if (arg.substr(0, 15) == "--checkpoint=") {
                options.checkpointPath = arg.substr(15);
            }
            
            // Logging options
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            }
            else if (arg == "--progress") {
                options.progress = true;
            }
            else if (arg.substr(0, 16) == "--log-level=") {
                options.logLevel = arg.substr(16);
            }
            else if (arg == "--no-colors") {
                options.enableColors = false;
            }
            
            // Performance and profiling
            else if (arg.substr(0, 16) == "--profile=") {
                options.profileOutput = arg.substr(16);
            }
            else if (arg == "--continuous") {
                options.runContinuous = true;
            }
            else if (arg.substr(0, 12) == "--runtime=") {
                options.runTime = static_cast<uint64_t>(std::stoul(arg.substr(12)));
            }
            
            // Simulation parameters
            else if (arg.substr(0, 12) == "--steps=") {
                options.steps = static_cast<uint64_t>(std::stoul(arg.substr(12)));
            }
            else if (arg.substr(0, 17) == "--neuron-count=") {
                options.neuronCount = static_cast<size_t>(std::stoul(arg.substr(17)));
            }
            else if (arg.substr(0, 17) == "--region-count=") {
                options.regionCount = static_cast<size_t>(std::stoul(arg.substr(17)));
            }
            else if (arg.substr(0, 23) == "--connection-prob=") {
                options.connectionProbability = std::stof(arg.substr(23));
            }
            
            // Advanced features
            else if (arg == "--advanced") {
                options.enableAdvanced = true;
            }
            else if (arg == "--debug") {
                options.verbose = true;
                options.progress = true;
                options.logLevel = "debug";
            }
            
            // Custom arguments for advanced users
            else if (arg.substr(0, 20) == "--custom-arg=") {
                options.customArgs.push_back(arg.substr(20));
            }
            else if (arg.substr(0, 6) == "--arg=") {
                options.customArgs.push_back(arg.substr(6));
            }
            
            // Legacy support for old command-line format
            else if (arg.find('=') != std::string::npos && arg.find(' ') == std::string::npos) {
                // Try to parse as key=value
                size_t equalsPos = arg.find('=');
                std::string key = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                
                if (key == "steps") {
                    options.steps = static_cast<uint64_t>(std::stoul(value));
                } else if (key == "neurons") {
                    options.neuronCount = static_cast<size_t>(std::stoul(value));
                } else if (key == "regions") {
                    options.regionCount = static_cast<size_t>(std::stoul(value));
                } else if (key == "connprob") {
                    options.connectionProbability = std::stof(value);
                } else {
                    options.customArgs.push_back(arg);
                }
            } else {
                // Unknown argument
                options.customArgs.push_back(arg);
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing command-line arguments: " << e.what() << std::endl;
        return false;
    }
}

// Main demo runner function
int runDemoFramework(int argc, char** argv) {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       NLM Demo Framework                        ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  Enhanced Phase 2 Demo System with Unified Interface         ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    try {
        // Parse command-line arguments
        DemoOptions options;
        if (!parseEnhancedArguments(argc, argv, options)) {
            return 1; // Help/version was printed
        }
        
        // If no demo specified, show help
        if (options.mode == DemoMode::CUSTOM_CONFIG && options.customArgs.empty()) {
            std::cout << "Error: No demo specified." << std::endl;
            std::cout << "Use --demo=<name> to specify a demo or --help for details." << std::endl;
            return 1;
        }
        
        // Validate options
        if (!DemoRunner::validateOptions(options)) {
            std::cerr << "Error: Invalid demo options." << std::endl;
            return 1;
        }
        
        // Create and setup demo runner
        DemoRunner& demoRunner = getDemoRunner();
        
        // Setup logging
        demoRunner.setupLogging(options);
        
        // Show demo information
        std::cout << std::endl;
        std::cout << "Running demo: " << DemoRunner::getDemoName(options.mode) << std::endl;
        std::cout << "Description: " << DemoRunner::getDemoDescription(options.mode) << std::endl;
        std::cout << "Configuration: " << (options.configFile.empty() ? "default" : options.configFile) << std::endl;
        std::cout << "Steps: " << options.steps << std::endl;
        std::cout << "Neuron count: " << options.neuronCount << std::endl;
        std::cout << "Region count: " << options.regionCount << std::endl;
        std::cout << std::endl;
        
        // Parse configuration file if specified
        if (!options.configFile.empty()) {
            std::cout << "Loading configuration from: " << options.configFile << std::endl;
            if (!demoRunner.parseConfigFile(options.configFile, options)) {
                std::cerr << "Warning: Could not load configuration file, using defaults" << std::endl;
            }
        }
        
        // Parse demo-specific arguments
        if (!options.customArgs.empty()) {
            if (!demoRunner.parseDemoArguments(options.customArgs, options)) {
                std::cerr << "Warning: Could not parse all demo arguments" << std::endl;
            }
        }
        
        // Run the demo
        std::cout << std::endl;
        std::cout << "Starting demo execution..." << std::endl;
        std::cout << std::endl;
        
        bool success = demoRunner.runDemo(options);
        
        // Save results if requested
        if (!options.outputFile.empty()) {
            std::string demoName = DemoRunner::getDemoName(options.mode);
            std::string timestamp = DemoRunner::getTimestamp();
            std::string resultsFile = options.outputFile;
            
            // Create directory if it doesn't exist
            std::filesystem::path outputPath(resultsFile);
            std::filesystem::path outputDir = outputPath.parent_path();
            if (!outputDir.empty() && !std::filesystem::exists(outputDir)) {
                std::filesystem::create_directories(outputDir);
            }
            
            std::cout << std::endl;
            std::cout << "Saving results to: " << resultsFile << std::endl;
            demoRunner.saveResults(resultsFile, "Demo: " + demoName + "\nTimestamp: " + timestamp + "\nSuccess: " + (success ? "YES" : "NO"));
        }
        
        // Show profile results if requested
        if (!options.profileOutput.empty()) {
            std::cout << std::endl;
            std::cout << "Performance profile saved to: " << options.profileOutput << std::endl;
        }
        
        // Display demo results
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      Demo Execution Complete                     ║" << std::endl;
        std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  Demo: " << std::setw(45) << std::left << DemoRunner::getDemoName(options.mode) << "║" << std::endl;
        std::cout << "║  Status: " << std::setw(43) << std::left << (success ? "SUCCESS" : "FAILED") << "║" << std::endl;
        std::cout << "║  Duration: " << std::setw(42) << std::left << "Recorded in profile" << "║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error during demo execution: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred during demo execution" << std::endl;
        return 1;
    }
}

// Global convenience functions
namespace demos {
    
    // Run demo by name
    bool runDemoByName(const std::string& demoName) {
        DemoOptions options;
        
        static const std::map<std::string, DemoMode> demoMap = {
            {"basic", DemoMode::BASIC_CONNECTIVITY},
            {"plasticity", DemoMode::PLASTICITY_LEARNING},
            {"stdp", DemoMode::STDP_VERIFICATION},
            {"phase6", DemoMode::PHASE6_INTEGRATION},
            {"text", DemoMode::TEXT_DEMO},
            {"benchmark", DemoMode::BENCHMARK},
            {"all", DemoMode::ALL_TESTS},
            {"custom", DemoMode::CUSTOM_CONFIG}
        };
        
        auto it = demoMap.find(demoName);
        if (it == demoMap.end()) {
            std::cerr << "Unknown demo: " << demoName << std::endl;
            return false;
        }
        
        options.mode = it->second;
        return getDemoRunner().runDemo(options);
    }
    
    // Run demo with configuration
    bool runDemoWithConfig(const std::string& configFile, const std::string& demoName) {
        DemoOptions options;
        options.configFile = configFile;
        
        static const std::map<std::string, DemoMode> demoMap = {
            {"basic", DemoMode::BASIC_CONNECTIVITY},
            {"plasticity", DemoMode::PLASTICITY_LEARNING},
            {"stdp", DemoMode::STDP_VERIFICATION},
            {"phase6", DemoMode::PHASE6_INTEGRATION},
            {"text", DemoMode::TEXT_DEMO},
            {"benchmark", DemoMode::BENCHMARK},
            {"all", DemoMode::ALL_TESTS},
            {"custom", DemoMode::CUSTOM_CONFIG}
        };
        
        auto it = demoMap.find(demoName);
        if (it == demoMap.end()) {
            std::cerr << "Unknown demo: " << demoName << std::endl;
            return false;
        }
        
        options.mode = it->second;
        
        // Parse configuration file
        if (!getDemoRunner().parseConfigFile(configFile, options)) {
            std::cerr << "Warning: Could not load configuration file" << std::endl;
        }
        
        return getDemoRunner().runDemo(options);
    }
    
    // Run benchmark
    bool runBenchmark(const std::map<std::string, std::pair<size_t, size_t>>& benchmarks) {
        DemoOptions options;
        options.mode = DemoMode::BENCHMARK;
        
        return getDemoRunner().runBenchmarkDemo(options, benchmarks);
    }
    
} // namespace demos

// Main entry point
int main(int argc, char** argv) {
    return runDemoFramework(argc, argv);
}

} // namespace nlm
