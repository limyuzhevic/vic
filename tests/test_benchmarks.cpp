// Benchmark Test Suite
// Performance benchmarks and microbenchmarks

#include "MemoryPoolManager.hpp"
#include "EventSystem.hpp"
#include "ConnectivityManager.hpp"
#include "ParallelProcessor.hpp"
#include "SIMDProcessor.hpp"
#include "Brain.hpp"
#include "BrainAdapter.hpp"
#include "Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace test_benchmarks {
    
    // Test fixtures
    class BenchmarkTestFixture {
    public:
        std::shared_ptr<nlm::Config> config_;
        
        BenchmarkTestFixture() {
            config_ = std::make_shared<nlm::Config>();
            config_->setNeuronCount(1000);
            config_->setRegionCount(1);
            config_->setVisionDimensions(64, 64);
        }
    };
    
    // Memory Management Benchmarks
    BENCHMARK("Memory Pool Allocation/Deallocation", "[benchmark][memory]") {
        nlm::MemoryPoolManager& poolManager = nlm::MemoryPoolManager::getInstance();
        poolManager.setConfig(config_);
        
        // Benchmark memory allocation and deallocation
        size_t iterations = 10000;
        std::vector<nlm::NeuronId> neuronIds;
        std::vector<nlm::SynapseId> synapseIds;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::NeuronId neuronId = poolManager.allocateNeuron(nlm::NeuronType::Internal);
            neuronIds.push_back(neuronId);
            
            nlm::SynapseId synapseId = poolManager.allocateSynapse(nlm::SynapseType::Excitatory);
            synapseIds.push_back(synapseId);
        }
        
        // Deallocate all
        for (auto id : neuronIds) {
            poolManager.deallocateNeuron(id);
        }
        for (auto id : synapseIds) {
            poolManager.deallocateSynapse(id);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count(); // Return time in microseconds
    }
    
    BENCHMARK("Memory Pool Batch Operations", "[benchmark][memory]") {
        nlm::MemoryPoolManager& poolManager = nlm::MemoryPoolManager::getInstance();
        poolManager.setConfig(config_);
        
        size_t iterations = 5000;
        std::vector<nlm::NeuronId> neuronIds;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Batch allocate
        for (size_t i = 0; i < iterations; ++i) {
            neuronIds.push_back(poolManager.allocateNeuron(nlm::NeuronType::Excitatory));
        }
        
        // Batch deallocate
        for (auto id : neuronIds) {
            poolManager.deallocateNeuron(id);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Event System Benchmarks
    BENCHMARK("Event System Event Scheduling", "[benchmark][events]") {
        nlm::EventSystem eventSystem(10000);
        
        size_t iterations = 10000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::EventSystem::Event event;
            event.type = nlm::EventSystem::EventType::Spike;
            event.step = i;
            event.time = static_cast<double>(i) / 1000.0;
            event.neuronId = static_cast<nlm::NeuronId>(i % 1000);
            event.synapseId = static_cast<nlm::SynapseId>(i % 1000);
            event.weightChange = 0.01f * (i % 100);
            
            eventSystem.scheduleEvent(event);
            
            if (eventSystem.hasEvents()) {
                eventSystem.processNextEvent();
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("Event System Processing Loop", "[benchmark][events]") {
        nlm::EventSystem eventSystem(5000);
        
        // Schedule many events
        for (size_t i = 0; i < 5000; ++i) {
            nlm::EventSystem::Event event;
            event.type = nlm::EventSystem::EventType::Spike;
            event.step = i;
            event.time = static_cast<double>(i) / 100.0;
            event.neuronId = static_cast<nlm::NeuronId>(i % 500);
            event.synapseId = static_cast<nlm::SynapseId>(i % 500);
            event.weightChange = 0.1f;
            
            eventSystem.scheduleEvent(event);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process all events
        while (eventSystem.hasEvents()) {
            eventSystem.processNextEvent();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Connectivity Management Benchmarks
    BENCHMARK("Connectivity Manager Connection Operations", "[benchmark][connectivity]") {
        nlm::ConnectivityManager connectivity(500, 0.1f);
        
        size_t iterations = 5000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            connectivity.addConnection(
                static_cast<nlm::NeuronId>(i % 500),
                static_cast<nlm::NeuronId>((i + 1) % 500),
                nlm::SynapseType::Excitatory
            );
            
            bool exists = connectivity.hasConnection(
                static_cast<nlm::NeuronId>(i % 500),
                static_cast<nlm::NeuronId>((i + 1) % 500)
            );
            
            auto outgoing = connectivity.getConnectionsFrom(static_cast<nlm::NeuronId>(i % 500));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("Connectivity Manager Statistics", "[benchmark][connectivity]") {
        nlm::ConnectivityManager connectivity(1000, 0.05f);
        
        // Add many connections
        for (nlm::NeuronId i = 0; i < 1000; ++i) {
            for (nlm::NeuronId j = 0; j < 1000; ++j) {
                if ((i + j) % 3 == 0) { // Sparse connectivity
                    connectivity.addConnection(i, j, nlm::SynapseType::Excitatory);
                }
            }
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Get statistics
        for (int i = 0; i < 100; ++i) {
            auto stats = connectivity.getStats();
            auto density = connectivity.getConnectionDensity();
            auto average = connectivity.getAverageConnectionStrength();
            
            // Get connections from random neuron
            auto randomId = static_cast<nlm::NeuronId>(i % 1000);
            auto outgoing = connectivity.getConnectionsFrom(randomId);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Parallel Processing Benchmarks
    BENCHMARK("Parallel Processor Task Submission", "[benchmark][parallel]") {
        nlm::ParallelProcessor processor(4);
        
        size_t iterations = 5000;
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<int> result(100, 0);
        
        for (size_t i = 0; i < iterations; ++i) {
            processor.submitTask([&result]() {
                for (auto& val : result) {
                    val++;
                }
            }, 0);
        }
        
        processor.waitForCompletion();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("Parallel Processor Work Distribution", "[benchmark][parallel]") {
        nlm::ParallelProcessor processor(8);
        
        std::vector<std::vector<int>> data(8, std::vector<int>(1000, 0));
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < 8; ++i) {
            processor.submitTask([&, i]() {
                for (size_t j = 0; j < 1000; ++j) {
                    data[i][j] = static_cast<int>(i * 1000 + j);
                }
            }, i);
        }
        
        processor.waitForCompletion();
        
        // Verify results
        int total = 0;
        for (size_t i = 0; i < 8; ++i) {
            for (size_t j = 0; j < 1000; ++j) {
                total += data[i][j];
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // SIMD Processor Benchmarks
    BENCHMARK("SIMD Processor Memory Alignment", "[benchmark][simd]") {
        nlm::SIMDProcessor simd;
        
        size_t iterations = 1000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            size_t size = 1024;
            void* alignedPtr = simd.alignedAllocate(size, 64);
            
            if (alignedPtr) {
                // Simulate SIMD operations
                float* floatPtr = static_cast<float*>(alignedPtr);
                for (size_t j = 0; j < size / sizeof(float); ++j) {
                    floatPtr[j] = static_cast<float>(j * 1.5f);
                }
                
                simd.alignedDeallocate(alignedPtr);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("SIMD Processor Feature Detection", "[benchmark][simd]") {
        nlm::SIMDProcessor simd;
        
        size_t iterations = 10000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            bool isAvailable = simd.isSIMDAvailable();
            bool isAligned = simd.isAligned(&i, alignof(int));
            
            // Just read features, don't use them
            (void)isAvailable;
            (void)isAligned;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Brain Adapter Benchmarks
    BENCHMARK("BrainAdapter Initialization", "[benchmark][brain_adapter]") {
        auto config = std::make_shared<nlm::Config>();
        config->setNeuronCount(1000);
        config->setRegionCount(1);
        config->setEnableEventDriven(true);
        config->setEnableMultithreading(true);
        config->setEnableSIMD(true);
        config->setEnableSparse(true);
        config->setThreadCount(4);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::BrainAdapter adapter(config);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("BrainAdapter Step Processing", "[benchmark][brain_adapter]") {
        auto config = std::make_shared<nlm::Config>();
        config->setNeuronCount(1000);
        config->setRegionCount(1);
        config->setEnableEventDriven(true);
        config->setEnableMultithreading(true);
        config->setEnableSIMD(true);
        config->setEnableSparse(true);
        config->setThreadCount(4);
        config->setEnablePerformanceMonitoring(true);
        
        nlm::BrainAdapter adapter(config);
        adapter.enablePerformanceMonitoring(true);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process multiple steps
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            adapter.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("BrainAdapter Performance Monitoring", "[benchmark][brain_adapter]") {
        auto config = std::make_shared<nlm::Config>();
        config->setNeuronCount(1000);
        config->setRegionCount(1);
        config->setEnableEventDriven(true);
        config->setEnableMultithreading(true);
        config->setEnableSIMD(true);
        config->setEnableSparse(true);
        config->setThreadCount(4);
        config->setEnablePerformanceMonitoring(true);
        
        nlm::BrainAdapter adapter(config);
        adapter.enablePerformanceMonitoring(true);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Get performance statistics
        for (int i = 0; i < 100; ++i) {
            auto stats = adapter.getPerformanceStats();
            auto optConfig = adapter.getOptimizationConfig();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Complete System Benchmarks
    BENCHMARK("Complete System Initialization", "[benchmark][complete]") {
        auto config = std::make_shared<nlm::Config>();
        config->setNeuronCount(1000);
        config->setRegionCount(1);
        config->setVisionDimensions(64, 64);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Initialize complete system
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        nlm::BrainAdapter adapter(config);
        adapter.enablePerformanceMonitoring(true);
        
        auto agentBrain = std::make_unique<nlm::AgentBrain>(brain, config);
        nlm::SimpleWorld world(64, 64);
        agentBrain->initialize(world);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    BENCHMARK("Complete System Processing", "[benchmark][complete]") {
        auto config = std::make_shared<nlm::Config>();
        config->setNeuronCount(500);
        config->setRegionCount(1);
        config->setVisionDimensions(32, 32);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        nlm::BrainAdapter adapter(config);
        adapter.enablePerformanceMonitoring(true);
        
        auto agentBrain = std::make_unique<nlm::AgentBrain>(brain, config);
        nlm::SimpleWorld world(32, 32);
        agentBrain->initialize(world);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 50; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.5f);
            
            agentBrain->processSensoryInput(percept);
            
            brain->step(step);
            adapter.step(step);
        
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
    // Memory Bandwidth Tests
    BENCHMARK("Memory Pool Bandwidth Test", "[benchmark][memory_bandwidth]") {
        nlm::MemoryPoolManager& poolManager = nlm::MemoryPoolManager::getInstance();
        poolManager.setConfig(config_);
        
        size_t iterations = 10000;
        size_t totalBytes = 0;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            // Allocate various sizes
            size_t sizes[] = {sizeof(int), sizeof(float), sizeof(double), sizeof(nlm::NeuronId), sizeof(nlm::SynapseId)};
            
            for (size_t size : sizes) {
                void* ptr = malloc(size);
                if (ptr) {
                    free(ptr);
                    totalBytes += size;
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (duration.count() > 0) {
            return totalBytes * 1000000 / duration.count(); // bytes per second
        }
        
        return 0;
    }
    
    // Cache Locality Tests
    BENCHMARK("Memory Pool Cache Locality", "[benchmark][cache]") {
        nlm::MemoryPoolManager& poolManager = nlm::MemoryPoolManager::getInstance();
        poolManager.setConfig(config_);
        
        size_t iterations = 5000;
        auto start = std::chrono::high_resolution_clock::now();
        
        // Allocate and deallocate in pattern that tests cache locality
        std::vector<nlm::NeuronId> neuronIds;
        
        for (size_t i = 0; i < iterations; ++i) {
            // Use sequential access for good cache locality
            nlm::NeuronId id = poolManager.allocateNeuron(nlm::NeuronType::Internal);
            neuronIds.push_back(id);
            
            if (i % 100 == 0 && !neuronIds.empty()) {
                // Deallocate oldest
                auto oldestId = neuronIds.front();
                neuronIds.erase(neuronIds.begin());
                poolManager.deallocateNeuron(oldestId);
            }
        }
        
        // Clean up remaining
        for (auto id : neuronIds) {
            poolManager.deallocateNeuron(id);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        return duration.count();
    }
    
} // namespace test_benchmarks