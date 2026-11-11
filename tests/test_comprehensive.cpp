// Comprehensive NLM Test Framework
// Tests all major subsystems: Configuration, Brain, Memory, Neuromodulation, Cognition

#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace test_nlm {

class NLMSystemTester {
public:
    NLMSystemTester() = default;
    
    void runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        
        try {
            testConfigurationSystem();
            std::cout << "✓ Configuration system tests passed" << std::endl;
            
            testBrainSystem();
            std::cout << "✓ Brain system tests passed" << std::endl;
            
            testMemorySystems();
            std::cout << "✓ Memory systems tests passed" << std::endl;
            
            testNeuromodulationSystems();
            std::cout << "✓ Neuromodulation systems tests passed" << std::endl;
            
            testCognitionSystems();
            std::cout << "✓ Cognition systems tests passed" << std::endl;
            
            testPlasticitySystems();
            std::cout << "✓ Plasticity systems tests passed" << std::endl;
            
            testDevelopmentSystem();
            std::cout << "✓ Development system tests passed" << std::endl;
            
            testPredictionSystem();
            std::cout << "✓ Prediction system tests passed" << std::endl;
            
            testIntegration();
            std::cout << "✓ Integration tests passed" << std::endl;
            
            std::cout << "\n=== All tests passed! ===" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "\n✗ Test failed: " << e.what() << std::endl;
            throw;
        }
    }
    
private:
    void testConfigurationSystem() {
        std::cout << "  Testing Configuration system..." << std::endl;
        
        // Test 1: Basic configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.01, nlm::ConfigSource::Default);
        
        assert(config->has("neuron_count"));
        assert(config->get<int64_t>("neuron_count") == 100);
        assert(config->getOr<int>("region_count", 0) == 2);
        assert(config->getOr<float>("connection_probability", 0.0f) == 0.1f);
        
        // Test 2: Configuration validation
        nlm::ConfigValidator validator;
        validator.setupDefaultValidators();
        
        std::string errorMsg;
        assert(validator.validate(*config, errorMsg));
        assert(errorMsg.empty());
        
        // Test 3: Invalid configuration
        config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
        assert(!validator.validate(*config, errorMsg));
        assert(!errorMsg.empty());
        
        // Test 4: Configuration summary
        std::string summary = config->summary();
        assert(!summary.empty());
        assert(summary.find("neuron_count") != std::string::npos);
        assert(summary.find("region_count") != std::string::npos);
        
        // Test 5: Configuration persistence
        std::string testFile = "/tmp/test_config.cfg";
        assert(config->saveToFile(testFile));
        
        auto config2 = std::make_shared<nlm::Config>();
        assert(config2->loadFromFile(testFile));
        assert(config2->get<int64_t>("neuron_count") == -1); // Negative value should be loaded
        
        std::cout << "    Configuration system: PASSED" << std::endl;
    }
    
    void testBrainSystem() {
        std::cout << "  Testing Brain system..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        
        // Test 1: Brain creation and initialization
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test 2: Brain statistics
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getRegionCount() == 1);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getActiveNeuronCount() == 0);
        assert(brain.getFiringNeuronCount() == 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        // Test 3: Brain step
        brain.step(0);
        assert(brain.getTotalNeuronCount() == 50); // Should not change
        
        // Test 4: Brain reset
        brain.reset();
        assert(brain.getTotalSpikeCount() == 0);
        assert(brain.getPendingSpikeEventCount() == 0);
        
        // Test 5: Brain regions
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 1);
        
        auto* region = brain.getRegion(regionIds[0]);
        assert(region != nullptr);
        assert(region->getTotalNeuronCount() == 50);
        
        // Test 6: Action production
        auto action = brain.produceAction();
        assert(action != nullptr);
        assert(action->getType() != nlm::ActionType::Custom);
        
        // Test 7: Checkpoint save/load
        std::string checkpointFile = "/tmp/test_brain_checkpoint.bin";
        assert(brain.save(checkpointFile));
        
        nlm::Brain brain2(config);
        assert(brain2.initialize());
        assert(brain2.load(checkpointFile));
        
        // Both brains should have same neuron count
        assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
        
        std::cout << "    Brain system: PASSED" << std::endl;
    }
    
    void testMemorySystems() {
        std::cout << "  Testing Memory systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: Working memory
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        nlm::NeuronId testNeuron(42);
        workingMemory->storeToNeuron(testNeuron, 0.5f);
        assert(workingMemory->getActiveTraces() > 0);
        
        // Test 2: Episodic memory
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 100;
        episode.positionX = 5.0f;
        episode.positionY = 3.0f;
        episode.action = nlm::ActionType::MoveForward;
        episode.reward = 1.0f;
        episode.age = 10;
        
        episodicMemory->storeEpisode(episode);
        assert(episodicMemory->getEpisodeCount() > 0);
        
        // Test 3: Associative memory
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        associativeMemory->associate(testNeuron, "test_pattern");
        
        // Test 4: Memory integration with brain
        brain.step(0);
        
        std::cout << "    Memory systems: PASSED" << std::endl;
    }
    
    void testNeuromodulationSystems() {
        std::cout << "  Testing Neuromodulation systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: Dopamine system
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        assert(dopamine->getLevel() >= 0.0f);
        
        // Test 2: Curiosity system
        auto* curiosity = brain.getCuriosity();
        assert(curiosity != nullptr);
        assert(curiosity->getLevel() >= 0.0f);
        
        // Test 3: Novelty system
        auto* novelty = brain.getNovelty();
        assert(novelty != nullptr);
        assert(novelty->getLevel() >= 0.0f);
        
        // Test 4: Prediction error system
        auto* predictionError = brain.getPredictionErrorSignal();
        assert(predictionError != nullptr);
        assert(predictionError->getError() >= 0.0f);
        
        // Test 5: Neuromodulation effects
        brain.step(0);
        
        // Test 6: Neuromodulator integration
        nlm::Dopamine neuromod(dopamine->getLevel());
        brain.applyNeuromodulation(neuromod);
        
        std::cout << "    Neuromodulation systems: PASSED" << std::endl;
    }
    
    void testCognitionSystems() {
        std::cout << "  Testing Cognition systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: Neural planner
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        assert(planner->getPlanningDepth() > 0);
        
        // Test 2: Concept formation
        auto* conceptFormation = brain.getConceptFormation();
        assert(conceptFormation != nullptr);
        
        std::vector<float> pattern1(24, 0.0f);
        std::vector<float> pattern2(24, 0.0f);
        
        for (int i = 5; i <= 10; ++i) {
            pattern1[i] = 0.3f;
            pattern2[i] = 0.3f;
        }
        
        pattern1[16] = 0.8f;
        pattern2[16] = 0.8f;
        
        float similarity = conceptFormation->computeSimilarity(pattern1, pattern2);
        assert(similarity >= 0.0f && similarity <= 1.0f);
        
        // Test 3: Concept formation self-similarity
        float selfSimilarity = conceptFormation->computeSimilarity(pattern1, pattern1);
        assert(selfSimilarity >= 0.9f); // Should be very similar to itself
        
        std::cout << "    Cognition systems: PASSED" << std::endl;
    }
    
    void testPlasticitySystems() {
        std::cout << "  Testing Plasticity systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: STDP system
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        // Test 2: Hebbian system
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
        
        // Test 3: Structural plasticity system
        auto* structuralPlasticity = brain.getStructuralPlasticity();
        assert(structuralPlasticity != nullptr);
        
        // Test 4: Plasticity during simulation
        brain.step(0);
        brain.step(1);
        brain.step(2);
        
        std::cout << "    Plasticity systems: PASSED" << std::endl;
    }
    
    void testDevelopmentSystem() {
        std::cout << "  Testing Development system..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: Development system
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test 2: Developmental stages
        assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Invalid);
        
        // Test 3: Development update
        brain.develop();
        
        std::cout << "    Development system: PASSED" << std::endl;
    }
    
    void testPredictionSystem() {
        std::cout << "  Testing Prediction system..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test 1: Prediction system
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test 2: Prediction during simulation
        brain.step(0);
        
        std::cout << "    Prediction system: PASSED" << std::endl;
    }
    
    void testIntegration() {
        std::cout << "  Testing Integration..." << std::endl;
        
        // Test 1: Full brain with all systems
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test 2: Complex simulation
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            auto action = brain.produceAction();
            assert(action != nullptr);
            assert(brain.getTotalNeuronCount() == 100);
        }
        
        // Test 3: Memory integration
        brain.logStatus();
        
        // Test 4: Configuration validation integration
        nlm::ConfigValidator validator;
        validator.setupDefaultValidators();
        std::string errorMsg;
        assert(validator.validate(*config, errorMsg));
        
        std::cout << "    Integration: PASSED" << std::endl;
    }
};

void runComprehensiveTests() {
    NLMSystemTester tester;
    tester.runAllTests();
}

} // namespace test_nlm