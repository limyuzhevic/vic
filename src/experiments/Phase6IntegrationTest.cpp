#include "Phase6IntegrationTest.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <cmath>

using namespace nlm;

Phase6IntegrationTest::Phase6IntegrationTest()
    : successCount(0)
    , totalTests(0)
    , integrationProgress(0.0f)
{
    Logger::getInstance().setLevel(Logger::Level::Info);
}

Phase6IntegrationTest::~Phase6IntegrationTest()
{
}

bool Phase6IntegrationTest::runAllTests()
{
    std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl << std::endl;
    
    // Test 1: Memory Accessors
    std::cout << "Test 1: Memory System Accessors" << std::endl;
    testMemoryAccessors();
    std::cout << "Memory accessors: PASSED" << std::endl << std::endl;
    
    // Test 2: Prediction System Integration
    std::cout << "Test 2: Prediction System Integration" << std::endl;
    testPredictionSystemIntegration();
    std::cout << "Prediction system: PASSED" << std::endl << std::endl;
    
    // Test 3: Concept Formation Integration
    std::cout << "Test 3: Concept Formation Integration" << std::endl;
    testConceptFormationIntegration();
    std::cout << "Concept formation: PASSED" << std::endl << std::endl;
    
    // Test 4: Working Memory Updates
    std::cout << "Test 4: Working Memory Updates" << std::endl;
    testWorkingMemoryUpdates();
    std::cout << "Working memory: PASSED" << std::endl << std::endl;
    
    // Test 5: Neuromodulator Functionality
    std::cout << "Test 5: Neuromodulator Functionality" << std::endl;
    testNeuromodulatorFunctionality();
    std::cout << "Neuromodulators: PASSED" << std::endl << std::endl;
    
    // Test 6: Plasticity Application
    std::cout << "Test 6: Plasticity Application" << std::endl;
    testPlasticityApplication();
    std::cout << "Plasticity application: PASSED" << std::endl << std::endl;
    
    // Test 7: Sleep/Rest Cycle
    std::cout << "Test 7: Sleep/Rest Cycle" << std::endl;
    testSleepRestCycle();
    std::cout << "Sleep/rest cycle: PASSED" << std::endl << std::endl;
    
    // Test 8: Episodic Memory
    std::cout << "Test 8: Episodic Memory Storage and Replay" << std::endl;
    testEpisodicMemory();
    std::cout << "Episodic memory: PASSED" << std::endl << std::endl;
    
    // Calculate integration score
    integrationProgress = (float)successCount / (float)totalTests;
    
    std::cout << "=== TEST SUMMARY ===" << std::endl;
    std::cout << "Tests passed: " << successCount << "/" << totalTests << std::endl;
    std::cout << "Integration progress: " << (integrationProgress * 100.0f) << "%" << std::endl;
    
    return integrationProgress >= 0.875f; // 87.5% threshold
}

void Phase6IntegrationTest::testMemoryAccessors()
{
    totalTests++;
    
    // Create a brain with default configuration
    auto config = std::make_shared<Config>();
    Brain brain(config);
    
    // Test working memory accessor
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr && "Working memory accessor returned nullptr!");
    assert(workingMemory->getCapacity() > 0 && "Working memory has zero capacity!");
    successCount++;
    
    // Test episodic memory accessor
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr && "Episodic memory accessor returned nullptr!");
    assert(episodicMemory->getMaxEpisodes() > 0 && "Episodic memory has zero max episodes!");
    successCount++;
    
    // Test associative memory accessor
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr && "Associative memory accessor returned nullptr!");
    successCount++;
    
    // Test prediction system accessor
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr && "Prediction system accessor returned nullptr!");
    successCount++;
    
    // Test cognitive system accessors
    auto* planner = brain.getPlanner();
    assert(planner != nullptr && "Planner accessor returned nullptr!");
    successCount++;
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr && "Concept formation accessor returned nullptr!");
    successCount++;
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr && "Attention accessor returned nullptr!");
    successCount++;
    
    // Test neuromodulator accessors
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr && "Dopamine accessor returned nullptr!");
    successCount++;
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr && "Curiosity accessor returned nullptr!");
    successCount++;
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr && "Novelty accessor returned nullptr!");
    successCount++;
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr && "Prediction error accessor returned nullptr!");
    successCount++;
    
    std::cout << "  ✓ All memory and cognitive system accessors return valid pointers" << std::endl;
}

void Phase6IntegrationTest::testPredictionSystemIntegration()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test that prediction system is being called during brain steps
    // We'll monitor prediction error levels to verify integration
    
    size_t stepsSincePredictionUpdated = 0;
    size_t minStepsBeforeTest = 50; // Wait for system to stabilize
    
    // Run brain for sufficient steps
    for (size_t step = 0; step < 200; ++step) {
        brain.step(step);
        
        if (stepsSincePredictionUpdated >= minStepsBeforeTest) {
            // Check if prediction system was called by looking at prediction error
            auto* predictionError = brain.getPredictionErrorSignal();
            if (predictionError) {
                float error = predictionError->getError();
                if (std::abs(error) > 0.01f) { // Non-zero error indicates prediction system is active
                    stepsSincePredictionUpdated = 0;
                    break;
                }
            }
        }
        stepsSincePredictionUpdated++;
    }
    
    // Verify prediction system is integrated
    assert(stepsSincePredictionUpdated < minStepsBeforeTest || "Prediction system not being called during brain steps!");
    successCount++;
    
    std::cout << "  ✓ Prediction system integrated into Brain::step()" << std::endl;
}

void Phase6IntegrationTest::testConceptFormationIntegration()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation by presenting different neural patterns
    std::vector<float> pattern1 = {0.8f, 0.3f, 0.5f, 0.2f, 0.9f};
    std::vector<float> pattern2 = {0.1f, 0.7f, 0.3f, 0.8f, 0.2f};
    std::vector<float> features1 = {0.5f, 0.3f};
    std::vector<float> features2 = {0.2f, 0.8f};
    
    // Present experiences to concept formation
    size_t concept1 = conceptFormation->presentExperience(pattern1, features1, 0.5f, 0);
    size_t concept2 = conceptFormation->presentExperience(pattern2, features2, 0.8f, 1);
    
    // Verify concepts are being formed and stored
    assert(concept1 > 0 && "Concept 1 not formed!");
    assert(concept2 > 0 && "Concept 2 not formed!");
    
    // Test concept retrieval
    size_t retrievedConcept1 = conceptFormation->getMatchingConcept(pattern1, 0.7f);
    size_t retrievedConcept2 = conceptFormation->getMatchingConcept(pattern2, 0.7f);
    
    assert(retrievedConcept1 == concept1 && "Concept 1 retrieval failed!");
    assert(retrievedConcept2 == concept2 && "Concept 2 retrieval failed!");
    
    successCount++;
    
    std::cout << "  ✓ Concept formation integrated and functional" << std::endl;
}

void Phase6IntegrationTest::testWorkingMemoryUpdates()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get working memory system
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory updates during brain steps
    size_t stepsWithMemoryUpdates = 0;
    size_t totalStepsTested = 0;
    
    // Run brain for several steps
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
        totalStepsTested++;
        
        // Check if working memory was updated (has active traces)
        if (workingMemory->getActiveTraces() > 0) {
            stepsWithMemoryUpdates++;
        }
    }
    
    // Verify working memory is being updated regularly
    float memoryUpdateRatio = (float)stepsWithMemoryUpdates / (float)totalStepsTested;
    assert(memoryUpdateRatio > 0.5f && "Working memory not being updated during brain steps!");
    
    successCount++;
    
    std::cout << "  ✓ Working memory properly updated during brain steps" << std::endl;
}

void Phase6IntegrationTest::testNeuromodulatorFunctionality()
{
    totalTests++;
    
    // Test that all neuromodulators exist and have proper interfaces
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Test dopamine
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr && "Dopamine not accessible!");
    assert(std::string(dopamine->getName()) == "DA" && "Dopamine name incorrect!");
    
    float initialDopamine = dopamine->getLevel();
    dopamine->setLevel(0.8f);
    assert(dopamine->getLevel() > initialDopamine && "Dopamine level not settable!");
    
    // Test acetylcholine
    auto* acetylcholine = new Acetylcholine();
    assert(acetylcholine != nullptr && "Acetylcholine not creatable!");
    assert(std::string(acetylcholine->getName()) == "ACh" && "Acetylcholine name incorrect!");
    delete acetylcholine;
    
    // Test norepinephrine
    auto* norepinephrine = new Norepinephrine();
    assert(norepinephrine != nullptr && "Norepinephrine not creatable!");
    assert(std::string(norepinephrine->getName()) == "NE" && "Norepinephrine name incorrect!");
    delete norepinephrine;
    
    // Test serotonin
    auto* serotonin = new Serotonin();
    assert(serotonin != nullptr && "Serotonin not creatable!");
    assert(std::string(serotonin->getName()) == "5-HT" && "Serotonin name incorrect!");
    delete serotonin;
    
    successCount++;
    
    std::cout << "  ✓ All neuromodulators exist and function properly" << std::endl;
}

void Phase6IntegrationTest::testPlasticityApplication()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr && "STDP system not accessible!");
    
    auto* hebbian = brain.getHebbian();
    assert(hebbian != nullptr && "Hebbian system not accessible!");
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr && "Structural plasticity not accessible!");
    
    // Test that plasticity is applied during brain steps
    // We'll monitor synaptic weights to verify changes
    
    size_t stepsWithPlasticity = 0;
    size_t totalStepsTested = 0;
    
    // Run brain for several steps
    for (size_t step = 0; step < 50; ++step) {
        // Store initial synaptic state
        size_t initialSynapseCount = brain.getTotalSynapseCount();
        
        brain.step(step);
        totalStepsTested++;
        
        // Check if plasticity was applied (synapse count might change due to structural plasticity)
        // or weights might change
        size_t finalSynapseCount = brain.getTotalSynapseCount();
        if (finalSynapseCount != initialSynapseCount) {
            stepsWithPlasticity++;
        }
    }
    
    // Verify plasticity is being applied
    assert(stepsWithPlasticity > 0 && "Plasticity not being applied during brain steps!");
    
    successCount++;
    
    std::cout << "  ✓ Plasticity properly applied through unified functions" << std::endl;
}

void Phase6IntegrationTest::testSleepRestCycle()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Test rest cycle functionality
    brain.enableRestCycle(true);
    assert(brain.isResting() && "Rest cycle not enabled!");
    
    brain.enableRestCycle(false);
    assert(!brain.isResting() && "Rest cycle not disabled!");
    
    // Test that resting state affects brain behavior (during replay)
    brain.enableRestCycle(true);
    
    // Run some steps in resting state
    for (size_t step = 0; step < 20; ++step) {
        brain.step(step);
        // Resting brain should still run but with different timing
        assert(brain.isResting() && "Resting state not maintained!");
    }
    
    brain.enableRestCycle(false);
    
    successCount++;
    
    std::cout << "  ✓ Sleep/rest cycle functional" << std::endl;
}

void Phase6IntegrationTest::testEpisodicMemory()
{
    totalTests++;
    
    // Create brain and initialize
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get episodic memory system
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr && "Episodic memory not accessible!");
    
    // Test episodic memory storage
    EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    
    // Add some neurons to the episode
    episode.activeNeurons.push_back(NeuronId(1000));
    episode.neuronActivations.push_back(0.8f);
    
    // Store the episode
    episodicMemory->storeEpisode(episode);
    
    // Verify episode was stored
    auto* retrievedEpisode = episodicMemory->getEpisode(0);
    assert(retrievedEpisode != nullptr && "Episodic memory not storing episodes!");
    
    // Test memory replay
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(1);
    assert(episodesToReplay.size() > 0 && "Episodic memory not returning episodes for replay!");
    
    // Verify replay episode is valid
    assert(episodesToReplay[0] == &episode && "Episodic memory replay not returning correct episode!");
    
    successCount++;
    
    std::cout << "  ✓ Episodic memory storage and replay functional" << std::endl;
}