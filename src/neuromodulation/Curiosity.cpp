#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <map>
#include <unordered_map>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    double lastUpdateTime;
    float explorationThreshold;
    float intrinsicDrive;
    std::map<std::string, float> curiosityMaps;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , lastUpdateTime(0.0)
        , explorationThreshold(0.3f)
        , intrinsicDrive(0.1f) {
        curiosityMaps["novelty"] = 1.0f;
        curiosityMaps["prediction_error"] = 0.8f;
        curiosityMaps["reward_prediction"] = 0.6f;
        curiosityMaps["social"] = 0.4f;
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize with biologically plausible parameters
    pImpl->explorationThreshold = 0.3f + 0.1f * (std::rand() / (RAND_MAX + 1.0f));
    pImpl->intrinsicDrive = 0.1f + 0.05f * (std::rand() / (RAND_MAX + 1.0f));
    
    NLM_LOG_INFO("Curiosity system initialized with exploration threshold: " + 
                 std::to_string(pImpl->explorationThreshold) + 
                 " and intrinsic drive: " + std::to_string(pImpl->intrinsicDrive));
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    double currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Calculate weighted curiosity based on multiple factors
    float weightedNovelty = novelty * pImpl->curiosityMaps["novelty"];
    float weightedPredictionError = predictionError * pImpl->curiosityMaps["prediction_error"];
    
    // Add intrinsic drive to prevent zero curiosity
    float intrinsic = pImpl->intrinsicDrive * (0.5f + 0.5f * std::sin(currentTime * 0.1f)); // Oscillating intrinsic drive
    
    // Calculate target curiosity level
    float target = weightedNovelty + weightedPredictionError + intrinsic;
    
    // Smooth update with time-dependent rate
    float updateRate = 0.1f + 0.05f * std::sin(currentTime * 0.5f); // Oscillating learning rate
    pImpl->level += (target - pImpl->level) * updateRate * static_cast<float>(deltaTime);
    
    // Decay with homeostatic regulation
    float homeostaticDecay = 0.05f + 0.02f * pImpl->level; // Higher decay at high levels
    pImpl->level = std::max(0.0f, pImpl->level - homeostaticDecay * static_cast<float>(deltaTime));
    
    // Ensure level is within bounds
    pImpl->level = std::min(1.0f, pImpl->level);
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
    pImpl->curiosityMaps["novelty"] = pImpl->noveltyWeight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
    pImpl->curiosityMaps["prediction_error"] = pImpl->predictionErrorWeight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    
    // Reset maps to default
    pImpl->curiosityMaps["novelty"] = 1.0f;
    pImpl->curiosityMaps["prediction_error"] = 0.8f;
    pImpl->curiosityMaps["reward_prediction"] = 0.6f;
    pImpl->curiosityMaps["social"] = 0.4f;
}

} // namespace nlm
