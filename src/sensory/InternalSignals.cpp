#include "InternalSignals.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    // Time series for trend analysis
    std::vector<float> energyHistory;
    std::vector<float> healthHistory;
    std::vector<float> movementHistory;
    std::vector<float> turningHistory;
    
    // Trend values
    float energyTrend;
    float healthTrend;
    float movementTrend;
    float turningTrend;
    
    Impl() : energyTrend(0.0f), healthTrend(0.0f), movementTrend(0.0f), turningTrend(0.0f) {}
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals) {
    const auto& rawSignals = signals.getData();
    
    // Generate 32 features from internal signals
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    
    // Assume rawSignals contain energy, health, movement, turning in that order
    float energy = 0.0f, health = 0.0f, movement = 0.0f, turning = 0.0f;
    size_t numSignals = rawSignals.size();
    
    if (numSignals >= 4) {
        energy = rawSignals[0];
        health = rawSignals[1];
        movement = rawSignals[2];
        turning = rawSignals[3];
    } else if (numSignals == 3) {
        energy = rawSignals[0];
        health = rawSignals[1];
        movement = rawSignals[2];
        turning = 0.0f;
    } else if (numSignals == 2) {
        energy = rawSignals[0];
        health = rawSignals[1];
        movement = 0.0f;
        turning = 0.0f;
    } else if (numSignals == 1) {
        energy = rawSignals[0];
        health = 0.0f;
        movement = 0.0f;
        turning = 0.0f;
    }
    
    // 1. Basic state variables
    pImpl->processedSignals.push_back(energy);      // Current energy level
    pImpl->processedSignals.push_back(health);      // Current health
    pImpl->processedSignals.push_back(movement);    // Current movement
    pImpl->processedSignals.push_back(turning);     // Current turning rate
    
    // 2. Stress metric (inverted health with movement factor)
    float stress = std::max(0.0f, 1.0f - health + movement * 0.3f);
    pImpl->processedSignals.push_back(stress);
    
    // 3. Motivation metric (energy * health factor)
    float motivation = energy * health * 0.8f;
    pImpl->processedSignals.push_back(motivation);
    
    // 4. Homeostasis metrics
    float homeostasis = std::min(energy, health) * 0.7f + std::min(movement, turning) * 0.3f;
    pImpl->processedSignals.push_back(homeostasis);
    
    // 5. Allostatic load (stress over time)
    float allostaticLoad = stress * 0.5f;
    pImpl->homeostaticSignals.push_back(allostaticLoad);
    
    // 6. Metabolic state (energy utilization efficiency)
    float metabolicState = energy * 0.4f + health * 0.3f + movement * 0.3f;
    pImpl->homeostaticSignals.push_back(metabolicState);
    
    // 7. Time derivatives and trends
    // Simple finite difference for trend calculation
    static float prevEnergy = energy, prevHealth = health;
    float energyTrend = energy - prevEnergy;
    float healthTrend = health - prevHealth;
    float movementTrend = movement - (pImpl->movementHistory.empty() ? 0.0f : pImpl->movementHistory.back());
    float turningTrend = turning - (pImpl->turningHistory.empty() ? 0.0f : pImpl->turningHistory.back());
    
    pImpl->processedSignals.push_back(energyTrend);
    pImpl->processedSignals.push_back(healthTrend);
    pImpl->processedSignals.push_back(movementTrend);
    pImpl->processedSignals.push_back(turningTrend);
    
    // 8. State stability (how much signals change)
    float stability = 1.0f / (1.0f + std::abs(energyTrend) + std::abs(healthTrend));
    pImpl->processedSignals.push_back(stability);
    
    // 9. Recovery rate (return toward homeostasis)
    float targetEnergy = 0.5f, targetHealth = 0.8f;
    float recoveryRate = std::abs(targetEnergy - energy) + std::abs(targetHealth - health);
    pImpl->processedSignals.push_back(1.0f - recoveryRate);  // Higher means closer to target
    
    // 10. Energy efficiency (output per input)
    float energyEfficiency = energy * health / (movement + 0.1f);
    pImpl->processedSignals.push_back(energyEfficiency);
    
    // Update history for trend analysis
    pImpl->energyHistory.push_back(energy);
    pImpl->healthHistory.push_back(health);
    pImpl->movementHistory.push_back(movement);
    pImpl->turningHistory.push_back(turning);
    
    // Keep history manageable
    const size_t maxHistory = 100;
    if (pImpl->energyHistory.size() > maxHistory) {
        pImpl->energyHistory.erase(pImpl->energyHistory.begin());
        pImpl->healthHistory.erase(pImpl->healthHistory.begin());
        pImpl->movementHistory.erase(pImpl->movementHistory.begin());
        pImpl->turningHistory.erase(pImpl->turningHistory.begin());
    }
    
    prevEnergy = energy;
    prevHealth = health;
    
    // Generate remaining features (aim for ~32 total)
    // Past performance features
    if (pImpl->energyHistory.size() >= 2) {
        float avgEnergy = std::accumulate(pImpl->energyHistory.begin(), pImpl->energyHistory.end(), 0.0f) / pImpl->energyHistory.size();
        float varEnergy = 0.0f;
        for (float e : pImpl->energyHistory) {
            varEnergy += (e - avgEnergy) * (e - avgEnergy);
        }
        varEnergy /= pImpl->energyHistory.size();
        pImpl->processedSignals.push_back(varEnergy);  // Energy variance
        pImpl->processedSignals.push_back(avgEnergy);  // Average energy
    } else {
        pImpl->processedSignals.push_back(0.0f);
        pImpl->processedSignals.push_back(energy);
    }
    
    // Current state quality
    float stateQuality = energy * health * movement * 0.25f;
    pImpl->processedSignals.push_back(stateQuality);
    
    // System resilience (ability to maintain function)
    float resilience = health * (1.0f - stress);
    pImpl->processedSignals.push_back(resilience);
    
    // Add some diversity features
    for (int i = 0; i < 5; ++i) {
        pImpl->processedSignals.push_back(static_cast<float>(i) * 0.05f);  // Placeholder features
    }
    
    // Print debug info
    std::cout << "InternalSignalsProcessor: Processed " << numSignals << " signals into " 
              << pImpl->processedSignals.size() << " features" << std::endl;
    std::cout << "  Energy: " << energy << ", Health: " << health << ", Stress: " << stress 
              << ", Motivation: " << motivation << ", Homeostasis: " << homeostasis << std::endl;
    std::cout << "  Trends: E+" << energyTrend << " H+" << healthTrend << ", "
              << "Movement+" << movementTrend << " Turning+" << turningTrend << std::endl;
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
