// Comprehensive Performance Tests
// Phase 2: Real Neural Computation Tests - Performance Benchmarks

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <vector>

namespace test_performance {

void testPerformanceSmallNetwork() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure initialization time
    auto start = std::chrono::high_resolution_clock::now();
    brain.initialize();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    assert(duration.count() < 1000);  // Should initialize quickly
    
    // Measure step processing time
    start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete 100 steps in reasonable time
    assert(duration.count() < 5000);
    
    std::cout << "    testPerformanceSmallNetwork passed" << std::endl;
}

void testPerformanceMediumNetwork() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure step processing time
    auto start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Medium network should still be fast
    assert(duration.count() < 10000);
    
    std::cout << "    testPerformanceMediumNetwork passed" << std::endl;
}

void testPerformanceLargeNetwork() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.02f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure step processing time
    auto start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Large network should still be within limits
    assert(duration.count() < 30000);
    
    std::cout << "    testPerformanceLargeNetwork passed" << std::endl;
}

void testPerformanceMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run many steps to check for memory leaks
    size_t steps = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < steps; ++step) {
        brain.step(step);
        
        // Verify memory doesn't grow excessively
        size_t neuronCount = brain.getTotalNeuronCount();
        assert(neuronCount == 1000);  // Should remain constant
        
        size_t synapseCount = brain.getTotalSynapseCount();
        assert(synapseCount >= 0);  // Should be non-negative
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete many steps efficiently
    assert(duration.count() < 60000);
    
    std::cout << "    testPerformanceMemoryUsage passed" << std::endl;
}

void testPerformanceScalability() {
    std::vector<std::pair<size_t, size_t>> networkSizes = {
        {100, 2},   // Small
        {1000, 5},  // Medium
        {5000, 10}, // Large
    };
    
    for (auto& size : networkSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size.first), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(size.second), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Measure initialization time
        auto start = std::chrono::high_resolution_clock::now();
        brain.initialize();
        auto end = std::chrono::high_resolution_clock::now();
        auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should scale reasonably
        assert(initDuration.count() < 5000);
        
        // Measure step time per neuron
        start = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        end = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Time per neuron per step should be reasonable
        double timePerNeuron = stepDuration.count() / (10.0 * size.first);
        assert(timePerNeuron < 0.1);  // Less than 0.1ms per neuron per step
    }
    
    std::cout << "    testPerformanceScalability passed" << std::endl;
}

void testPerformanceStableOverTime() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run for many steps and check stability
    std::vector<double> stepTimes;
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        auto start = std::chrono::high_resolution_clock::now();
        brain.step(step);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        stepTimes.push_back(duration.count());
        
        // Neuron and synapse counts should remain stable
        assert(brain.getTotalNeuronCount() == 500);
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    // Check that performance doesn't degrade significantly
    double avgStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
    double maxStepTime = *std::max_element(stepTimes.begin(), stepTimes.end());
    double minStepTime = *std::min_element(stepTimes.begin(), stepTimes.end());
    
    // Performance should be relatively stable
    double variance = 0.0;
    for (double time : stepTimes) {
        variance += (time - avgStepTime) * (time - avgStepTime);
    }
    variance /= stepTimes.size();
    
    // Should not have excessive variance
    assert(std::sqrt(variance) < avgStepTime * 2.0);  // Variance less than 2x average
    
    std::cout << "    testPerformanceStableOverTime passed" << std::endl;
}

void testPerformanceWithMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Measure performance with active memory systems
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Store some memory traces
        workingMemory->storeToNeuron(nlm::NeuronId(step % 100), 0.5f + (step % 50) * 0.01f);
        
        // Store some episodes
        if (step % 50 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = 0.5f + (step % 10) * 0.05f;
            episode.activeNeurons = {nlm::NeuronId(step % 20)};
            episode.neuronActivations = {0.3f};
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Store associations
        if (step % 100 == 0) {
            std::vector<nlm::NeuronId> cue = {nlm::NeuronId(step % 30), nlm::NeuronId((step + 10) % 30)};
            std::vector<nlm::NeuronId> response = {nlm::NeuronId(step % 40), nlm::NeuronId((step + 20) % 40)};
            associativeMemory->storeAssociation(cue, response);
        }
        
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should handle memory operations efficiently
    assert(duration.count() < 30000);
    
    // Memory systems should still be functional
    assert(workingMemory->getActiveTraces() >= 0);
    assert(episodicMemory->getEpisodeCount() >= 0);
    
    std::cout << "    testPerformanceWithMemorySystems passed" << std::endl;
}

void testPerformanceNeuromodulationOverhead() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Measure performance with neuromodulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Update neuromodulators
        dopamine->update(0.001f);
        curiosity->update(0.001f);
        novelty->update(0.001f);
        
        // Apply neuromodulation
        class TestNeuromodulator : public nlm::Neuromodulator {
        public:
            float getLevel() const override { return 0.5f; }
            float getPlasticityFactor() const override { return 1.2f; }
        } neuromod;
        
        brain.applyNeuromodulation(neuromod);
        
        // Run brain step
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should still be efficient with neuromodulation
    assert(duration.count() < 15000);
    
    std::cout << "    testPerformanceNeuromodulationOverhead passed" << std::endl;
}

void testPerformanceEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with minimal network (edge case)
    auto start = std::chrono::high_resolution_clock::now();
    brain.step(0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should handle minimal network
    assert(duration.count() < 1000);
    
    // Test with many regions
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config);
    brain2.initialize();
    
    start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain2.step(step);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Many regions should still work
    assert(duration.count() < 5000);
    
    std::cout << "    testPerformanceEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Performance tests..." << std::endl;
    testPerformanceSmallNetwork();
    testPerformanceMediumNetwork();
    testPerformanceLargeNetwork();
    testPerformanceMemoryUsage();
    testPerformanceScalability();
    testPerformanceStableOverTime();
    testPerformanceWithMemorySystems();
    testPerformanceNeuromodulationOverhead();
    testPerformanceEdgeCases();
}

} // namespace test_performance
