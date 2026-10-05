# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Python Bindings for Neural Components

NLM includes comprehensive Python bindings for all neural components, providing a user-friendly interface to the neural simulation framework. The bindings support:

### Core Neural Classes

#### Neuron Class
- **LIF Dynamics**: Leaky Integrate-and-Fire neuron model with full method bindings
- **State Management**: Complete access to membrane potential, threshold, firing state
- **Plasticity Controls**: Hebbian, STDP, and reward-modulated plasticity
- **Spike History**: Recording and management of spike events
- **Input/Output**: Excitatory/inhibitory inputs and current injection

#### Synapse Class
- **Connection Management**: Source/destination neuron relationships
- **Weight Control**: Dynamic weight adjustment with plasticity
- **Spike History Recording**: Pre/post spike tracking for STDP
- **Eligibility Traces**: Reward-modulated learning support

#### NeuralRegion Class
- **Population Management**: Create and manage neuron populations
- **Synapse Management**: Local connectivity within regions
- **Statistics Collection**: Real-time neural activity metrics
- **Random Connectivity**: Initialize with probabilistic connections

#### NeuralPopulation Class
- **Neuron Management**: Create and manage neuron groups
- **Statistics Collection**: Activity levels, firing rates, membrane potentials
- **Type Control**: Set neuron types for entire populations
- **Bulk Operations**: Batch operations for efficiency

### Neuromodulator System
- **Dopamine**: Reward and reinforcement learning signal
- **Acetylcholine**: Attention and memory consolidation
- **Norepinephrine**: Arousal and vigilance
- **Serotonin**: Mood, impulsivity, and social behavior

### Advanced Python Features

#### Context Managers
- Resource-safe region creation with `with` statements
- Automatic cleanup and error handling

#### Pythonic Operators
- **`__getitem__` and `__setitem__`**: Property-based access (e.g., `neuron.V = -65.0`)
- **`__iter__`**: Iterate over population neurons
- **`__len__`**: Get population size

#### Property-Based Access
- Direct property access for common attributes
- Clean, intuitive API design

#### Vector Conversions
- Convert neural properties to standard Python containers
- Batch vector operations for efficiency

#### Batch Operations
- Vectorized operations for large populations
- Efficient simulation of multiple neurons/synapses

### Example Usage

```python
from pynlm import Neuron, Synapse, NeuralRegion, NeuralPopulation
from pynlm import Dopamine, Acetylcholine, Norepinephrine, Serotonin
from pynlm import NeuronType, SynapseType

# Create neural region
with NeuralRegion(1, "hippocampus") as region:
    # Create neurons using Pythonic API
    neuron1 = Neuron(1)
    neuron1.type = NeuronType.Excitatory
    neuron1.V = -65.0
    neuron1.threshold = -55.0
    
    # Create populations
    population = region.addPopulation(100, NeuronType.Excitatory)
    
    # Simulate using batch operations
    for t in range(1000):
        population.step(t * 0.001)
```

## Building

### Building NLM Core
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Building Python Bindings
```bash
pip install -e .
```

### Building from Source
```bash
python -m pip install --upgrade pip setuptools wheel
python -m pip install scikit-build-core pybind11
python setup.py build_ext --inplace
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Demo
```bash
python demo.py
```

This runs the Python bindings demonstration.

### Running Tests
```bash
cd tests
make test
```

Or use the Python test suite:
```bash
pytest
```

## Project Structure

```
NLM/
├── python/                    # Python bindings and documentation
│   ├── __init__.py           # Python module
│   ├── bindings.cpp          # PyBind11 bindings
│   ├── demo.py               # Usage examples
│   └── easy_usage.md         # Quick start guide
├── CMakeLists.txt
├── README.md
├── docs/                     # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/                      # C++ source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components
│   ├── dynamics/             # Neural dynamics
│   ├── plasticity/           # Plasticity rules
│   ├── development/          # Developmental system
│   ├── neuromodulation/      # Neuromodulators
│   ├── memory/              # Memory systems
│   ├── prediction/           # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization
├── tests/                    # Test suite
└── configs/                  # Configuration files
```

## Python API Documentation

The Python bindings provide the following main classes:

### Neuron
```python
class Neuron:
    def __init__(id, type=NeuronType.Internal, resting_potential=-70.0)
    def get_id() -> NeuronId
    def get_type() -> NeuronType
    def set_type(type: NeuronType)
    # Properties: V, threshold, resting_potential, reset_potential, leak_conductance
    # Methods: stepLIF(), check_threshold(), record_spike()
    # etc.
```

### Synapse
```python
class Synapse:
    def __init__(id, source, destination)
    def get_id() -> SynapseId
    def get_source_neuron() -> NeuronId
    def get_destination_neuron() -> NeuronId
    # Properties: weight, delay, type, efficacy
    # Methods: set_weight(), step()
    # etc.
```

### NeuralRegion
```python
class NeuralRegion:
    def __init__(id, name="")
    def get_id() -> RegionId
    def get_name() -> str
    def set_name(name: str)
    def add_population(size: int, type=NeuronType.Internal) -> PopulationId
    def add_synapse(source, destination, weight=0.0, delay=1) -> SynapseId
    def get_population(id: PopulationId) -> NeuralPopulation
    def get_synapse(id: SynapseId) -> Synapse
    # etc.
```

### NeuralPopulation
```python
class NeuralPopulation:
    def __init__(id, size)
    def get_id() -> PopulationId
    def get_size() -> int
    def get_neuron(index: int) -> Neuron
    def set_neuron_type(type: NeuronType)
    def get_active_count() -> int
    def get_firing_count() -> int
    def step(current_time: float)
    # etc.
```

### Neuromodulators
```python
class Dopamine(Neuromodulator):
    def __init__()
    def signal_reward(reward: float)
    def signal_reward_prediction_error(error: float)

class Acetylcholine(Neuromodulator):
    def __init__()

class Norepinephrine(Neuromodulator):
    def __init__()

class Serotonin(Neuromodulator):
    def __init__()
```

### Utility Functions
```python
# Factory functions
def create_neuron(id: NeuronId, type: NeuronType) -> Neuron
def create_synapse(id: SynapseId, source: NeuronId, destination: NeuronId) -> Synapse
def create_region(id: RegionId, name: str) -> NeuralRegion

# Batch operations
def batch_step_neurons(neurons: List[Neuron], current_time: float)
def batch_step_synapses(synapses: List[Synapse], current_time: float)

# Statistics utilities
def calculate_mean(values: List[float]) -> float
def calculate_variance(values: List[float]) -> float
```

### Available Constants
```python
# Neuron types
NeuronType.Excitatory
NeuronType.Inhibitory
NeuronType.Modulatory
NeuronType.Sensory
NeuronType.Motor
NeuronType.Internal

# Synapse types
SynapseType.Excitatory
SynapseType.Inhibitory
SynapseType.Modulatory
SynapseType.Electrical
SynapseType.GapJunction

# Firing states
FiringState.Resting
FiringState.Active
FiringState.Refractory
FiringState.Inhibited
```

## Advanced Python Features

### Context Managers
```python
# Automatic resource management
with NeuralRegion(1, "my_region") as region:
    # Use region safely
    neuron = region.add_neuron()
    # Resources cleaned up automatically
```

### Pythonic Properties
```python
# Direct property access
neuron.V = -65.0  # Set membrane potential
neuron.threshold = -55.0  # Set threshold
neuron.refractory_period = 10  # Set refractory period

# Read-only properties
potential = neuron.V
firing_rate = neuron.firing_rate
spike_count = neuron.spike_count
```

### Vector Conversions
```python
# Convert neural properties to vectors
potentials = neuron.to_vector()  # [V, threshold, firing_rate]
weights = synapse.to_vector()    # [weight, delay, efficacy]

# Create vectors from arrays
values = [1.0, 2.0, 3.0, 4.0]
```

### Batch Operations
```python
# Process multiple neurons efficiently
neurons = [neuron1, neuron2, neuron3, ...]
batch_step_neurons(neurons, current_time)

# Get statistics for all neurons at once
potentials = batch_get_potential(neurons)
spike_counts = batch_get_spike_counts(neurons)
```

## Examples

### Basic Neural Simulation
```python
from pynlm import NeuralRegion, Neuron, Synapse, NeuronType

# Create neural region
region = NeuralRegion(1, "hippocampus")

# Create neurons
neuron1 = Neuron(1)
neuron1.type = NeuronType.Excitatory
neuron1.V = -65.0

neuron2 = Neuron(2)
neuron2.type = NeuronType.Inhibitory
neuron2.V = -70.0

# Create synapse
synapse = Synapse(1, neuron1.id, neuron2.id)
synapse.weight = 0.5

# Simulate
for t in range(1000):
    neuron1.stepLIF(t * 0.001, 0.001)
    neuron2.stepLIF(t * 0.001, 0.001)
    region.step(t * 0.001)
```

### Population Dynamics
```python
from pynlm import NeuralRegion, NeuralPopulation, NeuronType

# Create population with 1000 neurons
with NeuralRegion(2, "cortex") as region:
    population = region.addPopulation(1000, NeuronType.Excitatory)
    
    # Simulate and track activity
    for t in range(500):
        population.step(t * 0.001)
        
        # Record statistics
        active = population.active_count
        firing = population.firing_count
        activity_level = population.activity_level
```

### Neuromodulation
```python
from pynlm import Dopamine, Acetylcholine, Norepinephrine, Serotonin

# Create neuromodulators
dopamine = Dopamine()
acetylcholine = Acetylcholine()
norepinephrine = Norepinephrine()
serotonin = Serotonin()

# Apply reward signal
dopamine.signal_reward(1.0)

# Set neuromodulation levels
acetylcholine.set_level(0.7)
norepinephrine.set_level(0.5)
serotonin.set_level(0.3)
```

### Pythonic Vector Operations
```python
from pynlm import NeuralRegion, NeuronType

# Create batch of neurons
with NeuralRegion(3, "batch_test") as region:
    neurons = [region.add_neuron(NeuronType.Excitatory) for _ in range(100)]
    
    # Set all neuron potentials using vector operation
    potentials = [-70.0 + (i * 0.1) for i in range(100)]
    for neuron, potential in zip(neurons, potentials):
        neuron.V = potential
    
    # Get statistics using Python comprehensions
    mean_potential = sum(n.V for n in neurons) / len(neurons)
    max_potential = max(n.V for n in neurons)
```

## Testing

### C++ Tests
```bash
cd tests
make test
```

### Python Tests
```bash
pytest
```

### Running All Tests
```bash
# Build first
mkdir build && cd build && cmake .. && make -j4

# Run all tests
make test
pytest
```

## Troubleshooting

### ModuleNotFoundError
If you get "ModuleNotFoundError: No module named 'pynlm'", try:

```bash
# Install in development mode
pip install -e .

# Or install globally
pip install .
```

### Build Errors
If you encounter build errors:

1. Ensure you have a C++ compiler with C++20 support
2. Install dependencies:
```bash
pip install scikit-build-core pybind11
```

3. Check for required headers in include paths
4. Ensure pybind11 is installed:
```bash
pip install pybind11
```

### Performance Issues
For large simulations:

1. Use batch operations instead of individual calls
2. Consider reducing simulation timestep
3. Monitor memory usage with large populations
4. Use checkpointing for long simulations

## Phase 6 Integration Verification

Phase 6 includes comprehensive verification that all neural components integrate correctly:

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Check Python bindings integration
python -c "from pynlm import Neuron; print('Neuron binding works')"
```

## Future Enhancements

The Python bindings will be enhanced with:

1. **Advanced Visualization**: Plotting neural activity
2. **Parallel Processing**: Multi-core simulation capabilities
3. **Machine Learning Integration**: TensorFlow/PyTorch interoperability
4. **Web API**: REST interface for remote access
5. **GUI Tools**: Interactive visualization and control
6. **Distributed Computing**: Multi-machine simulation support

## License

MIT

## Authors

Research project — See docs for scientific background.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
