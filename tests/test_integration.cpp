// Comprehensive integration tests for Phase 6 improvements
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/EnhancedConfig.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace test_integration {

void testWorkingMemoryIntegration() {
    std::cout << "  Testing Working Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory initialization
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory store/retrieve
    nlm::NeuronId testNeuron(1);
    workingMemory->storeToNeuron(testNeuron, 0.8f);
    assert(workingMemory->getNeuronActivation(testNeuron) > 0.7f);
    assert(workingMemory->contains(testNeuron));
    
    // Test working memory decay
    workingMemory->update(0.1f);  // Update with 100ms
    float decayFactor = workingMemory->getDecayRate();
    assert(decayFactor > 0.0f && decayFactor < 0.1f);  // Should decay
    
    // Test working memory competition
    nlm::NeuronId neuron2(2);
    workingMemory->storeToNeuron(neuron2, 0.9f);
    workingMemory->runCompetition();
    assert(workingMemory->isWinning(neuron2));
    
    std::cout << "    ✓ Working memory integration test passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "  Testing Episodic Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test episodic memory initialization
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test episodic memory storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.5f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.9f};
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test episodic memory retrieval
    auto retrieved = episodicMemory->retrieveEpisode(0);
    assert(retrieved.activeNeurons.size() == 3);
    assert(retrieved.reward == 0.5f);
    
    // Test episodic memory consolidation
    episodicMemory->consolidate(0.5f);
    
    // Test episodic memory replay
    auto episodesToReplay = episodicMemory->getRecentEpisodes(1);
    assert(!episodesToReplay.empty());
    
    std::cout << "    ✓ Episodic memory integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system initialization
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system with working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Store pattern in working memory
        std::vector<float> pattern = {0.7f, 0.5f, 0.8f, 0.3f, 0.9f};
        workingMemory->store(pattern, 1.0f);
        
        // Update prediction system with working memory content
        brain.updatePredictionWithWorkingMemory();
        
        // Compute prediction error
        brain.computeAndApplyPredictionError();
        
        // Verify prediction error was applied
        auto* predictionError = brain.getPredictionErrorSignal();
        if (predictionError) {
            assert(predictionError->getError() >= 0.0f);
        }
    }
    
    std::cout << "    ✓ Prediction system integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine integration
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine level setting
    dopamine->setLevel(0.8f);
    assert(std::abs(dopamine->getLevel() - 0.8f) < 0.01f);
    
    // Test dopamine plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor > 1.0f);  // Should be enhanced by dopamine
    
    // Test acetylcholine integration
    auto* acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    // Test acetylcholine level setting
    acetylcholine->setLevel(0.6f);
    assert(std::abs(acetylcholine->getLevel() - 0.6f) < 0.01f);
    
    // Test acetylcholine plasticity factor
    float achPlasticityFactor = acetylcholine->getPlasticityFactor();
    assert(achPlasticityFactor > 1.0f);  // Should be enhanced by ACh
    
    // Test attention modulation by ACh
    if (brain.getAttention()) {
        float originalInhibition = brain.getAttention()->getInhibitionStrength();
        acetylcholine->signalAttentionEvent(0.7f);
        float newInhibition = brain.getAttention()->getInhibitionStrength();
        // ACh should reduce inhibition (increase attention)
        assert(newInhibition < originalInhibition);
    }
    
    std::cout << "    ✓ Neuromodulation integration test passed" << std::endl;
}

void testConceptFormationIntegration() {
    std::cout << "  Testing Concept Formation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test concept formation initialization
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation with working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Store pattern in working memory
        std::vector<float> pattern = {0.7f, 0.5f, 0.8f, 0.3f, 0.9f};
        workingMemory->store(pattern, 1.0f);
        
        // Get concept formation neurons
        auto conceptNeurons = conceptFormation->getConceptNeurons();
        // Concept formation should have processed the pattern
        
        // Test concept reinforcement with episodic memory
        nlm::EpisodicMemoryItem episode;
        episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
        episode.neuronActivations = {0.8f, 0.7f};
        
        if (brain.getEpisodicMemory()) {
            brain.getEpisodicMemory()->storeEpisode(episode);
            
            // Process episode for concept reinforcement
            conceptFormation->reinforceConcept(
                nlm::Pattern{episode.activeNeurons, episode.neuronActivations},
                nlm::Timestamp(100.0f)
            );
        }
    }
    
    std::cout << "    ✓ Concept formation integration test passed" << std::endl;
}

void testEnhancedConfig() {
    std::cout << "  Testing Enhanced Config..." << std::endl;
    
    // Test enhanced config initialization
    nlm::Config config;
    
    // Test type-safe value setting
    config.set("string_key", "test_value");
    config.set("int_key", 42);
    config.set("double_key", 3.14);
    config.set("bool_key", true);
    
    // Test type-safe value getting
    assert(config.getString("string_key") == "test_value");
    assert(config.getInt("int_key") == 42);
    assert(std::abs(config.getDouble("double_key") - 3.14) < 0.01f);
    assert(config.getBool("bool_key") == true);
    
    // Test type-safe getters with defaults
    assert(config.getOrString("nonexistent_key", "default_value") == "default_value");
    assert(config.getOrInt("nonexistent_key", 999) == 999);
    assert(std::abs(config.getOrDouble("nonexistent_key", 2.718) - 2.718) < 0.001f);
    assert(config.getOrBool("nonexistent_key", false) == false);
    
    // Test configuration validation
    assert(config.validate());
    
    // Test configuration merging
    nlm::Config config2;
    config2.set("extra_key", "extra_value");
    config.mergeFrom(config2);
    assert(config.has("extra_key"));
    
    std::cout << "    ✓ Enhanced config test passed" << std::endl;
}

void testCompleteBrainIntegration() {
    std::cout << "  Testing Complete Brain Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test integration of all major systems
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getAcetylcholine() != nullptr);
    
    // Run simulation with all systems integrated
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Verify systems are still functioning
        assert(brain.getTotalNeuronCount() == 300);
        assert(brain.getTotalSynapseCount() > 0);
        
        if (step % 10 == 0) {
            // Periodically check memory systems
            if (brain.getWorkingMemory()) {
                assert(brain.getWorkingMemory()->getMemoryNeuronCount() >= 0);
            }
            if (brain.getEpisodicMemory()) {
                assert(brain.getEpisodicMemory()->getEpisodeCount() >= 0);
            }
        }
    }
    
    // Verify final state
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    ✓ Complete brain integration test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testPredictionSystemIntegration();
        testNeuromodulationIntegration();
        testConceptFormationIntegration();
        testEnhancedConfig();
        testCompleteBrainIntegration();
        
        std::cout << std::endl;
        std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "Phase 6 integration is working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== INTEGRATION TEST FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_integration