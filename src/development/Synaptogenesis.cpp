#include "Synaptogenesis.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float currentDensity;
    SimulationStep lastFormationStep;
    float minWeightForFormation;
    
    Impl() 
        : formationRate(0.001f), targetDensity(0.1f), currentDensity(0.0f)
        , lastFormationStep(0), minWeightForFormation(0.01f) {}
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
    pImpl->targetDensity = std::max(0.0f, density);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || pImpl->formationRate <= 0.0f) return;
    
    const SimulationStep currentStep = brain->getTotalSpikeCount() / 100;
    const SimulationStep stepsSinceFormation = currentStep - pImpl->lastFormationStep;
    
    if (stepsSinceFormation < 100) return; // Update every 100 steps
    
    pImpl->lastFormationStep = currentStep;
    
    // Calculate current synapse density
    size_t totalSynapses = 0;
    for (const auto& region : brain->getRegions()) {
        totalSynapses += region->getSynapseCount();
    }
    
    size_t targetSynapses = static_cast<size_t>(brain->getTotalNeuronCount() * pImpl->targetDensity);
    if (totalSynapses >= targetSynapses) return; // Target reached
    
    pImpl->currentDensity = static_cast<float>(totalSynapses) / 
                           std::max(1.0f, static_cast<float>(brain->getTotalNeuronCount()));
    
    // Determine how many new synapses to create
    size_t synapsesToCreate = std::min(
        static_cast<size_t>(pImpl->formationRate * stepsSinceFormation),
        targetSynapses - totalSynapses
    );
    
    if (synapsesToCreate == 0) return;
    
    NLM_LOG_INFO(std::to_string(synapsesToCreate) + " new synapses forming");
    
    // Create new synapses
    for (size_t i = 0; i < synapsesToCreate; ++i) {
        // Find candidate neurons for new connection
        auto neurons = collectCandidateNeurons(brain, rng);
        if (neurons.size() < 2) continue;
        
        // Create new synapse
        createNewSynapse(brain, neurons.first, neurons.second, rng);
    }
}

std::pair<Neuron*, Neuron*> Synaptogenesis::collectCandidateNeurons(
    Brain* brain, RandomGenerator& rng) {
    // Collect all neurons from the first region (simplified)
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron);
            }
        }
    }
    
    if (allNeurons.size() < 2) return {nullptr, nullptr};
    
    // Select two distinct neurons
    size_t idx1 = rng.next() % allNeurons.size();
    size_t idx2;
    do {
        idx2 = rng.next() % allNeurons.size();
    } while (idx2 == idx1);
    
    return {allNeurons[idx1], allNeurons[idx2]};
}

void Synaptogenesis::createNewSynapse(Brain* brain, Neuron* source, Neuron* target,
                                      RandomGenerator& rng) {
    if (!source || !target) return;
    
    auto* region = source->getRegionId() ? brain->getRegion(source->getRegionId()) : nullptr;
    if (!region) return;
    
    // Check if connection already exists
    auto existingSynapses = region->getSynapsesFrom(source->getId());
    bool connectionExists = false;
    for (const auto* syn : existingSynapses) {
        if (syn->getDestinationNeuron() == target->getId()) {
            connectionExists = true;
            break;
        }
    }
    
    if (connectionExists) return;
    
    // Determine synapse type based on neuron types
    SynapseType synType = SynapseType::Excitatory;
    float initialWeight = 0.1f + rng.nextFloat() * 0.4f; // 0.1 to 0.5
    
    // Create new synapse
    SynapseId synId(rng.next());
    auto synapse = std::make_unique<Synapse>(synId, source->getId(), target->getId());
    synapse->setType(synType);
    synapse->setWeight(initialWeight);
    synapse->enablePlasticity(true, true, false);
    
    // Add to region
    region->addSynapse(std::move(synapse));
    
    NLM_LOG_INFO("New synapse created: " + std::to_string(source->getId().index()) + 
                " -> " + std::to_string(target->getId().index()) + 
                " (weight: " + std::to_string(initialWeight) + ")");
}

} // namespace nlm
