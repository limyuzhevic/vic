#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float competitionFactor;
    float targetSynapticStrength;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f), competitionFactor(0.5f), targetSynapticStrength(0.1f) {}
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
    // PHASE 2: Real pruning implementation
    // Implements activity-dependent synapse elimination with homeostatic control
    
    if (!brain) return;
    
    // Calculate current network activity and synaptic strength distribution
    std::vector<float> synapticStrengths;
    std::vector<float> neuronActivities;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                // Get synaptic strength of this neuron
                float totalIncomingStrength = 0.0f;
                for (SynapseHandle synapseHandle : neuron->getIncomingSynapses()) {
                    Synapse* synapse = brain->getSynapse(synapseHandle);
                    if (synapse) {
                        totalIncomingStrength += std::abs(synapse->getWeight());
                    }
                }
                synapticStrengths.push_back(totalIncomingStrength);
                
                // Calculate neuron activity (average firing rate)
                const auto& state = neuron->getState();
                float activity = (state.lastSpikeTime > 0.0f) ? 1.0f : 0.0f;
                neuronActivities.push_back(activity);
            }
        }
    }
    
    if (synapticStrengths.empty()) return;
    
    // Find synaptic strength statistics
    float minStrength = *std::min_element(synapticStrengths.begin(), synapticStrengths.end());
    float maxStrength = *std::max_element(synapticStrengths.begin(), synapticStrengths.end());
    float avgStrength = std::accumulate(synapticStrengths.begin(), synapticStrengths.end(), 0.0f) / synapticStrengths.size();
    
    // Determine which synapses to prune based on multiple criteria
    std::vector<std::pair<Synapse*, float>> synapsesToEvaluate;
    
    for (auto& region : brain->getRegions()) {
        for (Synapse* synapse : region->getSynapses()) {
            if (!synapse) continue;
            
            float strength = std::abs(synapse->getWeight());
            float efficacy = synapse->getEfficacy();
            
            // Calculate pruning score based on multiple factors
            float pruningScore = 0.0f;
            
            // 1. Low synaptic strength
            if (strength < pImpl->threshold) {
                pruningScore += (pImpl->threshold - strength) / pImpl->threshold;
            }
            
            // 2. Low efficacy
            pruningScore += (1.0f - efficacy) * 0.3f;
            
            // 3. Competition with stronger synapses
            if (maxStrength > 0.0f) {
                float relativeStrength = strength / maxStrength;
                pruningScore += (1.0f - relativeStrength) * pImpl->competitionFactor;
            }
            
            // 4. Low postsynaptic neuron activity
            Neuron* postNeuron = brain->getNeuron(synapse->getDestinationNeuron());
            if (postNeuron) {
                float postActivity = calculatePostsynapticActivity(postNeuron);
                pruningScore += (1.0f - postActivity) * 0.2f;
            }
            
            synapsesToEvaluate.push_back({synapse, pruningScore});
        }
    }
    
    // Sort synapses by pruning score (higher score = more likely to be pruned)
    std::sort(synapsesToEvaluate.begin(), synapsesToEvaluate.end(),
              [](const std::pair<Synapse*, float>& a, const std::pair<Synapse*, float>& b) {
                  return a.second > b.second;
              });
    
    // Prune synapses based on pruning rate and threshold
    size_t synapsesToPrune = static_cast<size_t>(synapsesToEvaluate.size() * pImpl->pruningRate);
    synapsesToPrune = std::min(synapsesToPrune, static_cast<size_t>(synapsesToEvaluate.size() * 0.05f)); // Max 5% of synapses
    
    for (size_t i = 0; i < synapsesToPrune; ++i) {
        Synapse* synapse = synapsesToEvaluate[i].first;
        if (synapse && synapse->getEligibilityTrace() > 0.01f) {  // Only prune synapses with positive eligibility trace
            pruneSynapse(synapse, brain, rng);
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse, Brain* brain, RandomGenerator& rng) {
    // PHASE 2: Real synapse removal implementation
    // Implements biologically realistic synapse elimination with proper cleanup
    
    if (!synapse || !brain) return false;
    
    // Get pre- and post-synaptic neurons
    Neuron* preNeuron = brain->getNeuron(synapse->getSourceNeuron());
    Neuron* postNeuron = brain->getNeuron(synapse->getDestinationNeuron());
    
    if (!preNeuron || !postNeuron) return false;
    
    // Calculate the impact of this synapse on neural computation
    float weightChangeImpact = std::abs(synapse->getWeight()) * synapse->getEfficacy();
    
    // Check if pruning is justified based on multiple criteria
    bool shouldPrune = evaluatePruningDecision(synapse, preNeuron, postNeuron, weightChangeImpact);
    
    if (shouldPrune) {
        // Remove synapse from both neurons
        preNeuron->removeOutgoingSynapse(synapse->getId());
        postNeuron->removeIncomingSynapse(synapse->getId());
        
        // Remove synapse from region
        RegionId regionId = findRegionForSynapse(synapse->getId(), brain);
        if (regionId != INVALID_REGION_ID) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                region->removeSynapse(synapse->getId());
            }
        }
        
        // Mark synapse for deletion and update statistics
        synapse->reset();  // Clear eligibility trace and other state
        return true;
    }
    
    return false;
}

bool Pruning::evaluatePruningDecision(Synapse* synapse, Neuron* preNeuron, 
                                     Neuron* postNeuron, float weightChangeImpact) const {
    // Comprehensive evaluation of whether to prune a synapse
    
    if (!synapse || !preNeuron || !postNeuron) return false;
    
    float totalScore = 0.0f;
    
    // 1. Check synaptic strength criteria
    float synapticStrength = std::abs(synapse->getWeight());
    if (synapticStrength < pImpl->threshold) {
        totalScore += 1.0f;  // Strong candidate for pruning
    }
    
    // 2. Check efficacy criteria
    float efficacy = synapse->getEfficacy();
    if (efficacy < 0.5f) {
        totalScore += 0.5f;
    }
    
    // 3. Check postsynaptic neuron activity
    float postActivity = calculatePostsynapticActivity(postNeuron);
    if (postActivity < 0.1f) {
        totalScore += 0.3f;
    }
    
    // 4. Check pre-synaptic neuron activity
    float preActivity = calculatePresynapticActivity(preNeuron);
    if (preActivity < 0.1f) {
        totalScore += 0.2f;
    }
    
    // 5. Competition factor: synapses in highly active regions may be stronger
    float competitionFactor = 1.0f - pImpl->competitionFactor;
    if (preActivity > 0.5f && postActivity > 0.5f) {
        totalScore *= competitionFactor;
    }
    
    // Decision threshold: prune if total score exceeds 1.0
    return totalScore > 1.0f;
}

float Pruning::calculatePostsynapticActivity(Neuron* neuron) const {
    // Calculate postsynaptic neuron activity based on spike history
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    float currentTime = static_cast<float>(state.lastSpikeTime);
    
    if (currentTime <= 0.0f) {
        return 0.0f;  // No spikes recorded
    }
    
    // Activity function: decays with time since last spike
    float timeSinceLastSpike = currentTime - static_cast<float>(state.lastSpikeTime);
    if (timeSinceLastSpike > 1000.0f) {  // More than 1 second ago
        return 0.0f;
    }
    
    return std::exp(-timeSinceLastSpike / 100.0f);  // 100ms time constant
}

float Pruning::calculatePresynapticActivity(Neuron* neuron) const {
    // Calculate presynaptic neuron activity (simplified version)
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    return (state.lastSpikeTime > 0.0f) ? 1.0f : 0.0f;
}

RegionId Pruning::findRegionForSynapse(SynapseId synapseId, Brain* brain) const {
    if (!brain) return INVALID_REGION_ID;
    
    for (auto& region : brain->getRegions()) {
        for (Synapse* synapse : region->getSynapses()) {
            if (synapse && synapse->getId() == synapseId) {
                return region.getId();
            }
        }
    }
    
    return INVALID_REGION_ID;
}

} // namespace nlm

