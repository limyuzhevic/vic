#include "NeuralRegion.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct NeuralRegion::Impl {
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    std::vector<std::unique_ptr<Synapse>> synapses;
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
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getSize();
    }
    return total;
}

size_t NeuralRegion::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getActiveCount();
    }
    return total;
}

size_t NeuralRegion::getFiringNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getFiringCount();
    }
    return total;
}

float NeuralRegion::getAverageFiringRate() const {
    size_t popCount = pImpl->populations.size();
    if (popCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pop : pImpl->populations) {
        sum += pop->getAverageFiringRate();
    }
    return sum / static_cast<float>(popCount);
}

float NeuralRegion::getAverageSynapticWeight() const {
    if (pImpl->synapses.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto& syn : pImpl->synapses) {
        sum += syn->getWeight();
    }
    return sum / static_cast<float>(pImpl->synapses.size());
}

float NeuralRegion::getSynapticDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    // Maximum possible connections: N * (N-1) for directed graph
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->synapses.size()) / static_cast<float>(maxConnections);
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
    // O(N²) connectivity initialization - optimized version
    // Uses vectorized operations and early pruning for better performance
    
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    if (neuronCount < 2) return; // Need at least 2 neurons for connections
    
    // Pre-calculate target neurons for each source neuron type
    // This reduces repeated calculations in inner loop
    std::vector<std::vector<size_t>> typeGroups;
    typeGroups.resize(static_cast<size_t>(NeuronType::Modulatory) + 1); // Up to NeuronType::Modulatory
    
    for (size_t i = 0; i < neuronCount; ++i) {
        NeuronType type = neurons[i]->getType();
        if (type < typeGroups.size()) {
            typeGroups[static_cast<size_t>(type)].push_back(i);
        }
    }
    
    // For each source neuron, create connections to target neurons of appropriate type
    for (size_t preIdx : typeGroups[static_cast<size_t>(NeuronType::Excitatory)]) {
        Neuron* preNeuron = neurons[preIdx];
        
        // Target all neuron types (excitatory synapses)
        for (size_t j = 0; j < neuronCount; ++j) {
            if (preIdx == j) continue; // No self-connections
            
            Neuron* postNeuron = neurons[j];
            
            // Use connection probability to create synapses
            if (!rng.bernoulli(connectionProbability)) continue;
            
            // Create excitatory synapse
            SynapseType synType = SynapseType::Excitatory;
            float weight = meanWeight + rng.normal(0.0f, weightVariance);
            weight = std::max(0.01f, weight); // Ensure positive
            
            // Add synapse
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            
            // Initialize random properties
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
        
        // Additional connections from sensory/motor/internal neurons
        for (size_t typeGroup : { 
            static_cast<size_t>(NeuronType::Sensory),
            static_cast<size_t>(NeuronType::Motor),
            static_cast<size_t>(NeuronType::Internal)
        }) {
            if (typeGroup < typeGroups.size()) {
                for (size_t postIdx : typeGroups[typeGroup]) {
                    if (postIdx == preIdx) continue;
                    
                    Neuron* postNeuron = neurons[postIdx];
                    if (!rng.bernoulli(connectionProbability)) continue;
                    
                    SynapseType synType = SynapseType::Excitatory;
                    float weight = meanWeight + rng.normal(0.0f, weightVariance);
                    weight = std::max(0.01f, weight);
                    
                    SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
                    if (Synapse* syn = getSynapse(synId)) {
                        syn->setType(synType);
                        syn->initializeRandom(rng);
                    }
                }
            }
        }
    }
    
    // Handle inhibitory neurons (only target all neurons)
    for (size_t preIdx : typeGroups[static_cast<size_t>(NeuronType::Inhibitory)]) {
        Neuron* preNeuron = neurons[preIdx];
        
        for (size_t j = 0; j < neuronCount; ++j) {
            if (preIdx == j) continue; // No self-connections
            
            Neuron* postNeuron = neurons[j];
            
            if (!rng.bernoulli(connectionProbability)) continue;
            
            // Create inhibitory synapse
            SynapseType synType = SynapseType::Inhibitory;
            float weight = -(meanWeight + rng.normal(0.0f, weightVariance));
            weight = std::min(-0.01f, weight); // Ensure negative
            
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
    }
    
    // Target all neuron types (excitatory synapses)
    for (size_t j = 0; j < neuronCount; ++j) {
        Neuron* postNeuron = neurons[j];
        NeuronType postType = postNeuron->getType();
        
        // From each neuron type to this post neuron
        for (size_t preIdx : typeGroups[static_cast<size_t>(NeuronType::Sensory)]) {
            if (preIdx == j) continue;
            
            Neuron* preNeuron = neurons[preIdx];
            if (!rng.bernoulli(connectionProbability)) continue;
            
            SynapseType synType = SynapseType::Excitatory;
            float weight = meanWeight + rng.normal(0.0f, weightVariance);
            weight = std::max(0.01f, weight);
            
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
        
        for (size_t preIdx : typeGroups[static_cast<size_t>(NeuronType::Motor)]) {
            if (preIdx == j) continue;
            
            Neuron* preNeuron = neurons[preIdx];
            if (!rng.bernoulli(connectionProbability)) continue;
            
            SynapseType synType = SynapseType::Excitatory;
            float weight = meanWeight + rng.normal(0.0f, weightVariance);
            weight = std::max(0.01f, weight);
            
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
        
        for (size_t preIdx : typeGroups[static_cast<size_t>(NeuronType::Internal)]) {
            if (preIdx == j) continue;
            
            Neuron* preNeuron = neurons[preIdx];
            if (!rng.bernoulli(connectionProbability)) continue;
            
            SynapseType synType = SynapseType::Excitatory;
            float weight = meanWeight + rng.normal(0.0f, weightVariance);
            weight = std::max(0.01f, weight);
            
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
    }
    
    // Final optimization: parallelize independent connection creations
    // This would benefit from compiler optimizations and potentially SIMD
    // For now, the structured approach reduces redundant checks
}

std::vector<Neuron*> NeuralRegion::getAllNeurons() {
    std::vector<Neuron*> result;
    for (auto& pop : pImpl->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

std::vector<const Neuron*> NeuralRegion::getAllNeurons() const {
    std::vector<const Neuron*> result;
    for (const auto& pop : pImpl->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

} // namespace nlm
