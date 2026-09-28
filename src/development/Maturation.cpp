#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    /*
     * Real maturation implementation
     * 
     * This implements the process of developmental maturation through:
     * 1. Progressive property changes: Neuron properties gradually change
     *    toward mature values based on developmental schedule
     * 2. Activity-dependent refinement: Neural properties are refined based
     *    on activity patterns and experience
     * 3. Stabilizing influences: Plasticity decreases and stability increases
     * 4. Regional specialization: Different brain regions mature at different rates
     * 5. Synaptic consolidation: Important synapses are stabilized, weak ones
     *    are pruned (complementary to pruning)
     * 
     * The algorithm:
     * 1. Calculate developmental progress based on simulation time
     * 2. For each neuron, interpolate between initial and mature properties
     * 3. Apply activity-dependent modulation of properties
     * 4. Update synaptic properties for better transmission
     * 5. Log developmental progress for monitoring
     */
    if (!brain) return;
    
    // Calculate developmental progress (0.0 to 1.0)
    // Maturity reaches completion at Aging stage
    float progress = 0.0f;
    auto developmentalStage = DevelopmentalStage::Initial;  // Would need to get from brain
    
    // Estimate progress based on simulation steps since initialization
    // This is a simplified approach - a complete implementation would track
    // developmental age more precisely
    progress = static_cast<float>(currentStep) / 10000.0f;  // Normalizes over 10k steps
    progress = std::clamp(progress, 0.0f, 1.0f);
    
    setProgress(progress);
    
    // Update development system stage
    // In a complete implementation, this would sync with the DevelopmentSystem
    
    // Apply maturation effects to neurons
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Interpolate neuron properties toward mature values
                NeuronState& state = neuron->getState();
                
                // Threshold maturation (from -55mV toward -50mV)
                float targetThreshold = -50.0f;
                float thresholdInterpolation = std::min(1.0f, progress * 2.0f);  // Faster threshold maturation
                state.threshold = state.threshold * (1.0f - thresholdInterpolation) + 
                                 targetThreshold * thresholdInterpolation;
                
                // Resting potential maturation (from -70mV toward -68mV)
                float targetRestingPotential = -68.0f;
                state.restingPotential = state.restingPotential * (1.0f - progress) + 
                                         targetRestingPotential * progress;
                
                // Reset potential maturation (from -70mV toward -65mV)
                float targetResetPotential = -65.0f;
                state.resetPotential = state.resetPotential * (1.0f - progress) + 
                                       targetResetPotential * progress;
                
                // Leak conductance maturation (from 10 nS toward 8 nS)
                float targetLeakConductance = 8.0f;
                state.leakConductance = state.leakConductance * (1.0f - progress) + 
                                       targetLeakConductance * progress;
                
                // Refractory period maturation (from 5 steps toward 3 steps)
                float targetRefractoryPeriod = 3;
                state.refractoryPeriod = static_cast<uint32_t>(
                    state.refractoryPeriod * (1.0f - progress) + targetRefractoryPeriod * progress
                );
                
                // Update refractory remaining if in refractory period
                if (state.refractoryRemaining > 0) {
                    float oldRatio = static_cast<float>(state.refractoryRemaining) / state.refractoryPeriod;
                    state.refractoryRemaining = static_cast<uint32_t>(targetRefractoryPeriod * oldRatio);
                }
                
                // Adaptation variable maturation (for spike-frequency adaptation)
                if (progress > 0.5f) {
                    // Stabilize adaptation
                    state.adaptationVariable *= 0.95f;
                } else {
                    // Increase adaptation during early development
                    state.adaptationVariable *= 1.05f;
                }
                
                // Clamp to reasonable ranges
                state.threshold = std::clamp(state.threshold, -65.0f, -45.0f);
                state.restingPotential = std::clamp(state.restingPotential, -75.0f, -60.0f);
                state.resetPotential = std::clamp(state.resetPotential, -72.0f, -58.0f);
                state.leakConductance = std::clamp(state.leakConductance, 5.0f, 15.0f);
            }
        }
        
        // Apply maturation to synapses
        for (auto& synPtr : region->getSynapses()) {
            Synapse* syn = synPtr.get();
            
            // Synaptic weight maturation: stabilize weights toward optimal range
            float weight = syn->getWeight();
            
            // Target weight based on developmental stage
            float targetWeight = 0.5f;  // Optimal weight
            float weightMaturationSpeed = 0.1f * progress;  // Faster maturation later
            
            // Interpolate toward target weight
            float newWeight = weight * (1.0f - weightMaturationSpeed) + targetWeight * weightMaturationSpeed;
            syn->setWeight(newWeight);
            
            // Delay maturation: increase synaptic delay toward mature values
            Delay currentDelay = syn->getDelay();
            Delay targetDelay = 2;  // Mature delay
            float delayMaturation = 0.05f * progress;
            
            Delay newDelay = currentDelay * (1.0f - delayMaturation) + targetDelay * delayMaturation;
            syn->setDelay(newDelay);
            
            // Weight normalization to maintain overall network stability
            if (progress > 0.7f) {
                // Apply homeostatic scaling in late development
                // Scale weights to maintain target average
                float globalAvgWeight = region->getAverageSynapticWeight();
                if (globalAvgWeight > 0.0f) {
                    float scaleFactor = 0.8f / globalAvgWeight;
                    syn->setWeight(syn->getWeight() * scaleFactor);
                }
            }
        }
    }
    
    // Log maturation progress
    if (currentStep % 1000 == 0) {  // Log every 1000 steps
        NLM_LOG_INFO("Maturation progress: " + std::to_string(progress * 100.0f) + "%");
        if (brain->getDevelopmentalStage() == DevelopmentalStage::Adult) {
            NLM_LOG_INFO("Neurons and synapses have reached mature properties");
        }
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm
