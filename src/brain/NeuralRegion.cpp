#include "NeuralRegion.hpp"
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

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

void NeuralRegion::createNeuronTypes(NeuronType type, size_t populationSize) {
    // Create a new population with the specified neuron type
    addPopulation(populationSize, type);
}

void NeuralRegion::balanceNeuronTypes() {
    // Implement balanced neuron type distribution based on RegionConfig
    // This is a placeholder for Phase 2 implementation
}

NeuronType NeuralRegion::getDominantNeuronType() const {
    // Return the neuron type with the largest population
    if (pImpl->populations.empty()) return NeuronType::Internal;
    
    NeuronType dominantType = NeuronType::Internal;
    size_t maxSize = 0;
    
    for (const auto& pop : pImpl->populations) {
        if (pop->getSize() > maxSize) {
            maxSize = pop->getSize();
            dominantType = pop->getNeuronType();
        }
    }
    
    return dominantType;
}

float NeuralRegion::getNeuronTypeRatio(NeuronType type) const {
    // Calculate ratio of neurons of given type to total neurons
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    size_t typeNeurons = 0;
    for (const auto& pop : pImpl->populations) {
        if (pop->getNeuronType() == type) {
            typeNeurons += pop->getSize();
        }
    }
    
    return static_cast<float>(typeNeurons) / static_cast<float>(totalNeurons);
}

void NeuralRegion::setNeuronTypeRatio(NeuronType type, float ratio) {
    // Placeholder for setting neuron type ratios
    // This would require re-balancing populations in a real implementation
}

float NeuralRegion::computeTypeSpecificFiringRates(NeuronType type) const {
    // Compute average firing rate for a specific neuron type
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& pop : pImpl->populations) {
        if (pop->getNeuronType() == type) {
            // Get firing rate from population
            totalFiringRate += pop->getAverageFiringRate();
            neuronCount++;
        }
    }
    
    return neuronCount > 0 ? totalFiringRate / static_cast<float>(neuronCount) : 0.0f;
}

float NeuralRegion::computeTypeSpecificRefractoryPeriods(NeuronType type) const {
    // Compute average refractory period for a specific neuron type
    float totalRefractory = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& pop : pImpl->populations) {
        if (pop->getNeuronType() == type) {
            // Access neurons in population and get their refractory periods
            // This is a placeholder implementation
            totalRefractory += static_cast<float>(pop->getSize() * 5); // Placeholder: 5 steps
            neuronCount += pop->getSize();
        }
    }
    
    return neuronCount > 0 ? totalRefractory / static_cast<float>(neuronCount) : 0.0f;
}

float NeuralRegion::computeTypeSpecificAdaptationStrengths(NeuronType type) const {
    // Compute adaptation strength for a specific neuron type
    float totalAdaptation = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& pop : pImpl->populations) {
        if (pop->getNeuronType() == type) {
            // Access neurons in population and get their adaptation
            // This is a placeholder implementation
            totalAdaptation += static_cast<float>(pop->getSize() * 0.8f); // Placeholder: 0.8 adaptation
            neuronCount += pop->getSize();
        }
    }
    
    return neuronCount > 0 ? totalAdaptation / static_cast<float>(neuronCount) : 0.0f;
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
    for (const auto& syn : pImpl->synapses) {
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

// === BATCH OPERATIONS ===

void NeuralRegion::batchSetSynapseWeights(const std::vector<SynapseId>& synapseIds,
                                         const std::vector<SynapticWeight>& weights) {
    if (synapseIds.size() != weights.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch update with single pass clamping
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->setWeight(weights[i]);
        }
    }
}

void NeuralRegion::batchAddToSynapseWeights(const std::vector<SynapseId>& synapseIds,
                                           const std::vector<SynapticWeight>& deltas) {
    if (synapseIds.size() != deltas.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch update with single pass clamping
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->addToWeight(deltas[i]);
        }
    }
}

void NeuralRegion::batchRecordPreSpikes(const std::vector<SynapseId>& synapseIds,
                                       const std::vector<Timestamp>& timestamps) {
    if (synapseIds.size() != timestamps.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch spike recording
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->recordPreSpike(timestamps[i]);
        }
    }
}

void NeuralRegion::batchRecordPostSpikes(const std::vector<SynapseId>& synapseIds,
                                        const std::vector<Timestamp>& timestamps) {
    if (synapseIds.size() != timestamps.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch spike recording
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->recordPostSpike(timestamps[i]);
        }
    }
}

void NeuralRegion::batchComputeSynapticCurrents(const std::vector<SynapseId>& synapseIds,
                                               const std::vector<NeuronId>& preNeuronIds,
                                               const std::vector<float>& preFiringRates,
                                               std::vector<MembranePotential>& currents) {
    if (synapseIds.size() != preNeuronIds.size() || 
        synapseIds.size() != preFiringRates.size() ||
        currents.size() != synapseIds.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    std::unordered_map<NeuronId, Neuron*> neuronMap;
    
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Get neurons from populations
    auto neurons = getAllNeurons();
    for (auto* neuron : neurons) {
        neuronMap[neuron->getId()] = neuron;
    }
    
    // Vectorized current computation
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto synIt = synapseMap.find(synapseIds[i]);
        auto neuronIt = neuronMap.find(preNeuronIds[i]);
        
        if (synIt != synapseMap.end() && neuronIt != neuronMap.end()) {
            // Synaptic current = weight * pre-synaptic firing rate
            currents[i] = synIt->second->getWeight() * preFiringRates[i];
        }
    }
}

void NeuralRegion::batchEnablePlasticity(const std::vector<SynapseId>& synapseIds,
                                        bool hebbian, bool stdp, bool rewardModulated) {
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch plasticity enable
    for (const auto& synId : synapseIds) {
        auto it = synapseMap.find(synId);
        if (it != synapseMap.end()) {
            it->second->enablePlasticity(hebbian, stdp, rewardModulated);
        }
    }
}

void NeuralRegion::batchSetEligibilityTraces(const std::vector<SynapseId>& synapseIds,
                                             const std::vector<float>& traces) {
    if (synapseIds.size() != traces.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Vectorized eligibility trace setting
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->setEligibilityTrace(traces[i]);
        }
    }
}

void NeuralRegion::batchDecayEligibilityTraces(const std::vector<SynapseId>& synapseIds,
                                               const std::vector<float>& decayRates) {
    if (synapseIds.size() != decayRates.size()) return;
    
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Vectorized eligibility trace decay
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->decayEligibilityTrace(decayRates[i]);
        }
    }
}

void NeuralRegion::batchOptimizeSpikeHistory(const std::vector<SynapseId>& synapseIds) {
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch spike history optimization
    for (const auto& synId : synapseIds) {
        auto it = synapseMap.find(synId);
        if (it != synapseMap.end()) {
            // Convert to circular buffer
            it->second->pImpl->preSpikeHistory.clear();
            it->second->pImpl->postSpikeHistory.clear();
        }
    }
}

void NeuralRegion::batchClearSpikeHistory(const std::vector<SynapseId>& synapseIds) {
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch clear
    for (const auto& synId : synapseIds) {
        auto it = synapseMap.find(synId);
        if (it != synapseMap.end()) {
            it->second->clearHistory();
        }
    }
}

// === LIFECYCLE MANAGEMENT ===

std::vector<SynapseId> NeuralRegion::createBatchSynapses(const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                                      SynapseType type,
                                                      SynapticWeight baseWeight) {
    std::vector<SynapseId> synapseIds;
    synapseIds.reserve(connections.size());
    
    for (const auto& connection : connections) {
        SynapseId synId(pImpl->nextSynapseId++);
        auto synapse = std::make_unique<Synapse>(synId, connection.first, connection.second);
        synapse->setType(type);
        synapse->setWeight(baseWeight);
        
        pImpl->outgoingSynapses[connection.first].push_back(synId);
        pImpl->incomingSynapses[connection.second].push_back(synId);
        
        pImpl->synapses.push_back(std::move(synapse));
        synapseIds.push_back(synId);
    }
    
    return synapseIds;
}

void NeuralRegion::batchInitializeSynapses(const std::vector<SynapseId>& synapseIds,
                                          class RandomGenerator& rng) {
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch initialization
    for (const auto& synId : synapseIds) {
        auto it = synapseMap.find(synId);
        if (it != synapseMap.end()) {
            it->second->initializeRandom(rng);
        }
    }
}

void NeuralRegion::batchDeleteSynapses(const std::vector<SynapseId>& synapseIds) {
    // Optimized lookup using hash map for O(1) access
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& syn : pImpl->synapses) {
        synapseMap[syn->getId()] = syn.get();
    }
    
    // Batch deletion with cleanup
    for (const auto& synId : synapseIds) {
        auto it = synapseMap.find(synId);
        if (it != synapseMap.end()) {
            // Remove from connectivity maps
            for (auto& [source, ids] : pImpl->outgoingSynapses) {
                ids.erase(std::remove(ids.begin(), ids.end(), synId), ids.end());
            }
            for (auto& [dest, ids] : pImpl->incomingSynapses) {
                ids.erase(std::remove(ids.begin(), ids.end(), synId), ids.end());
            }
            
            // Remove from synapses vector
            pImpl->synapses.erase(
                std::remove_if(pImpl->synapses.begin(), pImpl->synapses.end(),
                             [&synId](const std::unique_ptr<Synapse>& syn) {
                                 return syn->getId() == synId;
                             }),
                pImpl->synapses.end()
            );
        }
    }
}

// === STATE QUERY METHODS ===

void NeuralRegion::batchGetSynapseWeights(const std::vector<SynapseId>& synapseIds,
                                         std::vector<SynapticWeight>& weights) const {
    weights.clear();
    weights.reserve(synapseIds.size());
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            weights.push_back(syn->getWeight());
        }
    }
}

float NeuralRegion::batchComputeAverageSynapseWeight(const std::vector<SynapseId>& synapseIds) const {
    if (synapseIds.empty()) return 0.0f;
    
    float sum = 0.0f;
    int count = 0;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            sum += syn->getWeight();
            count++;
        }
    }
    
    return count > 0 ? sum / static_cast<float>(count) : 0.0f;
}

float NeuralRegion::batchComputeSynapseExcitationInhibitionRatio(const std::vector<SynapseId>& synapseIds) const {
    if (synapseIds.empty()) return 0.0f;
    
    int excitatoryCount = 0;
    int inhibitoryCount = 0;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            if (syn->isExcitatory()) {
                excitatoryCount++;
            } else if (syn->isInhibitory()) {
                inhibitoryCount++;
            }
        }
    }
    
    if (excitatoryCount == 0) return 0.0f;
    return static_cast<float>(inhibitoryCount) / static_cast<float>(excitatoryCount);
}

float NeuralRegion::batchComputeSynapseWeightVariance(const std::vector<SynapseId>& synapseIds) const {
    if (synapseIds.size() < 2) return 0.0f;
    
    float mean = batchComputeAverageSynapseWeight(synapseIds);
    float sumSquaredDiff = 0.0f;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            float diff = syn->getWeight() - mean;
            sumSquaredDiff += diff * diff;
        }
    }
    
    return sumSquaredDiff / static_cast<float>(synapseIds.size() - 1);
}

float NeuralRegion::batchComputeSynapseWeightSkewness(const std::vector<SynapseId>& synapseIds) const {
    if (synapseIds.size() < 3) return 0.0f;
    
    float mean = batchComputeAverageSynapseWeight(synapseIds);
    float variance = batchComputeSynapseWeightVariance(synapseIds);
    
    if (variance == 0.0f) return 0.0f;
    
    float sumCubedDiff = 0.0f;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            float diff = syn->getWeight() - mean;
            sumCubedDiff += diff * diff * diff;
        }
    }
    
    float skewness = sumCubedDiff / static_cast<float>(synapseIds.size() - 1);
    skewness /= std::sqrt(variance * variance * variance);
    
    return skewness;
}

std::vector<SynapseId> NeuralRegion::batchGetSynapseNeighborhood(SynapseId centerSynapseId,
                                                             float maxDistance) const {
    std::vector<SynapseId> neighborhood;
    
    // Find center synapse
    Synapse* centerSyn = getSynapse(centerSynapseId);
    if (!centerSyn) return neighborhood;
    
    // Calculate distance for all synapses
    for (const auto& syn : pImpl->synapses) {
        if (syn->getId() == centerSynapseId) continue;  // Skip self
        
        // Calculate distance based on neuron positions
        float distance = calculateDistance(
            centerSyn->getSourceNeuron(), centerSyn->getDestinationNeuron(),
            syn->getSourceNeuron(), syn->getDestinationNeuron()
        );
        
        if (distance <= maxDistance) {
            neighborhood.push_back(syn->getId());
        }
    }
    
    return neighborhood;
}

std::vector<SynapseId> NeuralRegion::batchGetSynapsesByType(SynapseType type) const {
    std::vector<SynapseId> result;
    
    for (const auto& syn : pImpl->synapses) {
        if (syn->getType() == type) {
            result.push_back(syn->getId());
        }
    }
    
    return result;
}

std::vector<SynapseId> NeuralRegion::batchGetSynapsesInWeightRange(SynapticWeight minWeight,
                                                                   SynapticWeight maxWeight) const {
    std::vector<SynapseId> result;
    
    for (const auto& syn : pImpl->synapses) {
        float weight = syn->getWeight();
        if (weight >= minWeight && weight <= maxWeight) {
            result.push_back(syn->getId());
        }
    }
    
    return result;
}

// === DEPENDENCY TRACKING ===

std::vector<SynapseId> NeuralRegion::batchGetSynapsesFromNeuron(NeuronId neuronId) const {
    std::vector<SynapseId> result;
    
    auto it = pImpl->outgoingSynapses.find(neuronId);
    if (it != pImpl->outgoingSynapses.end()) {
        result = it->second;
    }
    
    return result;
}

std::vector<SynapseId> NeuralRegion::batchGetSynapsesToNeuron(NeuronId neuronId) const {
    std::vector<SynapseId> result;
    
    auto it = pImpl->incomingSynapses.find(neuronId);
    if (it != pImpl->incomingSynapses.end()) {
        result = it->second;
    }
    
    return result;
}

std::vector<NeuronId> NeuralRegion::batchGetUniqueSourceNeurons(const std::vector<SynapseId>& synapseIds) const {
    std::vector<NeuronId> neurons;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            NeuronId source = syn->getSourceNeuron();
            if (std::find(neurons.begin(), neurons.end(), source) == neurons.end()) {
                neurons.push_back(source);
            }
        }
    }
    
    return neurons;
}

std::vector<NeuronId> NeuralRegion::batchGetUniqueDestinationNeurons(const std::vector<SynapseId>& synapseIds) const {
    std::vector<NeuronId> neurons;
    
    for (const auto& synId : synapseIds) {
        if (auto* syn = getSynapse(synId)) {
            NeuronId dest = syn->getDestinationNeuron();
            if (std::find(neurons.begin(), neurons.end(), dest) == neurons.end()) {
                neurons.push_back(dest);
            }
        }
    }
    
    return neurons;
}

// === STATISTICS ===

float NeuralRegion::computeRegionalConnectivityDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->synapses.size()) / static_cast<float>(maxConnections);
}

float NeuralRegion::computeRegionalAverageFiringRate() const {
    return getAverageFiringRate();
}

float NeuralRegion::computeRegionalAverageSynapticWeight() const {
    return getAverageSynapticWeight();
}

std::vector<Synapse*> NeuralRegion::getAllSynapses() const {
    std::vector<Synapse*> result;
    result.reserve(pImpl->synapses.size());
    for (const auto& syn : pImpl->synapses) {
        result.push_back(syn.get());
    }
    return result;
}

std::vector<const Synapse*> NeuralRegion::getAllSynapsesPtr() const {
    std::vector<const Synapse*> result;
    result.reserve(pImpl->synapses.size());
    for (const auto& syn : pImpl->synapses) {
        result.push_back(syn.get());
    }
    return result;
}

// === PRIVATE HELPERS ===

Synapse* NeuralRegion::getSynapseById(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

void NeuralRegion::addSynapseInternal(SynapseId id, NeuronId source, NeuronId destination,
                                    SynapticWeight weight, Delay delay) {
    auto synapse = std::make_unique<Synapse>(id, source, destination);
    synapse->setWeight(weight);
    synapse->setDelay(delay);
    
    pImpl->outgoingSynapses[source].push_back(id);
    pImpl->incomingSynapses[destination].push_back(id);
    
    pImpl->synapses.push_back(std::move(synapse));
}

std::vector<SynapseId> NeuralRegion::getAllSynapseIds() const {
    std::vector<SynapseId> ids;
    ids.reserve(pImpl->synapses.size());
    for (const auto& syn : pImpl->synapses) {
        ids.push_back(syn->getId());
    }
    return ids;
}

float NeuralRegion::calculateDistance(NeuronId source1, NeuronId dest1,
                                     NeuronId source2, NeuronId dest2) const {
    // Simplified distance calculation based on neuron IDs
    float d1 = static_cast<float>(source1.index()) - static_cast<float>(source2.index());
    float d2 = static_cast<float>(dest1.index()) - static_cast<float>(dest2.index());
    return std::sqrt(d1 * d1 + d2 * d2);
}

} // namespace nlm
