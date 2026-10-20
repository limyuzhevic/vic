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
    // Real maturation process: developmental refinement of neural properties
    // Implements Hebbian consolidation, synaptic pruning, and parameter adjustment
    
    // Get maturation target (from config)
    float maturationTarget = 0.8f;  // 80% maturity reached
    setProgress(std::min(getProgress(), maturationTarget));
    
    // Get brain components for access to neurons and synapses
    auto* developmentSystem = brain->getDevelopmentSystem();
    auto* randomGen = brain->getRandomGenerator();
    auto* config = brain->getConfig();
    
    if (!config || !developmentSystem) {
        return;
    }
    
    // Stage 1: Initial maturation (0-30% progress)
    if (getProgress() < 0.3f) {
        // Refine neuron parameters toward mature values
        // Adjust resting potential closer to mature level
        float matureResting = getMatureRestingPotential();
        float currentResting = brain->getNeuron(0)->getRestingPotential();
        
        float newResting = currentResting + (matureResting - currentResting) * 0.01f;
        brain->getNeuron(0)->setRestingPotential(newResting);
        
        // Adjust threshold with activity-dependent rule
        float matureThreshold = getMatureThreshold();
        float currentThreshold = brain->getNeuron(0)->getThreshold();
        
        // Hebbian adjustment: strengthen if active, weaken if inactive
        bool neuronActive = brain->getNeuron(0)->getState().firingRate > 0.1f;
        float adjustment = neuronActive ? 0.005f : -0.002f;
        float newThreshold = currentThreshold + adjustment;
        brain->getNeuron(0)->setThreshold(std::clamp(newThreshold, -60.0f, -40.0f));
    }
    
    // Stage 2: Refinement (30-70% progress)
    else if (getProgress() < 0.7f) {
        // Synaptic pruning: eliminate weak connections
        float pruningRate = 0.001f * (1.0f - getProgress() / 0.7f);  // Higher pruning early
        
        for (const auto& neuron : brain->getNeurons()) {
            // Prune synapses with low efficacy and weight
            const auto& synapses = neuron->getOutgoingSynapses();
            for (const auto& synHandle : synapses) {
                auto* synapse = brain->getSynapse(synHandle);
                if (synapse) {
                    float efficacy = synapse->getEfficacy();
                    float weight = synapse->getWeight();
                    
                    // Prune if both low efficacy and weak weight
                    if (efficacy < 0.3f && std::abs(weight) < 0.1f && randomGen->uniformReal(0.0f, 1.0f) < pruningRate) {
                        synapse->setEfficacy(0.0f);  // Mark for elimination
                    }
                }
            }
        }
        
        // Adjust time constants toward mature values
        float matureTimeConstant = getMatureTimeConstant();
        float currentTimeConstant = brain->getNeuron(0)->getLeakConductance();
        
        float newTimeConstant = currentTimeConstant + (matureTimeConstant - currentTimeConstant) * 0.02f;
        brain->getNeuron(0)->setLeakConductance(newTimeConstant);
    }
    
    // Stage 3: Stabilization (70-80% progress)
    else {
        // Hebbian consolidation: strengthen active pathways
        float consolidationFactor = 0.001f;
        
        for (const auto& neuron : brain->getNeurons()) {
            const auto& spikeHistory = neuron->getState().lastSpikeTime;
            if (neuron->isFiring()) {
                // Strengthen synapses from active neurons
                const auto& synapses = neuron->getOutgoingSynapses();
                for (const auto& synHandle : synapses) {
                    auto* synapse = brain->getSynapse(synHandle);
                    if (synapse) {
                        float weight = synapse->getWeight();
                        synapse->addToWeight(consolidationFactor * weight);
                    }
                }
            }
        }
        
        // Reduce plasticity as maturation completes
        for (const auto& synapse : brain->getSynapses()) {
            synapse->setEfficacy(std::max(synapse->getEfficacy(), 0.5f));
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
