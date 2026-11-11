#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Phase 6 text encoder: a teacher word becomes a sparse sensory
// pattern, never a string the brain can match on. Lower-case, trim,
// FNV-1a hash, then splitmix to pick 4 distinct neurons out of 16
// dedicated text neurons. Different words get different (overlapping,
// as in real sensory codes) patterns; the same word always gets the
// same pattern, which is what makes paired learning possible at all.
// The pattern is injected on sensoryVision_ neurons 16..30 — the
// tail vision neurons that vision itself never uses (vision is 16
// rays, the vision group has 31 neurons), so text has its own
// channel and cannot masquerade as a visual scene.
std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        // splitmix64
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    rates.reserve(internalNeurons_.size());
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    return rates;
}

std::vector<float> AgentBrain::conceptPattern() const {
    std::vector<float> pat;
    for (float v : lastVision_) pat.push_back(1.0f - v);
    for (float t : lastTouch_) pat.push_back(t);
    return pat;
}

// Phase 6 thought/planning counters: how often the planner, a
// learned concept, the self-model, or a social observation
// actually changed the decoded action (not just advised it).
int AgentBrain::getPlannerOverrideCount() const {
    return plannerOverrideCount_;
}

int AgentBrain::getPlannerAgreementCount() const {
    return plannerAgreementCount_;
}

int AgentBrain::getPlannerDecisionCount() const {
    return plannerDecisionCount_;
}

int AgentBrain::getConceptBiasCount() const {
    return conceptBiasCount_;
}

int AgentBrain::getSelfBiasCount() const {
    return selfBiasCount_;
}

int AgentBrain::getSocialBiasCount() const {
    return socialBiasCount_;
}

// Phase 6 wired systems, for demos/tests: concept count and the
// self-model's mean confidence, plus social observation. A second
// agent (teacher) is observed through observeOtherAgent; in a
// single-agent world this stays at zero and social bias stays off.
size_t AgentBrain::getConceptCount() const {
    return brain_ && brain_->getConceptFormation() ? brain_->getConceptFormation()->getConceptCount() : 0;
}

float AgentBrain::getSelfModelConfidence() const {
    if (!selfModelReady_) return 0.0f;
    float sum = 0.0f;
    int n = 0;
    for (int a = 0; a < 6; ++a) {
        float c = selfModel_.getSelfModelConfidence(static_cast<ActionType>(a));
        if (c > 0.0f) { sum += c; ++n; }
    }
    return n ? sum / n : 0.0f;
}

size_t AgentBrain::getSocialObservationCount() const {
    return socialReady_ ? socialLearning_.getObservationCount() : 0;
}

void AgentBrain::observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                                   const std::vector<float>& afterVision) {
    if (!socialReady_ && brain_) { socialLearning_.initialize(brain_.get()); socialReady_ = true; }
    ActionType act = ActionType::Wait;
    switch (observed) {
        case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
        case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
        case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
        case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
        case MotorCommand::Interact: act = ActionType::Interact; break;
        default: break;
    }
    socialLearning_.observeAgentAction(act, beforeVision, afterVision);
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Start with no prediction (all zeros), so the first scene is
    // novel — matching the old step-0 novelty spike — and the
    // prediction is then built from actual percepts.
    predictedVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

} // namespace nlm