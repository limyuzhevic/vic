/**
 * @file NeuralPopulation.cpp
 * @brief Implementation of the NeuralPopulation class - groups neurons with shared properties and functions
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full population-level dynamics implementation planned for Phase 2.
 */

#include "NeuralPopulation.hpp"

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

#include <algorithm>
#include <numeric>

namespace nlm {

/**
 * @struct NeuralPopulation::Impl
 * @brief Private implementation details for the NeuralPopulation class (Pimpl idiom)
 * @details This struct contains all the actual implementation details of the NeuralPopulation class,
 *          including neuron storage, population statistics, and neuron management. This allows
 *          the NeuralPopulation class to be forward-declared and maintains binary compatibility.
 *
 * The implementation includes:
 * - Population identity (ID and size)
 * - Neuron storage (vector of neuron pointers)
 * - Neuron type (shared across all neurons in the population)
 * - Statistics storage (membrane potentials for activity tracking)
 * - Methods for neuron management and statistics calculation
 */

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    /**
     * @brief Constructs a population implementation
     * @param id Population identifier
     * @param size Number of neurons in the population
     * @details Initializes the population with the given ID and size.
     *          The neuron type defaults to Internal.
     */
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

/**
 * @brief Constructs a neural population with ID and size
 * @param id Unique identifier for the population
 * @param size Number of neurons in the population
 * @details Creates a new neural population instance and initializes its internal state.
 *          The population starts with empty neuron vector and default Internal type.
 * @pre The population ID should be unique within its region
 * @warning Population IDs are assigned sequentially starting from 1.
 */
NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {}

/**
 * @brief Destructor cleans up population resources
 * @details Deletes the private implementation (pImpl) and cleans up
 *          any allocated resources. This ensures proper memory management.
 */
NeuralPopulation::~NeuralPopulation() {
    delete pImpl;
}

/**
 * @brief Move constructor - transfers ownership of the implementation
 * @param other The population being moved from
 * @note After move construction, the source population's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
NeuralPopulation::NeuralPopulation(NeuralPopulation&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * @brief Move assignment operator - transfers ownership of the implementation
 * @param other The population being moved from
 * @return NeuralPopulation& Reference to this population (for chaining)
 * @note After assignment, the source population's pImpl is set to nullptr
 *       to prevent double deletion. This is efficient for transferring ownership.
 */
NeuralPopulation& NeuralPopulation::operator=(NeuralPopulation&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * @brief Gets the population's unique identifier
 * @return PopulationId The population's unique ID
 */
PopulationId NeuralPopulation::getId() const {
    return pImpl->id;
}

/**
 * @brief Gets the number of neurons in the population
 * @return size_t Number of neurons
 */
size_t NeuralPopulation::getSize() const {
    return pImpl->size;
}

/**
 * @brief Checks if the population is empty
 * @return bool true if population contains no neurons, false otherwise
 */
bool NeuralPopulation::isEmpty() const {
    return pImpl->neurons.empty();
}

/**
 * @brief Accesses a neuron by index
 * @param index Index of the neuron to retrieve (0-based)
 * @return Neuron* Pointer to the neuron, or nullptr if index is invalid
 * @details Retrieves the neuron at the specified index. Returns nullptr if
 *          the index is out of bounds.
 * @warning Indices must be within [0, size()-1].
 */
Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

/**
 * @brief Accesses a neuron by index (const version)
 * @param index Index of the neuron to retrieve (0-based)
 * @return const Neuron* Const pointer to the neuron, or nullptr if index is invalid
 */
const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

/**
 * @brief Adds a neuron to the population
 * @param neuron Pointer to the neuron to add
 * @details Adds the specified neuron to the population. The neuron is stored
 *          and becomes part of the population's collective dynamics.
 * @pre The neuron should not already be in a different population
 * @warning Neurons should be properly initialized before adding to population.
 */
void NeuralPopulation::addNeuron(Neuron* neuron) {
    pImpl->neurons.push_back(neuron);
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
}

/**
 * @brief Gets all neurons in the population
 * @return const std::vector<Neuron*>& Reference to internal neuron vector
 * @note Returns a reference to the internal vector of neurons. For read-only
 *       access, const version is preferred.
 */
const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return pImpl->neurons;
}

/**
 * @brief Gets the neuron type for all neurons in the population
 * @return NeuronType The neuron type (all neurons share the same type)
 * @details Returns the type that all neurons in the population have.
 *          This determines how the population processes information.
 * @note All neurons in a population must have the same type.
 */
NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

/**
 * @brief Sets the neuron type for all neurons in the population
 * @param type Neuron type to set for all neurons
 * @details Updates the type of all neurons in the population. This affects
 *          how the population processes inputs and generates outputs.
 * @note All neurons in a population must be of the same type.
 * @see NeuronType enum for available types
 */
void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto* neuron : pImpl->neurons) {
        neuron->setType(type);
    }
}

/**
 * @brief Gets the number of active neurons in the population
 * @return size_t Number of neurons currently firing or active
 * @details Active neurons are those that are currently firing (spiking)
 *          or in the active state but not yet refractory.
 */
size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

/**
 * @brief Gets the number of firing neurons in the population
 * @return size_t Number of neurons in firing state
 * @details Firing neurons are those that have crossed threshold and are
 *          currently in the firing state.
 */
size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

/**
 * @brief Calculates the average firing rate across all neurons
 * @return float Average firing rate (Hz)
 * @details Computes the mean firing rate of all neurons in the population,
 *          not a global average of individual neuron firing rates.
 */
float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto* neuron : pImpl->neurons) {
        sum += neuron->getFiringRate();
    }
    return sum / static_cast<float>(pImpl->neurons.size());
}

/**
 * @brief Gets the activity level of the population
 * @return float Activity level as fraction of active neurons (0.0 to 1.0)
 * @details Computes the ratio of active neurons to total neurons in the population.
 *          This is a normalized measure of population activity.
 * @warning Returns 0.0 if population is empty.
 */
float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neurons.size());
}

/**
 * @brief Initializes all neurons with random parameters
 * @param rng Reference to random number generator
 * @details Sets up all neurons in the population with biologically plausible parameters:
 *          - Membrane potential near resting potential
 *          - Firing threshold at typical values (-55 mV)
 *          - Resting potential at typical values (-70 mV)
 *          - Leak conductance and refractory period within biological ranges
 *          - Initial firing state and spike history
 * @note This is used during network initialization to create realistic neurons.
 */
void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
}

/**
 * @brief Steps all neurons in the population
 * @param currentTime The current simulation time (seconds)
 * @details Updates all neurons in the population for one simulation step:
 *          - Processes neuron LIF dynamics
 *          - Handles spike events
 *          - Updates synaptic transmission
 *          - Manages short-term plasticity
 * @note This is called by NeuralRegion::step() for all populations.
 */
void NeuralPopulation::step(Timestamp currentTime) {
    for (auto* neuron : pImpl->neurons) {
        neuron->step(currentTime);
    }
}

/**
 * @brief Resets all neurons to initial state
 * @details Resets all neurons in the population to their initial states:
 *          - Membrane potential to resting potential
 *          - Firing state to resting
 *          - Spike history cleared
 *          - Refractory counters reset
 * @note Clears membrane potentials for statistics and resets activity tracking.
 */
void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
}

/**
 * @brief Gets all neuron IDs in the population
 * @return std::vector<NeuronId> Vector of neuron IDs
 * @details Collects the IDs of all neurons in the population.
 *          Used for connectivity queries and debugging.
 */
std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

/**
 * @brief Records current activity for statistics
 * @details Records the current membrane potential of all neurons for later statistical analysis.
 *          This is used for computing population-level metrics like mean and variance
 *          of membrane potentials.
 * @note Called during step processing to maintain activity statistics.
 */
void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        pImpl->membranePotentials.push_back(neuron->getMembranePotential());
    }
}

/**
 * @brief Gets the mean membrane potential of all neurons
 * @return float Mean membrane potential in millivolts (mV)
 * @details Computes the average membrane potential across all neurons in the population.
 *          Useful for monitoring population-level excitability.
 */
float NeuralPopulation::getMeanMembranePotential() const {
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(pImpl->membranePotentials.begin(), 
                                 pImpl->membranePotentials.end(), 0.0f);
    return sum / static_cast<float>(pImpl->membranePotentials.size());
}

/**
 * @brief Gets the variance of membrane potential across neurons
 * @return float Variance of membrane potentials in mV²
 * @details Measures the spread of membrane potentials across neurons in the population.
 *          Higher variance indicates greater diversity in neuronal states.
 */
float NeuralPopulation::getVarianceMembranePotential() const {
    if (pImpl->membranePotentials.size() < 2) {
        return 0.0f;
    }
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : pImpl->membranePotentials) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(pImpl->membranePotentials.size() - 1);
}

} // namespace nlm
