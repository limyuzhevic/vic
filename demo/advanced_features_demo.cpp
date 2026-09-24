// NLM Advanced Features Demo
// This example demonstrates the new advanced features added to the NLM (熙然) neural simulation framework.
// It showcases the enhanced API, event handlers, performance profiling, and simulation control features.

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>

// Demo event handlers that showcase the new callback system
void demoSpikeHandler(const SpikeEvent& event) {
    // Custom spike handler that logs to file and monitors for burst activity
    static std::unordered_map<NeuronId, int> spikeCounts;
    spikeCounts[event.neuronId]++;
    
    if (spikeCounts[event.neuronId] % 1000 == 0) {
        std::cout << "[Spike Handler] Neuron " << event.neuronId.index() 
                  << " has spiked " << spikeCounts[event.neuronId] << " times" << std::endl;
    }
}

void demoMemoryUpdatedHandler(const std::vector<NeuronId>& neurons, float totalActivity) {
    // Custom memory handler that detects memory consolidation events
    if (totalActivity > 0.7f) {
        std::cout << "[Memory Handler] High memory activity detected: " 
                  << neurons.size() << " neurons, " << totalActivity << " activity" << std::endl;
    }
}

void demoNeuromodulatorUpdatedHandler(const std::string& neuromodulator, 
                                       float level, float plasticityFactor) {
    // Custom neuromodulator handler that logs significant changes
    if (std::abs(level - 0.5f) > 0.3f || std::abs(plasticityFactor - 1.0f) > 0.2f) {
        std::cout << "[Neuromodulator Handler] " << neuromodulator 
                  << " changed: level=" << level 
                  << " plasticity_factor=" << plasticityFactor << std::endl;
    }
}

// Performance benchmark function
void benchmarkPerformance(nlm::Brain& brain, int steps) {
    std::cout << "\n=== Performance Benchmark ===" << std::endl;
    
    // Enable performance profiling
    brain.enablePerformanceProfiling();
    
    // Run benchmark
    auto startTime = std::chrono::steady_clock::now();
    
    for (int i = 0; i < steps; ++i) {
        brain.step(i, i * 0.001);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = steps / (duration.count() / 1000.0);
    
    std::cout << "Benchmark completed in " << duration.count() << " ms" << std::endl;
    std::cout << "Performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    // Get performance statistics
    auto performanceStats = brain.getPerformanceStats();
    std::cout << "Performance details:" << std::endl << performanceStats;
}

// Memory management demonstration
void demonstrateMemoryManagement(nlm::Brain& brain) {
    std::cout << "\n=== Memory Management Demo ===" << std::endl;
    
    // Set memory limits
    brain.setMemoryLimit(10 * 1024 * 1024);  // 10MB limit
    brain.enableDebugMode();
    brain.setDebugLevel(nlm::DebugLevel::Basic);
    
    std::cout << "Memory limit set to 10MB" << std::endl;
    std::cout << "Debug mode enabled" << std::endl;
    
    // Run simulation to generate memory activity
    for (int i = 0; i < 100; ++i) {
        brain.step(i, i * 0.001);
        
        if (i % 20 == 0) {
            std::cout << "Step " << i << ": Active neurons = " << brain.getActiveNeuronCount() << std::endl;
        }
    }
}

// Simulation control demonstration
void demonstrateSimulationControl(nlm::Brain& brain) {
    std::cout << "\n=== Simulation Control Demo ===" << std::endl;
    
    // Test pause/resume functionality
    std::cout << "Initial state: " << (brain.isRunning() ? "Running" : "Paused") << std::endl;
    
    brain.pause();
    std::cout << "After pause: " << (brain.isRunning() ? "Running" : "Paused") << std::endl;
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    brain.resume();
    std::cout << "After resume: " << (brain.isRunning() ? "Running" : "Paused") << std::endl;
    
    // Check current state
    std::cout << "Current step: " << brain.getCurrentStep() << std::endl;
    std::cout << "Current time: " << brain.getCurrentTime() << " ms" << std::endl;
    std::cout << "Current timestep: " << brain.getCurrentTimestep() << " ms" << std::endl;
}

// Event handler demonstration
void demonstrateEventHandlers(nlm::Brain& brain) {
    std::cout << "\n=== Event Handlers Demo ===" << std::endl;
    
    // Clear any existing handlers
    // Note: In the actual implementation, we would need to clear handlers
    
    // Register demo event handlers
    brain.registerSpikeHandler(demoSpikeHandler);
    brain.registerMemoryUpdatedHandler(demoMemoryUpdatedHandler);
    brain.registerNeuromodulatorUpdatedHandler(demoNeuromodulatorUpdatedHandler);
    
    std::cout << "Event handlers registered:" << std::endl;
    std::cout << "  - Spike handler (logs neuron activity)" << std::endl;
    std::cout << "  - Memory handler (detects high activity)" << std::endl;
    std::cout << "  - Neuromodulator handler (logs significant changes)" << std::endl;
    
    // Run simulation to trigger events
    std::cout << "Running simulation to trigger events..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001);
        
        if (i % 10 == 0) {
            std::cout << "Step " << i << ": " << brain.getActiveNeuronCount() << " active neurons" << std::endl;
        }
    }
    
    std::cout << "Event handler demonstration completed." << std::endl;
}

// Configuration validation demonstration
void demonstrateConfigurationValidation(nlm::Brain& brain) {
    std::cout << "\n=== Configuration Validation Demo ===" << std::endl;
    
    bool isValid = brain.validateConfiguration();
    std::cout << "Configuration validation result: " << (isValid ? "Valid" : "Invalid") << std::endl;
    
    if (!isValid) {
        std::cout << "Note: Invalid configurations will be replaced with defaults" << std::endl;
    }
    
    // Show current configuration
    auto config = brain.getConfig();
    if (config) {
        std::cout << "Current configuration:" << std::endl;
        std::cout << "  Neurons: " << config->getOr<size_t>("neuron_count", 0) << std::endl;
        std::cout << "  Regions: " << config->getOr<size_t>("region_count", 0) << std::endl;
        std::cout << "  Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << " s" << std::endl;
        std::cout << "  Connection probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
    }
}

// Checkpoint demonstration
void demonstrateCheckpointSystem(nlm::Brain& brain) {
    std::cout << "\n=== Checkpoint System Demo ===" << std::endl;
    
    // Create checkpoint with name
    bool checkpointCreated = brain.saveCheckpoint("demo_checkpoint");
    std::cout << "Checkpoint created: " << (checkpointCreated ? "Yes" : "No") << std::endl;
    
    if (checkpointCreated) {
        std::cout << "Checkpoint saved as: demo_checkpoint" << std::endl;
    }
    
    // Create snapshot file
    bool snapshotCreated = brain.saveSnapshot("demo_brain.chk");
    std::cout << "Snapshot created: " << (snapshotCreated ? "Yes" : "No") << std::endl;
    
    if (snapshotCreated) {
        std::cout << "Snapshot saved as: demo_brain.chk" << std::endl;
        
        // Try to load it back (this would be done in a separate brain instance)
        std::cout << "Checkpoint and snapshot demo completed successfully." << std::endl;
    }
}

// Main demo function
int main(int argc, char** argv) {
    std::cout << "=== NLM Advanced Features Demo ===" << std::endl;
    std::cout << "This demo showcases the new advanced features in NLM Phase 6." << std::endl;
    
    // Create brain with custom configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    // Create brain instance
    nlm::Brain brain(config);
    
    std::cout << "Brain created with configuration:" << std::endl;
    std::cout << "  Neurons: " << config->getOr<size_t>("neuron_count", 0) << std::endl;
    std::cout << "  Regions: " << config->getOr<size_t>("region_count", 0) << std::endl;
    std::cout << "  Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << " s" << std::endl;
    
    // Initialize brain
    std::cout << "\nInitializing brain..." << std::endl;
    if (!brain.initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    
    // Run demonstration of various features
    demonstrateConfigurationValidation(brain);
    
    demonstrateSimulationControl(brain);
    
    demonstrateEventHandlers(brain);
    
    demonstrateMemoryManagement(brain);
    
    // Performance benchmark
    benchmarkPerformance(brain, 200);
    
    demonstrateCheckpointSystem(brain);
    
    // Final statistics
    std::cout << "\n=== Demo Summary ===" << std::endl;
    std::cout << "Brain status:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    std::cout << "\n=== All demonstrations completed successfully! ===" << std::endl;
    std::cout << "\nThe NLM advanced features demonstration showcases:" << std::endl;
    std::cout << "✓ Enhanced API with new simulation control methods" << std::endl;
    std::cout << "✓ Event handlers for custom behavior" << std::endl;
    std::cout << "✓ Performance profiling and monitoring" << std::endl;
    std::cout << "✓ Memory management and optimization" << std::endl;
    std::cout << "✓ Simulation pause/resume functionality" << std::endl;
    std::cout << "✓ Configuration validation" << std::endl;
    std::cout << "✓ Checkpoint and snapshot functionality" << std::endl;
    
    return 0;
}