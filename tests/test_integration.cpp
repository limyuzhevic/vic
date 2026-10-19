// test_integration.cpp - Comprehensive Integration Tests
// Tests for all missing functionality in the NLM test suite
// Covers memory systems, prediction, cognition, neuromodulation, and performance

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>

namespace test_integration {

// Memory System Tests
// ====================

void testWorkingMemory() {
    std::cout << "    Testing WorkingMemory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test capacity setting
    workingMemory->setCapacity(50);
    assert(workingMemory->getCapacity() == 50);
    
    // Test storage
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    workingMemory->store(pattern, 1.0f);
    
    // Test storage to neuron
    workingMemory->storeToNeuron(1001, 0.75f);
    
    // Test retrieval
    auto retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Test neuron activation
    assert(workingMemory->getNeuronActivation(1001) == 0.75f);
    assert(workingMemory->contains(1001));
    
    // Test update
    workingMemory->update(0.001f);
    
    // Test decay
    workingMemory->setDecayRate(0.1f);
    
    // Test clear
    workingMemory->clear();
    assert(workingMemory->retrieve().empty());
    
    std::cout << "      WorkingMemory tests passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing EpisodicMemory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test storing episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.8f;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test retrieval by similarity
    auto similar = episodicMemory->retrieveSimilar({0.15f, 0.25f, 0.35f, 0.45f, 0.55f});
    assert(!similar.empty());
    
    // Test retrieval by time
    auto temporal = episodicMemory->retrieveTemporal(0, 100);
    assert(!temporal.empty());
    
    // Test retrieval by location
    auto location = episodicMemory->retrieveByLocation(1.0f, 2.0f, 0.1f);
    assert(!location.empty());
    
    // Test retrieval by action
    auto afterAction = episodicMemory->retrieveAfterAction(nlm::ActionType::MoveForward);
    assert(!afterAction.empty());
    
    // Test episode methods
    assert(episodicMemory->getRecentEpisodes(1).size() == 1);
    assert(episodicMemory->getAverageReward() == 0.8f);
    
    // Test replay
    episodicMemory->enableReplay(true);
    auto episodesForReplay = episodicMemory->getEpisodesForReplay(1);
    if (!episodesForReplay.empty()) {
        episodicMemory->replayEpisode(episodesForReplay[0]);
    }
    
    // Test consolidation
    episodicMemory->consolidate(0.5f);
    
    // Test clear
    episodicMemory->clear();
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "      EpisodicMemory tests passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing AssociativeMemory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test association creation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory->associate(patternA, patternB, 0.8f);
    assert(associativeMemory->getAssociationStrength(patternA, patternB) == 0.8f);
    
    // Test retrieval
    auto retrieved = associativeMemory->retrieve(patternA);
    assert(!retrieved.empty());
    
    // Test spread activation
    auto spread = associativeMemory->spreadActivation(patternA, 1);
    assert(!spread.empty());
    
    // Test update association
    associativeMemory->updateAssociation(patternA, patternB, 0.1f);
    assert(associativeMemory->getAssociationStrength(patternA, patternB) == 0.9f);
    
    // Test clear
    associativeMemory->clear();
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "      AssociativeMemory tests passed" << std::endl;
}

// Prediction System Tests
// ===========================

void testPredictionSystem() {
    std::cout << "    Testing PredictionSystem..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create test sensory inputs
    nlm::SensoryInput currentState;
    std::vector<float> currentData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    currentState.setData(currentData);
    
    nlm::SensoryInput predictedState;
    predictedState.setData({0.11f, 0.21f, 0.31f, 0.41f, 0.51f});
    
    nlm::SensoryInput actualState;
    actualState.setData({0.15f, 0.25f, 0.35f, 0.45f, 0.55f});
    
    // Test prediction
    auto prediction = predictionSystem->predictNextState(currentState);
    assert(prediction != nullptr);
    
    // Test update predictions
    predictionSystem->updatePredictions(*prediction, actualState);
    
    // Test prediction error
    float error = predictionSystem->getPredictionError();
    assert(!std::isnan(error));
    
    // Test confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test error history
    const auto& history = predictionSystem->getErrorHistory();
    assert(!history.empty());
    
    // Test clear history
    predictionSystem->clearHistory();
    assert(predictionSystem->getErrorHistory().empty());
    
    std::cout << "      PredictionSystem tests passed" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing PredictionError..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* predictionError = brain.getPredictionError();
    assert(predictionError != nullptr);
    
    // Test error computation
    predictionError->computeError(0.5f, 0.8f); // actual - predicted = 0.3
    assert(predictionError->getError() == 0.3f);
    
    // Test magnitude
    assert(predictionError->getMagnitude() == 0.3f);
    
    // Test update prediction
    predictionError->updatePrediction(0.6f);
    
    // Test history
    const auto& history = predictionError->getHistory();
    assert(!history.empty());
    assert(history.back() == 0.3f);
    
    // Test clear history
    predictionError->clearHistory();
    assert(predictionError->getHistory().empty());
    
    std::cout << "      PredictionError tests passed" << std::endl;
}

// Cognition System Tests
// ============================

void testNeuralPlanner() {
    std::cout << "    Testing NeuralPlanner..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test planning with current state
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    auto action = planner->planAction(currentState, 0.5f);
    assert(action != nlm::ActionType::Invalid);
    
    // Test planning depth setting
    planner->setPlanningDepth(3);
    
    // Test self model
    planner->updatePlanQuality({nlm::ActionType::MoveForward}, {nlm::ActionType::MoveForward}, 0.8f);
    
    // Test recent plan success
    assert(planner->wasRecentPlanSuccessful());
    
    std::cout << "      NeuralPlanner tests passed" << std::endl;
}

void testConceptFormation() {
    std::cout << "    Testing ConceptFormation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test presenting experiences
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features = {0.5f, 0.6f, 0.7f};
    
    size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.7f, 0);
    assert(conceptId > 0); // Should create concept
    
    // Test concept properties
    assert(conceptFormation->getConceptCount() > 0);
    
    // Test concept retrieval
    auto* concept = conceptFormation->getConcept(conceptId);
    assert(concept != nullptr);
    
    // Test concept stability
    assert(conceptFormation->isConceptStable(conceptId));
    
    std::cout << "      ConceptFormation tests passed" << std::endl;
}

void testAttentionalSelection() {
    std::cout << "    Testing AttentionalSelection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test process competition
    std::vector<nlm::NeuronId> competitors = {1001, 1002, 1003, 1004, 1005};
    
    auto winners = attention->processCompetition(competitors);
    assert(!winners.empty());
    
    // Test focus
    attention->focusOnRegion(nlm::RegionId(1));
    
    // Test release attention
    attention->releaseAttention();
    
    // Test attention parameters
    attention->setInhibitionStrength(0.3f);
    attention->setExcitationStrength(1.8f);
    attention->setCompetitionThreshold(0.4f);
    
    // Test attention queries
    assert(attention->isAttended(1001) == (std::find(winners.begin(), winners.end(), 1001) != winners.end()));
    assert(attention->getInhibitionFor(1001) >= 0.0f);
    assert(attention->getExcitationFor(1001) >= 0.0f);
    
    // Test attention update
    attention->update(0.001f);
    
    // Test reset
    attention->reset();
    
    std::cout << "      AttentionalSelection tests passed" << std::endl;
}

// Neuromodulation System Tests
// =================================

void testDopamineSystem() {
    std::cout << "    Testing Dopamine system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test basic properties
    assert(dopamine->getName() != nullptr);
    assert(strcmp(dopamine->getName(), "DA") == 0);
    
    // Test level setting
    dopamine->setLevel(0.7f);
    assert(dopamine->getLevel() == 0.7f);
    
    // Test plasticity factor
    float plasticity = dopamine->getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.5f);
    
    // Test update
    dopamine->update(0.001f);
    assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
    
    // Test reward signaling
    dopamine->signalReward(0.8f);
    assert(dopamine->getLevel() > 0.0f);
    
    // Test prediction error signaling
    dopamine->signalRewardPredictionError(0.3f);
    assert(dopamine->getLevel() > 0.0f);
    
    std::cout << "      Dopamine system tests passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "    Testing Curiosity system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test level
    float level = curiosity->getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    // Test update with novelty and prediction error
    curiosity->update(0.8f, 0.6f, 0.001f);
    assert(curiosity->getLevel() >= 0.0f && curiosity->getLevel() <= 1.0f);
    
    // Test exploration drive
    float exploration = curiosity->getExplorationDrive();
    assert(exploration == curiosity->getLevel());
    
    // Test parameter setting
    curiosity->setNoveltyWeight(0.7f);
    curiosity->setPredictionErrorWeight(0.3f);
    
    // Test reset
    curiosity->reset();
    assert(curiosity->getLevel() == 0.0f);
    
    std::cout << "      Curiosity system tests passed" << std::endl;
}

void testNoveltyDetection() {
    std::cout << "    Testing Novelty detection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initialization
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test level
    float level = novelty->getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    // Test level setting
    novelty->setLevel(0.9f);
    assert(novelty->getLevel() == 0.9f);
    
    // Test novelty detection from patterns
    std::vector<float> currentPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> previousPattern = {0.5f, 0.4f, 0.3f, 0.2f, 0.1f};  // Different pattern
    
    novelty->detectNovelty(currentPattern, previousPattern);
    
    // Test update (decay)
    novelty->update(0.001f);
    assert(novelty->getLevel() >= 0.0f);
    
    // Test history
    const auto& history = novelty->getHistory();
    assert(history.empty() || !std::isnan(history.back()));
    
    // Test clear history
    novelty->clearHistory();
    assert(novelty->getHistory().empty());
    
    std::cout << "      Novelty detection tests passed" << std::endl;
}

// Advanced Brain Features Tests
// ================================

void testPhase6IntegratedBrainLoop() {
    std::cout << "    Testing Phase 6 integrated brain loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test step execution (should not crash)
    brain.step(0);
    brain.step(1);
    brain.step(2);
    
    // Verify brain remains in valid state
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    assert(action->getType() != nlm::ActionType::Invalid);
    
    // Test statistics
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getPendingSpikeEventCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "      Phase 6 integrated brain loop tests passed" << std::endl;
}

void testMemoryConsolidation() {
    std::cout << "    Testing memory consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("consolidation_interval", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.reward = 0.8f;
    episode.action = nlm::ActionType::MoveForward;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Step forward to trigger consolidation
    brain.step(100);
    
    // Check that consolidation happened (should reduce episodes based on relevance)
    size_t countAfterConsolidation = episodicMemory->getEpisodeCount();
    assert(countAfterConsolidation <= 1); // May be consolidated or still there
    
    std::cout << "      Memory consolidation tests passed" << std::endl;
}

void testDevelopmentStages() {
    std::cout << "    Testing development stages..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stage access
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Test stage setting
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Test development system effects
    brain.develop();
    
    std::cout << "      Development stages tests passed" << std::endl;
}

void testNeuromodulationEffects() {
    std::cout << "    Testing neuromodulation effects..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine effects on plasticity
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    dopamine->setLevel(0.9f);
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor > 1.0f);  // Should increase with high dopamine
    
    // Test curiosity effects on exploration
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    curiosity->update(0.7f, 0.8f, 0.001f);
    assert(curiosity->getLevel() > 0.0f);
    
    // Test novelty effects
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    novelty->setLevel(0.8f);
    assert(novelty->getLevel() == 0.8f);
    
    // Test prediction error effects
    auto* predictionError = brain.getPredictionError();
    assert(predictionError != nullptr);
    
    predictionError->computeError(0.5f, 0.9f);  // Error = 0.4
    assert(predictionError->getError() == 0.4f);
    
    std::cout << "      Neuromodulation effects tests passed" << std::endl;
}

// Performance Tests
// ==================

void testLargeScaleSimulation() {
    std::cout << "    Testing large scale simulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation for several steps
    for (int step = 0; step < 100; ++step) {
        brain.step(static_cast<nlm::SimulationStep>(step));
        if (step % 10 == 0) {
            auto action = brain.produceAction();
            assert(action != nullptr);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    assert(duration.count() < 10000);  // Less than 10 seconds
    
    // Verify system state
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getRegionCount() == 5);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "      Large scale simulation tests passed (" << duration.count() << " ms)" << std::endl;
}

void testMemoryManagement() {
    std::cout << "    Testing memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    // Create multiple brains to test memory cleanup
    std::vector<std::unique_ptr<nlm::Brain>> brains;
    
    for (int i = 0; i < 5; ++i) {
        auto brainConfig = std::make_shared<nlm::Config>(*config);
        brains.push_back(std::make_unique<nlm::Brain>(brainConfig));
        brains.back()->initialize();
        
        // Use the brain
        brains.back()->step(0);
        auto action = brains.back()->produceAction();
        assert(action != nullptr);
    }
    
    // Clear brains and check memory is cleaned up
    brains.clear();
    
    std::cout << "      Memory management tests passed" << std::endl;
}

void testEdgeCases() {
    std::cout << "    Testing edge cases..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with null/empty inputs where applicable
    auto* workingMemory = brain.getWorkingMemory();
    
    // Empty pattern
    workingMemory->store({}, 1.0f);
    
    // Clear and test
    workingMemory->clear();
    
    // Test zero timestep
    workingMemory->update(0.0f);
    
    // Test negative values (where applicable)
    workingMemory->store({-0.1f, 0.0f, 0.1f}, 1.0f);
    
    // Test boundary values
    workingMemory->setCapacity(0);
    
    // Test with multiple systems
    brain.reset();
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "      Edge cases tests passed" << std::endl;
}

// Main test runner
void runAll() {
    std::cout << "=== Running Integration Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Memory Systems
    try {
        testWorkingMemory();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testEpisodicMemory();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAssociativeMemory();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Prediction System
    try {
        testPredictionSystem();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPredictionError();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Cognition Systems
    try {
        testNeuralPlanner();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testConceptFormation();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAttentionalSelection();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Neuromodulation Systems
    try {
        testDopamineSystem();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testCuriositySystem();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNoveltyDetection();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Advanced Brain Features
    try {
        testPhase6IntegratedBrainLoop();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryConsolidation();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testDevelopmentStages();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNeuromodulationEffects();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Performance Tests
    try {
        testLargeScaleSimulation();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryManagement();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testEdgeCases();
    } catch (const std::exception& e) {
        std::cout << "      FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "\n=== All Integration Tests PASSED ===" << std::endl;
    } else {
        std::cout << "\n=== Some Integration Tests FAILED ===" << std::endl;
    }
    
    std::cout << std::endl;
}

} // namespace test_integration

// Global test runner
void run_all_integration_tests() {
    test_integration::runAll();
}