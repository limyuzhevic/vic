#include "Adenosine.hpp"

namespace nlm {

Adenosine::Adenosine() : pImpl(new Impl) {}

Adenosine::~Adenosine() { delete pImpl; }

const char* Adenosine::getName() const {
    return "Adenosine";
}

float Adenosine::getLevel() const {
    return pImpl->level;
}

void Adenosine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Adenosine::getPlasticityFactor() const {
    // Adenosine reduces plasticity during high levels (sleep)
    return std::max(0.1f, 1.0f - pImpl->level);
}

void Adenosine::update(TimestepDuration dt) {
    // Adenosine dynamics: accumulates during wake, clears during sleep
    float dtFloat = static_cast<float>(dt);
    
    // Base decay (clears during rest)
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Signal: wake signaling (consolidation still happens during rest)
    if (pImpl->level < pImpl->clearanceRate * dtFloat) {
        pImpl->level = 0.0f;
    }
}

void Adenosine::signalWakeUp() {
    // Wake up signal - reduces adenosine
    if (pImpl->level > pImpl->clearanceRate * 1.0f) {
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->clearanceRate);
    }
}

void Adenosine::signalRest(float energySaved) {
    // Rest signal - increases adenosine based on energy conservation
    if (energySaved > 0) {
        pImpl->level += pImpl->accumulationRate * energySaved;
        pImpl->level = std::min(pImpl->level, pImpl->peak);
    }
}

void Adenosine::signalMetabolicDemand(float demand) {
    // Metabolic demand during wake increases adenosine
    if (demand > 0) {
        pImpl->level += pImpl->buildUpRate * demand;
        pImpl->level = std::min(pImpl->level, pImpl->peak);
    }
}

bool Adenosine::shouldTriggerRest() const {
    return pImpl->level >= pImpl->fatigueThreshold;
}

bool Adenosine::shouldEndRest() const {
    return pImpl->level <= pImpl->recoveryThreshold;
}

float Adenosine::getFatigueLevel() const {
    return std::min(1.0f, pImpl->level / pImpl->fatigueThreshold);
}

} // namespace nlm