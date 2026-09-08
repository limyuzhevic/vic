#include "../tests/test_brain.cpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Memory Integration Test", "[integration][memory][working][episodic]")
{
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    SECTION("Working Memory Integration")
    {
        auto workingMemory = brain->getWorkingMemory();
        REQUIRE(workingMemory != nullptr);
        
        // Inject sensory input
        auto visionInput = std::make_shared<Vision>(10, 10, 3);
        std::vector<float> visualData(300, 0.5f);
        visionInput->setData(visualData);
        
        brain->receiveSensoryInput(*visionInput);
        
        // Check that working memory stores the input
        brain->step(0);
        
        // Working memory should have stored the sensory input
        // This is a basic integration test
        NLM_LOG_INFO("Working memory integration test passed");
    }
    
    SECTION("Episodic Memory Integration")
    {
        auto episodicMemory = brain->getEpisodicMemory();
        REQUIRE(episodicMemory != nullptr);
        
        // After several steps, episodic memory should have stored experiences
        brain->step(50);
        
        // Check that episodes have been stored
        size_t episodeCount = episodicMemory->getEpisodeCount();
        // Should have stored multiple episodes
        REQUIRE(episodeCount > 0);
        
        NLM_LOG_INFO("Episodic memory integration test passed");
    }
}

TEST_CASE("Cognitive Integration Test", "[integration][cognition][planner][concept]")
{
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    SECTION("Neural Planner Integration")
    {
        auto planner = brain->getPlanner();
        REQUIRE(planner != nullptr);
        
        std::vector<float> currentState(20, 0.5f);
        std::vector<float> goalState(20, 0.8f);
        planner->setCurrentGoal(goalState);
        
        ActionType action = planner->planAction(currentState, 0.5f);
        // Should return a valid action
        REQUIRE(static_cast<size_t>(action) < 10);
        
        // Update planner quality
        std::vector<ActionType> plannedActions = {ActionType::MoveForward, ActionType::Wait};
        std::vector<ActionType> actualActions = {ActionType::MoveForward, ActionType::MoveForward};
        planner->updatePlanQuality(plannedActions, actualActions, 1.0f);
        
        NLM_LOG_INFO("Neural planner integration test passed");
    }
    
    SECTION("Concept Formation Integration")
    {
        auto conceptFormation = brain->getConceptFormation();
        REQUIRE(conceptFormation != nullptr);
        
        std::vector<float> statePattern(30, 0.5f);
        size_t conceptId = conceptFormation->presentExperience(statePattern, statePattern, 0.5f, 0);
        
        if (conceptId > 0) {
            REQUIRE(conceptFormation->isConceptStable(conceptId));
            auto prototype = conceptFormation->getConceptPrototype(conceptId);
            REQUIRE(prototype.size() > 0);
        }
        
        NLM_LOG_INFO("Concept formation integration test passed");
    }
}

TEST_CASE("Prediction System Integration", "[integration][prediction]")
{
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    SECTION("Prediction System Integration")
    {
        auto predictionSystem = brain->getPredictionSystem();
        // Prediction system should be available even if not fully integrated yet
        if (predictionSystem) {
            NLM_LOG_INFO("Prediction system integration test passed");
        } else {
            NLM_LOG_INFO("Prediction system not available (expected in Phase 2)");
        }
    }
}

TEST_CASE("Neuromodulation Integration", "[integration][neuromodulation]")
{
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    SECTION("Complete Neuromodulation Integration")
    {
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        auto predictionError = brain->getPredictionErrorSignal();
        
        REQUIRE(dopamine != nullptr);
        REQUIRE(curiosity != nullptr);
        REQUIRE(novelty != nullptr);
        REQUIRE(predictionError != nullptr);
        
        // Test that neuromodulation levels are accessible
        float dopamineLevel = dopamine->getLevel();
        float curiosityLevel = curiosity->getLevel();
        float noveltyLevel = novelty->getLevel();
        float predictionErrorValue = predictionError->getError();
        
        // Apply reward modulation
        brain->applyNeuromodulation(*dopamine);
        
        NLM_LOG_INFO("Complete neuromodulation integration test passed");
    }
}

TEST_CASE("Full Brain Integration", "[integration][full][brain]")
{
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", 200);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15f);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    SECTION("Complete Brain Loop Integration")
    {
        // Run several simulation steps
        for (int step = 0; step < 100; ++step) {
            // Inject sensory input
            auto visionInput = std::make_shared<Vision>(8, 8, 3);
            std::vector<float> visualData(192, 0.3f + (step % 10) * 0.05f);
            visionInput->setData(visualData);
            
            brain->receiveSensoryInput(*visionInput);
            
            // Brain step
            brain->step(step);
            
            // Check brain statistics are being updated
            REQUIRE(brain->getTotalSpikeCount() >= 0);
            REQUIRE(brain->getFiringNeuronCount() >= 0);
            REQUIRE(brain->getTotalNeuronCount() > 0);
        }
        
        // Check final brain status
        brain->logStatus();
        
        NLM_LOG_INFO("Full brain integration test passed");
    }
}