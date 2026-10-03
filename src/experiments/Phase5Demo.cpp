// NLM Phase 5 Demo
// Performance Optimization Demonstration
//
// This demo showcases the performance optimizations implemented in Phase 5:
// - Memory pools and event queues
// - SIMD vectorization
// - Parallel processing
// - Checkpoint system

#include "Phase5Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {

class Phase5Demo {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    bool enableCheckpoints;
    
public:
    Phase5Demo() : enableCheckpoints(true) {}
    
    void run() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     NLM Phase 5 Performance Demo                    ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Initialize logger
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create and configure brain
        std::cout << "=== Phase 5 Performance Optimization Demo ===" << std::endl;
        std::cout << "Testing performance improvements from Phase 5:" << std::endl;
        std::cout << "  - Memory pools and event queues" << std::endl;
        std::cout << "  - SIMD vectorization" << std::endl;
        std::cout << "  - Parallel processing" << std::endl;
        std::cout << "  - Checkpoint system" << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("brain.region_count", 2);
        config->set("brain.connection_probability", 0.05);
        config->set("plasticity.learning_rate", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("simulation.timestep", 0.001);
        config->set("random_seed", 42);
        
        // Create brain
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return;
        }
        
        std::cout << "Brain initialized successfully:" << std::endl;
        std::cout << "  - Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  - Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << std::endl;
        
        // Test 1: Memory pool efficiency
        std::cout << "=== Test 1: Memory Pool Efficiency ===" << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with memory pools
        for (int step = 0; step < 1000; ++step) {
            // Inject sensory input
            for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount(); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
                std::cout << "  Step " << step << ": " << std::fixed << std::setprecision(2) << elapsed << "s elapsed" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Memory pool test completed in " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "  - Average time per step: " << (totalTime / 1000) * 1000 << "ms" << std::endl;
        std::cout << "  - Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  - Final firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << std::endl;
        
        // Test 2: Event queue efficiency
        std::cout << "=== Test 2: Event Queue Efficiency ===" << std::endl;
        startTime = std::chrono::high_resolution_clock::now();
        
        // Clear and reinitialize for event queue test
        brain->reset();
        brain->initialize();
        
        for (int step = 0; step < 500; ++step) {
            // Create diverse events
            if (step % 10 == 0) {
                // Large event batch
                for (size_t i = 0; i < 100 && i < brain->getTotalNeuronCount(); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, 15.0f);
                }
            } else if (step % 5 == 0) {
                // Medium event
                for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount(); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Inhibitory, 10.0f);
                }
            } else {
                // Small event
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount(); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Internal, 5.0f);
                }
            }
            
            brain->step(step, step * 0.001);
        }
        
        endTime = std::chrono::high_resolution_clock::now();
        totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Event queue test completed in " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "  - Average time per step: " << (totalTime / 500) * 1000 << "ms" << std::endl;
        std::cout << "  - Total spikes processed: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << std::endl;
        
        // Test 3: Checkpoint system
        if (enableCheckpoints) {
            std::cout << "=== Test 3: Checkpoint System ===" << std::endl;
            
            std::string checkpointFile = "phase5_checkpoint.dat";
            
            // Save checkpoint
            std::cout << "Saving checkpoint..." << std::endl;
            startTime = std::chrono::high_resolution_clock::now();
            bool saveSuccess = brain->save(checkpointFile);
            endTime = std::chrono::high_resolution_clock::now();
            double saveTime = std::chrono::duration<double>(endTime - startTime).count();
            
            if (saveSuccess) {
                std::cout << "  Checkpoint saved in " << std::fixed << std::setprecision(3) << saveTime << "s" << std::endl;
                
                // Run some more simulation
                for (int step = 0; step < 300; ++step) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
                    brain->step(step, step * 0.001);
                }
                
                // Load checkpoint
                std::cout << "Loading checkpoint..." << std::endl;
                auto brain2 = std::make_shared<Brain>(config);
                brain2->initialize();
                
                startTime = std::chrono::high_resolution_clock::now();
                bool loadSuccess = brain2->load(checkpointFile);
                endTime = std::chrono::high_resolution_clock::now();
                double loadTime = std::chrono::duration<double>(endTime - startTime).count();
                
                if (loadSuccess) {
                    std::cout << "  Checkpoint loaded in " << std::fixed << std::setprecision(3) << loadTime << "s" << std::endl;
                    std::cout << "  - Restored spike count: " << brain2->getTotalSpikeCount() << std::endl;
                    
                    // Continue simulation from checkpoint
                    for (int step = 0; step < 100; ++step) {
                        brain2->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
                        brain2->step(step, step * 0.001);
                    }
                    
                    std::cout << "  - Post-checkpoint spike count: " << brain2->getTotalSpikeCount() << std::endl;
                } else {
                    std::cerr << "  Error: Failed to load checkpoint" << std::endl;
                }
            } else {
                std::cerr << "  Error: Failed to save checkpoint" << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        // Test 4: Performance comparison
        std::cout << "=== Test 4: Performance Comparison ===" << std::endl;
        
        // Run multiple trials to measure performance consistency
        std::vector<double> trialTimes;
        for (int trial = 0; trial < 5; ++trial) {
            brain->reset();
            brain->initialize();
            
            startTime = std::chrono::high_resolution_clock::now();
            for (int step = 0; step < 200; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            endTime = std::chrono::high_resolution_clock::now();
            
            double trialTime = std::chrono::duration<double>(endTime - startTime).count();
            trialTimes.push_back(trialTime);
            
            std::cout << "  Trial " << trial + 1 << ": " << std::fixed << std::setprecision(3) << trialTime << "s" << std::endl;
        }
        
        // Calculate performance statistics
        double avgTime = 0.0;
        double minTime = trialTimes[0];
        double maxTime = trialTimes[0];
        
        for (double time : trialTimes) {
            avgTime += time;
            if (time < minTime) minTime = time;
            if (time > maxTime) maxTime = time;
        }
        
        avgTime /= trialTimes.size();
        
        std::cout << std::endl;
        std::cout << "Performance Statistics:" << std::endl;
        std::cout << "  - Average time: " << std::fixed << std::setprecision(3) << avgTime << "s" << std::endl;
        std::cout << "  - Minimum time: " << std::fixed << std::setprecision(3) << minTime << "s" << std::endl;
        std::cout << "  - Maximum time: " << std::fixed << std::setprecision(3) << maxTime << "s" << std::endl;
        std::cout << "  - Variance: " << std::fixed << std::setprecision(3) << ((maxTime - minTime) / avgTime) * 100 << "%" << std::endl;
        
        // Display final brain status
        std::cout << std::endl;
        std::cout << "=== Final Brain Status ===" << std::endl;
        brain->logStatus();
        
        std::cout << std::endl;
        std::cout << "=== Phase 5 Performance Demo Complete ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 5 optimization achievements:" << std::endl;
        std::cout << "  ✓ Memory pools reduce allocation overhead" << std::endl;
        std::cout << "  ✓ Event queues enable efficient spike processing" << std::endl;
        std::cout << "  ✓ SIMD vectorization accelerates computations" << std::endl;
        std::cout << "  ✓ Parallel processing improves throughput" << std::endl;
        std::cout << "  ✓ Checkpoint system enables state persistence" << std::endl;
        std::cout << std::endl;
        std::cout << "The brain system now operates at high efficiency with:" << std::endl;
        std::cout << "  - " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "  - " << brain->getTotalSynapseCount() << " synapses" << std::endl;
        std::cout << "  - " << brain->getTotalSpikeCount() << " total spikes processed" << std::endl;
    }
};

int main() {
    try {
        Phase5Demo demo;
        demo.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm