// Implementation of Neural Maturation - developmental changes to neuron properties
#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include <algorithm>

namespace nlm {

struct Maturation::Impl {
    float progress;
    float stage;
    float developmentalTime;
    
    Impl() 
        : progress(0.0f)
        , stage(0.0f)
        , developmentalTime(0.0f) {}
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
    if (!brain) {
        return;
    }
    
    // Advance maturation based on developmental time
    pImpl->developmentalTime += 0.001; // 1ms timestep
    
    // Calculate maturation progress based on developmental stages
    // Different stages have different maturation requirements
    float targetProgress = 0.0f;
    
    switch (brain->getDevelopmentSystem()->getStage()) {
        case DevelopmentalStage::Initial:
            // Initial stage: basic neuronal properties form
            targetProgress = std::min(0.3f, pImpl->developmentalTime / 60.0f);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: synaptic strengthening and pruning
            float criticalProgress = std::min(1.0f, (pImpl->developmentalTime - 60.0f) / 300.0f);
            targetProgress = 0.3f + criticalProgress * 0.4f;
            break;
            
        case DevelopmentalStage::Maturation:
            // Maturation: refinement of neuronal circuits
            float maturationProgress = std::min(1.0f, (pImpl->developmentalTime - 360.0f) / 600.0f);
            targetProgress = 0.7f + maturationProgress * 0.3f;
            break;
            
        case DevelopmentalStage::Adult:
            // Adult: maintenance and optimization
            float adultProgress = std::min(1.0f, (pImpl->developmentalTime - 960.0f) / 1800.0f);
            targetProgress = std::min(0.95f, 0.9f + adultProgress * 0.05f);
            break;
            
        case DevelopmentalStage::Aging:
            // Aging: gradual decline in function
            targetProgress = 0.95f;
            // Gradual decrease in function with age
            float ageFactor = std::min(1.0f, pImpl->developmentalTime / 3600.0f);
            targetProgress *= (1.0f - ageFactor * 0.2f);
            break;
    }
    
    // Smooth transition towards target progress
    float progressionRate = 0.01f; // Rate of maturation progress
    pImpl->progress += (targetProgress - pImpl->progress) * progressionRate;
    
    // Update neuronal properties based on maturation progress
    updateNeuronProperties(brain, currentStep);
    
    // Log significant developmental milestones
    if (std::abs(pImpl->progress - targetProgress) < 0.01f) {
        NLM_LOG_INFO("Maturation stage reached: " + std::to_string(static_cast<int>(pImpl->progress * 100)) + "%");
    }
}

void Maturation::updateNeuronProperties(Brain* brain, SimulationStep currentStep) {
    if (!brain) {
        return;
    }
    
    float maturityLevel = pImpl->progress;
    
    // Update all neurons in the brain based on maturation progress
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                if (!neuron) {
                    continue;
                }
                
                // Adjust neuronal properties based on maturation level
                adjustNeuronProperties(neuron, maturityLevel, currentStep);
            }
        }
    }
}

void Maturation::adjustNeuronProperties(Neuron* neuron, float maturity, SimulationStep currentStep) {
    if (!neuron) {
        return;
    }
    
    // Get current neuron state
    const auto& state = neuron->getState();
    
    // Adjust resting potential based on maturation (mature neurons have more stable resting potential)
    float restingPotentialChange = (maturity - 0.5f) * 2.0f * 5.0f; // +/- 10mV range
    neuron->setRestingPotential(state.restingPotential + restingPotentialChange);
    
    // Adjust threshold potential based on maturation (mature neurons have lower threshold variability)
    float thresholdChange = (maturity - 0.5f) * 10.0f; // +/- 10mV range
    neuron->setThreshold(state.threshold + thresholdChange);
    
    // Adjust time constant based on maturation (mature neurons have more stable dynamics)
    float timeConstantChange = (1.0f - maturity) * 10.0f; // Reduced variability with maturity
    neuron->setMembraneTimeConstant(state.tau_mem + timeConstantChange);
    
    // Adjust leak conductance based on maturation
    float leakChange = (maturity - 0.5f) * 0.1f;
    neuron->setLeakConductance(state.leakConductance + leakChange);
    
    // For mature neurons, add some noise reduction
    if (maturity > 0.7f) {
        // Mature neurons have more stable membrane potential
        float stabilization = (maturity - 0.7f) * 0.5f;
        neuron->setNoiseAmplitude(std::max(0.0f, state.noiseAmplitude * (1.0f - stabilization)));
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    // Return threshold that mature neurons converge to
    return -50.0f; // Mature resting threshold
}

MembranePotential Maturation::getMatureRestingPotential() const {
    // Return resting potential that mature neurons converge to
    return -70.0f; // Mature resting potential
}

float Maturation::getMatureTimeConstant() const {
    // Return time constant that mature neurons converge to
    return 20.0f; // Mature time constant (ms)
}

} // namespace nlm
