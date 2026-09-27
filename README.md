# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is **NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
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
├── python/                    # Python interface
│   ├── __init__.py
│   ├── __pycache__/
│   ├── bindings.cpp
│   ├── nlm_api.py
│   ├── nlm_documentation.md
│   └── processors.py
├── src/                       # C++ source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Neural components (integrated)
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity rules
│   ├── development/           # Developmental system
│   ├── neuromodulation/       # Neuromodulators (integrated)
│   ├── memory/                # Memory systems (integrated)
│   ├── prediction/            # Prediction systems (integrated)
│   ├── cognition/             # Cognitive mechanisms (integrated)
│   ├── sensory/               # Sensory processing
│   ├── motor/                 # Motor system
│   ├── environment/           # Environment interface
│   ├── experiments/           # Experiment framework (Phase 6)
│   └── visualization/         # Visualization
├── tests/                     # Test suite
├── configs/                   # Configuration files
└── examples/                  # Example code
    ├── basic_training.cpp
    ├── config_validation_example.cpp
```

## Python Interface Highlights

### High-Level API

The NLM Python API provides two interfaces for different user needs:

#### Low-Level API (pynlm module)
- Direct bindings to C++ classes
- Maximum performance and feature access
- Detailed control over brain parameters

#### High-Level API (nlm module)
- Simplified classes for common operations
- Better documentation and error handling
- Intuitive convenience functions

### Key Python Classes

#### NLMBrain
A simplified brain interface for common neural simulation operations.

```python
import pynlm as nlm

# Create a simple brain
brain = nlm.NLMBrain(neuron_count=1000, complexity=0.7)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    firing_rate = brain.get_average_firing_rate()
    active = brain.get_active_neurons()
    print(f"Step {step}: Firing rate={firing_rate:.3f}, Active neurons={active}")
```

#### NLMExperiment
High-level experiment runner for complete simulation setups.

```python
import pynlm as nlm

# Create and run an experiment
experiment = nlm.NLMExperiment.create_standard(world_size=(20, 20))
results = experiment.run(num_steps=1000)

# Access results
print(f"Final firing rate: {results.metrics['final_firing_rate']:.3f}")
print(f"Total spikes: {results.metrics['final_spikes']}")
```

### Convenience Functions

#### Basic Usage
```python
import pynlm as nlm

# Quick test
results = nlm.quick_test(mode='connectivity', steps=500)
print(f"Test passed: {results['passed']}")

# Create standard experiment
experiment = nlm.NLMExperiment.create_standard(world_size=(15, 15))
brain = experiment['brain']
world = experiment['world']
agent = experiment['agent']

# Enable learning features
experiment.enable_features(reward_modulation=True, curiosity=True)

# Train agent
training_results = nlm.train_agent(
    world, brain, episodes=100,
    reward_function=lambda w, b, a, s: 1.0,
    stop_criteria={'max_steps_per_episode': 200}
)

print(f"Training completed: {training_results['episodes_completed']} episodes")
print(f"Average reward: {training_results['average_reward']:.3f}")
```

## Core Documentation

The NLM project includes comprehensive documentation covering all aspects of the system:

### Core Documentation Files

- **PYTHON_API_GUIDE.md**: Complete Python API documentation with examples
- **TRAINING_GUIDE.md**: Training module, patterns, and optimization guide
- **PERFORMANCE_GUIDE.md**: Performance optimization techniques and benchmarking

### Examples and Tutorials

#### Python Examples
- **examples/getting_started.py**: Complete beginner example
- **examples/basic_training.py**: Basic training with monitoring
- **examples/advanced_training.py**: Advanced training with callbacks
- **examples/performance_benchmarking.py**: Performance measurement and optimization
- **examples/experiment_tracking.py**: Reproducible experiment tracking

#### C++ Examples
- **examples/basic_training.cpp**: Basic training implementation
- **examples/advanced_training.cpp**: Advanced training with features
- **examples/performance_benchmarking.cpp**: C++ performance benchmarking
- **examples/experiment_tracking.cpp**: Experiment tracking in C++

## Scientific Background

NLM is a research project investigating computational brain-like systems. We make **NO claims** that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does **NOT** claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 **DOES** investigate:
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

## Performance Optimizations

NLM includes advanced performance optimizations:

- **Memory pools**: Zero-allocation memory management
- **SIMD optimization**: Vectorized operations for neural computations
- **Cache optimization**: Cache-friendly data layouts
- **Parallel processing**: Multi-core optimizations
- **Performance monitoring**: Real-time profiling

## Key Features

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations

### Neuromodulation
- Dopamine for reward prediction error
- Curiosity for intrinsic motivation
- Novelty detection for exploration

### Learning Mechanisms
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity
- Developmental plasticity

### Cognitive Abilities
- Neural prediction systems
- Concept formation
- Attention mechanisms
- Action planning

### Training and Optimization
- Reinforcement learning support
- Supervised learning patterns
- Unsupervised learning capabilities
- Hybrid training approaches
- Hyperparameter optimization
- Experiment tracking

## Getting Started

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Quick Start

1. **Build from source**:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings**:
   ```bash
   pip install .
   ```

3. **Run integration test**:
   ```bash
   ./nlm_phase6_demo
   ```

4. **Try Python examples**:
   ```bash
   python examples/getting_started.py
   ```

## Documentation Access

Complete documentation is available in the `docs/` directory:

- **ARCHITECTURE.md**: System architecture overview
- **SCIENCE.md**: Scientific background and principles
- **ROADMAP.md**: Development roadmap and future plans
- **EXPERIMENTS.md**: Experiment descriptions and protocols
- **PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit

Python-specific documentation is available in the `python/` directory:

- **nlm_documentation.md**: Complete API documentation
- **nlm_api.py**: Convenience functions and utilities

## Support and Community

For questions, issues, or feature requests:

- Check the documentation in the `docs/` and `python/` directories
- Review existing examples for usage patterns
- Consult the scientific papers and publications linked in the documentation

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Version Information

- **Current Phase**: Phase 6 (Final Integration)
- **Python API Version**: 1.0.0
- **Documentation Version**: 1.0.0
- **Last Updated**: 2026-09-27

## Quick Reference Commands

| Command | Description |
|---------|-------------|
| `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4` | Build from source |
| `pip install .` | Install Python bindings |
| `./nlm_phase6_demo` | Run integration test |
| `python examples/getting_started.py` | Run basic Python example |
| `python -c "import pynlm as nlm; brain = nlm.NLMBrain(); brain.initialize()"` | Quick Python test |

The NLM framework provides a comprehensive, extensible platform for researching artificial developmental systems with brain-like learning and cognition capabilities.