#pragma once

namespace nlm {

/**
 * @file BrainPrediction.cpp
 * @brief Implementation of brain prediction systems
 * 
 * This file contains the implementation of prediction systems that enable
 * the brain to anticipate future states and generate predictions:
 * - PredictionSystem: Sensory prediction and error computation
 * - PredictionError: Prediction error signals for learning
 * 
 * Prediction systems process sensory input to generate expectations and
 * compute prediction errors that drive learning and adaptation.
 */

#include "Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>

/**
 * @brief Prediction system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain's
 * prediction systems. Prediction mechanisms provide:
 * - Forward models of sensory input
 * - Prediction error signals
 * - Expectation generation
 * - Error-based learning signals
 */

// Prediction system getter implementations
PredictionSystem* Brain::getPredictionSystem() {
    if (pImpl->predictionSystem) {
        return pImpl->predictionSystem.get();
    }
    return nullptr;
}

/**
 * @brief Get prediction system statistics
 * 
 * @return String containing prediction system statistics
 */
std::string Brain::getPredictionStats() const {
    std::stringstream stats;
    stats << "Prediction Statistics:\n";
    
    if (pImpl->predictionSystem) {
        stats << "  PredictionSystem: Active\n";
        // Could add more detailed prediction statistics here
    }
    
    if (pImpl->predictionError) {
        stats << "  PredictionError: Active\n";
        stats << "  Current Error Level: " << pImpl->predictionError->getLevel() << "\n";
    }
    
    return stats.str();
}

/**
 * @brief Update prediction systems
 * 
 * This function updates prediction systems based on current sensory input
 * and brain state. It generates predictions and computes prediction errors
 * that drive learning and adaptation.
 * 
 * @param timestep Current simulation timestep
 */
void Brain::updatePredictionSystems(TimestepDuration timestep) {
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(timestep);
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update(timestep);
    }
}

/**
 * @brief Generate prediction error signal
 * 
 * @param predicted Expected sensory input
 * @param actual Actual sensory input
 * @return Prediction error signal
 */
float Brain::generatePredictionError(const class SensoryInput& predicted, const class SensoryInput& actual) {
    if (pImpl->predictionError) {
        return pImpl->predictionError->computeError(predicted, actual);
    }
    return 0.0f;
}

/**
 * @brief Reset prediction systems
 */
void Brain::resetPredictionSystems() {
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->reset();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->reset();
    }
}

} // namespace nlm
