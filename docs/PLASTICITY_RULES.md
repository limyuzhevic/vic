# Neural Plasticity Rules Documentation

## Plasticity Overview

Plasticity is the fundamental mechanism by which neural connections are modified based on activity patterns. In NLM, plasticity enables learning, adaptation, and memory formation through activity-dependent synaptic modification.

### Core Plasticity Systems

1. **Spike-Timing-Dependent Plasticity (STDP)**
2. **Hebbian Learning**
3. **Reward-Modulated Plasticity**
4. **Structural Plasticity**

## Spike-Timing-Dependent Plasticity (STDP)

### Biological Basis
STDP reflects the timing-dependent properties of synaptic strength modification observed in biological synapses. When a presynaptic neuron fires before a postsynaptic neuron, the connection is strengthened (Long-Term Potentiation, LTP). When the presynaptic neuron fires after the postsynaptic neuron, the connection is weakened (Long-Term Depression, LTD).

### Mathematical Formulation
For each pre-post spike pair with timing difference Δt = t_post - t_pre:

**If Δt > 0 (pre before post): POTENTIATION**
```
Δw = A+ × exp(-Δt / τ+)
```

**If Δt < 0 (post before pre): DEPRESSION**
```
Δw = -A- × exp(Δt / τ-)
```

**Where:**
- **A+** = ltpWeight (potentiation amplitude)
- **A-** = ltdWeight (depression amplitude)
- **τ+** = τ- = timeConstant (STDP time window)
- **Δt** = timing difference (post - pre) in milliseconds

### Implementation Details (STDP.cpp:23-94)

#### Update Function (lines 23-94)
```cpp
void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    // Calculate total weight change from all spike pairs
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Iterate over all pre-post spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime); // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds and scaling
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}
```

#### Biological Interpretations

1. **NMDA Receptor Dynamics**: Pre-before-post spikes activate NMDA receptors when postsynaptic depolarization occurs
2. **Backpropagating Action Potentials**: Post-before-pre spikes cause calcium influx through backpropagating action potentials
3. **Calcium Signaling**: Different calcium dynamics trigger different plasticity pathways
4. **Local Dominance**: Synaptic changes are localized to active synaptic zones

#### Parameters

- **ltpWeight**: LTP magnitude (typical: 0.01-0.1)
- **ltdWeight**: LTD magnitude (typical: 0.01-0.02)
- **timeConstant**: STDP time window (typical: 10-50 ms)
- **minWeight/maxWeight**: Weight bounds (-1.0 to 1.0 typical)

### Applications

1. **Memory Formation**: Encoding temporal correlations
2. **Feature Learning**: Detecting coincident patterns
3. **Timing Information**: Preserving temporal relationships
4. **Predictive Coding**: Forward models for expected inputs

## Hebbian Learning

### Theoretical Basis
Hebbian learning implements the principle that "neurons that fire together, wire together." This is typically implemented as correlation-based learning where synaptic strength is proportional to the product of pre- and postsynaptic activities.

### Implementation Details

#### Hebbian.cpp (placeholder for implementation)
```cpp
class Hebbian : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Correlation-based learning
        // Δw = η × pre × post
        // where η is learning rate, pre and post are firing rates
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Apply with bounds checking
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
    }
};
```

### Variants

1. **Standard Hebbian**: Simple correlation learning
2. **Normalized Hebbian**: Divide by pre/post activities
3. **Oja's Rule**: Normalized Hebbian with stability
4. **BCM Theory**: Sliding threshold Hebbian learning

## Reward-Modulated Plasticity

### Biological Basis
Reward signals modulate synaptic plasticity, strengthening connections that contribute to rewarded behaviors. This mechanism underlies reinforcement learning and goal-directed behavior.

### Neuromodulation Integration

#### Dopamine System (Neuromodulator.cpp:32-35)
```cpp
float Dopamine::getPlasticityFactor() const {
    // Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}
```

#### Plasticity Modulation in Brain.cpp (lines 449-467)
```cpp
// Calculate neuromodulation factor for plasticity
float plasticityMod = 1.0f;
if (pImpl->dopamine) {
    plasticityMod = pImpl->dopamine->getPlasticityFactor();
}

// Apply dopamine-scaled STDP learning rates
pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
float weight = syn->getWeight();
weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
syn->setWeight(weight);
```

### Reward Prediction Error (RPE)

#### RPE-based STDP
- **Positive RPE**: Strengthen reward-predictive synapses
- **Negative RPE**: Weaken non-reward-predictive synapses
- **Temporal difference**: Conveys prediction error information

#### Biological Correlates
- **VTA dopamine neurons**: Encode reward prediction errors
- **Spiking patterns**: Phasic bursts for unexpected rewards
- **Temporal dynamics**: Rapid signaling of prediction errors

## Structural Plasticity

### Overview
Structural plasticity modifies the physical connectivity of neural networks through synaptogenesis and pruning, changing the network's topology over time.

### Synaptogenesis (DevelopmentSystem.cpp:140-143)

#### Mechanisms
1. **Growth factors**: Chemical signals promoting synapse formation
2. **Electrical activity**: Activity-dependent synapse creation
3. **Neural activity patterns**: Coincidence detection

#### Implementation Details
```cpp
void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    // Spontaneous synaptogenesis based on developmental stage
    float rate = pImpl->synaptogenesisRate * developmentalStageFactor;
    for (size_t i = 0; i < rate * totalConnections; ++i) {
        createNewSynapse(brain, rng);
    }
    
    // Activity-dependent synaptogenesis
    float activityRate = pImpl->activityDependentRate * avgFiringRate;
    for (size_t i = 0; i < activityRate * totalConnections; ++i) {
        if (highCoincidenceActivity(brain)) {
            createNewSynapse(brain, rng);
        }
    }
}
```

### Pruning (DevelopmentSystem.cpp:140-143)

#### Mechanisms
1. **Activity-dependent pruning**: Eliminate weak or unused synapses
2. **Maturation-dependent pruning**: Remove excessive initial connections
3. **Competition**: Synaptic competition for resources

#### Implementation Details
```cpp
void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    // Spontaneous pruning based on developmental stage
    float rate = pImpl->pruningRate * (2.0f - developmentalStageFactor);
    for (size_t i = 0; i < rate * totalConnections; ++i) {
        pruneWeakSynapse(brain, rng);
    }
    
    // Activity-dependent pruning
    float activityRate = pImpl->activityPruningRate * (1.0f - avgFiringRate);
    for (size_t i = 0; i < activityRate * totalConnections; ++i) {
        if (lowCoincidenceActivity(brain)) {
            pruneWeakSynapse(brain, rng);
        }
    }
}
```

### Plasticity Rules Summary

| Plasticity Type | Mechanism | Key Parameters | Biological Correlate |
|----------------|-----------|----------------|---------------------|
| STDP | Timing-dependent | A+, A-, τ | NMDA receptor dynamics |
| Hebbian | Correlation-based | η (learning rate) | Co-activation |
| RPE | Reward-modulated | dopamine level | VTA dopamine neurons |
| Structural | Activity-dependent | synaptogenesis_rate | Growth factors |

## Performance Considerations

### Computational Complexity

#### STDP Complexity
- **Naïve implementation**: O(n²) spike pair computation
- **Optimization**: Limit spike history, use temporal windows
- **Vectorization**: Batch processing of spike pairs
- **Caching**: Pre-compute exponential terms

#### Memory Requirements
- **Synaptic weights**: O(N) for sparse connectivity
- **Eligibility traces**: O(N) for reward-modulated learning
- **Spike histories**: O(k) per synapse (k = recent spikes)

### Scalability

#### Problem Size
- **Small networks**: 1K-10K neurons (STDP dominant)
- **Medium networks**: 10K-100K neurons (requires optimizations)
- **Large networks**: 100K-1M+ neurons (requires parallel processing)

#### Optimization Strategies
1. **Sparse connectivity**: Use adjacency lists
2. **Time-bucketing**: Process spikes in temporal windows
3. **Approximate STDP**: Sample spike pairs instead of all pairs
4. **Parallel processing**: Distribute computation across cores
5. **SIMD vectorization**: Vectorized operations for common cases

## Future Extensions

### Advanced Plasticity

1. **Neuromodulator-specific STDP**: Different plasticity rules for different neuromodulators
2. **Metaplasticity**: Plasticity that changes plasticity rules
3. **Homeostatic plasticity**: Mechanisms to maintain stability
4. **Proteomic plasticity**: Protein-level modifications

### Computational Models

1. **Biophysical models**: Detailed ionic channel dynamics
2. **Neural field models**: Population-level plasticity
3. **Bayesian plasticity**: Probabilistic inference in learning
4. **Evolutionary plasticity**: Selection of connectivity patterns

## References

1. **Biologically-based models**: Gerstner et al. "Neuronal Dynamics"
2. **STDP literature**: Song et al. "Synaptic plasticity: From basic principles to clinical applications"
3. **Reward modulation**: Montague et al. "Learning representations in dopamine-based reinforcement learning"
4. **Structural plasticity**: Kozorovitskiy et al. "Experience-dependent remodeling of synaptic structure"
