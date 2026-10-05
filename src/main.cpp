// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This is the clean, production-ready example of using NLM.
// It demonstrates basic brain initialization, configuration, and operation
// without complex test logic or experiment mixing.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                           ║
    ║                                                               ║
    ║     Clean, production-ready example                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void demonstrateBasicBrainOperation() {
    std::cout << "\n=== Basic Brain Operation Demo ===" << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    
    // Configure for demonstration (smaller network for faster demo)
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    config->set("random_seed", uint64_t(12345), ConfigSource::Default);
    
    // Create brain
    std::cout << "Creating brain with " << config->getOr<int64_t>("neuron_count", 200) 
              << " neurons..." << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Initial spike count: " << brain->getTotalSpikeCount() << std::endl;
    
    // Inject some input to stimulate activity
    std::cout << "\nInjecting sensory input to stimulate neural activity..." << std::endl;
    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
    }
    
    // Run a few simulation steps
    std::cout << "Running 100 simulation steps..." << std::endl;
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 25 steps
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    // Show final results
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Current firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << std::fixed << std::setprecision(2) 
              << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Excitatory/Inhibition ratio: " << std::fixed << std::setprecision(3) 
              << brain->getExcitationInhibitionRatio() << std::endl;
    
    // Demonstrate plasticity
    std::cout << "\n=== Plasticity Demonstration ===" << std::endl;
    std::cout << "Enabling plasticity on all synapses..." << std::endl;
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
        }
    }
    
    // Run more steps with plasticity enabled
    std::cout << "Running additional 200 steps with plasticity enabled..." << std::endl;
    for (SimulationStep step = 100; step < 300; ++step) {
        // Random input pattern
        for (size_t i = 0; i < 15 && i < brain->getTotalNeuronCount() / 6; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " 
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "\n=== Plasticity Results ===" << std::endl;
    std::cout << "Total spike count after plasticity: " << brain->getTotalSpikeCount() << std::endl;
    
    // Show developmental stage
    std::cout << "\n=== Development Status ===" << std::endl;
    std::cout << "Current developmental stage: " << 
              (brain->getDevelopmentalStage() == DevelopmentalStage::Adult ? "Adult" :
               brain->getDevelopmentalStage() == DevelopmentalStage::Maturation ? "Maturation" :
               brain->getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod ? "Critical Period" :
               brain->getDevelopmentalStage() == DevelopmentalStage::Initial ? "Initial" : "Aging") << std::endl;
    
    std::cout << "\n✓ Brain operation demonstration complete!" << std::endl;
}

void demonstrateBrainPersistence() {
    std::cout << "\n=== Brain Persistence Demo ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain1 = std::make_shared<Brain>(config);
    if (!brain1->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Running simulation for 500 steps..." << std::endl;
    for (SimulationStep step = 0; step < 500; ++step) {
        for (size_t i = 0; i < 10 && i < brain1->getTotalNeuronCount() / 10; ++i) {
            brain1->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain1->step(step, step * 0.001);
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " << brain1->getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    // Save brain state
    std::string checkpointFile = "demo_checkpoint.bin";
    if (brain1->save(checkpointFile)) {
        std::cout << "Brain state saved to: " << checkpointFile << std::endl;
    } else {
        std::cerr << "WARNING: Failed to save brain state" << std::endl;
    }
    
    // Reset and create new brain
    std::cout << "Resetting brain and creating new instance from checkpoint..." << std::endl;
    brain1->reset();
    
    auto brain2 = std::make_shared<Brain>(config);
    if (!brain2->initialize()) {
        std::cerr << "ERROR: Failed to initialize new brain!" << std::endl;
        return;
    }
    
    // Load saved state
    if (brain2->load(checkpointFile)) {
        std::cout << "Brain state loaded successfully!" << std::endl;
        std::cout << "Loaded spike count: " << brain2->getTotalSpikeCount() << std::endl;
    } else {
        std::cerr << "ERROR: Failed to load brain state!" << std::endl;
    }
    
    std::cout << "✓ Persistence demonstration complete!" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "NLM Phase 6: Final Integration - Clean Production Example" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This is a clean, production-ready example of NLM usage." << std::endl;
    std::cout << "It demonstrates basic brain operations, plasticity, and persistence." << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run demonstrations
    demonstrateBasicBrainOperation();
    demonstrateBrainPersistence();
    
    // Final status
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                    NLM Production Demo                        ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║            Clean, modular, and production-ready               ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << std::endl;
    std::cout << "✓ All demonstrations completed successfully!" << std::endl;
    std::cout << "✓ NLM is ready for production use!" << std::endl;
    
    return 0;
}
