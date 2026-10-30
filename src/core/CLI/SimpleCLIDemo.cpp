// Simple demo application using the unified CLI framework

#include "core/CLI/CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace nlm;

class SimpleCLIDemo {
public:
    SimpleCLIDemo() = default;
    ~SimpleCLIDemo() = default;
    
    // Main demo function
    bool run(int argc, char* argv[]) {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create CLI instance
        UnifiedCLI cli;
        
        // Parse arguments
        if (!cli.parseArgs(argc, argv)) {
            std::cerr << "Error parsing command line arguments" << std::endl;
            return false;
        }
        
        // Handle help/version
        if (cli.getConfig().help) {
            cli.printHelp(argv[0]);
            return true;
        }
        
        if (cli.getConfig().version) {
            cli.printVersion();
            return true;
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Add common config options
        cli.addCommonConfigOptions(config);
        
        // Process configuration
        if (!cli.processConfig(config)) {
            std::cerr << "Error processing configuration" << std::endl;
            return false;
        }
        
        // Display configuration summary
        cli.printConfigSummary(config);
        
        // Create and initialize brain
        std::cout << "\nInitializing NLM brain..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        // Display brain status
        std::cout << "\nBrain initialized successfully:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        
        // Run simple test
        std::cout << "\nRunning connectivity test..." << std::endl;
        runConnectivityTest(brain.get());
        
        // Display final status
        std::cout << "\n=== Demo Complete ===" << std::endl;
        brain->logStatus();
        
        return true;
    }
    
private:
    void runConnectivityTest(Brain* brain) {
        // Simple connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "No regions found" << std::endl;
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "No neurons found" << std::endl;
            return;
        }
        
        // Record initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into some neurons
        std::cout << "Injecting current into " << std::min(size_t(10), neurons.size()) << " neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Generated " << spikes << " spikes" << std::endl;
        
        if (spikes > 0) {
            std::cout << "✓ Connectivity test passed: spikes propagated through network" << std::endl;
        } else {
            std::cout << "! No spikes generated - checking neuron parameters" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "  Neuron " << i << ": V=" << neurons[i]->getMembranePotential() 
                          << ", threshold=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    SimpleCLIDemo demo;
    return demo.run(argc, argv) ? 0 : 1;
}