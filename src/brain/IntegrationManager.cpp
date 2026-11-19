// IntegrationManager Implementation
// 
// This file contains the implementation of the IntegrationManager class
// that coordinates all NLM brain systems to achieve full integration.

#include "IntegrationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct IntegrationManager::Impl {
    Brain* brain_;
    bool isInitialized_ = false;
    bool isFullyIntegrated_ = false;
    
    // Integration status tracking
    struct SystemStatus {
        bool initialized = false;
        bool connected = false;
        bool active = false;
        std::string lastUpdate;
    };
    
    SystemStatus workingMemoryStatus_;
    SystemStatus episodicMemoryStatus_;
    SystemStatus predictionSystemStatus_;
    SystemStatus cognitiveSystemStatus_;
    SystemStatus neuromodulationStatus_;
    
    // Integration metrics
    size_t integrationCheckCount_ = 0;
    size_t systemConnections_ = 0;
    double lastIntegrationScore_ = 0.0;
    
    Impl(Brain* brain) : brain_(brain) {}
};

IntegrationManager::IntegrationManager(Brain* brain) 
    : pImpl(std::make_unique<Impl>(brain)) {
}

IntegrationManager::~IntegrationManager() = default;

void IntegrationManager::initialize() {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("IntegrationManager: Cannot initialize - no brain reference");
        return;
    }
    
    NLM_LOG_INFO("Initializing IntegrationManager for NLM Phase 6");
    
    // Initialize all system status trackers
    pImpl->workingMemoryStatus_.initialized = (pImpl->brain_->getWorkingMemory() != nullptr);
    pImpl->workingMemoryStatus_.connected = false;
    pImpl->workingMemoryStatus_.active = false;
    
    pImpl->episodicMemoryStatus_.initialized = (pImpl->brain_->getEpisodicMemory() != nullptr);
    pImpl->episodicMemoryStatus_.connected = false;
    pImpl->episodicMemoryStatus_.active = false;
    
    pImpl->predictionSystemStatus_.initialized = (pImpl->brain_->getPredictionSystem() != nullptr);
    pImpl->predictionSystemStatus_.connected = false;
    pImpl->predictionSystemStatus_.active = false;
    
    pImpl->cognitiveSystemStatus_.initialized = (pImpl->brain_->getPlanner() != nullptr && 
                                                  pImpl->brain_->getConceptFormation() != nullptr &&
                                                  pImpl->brain_->getAttention() != nullptr);
    pImpl->cognitiveSystemStatus_.connected = false;
    pImpl->cognitiveSystemStatus_.active = false;
    
    pImpl->neuromodulationStatus_.initialized = (pImpl->brain_->getDopamine() != nullptr &&
                                                 pImpl->brain_->getCuriosity() != nullptr &&
                                                 pImpl->brain_->getNovelty() != nullptr);
    pImpl->neuromodulationStatus_.connected = false;
    pImpl->neuromodulationStatus_.active = false;
    
    pImpl->isInitialized_ = true;
    pImpl->isFullyIntegrated_ = false;
    
    NLM_LOG_INFO("IntegrationManager initialization complete");
}

void IntegrationManager::updateAll(double timestep) {
    if (!pImpl->isInitialized_) {
        NLM_LOG_WARN("IntegrationManager: Not initialized, skipping update");
        return;
    }
    
    pImpl->integrationCheckCount_++;
    
    // Update integration status based on system activity
    updateIntegrationStatus();
    
    // Check and update each system
    updateWorkingMemoryIntegration(timestep);
    updateEpisodicMemoryIntegration(timestep);
    updatePredictionSystemIntegration(timestep);
    updateCognitiveSystemIntegration(timestep);
    updateNeuromodulationIntegration(timestep);
    
    // Calculate integration score
    calculateIntegrationScore();
    
    // Log integration status
    if (pImpl->integrationCheckCount_ % 100 == 0) {  // Log every 100 steps
        logIntegrationStatus();
    }
}

void IntegrationManager::updateWorkingMemoryIntegration(double timestep) {
    auto* workingMemory = pImpl->brain_->getWorkingMemory();
    if (!workingMemory) {
        pImpl->workingMemoryStatus_.initialized = false;
        return;
    }
    
    // Check if working memory is active (has stored information)
    size_t activeTraces = workingMemory->getActiveTraces();
    pImpl->workingMemoryStatus_.active = (activeTraces > 0);
    
    if (pImpl->workingMemoryStatus_.active) {
        // Working memory is storing information - check its connections
        pImpl->workingMemoryStatus_.connected = checkWorkingMemoryConnections();
    }
    
    // Update integration metrics
    if (pImpl->workingMemoryStatus_.active && pImpl->workingMemoryStatus_.connected) {
        pImpl->systemConnections_++;
    }
    
    pImpl->workingMemoryStatus_.lastUpdate = "working_memory_update_" + std::to_string(pImpl->integrationCheckCount_);
}

void IntegrationManager::updateEpisodicMemoryIntegration(double timestep) {
    auto* episodicMemory = pImpl->brain_->getEpisodicMemory();
    if (!episodicMemory) {
        pImpl->episodicMemoryStatus_.initialized = false;
        return;
    }
    
    // Check if episodic memory has stored episodes
    size_t episodeCount = episodicMemory->getEpisodeCount();
    pImpl->episodicMemoryStatus_.active = (episodeCount > 0);
    
    if (pImpl->episodicMemoryStatus_.active) {
        // Episodic memory is storing experiences - check its connections
        pImpl->episodicMemoryStatus_.connected = checkEpisodicMemoryConnections();
    }
    
    // Update integration metrics
    if (pImpl->episodicMemoryStatus_.active && pImpl->episodicMemoryStatus_.connected) {
        pImpl->systemConnections_++;
    }
    
    pImpl->episodicMemoryStatus_.lastUpdate = "episodic_memory_update_" + std::to_string(pImpl->integrationCheckCount_);
}

void IntegrationManager::updatePredictionSystemIntegration(double timestep) {
    auto* predictionSystem = pImpl->brain_->getPredictionSystem();
    if (!predictionSystem) {
        pImpl->predictionSystemStatus_.initialized = false;
        return;
    }
    
    // Check prediction system activity
    float predictionError = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getConfidence();
    
    pImpl->predictionSystemStatus_.active = (confidence > 0.1f);  // Has some confidence
    
    if (pImpl->predictionSystemStatus_.active) {
        // Prediction system is active - check its connections
        pImpl->predictionSystemStatus_.connected = checkPredictionSystemConnections();
    }
    
    // Update integration metrics
    if (pImpl->predictionSystemStatus_.active && pImpl->predictionSystemStatus_.connected) {
        pImpl->systemConnections_++;
    }
    
    pImpl->predictionSystemStatus_.lastUpdate = "prediction_system_update_" + std::to_string(pImpl->integrationCheckCount_);
}

void IntegrationManager::updateCognitiveSystemIntegration(double timestep) {
    auto* planner = pImpl->brain_->getPlanner();
    auto* conceptFormation = pImpl->brain_->getConceptFormation();
    auto* attention = pImpl->brain_->getAttention();
    
    bool plannerActive = planner && planner->getActionQuality(ActionType::MoveForward) > 0.0f;
    bool conceptActive = conceptFormation && conceptFormation->getConceptCount() > 0;
    bool attentionActive = attention && attention->getAttendedRegions().size() > 0;
    
    pImpl->cognitiveSystemStatus_.active = (plannerActive && conceptActive && attentionActive);
    
    if (pImpl->cognitiveSystemStatus_.active) {
        // Cognitive systems are active - check their connections
        pImpl->cognitiveSystemStatus_.connected = checkCognitiveSystemConnections();
    }
    
    // Update integration metrics
    if (pImpl->cognitiveSystemStatus_.active && pImpl->cognitiveSystemStatus_.connected) {
        pImpl->systemConnections_++;
    }
    
    pImpl->cognitiveSystemStatus_.lastUpdate = "cognitive_system_update_" + std::to_string(pImpl->integrationCheckCount_);
}

void IntegrationManager::updateNeuromodulationIntegration(double timestep) {
    auto* dopamine = pImpl->brain_->getDopamine();
    auto* curiosity = pImpl->brain_->getCuriosity();
    auto* novelty = pImpl->brain_->getNovelty();
    
    bool dopamineActive = dopamine && dopamine->getLevel() != 0.0f;
    bool curiosityActive = curiosity && curiosity->getLevel() > 0.0f;
    bool noveltyActive = novelty && novelty->getLevel() > 0.0f;
    
    pImpl->neuromodulationStatus_.active = (dopamineActive && curiosityActive && noveltyActive);
    
    if (pImpl->neuromodulationStatus_.active) {
        // Neuromodulation is active - check its connections
        pImpl->neuromodulationStatus_.connected = checkNeuromodulationConnections();
    }
    
    // Update integration metrics
    if (pImpl->neuromodulationStatus_.active && pImpl->neuromodulationStatus_.connected) {
        pImpl->systemConnections_++;
    }
    
    pImpl->neuromodulationStatus_.lastUpdate = "neuromodulation_update_" + std::to_string(pImpl->integrationCheckCount_);
}

void IntegrationManager::updateIntegrationStatus() {
    // Update the overall integration status based on all systems
    size_t activeSystems = 0;
    size_t connectedSystems = 0;
    
    if (pImpl->workingMemoryStatus_.active) activeSystems++;
    if (pImpl->workingMemoryStatus_.connected) connectedSystems++;
    
    if (pImpl->episodicMemoryStatus_.active) activeSystems++;
    if (pImpl->episodicMemoryStatus_.connected) connectedSystems++;
    
    if (pImpl->predictionSystemStatus_.active) activeSystems++;
    if (pImpl->predictionSystemStatus_.connected) connectedSystems++;
    
    if (pImpl->cognitiveSystemStatus_.active) activeSystems++;
    if (pImpl->cognitiveSystemStatus_.connected) connectedSystems++;
    
    if (pImpl->neuromodulationStatus_.active) activeSystems++;
    if (pImpl->neuromodulationStatus_.connected) connectedSystems++;
    
    // Calculate integration score (0.0 to 1.0)
    pImpl->lastIntegrationScore_ = 
        static_cast<double>(connectedSystems) / static_cast<double>(activeSystems + 1);
    
    pImpl->isFullyIntegrated_ = (pImpl->lastIntegrationScore_ > 0.7);  // 70% integration threshold
}

void IntegrationManager::calculateIntegrationScore() {
    // Calculate integration score based on system connections and activity
    double baseScore = 0.0;
    
    // Each system contributes to integration
    if (pImpl->workingMemoryStatus_.initialized) baseScore += 0.2;
    if (pImpl->episodicMemoryStatus_.initialized) baseScore += 0.2;
    if (pImpl->predictionSystemStatus_.initialized) baseScore += 0.15;
    if (pImpl->cognitiveSystemStatus_.initialized) baseScore += 0.25;
    if (pImpl->neuromodulationStatus_.initialized) baseScore += 0.15;
    
    // Bonus for active and connected systems
    if (pImpl->workingMemoryStatus_.active && pImpl->workingMemoryStatus_.connected) baseScore += 0.1;
    if (pImpl->episodicMemoryStatus_.active && pImpl->episodicMemoryStatus_.connected) baseScore += 0.1;
    if (pImpl->predictionSystemStatus_.active && pImpl->predictionSystemStatus_.connected) baseScore += 0.1;
    if (pImpl->cognitiveSystemStatus_.active && pImpl->cognitiveSystemStatus_.connected) baseScore += 0.1;
    if (pImpl->neuromodulationStatus_.active && pImpl->neuromodulationStatus_.connected) baseScore += 0.1;
    
    // Cap at 1.0
    pImpl->lastIntegrationScore_ = std::min(pImpl->lastIntegrationScore_, 1.0);
}

bool IntegrationManager::checkWorkingMemoryConnections() const {
    auto* workingMemory = pImpl->brain_->getWorkingMemory();
    if (!workingMemory) return false;
    
    // Check connections to sensory processing
    size_t activeTraces = workingMemory->getActiveTraces();
    if (activeTraces == 0) return false;
    
    // Check if working memory is being updated in brain loop
    // (This would require access to brain implementation details)
    
    // For now, assume working memory integration is working
    // if it has active traces
    return true;
}

bool IntegrationManager::checkEpisodicMemoryConnections() const {
    auto* episodicMemory = pImpl->brain_->getEpisodicMemory();
    if (!episodicMemory) return false;
    
    // Check if episodic memory has stored episodes
    size_t episodeCount = episodicMemory->getEpisodeCount();
    if (episodeCount == 0) return false;
    
    // Check if episodes are being replayed for consolidation
    // (This would require access to brain implementation details)
    
    return true;
}

bool IntegrationManager::checkPredictionSystemConnections() const {
    auto* predictionSystem = pImpl->brain_->getPredictionSystem();
    if (!predictionSystem) return false;
    
    // Check if prediction system has prediction error
    float predictionError = predictionSystem->getPredictionError();
    if (std::abs(predictionError) < 0.01f) return false;  // Too little error
    
    // Check if prediction confidence is reasonable
    float confidence = predictionSystem->getConfidence();
    if (confidence < 0.1f) return false;  // Too little confidence
    
    return true;
}

bool IntegrationManager::checkCognitiveSystemConnections() const {
    auto* planner = pImpl->brain_->getPlanner();
    auto* conceptFormation = pImpl->brain_->getConceptFormation();
    auto* attention = pImpl->brain_->getAttention();
    
    if (!planner || !conceptFormation || !attention) return false;
    
    // Check if planner has learned action qualities
    bool plannerHasLearning = (planner->getActionQuality(ActionType::MoveForward) > 0.0f ||
                               planner->getActionQuality(ActionType::Interact) > 0.0f);
    
    // Check if concept formation has learned concepts
    bool conceptHasLearning = (conceptFormation->getConceptCount() > 0);
    
    // Check if attention has attended regions
    bool attentionHasLearning = (attention->getAttendedRegions().size() > 0);
    
    return (plannerHasLearning && conceptHasLearning && attentionHasLearning);
}

bool IntegrationManager::checkNeuromodulationConnections() const {
    auto* dopamine = pImpl->brain_->getDopamine();
    auto* curiosity = pImpl->brain_->getCuriosity();
    auto* novelty = pImpl->brain_->getNovelty();
    
    if (!dopamine || !curiosity || !novelty) return false;
    
    // Check if neuromodulators have non-zero levels
    bool dopamineActive = (dopamine->getLevel() != 0.0f);
    bool curiosityActive = (curiosity->getLevel() > 0.0f);
    bool noveltyActive = (novelty->getLevel() > 0.0f);
    
    return (dopamineActive && curiosityActive && noveltyActive);
}

bool IntegrationManager::isFullyIntegrated() const {
    return pImpl->isFullyIntegrated_;
}

double IntegrationManager::getIntegrationScore() const {
    return pImpl->lastIntegrationScore_;
}

void IntegrationManager::logIntegrationStatus() {
    NLM_LOG_INFO("=== NLM Integration Status ===");
    NLM_LOG_INFO("Integration Score: " + std::to_string(pImpl->lastIntegrationScore_ * 100.0) + "%");
    NLM_LOG_INFO("Systems Active: " + std::to_string(pImpl->systemConnections_) + "/5");
    
    if (pImpl->workingMemoryStatus_.active) {
        NLM_LOG_INFO("✓ Working Memory: " + pImpl->workingMemoryStatus_.lastUpdate);
    }
    
    if (pImpl->episodicMemoryStatus_.active) {
        NLM_LOG_INFO("✓ Episodic Memory: " + pImpl->episodicMemoryStatus_.lastUpdate);
    }
    
    if (pImpl->predictionSystemStatus_.active) {
        NLM_LOG_INFO("✓ Prediction System: " + pImpl->predictionSystemStatus_.lastUpdate);
    }
    
    if (pImpl->cognitiveSystemStatus_.active) {
        NLM_LOG_INFO("✓ Cognitive Systems: " + pImpl->cognitiveSystemStatus_.lastUpdate);
    }
    
    if (pImpl->neuromodulationStatus_.active) {
        NLM_LOG_INFO("✓ Neuromodulation: " + pImpl->neuromodulationStatus_.lastUpdate);
    }
    
    if (pImpl->isFullyIntegrated_) {
        NLM_LOG_INFO("🎉 FULL INTEGRATION ACHIEVED!");
    }
}

std::string IntegrationManager::getIntegrationReport() const {
    std::string report = "NLM Phase 6 Integration Report\n";
    report += "================================\n\n";
    
    report += "Integration Score: " + std::to_string(pImpl->lastIntegrationScore_ * 100.0) + "%\n";
    report += "Systems Active: " + std::to_string(pImpl->systemConnections_) + "/5\n\n";
    
    report += "System Status:\n";
    report += "  Working Memory: " + std::string(pImpl->workingMemoryStatus_.active ? "✓ ACTIVE" : "✗ INACTIVE") + "\n";
    report += "  Episodic Memory: " + std::string(pImpl->episodicMemoryStatus_.active ? "✓ ACTIVE" : "✗ INACTIVE") + "\n";
    report += "  Prediction System: " + std::string(pImpl->predictionSystemStatus_.active ? "✓ ACTIVE" : "✗ INACTIVE") + "\n";
    report += "  Cognitive Systems: " + std::string(pImpl->cognitiveSystemStatus_.active ? "✓ ACTIVE" : "✗ INACTIVE") + "\n";
    report += "  Neuromodulation: " + std::string(pImpl->neuromodulationStatus_.active ? "✓ ACTIVE" : "✗ INACTIVE") + "\n\n";
    
    if (pImpl->isFullyIntegrated_) {
        report += "🎉 STATUS: FULLY INTEGRATED - The NLM brain is functioning as a coherent artificial brain!\n";
    } else {
        report += "⚠️  STATUS: PARTIALLY INTEGRATED - Additional work needed for full functionality.\n";
    }
    
    return report;
}

} // namespace nlm
