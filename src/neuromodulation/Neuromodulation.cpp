#include "Neuromodulation.hpp"
#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Neuromodulation::Impl {
    std::vector<std::unique_ptr<Neuromodulator>> neuromodulators;
    bool initialized;
    
    Impl() : initialized(false) {
        // Create all neuromodulators
        neuromodulators.emplace_back(std::make_unique<Dopamine>());
        neuromodulators.emplace_back(std::make_unique<Acetylcholine>());
        neuromodulators.emplace_back(std::make_unique<Norepinephrine>());
        neuromodulators.emplace_back(std::make_unique<Serotonin>());
    }
};

Neuromodulation::Neuromodulation() : pImpl(std::make_unique<Impl>()) {}

Neuromodulation::~Neuromodulation() = default;

bool Neuromodulation::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize neuromodulation without brain reference");
        return false;
    }
    
    NLM_LOG_INFO("Initializing neuromodulation system...");
    
    // Initialize all neuromodulators
    for (auto& neuromodulator : pImpl->neuromodulators) {
        neuromodulator->initialize(brain);
        NLM_LOG_INFO(std::string("Initialized ") + neuromodulator->getName());
    }
    
    // Initialize curiosity and prediction error systems
    curiosity.initialize(brain);
    novelty.initialize(brain);
    predictionError.initialize(brain);
    reward.initialize(brain);
    
    pImpl->initialized = true;
    
    NLM_LOG_INFO("Neuromodulation system initialized successfully");
    return true;
}

void Neuromodulation::update(TimestepDuration dt) {
    if (!pImpl->initialized) {
        NLM_LOG_WARNING("Neuromodulation not initialized, skipping update");
        return;
    }
    
    // Update all neuromodulators
    for (auto& neuromodulator : pImpl->neuromodulators) {
        neuromodulator->update(dt);
    }
    
    // Update curiosity and prediction error systems
    curiosity.update(0.0f, 0.0f, dt);  // These will be updated with real values from brain
    novelty.update(dt);
    predictionError.updatePrediction(0.0f);  // This will be updated from brain
    reward.reset();  // Reset reward each timestep
}

void Neuromodulation::signalReward(float rewardValue) {
    if (!pImpl->initialized) return;
    
    // Find and signal dopamine
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "DA") {
            neuromodulator->signalReward(rewardValue);
            break;
        }
    }
    
    // Record reward
    reward.add(rewardValue);
}

void Neuromodulation::signalPredictionError(float error) {
    if (!pImpl->initialized) return;
    
    // Find and signal dopamine and norepinephrine
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "DA") {
            neuromodulator->signalRewardPredictionError(error);
        } else if (std::string(neuromulator->getName()) == "NE") {
            neuromodulator->signalNoveltySurprise(error);
        }
    }
    
    // Record prediction error
    predictionError.computeError(0.0f, error);
}

void Neuromodulation::signalNovelty(float novelty) {
    if (!pImpl->initialized) return;
    
    // Find and signal norepinephrine and acetylcholine
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "NE") {
            neuromodulator->signalNoveltySurprise(novelty);
        } else if (std::string(neuromulator->getName()) == "ACh") {
            neuromodulator->signalAttention(novelty);
        }
    }
    
    // Update curiosity system
    curiosity.update(novelty, predictionError.getError(), 0.0f);
}

void Neuromodulation::signalAttention(float attention) {
    if (!pImpl->initialized) return;
    
    // Find acetylcholine and signal attention
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "ACh") {
            neuromodulator->signalAttention(attention);
            break;
        }
    }
}

void Neuromodulation::signalPunishment(float punishment) {
    if (!pImpl->initialized) return;
    
    // Find serotonin and signal punishment
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "5-HT") {
            neuromodulator->signalPunishment(punishment);
            break;
        }
    }
}

const Dopamine& Neuromodulation::getDopamine() const {
    for (const auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "DA") {
            return static_cast<const Dopamine&>(*neuromodulator);
        }
    }
    throw std::runtime_error("Dopamine neuromodulator not found");
}

const Acetylcholine& Neuromodulation::getAcetylcholine() const {
    for (const auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "ACh") {
            return static_cast<const Acetylcholine&>(*neuromodulator);
        }
    }
    throw std::runtime_error("Acetylcholine neuromodulator not found");
}

const Norepinephrine& Neuromodulation::getNorepinephrine() const {
    for (const auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "NE") {
            return static_cast<const Norepinephrine&>(*neuromodulator);
        }
    }
    throw std::runtime_error("Norepinephrine neuromodulator not found");
}

const Serotonin& Neuromodulation::getSerotonin() const {
    for (const auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromulator->getName()) == "5-HT") {
            return static_cast<const Serotonin&>(*neuromodulator);
        }
    }
    throw std::runtime_error("Serotonin neuromodulator not found");
}

const Curiosity& Neuromodulation::getCuriosity() const {
    return curiosity;
}

const Novelty& Neuromodulation::getNovelty() const {
    return novelty;
}

const PredictionError& Neuromodulation::getPredictionError() const {
    return predictionError;
}

const Reward& Neuromodulation::getReward() const {
    return reward;
}

float Neuromodulation::getOverallNeuromodulatoryEffect() const {
    if (!pImpl->initialized) return 0.0f;
    
    // Calculate weighted sum of neuromodulator effects
    float totalEffect = 0.0f;
    float weightSum = 0.0f;
    
    for (const auto& neuromodulator : pImpl->neuromodulators) {
        float effect = neuromodulator->getPlasticityFactor();
        totalEffect += effect;
        weightSum += 1.0f;
    }
    
    return totalEffect / weightSum;
}

void Neuromodulation::logStatus() const {
    NLM_LOG_INFO("Neuromodulation System Status:");
    NLM_LOG_INFO("  Initialized: " + std::string(pImpl->initialized ? "Yes" : "No"));
    
    if (pImpl->initialized) {
        for (const auto& neuromodulator : pImpl->neuromodulators) {
            NLM_LOG_INFO("  " + std::string(neuromulator->getName()) + 
                        ": Level=" + std::to_string(neuromulator->getLevel()) +
                        ", Plasticity=" + std::to_string(neuromulator->getPlasticityFactor()));
        }
        
        NLM_LOG_INFO("  Novelty: Level=" + std::to_string(novelty.getLevel()));
        NLM_LOG_INFO("  Curiosity: Level=" + std::to_string(curiosity.getLevel()));
        NLM_LOG_INFO("  Prediction Error: Magnitude=" + std::to_string(predictionError.getMagnitude()));
        NLM_LOG_INFO("  Reward: Value=" + std::to_string(reward.getValue()));
    }
}

} // namespace nlm