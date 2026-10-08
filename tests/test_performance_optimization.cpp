#include "Performance/OptimizedNeuralDynamics.hpp"
#include "Performance/OptimizedEventSystem.hpp"
#include "Performance/ParallelProcessing.hpp"
#include "Brain/Brain.hpp"
#include "Core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <vector>

namespace nlm {

class PerformanceOptimizationTests {
public:
    PerformanceOptimizationTests() {
        std::cout << "Running NLM Performance Optimization Tests..." << std::endl;
        
        // Test 1: Memory pool system
        testMemoryPoolSystem();
        
        // Test 2: Event queue system
        testEventQueueSystem();
        
        // Test 3: Parallel processing
        testParallelProcessing();
        
        // Test 4: Neural dynamics
        testNeuralDynamics();
        
        // Test 5: Integration with Brain
        testBrainIntegration();
        
        std::cout << "All performance optimization tests passed!" << std::endl;
    }
    
private:
    void testMemoryPoolSystem() {
        std::cout << "  Testing MemoryPoolSystem..." << std::endl;
        
        // Test memory pool statistics
        MemoryPoolStats stats;
        assert(stats.totalAllocations == 0);
        assert(stats.totalDeallocations == 0);
        assert(stats.allocationFailures == 0);
        
        std::cout << "    MemoryPoolSystem test passed." << std::endl;
    }
    
    void testEventQueueSystem() {
        std::cout << "  Testing EventQueueSystem..." << std::endl;
        
        // Test event queue configuration
        OptimizedEventQueue eventQueue(1024);
        assert(eventQueue.getSpikeCount() == 0);
        assert(eventQueue.getDelayedSpikeCount() == 0);
        assert(eventQueue.getPriorityEventCount() == 0);
        
        // Test queue operations
        bool pushResult = eventQueue.queueImmediate(1, 2, 3, 0.5, true);
        assert(pushResult == true);
        
        bool emptyResult = eventQueue.empty();
        assert(emptyResult == false);
        
        std::cout << "    EventQueueSystem test passed." << std::endl;
    }
    
    void testParallelProcessing() {
        std::cout << "  Testing ParallelProcessing..." << std::endl;
        
        // Test thread pool
        ThreadPool pool(4);
        assert(pool.getNumThreads() == 4);
        
        // Test parallel for
        std::vector<int> results(100, 0);
        ParallelFor::simple(0, 100, 
                          [&results](size_t i) {
                              results[i] = i * i;
                          });
        
        for (size_t i = 0; i < 100; ++i) {
            assert(results[i] == i * i);
        }
        
        std::cout << "    ParallelProcessing test passed." << std::endl;
    }
    
    void testNeuralDynamics() {
        std::cout << "  Testing NeuralDynamics..." << std::endl;
        
        // Test neural dynamics configuration
        NeuralDynamicsConfig config;
        assert(config.enableSIMD == true);
        assert(config.enableParallel == true);
        assert(config.enableMemoryPool == true);
        
        // Test performance metrics
        NeuralDynamicsMetrics metrics;
        assert(metrics.neuronsPerStep == 0);
        assert(metrics.spikesProcessed == 0);
        assert(metrics.totalStepTimeMs == 0.0);
        
        std::cout << "    NeuralDynamics test passed." << std::endl;
    }
    
    void testBrainIntegration() {
        std::cout << "  Testing Brain Integration..." << std::endl;
        
        // Create configuration with performance optimizations
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("use_memory_pool", true);
        config->set("use_optimized_event_system", true);
        config->set("use_optimized_neural_dynamics", true);
        config->set("use_optimized_plasticity", true);
        config->set("thread_count", 2);
        config->set("enable_simd", true);
        config->set("enable_parallel", true);
        
        // Create brain with optimized systems
        auto brain = std::make_unique<Brain>(config);
        
        // Initialize brain
        bool initResult = brain->initialize();
        assert(initResult == true);
        
        // Get performance statistics
        auto perfStats = brain->getPerformanceStats();
        assert(perfStats.neuronsPerStep > 0);
        assert(perfStats.spikesPerSecond >= 0);
        
        // Run a few steps to test performance
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        // Verify performance metrics updated
        auto updatedStats = brain->getPerformanceStats();
        assert(updatedStats.totalSteps >= 10);
        
        std::cout << "    Brain Integration test passed." << std::endl;
    }
};

} // namespace nlm

int main() {
    try {
        nlm::PerformanceOptimizationTests tests;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}