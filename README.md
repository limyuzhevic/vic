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

## Python Bindings

The NLM project provides Python bindings for interacting with the neural simulation framework:

### Installation

```bash
# Build Python bindings from the project root
cd python
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Available Modules

#### Original API (`pynlm_original`)

For backward compatibility and production use:

```python
import pynlm_original as nlm

# Create a brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
```

#### Enhanced API (`pynlm`)

The enhanced Python bindings provide a more Pythonic interface:

```python
import pynlm as nlm

# Enhanced configuration with dictionary-like interface
config = nlm.Config()
config.update({
    "neuron_count": 1000,
    "region_count": 2,
    "random_seed": 42
})

# Use context manager for automatic lifecycle management
with nlm.Brain(config) as brain:
    results = brain.run_simulation(steps=100)
```

### Features Comparison

| Feature | Original API | Enhanced API |
|---------|-------------|--------------|
| Configuration Interface | `config.set("key", "value")` | `config["key"] = "value"` |
| Resource Management | Manual initialize/reset | Automatic context manager |
| Simulation Control | Basic step-by-step | Built-in simulation runners |
| Error Handling | Basic | Robust with recovery |
| Result Reporting | None | Detailed metrics and statistics |
| Documentation | Minimal | Comprehensive with examples |

### Usage Examples

#### Basic Usage

See `python/easy_usage.md` for comprehensive Python usage examples.

#### Enhanced API Features

The enhanced Python API provides:

1. **Pythonic Configuration Interface**: Dictionary-like syntax for configuration
2. **Context Managers**: Automatic resource management
3. **Simulation Runners**: Step-by-step execution with detailed results
4. **Enhanced Agent Brain**: Better abstraction and usability
5. **Error Handling**: Robust error recovery
6. **Performance Monitoring**: Built-in metrics collection

### Quick Start

```python
import pynlm as nlm

# Create enhanced configuration
config = nlm.Config()
config.update({
    "neuron_count": 500,
    "region_count": 2,
    "random_seed": 42
})

# Create brain with context manager (automatic lifecycle)
with nlm.Brain(config) as brain:
    # Run simulation
    results = brain.run_simulation(steps=100)
    
    # Get statistics
    stats = brain.get_stats()
    print(f"Neurons: {stats['total_neurons']}")
    print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### Documentation

- `python/easy_usage.md`: Beginner-friendly guide
- `python/enhanced_api_guide.md`: Comprehensive enhanced API documentation
- `python/enhanced_examples.py`: Complete working examples

### Building Python Bindings

The Python bindings are built using CMake with pybind11:

```bash
# From the python directory
cd python
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

This will build both the original bindings (`pynlm_original`) and the enhanced bindings (`pynlm`).

### Testing Python Bindings

```bash
# Run Python tests
cd python
python -m pytest tests/ -v

# Run example scripts
python python/enhanced_examples.py
```

## Testing

### Unit Tests

```bash
# From build directory
cd build
ctest
```

### Python Tests

```bash
cd python
python -m pytest tests/ -v
```

## Configuration

### Core Configuration Options

```json
{
    "neuron_count": 1000,                    // Total neurons
    "region_count": 1,                       // Number of brain regions
    "connection_probability": 0.1,            // Connection probability
    "random_seed": 42,                        // Random number seed
    "simulation_timestep": 0.001,             // Simulation time step
    "working_memory_capacity": 100,           // Working memory size
    "episodic_memory_max_episodes": 1000,     // Max stored episodes
    "dopamine_learning_rate": 0.01,           // Neuromodulation strength
    "developmental_duration": 1000,           // Total development time
    "synaptogenesis_rate": 0.0001,           // Structural plasticity rate
    "pruning_rate": 0.00001                  // Memory pruning rate
}
```

### Configuration File Format

The config system supports:
- **Key=value format** (legacy)
- **Command line arguments** (`--key=value`)
- **JSON/YAML** (Phase 2 planned)

Example config file:

```
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
development_enabled = true
reward_modulation_enabled = true
structural_plasticity_enabled = true
curiosity_enabled = true
```

## License

MIT License

## Authors

Research project — See docs for scientific background.
