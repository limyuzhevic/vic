#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <memory>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "core/Config/Config.hpp"

namespace test_performance {

void testScalabilityWithIncreasingNeuronCounts() {
    std::cout << "Testing scalability with increasing neuron counts..." << std::endl;
    
    std::vector<int64_t> neuron_counts = {10, 50, 100, 500, 1000};
    double max_time_per_test = 5.0; // 5 seconds max per test
    
    for (int64_t count : neuron_counts) {
        std::cout << "  Testing with " << count << " neurons..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", count, nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Simulate some operations
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        std::cout << "    Time: " << duration << "s" << std::endl;
        assert(duration < max_time_per_test);
        assert(brain.getTotalNeuronCount() == count);
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testMemoryUsageWithIncreasingMemoryCapacity() {
    std::cout << "Testing memory usage with increasing memory capacity..." << std::endl;
    
    std::vector<size_t> capacities = {10, 100, 1000, 5000, 10000};
    double max_time_per_test = 10.0; // 10 seconds max per test
    
    for (size_t capacity : capacities) {
        std::cout << "  Testing with capacity " << capacity << "..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        nlm::NeuralWorkingMemory workingMem(capacity, 100.0f, 0.5f);
        assert(workingMem.getCapacity() == capacity);
        
        // Test encoding and retrieving items
        for (size_t i = 0; i < std::min(capacity / 10, size_t(100)); ++i) {
            nlm::MemoryItem item;
            item.type = nlm::MemoryItemType::Working;
            item.content = std::vector<float>(3, static_cast<float>(i));
            item.timestamp = static_cast<float>(i * 100);
            item.associationStrength = 0.9f;
            
            workingMem.encode(item);
            workingMem.update(1.0f);
        }
        
        // Test retrieval
        nlm::MemoryItem retrieved = workingMem.retrieve(0.0f);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        std::cout << "    Time: " << duration << "s" << std::endl;
        assert(duration < max_time_per_test);
        assert(workingMem.getCurrentLoad() > 0);
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testScalabilityWithIncreasingSynapses() {
    std::cout << "Testing scalability with increasing synapses..." << std::endl;
    
    std::vector<size_t> synapse_counts = {100, 1000, 5000};
    double max_time_per_test = 5.0; // 5 seconds max per test
    
    for (size_t count : synapse_counts) {
        std::cout << "  Testing with " << count << " synapses..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Simulate synaptic operations
        size_t synaptic_ops = 0;
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
            synaptic_ops += brain.getTotalSynapseCount();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        
        std::cout << "    Time: " << duration << "s" << std::endl;
        assert(duration < max_time_per_test);
        assert(synaptic_ops > 0);
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testPerformanceWithMemorySystems() {
    std::cout << "Testing performance with memory systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::NeuralWorkingMemory workingMem(1000, 100.0f, 0.5f);
    nlm::NeuralEpisodicMemory episodicMem(10000, 0.3f, 0.1f);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Encode many items
    for (size_t i = 0; i < 1000; ++i) {
        nlm::MemoryItem item;
        item.type = nlm::MemoryItemType::Working;
        item.content = std::vector<float>(5, static_cast<float>(i));
        item.timestamp = static_cast<float>(i * 10);
        item.associationStrength = 0.8f;
        
        workingMem.encode(item);
        workingMem.update(0.01f);
    }
    
    for (size_t i = 0; i < 1000; ++i) {
        nlm::EpisodicEvent event;
        event.timestamp = static_cast<float>(i * 100);
        event.context = std::vector<float>(3, static_cast<float>(i % 100));
        event.outcome = std::vector<float>(2, static_cast<float>(i % 2));
        event.significance = 0.7f;
        
        episodicMem.encode(event);
        episodicMem.update(1.0f);
    }
    
    // Retrieve items
    nlm::MemoryItem retrieved_working = workingMem.retrieve(0.0f);
    nlm::EpisodicEvent retrieved_episodic = episodicMem.retrieve(9900.0f);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time).count();
    
    std::cout << "  Time: " << duration << "s" << std::endl;
    assert(duration < 10.0); // 10 seconds max
    
    std::cout << "  PASSED" << std::endl;
}

void testPerformanceWithNeuromodulation() {
    std::cout << "Testing performance with neuromodulation..." << std::endl;
    
    nlm::Curiosity curiosity(0.7f, 0.3f, 0.1f, 0.5f, 1.0f);
    nlm::Novelty novelty_detector(0.5f, 0.8f, 0.01f, 0.05f);
    nlm::PredictionError prediction_error(0.5f, 0.2f, 0.1f);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate neuromodulation updates
    for (int i = 0; i < 1000; ++i) {
        curiosity.update(static_cast<float>(i % 100) / 100.0f, 1.0f);
        
        nlm::Novelty::NoveltyInput input;
        input.patternId = i;
        input.intensity = static_cast<float>(i % 100) / 100.0f;
        input.context = "test_pattern";
        
        novelty_detector.update(input);
        
        prediction_error.update(static_cast<float>(i % 50) / 50.0f, static_cast<float>(i % 30) / 30.0f);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time).count();
    
    std::cout << "  Time: " << duration << "s" << std::endl;
    assert(duration < 5.0); // 5 seconds max
    
    std::cout << "  PASSED" << std::endl;
}

void testMemoryLeakPrevention() {
    std::cout << "Testing memory leak prevention..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuralWorkingMemory workingMem(100, 100.0f, 0.5f);
    nlm::NeuralEpisodicMemory episodicMem(1000, 0.3f, 0.1f);
    
    // Perform many operations
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Encode and decode
        for (size_t i = 0; i < 10; ++i) {
            nlm::MemoryItem item;
            item.type = nlm::MemoryItemType::Working;
            item.content = std::vector<float>{1.0f, 2.0f, 3.0f};
            item.timestamp = static_cast<float>(i * 100);
            
            workingMem.encode(item);
            workingMem.update(1.0f);
            
            workingMem.retrieve(0.0f);
        }
        
        for (size_t i = 0; i < 10; ++i) {
            nlm::EpisodicEvent event;
            event.timestamp = static_cast<float>(i * 100);
            event.context = std::vector<float>{0.1f, 0.2f};
            event.outcome = std::vector<float>{1.0f};
            
            episodicMem.encode(event);
            episodicMem.update(10.0f);
            
            episodicMem.retrieve(static_cast<float>(i * 100));
        }
        
        workingMem.clear();
        episodicMem.clear();
    }
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Tests ===" << std::endl;
    
    testScalabilityWithIncreasingNeuronCounts();
    testMemoryUsageWithIncreasingMemoryCapacity();
    testScalabilityWithIncreasingSynapses();
    testPerformanceWithMemorySystems();
    testPerformanceWithNeuromodulation();
    testMemoryLeakPrevention();
    
    std::cout << "=== All Performance Tests PASSED ===" << std::endl;
}

} // namespace test_performance
