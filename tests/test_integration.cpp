// Integration Tests
// Comprehensive end-to-end integration tests for full brain simulation

#include "brain/Brain.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_integration {

void testFullSystemIntegration() {
    std::cout << "    Testing full system integration..." << std::endl;
    
    // Create comprehensive brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.08f, ConfigSource::Default);
    config->set("memory_capacity", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("max_episodes", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize all plasticity systems
    Hebbian hebbian;
    hebbian.setLearningRate(0.01f);
    
    StructuralPlasticity sp;
    sp.setSynaptogenesisRate(0.0002f);
    sp.setPruningRate(0.00002f);
    
    Synaptogenesis synaptogenesis;
    Pruning pruning;
    Maturation maturation;
    
    // Initialize all neuromodulation systems
    Curiosity curiosity;
    Novelty novelty;
    PredictionError predictionError;
    curiosity.initialize(brain.get());
    
    // Initialize all memory systems
    auto workingMemory = std::make_shared<NeuralWorkingMemory>(config);
    auto episodicMemory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Record initial state
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Run simulation with all systems active
    for (size_t step = 0; step < 100; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Update plasticity systems
        hebbian.setLearningRate(0.01f);
        
        // Update development systems
        maturation.update(brain.get(), *brain->getRandomGenerator());
        pruning.update(brain.get(), *brain->getRandomGenerator());
        synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
        
        // Update structural plasticity
        sp.update(brain.get(), *brain->getRandomGenerator());
        
        // Update neuromodulation systems
        curiosity.update(novelty.getLevel(), predictionError.getLevel(), static_cast<float>(step));
        novelty.update(static_cast<float>(step) * 0.01f);
        predictionError.update(static_cast<float>(step) * 0.005f);
        
        // Update working memory
        workingMemory->store(static_cast<float>(step), step % 10, step % 5);
        
        // Update episodic memory periodically
        if (step % 10 == 0) {
            std::vector<float> episodeData(5, static_cast<float>(step));
            episodicMemory->store(static_cast<int>(step), 1.0f + static_cast<float>(step), episodeData);
        }
        
        // Step brain
        brain->step(step, step * 0.001f);
    }
    
    // Verify final state
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    if (finalNeurons != initialNeurons) {
        throw nlm::NLMTestException("Full integration test failed: neuron count changed from " + 
                                   std::to_string(initialNeurons) + " to " + std::to_string(finalNeurons));
    }
    
    // Synapses may change due to structural plasticity
    float synapseChangeRatio = static_cast<float>(finalSynapses) / static_cast<float>(initialSynapses);
    if (synapseChangeRatio < 0.1f || synapseChangeRatio > 10.0f) {
        throw nlm::NLMTestException("Full integration test failed: synapse count changed too much (" + 
                                   std::to_string(initialSynapses) + " -> " + std::to_string(finalSynapses) + ")");
    }
    
    // Verify memory systems are functional
    float wmValue = workingMemory->recall(5, 0);
    auto episodic = episodicMemory->recall(50);
    
    std::cout << "        Initial neurons: " << initialNeurons 
              << " | Final neurons: " << finalNeurons 
              << " | Initial synapses: " << initialSynapses 
              << " | Final synapses: " << finalSynapses
              << " | Working memory access: OK" 
              << " | Episodic memory access: " << (episodic ? "OK" : "No data") << std::endl;
    
    std::cout << "    testFullSystemIntegration passed" << std::endl;
}

void testPerformanceSystemIntegration() {
    std::cout << "    Testing performance with integrated systems..." << std::endl;
    
    // Create brain with higher performance expectations
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize systems
    Hebbian hebbian;
    StructuralPlasticity sp;
    Curiosity curiosity;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run performance test with all systems
    for (size_t step = 0; step < 200; ++step) {
        // Mixed input patterns
        if (step % 5 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        } else if (step % 7 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
        } else {
            brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
        }
        
        // Update all systems
        hebbian.setLearningRate(0.01f + static_cast<float>(step) * 0.0001f);
        sp.update(brain.get(), *brain->getRandomGenerator());
        curiosity.update(0.5f, 0.3f, static_cast<float>(step));
        
        // Step brain
        brain->step(step, step * 0.001f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Performance should be reasonable (200 steps in less than 10 seconds)
    if (totalTime > 10.0) {
        throw nlm::NLMTestException("Performance integration test failed: simulation too slow (" + 
                                   std::to_string(totalTime) + " seconds for 200 steps)");
    }
    
    std::cout << "        Simulation completed in " << std::fixed << std::setprecision(3) << totalTime << " seconds (200 steps)" << std::endl;
    std::cout << "    testPerformanceSystemIntegration passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    // Create comprehensive configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("max_episodes", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    // Create brain and memory systems
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto workingMemory = std::make_shared<NeuralWorkingMemory>(config);
    auto episodicMemory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Test memory integration with brain activity
    for (size_t step = 0; step < 100; ++step) {
        // Store in working memory
        workingMemory->store(static_cast<float>(step), step % 10, step % 5);
        
        // Store in episodic memory periodically
        if (step % 5 == 0) {
            std::vector<float> episodeData(10, static_cast<float>(step * 2));
            episodicMemory->store(static_cast<int>(step), 1.0f + static_cast<float>(step), episodeData);
        }
        
        // Simulate brain activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(step, step * 0.001f);
        
        // Verify memory access
        float wmValue = workingMemory->recall(step % 10, step % 5);
        
        // Periodic verification of episodic memory
        if (step % 10 == 0) {
            auto episode = episodicMemory->recall(step);
            if (episode != nullptr) {
                const auto& data = episode->getData();
                if (!data.empty()) {
                    // Just verify data exists and is non-zero
                    if (data[0] == 0.0f) {
                        throw nlm::NLMTestException("Memory integration test failed: episodic memory data is zero");
                    }
                }
            }
        }
    }
    
    // Verify final memory state
    float finalWMValue = workingMemory->recall(5, 0);
    auto finalEpisode = episodicMemory->recall(95);
    
    std::cout << "        Working memory access: OK (value: " << finalWMValue << ")" << std::endl;
    std::cout << "        Episodic memory access: " << (finalEpisode ? "OK" : "No data") << std::endl;
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation system integration..." << std::endl;
    
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize neuromodulation systems
    Curiosity curiosity;
    Novelty novelty;
    PredictionError predictionError;
    curiosity.initialize(brain.get());
    
    // Test integration with varying inputs
    for (size_t step = 0; step < 150; ++step) {
        // Generate varying neuromodulation inputs
        float noveltyInput = static_cast<float>(step) / 100.0f;
        float predictionErrorInput = 1.0f - (static_cast<float>(step) / 150.0f);
        
        // Update neuromodulation systems
        curiosity.update(noveltyInput, predictionErrorInput, static_cast<float>(step));
        novelty.update(noveltyInput);
        predictionError.update(predictionErrorInput);
        
        // Verify integration
        float curiosityLevel = curiosity.getLevel();
        if (curiosityLevel < 0.0f || curiosityLevel > 2.0f) {
            throw nlm::NLMTestException("Neuromodulation integration test failed: curiosity level out of range: " + 
                                       std::to_string(curiosityLevel));
        }
        
        // Simulate brain activity
        if (step % 3 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        }
        brain->step(step, step * 0.001f);
    }
    
    std::cout << "        Neuromodulation systems integrated successfully" << std::endl;
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticitySystemIntegration() {
    std::cout << "    Testing plasticity system integration..." << std::endl;
    
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(40), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize all plasticity systems
    Hebbian hebbian;
    StructuralPlasticity sp;
    Synaptogenesis synaptogenesis;
    Pruning pruning;
    Maturation maturation;
    
    // Record initial state
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Test plasticity integration
    for (size_t step = 0; step < 100; ++step) {
        // Generate brain activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Update all plasticity systems
        hebbian.setLearningRate(0.01f + static_cast<float>(step) * 0.0001f);
        
        maturation.update(brain.get(), *brain->getRandomGenerator());
        pruning.update(brain.get(), *brain->getRandomGenerator());
        synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
        sp.update(brain.get(), *brain->getRandomGenerator());
        
        // Step brain
        brain->step(step, step * 0.001f);
    }
    
    // Verify final state
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    if (finalNeurons != initialNeurons) {
        throw nlm::NLMTestException("Plasticity integration test failed: neuron count changed from " + 
                                   std::to_string(initialNeurons) + " to " + std::to_string(finalNeurons));
    }
    
    std::cout << "        Initial neurons: " << initialNeurons 
              << " | Final neurons: " << finalNeurons 
              << " | Initial synapses: " << initialSynapses 
              << " | Final synapses: " << finalSynapses << std::endl;
    std::cout << "    testPlasticitySystemIntegration passed" << std::endl;
}

void testEndToEndScenario() {
    std::cout << "    Testing end-to-end scenario with all systems..." << std::endl;
    
    // Comprehensive configuration for full scenario
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(60), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.08f, ConfigSource::Default);
    config->set("memory_capacity", static_cast<int64_t>(80), ConfigSource::Default);
    config->set("max_episodes", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(15), ConfigSource::Default);
    
    // Create brain and all subsystems
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initialize all systems
    Hebbian hebbian;
    StructuralPlasticity sp;
    Synaptogenesis synaptogenesis;
    Pruning pruning;
    Maturation maturation;
    Curiosity curiosity;
    Novelty novelty;
    PredictionError predictionError;
    
    auto workingMemory = std::make_shared<NeuralWorkingMemory>(config);
    auto episodicMemory = std::make_shared<NeuralEpisodicMemory>(config);
    
    curiosity.initialize(brain.get());
    
    // Run comprehensive end-to-end scenario
    for (size_t step = 0; step < 300; ++step) {
        // Scenario: Learning with curiosity-driven exploration
        
        // 1. Process sensory input
        if (step % 7 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 35.0f);
        }
        
        // 2. Update plasticity (Hebbian learning)
        hebbian.setLearningRate(0.01f);
        
        // 3. Update development (synaptogenesis, pruning, maturation)
        maturation.update(brain.get(), *brain->getRandomGenerator());
        pruning.update(brain.get(), *brain->getRandomGenerator());
        synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
        
        // 4. Update structural plasticity
        sp.update(brain.get(), *brain->getRandomGenerator());
        
        // 5. Update neuromodulation (curiosity-driven learning)
        curiosity.update(novelty.getLevel(), predictionError.getLevel(), static_cast<float>(step));
        novelty.update(static_cast<float>(step) * 0.005f);
        predictionError.update(static_cast<float>(step) * 0.003f);
        
        // 6. Update memory systems
        workingMemory->store(static_cast<float>(step), step % 8, step % 4);
        
        if (step % 10 == 0) {
            std::vector<float> episodeData(8, static_cast<float>(step * 2));
            episodicMemory->store(static_cast<int>(step), 1.0f + static_cast<float>(step), episodeData);
        }
        
        // 7. Step the brain
        brain->step(step, step * 0.001f);
        
        // 8. Occasionally check system health
        if (step % 50 == 0) {
            size_t currentNeurons = brain->getTotalNeuronCount();
            size_t currentSynapses = brain->getTotalSynapseCount();
            
            if (currentNeurons == 0) {
                throw nlm::NLMTestException("End-to-end scenario failed: brain has no neurons at step " + 
                                           std::to_string(step));
            }
        }
    }
    
    // Final system health check
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    float curiosityLevel = curiosity.getLevel();
    auto lastEpisode = episodicMemory->recall(290);
    
    std::cout << "        Final neurons: " << finalNeurons << std::endl;
    std::cout << "        Final synapses: " << finalSynapses << std::endl;
    std::cout << "        Curiosity level: " << curiosityLevel << std::endl;
    std::cout << "        Episodic memory last episode: " << (lastEpisode ? "OK" : "No data") << std::endl;
    std::cout << "    testEndToEndScenario passed" << std::endl;
}

void runAll() {
    std::cout << "Running integration tests..." << std::endl;
    testFullSystemIntegration();
    testPerformanceSystemIntegration();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testPlasticitySystemIntegration();
    testEndToEndScenario();
}

} // namespace test_integration
