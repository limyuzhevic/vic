# NLM Biological Fidelity Documentation

## Overview

Phase 5 begins the systematic improvement of biological realism in NLM. This document explains the biological motivation, mathematical abstractions, engineering approximations, and limitations of each fidelity improvement.

## Design Philosophy

1. **Incremental fidelity**: Add complexity gradually, measure impact
2. **Biological motivation**: Every mechanism needs scientific justification
3. **Engineering approximation**: Practical implementation within computational limits
4. **Validation**: Test against biological data where possible
5. **Honest limitations**: Clearly state where we deviate from biology

## Current vs. Target Fidelity

| Feature | Phase 4 | Phase 5 | Long-term |
|---------|---------|---------|-----------|
| Neuron model | LIF | LIF+adaptive | Hodgkin-Huxley |
| Synapses | Static weights | Dynamic | Conductance-based |
| Connectivity | Random | Structured | Anatomical |
| Plasticity | STDP | STDP+homeostasis | Calcium-based |
| Inhibition | Fixed | Fast/slow | Interneuron types |
| Neuromodulation | Single | Multiple | Full receptor types |

## Neuron Models

### Phase 4: Leaky Integrate-and-Fire (LIF)

**Biological inspiration**: Abstracted spiking neurons

**Mathematical model**:
```
τ_m dV/dt = (V_rest - V) + I

if V >= V_threshold:
    spike
    V = V_reset
```

**Phase 5 additions**:
- Spike-frequency adaptation
- Subthreshold oscillations
- Configurable time constants

**Limitations**:
- No ion channel dynamics
- Fixed action potential shape
- No dendritic compartments

### Long-term: Hodgkin-Huxley

**Target biological realism**:
```
C dV/dt = g_na m³h (V - E_na) + g_k n⁴ (V - E_k) + g_leak (V - E_leak)

dm/dt = α_m(V) (1 - m) - β_m(V) m
dn/dt = α_n(V) (1 - n) - β_n(V) n
dh/dt = α_h(V) (1 - h) - β_h(V) h
```

**Challenges**:
- Computational cost (4 ODEs per neuron)
- Parameter fitting to biological data
- Numerical stability requirements

## Synaptic Dynamics

### Phase 4: Static Weights

**Current implementation**:
- Fixed synaptic weight
- Simple STDP updates
- Basic eligibility traces

### Phase 5: Dynamic Synapses

**Improvements**:

#### Short-Term Plasticity

**Biological inspiration**: Tsodyks-Markram model of facilitation/depression

**Mathematical model**:
```
dx/dt = (1 - x) / τ_facilitation - u * x * δ(t - t_pre)
du/dt = (U - u) / τ_recovery + U(1 - u) * δ(t - t_pre)

Effective weight = w * u * x
```

**Parameters**:
- τ_facilitation: 100-1000ms
- τ_depression: 200-500ms
- U (baseline utilization): 0.1-0.8

#### Synaptic Delays

**Biological realism**:
- Axonal propagation delay: 0.1-100ms
- Synaptic transmission delay: 0.5-5ms

**Implementation**:
- Configurable delay per synapse
- Time-bucketed event delivery
- Delay-dependent plasticity

#### Conductance-Based Synapses (Future)

**Target model**:
```
I_syn = g_syn * (V - E_syn)
dg_syn/dt = -g_syn / τ_syn + w * δ(t - t_spike)
```

**Benefits**:
- Synaptic input depends on membrane potential
- More realistic temporal dynamics
- Better E/I balance modeling

## Inhibitory Circuits

### Phase 4: Generic Inhibition

- Fixed inhibitory weight scaling
- No interneuron diversity

### Phase 5: Structured Inhibition

**Fast-spiking interneurons**:
- Short integration time constant
- High firing rates
- Perisomatic inhibition

**Low-threshold spiking interneurons**:
- Dendritic inhibition
- Response to inputs

**Implementation**:
```cpp
if (neuron.type == NeuronType::Inhibitory) {
    float inhibitionStrength = calculateInhibition(subtype);
    target.receiveInhibitoryInput(weight * inhibitionStrength);
}
```

## Plasticity Mechanisms

### STDP with Homeostatic Modulation

**Phase 4 STDP**:
```
Δw = A_plus * exp(-|Δt|/τ_plus) if Δt > 0
Δw = -A_minus * exp(|Δt|/τ_minus) if Δt < 0
```

**Phase 5 additions**:

#### Synaptic Scaling
```
dw/dt = λ * (w_target - w)
```

#### Intrinsic Plasticity
```
τ dV_th/dt = (V_th_target - V_th)
```

#### Homeostatic metaplasticity
```
A_plus = A_plus_base * (1 - θ * (w - w_avg))
```

### Eligibility Traces

**Biological inspiration**: Dopamine-modulated plasticity

**Implementation**:
```cpp
if (synapse.reward > 0) {
    synapse.eligibilityTrace *= decay;
    synapse.weight += eta * synapse.eligibilityTrace;
} else {
    synapse.eligibilityTrace *= decay;
}
```

## Neuromodulatory Systems

### Phase 4: Generic Reward

Single dopamine-like signal affecting plasticity.

### Phase 5: Multiple Modulators

#### Dopamine (Reward Prediction Error)

**Biological**: VTA dopamine neurons signal reward prediction error

**Implementation**:
- Tonic vs. phasic signaling
- Temporal difference learning
- Value function estimation

#### Norepinephrine (Arousal/Attention)

**Biological**: Locus coeruleus system

**Implementation**:
- Global arousal modulation
- Attention allocation
- Vigilance state control

#### Acetylcholine (Attention/Memory)

**Biological**: Basal forebrain cholinergic system

**Implementation**:
- Memory encoding modulation
- Attention enhancement
- Plasticity gates

## Structural Plasticity

### Synaptogenesis

**Biological**: Activity-dependent synapse formation

**Implementation**:
```cpp
if (activity > threshold && available_slots > 0) {
    new_synapse = createSynapse(pre_neuron, post_neuron);
    new_synapse.weight = initial_weight;
}
```

### Pruning

**Biological**: Activity-dependent synapse elimination

**Implementation**:
```cpp
if (synapse.weight < pruning_threshold && synapse.age > maturation_period) {
    removeSynapse(synapse);
}
```

## Developmental Processes

### Critical Periods

**Biological inspiration**: Experience-dependent development windows

**Implementation**:
```cpp
float plasticity = base_plasticity * modifier;
if (in_critical_period) {
    plasticity *= critical_period_modifier;
}
```

### Maturation

**Biological**: Adult state development

**Changes during maturation**:
- Membrane time constant increases
- Synaptic scaling adjustments
- Critical period closure

## Oscillatory Dynamics

### Phase 5 Goals

Implement population-level oscillations:

#### Gamma (30-80 Hz)
- Associated with attention
- Local synchrony

#### Theta (4-8 Hz)
- Memory consolidation
- Hippocampal-cortical interaction

### Implementation Approach

```cpp
// Population-level oscillation
for (neuron in population) {
    oscillation_input = amplitude * sin(2 * PI * frequency * time + phase);
    neuron.receiveModulatoryInput(oscillation_input);
}
```

## Validation Against Biology

### Quantitative Tests

1. **Firing rates**: Match biological observations
2. **Spike timing**: STDP windows
3. **Oscillation frequencies**: Power spectra
4. **Connectivity statistics**: Degree distributions

### Qualitative Tests

1. **Response to perturbations**: Similar to biological experiments
2. **Development**: Stages similar to biological development
3. **Learning**: Behavior consistent with animal learning

## Limitations

1. **Scale gap**: Biological brains have 86B neurons vs. millions in simulation
2. **Molecular processes**: No gene regulation, protein synthesis
3. **Glial cells**: Astrocytes, oligodendrocytes not modeled
4. **Metabolism**: No energy constraints, blood flow
5. **Anatomy**: No cortical layers, thalamic circuits
6. **Noise**: Biological variability not fully captured

## Future Fidelity Improvements

| Priority | Feature | Complexity | Impact |
|----------|---------|------------|--------|
| High | Conductance synapses | Medium | Better E/I |
| High | Multiple neuron types | High | Realistic circuits |
| Medium | Calcium-based STDP | High | Better plasticity |
| Medium | Glial interaction | Very High | Metabolic effects |
| Low | Full Hodgkin-Huxley | Very High | Best realism |

## Scientific Integrity

**What we claim**:
- "Biologically inspired approximation"
- "Self-organization from initial architecture"
- "Emergent behavior from neural dynamics"

**What we don't claim**:
- "Exact biological simulation"
- "Identical to neural tissue"
- "Conscious or sentient"