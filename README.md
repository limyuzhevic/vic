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
## Quick Start

### Building (Recommended Method)

The easiest way to build and install NLM is using the modern Python packaging system:

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install (this will compile all C++ code and create the Python bindings)
pip install .

# Or install in development mode for easier editing
pip install -e .
```

### Building from C++ Source

If you want to build just the C++ components or build with custom settings:

```bash
# Navigate to the project directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build and install the Python module
pip install python/
```

### Running the Project

After installation, you can run NLM demonstrations:

```bash
# Run the main C++ demo
./nlm

# Or the Phase 6 integration demo
./nlm_phase6_demo
```

Or use the Python bindings:

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Project Structure

NLM has a clean, modular structure:

```
NLM/
├── src/                # All C++ source code
│   ├── core/           # Core utilities and base classes
│   ├── brain/          # Neural computation components
│   ├── agent/          # Agent brain interface
│   ├── world/          # World simulation
│   └── ...             # Other modules
├── python/             # Python bindings
│   ├── bindings.cpp    # Pybind11 bindings
│   └── CMakeLists.txt  # Python module build
├── tests/             # Unit tests
├── configs/           # Configuration files
└── docs/              # Documentation
```

## Key Components

### Core Brain Systems

- **Neurons**: Leaky Integrate-and-Fire (LIF) models with event-driven dynamics
- **Synapses**: Excitatory, inhibitory, and modulatory connections with STDP
- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Development**: Plasticity that changes with brain age

### Agent-World Interface

- **AgentBrain**: Connects brain activity to world actions
- **SimpleWorld**: 2D environment with sensory input and motor output
- **SensoryPercept**: Multimodal input (vision, touch, internal signals)
- **Action**: Motor commands with parameters

### Advanced Features

- **Prediction System**: Forward models and prediction errors
- **Cognitive Systems**: Planning, concept formation, attention
- **Structural Plasticity**: Synaptogenesis and pruning
- **Curiosity & Exploration**: Drives exploration of novel experiences

## Python API Highlights

### Basic Usage

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.4f}")
```

### Advanced Usage

```python
import pynlm

# Create agent in world environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complete episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Configuration Management

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.005)

# Save and load configurations
config.saveToFile("my_config.json")
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

## Development Setup

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or Visual Studio

### Building with Different Configurations

#### Debug Build

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Installation

```bash
# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install
```

#### Python Development

```bash
# Install Python dependencies
pip install -r requirements-dev.txt

# Build Python module in development mode
pip install -e .

# Run tests
pytest test_pynlm.py
```

## Testing

### Unit Tests

Run the C++ unit tests:

```bash
cd build
ctest --output-on-failure
```

### Python Tests

Run the Python bindings test suite:

```bash
python test_pynlm.py
```

### Integration Tests

The project includes demonstration executables:

```bash
# Phase 3 demonstration (basic neural computation)
./nlm_phase3_demo

# Phase 4 demonstration (advanced cognition)
./nlm_phase4_demo

# Phase 6 demonstration (complete integration)
./nlm_phase6_demo
```

## Performance Considerations

### C++ Performance

- Event-driven spike propagation for efficient simulation
- SIMD optimizations for neural computations
- Parallel processing for large networks
- Memory pools for reduced allocation overhead

### Python Interface

- Minimal overhead through pybind11
- Direct access to C++ data structures
- No Python GIL limitations for core computations

## Contributing

### Building from Source

1. Clone the repository
2. Install dependencies: `pip install pybind11 scikit-build-core pytest numpy`
3. Build: `pip install .`
4. Run tests: `python test_pynlm.py` and `cd build && ctest`

### Adding New Features

NLM follows a modular architecture. New features should be added as separate modules in:
- `src/` for new C++ components
- `python/` for Python bindings
- `docs/` for documentation

### Python API Improvements

When adding Python functionality:
- Maintain consistency with existing API patterns
- Add comprehensive docstrings and examples
- Include test cases in `test_pynlm.py`
- Update documentation in `easy_usage.md` and `HOW_TO_USE.md`

## License

MIT License. See LICENSE file for details.

## Support

For questions and issues:
1. Check documentation in `docs/`
2. Read `easy_usage.md` for quick start guide
3. Read `HOW_TO_USE.md` for comprehensive API reference
4. Search existing issues and examples

The NLM project welcomes contributions and feedback from the research community!

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
