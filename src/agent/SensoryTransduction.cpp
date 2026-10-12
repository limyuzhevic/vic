#include "SensoryTransduction.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

SensoryTransduction::SensoryTransduction(std::shared_ptr<Config> config)
    : config_(config)
    , noveltyLevel_(0.0f)
    , sensoryNoveltyDecay_(0.99f)
    , visionWidth_(16)
    , visionHeight_(16)
    , touchCount_(8)
    , internalSignalCount_(4)
    , proprioceptionCount_(6)
{
}

SensoryTransduction::~SensoryTransduction() = default;

void SensoryTransduction::initialize(std::shared_ptr<Brain> brain, const AgentBody& body) {
    brain_ = brain;
    
    // Update dimensions from config
    visionWidth_ = config_->getVisionWidth();
    visionHeight_ = config_->getVisionHeight();
    touchCount_ = config_->getTouchCount();
    internalSignalCount_ = config_->getInternalSignalCount();
    proprioceptionCount_ = config_->getProprioceptionCount();
    
    // Collect sensory neurons from brain (same logic as old AgentBrain)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("SensoryTransduction: Initialized with " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioceptive neurons");
}

size_t SensoryTransduction::getSensoryInputSize() const {
    // Vision (configurable) + touch + internal + proprioception
    return visionWidth_ * visionHeight_ + touchCount_ + internalSignalCount_ + proprioceptionCount_;
}

void SensoryTransduction::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (configurable size)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (configurable count)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (configurable count)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (configurable count)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
}

void SensoryTransduction::injectCurrentToNeurons(const std::vector<Neuron*>& neurons, const std::vector<float>& signals, float scaleFactor) {
    for (size_t i = 0; i < neurons.size() && i < signals.size(); ++i) {
        if (neurons[i]) {
            neurons[i]->injectCurrent(signals[i] * scaleFactor);
        }
    }
}

} // namespace nlm
