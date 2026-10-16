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

### Build with Python Bindings (Recommended)

```bash
# Using scikit-build-core (recommended)
pip install -e .[dev]

# Or build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j4
```

### Build without Python Bindings (C++ only)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=OFF
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Demo

```bash
python -m examples.advanced_usage
```

This runs advanced Python examples demonstrating the enhanced NLM API.

### Running Tests

```bash
# Run C++ tests
cd build
ctest

# Run Python binding tests
pytest tests/test_pynlm_bindings.py -v

# Run all tests
pytest tests/ -v
```

### Interactive Python

```python
import pynlm
import numpy as np

# Create and test basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("NLM Python Bindings Test")
print("=" * 30)
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Version: {pynlm.__version__}")
```

## Python API Features

The NLM Python bindings provide a comprehensive, Pythonic interface to the neural
simulation framework with enhanced documentation, error handling, and convenience
functions.

### Key Improvements

- **Enhanced Documentation**: Comprehensive docstrings and examples
- **Better Error Handling**: Robust error handling with meaningful messages
- **Convenience Functions**: Higher-level APIs for common patterns
- **Type Hints**: Improved type annotations for better IDE support
- **Test Suite**: Comprehensive test coverage for Python bindings
- **Advanced Examples**: Example scripts demonstrating advanced usage
- **Performance Monitoring**: Built-in performance tracking utilities

### Example Usage

```python
import pynlm

# Create advanced brain with custom configuration
brain = pynlm.createBrain(
    config=pynlm.createDefaultConfig(),
    enable_learning=True,
    enable_development=True
)

# Run complex simulation
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enhanced simulation with learning
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    agent.applyRewardModulation(0.5, 0.3)
```

## Advanced Features

### 1. Complete Agent System

- Neural processing with LIF dynamics
- Working memory integration
- Episodic and semantic memory systems
- Prediction and planning modules
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages and structural plasticity

### 2. Enhanced API Design

- Pythonic method names and parameters
- Comprehensive error handling and validation
- Type hints and documentation strings
- Context managers for resource management
- Convenience functions for common patterns

### 3. Performance Optimization

- Vectorized operations where possible
- Memory-efficient data structures
- Performance monitoring tools
- Configurable complexity levels

### 4. Visualization and Analysis

- Network visualization support
- Statistical analysis tools
- Performance profiling
- Data export utilities

## Files and Directories

```
NLM/
├── python/              # Python bindings source
│   ├── bindings.cpp     # Main pybind11 bindings
│   └── CMakeLists.txt   # Build configuration
├── src/                 # C++ source code
├── examples/            # Python examples
├── tests/               # Test suites
├── docs/                # Documentation
├── configs/             # Configuration files
└── .kilo/               # Kilo configuration
```

## Testing

### Python Binding Tests

The test suite includes comprehensive tests for:

- Basic configuration management
- Brain simulation functionality
- World interaction
- Agent brain interface
- Advanced features and integrations
- Error handling and edge cases
- Performance benchmarks

Run Python tests:

```bash
pytest tests/test_pynlm_bindings.py -v --tb=short
```

### C++ Tests

Run C++ tests:

```bash
cd build
ctest -V
```

### Example Tests

Run example scripts:

```bash
python -m examples.advanced_usage
python -m examples.tutorial
```

## Documentation

### Available Documentation

- **README.md** - Project overview and setup instructions
- **docs/API.md** - Complete Python API documentation
- **docs/ARCHITECTURE.md** - System architecture
- **docs/SCIENCE.md** - Scientific background
- **docs/ROADMAP.md** - Development roadmap
- **examples/advanced_usage.py** - Advanced usage examples
- **examples/tutorial.py** - Tutorial for beginners

### Building Documentation

Build documentation:

```bash
sphinx-build -b html docs/ docs/_build/html/
```

## Contributing

### Python Development

1. Install development dependencies:
   ```bash
   pip install -e .[dev]
   ```

2. Run tests:
   ```bash
   pytest tests/ -v
   ```

3. Run linters:
   ```bash
   ruff check python/ examples/ tests/
   ```

4. Format code:
   ```bash
   ruff format python/ examples/ tests/
   ```

### C++ Development

1. Build with tests:
   ```bash
   mkdir build
   cd build
   cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON -DNLM_ENABLE_TESTS=ON
   make -j4
   ```

2. Run tests:
   ```bash
   ctest
   ```

3. Code style:
   ```bash
   clang-format -i src/*.cpp src/**/*.cpp
   ```

## License

MIT License. See LICENSE file for details.

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
