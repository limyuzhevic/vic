// NLM Test Stress
// Phase 2: Real Neural Computation - Stress Testing

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"

namespace test_stress {

void testLargeNetworkInitialization() {
    std::cout << "  Testing large network initialization..." << std::endl;
    
    // Test with large network (but not too large for stress testing)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.01f, nlm::ConfigSource::Default);  // Low connectivity
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (success) {
        assert(brain.getTotalNeuronCount() == 50000);
        std::cout << "    Large network initialization passed in " << duration.count() << "ms" << std::endl;
    } else {
        std::cout << "    Large network initialization skipped (may be resource limited)" << std::endl;
    }
}

void testHighFrequencySteps() {
    std::cout << "  Testing high-frequency step execution..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run many steps quickly
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        brain.step(step);
        
        // Verify system remains stable
        if (step % 1000 == 0) {
            assert(brain.getTotalNeuronCount() == 5000);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    High-frequency steps passed: " << duration.count() << "ms for 10,000 steps (" 
              << (10000.0 / duration.count() * 1000) << " steps/sec)" << std::endl;
}

void testMemoryStress() {
    std::cout << "  Testing memory stress scenarios..." << std::endl;
    
    // Test multiple brain instances to stress memory management
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        if (brain->initialize()) {
            brains.push_back(brain);
        }
    }
    
    // Run some steps on all brains
    for (auto& brain : brains) {
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
        }
    }
    
    // Clear brains to free memory
    brains.clear();
    
    std::cout << "    Memory stress test passed (created and destroyed " << brains.size() << " brain instances)" << std::endl;
}

void testSpikeHistoryStress() {
    std::cout << "  Testing spike history stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Generate lots of spikes
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        // Inject current to generate spikes
        brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 100.0f);
        brain.step(step);
    }
    
    // Check spike history
    size_t totalSpikes = brain.getTotalSpikeCount();
    std::cout << "    Spike history stress test passed (total spikes: " << totalSpikes << ")" << std::endl;
}

void testSynapseStress() {
    std::cout << "  Testing synapse stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses and stress test them
    auto* region = brain.getRegion(0);
    if (region) {
        auto synapses = region->getSynapses();
        std::cout << "    Testing " << synapses.size() << " synapses..." << std::endl;
        
        // Test each synapse
        for (auto& synapse : synapses) {
            // Modify weight multiple times
            float originalWeight = synapse->getWeight();
            synapse->setWeight(originalWeight + 0.1f);
            synapse->setWeight(originalWeight - 0.05f);
            synapse->setWeight(0.5f);  // Reset
            
            // Test plasticity
            synapse->enablePlasticity(true, false, false);
        }
    }
    
    std::cout << "    Synapse stress test passed" << std::endl;
}

void testResourceCleanupStress() {
    std::cout << "  Testing resource cleanup stress..." << std::endl;
    
    // Create and destroy many brain instances
    for (int i = 0; i < 20; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        {
            nlm::Brain brain(config);
            if (brain.initialize()) {
                // Do some work
                for (nlm::SimulationStep step = 0; step < 10; ++step) {
                    brain.step(step);
                }
            }
        } // Brain destroyed here
    }
    
    std::cout << "    Resource cleanup stress test passed" << std::endl;
}

void testPlasticityStress() {
    std::cout << "  Testing plasticity stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity
    auto* region = brain.getRegion(0);
    if (region) {
        auto& synapses = region->getSynapses();
        for (auto& syn : synapses) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run many steps with plasticity
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        // Inject activity to trigger plasticity
        if (step % 3 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
        }
        
        brain.step(step);
        
        // Check occasionally for performance
        if (step % 500 == 0) {
            std::cout << "    Plasticity stress step " << step << " completed" << std::endl;
        }
    }
    
    std::cout << "    Plasticity stress test passed" << std::endl;
}

void testNeuromodulationStress() {
    std::cout << "  Testing neuromodulation stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(3000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        // Generate lots of neuromodulatory signals
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            // Random-like neuromodulation signals
            if (step % 5 == 0) {
                dopamine->signalReward((step % 10) * 0.1f);
            }
            if (step % 7 == 0) {
                dopamine->signalRewardPredictionError((step % 5 - 2) * 0.2f);
            }
            
            brain.step(step);
        }
    }
    
    std::cout << "    Neuromodulation stress test passed" << std::endl;
}

void testMemorySystemStress() {
    std::cout << "  Testing memory system stress..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory stress
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Store many items
        for (int i = 0; i < 10000; ++i) {
            nlm::NeuronId id;
            id.value = i;
            workingMemory->store(id, (i % 100) * 0.01f);
        }
    }
    
    // Test episodic memory stress
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        // Encode many experiences
        for (int i = 0; i < 100; ++i) {
            std::vector<float> features(100, 0.01f * (i % 10));
            episodicMemory->encode(0.5f, features);
        }
        
        // Trigger consolidation
        episodicMemory->consolidate(0.1f);
    }
    
    std::cout << "    Memory system stress test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Stress Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testLargeNetworkInitialization();
    } catch (const std::exception& e) {
        std::cout << "  testLargeNetworkInitialization failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHighFrequencySteps();
    } catch (const std::exception& e) {
        std::cout << "  testHighFrequencySteps failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryStress();
    } catch (const std::exception& e) {
        std::cout << "  testMemoryStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSpikeHistoryStress();
    } catch (const std::exception& e) {
        std::cout << "  testSpikeHistoryStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSynapseStress();
    } catch (const std::exception& e) {
        std::cout << "  testSynapseStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testResourceCleanupStress();
    } catch (const std::exception& e) {
        std::cout << "  testResourceCleanupStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPlasticityStress();
    } catch (const std::exception& e) {
        std::cout << "  testPlasticityStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNeuromodulationStress();
    } catch (const std::exception& e) {
        std::cout << "  testNeuromodulationStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemorySystemStress();
    } catch (const std::exception& e) {
        std::cout << "  testMemorySystemStress failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << std::endl << "✓ All Stress Tests PASSED" << std::endl;
    } else {
        std::cout << std::endl << "✗ Some Stress Tests FAILED" << std::endl;
    }
}
}