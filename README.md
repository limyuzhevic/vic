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

### NLM (Phase 2) - Enhanced Neural Learning Machine

The NLM project now includes an enhanced command-line interface for the Phase 2 neural computation system:

```bash
./nlm --help
```

#### Execution Modes

- **demo** (default): Run demo/test suite with basic connectivity and learning experiments
- **test**: Run comprehensive test suite including basic connectivity, plasticity experiments, and STDP verification
- **experiment**: Run specific experiments with focused testing
- **benchmark**: Benchmark performance and measure system capabilities

#### Examples

```bash
# Run demo mode with custom configuration
./nlm demo --config=production.cfg --steps=5000

# Run comprehensive test suite
./nlm test --mode=experiment --verbose

# Run performance benchmark
./nlm benchmark --no-tests --output=brain_state.nlm

# Run with custom random seed for reproducible experiments
./nlm demo --seed=12345 --save-state

# Load previously saved brain state and continue
./nlm demo --input=saved_brain.nlm --load-state
```

#### Command Line Options

- `--help, -h`: Show detailed help and usage information
- `--version`: Display version and build information
- `--verbose, -v`: Enable verbose logging
- `--config=file.cfg`: Load configuration from specified file
- `--output=file`: Save brain state to file (default: brain_state.nlm)
- `--input=file`: Load brain state from file for continuation
- `--mode=MODE`: Set execution mode (demo, test, experiment, benchmark)
- `--steps=N`: Set maximum simulation steps
- `--time-limit=N`: Set simulation time limit in seconds
- `--seed=N`: Set random seed for reproducible experiments
- `--no-tests`: Skip automatic test execution
- `--no-experiments`: Skip learning experiment execution
- `--save-state`: Automatically save brain state at completion
- `--load-state`: Load previously saved brain state

## Configuration

The configuration system now supports multiple file formats:

### Configuration File Formats

- **configs/default.cfg**: Simple key=value format (default)
- **configs/*.json**: JSON configuration files (future support)
- **configs/*.yaml** or **configs/*.yml**: YAML configuration files (future support)

### Configuration Parameters

Key configuration parameters include:

- `random_seed`: Random number generator seed
- `simulation_timestep`: Simulation time step in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination
- `max_steps`: Maximum simulation steps
- `time_limit`: Simulation time limit

### Example Configuration File

```
# NLM Configuration File
# Enhanced with JSON/YAML support for future versions

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── IMPROVEMENTS_SUMMARY.md          # This document
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md        # Phase 6 audit
├── configs/
│   ├── default.cfg                  # Default configuration
│   └── ...                          # Additional configuration files
├── src/
│   ├── core/                       # Core utilities
│   │   ├── Config/                  # Configuration system
│   │   ├── Random/                  # Random number generation
│   │   ├── Logger/                  # Logging system
│   │   └── SimulationClock/         # Time management
│   ├── brain/                      # Neural components (integrated)
│   ├── dynamics/                   # Neural dynamics
│   ├── plasticity/                 # Plasticity rules
│   ├── development/                # Developmental system
│   ├── neuromodulation/             # Neuromodulators (integrated)
│   ├── memory/                     # Memory systems (integrated)
│   ├── prediction/                 # Prediction systems (integrated)
│   ├── cognition/                  # Cognitive mechanisms (integrated)
│   ├── sensory/                    # Sensory processing
│   ├── motor/                      # Motor system
│   ├── environment/                # Environment interface
│   ├── experiments/                # Experiment framework (Phase 6)
│   └── visualization/              # Visualization
├── tests/                          # Test suite
└── build/                          # Build directory (generated)
```

## Technical Improvements

### Configuration System
- **Enhanced file parsing**: Support for JSON/YAML configuration files
- **Better error handling**: Comprehensive error reporting and validation
- **Flexible I/O**: Multiple configuration file formats
- **Runtime overrides**: Command-line argument configuration

### Command-Line Interface
- **Comprehensive options**: Extensive command-line options for all operations
- **Multiple modes**: Demo, test, experiment, and benchmark modes
- **Help system**: Detailed help documentation with examples
- **Progress reporting**: Real-time progress monitoring

### Code Quality
- **Modern C++**: Full C++20 compliance and modern practices
- **Modular design**: Well-structured, maintainable code
- **Enhanced logging**: Better logging with timestamps and progress tracking
- **Error handling**: Robust error handling and recovery

### Performance
- **Benchmarking**: Built-in performance benchmarking capabilities
- **Optimization**: Performance optimizations for faster simulations
- **Scalability**: Code designed for scalable simulations

### Testing
- **Comprehensive tests**: Integration of all test scenarios
- **Isolated testing**: Each test runs in isolated environments
- **Performance validation**: Performance testing and validation

## Getting Started

### Quick Start

1. **Clone the repository**:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Run the demo**:
   ```bash
   ./nlm demo
   ```

4. **Explore features**:
   ```bash
   ./nlm --help
   ./nlm benchmark --output=brain_state.nlm
   ```

### Development Workflow

For development and research:

1. **Configuration files**:
   - Modify `configs/default.cfg` for basic configuration
   - Add new configuration files in `configs/` directory
   - Use `--config=` option to specify configuration files

2. **Running experiments**:
   - Use `./nlm experiment --steps=N` for focused experiments
   - Combine `--save-state` and `--load-state` for checkpointing
   - Use `--seed=N` for reproducible experiments

3. **Performance analysis**:
   - Use `./nlm benchmark` for performance testing
   - Monitor output for steps per second and other metrics
   - Use `--verbose` for detailed performance information

## Phase 6 Integration

The NLM project is currently at **Phase 6: FINAL INTEGRATION** with all systems fully connected:

- **Memory Systems**: Working memory, episodic memory, and associative memory fully integrated
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals integrated
- **Prediction System**: Integrated into brain loop with confidence tracking
- **Cognition Systems**: Neural planner, concept formation, and attentional selection connected
- **Development Integration**: Developmental stages affecting plasticity rates implemented
- **Persistence**: Checkpoint save/load and brain state serialization
- **Replay and Consolidation**: Episodic memory replay and memory consolidation functional

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

### Updated Features

This version includes significant improvements:

1. **Enhanced Configuration System**: JSON/YAML support, better error handling
2. **Comprehensive CLI**: Multiple execution modes, extensive options
3. **Improved Error Handling**: Better error messages and recovery
4. **Modular Design**: Better code organization and maintainability
5. **Performance Benchmarking**: Built-in performance testing
6. **Progress Reporting**: Real-time progress monitoring
7. **Checkpoint Support**: Save/load brain state functionality
8. **Documentation**: Comprehensive documentation and examples
9. **Modern C++**: Full C++20 compliance and modern practices
10. **Backward Compatibility**: All existing functionality preserved

For more information, visit: https://github.com/nlm-project/nlm


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
