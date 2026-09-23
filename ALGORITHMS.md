# NLM Neural Simulation Framework - Algorithms Guide

## Overview

This document provides comprehensive documentation of all algorithms and mathematical models used in the NLM (Neural Learning Machine) neural simulation framework. The algorithms range from fundamental neuron dynamics to advanced learning and cognitive processes, all implemented with biological plausibility in mind.

## Mathematical Foundations

### 1. Leaky Integrate-and-Fire (LIF) Neuron Model

**Core Equation:**
```
tau_m * dV/dt = -g_L * (V - E_L) + I_syn(t) + I_ext(t)
```

**Where:**
- `tau_m` = Membrane time constant (τ_m = C_m / g_L)
- `V` = Membrane potential
- `g_L` = Leak conductance
- `E_L` = Leak reversal potential (resting potential)
- `I_syn(t)` = Total synaptic current
- `I_ext(t)` = External injected current

**Discrete Implementation:**
```
V(t+dt) = V(t) * exp(-dt/tau_m) + (I_total * R_m * (1 - exp(-dt/tau_m)))
```

**Spike Detection:**
```
if V(t) >= V_th then:
    Fire spike at time t
    V(t) = V_reset
    start refractory period
```

**Refractory Period Model:**
```
refractory_remaining = max(0, refractory_remaining - 1)
if refractory_remaining == 0 then:
    can_fire = true
else:
    can_fire = false
```

### 2. Synaptic Transmission

**Excitatory Synapse:**
```
I_exc = g_max_exc * efficacy * (E_exc - V_post)
```

**Inhibitory Synapse:**
```
I_inh = g_max_inh * efficacy * (E_inh - V_post)
```

**Total Synaptic Current:**
```
I_syn = I_exc + I_inh + I_mod
```

**Short-term Plasticity (STP) Models:**

**Facilitation-Depression Model:**
```
x(t+dt) = x(t) + dt/tau_f * (1 - x(t)) * u
σ(t+dt) = σ(t) + dt/tau_d * (u * x(t) - σ(t))
efficacy(t+dt) = σ(t) * x(t)
```

**Where:**
- `x` = facilitation variable
- `σ` = depression variable
- `u` = utilization parameter
- `tau_f` = facilitation time constant
- `tau_d` = depression time constant

### 3. Spike-Timing-Dependent Plasticity (STDP)

**Pair-based STDP:**
```
if t_post > t_pre:
    Δw = A_LTP * exp(-(t_post - t_pre)/τ_plus)
else:
    Δw = -A_LTD * exp(-(t_pre - t_post)/τ_minus)
```

**Triplet STDP:**
```
Δw = A_plus3 * F(t_pre, t_post, τ3_plus) - A_minus3 * G(t_pre, t_post, τ3_minus)
```

**Hebbian Learning:**
```
Δw = learning_rate * pre_spike * post_spike
```

### 4. Neuromodulation Systems

**Reward Prediction Error (RPE):**
```
delta = reward - predicted_reward
```

**Dopamine Dynamics:**
```
dA/dt = -α * A + β * RPE
A(t) = A(t-1) + ΔA * dt
```

**Curiosity Signal:**
```
C = novelty * (1 - prediction_confidence)
```

**Novelty Detection:**
```
N = 1 - similarity(input, stored_patterns)
```

### 5. Working Memory Mechanisms

**Persistent Activity Model:**
```
dV/dt = -g_L * (V - E_L) + I_rec + I_input
I_rec = g_rec * (V_ref - V)
```

**Attentional Competition:**
```
I_i = w_ii * V_i - w_ij * sum(V_j for j != i)
```

**Memory Trace Decay:**
```
x(t) = x(0) * exp(-t/tau_decay)
```

### 6. Prediction Systems

**Temporal Difference Learning:**
```
δ = reward + γ * V(s') - V(s)
V(s) ← V(s) + α * δ
```

**Forward Models:**
```
V̂(a, s) = f(s, a; θ)
```

### 7. Concept Formation

**Unsupervised Clustering:**
```
center_k ← (1/n) * Σ x_i
```

**Stability-Plasticity Trade-off:**
```
learning_rate = base_rate * (1 - stability)
```

### 8. Development Systems

**Stage Progression:**
```
if developmental_age >= stage_duration:
    advance_to_next_stage()
```

**Plasticity Modulation:**
```
plasticity_modifier = base_plasticity * stage_factor
```

### 9. Social Learning

**Imitation Learning:**
```
if observation_confidence > threshold:
    adopt observed_action
```

**Communication Signals:**
```
signal_strength = f(action, outcome)
```

## Algorithm Implementation Details

### LIF Neuron Step Algorithm
```cpp
void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // 1. Check refractory status
    if (isRefractory()) {
        decrementRefractory();
        return;
    }
    
    // 2. Calculate total input
    MembranePotential I_total = getTotalCurrent();
    
    // 3. Apply LIF dynamics
    float alpha = exp(-dt / (leakConductance / restingPotential));
    MembranePotential newV = membranePotential * alpha;
    
    // 4. Add synaptic input
    if (I_total != 0.0f) {
        newV += I_total * restingPotential * (1.0f - alpha);
    }
    
    // 5. Update membrane potential
    membranePotential = newV;
    
    // 6. Check for spike
    if (membranePotential >= threshold && !isFiring()) {
        // Handle spike
        lastSpikeTime = currentTime;
        setFiringState(FiringState::Firing);
        // Apply reset
        membranePotential = resetPotential;
    }
    
    // 7. Update firing rate (rate-based component)
    firingRate = (firingRate * (1.0f - dt / tau_rate)) + 
                 (firingState == FiringState::Firing ? 1.0f / dt : 0.0f);
}
```

### Synaptic Transmission Algorithm
```cpp
void Synapse::step(Timestamp currentTime) {
    // 1. Get presynaptic spike history
    const auto& preSpikes = getPreSpikeHistory();
    const auto& postSpikes = getPostSpikeHistory();
    
    // 2. Calculate synaptic efficacy
    float currentEfficacy = calculateEfficacy(preSpikes, postSpikes, currentTime);
    setEfficacy(currentEfficacy);
    
    // 3. Calculate synaptic current
    float efficacy = getEfficacy();
    float I_syn = 0.0f;
    
    if (isExcitatory()) {
        I_syn = getWeight() * efficacy * (EXCITATORY_REVERSAL - getTargetNeuron()->getMembranePotential());
    } else if (isInhibitory()) {
        I_syn = getWeight() * efficacy * (INHIBITORY_REVERSAL - getTargetNeuron()->getMembranePotential());
    }
    
    // 4. Apply to postsynaptic neuron
    if (I_syn != 0.0f) {
        getTargetNeuron()->addToMembranePotential(I_syn);
    }
    
    // 5. Record spike for plasticity
    for (auto spikeTime : preSpikes) {
        recordPreSpike(spikeTime);
    }
    
    for (auto spikeTime : postSpikes) {
        recordPostSpike(spikeTime);
    }
}
```

### STDP Plasticity Algorithm
```cpp
void STDP::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    // Reset weight changes for this timestep
    float weightChange = 0.0f;
    
    // Process all presynaptic spikes
    for (auto preSpikeTime : preSpikes) {
        for (auto postSpikeTime : postSpikes) {
            if (postSpikeTime > preSpikeTime) {
                // Long-term potentiation (LTP)
                float delta = ltpWeight * exp(-(postSpikeTime - preSpikeTime) / tauPlus);
                weightChange += delta;
            } else if (preSpikeTime > postSpikeTime) {
                // Long-term depression (LTD)
                float delta = -ltdWeight * exp(-(preSpikeTime - postSpikeTime) / tauMinus);
                weightChange += delta;
            }
        }
    }
    
    // Apply weight change
    if (weightChange != 0.0f) {
        synapse->addToWeight(weightChange);
        synapse->recordPreSpike(preSpikes.back());
        synapse->recordPostSpike(postSpikes.back());
    }
}
```

### Working Memory Update Algorithm
```cpp
void NeuralWorkingMemory::update(TimestepDuration dt) {
    // 1. Decay existing traces
    decayWeakTraces();
    
    // 2. Update recurrent connections
    updateRecurrentConnections();
    
    // 3. Apply external input
    if (externalInput_.size() > 0) {
        std::vector<float> newPattern(externalInput_.size(), 0.0f);
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            // Project external input to memory neurons
            newPattern[i] += externalInput_[i] * inputWeight_;
        }
        
        // Integrate with existing pattern
        for (size_t i = 0; i < memoryPattern_.size(); ++i) {
            memoryPattern_[i] = memoryPattern_[i] * (1.0f - decayRate_ * dt) + newPattern[i];
        }
    }
    
    // 4. Run competition
    runCompetition();
    
    // 5. Update winning neurons
    updateWinningNeurons();
}
```

### Prediction System Algorithm
```cpp
void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                         const SensoryInput& actual) {
    // 1. Calculate prediction error
    float error = calculatePredictionError(predicted, actual);
    errorHistory_.push_back(error);
    
    // 2. Update confidence
    confidence_ = calculateConfidence(errorHistory_);
    
    // 3. Update prediction model
    if (confidence_ > predictionThreshold_) {
        updatePredictionModel(predicted, actual);
    }
    
    // 4. Record prediction
    predictions_.push_back(predicted);
}
```

## Algorithm Performance Characteristics

### Computational Complexity

| Algorithm | Time Complexity | Space Complexity | Notes |
|-----------|-----------------|------------------|-------|
| LIF Neuron | O(1) per neuron | O(1) per neuron | Constant time per timestep |
| Synaptic Transmission | O(k) per synapse | O(1) per synapse | k = number of active spikes |
| STDP | O(n*m) per timestep | O(n+m) | n = pre-spikes, m = post-spikes |
| Working Memory | O(N^2) per timestep | O(N) | N = memory capacity |
| Attention Competition | O(N^2) per timestep | O(N) | N = competing neurons |
| Prediction | O(T) per timestep | O(T) | T = prediction horizon |

### Memory Requirements

- **Neuron State**: ~50 bytes per neuron
- **Synapse State**: ~30 bytes per synapse
- **Working Memory**: ~100 bytes per memory item
- **Prediction History**: ~20 bytes per prediction step

### Numerical Stability Considerations

1. **Weight Clipping**: Prevent unbounded weight growth
   ```cpp
   float maxWeight = getConfig()->getMaxSynapseWeight();
   float currentWeight = synapse->getWeight();
   if (std::abs(currentWeight) > maxWeight) {
       synapse->setWeight(maxWeight * sign(currentWeight));
   }
   ```

2. **Voltage Saturation**: Prevent unrealistic membrane potentials
   ```cpp
   float minVoltage = -100.0f;
   float maxVoltage = 50.0f;
   if (membranePotential < minVoltage) {
       membranePotential = minVoltage;
   } else if (membranePotential > maxVoltage) {
       membranePotential = maxVoltage;
   }
   ```

3. **Time Step Constraints**: Ensure numerical stability
   ```cpp
dt_max = 0.1 * min(tau_m, tau_f, tau_d)
```

## Algorithm Variants and Extensions

### Alternative Neuron Models

#### 1. Adaptive Exponential Integrate-and-Fire (AdEIF)
```
C * dV/dt = -g_L * (V - E_L) + I_syn + Δ_T * exp((V - V_T)/Δ_V)
```

#### 2. Izhikevich Model
```
dV/dt = 0.04*V^2 + 5*V + 140 - u + I
du/dt = a*(b*V - u)
```

#### 3. Hindmarsh-Rose Model
```
dV/dt = y - a*V^3 + b*V^2 - z + I
dy/dt = c - d*V^2 - y
dz/dt = r*(s*(V - E_R) - z)
```

### Advanced Plasticity Rules

#### 1. Reward-Modulated STDP
```
Δw = STDP_weight * (A_LTP * exp(-Δt/τ_+) - A_LTD * exp(-Δt/τ_-)) * RPE
```

#### 2. Anti-Hebbian Plasticity
```
Δw = -learning_rate * pre_spike * post_spike
```

#### 3. Homeostatic Plasticity
```
Δw = -η * (current_rate - target_rate) * w
```

### Learning Algorithm Extensions

#### 1. Meta-Learning
```
learning_rate ← learning_rate * meta_factor
base_rate ← base_rate + meta_correction
```

#### 2. Transfer Learning
```
if source_task_similarity > threshold:
    transfer_weights(source_task)
```

#### 3. Curriculum Learning
```
if current_difficulty < target_difficulty:
    increase_difficulty()
```

## Algorithm Validation and Testing

### Testing Methodology

1. **Unit Testing**: Test individual algorithms in isolation
2. **Integration Testing**: Test algorithm interactions
3. **Performance Testing**: Benchmark computational requirements
4. **Numerical Testing**: Verify convergence and stability
5. **Biological Plausibility**: Compare with experimental data

### Test Cases

#### LIF Neuron Tests
- **Threshold Crossing**: Verify spike generation at threshold
- **Refractory Period**: Verify recovery timing
- **Input Integration**: Verify linear summation
- **Leak Dynamics**: Verify exponential decay

#### Synaptic Transmission Tests
- **Excitatory Response**: Verify depolarization
- **Inhibitory Response**: Verify hyperpolarization
- **Delay Accuracy**: Verify transmission timing
- **Weight Scaling**: Verify amplitude scaling

#### STDP Tests
- **LTP Verification**: Verify weight increase for post-before-pre
- **LTD Verification**: Verify weight decrease for pre-before-post
- **Timing Sensitivity**: Verify temporal specificity
- **Homeostatic Stability**: Verify weight boundedness

### Validation Metrics

- **Accuracy**: Comparison with analytical solutions
- **Stability**: Absence of runaway dynamics
- **Convergence**: Proper learning behavior
- **Biological Plausibility**: Reasonable parameter ranges
- **Performance**: Computational efficiency

## Algorithm Documentation Standards

### Code Comments
- Document mathematical equations with LaTeX notation
- Explain parameter ranges and biological plausibility
- Document algorithmic complexity and performance characteristics
- Provide references to related biological studies

### Mathematical Notation
- Use consistent LaTeX formatting
- Define all variables and parameters
- Include units for all physical quantities
- Provide example calculations

### Implementation Notes
- Document numerical methods and approximations
- Explain design choices and trade-offs
- Provide guidance for parameter tuning
- Document known limitations and future improvements

## References

1. Dayan, P., & Abbott, L. F. (2001). *Theoretical Neuroscience*. MIT Press.
2. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). *Neuronal Dynamics*. Cambridge University Press.
3. Song, S., Miller, K. D., & Abbott, L. F. (2000). *Hebbian learning via spike-timing-dependent synaptic plasticity*. Neuron, 32(1), 441-448.
4. Pfister, J.-P., & Gerstner, W. (2006). *Triplet STDP*. Neural Computation, 18(7), 1742-1778.
5. Abbott, L. F., & Kepecs, A. (2009). *Neural coding*. Annual review of neuroscience, 32, 213-243.

## Conclusion

The NLM framework implements a comprehensive suite of neural algorithms with biological plausibility and computational efficiency. The algorithms are designed to balance realism with performance, enabling large-scale simulations while maintaining neuroscientific validity. Continued development focuses on extending the algorithmic repertoire and improving numerical methods.

The documented algorithms provide a foundation for implementing advanced neural processing capabilities including learning, memory, attention, prediction, and cognition. Future extensions will include more complex neuron models, additional plasticity rules, and sophisticated learning algorithms.
