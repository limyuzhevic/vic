#include "Maturation.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    // Activity-dependent maturation state
    float totalSpikeCount;
    float averageFiringRate;
    float synapticComplexity;
    float connectivityPattern;
    
    // Time-dependent maturation
    float timeSinceStart;
    float maturationRate;
    
    // Experience-dependent factors
    float environmentalComplexity;
    float learningEfficiency;
    float neuroplasticity;
    
    // Biological constraints
    static constexpr float MIN_PROGRESS = 0.0f;
    static constexpr float MAX_PROGRESS = 1.0f;
    static constexpr float MATURATION_TIME_CONSTANT = 1000.0f;  // 1000ms maturation time
    static constexpr float MIN_SPIKE_COUNT = 100.0f;
    static constexpr float TARGET_FIRING_RATE = 5.0f;  // Hz
    static constexpr float MIN_SYNAPTIC_COMPLEXITY = 1.0f;
    static constexpr float MAX_SYNAPTIC_COMPLEXITY = 100.0f;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , totalSpikeCount(0.0f)
        , averageFiringRate(0.0f)
        , synapticComplexity(1.0f)
        , connectivityPattern(0.0f)
        , timeSinceStart(0.0f)
        , maturationRate(0.001f)
        , environmentalComplexity(1.0f)
        , learningEfficiency(0.8f)
        , neuroplasticity(0.9f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, Impl::MIN_PROGRESS, Impl::MAX_PROGRESS);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain || !pImpl) return;
    
    // Update time since start
    pImpl->timeSinceStart = currentStep * 0.001f;  // Convert to seconds
    
    // Calculate activity-dependent maturation factors
    float spikeCountFactor = calculateSpikeCountFactor();
    float firingRateFactor = calculateFiringRateFactor();
    float complexityFactor = calculateSynapticComplexityFactor();
    
    // Calculate experience-dependent maturation
    float environmentalFactor = calculateEnvironmentalFactor();
    float learningFactor = calculateLearningFactor();
    
    // Combined maturation rate
    float maturationDrive = calculateMaturationDrive(
        spikeCountFactor,
        firingRateFactor,
        complexityFactor,
        environmentalFactor,
        learningFactor
    );
    
    // Update maturation progress
    pImpl->progress += maturationDrive * pImpl->maturationRate * 0.001f;  // dt = 1ms
    pImpl->progress = std::clamp(pImpl->progress, Impl::MIN_PROGRESS, Impl::MAX_PROGRESS);
    
    // Update derived properties based on maturation progress
    updateNeuronProperties();
    
    // Update brain properties based on maturation
    updateBrainProperties(brain);
}

void Maturation::updateNeuronProperties() {
    if (!pImpl) return;
    
    // Interpolate between immature and mature properties based on progress
    float progress = pImpl->progress;
    
    // Threshold approaches mature threshold
    float immatureThreshold = -50.0f;
    pImpl->matureThreshold = immatureThreshold + (pImpl->matureThreshold - immatureThreshold) * progress;
    
    // Resting potential approaches mature resting potential
    float immatureResting = -60.0f;
    pImpl->matureRestingPotential = immatureResting + (pImpl->matureRestingPotential - immatureResting) * progress;
    
    // Time constant approaches mature time constant
    pImpl->matureTimeConstant = 10.0f + (pImpl->matureTimeConstant - 10.0f) * progress;
    
    // Adaptation properties develop with maturation
    float adaptationBase = 0.1f;
    pImpl->neuroplasticity = adaptationBase + (1.0f - adaptationBase) * progress;
}

void Maturation::updateBrainProperties(Brain* brain) {
    if (!brain || !pImpl) return;
    
    // Update brain properties based on maturation progress
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons == 0) return;
    
    // Calculate average neuron properties based on maturation
    float avgThreshold = -55.0f + (pImpl->progress * 10.0f);  // Threshold becomes more negative with maturation
    float avgResting = -70.0f + (pImpl->progress * 5.0f);    // Resting potential varies more
    
    // Update region properties based on maturation
    auto regionIds = brain->getRegionIds();
    for (RegionId regionId : regionIds) {
        if (auto* region = brain->getRegion(regionId)) {
            // Update synaptic properties in this region
            updateRegionSynapticProperties(region, regionId);
        }
    }
}

void Maturation::updateRegionSynapticProperties(NeuralRegion* region, RegionId regionId) {
    if (!region) return;
    
    // Get all synapses in this region
    auto synapses = region->getSynapses();
    
    for (const auto& syn : synapses) {
        if (syn) {
            // Update synaptic properties based on maturation
            updateSynapseProperties(syn.get());
        }
    }
    
    // Update synaptic connectivity patterns
    updateConnectivityPatterns(region, regionId);
}

void Maturation::updateSynapseProperties(Synapse* synapse) {
    if (!synapse) return;
    
    float progress = pImpl->progress;
    
    // Synaptic strength matures based on activity
    float currentWeight = synapse->getWeight();
    float matureWeight = calculateMatureWeight(synapse);
    
    // Gradual approach to mature weight
    float targetWeight = currentWeight + (matureWeight - currentWeight) * 0.1f * progress;
    synapse->setWeight(targetWeight);
    
    // Synaptic efficacy matures
    float currentEfficacy = synapse->getEfficacy();
    float matureEfficacy = calculateMatureEfficacy(synapse);
    
    float targetEfficacy = currentEfficacy + (matureEfficacy - currentEfficacy) * 0.1f * progress;
    synapse->setEfficacy(targetEfficacy);
    
    // Plasticity flags develop with maturation
    if (progress > 0.5f) {
        synapse->enablePlasticity(true, true, true);  // Full plasticity at 50% maturation
    } else if (progress > 0.2f) {
        synapse->enablePlasticity(true, true, false);  // Partial plasticity at 20% maturation
    }
}

void Maturation::updateConnectivityPatterns(NeuralRegion* region, RegionId regionId) {
    if (!region) return;
    
    // Update connectivity patterns based on maturation
    auto neurons = region->getAllNeurons();
    size_t neuronCount = neurons.size();
    
    if (neuronCount < 2) return;
    
    // Calculate optimal connectivity based on maturation
    float connectivityTarget = calculateTargetConnectivity(neuronCount);
    
    // Update actual connectivity towards target
    float currentConnectivity = calculateCurrentConnectivity(region);
    float connectivityAdjustment = connectivityTarget - currentConnectivity;
    
    // Adjust connectivity based on maturation progress
    if (connectivityAdjustment > 0.01f && pImpl->progress > 0.3f) {
        // Add new connections for maturation
        addMaturationConnections(region, connectivityAdjustment);
    } else if (connectivityAdjustment < -0.01f && pImpl->progress > 0.5f) {
        // Prune connections for refinement
        pruneConnections(region, std::abs(connectivityAdjustment));
    }
}

float Maturation::calculateSpikeCountFactor() const {
    if (!pImpl) return 0.0f;
    
    return std::min(pImpl->totalSpikeCount / Impl::MIN_SPIKE_COUNT, 1.0f);
}

float Maturation::calculateFiringRateFactor() const {
    if (!pImpl) return 0.0f;
    
    return std::min(pImpl->averageFiringRate / Impl::TARGET_FIRING_RATE, 1.0f);
}

float Maturation::calculateSynapticComplexityFactor() const {
    if (!pImpl) return 0.0f;
    
    return (pImpl->synapticComplexity - Impl::MIN_SYNAPTIC_COMPLEXITY) / 
           (Impl::MAX_SYNAPTIC_COMPLEXITY - Impl::MIN_SYNAPTIC_COMPLEXITY);
}

float Maturation::calculateEnvironmentalFactor() const {
    if (!pImpl) return 0.0f;
    
    return pImpl->environmentalComplexity;
}

float Maturation::calculateLearningFactor() const {
    if (!pImpl) return 0.0f;
    
    return pImpl->learningEfficiency;
}

float Maturation::calculateMaturationDrive(float spikeFactor, float firingFactor, 
                                          float complexityFactor, float envFactor, 
                                          float learningFactor) const {
    // Weighted combination of all factors
    float drive = 0.3f * spikeFactor +           // Activity is primary driver
                  0.25f * firingFactor +         // Firing rate important
                  0.15f * complexityFactor +     // Complexity matters
                  0.1f * envFactor +             // Environment influences
                  0.2f * learningFactor;         // Learning efficiency
    
    return std::clamp(drive, 0.0f, 1.0f);
}

float Maturation::calculateMatureWeight(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    float typeFactor = 1.0f;
    if (synapse->isExcitatory()) {
        typeFactor = 1.0f;  // Excitatory: 0.0 to 1.0
    } else if (synapse->isInhibitory()) {
        typeFactor = -0.5f;  // Inhibitory: -0.5 to -1.0
    }
    
    float maturityWeight = typeFactor * (0.2f + 0.8f * pImpl->progress);
    return std::clamp(maturityWeight, -1.0f, 1.0f);
}

float Maturation::calculateMatureEfficacy(Synapse* synapse) const {
    if (!synapse) return 1.0f;
    
    float baseEfficacy = 1.0f;
    float maturityEffect = 0.5f * pImpl->progress;
    
    return baseEfficacy + maturityEffect;
}

float Maturation::calculateTargetConnectivity(size_t neuronCount) const {
    // Optimal connectivity follows small-world network properties
    float baseConnectivity = 0.1f;  // 10% connection probability for immature brain
    float matureConnectivity = 0.3f;  // 30% connection probability for mature brain
    
    float connectivityProgress = std::min(pImpl->progress * 2.0f, 1.0f);
    return baseConnectivity + (matureConnectivity - baseConnectivity) * connectivityProgress;
}

float Maturation::calculateCurrentConnectivity(NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    size_t totalPossible = region->getTotalNeuronCount() * (region->getTotalNeuronCount() - 1);
    if (totalPossible == 0) return 0.0f;
    
    size_t actualConnections = region->getSynapseCount();
    return static_cast<float>(actualConnections) / static_cast<float>(totalPossible);
}

void Maturation::addMaturationConnections(NeuralRegion* region, float adjustment) {
    if (!region || adjustment <= 0.0f) return;
    
    // Add new connections based on maturation requirements
    auto neurons = region->getAllNeurons();
    size_t neuronCount = neurons.size();
    
    // Calculate number of new connections needed
    size_t connectionsToAdd = static_cast<size_t>(neuronCount * neuronCount * adjustment * 0.1f);
    
    for (size_t i = 0; i < connectionsToAdd && i < neurons.size(); ++i) {
        for (size_t j = i + 1; j < neurons.size() && connectionsToAdd > i; ++j) {
            Neuron* preNeuron = neurons[i];
            Neuron* postNeuron = neurons[j];
            
            if (preNeuron && postNeuron && preNeuron != postNeuron) {
                // Create new synaptic connection
                float weight = calculateMaturationConnectionWeight(preNeuron, postNeuron);
                Delay delay = calculateMaturationConnectionDelay(preNeuron, postNeuron);
                
                SynapseId synId = region->addSynapse(preNeuron->getId(), postNeuron->getId(), weight, delay);
                
                if (Synapse* syn = region->getSynapse(synId)) {
                    // Set synapse type based on neuron types
                    if (preNeuron->getType() == NeuronType::Excitatory || 
                        preNeuron->getType() == NeuronType::Sensory) {
                        syn->setType(SynapseType::Excitatory);
                    } else if (preNeuron->getType() == NeuronType::Inhibitory) {
                        syn->setType(SynapseType::Inhibitory);
                    } else {
                        syn->setType(SynapseType::Excitatory);
                    }
                    
                    // Initialize random properties
                    // Note: We need a RandomGenerator instance here
                    // For now, we'll use default initialization
                    syn->setWeight(weight);
                    syn->setDelay(delay);
                    syn->setEfficacy(1.0f);
                }
            }
        }
    }
}

void Maturation::pruneConnections(NeuralRegion* region, float adjustment) {
    if (!region || adjustment <= 0.0f) return;
    
    // Remove redundant or weak connections
    auto neurons = region->getAllNeurons();
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = i + 1; j < neurons.size(); ++j) {
            Neuron* preNeuron = neurons[i];
            Neuron* postNeuron = neurons[j];
            
            if (!preNeuron || !postNeuron) continue;
            
            // Find the synapse between these neurons
            auto preSynapses = region->getSynapsesFrom(preNeuron->getId());
            for (Synapse* syn : preSynapses) {
                if (syn && syn->getDestinationNeuron() == postNeuron->getId()) {
                    // Determine if this connection should be pruned
                    if (shouldPruneConnection(syn, preNeuron, postNeuron, adjustment)) {
                        // Remove the synapse
                        region->removeSynapse(syn->getId());
                        
                        // Update neuron connectivity maps
                        preNeuron->removeOutgoingSynapse(syn->getId());
                        postNeuron->removeIncomingSynapse(syn->getId());
                    }
                }
            }
        }
    }
}

float Maturation::calculateMaturationConnectionWeight(Neuron* preNeuron, Neuron* postNeuron) const {
    if (!preNeuron || !postNeuron) return 0.0f;
    
    float baseWeight = 0.1f + 0.3f * static_cast<float>(rand()) / RAND_MAX;
    
    // Modulate by neuron types
    if (preNeuron->getType() == NeuronType::Inhibitory) {
        baseWeight *= 0.5f;  // Inhibitory connections weaker
    }
    
    return std::clamp(baseWeight, -1.0f, 1.0f);
}

Delay Maturation::calculateMaturationConnectionDelay(Neuron* preNeuron, Neuron* postNeuron) const {
    if (!preNeuron || !postNeuron) return 1;
    
    // Distance-based delay
    Delay baseDelay = static_cast<Delay>(std::abs(
        static_cast<int>(preNeuron->getId().index()) - 
        static_cast<int>(postNeuron->getId().index())
    ));
    
    return std::max(1, static_cast<int>(baseDelay * 0.5f + 1.0f));  // Minimum delay of 1
}

bool Maturation::shouldPruneConnection(Synapse* synapse, Neuron* preNeuron, Neuron* postNeuron, float adjustment) const {
    if (!synapse || !preNeuron || !postNeuron) return true;
    
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    float matureProgress = pImpl->progress;
    
    // Prune weak or redundant connections
    if (std::abs(weight) < 0.05f) return true;  // Very weak connections
    
    if (efficacy < 0.5f && matureProgress > 0.7f) return true;  // Ineffective connections in mature brain
    
    // If mature, prune based on usage patterns
    if (matureProgress > 0.8f) {
        // Check if connection contributes to network function
        // For now, prune based on weight and efficacy
        if (std::abs(weight) < 0.1f && efficacy < 0.8f) {
            return true;
        }
    }
    
    return false;
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl ? pImpl->matureThreshold : -55.0f;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl ? pImpl->matureRestingPotential : -70.0f;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl ? pImpl->matureTimeConstant : 20.0f;
}

void Maturation::updateTotalSpikeCount(float count) {
    if (pImpl) {
        pImpl->totalSpikeCount += count;
    }
}

void Maturation::updateAverageFiringRate(float rate) {
    if (pImpl) {
        pImpl->averageFiringRate = (pImpl->averageFiringRate + rate) * 0.5f;  // Running average
    }
}

void Maturation::updateSynapticComplexity(float complexity) {
    if (pImpl) {
        pImpl->synapticComplexity = std::clamp(complexity, Impl::MIN_SYNAPTIC_COMPLEXITY, Impl::MAX_SYNAPTIC_COMPLEXITY);
    }
}

void Maturation::setEnvironmentalComplexity(float complexity) {
    if (pImpl) {
        pImpl->environmentalComplexity = std::clamp(complexity, 0.1f, 2.0f);
    }
}

void Maturation::setLearningEfficiency(float efficiency) {
    if (pImpl) {
        pImpl->learningEfficiency = std::clamp(efficiency, 0.1f, 1.0f);
    }
}

void Maturation::setNeuroplasticity(float neuroplasticity) {
    if (pImpl) {
        pImpl->neuroplasticity = std::clamp(neuroplasticity, 0.1f, 1.0f);
    }
}

} // namespace nlm
