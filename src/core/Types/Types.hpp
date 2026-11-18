#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

/**
 * @brief Unique identifier for neurons with validation support.
 * 
 * NeuronId provides a strongly-typed identifier for neurons with:
 * - Automatic validation against invalid sentinel values
 * - Equality and ordering operators for efficient use in containers
 * - Index-based access for efficient lookup
 * - Conversion to/from integer for interoperability
 */
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    // Validation and safety methods
    bool isValid() const { return value != UINT64_MAX; }
    bool isInvalid() const { return value == UINT64_MAX; }
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    constexpr NeuronId& operator++() { ++value; return *this; }
    constexpr NeuronId operator++(int) { NeuronId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
    
    /**
     * @brief Convert to integer for low-level operations
     * @return Underlying uint64_t value
     */
    constexpr uint64_t toInt() const { return value; }
    
    /**
     * @brief Create from integer with validation
     * @param v Integer value (UINT64_MAX becomes invalid)
     * @return NeuronId instance
     */
    static NeuronId fromInt(uint64_t v) {
        NeuronId id;
        id.value = v;
        return id;
    }
};

// Synapse identifier with enhanced safety and documentation

/**
 * @brief Unique identifier for synapses with validation support.
 * 
 * SynapseId provides a strongly-typed identifier for synapses with:
 * - Automatic validation against invalid sentinel values
 * - Equality and ordering operators for efficient use in containers
 * - Index-based access for efficient lookup
 * - Conversion utilities for interoperability
 */
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    // Validation and safety methods
    bool isValid() const { return value != UINT64_MAX; }
    bool isInvalid() const { return value == UINT64_MAX; }
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    constexpr SynapseId& operator++() { ++value; return *this; }
    constexpr SynapseId operator++(int) { SynapseId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
    
    /**
     * @brief Convert to integer for low-level operations
     * @return Underlying uint64_t value
     */
    constexpr uint64_t toInt() const { return value; }
    
    /**
     * @brief Create from integer with validation
     * @param v Integer value (UINT64_MAX becomes invalid)
     * @return SynapseId instance
     */
    static SynapseId fromInt(uint64_t v) {
        SynapseId id;
        id.value = v;
        return id;
    }
};

// Region identifier with comprehensive documentation

/**
 * @brief Unique identifier for neural regions.
 * 
 * RegionId identifies a collection of neurons in the brain that share common
 * properties or functional roles. Regions are logical groupings of neural
 * populations for organization and management.
 */
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    // Validation and safety methods
    bool isValid() const { return value != UINT64_MAX; }
    bool isInvalid() const { return value == UINT64_MAX; }
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    constexpr RegionId& operator++() { ++value; return *this; }
    constexpr RegionId operator++(int) { RegionId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
    
    /**
     * @brief Check if this is the default region
     * @return true if value is 0 (default)
     */
    bool isDefault() const { return value == 0; }
    
    /**
     * @brief Convert to integer for low-level operations
     * @return Underlying uint64_t value
     */
    constexpr uint64_t toInt() const { return value; }
    
    /**
     * @brief Create from integer with validation
     * @param v Integer value
     * @return RegionId instance
     */
    static RegionId fromInt(uint64_t v) {
        RegionId id;
        id.value = v;
        return id;
    }
};

// Population identifier with enhanced safety

/**
 * @brief Unique identifier for neuron populations.
 * 
 * PopulationId identifies a group of neurons that share similar properties,
 * functions, or spatial organization. Populations are subgroups within regions
 * that can be controlled independently for computational efficiency.
 */
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    // Validation and safety methods
    bool isValid() const { return value != UINT64_MAX; }
    bool isInvalid() const { return value == UINT64_MAX; }
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    constexpr PopulationId& operator++() { ++value; return *this; }
    constexpr PopulationId operator++(int) { PopulationId old = *this; ++value; return old; }
    
    constexpr uint64_t index() const { return value; }
    
    /**
     * @brief Convert to integer for low-level operations
     * @return Underlying uint64_t value
     */
    constexpr uint64_t toInt() const { return value; }
    
    /**
     * @brief Create from integer with validation
     * @param v Integer value
     * @return PopulationId instance
     */
    static PopulationId fromInt(uint64_t v) {
        PopulationId id;
        id.value = v;
        return id;
    }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// ==============================================================================
// CORE TYPE ALIASES AND DEFINITIONS
// ==============================================================================

/**
 * @brief Comprehensive type aliases for NLM system
 * 
 * This section defines type aliases that improve code readability and type safety
 * throughout the NLM neural simulation framework. These aliases represent commonly
 * used types, making the code more maintainable and self-documenting.
 */

// Basic scalar types for neural computation
using NeuronIndex = uint64_t;      ///< Index for neuron arrays
using SynapseIndex = uint64_t;      ///< Index for synapse arrays
using PopulationIndex = uint64_t;   ///< Index for population arrays
using RegionIndex = uint64_t;       ///< Index for region arrays

using SynapticWeight = float;       ///< Synaptic connection strength (-∞ to +∞)
using MembranePotential = float;    ///< Neuron's membrane voltage in millivolts
using FiringRate = float;           ///< Neuron firing frequency in Hertz
using Delay = uint32_t;             ///< Connection delay in simulation steps

// ==============================================================================
// NEURAL CONNECTION AND WIRING TYPES
// ==============================================================================

/**
 * @brief Neural connection endpoint representation
 * 
 * Represents a single connection between a source and destination neuron
 * with associated synapse information for event-driven neural computation.
 */
struct NeuralConnection {
    NeuronId source_neuron;     ///< Origin neuron of the connection
    NeuronId destination_neuron; ///< Target neuron of the connection
    SynapseId synapse_id;        ///< Unique identifier for this synaptic connection
    SynapticWeight weight;       ///< Connection strength (modifiable)
    SynapseType type;            ///< Type of synaptic connection
    
    NeuralConnection() : source_neuron(), destination_neuron(), synapse_id(),
                         weight(0.0f), type(SynapseType::Excitatory) {}
    
    NeuralConnection(NeuronId src, NeuronId dst, SynapseId syn, SynapticWeight w, SynapseType t)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn), weight(w), type(t) {}
    
    /**
     * @brief Check if connection is bidirectional
     * @return true if source and destination are the same neuron
     */
    bool isSelfConnection() const { return source_neuron == destination_neuron; }
    
    /**
     * @brief Get connection directionality
     * @return true for excitatory, false for inhibitory
     */
    bool isExcitatory() const { return type == SynapseType::Excitatory || type == SynapseType::Modulatory; }
};

/**
 * @brief Range type for synaptic weight limits
 * 
 * Defines minimum and maximum synaptic weights for different neuron types
 * and learning regimes, ensuring weights stay within biologically plausible ranges.
 */
struct SynapticWeightRange {
    SynapticWeight min_weight;    ///< Minimum allowed synaptic weight
    SynapticWeight max_weight;    ///< Maximum allowed synaptic weight
    SynapticWeight default_weight; ///< Default synaptic weight for new connections
    
    SynapticWeightRange() : min_weight(-10.0f), max_weight(10.0f), default_weight(0.0f) {}
    SynapticWeightRange(SynapticWeight min, SynapticWeight max, SynapticWeight def)
        : min_weight(min), max_weight(max), default_weight(def) {}
    
    /**
     * @brief Clamp weight to valid range
     * @param weight Input weight value
     * @return Weight clamped to [min_weight, max_weight]
     */
    SynapticWeight clamp(SynapticWeight weight) const {
        return std::max(min_weight, std::min(max_weight, weight));
    }
    
    /**
     * @brief Check if weight is within valid range
     * @param weight Weight to check
     * @return true if weight is within [min_weight, max_weight]
     */
    bool isValid(SynapticWeight weight) const {
        return weight >= min_weight && weight <= max_weight;
    }
};

/**
 * @brief Neural population composition type
 * 
 * Represents the complete set of neurons and connections within a single
 * neural population, including topology and connectivity patterns.
 */
struct PopulationTopology {
    PopulationId population_id;               ///< Identifier for this population
    RegionId region_id;                       ///< Parent region identifier
    std::vector<NeuronId> neurons;            ///< List of neuron IDs in this population
    std::vector<NeuralConnection> connections; ///< Internal connections within population
    NeuronType neuron_type;                   ///< Cell type for all neurons in population
    
    PopulationTopology() : population_id(), region_id(), neuron_type(NeuronType::Internal) {}
    
    /**
     * @brief Get number of neurons in population
     * @return Size of neurons vector
     */
    size_t getNeuronCount() const { return neurons.size(); }
    
    /**
     * @brief Get number of internal connections
     * @return Size of connections vector
     */
    size_t getConnectionCount() const { return connections.size(); }
    
    /**
     * @brief Add a neuron to the population
     * @param neuron_id ID of neuron to add
     */
    void addNeuron(NeuronId neuron_id) {
        neurons.push_back(neuron_id);
    }
    
    /**
     * @brief Add a connection to the population
     * @param connection Neural connection to add
     */
    void addConnection(const NeuralConnection& connection) {
        connections.push_back(connection);
    }
};

/**
 * @brief Regional neural network structure type
 * 
 * Defines the complete topology and properties of a brain region, including
 * multiple populations, inter-region connections, and regional parameters.
 */
struct RegionTopology {
    RegionId region_id;                         ///< Unique identifier for the region
    std::vector<PopulationTopology> populations; ///< All populations within this region
    std::vector<InterRegionConnection> inter_region_connections; ///< Connections to other regions
    std::string name;                           ///< Human-readable region name
    std::string description;                    ///< Detailed description of region function
    
    RegionTopology() : region_id(), name("Unnamed Region"), description("No description available") {}
    
    /**
     * @brief Get total neuron count across all populations
     * @return Sum of neurons in all populations
     */
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& pop : populations) {
            total += pop.getNeuronCount();
        }
        return total;
    }
    
    /**
     * @brief Get total connection count within region
     * @return Sum of internal connections in all populations
     */
    size_t getTotalConnectionCount() const {
        size_t total = 0;
        for (const auto& pop : populations) {
            total += pop.getConnectionCount();
        }
        return total;
    }
    
    /**
     * @brief Add a population to the region
     * @param population Population to add
     */
    void addPopulation(const PopulationTopology& population) {
        populations.push_back(population);
    }
    
    /**
     * @brief Add an inter-region connection
     * @param connection Connection to add
     */
    void addInterRegionConnection(const InterRegionConnection& connection) {
        inter_region_connections.push_back(connection);
    }
};

// ==============================================================================
// EVENT-DRIVEN COMPUTATION TYPES
// ==============================================================================

/**
 * @brief Extended spike event with additional metadata
 * 
 * Represents a neural spike event with comprehensive timing and contextual
 * information for event-driven neural simulation and analysis.
 */
struct EnhancedSpikeEvent {
    NeuronId source_neuron;     ///< Neuron that generated the spike
    Timestamp timestamp;       ///< Exact time when spike occurred
    SimulationStep step;       ///< Simulation step when spike occurred
    SynapseId originating_synapse; ///< Synapse that caused this spike (if any)
    FiringState pre_synaptic_state; ///< State of source neuron before spike
    FiringState post_synaptic_state; ///< State of postsynaptic neuron after spike (if applicable)
    std::vector<NeuronId> activated_neurons; ///< Neurons activated by this spike (cascade)
    
    EnhancedSpikeEvent() : source_neuron(), timestamp(0.0), step(0), originating_synapse(),
                           pre_synaptic_state(FiringState::Resting),
                           post_synaptic_state(FiringState::Resting) {}
    
    EnhancedSpikeEvent(NeuronId source, Timestamp ts, SimulationStep s, SynapseId syn = INVALID_SYNAPSE_ID)
        : source_neuron(source), timestamp(ts), step(s), originating_synapse(s),
          pre_synaptic_state(FiringState::Resting), post_synaptic_state(FiringState::Resting) {}
    
    /**
     * @brief Add a neuron to the activation cascade
     * @param neuron_id ID of neuron activated by this spike
     */
    void addActivatedNeuron(NeuronId neuron_id) {
        activated_neurons.push_back(neuron_id);
    }
    
    /**
     * @brief Get number of neurons in activation cascade
     * @return Size of activated_neurons vector
     */
    size_t getActivationCascadeSize() const { return activated_neurons.size(); }
};

/**
 * @brief Neural activity recording for analysis
 * 
 * Captures comprehensive neural activity data for post-simulation analysis,
 * debugging, and model validation.
 */
struct NeuralActivityRecord {
    SimulationStep step;                   ///< Simulation step when activity recorded
    Timestamp timestamp;                   ///< Absolute time when activity recorded
    std::vector<NeuronId> active_neurons;   ///< Neurons firing at this step
    std::vector<SpikeEvent> spike_events;   ///< Detailed spike events
    std::vector<float> population_firing_rates; ///< Average firing rate per population
    float global_firing_rate;               ///< Overall network firing rate
    size_t total_spike_count;               ///< Total spikes in this step
    float average_membrane_potential;       ///< Average membrane potential across all neurons
    
    NeuralActivityRecord() : step(0), timestamp(0.0), global_firing_rate(0.0f),
                             total_spike_count(0), average_membrane_potential(0.0f) {}
    
    /**
     * @brief Add an active neuron to the record
     * @param neuron_id ID of neuron that is active
     */
    void addActiveNeuron(NeuronId neuron_id) {
        active_neurons.push_back(neuron_id);
    }
    
    /**
     * @brief Add a spike event to the record
     * @param spike Event to record
     */
    void addSpikeEvent(const SpikeEvent& spike) {
        spike_events.push_back(spike);
    }
    
    /**
     * @brief Get number of active neurons
     * @return Size of active_neurons vector
     */
    size_t getActiveNeuronCount() const { return active_neurons.size(); }
    
    /**
     * @brief Get total spike events recorded
     * @return Size of spike_events vector
     */
    size_t getSpikeEventCount() const { return spike_events.size(); }
};

// ==============================================================================
// MEMORY AND COGNITION TYPES
// ==============================================================================

/**
 * @brief Concept representation for cognitive processing
 * 
 * Represents a learned concept formed through pattern association and
 * neural activity, including confidence, recency, and strength metrics.
 */
struct NeuralConcept {
    ConceptId concept_id;                   ///< Unique identifier for this concept
    std::vector<float> pattern_vector;      ///< Vector representation of the concept
    float strength;                         ///< Concept activation strength (0.0 to 1.0)
    float confidence;                       ///< Confidence in concept accuracy (0.0 to 1.0)
    Timestamp last_accessed;                ///< When this concept was last used
    SimulationStep formation_step;          ///< Step when concept was formed
    std::vector<NeuronId> supporting_neurons; ///< Neurons that support this concept
    
    NeuralConcept() : concept_id(), strength(0.0f), confidence(0.0f),
                      last_accessed(0.0), formation_step(0) {}
    
    /**
     * @brief Update concept strength based on usage
     * @param new_strength New strength value (will be clamped to [0,1])
     */
    void updateStrength(float new_strength) {
        strength = std::max(0.0f, std::min(1.0f, new_strength));
    }
    
    /**
     * @brief Update concept confidence
     * @param new_confidence New confidence value (will be clamped to [0,1])
     */
    void updateConfidence(float new_confidence) {
        confidence = std::max(0.0f, std::min(1.0f, new_confidence));
    }
    
    /**
     * @brief Add a supporting neuron
     * @param neuron_id ID of neuron to add
     */
    void addSupportingNeuron(NeuronId neuron_id) {
        supporting_neurons.push_back(neuron_id);
    }
    
    /**
     * @brief Get concept activation score
     * @return Combined activation metric based on strength and confidence
     */
    float getActivationScore() const {
        return strength * confidence;
    }
};

/**
 * @brief Planning action sequence for cognitive control
 * 
 * Represents a planned sequence of actions, including expected outcomes,
 * costs, and uncertainty estimates for neural decision-making.
 */
struct NeuralPlan {
    PlanId plan_id;                         ///< Unique identifier for this plan
    std::vector<MotorCommand> action_sequence; ///< Sequence of planned actions
    std::vector<float> expected_outcomes;    ///< Expected results for each action
    std::vector<float> action_costs;        ///< Cost/benefit of each action
    float plan_confidence;                  ///< Confidence in plan execution (0.0 to 1.0)
    Timestamp planned_execution_time;       ///< When plan should be executed
    SimulationStep creation_step;           ///< Step when plan was created
    float risk_level;                       ///< Estimated risk of plan failure (0.0 to 1.0)
    
    NeuralPlan() : plan_id(), plan_confidence(0.0f), planned_execution_time(0.0),
                   creation_step(0), risk_level(0.0f) {}
    
    /**
     * @brief Calculate total expected outcome of the plan
     * @return Sum of expected outcomes weighted by action probabilities
     */
    float getTotalExpectedOutcome() const {
        float total = 0.0f;
        for (size_t i = 0; i < action_sequence.size() && i < expected_outcomes.size(); ++i) {
            total += expected_outcomes[i] * (1.0f - action_costs[i]);
        }
        return total;
    }
    
    /**
     * @brief Get plan quality metric
     * @return Combined quality score based on outcomes and confidence
     */
    float getPlanQuality() const {
        return getTotalExpectedOutcome() * plan_confidence * (1.0f - risk_level);
    }
};

/**
 * @brief Memory trace for working memory system
 * 
 * Represents a short-term memory trace with decay dynamics, interference,
 * and retrieval mechanisms for active information maintenance.
 */
struct MemoryTrace {
    TraceId trace_id;                       ///< Unique identifier for this memory trace
    std::vector<float> trace_vector;        ///< Vector representation of memory content
    float decay_factor;                     ///< Rate of memory decay (0.0 to 1.0)
    Timestamp last_updated;                ///< When trace was last modified
    SimulationStep creation_step;           ///< Step when trace was created
    float activation_level;                 ///< Current activation strength (0.0 to 1.0)
    std::vector<NeuronId> active_neurons;   ///< Neurons currently representing this trace
    
    MemoryTrace() : trace_id(), decay_factor(0.95f), last_updated(0.0),
                    creation_step(0), activation_level(0.0f) {}
    
    /**
     * @brief Apply decay to memory trace
     * @param dt Time delta since last update
     */
    void decay(float dt) {
        activation_level *= std::pow(decay_factor, dt);
        last_updated += dt;
    }
    
    /**
     * @brief Boost trace activation
     * @param boost_factor Amount to increase activation
     */
    void boost(float boost_factor) {
        activation_level = std::min(1.0f, activation_level + boost_factor);
        last_updated = static_cast<Timestamp>(std::time(nullptr));
    }
    
    /**
     * @brief Check if trace is still active
     * @return true if activation level is above threshold
     */
    bool isActive(float threshold = 0.01f) const {
        return activation_level > threshold;
    }
};

// ==============================================================================
// VALIDATION AND UTILITY FUNCTIONS
// ==============================================================================

/**
 * @brief Type validation utilities for NLM types
 * 
 * Collection of functions to validate type constraints, check ranges,
 * and ensure type safety throughout the NLM neural simulation system.
 */
namespace TypeValidation {
    
    /**
     * @brief Validate neuron ID is not invalid sentinel
     * @param id NeuronId to validate
     * @return true if ID is valid (not INVALID_NEURON_ID)
     */
    constexpr bool validateNeuronId(const NeuronId& id) {
        return id.value != UINT64_MAX;
    }
    
    /**
     * @brief Validate synapse ID is not invalid sentinel
     * @param id SynapseId to validate
     * @return true if ID is valid (not INVALID_SYNAPSE_ID)
     */
    constexpr bool validateSynapseId(const SynapseId& id) {
        return id.value != UINT64_MAX;
    }
    
    /**
     * @brief Validate region ID is not invalid sentinel
     * @param id RegionId to validate
     * @return true if ID is valid (not INVALID_REGION_ID)
     */
    constexpr bool validateRegionId(const RegionId& id) {
        return id.value != UINT64_MAX;
    }
    
    /**
     * @brief Validate population ID is not invalid sentinel
     * @param id PopulationId to validate
     * @return true if ID is valid (not INVALID_POPULATION_ID)
     */
    constexpr bool validatePopulationId(const PopulationId& id) {
        return id.value != UINT64_MAX;
    }
    
    /**
     * @brief Validate synaptic weight is within reasonable bounds
     * @param weight Weight to validate
     * @return true if weight is within [-100.0f, 100.0f]
     */
    constexpr bool validateSynapticWeight(SynapticWeight weight) {
        return weight >= -100.0f && weight <= 100.0f;
    }
    
    /**
     * @brief Validate membrane potential is within biophysical bounds
     * @param potential Membrane potential to validate
     * @return true if potential is within [-100.0f, 50.0f]
     */
    constexpr bool validateMembranePotential(MembranePotential potential) {
        return potential >= -100.0f && potential <= 50.0f;
    }
    
    /**
     * @brief Validate firing rate is within biological limits
     * @param rate Firing rate to validate
     * @return true if rate is within [0.0f, 1000.0f]
     */
    constexpr bool validateFiringRate(FiringRate rate) {
        return rate >= 0.0f && rate <= 1000.0f;
    }
    
    /**
     * @brief Validate developmental stage transition
     * @param current Current developmental stage
     * @param next Next developmental stage
     * @return true if transition is biologically plausible
     */
    constexpr bool validateDevelopmentalStageTransition(
        DevelopmentalStage current, DevelopmentalStage next) {
        // Define valid transitions based on biological development
        return static_cast<uint8_t>(current) <= static_cast<uint8_t>(next);
    }
    
    /**
     * @brief Validate neuron type compatibility
     * @param type Neuron type to validate
     * @return true if neuron type is valid
     */
    constexpr bool validateNeuronType(NeuronType type) {
        return static_cast<uint8_t>(type) <= static_cast<uint8_t>(NeuronType::Internal);
    }
    
    /**
     * @brief Validate synapse type compatibility
     * @param type Synapse type to validate
     * @return true if synapse type is valid
     */
    constexpr bool validateSynapseType(SynapseType type) {
        return static_cast<uint8_t>(type) <= static_cast<uint8_t>(SynapseType::GapJunction);
    }
    
    /**
     * @brief Get default synaptic weight range for neuron type
     * @param neuron_type Type of neuron
     * @return Appropriate weight range for the neuron type
     */
    constexpr SynapticWeightRange getDefaultWeightRange(NeuronType neuron_type) {
        switch (neuron_type) {
            case NeuronType::Excitatory:
                return SynapticWeightRange(-10.0f, 10.0f, 0.5f);
            case NeuronType::Inhibitory:
                return SynapticWeightRange(-5.0f, 5.0f, -0.5f);
            case NeuronType::Modulatory:
                return SynapticWeightRange(-2.0f, 2.0f, 0.0f);
            case NeuronType::Sensory:
                return SynapticWeightRange(-8.0f, 8.0f, 1.0f);
            case NeuronType::Motor:
                return SynapticWeightRange(-8.0f, 8.0f, 1.0f);
            case NeuronType::Internal:
                return SynapticWeightRange(-4.0f, 4.0f, 0.0f);
            default:
                return SynapticWeightRange(-1.0f, 1.0f, 0.0f);
        }
    }
    
    /**
     * @brief Create comprehensive type validation report
     * @return String containing validation summary
     */
    std::string createValidationReport() {
        std::string report = "Type Validation Summary:\n";
        report += "- NeuronId validation: Valid IDs are those not equal to UINT64_MAX\n";
        report += "- SynapseId validation: Valid IDs are those not equal to UINT64_MAX\n";
        report += "- RegionId validation: Valid IDs are those not equal to UINT64_MAX\n";
        report += "- PopulationId validation: Valid IDs are those not equal to UINT64_MAX\n";
        report += "- SynapticWeight validation: Valid weights are within [-100.0f, 100.0f]\n";
        report += "- MembranePotential validation: Valid potentials are within [-100.0f, 50.0f]\n";
        report += "- FiringRate validation: Valid rates are within [0.0f, 1000.0f]\n";
        return report;
    }
}

/**
 * @brief Utility functions for common type operations
 * 
 * Collection of useful functions that operate on NLM types, providing
 * common functionality for neural simulation and analysis.
 */
namespace TypeUtilities {
    
    /**
     * @brief Generate unique ID based on current time and hash
     * @return New unique ID
     */
    NeuronId generateUniqueNeuronId() {
        NeuronId id;
        id.value = static_cast<uint64_t>(std::time(nullptr)) * 1000000 + std::hash<std::string>{}("neuron");
        return id;
    }
    
    /**
     * @brief Convert neuron type to string representation
     * @param type Neuron type to convert
     * @return String representation of neuron type
     */
    std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    /**
     * @brief Convert synapse type to string representation
     * @param type Synapse type to convert
     * @return String representation of synapse type
     */
    std::string synapseTypeToString(SynapseType type) {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }
    
    /**
     * @brief Format neuron ID for display
     * @param id NeuronId to format
     * @return Formatted string representation
     */
    std::string formatNeuronId(const NeuronId& id) {
        return "N" + std::to_string(id.value);
    }
    
    /**
     * @brief Format synapse ID for display
     * @param id SynapseId to format
     * @return Formatted string representation
     */
    std::string formatSynapseId(const SynapseId& id) {
        return "S" + std::to_string(id.value);
    }
    
    /**
     * @brief Format region ID for display
     * @param id RegionId to format
     * @return Formatted string representation
     */
    std::string formatRegionId(const RegionId& id) {
        return "R" + std::to_string(id.value);
    }
    
    /**
     * @brief Format population ID for display
     * @param id PopulationId to format
     * @return Formatted string representation
     */
    std::string formatPopulationId(const PopulationId& id) {
        return "P" + std::to_string(id.value);
    }
    
    /**
     * @brief Calculate neural connection compatibility score
     * @param connection Connection to evaluate
     * @return Compatibility score (0.0 to 1.0)
     */
    float calculateConnectionCompatibility(const NeuralConnection& connection) {
        float type_score = 1.0f;
        if (connection.isSelfConnection()) {
            type_score = 0.1f; // Self-connections generally less effective
        }
        
        float weight_score = std::abs(connection.weight) / 20.0f;
        weight_score = std::min(1.0f, weight_score);
        
        return (type_score + weight_score) * 0.5f;
    }
    
    /**
     * @brief Create comprehensive type summary for debugging
     * @return String containing type system summary
     */
    std::string createTypeSummary() {
        std::string summary = "NLM Type System Summary:\n";
        summary += "=======================\n";
        summary += "Core Identifiers:\n";
        summary += "  - NeuronId: 64-bit unsigned integer with validation\n";
        summary += "  - SynapseId: 64-bit unsigned integer with validation\n";
        summary += "  - RegionId: 64-bit unsigned integer with validation\n";
        summary += "  - PopulationId: 64-bit unsigned integer with validation\n";
        summary += "\nCore Types:\n";
        summary += "  - SimulationStep: uint64_t for simulation steps\n";
        summary += "  - Timestamp: double for time in seconds\n";
        summary += "  - TimestepDuration: double for integration timestep\n";
        summary += "  - SynapticWeight: float for connection strength\n";
        summary += "  - MembranePotential: float for neuron voltage\n";
        summary += "  - FiringRate: float for neural firing frequency\n";
        summary += "  - Delay: uint32_t for connection delays\n";
        summary += "\nType Aliases:\n";
        summary += "  - NeuronIndex, SynapseIndex, PopulationIndex, RegionIndex\n";
        summary += "  - EnhancedSpikeEvent: Extended spike with metadata\n";
        summary += "  - NeuralConnection: Single neural connection definition\n";
        summary += "  - PopulationTopology: Complete population structure\n";
        summary += "  - RegionTopology: Complete region structure\n";
        summary += "  - NeuralConcept: Cognitive concept representation\n";
        summary += "  - NeuralPlan: Action planning sequence\n";
        summary += "  - MemoryTrace: Working memory trace\n";
        summary += "\nValidation Functions:\n";
        summary += "  - TypeValidation: Comprehensive type validation\n";
        summary += "  - TypeUtilities: Common type operations\n";
        return summary;
    }
}

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

} // namespace nlm

namespace std {
template<> struct hash<nlm::NeuronId> {
    size_t operator()(const nlm::NeuronId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::SynapseId> {
    size_t operator()(const nlm::SynapseId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::RegionId> {
    size_t operator()(const nlm::RegionId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
template<> struct hash<nlm::PopulationId> {
    size_t operator()(const nlm::PopulationId& id) const noexcept { return std::hash<uint64_t>{}(id.value); }
};
} // namespace std
