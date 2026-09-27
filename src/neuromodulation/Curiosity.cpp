#include "Curiosity.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Curiosity::Impl {
    // Brain reference for neural integration
    class Brain* brain;
    
    // Curiosity state
    float level;                    // Current curiosity level
    float baseline;                 // Baseline curiosity level
    float noveltyWeight;           // Weight of novelty in curiosity
    float predictionErrorWeight;   // Weight of prediction error in curiosity
    float decayRate;               // Natural decay rate
    float learningRate;            // Learning rate for curiosity adaptation
    
    // Exploration components
    float noveltyContribution;      // Current novelty contribution
    float predictionErrorContribution; // Current prediction error contribution
    float explorationBonus;        // Exploration bonus from curiosity
    
    // Neural dynamics
    float temporalSmooth;          // Temporal smoothing factor
    float rewardModulation;        // Reward modulation of curiosity
    float attentionModulation;     // Attention modulation of curiosity
    
    // Statistics and history
    std::vector<float> levelHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    // Exploration thresholds
    float explorationThreshold;    // Minimum curiosity for exploration
    float optimalCuriosity;        // Target curiosity level
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , noveltyWeight(0.6f)
        , predictionErrorWeight(0.4f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , noveltyContribution(0.0f)
        , predictionErrorContribution(0.0f)
        , explorationBonus(0.0f)
        , temporalSmooth(0.3f)
        , rewardModulation(1.0f)
        , attentionModulation(1.0f)
        , explorationThreshold(0.2f)
        , optimalCuriosity(0.5f) {
        
        // Seed RNG with chrono for biological variability
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.8f, 1.2f);
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with biological dynamics");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Real curiosity-driven behavior: biologically-inspired implementation
    
    // Update contributions from novelty and prediction error
    pImpl->noveltyContribution = novelty * pImpl->noveltyWeight;
    pImpl->predictionErrorContribution = std::abs(predictionError) * pImpl->predictionErrorWeight;
    
    // Apply reward and attention modulation
    float noveltyModulated = pImpl->noveltyContribution * pImpl->rewardModulation * pImpl->attentionModulation;
    float errorModulated = pImpl->predictionErrorContribution * pImpl->rewardModulation * pImpl->attentionModulation;
    
    // Calculate target curiosity level
    float target = noveltyModulated + errorModulated;
    target = std::max(0.0f, std::min(target, 1.0f)); // Clamp to [0,1]
    
    // Temporal smoothing (like neural adaptation)
    float smoothing = std::pow(1.0f - pImpl->temporalSmooth, static_cast<float>(dt));
    float smoothedTarget = target * (1.0f - smoothing) + pImpl->level * smoothing;
    
    // Apply learning with biological constraints
    float learningStep = (smoothedTarget - pImpl->level) * pImpl->learningRate;
    
    // Add exploration bonus (dopamine-modulated)
    pImpl->explorationBonus = learningStep * 0.5f;
    
    // Update level with bounds and biological variability
    pImpl->level += learningStep + (dist(pImpl->rng) - 1.0f) * 0.02f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, std::min(pImpl->optimalCuriosity, pImpl->level));
    
    // Natural decay
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update history
    pImpl->levelHistory.push_back(pImpl->level);
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    // Trim history
    const size_t maxHistory = 1000;
    if (pImpl->levelHistory.size() > maxHistory) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setBaselineCuriosity(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Curiosity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->noveltyContribution = 0.0f;
    pImpl->predictionErrorContribution = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->rewardModulation = 1.0f;
    pImpl->attentionModulation = 1.0f;
}

float Curiosity::getNoveltyContribution() const {
    return pImpl->noveltyContribution;
}

float Curiosity::getPredictionErrorContribution() const {
    return pImpl->predictionErrorContribution;
}

float Curiosity::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

} // namespace nlm
