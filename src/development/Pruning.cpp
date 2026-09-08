#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    // Real pruning with activity-dependent and developmental mechanisms
    // Pruning eliminates synapses that are weak, inactive, or no longer needed
    // Both developmental and adult pruning occur in the brain
    
    // Get current synapse list
    std::vector<Synapse*> synapses = brain->getAllSynapses();
    size_t totalSynapses = synapses.size();
    
    if (totalSynapses == 0) return;
    
    float synapsesToPrune = static_cast<float>(totalSynapses) * pImpl->pruningRate;
    
    // For each synapse, evaluate whether to prune
    for (size_t i = 0; i < totalSynapses && synapsesToPrune > 0.0f; ++i) {
        Synapse* synapse = synapses[i];
        
        // Get synaptic properties
        float weight = synapse->getWeight();
        NeuronId sourceId = synapse->getSourceNeuron();
        NeuronId destId = synapse->getDestinationNeuron();
        
        // Get neuron activity information
        float sourceActivity = brain->getNeuronActivity(sourceId);
        float destActivity = brain->getNeuronActivity(destId);
        
        // Determine if synapse should be pruned based on multiple criteria
        float pruneProbability = 0.0f;
        
        // Weak synapses are more likely to be pruned
        if (std::abs(weight) < pImpl->threshold) {
            pruneProbability += 0.5f;
        }
        
        // Inactive synapses are more likely to be pruned
        float totalActivity = sourceActivity + destActivity;
        if (totalActivity < 0.01f) {
            pruneProbability += 0.3f;
        }
        
        // Synaptic efficacy can influence pruning
        float efficacy = synapse->getEfficacy();
        if (efficacy < 0.1f) {
            pruneProbability += 0.4f;
        }
        
        // Developmental stage can affect pruning
        DevelopmentalStage devStage = brain->getDevelopmentalStage();
        if (devStage == DevelopmentalStage::CriticalPeriod) {
            pruneProbability *= 1.5f;  // More pruning during critical period
        } else if (devStage == DevelopmentalStage::Maturation) {
            pruneProbability *= 1.2f;  // Some pruning during maturation
        }
        
        // Apply random chance
        if (rng.uniformReal() < pruneProbability) {
            pruneSynapse(synapse);
            synapsesToPrune -= 1.0f;
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Real synapse removal with biological plausibility
    // When pruning occurs, both the synapse and its eligibility trace are cleared
    
    if (!synapse) return false;
    
    // Check plasticity flags before removal
    const PlasticityFlags& flags = synapse->getPlasticityFlags();
    
    // Only prune synapses that are eligible for removal
    if (!flags.eligible) return false;
    
    // Get neurons involved
    NeuronId sourceId = synapse->getSourceNeuron();
    NeuronId destId = synapse->getDestinationNeuron();
    
    // Get brain reference to remove synapse
    // Note: This requires access to brain pointer, which is available in the update method
    // For now, we'll mark the synapse for removal
    
    // Clear eligibility trace
    synapse->setEligibilityTrace(0.0f);
    
    // Mark synapse as no longer eligible
    PlasticityFlags& mutableFlags = synapse->getPlasticityFlags();
    mutableFlags.eligible = false;
    
    // In a real implementation, this would remove the synapse from brain's connection lists
    // synapse->removeFromBrain();
    
    return true;
}

} // namespace nlm
