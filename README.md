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

## Building and Installation

### Prerequisites
- CMake 3.16 or higher
- C++20 compatible compiler (GCC 11+, Clang 14+, MSVC 19+)
- Python 3.8 or higher (for Python bindings)

### C++ Installation

1. **Standard Build** (Recommended):
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

2. **Install the NLM library**:
   ```bash
   cd build
   make install
   ```
   This will install NLM to `/usr/local` (or `%ProgramFiles%\NLM` on Windows)

3. **Development Build**:
   ```bash
   mkdir build-dev
cd build-dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 -Wall"
make -j$(nproc)
```

### Python Bindings

The NLM includes Python bindings via scikit-build-core:

1. **Development Installation** (editable, recommended):
   ```bash
   pip install -e .[dev]
   ```

2. **Standard Installation**:
   ```bash
   pip install .
   ```

3. **Build Python bindings separately**:
   ```bash
   pip install scikit-build-core pybind11
   python -m pip install --verbose -e .
   ```

## Running

### Phase 6 Demo (Integration Test)

The primary demo runs a comprehensive integration test verifying all brain systems are connected:

```bash
# In the build directory
./nlm_phase6_demo
```

### Additional Demos

- **Phase 3 Demo** (`nlm_phase3_demo`): Basic world interaction and learning
- **Phase 4 Demo** (`nlm_phase4_demo`): Advanced cognitive capabilities
- **Phase 5/6 Integration** (`nlm_phase5_demo`, `nlm_phase6_demo`): Full system integration

### Python Usage Examples

```python
import pynlm
import numpy as np

# Create a simple brain
config = pynlm.Config()
config.set("neuron_count", 100)
config.set("region_count", 1)

brain = pynlm.Brain(config)
brain.initialize()

# Run a simulation for 100 steps
for step in range(100):
    brain.step(step, step * 0.001)

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Quick Start Guide

For researchers and developers new to NLM:

1. **Install dependencies**:
   ```bash
   sudo apt-get update && sudo apt-get install -y cmake build-essential python3-dev
   pip install scikit-build-core pybind11 numpy pytest
   ```

2. **Clone and build**:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Run the integration demo**:
   ```bash
   ./nlm_phase6_demo
   ```

4. **Try Python bindings**:
   ```python
   import pynlm
   brain = pynlm.Brain(pynlm.Config())
   ```

## Usage Scenarios

### 1. Research and Simulation
Perfect for studying neural development, learning, and cognition:
- Run experiments with different brain configurations
- Save and load brain states
- Analyze neural activity patterns

### 2. Integration Testing
Use the Phase 6 demo to verify all systems work together:
- Memory systems integration
- Neuromodulation integration
- Prediction system integration
- Development system integration

### 3. Educational Demonstrations
The NLM demo mode is ideal for:
- Teaching neural network concepts
- Demonstrating brain-like behavior
- Interactive neuroscience education

## Configuration Files

NLM uses simple `.cfg` files for configuration. The default configuration is in `configs/default.cfg`.

### Example Custom Configuration

```bash
# Create a custom config
cat > my_config.cfg << EOF
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.2
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.04
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00005
dopamine_baseline = 0.1
reward_discount_factor = 0.95
environment_name = GridWorld
environment_width = 20
environment_height = 20
log_level = INFO
log_to_file = true
log_filename = my_experiment.log
max_simulation_steps = 50000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
EOF

# Use custom config
./nlm --config=my_config.cfg
```

## Troubleshooting

### Build Issues

**CMake not found**:
```bash
# On Ubuntu/Debian
sudo apt-get install cmake
# On macOS with Homebrew
brew install cmake
# On Windows
choco install cmake
```

**Python bindings not installing**:
```bash
# Try building with verbose output
python -m pip install --verbose -e .

# Or clean install
pip uninstall -y pynlm
pip install scikit-build-core pybind11
pip install --verbose -e .
```

**Memory issues during build**:
```bash
# Reduce build parallelism
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j2

# Or increase system memory
# For build environments, consider using larger instances
```

### Runtime Issues

**Brain initialization fails**:
- Check that config values are valid (positive neuron count, etc.)
- Ensure configuration file format is correct
- Verify available system resources

**Python bindings import errors**:
- Ensure pynlm is installed in the current Python environment
- Check for missing dependencies
- Try importing with verbose mode: `python -c "import pynlm; print(pynlm.__file__)"`

## Contributing

This project welcomes contributions! For major changes:

1. **Fork the repository**
2. **Create a feature branch**
3. **Make your changes**
4. **Test thoroughly**
5. **Submit a pull request**

### Code Style

- Use C++20 features and RAII
- Follow existing naming conventions
- Add comments for complex algorithms
- Write unit tests for new code

### Testing

Run the existing tests:
```bash
mkdir build
cd build
cmake ..
make -j4
ctest -V
```

### Documentation

- Update this README if you change build/install process
- Add docstrings to new public APIs
- Consider adding examples to docs/

## Project Structure

```
NLM/
├── CMakeLists.txt              # C++ build system
├── pyproject.toml              # Python project configuration
├── README.md                   # This file
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md        # System architecture
│   ├── SCIENCE.md             # Scientific background
│   ├── ROADMAP.md             # Development roadmap
│   ├── EXPERIMENTS.md         # Experiment descriptions
│   ├── LIMITATIONS.md         # Known limitations
│   └── PHASE6_FINAL_AUDIT.md  # Phase 6 integration audit
├── src/                        # C++ source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components
│   ├── dynamics/             # Neural dynamics
│   ├── plasticity/           # Plasticity rules
│   ├── development/          # Development system
│   ├── neuromodulation/       # Neuromodulators
│   ├── memory/               # Memory systems
│   ├── prediction/           # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization
├── tests/                      # C++ unit tests
├── python/                     # Python bindings
├── configs/                    # Configuration files
└── examples/                   # Usage examples (to be added)
```

## Phase Summary

(Existing phase summary remains unchanged)

## Scientific Limitations

(Existing limitations section remains unchanged)

## Architecture Philosophy

(Existing architecture philosophy remains unchanged)

## License

MIT

## Authors

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
