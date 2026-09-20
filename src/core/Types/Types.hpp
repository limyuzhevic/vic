// Enhanced neural type definitions with comprehensive documentation
// 
// This file contains type definitions and constants for the NLM system,
// providing a foundation for neural network simulation and modeling.

#pragma once

#include <cstdint>
#include <string>
#include <limits>

namespace nlm {

/**
 * @namespace nlm::types
 * @brief Neural type definitions and constants
 * 
 * This namespace provides type definitions, enumerations, and constants
 * used throughout the NLM neural simulation system. These types form the
 * foundation for neural network representation, neuron models, and
 * simulation mechanics.
 */

namespace types {

/**
 * @struct NeuronId
 * @brief Unique identifier for neural entities
 * 
 * Represents a unique identifier for neurons within the NLM system.
 * Uses uint64_t for scalability to support large neural networks
 * while maintaining efficient memory usage and fast comparison operations.
 * 
 * Key features:
 * - Stores neural entity identity as unsigned 64-bit integer
 * - Provides index extraction for efficient array lookups
 * - Implements equality and inequality operators
 * - Supports hashing for use in unordered containers
 * - Provides string representation for debugging
 * 
 * Usage example:
 * @code
 * nlm::types::NeuronId neuron1{42};
 * nlm::types::NeuronId neuron2;
 * neuron2.value = 43;
 * 
 * if (neuron1 == neuron2) { /* Compare neurons */ }
 * auto index = neuron1.index();
 * @endcode
 */
struct NeuronId {
    uint64_t value;  ///< Unique identifier value
    
    /**
     * @brief Default constructor
     * Creates an invalid neuron ID
     */
    NeuronId() : value(0) {}
    
    /**
     * @brief Parameterized constructor
     * @param val Unique identifier value
     */
    explicit NeuronId(uint64_t val) : value(val) {}
    
    /**
     * @brief Get array index from neuron ID
     * @return Index suitable for array access
     * @note Simple division-based mapping for efficient indexing
     */
    uint64_t index() const { return value; }
    
    /**
     * @brief Check equality with another neuron ID
     * @param other Neuron ID to compare against
     * @return true if IDs are equal
     */
    bool operator==(const NeuronId& other) const { return value == other.value; }
    
    /**
     * @brief Check inequality with another neuron ID
     * @param other Neuron ID to compare against
     * @return true if IDs are not equal
     */
    bool operator!=(const NeuronId& other) const { return value != other.value; }
    
    /**
     * @brief Hash function for unordered containers
     * @param id Neuron ID to hash
     * @return Hash value for the ID
     */
    struct Hash {
        size_t operator()(const NeuronId& id) const { return std::hash<uint64_t>()(id.value); }
    };
    
    /**
     * @brief String representation for debugging
     * @return Formatted string describing the neuron ID
     */
    std::string toString() const {
        return "<NeuronId: " + std::to_string(value) + ">";
    }
};

/**
 * @struct SynapseId
 * @brief Unique identifier for synaptic connections
 * 
 * Represents a unique identifier for synapses (neural connections)
 * within the NLM system. Provides similar functionality to NeuronId
 * but specialized for synaptic entities in the neural network.
 * 
 * Synaptic connections are fundamental to neural computation and
 * information flow in the NLM architecture, making this identifier
 * critical for simulation efficiency and accuracy.
 */
struct SynapseId {
    uint64_t value;  ///< Unique identifier value
    
    SynapseId() : value(0) {}
    explicit SynapseId(uint64_t val) : value(val) {}
    
    uint64_t index() const { return value; }
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
    
    struct Hash {
        size_t operator()(const SynapseId& id) const { return std::hash<uint64_t>()(id.value); }
    };
    
    std::string toString() const {
        return "<SynapseId: " + std::to_string(value) + ">";
    }
};

/**
 * @struct RegionId
 * @brief Unique identifier for neural regions
 * 
 * Identifies distinct neural regions within the brain structure.
 * Neural regions group neurons with similar functions or connectivity
 * patterns, enabling organized and efficient simulation of brain anatomy.
 */
struct RegionId {
    uint64_t value;  ///< Unique identifier value
    
    RegionId() : value(0) {}
    explicit RegionId(uint64_t val) : value(val) {}
    
    uint64_t index() const { return value; }
    bool operator==(const RegionId& other) const { return value == other.value; }
    bool operator!=(const RegionId& other) const { return value != other.value; }
    
    struct Hash {
        size_t operator()(const RegionId& id) const { return std::hash<uint64_t>()(id.value); }
    };
    
    std::string toString() const {
        return "<RegionId: " + std::to_string(value) + ">";
    }
};

/**
 * @struct PopulationId
 * @brief Unique identifier for neuron populations
 * 
 * Identifies groups of neurons that share functional properties
 * or spatial organization within the neural network.
 */
struct PopulationId {
    uint64_t value;  ///< Unique identifier value
    
    PopulationId() : value(0) {}
    explicit PopulationId(uint64_t val) : value(val) {}
    
    uint64_t index() const { return value; }
    bool operator==(const PopulationId& other) const { return value == other.value; }
    bool operator!=(const PopulationId& other) const { return value != other.value; }
    
    struct Hash {
        size_t operator()(const PopulationId& id) const { return std::hash<uint64_t>()(id.value); }
    };
    
    std::string toString() const {
        return "<PopulationId: " + std::to_string(value) + ">";
    }
};

/**
 * @enum NeuronType
 * @brief Classification of neuron types in NLM system
 * 
 * Defines the different types of neurons used in the NLM simulation,
 * each with distinct functional properties and roles in neural computation.
 * 
 * Neuron types are fundamental to the NLM architecture, determining
 * how neurons process inputs, generate outputs, and participate in
 * learning and information storage.
 */
enum class NeuronType {
    Excitatory,    ///< Neurons that increase firing probability of targets
    Inhibitory,    ///< Neurons that decrease firing probability of targets
    Modulatory,    ///< Neurons that regulate overall network excitability
    Sensory,       ///< Neurons that receive external input signals
    Motor,         ///< Neurons that produce output actions
    Internal       ///< Neurons internal to the brain network
};

/**
 * @enum SynapseType
 * @brief Classification of synaptic connection types
 * 
 * Defines the different types of synapses that can exist between
 * neurons, each with distinct transmission properties and effects.
 */
enum class SynapseType {
    Excitatory,    ///< Standard excitatory synaptic transmission
    Inhibitory,    ///< Standard inhibitory synaptic transmission
    Modulatory,    ///< Modulatory synaptic effects
    Electrical,    ///< Direct electrical coupling
    GapJunction    ///< Electrical synapse through gap junctions
};

/**
 * @enum DevelopmentalStage
 * @brief Brain development stages in NLM
 * 
 * Represents different stages of brain development that affect
 * neural connectivity, plasticity, and computational properties.
 * The developmental stage influences learning rates, connectivity
 * patterns, and overall brain function.
 */
enum class DevelopmentalStage {
    Initial,       ///< Embryonic/neonatal stage with high plasticity
    CriticalPeriod,///< Period of enhanced learning capability
    Maturation,    ///< Refinement and specialization phase
    Adult,         ///< Fully mature, stable functionality
    Aging          ///< Age-related decline and adaptation
};

/**
 * @enum FiringState
 * @brief Neuron firing state enumeration
 * 
 * Describes the current activity state of neurons, reflecting
 * their computational state and recent activity history.
 */
enum class FiringState {
    Resting,       ///< Not currently firing
    Active,        ///< Actively firing spikes
    Refractory,    ///< Temporary inability to fire
    Inhibited      ///< Suppressed firing capability
};

/**
 * @enum ActionType
 * @brief Types of motor actions
 * 
 * Defines the range of possible actions that the brain can
 * produce, corresponding to behavioral outputs in the simulated
 * environment.
 */
enum class ActionType {
    MoveForward,   ///< Move forward in the environment
    MoveBackward,  ///< Move backward in the environment
    MoveLeft,      ///< Turn/move left
    MoveRight,     ///< Turn/move right
    TurnLeft,      ///< Rotate left
    TurnRight,     ///< Rotate right
    Look,          ///< Look in current direction
    LookUp,        ///< Look upward
    LookDown,      ///< Look downward
    Interact,      ///< Interact with objects
    Eat,           ///< Consume food/resources
    Drink,         ///< Consume liquids
    Rest,          ///< Enter resting state
    Wait,          ///< Wait without action
    Custom        ///< Custom user-defined action
};

/**
 * @enum MotorCommand
 * @brief Low-level motor commands
 * 
 * Physical commands sent to the motor system, corresponding
 * to specific movements or actions.
 */
enum class MotorCommand {
    MoveForward,   ///< Move forward
    MoveBackward,  ///< Move backward
    TurnLeft,      ///< Turn left
    TurnRight,     ///< Turn right
    LookLeft,      ///< Look left
    LookRight,     ///< Look right
    Interact,      ///< Interact with environment
    Wait          ///< No movement
};

/**
 * @enum WorldObjectType
 * @brief Types of objects in the world
 * 
 * Defines the different kinds of objects that can exist in
 * the simulated environment, each with distinct properties
 * and interactions.
 */
enum class WorldObjectType {
    Empty,         ///< No object (empty space)
    Resource,      ///< Consumable resource
    Hazard,        ///< Dangerous object
    Wall,          ///< Impassable barrier
    Marker         ///< Reference point or landmark
};

/**
 * @enum SimulationStep
 * @brief Type for simulation step counters
 * 
 * Defines the type used to track and index simulation steps,
 * ensuring proper time progression and step counting throughout
 * the NLM simulation.
 */
using SimulationStep = uint64_t;

/**
 * @enum Timestamp
 * @brief Type for time measurements in simulation
 * 
 * Defines the type used for time measurements, typically in
 * milliseconds or other appropriate time units for the simulation.
 */
using Timestamp = double;

/**
 * @struct Constants
 * @brief Collection of commonly used constants
 * 
 * Provides mathematical and simulation constants used throughout
 * the NLM system, ensuring consistency and ease of modification.
 */
struct Constants {
    // Mathematical constants
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double E = 2.71828182845904523536;
    
    // Simulation constants
    static constexpr Timestamp DEFAULT_TIMESTEP = 0.001;  ///< Default simulation timestep (1ms)
    static constexpr SimulationStep DEFAULT_MAX_STEPS = 10000;  ///< Default maximum simulation steps
    static constexpr uint64_t DEFAULT_NEURON_COUNT = 1000;  ///< Default number of neurons
    static constexpr double DEFAULT_CONNECTION_PROBABILITY = 0.1;  ///< Default synaptic connectivity
    
    // Memory constants
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;  ///< Working memory items
    static constexpr size_t DEFAULT_MAX_EPISODES = 1000;  ///< Maximum episodic memory
    
    // Performance constants
    static constexpr size_t DEFAULT_MAX_THREADS = 4;  ///< Default thread count for parallel operations
    static constexpr size_t CACHE_LINE_SIZE = 64;  ///< Cache line size for optimization
    
    /**
     * @brief Get constant value by name (dynamic access)
     * @param name Name of constant
     * @return String representation of constant value
     */
    static std::string getConstant(const std::string& name);
    
    /**
     * @brief Get all available constant names
     * @return Vector of all constant names
     */
    static std::vector<std::string> getAvailableConstants();
};

/**
 * @brief Generate unique neuron ID
 * @return New unique neuron identifier
 */
NeuronId generateNeuronId();

/**
 * @brief Generate unique synapse ID
 * @return New unique synapse identifier
 */
SynapseId generateSynapseId();

/**
 * @brief Generate unique region ID
 * @return New unique region identifier
 */
RegionId generateRegionId();

/**
 * @brief Generate unique population ID
 * @return New unique population identifier
 */
PopulationId generatePopulationId();

/**
 * @brief Clamp value between minimum and maximum
 * @tparam T Numeric type
 * @param value Input value
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return Clamped value
 */
template<typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

/**
 * @brief Linear interpolation between two values
 * @tparam T Numeric type
 * @param start Starting value
 * @param end Ending value
 * @param t Interpolation parameter (0 to 1)
 * @return Interpolated value
 */
template<typename T>
T lerp(T start, T end, double t) {
    return start + (end - start) * t;
}

/**
 * @brief Check if value is approximately equal (within epsilon)
 * @tparam T Numeric type
 * @param a First value
 * @param b Second value
 * @param epsilon Tolerance for equality
 * @return true if values are approximately equal
 */
template<typename T>
bool approxEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) {
    return std::abs(a - b) < epsilon;
}

/**
 * @brief Calculate hash for neuron ID
 * @param id Neuron ID to hash
 * @return Hash value
 */
size_t hashNeuronId(const NeuronId& id);

/**
 * @brief Calculate hash for synapse ID
 * @param id Synapse ID to hash
 * @return Hash value
 */
size_t hashSynapseId(const SynapseId& id);

/**
 * @brief Calculate hash for region ID
 * @param id Region ID to hash
 * @return Hash value
 */
size_t hashRegionId(const RegionId& id);

/**
 * @brief Calculate hash for population ID
 * @param id Population ID to hash
 * @return Hash value
 */
size_t hashPopulationId(const PopulationId& id);

} // namespace types

// Legacy type aliases for backward compatibility
using NeuronId = types::NeuronId;
using SynapseId = types::SynapseId;
using RegionId = types::RegionId;
using PopulationId = types::PopulationId;

using SimulationStep = types::SimulationStep;
using Timestamp = types::Timestamp;

using NeuronType = types::NeuronType;
using SynapseType = types::SynapseType;
using DevelopmentalStage = types::DevelopmentalStage;
using FiringState = types::FiringState;
using ActionType = types::ActionType;
using MotorCommand = types::MotorCommand;
using WorldObjectType = types::WorldObjectType;

// Invalid ID constants for initialization
const NeuronId INVALID_NEURON_ID{0};
const SynapseId INVALID_SYNAPSE_ID{0};
const RegionId INVALID_REGION_ID{0};
const PopulationId INVALID_POPULATION_ID{0};

} // namespace nlm