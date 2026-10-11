# NLM Algorithm Documentation

This document provides comprehensive documentation of the key algorithms implemented in the NLM (熙然) artificial developmental brain project.

## Overview

NLM implements biologically-inspired neural algorithms that enable learning, memory, and cognitive capabilities through interaction with an environment. The algorithms are organized into several functional systems:

## 1. Neural Dynamics Algorithms

### 1.1 Leaky Integrate-and-Fire (LIF) Neuron Model

**Description:**
The LIF neuron model is a simplified representation of neuronal electrical activity that captures the essential dynamics of action potential generation while remaining computationally efficient.

**Mathematical Formulation:**
```
τ_m * dV/dt = -V + R * I
```

Where:
- V: Membrane potential
- I: Input current
- τ_m: Membrane time constant (typically 10-20ms)
- R: Membrane resistance

**Algorithm Implementation:**
- Inputs: Synaptic currents from incoming spikes
- Process: Membrane potential integration with exponential decay
- Output: Spike generation when V reaches threshold (typically 0mV)
- Reset: Membrane potential reset to resting state (typically -70mV)

**Key Parameters:**
- Threshold: Voltage level that triggers action potential
- Reset voltage: Voltage after spike generation
- Refractory period: Time during which neuron cannot fire again
- Membrane time constant: Controls integration time scale

**Usage Examples:**
```cpp
// Create LIF neuron with specific parameters
LIFNeuron neuron(threshold: 0.0f, reset: -0.07f, tau: 0.020f);

// Process incoming synaptic inputs
neuron.update(synaptic_currents, dt: 0.001);

// Check if neuron fired
if (neuron.didFire()) {
    handle_spike();
}
```

### 1.2 Event-Driven Spike Propagation

**Description:**
Efficient algorithm for transmitting spikes through neural networks using discrete event representation rather than continuous time steps.

**Key Features:**
- Asynchronous spike processing based on precise spike times
- No wasted computation during silent periods
- Preserves exact spike timing information
- Enables efficient handling of sparse neural activity

**Algorithm Implementation:**
```
For each spike event (neuron, timestamp):
    If synapse is active:
        Calculate spike delivery time based on synaptic delay
        Schedule postsynaptic spike at delivery time
        Update postsynaptic neuron's membrane potential
```

**Performance Considerations:**
- Event-driven approach can be 10-100x faster than time-stepped simulation
- Requires efficient data structures for spike scheduling
- Spike delivery timing accuracy affects neural computation fidelity

## 2. Plasticity Rules

### 2.1 Spike-Timing-Dependent Plasticity (STDP)

**Description:**
STDP is a learning rule where synaptic strength depends on the precise timing between pre- and post-synaptic spikes. It implements the biological observation that "neurons that fire together, wire together" - but only if pre fires before post.

**Mathematical Formulation:**
```
If Δt > 0 (pre before post): LTP
    Δw = A⁺ * exp(-Δt / τ⁺)
If Δt < 0 (post before pre): LTD
    Δw = -A⁻ * exp(Δt / τ⁻)
```

Where:
- Δt = t_post - t_pre (timing difference in ms)
- A⁺: Potentiation amplitude (typically 0.01-0.1)
- A⁻: Depression amplitude (typically 0.01-0.1)
- τ⁺, τ⁻: Time constants (typically 10-50ms)

**Algorithm Implementation:**
```
For each pre-synaptic spike time:
    For each post-synaptic spike time:
        Calculate Δt = t_post - t_pre
        If Δt > 0: Apply LTP
            weight_change = A⁺ * exp(-Δt / τ⁺)
        Else: Apply LTD
            weight_change = -A⁻ * exp(Δt / τ⁻)
        Apply weight change with bounds
```

**Biological Inspiration:**
- Reflects NMDA receptor-mediated calcium signaling
- Pre-before-post activates NMDA receptors when postsynaptic spikes arrive
- Post-before-pre causes backpropagating action potentials
- Implements memory for spike timing correlations

**Implementation Details in STDP.hpp:
- Class STDP implements the standard STDP rule
- Reward-modulated STDP (R-STDP) variant incorporates reward signals
- Triplet STDP extends to three spike interactions

### 2.2 Hebbian Learning

**Description:**
Hebbian learning strengthens synapses when pre- and post-synaptic neurons fire together (co-activation). Often formulated as covariance learning.

**Mathematical Formulation:**
```
Δw = η * (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
```

Where:
- ⟨·⟩ denotes average over time
- η: Learning rate
- The rule captures statistical correlations between pre- and post-synaptic activity

**Algorithm Implementation:**
```cpp
// Simplified implementation in Hebbian.cpp
for each pre spike time:
    for each post spike time:
        if |Δt| < correlation_window:
            correlation_count++

delta = learning_rate * correlation_count
apply_weight_change(delta)
```

**Variants:**
- **Standard Hebbian:** Simple co-activation rule
- **Covariance Hebbian:** Subtracts baseline activity to prevent runaway potentiation
- **Normalized Hebbian:** Normalizes weights to maintain stability

**Biological Inspiration:**
- Reflects AMPA receptor trafficking at synapses
- Implemented at Schaffer collateral synapses in hippocampus
- Correlation-based learning in visual cortex

## 3. Neuromodulation Mechanisms

### 3.1 Dopamine (Reward Prediction Error)

**Description:**
Dopamine signals carry information about reward prediction errors - the difference between expected and actual rewards. This is fundamental for reinforcement learning.

**Mathematical Formulation:**
```
δ = r + γ * V(s') - V(s)
```

Where:
- r: Immediate reward received
- V(s): Value of current state
- V(s'): Value of next state
- γ: Discount factor

**Algorithm Implementation:**
```
void Dopamine::signalReward(float reward) {
    error = reward - expected_reward;
    dopamine_level = calculate_dopamine_response(error);
}

void Dopamine::update(TimestepDuration dt) {
    integrate_dopamine_signal(dt);
    decay_dopamine_level(dt);
}
```

**Key Properties:**
- Phasic responses to unexpected rewards (prediction errors)
- Tonic baseline activity reflecting average reward rate
- Modulates synaptic plasticity and neural excitability
- Implements temporal difference learning

### 3.2 Novelty Detection (Curiosity)

**Description:**
Novelty signals detect unexpected environmental changes and promote exploration of unfamiliar situations.

**Algorithm Implementation:**
```
void NoveltyDetector::update(const SensoryInput& input) {
    current_features = extract_features(input);
    novelty = calculate_divergence(current_features, memory_bank);
    curiosity = novelty * exploration_factor;
}
```

**Usage:**
- Drives exploration behavior
- Encourages sampling of novel states
- Balances exploitation vs. exploration
- Prevents premature convergence to local optima

### 3.3 Prediction Error (Surprise)

**Description:**
Prediction error signals the discrepancy between expected and actual sensory input, driving learning about environmental regularities.

**Algorithm:**
- Compares predicted sensory input with actual input
- Large errors indicate important new information
- Small errors indicate predictable, stable environments
- Guides updating of internal models

## 4. Memory Systems

### 4.1 Working Memory (Persistent Activity)

**Description:**
Working memory maintains neural representations of information over short time periods through persistent activity patterns.

**Algorithm Implementation:**
```
void WorkingMemory::store(pattern_vector, duration) {
    activate_neural_population(pattern_vector);
    schedule_deactivation(duration);
}
```

**Key Features:**
- Recurrent neural connections maintain activity
- Competitive dynamics select and maintain dominant patterns
- Gradual decay preserves information over time
- Integration with attention mechanisms

### 4.2 Episodic Memory (Neural Trace)

**Description:**
Episodic memory stores complete experiences as integrated neural patterns for later retrieval.

**Algorithm:**
- Experiences encoded as distributed neural patterns
- Pattern completion enables partial cue-based retrieval
- Replay during consolidation strengthens memory traces
- Temporal and spatial indexing for efficient access

## 5. Prediction Systems

### 5.1 Temporal Sequence Prediction

**Description:**
Predicts future events based on learned temporal sequences and patterns.

**Algorithm:**
```
prediction = neural_network(sequence_input);
error = actual_output - prediction;
update_prediction_weights(error);
```

**Components:**
- Sequence learning from temporal spike patterns
- Prediction error signals for learning
- Confidence estimation for uncertainty
- Multi-step lookahead prediction

### 5.2 Action Consequence Prediction

**Description:**
Predicts the outcomes of potential actions based on learned action-effect associations.

**Algorithm:**
```
void ActionConsequencePredictor::update(Action action, Outcome outcome) {
    prediction = historical_lookup(action, context);
    prediction_error = outcome - prediction;
    update_association(action, outcome, prediction_error);
}
```

**Usage:**
- Enables planning and decision making
- Supports causal inference from observation
- Drives curiosity-driven exploration

## 6. Cognitive Mechanisms

### 6.1 Competitive Attention Selection

**Description:**
Selects relevant information from multiple competing inputs through lateral inhibition and winner-take-all dynamics.

**Algorithm:**
```
float max_activation = -infinity;
int winner = -1;

for each input neuron:
    activation = compute_activation(input);
    inhibited_activation = activation - lateral_inhibition_sum;
    
    if (inhibited_activation > max_activation):
        max_activation = inhibited_activation;
        winner = neuron_index;
```

**Properties:**
- Lateral inhibition creates winner-take-all competition
- Dynamic modulation by top-down signals
- Competitive selection of relevant features
- Prevents resource competition and interference

### 6.2 Neural Planning

**Description:**
Evaluates potential action sequences using learned predictions and rewards.

**Algorithm:**
```
float plan_score = evaluate_action_sequence(sequence) {
    float total_reward = 0;
    for each action in sequence:
        prediction = predict_outcome(action);
        total_reward += prediction.reward;
        total_reward += prediction.confidence * exploration_bonus;
    return total_reward;
}
```

**Implementation:**
- Uses prediction system for outcome evaluation
- Incorporates uncertainty and risk assessment
- Balances immediate and delayed rewards
- Considers alternative action sequences

## 7. Developmental Algorithms

### 7.1 Critical Period Plasticity

**Description:**
Developmentally regulated period of heightened plasticity during which learning is most effective.

**Algorithm:**
```
void CriticalPeriod::update(DevelopmentalStage stage) {
    if (stage == CRITICAL_PERIOD) {
        plasticity_modifier = 1.5f;  // Increased learning rate
        synaptic_reorganization = true;
    } else {
        plasticity_modifier = calculate_age_dependent_modifier(stage);
        synaptic_reorganization = false;
    }
}
```

**Characteristics:**
- Time-limited window for sensory learning
- High sensitivity to environmental input
- Age-dependent plasticity rates
- Essential for normal development

## Performance Considerations

### 7.1 Computational Efficiency

**Event-Driven Computation:**
- Spike-based simulation avoids continuous time stepping
- Reduces computational overhead during silent periods
- Enables real-time performance for large networks

**Memory Management:**
- Pool-based memory allocation for frequently created objects
- Sparse matrix operations for connection weights
- Cache-friendly data structures for neural populations

### 7.2 Scalability

**Parallel Processing:**
- Thread-local spike queues for parallel simulation
- Distributed memory management for large networks
- Load balancing across multiple CPU cores

**Approximations:**
- Dimensionality reduction for high-dimensional inputs
- Approximate nearest neighbor for pattern matching
- Stochastic sampling for uncertainty quantification

## Algorithm Integration and Workflow

### 7.1 Main Simulation Loop

```cpp
void NLM::run() {
    // Sensory processing
    SensoryPercept percept = world.sense();
    agent.processSensoryInput(percept);
    
    // Neural computation
    brain.update(dt);
    
    // Action selection
    MotorCommand action = agent.decodeMotorCommand();
    world.applyAction(action);
    
    // Learning
    float reward = world.getReward();
    agent.applyRewardModulation(reward, expected_reward);
    brain.updateDevelopment(dt);
    
    // Memory consolidation
    memory.consolidate();
    
    // Developmental progression
    development.advanceStage();
}
```

### 7.2 Learning System Integration

The NLM implements a multi-layered learning system where:

1. **Low-level Learning** (STDP, Hebbian): Shape synaptic connections
2. **Mid-level Learning** (Prediction, Memory): Form internal models
3. **High-level Learning** (Planning, Curiosity): Guide behavior selection
4. **Developmental Regulation**: Control learning rates over time

## Experimental Considerations

### 7.1 Reproducible Experiments

**Deterministic Simulation:**
- Fixed random seeds for experimental reproducibility
- Deterministic spike timing algorithms
- Reproducible developmental trajectories

**Controlled Variables:**
- Network architecture parameters
- Learning rate settings
- Environmental conditions
- Measurement protocols

### 7.2 Analysis and Validation

**Performance Metrics:**
- Learning speed and accuracy
- Memory retention and recall
- Behavioral adaptation capabilities
- Computational efficiency

**Validation Methods:**
- Comparison with biological observations
- Statistical significance testing
- Cross-validation of learned representations
- Ablation studies of algorithm components

## Future Algorithm Extensions

### 7.3 Advanced Learning Mechanisms

**Planned Enhancements:**
- Spike-timing-dependent plasticity with triplet interactions
- Dopamine-modulated STDP for reinforcement learning
- Complex value functions for decision making
- Social learning from observing other agents

### 7.4 Neurobiological Realism

**Future Directions:**
- Incorporate detailed ion channel dynamics
- Model dendritic computation and compartmentalization
- Include glial cell interactions
- Simulate neurovascular coupling

## References

This documentation draws from the following sources:
- Biology: Liberman et al., "Neuronal turnover in the hippocampus and cortex", Science 2012
- Neuroscience: Song et al., "Competitive learning in spiking networks", Nature Neuroscience 2000
- Machine Learning: Bulsara et al., "Stochastic resonance and neural networks", Physica D 1990
- Computational Modeling: Gerstner et al., "Neuronal Dynamics", Cambridge University Press 2014

## Algorithm Summary Table

| Algorithm | Purpose | Time Scale | Key Parameters | Biological Basis |
|-----------|---------|------------|----------------|------------------|
| STDP | Synaptic timing learning | Milliseconds | A⁺, A⁻, τ | NMDA receptors |
| Hebbian | Co-activation learning | Seconds | η, thresholds | AMPA receptors |
| Working Memory | Short-term storage | Seconds | decay rate | Persistent activity |
| Prediction | Future expectation | Variable | confidence | Bayesian inference |
| Attention | Feature selection | Milliseconds | competition | Lateral inhibition |
| Novelty | Change detection | Variable | sensitivity | Predictable change |

