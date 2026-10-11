#include "NeuralRegion.hpp"
#include "BasePimpl.hpp"

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

NeuralRegion::NeuralRegion(RegionId id) : pImpl(Impl(id)) {}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(Impl(id)) {
    pImpl->get()->get()->name = name;
}

NeuralRegion::~NeuralRegion() = default;

NeuralRegion::NeuralRegion(NeuralRegion&&) noexcept = default;

NeuralRegion& NeuralRegion::operator=(NeuralRegion&&) noexcept = default;


RegionId NeuralRegion::getId() const {
    return pImpl->get()->get()->id;
}

const std::string& NeuralRegion::getName() const {
    return pImpl->get()->get()->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->get()->get()->name = name;
}

PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->get()->get()->populations.size() + 1);
    auto population = std::make_unique<NeuralPopulation>(popId, size);
    population->setNeuronType(type);
    pImpl->get()->get()->populations.push_back(std::move(population));
    return popId;
}

NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) {
    if (id.index() == 0 || id.index() > pImpl->get()->get()->populations.size()) {
        return nullptr;
    }
    return pImpl->get()->get()->populations[id.index() - 1].get();
}

const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->get()->get()->populations.size()) {
        return nullptr;
    }
    return pImpl->get()->get()->populations[id.index() - 1].get();
}

size_t NeuralRegion::getPopulationCount() const {
    return pImpl->get()->get()->populations.size();
}

const std::vector<std::unique_ptr<NeuralPopulation>>& NeuralRegion::getPopulations() const {
    return pImpl->get()->get()->populations;
}

std::vector<NeuralPopulation*> NeuralRegion::getAllPopulations() {
    std::vector<NeuralPopulation*> result;
    result.reserve(pImpl->get()->get()->populations.size());
    for (auto& pop : pImpl->get()->get()->populations) {
        result.push_back(pop.get());
    }
    return result;
}

SynapseId NeuralRegion::addSynapse(NeuronId source, NeuronId destination,
                                   SynapticWeight weight, Delay delay) {
    SynapseId synId(pImpl->get()->nextSynapseId++);
    auto synapse = std::make_unique<Synapse>(synId, source, destination);
    synapse->setWeight(weight);
    synapse->setDelay(delay);
    
    pImpl->get()->get()->outgoingSynapses[source].push_back(synId);
    pImpl->get()->get()->incomingSynapses[destination].push_back(synId);
    
    pImpl->get()->get()->synapses.push_back(std::move(synapse));
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->get()->get()->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->get()->get()->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

size_t NeuralRegion::getSynapseCount() const {
    return pImpl->get()->get()->synapses.size();
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->get()->get()->synapses;
}

std::vector<Synapse*> NeuralRegion::getSynapsesFrom(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->get()->get()->outgoingSynapses.find(neuron);
    if (it != pImpl->get()->get()->outgoingSynapses.end()) {
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
    auto it = pImpl->get()->get()->incomingSynapses.find(neuron);
    if (it != pImpl->get()->get()->incomingSynapses.end()) {
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
    for (const auto& pop : pImpl->get()->get()->populations) {
        total += pop->getSize();
    }
    return total;
}

size_t NeuralRegion::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->get()->get()->populations) {
        total += pop->getActiveCount();
    }
    return total;
}

size_t NeuralRegion::getFiringNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->get()->get()->populations) {
        total += pop->getFiringCount();
    }
    return total;
}

float NeuralRegion::getAverageFiringRate() const {
    size_t popCount = pImpl->get()->get()->populations.size();
    if (popCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pop : pImpl->get()->get()->populations) {
        sum += pop->getAverageFiringRate();
    }
    return sum / static_cast<float>(popCount);
}

float NeuralRegion::getAverageSynapticWeight() const {
    if (pImpl->get()->get()->synapses.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto& syn : pImpl->get()->get()->synapses) {
        sum += syn->getWeight();
    }
    return sum / static_cast<float>(pImpl->get()->get()->synapses.size());
}

float NeuralRegion::getSynapticDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    // Maximum possible connections: N * (N-1) for directed graph
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->get()->get()->synapses.size()) / static_cast<float>(maxConnections);
}

void NeuralRegion::step(Timestamp currentTime) {
    // Step all populations
    for (auto& pop : pImpl->get()->get()->populations) {
        pop->step(currentTime);
    }
    
    // Step all synapses
    for (auto& syn : pImpl->get()->get()->synapses) {
        syn->step(currentTime);
    }
}

void NeuralRegion::reset() {
    for (auto& pop : pImpl->get()->get()->populations) {
        pop->reset();
    }
    for (auto& syn : pImpl->get()->get()->synapses) {
        syn->reset();
    }
}

void NeuralRegion::initializeRandomConnectivity(RandomGenerator& rng,
                                               float connectionProbability,
                                               float meanWeight,
                                               float weightVariance) {
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    // Create random connections with improved locality
    for (size_t i = 0; i < neuronCount; ++i) {
        Neuron* preNeuron = neurons[i];
        NeuronType preType = preNeuron->getType();
        
        // Skip if not a proper source neuron
        if (preType == NeuronType::Modulatory) continue;
        
        // Pre-calculate synapse type and weight range based on neuron type
        SynapseType synType;
        float weightMean = meanWeight;
        bool isExcitatory = false;
        
        if (preType == NeuronType::Excitatory || 
            preType == NeuronType::Sensory ||
            preType == NeuronType::Motor ||
            preType == NeuronType::Internal) {
            // excitatory synapse
            synType = SynapseType::Excitatory;
            weightMean = meanWeight;
            isExcitatory = true;
        } else if (preType == NeuronType::Inhibitory) {
            // inhibitory synapse
            synType = SynapseType::Inhibitory;
            weightMean = -meanWeight;
            isExcitatory = false;
        } else {
            synType = SynapseType::Excitatory;
            weightMean = meanWeight;
            isExcitatory = true;
        }
        
        // Process all postsynaptic neurons
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i == j) continue;  // No self-connections
            
            Neuron* postNeuron = neurons[j];
            
            // Probabilistic connection
            if (!rng.bernoulli(connectionProbability)) continue;
            
            // Calculate weight with type-appropriate bounds
            float weight = weightMean + rng.normal(0.0f, weightVariance);
            
            if (isExcitatory) {
                weight = std::max(0.01f, weight);  // Ensure positive for excitatory
            } else {
                weight = std::min(-0.01f, weight);  // Ensure negative for inhibitory
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
    for (auto& pop : pImpl->get()->get()->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

std::vector<const Neuron*> NeuralRegion::getAllNeurons() const {
    std::vector<const Neuron*> result;
    for (const auto& pop : pImpl->get()->get()->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

} // namespace nlm
