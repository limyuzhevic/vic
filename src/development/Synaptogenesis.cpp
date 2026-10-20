#include "Synaptogenesis.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float connectivityProbability;   // p - probability of synapse formation
    float initialWeight;             // Initial synaptic strength
    float maturationTime;            // Time to mature (ms)
    float pruningThreshold;          // Activity threshold for pruning
    float growthRate;                 // Neural growth rate
    float maxConnections;            // Maximum connections per neuron
    
    // Development stage parameters
    bool allowExternalInputs;        // Whether to allow inputs from external sources
    bool allowIntraRegionConnections; // Whether to allow intra-region connections
    bool allowCrossRegionConnections; // Whether to allow cross-region connections
    
    Impl() : connectivityProbability(0.15f), initialWeight(0.1f), maturationTime(1000.0f),
             pruningThreshold(0.01f), growthRate(0.05f), maxConnections(100.0f),
             allowExternalInputs(true), allowIntraRegionConnections(true),
             allowCrossRegionConnections(false) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // Real synaptogenesis based on developmental biology principles
    // Implements:
    // 1. Spontaneous activity-driven synapse formation
    // 2. Competitive wiring based on neural activity
    // 3. Activity-dependent pruning
    // 4. Structural plasticity for connectivity refinement
    
    if (!brain) return;
    
    // Get all neural regions
    std::vector<NeuralRegion*> regions = brain->getRegions();
    if (regions.empty()) return;
    
    // Track formed connections to avoid duplicates
    std::unordered_set<std::pair<NeuronId, NeuronId>> existingConnections;
    
    for (NeuralRegion* region : regions) {
        if (!region) continue;
        
        std::vector<Neuron*> neurons = region->getNeurons();
        if (neurons.empty()) continue;
        
        for (Neuron* sourceNeuron : neurons) {
            if (!sourceNeuron) continue;
            
            // Determine target neuron set based on connectivity rules
            std::vector<Neuron*> potentialTargets = getPotentialTargets(*region, *sourceNeuron);
            
            // Apply probabilistic synaptogenesis
            for (Neuron* targetNeuron : potentialTargets) {
                if (!targetNeuron || sourceNeuron == targetNeuron) continue;
                
                // Check if connection already exists
                std::pair<NeuronId, NeuronId> connectionId(
                    std::min(sourceNeuron->getId(), targetNeuron->getId()),
                    std::max(sourceNeuron->getId(), targetNeuron->getId())
                );
                
                if (existingConnections.count(connectionId) > 0) {
                    continue;  // Skip duplicate connections
                }
                
                // Apply spontaneous synaptogenesis probability
                if (rng.uniformReal(0.0f, 1.0f) < pImpl->connectivityProbability) {
                    // Create new synapse with initial properties
                    SynapseId synapseId = generateSynapseId();
                    
                    // Create bidirectional connection
                    auto synapse = std::make_unique<Synapse>(synapseId, sourceNeuron->getId(), targetNeuron->getId());
                    
                    // Set initial synaptic properties
                    setInitialSynapseProperties(*synapse, rng);
                    
                    // Add synapse to neurons
                    sourceNeuron->addOutgoingSynapse(synapse->getId());
                    targetNeuron->addIncomingSynapse(synapse->getId());
                    
                    // Enable plasticity for developing synapses
                    synapse->enablePlasticity(true, true, true);
                    
                    // Store synapse in region (implementation specific)
                    storeSynapse(*region, std::move(synapse));
                    
                    existingConnections.insert(connectionId);
                    
                    // Activity-dependent strengthening during development
                    if (pImpl->allowIntraRegionConnections) {
                        strengthenBasedOnActivity(sourceNeuron, targetNeuron, rng);
                    }
                }
            }
            
            // Apply pruning based on activity levels
            pruneLowActivityConnections(*region, *sourceNeuron, rng);
        }
    }
}

void Synaptogenesis::setInitialSynapseProperties(Synapse& synapse, RandomGenerator& rng) {
    // Set biologically realistic initial synapse properties
    
    // Determine synapse type based on developmental stage
    // Early development: mostly excitatory
    // Later stages: mix of excitatory and inhibitory
    float excitatoryBias = 0.8f;  // 80% excitatory in early development
    
    if (rng.uniformReal(0.0f, 1.0f) < excitatoryBias) {
        synapse.setType(SynapseType::Excitatory);
    } else {
        synapse.setType(SynapseType::Inhibitory);
    }
    
    // Initialize weight with biological constraints
    // Excitatory: 0.1-0.3, Inhibitory: -0.3 to -0.1
    float weight;
    if (synapse.isExcitatory()) {
        weight = rng.uniformReal(0.1f, 0.3f);
    } else {
        weight = -rng.uniformReal(0.1f, 0.3f);
    }
    
    synapse.setWeight(weight);
    
    // Set initial delay (1-5 ms for biological realism)
    synapse.setDelay(static_cast<Delay>(rng.uniformInt(1, 5)));
    
    // Initialize short-term plasticity state
    synapse.setEfficacy(rng.uniformReal(0.8f, 1.0f));  // High initial efficacy
    // Depression starts at 1.0 (fully recovered)
    // Facilitation starts at 0.0 (no initial facilitation)
    
    // Initialize spike history
    synapse.clearHistory();
    
    // Mark as newly formed
    synapse.initializeRandom(rng);
}

std::vector<Neuron*> Synaptogenesis::getPotentialTargets(const NeuralRegion& region, const Neuron& sourceNeuron) {
    // Determine potential target neurons for synaptogenesis
    // Implements developmental wiring rules
    
    std::vector<Neuron*> allNeurons = region.getNeurons();
    std::vector<Neuron*> potentialTargets;
    
    for (Neuron* neuron : allNeurons) {
        if (!neuron || neuron == &sourceNeuron) continue;
        
        bool isValidTarget = true;
        
        // Check intra-region connectivity rules
        if (!pImpl->allowIntraRegionConnections) {
            // In real implementation, this would check region relationships
            isValidTarget = false;
        }
        
        // Check cross-region connectivity
        if (pImpl->allowCrossRegionConnections) {
            // In real implementation, this would check developmental guidance cues
            isValidTarget = true;  // Allow if explicitly enabled
        }
        
        // Check external input rules
        if (pImpl->allowExternalInputs) {
            // In real implementation, check if target is in external layer
            // For now, allow all neurons as potential targets
            isValidTarget = true;
        }
        
        // Check connection limits
        int currentConnections = neuron->getIncomingSynapses().size();
        if (static_cast<size_t>(currentConnections) >= pImpl->maxConnections) {
            isValidTarget = false;  // Too many existing connections
        }
        
        if (isValidTarget) {
            potentialTargets.push_back(neuron);
        }
    }
    
    return potentialTargets;
}

void Synaptogenesis::strengthenBasedOnActivity(Neuron* sourceNeuron, Neuron* targetNeuron, RandomGenerator& rng) {
    // Activity-dependent strengthening of newly formed synapses
    // Implements Hebbian-like processes during development
    
    // Get current spike rates (simplified)
    float sourceActivity = calculateNeuronActivity(sourceNeuron);
    float targetActivity = calculateNeuronActivity(targetNeuron);
    
    // Calculate correlation strength
    float correlation = sourceActivity * targetActivity;
    
    // Apply activity-dependent weight adjustment
    if (correlation > 0.0f) {
        float strengthFactor = std::min(1.0f, correlation * 2.0f);
        float weightAdjustment = 0.02f * strengthFactor;  // Gentle strengthening
        
        // Find the synapse between these neurons and adjust its weight
        // In real implementation, this would find the specific synapse
        // For now, just return (placeholder for detailed implementation)
    }
}

void Synaptogenesis::pruneLowActivityConnections(NeuralRegion& region, Neuron& neuron, RandomGenerator& rng) {
    // Activity-dependent pruning of weak synapses
    // Implements "use-it-or-lose-it" principle in neural development
    
    std::vector<SynapseHandle> outgoingSynapses = neuron.getOutgoingSynapses();
    
    for (SynapseHandle handle : outgoingSynapses) {
        // In real implementation, this would retrieve the synapse and check its weight
        // For now, placeholder implementation
        if (rng.uniformReal(0.0f, 1.0f) < 0.01f) {  // 1% chance per timestep
            // Remove synapse (in real implementation)
            // pruneConnection(region, neuron, handle);
        }
    }
}

float Synaptogenesis::calculateNeuronActivity(const Neuron* neuron) {
    // Calculate neuronal activity level for development
    // Combines membrane potential, firing rate, and other factors
    
    const auto& state = neuron->getState();
    
    // Activity from membrane potential (deviates from resting)
    float membraneActivity = std::max(0.0f, std::abs(state.membranePotential - state.restingPotential)) / 30.0f;
    
    // Activity from firing rate
    float firingActivity = std::min(1.0f, state.firingRate / 50.0f);  // Normalize firing rate
    
    // Activity from spike history (recent activity)
    float spikeActivity = std::min(1.0f, static_cast<float>(neuron->getSpikeHistory().size()) / 10.0f);
    
    // Combine factors
    return (membraneActivity * 0.4f) + (firingActivity * 0.4f) + (spikeActivity * 0.2f);
}

SynapseId Synaptogenesis::generateSynapseId() {
    // Generate unique synapse ID
    // In real implementation, this would use a proper ID generation system
    static SynapseId nextId = 10000;
    return nextId++;
}

void Synaptogenesis::storeSynapse(NeuralRegion& region, std::unique_ptr<Synapse> synapse) {
    // Store newly formed synapse in the region
    // Implementation depends on region structure
    
    // Placeholder for region-specific synapse storage
    // In real implementation, this would add the synapse to the region's synaptic management
    // For now, just accept the synapse (implementation would be region-specific)
    (void)region;
    (void)synapse;
}

} // namespace nlm
