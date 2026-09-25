# NLM Algorithm and Mathematical Models Documentation

## Overview

This document provides comprehensive documentation of the mathematical models and algorithms that form the core of the NLM (Neural Learning Machine) system. These algorithms implement biologically-inspired neural computation with real spiking dynamics, learning mechanisms, and adaptive behavior.

## Core Mathematical Models

### 1. Leaky Integrate-and-Fire (LIF) Neuron Dynamics

#### Mathematical Formulation

The LIF neuron model is described by the differential equation:

```
τ_m * dV/dt = -g_L * (V - E_L) + I_syn + I_ext
```

Where:
- `τ_m`: Membrane time constant (typically 10-30 ms)
- `g_L`: Leak conductance (typically 10-30 nS)
- `E_L`: Resting potential (typically -70 mV)
- `I_syn`: Synaptic current from post-synaptic inputs
- `I_ext`: External input current

#### Discrete Time Implementation

For numerical simulation with time step `dt`:

```
V(t+dt) = V(t) + (dt/τ_m) * (-g_L * (V(t) - E_L) + I_syn + I_ext)
```

#### Spike Detection and Refractoriness

```
if V(t) >= V_th:
    Generate spike at time t
    V(t) = V_reset
    Enter refractory period of duration τ_ref
```

Where:
- `V_th`: Spike threshold (typically -55 mV)
- `V_reset`: Reset potential (typically -70 mV)
- `τ_ref`: Refractory period (typically 1-5 ms)

#### Neuron State Variables

```
struct NeuronState {
    V_m: Membrane potential (mV)
    V_rest: Resting potential (mV)
    V_th: Spike threshold (mV)
    V_reset: Reset potential (mV)
    g_L: Leak conductance (nS)
    g_syn: Synaptic conductance (nS)
    f_rate: Firing rate (Hz)
    f_state: Firing state (Resting, Firing, Refractory)
    ref_remaining: Remaining refractory steps
    ref_period: Total refractory period (steps)
    adaptation: Spike-frequency adaptation variable
    last_spike: Last spike timestamp (-1 if none)
}
```

### 2. Spike-Timing Dependent Plasticity (STDP)

#### Learning Rule

STDP modifies synaptic weights based on the relative timing of pre- and post-synaptic spikes:

```
w(t+1) = w(t) + Δw_STDP

Δw_STDP = {
    A₊ * exp(-(t_post - t_pre)/τ₊)  if t_post > t_pre (LTP)
    -A₋ * exp(-(t_pre - t_post)/τ₋) if t_pre > t_post (LTD)
    0                              otherwise
}
```

Where:
- `A₊`: Maximum potentiation amplitude (typically 0.01-0.1)
- `A₋`: Maximum depression amplitude (typically 0.01-0.1)
- `τ₊`: Potentiation time constant (typically 10-20 ms)
- `τ₋`: Depression time constant (typically 10-20 ms)

#### Implementation Details

```
void applySTDP(float t_pre, float t_post, float& weight) {
    if (t_pre == -1.0f || t_post == -1.0f) return;  // No spike recorded
    
    float dt = fabs(t_post - t_pre);
    
    if (t_post > t_pre) {  // Post fires after pre (LTP)
        weight += A_plus * exp(-dt / tau_plus);
    } else {  // Pre fires after post (LTD)
        weight -= A_minus * exp(-dt / tau_minus);
    }
    
    // Apply weight constraints
    weight = clamp(weight, w_min, w_max);
}
```

#### Synaptic Modification Bounds

```
w_min = 0.0f      // Minimum synaptic weight
w_max = 1.0f      // Maximum synaptic weight
```

### 3. Hebbian Learning

#### Principle

Hebbian learning strengthens synapses that are simultaneously active:

```
Δw_Hebbian = η * (pre_activity * post_activity)
```

Where:
- `η`: Learning rate (typically 0.001-0.01)
- `pre_activity`: Pre-synaptic neuron activity (0 or 1)
- `post_activity`: Post-synaptic neuron activity (0 or 1)

#### Implementation

```
void applyHebbian(float pre_act, float post_act, float& weight, float learning_rate) {
    if (pre_act > 0.5f && post_act > 0.5f) {
        weight += learning_rate * (1.0f - weight);  // Ensure weight stays in [0,1]
    }
}
```

### 4. Neuromodulation Signals

#### Dopamine (DA) Signals

Dopamine encodes reward prediction errors:

```
dopamine = R - V_predicted

Where:
- R: Actual reward
- V_predicted: Predicted reward
```

#### Curiosity Signals

Curiosity drives exploration of novel stimuli:

```
curiosity = novelty_score * exploration_rate

novelty_score = ||x_current - x_expected||^2
```

#### Novelty Detection

Novelty is detected as statistical deviation:

```
novelty = exp(-||x - μ||^2 / (2σ^2))

Where:
- x: Current input
- μ: Expected input (mean)
- σ: Expected variability (standard deviation)
```

### 5. Memory Systems Algorithms

#### Working Memory Updating

```
void updateWorkingMemory(NeuralState& state, float new_input) {
    // Decay old information
    state.activation *= decay_factor;
    
    // Integrate new information
    state.activation += new_input * learning_rate;
    
    // Apply threshold
    if (state.activation > threshold) {
        state.activation = 1.0f;
        state.active_pattern = encodePattern(new_input);
    }
}
```

#### Episodic Memory Consolidation

```
void consolidateEpisodicMemory(Episode& episode, float consolidation_strength) {
    // Strengthen recent experiences
    for (auto& synaptic_change : episode.recentes) {
        strengthenSynapticChange(synaptic_change, consolidation_strength);
    }
    
    // Weaken older experiences
    for (auto& synaptic_change : episode.older) {
        weakenSynapticChange(synaptic_change, consolidation_strength);
    }
    
    // Update temporal associations
    episode.temporal_strength *= temporal_decay;
}
```

#### Associative Memory Retrieval

```
Pattern retrieveAssociativeMemory(Pattern cue, float threshold) {
    Pattern best_match;
    float best_score = -1.0f;
    
    for (auto& stored_pattern : memory_bank) {
        float score = patternSimilarity(cue, stored_pattern);
        
        if (score > best_score && score > threshold) {
            best_score = score;
            best_match = stored_pattern;
        }
    }
    
    return best_match;
}
```

### 6. Prediction System Algorithms

#### Forward Model Prediction

```
float predictSensoryOutcome(MemoryState& state) {
    // Use current neural activity to predict future sensory input
    float prediction = 0.0f;
    
    for (auto& neuron : active_neurons) {
        prediction += neuron.weight * neuron.activation;
    }
    
    return prediction;
}
```

#### Prediction Error Computation

```
float computePredictionError(float predicted, float actual) {
    return actual - predicted;
}
```

#### Error-Based Learning

```
void updatePredictionBasedOnError(float error, float learning_rate) {
    // Update prediction based on error
    prediction_error_ = error;
    learning_signal_ = error * learning_rate;
}
```

### 7. Structural Plasticity Algorithms

#### Synaptogenesis (Connection Formation)

```
void synaptogenesis(NeuralNetwork& network, float rate) {
    for (auto& region : network.regions) {
        for (auto& pre_neuron : region.neurons) {
            // Probability of forming new connection
            if (rand() / (float)RAND_MAX) < rate) {
                NeuronId new_post = selectTargetNeuron(region);
                if (!hasConnection(pre_neuron, new_post)) {
                    Synapse new_synapse(pre_neuron.id, new_post.id);
                    region.addSynapse(new_synapse);
                    initializeSynapticProperties(new_synapse);
                }
            }
        }
    }
}
```

#### Pruning (Connection Elimination)

```
void pruning(NeuralNetwork& network, float rate) {
    for (auto& region : network.regions) {
        for (auto& synapse : region.synapses) {
            if (rand() / (float)RAND_MAX) < rate) {
                float stability = computeSynapticStability(synapse);
                if (stability < stability_threshold) {
                    region.removeSynapse(synapse.id);
                }
            }
        }
    }
}
```

#### Maturation (Parameter Development)

```
void maturation(NeuralNetwork& network, DevelopmentalStage stage) {
    for (auto& neuron : network.allNeurons()) {
        // Adjust neuron parameters based on developmental stage
        adjustNeuronParameters(neuron, stage);
        
        // Update plasticity rule parameters
        adjustPlasticityRules(neuron, stage);
        
        // Modify connectivity patterns
        adjustConnectivity(neuron, stage);
    }
}
```

## Algorithm Complexity Analysis

### Time Complexity

| Algorithm | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| LIF Update | O(1) per neuron | O(1) per neuron | Constant time per neuron |
| STDP Update | O(1) per synapse | O(1) per synapse | Constant time per synapse |
| Hebbian Learning | O(1) per synapse | O(1) per synapse | Constant time per synapse |
| Working Memory Update | O(n) for pattern matching | O(n) | Linear in pattern length |
| Episodic Memory Consolidation | O(m) for episodes | O(m) | Linear in episode count |
| Associative Memory Retrieval | O(n*m) for pattern search | O(n) | Quadratic in patterns and length |
| Synaptogenesis | O(n²) for connection search | O(n²) | Quadratic in neuron count |
| Pruning | O(n²) for stability computation | O(n²) | Quadratic in neuron count |

### Memory Requirements

```
Memory Usage = {
    neurons: N * sizeof(Neuron),
    synapses: N² * sizeof(Synapse),  // Sparse connectivity reduces this
    working_memory: W * sizeof(Pattern),
    episodic_memory: E * sizeof(Episode),
    associative_memory: A * sizeof(PatternAssociation),
    predictions: P * sizeof(Prediction),
    neuromodulators: M * sizeof(Neuromodulator),
    temporary_buffers: T * sizeof(Buffer)
}
```

### Performance Optimizations

#### Event-Driven Processing
- Only update neurons that receive input or are currently active
- Skip neurons in refractory periods
- Batch process spike propagation

#### Sparse Matrix Operations
- Use sparse data structures for synaptic connectivity
- Exploit locality of connections within regions
- Use vectorized operations for pattern matching

#### Parallel Processing
- Distribute neuron updates across multiple cores
- Use concurrent data structures for thread safety
- Implement asynchronous spike propagation

## Numerical Stability Considerations

### Floating-Point Precision

```
// Prevent numerical overflow/underflow
weight = clamp(weight, MIN_WEIGHT, MAX_WEIGHT);
V = clamp(V, MIN_POTENTIAL, MAX_POTENTIAL);
I = clamp(I, MIN_CURRENT, MAX_CURRENT);

// Use appropriate precision for different calculations
float precision_float = 1.0f / 255.0f;  // 8-bit equivalent
float double_precision = 1.0 / 1000000.0;  // High precision
```

### Time Step Stability

```
// Ensure numerical stability with time step
max_dt = min(τ_m * 0.1, τ_ref * 0.5)
dt = min(user_dt, max_dt)

// Adaptive time stepping for plasticity updates
if (|Δw| > threshold) {
    dt = dt * stability_factor;
}
```

## Parameter Calibration and Tuning

### Parameter Sensitivity Analysis

```
// Key parameter sensitivities
tau_m_sensitivity: High (affects temporal dynamics)
g_L_sensitivity: Medium (affects stability)
V_th_sensitivity: Low (robust to small changes)
A_plus_sensitivity: Medium (affects learning rate)
A_minus_sensitivity: Medium (affects unlearning rate)
```

### Recommended Parameter Ranges

```
// LIF Neuron Parameters
τ_m: [10, 30] ms      // Membrane time constant
g_L: [10, 30] nS     // Leak conductance
V_th: [-55, -50] mV  // Spike threshold
V_reset: [-70, -65] mV  // Reset potential
τ_ref: [1, 5] ms     // Refractory period

// STDP Parameters
A_plus: [0.01, 0.1]   // Potentiation amplitude
A_minus: [0.01, 0.1]  // Depression amplitude
τ_plus: [10, 20] ms   // Potentiation time constant
τ_minus: [10, 20] ms  // Depression time constant

// Learning Parameters
η_Hebbian: [0.001, 0.01]  // Hebbian learning rate
η_STDP: [0.01, 0.1]      // STDP learning rate
```

## Convergence and Stability Analysis

### Learning Dynamics

```
// Conditions for stable learning
learning_rate * ||∇E|| < 1  // Gradient descent stability
weight_bounds respected      // Prevent runaway weight growth
energy_function_converges    // Loss function decreases
```

### Network Stability

```
// Eigenvalue analysis for network stability
λ_max < 1  // Spectral radius condition
E/I balance maintained  // Excitation-inhibition balance
spike_rates bounded      // Prevent network explosion
```

## Algorithm Validation

### Test Cases

1. **Baseline Test**: Verify LIF neuron dynamics with known input patterns
2. **STDP Validation**: Test weight changes with controlled spike timing
3. **Learning Capacity**: Measure information storage capacity
4. **Generalization**: Test performance on novel inputs
5. **Robustness**: Test stability under parameter variations

### Metrics

```
// Learning metrics
learning_curve: Weight change over time
training_error: Prediction error during training
test_error: Prediction error on test data
convergence_rate: Speed of learning

// Performance metrics
spike_timing_accuracy: Accuracy of spike timing
pattern_completion: Ability to complete partial patterns
memory_retention: Longevity of stored information
computational_efficiency: Operations per neuron per second
```

## Future Algorithm Extensions

### Potential Enhancements

1. **Enhanced Plasticity**: {
   - spike-dependent plasticity,
   - reward-modulated plasticity,
   - homeostatic plasticity
   }

2. **Advanced Learning**: {
   - meta-learning algorithms,
   - transfer learning,
   - continual learning
   }

3. **Network Mechanisms**: {
   - topological plasticity,
   - neuromodulatory circuits,
   - oscillatory dynamics
   }

4. **Theoretical Extensions**: {
   - probabilistic inference,
   - Bayesian learning,
   - information theory
   }

## References

1. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neuronal Dynamics. Cambridge University Press.
3. Seung, H. S. (2012). How to Think Like a Neuron. Princeton University Press.
4. Song, S., Miller, K. D., & Abbott, L. F. (2000). Competitive Hebbian learning through spike-timing-dependent synaptic plasticity. Neuron, 32(6), 1147-1158.
5. Abbott, L. F., & Nelson, S. B. (2000). Synaptic plasticity: taming the beast. Nature Neuroscience, 3(7), 591-598.

This documentation provides comprehensive coverage of the mathematical models and algorithms that form the foundation of the NLM system. It serves as both a reference for implementation and a guide for future extensions and modifications to the neural computation framework.