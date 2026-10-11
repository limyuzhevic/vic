#include "NeuralRegion.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * 
 * **Neural Region Dynamics and Connectivity**
 * =======================================
 * 
 * This file implements the neural region class, representing anatomical brain regions
 * that contain multiple neural populations and implement local connectivity patterns.
 * Neural regions are fundamental organizational units of the brain that mirror
 * biological structures like cortical areas, subcortical nuclei, and neural circuits.
 * 
 * **Region-Level Computation**
 * ---------------------------
 * Neural regions implement two primary functions:
 * 1. **Population Management**: Organize neurons by functional type and properties
 * 2. **Local Connectivity**: Establish synaptic connections within the region
 * 
 * The region acts as both a container and a computational unit, implementing
 * parallel processing through population-level operations.
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
     * 
     * **Region Identity and Classification**
     * ---------------------------------
     * 
     * **id**: Unique identifier for the region within the brain
     * **name**: Human-readable name for the region (e.g., "Visual Cortex", "Motor Cortex")
     * **populations**: Collection of neural populations organized by neuron type
     * 
     * **Region Types and Functions**:
     * - **Sensory Regions**: Detect and encode external/internal stimuli
     * - **Internal Regions**: Process, store, and integrate information
     * - **Motor Regions**: Generate appropriate responses
     * - **Association Regions**: Integrate information across multiple modalities
     * - **Modulatory Regions**: Regulate plasticity and excitability
     * 
     * **Architectural Significance**:
     * - Hierarchical organization (regions -> populations -> neurons)
     * - Functional specialization within regions
     * - Boundary constraints for local connectivity
     * - Integration points for inter-regional communication
     */
    
    explicit Impl(RegionId id) : id(id), nextSynapseId(1) {}
};

/**
 * 
 * **Neural Region Construction**
 * ----------------------------
 * 
 * **NeuralRegion(RegionId id)**
 * ---------------------------
 * 
 * Creates a basic neural region with only an identifier.
 * 
 * **Purpose**: Internal representation and region creation
 * 
 * **Parameters**:
 * - id: Unique region identifier
 * 
 * **Usage**:
 * - Used by Brain for internal region management
 * - Minimal overhead for region creation
 * 
 * **NeuralRegion(RegionId id, const std::string& name)**
 * -------------------------------------------
 * 
 * Creates a neural region with identifier and name.
 * 
 * **Purpose**: Human-readable region identification
 * 
 * **Parameters**:
 * - id: Unique region identifier
 * - name: Human-readable region name
 * 
 * **Usage**:
 * - User-facing interfaces
 * - Debugging and visualization
 * - Configuration management
 * 
 * **Construction Sequence**:
 * 1. Create implementation structure with identifier
 * 2. Assign human-readable name (if provided)
 * 3. Initialize empty containers for populations and synapses
 * 4. Set up indexing structures for efficient connectivity queries
 */

NeuralRegion::NeuralRegion(RegionId id) : pImpl(new Impl(id)) {}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(new Impl(id)) {
    pImpl->name = name;
}

/**
 * 
 * **Region Memory Management**
 * ---------------------------
 * 
 * **Destructor**: Releases implementation memory
 * 
 * The pImpl idiom ensures proper cleanup of dynamically allocated
 * implementation structure.
 */

NeuralRegion::~NeuralRegion() {
    delete pImpl;
}

/**
 * 
 * **Region Move Semantics**
 * ------------------------
 * 
 * **Move Constructor**: Efficient transfer of region implementation
 * - Transfers ownership of pImpl
 * - Leaves source with nullptr (ownership transferred)
 * 
 * **Move Assignment**: Assignment with cleanup
 * - Deletes current implementation
 * - Transfers ownership from source
 * - Source becomes null (ownership transferred)
 */

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

/**
 * 
 * **Region Identity Accessors**
 * ----------------------------
 * 
 * **getId()**: Returns the region's unique identifier
 * 
 * **Returns**: RegionId
 * 
 * **Usage**:
 * - Region identification in connectivity queries
 * - Debugging and logging
 * - Network statistics
 * 
 * **getName()**: Returns the region's human-readable name
 * 
 * **Returns**: const std::string&
 * 
 * **setName(const std::string& name)**: Sets the region's name
 * 
 * **Parameters**:
 * - name: New region name
 * 
 * **Usage**:
 * - User interfaces
 * - Configuration management
 * - Visualization
 */

RegionId NeuralRegion::getId() const {
    return pImpl->id;
}

const std::string& NeuralRegion::getName() const {
    return pImpl->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

/**
 * 
 * **Population Management**
 * ------------------------
 * 
 * **addPopulation(size_t size, NeuronType type)**: Creates a new neural population
 * 
 * **Population Creation Sequence**:
 * 1. Generate unique population ID based on current count
 * 2. Create NeuralPopulation object with ID and size
 * 3. Set neuron type for the population
 * 4. Move population into region's collection
 * 5. Return population ID
 * 
 * **Parameters**:
 * - size: Number of neurons in the population
 * - type: Neuron type for all neurons in the population
 * 
 * **Population Types**:
 * - **Sensory Populations**: Encode external/internal stimuli
 * - **Internal Populations**: Process and integrate information
 * - **Motor Populations**: Generate motor commands
 * - **Specialized Populations**: Implement specific computational functions
 * 
 * **Returns**:
 * PopulationId: Unique identifier for the new population
 * 
 * **Usage**:
 * - Brain initialization for region composition
 * - Dynamic population creation during development
 * - Experimental manipulation of network composition
 */

PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->populations.size() + 1);
    auto population = std::make_unique<NeuralPopulation>(popId, size);
    population->setNeuronType(type);
    pImpl->populations.push_back(std::move(population));
    return popId;
}

/**
 * 
 * **Population Accessors**
 * -----------------------
 * 
 * **getPopulation(PopulationId id)**: Returns population by ID
 * 
 * **Parameters**:
 * - id: Population identifier
 * 
 * **Returns**:
 * NeuralPopulation*: Pointer to population or nullptr if not found
 * 
 * **const NeuralPopulation* getPopulation(PopulationId id) const**: Const version
 * 
 * **Population ID Scheme**:
 * - ID 0: Invalid/null population
 * - ID 1..N: Sequential IDs for created populations
 * - IDs are stable throughout population lifetime
 * 
 * **Error Handling**:
 * - Returns nullptr for invalid IDs
 * - Prevents out-of-bounds access
 * 
 * **Usage**:
 * - Targeted population manipulation
 * - Population-specific operations
 * - Debugging and monitoring
 */

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

/**
 * 
 * **Population Statistics**
 * ------------------------
 * 
 * **getPopulationCount()**: Returns number of populations
 * 
 * **Returns**:
 * size_t: Number of populations in region
 * 
 * **getPopulations()**: Returns all populations
 * 
 * **Returns**:
 * const std::vector<std::unique_ptr<NeuralPopulation>>&: All populations
 * 
 * **getAllPopulations()**: Returns all populations as pointers
 * 
 * **Returns**:
 * std::vector<NeuralPopulation*>: All populations as raw pointers
 * 
 * **Use Case**:
 * - Region-wide operations (e.g., step all populations)
 * - Batch processing across populations
 * - Statistical aggregation across populations
 */

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

/**
 * 
 * **Synapse Management**
 * ---------------------
 * 
 * **addSynapse(NeuronId source, NeuronId destination, SynapticWeight weight, Delay delay)**
 * -------------------------------------------------------------------------------
 * 
 * Creates a synaptic connection between two neurons.
 * 
 * **Synapse Creation Sequence**:
 * 1. Generate unique synapse ID based on current count
 * 2. Create Synapse object with source and destination neurons
 * 3. Set weight and delay parameters
 * 4. Update indexing structures for connectivity queries
 * 5. Add synapse to region's synapse collection
 * 6. Return synapse ID
 * 
 * **Parameters**:
 * - source: Pre-synaptic neuron identifier
 * - destination: Post-synaptic neuron identifier
 * - weight: Synaptic weight (positive=excitatory, negative=inhibitory)
 * - delay: Transmission delay in simulation steps
 * 
 * **Indexing Updates**:
 * - Update outgoingSynapses map for source neuron
 * - Update incomingSynapses map for destination neuron
 * 
 * **Returns**:
 * SynapseId: Unique identifier for the new synapse
 * 
 * **Usage**:
 * - Direct connectivity specification
 * - Experimental manipulation of connections
 * - Testing specific circuit motifs
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
 * 
 * **Synapse Accessors**
 * --------------------
 * 
 * **getSynapse(SynapseId id)**: Returns synapse by ID
 * 
 * **Parameters**:
 * - id: Synapse identifier
 * 
 * **Returns**:
 * Synapse*: Pointer to synapse or nullptr if not found
 * 
 * **const Synapse* getSynapse(SynapseId id) const**: Const version
 * 
 * **Synapse ID Scheme**:
 * - IDs are assigned sequentially (1, 2, 3, ...)
 * - IDs are stable throughout synapse lifetime
 * - No ID reuse to prevent conflicts
 * 
 * **Search Algorithm**:
 * - Linear search through synapse collection
 * - Fast lookup for small-to-medium regions
 * - Could be optimized for large regions with hash tables
 * 
 * **Error Handling**:
 * - Returns nullptr for invalid IDs
 * - Prevents null pointer dereference
 * 
 * **Usage**:
 * - Targeted synapse manipulation
 * - Synapse-specific operations
 * - Debugging and monitoring
 */

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

/**
 * 
 * **Synapse Statistics**
 * ---------------------
 * 
 * **getSynapseCount()**: Returns total synapse count
 * 
 * **Returns**:
 * size_t: Total synapses in region
 * 
 * **getSynapses()**: Returns all synapses
 * 
 * **Returns**:
 * const std::vector<std::unique_ptr<Synapse>>&: All synapses
 * 
 * **getSynapsesFrom(NeuronId neuron)**: Returns outgoing synapses
 * 
 * **Parameters**:
 * - neuron: Source neuron identifier
 * 
 * **Returns**:
 * std::vector<Synapse*>: All synapses from the neuron
 * 
 * **getSynapsesTo(NeuronId neuron)**: Returns incoming synapses
 * 
 * **Parameters**:
 * - neuron: Destination neuron identifier
 * 
 * **Returns**:
 * std::vector<Synapse*>: All synapses to the neuron
 * 
 * **Indexing Strategy**:
 * - outgoingSynapses: Fast lookup for source neurons
 * - incomingSynapses: Fast lookup for destination neurons
 * - Direct mapping avoids linear search
 * - Enables efficient connectivity queries
 */

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

/**
 * 
 * **Neuron Statistics**
 * ---------------------
 * 
 * **getTotalNeuronCount()**: Returns total neuron count
 * 
 * **Returns**:
 * size_t: Total neurons in all populations
 * 
 * **getActiveNeuronCount()**: Returns active neuron count
 * 
 * **Returns**:
 * size_t: Neurons with membrane potential != resting potential
 * 
 * **getFiringNeuronCount()**: Returns firing neuron count
 * 
 * **Returns**:
 * size_t: Neurons that fired in current step
 * 
 * **getAverageFiringRate()**: Returns average firing rate
 * 
 * **Returns**:
 * float: Average firing rate in Hz across all populations
 * 
 * **Statistical Aggregation**:
 * - Sum across all populations
 * - Normalized by population count
 * - Enables region-wide analysis
 * 
 * **Usage**:
 * - Network activity monitoring
 * - Performance evaluation
 * - Research analysis
 */

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

/**
 * 
 * **Local Connectivity Statistics**
 * -------------------------------
 * 
 * **getAverageSynapticWeight()**: Returns average synaptic weight
 * 
 * **Returns**:
 * float: Average weight across all synapses
 * - Positive values: Net excitation
 * - Negative values: Net inhibition
 * - Zero: Balanced excitation/inhibition
 * 
 * **getSynapticDensity()**: Returns connection density
 * 
 * **Returns**:
 * float: Fraction of possible connections that are realized
 * - Range: 0.0 (no connections) to 1.0 (complete graph)
 * - Higher density: More parallel processing
 * - Lower density: Sparse coding efficiency
 * 
 * **Density Calculation**:
 * - Maximum connections: N × (N-1) for directed graph
 * - Actual connections: Current synapse count
 * - Density = actual / maximum
 * 
 * **Biological Significance**:
 * - Sparse connectivity in biological networks (density ~0.01-0.1)
 * - Energy efficiency through reduced wiring
 * - Robustness through distributed processing
 * - Computational capacity through specific connection patterns
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

float NeuralRegion::getSynapticDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    // Maximum possible connections: N * (N-1) for directed graph
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->synapses.size()) / static_cast<float>(maxConnections);
}

/**
 * 
 * **Simulation Update**
 * --------------------
 * 
 * **step(Timestamp currentTime)**: Updates region dynamics for one timestep
 * 
 * **Update Sequence**:
 * 1. **Update populations**: Step all neural populations
 * 2. **Update synapses**: Step all synaptic connections
 * 3. **Order matters**: Populations first, then synapses (spike timing)
 * 
 * **Population Processing**:
 * - Each population steps all its neurons
 * - Implements local neural dynamics
 * - Updates membrane potentials, spikes, state variables
 * 
 * **Synaptic Processing**:
 * - Each synapse steps its dynamics
 * - Implements synaptic plasticity and short-term effects
 * - Updates eligibility traces and efficacy
 * 
 * **Timing Considerations**:
 * - Populations first: Neurons compute their outputs
 * - Synapses second: Outputs are transmitted and integrated
 * - Ensures correct spike timing and causal ordering
 * 
 * **Parameters**:
 * - currentTime: Current simulation time in milliseconds
 * 
 * **Use Cases**:
 * - Main brain simulation loop
 * - Region-level testing
 * - Performance benchmarking
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
 * 
 * **Region Reset**
 * ------------
 * 
 * **reset()**: Resets region to initial state
 * 
 * **Reset Sequence**:
 * 1. **Reset populations**: Clear all neural state
 * 2. **Reset synapses**: Clear all synaptic state
 * 3. **Maintain structure**: Populations and synapses remain
 * 
 * **Population Reset**:
 * - Clear membrane potentials
 * - Reset firing states
 * - Clear history variables
 * - Restore initial conditions
 * 
 * **Synaptic Reset**:
 * - Clear weights
 * - Reset spike histories
 * - Clear eligibility traces
 * - Reset efficacy variables
 * 
 * **Use Cases**:
 * - Learning episode boundaries
 * - Experiment restart
 * - State restoration
 * - Error recovery
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
 * 
 * **Connectivity Initialization**
 * ----------------------------
 * 
 * **initializeRandomConnectivity(RandomGenerator& rng, float connectionProbability, float meanWeight, float weightVariance)**
 * -------------------------------------------------------------------------------------------------------------------
 * 
 * **Algorithm Overview**
 * -------------------
 * Implements biologically realistic random connectivity generation with the following properties:
 * 
 * 1. **Type-specific connectivity**: Different neuron types connect preferentially
 * 2. **Probabilistic wiring**: Connections established with specified probability
 * 3. **Weight distribution**: Weights drawn from Gaussian distribution
 * 4. **No self-connections**: Neurons don't connect to themselves
 * 5. **Synaptic properties**: Type and parameters set based on connectivity rules
 * 
 * **Biological Plausibility Features**:
 * - Excitatory neurons connect to most neuron types
 * - Inhibitory neurons provide broad control
 * - Sparse connectivity (~10% typical)
 * - Activity-dependent connectivity rules
 * - Realistic weight distributions
 * 
 * **Mathematical Formulation**:
 * 
 * For each neuron pair (i, j) where i ≠ j:
 * 
 * 1. **Connection probability**: P(connect) = connectionProbability
 * 2. **If connected**:
 *    a. **Source neuron type determines target type**:
 *       - Excitatory source → Excitatory/Inhibitory/Internal/Motor/Sensory
 *       - Inhibitory source → All neuron types
 *    b. **Weight sampling**:
 *       - Excitatory: weight ~ N(meanWeight, weightVariance)
 *       - Inhibitory: weight ~ -N(meanWeight, weightVariance)
 *       - Other types: weight ~ N(meanWeight, weightVariance)
 *    c. **Constraint enforcement**:
 *       - Excitatory: weight ≥ 0.01 (positive)
 *       - Inhibitory: weight ≤ -0.01 (negative)
 *    d. **Synaptic initialization**:
 *       - Set type, delay, and random properties
 * 
 * **Algorithm Complexity**:
 * - Time: O(N²) where N is neuron count
 * - Space: O(N²) for connectivity storage
 * - Optimized for sparse connectivity
 * 
 * **Parameters**:
 * - **connectionProbability**: Probability of connection (0.0-1.0)
 * - **meanWeight**: Average synaptic weight magnitude
 * - **weightVariance**: Standard deviation of weights
 * 
 * **Typical Values**:
 * - connectionProbability: 0.05-0.15 (5-15% connectivity)
 * - meanWeight: 0.2-0.4 (typical synaptic strength)
 * - weightVariance: 0.05-0.1 (biological variability)
 * 
 * **Use Cases**:
 * - Initial network setup
 * - Developmental connectivity patterns
 * - Experimental manipulation of connectivity
 * - Benchmarking and testing
 * 
 * **Performance Considerations**:
 * - Efficient for small-to-medium regions (N < 10,000)
 * - Sparse connectivity improves performance
 * - Parallelizable for large-scale regions
 * - Memory-efficient with probabilistic generation
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
 * 
 * **Neuron Accessors**
 * -------------------
 * 
 * **getAllNeurons()**: Returns all neurons as pointers
 * 
 * **Returns**:
 * std::vector<Neuron*>: All neurons in the region
 * 
 * **Usage**:
 * - Connectivity initialization (region-wide operations)
 * - Simulation setup
 * - Global operations across region
 * 
 * **Algorithm**:
 * - Iterate through all populations
 * - Collect neurons from each population
 * - Flatten into single vector
 * - Return as raw pointers for efficiency
 * 
 * **Memory Management**:
 * - Returns pointers to owned objects
 * - Caller must respect lifetime constraints
 * - Used within region for internal operations
 * 
 * **Performance**:
 * - Efficient for typical region sizes
 * - Pre-allocation could improve performance
 * - Good cache locality
 */

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
