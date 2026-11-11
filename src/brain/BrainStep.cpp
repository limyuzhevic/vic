#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void Brain::Impl::step(SimulationStep step, Timestamp currentTime) {
    // Run 15 simulation steps as documented in Phase 6
    for (int s = 0; s < 15; ++s) {
        SimulationStep stepNum = step + s;
        Timestamp time = currentTime + s * timestep;
        
        // Step 1: Update spike system (generate new spikes based on current membrane potentials)
        if (spikeSystem) {
            spikeSystem->update(this, time);
        }
        
        // Step 2: Process spike events (propagate spikes through synaptic connections)
        if (spikeSystem) {
            spikeSystem->processEvents(time);
        }
        
        // Step 3: Update neuron states (LIF dynamics, refractory periods, membrane potentials)
        for (auto& region : regions) {
            region->updateNeurons(time);
        }
        
        // Step 4: Apply plasticity (STDP, Hebbian, structural plasticity)
        if (stdp) stdp->update(time);
        if (hebbian) hebbian->update(time);
        if (structuralPlasticity) structuralPlasticity->update(time, *rng);
        
        // Step 5: Update neuromodulation systems
        if (dopamine) dopamine->update(time);
        if (novelty) novelty->update(time);
        if (predictionError) predictionError->update(time);
        if (curiosity) curiosity->update(time);
        
        // Step 6: Update memory systems
        if (workingMemory) workingMemory->update(time);
        if (episodicMemory) episodicMemory->update(time);
        if (associativeMemory) associativeMemory->update(time);
        if (attention) attention->update(time);
        
        // Step 7: Update prediction system
        if (predictionSystem) predictionSystem->update(time);
        
        // Step 8: Update concept formation
        if (conceptFormation) conceptFormation->update(time);
        
        // Step 9: Update planner
        if (planner) planner->update(time);
        
        // Step 10: Update development system
        if (developmentSystem) developmentSystem->update(time);
        
        // Step 11: Update spike system (after first 5 steps)
        if (s >= 5 && spikeSystem) {
            spikeSystem->postSynapticUpdate(time);
        }
        
        // Step 12: Handle inter-region connections (for Phase 5+ functionality)
        for (const auto& conn : interRegionConnections) {
            auto source = std::find_if(regions.begin(), regions.end(),
                [id = conn.sourceRegion](const std::unique_ptr<NeuralRegion>& r) {
                    return r->getId() == id;
                });
            auto target = std::find_if(regions.begin(), regions.end(),
                [id = conn.targetRegion](const std::unique_ptr<NeuralRegion>& r) {
                    return r->getId() == id;
                });
            if (source != regions.end() && target != regions.end()) {
                source->get()->updateInterRegionConnections(*target->get(), conn, time);
            }
        }
        
        // Step 13: Update checkpoint manager
        if (checkpointManager) {
            checkpointManager->update(stepNum, time);
        }
        
        // Step 14: Update development system (second check)
        if (developmentSystem) {
            developmentSystem->updateDevelopmentContributions(time);
        }
        
        // Step 15: Apply neuromodulatory effects on plasticity
        if (dopamine) dopamine->apply();
        if (novelty) novelty->apply();
        if (predictionError) predictionError->apply();
        if (curiosity) curiosity->apply();
    }
}

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