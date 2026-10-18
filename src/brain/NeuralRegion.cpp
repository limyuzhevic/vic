#include "NeuralRegion.hpp"
#include "StatisticsManager.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Global statistics manager instance
static StatisticsManager statsManager;

struct NeuralRegion::Impl {
    RegionId id;
    std::string name;
    // SoA arrays for populations
    std::vector<PopulationId> populationIds;
    std::vector<NeuronType> populationTypes;
    std::vector<size_t> populationSizes;
    // SoA arrays for synapses
    std::vector<NeuronId> synapseSources;
    std::vector<NeuronId> synapseDests;
    std::vector<SynapticWeight> synapseWeights;
    std::vector<Delay> synapseDelays;
    std::vector<SynapseType> synapseTypes;
    std::vector<SynapseId> synapseIds;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;  // source -> synapse ids
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;  // dest -> synapse ids
    SynapseId nextSynapseId;
    
    explicit Impl(RegionId id) : id(id), nextSynapseId(1) {}
};

NeuralRegion::NeuralRegion(RegionId id) : pImpl(new Impl(id)) {}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(new Impl(id)) {
    pImpl->name = name;
}

NeuralRegion::~NeuralRegion() = default;

NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

RegionId NeuralRegion::getId() const {
    return pImpl->id;
}

const std::string& NeuralRegion::getName() const {
    return pImpl->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->populations.size() + 1);
    auto population = std::make_unique<NeuralPopulation>(popId, size);
    population->setNeuronType(type);
    pImpl->populations.push_back(std::move(population));
    return popId;
}

NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

size_t NeuralRegion::getPopulationCount() const {
    return pImpl->populations.size();
}

const std::vector<std::unique_ptr<NeuralPopulation>>& NeuralRegion::getPopulations() const {
    return pImpl->populations;
}

std::vector<NeuralPopulation*> NeuralRegion::getAllPopulations() {
    std::vector<NeuralPopulation*> result;
    result.reserve(pImpl->populations.size());
    for (auto& pop : pImpl->populations) {
        result.push_back(pop.get());
    }
    return result;
}

SynapseId NeuralRegion::addSynapse(NeuronId source, NeuronId destination,
                                   SynapticWeight weight, Delay delay) {
    SynapseId synId(pImpl->nextSynapseId++);
    auto synapse = std::make_unique<Synapse>(synId, source, destination);
    synapse->setWeight(weight);
    synapse->setDelay(delay);
    
    pImpl->outgoingSynapses[source].push_back(synId);
    pImpl->incomingSynapses[destination].push_back(synId);
    
    pImpl->synapses.push_back(std::move(synapse));
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

size_t NeuralRegion::getSynapseCount() const {
    return pImpl->synapses.size();
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->synapses;
}

std::vector<Synapse*> NeuralRegion::getSynapsesFrom(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->outgoingSynapses.find(neuron);
    if (it != pImpl->outgoingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

std::vector<Synapse*> NeuralRegion::getSynapsesTo(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->incomingSynapses.find(neuron);
    if (it != pImpl->incomingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

size_t NeuralRegion::getTotalNeuronCount() const {
    // Use SoA array for efficient computation
    RegionStats regionStats;
    statsManager.initializeRegionStats(regionStats, pImpl->populationSizes.size(), pImpl->synapseIds.size());
    
    // Populate SoA arrays
    regionStats.populationSizes = pImpl->populationSizes;
    regionStats.synapseConnections = pImpl->synapseIds;
    
    // Update statistics using SoA access
    statsManager.updateRegionStats(regionStats,
                                 regionStats.populationSizes,
                                 regionStats.synapseConnections,
                                 regionStats.synapseConnections);
    
    return regionStats.totalNeuronCount;
}

size_t NeuralRegion::getActiveNeuronCount() const {
    size_t total = 0;
    // Would need neuron state access
    return total;
}

size_t NeuralRegion::getFiringNeuronCount() const {
    size_t total = 0;
    // Would need neuron state access
    return total;
}

float NeuralRegion::getAverageFiringRate() const {
    size_t popCount = pImpl->populationSizes.size();
    if (popCount == 0) return 0.0f;
    
    // Placeholder implementation
    return 0.0f;
}

float NeuralRegion::getAverageSynapticWeight() const {
    // Use SoA array for efficient computation
    RegionStats regionStats;
    statsManager.initializeRegionStats(regionStats, pImpl->populationSizes.size(), pImpl->synapseIds.size());
    
    // Populate SoA arrays
    regionStats.populationSizes = pImpl->populationSizes;
    regionStats.synapseWeights = pImpl->synapseWeights;
    regionStats.synapseConnections = pImpl->synapseIds;
    
    // Update statistics using SoA access
    statsManager.updateRegionStats(regionStats,
                                 regionStats.populationSizes,
                                 regionStats.synapseWeights,
                                 regionStats.synapseConnections);
    
    return regionStats.averageSynapticWeight;
}

float NeuralRegion::getSynapticDensity() const {
    // Use SoA array for efficient computation
    RegionStats regionStats;
    statsManager.initializeRegionStats(regionStats, pImpl->populationSizes.size(), pImpl->synapseIds.size());
    
    // Populate SoA arrays
    regionStats.populationSizes = pImpl->populationSizes;
    regionStats.synapseConnections = pImpl->synapseIds;
    
    // Update statistics using SoA access
    statsManager.updateRegionStats(regionStats,
                                 regionStats.populationSizes,
                                 regionStats.synapseConnections,
                                 regionStats.synapseConnections);
    
    return regionStats.synapticDensity;
}

void NeuralRegion::step(Timestamp currentTime) {
    // Step all populations
    for (auto& pop : pImpl->populations) {
        pop->step(currentTime);
    }
    
    // Step all synapses
    for (auto& syn : pImpl->synapses) {
        syn->step(currentTime);
    }
}

void NeuralRegion::reset() {
    for (auto& pop : pImpl->populations) {
        pop->reset();
    }
    for (auto& syn : pImpl->synapses) {
        syn->reset();
    }
}

void NeuralRegion::initializeRandomConnectivity(RandomGenerator& rng,
                                               float connectionProbability,
                                               float meanWeight,
                                               float weightVariance) {
    // Efficient random connectivity initialization
    // Creates synapses based on neuron types:
    // - Excitatory neurons -> all neurons (excitatory synapses)
    // - Inhibitory neurons -> all neurons (inhibitory synapses)
    
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    // Create random connections
    for (size_t i = 0; i < neuronCount; ++i) {
        Neuron* preNeuron = neurons[i];
        NeuronType preType = preNeuron->getType();
        
        // Skip if not a proper source neuron
        if (preType == NeuronType::Modulatory) continue;
        
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i == j) continue;  // No self-connections
            
            Neuron* postNeuron = neurons[j];
            
            // Probabilistic connection
            if (!rng.bernoulli(connectionProbability)) continue;
            
            // Determine synapse type based on pre-synaptic neuron type
            SynapseType synType;
            float weight;
            
            if (preType == NeuronType::Excitatory || 
                preType == NeuronType::Sensory ||
                preType == NeuronType::Motor ||
                preType == NeuronType::Internal) {
                // excitatory synapse
                synType = SynapseType::Excitatory;
                weight = meanWeight + rng.normal(0.0f, weightVariance);
                weight = std::max(0.01f, weight);  // Ensure positive
            } else if (preType == NeuronType::Inhibitory) {
                // inhibitory synapse
                synType = SynapseType::Inhibitory;
                weight = -(meanWeight + rng.normal(0.0f, weightVariance));
                weight = std::min(-0.01f, weight);  // Ensure negative
            } else {
                synType = SynapseType::Excitatory;
                weight = meanWeight + rng.normal(0.0f, weightVariance);
            }
            
            // Add synapse
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            
            // Initialize random properties
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
    }
}

std::vector<Neuron*> NeuralRegion::getAllNeurons() {
    std::vector<Neuron*> result;
    // SoA implementation - would need neuron lookup system
    return result;
}

std::vector<const Neuron*> NeuralRegion::getAllNeurons() const {
    std::vector<const Neuron*> result;
    // SoA implementation - would need neuron lookup system
    return result;
}

SynapseId NeuralRegion::addSynapse(NeuronId source, NeuronId destination,
                                   SynapticWeight weight, Delay delay) {
    SynapseId synId(pImpl->nextSynapseId++);
    
    // Store synapse data in SoA arrays
    pImpl->synapseSources.push_back(source);
    pImpl->synapseDests.push_back(destination);
    pImpl->synapseWeights.push_back(weight);
    pImpl->synapseDelays.push_back(delay);
    pImpl->synapseIds.push_back(synId);
    pImpl->synapseTypes.push_back(SynapseType::Excitatory);  // Default type
    
    // Update connectivity maps
    pImpl->outgoingSynapses[source].push_back(synId);
    pImpl->incomingSynapses[destination].push_back(synId);
    
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    // Search through synapse arrays
    for (size_t i = 0; i < pImpl->synapseIds.size(); ++i) {
        if (pImpl->synapseIds[i] == id) {
            // Return synapse pointer - would need proper synapse implementation
            return nullptr;
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (size_t i = 0; i < pImpl->synapseIds.size(); ++i) {
        if (pImpl->synapseIds[i] == id) {
            return nullptr;
        }
    }
    return nullptr;
}

size_t NeuralRegion::getSynapseCount() const {
    return pImpl->synapseIds.size();
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    // Return placeholder for now
    static const std::vector<std::unique_ptr<Synapse>> empty;
    return empty;
}

} // namespace nlm
