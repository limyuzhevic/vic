// Example usage of advanced user interface utilities
// Demonstrates the use of Enhanced CLI Parser, Output Formatter, and Progress Reporter

#include "AdvancedUserInterface.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void example_cli_parser() {
    std::cout << "=== CLI Parser Example ===" << std::endl;
    
    // Create help options
    auto helpOptions = AdvancedCLIParser::generateHelpOptions(
        "nlm_phase3_demo",
        "1.0.0",
        "Phase 3 world interaction demonstration with advanced features",
        "nlm_phase3_demo [OPTIONS] [EPISODES] [STEPS]"
    );
    
    // Create parser
    AdvancedCLIParser parser;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"program_name",
        (char*)"--neuron_count=2000",
        (char*)"--enable_development",
        (char*)"--help",
        (char*)NULL
    };
    
    // Parse arguments
    auto result = parser.parseArgs(argc, argv, helpOptions);
    
    std::cout << "Configuration loaded:" << std::endl;
    for (const auto& pair : result.options) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    if (!result.configFile.empty()) {
        std::cout << "Config file: " << result.configFile << std::endl;
    }
    
    std::cout << "Positional arguments: " << result.positionalArgs.size() << std::endl;
    std::cout << "Help requested: " << (result.helpRequested ? "Yes" : "No") << std::endl;
    std::cout << "Verbose mode: " << (result.verboseMode ? "Yes" : "No") << std::endl;
}

void example_output_formatter() {
    std::cout << "\n=== Output Formatter Example ===" << std::endl;
    
    // Configure formatter options
    OutputFormatter::FormatOptions options;
    options.jsonOutput = false;
    options.verboseOutput = true;
    options.includeTimestamps = true;
    options.includeMemoryUsage = true;
    options.includePerformanceStats = true;
    
    // Sample configuration
    std::map<std::string, std::string> config = {
        {"neuron_count", "1000"},
        {"region_count", "1"},
        {"connection_probability", "0.1"},
        {"enable_development", "true"},
        {"enable_curiosity", "true"}
    };
    
    OutputFormatter formatter;
    
    // Format configuration
    std::string formattedConfig = formatter.formatConfiguration(config, options);
    std::cout << "Formatted Configuration:" << std::endl;
    std::cout << formattedConfig;
    
    // Sample results
    std::map<std::string, double> results = {
        {"total_reward", 125.5},
        {"average_firing_rate", 8.3},
        {"memory_usage", 512.0},
        {"processing_time", 45.2}
    };
    
    // Format results
    std::string formattedResults = formatter.formatResults(results, options);
    std::cout << "\nFormatted Results:" << std::endl;
    std::cout << formattedResults;
    
    // Sample performance metrics
    std::map<std::string, double> metrics = {
        {"cpu_time", 12.5},
        {"memory_time", 8.2},
        {"io_time", 2.1},
        {"total_time", 22.8}
    };
    
    // Format performance
    std::string formattedMetrics = formatter.formatPerformance(metrics, options);
    std::cout << "\nFormatted Performance Metrics:" << std::endl;
    std::cout << formattedMetrics;
}

void example_progress_reporter() {
    std::cout << "\n=== Progress Reporter Example ===" << std::endl;
    
    // Configure progress reporter options
    ProgressReporter::ProgressOptions options;
    options.showProgress = true;
    options.showEstimatedTime = true;
    options.showCompletionPercentage = true;
    options.completionMessage = "Experiment completed successfully!";
    
    ProgressReporter reporter;
    reporter.setTotalSteps(100);
    
    // Simulate progress
    for (int i = 0; i <= 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Simulate work
        reporter.reportProgress(i, 100, "Processing episode " + std::to_string(i), options);
    }
    
    reporter.reportCompletion("All episodes completed!", options);
}

void example_advanced_cli_options() {
    std::cout << "\n=== Advanced CLI Options Example ===" << std::endl;
    
    // Create advanced help options
    auto helpOptions = AdvancedCLIParser::generateHelpOptions(
        "nlm_phase3_demo",
        "1.0.0",
        "Phase 3 world interaction demonstration with advanced features",
        "nlm_phase3_demo [OPTIONS] [EPISODES] [STEPS]"
    );
    
    std::cout << "Help options created successfully:" << std::endl;
    std::cout << "  Program name: " << helpOptions.programName << std::endl;
    std::cout << "  Version: " << helpOptions.version << std::endl;
    std::cout << "  Description: " << helpOptions.description << std::endl;
    std::cout << "  Usage: " << helpOptions.usage << std::endl;
    std::cout << "  Option descriptions: " << helpOptions.optionDescriptions.size() << std::endl;
    std::cout << "  Example commands: " << helpOptions.exampleCommands.size() << std::endl;
    std::cout << "  Advanced options: " << helpOptions.advancedOptions.size() << std::endl;
}

void main_example() {
    std::cout << "NLM Advanced User Interface Examples" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    example_cli_parser();
    example_output_formatter();
    example_progress_reporter();
    example_advanced_cli_options();
    
    std::cout << std::endl;
    std::cout << "All examples completed successfully!" << std::endl;
}

// For use in main function:
// int main(int argc, char* argv[]) {
//     main_example();
//     return 0;
// }