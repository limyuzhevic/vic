// Performance Benchmarks
// Performance benchmarks for NLM improvements

#include "test_infrastructure.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_performance {

void benchmarkMemoryAllocation() {
    std::cout << "    Benchmarking memory allocation performance..." << std::endl;
    
    // Benchmark unique_ptr allocation
    auto allocationTime = Benchmark::measure([]() {
        std::vector<std::unique_ptr<int>> allocations;
        for (int i = 0; i < 1000; ++i) {
            allocations.emplace_back(std::make_unique<int>(i));
        }
        return allocations;
    });
    
    std::cout << "    Memory allocation (1000 unique_ptrs): " << allocationTime << " ms" << std::endl;
    
    // Benchmark shared_ptr allocation
    auto sharedAllocationTime = Benchmark::measure([]() {
        std::vector<std::shared_ptr<int>> allocations;
        for (int i = 0; i < 1000; ++i) {
            allocations.emplace_back(std::make_shared<int>(i));
        }
        return allocations;
    });
    
    std::cout << "    Shared_ptr allocation (1000 shared_ptrs): " << sharedAllocationTime << " ms" << std::endl;
    
    std::cout << "    benchmarkMemoryAllocation passed" << std::endl;
}

void benchmarkConfigOperations() {
    std::cout << "    Benchmarking configuration operations..." << std::endl;
    
    // Benchmark config set/get operations
    auto configTime = Benchmark::measure([]() {
        nlm::Config config;
        for (int i = 0; i < 1000; ++i) {
            config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
            config.get<int>(std::to_string(i));
        }
    });
    
    std::cout << "    Config set/get operations (1000): " << configTime << " ms" << std::endl;
    
    // Benchmark config keys retrieval
    auto keysTime = Benchmark::measure([]() {
        nlm::Config config;
        for (int i = 0; i < 1000; ++i) {
            config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
        }
        config.getKeys();
    });
    
    std::cout << "    Config getKeys (1000 entries): " << keysTime << " ms" << std::endl;
    
    std::cout << "    benchmarkConfigOperations passed" << std::endl;
}

void benchmarkBrainOperations() {
    std::cout << "    Benchmarking brain operations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Benchmark brain initialization
    auto brainInitTime = Benchmark::measure([&]() {
        nlm::Brain brain(config);
        brain.initialize();
    });
    
    std::cout << "    Brain initialization: " << brainInitTime << " ms" << std::endl;
    
    // Benchmark brain step operation
    auto brainStepTime = Benchmark::measure([&]() {
        nlm::Brain brain(config);
        brain.initialize();
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
    });
    
    std::cout << "    Brain step operation (100 steps): " << brainStepTime << " ms" << std::endl;
    
    std::cout << "    benchmarkBrainOperations passed" << std::endl;
}

void benchmarkMemoryUsage() {
    std::cout << "    Benchmarking memory usage patterns..." << std::endl;
    
    // Benchmark working memory operations
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto workingMemTime = Benchmark::measure([&]() {
        nlm::Brain brain(config);
        brain.initialize();
        for (int i = 0; i < 100; ++i) {
            brain.updateWorkingMemory(i);
        }
    });
    
    std::cout << "    Working memory operations (100 updates): " << workingMemTime << " ms" << std::endl;
    
    // Benchmark episodic memory operations
    auto episodicMemTime = Benchmark::measure([&]() {
        nlm::Brain brain(config);
        brain.initialize();
        for (int i = 0; i < 100; ++i) {
            brain.storeEpisodicMemory(std::to_string(i), i * 1000, i * 0.1f);
        }
    });
    
    std::cout << "    Episodic memory operations (100 stores): " << episodicMemTime << " ms" << std::endl;
    
    std::cout << "    benchmarkMemoryUsage passed" << std::endl;
}

void benchmarkDataGeneration() {
    std::cout << "    Benchmarking data generation..." << std::endl;
    
    // Benchmark test data generation
    auto dataGenTime = Benchmark::measure([]() {
        std::vector<int64_t> neuronIds = nlm::test::TestDataGenerator::generateNeuronIds(1000);
        std::vector<double> weights = nlm::test::TestDataGenerator::generateWeights(1000);
        std::vector<std::string> keys = nlm::test::TestDataGenerator::generateConfigKeys(1000);
        return std::tuple(neuronIds, weights, keys);
    });
    
    std::cout << "    Test data generation (3x1000 elements): " << dataGenTime << " ms" << std::endl;
    
    std::cout << "    benchmarkDataGeneration passed" << std::endl;
}

void runAll() {
    benchmarkMemoryAllocation();
    benchmarkConfigOperations();
    benchmarkBrainOperations();
    benchmarkMemoryUsage();
    benchmarkDataGeneration();
}

} // namespace test_performance
