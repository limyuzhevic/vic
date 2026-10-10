#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

// Neuromodulator implementation
Neuromodulator::Neuromodulator(const NeuromodulatorConfig& config)
    : config_(config), brain_(nullptr), plasticity_(nullptr), network_(nullptr), dynamics_(nullptr) {
    // Initialize with default state
    state_.currentLevel = config.baselineLevel;
    state_.targetLevel = config.baselineLevel;
    state_.homeostasisValue = config.homeostaticTarget;
    
    NLM_LOG_INFO("Neuromodulator created");
}

Neuromodulator::~Neuromodulator() {
    NLM_LOG_INFO("Neuromodulator destroyed");
}

Neuromodulator::Neuromodulator(Neuromodulator&& other) noexcept
    : config_(std::move(other.config_)), state_(std::move(other.state_)),
      brain_(std::exchange(other.brain_, nullptr)),
      plasticity_(std::exchange(other.plasticity_, nullptr)),
      network_(std::exchange(other.network_, nullptr)),
      dynamics_(std::exchange(other.dynamics_, nullptr)) {
}

Neuromodulator& Neuromodulator::operator=(Neuromodulator&& other) noexcept {
    if (this != &other) {
        shutdown();
        config_ = std::move(other.config_);
        state_ = std::move(other.state_);
        brain_ = std::exchange(other.brain_, nullptr);
        plasticity_ = std::exchange(other.plasticity_, nullptr);
        network_ = std::exchange(other.network_, nullptr);
        dynamics_ = std::exchange(other.dynamics_, nullptr);
    }
    return *this;
}

void Neuromodulator::setConfig(const NeuromodulatorConfig& config) {
    config_ = config;
    
    // Update state based on new config
    state_.targetLevel = config.homeostaticTarget;
    state_.homeostasisValue = config.homeostaticTarget;
    
    NLM_LOG_DEBUG("Neuromodulator config updated");
}

void Neuromodulator::updateState(const NeuromodulatorState& state) {
    state_ = state;
}

void Neuromodulator::updateLevel(TimestepDuration dt) {
    // Update level based on dynamics
    float current = state_.currentLevel;
    float target = state_.targetLevel;
    float production = state_.productionRate;
    float decay = state_.decayRate;
    float inhibition = state_.inhibitionRate;
    float adaptation = state_.adaptationSpeed;
    
    // Production from external input
    float levelChange = production * (1.0f - current / state_.saturation);
    
    // Decay
    levelChange -= decay * current;
    
    // Inhibition
    levelChange -= inhibition * current;
    
    // Homeostatic regulation
    float homeostasisError = state_.homeostasisValue - current;
    levelChange += adaptation * homeostasisError;
    
    // External and internal inputs
    levelChange += (state_.externalInput + state_.internalInput) * dt;
    
    // Apply noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noiseDist(0.0f, state_.noiseLevel);
    levelChange += noiseDist(gen);
    
    // Euler integration
    state_.currentLevel += levelChange * dt;
    
    // Apply bounds
    state_.currentLevel = std::clamp(state_.currentLevel, config_.minLevel, config_.maxLevel);
    
    // Update temporal derivative
    state_.temporalDerivative = computeTemporalDerivative(state_.currentLevel, state_.targetLevel, dt);
}

void Neuromodulator::applyHomeostasis() {
    // Apply homeostatic regulation
    float error = state_.homeostasisValue - state_.currentLevel;
    float correction = -error * config_.adaptationSpeed;
    
    state_.currentLevel += correction;
    state_.currentLevel = std::clamp(state_.currentLevel, config_.minLevel, config_.maxLevel);
}

void Neuromodulator::adaptToChanges() {
    // Adapt to changes in environment or internal state
    float adaptation = 0.0f;
    
    // Adapt based on prediction error
    if (std::abs(state_.predictionError) > 0.1f) {
        adaptation += state_.predictionError * config_.adaptationSpeed * 0.1f;
    }
    
    // Adapt based on reward prediction error
    if (std::abs(state_.rewardPrediction - state_.actualReward) > 0.1f) {
        adaptation += (state_.actualReward - state_.rewardPrediction) * config_.adaptationSpeed * 0.1f;
    }
    
    // Update target level
    state_.targetLevel += adaptation;
    state_.targetLevel = std::clamp(state_.targetLevel, config_.minLevel, config_.maxLevel);
}

void Neuromodulator::applyNoise() {
    if (!config_.enableNoise) return;
    
    // Add random noise to current level
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noiseDist(0.0f, config_.noiseLevel);
    
    float noise = noiseDist(gen);
    state_.currentLevel += noise;
    
    // Apply bounds
    state_.currentLevel = std::clamp(state_.currentLevel, config_.minLevel, config_.maxLevel);
}

void Neuromodulator::enforceBounds() {
    // Enforce level bounds
    state_.currentLevel = std::clamp(state_.currentLevel, config_.minLevel, config_.maxLevel);
    state_.targetLevel = std::clamp(state_.targetLevel, config_.minLevel, config_.maxLevel);
}

float Neuromodulator::computeTemporalDerivative(float current, float previous, TimestepDuration dt) const {
    if (dt > 0.0f) {
        return (current - previous) / dt;
    }
    return 0.0f;
}

float Neuromodulator::computeSpectralDensity(const std::vector<float>& signal) const {
    if (signal.empty()) return 0.0f;
    
    // Simplified spectral density computation
    float mean = std::accumulate(signal.begin(), signal.end(), 0.0f) / signal.size();
    float variance = 0.0f;
    
    for (float value : signal) {
        variance += (value - mean) * (value - mean);
    }
    
    variance /= signal.size();
    
    return variance;
}

float Neuromodulator::computeCoherence(const std::vector<float>& signal1, const std::vector<float>& signal2) const {
    if (signal1.size() != signal2.size() || signal1.empty()) return 0.0f;
    
    // Compute coherence (simplified)
    float mean1 = std::accumulate(signal1.begin(), signal1.end(), 0.0f) / signal1.size();
    float mean2 = std::accumulate(signal2.begin(), signal2.end(), 0.0f) / signal2.size();
    
    float numerator = 0.0f;
    float denom1 = 0.0f;
    float denom2 = 0.0f;
    
    for (size_t i = 0; i < signal1.size(); ++i) {
        numerator += (signal1[i] - mean1) * (signal2[i] - mean2);
        denom1 += (signal1[i] - mean1) * (signal1[i] - mean1);
        denom2 += (signal2[i] - mean2) * (signal2[i] - mean2);
    }
    
    float coherence = numerator / std::sqrt(denom1 * denom2);
    
    return std::abs(coherence);
}

} // namespace nlm
