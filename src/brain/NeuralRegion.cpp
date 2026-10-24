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
        NLM_LOG_ERROR("Invalid population ID: " + std::to_string(id.index()));
        return nullptr;
    }
    auto* pop = pImpl->populations[id.index() - 1].get();
    if (!pop) {
        NLM_LOG_ERROR("Population pointer is null for ID: " + std::to_string(id.index()));
    }
    return pop;
}

const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        NLM_LOG_ERROR("Invalid population ID (const): " + std::to_string(id.index()));
        return nullptr;
    }
    const auto* pop = pImpl->populations[id.index() - 1].get();
    if (!pop) {
        NLM_LOG_ERROR("Population pointer is null for ID (const): " + std::to_string(id.index()));
    }
    return pop;
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
    // Validate input parameters
    if (source.index() == 0 || destination.index() == 0) {
        NLM_LOG_ERROR("Cannot add synapse with invalid neuron IDs: source=" + 
                     std::to_string(source.index()) + ", destination=" + std::to_string(destination.index()));
        return SynapseId(0); // Invalid synapse ID
    }
    
    if (weight < -10.0f || weight > 10.0f) {
        NLM_LOG_ERROR("Synapse weight out of valid range: " + std::to_string(weight) + 
                     " (valid range: [-10.0, 10.0])");
        return SynapseId(0);
    }
    
    if (delay < 0) {
        NLM_LOG_ERROR("Synapse delay must be non-negative: " + std::to_string(delay));
        return SynapseId(0);
    }
    
    // Check if source and destination neurons exist
    bool sourceExists = false;
    bool destExists = false;
    
    // Note: In a full implementation, you would have access to the brain's neurons
    // For now, we'll assume the neurons exist based on the IDs being valid
    if (source.index() > 0) sourceExists = true;
    if (destination.index() > 0) destExists = true;
    
    if (!sourceExists) {
        NLM_LOG_ERROR("Source neuron not found for synapse: " + std::to_string(source.index()));
        return SynapseId(0);
    }
    
    if (!destExists) {
        NLM_LOG_ERROR("Destination neuron not found for synapse: " + std::to_string(destination.index()));
        return SynapseId(0);
    }
    
    SynapseId synId(pImpl->nextSynapseId++);
    auto synapse = std::make_unique<Synapse>(synId, source, destination);
    synapse->setWeight(weight);
    synapse->setDelay(delay);
    
    // Validate synapse was created successfully
    if (!synapse) {
        NLM_LOG_ERROR("Failed to create synapse object");
        return SynapseId(0);
    }
    
    pImpl->outgoingSynapses[source].push_back(synId);
    pImpl->incomingSynapses[destination].push_back(synId);
    
    // Validate connection before adding
    if (pImpl->outgoingSynapses.find(source) == pImpl->outgoingSynapses.end() ||
        pImpl->incomingSynapses.find(destination) == pImpl->incomingSynapses.end()) {
        NLM_LOG_ERROR("Failed to create synapse connections");
        return SynapseId(0);
    }
    
    pImpl->synapses.push_back(std::move(synapse));
    
    // Validate synapse was added successfully
    if (pImpl->synapses.empty() || pImpl->synapses.back() == nullptr) {
        NLM_LOG_ERROR("Failed to add synapse to region");
        return SynapseId(0);
    }
    
    NLM_LOG_INFO("Created synapse from " + std::to_string(source.index()) + 
               " to " + std::to_string(destination.index()) + 
               " (ID: " + std::to_string(synId.index()) + 
               ", weight: " + std::to_string(weight) + ")");
    
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            if (!syn) {
                NLM_LOG_ERROR("Synapse pointer is null for ID: " + std::to_string(id.index()));
                return nullptr;
            }
            return syn.get();
        }
    }
    
    NLM_LOG_ERROR("Synapse not found with ID: " + std::to_string(id.index()));
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            if (!syn) {
                NLM_LOG_ERROR("Synapse pointer is null for ID (const): " + std::to_string(id.index()));
                return nullptr;
            }
            return syn.get();
        }
    }
    
    NLM_LOG_ERROR("Synapse not found with ID (const): " + std::to_string(id.index()));
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
            auto* syn = getSynapse(synId);
            if (syn) {
                result.push_back(syn);
            } else {
                NLM_LOG_WARNING("Found invalid synapse reference in outgoingSynapses mapping for neuron " + 
                               std::to_string(neuron.index()));
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
            auto* syn = getSynapse(synId);
            if (syn) {
                result.push_back(syn);
            } else {
                NLM_LOG_WARNING("Found invalid synapse reference in incomingSynapses mapping for neuron " + 
                               std::to_string(neuron.index()));
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
        NLM_LOG_DEBUG("No synapses in region " + std::to_string(pImpl->id.index()) + ", returning 0.0f");
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t validSynapses = 0;
    
    for (const auto& syn : pImpl->synapses) {
        if (!syn) {
            NLM_LOG_WARNING("Found null synapse pointer in region " + 
                           std::to_string(pImpl->id.index()) + ", skipping synapse);
            continue;
        }
        
        float weight = syn->getWeight();
        if (std::isnan(weight) || std::isinf(weight)) {
            NLM_LOG_WARNING("Invalid synapse weight: " + std::to_string(weight) + 
                           " in region " + std::to_string(pImpl->id.index()));
            continue;
        }
        
        sum += weight;
        validSynapses++;
    }
    
    if (validSynapses == 0) {
        NLM_LOG_WARNING("No valid synapses found in region " + std::to_string(pImpl->id.index()));
        return 0.0f;
    }
    
    return sum / static_cast<float>(validSynapses);
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
