// Performance Profiler Demo
// This demo showcases the Performance Profiler capabilities

#include "PerformanceProfiler.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <cmath>

using namespace nlm;

class PerformanceProfilerDemo {
public:
    PerformanceProfilerDemo() {}
    ~PerformanceProfilerDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "PERFORMANCE PROFILER DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize profiler
        PerformanceProfiler profiler;
        
        // Start benchmark with different configurations
        std::cout << "--- Performance Benchmarks ---\n";
        
        // Benchmark 1: Small brain
        std::cout << "Benchmark 1: Small Brain (100 neurons)\n";
        auto smallConfig = createConfig(100, 2, 0.1f);
        profiler.startBenchmark("small_brain", smallConfig);
        
        auto smallBrain = std::make_shared<Brain>(smallConfig);
        smallBrain->initialize();
        runSimulation(smallBrain);
        
        auto smallResult = profiler.stopBenchmark();
        std::cout << "  Average FPS: " << smallResult.averageFps << "\n";
        std::cout << "  Average memory usage: " << smallResult.averageMemoryUsage << "KB\n";
        std::cout << "  Total time: " << smallResult.totalTime << "s\n\n";
        
        // Benchmark 2: Medium brain
        std::cout << "Benchmark 2: Medium Brain (500 neurons)\n";
        auto mediumConfig = createConfig(500, 4, 0.15f);
        profiler.startBenchmark("medium_brain", mediumConfig);
        
        auto mediumBrain = std::make_shared<Brain>(mediumConfig);
        mediumBrain->initialize();
        runSimulation(mediumBrain);
        
        auto mediumResult = profiler.stopBenchmark();
        std::cout << "  Average FPS: " << mediumResult.averageFps << "\n";
        std::cout << "  Average memory usage: " << mediumResult.averageMemoryUsage << "KB\n";
        std::cout << "  Total time: " << mediumResult.totalTime << "s\n\n";
        
        // Benchmark 3: Large brain
        std::cout << "Benchmark 3: Large Brain (2000 neurons)\n";
        auto largeConfig = createConfig(2000, 8, 0.2f);
        profiler.startBenchmark("large_brain", largeConfig);
        
        auto largeBrain = std::make_shared<Brain>(largeConfig);
        largeBrain->initialize();
        runSimulation(largeBrain);
        
        auto largeResult = profiler.stopBenchmark();
        std::cout << "  Average FPS: " << largeResult.averageFps << "\n";
        std::cout << "  Average memory usage: " << largeResult.averageMemoryUsage << "KB\n";
        std::cout << "  Total time: " << largeResult.totalTime << "s\n\n";
        
        // Profile specific operations
        std::cout << "--- Specific Operation Profiling ---\n";
        
        // Profile memory allocation
        profiler.profileMemoryAllocation("brain_creation", [largeConfig]() {
            return std::make_shared<Brain>(largeConfig);
        });
        
        // Profile synaptic operations
        profiler.profileSynapticOperations("synapse_creation", [largeBrain]() {
            largeBrain->addSynapse(0, 1, 0.5f);
        });
        
        // Profile plasticity operations
        profiler.profilePlasticityOperations("plasticity_update", [largeBrain]() {
            largeBrain->updatePlasticity(0.001f);
        });
        
        // Profile memory system operations
        profiler.profileMemoryOperations("working_memory", [largeBrain]() {
            auto* wm = largeBrain->getWorkingMemory();
            if (wm) wm->createTrace(0, 0.0f);
        });
        
        // Display performance profile results
        std::cout << "\n--- Performance Profile Results ---\n";
        auto profile = profiler.getProfile();
        
        std::cout << "Total benchmarks: " << profile.benchmarks.size() << "\n";
        std::cout << "Memory allocations: " << profile.memoryAllocations.size() << "\n";
        std::cout << "Synaptic operations: " << profile.synapticOperations.size() << "\n";
        std::cout << "Plasticity operations: " << profile.plasticityOperations.size() << "\n";
        std::cout << "Memory operations: " << profile.memoryOperations.size() << "\n";
        
        // Find performance bottlenecks
        std::cout << "\n--- Performance Bottleneck Analysis ---\n";
        auto bottlenecks = profiler.findBottlenecks();
        std::cout << "Operation with highest time: " << bottlenecks.mostTimeConsumingOperation << " (" << bottlenecks.highestTime << "s)\n";
        std::cout << "Operation with highest memory: " << bottlenecks.highestMemoryOperation << " (" << bottlenecks.highestMemory << "KB)\n";
        
        // Calculate performance ratios
        std::cout << "\n--- Performance Ratios ---\n";
        if (smallResult.totalTime > 0 && mediumResult.totalTime > 0 && largeResult.totalTime > 0) {
            float smallToMedium = smallResult.totalTime / mediumResult.totalTime;
            float mediumToLarge = mediumResult.totalTime / largeResult.totalTime;
            float smallToLarge = smallResult.totalTime / largeResult.totalTime;
            
            std::cout << "Small to Medium brain time ratio: " << smallToMedium << "\n";
            std::cout << "Medium to Large brain time ratio: " << mediumToLarge << "\n";
            std::cout << "Small to Large brain time ratio: " << smallToLarge << "\n";
            
            // Calculate efficiency score
            float efficiency = (1.0f / smallToMedium) + (1.0f / mediumToLarge) + (1.0f / smallToLarge);
            std::cout << "Overall efficiency score: " << efficiency << "\n";
        }
        
        // Profile against baseline
        std::cout << "\n--- Baseline Comparison ---\n";
        auto baselineConfig = createConfig(500, 4, 0.15f);
        
        // Run baseline
        profiler.startBenchmark("baseline", baselineConfig);
        auto baselineBrain = std::make_shared<Brain>(baselineConfig);
        baselineBrain->initialize();
        runSimulation(baselineBrain);
        
        auto baselineResult = profiler.stopBenchmark();
        
        // Compare with small brain
        float improvement = (mediumResult.totalTime - baselineResult.totalTime) / baselineResult.totalTime * 100.0f;
        std::cout << "Performance improvement over baseline: " << improvement << "%\n";
        
        // Generate performance report
        std::cout << "\n--- Performance Report ---\n";
        std::string report = profiler.generateReport();
        std::cout << "Performance report generated (" << report.length() << " characters)\n";
        
        // Save performance profile
        std::cout << "\n--- Saving Performance Profile ---\n";
        std::string profilePath = "/tmp/performance_profile.json";
        profiler.saveProfile(profilePath);
        std::cout << "✓ Performance profile saved to: " << profilePath << "\n";
        
        std::cout << "\n========================================\n";
        std::cout << "PERFORMANCE PROFILER DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
    
private:
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        return config;
    }
    
    void runSimulation(std::shared_ptr<Brain> brain) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Run simulation for 100 steps
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001f);
            
            // Inject some activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Simulate some metrics collection
        float totalFiring = brain->getAverageFiringRate() * 100.0f;
        size_t totalSpikes = brain->getTotalSpikeCount();
    }
};

int main(int argc, char* argv[]) {
    nlm::PerformanceProfilerDemo demo;
    demo.run();
    return 0;
}
