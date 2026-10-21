#pragma once

namespace nlm {

/**
 * @file BrainNeuromodulation.cpp
 * @brief Implementation of brain neuromodulation systems
 * 
 * This file contains the implementation of neuromodulatory systems that regulate
 * brain function and plasticity:
 * - Dopamine: Reward prediction and reinforcement learning
 * - Curiosity: Exploration motivation and novelty detection
 * - Novelty: Novelty detection and surprise processing
 * - PredictionError: Prediction error signals for learning
 * 
 * Neuromodulators influence neural excitability, plasticity rates, and learning
 * dynamics throughout the brain network.
 */

#include "Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>

/**
 * @brief Neuromodulation system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain's
 * neuromodulatory systems. Neuromodulators provide:
 * - Global regulation of neural excitability
 * - Modulation of plasticity rates
 * - Reward-based learning signals
 * - Exploration vs. exploitation balance
 */

// Neuromodulator getter implementations
Dopamine* Brain::getDopamine() {
    if (pImpl->dopamine) {
        return pImpl->dopamine.get();
    }
    return nullptr;
}

Curiosity* Brain::getCuriosity() {
    if (pImpl->curiosity) {
        return pImpl->curiosity.get();
    }
    return nullptr;
}

Novelty* Brain::getNovelty() {
    if (pImpl->novelty) {
        return pImpl->novelty.get();
    }
    return nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (pImpl->predictionError) {
        return pImpl->predictionError.get();
    }
    return nullptr;
}

/**
 * @brief Get dopamine level
 * 
 * @return Current dopamine level (0.0 to 1.0)
 */
float Brain::getDopamineLevel() const {
    if (pImpl->dopamine) {
        return pImpl->dopamine->getLevel();
    }
    return 0.0f;
}

/**
 * @brief Get curiosity level
 * 
 * @return Current curiosity level (0.0 to 1.0)
 */
float Brain::getCuriosityLevel() const {
    if (pImpl->curiosity) {
        return pImpl->curiosity->getLevel();
    }
    return 0.0f;
}

/**
 * @brief Get novelty level
 * 
 * @return Current novelty level (0.0 to 1.0)
 */
float Brain::getNoveltyLevel() const {
    if (pImpl->novelty) {
        return pImpl->novelty->getLevel();
    }
    return 0.0f;
}

/**
 * @brief Apply neuromodulatory signals
 * 
 * This function applies neuromodulatory signals to the brain systems,
 * affecting neural excitability, plasticity rates, and learning dynamics.
 * 
 * @param signal Neuromodulator signal to apply
 */
void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates based on neuromodulation
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
    
    // Apply neuromodulation effects to plasticity factors
    if (pImpl->dopamine) {
        float currentPlasticityFactor = pImpl->dopamine->getPlasticityFactor();
        // Adjust plasticity factor based on external modulation
        float newPlasticityFactor = currentPlasticityFactor * modulation;
        // Clamp to reasonable range
        newPlasticityFactor = std::max(0.0f, std::min(1.0f, newPlasticityFactor));
        pImpl->dopamine->setPlasticityFactor(newPlasticityFactor);
    }
}

/**
 * @brief Update neuromodulation systems
 * 
 * This function updates all neuromodulation systems based on current brain state
 * and environmental inputs. It processes:
 * - Novelty detection from unexpected inputs
 * - Curiosity signals for exploration motivation
 * - Reward prediction errors
 * - Dopamine release for reinforcement learning
 */
void Brain::updateNeuromodulation() {
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update(pImpl->timestep);
    }
}

/**
 * @brief Get current neuromodulation statistics
 * 
 * @return String containing neuromodulation statistics
 */
std::string Brain::getNeuromodulationStats() const {
    std::stringstream stats;
    stats << "Neuromodulation Statistics:\n";
    
    if (pImpl->dopamine) {
        stats << "  Dopamine: level=" << pImpl->dopamine->getLevel() 
              << ", plasticity_factor=" << pImpl->dopamine->getPlasticityFactor() << "\n";
    }
    
    if (pImpl->curiosity) {
        stats << "  Curiosity: level=" << pImpl->curiosity->getLevel() << "\n";
    }
    
    if (pImpl->novelty) {
        stats << "  Novelty: level=" << pImpl->novelty->getLevel() << "\n";
    }
    
    if (pImpl->predictionError) {
        stats << "  PredictionError: level=" << pImpl->predictionError->getLevel() << "\n";
    }
    
    return stats.str();
}

/**
 * @brief Reset neuromodulation systems
 */
void Brain::resetNeuromodulation() {
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->reset();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->reset();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->reset();
    }
}

} // namespace nlm
