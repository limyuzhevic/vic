// Performance and Stress Testing
// Tests brain performance under load and stress conditions
// Validates scalability, memory management, and system stability

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <memory>
#include <limits>

namespace test_performance_stress {

void testPerformanceWithLargeNetwork() {
    std::cout << "    Testing performance with large neural network..." << std::endl;
    
    // Test with increasing network sizes
    std::vector<size_t> neuronCounts = {200, 500, 1000, 2000};
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "    Testing with " << neuronCount << " neurons..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(std::max<size_t>(2, neuronCount / 200)), nlm::ConfigSource::Runtime);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
            
            if (step % 10 == 0) {
                brain.updatePlasticity();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "      " << neuronCount << " neurons: " << duration.count() << "ms" << std::endl;
        
        // Performance should be reasonable (not too slow)
        assert(duration.count() < 5000); // Should complete in 5 seconds or less
        assert(brain.getTotalNeuronCount() == neuronCount);
    }
    
    std::cout << "    testPerformanceWithLargeNetwork passed" << std::endl;
}

void testMemoryUsageAndScaling() {
    std::cout << "    Testing memory usage and scaling..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    
    // Fill memory systems
    size_t maxMemoryItems = 1000;
    
    std::cout << "    Testing working memory scaling..." << std::endl;
    for (size_t i = 0; i < maxMemoryItems; ++i) {
        if (workingMem) {
            workingMem->store(i, 0.5f + 0.5f * std::sin(i * 0.1f));
        }
    }
    
    assert(workingMem->getCurrentSize() == maxMemoryItems);
    assert(workingMem->getCapacity() >= maxMemoryItems);
    
    std::cout << "    Testing episodic memory scaling..." << std::endl;
    for (size_t i = 0; i < 500; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons = {static_cast<nlm::NeuronId>(100 + i), static_cast<nlm::NeuronId>(200 + i)};
        episode.values = {0.3f * (i + 1) / 500.0f, 0.7f * (i + 1) / 500.0f};
        episode.metadata = "scaling_test_episode_" + std::to_string(i);
        
        if (episodicMem) {
            episodicMem->storeEpisode(episode);
        }
    }
    
    assert(episodicMem->getEpisodeCount() == 500);
    
    std::cout << "    Memory scaling test completed" << std::endl;
    
    std::cout << "    testMemoryUsageAndScaling passed" << std::endl;
}

void testStressConditions() {
    std::cout << "    Testing stress conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    
    // Stress test: High neuromodulation levels
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        double currentTime = step * 0.05;
        
        // Extreme neuromodulation
        if (dopamine) {
            // High reward signals
            dopamine->signalReward(2.0f + 1.0f * std::sin(step * 0.5f));
            dopamine->update(0.2f); // Large update
            
            // Apply extreme neuromodulation
            brain.applyNeuromodulation(*dopamine);
        }
        
        if (curiosity) {
            // Very high curiosity
            curiosity->setLevel(2.0f);
            curiosity->update(0.2f);
            
            brain.applyNeuromodulation(*curiosity);
        }
        
        // Extreme prediction error
        if (predError) {
            predError->setPredictionError(1.5f + 0.5f * std::cos(step * 0.3f));
        }
        
        // Extreme plasticity
        if (hebbian) {
            hebbian->setLearningRate(2.0f);
            hebbian->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        if (stdp) {
            stdp->setLearningRate(2.0f);
            stdp->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Stress memory system
        if (workingMem) {
            workingMem->store(step % 100, 2.0f); // Large values
            workingMem->decay(0.5f); // Fast decay
        }
        
        // Brain step with extreme conditions
        brain.step(step, currentTime);
        brain.updatePlasticity();
        
        // System must remain stable
        assert(brain.getTotalNeuronCount() == 800);
        assert(brain.getRegionCount() == 4);
        
        // Check that systems don't crash
        if (dopamine) {
            assert(dopamine->getLevel() >= 0.0f);
        }
        if (curiosity) {
            assert(curiosity->getLevel() >= 0.0f);
        }
    }
    
    std::cout << "    testStressConditions passed" << std::endl;
}

void testRealTimePerformance() {
    std::cout << "    Testing real-time performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    
    // Target: Real-time processing (50-100 steps per second)
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t stepsProcessed = 0;
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        double currentTime = step * 0.01; // 10ms timestep
        
        // All subsystems process simultaneously
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        if (conceptForm) {
            conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
        }
        
        if (workingMem) {
            workingMem->store(step % 50, 0.5f + 0.3f * std::sin(step * 0.1f));
        }
        
        brain.step(step, currentTime);
        brain.updatePlasticity();
        
        stepsProcessed++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (stepsProcessed / duration.count()) * 1000.0;
    std::cout << "    Processed " << stepsProcessed << " steps in " << duration.count() << "ms" << std::endl;
    std::cout << "    Performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    // Should achieve reasonable real-time performance
    assert(stepsPerSecond > 50); // At least 50 steps per second
    assert(brain.getTotalNeuronCount() == 600);
    assert(brain.getRegionCount() == 3);
    
    std::cout << "    testRealTimePerformance passed" << std::endl;
}

void runAll() {
    testPerformanceWithLargeNetwork();
    testMemoryUsageAndScaling();
    testStressConditions();
    testRealTimePerformance();
}

} // namespace test_performance_stress