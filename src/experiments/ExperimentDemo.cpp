#include "ExperimentFactoryRegistry.cpp"
#include "ExperimentCatalog.cpp"
#include "ExperimentEnhanced.cpp"
#include "ExperimentRunner.hpp"

// Initialize the experiment system
void initializeExperimentSystem() {
    // This will register all built-in experiment factories
    nlm::getExperimentCatalog();
    nlm::getExperimentRunner();
}

// Simple test application demonstrating the enhanced experiment system
int main(int argc, char** argv) {
    // Initialize experiment system
    initializeExperimentSystem();
    
    // Get the experiment runner
    auto& runner = nlm::getExperimentRunner();
    
    // Parse command line arguments
    auto args = runner.parseArgs(argc, argv);
    
    if (args.help) {
        runner.printHelp();
        return 0;
    }
    
    if (args.experimentType.empty()) {
        std::cerr << "Error: No experiment type specified. Use --help for usage." << std::endl;
        return 1;
    }
    
    // Check if experiment type is available
    if (!runner.getAvailableExperiments().empty()) {
        bool hasType = false;
        for (const auto& type : runner.getAvailableExperiments()) {
            if (type == args.experimentType) {
                hasType = true;
                break;
            }
        }
        
        if (!hasType) {
            std::cerr << "Error: Unknown experiment type: " << args.experimentType << std::endl;
            runner.printHelp();
            return 1;
        }
    }
    
    // Validate configuration
    if (!runner.validateConfig(args.experimentType, args.config)) {
        std::cerr << "Error: Invalid configuration for experiment: " << args.experimentType << std::endl;
        return 1;
    }
    
    // Run the experiment
    std::cout << "Running experiment: " << args.experimentType << std::endl;
    if (!args.experimentName.empty()) {
        std::cout << "Experiment name: " << args.experimentName << std::endl;
    }
    
    if (args.verbose) {
        std::cout << "Experiment configuration:" << std::endl;
        for (const auto& pair : args.config) {
            std::cout << "  " << pair.first << " = " << nlm::ExperimentRunner::parameterToString(pair.second) << std::endl;
        }
    }
    
    ExperimentResult result = runner.runExperiment(args.experimentType, args.experimentName, args.config);
    
    if (result.success) {
        std::cout << "Experiment completed successfully!" << std::endl;
        std::cout << "Experiment name: " << result.experimentName << std::endl;
        std::cout << "Experiment type: " << result.experimentType << std::endl;
        std::cout << "Execution time: " << result.executionTime << " seconds" << std::endl;
        std::cout << "Metrics recorded: " << result.experiment->getMetricCount() << std::endl;
        
        if (!result.notes.empty()) {
            std::cout << "Notes:" << std::endl;
            for (const auto& note : result.notes) {
                std::cout << "  - " << note << std::endl;
            }
        }
        
        // Export results if requested
        if (!args.outputFile.empty()) {
            ExportFormat format = ExportFormat::CSV; // Simplified - could be made more sophisticated
            if (!result.exportToFile(args.outputFile, format)) {
                std::cerr << "Warning: Failed to export results to " << args.outputFile << std::endl;
            }
        }
        
        return 0;
    } else {
        std::cerr << "Experiment failed: " << result.errorMessage << std::endl;
        return 1;
    }
}
