#include "SelfModel.hpp"
#include <NLM_LOG_INFO>

namespace nlm {

struct SelfModel::Impl {
    SelfModel::Impl() : capabilityLevel_(0.0f) {}
    
    Brain* brain_;
    float capabilityLevel_;
    std::vector<std::vector<ActionEffect>> actionEffects_;
};

SelfModel::SelfModel() : pImpl(new Impl) {}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    // NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                              const std::vector<float>& beforeState,
                              const std::vector<float>& afterState) {
    // PLACEHOLDER: Simple recording for now
    auto& effects = pImpl->actionEffects_[static_cast<size_t>(action)];
    effects.beforeState = beforeState;
    effects.afterState = afterState;
    effects.confidence = 1.0f;
    effects.observationCount = 1;
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                   const std::vector<float>& currentState) {
    // PLACEHOLDER: For now, return current state (no actual prediction)
    return currentState;
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    return 0.5f;
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                          const std::vector<float>& afterState,
                                          ActionType action) const {
    return 1.0f;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    return ActionType::Idle;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                           const std::vector<float>& actual,
                           ActionType action) {
    // PLACEHOLDER: No learning for now
}

float SelfModel::getCapabilityLevel() const {
    return pImpl->capabilityLevel_;
}

float SelfModel::getBodyAwareness() const {
    return 0.5f;
}

void SelfModel::clear() {
    pImpl->actionEffects_.clear();
}

bool SelfModel::hasSelfModel() const {
    return !pImpl->actionEffects_.empty();
}

} // namespace nlm