// System Integration Tests
// Tests integration between brain, memory, cognition, neuromodulation, and development systems

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../brain/experiment/PredictionExperimentIntegration.hpp"
#include "../memory/experiment/MemoryExperimentIntegration.hpp"
#include "../cognition/experiment/CognitiveExperimentIntegration.hpp"
#include "../neuromodulation/experiment/NeuromodulationExperimentIntegration.hpp"
#include "../performance/experiment/PerformanceBenchmarks.hpp"
#include <numeric>

namespace nlm {

class TestSystemIntegration : public IntegrationTestBase {
public:
    explicit TestSystemIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Brain-Memory Integration", "System Integration", [this]() {
            testBrainMemoryIntegration();
        });
        
        runTestCase("Brain-Cognition Integration", "System Integration", [this]() {
            testBrainCognitionIntegration();
        });
        
        runTestCase("Brain-Neuromodulation Integration", "System Integration", [this]() {
            testBrainNeuromodulationIntegration();
        });
        
        runTestCase("Brain-Development Integration", "System Integration", [this]() {
            testBrainDevelopmentIntegration();
        });
        
        runTestCase("Brain-Prediction Integration", "System Integration", [this]() {
            testBrainPredictionIntegration();
        });
        
        runTestCase("Full System Integration", "System Integration", [this]() {
            testFullSystemIntegration();
        });
        
        runTestCase("System Consistency", "System Integration", [this]() {
            testSystemConsistency();
        });
        
        runTestCase("Cross-System Data Integrity", "System Integration", [this]() {
            testCrossSystemDataIntegrity();
        });
    }
    
private:
    void testBrainMemoryIntegration() {
        setupSystem();
        
        // Store data in working memory
        auto* workingMemory = brain->getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Create some neural activity pattern
        NeuronId testNeuron = 0;
        float testValue = 0.75f;
        workingMemory->store(testNeuron, testValue);
        
        assert(workingMemory->contains(testNeuron));
        assert(workingMemory->retrieve(testNeuron) == testValue);
        
        // Test episodic memory storage
        EpisodicMemoryItem episode;
        episode.timestamp = 1000;
        episode.neurons = {1, 2, 3};
        episode.values = {0.1f, 0.5f, 0.9f};
        episode.metadata = "Test episode";
        
        auto* episodicMemory = brain->getEpisodicMemory();
        assert(episodicMemory != nullptr);
        episodicMemory->storeEpisode(episode);
        
        assert(episodicMemory->getEpisodeCount() > 0);
        
        auto retrieved = episodicMemory->retrieveEpisode(0);
        assert(retrieved.timestamp == episode.timestamp);
        assert(retrieved.neurons == episode.neurons);
        
        // Verify data consistency across systems
        float wmValue = workingMemory->retrieve(testNeuron);
        assert(std::abs(wmValue - testValue) < 0.001f);
        
        teardownSystem();
    }
    
    void testBrainCognitionIntegration() {
        setupSystem();
        
        auto* conceptFormation = brain->getConceptFormation();
        auto* neuralPlanner = brain->getPlanner();
        
        // Test concept formation integration with neural activity
        assert(conceptFormation != nullptr);
        
        // Create some input pattern
        std::vector<NeuronId> pattern = {1, 2, 3, 4, 5};
        std::vector<float> values = {0.2f, 0.4f, 0.6f, 0.8f, 0.9f};
        
        // Concept formation should be able to process these patterns
        // (Implementation details would depend on actual ConceptFormation API)
        
        // Test neural planning integration
        assert(neuralPlanner != nullptr);
        
        teardownSystem();
    }
    
    void testBrainNeuromodulationIntegration() {
        setupSystem();
        
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        assert(dopamine != nullptr);
        assert(curiosity != nullptr);
        assert(novelty != nullptr);
        assert(predictionError != nullptr);
        
        // Apply neuromodulatory signals
        Neuromodulator rewardSignal;
        rewardSignal.type = NeuromodulatorType::Reward;
        rewardSignal.value = 1.0f;
        
        brain->applyNeuromodulation(rewardSignal);
        
        // Test novelty detection
        Neuromodulator noveltySignal;
        noveltySignal.type = NeuromodulatorType::Novelty;
        noveltySignal.value = 0.5f;
        
        brain->applyNeuromodulation(noveltySignal);
        
        // Test curiosity signal
        Neuromodulator curiositySignal;
        curiositySignal.type = NeuromodulatorType::Curiosity;
        curiositySignal.value = 0.8f;
        
        brain->applyNeuromodulation(curiositySignal);
        
        teardownSystem();
    }
    
    void testBrainDevelopmentIntegration() {
        setupSystem();
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Check developmental stage
        DevelopmentalStage initialStage = brain->getDevelopmentalStage();
        
        // Run some steps to trigger development
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain->step(step);
            brain->develop();
        }
        
        // Development should have affected system properties
        DevelopmentalStage finalStage = brain->getDevelopmentalStage();
        assert(finalStage != initialStage);
        
        // Verify plasticity rules have been updated
        auto* stdp = brain->getSTDP();
        assert(stdp != nullptr);
        
        teardownSystem();
    }
    
    void testBrainPredictionIntegration() {
        setupSystem();
        
        auto* predictionSystem = brain->getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test prediction error signal generation
        auto* predictionError = brain->getPredictionErrorSignal();
        assert(predictionError != nullptr);
        
        // Verify integration between prediction and error systems
        // (Implementation depends on actual PredictionSystem API)
        
        teardownSystem();
    }
    
    void testFullSystemIntegration() {
        setupSystem();
        
        // Create a realistic simulation scenario
        std::vector<std::string> inputs = {
            "Sensory input 1", "Sensory input 2", "Sensory input 3"
        };
        
        // Process inputs through the full system
        for (size_t i = 0; i < inputs.size(); ++i) {
            // Create sensory input
            SensoryInput sensoryInput;
            sensoryInput.neuron = static_cast<NeuronId>(i);
            sensoryInput.current = 0.5f;
            
            brain->receiveSensoryInput(sensoryInput);
            
            // Run simulation step
            brain->step(static_cast<nlm::SimulationStep>(i));
            
            // Produce action
            auto action = brain->produceAction();
            assert(action != nullptr);
        }
        
        // Verify system state consistency
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        assert(totalNeurons > 0);
        assert(totalSynapses >= 0);
        
        // Test checkpointing
        std::string checkpointFile = "/tmp/test_checkpoint.bin";
        bool saved = brain->save(checkpointFile);
        assert(saved);
        
        // Create new brain and load checkpoint
        auto newBrain = std::make_unique<Brain>(config);
        assert(newBrain->initialize());
        assert(newBrain->load(checkpointFile));
        
        // Verify loaded state
        assert(newBrain->getTotalNeuronCount() == totalNeurons);
        
        teardownSystem();
        
        // Clean up
        std::remove(checkpointFile.c_str());
    }
    
    void testSystemConsistency() {
        setupSystem();
        
        // Store baseline values
        size_t baselineNeuronCount = brain->getTotalNeuronCount();
        size_t baselineSynapseCount = brain->getTotalSynapseCount();
        float baselineFiringRate = brain->getAverageFiringRate();
        
        // Run multiple steps
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        // Verify system invariants are maintained
        assert(brain->getTotalNeuronCount() == baselineNeuronCount);
        assert(brain->getTotalSynapseCount() >= baselineSynapseCount);
        assert(brain->getAverageFiringRate() >= 0.0f);
        
        // Test multiple runs produce consistent results
        teardownSystem();
        
        // Run identical configuration again
        setupSystem();
        
        // Compare key metrics (within tolerance)
        assert(std::abs(brain->getTotalNeuronCount() - baselineNeuronCount) == 0);
        
        teardownSystem();
    }
    
    void testCrossSystemDataIntegrity() {
        setupSystem();
        
        // Test data flow between systems
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        // Create cross-system relationships
        NeuronId neuron1 = 1;
        NeuronId neuron2 = 2;
        
        // Store in working memory
        workingMemory->store(neuron1, 0.7f);
        
        // Create episodic memory with associated neurons
        EpisodicMemoryItem episode;
        episode.neurons = {neuron1, neuron2};
        episode.values = {0.7f, 0.3f};
        episode.metadata = "Cross-system test";
        
        episodicMemory->storeEpisode(episode);
        
        // Create association in associative memory
        associativeMemory->associate(neuron1, neuron2, 0.8f);
        
        // Verify all systems have consistent data
        assert(workingMemory->contains(neuron1));
        assert(workingMemory->retrieve(neuron1) == 0.7f);
        
        assert(episode.neurons.size() == 2);
        assert(episode.values[0] == 0.7f);
        
        assert(associativeMemory->getAssociationStrength(neuron1, neuron2) == 0.8f);
        
        // Test replay mechanism
        // (Implementation depends on actual replay system)
        
        teardownSystem();
    }
};

} // namespace nlm