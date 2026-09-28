# NLM Anti-Hebbian and BCM Plasticity Rules

## Overview

This document provides comprehensive information about the Anti-Hebbian and BCM (Bienenstock-Cooper-Munro) plasticity rules implemented in the NLM (Neural Learning Machine) framework. These rules extend the basic Hebbian and STDP plasticity mechanisms to provide more sophisticated learning capabilities.

## Anti-Hebbian Learning Rule

### Concept
The Anti-Hebbian rule implements "neurons that fire together, weaken together" learning. Unlike standard Hebbian learning which strengthens connections between co-active neurons, Anti-Hebbian learning weakens connections between neurons that fire together, promoting inhibitory interactions and preventing runaway excitation.

### Mathematical Formulation

Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)

Where:
- **η** = learningRate (positive constant controlling learning strength)
- **pre * post** = correlated activity (coincidence counting)
- **⟨pre⟩⟨post⟩** = uncorrelated activity (baseline)
- **Negative sign** means correlated activity weakens the connection

### Biological Inspiration

- Reflects inhibitory synaptic plasticity mechanisms
- Prevents runaway excitation in neural circuits
- Implements homeostatic mechanisms for stability
- Related to homosynaptic depression processes
- Found in circuits requiring balance between excitation and inhibition

### When to Use

1. **Inhibitory Circuit Design**: Creating balanced excitation-inhibition systems
2. **Stability Mechanisms**: Preventing neural runaway activity
3. **Homeostatic Regulation**: Implementing circuit stability under varying inputs
4. **Complex Pattern Recognition**: Circuits with strong recurrent connections
5. **Memory Systems**: Preventing uncontrolled strengthening in associative memory

### Implementation Details

The NLM implementation uses a spike-timing window approach:

```cpp
// Anti-Hebbian rule: correlated activity weakens connection
// Use spike timing window similar to STDP but inverted
if (std::abs(dt) < pImpl->threshold * 100.0f) {
    float delta = -pImpl->learningRate * std::exp(-std::abs(dt) / 50.0f);
    totalDelta += delta;
}
```

### Parameters

| Parameter | Description | Default Value | Range |
|-----------|-------------|---------------|-------|
| `learningRate` | Strength of anti-hebbian learning | 0.01f | [0.0, 1.0] |
| `threshold` | Activity threshold for anti-hebbian effect | 0.5f | [0.0, 1.0] |

### Example Usage

```cpp
#include "plasticity/PlasticityRule.hpp"
#include "brain/Brain.hpp"

// Enable Anti-Hebbian plasticity on synapses
auto* brain = new Brain(config);
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(false, false, true);  // Enable only Anti-Hebbian
    }
}

// Configure Anti-Hebbian parameters
if (auto* antiHebbian = brain->getAntiHebbian()) {
    antiHebbian->setLearningRate(0.02f);
    antiHebbian->setThreshold(0.3f);
}
```

## BCM (Bienenstock-Cooper-Munro) Learning Rule

### Concept

BCM rule implements the Synaptic Modification Ratio Theory, using a sliding threshold that depends on average postsynaptic activity. This allows the rule to be state-dependent and maintain stability under varying input statistics.

### Mathematical Formulation

Δw = η * (post * (pre - θ))

Where:
- **η** = learningRate (base learning rate)
- **post** = postsynaptic activity
- **pre** = presynaptic activity
- **θ** = sliding threshold (depends on average postsynaptic activity)

### Biological Inspiration

- Reflects calcium dynamics in postsynaptic neuron
- Threshold depends on average firing rate (homeostatic)
- Prevents runaway potentiation while allowing LTD
- Found in hippocampal and cortical synapses
- Implements BCM theory from neuronal calcium dynamics

### Key Features

1. **Sliding Threshold**: θ adapts based on recent postsynaptic activity
2. **State-Dependent Learning**: Rule behavior changes with network state
3. **Homeostatic Regulation**: Maintains stability under varying inputs
4. **Asymmetric LTP/LTD**: Produces different strengthening/weakening curves
5. **Biologically Plausible**: Based on actual neuronal calcium dynamics

### Implementation Details

The sliding threshold is updated based on recent postsynaptic activity:

```cpp
// Update sliding threshold based on recent postsynaptic activity
float avgPostActivity = 0.0f;
size_t postCount = postSpikes.size();
if (postCount > 0) {
    avgPostActivity = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f) / postCount;
}

// Update sliding threshold (homeostatic regulation)
pImpl->slidingThreshold += pImpl->thetaLearningRate * (avgPostActivity - pImpl->baselineTheta);
pImpl->slidingThreshold = std::clamp(pImpl->slidingThreshold, pImpl->thetaMin, pImpl->thetaMax);
```

### Parameters

| Parameter | Description | Default Value | Range |
|-----------|-------------|---------------|-------|
| `learningRate` | Base learning rate | 0.01f | [0.0, 1.0] |
| `slidingThreshold` | Current sliding threshold | 0.5f | [0.0, 1.0] |
| `thetaMin` | Minimum threshold value | 0.1f | [0.0, 1.0] |
| `thetaMax` | Maximum threshold value | 1.0f | [0.0, 1.0] |
| `thetaLearningRate` | Rate of threshold adaptation | 0.001f | [0.0, 0.01] |
| `baselineTheta` | Baseline threshold value | 0.5f | [0.0, 1.0] |

### When to Use

1. **Homeostatic Circuits**: Systems requiring stability under varying inputs
2. **Cortical Models**: Simulating cortical synaptic plasticity
3. **Hippocampal Learning**: Modeling hippocampal synaptic mechanisms
4. **Balanced Networks**: Circuits with excitation-inhibition balance
5. **Complex Learning**: Systems needing state-dependent learning rules

### Example Usage

```cpp
#include "plasticity/PlasticityRule.hpp"
#include "brain/Brain.hpp"

// Enable BCM plasticity on synapses
auto* brain = new Brain(config);
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, true);  // Enable all plasticity including BCM
    }
}

// Configure BCM parameters
if (auto* bcm = brain->getBCM()) {
    bcm->setLearningRate(0.02f);
    bcm->setSlidingThreshold(0.3f);
    bcm->setBaselineTheta(0.4f);
    bcm->setThetaLearningRate(0.0005f);
}
```

## Integration with NLM Systems

### Brain Integration

Both Anti-Hebbian and BCM rules are integrated into the Brain class as part of the comprehensive plasticity system:

```cpp
// Access plasticity rules from brain
- getSTDP(): STDP* - Spike-timing dependent plasticity
- getHebbian(): Hebbian* - Standard Hebbian learning  
- getAntiHebbian(): AntiHebbian* - Anti-Hebbian learning
- getBCM(): BCM* - BCM learning rule
- getStructuralPlasticity(): StructuralPlasticity* - Synaptogenesis/pruning
```

### Agent Integration

The enhanced brain integration allows these rules to be used in agent-based simulations:

```cpp
// Configure agent plasticity
agentBrain->enableAntiHebbian(true);
agentBrain->enableBCM(true);
agentBrain->enableDevelopment(true);

// Use in experiments
// Experiments can now leverage advanced plasticity rules for more sophisticated learning behaviors
```

## Performance Considerations

### Computational Complexity

Both rules have O(n²) complexity for spike pair calculations, similar to STDP. However, they include additional threshold calculations and parameter updates.

### Memory Requirements

- **Anti-Hebbian**: Minimal additional parameters (2 floats)
- **BCM**: More parameters due to sliding threshold dynamics (6 floats)

### Numerical Stability

- Both rules include bounds checking and clamping
- BCM requires careful handling of threshold dynamics
- Anti-Hebbian learning rate should be smaller to prevent instability

## Testing and Validation

### Test Coverage

1. **Anti-Hebbian Tests**:
   - Correlated spike pair response
   - Weight bounds enforcement
   - Threshold-based window effectiveness
   - Learning rate scaling

2. **BCM Tests**:
   - Sliding threshold dynamics
   - Homeostatic regulation
   - State-dependent learning behavior
   - LTP/LTD asymmetry

### Validation Metrics

- **Convergence**: Stability under continuous learning
- **Balance**: Maintaining excitation-inhibition equilibrium
- **Generalization**: Transfer to novel input patterns
- **Memory Retention**: Preservation of learned connections

## Future Enhancements

### Research Directions

1. **Neuromodulation Integration**:
   - Dopamine modulation of Anti-Hebbian learning
   - Neuromodulator effects on BCM threshold dynamics

2. **Higher-Order Interactions**:
   - Multi-spike patterns
   - Temporal sequence learning
   - Context-dependent plasticity

3. **Online Parameter Adaptation**:
   - Activity-dependent learning rate adjustment
   - Dynamic threshold optimization

4. **Biological Plausibility**:
   - More realistic calcium dynamics
   - Spike timing precision improvements
   - Synaptic eligibility trace integration

## References

1. **Bienenstock, E.L., Cooper, L.N., & Munro, R.D. (1982)**. A model for neural learning and memory. *Proceedings of the National Academy of Sciences*, 79(9), 2800-2804.

2. **Gallistel, C.R. (1995)**. The organization of learning and memory. *Annual Review of Neuroscience*, 18, 243-268.

3. **Danbolt, J., & Ottersen, P. (2006)**. Long-term depression of CNS synapses. *Physiological Reviews*, 86(1), 185-231.

4. **Abbott, L.F., & Nelson, S.B. (2000)**. Synaptic plasticity: taming the beast. *Nature Reviews Neuroscience*, 1(1), 45-54.

## Conclusion

The Anti-Hebbian and BCM plasticity rules significantly enhance the NLM framework's learning capabilities. They provide:

1. **Complementary Learning Mechanisms**: Balance to standard Hebbian/STDP rules
2. **Biological Plausibility**: Grounded in actual synaptic plasticity mechanisms
3. **Homeostatic Regulation**: Stability under varying network conditions
4. **Research Flexibility**: Tools for investigating complex neural dynamics
5. **Advanced Applications**: Support for sophisticated learning scenarios

These rules enable researchers to build more realistic and stable neural models, supporting the NLM's mission of creating experimental artificial developmental brains with sophisticated learning capabilities.