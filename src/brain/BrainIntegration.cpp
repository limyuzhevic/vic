// Brain integration implementation - handles the main coordination loop and system integration
#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

BrainIntegration::BrainIntegration(Brain& brainRef) : brain(brainRef) {
    integration = std::make_unique<Integration>();
}

BrainIntegration::~BrainIntegration() = default;

bool BrainIntegration::initialize() {
    return integration ? true : false;
}

void BrainIntegration::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!integration) return;
    
    // Coordinate all systems
    coordinateSystems();
    
    // Apply cross-system influences
    applyCrossSystemInfluences();
    
    // Log system status
    integration->logSystemStatus("Brain");
}

void BrainIntegration::coordinateSystems() {
    if (!integration) return;
    
    // Check system health
    if (!integration->validateSystemHealth("Memory")) {
        integration->handleCrossSystemError("Memory System", std::runtime_error("Memory system health check failed"));
    }
    if (!integration->validateSystemHealth("Prediction")) {
        integration->handleCrossSystemError("Prediction System", std::runtime_error("Prediction system health check failed"));
    }
    if (!integration->validateSystemHealth("Cognition")) {
        integration->handleCrossSystemError("Cognition System", std::runtime_error("Cognition system health check failed"));
    }
    if (!integration->validateSystemHealth("Neuromodulation")) {
        integration->handleCrossSystemError("Neuromodulation System", std::runtime_error("Neuromodulation system health check failed"));
    }
}

void BrainIntegration::applyCrossSystemInfluences() {
    // Apply neuromodulatory effects
    if (brain.getDopamine()) {
        brain.applyNeuromodulation(*brain.getDopamine());
    }
    
    // Apply developmental effects
    if (brain.getDevelopmentSystem()) {
        brain.develop();
    }
}

} // namespace nlm
