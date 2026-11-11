#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float maturationRate;
    float neuronalDensity;
    float structuralMaturation;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.01f)
        , neuronalDensity(0.5f)
        , structuralMaturation(0.0f) {}
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
    // TODO PHASE 2: Implement real maturation
    // PLACEHOLDER: Progress increases over simulation time
    // More sophisticated implementation based on biological maturation
    
    if (!brain) return;
    
    // Calculate maturation progress based on current step
    // Maturation is a developmental process that occurs over time
    float timeBasedProgress = static_cast<float>(currentStep) / 10000.0f;  // Normalize to 0-1
    pImpl->progress = std::min(timeBasedProgress, 1.0f);
    
    // Apply maturation effects to neurons
    applyMaturationEffects(brain);
    
    // Update structural maturation based on developmental factors
    updateStructuralMaturation(brain);
    
    // Maturation influences synaptic properties and neuronal characteristics
    matureSynapticProperties(brain);
    matureNeuronalProperties(brain);
}

void Maturation::applyMaturationEffects(Brain* brain) {
    if (!brain) return;
    
    auto regions = brain->getRegions();
    
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            
            for (const auto* neuron : neurons) {
                Neuron* mutableNeuron = const_cast<Neuron*>(neuron);
                
                // Apply maturation effects to neuron properties
                matureMembraneProperties(mutableNeuron);
                maturePlasticityProperties(mutableNeuron);
                matureConnectivity(mutableNeuron, region.get());
            }
        }
    }
}

void Maturation::matureMembraneProperties(Neuron* neuron) {
    if (!neuron) return;
    
    // Gradually move membrane properties towards mature values
    NeuronState& state = neuron->getState();
    
    // Resting potential matures towards mature value
    float maturationFactor = pImpl->maturationRate * pImpl genotype;
    float restingDiff = state.restingPotential - pImpl->matureRestingPotential;
    state.restingPotential -= restingDiff * maturationFactor;
    
    // Threshold matures towards mature threshold
    float thresholdDiff = state.threshold - pImpl->matureThreshold;
    state.threshold -= thresholdDiff * maturationFactor * 0.5f;
    
    // Leak conductance matures
    float matureLeakConductance = 10.0f;
    float leakDiff = state.leakConductance - matureLeakConductance;
    state.leakConductance -= leakDiff * maturationFactor;
}

void Maturation::maturePlasticityProperties(Neuron* neuron) {
    if (!neuron) return;
    
    PlasticityFlags& plasticity = neuron->getPlasticityFlags();n    
    // Maturation affects plasticity parameters
    float maturationFactor = pImpl->maturationRate;
    
    // Hebbian plasticity matures - becomes more efficient
    plasticity.hebbian = std::min(1.0f, plasticity.hebbian + 0.1f * maturationFactor);
    
    // STDP matures - develops more precise timing
    plasticity.stdp = std::min(1.0f, plasticity.stdp + 0.05f * maturationFactor);
    
    // Reward-modulated plasticity matures
    plasticity.reward_modulated = std::min(1.0f, plasticity.reward_modulated + 0.08f * maturationFactor);
}

void Maturation::matureConnectivity(Neuron* neuron, NeuralRegion* region) {
    if (!neuron || !region) return;
    
    float maturationFactor = pImpl->maturationRate;
    
    // Mature neurons have more stable connectivity
    auto incomingSynapses = neuron->getIncomingSynapses();
    auto outgoingSynapses = neuron->getOutgoingSynapses();
    
    for (const auto& synapseHandle : incomingSynapses) {
        // Note: This would require access to the actual synapse object
        // For now, we'll assume the synapse matures through other mechanisms
    }
    
    for (const auto& synapseHandle : outgoingSynapses) {
        // Same note as above
    }
}

void Maturation::updateStructuralMaturation(Brain* brain) {
    if (!brain) return;
    
    // Structural maturation affects brain organization
    float maturationFactor = pImpl->maturationRate;
    
    // Increase structural maturation level
    pImpl->structuralMaturation = std::min(1.0f, pImpl->structuralMaturation + maturationFactor * 0.1f);
    
    // Mature structural properties affect brain function
    // This would include changes to neural circuit organization
    // For simplicity, we'll apply general maturation effects
}

void Maturation::matureSynapticProperties(Brain* brain) {
    if (!brain) return;
    
    auto regions = brain->getRegions();
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        
        for (const auto& syn : synapses) {
            Synapse* mutableSynapse = const_cast<Synapse*>(syn.get());
            
            // Apply maturation effects to synaptic properties
            matureSynapticWeight(mutableSynapse);
            matureSynapticDelay(mutableSynapse);
            matureSynapticEfficacy(mutableSynapse);
        }
    }
}

void Maturation::matureSynapticWeight(Synapse* synapse) {
    if (!synapse) return;
    
    float maturationFactor = pImpl->maturationRate;
    
    // Synaptic weights mature towards more stable values
    float currentWeight = synapse->getWeight();
    
    // Mature weights are less extreme (closer to zero)
    if (std::abs(currentWeight) > 0.3f) {
        float targetWeight = std::copysign(0.3f, currentWeight);
        float weightDiff = targetWeight - currentWeight;
        synapse->setWeight(currentWeight + weightDiff * maturationFactor);
    }
}

void Maturation::matureSynapticDelay(Synapse* synapse) {
    if (!synapse) return;
    
    float maturationFactor = pImpl->maturationRate;
    
    // Synaptic delays mature - become more stable
    Delay currentDelay = synapse->getDelay();
    float targetDelay = 2.0f;  // Mature synapses have ~2ms delay
    
    float delayDiff = targetDelay - static_cast<float>(currentDelay);
    Delay newDelay = static_cast<Delay>(static_cast<float>(currentDelay) + delayDiff * maturationFactor);
    synapse->setDelay(newDelay);
}

void Maturation::matureSynapticEfficacy(Synapse* synapse) {
    if (!synapse) return;
    
    float maturationFactor = pImpl->maturationRate;
    
    // Synaptic efficacy matures - becomes more stable
    float currentEfficacy = synapse->getEfficacy();
    float targetEfficacy = 1.0f;  // Mature synapses have normal efficacy
    
    float efficacyDiff = targetEfficacy - currentEfficacy;
    synapse->setEfficacy(currentEfficacy + efficacyDiff * maturationFactor);
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
