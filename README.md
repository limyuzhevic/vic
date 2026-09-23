# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## CLI Interface

NLM now includes a comprehensive command-line interface for runtime control:

### Basic Commands

**Help and Version:**
```bash
nlm --help                    # Show help message
nlm --version                 # Show version information
```

**Configuration:**
```bash
nlm --config experiment.cfg   # Load configuration from file
nlm --log-level DEBUG         # Set log level
nlm --verbose                 # Enable verbose logging
nlm --quiet                   # Disable all logging
```

**Examples:**
```bash
# Load custom configuration
nlm --config my_settings.cfg

# Run with debug logging
nlm --log-level DEBUG --verbose

# Quiet operation for scripts
nlm --log-level ERROR --quiet
```

### Configuration File Support

NLM supports multiple configuration file formats:

**Simple .cfg format (existing format):**
```
# Settings file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1
```

**JSON format (new format):**
```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "connection_probability": 0.1,
  "log_level": "INFO"
}
```

**YAML format (new format):**
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
connection_probability: 0.1
log_level: INFO
```

### Command Line Arguments

The NLM CLI supports the following arguments:

| Argument | Description |
|----------|-------------|
| `--help` | Show help message |
| `--version` | Show version information |
| `--config FILE` | Load configuration from FILE |
| `--log-level LEVEL` | Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `--verbose` | Enable verbose logging |
| `--quiet` | Disable all logging output |

## Testing

NLM now includes a comprehensive test suite for ensuring code quality and preventing regressions:

### Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test suite
./nlm_test
```

### Test Coverage

The test suite includes:

1. **Config Tests**: Unit tests for configuration system
   - Testing save/load functionality
   - Testing argument parsing
   - Testing vector type support
   - Testing error handling

2. **CLI Tests**: Command-line interface testing
   - Argument parsing validation
   - Configuration loading testing
   - Error handling scenarios

3. **Integration Tests**: System-level testing
   - Basic system functionality
   - Component interaction
   - Performance benchmarks

### Adding New Tests

To add a new test:

1. Create a new test class inheriting from `TestBase`
2. Implement the `getName()` and `run()` methods
3. Add the test to a test suite
4. Rebuild and run the tests

## Configuration System Improvements

### Bug Fixes

The NLM configuration system has been fixed for several critical issues:

1. **saveToFile placeholder value**: Now properly serializes all configuration values instead of writing "PLACEHOLDER_VALUE"
2. **loadFromArgs syntax**: Fixed incorrect argument parsing in command-line processing
3. **Duplicate configuration**: Fixed duplicate `dopamine_baseline` setting in default.cfg

### New Features

1. **JSON/YAML Support**: Added support for JSON and YAML configuration files
2. **Enhanced Vector Types**: Full support for std::vector<int>, std::vector<double>, and std::vector<std::string>
3. **Better Error Handling**: Improved error messages and validation
4. **Type Safety**: Enhanced type conversion and validation

### Example Configuration Files

**Minimal configuration (default.cfg):**
```
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

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

**JSON configuration example:**
```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "plasticity_learning_rate": 0.01,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_time_constant": 20.0,
  "development_synaptogenesis_rate": 0.001,
  "development_pruning_rate": 0.0001,
  "dopamine_baseline": 0.1,
  "reward_discount_factor": 0.99,
  "environment_name": "GridWorld",
  "environment_width": 10,
  "environment_height": 10,
  "log_level": "INFO",
  "log_to_file": false,
  "log_filename": "nlm.log",
  "max_simulation_steps": 10000,
  "simulation_time_limit": 0.0,
  "visualization_enabled": false,
  "visualization_update_rate": 30.0
}
```

## Project Structure Updates

### New Directories

- **`src/core/`**: Core utilities including new CLI and test infrastructure
  - `CLI.hpp` - Command-line interface header
  - `CLI.cpp` - CLI implementation
  - `TestSuite.hpp` - Test framework header
  - `TestRunner.hpp` - Test runner header
  - `ConfigTest.cpp` - Configuration system unit tests

### File Organization

The project structure now includes:

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
├── src/                     # Source code
│   ├── core/               # Core utilities
│   │   ├── CLI.hpp
│   │   ├── CLI.cpp
│   │   ├── TestSuite.hpp
│   │   ├── TestRunner.hpp
│   │   ├── ConfigTest.cpp
│   │   └── ...
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
├── tests/                  # Test suite
└── configs/                # Configuration files
    ├── default.cfg
    └── ...
```

## Building

### Standard Build Process

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Options

The NLM build system supports the following options:

| Option | Description |
|--------|-------------|
| `-DCMAKE_BUILD_TYPE=Debug` | Build with debug symbols and assertions |
| `-DCMAKE_BUILD_TYPE=Release` | Build with optimizations (default) |
| `-DPYTHON_EXECUTABLE=path` | Specify Python interpreter for bindings |

### Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### NLM Console Application

```bash
./nlm [OPTIONS] [ARGS...]
```

For help and usage information:
```bash
./nlm --help
./nlm --version
```

### Python Bindings

For Python users, NLM includes Python bindings:

```bash
# Install Python bindings
pip install pynlm

# Example usage
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

For more information, see the Python documentation.

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
- CLI interface for runtime control
- Comprehensive test suite
- JSON/YAML configuration support
- Fixed critical bugs

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
