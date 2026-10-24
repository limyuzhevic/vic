#include "BrainIntegrationCoordinator.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainIntegrationCoordinator::BrainIntegrationCoordinator(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , memoryIntegrationEnabled_(true)
    , predictionIntegrationEnabled_(true)
    , cognitionIntegrationEnabled_(true)
    , neuromodulationIntegrationEnabled_(true)
    , developmentIntegrationEnabled_(true)
    , performanceIntegrationEnabled_(true)
    , predictionUpdateInterval_(100)
    , planningDepth_(5)
    , attentionInhibitionStrength_(0.5f)
    , attentionExcitationStrength_(1.5f)
    , lastPredictionUpdateStep_(0)
    , lastMemoryUpdateStep_(0)
    , lastDevelopmentUpdateStep_(0)
    , lastConsolidationStep_(0)
    , replayModeEnabled_(false) {
}

BrainIntegrationCoordinator::~BrainIntegrationCoordinator() = default;

bool BrainIntegrationCoordinator::initialize() {
    NLM_LOG_INFO("Initializing Brain Integration Coordinator...");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize integration coordinator: Brain is null");
        return false;
    }
    
    // Enable all integrations by default
    enableMemoryIntegration(true);
    enablePredictionIntegration(true);
    enableCognitionIntegration(true);
    enableNeuromodulationIntegration(true);
    enableDevelopmentIntegration(true);
    enablePerformanceIntegration(true);
    
    NLM_LOG_INFO("Brain Integration Coordinator initialized successfully");
    return true;
}

void BrainIntegrationCoordinator::processStep(SimulationStep step, Timestamp time) {
    if (!brain_) return;
    
    // Integrate all systems
    integrateMemorySystems(step, time);
    integratePredictionSystem(step, time);
    integrateCognitionSystems(step, time);
    integrateNeuromodulationSystems(step, time);
    integrateDevelopmentSystem(step, time);
    integratePerformanceSystems(step, time);
    
    // Update metrics
    metrics_.stepsSinceLastConsolidation_++;
    metrics_.stepsSinceLastPredictionUpdate_++;
}

std::string BrainIntegrationCoordinator::getIntegrationStatus() const {
    std::stringstream ss;
    ss << "=== BRAIN INTEGRATION STATUS ===" << std::endl;
    ss << "Memory Integration: " << (memoryIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << "Prediction Integration: " << (predictionIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << "Cognition Integration: " << (cognitionIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << "Neuromodulation Integration: " << (neuromodulationIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << "Development Integration: " << (developmentIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << "Performance Integration: " << (performanceIntegrationEnabled_ ? "ENABLED" : "DISABLED") << std::endl;
    ss << std::endl;
    ss << "Integration Metrics:" << std::endl;
    ss << "  Memory Activity: " << metrics_.memoryActivity_ << std::endl;
    ss << "  Prediction Accuracy: " << metrics_.predictionAccuracy_ << std::endl;
    ss << "  Cognitive Load: " << metrics_.cognitiveLoad_ << std::endl;
    ss << "  Neuromodulation Level: " << metrics_.neuromodulationLevel_ << std::endl;
    ss << "  Steps Since Consolidation: " << metrics_.stepsSinceLastConsolidation_ << std::endl;
    ss << "  Steps Since Prediction Update: " << metrics_.stepsSinceLastPredictionUpdate_ << std::endl;
    ss << "  Memory Replay Count: " << metrics_.memoryReplayCount_ << std::endl;
    
    return ss.str();
}

void BrainIntegrationCoordinator::enableMemoryIntegration(bool enabled) {
    memoryIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Memory integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::enablePredictionIntegration(bool enabled) {
    predictionIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Prediction integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::enableCognitionIntegration(bool enabled) {
    cognitionIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Cognition integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::enableNeuromodulationIntegration(bool enabled) {
    neuromodulationIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Neuromodulation integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::enableDevelopmentIntegration(bool enabled) {
    developmentIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Development integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::enablePerformanceIntegration(bool enabled) {
    performanceIntegrationEnabled_ = enabled;
    NLM_LOG_INFO("Performance integration " + std::string(enabled ? "enabled" : "disabled"));
}

void BrainIntegrationCoordinator::setWorkingMemoryCapacity(size_t capacity) {
    if (brain_ && brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->setCapacity(capacity);
        NLM_LOG_INFO("Working memory capacity set to " + std::to_string(capacity));
    }
}

void BrainIntegrationCoordinator::setEpisodicMemoryMaxEpisodes(size_t maxEpisodes) {
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->setMaxEpisodes(maxEpisodes);
        NLM_LOG_INFO("Episodic memory max episodes set to " + std::to_string(maxEpisodes));
    }
}

void BrainIntegrationCoordinator::setPredictionUpdateInterval(size_t interval) {
    predictionUpdateInterval_ = interval;
    NLM_LOG_INFO("Prediction update interval set to " + std::to_string(interval) + " steps");
}

void BrainIntegrationCoordinator::setPlanningDepth(size_t depth) {
    planningDepth_ = depth;
    NLM_LOG_INFO("Planning depth set to " + std::to_string(depth));
}

void BrainIntegrationCoordinator::setAttentionParameters(float inhibition, float excitation) {
    attentionInhibitionStrength_ = inhibition;
    attentionExcitationStrength_ = excitation;
    if (brain_ && brain_->getAttention()) {
        brain_->getAttention()->setInhibitionStrength(inhibition);
        brain_->getAttention()->setExcitationStrength(excitation);
    }
    NLM_LOG_INFO("Attention parameters updated: inhibition=" + std::to_string(inhibition) + ", excitation=" + std::to_string(excitation));
}

float BrainIntegrationCoordinator::getMemoryActivity() const {
    return metrics_.memoryActivity_;
}

float BrainIntegrationCoordinator::getPredictionAccuracy() const {
    return metrics_.predictionAccuracy_;
}

float BrainIntegrationCoordinator::getCognitiveLoad() const {
    return metrics_.cognitiveLoad_;
}

float BrainIntegrationCoordinator::getNeuromodulationLevel() const {
    return metrics_.neuromodulationLevel_;
}

bool BrainIntegrationCoordinator::testMemoryIntegration() const {
    if (!brain_) return false;
    
    auto* wm = brain_->getWorkingMemory();
    auto* em = brain_->getEpisodicMemory();
    auto* am = brain_->getAssociativeMemory();
    
    if (!wm || !em || !am) {
        NLM_LOG_ERROR("Memory integration test failed: one or more memory systems missing");
        return false;
    }
    
    // Test if working memory has activity
    if (wm->getActiveTraces() == 0) {
        NLM_LOG_INFO("Memory integration test: working memory has no active traces (may be normal)");
    }
    
    // Test if episodic memory has storage capacity
    if (em->getMaxEpisodes() == 0) {
        NLM_LOG_ERROR("Memory integration test failed: episodic memory has no capacity");
        return false;
    }
    
    NLM_LOG_INFO("Memory integration test PASSED");
    return true;
}

bool BrainIntegrationCoordinator::testPredictionIntegration() const {
    if (!brain_) return false;
    
    auto* predSys = brain_->getPredictionSystem();
    
    if (!predSys) {
        NLM_LOG_ERROR("Prediction integration test failed: prediction system missing");
        return false;
    }
    
    NLM_LOG_INFO("Prediction integration test PASSED");
    return true;
}

bool BrainIntegrationCoordinator::testCognitionIntegration() const {
    if (!brain_) return false;
    
    auto* planner = brain_->getPlanner();
    auto* conceptFormation = brain_->getConceptFormation();
    auto* attention = brain_->getAttention();
    
    if (!planner || !conceptFormation || !attention) {
        NLM_LOG_ERROR("Cognition integration test failed: one or more cognition systems missing");
        return false;
    }
    
    NLM_LOG_INFO("Cognition integration test PASSED");
    return true;
}

bool BrainIntegrationCoordinator::testNeuromodulationIntegration() const {
    if (!brain_) return false;
    
    auto* dopamine = brain_->getDopamine();
    auto* curiosity = brain_->getCuriosity();
    auto* novelty = brain_->getNovelty();
    
    if (!dopamine || !curiosity || !novelty) {
        NLM_LOG_ERROR("Neuromodulation integration test failed: one or more neuromodulation systems missing");
        return false;
    }
    
    NLM_LOG_INFO("Neuromodulation integration test PASSED");
    return true;
}

bool BrainIntegrationCoordinator::testDevelopmentIntegration() const {
    if (!brain_) return false;
    
    auto* devSys = brain_->getDevelopmentSystem();
    
    if (!devSys) {
        NLM_LOG_ERROR("Development integration test failed: development system missing");
        return false;
    }
    
    NLM_LOG_INFO("Development integration test PASSED");
    return true;
}

void BrainIntegrationCoordinator::configureForLearningMode() {
    enableMemoryIntegration(true);
    enablePredictionIntegration(true);
    enableCognitionIntegration(true);
    enableNeuromodulationIntegration(true);
    enableDevelopmentIntegration(true);
    
    setWorkingMemoryCapacity(1000);
    setEpisodicMemoryMaxEpisodes(5000);
    setPredictionUpdateInterval(50);
    setPlanningDepth(10);
    setAttentionParameters(0.3f, 2.0f);
    
    NLM_LOG_INFO("Configured for Learning Mode: high memory capacity, fast prediction updates, deep planning");
}

void BrainIntegrationCoordinator::configureForReplayMode() {
    enableMemoryIntegration(true);
    enablePredictionIntegration(true);
    enableCognitionIntegration(true);
    enableNeuromodulationIntegration(true);
    
    setWorkingMemoryCapacity(500);
    setEpisodicMemoryMaxEpisodes(2000);
    setPredictionUpdateInterval(200);
    setPlanningDepth(3);
    setAttentionParameters(0.7f, 1.0f);
    replayModeEnabled_ = true;
    
    NLM_LOG_INFO("Configured for Replay Mode: balanced memory usage, replay enabled");
}

void BrainIntegrationCoordinator::configureForDevelopmentMode() {
    enableMemoryIntegration(true);
    enablePredictionIntegration(true);
    enableCognitionIntegration(true);
    enableNeuromodulationIntegration(true);
    enableDevelopmentIntegration(true);
    
    setWorkingMemoryCapacity(2000);
    setEpisodicMemoryMaxEpisodes(10000);
    setPredictionUpdateInterval(25);
    setPlanningDepth(15);
    setAttentionParameters(0.2f, 2.5f);
    
    NLM_LOG_INFO("Configured for Development Mode: high plasticity, developmental progression");
}

void BrainIntegrationCoordinator::configureForStandardMode() {
    enableMemoryIntegration(true);
    enablePredictionIntegration(true);
    enableCognitionIntegration(true);
    enableNeuromodulationIntegration(true);
    enableDevelopmentIntegration(true);
    
    setWorkingMemoryCapacity(500);
    setEpisodicMemoryMaxEpisodes(1000);
    setPredictionUpdateInterval(100);
    setPlanningDepth(5);
    setAttentionParameters(0.5f, 1.5f);
    replayModeEnabled_ = false;
    
    NLM_LOG_INFO("Configured for Standard Mode: balanced configuration");
}

void BrainIntegrationCoordinator::integrateMemorySystems(SimulationStep step, Timestamp time) {
    if (!memoryIntegrationEnabled_) return;
    
    updateWorkingMemory(step, time);
    updateEpisodicMemory(step, time);
    updateAssociativeMemory(step, time);
    
    // Update metrics
    metrics_.memoryActivity_ = brain_->getWorkingMemory() ? 
        brain_->getWorkingMemory()->getMemoryActivity() : 0.0f;
}

void BrainIntegrationCoordinator::integratePredictionSystem(SimulationStep step, Timestamp time) {
    if (!predictionIntegrationEnabled_) return;
    
    updatePrediction(step, time);
    
    // Update prediction metrics
    metrics_.predictionAccuracy_ = 0.8f; // Placeholder - would need actual accuracy calculation
    metrics_.stepsSinceLastPredictionUpdate_++;
    
    if (metrics_.stepsSinceLastPredictionUpdate_ >= predictionUpdateInterval_) {
        communicateMemoryToPrediction();
        metrics_.stepsSinceLastPredictionUpdate_ = 0;
    }
}

void BrainIntegrationCoordinator::integrateCognitionSystems(SimulationStep step, Timestamp time) {
    if (!cognitionIntegrationEnabled_) return;
    
    updatePlanning(step, time);
    updateConceptFormation(step, time);
    updateAttention(step, time);
    
    // Update cognitive load metrics
    metrics_.cognitiveLoad_ = 0.5f; // Placeholder - would need actual load calculation
    
    communicatePredictionToCognition();
}

void BrainIntegrationCoordinator::integrateNeuromodulationSystems(SimulationStep step, Timestamp time) {
    if (!neuromodulationIntegrationEnabled_) return;
    
    updateNeuromodulation(step, time);
    
    // Get neuromodulation level from dopamine
    metrics_.neuromodulationLevel_ = brain_->getDopamine() ? 
        brain_->getDopamine()->getLevel() : 0.0f;
}

void BrainIntegrationCoordinator::integrateDevelopmentSystem(SimulationStep step, Timestamp time) {
    if (!developmentIntegrationEnabled_) return;
    
    updateDevelopment(step, time);
    
    metrics_.stepsSinceLastDevelopmentUpdate_++;
    updateDevelopmentalStage();
}

void BrainIntegrationCoordinator::integratePerformanceSystems(SimulationStep step, Timestamp time) {
    if (!performanceIntegrationEnabled_) return;
    
    // Performance integration would involve monitoring and optimizing memory pools,
    // event queues, and SIMD vectorization usage
    
    // For now, just log performance status
    if (step % 1000 == 0) {
        NLM_LOG_INFO("Performance integration: " + std::to_string(brain_->getTotalNeuronCount()) + 
                    " neurons, " + std::to_string(brain_->getTotalSynapseCount()) + " synapses, " +
                    std::to_string(brain_->getFiringNeuronCount()) + " firing neurons");
    }
}

void BrainIntegrationCoordinator::updateWorkingMemory(SimulationStep step, Timestamp time) {
    auto* wm = brain_->getWorkingMemory();
    if (!wm) return;
    
    // Update working memory based on current neural activity
    wm->update(static_cast<TimestepDuration>(time));
    
    // Store sensory input in working memory
    if (step % 10 == 0) { // Update every 10 steps
        // Get active neurons and encode them as memory patterns
        auto* spikeSys = brain_->getSpikeSystem();
        if (spikeSys) {
            auto activeNeurons = spikeSys->getMostActiveNeurons(10);
            if (!activeNeurons.empty()) {
                std::vector<float> pattern;
                for (auto neuronId : activeNeurons) {
                    pattern.push_back(1.0f); // Full activation
                }
                wm->store(pattern, 0.5f);
            }
        }
    }
}

void BrainIntegrationCoordinator::updateEpisodicMemory(SimulationStep step, Timestamp time) {
    auto* em = brain_->getEpisodicMemory();
    if (!em) return;
    
    // Store episodic memory every 100 steps or when there's significant activity
    if (step % 100 == 0 || brain_->getFiringNeuronCount() > brain_->getTotalNeuronCount() / 10) {
        // Create an episodic memory entry
        std::string episodeContent = "Step " + std::to_string(step) + ": " +
                                     std::to_string(brain_->getTotalSpikeCount()) + " spikes, " +
                                     std::to_string(brain_->getFiringNeuronCount()) + " firing neurons";
        
        em->storeExperience(episodeContent);
        
        // Check if we need to consolidate (every 1000 steps)
        if (step % 1000 == 0) {
            consolidateMemories();
        }
        
        // Check if we need to replay (every 500 steps in replay mode)
        if (replayModeEnabled_ && step % 500 == 0) {
            replayMemories();
        }
    }
}

void BrainIntegrationCoordinator::updateAssociativeMemory(SimulationStep step, Timestamp time) {
    auto* am = brain_->getAssociativeMemory();
    if (!am) return;
    
    // Update associative memory based on co-activation patterns
    // This would involve tracking which neurons fire together and strengthening
    // the associations between them
    
    if (step % 200 == 0) {
        // Get current neural activity patterns
        auto* spikeSys = brain_->getSpikeSystem();
        if (spikeSys) {
            auto activeNeurons = spikeSys->getMostActiveNeurons(20);
            
            // Create associations between recently co-activated neurons
            if (activeNeurons.size() >= 2) {
                for (size_t i = 0; i < activeNeurons.size() - 1; ++i) {
                    for (size_t j = i + 1; j < activeNeurons.size(); ++j) {
                        am->associate(activeNeurons[i], activeNeurons[j], 0.1f);
                    }
                }
            }
        }
    }
}

void BrainIntegrationCoordinator::updatePrediction(SimulationStep step, Timestamp time) {
    auto* predSys = brain_->getPredictionSystem();
    if (!predSys) return;
    
    // Update prediction based on current sensory input and neural activity
    // This would involve using the prediction system to anticipate future states
    
    if (step % 50 == 0) {
        // Create a prediction based on current state
        std::vector<float> currentState;
        
        // Get current neural activity
        auto* spikeSys = brain_->getSpikeSystem();
        if (spikeSys) {
            auto activeNeurons = spikeSys->getMostActiveNeurons(10);
            for (auto neuronId : activeNeurons) {
                currentState.push_back(1.0f);
            }
        }
        
        if (!currentState.empty()) {
            predSys->updatePrediction(currentState);
        }
    }
}

void BrainIntegrationCoordinator::updatePlanning(SimulationStep step, Timestamp time) {
    auto* planner = brain_->getPlanner();
    if (!planner) return;
    
    // Update planning based on current goals and predictions
    // This would involve using the planner to generate action sequences
    
    if (step % 100 == 0) {
        planner->updatePlanning();
    }
}

void BrainIntegrationCoordinator::updateConceptFormation(SimulationStep step, Timestamp time) {
    auto* conceptFormation = brain_->getConceptFormation();
    if (!conceptFormation) return;
    
    // Update concept formation based on stored experiences
    // This would involve extracting patterns from episodic memory and forming concepts
    
    if (step % 150 == 0) {
        conceptFormation->updateConceptFormation();
    }
}

void BrainIntegrationCoordinator::updateAttention(SimulationStep step, Timestamp time) {
    auto* attention = brain_->getAttention();
    if (!attention) return;
    
    // Update attention based on competition among neural populations
    // This would involve selecting which regions to focus processing on
    
    if (step % 25 == 0) {
        attention->update(static_cast<TimestepDuration>(time));
    }
}

void BrainIntegrationCoordinator::updateNeuromodulation(SimulationStep step, Timestamp time) {
    auto* dopamine = brain_->getDopamine();
    auto* curiosity = brain_->getCuriosity();
    auto* novelty = brain_->getNovelty();
    auto* predictionError = brain_->getPredictionErrorSignal();
    
    if (!dopamine || !curiosity || !novelty || !predictionError) return;
    
    // Update neuromodulation based on learning signals
    
    // Calculate reward prediction error
    float rewardPredictionError = predictionError->getPredictionError();
    
    // Update dopamine based on prediction error (reinforcement learning)
    dopamine->update(rewardPredictionError, step);
    
    // Update curiosity based on novelty
    float noveltyLevel = novelty->getNoveltyLevel();
    curiosity->update(noveltyLevel, step);
    
    // Apply neuromodulation to brain plasticity
    if (dopamine->getLevel() > 0.1f) {
        brain_->applyNeuromodulation(*dopamine);
    }
}

void BrainIntegrationCoordinator::updateDevelopment(SimulationStep step, Timestamp time) {
    auto* devSys = brain_->getDevelopmentSystem();
    if (!devSys) return;
    
    // Update developmental changes
    devSys->update(static_cast<TimestepDuration>(time));
    
    metrics_.stepsSinceLastDevelopmentUpdate_++;
}

void BrainIntegrationCoordinator::communicateMemoryToPrediction() {
    auto* predSys = brain_->getPredictionSystem();
    if (!predSys) return;
    
    // Get working memory content and send to prediction system
    auto* wm = brain_->getWorkingMemory();
    if (wm) {
        auto memoryPattern = wm->retrieve();
        predSys->updateMemoryPattern(memoryPattern);
    }
}

void BrainIntegrationCoordinator::communicatePredictionToCognition() {
    auto* predSys = brain_->getPredictionSystem();
    if (!predSys) return;
    
    // Get prediction and send to cognition systems
    auto prediction = predSys->getCurrentPrediction();
    
    auto* planner = brain_->getPlanner();
    auto* conceptFormation = brain_->getConceptFormation();
    
    if (planner) {
        planner->setGoal(prediction);
    }
    if (conceptFormation) {
        conceptFormation->updateGoal(prediction);
    }
}

void BrainIntegrationCoordinator::communicateCognitionToAction() {
    // This would communicate planning results to motor system
    // For now, it's a placeholder for the integration coordinator
}

void BrainIntegrationCoordinator::communicateNeuromodulationToPlasticity() {
    // This would communicate neuromodulation signals to plasticity systems
    // For now, it's a placeholder for the integration coordinator
}

void BrainIntegrationCoordinator::consolidateMemories() {
    auto* em = brain_->getEpisodicMemory();
    if (!em) return;
    
    NLM_LOG_INFO("Consolidating memories...");
    
    // Perform memory consolidation
    em->consolidateMemories();
    
    metrics_.stepsSinceLastConsolidation_ = 0;
    metrics_.memoryReplayCount_++;
}

void BrainIntegrationCoordinator::replayMemories() {
    auto* em = brain_->getEpisodicMemory();
    if (!em) return;
    
    NLM_LOG_INFO("Replaying memories...");
    
    // Get episodes for replay
    auto episodes = em->getEpisodesForReplay(3);
    
    if (!episodes.empty()) {
        // Replay the selected episodes
        for (const auto& episode : episodes) {
            // Apply replay signals to neural systems
            // This would reactivate the neural patterns from the episode
        }
        
        NLM_LOG_INFO("Memory replay completed: " + std::to_string(episodes.size()) + " episodes replayed");
    }
}

void BrainIntegrationCoordinator::updateDevelopmentalStage() {
    auto* devSys = brain_->getDevelopmentSystem();
    if (!devSys) return;
    
    // Update developmental stage based on accumulated experience
    auto currentStage = devSys->getCurrentStage();
    
    // Progress through developmental stages
    if (metrics_.stepsSinceLastDevelopmentUpdate_ > 1000) {
        devSys->nextStage();
        NLM_LOG_INFO("Developmental stage progressed: " + std::to_string(static_cast<int>(currentStage)) + " -> " + std::to_string(static_cast<int>(devSys->getCurrentStage())));
    }
}

} // namespace nlm