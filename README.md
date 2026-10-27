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

### From Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings

The project includes Python bindings via pybind11:

```bash
# Build the Python module
cd python
pip install -e .
```

Or build with CMake:

```bash
mkdir python_build
cd python_build
cmake ../python -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Using the Command Line Interface

After building, you can use the NLM CLI:

```bash
./nlm-cli demo                    # Run Phase 6 integration demo
./nlm-cli experiment              # Run custom experiment
./nlm-cli config configs/myconfig.cfg  # Load and validate config
./nlm-cli test                    # Run unit tests
./nlm-cli visualize               # Start visualization
./nlm-cli save-config myconfig.cfg  # Save current config
```

### Quick Start Examples

```bash
# 1. Run the Phase 6 integration demo (tests all systems)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm_phase6_demo
./nlm_phase6_demo

# 2. Run Phase 2 basic neural computation demo
make nlm_phase2_demo
./nlm_phase2_demo

# 3. Run the main NLM executable
make nlm
./nlm --neurons 1000 --steps 5000

# 4. Run unit tests
make test
ctest --output-on-failure

# 5. Use the Python bindings
python -c "from pynlm import *; print('NLM Python bindings available')"
```

## Usage

### Command Line Interface

The NLM CLI provides several commands for different use cases:

#### `nlm-cli demo`
Runs the Phase 6 integration demo, which verifies that all brain systems (memory, neuromodulation, prediction, cognition, development) are properly connected and working together.

#### `nlm-cli experiment`
Runs a custom NLM experiment with configurable parameters:

```bash
./nlm-cli experiment \
  --neurons 2000 \
  --steps 10000 \
  --regions 2 \
  --connection-prob 0.2 \
  --width 20 \
  --height 20
```

#### `nlm-cli config <file>`
Loads and validates a configuration file:

```bash
./nlm-cli config configs/experiment.cfg
```

#### `nlm-cli test`
Runs the unit test suite:

```bash
./nlm-cli test
```

#### `nlm-cli visualize`
Starts the visualization module (placeholder for now):

```bash
./nlm-cli visualize
```

#### `nlm-cli save-config <file>`
Saves the current configuration to a file:

```bash
./nlm-cli save-config my_config.cfg
```

### Configuration Files

NLM supports two configuration file formats:

#### Simple Key=Value Format
```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
platomacy_learning_rate = 0.01
```

#### JSON Format (planned for Phase 2)
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "plasticity_learning_rate": 0.01
}
```

### Command Line Arguments

The main NLM executable supports the following command line arguments:

```bash
./nlm [options]

Options:
  --config <file>           Load configuration from file
  --neurons <count>         Number of neurons (default: 1000)
  --steps <count>           Number of simulation steps (default: 10000)
  --regions <count>         Number of brain regions (default: 2)
  --connection-prob <float> Connection probability (0.0-1.0, default: 0.15)
  --help                    Show help message
```

### Examples

#### Running Different Demos

**Phase 6 Integration Demo** (most comprehensive):
```bash
./nlm_phase6_demo
```
This runs a full integration test of all Phase 6 systems.

**Phase 2 Neural Computation Demo** (basic neural dynamics):
```bash
./nlm_phase2_demo
```
This demonstrates real LIF neuron dynamics and synaptic plasticity.

**Phase 3 Learning Demo** (world interaction and learning):
```bash
./nlm_phase3_demo
```
This shows how the agent learns through interaction with the environment.

**Phase 4 Advanced Cognition Demo** (cognition and planning):
```bash
./nlm_phase4_demo
```
This demonstrates advanced cognitive abilities like planning and concept formation.

#### Running Custom Experiments

**Basic Neural Network**:
```bash
./nlm --neurons 500 --steps 2000
```

**Large-Scale Network**:
```bash
./nlm --config configs/large_network.cfg
```

**Reproducible Experiment**:
```bash
./nlm --config configs/reproducible.cfg
```

### Python Usage Examples

The NLM project includes Python bindings for easier scripting and analysis:

```python
from pynlm import NLM, BrainConfig

# Create a brain with custom configuration
config = BrainConfig()
config.random_seed = 42
config.neuron_count = 1000
config.simulation_timestep = 0.001

# Initialize brain
brain = NLM(config)

# Run simulation for 100 steps
for i in range(100):
    brain.step(i * config.simulation_timestep)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

```python
# Example: Save brain state
brain.saveState("brain_state.json")

# Load brain state
brain.loadState("brain_state.json")
```

### Development Workflow

#### 1. Adding New Features

To add new features to NLM:

1. **Add new components** in the appropriate `src/` directory
2. **Update CMakeLists.txt** to include new files
3. **Add tests** in the `tests/` directory
4. **Update documentation** as needed

#### 2. Running Tests

```bash
# Build tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R test_neuron
```

#### 3. Profiling and Performance Optimization

```bash
# Build with optimizations
mkdir build_opt && cd build_opt
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERF_PROFILE=ON
make -j4

# Run performance benchmarks
./benchmark --mode full
```

### Performance Tips

1. **Use appropriate simulation timestep**: Smaller timesteps increase accuracy but reduce performance
2. **Limit neuron count for testing**: Use 100-1000 neurons for rapid prototyping
3. **Enable structural plasticity only for long runs**: Reduces computational overhead
4. **Use checkpointing for long experiments**: Allows recovery from crashes
5. **Monitor memory usage**: Large networks may require optimization

### Troubleshooting

#### "Module not found" errors
```bash
# For Python bindings
pip install pybind11
pip install -e .
```

#### Build failures
```bash
# Ensure you have required dependencies
 sudo apt-get install cmake build-essential python3-dev
```

#### Configuration file errors
```bash
# Check for syntax errors in config files
# Use simple key=value format for now
key = value
# Comments start with #
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Main build system
├── pyproject.toml                     # Python package configuration
├── README.md                          # This documentation
├── easy_usage.md                      # Quick usage guide
├── HOW_TO_USE.md                      # Detailed instructions
├── configs/                           # Configuration files
│   └── default.cfg                   # Default configuration
├── docs/                              # Technical documentation
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components
│   ├── memory/                       # Memory systems
│   ├── neuromodulation/              # Neuromodulators
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   ├── visualization/                # Visualization
│   └── agent/                        # Agent system
├── tests/                            # Unit tests
├── python/                           # Python bindings
├── examples/                         # Example programs
└── nlm-cli                           # Command line interface
```

## Phase Roadmap

### Phase 1: Project Skeleton (COMPLETE)
- Project skeleton and core types
- Neural interfaces

### Phase 2: Real Neural Computation (COMPLETE)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3: World Interaction (COMPLETE)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4: Advanced Cognition (COMPLETE)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance Optimizations (COMPLETE)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration (COMPLETE)
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
