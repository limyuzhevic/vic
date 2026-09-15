# Neuron.hpp - Neuron Class Documentation

## Overview
The `Neuron` class implements a biologically realistic Leaky Integrate-and-Fire (LIF) neuron model with spike-frequency adaptation and complex state dynamics. It forms the fundamental computational unit of the NLM brain.

## Key Features
- **Realistic LIF Dynamics**: Mathematical model based on membrane potential integration
- **Spike-Frequency Adaptation**: Adaptation variable modulates firing rate over time
- **Refractory Period**: Absolute refractory period prevents immediate re-firing
- **State Management**: Complete neuron state tracking including membrane potential, threshold, and firing history
- **Synaptic Integration**: Handles excitatory and inhibitory synaptic inputs
- **Plasticity Support**: Tracks spike history for plasticity rules (STDP, Hebbian)
- **Performance Optimized**: Efficient state updates and minimal allocations during simulation

## Biological Basis
The neuron model implements the following biological principles:
- **Membrane Potential Dynamics**: dV/dt = (V_rest - V)/τ + I/C
- **Spike Generation**: Threshold-based action potential generation
- **Refractory Period**: Absolute refractory period following spike
- **Adaptation**: Spike-frequency adaptation via calcium-dependent potassium currents
- **Synaptic Integration**: Linear summation of excitatory and inhibitory inputs

## Usage
```cpp
#include "brain/Neuron.hpp"

// Create neuron with unique ID
Neuron neuron(neuronId);

// Configure neuron parameters
neuron.setThreshold(-55.0f);
neuron.setRestingPotential(-70.0f);
neuron.setResetPotential(-65.0f);
neuron.setRefractoryPeriod(5); // 5ms

// Inject inputs
neuron.injectCurrent(10.0f);  // Excitatory current
neuron.receiveExcitatoryInput(5.0f);
neuron.receiveInhibitoryInput(3.0f);

// Process simulation step
bool fired = neuron.stepLIF(currentTime, timestep);
if (fired) {
    // Handle spike
    neuron.recordSpike(currentTime);
}
```

## Thread Safety
Neuron operations are not thread-safe. Each neuron should be accessed from a single thread during simulation.

## Performance Considerations
- **Minimal Allocations**: Uses value semantics where possible
- **Efficient State Updates**: In-place modifications where feasible
- **Cache-Friendly**: Contiguous memory layout for frequently accessed data
- **SIMD Ready**: Designed for potential SIMD optimization

## Methods

### State Management
- `getState()` - Get complete neuron state reference
- `getMembranePotential()`, `setMembranePotential()` - Membrane potential
- `getThreshold()`, `setThreshold()` - Firing threshold
- `getRestingPotential()`, `setRestingPotential()` - Resting potential
- `getResetPotential()`, `setResetPotential()` - Reset potential after spike

### Dynamic State
- `stepLIF(Timestamp, TimestepDuration)` - Process LIF dynamics
- `step(Timestamp)` - Convenience method with default timestep
- `injectCurrent(MembranePotential)` - Direct current injection
- `receiveExcitatoryInput()`, `receiveInhibitoryInput()` - Synaptic inputs
- `receiveModulatoryInput()` - Neuromodulatory inputs

### Spike Management
- `isFiring()` - Check if neuron is currently firing
- `checkThreshold()` - Check if membrane potential exceeds threshold
- `recordSpike(Timestamp)` - Record spike in history
- `getLastSpikeTime()` - Get last spike time
- `clearSpikeHistory()` - Clear spike history

### Refractory Period
- `setRefractoryPeriod(uint32_t)` - Set refractory period duration
- `getRefractoryPeriod()` - Get current refractory period
- `decrementRefractory()` - Decrement refractory counter

### Plasticity Support
- `getPlasticityFlags()` - Get plasticity configuration
- `enablePlasticity(bool, bool, bool)` - Configure plasticity rules
- `getSpikeHistory()` - Get spike history for STDP

### Configuration
- `setType(NeuronType)` - Set neuron type (Sensory, Internal, Motor)
- `setRegionId()`, `setPopulationId()` - Set neural location
- `initializeRandom(RandomGenerator&)` - Initialize with biological constraints
- `reset()` - Reset to initial state

## NeuronState Structure
```cpp
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential resetPotential;      // Reset potential after spike (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential leakConductance;     // Leak conductance
    FiringState firingState;              // Current firing state
    uint32_t refractoryRemaining;          // Remaining refractory steps
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // Spike-frequency adaptation variable
    float lastSpikeTime;                   // Time of last spike
    float firingRate;                      // Instantaneous firing rate
};
```

## LIF Parameters (Biological Ranges)
- **Membrane Capacitance**: ~1 nF
- **Time Constant**: 20-30 ms
- **Threshold**: -55 ± 2 mV
- **Resting Potential**: -70 ± 2 mV
- **Reset Potential**: -65 ± 3 mV
- **Refractory Period**: 2-10 ms
- **Adaptation**: 0-2.0 (scaled)

## Error Handling
- Returns boolean values for operation success
- Clear error messages for invalid operations
- Bounds checking on parameter values

## Development Notes
- **Phase 2**: Basic LIF implementation with refractory period
- **Phase 3**: Added spike-frequency adaptation
- **Phase 4**: Enhanced synaptic integration
- **Phase 6**: Full biological realism with adaptation and plasticity support

## Performance Optimization Tips
1. **Reuse Neurons**: Neuron objects are expensive to create; reuse during simulation
2. **Batch Operations**: Process multiple neurons in contiguous memory
3. **Pre-allocate**: Reserve spike history capacity upfront
4. **Minimize Logging**: Reduce debug output during performance-critical phases

## See Also
- `Synapse.hpp` - Synaptic connections
- `Brain.hpp` - Brain-level integration
- `SpikeSystem.hpp` - Spike event management
- `PlasticityRule.hpp` - Plasticity rules

## Version History
- **v6.0.0**: Complete LIF implementation with adaptation and plasticity
- **v4.0.0**: Added spike-frequency adaptation
- **v3.0.0**: Basic LIF with refractory period
- **v2.0.0**: Initial LIF skeleton
- **v1.0.0**: Conceptual design

The Neuron class represents one of the most critical components of the NLM brain, providing the foundation for all neural computation through realistic neuron dynamics and spike generation.