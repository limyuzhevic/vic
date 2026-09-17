#include "CognitiveDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct CognitiveDevelopment::Impl {
    double lastUpdate;
    float developmentRate;
    
    Impl() : lastUpdate(0.0), developmentRate(0.01f) {}
};

CognitiveDevelopment::CognitiveDevelopment() : pImpl(new Impl) {}

CognitiveDevelopment::~CognitiveDevelopment() = default;

void CognitiveDevelopment::update(Brain* brain, double age, double dt) {
    double ageFraction = std::min(1.0, age / 960.0);  // Normalize to developmental timeline
    
    developWorkingMemory(brain, age);
    developPlanning(brain, age);
    developAbstractThinking(brain, age);
    developCausalReasoning(brain, age);
}

void CognitiveDevelopment::developWorkingMemory(Brain* brain, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Working memory capacity develops sigmoidally
    capabilities.workingMemoryCapacity = 1.0f + 9.0f * static_cast<float>(ageFraction / (1.0 + std::exp(-5.0 * (ageFraction - 0.5))));
    
    if (brain && brain->getWorkingMemory()) {
        brain->getWorkingMemory()->setCapacity(static_cast<size_t>(capabilities.workingMemoryCapacity * 10));
    }
}

void CognitiveDevelopment::developPlanning(Brain* brain, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Planning depth develops with age
    capabilities.planningDepth = 1.0f + 4.0f * static_cast<float>(ageFraction);
    capabilities.temporalAbstraction = 1.0f + 2.0f * static_cast<float>(ageFraction);
    
    if (brain && brain->getPlanner()) {
        brain->getPlanner()->setPlanningDepth(static_cast<int>(capabilities.planningDepth));
    }
}

void CognitiveDevelopment::developAbstractThinking(Brain* brain, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Abstract thinking develops later
    capabilities.abstractThinking = std::min(1.0f, static_cast<float>(ageFraction - 0.3) * 2.0f);
    capabilities.creativity = std::max(0.0f, std::min(1.0f, static_cast<float>(ageFraction - 0.4) * 1.5f));
}

void CognitiveDevelopment::developCausalReasoning(Brain* brain, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Causal reasoning develops progressively
    capabilities.causalReasoning = std::min(1.0f, static_cast<float>(ageFraction - 0.2) * 1.5f);
}

CognitiveCapabilities CognitiveDevelopment::getCapabilities() const {
    return capabilities;
}

bool CognitiveDevelopment::isCapabilityDeveloped(float current, float threshold) const {
    return current >= threshold;
}

} // namespace nlm