/**
 * Phase 6 Demo - Integration Test (Advanced)
 *
 * This demo runs the Phase 6 integration experiment with advanced features:
 * - Multiple demo modes (Quick, Thorough, Benchmark, Comparison)
 * - Performance benchmarking
 * - Detailed metrics collection
 * - Export capabilities
 * - Progress tracking
 * - Comparison testing
 *
 * Usage:
 *   nlm_phase6_demo_advanced [options]
 *
 * Options are documented in the help message.
 */

#include "experiments/Phase6DemoSystem.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo (Advanced) ===\n";
    std::cout << "Testing the integrated artificial brain with advanced features...\n\n";
    
    try {
        // Create advanced demo system
        Phase6DemoSystem demo;
        
        // Parse and run with command line arguments
        DemoBase::DemoConfig baseConfig;
        
        // Note: In a real implementation, we would pass argc and argv to parseAdvancedArgs
        // For now, we'll simulate command line parsing
        AdvancedConfig config;
        
        // Parse command line arguments
        config = demo.parseAdvancedArgs(argc, argv);
        
        // Setup logging based on configuration
        demo.setupLogging(config);
        
        std::cout << "=== Starting Phase 6 Advanced Demo ===\n";
        std::cout << "Demo Mode: " << config.demoMode << "\n";
        std::cout << "Neurons: " << config.neuronCount << "\n";
        std::cout << "Steps: " << config.maxSteps << "\n";
        std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << "\n";
        std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << "\n";
        std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << "\n";
        std::cout << "\n";
        
        // Run the demo
        AdvancedResults results = demo.run(config);
        
        // Print results
        demo.printResults(results);
        
        // Export results if requested
        if (config.exportResults && !config.outputFile.empty()) {
            demo.exportResults(results.baseResults, config.outputFile);
            std::cout << "\nResults exported to: " << config.outputFile << "\n";
        }
        
        if (results.baseResults.success) {
            std::cout << "\n=== DEMONSTRATION COMPLETED SUCCESSFULLY ===\n";
        } else {
            std::cout << "\n=== DEMONSTRATION FAILED ===\n";
            if (!results.baseResults.errorMessage.empty()) {
                std::cout << "Error: " << results.baseResults.errorMessage << "\n";
            }
        }
        
        return results.baseResults.success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}