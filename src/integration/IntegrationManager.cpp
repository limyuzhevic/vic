#include "IntegrationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

IntegrationManager::IntegrationManager()
    : integrationEnabled_(false)
    , lastIntegrationCheck_(0.0)
{
}

IntegrationManager::~IntegrationManager() {
}

bool IntegrationManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("IntegrationManager: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    integrationEnabled_ = true;
    
    // Initialize integration state
    integrationState_.systemsIntegrated = 0;
    integrationState_.totalSystems = 6;  // Working, Episodic, Associative, Prediction, Cognition, Neuromodulation
    integrationState_.integrationScore = 0.0f;
    integrationState_.lastActivityTime = 0.0;
    
    // Create and initialize subsystems
    updateIntegration();
    
    NLM_LOG_INFO("IntegrationManager: Initialized successfully");
    return true;
}

void IntegrationManager::updateIntegration() {
    if (!integrationEnabled_ || !brain_) {
        return;
    }
    
    double currentTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Count integrated systems
    size_t integratedCount = 0;
    
    // Check each system
    if (brain_->getWorkingMemory() != nullptr) integratedCount++;
    if (brain_->getEpisodicMemory() != nullptr) integratedCount++;
    if (brain_->getAssociativeMemory() != nullptr) integratedCount++;
    if (brain_->getPredictionSystem() != nullptr) integratedCount++;
    if (brain_->getPlanner() != nullptr) integratedCount++;
    if (brain_->getDopamine() != nullptr) integratedCount++;
    if (brain_->getCuriosity() != nullptr) integratedCount++;
    if (brain_->getNovelty() != nullptr) integratedCount++;
    if (brain_->getDevelopmentSystem() != nullptr) integratedCount++;
    
    integrationState_.systemsIntegrated = integratedCount;
    integrationState_.integrationScore = static_cast<float>(integratedCount) / integrationState_.totalSystems;
    integrationState_.lastActivityTime = currentTime;
    
    // Log integration status
    NLM_LOG_INFO("IntegrationManager: Systems integrated: " + std::to_string(integratedCount) + 
                 "/" + std::to_string(integrationState_.totalSystems) + 
                 " (score: " + std::to_string(integrationState_.integrationScore * 100.0f) + "%)");
}

void IntegrationManager::update() {
    if (!integrationEnabled_ || !brain_) {
        return;
    }
    
    double currentTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Update integration state
    updateIntegration();
    
    // Check for integration issues
    if (integrationState_.integrationScore < 0.8f) {  // Less than 80% integration
        NLM_LOG_WARNING("IntegrationManager: Low integration score (" + 
                        std::to_string(integrationState_.integrationScore * 100.0f) + "%)");
        handleIntegrationIssues();
    }
}

void IntegrationManager::handleIntegrationIssues() {
    if (!brain_) return;
    
    // Check for specific missing systems and log warnings
    if (brain_->getWorkingMemory() == nullptr) {
        NLM_LOG_WARNING("IntegrationManager: Working memory not integrated");
    }
    
    if (brain_->getEpisodicMemory() == nullptr) {
        NLM_LOG_WARNING("IntegrationManager: Episodic memory not integrated");
    }
    
    if (brain_->getPredictionSystem() == nullptr) {
        NLM_LOG_WARNING("IntegrationManager: Prediction system not integrated");
    }
    
    if (brain_->getDevelopmentSystem() == nullptr) {
        NLM_LOG_WARNING("IntegrationManager: Development system not integrated");
    }
}

void IntegrationManager::enableIntegration(bool enable) {
    integrationEnabled_ = enable;
    
    if (enable) {
        NLM_LOG_INFO("IntegrationManager: Integration enabled");
    } else {
        NLM_LOG_INFO("IntegrationManager: Integration disabled");
    }
}

bool IntegrationManager::isIntegrationEnabled() const {
    return integrationEnabled_;
}

const IntegrationState& IntegrationManager::getIntegrationState() const {
    return integrationState_;
}

bool IntegrationManager::reconnectSystem(const std::string& systemName) {
    if (!brain_) return false;
    
    NLM_LOG_INFO("IntegrationManager: Attempting to reconnect system: " + systemName);
    
    // Reinitialize the brain (this will reinitialize all systems)
    // Note: In a real implementation, you would have more granular reconnection logic
    // For now, we'll just mark the integration state as needing update
    integrationState_.lastActivityTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    return true;
}

} // namespace nlm
