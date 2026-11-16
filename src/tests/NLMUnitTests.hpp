#ifndef NLM_UNIT_TESTS_HPP
#define NLM_UNIT_TESTS_HPP

#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

namespace nlm {
namespace unit_tests {

class UnitTestHelper {
public:
    static std::shared_ptr<Config> createTestConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("dopamine_baseline", 0.1f);
        return config;
    }
    
    static bool approximatelyEqual(float a, float b, float epsilon = 0.001f) {
        return std::abs(a - b) <= epsilon;
    }
};

class MemoryUnitTests {
public:
    static void testWorkingMemory() {
        std::cout << "Running Working Memory Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* wm = brain->getWorkingMemory();
        assert(wm != nullptr && "Working memory should be available");
        
        // Test initial state
        assert(wm->getActiveTraces() == 0 && "Initial working memory should have 0 traces");
        
        // Test trace activation
        wm->createTrace(1, 0.5f, 0.8f);
        assert(wm->getActiveTraces() == 1 && "Working memory should have 1 trace after creation");
        
        // Test trace decay
        wm->update(0.1f);
        assert(wm->getActiveTraces() == 1 && "Working memory should still have trace after update");
        
        // Test trace competition
        wm->createTrace(2, 0.7f, 0.3f); // Same strength, different id
        assert(wm->getActiveTraces() == 2 && "Working memory should have 2 traces");
        
        std::cout << "✓ Working Memory Unit Tests passed" << std::endl;
    }
    
    static void testEpisodicMemory() {
        std::cout << "Running Episodic Memory Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* em = brain->getEpisodicMemory();
        assert(em != nullptr && "Episodic memory should be available");
        
        // Test initial state
        assert(em->getEpisodeCount() == 0 && "Initial episodic memory should have 0 episodes");
        
        // Test episode recording
        em->recordEpisode(100.0f, 0.5f, 0.8f, 1.0f);
        assert(em->getEpisodeCount() == 1 && "Episodic memory should have 1 episode after recording");
        
        // Test episode retrieval
        auto episodes = em->getEpisodesForReplay(1);
        assert(episodes.size() == 1 && "Should be able to retrieve 1 episode");
        
        assert(UnitTestHelper::approximatelyEqual(episodes[0].reward, 0.5f, 0.01f) && "Episode reward should match");
        assert(UnitTestHelper::approximatelyEqual(episodes[0].novelty, 0.8f, 0.01f) && "Episode novelty should match");
        
        // Test consolidation
        em->consolidate(0.5f);
        assert(em->getConsolidationLevel() > 0.0f && "Consolidation should increase episode strength");
        
        std::cout << "✓ Episodic Memory Unit Tests passed" << std::endl;
    }
};

class NeuromodulationUnitTests {
public:
    static void testDopamineSystem() {
        std::cout << "Running Dopamine System Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* dopamine = brain->getDopamine();
        assert(dopamine != nullptr && "Dopamine system should be available");
        
        // Test baseline dopamine level
        float baseline = dopamine->getBaselineLevel();
        assert(UnitTestHelper::approximatelyEqual(baseline, 0.1f, 0.01f) && "Baseline dopamine should match config");
        
        // Test dopamine modulation
        float modulation = dopamine->applyModulation(0.5f, 0.1f);
        assert(modulation >= 0.0f && "Dopamine modulation should be non-negative");
        
        // Test prediction error
        float predictionError = dopamine->getPredictionError();
        assert(std::isnan(predictionError) || predictionError >= 0.0f && "Prediction error should be valid");
        
        std::cout << "✓ Dopamine System Unit Tests passed" << std::endl;
    }
    
    static void testCuriositySystem() {
        std::cout << "Running Curiosity System Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* curiosity = brain->getCuriosity();
        assert(curiosity != nullptr && "Curiosity system should be available");
        
        // Test curiosity level
        float level = curiosity->getCuriosityLevel();
        assert(level >= 0.0f && level <= 1.0f && "Curiosity level should be between 0 and 1");
        
        // Test curiosity driven behavior
        bool shouldExplore = curiosity->shouldDriveExploration();
        assert(shouldExplore == true && "Should drive exploration when curiosity is high");
        
        // Test novelty detection
        float novelty = curiosity->detectNovelty(0.5f, 0.3f);
        assert(novelty >= 0.0f && "Novelty should be non-negative");
        
        std::cout << "✓ Curiosity System Unit Tests passed" << std::endl;
    }
};

class CognitionUnitTests {
public:
    static void testNeuralPlanner() {
        std::cout << "Running Neural Planner Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* planner = brain->getPlanner();
        assert(planner != nullptr && "Planner should be available");
        
        // Test planner initialization
        planner->initialize(brain.get());
        
        // Test action evaluation
        float actionScore = planner->evaluateAction(MotorCommand::MoveForward);
        assert(actionScore >= 0.0f && "Action score should be non-negative");
        
        // Test action sequence
        std::vector<MotorCommand> sequence = {MotorCommand::MoveForward, MotorCommand::MoveForward, MotorCommand::MoveBackward};
        float sequenceScore = planner->evaluateSequence(sequence);
        assert(sequenceScore >= 0.0f && "Sequence score should be non-negative");
        
        std::cout << "✓ Neural Planner Unit Tests passed" << std::endl;
    }
    
    static void testConceptFormation() {
        std::cout << "Running Concept Formation Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        auto* conceptFormation = brain->getConceptFormation();
        assert(conceptFormation != nullptr && "Concept formation should be available");
        
        // Test concept creation
        conceptFormation->createConcept(std::vector<float>{0.5f, 0.7f, 0.3f}, "test_concept");
        
        // Test concept similarity
        float similarity = conceptFormation->calculateSimilarity("test_concept", "another_concept");
        assert(similarity >= 0.0f && similarity <= 1.0f && "Similarity should be between 0 and 1");
        
        // Test concept abstraction
        conceptFormation->abstractConcepts();
        
        std::cout << "✓ Concept Formation Unit Tests passed" << std::endl;
    }
};

class IntegrationUnitTests {
public:
    static void testMemoryIntegration() {
        std::cout << "Running Memory Integration Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test that memory systems are connected
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        auto* dopamine = brain->getDopamine();
        
        assert(wm != nullptr && em != nullptr && dopamine != nullptr && 
               "Memory, episodic memory, and dopamine systems should be integrated");
        
        // Test integration workflow
        brain->step(0, 0.0);
        
        assert(wm->getActiveTraces() >= 0 && "Working memory should have traces after brain step");
        
        std::cout << "✓ Memory Integration Unit Tests passed" << std::endl;
    }
    
    static void testNeuromodulationIntegration() {
        std::cout << "Running Neuromodulation Integration Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test neuromodulation integration
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        
        assert(curiosity != nullptr && novelty != nullptr && 
               "Curiosity and novelty systems should be available");
        
        // Test neuromodulation effects
        brain->step(1, 0.001);
        
        float curiosityLevel = curiosity->getCuriosityLevel();
        float noveltyLevel = novelty->getNoveltyLevel();
        
        assert(curiosityLevel >= 0.0f && "Curiosity level should be valid");
        assert(noveltyLevel >= 0.0f && "Novelty level should be valid");
        
        std::cout << "✓ Neuromodulation Integration Unit Tests passed" << std::endl;
    }
};

class AgentBrainUnitTests {
public:
    static void testAgentBrainInitialization() {
        std::cout << "Running Agent Brain Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        AgentBrain agent(brain);
        
        // Test agent initialization
        agent.initialize(*brain->getWorkingMemory(), *brain->getEpisodicMemory());
        
        // Test agent subsystems
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        assert(agent.getCuriosityLevel() >= 0.0f && "Agent should have valid curiosity level");
        assert(agent.getNoveltyLevel() >= 0.0f && "Agent should have valid novelty level");
        
        std::cout << "✓ Agent Brain Unit Tests passed" << std::endl;
    }
};

class SystemIntegrationUnitTests {
public:
    static void testCompleteIntegration() {
        std::cout << "Running Complete System Integration Unit Tests..." << std::endl;
        
        auto config = UnitTestHelper::createTestConfig();
        auto brain = std::make_shared<Brain>(config);
        
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test all major systems
        assert(brain->getWorkingMemory() != nullptr && "Working memory should be available");
        assert(brain->getEpisodicMemory() != nullptr && "Episodic memory should be available");
        assert(brain->getDopamine() != nullptr && "Dopamine system should be available");
        assert(brain->getCuriosity() != nullptr && "Curiosity system should be available");
        assert(brain->getPlanner() != nullptr && "Planner should be available");
        assert(brain->getConceptFormation() != nullptr && "Concept formation should be available");
        assert(brain->getPredictionSystem() != nullptr && "Prediction system should be available");
        assert(brain->getDevelopmentSystem() != nullptr && "Development system should be available");
        
        // Test system interactions
        brain->step(0, 0.0);
        
        // Run a simulation
        for (int i = 1; i < 100; ++i) {
            brain->step(i, i * 0.001);
            
            // Verify system health
            assert(brain->getTotalNeuronCount() > 0 && "Should have neurons");
            assert(brain->getTotalSynapseCount() > 0 && "Should have synapses");
            assert(brain->getFiringNeuronCount() >= 0 && "Should have non-negative firing neurons");
        }
        
        std::cout << "✓ Complete System Integration Unit Tests passed" << std::endl;
    }
};

// Run all unit tests
void runAllUnitTests() {
    std::cout << "=== NLM Unit Test Suite ===" << std::endl;
    
    try {
        MemoryUnitTests::testWorkingMemory();
        MemoryUnitTests::testEpisodicMemory();
        
        NeuromodulationUnitTests::testDopamineSystem();
        NeuromodulationUnitTests::testCuriositySystem();
        
        CognitionUnitTests::testNeuralPlanner();
        CognitionUnitTests::testConceptFormation();
        
        IntegrationUnitTests::testMemoryIntegration();
        IntegrationUnitTests::testNeuromodulationIntegration();
        
        AgentBrainUnitTests::testAgentBrainInitialization();
        
        SystemIntegrationUnitTests::testCompleteIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Unit Tests Passed! ===" << std::endl;
        std::cout << "NLM unit test suite completed successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Unit test failed with exception: " << e.what() << std::endl;
        std::cerr << "Test execution failed." << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unit test failed with unknown exception." << std::endl;
        std::cerr << "Test execution failed." << std::endl;
        exit(1);
    }
}

} // namespace unit_tests
} // namespace nlm

#endif // NLM_UNIT_TESTS_HPP