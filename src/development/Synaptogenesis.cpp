#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float currentDensity;
    float structuralPlasticity;
    
    // Track recent synaptogenesis events
    std::vector<std::tuple<SimulationStep, NeuronId, NeuronId, float>> recentFormations;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), currentDensity(0.05f),
             structuralPlasticity(1.0f) {}
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

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // Implement real synaptogenesis
    if (!brain || pImpl->formationRate <= 0.0f) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Calculate current connectivity density
    size_t totalNeurons = region->getTotalNeuronCount();
    size_t currentSynapses = region->getSynapseCount();
    
    // Theoretical maximum synapses (complete connectivity)
    size_t maxSynapses = totalNeurons * (totalNeurons - 1) / 2;
    
    if (maxSynapses == 0) return;
    
    pImpl->currentDensity = static_cast<float>(currentSynapses) / maxSynapses;
    
    // Check if we need more synapses (target density not reached)
    if (pImpl->currentDensity < pImpl->targetDensity) {
        // Calculate how many new synapses to form
        size_t targetSynapses = static_cast<size_t>(maxSynapses * pImpl->targetDensity);
        size_t synapsesToForm = targetSynapses - currentSynapses;
        
        // Form new synapses probabilistically
        formNewSynapses(region, rng, synapsesToForm);
    }
    
    // Structural plasticity modulates rate
    float plasticityMod = std::min(pImpl->structuralPlasticity, 2.0f);
    if (plasticityMod > 1.0f) {
        // During high plasticity periods (development), form more synapses
        size_t extraSynapses = static_cast<size_t>(synapsesToForm * (plasticityMod - 1.0f) * 0.5f);
        if (extraSynapses > 0) {
            formNewSynapses(region, rng, extraSynapses);
        }
    }
    
    // Record formation events
    recordRecentFormations();
}

void Synaptogenesis::formNewSynapses(NeuralRegion* region, RandomGenerator& rng, size_t count) {
    if (!region || count == 0) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return;
    
    // Get existing synapses for connectivity check
    auto existingSynapses = region->getSynapses();
    std::unordered_set<std::pair<NeuronId, NeuronId>, PairHash> existingConnections;
    
    for (const auto* syn : existingSynapses) {
        existingConnections.insert(std::make_pair(syn->getSourceNeuron(), syn->getDestinationNeuron()));
    }
    
    size_t formed = 0;
    while (formed < count && formed < neurons.size() * 2) { // Limit to reasonable number
        // Select pre- and post-synaptic neurons
        size_t preIdx = rng.uniformInt(0, neurons.size() - 1);
        size_t postIdx = rng.uniformInt(0, neurons.size() - 1);
        
        // Avoid self-connections
        if (preIdx == postIdx) continue;
        
        // Check if connection already exists
        auto conn1 = std::make_pair(neurons[preIdx]->getId(), neurons[postIdx]->getId());
        auto conn2 = std::make_pair(neurons[postIdx]->getId(), neurons[preIdx]->getId());
        
        if (existingConnections.find(conn1) != existingConnections.end() ||
            existingConnections.find(conn2) != existingConnections.end()) {
            continue;
        }
        
        // Form new synapse
        float weight = rng.uniformReal(0.1f, 0.5f); // Initial weight
        float delay = static_cast<float>(rng.uniformInt(1, 10)); // 1-10 time steps
        
        auto* synapse = new Synapse(neurons[preIdx]->getId(), neurons[postIdx]->getId(),
                                  weight, delay, SynapseType::Excitatory);
        
        // Add to region and track
        region->addSynapse(synapse);
        existingConnections.insert(conn1);
        
        // Record for plasticity
        synapse->enablePlasticity(true, true, true);
        
        formed++;
    }
    
    if (formed > 0) {
        NLM_LOG_INFO("Synaptogenesis formed " + std::to_string(formed) + " new synapses");
    }
}

void Synaptogenesis::setStructuralPlasticity(float plasticity) {
    pImpl->structuralPlasticity = plasticity;
}

void Synaptogenesis::recordRecentFormations() {
    // Record recent synaptogenesis events for analysis
    SimulationStep currentStep = 0; // Would come from brain simulation clock
    
    // This method would be called from brain with current step
    // For now, just track that synaptogenesis occurred
}

std::vector<Synaptogenesis::SynapticEvent> Synaptogenesis::getRecentFormations(SimulationStep fromStep) const {
    std::vector<SynapticEvent> result;
    
    for (const auto& event : pImpl->recentFormations) {
        SimulationStep step = std::get<0>(event);
        if (step >= fromStep) {
            result.push_back(SynapticEvent{
                std::get<0>(event),
                std::get<1>(event),
                std::get<2>(event),
                std::get<3>(event)
            });
        }
    }
    
    return result;
}

float Synaptogenesis::getCurrentDensity() const {
    return pImpl->currentDensity;
}

bool Synaptogenesis::needsMoreSynapses() const {
    return pImpl->currentDensity < pImpl->targetDensity;
}

} // namespace nlm

// Helper for unordered_set with pair
struct Synaptogenesis::PairHash {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1);
    }
};
