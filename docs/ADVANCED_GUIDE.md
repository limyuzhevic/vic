# NLM Advanced Guide

## Overview

This guide provides comprehensive technical details for advanced NLM users, including:

- Deep dive into neural computation models
- Configuration options and presets
- Performance optimization techniques
- Memory system details
- Neuromodulation mechanisms
- Development process insights

## Neural Computation Models

### Neuron Dynamics

#### Leaky Integrate-and-Fire (LIF) Model
```cpp
class Neuron {
public:
    // Membrane potential
    float v;                    // Current membrane potential (mV)
    float v_rest;              // Resting potential (mV)
    float v_thresh;            // Spike threshold (mV)
    float v_reset;             // Reset potential after spike (mV)
    
    // Time constants
    float tau_m;               // Membrane time constant (ms)
    float tau_ref;             // Refractory period (ms)
    
    // State variables
    float I_syn;               // Total synaptic current
    int refractory_steps;      // Remaining refractory steps
    
    // Update equation (continuous time):
    // τ_m * dv/dt = -(v - v_rest) + I_syn(t)
    
    // Discrete update (per simulation step):
    // v = v + (dt/tau_m) * (-(v - v_rest) + I_syn)
};
```

#### Key Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `v_thresh` | float | -50.0 | [-100, 0] | Spike threshold potential |
| `v_rest` | float | -70.0 | [-80, -60] | Membrane resting potential |
| `v_reset` | float | -75.0 | [-80, -70] | Potential after spike |
| `tau_m` | float | 20.0 | [5, 50] | Membrane time constant |
| `tau_ref` | float | 2.0 | [0.5, 5] | Refractory period |

### Synaptic Transmission

#### Synapse Model
```cpp
class Synapse {
public:
    float weight;              // Synaptic strength
    float delay;               // Transmission delay (ms)
    float conductance;         // Synaptic conductance
    float reversal_potential;  // Nernst potential (mV)
    
    // Transmission equation:
    // I_syn = g_syn * (v - E_syn)
    
    // STDP update:
    // Δw = A_plus * exp(-Δt/τ_plus)  if Δt > 0 (post before pre)
    // Δw = -A_minus * exp(Δt/τ_minus) if Δt < 0 (post after pre)
};
```

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticity (STDP)

#### Implementation Details
```cpp
class STDP {
public:
    // Potentiation parameters
    float A_plus;              // Maximum potentiation
    float tau_plus;            // Time constant (ms)
    
    // Depression parameters  
    float A_minus;             // Maximum depression
    float tau_minus;           // Time constant (ms)
    
    // Update rules:
    // if t_post > t_pre:  Δw = A_plus * exp(-(t_post - t_pre)/τ_plus)
    // if t_post < t_pre:  Δw = -A_minus * exp((t_post - t_pre)/τ_minus)
    
    // Default parameters
    static STDP getDefaultConfig() {
        STDP stdp;
        stdp.A_plus = 0.01;
        stdp.tau_plus = 20.0;
        stdp.A_minus = 0.012;
        stdp.tau_minus = 20.0;
        return stdp;
    }
};
```

### Hebbian Learning

#### Pattern Completion
```cpp
class Hebbian {
public:
    float learning_rate;       // Learning speed
    float weight_max;          // Maximum weight
    float weight_min;          // Minimum weight
    
    // Update rule:
    // w(t+dt) = w(t) + η * x_pre * x_post
    
    // Weight normalization:
    // w_new = w_old * (1 - η) + η * (x_pre * x_post)
};
```

### Structural Plasticity

#### Synapse Formation and Pruning
```cpp
class StructuralPlasticity {
public:
    // Formation parameters
    float formation_rate;      // Probability of new synapse per ms
    float max_synapses_per_neuron;  // Hard limit
    
    // Pruning parameters
    float pruning_threshold;   // Weight below which synapse is removed
    float pruning_rate;        // Probability of pruning per ms
    
    // Developmental modulation
    float critical_period_start;   // When development begins
    float critical_period_end;     // When development ends
    
    // Activity-dependent pruning:
    // If |w| < threshold AND activity < minimum_activity:
    //   prune_synapse()
};
```

## Configuration System

### Configuration Structure

```ini
[section.subsection.parameter]
value = type
```

#### Core Configuration

| Path | Type | Default | Description |
|------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `brain.initial_weight_mean` | float | 0.5 | Initial weights |
| `brain.initial_weight_std` | float | 0.1 | Initial weight variation |

#### Plasticity Configuration

```ini
[plasticity.stdp.enable] = true
[plasticity.stdp.learning_rate] = 0.001
[plasticity.stdp.tau_plus] = 20.0
[plasticity.stdp.tau_minus] = 20.0

[plasticity.hebbian.enable] = true
[plasticity.hebbian.learning_rate] = 0.001

[plasticity.structural.enable] = true
[plasticity.structural.formation_rate] = 0.01
[plasticity.structural.pruning_threshold] = 0.1
```

#### Neuromodulation Configuration

```ini
[neuromod.dopamine.scale] = 1.0
[neuromod.curiosity.enable] = true
[neuromod.novelty.enable] = true

[neuromod.prediction_error.scale] = 1.0
[neuromod.prediction_error.decay_rate] = 0.1
```

### Configuration Management API

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query values
if config.has("brain.neuron_count"):
    neurons = config.getInt("brain.neuron_count")

# Modify values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Save/Load
config.saveToFile("my_config.cfg")
config.loadFromFile("my_config.cfg")

# Validate
validation_result = config.validate()
if not validation_result.isValid:
    print("Invalid config:", validation_result.errors)
```

## Performance Optimization

### Event-Driven Processing

#### Spike Propagation
```cpp
// Event queue for spikes
class EventQueue {
public:
    void scheduleSpikes(Timestep current_step) {
        // Use priority queue for delayed events
        // Process spikes in order of delivery time
    }
    
    // Optimization techniques:
    // - Sparse connectivity representation
    // - Event-driven vs time-stepped updates
    // - Delayed spike transmission
};
```

### Memory Efficiency

#### Memory Pool System
```cpp
// Pre-allocated neuron/synapse pools
class MemoryPool {
public:
    template<typename T>
    class Pool {
    public:
        T* allocate() { return pool_array[current_index++]; }
        void reset() { current_index = 0; }
    };
    
    // Benefits:
    // - Reduced memory allocation overhead
    // - Better cache locality
    // - Faster initialization
};
```

### SIMD Vectorization

#### Vectorized Neuron Updates
```cpp
// Using SIMD for parallel neuron updates
void updateNeuronsSIMD(float* v, float* I_syn, float* tau_m, 
                      int neuron_count, float dt) {
    // Process multiple neurons simultaneously
    // AVX/AVX2/AVX-512 instructions for floating-point ops
    
    const int SIMD_WIDTH = 8;  // AVX
    int simd_neuron_count = (neuron_count / SIMD_WIDTH) * SIMD_WIDTH;
    
    for (int i = 0; i < simd_neuron_count; i += SIMD_WIDTH) {
        // Vectorized computation:
        // v[i..i+7] = v[i..i+7] + (dt/tau_m[i..i+7]) * (- (v[i..i+7] - v_rest) + I_syn[i..i+7])
    }
    
    // Handle remainder
    for (int i = simd_neuron_count; i < neuron_count; ++i) {
        v[i] = v[i] + (dt/tau_m[i]) * (- (v[i] - v_rest) + I_syn[i]);
    }
}
```

## Memory Systems

### Working Memory

#### Persistent Activity
```cpp
class NeuralWorkingMemory {
public:
    // Storage mechanism
    float* persistent_activity;     // Sustained firing rates
    float* recurrent_weights;       // Recurrent connections
    
    // Competition mechanism
    void updateWorkingMemory(float* neural_activity, int population_size) {
        // Competitive inhibition
        // Lateral inhibition between memories
        // Winner-take-more dynamics
    }
    
    // Consolidation
    void consolidateToEpisodic(float* experience_pattern);
};
```

### Episodic Memory

#### Pattern Storage
```cpp
class NeuralEpisodicMemory {
public:
    // Neural pattern storage
    float* episode_patterns;         // Stored neural patterns
    float* temporal_context;        // Time stamps
    float* spatial_context;         // Location information
    
    // Pattern completion
    void recallEpisode(float* cue_pattern, float* retrieved_episode) {
        // Find closest stored pattern
        // Return partial or complete episode
    }
    
    // Encoding
    void encodeExperience(float* neural_activity, float timestamp);
};
```

### Associative Memory

#### Hebbian Pattern Association
```cpp
class NeuralAssociativeMemory {
public:
    // Pattern associations
    std::vector<float*> patterns;  // Stored patterns
    std::vector<float> similarities;  // Pattern similarity scores
    
    // Retrieval
    float retrieveAssociation(float* query_pattern) {
        // Find most similar stored pattern
        // Return association strength
    }
    
    // Interference management
    void managePatternInterference();
};
```

## Neuromodulation Systems

### Dopamine System

#### Reward Prediction Error
```cpp
class Dopamine {
public:
    // Variables
    float prediction_error;        // δ = r + γ*V(s') - V(s)
    float eligibility_trace;       // e = γ * e_prev + φ
    
    // Update rule
    void update(float reward, float prediction_error, float learning_rate) {
        // TD learning: Δw = α * δ * e
        // Neuromodulation: w *= (1 + β * δ)
    }
    
    // Time constants
    float reward_decay;            // How long reward signals last
    float prediction_decay;       // How long prediction errors last
};
```

### Curiosity and Novelty Detection

#### Intrinsic Motivation
```cpp
class CuriositySystem {
public:
    // Novelty computation
    float computeNovelty(float* current_activity, float* baseline_activity) {
        // Measure deviation from expected activity
        // Normalize by baseline statistics
        return novelty_score;
    }
    
    // Exploration bonus
    float getExplorationBonus(float novelty, float curiosity_strength) {
        // B = β * novelty
        // Encourages exploration of novel states
    }
};
```

## Development Process

### Developmental Stages

#### Timeline
```cpp
enum DevelopmentalStage {
    INITIAL = 0,        // Birth - primitive networks
    CRITICAL_PERIOD,    // Rapid learning, high plasticity
    MATURATION,         // Refinement, skill specialization
    ADULT,              // Stable adult capabilities
    AGING               // Declining plasticity
};

class DevelopmentSystem {
public:
    // Stage progression
    void updateDevelopmentalStage(float time_elapsed) {
        // Stage-dependent plasticity modulation
        // Activity-dependent stage transitions
    }
    
    // Plasticity modulation
    float getPlasticityModulation() {
        // Different rates for each stage
        // Activity-dependent downscaling
    }
};
```

### Synaptogenesis

#### Activity-Dependent Formation
```cpp
class Synaptogenesis {
public:
    // Probabilistic formation
    bool attemptNewSynapse(float pre_activity, float post_activity, 
                          float developmental_factor) {
        // Higher probability with high activity
        // Limited by available resources
        // Modulated by developmental stage
    }
    
    // Competitive formation
    void manageSynapseNumber(int target_count, int current_count);
};
```

### Pruning

#### Activity-Dependent Elimination
```cpp
class PruningSystem {
public:
    // Weight-based pruning
    bool shouldPruneSynapse(float weight, float activity_level) {
        // Weak synapses: |w| < threshold
        // Inactive synapses: activity < minimum
        // Developmental enhancement
    }
    
    // Competition-based pruning
    void competitivePruning(std::vector<Synapse*>& all_synapses);
};
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### 1. Brain Initialization Problems
```bash
# Symptoms:
# - Simulation stuck at step 0
# - All neurons have v = -70mV
# - No spikes generated

# Causes:
# 1. Did not call brain.initialize()
# 2. Invalid configuration
# 3. Memory allocation failure

# Solutions:
1. Call brain.initialize() before brain.step()
2. Validate configuration:
   config.validate()
3. Check system resources:
   top -p <pid>
```

#### 2. Plasticity Not Working
```python
# Debug script
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check plasticity status
print("STDP enabled:", brain.getPlasticityStatus("stdp"))
print("Hebbian enabled:", brain.getPlasticityStatus("hebbian"))
print("Structural enabled:", brain.getPlasticityStatus("structural"))

# Test STDP manually
print("Testing STDP...")
brain.testSTDP(pre_spike=True, post_spike=False)
```

#### 3. Memory System Issues
```python
# Working memory debug
brain = pynlm.createBrain(config)
brain.initialize()

# Check working memory
wm = brain.getWorkingMemory()
print("Working memory capacity:", wm.getCapacity())
print("Current memory load:", wm.getMemoryLoad())

# Test episodic memory
brain.testEpisodicMemory("experience_pattern")
```

### Performance Profiling

```bash
# Use GProf
make clean
make gprof

# Run simulation
./nlm_phase6_demo

# Generate profile
gprof nlm_phase6_demo main.o > profile.txt

# Analyze
less profile.txt  # Look for:
                  #   time spent in key functions
                  #   cache miss rates  
                  #   memory allocation
```

## Future Directions (Phase 7+)

### Biological Fidelity Improvements

#### Multi-Compartment Models
```cpp
class MultiCompartmentNeuron {
public:
    // Dendritic compartments
    struct Dendrite {
        float membrane_potential;
        float sodium_conc;
        float calcium_conc;
    };
    
    // Dendritic processing
    void processDendriticInput(float spike_input, int compartment_idx);
    
    // Spike initiation
    bool checkThreshold(float soma_voltage);
};
```

#### Ion Channel Dynamics
```cpp
class IonChannelModel {
public:
    // Hodgkin-Huxley gates
    float m_inf(float v);      // Sodium activation
    float h_inf(float v);      // Sodium inactivation
    float n_inf(float v);      // Potassium activation
    
    // Gating kinetics
    void updateGatingVariables(float v, float dt);
    
    // Current computation
    float computeIonicCurrent(float v);
};
```

### Advanced Features

#### Reinforcement Learning Integration
```cpp
class ReinforcementLearner {
public:
    // Q-learning for action selection
    void updateQValue(float state, float action, float reward, float next_state);
    
    // Policy gradient methods
    void optimizePolicy(float gradient, float advantage);
    
    // Credit assignment
    void distributeReward(float reward, float trace_decay);
};
```

#### Meta-Learning
```cpp
class MetaLearner {
public:
    // Learn how to learn
    void updateLearningRate(float performance_metric);
    
    // Transfer learning
    void extractKnowledge(float source_task, float target_task);
    
    // Continual learning
    void preventCatastrophicForgetting(float new_task_data);
};
```

## Key Takeaways

1. **Understanding Neural Dynamics**: LIF model is the foundation, but real neurons have complex ion channel dynamics.

2. **Memory Integration**: Working memory, episodic memory, and associative memory work together for rich cognitive capabilities.

3. **Neuromodulation**: Dopamine, curiosity, and novelty detection drive exploration and learning.

4. **Development**: Critical periods and developmental stages shape brain architecture over time.

5. **Performance**: Event-driven processing and SIMD vectorization are essential for large-scale simulations.

6. **Anti-Transformer Philosophy**: NLM always uses spiking neurons, not ML frameworks or transformer architectures.

This guide provides the foundation for advanced NLM development. Continue exploring the API_REFERENCE.md for detailed class documentation and EXAMPLES.md for practical implementations.