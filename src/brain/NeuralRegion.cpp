/**
 * @file NeuralRegion.cpp
 * @brief Implementation of the NeuralRegion class - brain regions with populations and local connectivity
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full region-level dynamics implementation planned for Phase 2.
 */

#include "NeuralRegion.hpp"

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * @struct NeuralRegion::Impl
 * @brief Private implementation details for the NeuralRegion class (Pimpl idiom)
 * @details This struct contains all the actual implementation details of the NeuralRegion class,
 *          including population management, synaptic connectivity, and region statistics. This allows
 *          the NeuralRegion class to be forward-declared and maintains binary compatibility.
 *
 * The implementation includes:
 * - Region identity (ID and name)
 * - Population management (storage and access)
 * - Synapse management (local connections within region)
 * - Connectivity maps for efficient synaptic queries
 * - Synapse ID counter for unique synapse identification
 * - Methods for population and synapse manipulation
 */

struct NeuralRegion::Impl {
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    std::vector<std::unique_ptr<Synapse>> synapses;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;  // source -> synapse ids
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;  // dest -> synapse ids
    SynapseId nextSynapseId;
    
    /**
     * @brief Constructs a region implementation
     * @param id Region identifier
     * @details Initializes the region with the given ID and prepares data structures
     *          for population and synapse management. The region starts empty with
     *          no populations or synapses.
     */
    explicit Impl(RegionId id) : id(id), nextSynapseId(1) {}
};

/**
 * @brief Constructs a neural region with the given ID
 * @param id Unique identifier for the region
 * @details Creates a new neural region instance and initializes its internal state.
 *          The region is created with the specified ID and a default empty name.
 * @pre The region ID should be unique within the brain
 */
NeuralRegion::NeuralRegion(RegionId id) : pImpl(new Impl(id)) {}

/**
 * @brief Constructs a neural region with ID and name
 * @param id Unique identifier for the region
 * @param name Human-readable name for the region
 * @details Creates a new neural region with a descriptive name for identification
 *          and debugging purposes.
 * @pre The region ID should be unique within the brain
 */
NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(new Impl(id)) {
    pImpl->name = name;
}

/**
 * @brief Destructor cleans up region resources
 * @details Deletes the private implementation (pImpl) and cleans up
 *          any allocated resources. This ensures proper memory management.
 */
NeuralRegion::~NeuralRegion() {
    delete pImpl;
}

/**
 * @brief Move constructor - transfers ownership of the implementation
 * @param other The region being moved from
 * @note After move construction, the source region's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers ownership of the implementation
 * @param other The region being moved from
 * @return NeuralRegion& Reference to this region (for chaining)
 * @note After assignment, the source region's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @brief Gets the region's unique identifier
 * @return RegionId The region's unique ID
 */
RegionId NeuralRegion::getId() const {
    return pImpl->id;
}

/**
 * @brief Gets the region's name
 * @return const std::string& The region's name
 */
const std::string& NeuralRegion::getName() const {
    return pImpl->name;
}

/**
 * @brief Sets the region's name
 * @param name New name for the region
 * @details Updates the region's human-readable name. Useful for
 *          debugging, visualization, and region identification.
 */
void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

/**
 * @brief Creates a new neuron population
 * @param size Number of neurons in the population
 * @param type Neuron type for all neurons in the population (default: Internal)
 * @return PopulationId ID of the newly created population
 * @details Creates a new population of neurons with the specified size and type.
 *          Populations are functional units within regions that contain neurons
 *          with similar properties and functions.
 * @warning Population IDs are assigned sequentially starting from 1.
 */
PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->populations.size() + 1);
    auto population = std::make_unique<NeuralPopulation>(popId, size);
    population->setNeuronType(type);
    pImpl->populations.push_back(std::move(population));
    return popId;
}

/**
 * @brief Accesses a neuron population by ID
 * @param id Population ID
 * @return NeuralPopulation* Pointer to the population, or nullptr if not found
 * @details Retrieves the population with the specified ID from this region.
 *          Returns nullptr if the population ID is invalid or doesn't exist.
 */
NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

/**
 * @brief Accesses a neuron population by ID (const version)
 * @param id Population ID
 * @return const NeuralPopulation* Const pointer to the population, or nullptr if not found
 */
const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

/**
 * @brief Gets the number of populations in the region
 * @return size_t Number of populations
 */
 size_t NeuralRegion::getPopulationCount() const {
    return pImpl->populations.size();
}

/**
 * @brief Gets all populations in the region
 * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Reference to internal populations
 * @note Returns a reference to the internal vector of populations. For read-only
 *       access, const version is preferred.
 */
const std::vector<std::unique_ptr<NeuralPopulation>>& NeuralRegion::getPopulations() const {
    return pImpl->populations;
}

/**
 * @brief Gets all population pointers (non-const version)
 * @return std::vector<NeuralPopulation*> Vector of population pointers
 * @details Provides access to population pointers for iteration and modification.
 *          This is useful when direct manipulation of populations is needed.
 */
std::vector<NeuralPopulation*> NeuralRegion::getAllPopulations() {
    std::vector<NeuralPopulation*> result;
    result.reserve(pImpl->populations.size());
    for (auto& pop : pImpl->populations) {
        result.push_back(pop.get());
    }
    return result;
}

/**
 * @brief Creates a synapse between two neurons in this region
 * @param source Neuron ID of the presynaptic neuron
 * @param destination Neuron ID of the postsynaptic neuron
 * @param weight Synaptic weight (default: 0.0)
 * @param delay Synaptic delay in steps (default: 1)
 * @return SynapseId ID of the newly created synapse
 * @details Creates a new synapse connecting two neurons within this region.
 *          The synapse is added to both incoming and outgoing connectivity maps
 *          for efficient access during spike processing.
 * @warning Neurons must exist in the region before creating a synapse.
 * @warning Source and destination neurons must be distinct.
 */
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

/**
 * @brief Accesses a synapse by ID
 * @param id Synapse ID
 * @return Synapse* Pointer to the synapse, or nullptr if not found
 */
Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

/**
 * @brief Accesses a synapse by ID (const version)
 * @param id Synapse ID
 * @return const Synapse* Const pointer to the synapse, or nullptr if not found
 */
const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

/**
 * @brief Gets the number of synapses in the region
 * @return size_t Number of synapses
 */
size_t NeuralRegion::getSynapseCount() const {
    return pImpl->synapses.size();
}

/**
 * @brief Gets all synapses in the region
 * @return const std::vector<std::unique_ptr<Synapse>>& Reference to internal synapses
 */
const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->synapses;
}

/**
 * @brief Gets all synapses originating from a specific neuron
 * @param neuron Neuron ID to find outgoing synapses for
 * @return std::vector<Synapse*> Vector of synapse pointers
 * @details Retrieves all synapses where the specified neuron is the source.
 *          Useful for spike processing and synaptic output management.
 */
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

/**
 * @brief Gets all synapses terminating at a specific neuron
 * @param neuron Neuron ID to find incoming synapses for
 * @return std::vector<Synapse*> Vector of synapse pointers
 * @details Retrieves all synapses where the specified neuron is the destination.
 *          Used for synaptic input processing and spike handling.
 */
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

/**
 * @brief Gets the total number of neurons in the region
 * @return size_t Total neuron count across all populations
 */
size_t NeuralRegion::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getSize();
    }
    return total;
}

/**
 * @brief Gets the number of active neurons in the region
 * @return size_t Number of neurons currently firing or active
 * @details Active neurons are those that are currently firing (spiking)
 *          or in the active state but not yet refractory.
 */
size_t NeuralRegion::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getActiveCount();
    }
    return total;
}

/**
 * @brief Gets the number of firing neurons in the region
 * @return size_t Number of neurons in firing state
 * @details Firing neurons are those that have crossed threshold and are
 *          currently in the firing state.
 */
size_t NeuralRegion::getFiringNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getFiringCount();
    }
    return total;
}

/**
 * @brief Calculates the average firing rate across all populations
 * @return float Average firing rate (Hz)
 * @details Computes the mean firing rate of all populations in the region,
 *          not a global average of individual neuron firing rates.
 */
float NeuralRegion::getAverageFiringRate() const {
    size_t popCount = pImpl->populations.size();
    if (popCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pop : pImpl->populations) {
        sum += pop->getAverageFiringRate();
    }
    return sum / static_cast<float>(popCount);
}

/**
 * @brief Calculates the average synaptic weight in the region
 * @return float Average synaptic weight
 * @details Computes the mean weight of all synapses in the region.
 *          Useful for monitoring network excitability and stability.
 */
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

/**
 * @brief Calculates the synaptic density in the region
 * @return float Synaptic density (fraction of possible connections)
 * @details Computes the ratio of actual synapses to maximum possible connections.
 *          Maximum connections = N * (N-1) for a directed graph where N is
 *          the total number of neurons in the region.
 * @warning Returns 0.0 if region has 0 or 1 neurons (no possible connections).
 */
float NeuralRegion::getSynapticDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    // Maximum possible connections: N * (N-1) for directed graph
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->synapses.size()) / static_cast<float>(maxConnections);
}

/**
 * @brief Steps all neurons and synapses in the region
 * @param currentTime The current simulation time (seconds)
 * @details Updates all neurons and synapses in the region for one simulation step.
 *          This includes:
 *          - Processing neuron LIF dynamics
 *          - Handling spike events
 *          - Updating synaptic transmission
 *          - Managing short-term plasticity
 * @note This is called by SimulationStepCoordinator::executeStep2() and
 *       executeStep7().
 */
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

/**
 * @brief Resets all neurons and synapses to initial state
 * @details Resets all neurons in all populations to their initial states:
 *          - Membrane potential to resting potential
 *          - Firing state to resting
 *          - Spike history cleared
 *          - Refractory counters reset
 *          Similarly resets all synapses to initial state.
 * @note This preserves the region's structure (populations, neurons, synapses)
 *       but clears their dynamic state. Useful for episode termination.
 */
void NeuralRegion::reset() {
    for (auto& pop : pImpl->populations) {
        pop->reset();
    }
    for (auto& syn : pImpl->synapses) {
        syn->reset();
    }
}

/**
 * @brief Initializes random connectivity patterns
 * @param rng Reference to random number generator
 * @param connectionProbability Probability of creating a synapse between neurons
 * @param meanWeight Mean synaptic weight for new synapses
 * @param weightVariance Variance of synaptic weights for new synapses
 * @details Creates random synaptic connections based on neuron types:
 *          - Excitatory neurons connect to all neuron types (excitatory synapses)
 *          - Inhibitory neurons connect to all neuron types (inhibitory synapses)
 *          - Modulatory neurons do not create direct connections
 *          - Self-connections are avoided
 * @note This method implements Phase 1 connectivity initialization.
 *       More sophisticated connectivity patterns will be added in Phase 2.
 */
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

/**
 * @brief Gets all neurons across all populations
 * @return std::vector<Neuron*> Vector of neuron pointers
 * @details Collects all neurons from all populations in the region.
 *          Useful for region-wide operations and statistics.
 */
std::vector<Neuron*> NeuralRegion::getAllNeurons() {
    std::vector<Neuron*> result;
    for (auto& pop : pImpl->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

/**
 * @brief Gets all neurons across all populations (const version)
 * @return std::vector<const Neuron*> Vector of const neuron pointers
 * @details Const version for read-only access to neurons.
 */
std::vector<const Neuron*> NeuralRegion::getAllNeurons() const {
    std::vector<const Neuron*> result;
    for (const auto& pop : pImpl->populations) {
        auto popNeurons = pop->getNeurons();
        result.insert(result.end(), popNeurons.begin(), popNeurons.end());
    }
    return result;
}

} // namespace nlm
