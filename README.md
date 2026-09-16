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

**PHASE 2: Real Neural Computation**

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 2 Implementation

The core NLM Phase 2 implementation provides:

#### Neural Computation
- **Real LIF Neuron Dynamics**: Biologically plausible integrate-and-fire neurons with adaptive properties
- **Event-Driven Spike Propagation**: Asynchronous spike transmission with synaptic delays
- **Multiple Plasticity Mechanisms**: STDP, Hebbian learning, and structural plasticity

#### Command-Line Interface
- `--help, -h`: Show help message and exit
- `--version, -v`: Show version information
- `--config <file>`: Load configuration from file
- `--key <value>`: Set configuration values
- `--key=value`: Set configuration with equals syntax
- Support for both `--key value` and `--key=value` formats

#### Configuration System
- **Multi-Format Support**: JSON, YAML, and simple key=value formats
- **Type Safety**: Automatic type inference (int, double, bool, string)
- **Validation**: Range checking and parameter validation
- **Command-Line Override**: Runtime configuration via CLI
- **Persistence**: Save/load configuration to file

#### Advanced Features
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Forward models and prediction error computation
- **Cognition**: Planning, concept formation, attentional selection
- **Development**: Developmental stages, structural changes

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Basic Usage

```bash
./nlm [options]
```

### Examples

```bash
# Run with default configuration
./nlm

# Load custom configuration
./nlm --config custom.cfg

# Override configuration values
./nlm --neuron_count 1000 --random_seed 123 --stdp_ltp_weight 0.02

# Set multiple values
./nlm --neuron_count=500 --region_count=2 --connection_probability=0.15

# Show help
./nlm --help

# Show version
./nlm --version
```

### Running the Phase 2 Demo

```bash
./nlm_phase2_demo
```

This runs a comprehensive demonstration of:
- Basic neural connectivity testing
- Plasticity learning experiments
- STDP verification
- Synaptic weight changes through experience

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Random, Logger, Clock)
│   ├── brain/          # Neural components (Neuron, Synapse, Brain)
│   ├── dynamics/       # Neural dynamics (LIF, SpikeSystem)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, Structural)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (Dopamine, Curiosity, etc.)
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   └── experiments/    # Experiment framework
├── tests/              # Unit and integration tests
├── configs/            # Configuration files
└── docs/              # Documentation
```

## Key Features

### Real Neural Computation
- **LIF Neurons**: Biologically realistic integrate-and-fire dynamics
- **Synaptic Transmission**: Event-driven spikes with realistic delays
- **Plasticity Rules**: STDP, Hebbian, reward-modulated learning
- **Structural Changes**: Synapse creation and elimination

### Advanced Capabilities
- **Memory Integration**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Coordinated dopamine, curiosity, novelty systems
- **Prediction**: Forward models with error signals
- **Cognition**: Planning, concept formation, attentional focus
- **Development**: Age-dependent plasticity and maturation

### User-Friendly Interface
- **Intuitive CLI**: Simple command-line arguments
- **Flexible Config**: Multiple file formats with validation
- **Help System**: Comprehensive documentation
- **Error Handling**: Clear error messages and recovery

## Configuration

The default configuration is in `configs/default.cfg` and includes:

```ini
# Neural parameters
neuron_count = 500
region_count = 1
connection_probability = 0.15
random_seed = 42
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

Configuration can be overridden via command line or custom config files.

## Scientific Approach

NLM investigates whether computational systems can:

- Evolve from simple developmental states
- Learn from experience through neural plasticity
- Integrate multiple brain-like systems
- Develop adaptive behaviors
- Show emergent cognitive capabilities

The project makes no claims of biological accuracy or intelligence, but rather serves as a test bed for neural computation research.

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

For questions about NLM, visit the documentation or check the examples directory for usage patterns.

---

**Version**: 2.0.0 (Phase 2)
**Status**: Ready for research and experimentation
**Interface**: Command-line with comprehensive configuration support