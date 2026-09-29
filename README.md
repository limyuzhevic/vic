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

## Quick Start

### Installation

```bash
# Install NLM with Python bindings
pip install pynlm

# For development from source
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

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

### Installation

```bash
# Install NLM with Python bindings
pip install pynlm

# For development from source
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Project Improvements

This version of NLM includes significant improvements to the Python API:

### Enhanced Error Handling
- Comprehensive exception handling with specific error types
- Input validation and parameter checking
- Graceful error recovery mechanisms

### Improved Configuration Management
- `validate()` and `validateWithErrors()` methods for configuration validation
- `isComplete()` method to check for required keys
- Type-safe configuration access with `.get()` method
- Safe value setting with type checking

### Convenience Methods
- `createBrainConfig()` - optimized brain configuration
- `createSimpleWorldConfig()` - optimized world configuration
- `stepWithDuration()` - control simulation timing
- `runQuickTest()` - smoke test to verify installation

### Advanced Features
- Parallel processing examples
- Plugin architecture for extensibility
- Visualization integration support
- Enhanced documentation and examples

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/                  # Python bindings
│   ├── bindings.cpp         # PyBind11 bindings
│   ├── CMakeLists.txt
│   └── DOCUMENTS/          # Documentation
│       ├── IMPLEMENTATION_GUIDE.md
│       └── API_REFERENCE.md
├── src/                    # C++ source code
│   ├── brain/              # Neural components (integrated)
│   ├── cognition/          # Cognitive mechanisms (integrated)
│   ├── memory/            # Memory systems (integrated)
│   ├── neuromodulation/     # Neuromodulators (integrated)
│   ├── prediction/         # Prediction systems (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
├── tests/                  # Unit tests
└── configs/                # Configuration files
```

## Building from Source

### Prerequisites
- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+
- pybind11 2.11.0+

### Build Steps

```bash
# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Run tests
ctest --output-on-failure
```

### Install Python Bindings

```bash
# Install Python package
cd python/pip install -e .

# Or install system-wide
pip install .
```

## Running Examples

### Simple Brain Simulation

```python
import pynlm

# Create brain with optimized configuration
config = pynlm.createBrainConfig(neuron_count=1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

### Agent in World

```python
import pynlm

# Setup brain and agent
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent episode complete!")
```

### Configuration Management

```python
import pynlm

# Create optimized configuration
config = pynlm.createBrainConfig(
    neuron_count=1000,
    enable_plasticity=True,
    enable_development=True
)

# Validate configuration
if config.validate():
    print("Configuration is valid!")
    
    # Use configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Configuration has errors")
    
# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config2 = pynlm.importJsonConfig("my_config.cfg")
```

## Testing

### Running Tests

```bash
# Build and run tests
mkdir build
cd build
cmake ..
make -j4
ctest
```

### Individual Test Programs

Some test programs are also available as standalone executables:

- `nlm` - Main simulation executable
- `nlm_phase3_demo` - Phase 3 demonstration
- `nlm_phase4_demo` - Phase 4 demonstration
- `nlm_test` - Unit test suite

## Documentation

### Key Documentation Files

- `easy_usage.md` - Beginner-friendly usage guide
- `HOW_TO_USE.md` - Comprehensive API documentation
- `python/DOCUMENTS/IMPLEMENTATION_GUIDE.md` - Advanced usage and implementation details

### Architecture Documentation

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 implementation audit

## Development Phase

### Phase 5: Performance Optimizations (Complete)

Phase 5 focused on performance optimizations to make NLM more efficient and scalable:

#### Performance Optimizations
- Memory pools for efficient memory allocation
- Event queues for better event handling
- SIMD vectorization for numerical operations
- Parallel processing capabilities
- Checkpoint system for state persistence

### Phase 6: Final Integration (Complete - Summary)

Phase 6 completes the NLM project by integrating all previous phases into a coherent artificial brain system.

## License

MIT

## Authors

Research project — See docs for scientific background.
