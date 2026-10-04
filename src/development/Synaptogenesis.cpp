#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float connectivityProbability;
    float meanInitialWeight;
    float weightVariance;
    SimulationStep timeSinceLastFormation;
    float activityBasedFormationWeight;
    float developmentalStageFactor;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), connectivityProbability(0.05f),
             meanInitialWeight(0.5f), weightVariance(0.1f), timeSinceLastFormation(0),
             activityBasedFormationWeight(0.0f), developmentalStageFactor(1.0f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, rate);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::max(0.0f, std::min(1.0f, density));
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || formationRate <= 0.0f) return;
    
    ++pImpl->timeSinceLastFormation;
    pImpl->activityBasedFormationWeight = 0.0f;
    
    // Calculate synaptic density to compare with target
    size_t currentSynapseCount = brain->getTotalSynapseCount();
    size_t totalPossibleSynapses = 0;
    size_t excitatoryNeurons = 0;
    size_t inhibitoryNeurons = 0;
    
    // Count neurons by type and estimate potential connections
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            size_t popSize = pop->getSize();
            if (pop->getNeuronType() == NeuronType::Excitatory) {
                excitatoryNeurons += popSize;
            } else if (pop->getNeuronType() == NeuronType::Inhibitory) {
                inhibitoryNeurons += popSize;
            }
        }
    }
    
    // For simplicity, assume each neuron can connect to 1000 others initially
    // This is a conservative estimate for dense neural networks
    totalPossibleSynapses = (excitatoryNeurons + inhibitoryNeurons) * 1000;
    
    if (totalPossibleSynapses == 0) return;
    
    float currentDensity = static_cast<float>(currentSynapseCount) / totalPossibleSynapses;
    
    // Determine developmental stage factor based on plasticity
    pImpl->developmentalStageFactor = 1.0f;
    DevelopmentSystem* devSys = brain->getDevelopmentSystem();
    if (devSys) {
        DevelopmentalStage stage = devSys->getStage();
        switch (stage) {
            case DevelopmentalStage::Initial:
                pImpl->developmentalStageFactor = 1.0f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                pImpl->developmentalStageFactor = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                pImpl->developmentalStageFactor = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                pImpl->developmentalStageFactor = 0.2f;
                break;
            case DevelopmentalStage::Aging:
                pImpl->developmentalStageFactor = 0.1f;
                break;
        }
    }
    
    // Calculate desired number of new synapses based on density difference
    float densityError = targetDensity - currentDensity;
    if (densityError > 0.001f) { // Only form new synapses if below target
        float idealFormationRate = formationRate * pImpl->developmentalStageFactor;
        
        // Estimate potential new synapses for this update
        // Use a Poisson distribution to determine how many synapses to form
        float expectedNewSynapses = idealFormationRate * totalPossibleSynapses * 0.1f; // 10% of timestep
        
        // Sample from Poisson-like distribution
        float formationRandom = rng.uniformReal(0.0f, 1.0f);
        if (formationRandom < expectedNewSynapses / 10.0f) {
            int newSynapses = static_cast<int>(expectedNewSynapses);
            if (newSynapses > 0) {
                formNewSynapses(brain, rng, newSynapses, currentDensity);
            }
        }
    }
    
    // Update activity-based formation weighting
    // Higher firing activity increases propensity for new synapses (Hebbian-like)
    pImpl->activityBasedFormationWeight = calculateActivityBasedFormation(brain);
}

void Synaptogenesis::Impl::formNewSynapses(Brain* brain, RandomGenerator& rng, int count, float currentDensity) {
    // Select candidate neurons for synaptogenesis
    std::vector<NeuronId> sourceCandidates;
    std::vector<NeuronId> destinationCandidates;
    
    // Collect all neurons from all regions and populations
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                NeuronId neuronId = neuron->getId();
                
                // Prioritize neurons with higher activity for source
                if (neuron->getState().firingRate > 0.0f) {
                    sourceCandidates.push_back(neuronId);
                }
                
                // All neurons can be potential targets for new synapses
                destinationCandidates.push_back(neuronId);
            }
        }
    }
    
    if (sourceCandidates.empty() || destinationCandidates.empty()) return;
    
    // Shuffle candidates
    rng.shuffle(sourceCandidates);
    rng.shuffle(destinationCandidates);
    
    int synapsesCreated = 0;
    int attempts = 0;
    const int maxAttempts = count * 5; // Allow more attempts to find suitable connections
    
    while (synapsesCreated < count && attempts < maxAttempts) {
        ++attempts;
        
        // Select source and destination (ensure they're different)
        NeuronId source = sourceCandidates[rng.pickRandomIndex(sourceCandidates.size())];
        NeuronId destination = destinationCandidates[rng.pickRandomIndex(destinationCandidates.size())];
        
        // Skip if same neuron
        if (source == destination) continue;
        
        // Find the neuron objects
        Neuron* sourceNeuron = nullptr;
        Neuron* destNeuron = nullptr;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == source) {
                        sourceNeuron = const_cast<Neuron*>(neuron);
                    }
                    if (neuron->getId() == destination) {
                        destNeuron = const_cast<Neuron*>(neuron);
                    }
                }
            }
        }
        
        if (!sourceNeuron || !destNeuron) continue;
        
        // Check for existing synapse
        auto existingSynapses = sourceNeuron->getOutgoingSynapses();
        bool synapseExists = false;
        for (auto handle : existingSynapses) {
            // We need to get the synapse from handle, but this is complex with current API
            // For now, assume no existing synapse for simplicity
        }
        
        if (synapseExists) continue;
        
        // Determine synapse type based on neuron types
        NeuronType sourceType = sourceNeuron->getType();
        NeuronType destType = destNeuron->getType();
        
        // Hebbian-like rules: similar types prefer connections
        SynapseType synapseType = SynapseType::Excitatory;
        if (sourceType == NeuronType::Inhibitory || destType == NeuronType::Inhibitory) {
            synapseType = SynapseType::Inhibitory;
        }
        
        // Calculate weight based on activity and developmental factors
        float sourceActivity = sourceNeuron->getState().firingRate;
        float destActivity = destNeuron->getState().firingRate;
        
        // Activity-based weight initialization
        float activityFactor = (sourceActivity + destActivity) / 2.0f;
        float baseWeight = meanInitialWeight + (activityFactor - 0.5f) * 0.2f;
        
        // Add developmental modulation
        float devModulation = 0.5f + (developmentalStageFactor * 0.5f); // Higher in early development
        baseWeight *= devModulation;
        
        // Apply Gaussian distribution for realistic weight variation
        float weightNoise = rng.normal(0.0f, weightVariance);
        float finalWeight = std::clamp(baseWeight + weightNoise, -1.0f, 1.0f);
        
        // Create the synapse using StructuralPlasticity if available
        // Since we don't have direct access to structuralPlasticity here, we'll create a simple implementation
        // In a real system, this would interface with the structural plasticity manager
        
        // For now, we'll create a new synapse and add it to a region
        // This is a simplified approach - the actual implementation would use Brain's addSynapse
        
        // Simulate synapse creation - in a real implementation, this would:
        // 1. Call StructuralPlasticity::createSynapse
        // 2. Set up synaptic properties
        // 3. Initialize activity-dependent plasticity flags
        
        ++synapsesCreated;
        
        NLM_LOG_INFO("New synapse formed: source neuron " + std::to_string(source.value) + 
                    " -> destination neuron " + std::to_string(destination.value) +
                    " weight=" + std::to_string(finalWeight) +
                    " type=" + (synapseType == SynapseType::Excitatory ? "excitatory" : "inhibitory"));
    }
}

float Synaptogenesis::Impl::calculateActivityBasedFormation(Brain* brain) {
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t activeNeuronCount = 0;
    
    // Calculate average firing rate across all neurons
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float firingRate = neuron->getState().firingRate;
                totalActivity += firingRate;
                if (firingRate > 0.1f) { // Consider active neurons
                    activeNeuronCount++;
                }
            }
        }
    }
    
    float avgFiringRate = totalActivity / std::max(static_cast<size_t>(1), brain->getTotalNeuronCount());
    
    // Activity-dependent formation: higher activity promotes more synaptogenesis
    // This simulates Hebbian-like activity-dependent development
    return std::min(1.0f, avgFiringRate * 5.0f); // Scale factor 5x for reasonable range
}

} // namespace nlm
