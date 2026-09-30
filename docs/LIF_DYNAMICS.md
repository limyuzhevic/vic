# LIF Dynamics Documentation

## Leaky Integrate-and-Fire (LIF) Neuron Model

### Overview

The Leaky Integrate-and-Fire (LIF) model is the fundamental computational model used throughout the NLM brain. It balances biological plausibility with computational efficiency, making it suitable for large-scale neural simulations.

### Mathematical Formulation

The LIF model is described by the differential equation:

```
dV/dt = (V_rest - V)/tau + I/C
```

Where:
- **V** = membrane potential (mV)
- **V_rest** = resting potential (typically -70mV)
- **tau** = membrane time constant (20ms typical)
- **I** = total synaptic current (nA)
- **C** = membrane capacitance (1nF)

### Discrete Implementation

The continuous model is discretized using exponential Euler integration:

```
V_new = V + dt * ((V_rest - V)/tau + I/C)
```

Where:
- **dt** = time step (in seconds)
- All other parameters as defined above

**Unit consistency:** dt is in seconds, tau in milliseconds, so factor 1000.0 multiplies dt.

### Biological Interpretations

1. **Leak term (V_rest - V)/tau:** Models ion channel leakage and membrane conductance
2. **I/C term:** Represents synaptic currents integrated over capacitance
3. **Membrane capacitance (C):** Total charge storage capacity
4. **Time constant (tau):** Determines how quickly voltage decays to resting

### Key Parameters

#### Membrane Properties
- **MEMBRANE_CAPACITANCE**: 1.0f nF (total charge storage)
- **TIME_CONSTANT**: 20.0f ms (exponential decay rate)
- **LEAK_CONDUCTANCE**: Variable conductance for different neuron types

#### Electrical Properties
- **RESTING_POTENTIAL**: -70mV typical for cortical neurons
- **THRESHOLD**: -55mV typical for action potential initiation
- **RESET_POTENTIAL**: -65mV to -75mV typical post-spike
- **REFRACTORY_PERIOD**: 2-10ms biologically realistic

#### Input Properties
- **EXCITATORY_SYNAPTIC_WEIGHTS**: +0.1 to +1.0 nA
- **INHIBITORY_SYNAPTIC_WEIGHTS**: -0.1 to -1.0 nA
- **INJECTED_CURRENT**: -100 to +100 nA (external inputs)

### Implementation Details in Neuron.cpp

#### StepLIF Method (lines 214-282)

```cpp
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // 1. Refractory handling
    if (pImpl->state.refractoryRemaining > 0) {
        // Absolute refractory prevents spiking
        pImpl->synapticInput = 0.0f;
        return false;
    }
    
    // 2. LIF dynamics computation
    float synapticContribution = pImpl->synapticInput / C;
    float leakContribution = (V_rest - V) / tau;
    V = V + static_cast<float>(dt) * 1000.0f * 
        (leakContribution + synapticContribution);
    
    // 3. Spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    // 4. Boundary conditions
    V = std::clamp(V, -100.0f, 50.0f);
    
    // 5. Spike detection and reset
    if (V >= threshold) {
        // Record spike, reset voltage, enter refractory
        V = V_reset;
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.adaptationVariable += 1.0f;
        return true;
    }
    
    // 6. Clear for next timestep
    pImpl->synapticInput = 0.0f;
    return false;
}
```

### Input Integration Methods

#### receiveExcitatoryInput (lines 122-131)
- **Purpose**: Add excitatory synaptic current
- **Range**: [-100.0f, 100.0f] mV (biological plausibility)
- **Validation**: Rejects NaN/infinite values
- **Integration**: Direct addition to synaptic input buffer

#### receiveInhibitoryInput (lines 133-143)
- **Purpose**: Add inhibitory synaptic current
- **Range**: [-100.0f, 100.0f] mV
- **Mechanism**: Subtracts from synaptic input (hyperpolarization)
- **Validation**: Same as excitatory inputs

#### receiveModulatoryInput (lines 145-155)
- **Purpose**: Modulatory inputs affecting plasticity (not direct integration)
- **Example**: Dopamine, acetylcholine effects
- **Integration**: Affects adaptation variable, not voltage directly
- **Range**: [-100.0f, 100.0f] mV

#### injectCurrent (lines 157-167)
- **Purpose**: External current injection (sensory, motor, external)
- **Range**: [-100.0f, 100.0f] mV
- **Integration**: Added to synaptic input buffer for processing

### Spike Handling

#### recordSpike (lines 173-178)
- **Purpose**: Record spike timing for plasticity and analysis
- **History limit**: MAX_SPIKE_HISTORY (100) spikes
- **Data structure**: std::vector<Timestamp> with circular buffer semantics

#### Refractory Period
- **Duration**: Configurable per neuron (2-10ms typical)
- **Behavior**: Complete silencing, input ignored
- **Implementation**: pImpl->state.refractoryRemaining counter
- **State transition**: Refractory → Resting after completion

### Numerical Stability

#### Clamping
- **Voltage range**: -100.0f to 50.0f mV (prevents runaway excitation)
- **Timing**: Applied after each integration step
- **Rationale**: Biological limits and numerical stability

#### Minimum Delta Check
- **Epsilon**: 1e-6f for weight changes
- **Purpose**: Avoid unnecessary precision operations
- **Location**: In applyWeightChange methods

#### Adaptation Decay
- **Rate**: 0.95f per timestep (5% decay per step)
- **Formula**: pImpl->state.adaptationVariable *= 0.95f
- **Effect**: Slow decay of adaptation variable

### Performance Considerations

#### Integration Order
1. Refractory check (fast path)
2. Voltage update (LIF computation)
3. Adaptation application (slow component)
4. Boundary conditions
5. Spike detection and handling
6. Input buffer clearing

#### Cache Efficiency
- **SoA layout**: Neuron state stored in separate arrays
- **Local variables**: Frequently accessed values stored in registers
- **Pointer dereferencing**: Minimized through references

#### SIMD Potential
- **Vectorized operations**: Leak and synaptic contributions
- **Batch processing**: Multiple neurons can be processed together
- **SIMD considerations**: Fixed-size operations, aligned memory

### Biological Fidelity Notes

#### Realistic Parameter Ranges
- **Resting potential**: -60 to -80 mV (neuron type dependent)
- **Threshold**: -55 to -40 mV
- **Refractory period**: 1-20 ms (species and neuron type dependent)
- **Time constant**: 10-50 ms
- **Membrane capacitance**: 0.5-2.0 nF

#### Limitations
- **Single compartment**: Ignores dendritic and axonal currents
- **Point neuron**: No spatial integration properties
- **Deterministic**: No noise or stochastic channel dynamics
- **Linear synapses**: No nonlinear interaction effects

#### Extensions
- **Multiple compartments**: Hodgkin-Huxley models for detailed dynamics
- **Synaptic plasticity**: STDP, Hebbian rules modify connectivity
- **Neuromodulation**: Dopamine, serotonin affect excitability
- **Active properties**: Channel-based dynamics
