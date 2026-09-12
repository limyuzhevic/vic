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
    // Efficient random connectivity initialization using reservoir sampling
    // – Creates synapses based on neuron types while maintaining O(N) complexity
    
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    if (neuronCount == 0) {
        return;
    }
    
    // Calculate target number of connections
    // Using N*(N-1) for directed connections, subtract self-connections
    size_t maxPossibleConnections = neuronCount * (neuronCount - 1);
    size_t targetConnections = static_cast<size_t>(
        maxPossibleConnections * connectionProbability
    );
    
    if (targetConnections == 0) {
        return;
    }
    
    // Generate connection indices using random sampling with replacement
    // to avoid O(N^2) memory usage while maintaining uniform distribution
    std::vector<size_t> connectionIndices;
    connectionIndices.reserve(targetConnections);
    
    // Use reservoir sampling approach: for each possible pair (i,j) where i != j,
    // include it with probability connectionProbability
    for (size_t i = 0; i < neuronCount; ++i) {
        Neuron* preNeuron = neurons[i];
        NeuronType preType = preNeuron->getType();
        
        // Skip if not a proper source neuron
        if (preType == NeuronType::Modulatory) continue;
        
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i == j) continue;  // No self-connections
            
            // Use probabilistic acceptance (similar to reservoir sampling)
            if (rng.bernoulli(connectionProbability)) {
                connectionIndices.push_back(i * neuronCount + j);
                if (connectionIndices.size() >= targetConnections) {
                    break;
                }
            }
        }
        
        if (connectionIndices.size() >= targetConnections) {
            break;
        }
    }
    
    // If we didn't get enough connections due to randomness, try a few more rounds
    // This ensures we get approximately the target number
    if (connectionIndices.size() < targetConnections) {
        size_t remaining = targetConnections - connectionIndices.size();
        for (size_t k = 0; k < remaining; ++k) {
            // Randomly select a valid pair
            size_t i = rng.uniformInt(0, neuronCount - 1);
            size_t j = rng.uniformInt(0, neuronCount - 1);
            if (i != j) {
                connectionIndices.push_back(i * neuronCount + j);
            }
        }
    }
    
    // Process only the sampled connections
    for (size_t index : connectionIndices) {
        size_t i = index / neuronCount;
        size_t j = index % neuronCount;
        
        Neuron* preNeuron = neurons[i];
        NeuronType preType = preNeuron->getType();
        Neuron* postNeuron = neurons[j];
        
        // Skip invalid source neurons
        if (preType == NeuronType::Modulatory) continue;
        
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
    
    NLM_LOG_INFO(std::to_string(connectionIndices.size()) + 
                 " synapses initialized in region " + getName());
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
