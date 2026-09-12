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
    // Real maturation: biological brain development process
    // Implements developmental stages with synaptic refinement and myelination
    // Based on activity-dependent pruning and connectivity optimization
    
    if (!brain) return;
    
    // Calculate maturation progress based on simulation time
    // Different developmental stages have different maturation rates
    float maturationRate = 0.001f;  // Base maturation rate
    
    // Developmental stage specific maturation
    switch (brain->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            // Rapid synaptogenesis and initial connectivity
            maturationRate = 0.002f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for sensory refinement
            maturationRate = 0.0015f;
            break;
        case DevelopmentalStage::Maturation:
            // Synaptic pruning and myelination
            maturationRate = 0.0008f;
            break;
        case DevelopmentalStage::Adult:
            // Maintenance and optimization
            maturationRate = 0.0003f;
            break;
        case DevelopmentalStage::Aging:
            // Gradual decline in plasticity
            maturationRate = 0.0005f;
            break;
    }
    
    // Update progress
    pImpl->progress += maturationRate * currentStep;
    pImpl->progress = std::clamp(pImpl->progress, 0.0f, 1.0f);
    
    // Apply stage-specific maturation effects
    if (brain->getDevelopmentalStage() == DevelopmentalStage::Maturation) {
        // Synaptic pruning based on Hebbian rule
        // "Neurons that fire together, wire together; otherwise, disconnect"
        // Implement activity-dependent synapse elimination
        performSynapticPruning(brain, currentStep);
        
        // Myelination increases signal conduction speed
        increaseConductionVelocity(brain, currentStep);
    }
    
    // Network optimization in adult stage
    if (brain->getDevelopmentalStage() == DevelopmentalStage::Adult) {
        optimizeNetworkTopology(brain, currentStep);
        stabilizePlasticity(brain, currentStep);
    }
    
    // Age-related changes
    if (brain->getDevelopmentalStage() == DevelopmentalStage::Aging) {
        reducePlasticity(brain, currentStep);
        increaseNeuralNoise(brain, currentStep);
    }
}

void Maturation::performSynapticPruning(Brain* brain, SimulationStep currentStep) {
    // Activity-dependent synaptic pruning
    // Remove weak or redundant connections based on Hebbian principles
    if (!brain) return;
    
    // For each region, identify synapses with low activity correlation
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        // Get all synapses in region
        auto& synapses = region->getSynapses();
        
        // Calculate activity correlations for each synapse
        std::vector<std::pair<Synapse*, float>> weakSynapses;
        
        for (auto& syn : synapses) {
            // In real implementation, calculate correlation between
            // pre-synaptic and post-synaptic firing patterns
            float activityCorrelation = calculateFiringCorrelation(syn, currentStep);
            
            // Weak synapses have low activity correlation
            if (activityCorrelation < 0.2f) {
                weakSynapses.push_back(std::make_pair(syn.get(), activityCorrelation));
            }
        }
        
        // Remove weakest synapses (up to 5% per step)
        size_t synapsesToRemove = std::min<size_t>(weakSynapses.size() / 20, 10);
        
        // Sort by activity correlation (weakest first)
        std::sort(weakSynapses.begin(), weakSynapses.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });
        
        for (size_t i = 0; i < synapsesToRemove && i < weakSynapses.size(); ++i) {
            auto* synToRemove = weakSynapses[i].first;
            if (synToRemove) {
                // Remove synapse from brain
                region->removeSynapse(synToRemove->getId());
            }
        }
    }
}

void Maturation::increaseConductionVelocity(Brain* brain, SimulationStep currentStep) {
    // Simulate myelination process
    // Increases action potential conduction velocity
    if (!brain) return;
    
    // Myelination affects all neurons, but especially those in active pathways
    float myelinationRate = 0.0001f * currentStep;
    
    // Update neuronal parameters for faster conduction
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron) {
                    // Increase leak conductance (myelination effect)
                    float leakConductance = neuron->getLeakConductance();
                    leakConductance *= (1.0f + myelinationRate);
                    neuron->setLeakConductance(leakConductance);
                    
                    // Decrease membrane time constant
                    // This is a simplified model of myelination effects
                    float threshold = neuron->getThreshold();
                    threshold += 0.1f * myelinationRate;  // Slightly higher threshold
                    neuron->setThreshold(threshold);
                }
            }
        }
    }
}

void Maturation::optimizeNetworkTopology(Brain* brain, SimulationStep currentStep) {
    // Network optimization in adult brain
    // Rewires connections for efficiency
    if (!brain) return;
    
    // Optimize regional connectivity
    float optimizationRate = 0.0002f * currentStep;
    
    // For each pair of connected regions, optimize connection strength
    for (const auto& sourceId : brain->getRegionIds()) {
        for (const auto& targetId : brain->getRegionIds()) {
            if (sourceId == targetId) continue;
            
            // Check if regions are connected
            auto* source = brain->getRegion(sourceId);
            auto* target = brain->getRegion(targetId);
            
            if (!source || !target) continue;
            
            // Calculate connection efficiency
            float connectionEfficiency = calculateRegionConnectivity(sourceId, targetId, brain);
            
            // Optimize based on efficiency
            if (connectionEfficiency > 0.8f) {
                // Strong connection, maintain
                float newWeight = std::min(1.0f, 1.0f + optimizationRate);
                brain->addInterRegionConnection(sourceId, targetId, newWeight);
            } else if (connectionEfficiency < 0.3f) {
                // Weak connection, potentially prune
                if (rng()->uniformReal(0.0f, 1.0f) < 0.1f) {
                    // Remove connection with 10% probability
                    brain->removeInterRegionConnection(sourceId, targetId);
                }
            }
        }
    }
}

void Maturation::stabilizePlasticity(Brain* brain, SimulationStep currentStep) {
    // Stabilize plasticity mechanisms in adult brain
    // Reduce overall plasticity for stability
    if (!brain) return;
    
    float plasticityReduction = 0.0005f * currentStep;
    
    // Apply to all plasticity rules
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structural = brain->getStructuralPlasticity();
    
    if (stdp) {
        float currentLTP = stdp->getLTPWeight();
        stdp->setLTPWeight(std::max(0.001f, currentLTP - plasticityReduction));
    }
    
    if (hebbian) {
        float currentLR = hebbian->getLearningRate();
        hebbian->setLearningRate(std::max(0.0001f, currentLR - plasticityReduction));
    }
    
    if (structural) {
        float currentRate = structural->getSynaptogenesisRate();
        structural->setSynaptogenesisRate(std::max(0.00001f, currentRate - plasticityReduction));
    }
}

void Maturation::reducePlasticity(Brain* brain, SimulationStep currentStep) {
    // Age-related plasticity reduction
    float agePlasticityReduction = 0.001f * currentStep;
    
    // Reduce plasticity across all systems
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structural = brain->getStructuralPlasticity();
    
    if (stdp) {
        float currentLTP = stdp->getLTPWeight();
        stdp->setLTPWeight(std::max(0.0001f, currentLTP - agePlasticityReduction));
    }
    
    if (hebbian) {
        float currentLR = hebbian->getLearningRate();
        hebbian->setLearningRate(std::max(0.00001f, currentLR - agePlasticityReduction));
    }
    
    if (structural) {
        float currentRate = structural->getSynaptogenesisRate();
        structural->setSynaptogenesisRate(std::max(0.000001f, currentRate - agePlasticityReduction));
    }
}

void Maturation::increaseNeuralNoise(Brain* brain, SimulationStep currentStep) {
    // Increase neural noise with age
    // Models age-related neural instability
    if (!brain) return;
    
    float noiseIncreaseRate = 0.0002f * currentStep;
    
    // Apply increased noise to neurons
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron) {
                    // Increase baseline membrane potential noise
                    float currentNoise = neuron->getState().adaptationVariable;
                    neuron->getState().adaptationVariable += noiseIncreaseRate;
                    
                    // Clamp noise to reasonable range
                    if (neuron->getState().adaptationVariable > 0.5f) {
                        neuron->getState().adaptationVariable = 0.5f;
                    }
                }
            }
        }
    }
}

// Helper methods implementation
float Maturation::calculateFiringCorrelation(Synapse* synapse, SimulationStep currentStep) {
    if (!synapse) return 0.0f;
    
    // Calculate correlation between pre and post synaptic firing
    // In real implementation, use spike history and neural activity patterns
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simple correlation based on spike timing proximity
    float correlation = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(preTime - postTime));
            if (timeDiff < 10.0f) {  // Within 10ms window
                correlation += 1.0f / (1.0f + timeDiff / 10.0f);
            }
        }
    }
    
    return correlation / static_cast<float>(preSpikes.size() * postSpikes.size());
}

RandomGenerator* Maturation::rng() {
    // Get random generator from global or brain config
    // This would typically access a global random number generator
    // or a random generator passed from the brain
    return nullptr;  // TODO: Implement proper random generator access
}

float Maturation::calculateRegionConnectivity(RegionId sourceId, RegionId targetId, Brain* brain) {
    if (!brain) return 0.0f;
    
    auto* source = brain->getRegion(sourceId);
    auto* target = brain->getRegion(targetId);
    
    if (!source || !target) return 0.0f;
    
    // Calculate connectivity based on number of inter-region synapses
    float connectionCount = 0.0f;
    
    // In real implementation, count actual synapses between regions
    // For now, return a placeholder value
    
    return 0.5f;  // Placeholder
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
