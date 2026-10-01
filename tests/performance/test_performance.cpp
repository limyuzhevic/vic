// Performance and Stress Tests for NLM
// Tests memory system performance, prediction system under stress, and integration overhead

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>

namespace test_performance {

void testMemorySystemPerformance() {
    std::cout << "    Testing memory system performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("memory_capacity", 10000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Performance test: Large-scale memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Store many working memory entries
    for (int i = 0; i < 1000; ++i) {
        std::string data = "working_memory_entry_" + std::to_string(i);
        workingMemory->updateWorkingMemory(data);
    }
    
    // Store many episodic memories
    for (int i = 0; i < 500; ++i) {
        std::string experience = "episodic_memory_" + std::to_string(i);
        episodicMemory->storeExperience(experience);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    assert(duration.count() < 5000); // 5 seconds max for 1500 operations
    
    // Test retrieval performance
    startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        workingMemory->getWorkingMemory("working_memory_entry_" + std::to_string(i % 1000));
        episodicMemory->getEpisode(i % 500);
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    assert(duration.count() < 1000); // 1 second max for 200 retrievals
    
    std::cout << "    testMemorySystemPerformance passed" << std::endl;
}

void testPredictionSystemStress() {
    std::cout << "    Testing prediction system under stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Stress test: Rapid prediction error generation and learning
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    for (int i = 0; i < 500; ++i) {
        std::vector<float> predicted(10);
        std::vector<float> actual(10);
        
        for (int j = 0; j < 10; ++j) {
            predicted[j] = dis(gen);
            actual[j] = dis(gen);
        }
        
        float error = predictionSystem->computePredictionError(predicted, actual);
        predictionSystem->updatePrediction(predicted);
        
        // Verify error tracking
        auto errorHistory = predictionSystem->getPredictionErrorHistory();
        assert(errorHistory.size() > 0);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should handle 500 prediction cycles within reasonable time
    assert(duration.count() < 3000); // 3 seconds max
    
    std::cout << "    testPredictionSystemStress passed" << std::endl;
}

void testIntegrationOverhead() {
    std::cout << "    Testing integration system overhead..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure baseline (just brain steps)
    auto baselineStart = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    auto baselineEnd = std::chrono::high_resolution_clock::now();
    auto baselineDuration = std::chrono::duration_cast<std::chrono::milliseconds>(baselineEnd - baselineStart);
    
    // Measure with integrated systems (neuromodulation, memory, prediction)
    auto integrationStart = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Apply neuromodulation
        nlm::Neuromodulator curiositySignal(nlm::Curiosity::Create(0.5f, 0.3f));
        brain.applyNeuromodulation(curiositySignal);
        
        // Update prediction system
        auto* predictionSystem = brain.getPredictionSystem();
        if (predictionSystem) {
            predictionSystem->updatePrediction({0.5f, 0.5f, 0.5f});
        }
        
        // Update working memory
        brain.getWorkingMemory()->updateWorkingMemory("integration_test");
        
        // Step brain
        brain.step(step);
        
        // Apply development
        brain.develop();
    }
    
    auto integrationEnd = std::chrono::high_resolution_clock::now();nlm::SimulationStep step = 0; ++step) {
        brain.step(step);
    }
    auto baselineEnd = std::chrono::high_resolution_clock::now();
    auto baselineDuration = std::chrono::duration_cast<std::chrono::milliseconds>(baselineEnd - baselineStart);
    
    auto integrationEnd = std::chrono::high_resolution_clock::now();
    auto integrationDuration = std::chrono::duration_cast<std::chrono::milliseconds>(integrationEnd - integrationStart);
    
    // Integration overhead should be reasonable (not more than 5x baseline)
    double overheadRatio = static_cast<double>(integrationDuration.count()) / baselineDuration.count();
    assert(overheadRatio < 5.0); // Should not be more than 5x slower
    
    std::cout << "    testIntegrationOverhead passed" << std::endl;
}

void testConcurrentSystemAccess() {
    std::cout << "    Testing concurrent system access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Simulate concurrent access to different systems
    std::vector<std::thread> threads;
    
    auto memoryAccess = [&]() {
        auto* workingMemory = brain.getWorkingMemory();
        for (int i = 0; i < 50; ++i) {
            workingMemory->updateWorkingMemory("thread_memory_" + std::to_string(i));
        }
    };
    
    auto predictionAccess = [&]() {
        auto* predictionSystem = brain.getPredictionSystem();
        for (int i = 0; i < 50; ++i) {
            if (predictionSystem) {
                predictionSystem->updatePrediction({0.1f, 0.2f, 0.3f});
            }
        }
    };
    
    auto neuromodulationAccess = [&]() {
        auto* curiosity = brain.getCuriosity();
        for (int i = 0; i < 50; ++i) {
            nlm::Neuromodulator signal(nlm::Curiosity::Create(0.3f, 0.2f));
            brain.applyNeuromodulation(signal);
        }
    };
    
    // Start concurrent access
    threads.emplace_back(memoryAccess);
    threads.emplace_back(predictionAccess);
    threads.emplace_back(neuromodulationAccess);
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify systems still functional
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    
    std::cout << "    testConcurrentSystemAccess passed" << std::endl;
}

void runAll() {
    std::cout << "=== Performance and Stress Tests ===" << std::endl;
    testMemorySystemPerformance();
    testPredictionSystemStress();
    testIntegrationOverhead();
    testConcurrentSystemAccess();
    std::cout << "=== Performance and Stress Tests Complete ===" << std::endl;
}

} // namespace test_performance