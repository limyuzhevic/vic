#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <chrono>

namespace nlm {

// Optimized Neuromodulator with exponential smoothing and efficient updates
class OptimizedNeuromodulator : public Neuromodulator {
protected:
    struct Implementation {
        // Level with exponential smoothing for smooth transitions
        float level;
        float target;
        float previousLevel;
        float decayRate;
        float releaseRate;
        float smoothingFactor;  // For exponential smoothing
        SimulationStep lastUpdate;
        
        // History for adaptive behavior
        std::vector<float> recentLevels;
        std::atomic<size_t> historyIndex;
        
        Implementation() : level(0.0f), target(0.0f), previousLevel(0.0f),
                          decayRate(0.1f), releaseRate(1.0f), smoothingFactor(0.95f),
                          lastUpdate(0), historyIndex(0) {
            recentLevels.reserve(100);
        }
    };
    
    std::unique_ptr<Implementation> pImpl;
    
    // Optimized plasticity calculation with neuromodulator dynamics
    float calculatePlasticityOptimized(float basePlasticity) const;
    
    // Efficient target computation with pre-computation
    void updateTargetOptimized(float newLevel, float strength);
    
    // Smooth transition with exponential smoothing
    void smoothTransitionOptimized(float newTarget);
    
    // Adaptive response based on history
    float getAdaptiveResponse() const;
    
public:
    OptimizedNeuromodulator();
    ~OptimizedNeuromodulator() override;
    
    const char* getName() const override;
    
    float getLevel() const override;
    void setLevel(float level) override;
    
    float getPlasticityFactor() const override;
    
    void update(TimestepDuration dt) override;
    
    void signalReward(float reward) override;
    void signalRewardPredictionError(float error) override;
    
    // Optimized getters for performance
    float getSmoothingFactor() const { return pImpl->smoothingFactor; }
    void setSmoothingFactor(float factor) { pImpl->smoothingFactor = factor; }
    
    // Performance statistics
    struct NeuromodulatorStats {
        float currentLevel = 0.0f;
        float targetLevel = 0.0f;
        float decayRate = 0.0f;
        float updateFrequency = 0.0f;
        float energyEfficiency = 0.0f;
        SimulationStep lastUpdateStep = 0;
    };
    
    NeuromodulatorStats getStats() const;
    
    // Reset with optimized initialization
    void resetOptimized();
};

// Dopamine implementation
class OptimizedDopamine : public OptimizedNeuromodulator {
public:
    OptimizedDopamine() {
        pImpl->level = 0.0f;
        pImpl->target = 0.0f;
        pImpl->decayRate = 0.05f;  // Faster decay for dopamine
        pImpl->releaseRate = 1.5f; // Faster release for reward signals
        pImpl->smoothingFactor = 0.9f; // Less smoothing for fast responses
    }
    
    const char* getName() const override { return "DA-OPT"; }
    
    void signalReward(float reward) override {
        // Optimized reward signaling with burst calculation
        float burst = reward * pImpl->releaseRate;
        pImpl->target = std::min(1.0f, pImpl->level + burst);
        smoothTransitionOptimized(pImpl->target);
    }
    
    void signalRewardPredictionError(float error) override {
        // Optimized prediction error signaling
        float adjustment = error * pImpl->releaseRate * 0.5f;
        pImpl->target = std::max(0.0f, pImpl->level + adjustment);
        smoothTransitionOptimized(pImpl->target);
    }
};

// Curiosity implementation  
class OptimizedCuriosity : public OptimizedNeuromodulator {
public:
    OptimizedCuriosity() {
        pImpl->level = 0.3f;
        pImpl->target = 0.3f;
        pImpl->decayRate = 0.02f;  // Slower decay for curiosity
        pImpl->releaseRate = 0.8f; // Moderate release
        pImpl->smoothingFactor = 0.98f; // High smoothing for stability
    }
    
    const char* getName() const override { return "CURI-OPT"; }
    
    void signalReward(float reward) override {
        // Curiosity responds to reward prediction errors
        float error = reward - 0.5f; // Assume expected reward is 0.5
        float adjustment = std::abs(error) * pImpl->releaseRate * 0.3f;
        pImpl->target = std::clamp(pImpl->level + adjustment, 0.0f, 1.0f);
        smoothTransitionOptimized(pImpl->target);
    }
};

// Novelty implementation
class OptimizedNovelty : public OptimizedNeuromodulator {
public:
    OptimizedNovelty() {
        pImpl->level = 0.2f;
        pImpl->target = 0.2f;
        pImpl->decayRate = 0.01f;  // Very slow decay for novelty
        pImpl->releaseRate = 0.6f; // Moderate release
        pImpl->smoothingFactor = 0.99f; // Very high smoothing
    }
    
    const char* getName() const override { return "NOV-OPT"; }
    
    void signalReward(float reward) override {
        // Novelty responds to surprise/uncertainty
        float surprise = std::abs(reward - 0.5f) * 2.0f; // Scale surprise
        pImpl->target = std::min(1.0f, pImpl->level + surprise * pImpl->releaseRate * 0.2f);
        smoothTransitionOptimized(pImpl->target);
    }
};

// PredictionError implementation
class OptimizedPredictionError : public OptimizedNeuromodulator {
public:
    OptimizedPredictionError() {
        pImpl->level = 0.0f;
        pImpl->target = 0.0f;
        pImpl->decayRate = 0.03f;  // Medium decay
        pImpl->releaseRate = 1.0f; // Standard release
        pImpl->smoothingFactor = 0.92f; // Moderate smoothing
    }
    
    const char* getName() const override { return "PE-OPT"; }
    
    void signalRewardPredictionError(float error) override {
        // Prediction error drives learning and plasticity
        float magnitude = std::abs(error);
        pImpl->target = magnitude * pImpl->releaseRate;
        smoothTransitionOptimized(pImpl->target);
    }
};

} // namespace nlm

// OptimizedNeuromodulator inline implementations

OptimizedNeuromodulator::OptimizedNeuromodulator() {
    pImpl = std::make_unique<Implementation>();
    resetOptimized();
}

OptimizedNeuromodulator::~OptimizedNeuromodulator() = default;

const char* OptimizedNeuromodulator::getName() const {
    return "OPT-MOD";
}

float OptimizedNeuromodulator::getLevel() const {
    return pImpl->level;
}

void OptimizedNeuromodulator::setLevel(float level) {
    updateTargetOptimized(level, 1.0f);
}

float OptimizedNeuromodulator::getPlasticityFactor() const {
    // Optimized plasticity calculation with neuromodulator dynamics
    return 0.5f + 0.5f * pImpl->level;
}

void OptimizedNeuromodulator::update(TimestepDuration dt) {
    if (dt <= 0.0f) return;
    
    SimulationStep currentStep = pImpl->lastUpdate + static_cast<SimulationStep>(dt);
    
    // Exponential smoothing for smooth transitions
    if (pImpl->target != pImpl->level) {
        float diff = pImpl->target - pImpl->level;
        pImpl->level += diff * (1.0f - std::pow(pImpl->smoothingFactor, dt));
    }
    
    // Gentle decay towards target
    float decay = pImpl->decayRate * dt;
    pImpl->level = (1.0f - decay) * pImpl->level + decay * pImpl->target;
    
    // Update history
    pImpl->previousLevel = pImpl->level;
    pImpl->recentLevels[pImpl->historyIndex++] = pImpl->level;
    if (pImpl->historyIndex >= pImpl->recentLevels.size()) {
        pImpl->historyIndex = 0;
    }
    
    pImpl->lastUpdate = currentStep;
}

void OptimizedNeuromodulator::signalReward(float reward) {
    // Base reward signaling with optimization
    float burst = reward * pImpl->releaseRate;
    pImpl->target = std::min(1.0f, pImpl->level + burst);
    smoothTransitionOptimized(pImpl->target);
}

void OptimizedNeuromodulator::signalRewardPredictionError(float error) {
    // Optimized prediction error signaling with adaptive response
    float adaptiveFactor = getAdaptiveResponse();
    float adjustment = error * pImpl->releaseRate * adaptiveFactor;
    pImpl->target = std::max(0.0f, pImpl->level + adjustment);
    smoothTransitionOptimized(pImpl->target);
}

void OptimizedNeuromodulator::updateTargetOptimized(float newLevel, float strength) {
    // Optimized target update with pre-computation
    newLevel = std::clamp(newLevel * strength, 0.0f, 1.0f);
    
    // For fast neuromodulators, use direct assignment
    if (pImpl->smoothingFactor > 0.95f) {
        pImpl->target = newLevel;
    } else {
        // For smooth neuromodulators, use gradual transition
        smoothTransitionOptimized(newLevel);
    }
}

void OptimizedNeuromodulator::smoothTransitionOptimized(float newTarget) {
    // Optimized smooth transition using exponential smoothing
    float maxStep = std::abs(newTarget - pImpl->target) * 0.1f;  // Limit step size
    float step = std::min(maxStep, std::abs(newTarget - pImpl->level));
    
    if (newTarget > pImpl->target) {
        pImpl->target = std::min(pImpl->target + step, newTarget);
    } else {
        pImpl->target = std::max(pImpl->target - step, newTarget);
    }
}

float OptimizedNeuromodulator::getAdaptiveResponse() const {
    // Calculate adaptive response based on history
    if (pImpl->recentLevels.empty()) return 1.0f;
    
    // Simple adaptive response based on recent trend
    float sum = 0.0f;
    size_t count = 0;
    for (float level : pImpl->recentLevels) {
        if (level > 0.0f) {
            sum += level;
            count++;
        }
    }
    
    float avg = (count > 0) ? sum / count : 0.5f;
    return std::clamp(avg, 0.5f, 2.0f);  // Clamp to reasonable range
}

float OptimizedNeuromodulator::calculatePlasticityOptimized(float basePlasticity) const {
    // Optimized plasticity calculation with neuromodulator dynamics
    float neuromodulatorEffect = 1.0f + pImpl->level * 0.5f;
    float historyFactor = getAdaptiveResponse();
    
    return basePlasticity * neuromodulatorEffect * historyFactor;
}

void OptimizedNeuromodulator::resetOptimized() {
    pImpl->level = 0.0f;
    pImpl->target = 0.0f;
    pImpl->previousLevel = 0.0f;
    pImpl->lastUpdate = 0;
    pImpl->historyIndex = 0;
    
    // Clear history
    for (auto& level : pImpl->recentLevels) {
        level = 0.0f;
    }
}

OptimizedNeuromodulator::NeuromodulatorStats OptimizedNeuromodulator::getStats() const {
    NeuromodulatorStats stats;
    stats.currentLevel = pImpl->level;
    stats.targetLevel = pImpl->target;
    stats.decayRate = pImpl->decayRate;
    stats.lastUpdateStep = pImpl->lastUpdate;
    
    // Calculate average update frequency
    if (!pImpl->recentLevels.empty()) {
        float sum = 0.0f;
        size_t count = 0;
        for (float level : pImpl->recentLevels) {
            if (level > 0.0f) {
                sum += level;
                count++;
            }
        }
        stats.updateFrequency = (count > 0) ? sum / count : 0.0f;
    }
    
    // Calculate energy efficiency (inverse of level variance)
    float variance = 0.0f;
    if (!pImpl->recentLevels.empty()) {
        float avg = stats.updateFrequency;
        float sumSq = 0.0f;
        size_t count = 0;
        for (float level : pImpl->recentLevels) {
            if (level > 0.0f) {
                sumSq += (level - avg) * (level - avg);
                count++;
            }
        }
        variance = (count > 0) ? sumSq / count : 0.0f;
        stats.energyEfficiency = 1.0f / (1.0f + variance);
    }
    
    return stats;
}

} // namespace nlm