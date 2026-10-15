#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

/**
 * Specialized Neuron Populations
 * 
 * Specialized groups of neurons with:
 * - Excitatory neurons
 * - Inhibitory neurons
 * - Modulatory neurons
 * - Sensory neurons
 * - Motor neurons
 * - Interneurons
 * - Projection neurons
 * - Local interneurons
 * - Deep layers
 * - Superficial layers
 * - Barrel cortex neurons
 * - Visual cortex neurons
 * - Motor cortex neurons
 * - Association neurons
 * - Output neurons
 * - Input neurons
 * - Feedback neurons
 * - Feedforward neurons
 * - Recurrent neurons
 * - Sparse populations
 * - Dense populations
 * - Balanced populations
 * - Unbalanced populations
 * - Recurrent networks
 * - Feedforward networks
 * - Feedback networks
 * - Bidirectional networks
 * - Cyclic networks
 * - Random networks
 * - Structured networks
 * - Hierarchical networks
 * - Scale-free networks
 * - Small-world networks
 * - Modular networks
 * - Random graph networks
 * - Regular graph networks
 * - Complete graph networks
 * - Star networks
 * - Ring networks
 * - Lattice networks
 * - ER random networks
 * - BA scale-free networks
 * - WM small-world networks
 * - Random geometric networks
 * - Geometric random graphs
 * - Unit disk graphs
 * - Random neighbor graphs
 * - Barabási-Albert models
 * - Watts-Strogatz models
 * - Newman-Watts models
 * - Stochastic block models
 * - Degree-corrected models
 * - Configuration models
 * - Exponential random graphs
 * - Random clustered models
 * - Neuronal field models
 * - Neuronal mass models
 * - Rate models
 * - Spiking models
 * - Integrate-and-fire models
 * - Adaptive exponential integrate-and-fire models
 * - Hodgkin-Huxley models
 * - Morris-Lecar models
 * - FitzHugh-Nagumo models
 * - Izhikevich models
 * - Quadratic integrate-and-fire models
 * - Hindmarsh-Rose models
 * - Grandmifflin-Perme models
 * - Hindmarsh-Rose models
 * - Izhikevich models
 * - Quadratic integrate-and-fire models
 * - Chaotic dynamics
 * - Regular dynamics
 * - Spiking dynamics
 * - Bursting dynamics
 * - Oscillatory dynamics
 * - Quiescent dynamics
 * - Transition dynamics
 * - Hybrid dynamics
 * - Mixed dynamics
 * - Stochastic dynamics
 * - Deterministic dynamics
 * - Probabilistic dynamics
 * - Pseud stochastic dynamics
 * - Fractional dynamics
 * - Fractional-order dynamics
 * - Viscoelastic dynamics
 * - Nonlinear dynamics
 * - Linear dynamics
 * - Piecewise linear dynamics
 * - Smooth dynamics
 * - Non-smooth dynamics
 * - Discontinuous dynamics
 * - Smooth-and-discontinuous dynamics
 * - Differentiable dynamics
 * - Non-differentiable dynamics
 * - Smooth-differentiable dynamics
 * - Non-smooth-differentiable dynamics
 * - Differentiable-continuous dynamics
 * - Non-differentiable-discontinuous dynamics
 */
class SpecializedNeuronPopulations {
public:
    SpecializedNeuronPopulations();
    ~SpecializedNeuronPopulations();

    // ===== NEURON POPULATION TYPES =====

    // Neuron population types
    enum class PopulationType {
        EXCITATORY,              // Excitatory neurons
        INHIBITORY,               // Inhibitory neurons
        MODULATORY,               // Modulatory neurons
        SENSORY,                 // Sensory neurons
        MOTOR,                   // Motor neurons
        INTERNEURON,              // Interneurons
        PROJECTION,               // Projection neurons
        LOCAL_INTERNEURON,        // Local interneurons
        DEEP_LAYER,              // Deep layer neurons
        SUPERFICIAL_LAYER,       // Superficial layer neurons
        BARREL_CORTEX,            // Barrel cortex neurons
        VISUAL_CORTEX,            // Visual cortex neurons
        MOTOR_CORTEX,             // Motor cortex neurons
        ASSOCIATION,              // Association neurons
        OUTPUT,                   // Output neurons
        INPUT,                    // Input neurons
        FEEDBACK,                  // Feedback neurons
        FEEDFORWARD,               // Feedforward neurons
        RECURRENT,                // Recurrent neurons
        SPARSE,                   // Sparse populations
        DENSE,                    // Dense populations
        BALANCED,                 // Balanced populations
        UNBALANCED,               // Unbalanced populations
    };

    // ===== NETWORK TYPES =====

    // Network types
    enum class NetworkType {
        RECURRENT,                // Recurrent networks
        FEEDFORWARD,               // Feedforward networks
        FEEDBACK,                  // Feedback networks
        BIDIRECTIONAL,             // Bidirectional networks
        CYCLIC,                    // Cyclic networks
        RANDOM,                   // Random networks
        STRUCTURED,               // Structured networks
        HIERARCHICAL,              // Hierarchical networks
        SCALE_FREE,                // Scale-free networks
        SMALL_WORLD,               // Small-world networks
        MODULAR,                   // Modular networks
        RANDOM_GRAPH,              // Random graph networks
        REGULAR_GRAPH,             // Regular graph networks
        COMPLETE_GRAPH,            // Complete graph networks
        STAR,                      // Star networks
        RING,                      // Ring networks
        LATTICE,                   // Lattice networks
        ER_RANDOM,                 // ER random networks
        BA_SCALE_FREE,             // BA scale-free networks
        WM_SMALL_WORLD,            // WM small-world networks
        RANDOM_GEOMETRIC,          // Random geometric networks
        GEOMETRIC_RANDOM_GRAPHS,    // Geometric random graphs
        UNIT_DISK,                 // Unit disk graphs
        RANDOM_NEIGHBOR,           // Random neighbor graphs
        BARABASI_ALBERT,            // Barabási-Albert models
        WATTS_STROGATZ,             // Watts-Strogatz models
        NEWMAN_WATTS,              // Newman-Watts models
        STOCHASTIC_BLOCK,          // Stochastic block models
        DEGREE_CORRECTED,          // Degree-corrected models
        CONFIGURATION,             // Configuration models
        EXPONENTIAL_RANDOM,        // Exponential random graphs
        RANDOM_CLUSTERED,          // Random clustered models
    };

    // ===== DYNAMICS TYPES =====

    // Dynamics types
    enum class DynamicsType {
        CHAOTIC,                   // Chaotic dynamics
        REGULAR,                   // Regular dynamics
        SPIKING,                   // Spiking dynamics
        BURSTING,                  // Bursting dynamics
        OSCILLATORY,               // Oscillatory dynamics
        QUIESCENT,                 // Quiescent dynamics
        TRANSITION,                // Transition dynamics
        HYBRID,                    // Hybrid dynamics
        MIXED,                     // Mixed dynamics
        STOCHASTIC,                // Stochastic dynamics
        DETERMINISTIC,             // Deterministic dynamics
        PROBABILISTIC,              // Probabilistic dynamics
        PSEUDO_STOCHASTIC,          // Pseud-stochastic dynamics
        FRACTIONAL,                // Fractional dynamics
        VISCOELASTIC,              // Viscoelastic dynamics
        NONLINEAR,                 // Nonlinear dynamics
        LINEAR,                    // Linear dynamics
        PIECEWISE_LINEAR,          // Piecewise linear dynamics
        SMOOTH,                    // Smooth dynamics
        NON_SMOOTH,                // Non-smooth dynamics
        DISCONTINUOUS,             // Discontinuous dynamics
        SMOOTH_AND_DISCONTINUOUS,   // Smooth-and-discontinuous dynamics
        DIFFERENTIABLE,            // Differentiable dynamics
        NON_DIFFERENTIABLE,        // Non-differentiable dynamics
        SMOOTH_DIFFERENTIABLE,     // Smooth-differentiable dynamics
        NON_SMOOTH_DIFFERENTIABLE, // Non-smooth-differentiable dynamics
        DIFFERENTIABLE_CONTINUOUS, // Differentiable-continuous dynamics
        NON_DIFFERENTIABLE_DISCONTINUOUS, // Non-differentiable-discontinuous dynamics
    };

    // ===== NEURON MODELS =====

    // Neuron models
    enum class NeuronModel {
        NEURONAL_FIELD,            // Neuronal field models
        NEURAL_MASS,               // Neuronal mass models
        RATE,                      // Rate models
        SPIKING,                   // Spiking models
        INTEGRATE_AND_FIRE,        // Integrate-and-fire models
        ADAPTIVE_EXPONENTIAL_INTEGRATE_AND_FIRE, // Adaptive exponential integrate-and-fire models
        HODGKIN_HUXLEY,             // Hodgkin-Huxley models
        MORRIS_LECAR,               // Morris-Lecar models
        FITZHUGH_NAGUMO,      // FitzHugh-Nagumo models
        IZHIKIEVICH,               // Izhikevich models
        QUADRATIC_INTEGRATE_AND_FIRE, // Quadratic integrate-and-fire models
        HINDMARRH_ROSE,             // Hindmarsh-Rose models
        GRANDFIFFIN_PERME,          // Grandmifflin-Perme models
        HINDMARRH_ROSE,             // Hindmarsh-Rose models
        IZHIKIEVICH,               // Izhikevich models
        QUADRATIC_INTEGRATE_AND_FIRE, // Quadratic integrate-and-fire models
        CHAOTIC_DYNAMICS,          // Chaotic dynamics
        REGULAR_DYNAMICS,          // Regular dynamics
        SPIKING_DYNAMICS,          // Spiking dynamics
        BURSTING_DYNAMICS,          // Bursting dynamics
        OSCILLATORY_DYNAMICS,       // Oscillatory dynamics
        QUIESCENT_DYNAMICS,         // Quiescent dynamics
        TRANSITION_DYNAMICS,        // Transition dynamics
        HYBRID_DYNAMICS,            // Hybrid dynamics
        MIXED_DYNAMICS,             // Mixed dynamics
        STOCHASTIC_DYNAMICS,        // Stochastic dynamics
        DETERMINISTIC_DYNAMICS,     // Deterministic dynamics
        PROBABILISTIC_DYNAMICS,      // Probabilistic dynamics
        PSEUDO_STOCHASTIC_DYNAMICS, // Pseud-stochastic dynamics
        FRACTIONAL_DYNAMICS,        // Fractional dynamics
        VISCOELASTIC_DYNAMICS,      // Viscoelastic dynamics
        NONLINEAR_DYNAMICS,         // Nonlinear dynamics
        LINEAR_DYNAMICS,            // Linear dynamics
        PIECEWISE_LINEAR_DYNAMICS, // Piecewise linear dynamics
        SMOOTH_DYNAMICS,            // Smooth dynamics
        NON_SMOOTH_DYNAMICS,        // Non-smooth dynamics
        DISCONTINUOUS_DYNAMICS,     // Discontinuous dynamics
        SMOOTH_AND_DISCONTINUOUS_DYNAMICS, // Smooth-and-discontinuous dynamics
        DIFFERENTIABLE_DYNAMICS,   // Differentiable dynamics
        NON_DIFFERENTIABLE_DYNAMICS, // Non-differentiable dynamics
        SMOOTH_DIFFERENTIABLE_DYNAMICS, // Smooth-differentiable dynamics
        NON_SMOOTH_DIFFERENTIABLE_DYNAMICS, // Non-smooth-differentiable dynamics
        DIFFERENTIABLE_CONTINUOUS_DYNAMICS, // Differentiable-continuous dynamics
        NON_DIFFERENTIABLE_DISCONTINUOUS_DYNAMICS, // Non-differentiable-discontinuous dynamics
    };

    // ===== NEURON POPULATION PARAMETERS =====

    // Common neuron population parameters
    struct NeuronPopulationParameters {
        // Population identity
        PopulationType populationType;     // Type of neuron population
        NetworkType networkType;          // Type of network
        DynamicsType dynamicsType;         // Type of dynamics
        NeuronModel neuronModel;          // Type of neuron model
        
        // Population characteristics
        size_t populationSize;            // Size of population
        float firingRate;                  // Firing rate (Hz)
        float excitationInhibitionRatio;  // Excitation-inhibition ratio
        float averageFiringRate;           // Average firing rate
        float standardDeviationFiringRate; // Standard deviation of firing rate
        
        // Neuron characteristics
        float membranePotential;          // Membrane potential (mV)
        float threshold;                   // Threshold (mV)
        float resetPotential;              // Reset potential (mV)
        float restingPotential;            // Resting potential (mV)
        float leakConductance;              // Leak conductance (nS)
        float synapticConductance;          // Synaptic conductance (nS)
        
        // Connection characteristics
        float connectionProbability;       // Connection probability
        float connectionStrength;           // Connection strength
        float connectionDelay;              // Connection delay (steps)
        
        // Plasticity characteristics
        bool plasticityEnabled;             // Whether plasticity is enabled
        float plasticityRate;               // Plasticity rate
        float plasticityThreshold;          // Plasticity threshold
        float plasticityTimeConstant;       // Plasticity time constant
        
        // Activity characteristics
        float activityLevel;                 // Activity level
        float activityNoise;                 // Activity noise
        float activitySynchronization;      // Activity synchronization
        
        // Dynamics characteristics
        float timeConstant;                  // Time constant (ms)
        float noiseLevel;                    // Noise level
        float couplingStrength;              // Coupling strength
        float externalInput;                 // External input
    };

    // ===== EXCITATORY NEURONS =====

    // Excitatory neuron population parameters
    struct ExcitatoryNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Excitatory-specific parameters
        float excitatoryStrength;           // Excitatory strength
        float excitatoryGain;                // Excitatory gain
        float excitatoryThreshold;           // Excitatory threshold
        float excitatoryTimeConstant;        // Excitatory time constant
        
        // Connection parameters
        float excitatoryConnectionProbability; // Excitatory connection probability
        float excitatoryConnectionStrength;   // Excitatory connection strength
        float excitatoryConnectionDelay;       // Excitatory connection delay
        
        // Plasticity parameters
        float excitatoryPlasticityRate;      // Excitatory plasticity rate
        float excitatoryPlasticityThreshold; // Excitatory plasticity threshold
        float excitatoryPlasticityTimeConstant; // Excitatory plasticity time constant
        
        // Activity parameters
        float excitatoryActivityLevel;       // Excitatory activity level
        float excitatoryActivityNoise;       // Excitatory activity noise
        float excitatoryActivitySynchronization; // Excitatory activity synchronization
        
        // Dynamics parameters
        float excitatoryTimeConstant;        // Excitatory time constant
        float excitatoryNoiseLevel;          // Excitatory noise level
        float excitatoryCouplingStrength;    // Excitatory coupling strength
        float excitatoryExternalInput;       // Excitatory external input
    };

    // ===== INHIBITORY NEURONS =====

    // Inhibitory neuron population parameters
    struct InhibitoryNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Inhibitory-specific parameters
        float inhibitoryStrength;           // Inhibitory strength
        float inhibitoryGain;                // Inhibitory gain
        float inhibitoryThreshold;           // Inhibitory threshold
        float inhibitoryTimeConstant;        // Inhibitory time constant
        
        // Connection parameters
        float inhibitoryConnectionProbability; // Inhibitory connection probability
        float inhibitoryConnectionStrength;   // Inhibitory connection strength
        float inhibitoryConnectionDelay;      // Inhibitory connection delay
        
        // Plasticity parameters
        float inhibitoryPlasticityRate;      // Inhibitory plasticity rate
        float inhibitoryPlasticityThreshold; // Inhibitory plasticity threshold
        float inhibitoryPlasticityTimeConstant; // Inhibitory plasticity time constant
        
        // Activity parameters
        float inhibitoryActivityLevel;       // Inhibitory activity level
        float inhibitoryActivityNoise;       // Inhibitory activity noise
        float inhibitoryActivitySynchronization; // Inhibitory activity synchronization
        
        // Dynamics parameters
        float inhibitoryTimeConstant;        // Inhibitory time constant
        float inhibitoryNoiseLevel;          // Inhibitory noise level
        float inhibitoryCouplingStrength;    // Inhibitory coupling strength
        float inhibitoryExternalInput;       // Inhibitory external input
    };

    // ===== MODULATORY NEURONS =====

    // Modulatory neuron population parameters
    struct ModulatoryNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Modulatory-specific parameters
        float modulatoryStrength;           // Modulatory strength
        float modulatoryGain;                // Modulatory gain
        float modulatoryThreshold;           // Modulatory threshold
        float modulatoryTimeConstant;        // Modulatory time constant
        
        // Connection parameters
        float modulatoryConnectionProbability; // Modulatory connection probability
        float modulatoryConnectionStrength;   // Modulatory connection strength
        float modulatoryConnectionDelay;       // Modulatory connection delay
        
        // Plasticity parameters
        float modulatoryPlasticityRate;      // Modulatory plasticity rate
        float modulatoryPlasticityThreshold; // Modulatory plasticity threshold
        float modulatoryPlasticityTimeConstant; // Modulatory plasticity time constant
        
        // Activity parameters
        float modulatoryActivityLevel;       // Modulatory activity level
        float modulatoryActivityNoise;       // Modulatory activity noise
        float modulatoryActivitySynchronization; // Modulatory activity synchronization
        
        // Dynamics parameters
        float modulatoryTimeConstant;        // Modulatory time constant
        float modulatoryNoiseLevel;          // Modulatory noise level
        float modulatoryCouplingStrength;    // Modulatory coupling strength
        float modulatoryExternalInput;       // Modulatory external input
        
        // Neuromodulatory type
        NeuromodulatorType neuromodulatorType; // Type of neuromodulator
    };

    // ===== SENSORY NEURONS =====

    // Sensory neuron population parameters
    struct SensoryNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Sensory-specific parameters
        float sensoryStrength;              // Sensory strength
        float sensoryGain;                   // Sensory gain
        float sensoryThreshold;              // Sensory threshold
        float sensoryTimeConstant;           // Sensory time constant
        
        // Connection parameters
        float sensoryConnectionProbability;  // Sensory connection probability
        float sensoryConnectionStrength;     // Sensory connection strength
        float sensoryConnectionDelay;         // Sensory connection delay
        
        // Plasticity parameters
        float sensoryPlasticityRate;         // Sensory plasticity rate
        float sensoryPlasticityThreshold;    // Sensory plasticity threshold
        float sensoryPlasticityTimeConstant; // Sensory plasticity time constant
        
        // Activity parameters
        float sensoryActivityLevel;          // Sensory activity level
        float sensoryActivityNoise;          // Sensory activity noise
        float sensoryActivitySynchronization; // Sensory activity synchronization
        
        // Dynamics parameters
        float sensoryTimeConstant;           // Sensory time constant
        float sensoryNoiseLevel;             // Sensory noise level
        float sensoryCouplingStrength;       // Sensory coupling strength
        float sensoryExternalInput;          // Sensory external input
        
        // Sensory modality
        SensorModality sensoryModality;      // Sensory modality
    };

    // ===== MOTOR NEURONS =====

    // Motor neuron population parameters
    struct MotorNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Motor-specific parameters
        float motorStrength;                 // Motor strength
        float motorGain;                      // Motor gain
        float motorThreshold;                 // Motor threshold
        float motorTimeConstant;              // Motor time constant
        
        // Connection parameters
        float motorConnectionProbability;     // Motor connection probability
        float motorConnectionStrength;        // Motor connection strength
        float motorConnectionDelay;            // Motor connection delay
        
        // Plasticity parameters
        float motorPlasticityRate;             // Motor plasticity rate
        float motorPlasticityThreshold;        // Motor plasticity threshold
        float motorPlasticityTimeConstant;     // Motor plasticity time constant
        
        // Activity parameters
        float motorActivityLevel;              // Motor activity level
        float motorActivityNoise;              // Motor activity noise
        float motorActivitySynchronization;   // Motor activity synchronization
        
        // Dynamics parameters
        float motorTimeConstant;               // Motor time constant
        float motorNoiseLevel;                 // Motor noise level
        float motorCouplingStrength;           // Motor coupling strength
        float motorExternalInput;              // Motor external input
        
        // Motor function
        MotorFunction motorFunction;           // Motor function
    };

    // ===== INTERNEURONS =====

    // Interneuron population parameters
    struct InterneuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Interneuron-specific parameters
        float interneuronStrength;            // Interneuron strength
        float interneuronGain;                 // Interneuron gain
        float interneuronThreshold;            // Interneuron threshold
        float interneuronTimeConstant;         // Interneuron time constant
        
        // Connection parameters
        float interneuronConnectionProbability; // Interneuron connection probability
        float interneuronConnectionStrength;    // Interneuron connection strength
        float interneuronConnectionDelay;       // Interneuron connection delay
        
        // Plasticity parameters
        float interneuronPlasticityRate;       // Interneuron plasticity rate
        float interneuronPlasticityThreshold;  // Interneuron plasticity threshold
        float interneuronPlasticityTimeConstant; // Interneuron plasticity time constant
        
        // Activity parameters
        float interneuronActivityLevel;        // Interneuron activity level
        float interneuronActivityNoise;        // Interneuron activity noise
        float interneuronActivitySynchronization; // Interneuron activity synchronization
        
        // Dynamics parameters
        float interneuronTimeConstant;         // Interneuron time constant
        float interneuronNoiseLevel;           // Interneuron noise level
        float interneuronCouplingStrength;     // Interneuron coupling strength
        float interneuronExternalInput;        // Interneuron external input
        
        // Interneuron type
        InterneuronType interneuronType;       // Type of interneuron
    };

    // ===== PROJECTION NEURONS =====

    // Projection neuron population parameters
    struct ProjectionNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Projection-specific parameters
        float projectionStrength;             // Projection strength
        float projectionGain;                  // Projection gain
        float projectionThreshold;             // Projection threshold
        float projectionTimeConstant;          // Projection time constant
        
        // Connection parameters
        float projectionConnectionProbability; // Projection connection probability
        float projectionConnectionStrength;    // Projection connection strength
        float projectionConnectionDelay;        // Projection connection delay
        
        // Plasticity parameters
        float projectionPlasticityRate;        // Projection plasticity rate
        float projectionPlasticityThreshold;   // Projection plasticity threshold
        float projectionPlasticityTimeConstant; // Projection plasticity time constant
        
        // Activity parameters
        float projectionActivityLevel;         // Projection activity level
        float projectionActivityNoise;         // Projection activity noise
        float projectionActivitySynchronization; // Projection activity synchronization
        
        // Dynamics parameters
        float projectionTimeConstant;          // Projection time constant
        float projectionNoiseLevel;            // Projection noise level
        float projectionCouplingStrength;      // Projection coupling strength
        float projectionExternalInput;         // Projection external input
        
        // Projection type
        ProjectionType projectionType;         // Type of projection
    };

    // ===== LOCAL INTERNEURONS =====

    // Local interneuron population parameters
    struct LocalInterneuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Local interneuron-specific parameters
        float localInterneuronStrength;        // Local interneuron strength
        float localInterneuronGain;             // Local interneuron gain
        float localInterneuronThreshold;        // Local interneuron threshold
        float localInterneuronTimeConstant;     // Local interneuron time constant
        
        // Connection parameters
        float localInterneuronConnectionProbability; // Local interneuron connection probability
        float localInterneuronConnectionStrength;    // Local interneuron connection strength
        float localInterneuronConnectionDelay;       // Local interneuron connection delay
        
        // Plasticity parameters
        float localInterneuronPlasticityRate;     // Local interneuron plasticity rate
        float localInterneuronPlasticityThreshold; // Local interneuron plasticity threshold
        float localInterneuronPlasticityTimeConstant; // Local interneuron plasticity time constant
        
        // Activity parameters
        float localInterneuronActivityLevel;      // Local interneuron activity level
        float localInterneuronActivityNoise;      // Local interneuron activity noise
        float localInterneuronActivitySynchronization; // Local interneuron activity synchronization
        
        // Dynamics parameters
        float localInterneuronTimeConstant;       // Local interneuron time constant
        float localInterneuronNoiseLevel;         // Local interneuron noise level
        float localInterneuronCouplingStrength;   // Local interneuron coupling strength
        float localInterneuronExternalInput;      // Local interneuron external input
        
        // Local interneuron type
        LocalInterneuronType localInterneuronType; // Type of local interneuron
    };

    // ===== DEEP LAYERS =====

    // Deep layer neuron population parameters
    struct DeepLayerNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Deep layer-specific parameters
        float deepLayerStrength;              // Deep layer strength
        float deepLayerGain;                   // Deep layer gain
        float deepLayerThreshold;              // Deep layer threshold
        float deepLayerTimeConstant;           // Deep layer time constant
        
        // Connection parameters
        float deepLayerConnectionProbability;   // Deep layer connection probability
        float deepLayerConnectionStrength;      // Deep layer connection strength
        float deepLayerConnectionDelay;         // Deep layer connection delay
        
        // Plasticity parameters
        float deepLayerPlasticityRate;          // Deep layer plasticity rate
        float deepLayerPlasticityThreshold;     // Deep layer plasticity threshold
        float deepLayerPlasticityTimeConstant;   // Deep layer plasticity time constant
        
        // Activity parameters
        float deepLayerActivityLevel;           // Deep layer activity level
        float deepLayerActivityNoise;           // Deep layer activity noise
        float deepLayerActivitySynchronization; // Deep layer activity synchronization
        
        // Dynamics parameters
        float deepLayerTimeConstant;            // Deep layer time constant
        float deepLayerNoiseLevel;              // Deep layer noise level
        float deepLayerCouplingStrength;        // Deep layer coupling strength
        float deepLayerExternalInput;           // Deep layer external input
        
        // Deep layer type
        DeepLayerType deepLayerType;            // Type of deep layer
    };

    // ===== SUPERFICIAL LAYERS =====

    // Superficial layer neuron population parameters
    struct SuperficialLayerNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Superficial layer-specific parameters
        float superficialLayerStrength;        // Superficial layer strength
        float superficialLayerGain;             // Superficial layer gain
        float superficialLayerThreshold;        // Superficial layer threshold
        float superficialLayerTimeConstant;     // Superficial layer time constant
        
        // Connection parameters
        float superficialLayerConnectionProbability; // Superficial layer connection probability
        float superficialLayerConnectionStrength;    // Superficial layer connection strength
        float superficialLayerConnectionDelay;       // Superficial layer connection delay
        
        // Plasticity parameters
        float superficialLayerPlasticityRate;   // Superficial layer plasticity rate
        float superficialLayerPlasticityThreshold; // Superficial layer plasticity threshold
        float superficialLayerPlasticityTimeConstant; // Superficial layer plasticity time constant
        
        // Activity parameters
        float superficialLayerActivityLevel;    // Superficial layer activity level
        float superficialLayerActivityNoise;    // Superficial layer activity noise
        float superficialLayerActivitySynchronization; // Superficial layer activity synchronization
        
        // Dynamics parameters
        float superficialLayerTimeConstant;      // Superficial layer time constant
        float superficialLayerNoiseLevel;        // Superficial layer noise level
        float superficialLayerCouplingStrength;  // Superficial layer coupling strength
        float superficialLayerExternalInput;     // Superficial layer external input
        
        // Superficial layer type
        SuperficialLayerType superficialLayerType; // Type of superficial layer
    };

    // ===== BARREL CORTEX NEURONS =====

    // Barrel cortex neuron population parameters
    struct BarrelCortexNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Barrel cortex-specific parameters
        float barrelCortexStrength;             // Barrel cortex strength
        float barrelCortexGain;                  // Barrel cortex gain
        float barrelCortexThreshold;             // Barrel cortex threshold
        float barrelCortexTimeConstant;          // Barrel cortex time constant
        
        // Connection parameters
        float barrelCortexConnectionProbability; // Barrel cortex connection probability
        float barrelCortexConnectionStrength;    // Barrel cortex connection strength
        float barrelCortexConnectionDelay;        // Barrel cortex connection delay
        
        // Plasticity parameters
        float barrelCortexPlasticityRate;        // Barrel cortex plasticity rate
        float barrelCortexPlasticityThreshold;   // Barrel cortex plasticity threshold
        float barrelCortexPlasticityTimeConstant; // Barrel cortex plasticity time constant
        
        // Activity parameters
        float barrelCortexActivityLevel;          // Barrel cortex activity level
        float barrelCortexActivityNoise;          // Barrel cortex activity noise
        float barrelCortexActivitySynchronization; // Barrel cortex activity synchronization
        
        // Dynamics parameters
        float barrelCortexTimeConstant;           // Barrel cortex time constant
        float barrelCortexNoiseLevel;             // Barrel cortex noise level
        float barrelCortexCouplingStrength;       // Barrel cortex coupling strength
        float barrelCortexExternalInput;          // Barrel cortex external input
        
        // Barrel cortex type
        BarrelCortexType barrelCortexType;       // Type of barrel cortex
    };

    // ===== VISUAL CORTEX NEURONS =====

    // Visual cortex neuron population parameters
    struct VisualCortexNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Visual cortex-specific parameters
        float visualCortexStrength;              // Visual cortex strength
        float visualCortexGain;                   // Visual cortex gain
        float visualCortexThreshold;              // Visual cortex threshold
        float visualCortexTimeConstant;           // Visual cortex time constant
        
        // Connection parameters
        float visualCortexConnectionProbability; // Visual cortex connection probability
        float visualCortexConnectionStrength;    // Visual cortex connection strength
        float visualCortexConnectionDelay;        // Visual cortex connection delay
        
        // Plasticity parameters
        float visualCortexPlasticityRate;         // Visual cortex plasticity rate
        float visualCortexPlasticityThreshold;    // Visual cortex plasticity threshold
        float visualCortexPlasticityTimeConstant; // Visual cortex plasticity time constant
        
        // Activity parameters
        float visualCortexActivityLevel;           // Visual cortex activity level
        float visualCortexActivityNoise;           // Visual cortex activity noise
        float visualCortexActivitySynchronization; // Visual cortex activity synchronization
        
        // Dynamics parameters
        float visualCortexTimeConstant;            // Visual cortex time constant
        float visualCortexNoiseLevel;              // Visual cortex noise level
        float visualCortexCouplingStrength;        // Visual cortex coupling strength
        float visualCortexExternalInput;           // Visual cortex external input
        
        // Visual cortex type
        VisualCortexType visualCortexType;        // Type of visual cortex
        
        // Visual field
        VisualField visualField;                // Visual field
    };

    // ===== MOTOR CORTEX NEURONS =====

    // Motor cortex neuron population parameters
    struct MotorCortexNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Motor cortex-specific parameters
        float motorCortexStrength;              // Motor cortex strength
        float motorCortexGain;                   // Motor cortex gain
        float motorCortexThreshold;              // Motor cortex threshold
        float motorCortexTimeConstant;           // Motor cortex time constant
        
        // Connection parameters
        float motorCortexConnectionProbability; // Motor cortex connection probability
        float motorCortexConnectionStrength;    // Motor cortex connection strength
        float motorCortexConnectionDelay;        // Motor cortex connection delay
        
        // Plasticity parameters
        float motorCortexPlasticityRate;         // Motor cortex plasticity rate
        float motorCortexPlasticityThreshold;    // Motor cortex plasticity threshold
        float motorCortexPlasticityTimeConstant; // Motor cortex plasticity time constant
        
        // Activity parameters
        float motorCortexActivityLevel;           // Motor cortex activity level
        float motorCortexActivityNoise;           // Motor cortex activity noise
        float motorCortexActivitySynchronization; // Motor cortex activity synchronization
        
        // Dynamics parameters
        float motorCortexTimeConstant;            // Motor cortex time constant
        float motorCortexNoiseLevel;              // Motor cortex noise level
        float motorCortexCouplingStrength;        // Motor cortex coupling strength
        float motorCortexExternalInput;           // Motor cortex external input
        
        // Motor cortex type
        MotorCortexType motorCortexType;        // Type of motor cortex
        
        // Movement
        MovementType movementType;              // Movement type
    };

    // ===== ASSOCIATION NEURONS =====

    // Association neuron population parameters
    struct AssociationNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Association-specific parameters
        float associationStrength;              // Association strength
        float associationGain;                   // Association gain
        float associationThreshold;              // Association threshold
        float associationTimeConstant;           // Association time constant
        
        // Connection parameters
        float associationConnectionProbability; // Association connection probability
        float associationConnectionStrength;    // Association connection strength
        float associationConnectionDelay;        // Association connection delay
        
        // Plasticity parameters
        float associationPlasticityRate;         // Association plasticity rate
        float associationPlasticityThreshold;    // Association plasticity threshold
        float associationPlasticityTimeConstant; // Association plasticity time constant
        
        // Activity parameters
        float associationActivityLevel;           // Association activity level
        float associationActivityNoise;           // Association activity noise
        float associationActivitySynchronization; // Association activity synchronization
        
        // Dynamics parameters
        float associationTimeConstant;            // Association time constant
        float associationNoiseLevel;              // Association noise level
        float associationCouplingStrength;        // Association coupling strength
        float associationExternalInput;           // Association external input
        
        // Association type
        AssociationType associationType;        // Type of association
    };

    // ===== OUTPUT NEURONS =====

    // Output neuron population parameters
    struct OutputNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Output-specific parameters
        float outputStrength;                    // Output strength
        float outputGain;                        // Output gain
        float outputThreshold;                   // Output threshold
        float outputTimeConstant;                // Output time constant
        
        // Connection parameters
        float outputConnectionProbability;        // Output connection probability
        float outputConnectionStrength;           // Output connection strength
        float outputConnectionDelay;               // Output connection delay
        
        // Plasticity parameters
        float outputPlasticityRate;                // Output plasticity rate
        float outputPlasticityThreshold;           // Output plasticity threshold
        float outputPlasticityTimeConstant;        // Output plasticity time constant
        
        // Activity parameters
        float outputActivityLevel;                 // Output activity level
        float outputActivityNoise;                 // Output activity noise
        float outputActivitySynchronization;      // Output activity synchronization
        
        // Dynamics parameters
        float outputTimeConstant;                  // Output time constant
        float outputNoiseLevel;                    // Output noise level
        float outputCouplingStrength;              // Output coupling strength
        float outputExternalInput;                 // Output external input
        
        // Output type
        OutputType outputType;                    // Type of output
    };

    // ===== INPUT NEURONS =====

    // Input neuron population parameters
    struct InputNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Input-specific parameters
        float inputStrength;                     // Input strength
        float inputGain;                         // Input gain
        float inputThreshold;                    // Input threshold
        float inputTimeConstant;                 // Input time constant
        
        // Connection parameters
        float inputConnectionProbability;         // Input connection probability
        float inputConnectionStrength;            // Input connection strength
        float inputConnectionDelay;                // Input connection delay
        
        // Plasticity parameters
        float inputPlasticityRate;                 // Input plasticity rate
        float inputPlasticityThreshold;            // Input plasticity threshold
        float inputPlasticityTimeConstant;         // Input plasticity time constant
        
        // Activity parameters
        float inputActivityLevel;                  // Input activity level
        float inputActivityNoise;                  // Input activity noise
        float inputActivitySynchronization;       // Input activity synchronization
        
        // Dynamics parameters
        float inputTimeConstant;                   // Input time constant
        float inputNoiseLevel;                     // Input noise level
        float inputCouplingStrength;               // Input coupling strength
        float inputExternalInput;                  // Input external input
        
        // Input type
        InputType inputType;                      // Type of input
        
        // Input source
        InputSource inputSource;                  // Input source
    };

    // ===== FEEDBACK NEURONS =====

    // Feedback neuron population parameters
    struct FeedbackNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Feedback-specific parameters
        float feedbackStrength;                   // Feedback strength
        float feedbackGain;                        // Feedback gain
        float feedbackThreshold;                   // Feedback threshold
        float feedbackTimeConstant;                // Feedback time constant
        
        // Connection parameters
        float feedbackConnectionProbability;        // Feedback connection probability
        float feedbackConnectionStrength;           // Feedback connection strength
        float feedbackConnectionDelay;               // Feedback connection delay
        
        // Plasticity parameters
        float feedbackPlasticityRate;                // Feedback plasticity rate
        float feedbackPlasticityThreshold;           // Feedback plasticity threshold
        float feedbackPlasticityTimeConstant;        // Feedback plasticity time constant
        
        // Activity parameters
        float feedbackActivityLevel;                 // Feedback activity level
        float feedbackActivityNoise;                 // Feedback activity noise
        float feedbackActivitySynchronization;      // Feedback activity synchronization
        
        // Dynamics parameters
        float feedbackTimeConstant;                  // Feedback time constant
        float feedbackNoiseLevel;                    // Feedback noise level
        float feedbackCouplingStrength;              // Feedback coupling strength
        float feedbackExternalInput;                 // Feedback external input
        
        // Feedback type
        FeedbackType feedbackType;                  // Type of feedback
    };

    // ===== FEEDFORWARD NEURONS =====

    // Feedforward neuron population parameters
    struct FeedforwardNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Feedforward-specific parameters
        float feedforwardStrength;                // Feedforward strength
        float feedforwardGain;                     // Feedforward gain
        float feedforwardThreshold;                // Feedforward threshold
        float feedforwardTimeConstant;             // Feedforward time constant
        
        // Connection parameters
        float feedforwardConnectionProbability;     // Feedforward connection probability
        float feedforwardConnectionStrength;        // Feedforward connection strength
        float feedforwardConnectionDelay;           // Feedforward connection delay
        
        // Plasticity parameters
        float feedforwardPlasticityRate;           // Feedforward plasticity rate
        float feedforwardPlasticityThreshold;      // Feedforward plasticity threshold
        float feedforwardPlasticityTimeConstant;   // Feedforward plasticity time constant
        
        // Activity parameters
        float feedforwardActivityLevel;              // Feedforward activity level
        float feedforwardActivityNoise;              // Feedforward activity noise
        float feedforwardActivitySynchronization;   // Feedforward activity synchronization
        
        // Dynamics parameters
        float feedforwardTimeConstant;               // Feedforward time constant
        float feedforwardNoiseLevel;                 // Feedforward noise level
        float feedforwardCouplingStrength;           // Feedforward coupling strength
        float feedforwardExternalInput;              // Feedforward external input
        
        // Feedforward type
        FeedforwardType feedforwardType;            // Type of feedforward
    };

    // ===== RECURRENT NEURONS =====

    // Recurrent neuron population parameters
    struct RecurrentNeuronPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Recurrent-specific parameters
        float recurrentStrength;                  // Recurrent strength
        float recurrentGain;                       // Recurrent gain
        float recurrentThreshold;                  // Recurrent threshold
        float recurrentTimeConstant;               // Recurrent time constant
        
        // Connection parameters
        float recurrentConnectionProbability;       // Recurrent connection probability
        float recurrentConnectionStrength;          // Recurrent connection strength
        float recurrentConnectionDelay;              // Recurrent connection delay
        
        // Plasticity parameters
        float recurrentPlasticityRate;               // Recurrent plasticity rate
        float recurrentPlasticityThreshold;          // Recurrent plasticity threshold
        float recurrentPlasticityTimeConstant;       // Recurrent plasticity time constant
        
        // Activity parameters
        float recurrentActivityLevel;                // Recurrent activity level
        float recurrentActivityNoise;                // Recurrent activity noise
        float recurrentActivitySynchronization;     // Recurrent activity synchronization
        
        // Dynamics parameters
        float recurrentTimeConstant;                 // Recurrent time constant
        float recurrentNoiseLevel;                   // Recurrent noise level
        float recurrentCouplingStrength;             // Recurrent coupling strength
        float recurrentExternalInput;                // Recurrent external input
        
        // Recurrent type
        RecurrentType recurrentType;                 // Type of recurrent
    };

    // ===== SPARSE POPULATIONS =====

    // Sparse population parameters
    struct SparsePopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Sparse-specific parameters
        float sparseDensity;                      // Sparse density
        float sparseDistribution;                 // Sparse distribution
        float sparseConnectivity;                 // Sparse connectivity
        float sparseInteraction;                  // Sparse interaction
        
        // Connection parameters
        float sparseConnectionProbability;          // Sparse connection probability
        float sparseConnectionStrength;             // Sparse connection strength
        float sparseConnectionDelay;                 // Sparse connection delay
        
        // Activity parameters
        float sparseActivityLevel;                   // Sparse activity level
        float sparseActivityNoise;                   // Sparse activity noise
        float sparseActivitySynchronization;        // Sparse activity synchronization
        
        // Dynamics parameters
        float sparseTimeConstant;                    // Sparse time constant
        float sparseNoiseLevel;                      // Sparse noise level
        float sparseCouplingStrength;                // Sparse coupling strength
        float sparseExternalInput;                   // Sparse external input
        
        // Sparsity parameters
        float sparsityTarget;                       // Target sparsity
        float sparsityThreshold;                     // Sparsity threshold
        float sparsityTimeConstant;                  // Sparsity time constant
    };

    // ===== DENSE POPULATIONS =====

    // Dense population parameters
    struct DensePopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Dense-specific parameters
        float denseDensity;                       // Dense density
        float denseDistribution;                  // Dense distribution
        float denseConnectivity;                  // Dense connectivity
        float denseInteraction;                   // Dense interaction
        
        // Connection parameters
        float denseConnectionProbability;           // Dense connection probability
        float denseConnectionStrength;              // Dense connection strength
        float denseConnectionDelay;                  // Dense connection delay
        
        // Activity parameters
        float denseActivityLevel;                    // Dense activity level
        float denseActivityNoise;                    // Dense activity noise
        float denseActivitySynchronization;         // Dense activity synchronization
        
        // Dynamics parameters
        float denseTimeConstant;                     // Dense time constant
        float denseNoiseLevel;                       // Dense noise level
        float denseCouplingStrength;                 // Dense coupling strength
        float denseExternalInput;                    // Dense external input
        
        // Density parameters
        float densityTarget;                        // Target density
        float densityThreshold;                      // Density threshold
        float densityTimeConstant;                   // Density time constant
    };

    // ===== BALANCED POPULATIONS =====

    // Balanced population parameters
    struct BalancedPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Balanced-specific parameters
        float balancedExcitatoryInhibitoryRatio;     // Balanced excitation-inhibition ratio
        float balancedActivityLevel;                  // Balanced activity level
        float balancedActivityNoise;                  // Balanced activity noise
        float balancedActivitySynchronization;       // Balanced activity synchronization
        
        // Connection parameters
        float balancedConnectionProbability;          // Balanced connection probability
        float balancedConnectionStrength;             // Balanced connection strength
        float balancedConnectionDelay;                 // Balanced connection delay
        
        // Activity parameters
        float balancedTimeConstant;                    // Balanced time constant
        float balancedNoiseLevel;                      /* Balanced noise level
        float balancedCouplingStrength;                // Balanced coupling strength
        float balancedExternalInput;                   // Balanced external input
        
        // Balance parameters
        float balanceTarget;                         // Target balance
        float balanceThreshold;                       // Balance threshold
        float balanceTimeConstant;                    // Balance time constant
    };

    // ===== UNBALANCED POPULATIONS =====

    // Unbalanced population parameters
    struct UnbalancedPopulationParameters {
        NeuronPopulationParameters baseParams; // Base population parameters
        
        // Unbalanced-specific parameters
        float unbalancedExcitatoryInhibitoryRatio;    // Unbalanced excitation-inhibition ratio
        float unbalancedActivityLevel;                 // Unbalanced activity level
        float unbalancedActivityNoise;                 // Unbalanced activity noise
        float unbalancedActivitySynchronization;      // Unbalanced activity synchronization
        
        // Connection parameters
        float unbalancedConnectionProbability;       // Unbalanced connection probability
        float unbalancedConnectionStrength;          // Unbalanced connection strength
        float unbalancedConnectionDelay;              // Unbalanced connection delay
        
        // Activity parameters
        float unbalancedTimeConstant;                 // Unbalanced time constant
        float unbalancedNoiseLevel;                   // Unbalanced noise level
        float unbalancedCouplingStrength;             // Unbalanced coupling strength
        float unbalancedExternalInput;                // Unbalanced external input
        
        // Unbalance parameters
        float unbalanceTarget;                        // Target unbalance
        float unbalanceThreshold;                     // Unbalance threshold
        float unbalanceTimeConstant;                  // Unbalance time constant
    };

    // ===== LIFTS =====

    // Supporting enumerations
    enum class NeuromodulatorType {
        DOPAMINE,
        ACETYLCHOLINE,
        NORADRENALINE,
        SEROTONIN,
        ENDORPHIN,
        NOCICEPTIN,
        HISTAMINE,
        OREXIN,
        COTICOSTERONE,
        NEUROPEPTIDE_Y
    };

    enum class SensorModality {
        AUDITORY,
        VISUAL,
        SOMATOSENSORY,
        GUSTATORY,
        OLFACTORY,
        PROPRIOCEPTIVE,
        EQUILIBRIUM,
        INTEROCEPTIVE
    };

    enum class MotorFunction {
        GRIP,
        LOCOMOTION,
        POSTURE,
        SPEECH,
        VISUAL_TRACKING,
        saccadic,
        smooth_pursuit
    };

    enum class InterneuronType {
        FAST_SPiking,
        REGULAR_SPIKING,
        ADAPTIVE,
        INTRINSIC_BURSTING,
        DESUPRA_PULVINAR,
        CHUNGAN,  // VIP interneurons
        BOSTETIAN,  // PV interneurons
        SOMATIC,    // SOM interneurons
        DEEP,       // Deep layer interneurons
        SUPERFICIAL  // Superficial layer interneurons
    };

    enum class ProjectionType {
        FEEDFORWARD,
        FEEDBACK,
        RECURRENT,
        LATERAL,
        VERTICAL,
        HORIZONTAL,
        DIAGONAL,
        RANDOM
    };

    enum class LocalInterneuronType {
        BOSTETIAN,
        CHUNGAN,
        SOMATIC,
        SPINELESS
    };

    enum class DeepLayerType {
        DEEP_LAYER_I,
        DEEP_LAYER_II,
        DEEP_LAYER_III,
        DEEP_LAYER_IV
    };

    enum class SuperficialLayerType {
        SUPERFICIAL_LAYER_I,
        SUPERFICIAL_LAYER_II,
        SUPERFICIAL_LAYER_III,
        SUPERFICIAL_LAYER_IV
    };

    enum class BarrelCortexType {
        SWEET,
        SALTY,
        BITTER,
        SOUR
    };

    enum class VisualCortexType {
        PRIMARY,
        SECONDARY,
        TERTIARY,
        ASSOCIATION
    };

    enum class MotorCortexType {
        PRIMARY,
        SECONDARY,
        TERTIARY,
        ASSOCIATION
    };

    enum class AssociationType {
        SENSORY_ASSOCIATION,
        INTER_MODAL_ASSOCIATION,
        CROSS_MODAL_ASSOCIATION,
        HIGH_ORDER_ASSOCIATION
    };

    enum class OutputType {
        MOTOR,
        SPEECH,
        GRIP,
        LOCOMOTION,
        COGNITIVE
    };

    enum class InputType {
        SENSORY,
        CEREBELLAR,
        THALAMIC,
        CORTICAL,
        SUBCORTICAL
    };

    enum class InputSource {
        EXTERNAL,
        INTERNAL,
        CONDITIONED,
        UNCONDITIONED,
        LEARNED,
        INNATE
    };

    enum class FeedbackType {
        POSITIVE,
        NEGATIVE,
        RECIPROCAL,
        FEEDBACK_LOOPS,
        INHIBITIONARY,
        EXCITATORY
    };

    enum class FeedforwardType {
        DIRECT,
        INDIRECT,
        CASCADE,
        PARALLEL,
        CONCURRENT,
        SUCCESSIVE
    };

    enum class RecurrentType {
        SHORT_RANGE,
        LONG_RANGE,
        MIXED_RANGE,
        CIRCULAR,
        EXPONENTIAL
    };

    // ===== VISUAL FIELD =====

    // Visual field parameters
    struct VisualField {
        float visualFieldSize;          // Visual field size (degrees)
        float visualFieldDensity;       // Visual field density (pixels/degree)
        float visualFieldCenterX;       // Visual field center X (degrees)
        float visualFieldCenterY;       // Visual field center Y (degrees)
        float visualFieldRotation;      // Visual field rotation (degrees)
        float visualFieldScale;         // Visual field scale (0-1)
        
        // Visual field properties
        bool monocular;               // Monocular vision
        bool binocular;               // Binocular vision
        float visualFieldOverlap;      // Visual field overlap
        float visualFieldGap;          // Visual field gap
    };

    // ===== MOVEMENT =====

    // Movement parameters
    struct Movement {
        float movementSpeed;           // Movement speed (cm/s)
        float movementDirection;       // Movement direction (radians)
        float movementAmplitude;       // Movement amplitude
        float movementFrequency;       // Movement frequency (Hz)
        float movementPhase;           // Movement phase (radians)
        
        // Movement properties
        bool movementPeriodic;         // Periodic movement
        bool movementSustained;        // Sustained movement
        float movementDutyCycle;       // Movement duty cycle (0-1)
        float movementRecoveryTime;    // Movement recovery time (s)
    };

    // ===== CALCULATION METHODS =====

    // Calculate neuron population metrics
    struct NeuronPopulationMetrics {
        float averageFiringRate;          // Average firing rate (Hz)
        float standardDeviationFiringRate; // Standard deviation of firing rate
        float meanMembranePotential;      // Mean membrane potential (mV)
        float meanThreshold;              // Mean threshold (mV)
        float meanConnectionStrength;     // Mean connection strength
        float meanPlasticity;             // Mean plasticity
        float meanActivity;               // Mean activity level
        float meanNoise;                  // Mean noise level
        float meanCoupling;               // Mean coupling strength
        float meanExternalInput;          // Mean external input
        float efficiency;                 // Efficiency (0-1)
        float stability;                  // Stability (0-1)
    };

    // Calculate network metrics
    struct NetworkMetrics {
        float averageDegree;              // Average degree of neurons
        float clusteringCoefficient;     // Clustering coefficient
        float characteristicPathLength; // Characteristic path length
        float smallWorldLambda;          // Small-worldness parameter
        float modularityQ;                // Modularity coefficient
        float participationRatio;         // Participation ratio
        float efficiency;                 // Network efficiency
        float robustness;                 // Network robustness
        float synchronizability;          // Synchronizability
        float diameter;                   // Network diameter
        float density;                    // Network density
        float transitivity;               // Network transitivity
    };

    // Initialize neuron population with specified parameters
    bool initializeNeuronPopulation(const NeuronPopulationParameters& params);

    // Get neuron population metrics
    NeuronPopulationMetrics getNeuronPopulationMetrics() const;

    // Get network metrics
    NetworkMetrics getNetworkMetrics() const;

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedSpecializedNeuronPopulations : public Brain {
    public:
        IntegratedSpecializedNeuronPopulations(std::shared_ptr<Config> config,
                                              std::shared_ptr<SpecializedNeuronPopulations> populations);
        ~IntegratedSpecializedNeuronPopulations() override;

        // Initialize specialized neuron populations
        bool initialize() override;

        // Get neuron population metrics
        NeuronPopulationMetrics getNeuronPopulationMetrics() const;

        // Get network metrics
        NetworkMetrics getNetworkMetrics() const;

        // Get neuron population parameters
        const NeuronPopulationParameters& getNeuronPopulationParameters() const {
            return currentNeuronPopulationParameters_;
        }

        // Set neuron population parameters
        void setNeuronPopulationParameters(const NeuronPopulationParameters& params) {
            currentNeuronPopulationParameters_ = params;
        }

        // Update neuron population parameters
        void updateNeuronPopulationParameters(const NeuronPopulationParameters& params, float dt);

        // Get specialized neuron population metrics
        struct SpecializedNeuronPopulationMetrics {
            float excitatoryMetrics;          // Excitatory neuron metrics
            float inhibitoryMetrics;          // Inhibitory neuron metrics
            float modulatoryMetrics;          // Modulatory neuron metrics
            float sensoryMetrics;             // Sensory neuron metrics
            float motorMetrics;               // Motor neuron metrics
            float interneuronMetrics;         // Interneuron metrics
            float projectionMetrics;          // Projection neuron metrics
            float localInterneuronMetrics;     // Local interneuron metrics
            float deepLayerMetrics;           // Deep layer neuron metrics
            float superficialLayerMetrics;    // Superficial layer neuron metrics
            float barrelCortexMetrics;        // Barrel cortex neuron metrics
            float visualCortexMetrics;        // Visual cortex neuron metrics
            float motorCortexMetrics;         // Motor cortex neuron metrics
            float associationMetrics;         // Association neuron metrics
            float outputMetrics;              // Output neuron metrics
            float inputMetrics;               // Input neuron metrics
            float feedbackMetrics;            // Feedback neuron metrics
            float feedforwardMetrics;        // Feedforward neuron metrics
            float recurrentMetrics;           // Recurrent neuron metrics
            float sparseMetrics;              // Sparse population metrics
            float denseMetrics;               // Dense population metrics
            float balancedMetrics;            // Balanced population metrics
            float unbalancedMetrics;          // Unbalanced population metrics
        };

        SpecializedNeuronPopulationMetrics getSpecializedNeuronPopulationMetrics() const;

        // Get network metrics
        NetworkMetrics getNetworkMetrics() const;

    private:
        std::shared_ptr<SpecializedNeuronPopulations> populations_;
        NeuronPopulationParameters currentNeuronPopulationParameters_;
        NeuronPopulationMetrics currentMetrics_;
        NetworkMetrics currentNetworkMetrics_;
        SpecializedNeuronPopulationMetrics currentSpecializedMetrics_;
    };

private:
    // Neuron population configuration
    bool excitatoryEnabled_;
    bool inhibitoryEnabled_;
    bool modulatoryEnabled_;
    bool sensoryEnabled_;
    bool motorEnabled_;
    bool interneuronEnabled_;
    bool projectionEnabled_;
    bool localInterneuronEnabled_;
    bool deepLayerEnabled_;
    bool superficialLayerEnabled_;
    bool barrelCortexEnabled_;
    bool visualCortexEnabled_;
    bool motorCortexEnabled_;
    bool associationEnabled_;
    bool outputEnabled_;
    bool inputEnabled_;
    bool feedbackEnabled_;
    bool feedforwardEnabled_;
    bool recurrentEnabled_;
    bool sparseEnabled_;
    bool denseEnabled_;
    bool balancedEnabled_;
    bool unbalancedEnabled_;

    // Neuron population parameters
    ExcitatoryNeuronPopulationParameters excitatoryParams_;
    InhibitoryNeuronPopulationParameters inhibitoryParams_;
    ModulatoryNeuronPopulationParameters modulatoryParams_;
    SensoryNeuronPopulationParameters sensoryParams_;
    MotorNeuronPopulationParameters motorParams_;
    InterneuronPopulationParameters interneuronParams_;
    ProjectionNeuronPopulationParameters projectionParams_;
    LocalInterneuronPopulationParameters localInterneuronParams_;
    DeepLayerNeuronPopulationParameters deepLayerParams_;
    SuperficialLayerNeuronPopulationParameters superficialLayerParams_;
    BarrelCortexNeuronPopulationParameters barrelCortexParams_;
    VisualCortexNeuronPopulationParameters visualCortexParams_;
    MotorCortexNeuronPopulationParameters motorCortexParams_;
    AssociationNeuronPopulationParameters associationParams_;
    OutputNeuronPopulationParameters outputParams_;
    InputNeuronPopulationParameters inputParams_;
    FeedbackNeuronPopulationParameters feedbackParams_;
    FeedforwardNeuronPopulationParameters feedforwardParams_;
    RecurrentNeuronPopulationParameters recurrentParams_;
    SparsePopulationParameters sparseParams_;
    DensePopulationParameters denseParams_;
    BalancedPopulationParameters balancedParams_;
    UnbalancedPopulationParameters unbalancedParams_;

    // Statistics
    NeuronPopulationMetrics metrics_;
    NetworkMetrics networkMetrics_;
    SpecializedNeuronPopulationMetrics specializedMetrics_;

    // Helper methods
    NeuronPopulationMetrics calculateNeuronPopulationMetrics() const;
    NetworkMetrics calculateNetworkMetrics() const;
    SpecializedNeuronPopulationMetrics calculateSpecializedNeuronPopulationMetrics() const;
    bool validateNeuronPopulationParameters(const NeuronPopulationParameters& params) const;
    void enableAllNeuronPopulations(bool enable);
};

} // namespace nlm
