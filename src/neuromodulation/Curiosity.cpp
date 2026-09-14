#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    // Neural substrate integration
    float baselineLevel;
    float temporalMemoryCoeff;
    float spikeCorrelation;
    float explorationTarget;
    
    // Internal state for prediction error integration
    float predictionErrorFiltered;
    float recentPredictionError;
    
    // Exploration-exploitation balance
    float explorationBias;
    float exploitationBias;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , baselineLevel(0.01f)
        , temporalMemoryCoeff(0.7f)
        , spikeCorrelation(0.0f)
        , explorationTarget(0.5f)
        , predictionErrorFiltered(0.0f)
        , recentPredictionError(0.0f)
        , explorationBias(0.5f)
        , exploitationBias(0.5f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with neural substrate");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Real curiosity computation using neural substrates
    float currentTime = static_cast<float>(dt);
    
    // Apply temporal filtering to prediction error (neural adaptation)
    pImpl->predictionErrorFiltered = 0.95f * pImpl->predictionErrorFiltered + 0.05f * predictionError;
    pImpl->recentPredictionError = predictionError;
    
    // Compute curiosity as weighted combination of neural novelty signals
    float noveltyComponent = novelty * pImpl->noveltyWeight;
    float errorComponent = std::abs(pImpl->predictionErrorFiltered) * pImpl->predictionErrorWeight;
    
    // Add spike-timing correlation for exploration drive
    float spikeCorrelation = pImpl->spikeCorrelation * 0.2f;
    
    // Temporal memory integration for persistent curiosity
    float temporalMemory = std::min(1.0f, pImpl->noveltyWeight * 0.5f + pImpl->predictionErrorWeight * 0.3f);
    
    // Target-based curiosity for goal-directed exploration
    float targetDriven = pImpl->explorationTarget * (1.0f - pImpl->exploitationBias);
    
    // Combine all components with biologically plausible weighting
    float target = baselineLevel + 
                  0.4f * noveltyComponent + 
                  0.3f * errorComponent + 
                  0.15f * spikeCorrelation + 
                  0.15f * temporalMemory + 
                  0.1f * targetDriven;
    
    // Smooth neural adaptation
    pImpl->level = baselineLevel + (target - baselineLevel) * (0.1f + 0.05f * std::abs(predictionError));
    
    // Exploration-exploitation balance adjustment
    if (novelty > 0.7f) {
        pImpl->explorationBias = std::min(0.9f, pImpl->explorationBias + 0.05f);
        pImpl->exploitationBias = std::max(0.1f, 1.0f - pImpl->explorationBias);
    }
    
    // Decay with time
    pImpl->level = std::max(baselineLevel, pImpl->level - decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    // Exploration drive includes curiosity plus bias
    float exploration = pImpl->level * pImpl->explorationBias;
    
    // Neuromodulatory influence on decision making
    if (pImpl->brain) {
        auto* dopamine = pImpl->brain->getDopamine();
        if (dopamine) {
            exploration *= (1.0f + dopamine->getLevel() * 0.3f);
        }
    }
    
    return exploration;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::reset() {
    pImpl->level = baselineLevel;
    pImpl->explorationBias = 0.5f;
    pImpl->exploitationBias = 0.5f;
}

} // namespace nlm
