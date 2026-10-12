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

### CLI Interface (New in Phase 6)

NLM now includes a comprehensive command-line interface for running simulations, managing configurations, and performing analysis:

#### Basic Commands

- `nlm` - Run a simulation from configuration
  ```bash
  nlm run --config my_config.json --steps 1000
  nlm run --config config.cfg --output results.json
  ```

- `nlm create` - Create new configurations
  ```bash
  nlm create brain --output config.json
  nlm create world --width 20 --height 20
  ```

- `nlm validate` - Validate configuration files
  ```bash
  nlm validate --config config.json
  nlm validate --config config.cfg --strict
  ```

- `nlm info` - Display information about configuration or brain
  ```bash
  nlm info --config config.json
  nlm info --brain brain.bin
  ```

- `nlm benchmark` - Run performance benchmarks
  ```bash
  nlm benchmark --config config.json --duration 60
  nlm benchmark --config config.json --iterations 5
  ```

#### Common Options

- `--config` - Configuration file path
- `--output` - Output file path
- `--steps` - Number of simulation steps
- `--time` - Time limit in seconds
- `--pause` - Pause duration in seconds
- `--verbose` - Enable verbose output
- `--quiet` - Suppress output
- `--debug` - Enable debug mode
- `--test` - Run in test mode
- `--validate` - Validate configuration only

#### Python API

The NLM Python API (`pynlm`) provides programmatic access to all NLM functionality:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simulation step
brain.step(step, time)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

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

### Command Line Interface

After building, the `nlm` command will be available in your PATH. Run `nlm --help` to see all available commands and options.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/           # Documentation
├── src/           # Source code
│   ├── cli/       # CLI implementation (NEW)
│   ├── core/      # Core utilities
│   ├── brain/     # Neural components (integrated)
│   ├── ...        # Other systems
│   └── ...
├── tests/         # Unit tests
├── configs/       # Example configuration files
└── python/        # Python bindings
    ├── pyproject.toml  # CLI entry points (NEW)
    └── bindings.cpp   # Python bindings
```

## CLI Command Reference

### nlm run

Runs a simulation from a configuration file.

**Options:**
- `--config PATH` - Configuration file (JSON, YAML, or .cfg format)
- `--steps N` - Number of simulation steps
- `--output PATH` - Output file to save brain state
- `--pause SECONDS` - Pause between steps

**Examples:**
```bash
# Run with JSON config
nlm run --config my_config.json --steps 1000

# Run with simple config and save results
nlm run --config config.cfg --output results.json
```

### nlm create

Creates a new configuration file.

**Options:**
- `--type TYPE` - Configuration type: `brain` or `world`
- `--output PATH` - Output file path

**Examples:**
```bash
# Create a brain configuration
nlm create brain --output brain_config.json

# Create a world configuration
nlm create world --width 20 --height 20 --output world_config.json
```

### nlm validate

Validates a configuration file.

**Options:**
- `--config PATH` - Configuration file to validate
- `--strict` - Enable strict validation mode

**Examples:**
```bash
# Validate configuration
nlm validate --config config.json

# Validate with strict checking
nlm validate --config config.cfg --strict
```

### nlm info

Displays information about configuration or brain state.

**Options:**
- `--config PATH` - Configuration file to inspect
- `--brain PATH` - Brain file to analyze
- `--field FIELD` - Specific field to display (default: all)

**Examples:**
```bash
# Show configuration details
nlm info --config config.json

# Show brain statistics
nlm info --brain brain.bin
```

### nlm benchmark

Runs a performance benchmark on the NLM brain.

**Options:**
- `--config PATH` - Configuration file
- `--duration SECONDS` - Benchmark duration
- `--iterations N` - Number of iterations

**Examples:**
```bash
# Run benchmark for 60 seconds
nlm benchmark --config config.json --duration 60

# Run 5 iterations
nlm benchmark --config config.json --iterations 5
```

## Technical Considerations

### Configuration File Formats

NLM supports three configuration file formats:

1. **JSON (.json)** - Structured data format
2. **YAML (.yaml/.yml)** - Human-readable format
3. **Simple (.cfg)** - Key=value format

Configuration files are automatically detected based on file extension.

### Argument Parsing

The CLI uses CLI11 library for modern, intuitive command-line argument parsing with support for:
- Subcommands (run, create, validate, info, benchmark)
- Options with and without values
- Flags (boolean options)
- Help generation

### Integration with Existing Systems

The CLI integrates seamlessly with:
- **Config system** - Uses existing Config class for configuration management
- **Logger system** - Integrates with NLM logging framework
- **Python API** - Provides command-line access to Python functionality
- **Testing system** - Includes built-in testing capabilities

### Output Formats

The CLI supports multiple output formats:
- **JSON** - Structured data for machine processing
- **CSV** - Tabular data for analysis
- **Console** - Human-readable output
- **YAML** - Structured data format

### Error Handling

Comprehensive error handling includes:
- File not found errors
- Format validation errors
- Configuration validation errors
- Runtime errors with helpful messages

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

### Phase 6 (Complete - Final Integration with CLI)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created
- **NEW: Comprehensive CLI interface added**

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
- **Whether the CLI provides an accessible interface for research**

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
