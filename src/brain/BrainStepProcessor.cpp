// brain/BrainStepProcessor.cpp - Brain step processing implementation
// Contains the main simulation step loop with all 15 steps

#include "brain/BrainStepProcessor.hpp"
#include "core/Logger/Logger.hpp"
#include <cmath>

// Modern C++ constants for better code readability and maintainability
constexpr float INPUT_NORMALIZATION_FACTOR = 10.0f;
constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
constexpr float LIF_THRESHOLD_VOLTAGE = 25.0f;
constexpr float LIF_RESET_VOLTAGE = 15.0f;

namespace nlm {

/**
 * @brief Perform a single simulation step of the integrated brain
 * 
 * This method implements the complete 15-step integrated brain simulation loop:
 * Each step represents a distinct computational process in the artificial brain.
 * 
 * @param currentStep Current simulation step number
 * @param currentTime Current simulation time (seconds)
 * 
 * @note The 15-step process represents a single moment of brain activity, where:
 *       - Steps 1-3: Neural dynamics (spike generation and propagation)
 *       - Steps 4-5: Memory and modulation (working memory, neuromodulation)
 *       - Steps 6-7: Learning and memory (plasticity, episodic memory)
 *       - Steps 8-10: Higher cognition (prediction, attention, concept formation)
 *       - Steps 11-13: Structural adaptation (plasticity, replay, development)
 *       - Step 14: Statistics collection (performance monitoring)
 * 
 * @see nlm::Brain::Impl::step(SimulationStep, Timestamp) for implementation details
 * 
 * The step processing order is critical for proper brain operation:
 * 1. Delayed spike processing → 2. Neuron potential updates → 3. Spike detection
 * 4. Working memory updates → 5. Neuromodulation → 6. Plasticity rules
 * 7. Episodic memory → 8. Prediction system → 9. Attention system
 * 10. Concept formation → 11. Structural plasticity → 12. Memory replay
 * 13. Development effects → 14. Statistics collection
 */
void Brain::Impl::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spike events (deliver synaptic input) ==========
    // This step delivers synaptic input that was delayed during previous steps
    // Synaptic delays are modeled to represent biological transmission delays
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neuron membrane potentials (LIF dynamics) ==========
    // This step updates the membrane potentials of all neurons according to the
    // leaky integrate-and-fire (LIF) neuron model
    pImpl->neuronSystem->updateNeurons(currentStep, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule outgoing spike events ==========
    // This step detects neurons that have reached firing threshold and schedules
    // outgoing spikes to their synaptic targets
    pImpl->spikeSystem->detectSpikes(currentStep, currentTime);
    
    // ========== STEP 4: Update working memory (maintenance and competition) ==========
    // This step updates the working memory with current neural activity and
    // maintains the most salient information through competition
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(currentStep, currentTime);
    }
    
    // ========== STEP 5: Apply neuromodulation effects on neural excitability ==========
    // This step applies neuromodulatory signals (dopamine, acetylcholine, etc.) to
    // influence neural excitability and plasticity based on behavioral relevance
    if (pImpl->dopamine) {
        pImpl->dopamine->update(currentTime);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->update(currentTime);
    }
    if (pImpl->novelty) {
        pImpl->novelty->update(currentTime);
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP, Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory with current experience ==========
    // This step stores current neural activity in episodic memory for later recall
    // and consolidation into long-term memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->update(currentStep, currentTime);
    }
    
    // ========== STEP 8: Update prediction system ==========
    // This step updates the prediction system's predictive models and prediction errors
    // based on current sensory input and neural activity
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(currentTime);
    }
    
    // ========== STEP 9: Update attention system ==========
    // This step updates attentional selection mechanisms to focus processing on
    // the most relevant information and tasks
    if (pImpl->attention) {
        pImpl->attention->update(currentTime);
    }
    
    // ========== STEP 10: Update concept formation ==========
    // This step updates concept formation mechanisms to extract abstract patterns
    // and relationships from neural activity
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(currentTime);
    }
    
    // ========== STEP 11: Apply structural plasticity (synaptogenesis, pruning) ==========
    // This step manages structural plasticity, including formation and elimination
    // of synaptic connections to adapt the brain's connectivity architecture
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(currentTime);
    }
    
    // ========== STEP 12: Replay important memories (during rest or periodically) ==========
    // This step replays important memories to strengthen memory traces and
    // facilitate consolidation, particularly during rest or periodic replay
    if (currentStep % pImpl->replayInterval == 0 && !pImpl->isResting && pImpl->episodicMemory) {
        pImpl->episodicMemory->replay(currentTime);
    }
    
    // ========== STEP 13: Apply development effects ==========
    // This step applies developmental changes and plasticity modulation based on
    // the brain's current developmental stage
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(currentTime);
    }
    
    // ========== STEP 14: Collect statistics ==========
    // This step collects and updates various neural activity statistics for
    // monitoring and analysis of brain performance and dynamics
    pImpl->totalSpikesThisStep = 0;
    if (pImpl->spikeSystem) {
        pImpl->totalSpikesThisStep = pImpl->spikeSystem->getTotalSpikeCount();
    }
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
    
    // ========== STEP 15: LOG STATUS ==========
    // Log status periodically for monitoring
    if (currentStep % 1000 == 0) {
        pImpl->logStatus();
    }
}

} // namespace nlm

// Template function for processing all regions and populations
// This provides a clean way to apply the same operation to all regions
// and their populations with structured binding support
template<typename ProcessingFunc>
void processAllRegions(const ProcessingFunc& func) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            func(pop.get());
        }
    }
}

// Alternative template for processing all neurons directly
template<typename ProcessingFunc>
void processAllNeurons(const ProcessingFunc& func) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            const auto& neurons = pop->getNeurons();
            for (auto& neuron : neurons) {
                func(neuron.get());
            }
        }
    }
}

// Template function for processing all regions and populations
    // This provides a clean way to apply the same operation to all regions
    // and their populations with structured binding support
    template<typename ProcessingFunc>
    void processAllRegions(const ProcessingFunc& func) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                func(pop.get());
            }
        }
    }
    
    // Alternative template for processing all neurons directly
    template<typename ProcessingFunc>
    void processAllNeurons(const ProcessingFunc& func) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                const auto& neurons = pop->getNeurons();
                for (auto& neuron : neurons) {
                    func(neuron.get());
                }
            }
        }
    }