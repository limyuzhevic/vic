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

### C++ Core Library
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON=ON
make pynlm_bindings
```

The Python bindings will be built as `pynlm.so` in the `build/python/` directory.

## Running

### Phase 2: Basic Neural Computation Demo
```bash
./nlm
```

### Phase 3: Learning Environment Demo
```bash
./nlm_phase3_demo
```

### Phase 4: Social Learning Demo
```bash
./nlm_phase4_demo
```

### Phase 5: Performance Optimized Demo
```bash
./nlm_phase5_demo
```

### Phase 6: Final Integration Test
```bash
./nlm_phase6_demo
```

### Phase 6: Integrated Experiment
```bash
./nlm_phase6_integrated
```

## Python Usage Examples

### Basic Brain Simulation
```python
from pynlm import createBrain, createSimpleWorld, createDefaultConfig

# Create a brain with configuration
config = createDefaultConfig()
brain = createBrain(config)

# Initialize brain
brain.initialize()

# Create a simple world
world = createSimpleWorld()
world.configure(100, 100, 20, 20)
world.setAgentStart(50, 50)

# Run a simple simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input through brain
    # (In practice, use AgentBrain for this)
    
    # Update world
    world.update(0.001)

brain.logStatus()
```

### Advanced Usage with Performance Monitoring
```python
from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld,
    createPerformanceMonitor, SimulationHelper
)

# Create brain and monitor
config = createOptimizedConfig({
    "NLM_RANDOM_SEED": "12345",
    "NLM_NEURON_COUNT": "1000"
})

brain = createBrain(config)
monitor = createPerformanceMonitor()
brain.initialize()

# Create simulation helper
helper = brain.createSimulationHelper()

# Run episode and collect statistics
inputs = [createSimpleVisionInput()] * 50
stats = helper.runAndCollectStats(50, inputs)
print(f"Simulation stats: {stats}")

# Get performance metrics
print(f"Average time per step: {monitor.getAverageTimePerStep()} ms")
print(f"Total steps: {monitor.getStepCount()}")

# Validate configuration
errors = validateConfig(config)
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid!")
```

### NumPy Integration
```python
import numpy as np
from pynlm import createSimpleVisionInput

# Create vision input and convert to NumPy
vision = createSimpleVisionInput(64, 64, 3)
numpy_array = vision.to_numpy()

print(f"Vision array shape: {numpy_array.shape}")
print(f"Vision array dtype: {numpy_array.dtype}")

# Create new vision from NumPy array
vision2 = createSimpleVisionInput()
vision2.setData(numpy_array)
```

### Error Handling
```python
from pynlm import createBrain, createDefaultConfig, NLMError, check, safeExecute

try:
    config = createDefaultConfig()
    brain = createBrain(config)
    
    # Validate configuration
    check(config.has("neuron_count"), "Configuration must have neuron_count")
    
    # Safely execute operations
    safeExecute(lambda: brain.initialize(), "Brain initialization failed")
    
    brain.logStatus()
    
except NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Documentation and Help
```python
from pynlm import show_docs

# Display comprehensive documentation
docs = show_docs()
print(docs)
```

## Project Structure

```
NLM/
├── Python Bindings:
│   ├── python/              # Python bindings directory
│   │   ├── bindings.cpp     # Enhanced C++ bindings
│   │   └── CMakeLists.txt   # Python build system
│   └── examples/            # Example scripts (future)
│
├── C++ Core Library:
│   ├── src/               # Source directory
│   │   ├── core/          # Core utilities
│   │   ├── brain/         # Neural components
│   │   ├── memory/        # Memory systems
│   │   ├── cognition/     # Cognitive mechanisms
│   │   ├── sensory/       # Sensory processing
│   │   ├── motor/         # Motor system
│   │   ├── prediction/    # Prediction systems
│   │   ├── neuromodulation/# Neuromodulators
│   │   ├── development/   # Developmental system
│   │   ├── plasticity/    # Plasticity rules
│   │   ├── world/         # Environment interface
│   │   └── experiments/   # Experiment framework
│   └── main.cpp          # Main executable
│
├── Tests:
│   ├── tests/            # Test directory
│   │   ├── test_*.cpp     # Unit tests
│   │   └── test_*.py      # Python bindings tests
│
├── Documentation:
│   ├── README.md         # Main documentation
│   ├── HOW_TO_USE.md     # Usage guide
│   └── easy_usage.md     # Quick start guide
│
└── Config:
    └── configs/          # Configuration files
```

## Features

### Enhanced Python Bindings

#### Core Classes
- **Brain**: Central neural simulation with performance monitoring
- **Config**: Configuration management with validation
- **AgentBrain**: Agent-brain interface for world interaction
- **SimpleWorld**: 2D world simulation with physics
- **Vision/Audio**: Sensory input with NumPy integration
- **Action**: Motor action representation
- **SensoryPercept**: Sensory perception data

#### New Features
- **Performance Monitoring**: Built-in timing and memory tracking
- **Simulation Helpers**: Advanced simulation control utilities
- **Error Handling**: Comprehensive exception handling
- **Memory Management**: Efficient memory conversion utilities
- **Configuration Validation**: Configuration validation and optimization
- **NumPy Integration**: Seamless NumPy array support
- **Debugging Tools**: Network visualization and debug utilities

#### Advanced APIs
- **Optimized Configuration**: Environment-based configuration
- **Memory Snapshots**: Neural state backup and comparison
- **Safe Execution**: Wrapper functions with error handling
- **Documentation Help**: Comprehensive documentation access

### C++ Code Quality Improvements

#### Performance Optimizations
- **Optimized Build Flags**: -O3 -march=native -ffast-math
- **Memory Pooling**: Efficient memory allocation
- **SIMD Vectorization**: Parallel processing capabilities
- **Cache Optimization**: Improved data locality

#### Code Structure
- **Modular Design**: Well-separated concerns
- **Type Safety**: Strong typing with validation
- **Error Handling**: Comprehensive error management
- **Documentation**: Doxygen-style comments
- **Memory Safety**: Smart pointer usage throughout

#### Validation and Safety
- **Configuration Validation**: Input range checking
- **Memory Bounds**: Bounds checking in critical sections
- **Exception Safety**: Strong exception guarantee where needed
- **Thread Safety**: Thread-safe data structures where appropriate

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

## Acknowledgments

This project builds on decades of research in computational neuroscience, artificial intelligence, and neural systems. Special thanks to the open-source community for contributing tools and libraries used in this project.
