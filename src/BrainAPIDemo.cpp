# BrainAPI Demo - Demonstrating the New Simplified NLM Interface

/**
 * BrainAPI Demo - Demonstrating the new BrainAPI for NLM
 * 
 * This demo shows how the new BrainAPI provides a simplified interface
 * to the complex NLM brain system, making it easier to use while providing
 * access to advanced features.
 */

#include "BrainAPI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM BrainAPI Demo - Simplified Interface                   ║
    ║                                                               ║
    ║     Easy-to-use brain simulation with advanced features        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void demoBasicUsage() {
    std::cout << "\n=== Basic Usage Demo ===" << std::endl;
    
    // Create brain with default configuration
    auto brain = BrainAPI::createDefault();
    
    // Initialize the brain
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Get brain statistics
    auto stats = brain->getStats();
    std::cout << "Stats: " << stats.totalNeurons << " neurons, " 
              << stats.totalSynapses << " synapses, "
              << "avg firing rate: " << stats.avgFiringRate << std::endl;
    
    // Step the brain
    std::cout << "Running simulation for 100 steps..." << std::endl;
    brain->step(100);
    
    // Get updated stats
    stats = brain->getStats();
    std::cout << "After 100 steps: " << stats.totalSpikes << " spikes, "
              << "firing rate: " << stats.avgFiringRate << std::endl;
    
    std::cout << "Basic usage demo completed successfully!" << std::endl;
}

void demoConfigurationProfiles() {
    std::cout << "\n=== Configuration Profiles Demo ===" << std::endl;
    
    // Get available profiles
    auto profiles = BrainAPI::getAvailableProfiles();
    std::cout << "Available profiles: ";
    for (size_t i = 0; i < profiles.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << profiles[i];
    }
    std::cout << std::endl;
    
    // Load research profile
    std::cout << "Loading research profile..." << std::endl;
    auto brain = BrainAPI::createDefault();
    
    if (!brain->loadProfile("research")) {
        std::cerr << "ERROR: Failed to load research profile" << std::endl;
        return;
    }
    
    std::cout << "Research profile loaded successfully!" << std::endl;
    
    // Check enabled features
    std::cout << "Enabled features in research profile:" << std::endl;
    std::cout << "  Reward modulation: " << (brain->isFeatureEnabled("reward_modulation") ? "yes" : "no") << std::endl;
    std::cout << "  Structural plasticity: " << (brain->isFeatureEnabled("structural_plasticity") ? "yes" : "no") << std::endl;
    std::cout << "  Development enabled: " << (brain->isFeatureEnabled("development") ? "yes" : "no") << std::endl;
    
    // Initialize and test
    if (brain->initialize()) {
        std::cout << "Research profile brain initialized!" << std::endl;
        brain->step(50);
        std::cout << "Research profile simulation completed!" << std::endl;
    }
    
    std::cout << "Configuration profiles demo completed successfully!" << std::endl;
}

void demoFeatureManagement() {
    std::cout << "\n=== Feature Management Demo ===" << std::endl;
    
    // Create brain
    auto brain = BrainAPI::createDefault();
    
    // Initial state
    std::cout << "Initial feature states:" << std::endl;
    std::cout << "  Reward modulation: " << (brain->isFeatureEnabled("reward_modulation") ? "yes" : "no") << std::endl;
    std::cout << "  Structural plasticity: " << (brain->isFeatureEnabled("structural_plasticity") ? "yes" : "no") << std::endl;
    std::cout << "  Development: " << (brain->isFeatureEnabled("development") ? "yes" : "no") << std::endl;
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    // Enable features
    std::cout << "\nEnabling additional features..." << std::endl;
    brain->enableFeature("reward_modulation", true);
    brain->enableFeature("structural_plasticity", true);
    brain->enableFeature("development", true);
    
    // Check again
    std::cout << "Feature states after enabling:" << std::endl;
    std::cout << "  Reward modulation: " << (brain->isFeatureEnabled("reward_modulation") ? "yes" : "no") << std::endl;
    std::cout << "  Structural plasticity: " << (brain->isFeatureEnabled("structural_plasticity") ? "yes" : "no") << std::endl;
    std::endl("  Development: " << (brain->isFeatureEnabled("development") ? "yes" : "no") << std::endl;
    
    // Run simulation
    std::cout << "\nRunning simulation with enabled features..." << std::endl;
    brain->step(200);
    
    // Get stats
    auto stats = brain->getStats();
    std::cout << "Simulation completed! Stats: " << stats.totalSpikes << " spikes, "
              << "dopamine: " << stats.dopamineLevel << std::endl;
    
    std::cout << "Feature management demo completed successfully!" << std::endl;
}

void demoFileOperations() {
    std::cout << "\n=== File Operations Demo ===" << std::endl;
    
    // Create and initialize brain
    auto brain = BrainAPI::createDefault();
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    // Save brain state
    std::string savePath = "demo_brain_save.bin";
    std::cout << "Saving brain state to: " << savePath << std::endl;
    if (brain->save(savePath)) {
        std::cout << "Save successful!" << std::endl;
    } else {
        std::cerr << "ERROR: Failed to save brain state" << std::endl;
        return;
    }
    
    // Reset brain
    std::cout << "Resetting brain..." << std::endl;
    brain->reset();
    
    // Get stats after reset
    auto stats = brain->getStats();
    std::cout << "After reset: " << stats.totalSpikes << " spikes (should be 0)" << std::endl;
    
    // Load brain state
    std::cout << "Loading brain state from: " << savePath << std::endl;
    if (brain->load(savePath)) {
        std::cout << "Load successful!" << std::endl;
    } else {
        std::cerr << "ERROR: Failed to load brain state" << std::endl;
        return;
    }
    
    // Get stats after load
    stats = brain->getStats();
    std::cout << "After load: " << stats.totalSpikes << " spikes" << std::endl;
    
    std::cout << "File operations demo completed successfully!" << std::endl;
}

void demoDevelopmentStages() {
    std::cout << "\n=== Development Stages Demo ===" << std::endl;
    
    // Create brain
    auto brain = BrainAPI::createDefault();
    
    // Show all development stages
    std::cout << "Available development stages:" << std::endl;
    std::cout << "  0: Initial" << std::endl;
    std::cout << "  1: CriticalPeriod" << std::endl;
    std::cout << "  2: Maturation" << std::endl;
    std::cout << "  3: Adult" << std::endl;
    std::cout << "  4: Aging" << std::endl;
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test development stages
    std::cout << "\nTesting development stages..." << std::endl;
    for (int stage = 0; stage <= 4; ++stage) {
        brain->setDevelopmentStage(static_cast<DevelopmentalStage>(stage));
        brain->reset();
        brain->initialize();
        
        std::cout << "Stage " << stage << ": ";
        brain->step(50);
        
        auto stats = brain->getStats();
        std::cout << "Firing: " << stats.totalSpikes << " spikes" << std::endl;
    }
    
    std::cout << "Development stages demo completed successfully!" << std::endl;
}

void demoCallbackSystem() {
    std::cout << "\n=== Callback System Demo ===" << std::endl;
    
    // Create brain
    auto brain = BrainAPI::createDefault();
    
    // Track steps and spikes
    int stepCount = 0;
    int spikeCount = 0;
    
    // Set up step callback
    brain->setStepCallback([&stepCount](int step) {
        stepCount++;
        if (stepCount % 50 == 0) {
            std::cout << "Step callback: Reached step " << step << std::endl;
        }
    });
    
    // Set up spike callback
    brain->setSpikeCallback([&spikeCount](const DetailedSpikeEvent& event) {
        spikeCount++;
        if (spikeCount % 10 == 0) {
            std::cout << "Spike callback: Spike " << spikeCount << " from neuron " 
                      << event.source_neuron << std::endl;
        }
    });
    
    // Initialize and run
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Running simulation with callbacks..." << std::endl;
    brain->step(200);
    
    std::cout << "Simulation completed! " << stepCount << " steps, " 
              << spikeCount << " spikes" << std::endl;
    
    std::cout << "Callback system demo completed successfully!" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    std::cout << "=== NLM BrainAPI Demo ===" << std::endl;
    std::cout << "Demonstrating the new simplified BrainAPI interface" << std::endl;
    
    // Run demos
    demoBasicUsage();
    demoConfigurationProfiles();
    demoFeatureManagement();
    demoFileOperations();
    demoDevelopmentStages();
    demoCallbackSystem();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Final status
    std::cout << "\n=== Demo Summary ===" << std::endl;
    std::cout << "All demos completed successfully!" << std::endl;
    std::cout << "The BrainAPI provides:" << std::endl;
    std::cout << "  - Simplified brain creation and initialization" << std::endl;
    std::cout << "  - Easy-to-use configuration profiles" << std::endl;
    std::cout << "  - Feature management and monitoring" << std::endl;
    std::cout << "  - File save/load operations" << std::endl;
    std::cout << "  - Development stage control" << std::endl;
    std::cout << "  - Callback system for event handling" << std::endl;
    
    std::cout << "\nBrainAPI demo completed successfully!" << std::endl;
    
    return 0;
}
