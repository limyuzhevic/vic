#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include <cstddef>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

    if (!brain) return;
    
    // Get developmental stage for rate modulation
    float plasticityMod = 1.0f;
    if (brain->getDevelopmentSystem()) {
        plasticityMod = brain->getDevelopmentSystem()->getPlasticityModifier();
    }
    
    float actualFormationRate = getFormationRate() * plasticityMod;
    
    /*
     * Implement real synaptogenesis:
     * 
     * 1. New synapses form probabilistically between active neurons
     * 2. Formation rate is developmentally modulated (higher in early stages)
     * 3. Synapses preferentially form between neurons with similar activity patterns
     * 4. Integration with neuromodulation systems (dopamine, curiosity, novelty)
     * 5. Consider metabolic constraints and energy efficiency
     */
    
    // Collect neural activity statistics
    struct ActiveNeuron {
        NeuronId id;
        NeuronType type;
        float activity;
        float avgIncomingWeight;
        float avgOutgoingWeight;
        size_t incomingSynapseCount;
        size_t outgoingSynapseCount;
    };
    
    std::vector<ActiveNeuron> activeNeurons;
    
    // Scan all regions and collect neuron activity data
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                ActiveNeuron active;
                active.id = neuron->getId();
                active.type = pop->getNeuronType();
                
                // Calculate activity based on spike history and membrane potential
                const auto& spikes = neuron->getSpikeHistory();
                if (!spikes.empty()) {
                    // Recent spikes = higher activity
                    active.activity = static_cast<float>(spikes.size()) * 0.1f;
                }
                
                // Add membrane potential contribution
                float vRel = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                active.activity += vRel / 100.0f;
                
                // Calculate average synaptic weights
                auto incoming = region->getSynapsesTo(neuron->getId());
                auto outgoing = region->getSynapsesFrom(neuron->getId());
                
                active.incomingSynapseCount = incoming.size();
                active.outgoingSynapseCount = outgoing.size();
                
                float totalIncomingWeight = 0.0f;
                float totalOutgoingWeight = 0.0f;
                
                for (auto* syn : incoming) {
                    totalIncomingWeight += std::abs(syn->getWeight());
                }
                
                for (auto* syn : outgoing) {
                    totalOutgoingWeight += std::abs(syn->getWeight());
                }
                
                if (incoming.size() > 0) active.avgIncomingWeight = totalIncomingWeight / incoming.size();
                if (outgoing.size() > 0) active.avgOutgoingWeight = totalOutgoingWeight / outgoing.size();
                
                activeNeurons.push_back(active);
            }
        }
    }
    
    if (activeNeurons.size() < 2) return;
    
    // Synaptogenesis occurs in two phases:
    
    // Phase 1: Activity-dependent synaptogenesis
    // Synapses form between neurons that are frequently co-active
    for (size_t attempt = 0; attempt < activeNeurons.size() * 2; ++attempt) {
        if (!rng.bernoulli(actualFormationRate)) continue;
        
        size_t idx1 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
        size_t idx2 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
        
        if (idx1 == idx2) continue;
        
        const ActiveNeuron& neuron1 = activeNeurons[idx1];
        const ActiveNeuron& neuron2 = activeNeurons[idx2];
        
        // Preference for neurons with similar activity levels (Hebbian-like)
        float activitySimilarity = 1.0f - std::abs(neuron1.activity - neuron2.activity) / (neuron1.activity + neuron2.activity + 0.1f);
        
        // Preference for neurons of similar types (modal connectivity)
        bool sameType = neuron1.type == neuron2.type;
        
        // Preference for neurons with appropriate connection availability
        bool connectionAvailable = (neuron1.outgoingSynapseCount < 100) && (neuron2.incomingSynapseCount < 100);
        
        // Combined probability of formation
        float formationProb = actualFormationRate;
        if (activitySimilarity > 0.7f) formationProb *= 1.5f;
        if (sameType) formationProb *= 1.2f;
        if (connectionAvailable) formationProb *= 1.1f;
        
        if (rng.bernoulli(formationProb)) {
            // Calculate initial weight based on developmental stage and activity
            float baseWeight = 0.05f + rng.uniformReal(0.0f, 0.15f);
            
            // Modulate weight based on developmental stage
            if (brain->getDevelopmentSystem()) {
                DevelopmentalStage stage = brain->getDevelopmentSystem()->getStage();
                switch (stage) {
                    case DevelopmentalStage::Initial:
                        baseWeight *= 0.8f;  // Initial weak connections
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        baseWeight *= 1.2f;  // Enhanced formation
                        break;
                    case DevelopmentalStage::Maturation:
                        baseWeight *= 1.0f;
                        break;
                    case DevelopmentalStage::Adult:
                        baseWeight *= 0.9f;  // More selective
                        break;
                }
            }
            
            // Integration with neuromodulation
            float neuromodMod = 1.0f;
            if (brain->getDopamine()) {
                float dopamineLevel = brain->getDopamine()->getLevel();
                neuromodMod *= (1.0f + dopamineLevel * 0.5f);  // Dopamine enhances formation
            }
            if (brain->getCuriosity()) {
                float curiosityLevel = brain->getCuriosity()->getLevel();
                neuromodMod *= (1.0f + curiosityLevel * 0.3f);  // Curiosity drives exploration
            }
            
            float finalWeight = baseWeight * neuromodMod;
            
            // Determine which neuron is source and which is destination
            // Prefer connections from higher to lower activity neurons
            NeuronId source, destination;
            if (neuron1.activity > neuron2.activity) {
                source = neuron1.id;
                destination = neuron2.id;
            } else {
                source = neuron2.id;
                destination = neuron1.id;
            }
            
            // Find the region containing the destination neuron and create synapse
            for (auto& region : brain->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == destination) {
                        // Check if connection already exists
                        auto existingSynapses = region->getSynapsesTo(destination);
                        bool exists = false;
                        for (Synapse* syn : existingSynapses) {
                            if (syn->getSourceNeuron() == source) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists && brain->getStructuralPlasticity()) {
                            // Create the new synapse
                            SynapseId synId = brain->getStructuralPlasticity()->createSynapse(brain, source, destination, finalWeight);
                            if (synId != INVALID_SYNAPSE_ID) {
                                break;  // Synapse created successfully
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Phase 2: Developmental stage-specific synaptogenesis patterns
    // Different patterns based on developmental stage
    if (brain->getDevelopmentSystem()) {
        DevelopmentalStage stage = brain->getDevelopmentSystem()->getStage();
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                // Initial burst of widespread connectivity
                for (size_t attempt = 0; attempt < activeNeurons.size(); ++attempt) {
                    if (!rng.bernoulli(formationRate * 0.5f)) continue;
                    
                    size_t idx1 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    size_t idx2 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    
                    if (idx1 == idx2) continue;
                    
                    NeuronId source = activeNeurons[idx1].id;
                    NeuronId destination = activeNeurons[idx2].id;
                    
                    // Create initial weak connections for bootstrapping
                    float weight = 0.02f + rng.uniformReal(0.0f, 0.03f);
                    
                    for (auto& region : brain->getRegions()) {
                        auto neurons = region->getAllNeurons();
                        for (auto* neuron : neurons) {
                            if (neuron->getId() == destination) {
                                auto existing = region->getSynapsesTo(destination);
                                bool exists = false;
                                for (Synapse* syn : existing) {
                                    if (syn->getSourceNeuron() == source) {
                                        exists = true;
                                        break;
                                    }
                                }
                                
                                if (!exists && brain->getStructuralPlasticity()) {
                                    brain->getStructuralPlasticity()->createSynapse(brain, source, destination, weight);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                // Critical period: enhanced refinement of connections
                for (size_t attempt = 0; attempt < activeNeurons.size() * 0.5f; ++attempt) {
                    if (!rng.bernoulli(formationRate * 1.5f)) continue;
                    
                    size_t idx1 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    size_t idx2 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    
                    if (idx1 == idx2) continue;
                    
                    // Stronger connections during critical period
                    NeuronId source = activeNeurons[idx1].id;
                    NeuronId destination = activeNeurons[idx2].id;
                    
                    float weight = 0.15f + rng.uniformReal(0.0f, 0.2f);
                    
                    for (auto& region : brain->getRegions()) {
                        auto neurons = region->getAllNeurons();
                        for (auto* neuron : neurons) {
                            if (neuron->getId() == destination) {
                                auto existing = region->getSynapsesTo(destination);
                                bool exists = false;
                                for (Synapse* syn : existing) {
                                    if (syn->getSourceNeuron() == source) {
                                        exists = true;
                                        break;
                                    }
                                }
                                
                                if (!exists && brain->getStructuralPlasticity()) {
                                    brain->getStructuralPlasticity()->createSynapse(brain, source, destination, weight);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
                
            case DevelopmentalStage::Adult:
                // Adult stage: selective, experience-driven synaptogenesis
                for (size_t attempt = 0; attempt < activeNeurons.size() * 0.1f; ++attempt) {
                    if (!rng.bernoulli(formationRate * 0.3f)) continue;
                    
                    size_t idx1 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    size_t idx2 = rng.uniformInt(0, static_cast<int>(activeNeurons.size()) - 1);
                    
                    if (idx1 == idx2) continue;
                    
                    // Check for correlated activity patterns for adult learning
                    const ActiveNeuron& n1 = activeNeurons[idx1];
                    const ActiveNeuron& n2 = activeNeurons[idx2];
                    
                    // Only form connections between neurons with well-established activity patterns
                    if (n1.activity < 0.5f || n2.activity < 0.5f) continue;
                    
                    float weight = 0.1f + rng.uniformReal(0.0f, 0.15f);
                    
                    for (auto& region : brain->getRegions()) {
                        auto neurons = region->getAllNeurons();
                        for (auto* neuron : neurons) {
                            if (neuron->getId() == n2.id) {
                                auto existing = region->getSynapsesTo(n2.id);
                                bool exists = false;
                                for (Synapse* syn : existing) {
                                    if (syn->getSourceNeuron() == n1.id) {
                                        exists = true;
                                        break;
                                    }
                                }
                                
                                if (!exists && brain->getStructuralPlasticity()) {
                                    brain->getStructuralPlasticity()->createSynapse(brain, n1.id, n2.id, weight);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
                
            default:
                // Default adult-like behavior
                break;
        }
    }
}

} // namespace nlm
