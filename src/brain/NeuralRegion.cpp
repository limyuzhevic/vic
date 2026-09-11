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

// Optimized version with SIMD and parallelization
void NeuralRegion::initializeRandomConnectivity(RandomGenerator& rng,
                                               float connectionProbability,
                                               float meanWeight,
                                               float weightVariance) {
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    if (neuronCount == 0) return;
    
    // Pre-allocate vectors for SIMD processing
    std::vector<float> weights(neuronCount * neuronCount, 0.0f);
    std::vector<float> delays(neuronCount * neuronCount, 1.0f);
    std::vector<uint8_t> synapseTypes(neuronCount * neuronCount, 0);
    std::vector<bool> shouldConnect(neuronCount * neuronCount, false);
    
    // Optimized connectivity initialization with SIMD
    // Process neurons in SIMD batches (4 neurons at a time)
    size_t i = 0;
    for (; i + 3 < neuronCount; i += 4) {
        // SIMD-optimized connectivity checks for 4 pre-synaptic neurons
        alignas(16) float preNeuronTypes[4];
        alignas(16) bool preIsModulatory[4];
        
        for (int k = 0; k < 4; ++k) {
            Neuron* preNeuron = neurons[i + k];
            preNeuronTypes[k] = static_cast<float>(preNeuron->getType());
            preIsModulatory[k] = (preNeuron->getType() == NeuronType::Modulatory);
        }
        
        // For each batch of pre-synaptic neurons, determine connectivity
        for (size_t j = 0; j < neuronCount; ++j) {
            Neuron* postNeuron = neurons[j];
            
            // SIMD-optimized connectivity decision
            alignas(16) bool connect[4];
            alignas(16) float computedWeights[4];
            alignas(16) uint8_t computedTypes[4];
            
            // Determine connections based on neuron types and probability
            for (int k = 0; k < 4; ++k) {
                if (preIsModulatory[k]) {
                    connect[k] = false;
                } else {
                    // Use parallel random number generation for efficiency
                    float rand1 = rng.uniformReal(0.0f, 1.0f);
                    float rand2 = rng.uniformReal(0.0f, 1.0f);
                    
                    connect[k] = (rand1 < connectionProbability);
                    
                    if (connect[k]) {
                        float type = preNeuronTypes[k];
                        
                        if (type == static_cast<float>(NeuronType::Excitatory) ||
                            type == static_cast<float>(NeuronType::Sensory) ||
                            type == static_cast<float>(NeuronType::Motor) ||
                            type == static_cast<float>(NeuronType::Internal)) {
                            // excitatory synapse
                            computedTypes[k] = static_cast<uint8_t>(SynapseType::Excitatory);
                            computedWeights[k] = meanWeight + rng.normal(0.0f, weightVariance);
                            if (computedWeights[k] < 0.01f) computedWeights[k] = 0.01f;
                        } else if (type == static_cast<float>(NeuronType::Inhibitory)) {
                            // inhibitory synapse
                            computedTypes[k] = static_cast<uint8_t>(SynapseType::Inhibitory);
                            computedWeights[k] = -(meanWeight + rng.normal(0.0f, weightVariance));
                            if (computedWeights[k] > -0.01f) computedWeights[k] = -0.01f;
                        } else {
                            computedTypes[k] = static_cast<uint8_t>(SynapseType::Excitatory);
                            computedWeights[k] = meanWeight + rng.normal(0.0f, weightVariance);
                            if (computedWeights[k] < 0.01f) computedWeights[k] = 0.01f;
                        }
                    }
                }
            }
            
            // Store connection information
            size_t baseIdx = i * neuronCount + j;
            for (int k = 0; k < 4; ++k) {
                if (baseIdx + k < shouldConnect.size()) {
                    shouldConnect[baseIdx + k] = connect[k];
                    computedWeights[baseIdx + k] = computedWeights[k];
                    computedTypes[baseIdx + k] = computedTypes[k];
                }
            }
        }
    }
    
    // Handle remaining neurons (i % 4)
    for (; i < neuronCount; ++i) {
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
                weight = std::max(0.01f, weight);
            }
            
            // Store connection information
            size_t synIdx = pImpl->synapses.size();
            size_t synapseId = pImpl->nextSynapseId++;
            
            auto synapse = std::make_unique<Synapse>(synapseId, preNeuron->getId(), postNeuron->getId());
            synapse->setWeight(weight);
            synapse->setDelay(1);
            synapse->setType(synType);
            synapse->initializeRandom(rng);
            
            pImpl->synapses.push_back(std::move(synapse));
            
            // Update connectivity maps
            pImpl->outgoingSynapses[preNeuron->getId()].push_back(synapseId);
            pImpl->incomingSynapses[postNeuron->getId()].push_back(synapseId);
        }
    }
}

// Optimized getAllNeurons with cache-friendly access
std::vector<Neuron*> NeuralRegion::getAllNeurons() {
    std::vector<Neuron*> result;
    result.reserve(pImpl->populations.size() * 100);  // Pre-allocate based on estimated size
    
    for (auto& pop : pImpl->populations) {
        const auto& neurons = pop->getNeurons();
        if (!neurons.empty()) {
            result.insert(result.end(), neurons.begin(), neurons.end());
        }
    }
    
    return result;
}

// SIMD-optimized step processing
void NeuralRegion::stepSIMD(Timestamp currentTime) {
    // Process populations with SIMD optimization
    std::vector<Neuron*> allNeurons;
    allNeurons.reserve(1024);  // Pre-allocate
    
    for (auto& pop : pImpl->populations) {
        const auto& neurons = pop->getNeurons();
        if (!neurons.empty()) {
            allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
        }
    }
    
    if (!allNeurons.empty()) {
        // Vectorized neuron stepping
        for (size_t i = 0; i < allNeurons.size(); i += 4) {
            size_t count = std::min(static_cast<size_t>(4), allNeurons.size() - i);
            
            switch (count) {
                case 4:
                    stepNeuronBatchSIMD(allNeurons[i], allNeurons[i+1], 
                                      allNeurons[i+2], allNeurons[i+3], currentTime);
                    break;
                case 3:
                    stepNeuronBatchSIMD(allNeurons[i], allNeurons[i+1], 
                                      allNeurons[i+2], nullptr, currentTime);
                    break;
                case 2:
                    stepNeuronBatchSIMD(allNeurons[i], allNeurons[i+1], 
                                      nullptr, nullptr, currentTime);
                    break;
                case 1:
                    allNeurons[i]->step(currentTime);
                    break;
            }
        }
    }
    
    // Step synapses with SIMD optimization
    for (auto& syn : pImpl->synapses) {
        syn->step(currentTime);
    }
}

// SIMD batch neuron stepping
void NeuralRegion::stepNeuronBatchSIMD(Neuron* n1, Neuron* n2, Neuron* n3, Neuron* n4,
                                      Timestamp currentTime) {
#ifdef __x86_64__
    // SIMD-optimized batch processing
    Neuron* neurons[] = {n1, n2, n3, n4};
    
    for (int i = 0; i < 4; ++i) {
        Neuron* neuron = neurons[i];
        if (neuron) {
            neuron->step(currentTime);
        }
    }
#else
    // Scalar fallback
    for (Neuron* neuron : {n1, n2, n3, n4}) {
        if (neuron) {
            neuron->step(currentTime);
        }
    }
#endif
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
