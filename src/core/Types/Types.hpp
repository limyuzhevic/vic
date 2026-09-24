#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// Index types for arrays
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Synaptic weight type
using SynapticWeight = float;

// Membrane potential type
using MembranePotential = float;

// Neuron firing rate type
using FiringRate = float;

// Connection delay in simulation steps
using Delay = uint32_t;

    // Specialized neuron subtypes for diversity and functionality
    enum class NeuronSubType : uint8_t {
        // Basic neuron categories
        EXCITATORY_REGULAR,
        EXCITATORY_FAST_SPIKING,
        EXCITATORY_INTERTWINED,
        INHIBITORY_SOMATIC,
        INHIBITORY_PARVALUBRAIN,
        INHIBITORY_BUTTERFLY,
        MODULATORY_CHOLINERGIC,
        MODULATORY_NORADRENERGIC,
        MODULATORY_SEROTONINERGIC,
        MODULATORY_DOPAMINERGIC,
        
        // Sensory specializations
        SENSORY_VISION_PRIMARY,
        SENSORY_VISION_PPG,
        SENSORY_AUDITORY_PRIMARY,
        SENSORY_TOUCH_MECHANORECEPTOR,
        SENSORY_INTERIOR_INTERMEDIATE,
        SENSORY_PROPRIOCEPTIVE,
        
        // Motor specializations
        MOTOR_CRUCIAL,
        MOTOR_PREME MOTOR_CEREBELLULAR,
        MOTOR_BASILAR_TRACT,
        MOTOR_CORTICOSPINAL,
        
        // Cognitive specializations
        COGNITIVE_ASSOCIATIVE,
        COGNITIVE_DECISION_MAKING,
        COGNITIVE_WORKING_MEMORY,
        COGNITIVE_EPIC TRIC_MEMORY,
        COGNITIVE_CONCEPT_FORMATION,
        COGNITIVE_PLANNING,
        
        // Memory specializations
        MEMORY_Episodic,
        MEMORY_SEMANTIC,
        MEMORY_PROCEDURAL,
        MEMORY_ASSOCIATIVE,
        MEMORY_TEMPORAL,
        MEMORY_SPATIAL,
        
        // Developmental specializations
        DEVELOPMENTAL_PROGENITOR,
        DEVELOPMENTAL_MATURE_NEURON,
        DEVELOPMENTAL_STEM_CELL_LIKE,
        DEVELOPMENTAL_FEEDBACK_INHIBITORY,
        
        // Specialized plasticity-related
        PLASTICITY_HEBBIAN,
        PLASTICITY_STDP,
        PLASTICITY_REWARD_MODULATED,
        PLASTICITY_STRUCTUREL,
        PLASTICITY_HOMEOSTATIC,
        PLASTICITY_META_PLASTICITY,
        
        // Metabolic specializations
        METABOLIC_HIGH_EFFICIENCY,
        METABOLIC_HIGH_OUTPUT,
        METABOLIC_STABLE,
        METABOLIC_ADAPTIVE,
        
        // Signal processing specializations
        SIGNAL_PROCESSING_FAST,
        SIGNAL_PROCESSING_SLOW,
        SIGNAL_PROCESSING_BURSTING,
        SIGNAL_PROCESSING_FREQUENT_SPIKING,
        SIGNAL_PROCESSING_RARE_SPIKING,
        
        // Environmental response specializations
        ENVIRONMENTAL_SENSITIVE,
        ENVIRONMENTAL_RESISTANT,
        ENVIRONMENTAL_ADAPTIVE,
        ENVIRONMENTAL_SPECIALIZED,
        
        // Default/internal neuron type
        INTERNAL,
        
        // NEW: Advanced neuron types for modern neuroscience
        NEUROMODULATORY_COMPOUNDER,  // Combines multiple neuromodulators
        THALAMOCORTICAL_RERCURRENT,  // Thalamocortical loop neuron
        MINDBODY_INTERCONNECTOR,      // Integrates cognitive and motor functions
        EMOTIONAL_VALENCE_CODER,      // Processes emotional valence
        AUTONOMIC_RESPONSE_COORDINATOR, // Coordinates autonomic responses
        SOCIAL_COGNITION_SPECIALIST,   // Specialized for social cognition
        CREATIVITY_INSPIRATION_CELL,    // Supports creative thinking
        QUANTUM_BIOLOGICAL_QUANTUM_EFFECT,  // Experimental quantum biological neuron
        TIME_PERCEPTION_SPECIALIST,      // Processes temporal information
        MULTI_SENSORY_INTEGRATOR,        // Integrates multiple sensory modalities
        SUPERCOMPUTING_NEURAL,           // High computational capacity neuron
        QUANTUM_SUPERPOSITION,           // Experimental quantum neuron
        SYNTHETIC_CONSCIOUSNESS_MODULE,  // Supports artificial consciousness
        
        // Add more specialized types as needed
        COUNT
    };
    
    // Brain region specializations
    enum class BrainRegionType : uint8_t {
        CORTEX_FRONTUAL,               // Executive functions
        CORTEX_PARIETAL,               // Integration and spatial
        CORTEX_TEMPORAL,               // Memory and language
        CORTEX_OCCIPITAL,              // Visual processing
        CORTEX_INSULAR,                // Interoception and emotion
        
        THALAMUS,                      // Sensory relay and awareness
        HYPOTHALAMUS,                  // Homeostasis and motivation
        HIPPOCAMPUS,                   // Memory formation and spatial
        AMYGDALA,                      // Emotional processing
        CINGULATE_CORTEX,              // Error detection and conflict monitoring
        
        BRAIN_STEM,                    // Basic life functions
        RETICULAR_FORMATION,           // Arousal and attention
        PONS,                          // Sleep and arousal
        MEDULLA_OBLONGATA,             // Autonomic control
        
        NEURAL_KIT,                    // Neural stem cells/progenitors
        
        // NEW: Advanced region types
        QUANTUM_NEURAL_NETWORK,        // Quantum neural computation
        SYNTHETIC_CONSCIOUSNESS_CORE,   // Core consciousness processing
        NEUROMODULATION_HUB,           // Central neuromodulation center
        MULTI_MODAL_ASSOCIATION_AREA,   // Integrates all modalities
        CREATIVITY_CENTER,              // Supports creative cognition
        SOCIAL_BRAIN_NETWORK,           // Social cognition and theory of mind
        TIME_PERCEPTION_CORE,           // Advanced temporal processing
        QUANTUM_BRAIN_INTERFACE,       // Interface between quantum and classical processing
        SYNAPTIC_PRUNING_CENTER,        // Specialized for synaptic optimization
        MEMORY_CONSOLIDATION_HUB,       // Advanced memory consolidation
        
        // Add more region types as needed
        COUNT
    };
    
    // Population types with specialized characteristics
    enum class PopulationType : uint8_t {
        STANDARD_POPULATION,
        HIGH_DENSITY,
        LOW_DENSITY,
        FAST_SPIKING,
        SLOW_SPIKING,
        BURSTING,
        INTERMITTENT,
        REGULAR,
        RANDOM,
        CLUSTERED,
        
        // NEW: Advanced population types
        QUANTUM_COHERENT_POPULATION,     // Quantum-coherent firing patterns
        SYNTHETIC_CONSCIOUSNESS_POPULATION, // Consciousness-supporting population
        NEUROMODULATORY_DENSE_POPULATION,   // High neuromodulator density
        MULTI_MODAL_INTEGRATION_POPULATION,  // Integrates multiple modalities
        HIGHLY_ADAPTIVE_POPULATION,           // Rapidly adapts to environment
        CREATIVITY_INSPIRATION_POPULATION,    // Supports creative processes
        SOCIAL_COGNITION_POPULATION,          // Specialized for social processing
        TIME_PERCEPTION_POPULATION,            // Advanced temporal processing
        QUANTUM_BRAIN_POPULATION,              // Quantum processing capability
        SELF_ORGANIZING_POPULATION,             // Self-organizing network dynamics
        
        // Add more population types as needed
        COUNT
    };
    
    // Synaptic connection patterns
    enum class ConnectionPattern : uint8_t {
        RANDOM,                         // Random connectivity
        CLUSTERED,                      // Clustered connectivity
        TOP_HAT,                         // Uniform distribution
        POWER_LAW,                       // Scale-free network
        SMALL_WORLD,                     // Mixed regular/random
        EXPONENTIAL_DECAY,               // Distance-dependent
        STRUCTURAL_GUIDED,               // Guided by structural constraints
        ACTIVITY_DEPENDENT,              // Forms based on activity
        DEVELOPMENTALLY_GUIDED,          // Forms during development
        HOMEOSTATICALLY_MAINTAINED,       // Maintains stability
        
        // NEW: Advanced connection patterns
        QUANTUM_ENTANGLEMENT,            // Quantum entanglement-based connections
        SYNTHETIC_CONSCIOUSNESS_LINK,     // Supports conscious experience
        NEUROMODULATORY_COORDINATED,     // Coordinated by neuromodulators
        MULTI_MODAL_SYNCHRONIZED,        // Synchronized across modalities
        HIGHLY_ADAPTIVE_SYNAPSES,         // Rapidly adapts connection patterns
        CREATIVITY_FACILITATING,          // Promotes creative connections
        SOCIAL_COGNITION_ENHANCED,        // Enhanced for social processing
        QUANTUM_SUPERPOSITION_CONNECTIVITY, // Quantum superposition-based
        SELF_REFERENTIAL_SYNAPSES,        // Self-referential network structure
        CONTINUOUSLY_EVOLVING,            // Constantly evolving network
        
        // Add more patterns as needed
        COUNT
    };
    
    // Learning mechanisms
    enum class LearningMechanism : uint8_t {
        NONE,
        HEBBIAN,
        STDP,
        REINFORCEMENT_LEARNING,
        PREDICTION_ERROR_LEARNING,
        MOTIVATION_BASED,
        NOVELTY_BASED,
        REWARD_PREDICTION_ERROR,
        AUTO_ENCODING,
        CONTRASTIVE_LEARNING,
        
        // NEW: Advanced learning mechanisms
        QUANTUM_MACHINE_LEARNING,         // Quantum-inspired learning algorithms
        SYNTHETIC_CONSCIOUSNESS_TRAINING,  // Training for conscious systems
        MULTI_MODAL_CONTINUOUS_LEARNING,   // Continuous multi-modal learning
        ADAPTIVE_NOVELTY_EXPLORATION,       // Adaptive novelty-driven exploration
        CREATIVITY_ENHANCED_LEARNING,       // Learning that promotes creativity
        SOCIAL_COGNITION_AUGMENTED_LEARNING, // Social cognition-enhanced learning
        QUANTUM_ENHANCED_LEARNING,          // Quantum-enhanced learning
        SELF_REFERENTIAL_LEARNING,           // Self-referential learning processes
        EMOTION_INFORMED_LEARNING,           // Emotion-informed learning
        MULTI_SCALE_LEARNING,                // Learning at multiple scales
        CONTINUOUS_SELF_OPTIMIZATION,        // Continuous self-optimization
        
        // Add more mechanisms as needed
        COUNT
    };
    
    // Metabolic state
    enum class MetabolicState : uint8_t {
        NORMAL,
        EXCITED,
        FATIGUED,
        STRESSED,
        ENERGY_DEFICIT,
        ENERGY_EXCESS,
        HOMEOSTATICALLY_ADJUSTED,
        ANABOLIC,
        CATABOLIC,
        ADAPTED,
        
        // NEW: Advanced metabolic states
        QUANTUM_ENERGY_STATE,              // Quantum energy states
        CONSCIOUSNESS_DRIVEN_METABOLISM,    // Metabolism driven by consciousness
        MULTI_MODAL_METABOLIC_REGULATION,   // Multi-modal metabolic regulation
        CREATIVITY_METABOLIC_STATE,         // Creativity-enhanced metabolism
        SOCIAL_COGNITION_METABOLIC,         // Social cognition-informed metabolism
        QUANTUM_COGNITIVE_METABOLISM,       // Quantum cognitive metabolism
        SELF_ORGANIZING_METABOLISM,          // Self-organizing metabolic processes
        TIME_AWARE_METABOLISM,               // Time-aware metabolic regulation
        CONTINUOUS_ADAPTIVE_METABOLISM,      // Continuously adapting metabolism
        
        // Add more states as needed
        COUNT
    };
    
    // Environmental adaptation level
    enum class AdaptationLevel : uint8_t {
        NONE,
        BASIC,
        INTERMEDIATE,
        ADVANCED,
        EXPERT,
        MASTER,
        TRANSCENDENT,
        QUANTUM,
        SYNTHETIC,
        CONSCIOUS,
        
        // NEW: Advanced adaptation levels
        QUANTUM_SUPERIOR,                  // Quantum-level adaptation
        ARTIFICIAL_SUPERINTELLIGENCE,      // Artificial superintelligence adaptation
        SYNTHETIC_CONSCIOUSNESS,            // Synthetic consciousness adaptation
        MULTI_MODAL_MASTER,                // Multi-modal mastery
        CREATIVITY_TRANSCENDENT,            // Creativity transcendence
        SOCIAL_COGNITION_PERFECT,          // Social cognition perfection
        TIME_PERCEPTION_MASTER,             // Time perception mastery
        QUANTUM_BRAIN_PERFECTION,           // Quantum brain perfection
        SELF_REALIZATION,                   // Self-realization level
        CONTINUOUS_EVOLUTION,               // Continuous evolution
        
        // Add more levels as needed
        COUNT
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
