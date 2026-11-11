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
    // Real maturation: neuronal properties gradually change toward adult values
    // Based on developmental biology: neurons mature through activity-dependent processes
    
    if (!brain) return;
    
    // Get development system for stage information
    DevelopmentSystem* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) return;
    
    // Maturation is a continuous process that accelerates during critical periods
    float developmentalStage = devSystem->getCriticalPeriodProgress();
    
    // Maturation rate depends on developmental stage
    // Faster during critical period, slower in adulthood
    float maturationRate = 0.001f * (1.0f + developmentalStage * 5.0f);
    
    // Update progress
    pImpl->progress += maturationRate;
    if (pImpl->progress > 1.0f) {
        pImpl->progress = 1.0f;
    }
    
    // Apply maturation effects to neurons based on progress
    applyMaturationEffects(brain, currentStep);
    
    // Log progress periodically
    if (currentStep % 100 == 0) {
        NLM_LOG_INFO("Maturation progress: " + std::to_string(pImpl->progress * 100.0f) + "%");
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

// Helper method to apply maturation effects to neurons
void Maturation::applyMaturationEffects(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    float maturationFactor = pImpl->progress; // 0.0 to 1.0
    
    // Get all regions
    auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        
        for (const auto* neuron : neurons) {
            // Apply gradual changes to neuron properties based on maturation progress
            
            // 1. Threshold moves toward mature value
            NeuronState& state = neuron->getState();
            float targetThreshold = pImpl->matureThreshold;
            float currentThreshold = state.threshold;
            
            float thresholdStep = (targetThreshold - currentThreshold) * maturationFactor * 0.1f;
            state.threshold += thresholdStep;
            
            // 2. Resting potential moves toward mature value
            float targetResting = pImpl->matureRestingPotential;
            float currentResting = state.restingPotential;
            
            float restingStep = (targetResting - currentResting) * maturationFactor * 0.05f;
            state.restingPotential += restingStep;
            
            // 3. Leak conductance increases with maturation (more stable)
            float matureConductance = 20.0f; // Mature neurons have higher conductance
            float currentConductance = state.leakConductance;
            
            float conductanceStep = (matureConductance - currentConductance) * maturationFactor * 0.02f;
            state.leakConductance += conductanceStep;
            
            // 4. Refractory period matures toward optimal value
            uint32_t matureRefractory = 10; // 10ms mature refractory period
            uint32_t currentRefractory = state.refractoryPeriod;
            
            if (matureRefractory > currentRefractory) {
                uint32_t refractoryStep = static_cast<uint32_t>(maturationFactor * 2.0f);
                state.refractoryPeriod = std::min(matureRefractory, currentRefractory + refractoryStep);
            }
            
            // 5. Adaptation variable matures (reduces spike-frequency adaptation)
            float matureAdaptation = 0.5f;
            float currentAdaptation = state.adaptationVariable;
            
            float adaptationStep = (matureAdaptation - currentAdaptation) * maturationFactor * 0.01f;
            state.adaptationVariable += adaptationStep;
            
            // 6. Threshold noise decreases with maturation (more stable firing)
            // This is simulated by reducing the random initialization range
            // In real implementation, this would affect neural excitability
        }
    }
}

} // namespace nlm