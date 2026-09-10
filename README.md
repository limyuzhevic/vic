# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

This project implements an experimental artificial developmental brain (NLM - Neural Learning Machine) with real spiking neural computation. For quick access to Kilo commands and agent management, see the project documentation.

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Key Features:**
- Real spiking neural computation with Leaky Integrate-and-Fire neurons
- Event-driven spike propagation with synaptic delays
- Dual plasticity mechanisms: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning
- Structural plasticity (synaptogenesis and pruning)
- Memory systems: Working memory, Episodic memory, Associative memory
- Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error
- Cognitive systems: Neural planning, Concept formation, Attention
- Development stages from initial to adult

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

### Key Achievements

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

## Getting Started

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Kilo framework (for agent management and tool integration)

### Building

```bash
# Standard build with CMake
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Kilo-enhanced build
# Check for kilo.json configuration
if [ -f "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_fc69ae8e-2d19-4e42-8951-48ea236075f8/kilo.json" ]; then
    echo "Kilo configuration found - using optimized build settings"
    cmake .. -DKILO_INTEGRATION=1
    make -j4
else
    echo "No kilo.json found - using standard build"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
fi
```

### Running

#### Phase 6 Demo (Integration Test)

```bash
./nlm
./nlm_phase6_demo
```

These run comprehensive integration tests verifying all brain systems are connected and working together.

#### Kilo Agent Management

```bash
# Launch with Kilo agent system
./nlm --with-kilo

# View available Kilo commands
./nlm --help
```

## Project Structure

### Core Components

```
NLM/
├── kilo.json                    # Kilo framework configuration (NEW)
├── CMakeLists.txt                # Modern CMake build system
├── AGENTS.md                    # Kilo agent documentation (NEW)
├── README.md                    # Project documentation
├── docs/                        # Detailed documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                         # Source code
│   ├── core/                    # Core utilities
│   │   ├── Config/              # Configuration management
│   │   ├── Random/              # Random number generation
│   │   ├── Logger/              │ Logging system
│   │   ├── SimulationClock/     # Time management
│   │   └── Types/              # Type definitions
│   ├── brain/                   # Neural components
│   │   ├── Neuron.cpp           # LIF neuron implementation
│   │   ├── Synapse.cpp          # Synaptic connections
│   │   ├── NeuralPopulation.cpp # Neuron groups
│   │   ├── NeuralRegion.cpp     # Brain regions
│   │   └── Brain.cpp            # Central brain controller
│   ├── dynamics/                # Neural dynamics
│   ├── plasticity/              # Plasticity rules
│   ├── development/             # Developmental system
│   ├── neuromodulation/         # Neuromodulators
│   ├── memory/                 # Memory systems
│   ├── cognition/              # Cognitive mechanisms
│   ├── sensory/                 # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/             # Environment interface
│   ├── experiments/             # Experiment framework
│   └── visualization/           # Visualization
│
├── tests/                       # Unit tests
├── configs/                     # Configuration files
│   └── default.cfg
└── python/                      # Python bindings
```

## Project Organization

### 1. Core Architecture

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

### 2. Development Phases

| Phase | Status | Achievements |
|-------|--------|---------------|
| Phase 1 | ✅ Complete | Project skeleton, core types, neural interfaces |
| Phase 2 | ✅ Complete | Real LIF neuron dynamics, STDP, structural plasticity |
| Phase 3 | ✅ Complete | World interaction, sensory input, reward prediction error |
| Phase 4 | ✅ Complete | Neural prediction, working memory, episodic memory |
| Phase 5 | ✅ Complete | Performance optimizations, memory pools, checkpoint system |
| Phase 6 | ✅ Complete | All systems integrated into coherent brain loop |

### 3. Technical Specifications

#### Neural Computation
- **Model**: Leaky Integrate-and-Fire (LIF) neurons
- **Dynamics**: Event-driven spike propagation with synaptic delays
- **Scale**: Up to 1000 neurons in current configurations
- **Plasticity**: Dual mechanisms (STDP and Hebbian)

#### Performance Optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Sparse connectivity optimization

#### Memory Systems
- **Working Memory**: Persistent activity with competition
- **Episodic Memory**: Experience encoding and replay
- **Semantic Memory**: Pattern associations
- **Procedural Memory**: Motor skill learning
- **Associative Memory**: Hebbian pattern associations

#### Neuromodulation
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration drive
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signals

## Configuration Management

### Core Configuration

NLM uses a JSON-based configuration system that supports:
- File-based configuration (`configs/default.cfg`)
- Command-line argument overrides
- Runtime configuration changes
- Configuration validation and summaries

### Key Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64 | 42 | Random number generator seed |
| `simulation_timestep` | double | 0.001 | Simulation time step (seconds) |
| `neuron_count` | int64 | 1000 | Number of neurons in brain |
| `region_count` | int64 | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |

### Advanced Configuration

For advanced users and Kilo integration, constants are defined in `src/core/Constants.hpp`:

```cpp
namespace nlm {
    // Neural constants
    constexpr size_t SENSE_NEUTRAL_SIZE = 256;  // Vision input size
    constexpr float SENSE_NOVELTY_DECAY = 0.99f;
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Development constants
    constexpr double DEVELOPMENT_INITIAL_TIME = 60.0;
    constexpr double DEVELOPMENT_CRITICAL_TIME = 300.0;
    
    // Neuromodulation constants
    constexpr float NEUROMODULATION_DOPAMINE_POSITIVE = 1.0f;
    constexpr float NEUROMODULATION_DOPAMINE_NEGATIVE = -1.0f;
    
    // Memory system constants
    constexpr size_t MEMORY_CAPACITY = 10000;
    constexpr float MEMORY_DECAY = 0.99f;
}
```

## Testing and Validation

### Integration Tests

NLM includes comprehensive integration tests in `Phase 6`:
- Memory system connectivity
- Neuromodulation integration
- Prediction system integration
- Developmental stage transitions
- Checkpoint save/load functionality
- Replay and consolidation

### Unit Tests

The project includes unit tests for individual components:
- `tests/test_neuron.cpp` - Neuron LIF dynamics
- `tests/test_synapse.cpp` - Synaptic plasticity
- `tests/test_stdp.cpp` - STDP learning rule
- `tests/test_config.cpp` - Configuration system
- `tests/test_clock.cpp` - Simulation clock
- `tests/test_random.cpp` - Random number generation

### Running Tests

```bash
mkdir build
cd build
cmake ..
make
ctest --output-on-failure
```

## Kilo Integration

### Overview

This project is designed to work with the Kilo framework for agent management and tool integration. Key features include:

### Kilo Configuration

The `kilo.json` file (when created) provides:
- Agent definitions for the NLM brain
- Tool integration points
- Command configurations
- Workflow definitions

### Agent Management

Agents can be managed through Kilo to:
- Run experiments
- Profile performance
- Monitor system state
- Generate reports

### Tool Integration

Kilo tools can interface with NLM for:
- Configuration management
- Performance analysis
- Simulation control
- Result visualization

## Advanced Usage

### For Advanced Users

This project includes several features designed for advanced users:

#### 1. Performance Profiling

NLM includes performance profiling capabilities in `src/performance/`:
- Sparse connectivity optimization
- SIMD vectorization
- Parallel processing
- Memory pool management
- Event queue optimization

#### 2. Development System

The developmental system supports:
- Stage-based learning (Initial → Critical Period → Maturation → Adult)
- Structural plasticity changes with development
- Synaptogenesis and pruning rates
- Neural excitability modulation

#### 3. Neuromodulation Integration

Advanced neuromodulation features include:
- Dopamine-based reward prediction error
- Curiosity-driven exploration
- Novelty detection
- Integrated prediction error

### Extending NLM

#### Adding New Plasticity Rules

```cpp
// Example: Create a new plasticity rule
class MyPlasticityRule : public PlasticityRule {
public:
    void apply(float preSpikeTime, float postSpikeTime, 
               float& weightChange) override {
        // Implement custom learning rule
    }
};
```

#### Adding New Memory Systems

```cpp
// Example: Create a new memory system
class CustomMemorySystem : public MemorySystem {
public:
    bool store(const std::string& key, const Vector& data) override {
        // Custom storage implementation
    }
    
    bool retrieve(const std::string& key, Vector& data) override {
        // Custom retrieval implementation
    }
};
```

#### Performance Optimization

For performance-critical applications:
- Use SIMD instructions
- Implement sparse connectivity
- Utilize memory pools
- Optimize event queues
- Profile with `perf` or `gdb`

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Contributing

This is a research project. Contributions should focus on:
- Scientific accuracy
- Technical improvements
- Documentation enhancements
- Bug fixes
- Performance optimizations

```
Code of Conduct: https://github.com/nims-project/nlm/blob/main/CODE_OF_CONDUCT.md
Contributing Guidelines: https://github.com/nims-project/nlm/blob/main/CONTRIBUTING.md
```

## Support and Contact

For questions, issues, or collaboration:

- **GitHub Issues**: https://github.com/nims-project/nlm/issues
- **Documentation**: https://nims-project.github.io/nlm/
- **Research Papers**: Check the `docs/` directory for publications
- **Project Status**: Phase 6 - Final Integration Complete

---

**Last Updated**: September 10, 2026
**Phase**: 6 (Final Integration)
**Status**: Research and Development
