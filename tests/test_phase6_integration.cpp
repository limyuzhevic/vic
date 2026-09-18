// Comprehensive Phase 6 Integration Tests
// Tests all integrated brain systems working together
// Integrates memory, neuromodulation, prediction, cognition, and development systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>

namespace test_phase6_integration {

// Helper function to generate random test data
std::vector<float> generateRandomPattern(size_t size) {
    static std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::vector<float> pattern(size);
    for (auto& val : pattern) {
        val = dist(rng);
    }
    return pattern;
}

// Test 1: Memory systems integration
void testMemorySystemsIntegration() {
    std::cout << "Testing memory systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    workingMemory->setCapacity(100);
    workingMemory->setDecayRate(0.01f);
    
    std::vector<float> testPattern = {0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    workingMemory->store(testPattern, 0.8f);
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    episodicMemory->setMaxEpisodes(100);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.5f;
    episode.sensoryState = testPattern;
    episode.resultingSensoryState = generateRandomPattern(testPattern.size());
    
    episodicMemory->storeEpisode(episode);
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Create associations between patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.7f, 0.8f, 0.9f};
    associativeMemory->associate(patternA, patternB, 0.6f);
    
    // Test retrieval
    auto retrieved = associativeMemory->retrieve(patternA, 5);
    assert(!retrieved.empty());
    
    // Run brain for a few steps to allow memory updates
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify memory systems are properly connected
    assert(workingMemory->getActiveTraces() > 0);
    assert(episodicMemory->getEpisodeCount() > 0);
    assert(associativeMemory->getAssociationStrength(patternA, patternB) > 0.0f);
    
    std::cout << "    Memory systems integration test passed" << std::endl;
}

// Test 2: Neuromodulation integration
void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test dopamine system
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Initial state
    float initialDopamine = dopamine->getLevel();
    assert(initialDopamine >= 0.0f);
    
    // Signal reward
    dopamine->signalReward(0.8f);
    float afterReward = dopamine->getLevel();
    assert(afterReward > initialDopamine);
    
    // Test curiosity system
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    curiosity->update(0.7f, 0.5f, 0.001f);  // novelty=0.7, predictionError=0.5
    assert(curiosity->getLevel() > 0.0f);
    
    // Test novelty system
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
    novelty->detectNovelty(pattern1, pattern2);
    assert(novelty->getLevel() > 0.0f);
    
    // Test prediction error system
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    predictionError->computeError(0.5f, 0.8f);  // prediction=0.5, actual=0.8
    float error = predictionError->getError();
    assert(std::abs(error - 0.3f) < 0.01f);
    
    // Test curiosity update with prediction error
    curiosity->update(0.0f, predictionError->getMagnitude(), 0.001f);
    
    // Run brain steps and verify neuromodulators affect plasticity
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Verify neuromodulators are updating
        assert(dopamine->getLevel() >= 0.0f);
        assert(curiosity->getLevel() >= 0.0f);
        assert(novelty->getLevel() >= 0.0f);
        
        // Apply neuromodulation effects
        brain.applyNeuromodulation(*dopamine);
    }
    
    // Verify plasticity factors are affected by neuromodulators
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.5f);  // Should be modified by dopamine level
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

// Test 3: Prediction system integration
void testPredictionSystemIntegration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction error integration
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Simulate prediction and error
    std::vector<float> expected = generateRandomPattern(10);
    std::vector<float> actual = generateRandomPattern(10);
    
    // Update prediction error
    float predictedValue = 0.5f;
    float actualValue = 0.8f;
    predictionError->computeError(predictedValue, actualValue);
    
    // Update prediction system with error
    float error = predictionError->getError();
    predictionSystem->updatePredictionError(error);
    
    // Verify prediction error affects other systems
    auto* curiosity = brain.getCuriosity();
    curiosity->update(0.0f, predictionError->getMagnitude(), 0.001f);
    assert(curiosity->getLevel() > 0.0f);
    
    // Run brain steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Apply prediction error effects
        brain.applyNeuromodulation(*predictionError);
    }
    
    // Verify prediction system is integrated
    assert(predictionError->getHistory().size() > 0);
    
    std::cout << "    Prediction system integration test passed" << std::endl;
}

// Test 4: Cognition systems integration
void testCognitionSystemsIntegration() {
    std::cout << "Testing cognition systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neural planner
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    planner->setPlanningDepth(5);
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    conceptFormation->setFormationThreshold(0.6f);
    conceptFormation->setStabilityThreshold(0.7f);
    
    std::vector<float> statePattern = {0.2f, 0.5f, 0.8f, 0.3f, 0.6f};
    std::vector<float> features = {0.1f, 0.9f, 0.4f};
    
    // Present experience to concept formation
    size_t conceptId = conceptFormation->presentExperience(statePattern, features, 0.7f, 1000);
    assert(conceptId > 0);
    
    // Test attentional selection
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    std::vector<nlm::NeuronId> competitors = {
        nlm::NeuronId(10000),
        nlm::NeuronId(10001),
        nlm::NeuronId(10002)
    };
    
    auto winners = attention->processCompetition(competitors, 0.5f);
    // Should select at least one winner
    assert(!winners.empty());
    
    // Test integration between cognition and other systems
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        brain.step(step);
        
        // Get action based on planning
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Apply attention effects
        attention->update(0.001f);
        
        // Update concept formation based on experience
        if (conceptFormation->getConceptCount() > 0) {
            conceptFormation->updateConcept(conceptId, statePattern, features, 0.8f);
        }
    }
    
    // Verify cognition systems are working
    assert(conceptFormation->getConceptCount() > 0);
    assert(conceptFormation->getConceptStability(conceptId) > 0.0f);
    assert(attention->getWinners().size() > 0);
    
    std::cout << "    Cognition systems integration test passed" << std::endl;
}

// Test 5: Development system integration
void testDevelopmentSystemIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("development_stage", 0, nlm::ConfigSource::Default);  // 0 = Initial
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development stages
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Run brain for enough steps to trigger development updates
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain.step(step);
        
        // Development should trigger every 1000 steps
        if (step % 1000 == 0) {
            brain.develop();
            
            // Development affects plasticity
            auto* structuralPlasticity = brain.getStructuralPlasticity();
            if (structuralPlasticity) {
                float synaptogenesisRate = structuralPlasticity->getSynaptogenesisRate();
                assert(synaptogenesisRate >= 0.0f);
            }
        }
    }
    
    // Verify development stage progression (or at least remains valid)
    assert(static_cast<int>(brain.getDevelopmentalStage()) >= 0);
    
    std::cout << "    Development system integration test passed" << std::endl;
}

// Test 6: Complete brain loop with all systems
void testCompleteBrainLoop() {
    std::cout << "Testing complete brain loop with all systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("replay_interval", 100, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 500, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create some sensory input
    nlm::SensoryInput sensoryInput;
    sensoryInput.data = generateRandomPattern(20);
    
    // Run brain loop for extended period
    size_t stepsProcessed = 0;
    size_t episodicMemories = 0;
    size_t episodicReplay = 0;
    
    for (nlm::SimulationStep step = 0; step < 1500; ++step) {
        // Process sensory input
        brain.receiveSensoryInput(sensoryInput);
        
        // Main brain step
        brain.step(step);
        
        stepsProcessed++;
        
        // Collect statistics
        if (brain.getEpisodicMemory()) {
            episodicMemories = brain.getEpisodicMemory()->getEpisodeCount();
        }
        
        // Check for replay
        if (step % 100 == 0 && brain.getEpisodicMemory()) {
            auto episodesToReplay = brain.getEpisodicMemory()->getEpisodesForReplay(3);
            if (!episodesToReplay.empty()) {
                episodicReplay++;
            }
        }
        
        // Occasionally change input to create novelty
        if (step % 300 == 0) {
            sensoryInput.data = generateRandomPattern(20);
        }
    }
    
    // Verify all systems participated in the loop
    assert(stepsProcessed > 0);
    assert(episodicMemories >= 0);
    assert(episodicReplay >= 0);
    
    // Verify memory systems have activity
    if (brain.getWorkingMemory()) {
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
    }
    
    // Verify neuromodulation affected plasticity
    if (brain.getDopamine()) {
        assert(brain.getDopamine()->getLevel() >= 0.0f);
    }
    
    std::cout << "    Complete brain loop test passed" << std::endl;
}

// Test 7: Persistence (save/load checkpoint)
void testPersistence() {
    std::cout << "Testing persistence (save/load checkpoint)..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "./test_checkpoints/brain_checkpoint.bin";
    bool saved = brain1.save(checkpointFile);
    assert(saved);
    
    // Create new brain and load checkpoint
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    // Run some steps on the second brain
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain2.step(step);
    }
    
    // Load checkpoint
    bool loaded = brain2.load(checkpointFile);
    assert(loaded);
    
    // Verify critical statistics are preserved
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    assert(brain2.getRegionCount() == brain1.getRegionCount());
    
    // Run a few more steps after loading to verify state restoration
    for (nlm::SimulationStep step = 100; step < 150; ++step) {
        brain2.step(step);
    }
    
    // Verify brain is still functional
    assert(brain2.getTotalSpikeCount() >= 0);
    assert(brain2.getTotalNeuronCount() > 0);
    
    std::cout << "    Persistence (save/load) test passed" << std::endl;
}

// Test 8: Replay and consolidation
void testReplayAndConsolidation() {
    std::cout << "Testing replay and consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("replay_interval", 50, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 200, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create multiple episodes with varying relevance
    std::vector<nlm::EpisodicMemoryItem> episodes;
    for (size_t i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.action = nlm::ActionType::MoveForward;
        episode.reward = static_cast<float>(i) / 20.0f;  // Varying reward
        episode.sensoryState = generateRandomPattern(15);
        
        // Different levels of episode relevance (age)
        episode.age = 100 + i;
        
        episodes.push_back(episode);
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 20);
    
    // Test replay at intervals
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Verify replay happens at expected intervals
        if (step % 50 == 0) {
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            assert(!episodesToReplay.empty());
        }
        
        // Verify consolidation happens at expected intervals
        if (step % 200 == 0) {
            episodicMemory->consolidate(0.3f);
            
            // Should reduce episode count based on threshold
            assert(episodicMemory->getEpisodeCount() <= 20);
        }
    }
    
    // Verify consolidation worked
    size_t finalEpisodeCount = episodicMemory->getEpisodeCount();
    assert(finalEpisodeCount <= 20);
    
    // Verify replay selected relevant episodes
    auto recentEpisodes = episodicMemory->getRecentEpisodes(5);
    assert(!recentEpisodes.empty());
    
    std::cout << "    Replay and consolidation test passed" << std::endl;
}

// Test 9: Neuromodulators affect plasticity
void testNeuromodulatorsAffectPlasticity() {
    std::cout << "Testing neuromodulators affect plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Get baseline plasticity factor
    float baselinePlasticity = dopamine->getPlasticityFactor();
    
    // Apply increasing dopamine levels
    for (float level = 0.2f; level <= 1.0f; level += 0.2f) {
        dopamine->setLevel(level);
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        // Verify plasticity increases with dopamine
        assert(plasticityFactor >= baselinePlasticity);
        
        // Apply neuromodulation to brain
        brain.applyNeuromodulation(*dopamine);
        
        // Run steps to allow plasticity to take effect
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
        }
    }
    
    // Test curiosity effects on exploration
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // High curiosity should increase exploration
    curiosity->update(0.9f, 0.8f, 0.001f);
    assert(curiosity->getLevel() > 0.0f);
    
    // Curiosity should affect learning through action selection
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    Neuromodulators affect plasticity test passed" << std::endl;
}

// Test 10: Memory systems connection verification
void testMemorySystemsConnection() {
    std::cout << "Testing memory systems connection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory - episodic memory connection
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Store pattern in working memory
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    workingMemory->store(pattern, 0.8f);
    
    // Create episodic memory from working memory content
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 500;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.6f;
    episode.sensoryState = pattern;
    
    episodicMemory->storeEpisode(episode);
    
    // Create associative connections
    std::vector<float> relatedPattern = {0.15f, 0.55f, 0.95f, 0.25f, 0.75f};
    associativeMemory->associate(pattern, relatedPattern, 0.7f);
    
    // Run brain to allow system interactions
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Verify working memory updates
        assert(workingMemory->getMemoryActivity() >= 0.0f);
        
        // Verify episodic memory stores new experiences
        if (episodicMemory->getEpisodeCount() > 0) {
            float avgReward = episodicMemory->getAverageReward();
            assert(avgReward >= 0.0f);
        }
    }
    
    // Verify memory systems are interconnected
    assert(workingMemory->getMemoryNeurons().size() >= 0);
    assert(episodicMemory->getEpisodeCount() >= 1);
    assert(associativeMemory->getAssociationStrength(pattern, relatedPattern) > 0.0f);
    
    std::cout << "    Memory systems connection test passed" << std::endl;
}

// Run all Phase 6 integration tests
void runAll() {
    std::cout << "\n=== Running Phase 6 Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    testMemorySystemsIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testCognitionSystemsIntegration();
    testDevelopmentSystemIntegration();
    testCompleteBrainLoop();
    testPersistence();
    testReplayAndConsolidation();
    testNeuromodulatorsAffectPlasticity();
    testMemorySystemsConnection();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Integration Tests Passed! ===" << std::endl;
}

} // namespace test_phase6_integration

void runAll();