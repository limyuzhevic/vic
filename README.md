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

## Python Bindings

### Overview

NLM includes Python bindings through the **pynlm** package, which provides a high-performance Python interface to the C++ neural simulation framework. These bindings enable Python developers to create, simulate, and analyze neural networks without needing to write low-level C++ code.

### Installation

#### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.16 or higher

#### Installation Options

**Option 1: Using pip (Recommended)**

```bash
pip install pynlm
```

This will automatically build and install the Python bindings from the source.

**Option 2: From Source**

```bash
# Clone the repository
 git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build and install
 pip install -e .
```

**Option 3: Build Python Bindings Separately**

```bash
mkdir build
cd build
cmake ..
make pynlm
pip install dist/pynlm-*.whl
```

### Python API Examples

#### Creating a Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.Brain()
brain.initialize()

# Create brain with custom configuration
config = pynlm.Config()
config.set("neuron_count", 500)
config.set("region_count", 2)
brain = pynlm.Brain(config)
brain.initialize()
```

#### Running Simulation

```python
import pynlm
import time

# Create brain
brain = pynlm.Brain()
brain.initialize()

# Run simulation for 1000 steps
start_time = time.time()
for step in range(1000):
    brain.step(step, step * 0.001)

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Working with Memory Systems

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

if working_memory:
    print(f"Active memory traces: {working_memory.getActiveTraces()}")
    
if episodic_memory:
    print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")
```

#### Neuromodulation

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

print(f"Dopamine level: {agent.getNeuromodulationLevel():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
```

### Build Options

#### Custom Build Configuration

The Python bindings can be built with different optimization options:

```bash
# Debug build (with symbols and assertions)
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
pip install dist/pynlm-*.whl

# Release build (optimized)
mkdir build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j4
pip install dist/pynlm-*.whl

# Build with specific features disabled
mkdir build_minimal
cd build_minimal
cmake .. -DNLM_ENABLE_VISUALIZATION=OFF -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j4
pip install dist/pynlm-*.whl
```

#### Environment Variables

- `NLM_BUILD_TYPE`: Set to `Debug`, `Release`, or `RelWithDebInfo`
- `NLM_PYTHON_EXECUTABLE`: Path to Python interpreter
- `NLM_CXX_COMPILER`: Path to C++ compiler

### Development and Testing

#### Running Tests

```bash
# Install test dependencies
pip install pytest numpy

# Run unit tests
pytest tests/

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_neuron.py
pytest tests/Phase6IntegrationTests.py
```

#### Performance Benchmarking

```python
import pynlm
import timeit

# Benchmark brain creation
creation_time = timeit.timeit(
    lambda: pynlm.Brain(), 
    number=10
)
print(f"Average brain creation time: {creation_time/10:.3f} seconds")

# Benchmark simulation steps
brain = pynlm.Brain()
brain.initialize()

step_time = timeit.timeit(
    lambda: brain.step(0, 0.0),
    number=100
)
print(f"Average step time: {step_time/100:.6f} seconds")
```

### Package Structure

The pynlm package provides the following main modules:

#### Core Module

- `pynlm.Brain`: Main brain class for neural computation
- `pynlm.Config`: Configuration management
- `pynlm.Logger`: Logging functionality
- `pynlm.SimulationClock`: Time management

#### Brain Components

- `pynlm.neural_region`: Neural regions and populations
- `pynlm.neuron`: Individual neurons and their dynamics
- `pynlm.synapse`: Synaptic connections and plasticity

#### Memory Systems

- `pynlm.working_memory`: Working memory for active traces
- `pynlm.episodic_memory`: Episodic memory for experience storage
- `pynlm.associative_memory`: Pattern association memory

#### Neuromodulation

- `pynlm.dopamine`: Reward-based neuromodulation
- `pynlm.curiosity`: Exploration motivation
- `pynlm.novelty`: Novelty detection
- `pynlm.prediction_error`: Prediction error signals

#### Sensory and Motor

- `pynlm.sensory`: Sensory processing and input
- `pynlm.motor`: Motor command generation and output
- `pynlm.environment`: Environment interaction

### Performance Considerations

1. **Memory Efficiency**: The bindings use zero-copy data structures where possible
2. **Speed**: C++ implementation ensures high-performance neural simulation
3. **Scalability**: Can handle thousands of neurons efficiently
4. **Multi-threading**: Some components support parallel processing

### Troubleshooting

#### Common Issues

**Import Error**

```bash
pip: cannot import name 'pynlm' from 'nlm'
```

Solution: Ensure the package is installed correctly:
```bash
pip install --force-reinstall pynlm
```

**Build Failures**

```bash
cmake: error: Unable to find Pybind11
```

Solution: Install pybind11 development package:
```bash
sudo apt-get install pybind11-dev  # Ubuntu/Debian
brew install pybind11  # macOS
```

**Missing Dependencies**

```python
ImportError: libnlm_core.so: cannot open shared object file
```

Solution: Ensure the shared library is in the library path:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(python -c "import pynlm; print(pynlm.__path__[0])")
```

### Extending the Python API

To add new Python bindings or extend existing ones:

1. Add new Python module in the `python/` directory
2. Update `pyproject.toml` with new dependencies
3. Use `pybind11` to create the bindings
4. Add tests in the `tests/` directory

Example of creating a new Python module:

```python
# python/my_module.py
import pynlm

class MyBrain(pynlm.Brain):
    def __init__(self, config=None):
        super().__init__(config)
        self.custom_data = {}
    
    def add_custom_feature(self, key, value):
        self.custom_data[key] = value
```

### Further Reading

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **API Reference**: Generated documentation available after installation
- **Examples**: Check the `examples/` directory for more use cases

### License

MIT

### Authors

Research project — See docs for scientific background.

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
