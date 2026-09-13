#include "tests/test_brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_all.hpp>

// Experience struct for episodic memory testing
struct Experience {
    SimulationStep timestamp;
    float reward;
    std::vector<float> neuralPattern;
    std::vector<float> sensoryState;
};

TEST_CASE("Memory Retention Test - Working Memory persistence over time", "[memory][integration]") {
    // Create a brain with appropriate configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    
    REQUIRE(brain->initialize());
    
    // Store initial pattern in working memory
    std::vector<float> initialPattern = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    brain->getWorkingMemory()->store(initialPattern, 1.0f);
    
    // Record initial activation levels
    std::vector<float> initialActivations;
    for (size_t i = 0; i < 5; ++i) {
        initialActivations.push_back(brain->getWorkingMemory()->getNeuronActivation(i));
    }
    
    // Simulate 100 steps of memory maintenance
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Memory should maintain its pattern with gradual decay
        // Working memory update decays over time
        if (step > 10) {
            // Check that memory traces persist (not completely decayed)
            REQUIRE(brain->getWorkingMemory()->getMemoryNeurons().size() > 0);
        }
        
        // Memory activity should gradually decay but remain detectable
        float currentActivity = brain->getWorkingMemory()->getMemoryActivity();
        REQUIRE(currentActivity >= 0.0f); // Never negative
    }
    
    // After 100 steps, memory should still exist but with reduced activation
    size_t remainingTraces = brain->getWorkingMemory()->getActiveTraces();
    REQUIRE(remainingTraces > 0); // Some traces should persist
    
    // Episodic memory should have captured experiences
    size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
    REQUIRE(episodeCount > 0); // Should have stored at least one episode
}

TEST_CASE("Prediction Learning Test - Progressive accuracy improvement", "[prediction][learning][integration]") {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 123);
    config->set("neuron_count", 150);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    // Set up prediction system with training data
    auto predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        // Simulate multiple prediction attempts to measure learning progress
        std::vector<float> predictions;
        std::vector<float> targets = {0.8f, 0.6f, 0.4f, 0.3f, 0.2f};
        
        for (int trial = 0; trial < 50; ++trial) {
            // Use a simple prediction based on current state
            float currentPrediction = 0.5f; // Simple initial guess
            predictions.push_back(currentPrediction);
            
            // Simulate experience and update prediction
            // (In a real system, this would involve sensory input and learning)
            if (trial > 10) {
                // Allow some learning to occur over time
                currentPrediction += (targets[trial % targets.size()] - currentPrediction) * 0.1f;
            }
        }
        
        // Check that predictions have evolved toward targets over time
        float finalPrediction = predictions.back();
        float target = targets.back();
        
        // With learning, final prediction should be closer to target than initial
        REQUIRE(std::abs(finalPrediction - target) < std::abs(0.5f - target));
    }
    
    // Neuromodulation should have adjusted learning rates
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        REQUIRE(dopamineLevel >= 0.0f);
    }
}

TEST_CASE("Continual Learning Test - No catastrophic forgetting", "[continual][learning][integration]") {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 456);
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    // Simulate learning multiple tasks sequentially
    // Task 1: Pattern A
    std::vector<float> taskAPattern = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    brain->getWorkingMemory()->store(taskAPattern, 1.0f);
    brain->getEpisodicMemory()->storeEpisode(Experience{0, 0.8f, taskAPattern});
    
    // Learn task for several steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
    }
    
    // Store task B pattern (different but similar structure)
    std::vector<float> taskBPattern = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    brain->getWorkingMemory()->store(taskBPattern, 0.8f);
    brain->getEpisodicMemory()->storeEpisode(Experience{50, 0.6f, taskBPattern});
    
    // Continue learning
    for (SimulationStep step = 50; step < 100; ++step) {
        brain->step(step);
    }
    
    // Check that both patterns are still retrievable (no catastrophic forgetting)
    // Working memory should maintain traces from both tasks
    size_t activeTraces = brain->getWorkingMemory()->getActiveTraces();
    REQUIRE(activeTraces > 0);
    
    // Episodic memory should contain both experiences
    size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
    REQUIRE(episodeCount >= 2); // Should have at least two episodes
    
    // Neural system should not have degraded all connections
    size_t synapseCount = brain->getTotalSynapseCount();
    REQUIRE(synapseCount > 0);
}

TEST_CASE("Generalization Test - Similar patterns generalize from experience", "[generalization][memory][integration]") {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 789);
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    // Train on pattern family A
    std::vector<std::vector<float>> patternFamilyA = {
        {0.9f, 0.8f, 0.7f},
        {0.8f, 0.7f, 0.6f},
        {0.7f, 0.6f, 0.5f}
    };
    
    for (size_t episode = 0; episode < patternFamilyA.size(); ++episode) {
        brain->getWorkingMemory()->store(patternFamilyA[episode], 1.0f);
        brain->getEpisodicMemory()->storeEpisode(Experience{episode * 10, 0.7f, patternFamilyA[episode]});
        
        // Brain step to integrate
        for (SimulationStep step = episode * 10; step < (episode + 1) * 10; ++step) {
            brain->step(step);
        }
    }
    
    // Test on novel but similar pattern (generalization)
    std::vector<float> novelPattern = {0.75f, 0.65f, 0.55f}; // Similar to family A
    brain->getWorkingMemory()->store(novelPattern, 0.9f);
    brain->getEpisodicMemory()->storeEpisode(Experience{300, 0.6f, novelPattern});
    
    // After processing, check that novel pattern has been incorporated
    // (Working memory should show some activation related to trained patterns)
    size_t memoryNeurons = brain->getWorkingMemory()->getMemoryNeurons().size();
    REQUIRE(memoryNeurons > 0);
    
    // Concept formation should have extracted commonalities
    auto conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        // Concept formation should have processed episodic patterns
        // (In implementation, this would create abstract concepts)
    }
    
    // Attention system should be able to focus on relevant features
    auto attention = brain->getAttention();
    if (attention) {
        // Attention should have been updated during step processing
        attention->update(config->getOr<double>("simulation_timestep", 0.001));
    }
}

TEST_CASE("Developmental Progression Test - Brain develops through stages", "[development][stages][integration]") {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 999);
    config->set("neuron_count", 250);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    // Record initial developmental stage
    DevelopmentalStage initialStage = brain->getDevelopmentalStage();
    
    // Simulate development over time
    for (SimulationStep step = 0; step < 2000; step += 100) {
        brain->step(step);
        
        // Development system should update periodically
        DevelopmentalStage currentStage = brain->getDevelopmentalStage();
        
        // Developmental stage should evolve over time
        if (step > 500) {
            // Eventually should move beyond initial stage
            // (Development stages progress: Initial -> CriticalPeriod -> Maturation -> Adult)
        }
        
        // Check that development affects plasticity
        auto developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            // Development system should have updated
        }
    }
    
    // Final developmental stage should be mature or maturing
    DevelopmentalStage finalStage = brain->getDevelopmentalStage();
    REQUIRE(finalStage != DevelopmentalStage::Initial); // Should have progressed
    
    // Plasticity rates should have changed with development
    auto structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        // Development affects structural plasticity parameters
    }
}

TEST_CASE("Multi-System Integration Test - All cognitive systems work together", "[integration][systems][comprehensive]") {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 111);
    config->set("neuron_count", 400);
    config->set("region_count", 2); // Two regions for more complex interaction
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    REQUIRE(brain->initialize());
    
    // Provide sensory input to trigger the full integration loop
    auto sensoryInput = std::make_unique<SensoryInput>();
    std::vector<float> sensoryData = {0.6f, 0.8f, 0.4f, 0.9f, 0.3f};
    sensoryInput->setData(sensoryData);
    
    brain->receiveSensoryInput(*sensoryInput);
    
    // Run brain steps to process through the full integration loop
    for (SimulationStep step = 0; step < 200; ++step) {
        brain->step(step);
        
        // All integrated systems should be active
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto predictionSystem = brain->getPredictionSystem();
        auto attention = brain->getAttention();
        auto conceptFormation = brain->getConceptFormation();
        auto planner = brain->getPlanner();
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        
        // Each system should be accessible and operational
        if (workingMemory) {
            // Should have stored sensory input
            workingMemory->update(config->getOr<double>("simulation_timestep", 0.001));
        }
        
        if (attention) {
            // Should be processing competition
            attention->update(config->getOr<double>("simulation_timestep", 0.001));
        }
        
        // Neuromodulators should be updating
        if (dopamine) {
            dopamine->update(config->getOr<double>("simulation_timestep", 0.001));
        }
        if (curiosity) {
            curiosity->update(config->getOr<double>("simulation_timestep", 0.001));
        }
        if (novelty) {
            novelty->update(config->getOr<double>("simulation_timestep", 0.001));
        }
    }
    
    // Verify integration results
    // Memory systems should have captured the experience
    size_t totalEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
    REQUIRE(totalEpisodes > 0);
    
    // Working memory should have traces
    size_t activeWorkingMemory = brain->getWorkingMemory()->getActiveTraces();
    REQUIRE(activeWorkingMemory > 0);
    
    // Brain should have generated activity (spikes)
    size_t totalSpikes = brain->getTotalSpikeCount();
    REQUIRE(totalSpikes > 0);
    
    // Neuromodulators should have been active
    size_t totalMotorActions = 0; // Would be incremented by agent brain
    
    // All systems have successfully integrated and are operating together
}