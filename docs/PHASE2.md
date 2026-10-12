# NLM/熙然 Phase 2 Architecture Documentation

## Overview

Phase 2 implements the core neural computation foundation of NLM (熙然). It establishes biologically inspired neural dynamics including leaky integrate-and-fire (LIF) neuron models, synaptic transmission with realistic dynamics, and fundamental plasticity mechanisms. Phase 2 neurons communicate via spikes and enable learning through spike-timing-dependent plasticity (STDP) and Hebbian mechanisms.

This phase represents the transition from skeleton interfaces to working neural systems. Phase 2 neurons implement real spiking dynamics with biological fidelity while maintaining computational efficiency for large-scale simulations.

## Core Neural Computation

### Leaky Integrate-and-Fire (LIF) Dynamics

**Biological Inspiration:**
LIF neurons abstract the essential computational properties of real neurons: membrane integration of inputs, threshold-based spiking, and post-spike refractory periods. This provides a balance between biological realism and computational tractability.

**Mathematical Model:**
```
tau_m dV/dt = (V_rest - V) + R * I

if V >= V_threshold:
    spike event
    V = V_reset
    refractory period (tau_ref)
```

**Implementation Details:**
- **Membrane time constant (τ_m):** 10-30ms (typical cortical neuron)
- **Resting potential (V_rest):** -70mV
- **Threshold potential (V_threshold):** -55mV
- **Reset potential (V_reset):** -65mV
- **Membrane resistance (R):** 100-200MOhm

**Numerical Integration:**
```cpp
float dV = (V_rest - V) + R * I * dt / tau_m;
V += dV;
if (V >= V_threshold && !in_refractory) {
    V = V_reset;
    refractory_timer = tau_ref;
    emit_spike();
}
```

**Memory Management:**
- Each neuron maintains: membrane potential, refractory timer, spike history
- Spike history tracked for STDP computations
- Efficient data layout for large populations

### Synaptic Transmission

**Biological Model:**
Short-term plasticity (STP) captures dynamic synaptic behavior observed in real neurons. Synapses can facilitate (increase strength) or depress (decrease strength) based on recent activity patterns.

**Tsodyks-Markram Model:**
```
dx/dt = (1 - x) / τ_facilitation - u * x * δ(t - t_pre)
du/dt = (U - u) / τ_recovery + U(1 - u) * δ(t - t_pre)

Effective synaptic current: I_syn = w * u * x * (V - E_syn)
```

**Parameters:**
- **U (baseline utilization):** 0.1-0.8 (probability of vesicle release)
- **τ_facilitation:** 100-1000ms (time constant for facilitation)
- **τ_recovery:** 200-500ms (time constant for depression recovery)
- **w (synaptic weight):** Sets baseline efficacy (0-1 scale)

**Synaptic Delays:**
- **Axonal delay:** 0.1-10ms (propagation speed)
- **Synaptic transmission delay:** 0.5-5ms (neurotransmitter dynamics)

**Implementation:**
- Time-bucketed event delivery for delayed spikes
- Linear state updates per time step
- Configurable parameters per synapse type

## Spike Processing

**Event-Driven Architecture:**
NLM uses event-driven processing where neurons only update when they receive input or when time advances. This enables efficient sparse simulation of spiking neural networks.

**Processing Pipeline:**
1. **Input Integration:** Neurons integrate incoming synaptic currents
2. **Threshold Detection:** Membrane potential crosses threshold
3. **Spike Emission:** Spike event recorded and scheduled for delivery
4. **Delayed Propagation:** Spike travels through synapses with configurable delays
5. **Post-synaptic Response:** Target neuron receives delayed spike

**Performance Optimizations:**
- Skip inactive neurons during time advances
- Use event queues for spike scheduling
- Implement time-bucketed updates
- Use vectorized operations for large populations

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticy (STDP)

**Biological Basis:**
STDP encodes temporal relationships between pre- and post-synaptic activity. When presynaptic spikes precede postsynaptic spikes, synapses strengthen (LTP). When postsynaptic spikes precede presynaptic spikes, synapses weaken (LTD).

**Mathematical Model:**
```
if Δt > 0:  // Pre before post
    Δw = A⁺ * exp(-|Δt|/τ⁺)  // Long-term potentiation
else:       // Post before pre
    Δw = -A⁻ * exp(|Δt|/τ⁻)  // Long-term depression
```

**Parameters:**
- **A⁺:** Maximum potentiation amplitude (0.001-0.1)
- **A⁻:** Maximum depression amplitude (-0.001 to -0.1)
- **τ⁺:** Potentiation time constant (20-30ms)
- **τ⁻:** Depression time constant (30-40ms)
- **Δt:** Time difference (post_spike - pre_spike)

**Implementation:**
```cpp
if (post_spike > pre_spike) {
    // LTP
    float delta = A_plus * exp(-(post_spike - pre_spike) / tau_plus);
    weight += delta;
} else {
    // LTD
    float delta = -A_minus * exp((post_spike - pre_spike) / tau_minus);
    weight += delta;
}
weight = clamp(weight, min_weight, max_weight);
```

### Hebbian Learning

**Biological Basis:**
Hebbian plasticity implements the principle "neurons that fire together, wire together." This correlation-based learning strengthens connections between co-active neurons.

**Implementation:**
```cpp
// Hebbian co-activity rule
if (pre_spike && post_spike) {
    delta = learning_rate * post_trace * pre_trace;
    weight += delta;
}
```

**Traces:**
- **Post-synaptic trace:** decays exponentially, accumulates on post-spikes
- **Pre-synaptic trace:** decays exponentially, accumulates on pre-spikes
- **Time constants:** typically 10-50ms

### Reward-Modulated Plasticity

**Biological Basis:**
Neuromodulatory signals like dopamine gate plasticity, making learning contingent on behavior outcomes. Reward prediction error signals modulate synaptic changes.

**Implementation:**
```cpp
// Reward-modulated STDP
if (reward_signal > 0) {
    eligibility *= reward_signal;
    weight += learning_rate * eligibility;
} else {
    eligibility *= decay_factor;
}
```

**Components:**
- **Eligibility trace:** Tracks recent co-activity for reward association
- **Learning rate:** Scaled by neuromodulator concentration
- **Decay factor:** Reduces trace without reward

### Synaptic Scaling

**Homeostatic Mechanism:**
Synaptic scaling maintains network stability by uniformly adjusting all synaptic weights to maintain target firing rates.

**Implementation:**
```cpp
float average_firing_rate = compute_average_rate(neuron_population);
float scaling_factor = target_rate / average_firing_rate;
weight *= scaling_factor;
```

## Structural Plasticity

### Synaptogenesis (New Synapse Formation)

**Biological Basis:**
Activity-dependent synapse formation allows neural networks to grow and adapt their connectivity based on experience and developmental stage.

**Rules:**
```cpp
bool should_form_synapse(Neuron* pre, Neuron* post) {
    // Activity-dependent
    float activity_correlation = compute_correlation(pre, post);
    if (activity_correlation < synaptogenesis_threshold) {
        return false;
    }
    
    // Developmental stage
    float plasticity = get_current_plasticity_rate();
    if (rand() < synaptogenesis_rate * plasticity) {
        return true;
    }
    
    // Topological constraints
    if (synapses_from_pre >= max_outgoing) {
        return false;
    }
    
    if (synapses_to_post >= max_incoming) {
        return false;
    }
    
    return true;
}
```

**Parameters:**
- **Base rate:** 0.001-0.01 per possible connection
- **Activity threshold:** Correlation-based filter
- **Max connections:** Prevent over-connectivity
- **Developmental modulation:** Higher in early stages

### Pruning (Synapse Elimination)

**Biological Basis:**
Pruning removes weak or unused synapses to maintain efficient, stable networks and prevent over-connectivity.

**Rules:**
```cpp
bool should_prune(Synapse* synapse) {
    // Weight threshold
    if (synapse->weight < pruning_weight_threshold) {
        return true;
    }
    
    // Age requirement
    if (synapse->age < pruning_min_age) {
        return false;
    }
    
    // Activity level
    float activity = compute_synapse_activity(synapse);
    if (activity < pruning_activity_threshold) {
        return true;
    }
    
    // Developmental stage
    if (development_stage == Adult && synapse->weight < adult_pruning_threshold) {
        return true;
    }
    
    return false;
}
```

**Parameters:**
- **Pruning thresholds:** Weight and activity based
- **Age constraints:** Prevent premature removal
- **Stage-dependent:** Different rates for different developmental stages

## Configuration Options

### Neuron Parameters

**Membrane Properties:**
```cpp
struct NeuronConfig {
    float membrane_time_constant = 20.0f;  // τ_m in ms
    float resting_potential = -70.0f;     // V_rest in mV
    float threshold_potential = -55.0f;   // V_threshold in mV
    float reset_potential = -65.0f;       // V_reset in mV
    float membrane_resistance = 150.0f;   // R in MOhm
    float refractory_period = 2.0f;       // τ_ref in ms
    int max_incoming_synapses = 1000;
    float min_weight = -1.0f;
    float max_weight = 1.0f;
};
```

### Synapse Parameters

**Transmission Properties:**
```cpp
struct SynapseConfig {
    float baseline_weight = 0.5f;         // w
    float utilization = 0.3f;              // U
    float facilitation_time = 500.0f;     // τ_facilitation in ms
    float recovery_time = 300.0f;         // τ_recovery in ms
    float delay = 2.0f;                   // transmission delay in ms
    float reversal_potential = -70.0f;    // E_syn in mV
    bool enable_stdp = true;
    bool enable_heter = true;
    bool enable_short_term_plasticity = true;
};
```

**Plasticity Parameters:**
```cpp
struct PlasticityConfig {
    // STDP parameters
    float stdp_A_plus = 0.03f;            // potentiation amplitude
    float stdp_A_minus = 0.035f;          // depression amplitude
    float stdp_tau_plus = 20.0f;          // potentiation time constant
    float stdp_tau_minus = 30.0f;         // depression time constant
    
    // Hebbian parameters
    float hebbian_learning_rate = 0.001f;
    float hebbian_trace_decay = 20.0f;
    
    // Reward-modulated parameters
    float reward_learning_rate = 0.01f;
    float eligibility_decay = 0.99f;
    
    // Synaptic scaling
    float scaling_target_rate = 10.0f;    // Hz
    float scaling_learning_rate = 0.01f;
};
```

### Structural Plasticity Parameters

```cpp
struct StructuralPlasticityConfig {
    // Synaptogenesis
    float synaptogenesis_rate = 0.01f;     // per ms
    float synaptogenesis_activity_threshold = 0.1f;
    int max_outgoing_synapses = 1000;
    int max_incoming_synapses = 1000;
    
    // Pruning
    float pruning_weight_threshold = 0.05f;
    float pruning_min_age = 100.0f;       // ms
    float pruning_activity_threshold = 0.01f;
    float adult_pruning_threshold = 0.02f;
    
    // Developmental modulation
    float critical_period_plasticity_multiplier = 2.0f;
};
```

### Simulation Parameters

```cpp
struct SimulationConfig {
    float timestep = 0.1f;                // ms
    float simulation_duration = 1000.0f;   // ms
    bool use_event_driven = true;
    bool use_time_buckets = true;
    int time_bucket_size = 10;             // ms
    int max_spike_events = 1000000;
    float max_weight_change = 0.1f;        // per timestep
};
```

## Implementation Details

### Numerical Integration Methods

**Euler Integration:**
```cpp
// Membrane potential update
float dV = (V_rest - V) * dt / tau_m;
for (each synaptic_input) {
    dV += R * synaptic_input * dt / tau_m;
}
V += dV;
```

**Euler-Cromer (semi-implicit):**
```cpp
V += (V_rest - V + R * I) * dt / tau_m;
V = clamp(V, V_reset, V_threshold);
```

**Choice:** Euler method used for simplicity and stability with small timestep.

### Time Step Considerations

**Constraints:**
- **Stability:** dt < tau_m/10 for numerical stability
- **Accuracy:** Smaller dt for detailed dynamics
- **Performance:** Larger dt for speed

**Recommended:** 0.1ms timestep balances stability and performance.

### Memory Layout for Spike History

**Efficient Storage:**
```cpp
struct SpikeHistory {
    float last_spike_time;     // For recent spike timing
    int spike_count;           // For rate calculations
    float activity_trace;      // For plasticity eligibility
    uint32_t spike_events;     // Bitmask for recent spikes
};
```

**Performance Optimizations:**
- Store spike history in arrays aligned to cache lines
- Use circular buffers for sliding window statistics
- Pre-allocate spike event queues

### Performance Optimization Strategies

**For Large Networks:**
1. **Event-driven updates:** Skip inactive neurons
2. **Time bucketing:** Batch updates by time ranges
3. **Sparse connectivity:** Use adjacency lists
4. **Vectorization:** SIMD for parallel neuron updates
5. **Memory pools:** Pre-allocate neuron/synapse objects
6. **Multi-threading:** Parallel processing for independent regions

**Implementation Example:**
```cpp
class NeuralProcessor {
public:
    void processStep(float dt) {
        // 1. Update neurons with incoming spikes
        processIncomingSpikes();
        
        // 2. Update all neurons (vectorized)
        updateNeurons(dt);
        
        // 3. Detect and schedule spikes
        detectAndScheduleSpikes();
        
        // 4. Process delayed spikes
        processDelayedSpikes();
        
        // 5. Apply plasticity
        applyPlasticity();
    }
private:
    // Optimized data structures
    VectorizedNeuronArray neurons_;
    EventQueue spike_queue_;\n    TimeBucketer time_bucketer_;
    MemoryPool<Neuron> neuron_pool_;
    MemoryPool<Synapse> synapse_pool_;
};
```

## Key References

1. **LIF Model:** Gerstner et al. "Neuronal Dynamics. From Single Neurons to Networks and Models of Cognition"
2. **STDP:** Bi & Poo "Synaptic Modifications in Developing Systems"
3. **Short-term Plasticity:** Tsodyks & Markram "The neural code between spikes"
4. **Structural Plasticity:** Boxer & Moyon "Activity-dependent synapse formation and elimination"
5. **Computational Neuroscience:** Dayan & Abbott "Theoretical Neuroscience"