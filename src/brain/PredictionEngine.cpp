#include "PredictionEngine.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>

namespace nlm {

struct PredictionEngine::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Prediction error signal
    float predictionError = 0.0f;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        predictionError = 0.0f;
    }
};

PredictionEngine::PredictionEngine(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

PredictionEngine::~PredictionEngine() = default;

PredictionEngine::PredictionEngine(PredictionEngine&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

PredictionEngine& PredictionEngine::operator=(PredictionEngine&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool PredictionEngine::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("PredictionEngine: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    NLM_LOG_INFO("PredictionEngine initialized successfully");
    return true;
}

void PredictionEngine::update(const class SensoryInput& input, TimestepDuration dt) {
    if (!pImpl->predictionSystem) {
        return;
    }
    
    // Update prediction system with current input
    pImpl->predictionSystem->update(input, dt);
    
    // Compute prediction error
    pImpl->predictionError = pImpl->predictionSystem->getPredictionError();
}

float PredictionEngine::getPredictionError() const {
    return pImpl->predictionError;
}

void PredictionEngine::reset() {
    NLM_LOG_INFO("PredictionEngine reset");
    pImpl->predictionError = 0.0f;
}

} // namespace nlm