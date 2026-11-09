# Simple NLM Usage Example

This file provides a quick start guide for users getting started with NLM (熙然).

## Quick Start

The NLM (熙然) neural simulation framework is designed to create and study artificial brains with realistic neural dynamics.

### Phase 2: Real Neural Computation

This is the entry level for real spiking neural computation:

- **Leaky Integrate-and-Fire (LIF) neurons** with realistic membrane dynamics
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** for learning
- **Structural plasticity** for synapse formation and pruning
- **Integrated memory systems** for working and episodic memory

## Getting Started

### 1. Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### 2. Run the Basic Demo

```bash
# Phase 2 demo - basic neural computation
./nlm

# Phase 3 demo - world interaction
./nlm_phase3_demo

# Phase 4 demo - full integration
./nlm_phase4_demo

# Phase 6 demo - complete integration test
./nlm_phase6_demo

# Benchmark - performance testing
./nlm_benchmark
```

## Configuration Files

NLM uses configuration files to set up simulations:

### Default Configuration

**File:** `configs/default.cfg`

```ini
# NLM Configuration File
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
```

### Advanced Configuration

**File:** `configs/advanced.cfg`

Contains advanced settings for research and large-scale simulations with detailed neuromodulation and memory system configurations.

## Command-Line Interface

NLM supports both configuration files and command-line arguments for flexibility:

### Basic Arguments

```bash
# Load custom configuration
./nlm --config my_config.cfg

# Override specific parameters
./nlm --neuron-count 500 --region-count 2 --timestep 0.005

# Enable specific test modes
./nlm --basic          # Basic neural connectivity test
./nlm --plasticity     # Plasticity learning experiment
./nlm --stdp           # STDP verification test
./nlm --integration    # Complete integration test
```

### Configuration File Format

NLM uses a simple key=value format:

```ini
# This is a comment
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42

# Boolean values
log_to_file = true
visualization_enabled = false

# Vector values
sensory_neurons = [0, 1, 2, 3, 4]
motor_neurons = [10, 11, 12]
```

## Example Configurations

### Example 1: Small Neural Network

```bash
./nlm --neuron-count 100 --region-count 1 --steps 500
```

Creates a simple brain with 100 neurons for quick testing.

### Example 2: Learning Experiment

```bash
./nlm --plasticity --neuron-count 500 --checkpoint learning_session.bin
```

Runs a plasticity learning experiment and saves the brain state for later analysis.

### Example 3: Research Configuration

```bash
./nlm --config configs/advanced.cfg --enable-development --enable-replay
```

Uses advanced configuration with development and replay features enabled.

## Monitoring and Progress Tracking

NLM provides comprehensive monitoring during simulations:

### Real-time Output

During simulation runs, NLM outputs:

- **Step progress**: Current step number
- **Activity metrics**: Spike counts, firing rates
- **System status**: Memory usage, integration status
- **Learning progress**: Synaptic weight changes

### Example Output

```
=== NLM Phase 2: Real Neural Computation ===
Step 0 | Spikes: 8192 | Firing: 123.4 Hz | WorkingMem: 5
Step 1000 | Spikes: 15678 | Firing: 234.7 Hz | WorkingMem: 12
Step 2000 | Spikes: 22456 | Firing: 345.2 Hz | WorkingMem: 18
...
```

## Memory Systems

NLM includes integrated memory systems:

### Working Memory

Transient memory for active information with persistence and competition dynamics.

### Episodic Memory

Experience storage with replay and consolidation capabilities.

### Associative Memory

Pattern associations through Hebbian learning mechanisms.

## Neuromodulation

NLM includes biologically-inspired neuromodulation:

- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signal for model updating

## Development

NLM simulates neural development with:

- **Developmental stages**: Initial → Critical Period → Maturation → Adult
- **Plasticity decay**: Age-dependent reduction in learning rate
- **Structural changes**: Synaptogenesis and pruning

## Integration Features

Phase 6 integrates all systems into a coherent brain:

- **Memory integration**: Neural processing connected to memory systems
- **Neuromodulation integration**: Affect on plasticity and dynamics
- **Prediction integration**: Predictive processing as central principle
- **Development integration**: Growth affects learning capabilities
- **Replay and consolidation**: Memory strengthening during rest

## File Structure

```
src/
├── core/              # Core utilities (Config, Logger, Random, etc.)
├── brain/             # Neural components (Neuron, Synapse, Brain)
├── dynamics/          # Neural dynamics (LIF, SpikeSystem)
├── plasticity/        # Plasticity rules (STDP, Hebbian, Structural)
├── development/       # Developmental system
├── neuromodulation/   # Neuromodulators (Dopamine, Curiosity, etc.)
├── memory/           # Memory systems
├── prediction/        # Prediction systems
├── cognition/        # Cognitive mechanisms
├── sensory/           # Sensory processing
├── motor/            # Motor system
├── environment/       # Environment interface
├── agent/            # Agent-brain interface
├── world/            # Simple world simulation
└── experiments/       # Experiment frameworks (Phase 3-6)

configs/
├── default.cfg       # Default configuration
└── advanced.cfg      # Advanced configuration (research)

examples/
├── basic/           # Basic usage examples
├── advanced/        # Advanced examples
└── documentation.md # Comprehensive documentation

build/
├── nlm               # Main executable (Phase 2)
├── nlm_phase3_demo   # Phase 3 demo
├── nlm_phase4_demo   # Phase 4 demo
├── nlm_phase6_demo   # Phase uration test
└── nlm_benchmark     # Performance benchmarks
```

## Learning Outcomes

When you run NLM, you will learn:

### Phase 2: Basic Neural Computation

1. **LIF Neuron Dynamics**: How neurons integrate inputs and produce spikes
2. **Event-Driven Processing**: Efficient spike propagation with delays
3. **Plasticity Mechanisms**: How synapses change through learning
4. **Network Behavior**: Emergent properties of neural networks

### Phase 3: World Interaction

5. **Sensory Input**: How brain receives environmental information
6. **Action Selection**: How brain chooses motor outputs
7. **Learning from Experience**: Reward-based learning mechanisms
8. **Behavior Generation**: How neural activity produces behavior

### Phase 4: Cognitive Integration

9. **Memory Systems**: Working and episodic memory integration
10. **Prediction Systems**: Forward models and error signals
11. **Concept Formation**: Pattern discovery from experience
12. **Neural Planning**: Action sequence evaluation

### Phase 5: Performance Optimizations

13. **Memory Pools**: Efficient memory management
14. **Event Queues**: Spike event processing
15. **SIMD Processing**: Vectorized computation
16. **Parallel Processing**: Multi-core utilization

### Phase 6: Complete Integration

17. **Unified Brain Loop**: All systems working together
18. **Developmental Trajectory**: Growth and maturation
19. **Replay and Consolidation**: Memory strengthening
20. **Research Applications**: Large-scale simulations

## Troubleshooting

### Common Issues and Solutions

**Issue:** "No spikes generated"
**Solution:** Check neuron parameters, synaptic weights, and input currents

**Issue:** "Memory systems not responding"
**Solution:** Verify memory initialization and brain connectivity

**Issue:** "Checkpoint save/load fails"
**Solution:** Check file permissions and ensure consistent brain state

**Issue:** "Performance too slow"
**Solution:** Reduce neuron count, decrease plasticity rates, or use simpler configurations

### Debug Mode

Enable debug logging:

```bash
export NLM_LOG_LEVEL=DEBUG
./nlm
```

### Configuration Validation

Validate configuration files:

```bash
# Check syntax
python3 validate_config.py configs/default.cfg

# Test with small brain
./nlm --neuron-count 50 --steps 100
```

## Further Reading

### Documentation

- **README.md**: Main documentation and project overview
- **docs/ARCHITECTURE.md**: Detailed technical architecture
- **docs/SCIENCE.md**: Scientific background and principles
- **docs/ROADMAP.md**: Development roadmap and phases

### Research Papers

The NLM project is based on:

- **Biological Neural Models**: LIF neuron dynamics
- **Hebbian Learning**: Activity-dependent synaptic modification
- **Neuromodulation**: Dopamine and curiosity signals
- **Memory Systems**: Working and episodic memory integration

### Extensions

Consider these research directions:

1. **Large-Scale Networks**: Simulate thousands of neurons
2. **Multi-Modal Sensory**: Vision, audio, and internal signals
3. **Social Learning**: Multi-agent environments
4. **Developmental Trajectories**: Longitudinal studies
5. **Clinical Applications**: Simulate neurological conditions

## Contact and Support

For questions and support:

- **Documentation**: README.md and docs/
- **Examples**: examples/ directory
- **Configuration**: configs/ directory
- **Build Issues**: CMakeLists.txt

NLM is an open-source research project. Contributions and improvements are welcome!
