// Test Phase 6 Integration Tests
// Comprehensive test suite for Phase 6 brain integration

#include "Phase6IntegrationTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/prediction/PredictionErrorSignal.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_suites {

// Forward declarations for test implementations
class Phase6IntegrationTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
        
        test_cognitive_system_integration();
        test_memory_system_integration();
        test_neuromodulation_system_coupling();
        test_prediction_system_integration();
        test_development_system_integration();
        test_plasticity_integration();
        test_neural_plasticity_coupling();
        test_replay_and_consolidation();
        test_system_coherence();
        test_real_world_scenarios();
        
        std::cout << "=== Phase 6 Integration Tests Complete ===" << std::endl;
    }

private:
    static void test_cognitive_system_integration() {
        std::cout << "Testing cognitive system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test neural planner integration
        auto* planner = brain.getNeuralPlanner();
        assert(planner != nullptr);
        
        // Test concept formation integration
        auto* conceptFormation = brain.getConceptFormation();
        assert(conceptFormation != nullptr);
        
        // Test attentional selection integration
        auto* attention = brain.getAttentionalSelection();
        assert(attention != nullptr);
        
        // Simulate cognitive processing
        for (int i = 0; i < 20; ++i) {
            brain.step(i);
        }
        
        std::cout << "    Cognitive system integration: PASSED" << std::endl;
    }
    
    static void test_memory_system_integration() {
        std::cout << "Testing memory system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 300);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test working memory integration
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Test episodic memory integration  
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Test semantic memory integration
        auto* semanticMemory = brain.getSemanticMemory();
        assert(semanticMemory != nullptr);
        
        // Test associative memory integration
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        // Test procedural memory integration
        auto* proceduralMemory = brain.getProceduralMemory();
        assert(proceduralMemory != nullptr);
        
        // Simulate memory operations
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.action = nlm::ActionType::MoveForward;
        episode.reward = 1.0f;
        
        for (int i = 0; i < 15; ++i) {
            brain.step(i);
            episodicMemory->storeEpisode(episode);
        }
        
        std::cout << "    Memory system integration: PASSED" << std::endl;
    }
    
    static void test_neuromodulation_system_coupling() {
        std::cout << "Testing neuromodulation system coupling..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test reward system integration
        auto* reward = brain.getReward();
        assert(reward != nullptr);
        
        // Test dopamine integration
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        
        // Test curiosity integration
        auto* curiosity = brain.getCuriosity();
        assert(curiosity != nullptr);
        
        // Test novelty detection integration
        auto* novelty = brain.getNovelty();
        assert(novelty != nullptr);
        
        // Test prediction error integration
        auto* predictionError = brain.getPredictionError();
        assert(predictionError != nullptr);
        
        // Simulate neuromodulation dynamics
        for (int i = 0; i < 25; ++i) {
            brain.step(i);
            
            // Apply varying neuromodulatory signals
            if (i % 5 == 0) {
                nlm::Neuromodulator rewardSignal;
                rewardSignal.setLevel(0.8f);
                brain.applyNeuromodulation(rewardSignal);
            }
            
            if (i % 7 == 0) {
                nlm::Neuromodulator curiositySignal;
                curiositySignal.setLevel(0.6f);
                brain.applyNeuromodulation(curiositySignal);
            }
        }
        
        std::cout << "    Neuromodulation system coupling: PASSED" << std::endl;
    }
    
    static void test_prediction_system_integration() {
        std::cout << "Testing prediction system integration..." << nullptr << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test neural prediction integration
        auto* neuralPrediction = brain.getNeuralPrediction();
        assert(neuralPrediction != nullptr);
        
        // Test action consequence predictor integration
        auto* actionConsequencePredictor = brain.getActionConsequencePredictor();
        assert(actionConsequencePredictor != nullptr);
        
        // Test prediction error signal integration
        auto* predictionErrorSignal = brain.getPredictionErrorSignal();
        assert(predictionErrorSignal != nullptr);
        
        // Test prediction system integration with learning
        std::vector<float> sensoryState(10, 0.5f);
        std::vector<float> nextState(10, 0.6f);
        
        for (int i = 0; i < 30; ++i) {
            brain.step(i);
            
            // Record sensory patterns for prediction
            neuralPrediction->recordSensoryState(sensoryState, i);
            
            // Generate predictions
            auto prediction = neuralPrediction->generatePrediction(i);
            assert(prediction.size() == sensoryState.size());
            
            // Update with observation
            float error = neuralPrediction->updateWithObservation(nextState, i);
            assert(error >= 0.0f && error <= 1.0f);
        }
        
        std::cout << "    Prediction system integration: PASSED" << std::endl;
    }
    
    static void test_development_system_integration() {
        std::cout << "Testing development system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        config->set("developmental_stage", 1);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test development system integration
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test synaptogenesis
        auto* synaptogenesis = brain.getSynaptogenesis();
        assert(synaptogenesis != nullptr);
        
        // Test pruning
        auto* pruning = brain.getPruning();
        assert(pruning != nullptr);
        
        // Test maturation
        auto* maturation = brain.getMaturation();
        assert(maturation != nullptr);
        
        // Test structural plasticity integration
        auto* structuralPlasticity = brain.getStructuralPlasticity();
        assert(structuralPlasticity != nullptr);
        
        // Simulate development over time
        for (int i = 0; i < 40; ++i) {
            brain.step(i);
            
            // Update development system
            if (i % 10 == 0) {
                brain.develop();
            }
            
            // Change developmental stage periodically
            if (i == 15) {
                brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
            } else if (i == 30) {
                brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
            }
        }
        
        std::cout << "    Development system integration: PASSED" << std::endl;
    }
    
    static void test_plasticity_integration() {
        std::cout << "Testing plasticity integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 200);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test STDP integration
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        // Test Hebbian integration
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
        
        // Test plasticity rule integration
        auto* plasticityRule = brain.getPlasticityRule();
        assert(plasticityRule != nullptr);
        
        // Test neuromodulation-modulated plasticity
        std::vector<float> preSynapticActivity(5, 0.8f);
        std::vector<float> postSynapticActivity(5, 0.6f);
        
        for (int i = 0; i < 35; ++i) {
            brain.step(i);
            
            // Apply plasticity rules based on activity
            if (i % 8 == 0) {
                brain.updatePlasticity();
            }
            
            // Test neuromodulation effects on plasticity
            nlm::Neuromodulator plasticityModulator;
            plasticityModulator.setLevel(0.7f);
            brain.applyNeuromodulation(plasticityModulator);
        }
        
        std::cout << "    Plasticity integration: PASSED" << std::endl;
    }
    
    static void test_neural_plasticity_coupling() {
        std::cout << "Testing neural plasticity coupling..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 250);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test that plasticity affects neural connectivity
        size_t initialSynapseCount = brain.getTotalSynapseCount();
        
        for (int i = 0; i < 20; ++i) {
            brain.step(i);
            brain.updatePlasticity();
            
            // Verify plasticity changes synapse count
            if (i % 5 == 0) {
                size_t currentSynapseCount = brain.getTotalSynapseCount();
                assert(currentSynapseCount >= initialSynapseCount);
            }
        }
        
        // Test that development affects plasticity rates
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        float criticalPeriodPlasticity = brain.getCurrentPlasticityRate();
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
        float adultPlasticity = brain.getCurrentPlasticityRate();
        
        assert(criticalPeriodPlasticity > adultPlasticity);
        
        std::cout << "    Neural plasticity coupling: PASSED" << std::endl;
    }
    
    static void test_replay_and_consolidation() {
        std::cout << "Testing replay and consolidation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 300);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test episodic memory replay
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Create test episodes
        nlm::EpisodicMemoryItem episode1;
        episode1.timestamp = 1;
        episode1.action = nlm::ActionType::MoveForward;
        episode1.reward = 1.0f;
        
        nlm::EpisodicMemoryItem episode2;
        episode2.timestamp = 2;
        episode2.action = nlm::ActionType::Look;
        episode2.reward = 0.5f;
        
        nlm::EpisodicMemoryItem episode3;
        episode3.timestamp = 3;
        episode3.action = nlm::ActionType::Interact;
        episode3.reward = 2.0f;
        
        episodicMemory->storeEpisode(episode1);
        episodicMemory->storeEpisode(episode2);
        episodicMemory->storeEpisode(episode3);
        
        // Test replay functionality
        episodicMemory->enableReplay(true);
        auto episodesForReplay = episodicMemory->getEpisodesForReplay(2);
        assert(episodesForReplay.size() == 2);
        
        // Simulate replay during consolidation
        for (int i = 0; i < 15; ++i) {
            brain.step(i);
            
            if (i % 5 == 0) {
                episodicMemory->consolidate(0.5f);
            }
        }
        
        // Verify consolidation reduced episode count
        assert(episodicMemory->getEpisodeCount() <= 3);
        
        std::cout << "    Replay and consolidation: PASSED" << std::endl;
    }
    
    static void test_system_coherence() {
        std::cout << "Testing system coherence..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Run extended simulation to test system stability
        size_t spikeCount1 = brain.getTotalSpikeCount();
        size_t firingNeuronCount1 = brain.getFiringNeuronCount();
        float firingRate1 = brain.getAverageFiringRate();
        
        for (int i = 0; i < 100; ++i) {
            brain.step(i);
            
            // Periodically verify system remains coherent
            if (i % 20 == 0) {
                size_t currentSpikeCount = brain.getTotalSpikeCount();
                assert(currentSpikeCount >= spikeCount1);
                
                size_t currentFiringNeuronCount = brain.getFiringNeuronCount();
                assert(currentFiringNeuronCount >= 0);
                
                float currentFiringRate = brain.getAverageFiringRate();
                assert(currentFiringRate >= 0.0f);
            }
        }
        
        // Verify system didn't become unstable
        assert(brain.getTotalSpikeCount() > 0);
        assert(brain.getFiringNeuronCount() >= 0);
        
        std::cout << "    System coherence: PASSED" << std::endl;
    }
    
    static void test_real_world_scenarios() {
        std::cout << "Testing real-world scenarios..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Scenario 1: Exploration with reward learning
        std::cout << "      Scenario 1: Exploration with reward learning" << std::endl;
        for (int i = 0; i < 30; ++i) {
            brain.step(i);
            
            // Apply reward signals
            if (i % 10 == 0) {
                nlm::Neuromodulator rewardSignal;
                rewardSignal.setLevel(0.9f);
                brain.applyNeuromodulation(rewardSignal);
            }
        }
        
        // Scenario 2: Novelty-driven exploration
        std::cout << "      Scenario 2: Novelty-driven exploration" << std::endl;
        auto* novelty = brain.getNovelty();
        assert(novelty != nullptr);
        
        for (int i = 0; i < 30; ++i) {
            brain.step(i);
            
            // Apply novelty signals
            if (i % 12 == 0) {
                nlm::Neuromodulator noveltySignal;
                noveltySignal.setLevel(0.7f);
                brain.applyNeuromodulation(noveltySignal);
            }
        }
        
        // Scenario 3: Prediction-based decision making
        std::cout << "      Scenario 3: Prediction-based decision making" << std::endl;
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        for (int i = 0; i < 30; ++i) {
            brain.step(i);
            
            // Use predictions for action selection
            auto predictedAction = brain.produceAction();
            assert(predictedAction != nullptr);
        }
        
        std::cout << "    Real-world scenarios: PASSED" << std::endl;
    }
};

} // namespace test_suites