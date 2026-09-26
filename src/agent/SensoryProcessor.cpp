#include "SensoryProcessor.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

SensoryProcessor::SensoryProcessor(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
    : brain_(std::move(brain))
    , world_(std::move(world))
    , visionNeuronCount_(0)
    , touchNeuronCount_(0)
    , internalNeuronCount_(0)
    , proprioceptionNeuronCount_(0)
    , noveltyLevel_(0.0f)
    , noveltyDecayRate_(0.99f)
    , visionInputSize_(256)
{
    if (!brain_ || !world_) {
        throw std::invalid_argument("SensoryProcessor requires valid brain and world pointers");
    }
}

void SensoryProcessor::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_ || !world_) return;
    
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // We need access to the sensory neuron groups through AgentBrain,
    // so this component should be given the neurons directly or through a different interface
    // For now, we'll provide the signals and let the parent handle neuron access
    
    // Store current vision for novelty computation
    if (!vision.empty()) {
        visionBuffer_ = vision;
    }
    
    // Update novelty
    updateNovelty(vision, world_->getWidth(), world_->getHeight());
}

void SensoryProcessor::updateNovelty(const std::vector<float>& vision, float worldWidth, float worldHeight) {
    if (!worldWidth || !worldHeight || vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    noveltyLevel_ = computeNovelty(vision);
    noveltyLevel_ *= noveltyDecayRate_;
}

void SensoryProcessor::initialize(const SimpleWorld& world) {
    configureVision(world.getVisionWidth(), world.getVisionHeight());
    previousVision_.resize(visionInputSize_, 0.0f);
}

void SensoryProcessor::reset() {
    noveltyLevel_ = 0.0f;
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

std::tuple<size_t, size_t, size_t, size_t> SensoryProcessor::getSensoryNeuronCounts() const {
    return std::make_tuple(visionNeuronCount_, touchNeuronCount_, 
                          internalNeuronCount_, proprioceptionNeuronCount_);
}

float SensoryProcessor::computeNovelty(const std::vector<float>& currentVision) {
    if (currentVision.empty() || visionInputSize_ == 0) {
        return 0.0f;
    }
    
    size_t samples = std::min(currentVision.size(), previousVision_.size());
    if (samples == 0) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < samples; ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize by number of samples
    float novelty = totalDiff / static_cast<float>(samples);
    
    // Store for next comparison
    previousVision_ = currentVision;
    
    return novelty;
}

void SensoryProcessor::clampSignal(std::vector<float>& signal, float min, float max) {
    for (float& value : signal) {
        value = std::clamp(value, min, max);
    }
}

} // namespace nlm
