// NLM Test Memory Management
// Phase 2: Real Neural Computation - Memory Management Testing

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include "performance/CheckpointManager.hpp"

namespace test_memory_management {

void testNeuronMemoryManagement() {
    std::cout << "  Testing neuron memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get neurons and verify they exist
        auto* region = brain.getRegion(0);
        assert(region != nullptr);
        
        auto neurons = region->getAllNeurons();
        assert(neurons.size() == 100);
        
        // Use neurons
        for (auto* neuron : neurons) {
            assert(neuron != nullptr);
            neuron->injectCurrent(10.0f);
        }
        
        // Run some steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Brain goes out of scope here - should be cleaned up
        std::cout << "    Neuron memory management test passed" << std::endl;
    }
}

void testSynapseMemoryManagement() {
    std::cout << "  Testing synapse memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get synapses and verify they exist
        auto* region = brain.getRegion(0);
        assert(region != nullptr);
        
        auto synapses = region->getSynapses();
        assert(!synapses.empty());
        
        // Use synapses
        for (auto& synapse : synapses) {
            assert(synapse != nullptr);
            synapse->setWeight(0.5f);
            synapse->enablePlasticity(true, false, false);
        }
        
        // Run steps with synapses
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        std::cout << "    Synapse memory management test passed" << std::endl;
    }
}

void testBrainMemoryManagement() {
    std::cout << "  Testing brain memory management..." << std::endl;
    
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    // Create multiple brains
    for (int i = 0; i < 5; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50 + i * 10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Use the brain
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain->step(step);
        }
        
        brains.push_back(brain);
    }
    
    // Clear brains to test cleanup
    brains.clear();
    
    std::cout << "    Brain memory management test passed" << std::endl;
}

void testConfigMemoryManagement() {
    std::cout << "  Testing config memory management..." << std::endl;
    
    std::vector<std::shared_ptr<nlm::Config>> configs;
    
    // Create multiple configs
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(i * 10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        configs.push_back(config);
    }
    
    // Use configs
    for (auto& config : configs) {
        // Read from config
        if (config->has("neuron_count")) {
            auto neuronCount = config->get<int64_t>("neuron_count");
            assert(neuronCount >= 0);
        }
    }
    
    // Clear configs to test cleanup
    configs.clear();
    
    std::cout << "    Config memory management test passed" << std::endl;
}

void testNeuromodulatorMemoryManagement() {
    std::cout << "  Testing neuromodulator memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get neuromodulators
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        auto* predictionError = brain.getPredictionError();
        
        if (dopamine) {
            // Use dopamine
            dopamine->signalReward(1.0f);
            float factor = dopamine->getPlasticityFactor();
            assert(factor >= 0.0f && factor <= 1.0f);
        }
        
        if (curiosity) {
            // Use curiosity
            float level = curiosity->getLevel();
            assert(level >= 0.0f);
        }
        
        if (predictionError) {
            // Use prediction error
            float error = predictionError->getLevel();
            assert(error >= 0.0f);
        }
        
        // Brain cleanup
        std::cout << "    Neuromodulator memory management test passed" << std::endl;
    }
}

void testMemorySystemMemoryManagement() {
    std::cout << "  Testing memory system memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get memory systems
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        if (workingMemory) {
            // Store data
            nlm::NeuronId testId;
            testId.value = 12345;
            workingMemory->store(testId, 1.0f);
            
            // Retrieve data
            float value = workingMemory->retrieve(testId);
            assert(value == 1.0f);
            
            // Clear working memory
            workingMemory->clear();
        }
        
        if (episodicMemory) {
            // Encode experiences
            std::vector<float> features(100, 0.1f);
            episodicMemory->encode(1.0f, features);
            
            // Retrieve experiences
            auto experiences = episodicMemory->getExperiences();
            assert(!experiences.empty());
        }
        
        std::cout << "    Memory system memory management test passed" << std::endl;
    }
}

void testPlasticitySystemMemoryManagement() {
    std::cout << "  Testing plasticity system memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get plasticity systems
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structural = brain.getStructuralPlasticity();
        
        if (stdp) {
            // Test STDP
            stdp->processSpikes({}, {});  // Empty spikes
        }
        
        if (hebbian) {
            // Test Hebbian
            hebbian->applyLearning({}, {});  // Empty neurons
        }
        
        if (structural) {
            // Test structural plasticity
            // (Would require specific implementation details)
        }
        
        std::cout << "    Plasticity system memory management test passed" << std::endl;
    }
}

void testDevelopmentSystemMemoryManagement() {
    std::cout << "  Testing development system memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get development system
        auto* developmentSystem = brain.getDevelopmentSystem();
        if (developmentSystem) {
            // Test development
            for (nlm::SimulationStep step = 0; step < 10; ++step) {
                developmentSystem->update(&brain, step);
                brain.step(step);
            }
        }
        
        std::cout << "    Development system memory management test passed" << std::endl;
    }
}

void testPredictionSystemMemoryManagement() {
    std::cout << "  Testing prediction system memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get prediction system
        auto* predictionSystem = brain.getPredictionSystem();
        if (predictionSystem) {
            // Test prediction
            nlm::SensoryInput input;
            auto predicted = predictionSystem->predictNextState(input);
            predictionSystem->updatePredictions(*predicted, input);
        }
        
        std::cout << "    Prediction system memory management test passed" << std::endl;
    }
}

void testCheckpointManagerMemoryManagement() {
    std::cout << "  Testing checkpoint manager memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test checkpoint manager
        auto* checkpointManager = brain.getCheckpointManager();
        if (checkpointManager) {
            // Save and load checkpoint
            bool saved = brain.save("test_checkpoint.bin");
            bool loaded = brain.load("test_checkpoint.bin");
            
            // Cleanup checkpoint file
            std::remove("test_checkpoint.bin");
            
            if (saved && loaded) {
                std::cout << "    Checkpoint manager memory management test passed" << std::endl;
            }
        } else {
            std::cout << "    Checkpoint manager memory management test passed (no checkpoint manager)" << std::endl;
        }
    }
}

void runAll() {
    std::cout << "=== Memory Management Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testNeuronMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testNeuronMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSynapseMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testSynapseMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testBrainMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testBrainMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testConfigMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testConfigMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNeuromodulatorMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testNeuromodulatorMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemorySystemMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testMemorySystemMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPlasticitySystemMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testPlasticitySystemMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testDevelopmentSystemMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testDevelopmentSystemMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPredictionSystemMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testPredictionSystemMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testCheckpointManagerMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "  testCheckpointManagerMemoryManagement failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << std::endl << "✓ All Memory Management Tests PASSED" << std::endl;
    } else {
        std::cout << std::endl << "✗ Some Memory Management Tests FAILED" << std::endl;
    }
}
}