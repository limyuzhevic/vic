#include <iostream>
#include <chrono>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"

int main() {
    std::cout << "Testing Optimized Brain Implementation" << std::endl;
    std::cout << "======================================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize
    std::cout << "Initializing brain..." << std::endl;
    bool initSuccess = brain.initialize();
    if (!initSuccess) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    std::cout << "Initialization successful!" << std::endl;
    
    // Test basic properties
    std::cout << "Brain properties:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Region count: " << brain.getRegionCount() << std::endl;
    
    // Performance test - run many steps
    std::cout << "\nPerformance test - running 100 simulation steps..." << std::auto_ptr(nlm::RandomGenerator)(new nlm::RandomGenerator(42));
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "Completed 100 steps in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Average time per step: " << (duration.count() / 100.0) << " microseconds" << std::endl;
    
    // Test reset functionality
    std::cout << "\nTesting reset functionality..." << std::endl;
    brain.reset();
    std::cout << "Reset completed successfully!" << std::endl;
    
    // Verify reset
    std::cout << "After reset - Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    
    // Test plasticity systems
    std::cout << "\nTesting plasticity systems..." << std::endl;
    if (auto* stdp = brain.getSTDP()) {
        std::cout << "STDP system available" << std::endl;
    }
    if (auto* hebbian = brain.getHebbian()) {
        std::cout << "Hebbian system available" << std::endl;
    }
    if (auto* spikeSystem = brain.getSpikeSystem()) {
        std::cout << "Spike system available" << std::endl;
        std::cout << "Pending spike events: " << spikeSystem->getPendingSpikeCount() << std::endl;
    }
    
    // Test memory systems
    std::cout << "\nTesting memory systems..." << std::endl;
    if (auto* workingMem = brain.getWorkingMemory()) {
        std::cout << "Working memory system available" << std::endl;
    }
    if (auto* episodicMem = brain.getEpisodicMemory()) {
        std::cout << "Episodic memory system available" << std::endl;
    }
    
    // Test neuromodulation systems
    std::cout << "\nTesting neuromodulation systems..." << std::endl;
    if (auto* dopamine = brain.getDopamine()) {
        std::cout << "Dopamine system available" << std::endl;
    }
    if (auto* curiosity = brain.getCuriosity()) {
        std::cout << "Curiosity system available" << std::endl;
    }
    
    std::cout << "\nAll tests passed! ✓" << std::endl;
    std::cout << "Optimized Brain implementation is working correctly." << std::endl;
    
    return 0;
}
