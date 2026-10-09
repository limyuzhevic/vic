#include "cli/NLMCLI.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    try {
        // Initialize CLI system
        NLMCLI::initialize();
        
        // Parse command line arguments
        auto args = NLMCLI::parseArguments(argc, argv);
        
        // Get configuration
        auto config = NLMCLI::loadConfig(args.configFile);
        
        // Apply command line overrides
        NLMCLI::applyOverrides(config, args);
        
        // Set up logging based on verbosity
        NLMCLI::setupLogging(args.verbosity, args.logFile, args.quiet);
        
        std::cout << "NLM — 熙然" << std::endl;
        std::cout << "Phase 6: Integrated Artificial Brain" << std::endl << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run the selected mode
        switch (args.mode) {
            case CLI::Mode::DEMO:
                if (args.experimentName.empty()) {
                    NLMCLI::runBuiltInDemo(config);
                } else {
                    NLMCLI::runNamedExperiment(config, args.experimentName);
                }
                break;
                
            case CLI::Mode::EXPERIMENT:
                NLMCLI::runExperiment(config, args.experimentName);
                break;
                
            case CLI::Mode::BATCH:
                NLMCLI::runBatch(config, args.batchFile);
                break;
                
            case CLI::Mode::CHECKPOINT:
                NLMCLI::handleCheckpointCommand(args.checkpointCommand, config, args.checkpointPath);
                break;
                
            case CLI::Mode::VISUALIZE:
                NLMCLI::runVisualization(args.vizType, config);
                break;
                
            case CLI::Mode::TEST:
                NLMCLI::runSystemTests(args.testSuite);
                break;
                
            case CLI::Mode::CONFIG:
                NLMCLI::generateConfigTemplate(args.configTemplate);
                break;
                
            default:
                NLMCLI::printHelp();
                return 1;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << std::endl << "Execution completed in " << duration.count() << " seconds." << std::endl;
        
        // Save final checkpoint if requested
        if (args.autoSaveCheckpoint) {
            std::string checkpointPath = "./nlm_final_checkpoint.bin";
            NLMCLI::saveCheckpoint(config, checkpointPath);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}