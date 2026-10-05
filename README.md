# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

The easiest way to get started with NLM is using the command-line interface:

```bash
# Run a quick demo
./nlm_cli.py demo --steps 50

# Run a full experiment with custom settings
./nlm_cli.py experiment --neurons 200 --regions 2 --steps 1000

# Save a brain configuration
./nlm_cli.py create --neurons 500 --regions 1 --save my_brain.bin

# Load and analyze a saved brain
./nlm_cli.py analyze my_brain.bin

# Enter interactive mode
./nlm_cli.py interactive
```

## Python Usage

NLM also provides a comprehensive Python API. Here are some common patterns:

```python
import pynlm

# Create a complete demo system
system = pynlm.createDemoSystem()
brain = system["brain"]
world = system["world"]
agent = system["agent"]

# Configure and initialize
pynlm.configure_brain(brain, neuronCount=200, regionCount=2)
brain.initialize()
agent.initialize(world)
pynlm.enable_all_features(agent)

# Run simulation
pynlm.run_simulation(brain, world, agent, 500)

# Get comprehensive statistics
stats = pynlm.get_network_stats(brain)
for key, value in stats.items():
    print(f"{key}: {value}")
```

## Building

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8 or higher

### Build Instructions

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install -e .

# Build and install CLI tool
cd build
make nlm_cli
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Command Line Interface

```bash
# Run demo
./nlm_cli.py demo

# Run experiment with custom settings
./nlm_cli.py experiment --neurons 200 --regions 2 --steps 1000

# Save brain state
./nlm_cli.py create --neurons 500 --regions 1 --save my_brain.bin

# Load brain state
./nlm_cli.py load --neurons 100 --regions 1 --load my_brain.bin

# Analyze brain file
./nlm_cli.py analyze my_brain.bin

# Interactive mode
./nlm_cli.py interactive
```

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
│   └── PHASE6_FINAL_AUDIT.md
├── src/                    # Source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
├── python/                 # Python bindings
│   └── bindings.cpp
├── nlm_cli.py             # Command Line Interface
├── easy_usage.md           # Easy usage guide
├── pyproject.toml         # Python package configuration
├── tests/                 # Test suite
└── configs/               # Configuration files
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
- Neural attention
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