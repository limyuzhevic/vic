#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

// Brain neuromodulation and plasticity methods

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulatory signals
    if (pImpl->dopamine) {
        pImpl->dopamine->applySignal(signal);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->applySignal(signal);
    }
    if (pImpl->novelty) {
        pImpl->novelty->applySignal(signal);
    }
    if (pImpl->predictionError) {
        pImpl->predictionError->applySignal(signal);
    }
}

void Brain::updatePlasticity() {
    // Update plasticity rules
    if (pImpl->stdp) {
        pImpl->stdp->update();
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->update();
    }
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update();
    }
}

void Brain::develop() {
    // Apply developmental changes
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->develop();
    }
}

void Brain::reset() {
    // Reset brain state
    // Reset neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->reset();
            }
        }
    }
    
    // Reset synapses
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            syn->reset();
        }
    }
    
    // Reset system state
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset integrated systems
    if (pImpl->workingMemory) pImpl->workingMemory->reset();
    if (pImpl->episodicMemory) pImpl->episodicMemory->reset();
    if (pImpl->associativeMemory) pImpl->associativeMemory->reset();
    if (pImpl->predictionSystem) pImpl->predictionSystem->reset();
    if (pImpl->planner) pImpl->planner->reset();
    if (pImpl->conceptFormation) pImpl->conceptFormation->reset();
    if (pImpl->attention) pImpl->attention->reset();
    if (pImpl->developmentSystem) pImpl->developmentSystem->reset();
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
    if (pImpl->stdp) pImpl->stdp->reset();
    if (pImpl->hebbian) pImpl->hebbian->reset();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->reset();
    if (pImpl->checkpointManager) pImpl->checkpointManager->clear();
    
    NLM_LOG_INFO("Brain state reset complete");
}

} // namespace nlm