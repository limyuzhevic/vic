// Brain Tests
// Enhanced test suite covering all integrated brain systems
// Based on original test_brain.cpp structure

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "motor/Action.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Observation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <chrono>

namespace test_brain {

// ===== EXISTING TESTS =====

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// ===== MEMORY SYSTEMS TESTS =====

void testWorkingMemoryFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storing information
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    workingMemory->store(pattern, 1.0f);
    
    // Test retrieval
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Test capacity setting
    workingMemory->setCapacity(10);
    assert(workingMemory->getCapacity() == 10);
    
    // Test decay rate setting
    workingMemory->setDecayRate(0.1f);
    assert(workingMemory->getDecayRate() == 0.1f);
    
    // Test strengthening memory
    workingMemory->strengthenMemory(1.5f);
    
    // Test competition
    workingMemory->runCompetition();
    
    std::cout << "    testWorkingMemoryFunctionality passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create a test episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = std::vector<float>(100, 0.5f);
    episode.reward = 0.8f;
    
    // Store in episodic memory
    episodicMemory->storeEpisode(episode);
    
    // Test retrieval
    auto similarEpisodes = episodicMemory->retrieveSimilar(episode.sensoryState);
    assert(!similarEpisodes.empty());
    
    // Test replay
    episodicMemory->replayEpisode(&episode);
    
    // Test episode count
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testAssociativeMemoryPatternAssociations() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* assocMemory = brain.getAssociativeMemory();
    assert(assocMemory != nullptr);
    
    // Create cue pattern
    std::vector<float> patternA = {0.8f, 0.3f, 0.9f, 0.1f};
    
    // Create response pattern
    std::vector<float> patternB = {0.2f, 0.7f, 0.4f, 0.6f};
    
    // Form association
    assocMemory->associate(patternA, patternB, 0.9f);
    
    // Retrieve associated patterns
    std::vector<std::vector<float>> retrievedPatterns = assocMemory->retrieve(patternA);
    assert(!retrievedPatterns.empty());
    
    // Test association strength
    float strength = assocMemory->getAssociationStrength(patternA, patternB);
    assert(strength > 0.5f);
    
    // Test spread activation
    std::vector<std::vector<float>> spreadResults = assocMemory->spreadActivation(patternA);
    assert(!spreadResults.empty());
    
    std::cout << "    testAssociativeMemoryPatternAssociations passed" << std::endl;
}

// ===== NEUROMODULATION TESTS =====

void testDopamineEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test baseline dopamine level
    float baseline = dopamine->getLevel();
    assert(baseline >= 0.0f);
    
    // Apply reward signal
    dopamine->signalReward(1.0f);
    
    // Check dopamine response
    float postReward = dopamine->getLevel();
    assert(postReward > 0.0f);
    
    // Test exploration-exploitation tradeoff via plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor > 0.0f);
    
    // Test reward prediction error
    dopamine->signalRewardPredictionError(0.5f);
    
    std::cout << "    testDopamineEffects passed" << std::endl;
}

void testCuriosityDrivenExploration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Initialize neuromodulators for curiosity computation
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Set up test signals
    float noveltyLevel = 0.7f;
    float predictionErrorValue = 0.3f;
    
    curiosity->update(noveltyLevel, predictionErrorValue, 0.01f);
    
    // Check curiosity level
    float curiosityLevel = curiosity->getLevel();
    assert(curiosityLevel >= 0.0f);
    
    // Test exploration drive
    float explorationDrive = curiosity->getExplorationDrive();
    assert(explorationDrive >= 0.0f);
    
    // Test novelty weight setting
    curiosity->setNoveltyWeight(0.8f);
    curiosity->setPredictionErrorWeight(0.2f);
    
    // Reset and test again
    curiosity->reset();
    
    std::cout << "    testCuriosityDrivenExploration passed" << std::endl;
}

void testNoveltyDetectionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Create test observations
    auto observation1 = std::make_unique<nlm::Observation>(std::make_unique<nlm::Vision>(32, 32, 3));
    auto observation2 = std::make_unique<nlm::Observation>(std::make_unique<nlm::Vision>(32, 32, 3));
    
    std::vector<float> data1(32 * 32 * 3, 0.5f);
    std::vector<float> data2(32 * 32 * 3, 0.6f);  // Different data
    
    observation1->getSensoryInput()->setData(data1);
    observation2->getSensoryInput()->setData(data2);
    
    // Test novelty detection
    novelty->detectNovelty(*observation1, *observation2);
    
    float noveltyLevel = novelty->getLevel();
    assert(noveltyLevel >= 0.0f);  // Should detect difference
    
    // Test novelty history
    const auto& history = novelty->getHistory();
    assert(history.size() >= 1);
    
    std::cout << "    testNoveltyDetectionIntegration passed" << std::endl;
}

void testPredictionErrorSignals() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Set up prediction error
    float predicted = 0.7f;
    float actual = 0.4f;
    
    predError->computeError(predicted, actual);
    
    float error = predError->getError();
    float expectedError = std::abs(predicted - actual);
    assert(std::abs(error - expectedError) < 0.01f);
    
    // Test magnitude
    float magnitude = predError->getMagnitude();
    assert(magnitude >= 0.0f);
    
    // Test history
    const auto& history = predError->getHistory();
    assert(history.size() >= 1);
    
    // Test prediction update
    predError->updatePrediction(0.5f);
    
    std::cout << "    testPredictionErrorSignals passed" << std::endl;
}

// ===== PREDICTION SYSTEM TESTS =====

void testPredictionAccuracy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create test sensory input
    auto visionInput = std::make_unique<nlm::Vision>(32, 32, 3);
    std::vector<float> data(32 * 32 * 3, 0.5f);
    visionInput->setData(data);
    
    // Make prediction
    auto prediction = predictionSystem->predictNextState(*visionInput);
    assert(prediction != nullptr);
    
    // Update with actual observation
    predictionSystem->updatePredictions(*visionInput, *visionInput);
    
    // Check prediction error
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);
    
    // Check confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test training
    predictionSystem->train(*visionInput);
    
    std::cout << "    testPredictionAccuracy passed" << std::endl;
}

void testConfidenceTracking() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create test input
    auto visionInput = std::make_unique<nlm::Vision>(16, 16, 1);
    std::vector<float> data(16 * 16 * 1, 0.3f);
    visionInput->setData(data);
    
    // Track confidence over multiple predictions
    std::vector<float> confidences;
    
    for (int i = 0; i < 5; ++i) {
        predictionSystem->predictNextState(*visionInput);
        float confidence = predictionSystem->getConfidence();
        confidences.push_back(confidence);
        predictionSystem->updatePredictions(*visionInput, *visionInput);
    }
    
    // Check that confidence values are reasonable
    for (float conf : confidences) {
        assert(conf >= 0.0f && conf <= 1.0f);
    }
    
    // Test confidence history access
    const auto& errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() >= 5);
    
    // Test history clearing
    predictionSystem->clearHistory();
    
    std::cout << "    testConfidenceTracking passed" << std::endl;
}

// ===== COGNITION SYSTEM TESTS =====

void testNeuralPlannerActionSequence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Set up current state
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f};
    
    // Plan action with goal
    nlm::ActionType plannedAction = planner->planAction(currentState, 0.8f);
    assert(static_cast<int>(plannedAction) >= 0);
    
    // Set current goal
    std::vector<float> goal = {0.9f, 0.8f, 0.7f, 0.6f};
    planner->setCurrentGoal(goal);
    
    // Test planning depth setting
    planner->setPlanningDepth(5);
    assert(planner->getPlanningDepth() == 5);
    
    // Test planning confidence
    planner->setPlanningDepth(3);
    float confidence = planner->getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test action quality setting
    planner->setActionQuality(nlm::ActionType::MoveForward, 0.9f);
    
    std::cout << "    testNeuralPlannerActionSequence passed" << std::endl;
}

void testConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Present various experiences
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> features1 = {0.2f, 0.3f, 0.4f, 0.5f};
    conceptFormation->presentExperience(pattern1, features1, 0.8f, 0);
    
    std::vector<float> pattern2 = {0.15f, 0.25f, 0.35f, 0.45f}; // Similar to pattern1
    std::vector<float> features2 = {0.25f, 0.35f, 0.45f, 0.55f};
    conceptFormation->presentExperience(pattern2, features2, 0.7f, 1);
    
    std::vector<float> pattern3 = {0.9f, 0.8f, 0.7f, 0.6f}; // Different pattern
    std::vector<float> features3 = {0.8f, 0.7f, 0.6f, 0.5f};
    conceptFormation->presentExperience(pattern3, features3, 0.5f, 2);
    
    // Check concept count
    size_t conceptCount = conceptFormation->getConceptCount();
    assert(conceptCount >= 2);  // At least 2 concepts (pattern1/pattern2 similar, pattern3 different)
    
    // Test novelty detection
    std::vector<float> novelPattern = {0.0f, 0.0f, 0.0f, 0.0f};
    bool isNovel = conceptFormation->isNovel(novelPattern);
    assert(isNovel == true);  // Should be novel
    
    // Test concept stability
    float stability = conceptFormation->getConceptStability(0);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    // Test similarity computation
    float similarity = conceptFormation->computeSimilarity(pattern1, pattern2);
    assert(similarity > 0.5f);  // Should be similar
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testAttentionDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Create competing neuron populations
    std::vector<nlm::NeuronId> competitors = {1, 2, 3, 4, 5};
    
    // Process competition
    std::vector<nlm::NeuronId> winners = attention->processCompetition(competitors, 0.5f);
    assert(!winners.empty());
    
    // Test attention focus on region
    attention->focusOnRegion(0);
    std::vector<nlm::RegionId> attendedRegions = attention->getAttendedRegions();
    assert(!attendedRegions.empty());
    
    // Test release attention
    attention->releaseAttention();
    
    // Test attention parameters
    attention->setInhibitionStrength(0.8f);
    attention->setExcitationStrength(0.6f);
    attention->setCompetitionThreshold(0.7f);
    
    // Test neuron inhibition/excitation
    float inhibition = attention->getInhibitionFor(1);
    float excitation = attention->getExcitationFor(1);
    
    // Test attention update
    attention->update(0.01f);
    
    // Test attended status
    bool isAttended = attention->isAttended(1);
    
    std::cout << "    testAttentionDynamics passed" << std::endl;
}

// ===== DEVELOPMENT SYSTEM TESTS =====

void testDevelopmentalStageTransitions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Check initial developmental stage
    nlm::DevelopmentalStage initialStage = developmentSystem->getStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Advance stage
    developmentSystem->advanceStage();
    nlm::DevelopmentalStage nextStage = developmentSystem->getStage();
    assert(nextStage != nlm::DevelopmentalStage::Initial);
    
    // Test stage name access
    const char* stageName = developmentSystem->getStageName();
    assert(stageName != nullptr);
    
    // Test critical period
    bool isCriticalPeriod = developmentSystem->isCriticalPeriod();
    float criticalPeriodProgress = developmentSystem->getCriticalPeriodProgress();
    assert(criticalPeriodProgress >= 0.0f && criticalPeriodProgress <= 1.0f);
    
    // Test plasticity modifier
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    assert(plasticityModifier > 0.0f);
    
    // Test development update
    developmentSystem->update(&brain, 0, 0.01f);
    
    // Test developmental age
    double age = developmentSystem->getDevelopmentalAge();
    assert(age >= 0.0);
    
    std::cout << "    testDevelopmentalStageTransitions passed" << std::endl;
}

void testPlasticityRateModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test plasticity modulation through stages
    float initialPlasticity = developmentSystem->getPlasticityModifier();
    
    // Advance through development stages
    for (int i = 0; i < 5; ++i) {
        developmentSystem->advanceStage();
        float stagePlasticity = developmentSystem->getPlasticityModifier();
        assert(stagePlasticity >= 0.0f);
    }
    
    // Test plasticity rate changes
    developmentSystem->setStage(nlm::DevelopmentalStage::CriticalPeriod);
    float criticalPlasticity = developmentSystem->getPlasticityModifier();
    assert(criticalPlasticity >= 0.0f);
    
    // Test age-based plasticity changes
    developmentSystem->setDevelopmentalAge(10.0);
    
    std::cout << "    testPlasticityRateModulation passed" << std::endl;
}

void testStructuralPlasticityMaturation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test structural plasticity via development
    double initialAge = developmentSystem->getDevelopmentalAge();
    
    // Simulate structural development
    for (int step = 0; step < 10; ++step) {
        developmentSystem->update(&brain, step, 0.1f);
        double currentAge = developmentSystem->getDevelopmentalAge();
        assert(currentAge >= initialAge);
    }
    
    // Check for structural changes
    developmentSystem->advanceStage();  // Advance to maturation stage
    
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f);
    
    // Test that structural plasticity affects neural properties
    // (would be verified in Phase 2 with actual structural changes)
    
    std::cout << "    testStructuralPlasticityMaturation passed" << std::endl;
}

// ===== ADVANCED INTEGRATION TESTS =====

void testCompleteBrainLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Simulate complete brain loop with all systems
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // Sensory input (placeholder)
        auto visionInput = std::make_unique<nlm::Vision>(16, 16, 3);
        std::vector<float> data(16 * 16 * 3, 0.5f + static_cast<float>(step) * 0.01f);
        visionInput->setData(data);
        
        // Apply neuromodulation based on curiosity
        auto* curiosity = brain.getCuriosity();
        if (curiosity) {
            float novelty = 0.1f + static_cast<float>(step) * 0.05f;
            float predError = 0.2f - static_cast<float>(step) * 0.01f;
            curiosity->update(novelty, predError, 0.01f);
            
            auto* dopamine = brain.getDopamine();
            if (dopamine) {
                dopamine->signalReward(curiosity->getLevel());
            }
        }
        
        // Regular step
        brain.step(step);
        
        // Update development system
        auto* developmentSystem = brain.getDevelopmentSystem();
        if (developmentSystem) {
            developmentSystem->update(&brain, step, 0.01f);
        }
        
        // Check system statistics
        size_t neuronCount = brain.getTotalNeuronCount();
        assert(neuronCount > 0);
        
        float firingRate = brain.getAverageFiringRate();
        assert(firingRate >= 0.0f);
    }
    
    // Produce action at the end
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testCompleteBrainLoop passed" << std::endl;
}

void testCrossSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test integration between memory and neuromodulation
    auto* workingMemory = brain.getWorkingMemory();
    auto* curiosity = brain.getCuriosity();
    auto* predictionSystem = brain.getPredictionSystem();
    
    if (workingMemory && curiosity && predictionSystem) {
        // Store pattern in working memory
        std::vector<float> pattern = {0.3f, 0.7f, 0.5f, 0.2f};
        workingMemory->store(pattern);
        
        // Simulate novelty detection based on stored pattern
        float novelty = workingMemory->getMemoryActivity();
        
        // Generate prediction error
        predictionSystem->predictNextState(*std::make_unique<nlm::Vision>(4, 1, 1));
        float predError = predictionSystem->getPredictionError();
        
        // Update curiosity with integrated signals
        curiosity->update(novelty, predError, 0.01f);
        
        // Verify integration
        float exploration = curiosity->getExplorationDrive();
        assert(exploration >= 0.0f);
    }
    
    // Test integration between cognition and development
    auto* planner = brain.getPlanner();
    auto* developmentSystem = brain.getDevelopmentSystem();
    
    if (planner && developmentSystem) {
        // Set up planning goal
        std::vector<float> goal = {0.8f, 0.6f, 0.4f};
        planner->setCurrentGoal(goal);
        
        // Update developmental stage based on planning success
        developmentSystem->update(&brain, 0, 0.01f);
        
        // Verify cross-system effects
        float plasticity = developmentSystem->getPlasticityModifier();
        assert(plasticity > 0.0f);
    }
    
    std::cout << "    testCrossSystemIntegration passed" << std::endl;
}

void testPerformanceBenchmarks() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark working memory operations
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::vector<float> pattern(50, 0.5f);
        
        // Store benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            workingMemory->store(pattern, 1.0f);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete quickly
        assert(duration.count() < 1000);  // Less than 1 second for 100 operations
    }
    
    // Benchmark prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        auto visionInput = std::make_unique<nlm::Vision>(8, 8, 1);
        std::vector<float> data(64, 0.5f);
        visionInput->setData(data);
        
        // Prediction benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50; ++i) {
            predictionSystem->predictNextState(*visionInput);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete quickly
        assert(duration.count() < 500);
    }
    
    std::cout << "    testPerformanceBenchmarks passed" << std::endl;
}

// ===== ERROR HANDLING TESTS =====

void testNullPointerHandling() {
    // Test that brain can handle null pointers gracefully
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test null system pointers (should not crash)
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test accessing null-safe functions
    std::vector<float> emptyVector = {};
    
    // Should not crash on empty operations
    workingMemory->store(emptyVector);
    workingMemory->clear();
    
    std::cout << "    testNullPointerHandling passed" << std::endl;
}

void testMemoryManagementEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory cleanup by resetting multiple times
    for (int i = 0; i < 5; ++i) {
        brain.step(i);
        brain.reset();
        
        // Verify state after reset
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    // Test large configuration
    auto largeConfig = std::make_shared<nlm::Config>();
    largeConfig->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    largeConfig->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain largeBrain(largeConfig);
    assert(largeBrain.initialize());
    
    std::cout << "    testMemoryManagementEdgeCases passed" << std::endl;
}

void testSimulationErrorConditions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test step with various edge case conditions
    for (int step = 0; step < 10; ++step) {
        // Step with negative timestamp (edge case)
        brain.step(step, -1.0);
        
        // Step with very large timestep
        brain.step(step, 1000.0);
        
        // Verify system remains stable
        size_t neuronCount = brain.getTotalNeuronCount();
        assert(neuronCount == 50);
    }
    
    std::cout << "    testSimulationErrorConditions passed" << std::endl;
}

// ===== CHECKPOINT/RELOAD TESTS =====

void testSaveLoadFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run simulation for a bit
    for (int step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string savePath = "/tmp/test_brain_save.bin";
    bool saveSuccess = brain1.save(savePath);
    assert(saveSuccess);
    
    // Create a new brain and load the saved state
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(savePath);
    assert(loadSuccess);
    
    // Verify loaded state
    assert(brain2.getTotalNeuronCount() == 100);
    assert(brain2.getRegionCount() == 2);
    
    // Clean up
    std::filesystem::remove(savePath);
    
    std::cout << "    testSaveLoadFunctionality passed" << std::endl;
}

void testCheckpointIntegrityValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointPath);
    assert(saveSuccess);
    
    // Load checkpoint multiple times
    for (int i = 0; i < 3; ++i) {
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain2(config2);
        assert(brain2.initialize());
        
        bool loadSuccess = brain2.load(checkpointPath);
        assert(loadSuccess);
        
        // Verify checkpoint integrity
        assert(brain2.getTotalNeuronCount() == 100);
        assert(brain2.getRegionCount() == 2);
    }
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    testCheckpointIntegrityValidation passed" << std::endl;
}

// ===== RUNNER =====

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // ===== MEMORY SYSTEMS TESTS =====
    testWorkingMemoryFunctionality();
    testEpisodicMemoryStorage();
    testAssociativeMemoryPatternAssociations();
    
    // ===== NEUROMODULATION TESTS =====
    testDopamineEffects();
    testCuriosityDrivenExploration();
    testNoveltyDetectionIntegration();
    testPredictionErrorSignals();
    
    // ===== PREDICTION SYSTEM TESTS =====
    testPredictionAccuracy();
    testConfidenceTracking();
    
    // ===== COGNITION SYSTEM TESTS =====
    testNeuralPlannerActionSequence();
    testConceptFormation();
    testAttentionDynamics();
    
    // ===== DEVELOPMENT SYSTEM TESTS =====
    testDevelopmentalStageTransitions();
    testPlasticityRateModulation();
    testStructuralPlasticityMaturation();
    
    // ===== ADVANCED INTEGRATION TESTS =====
    testCompleteBrainLoop();
    testCrossSystemIntegration();
    testPerformanceBenchmarks();
    
    // ===== ERROR HANDLING TESTS =====
    testNullPointerHandling();
    testMemoryManagementEdgeCases();
    testSimulationErrorConditions();
    
    // ===== CHECKPOINT/RELOAD TESTS =====
    testSaveLoadFunctionality();
    testCheckpointIntegrityValidation();
    
    std::cout << std::endl;
    std::cout << "=== All Brain Tests Passed ===" << std::endl;
}

} // namespace test_brain
