//=============================================================
// SYSTEM ORCHESTRATOR - Brain.cpp Refactoring (Phase 7)
// Single Responsibility: System Coordination
//=============================================================

#include "SystemOrchestrator.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// SYSTEM ORCHESTRATOR IMPLEMENTATION
// Focuses on coordinating all systems and maintaining integration

SystemOrchestrator::SystemOrchestrator() : m_coreBrain(nullptr), m_brainManager(nullptr) {}

SystemOrchestrator::~SystemOrchestrator() = default;

void SystemOrchestrator::initialize(BrainCore* coreBrain, BrainManager* brainManager, std::shared_ptr<Config> config) {
    m_coreBrain = coreBrain;
    m_brainManager = brainManager;
    m_config = config;
    
    NLM_LOG_INFO("System Orchestrator initialized (Phase 7 - Refactored)");
}

void SystemOrchestrator::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    
    // Execute complete integration step through brain manager
    m_brainManager->executeStep(currentStep, currentTime);
}

void SystemOrchestrator::processSensoryInput(const class SensoryInput& input) {
    // Process sensory input through brain manager
    m_coreBrain->receiveSensoryInput(input);
}

void SystemOrchestrator::update(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    
    // Execute integration step
    executeStep(currentStep, currentTime);
}

void SystemOrchestrator::reset() {
    if (m_brainManager) {
        m_brainManager->reset();
    }
    
    NLM_LOG_INFO("System Orchestrator reset complete (Phase 7)");
}

void SystemOrchestrator::logStatus() const {
    NLM_LOG_INFO("=== NLM System Orchestrator Status (Phase 7 - Refactored) ===");
    
    if (m_coreBrain) {
        m_coreBrain->logStatus();
    }
    
    if (m_brainManager) {
        m_brainManager->logStatus();
    }
}

} // namespace nlm
