# NLM Phase 2: Real Neural Computation

This document describes the Phase 2 implementation of NLM: Real Neural Computation.

## Overview

Phase 2 implements the core neural computation engine using biologically realistic Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, and Hebbian/STDP plasticity rules. This provides the fundamental neural substrate for Phase 3+ integration.

## Key Features

### 1. Real LIF Neuron Dynamics
- Membrane potential integration with leak
- Spike detection and reset
- Refractory periods
- Spike-frequency adaptation
- Realistic parameter ranges (-70mV resting, -55mV threshold)

### 2. Event-Driven Synaptic Transmission
- Synaptic delays (1-5ms)
- Short-term plasticity (Tsodyks-Markram model)
- Use-dependent efficacy modulation
- Eligibility traces for reward-modulated learning

### 3. Hebbian and STDP Plasticity
- "Neurons that fire together, wire together"
- Spike-timing dependent plasticity (STDP)
- LTP for correlated pre-post spikes
- LTD for anti-correlated timing
- Reward-modulated learning

### 4. Sensory Processing
- Vision processing with edge detection
- Internal signal processing (homeostatic regulation)
- Audio processing with spectral analysis
- Feature extraction mimicking cortical processing

### 5. Motor Output
- Action selection based on neural activity
- Reinforcement learning compatible
- Multiple action types (move, look, interact, etc.)

### 6. Development
- Synaptogenesis (activity-dependent synapse formation)
- Maturation (neural parameter refinement)
- Pruning (weight-based synaptic elimination)
- Activity-dependent development

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running Phase 2 Tests

```bash
./nlm
# Runs comprehensive Phase 2 tests:
# 1. Basic connectivity test
# 2. Plasticity learning experiment
# 3. STDP verification
```

## Running Phase 6 Integration

For full Phase 6 integration with all systems:

```bash
./nlm_phase6_demo
```

## Python Usage

For Python bindings and examples:

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Key Methods

### Brain Class
- `initialize()`: Start brain simulation with biologically realistic neurons
- `step(step)`: Advance simulation using LIF dynamics
- `injectCurrent(neuron, current)`: Add external input
- `produceAction()`: Generate motor output from neural activity
- `save(filepath)`, `load(filepath)`: Persist brain state

### AgentBrain Class  
- `processSensoryInput(percept)`: Inject sensory data into neural network
- `decodeMotorCommand()`: Read motor neuron activity for actions
- `applyRewardModulation(reward, predicted)`: Learn from rewards

### SimpleWorld Class
- `update(dt)`: Advance world simulation
- `applyMotorCommand(cmd, time)`: Execute actions
- `getSensoryPercept()`: Get current sensory input

## Configuration

```cpp
config.set("neuron_count", 1000);           // Number of neurons
config.set("region_count", 2);               // Brain regions
config.set("connection_probability", 0.2f);  // Synaptic density
config.set("stdp_ltp_weight", 0.02f);        // LTP strength
config.set("stdp_ltd_weight", 0.018f);       // LTD strength
config.set("synaptogenesis_rate", 0.001f);   // Synapse formation
config.set("pruning_rate", 0.0001f);         // Synapse elimination
```

## Files Modified in Phase 2

### Core Files
- `src/brain/Neuron.cpp`: Real LIF neuron implementation
- `src/brain/Synapse.cpp`: Real synaptic dynamics
- `src/plasticity/PlasticityRule.cpp`: Real Hebbian/STDP learning
- `src/neuromodulation/Neuromodulator.cpp`: Real neuromodulation
- `src/prediction/PredictionSystem.cpp`: Real prediction
- `src/core/Config/Config.cpp`: Real configuration parser

### Sensory Processing
- `src/sensory/Vision.cpp`: Real vision processing with edge detection
- `src/sensory/InternalSignals.cpp`: Real homeostatic signal processing
- `src/sensory/Audio.cpp`: Real audio processing with spectral analysis

### Dynamics
- `src/dynamics/NeuralDynamics.cpp`: Real LIF dynamics

### Development
- `src/development/Synaptogenesis.cpp`: Real synapse formation
- `src/development/Maturation.cpp`: Real neural maturation
- `src/development/Pruning.cpp`: Real synaptic pruning

### Motor System
- `src/motor/MotorSystem.cpp`: Real action selection

## Testing

### Unit Tests
```bash
cd tests
./test_neuron  # Test neuron dynamics
./test_synapse # Test synaptic transmission
./test_stdp   # Test spike-timing dependent plasticity
./test_plasticity # Test learning rules
```

### Integration Tests
```bash
# Phase 2 demo
./nlm

# Phase 6 integration demo
./nlm_phase6_demo
```

## Biological Realism

Phase 2 implements several biological constraints:

1. **Membrane potentials**: -70mV resting, -55mV threshold, -70mV reset
2. **Refractory periods**: 2-10ms post-spike
3. **Synaptic delays**: 1-5ms realistic transmission delays
4. **Weight bounds**: -1.0 to 1.0 for stability
5. **Activity-dependent dynamics**: Use-dependent synaptic efficacy

## Performance Characteristics

- **Memory usage**: ~100 bytes per neuron, ~200 bytes per synapse
- **Time step**: 1ms (configurable)
- **Parallelization**: Event-driven for efficiency
- **Scalability**: Can simulate thousands of neurons

## Future Enhancements

Phase 2 provides the foundation for:

1. **Phase 3**: World interaction and learning
2. **Phase 4**: Cognitive abilities (planning, attention, etc.)
3. **Phase 5**: Advanced plasticity and neuromodulation
4. **Phase 6**: Full integration of all systems

## License

MIT License - Experimental research project
