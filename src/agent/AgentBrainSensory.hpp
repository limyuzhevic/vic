#pragma once

#include "AgentBrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class AgentBrainSensory {
public:
    AgentBrainSensory(AgentBrainCore* parent) : parent_(parent) {}
    
    // Distribute sensory neurons
    void distributeSensoryNeurons(const Brain* brain);
    
    // Initialize sensory processing
    void initializeSensoryProcessing();
    
    // Process vision input
    void processVisionInput(const std::vector<float>& vision);
    
    // Process touch input
    void processTouchInput(const std::vector<float>& touch);
    
    // Process internal input
    void processInternalInput(const std::vector<float>& internal);
    
    // Process proprioception input
    void processProprioceptionInput(const std::vector<float>& proprio);
    
    // Compute novelty
    void computeNovelty(const std::vector<float>& vision);
    
    // Update curiosity
    void updateCuriosity(float predictionError);
    
    // Get novelty level
    float getNoveltyLevel() const { return parent_->noveltyLevel_; }
    
    // Get curiosity level
    float getCuriosityLevel() const { return parent_->curiosityLevel_; }
    
    // Get vision prediction
    const std::vector<float>& getPredictedVision() const { return parent_->predictedVision_; }
    
    // Get forward touch value
    float getForwardTouch() const { return parent_->forwardTouch_; }
    
    // Get forward object vision
    float getForwardObjectVision() const { return parent_->forwardObjectVision_; }
    
    // Get max touch value
    float getMaxTouch() const { return parent_->maxTouch_; }
    
    // Set vision prediction
    void setPredictedVision(const std::vector<float>& vision) { parent_->predictedVision_ = vision; }
    
    // Get last vision
    const std::vector<float>& getLastVision() const { return parent_->lastVision_; }
    
    // Set last vision
    void setLastVision(const std::vector<float>& vision) { parent_->lastVision_ = vision; }
    
    // Get thought percept
    const std::string& getThoughtPercept() const { return parent_->thoughtPercept_; }
    
    // Set thought percept
    void setThoughtPercept(const std::string& percept) { parent_->thoughtPercept_ = percept; }
    
    // Check if thought outcome exists
    bool hasThoughtOutcome() const { return parent_->hasThoughtOutcome_; }
    
    // Set thought outcome
    void setThoughtOutcome(bool hasOutcome) { parent_->hasThoughtOutcome_ = hasOutcome; }
    
    // Get thought reward
    float getThoughtReward() const { return parent_->thoughtReward_; }
    
    // Set thought reward
    void setThoughtReward(float reward) { parent_->thoughtReward_ = reward; }
    
    // Get thought planned
    MotorCommand getThoughtPlanned() const { return parent_->thoughtPlanned_; }
    
    // Set thought planned
    void setThoughtPlanned(MotorCommand cmd) { parent_->thoughtPlanned_ = cmd; }
    
    // Check if thought planned
    bool hasThoughtPlanned() const { return parent_->hasThoughtPlanned_; }
    
    // Set thought planned
    void setHasThoughtPlanned(bool hasPlanned) { parent_->hasThoughtPlanned_ = hasPlanned; }
    
    // Get sensory novelty decay
    float getSensoryNoveltyDecay() const { return parent_->sensoryNoveltyDecay_; }
    
    // Set sensory novelty decay
    void setSensoryNoveltyDecay(float decay) { parent_->sensoryNoveltyDecay_ = decay; }
    
private:
    // Winner-take-all lateral inhibition
    void applyWinnerTakeAllInhibition(const std::vector<float>& vision);
    
    // Update prediction with new vision
    void updatePrediction(const std::vector<float>& vision);
    
    AgentBrainCore* parent_;
};

inline void AgentBrainSensory::distributeSensoryNeurons(const Brain* brain) {
    if (!brain) return;
    
    parent_->sensoryVision_.clear();
    parent_->sensoryTouch_.clear();
    parent_->sensoryInternal_.clear();
    parent_->sensoryProprioception_.clear();
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = parent_->sensoryVision_.size() + parent_->sensoryTouch_.size() +
                                parent_->sensoryInternal_.size() + parent_->sensoryProprioception_.size();
                    
                    switch (idx % 4) {
                        case 0: parent_->sensoryVision_.push_back(n); break;
                        case 1: parent_->sensoryTouch_.push_back(n); break;
                        case 2: parent_->sensoryInternal_.push_back(n); break;
                        case 3: parent_->sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
}

inline void AgentBrainSensory::initializeSensoryProcessing() {
    parent_->predictedVision_.clear();
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(parent_->sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(parent_->sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(parent_->sensoryInternal_.size()) + " internal sensory neurons");
}

inline void AgentBrainSensory::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < parent_->sensoryVision_.size() && i < vision.size(); ++i) {
        if (parent_->sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            parent_->sensoryVision_[i]->injectCurrent(current);
        }
    }
}

inline void AgentBrainSensory::processTouchInput(const std::vector<float>& touch) {
    float maxTouch = 0.0f;
    for (float t : touch) maxTouch = std::max(maxTouch, t);
    parent_->maxTouch_ = maxTouch;
}

inline void AgentBrainSensory::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < parent_->sensoryInternal_.size() && i < internal.size(); ++i) {
        if (parent_->sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
            parent_->sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

inline void AgentBrainSensory::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < parent_->sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (parent_->sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            parent_->sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

inline void AgentBrainSensory::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        parent_->noveltyLevel_ = 0.0f;
        return;
    }
    
    if (parent_->predictedVision_.size() != vision.size()) {
        parent_->predictedVision_.assign(vision.size(), 0.0f);
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size(); ++i) {
        totalDiff += std::abs(vision[i] - parent_->predictedVision_[i]);
    }
    
    parent_->noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    parent_->noveltyLevel_ *= parent_->sensoryNoveltyDecay_;
    
    constexpr float kPredictionAlpha = 0.05f;
    for (size_t i = 0; i < vision.size(); ++i) {
        parent_->predictedVision_[i] += kPredictionAlpha * (vision[i] - parent_->predictedVision_[i]);
    }
}

inline void AgentBrainSensory::updateCuriosity(float predictionError) {
    if (parent_->curiosityEnabled_) {
        parent_->curiosityLevel_ = parent_->noveltyLevel_ * 2.0f + std::abs(predictionError) * 0.5f +
                                0.6f * parent_->curiosityLevel_;
        parent_->curiosityLevel_ = std::clamp(parent_->curiosityLevel_, 0.0f, 1.0f);
    }
}

inline void AgentBrainSensory::applyWinnerTakeAllInhibition(const std::vector<float>& vision) {
    if (parent_->lastWinnerGroup_ >= 0 && parent_->lastWinnerGroup_ != 4 &&
        parent_->consecutiveWinnerCount_ < 3 &&
        parent_->lastWinnerMargin_ > 5.0f && parent_->noveltyLevel_ < 0.3f) {
        const std::vector<Neuron*>* groups[5] = {
            &parent_->motorForward_, &parent_->motorBackward_, &parent_->motorTurnLeft_,
            &parent_->motorTurnRight_, &parent_->motorInteract_};
        for (int g = 0; g < 5; ++g) {
            if (g == parent_->lastWinnerGroup_) continue;
            for (Neuron* n : *groups[g]) {
                if (n) n->injectCurrent(-4.0f);
            }
        }
        parent_->wtaPulseCount_++;
    }
}

inline void AgentBrainSensory::updatePrediction(const std::vector<float>& vision) {
    constexpr float kPredictionAlpha = 0.05f;
    for (size_t i = 0; i < vision.size(); ++i) {
        parent_->predictedVision_[i] += kPredictionAlpha * (vision[i] - parent_->predictedVision_[i]);
    }
}

} // namespace nlm