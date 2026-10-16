// Improved main.cpp with advanced CLI support
// Replaces the original monolithic main.cpp with better organization

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "advanced_cli.cpp"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    std::cout << "NLM Advanced Main CLI" << std::endl;
    std::cout << "======================\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::ArgumentParser parser;
    
    // Add arguments
    parser.addArgument("config", "Configuration file path", "configs/default.cfg");
    parser.addArgument("mode", "Operation mode: demo, test, or benchmark", "demo");
    parser.addArgument("neuron-count", "Number of neurons", "500");
    parser.addArgument("region-count", "Number of brain regions", "1");
    parser.addArgument("episodes", "Number of demo episodes", "5");
    parser.addArgument("steps", "Number of steps per demo", "200");
    
    // Parse arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    // Load configuration
    auto config = nlm::EnhancedConfigManager::loadConfig(parser.getValue("config"), parser);
    
    // Apply command line overrides
    nlm::EnhancedConfigManager::applyCommandLineOverrides(config, parser);
    
    // Print configuration summary
    nlm::EnhancedConfigManager::printConfigurationSummary(*config);
    
    // Initialize brain
    std::cout << "\nInitializing NLM Brain..." << std::endl;
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run based on mode
    std::string mode = parser.getValue("mode");
    
    if (mode == "demo") {
        std::cout << "\n=== NLM Demo Mode ===" << std::endl;
        
        // Use the advanced Phase 3 demo implementation
        std::string demoConfigFile = parser.getValue("config");
        int numEpisodes = std::stoi(parser.getValue("episodes"));
        int stepsPerEpisode = std::stoi(parser.getValue("steps"));
        
        // For demo purposes, we'll use the existing demo structure
        // but with enhanced CLI argument support
        std::cout << "Running Phase 3 demo with:" << std::endl;
        std::cout << "  Episodes: " << numEpisodes << std::endl;
        std::cout << "  Steps per episode: " << stepsPerEpisode << std::endl;
        std::cout << "  Config: " << demoConfigFile << std::endl;
        
        // Run a simplified version of the demo
        brain->reset();
        
        // Simulate world interaction loop
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "\n--- Episode " << episode << " ---" << std::endl;
            
            // Simulate some neural activity
            for (int step = 0; step < stepsPerEpisode; ++step) {
                brain->step(step);
                
                // Log progress periodically
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << " | Spikes: " << brain->getTotalSpikeCount()
                              << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
                }
            }
            
            // Show episode summary
            std::cout << "  Episode " << episode << " complete:" << std::endl;
            std::cout << "    Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "    Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        }
        
    } else if (mode == "test") {
        std::cout << "\n=== NLM Test Mode ===" << std::endl;
        
        // Run the same comprehensive tests as the original main.cpp
        // but using the improved test runner
        nlm::TestRunner::runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        nlm::TestRunner::runLearningExperiment(brain, "Plasticity Learning Experiment");
        
        brain->reset();
        brain->initialize();
        nlm::TestRunner::runStdpVerification(brain);
        
    } else if (mode == "benchmark") {
        std::cout << "\n=== NLM Benchmark Mode ===" << std::endl;
        
        // Simple benchmark of brain performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        int steps = std::stoi(parser.getValue("steps"));
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Benchmark completed:" << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Steps per second: " << (steps * 1000.0 / duration.count()) << std::endl;
        
    } else {
        std::cerr << "Error: Unknown mode '" << mode << "'. Use 'demo', 'test', or 'benchmark'." << std::endl;
        return 1;
    }
    
    // Final brain status
    std::cout << "\n=== Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << "\n=== Improved Main Complete ===" << std::endl;
    std::cout << "Enhanced features demonstrated:" << std::endl;
    std::cout << "  ✓ Advanced argument parsing" << std::endl;
    std::cout << "  ✓ Enhanced configuration management" << std::endl;
    std::cout << "  ✓ Modular test execution" << std::endl;
    std::cout << "  ✓ Multiple operation modes" << std::endl;
    
    return 0;
}
