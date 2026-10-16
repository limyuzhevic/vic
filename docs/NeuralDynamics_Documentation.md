# Neural Dynamics Documentation

## Overview

The Neural Dynamics module implements biologically realistic neural computation systems for the NLM (Neural Learning Machine) project. This module provides several advanced neuron models with proper integrate-and-fire dynamics, synaptic transmission, and plasticity mechanisms.

## Core Components

### 1. NeuralDynamics Abstract Base Class

The `NeuralDynamics` class defines the interface for all neural dynamics models:

**Purpose**: Abstract base class for implementing different neuron models
- `updateNeuron(Neuron* neuron, TimestepDuration dt)`: Update neuron state for one timestep
- `updateSynapse(Synapse* synapse, TimestepDuration dt)`: Update synapse state and dynamics
- `applySpikeInput(Neuron* neuron, const Synapse* synapse)`: Apply synaptic input to neuron
- `shouldFire(const Neuron* neuron) const`: Determine if neuron should fire based on membrane potential
- `reset()`: Reset internal dynamics state

### 2. IntegrateAndFireDynamics Class

The `IntegrateAndFireDynamics` class implements the standard Leaky Integrate-and-Fire (LIF) neuron model with enhanced features:

**Key Features**:
- Exponential integration for numerical stability
- Adaptive threshold with spike-frequency adaptation
- Refractory period with exponential recovery
- Membrane leak conductance for realistic dynamics
- Time-tracking for spike events

**Mathematical Model**:
```
dV/dt = (-(V - V_rest) + I * R) / tau
```

Where:
- `V`: Membrane potential
- `V_rest`: Resting potential (-70 mV)
- `I`: Total injected current
- `R`: Membrane resistance
- `tau`: Membrane time constant

**Enhanced Features**:
1. **Exponential Integration**: Uses analytic solution of LIF equation for better stability
2. **Adaptive Threshold**: Incorporates adaptation variable for spike-frequency adaptation
3. **Refractory Recovery**: Exponential recovery from refractory period
4. **Time Tracking**: Precise spike timing using actual dt values
5. **Leak Dynamics**: Realistic membrane leak conductance

**Methods**:
- `setMembraneTimeConstant(float tau)`: Set membrane time constant (clamped to 1-100 ms)
- `getMembraneTimeConstant() const`: Get current membrane time constant
- `setMembraneResistance(float R)`: Set membrane resistance (clamped to 1-1000 MOhm)
- `getMembraneResistance() const`: Get current membrane resistance
- `setThresholdFactor(float factor)`: Set threshold scaling factor (clamped to 0.5-3.0)
- `getThresholdFactor() const`: Get current threshold factor

### 3. HodgkinHuxleyDynamics Class

The `HodgkinHuxleyDynamics` class implements the detailed Hodgkin-Huxley model of neuronal dynamics:

**Biological Basis**:
- Models voltage-gated sodium and potassium channels
- Captures action potential shape and firing patterns
- Includes adaptation mechanisms for spike-frequency adaptation

**Key Variables**:
- `gNa`: Sodium conductance (mS/cm²)
- `gK`: Potassium conductance (mS/cm²)
- `gL`: Leak conductance (mS/cm²)
- `VNa`: Sodium reversal potential (50 mV)
- `VK`: Potassium reversal potential (-77 mV)
- `EL`: Leak reversal potential (-54.387 mV)
- `Cm`: Membrane capacitance (μF/cm²)

**Gating Variables**:
- `m`: Sodium activation
- `h`: Sodium inactivation
- `n`: Potassium activation

**Implementation Details**:
- Updates gating variables using standard HH differential equations
- Calculates ion currents using Hodgkin-Huxley formulations
- Applies external current injection
- Handles spike generation and adaptation

### 4. FitzHughNagumoDynamics Class

The `FitzHughNagumoDynamics` class implements the simplified FHN model:

**Mathematical Model**:
```
dV/dt = c * (V - V³/3 + W + I)
dW/dt = (V - a*W + b) / c
```

Where:
- `V`: Membrane potential
- `W`: Recovery variable
- `c`: Time scale separation parameter
- `a`, `b`: Model parameters
- `I`: External input

**Characteristics**:
- Captures excitable dynamics and action potentials
- Exhibits Hopf bifurcation for frequency changes
- Simple yet powerful model for studying neural dynamics

## Synaptic Dynamics

### Short-Term Plasticity

The system implements short-term synaptic plasticity (STP) with:
- **Synaptic Depression**: Reduced efficacy with repeated stimulation
- **Synaptic Facilitation**: Increased efficacy with high-frequency stimulation
- **Adaptation Dynamics**: Efficacy evolves based on recent activity

**Implementation**:
- Time constants for depression and facilitation
- Use-dependent efficacy modulation
- Weight stabilization within biological bounds

### Spike Timing-Dependent Plasticity (STDP)

Implemented in the `HebbianRule` class:
- **LTP (Long-Term Potentiation)**: Post-synaptic spike before pre-synaptic spike (within ~20ms)
- **LTD (Long-Term Depression)**: Pre-synaptic spike before post-synaptic spike (within ~20ms)
- **Learning Window**: Asymmetric Hebbian learning rule
- **Eligibility Traces**: For reward-modulated STDP

**Mathematical Formulation**:
```
if (t_post > t_pre): Δw = A⁺ * exp(-(t_post - t_pre)/τ⁺)
if (t_pre > t_post): Δw = -A⁻ * exp(-(t_pre - t_post)/τ⁻)
```

## Configuration and Parameters

### Neural Dynamics Parameters

1. **Membrane Time Constant (τ)**:
   - Range: 1-100 ms
   - Default: 20 ms
   - Biological relevance: Typical cortical neuron time constants

2. **Membrane Resistance (R)**:
   - Range: 1-1000 MOhm
   - Default: 10 MOhm
   - Biological relevance: Membrane resistance varies by neuron type

3. **Threshold Factor (α)**:
   - Range: 0.5-3.0
   - Default: 1.0
   - Biological relevance: Modulates firing threshold sensitivity

### Synaptic Parameters

1. **Synaptic Weight**:
   - Range: 0-2.0
   - Default: 1.0
   - Biological relevance: Normalized synaptic strength

2. **Synaptic Efficacy**:
   - Range: 0.1-5.0
   - Default: 1.0
   - Biological relevance: Short-term plasticity modulation

## Usage Examples

### Basic Usage
```cpp
#include "NeuralDynamics.hpp"
#include "Brain.hpp"

// Create different neuron models
auto lifNeuron = std::make_unique<IntegrateAndFireDynamics>();
auto hhNeuron = std::make_unique<HodgkinHuxleyDynamics>();
auto fhnNeuron = std::make_unique<FitzHughNagumoDynamics>();

// Configure neuron parameters
lifNeuron->setMembraneTimeConstant(30.0f);
lifNeuron->setMembraneResistance(15.0f);
lifNeuron->setThresholdFactor(1.5f);

// Create neuron and connect to dynamics
Neuron* neuron = new Neuron(NeuronId(1));
lifNeuron->updateNeuron(neuron, 0.001f);  // 1ms timestep
```

### Advanced Configuration
```cpp
// Configure for specific neuron types
if (neuronType == "cortical") {
    lifNeuron->setMembraneTimeConstant(20.0f);
    lifNeuron->setMembraneResistance(10.0f);
    lifNeuron->setThresholdFactor(1.0f);
} else if (neuronType == "fast_spiking") {
    lifNeuron->setMembraneTimeConstant(10.0f);
    lifNeuron->setMembraneResistance(5.0f);
    lifNeuron->setThresholdFactor(0.8f);
}
```

## Performance Considerations

1. **Numerical Stability**:
   - Exponential integration provides better stability than Euler method
   - Adaptive timestep handling for variable simulation speeds

2. **Memory Efficiency**:
   - pImpl pattern for hiding implementation details
   - Efficient state management

3. **Computational Complexity**:
   - LIF: O(1) per neuron per timestep
   - HH: O(1) per neuron per timestep (constant gating variables)
   - FHN: O(1) per neuron per timestep (simple equations)

## Integration with Other Systems

### Neuromodulation
- Dopamine affects plasticity factors
- Acetylcholine modulates attention and learning
- Norepinephrine influences arousal and vigilance
- Serotonin affects mood and impulsivity

### Memory Systems
- Working memory maintained through persistent activity
- Episodic memory enabled through spike timing-dependent plasticity
- Associative memory through Hebbian learning rules

### Action Selection
- Motor system converts neural activity to behavioral outputs
- Reward prediction error guides learning
- Action preferences learned through reinforcement

## Future Extensions

1. **Advanced Neuron Models**:
   - Adaptive exponential integrate-and-fire (AdEx)
   - Morris-Lecar model for bursting neurons
   - Neural field models for population dynamics

2. **Enhanced Plasticity**:
   - Spike-timing dependent plasticity with multiple timescales
   - Structural plasticity for synapse formation/elimination
   - Homeostatic plasticity for stability

3. **Network Dynamics**:
   - Complex network simulations
   - Activity-dependent plasticity
   - Synchronization phenomena

4. **Real-world Applications**:
   - Sensorimotor control
   - Cognitive modeling
   - Neuromorphic computing

## References

1. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neuronal Dynamics. Cambridge University Press.
2. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
3. Koch, C. (2004). Biophysics of Computation. Wiley.
4. Song, S., Miller, K. D., & Abbott, L. F. (2000). Neural spread of synchronous spiking in cortical networks. Neuron, 26(3), 611-617.

## License

This documentation is part of the NLM (Neural Learning Machine) project.
The source code is licensed under the MIT License.
