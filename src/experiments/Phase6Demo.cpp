/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "nlm/CLIFramework.hpp"

#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments using the unified CLI framework
        NLMCliOptions options = nlm::parseCommandLine(argc, argv);
        
        // Handle help/version requests
        if (options.showHelp) {
            nlm::printHelp("nlm_phase6_demo");
            return 0;
        }
        
        if (options.showVersion) {
            nlm::printVersion();
            return 0;
        }
        
        // Validate options
        nlm::validateOptions(options);
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(
            options.debug ? LogLevel::Debug : 
            options.logLevel == "DEBUG" ? LogLevel::Debug :
            options.logLevel == "WARN" ? LogLevel::Warning :
            options.logLevel == "ERROR" ? LogLevel::Error : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Log CLI options for debugging
        nlm::logOptions(options);
        
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment with configuration from CLI options
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = static_cast<size_t>(options.neuronCount);
        config.maxSteps = options.demoSteps;
        config.enableCheckpointing = options.enableCheckpointing;
        config.enableReplay = options.enableReplay;
        config.enableDevelopment = options.enableDevelopment;
        
        // Override with CLI options if provided
        if (options.maxSteps > 0) {
            config.maxSteps = static_cast<size_t>(options.maxSteps);
        }
        if (!options.checkpointPath.empty()) {
            config.checkpointPath = options.checkpointPath;
        }
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        return 0;
        
    } catch (const CLIException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}