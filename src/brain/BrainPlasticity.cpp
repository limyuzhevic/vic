#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Brain::Impl::applyRewardModulation(float reward) {
    if (rewardModulationEnabled) {
        predictionError = reward;
        expectedReward = reward;
        
        if (dopamine) {
            dopamine->update(predictionError);
        }
        
        if (predictionError) {
            predictionError->update(reward);
        }
        
        if (novelty) {
            novelty->update(reward);
        }
        
        if (curiosity) {
            curiosity->update(reward);
        }
    }
}

void Brain::Impl::updatePlasticity() {
    // Plasticity updates are now integrated into the step() loop
    // This method is kept for API compatibility
}

void Brain::Impl::develop() {
    // Development is integrated into the step() loop
    // This method is kept for API compatibility
}

} // namespace nlm