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

## Enhanced Command-Line Interface (Phase 2)

The main NLM executable now includes an enhanced command-line interface with advanced features:

### New Command-Line Options

- `--help, -h` - Show comprehensive help message
- `--version` - Display NLM version information and system details
- `--config=file, -c file` - Load configuration from file
- `--quiet, -q` - Run in quiet mode (minimal output)
- `--list-config` - List all configuration settings
- `--benchmark` - Run comprehensive performance benchmark suite
- `--save-brain=file, --checkpoint` - Save brain state to checkpoint file
- `--load-brain=file, --restore` - Load brain state from checkpoint
- `--output=file, -o` - Save simulation output to file
- `--profile-file=file` - Save profiling data for performance analysis
- `--experiments, --run-all` - Run all integrated experiments
- `--experiment-steps=steps` - Set number of steps for experiments
- `--visualization, --vis` - Enable visualization mode
- `--debug` - Enable debug mode for troubleshooting
- `--export-format=format` - Set export format (text, json, csv)
- `--export=file, --export-data` - Export data to file

### Usage Examples

#### Basic Usage
```bash
# Run NLM with default configuration
./nlm

# Run with custom configuration
./nlm --config=my_config.cfg

# Run in quiet mode (minimal output)
./nlm --quiet --benchmark
```

#### Advanced Usage
```bash
# Save brain state checkpoint
./nlm --save-brain=checkpoint.nbt --benchmark

# Load and continue from checkpoint
./nlm --load-brain=saved_brain.nbt --output=results.txt

# Run full experiment suite
./nlm --experiments --experiment-steps=5000 --visualization

# Debug and export for analysis
./nlm --debug --export-format=json --export=analysis.json
```

#### Configuration Options
```bash
# Configure key NLM parameters
./nlm --neuron-count=2000 --region-count=4 --stdp-ltp-weight=0.03 --stdp-ltd-weight=0.015

# Enable specific features
./nlm --enable-reward-modulation --enable-structural-plasticity --enable-development
```

## Enhanced Configuration System (Phase 1 → Phase 3)

The configuration system has been significantly enhanced with the following improvements:

### Key Features

1. **Hybrid Configuration Format**
   - Supports both simple key=value format (existing)
   - JSON format for structured configuration (new)
   - Type-safe configuration with validation

2. **Enhanced Configuration Keys**
   - **Brain Configuration**: neuron_count, region_count, connection_probability
   - **Simulation**: simulation_timestep, random_seed
   - **Plasticity**: stdp_ltp_weight, stdp_ltd_weight, stdp_tau
   - **Development**: synaptogenesis_rate, pruning_rate
   - **Neuromodulation**: dopamine_baseline, curiosity_threshold
   - **Memory**: working_memory_capacity, episodic_max_episodes

3. **Advanced Configuration Options**
   ```bash
   # Set multiple configuration options
   ./nlm \
     --neuron-count=3000 \
     --region-count=6 \
     --stdp-ltp-weight=0.025 \
     --stdp-ltd-weight=0.02 \
     --stdp-time-constant=15.0 \
     --synaptogenesis-rate=0.0005 \
     --pruning-rate=0.00005
   ```

4. **Configuration File Format**
   ```
   # Simple key=value format (existing)
   neuron_count = 1000
   region_count = 2
   connection_probability = 0.1
   
   # Optional JSON format (new)
   {
     "neuron_count": 1000,
     "region_count": 2,
     "connection_probability": 0.1,
     "plasticity": {
       "stdp_ltp_weight": 0.01,
       "stdp_ltd_weight": 0.012
     }
   }
   ```

### Configuration Management

#### List Configuration
```bash
./nlm --list-config
```
Displays all available configuration settings with their current values and sources.

#### Show System Information
```bash
./nlm --version
```
Displays detailed information about the NLM system, including:
- Version and build information
- Available features
- Command-line options
- System environment details

## Real STDP Implementation (Phase 2 Enhancement)

The Spike-Timing-Dependent Plasticity (STDP) implementation has been completely replaced with a biologically realistic model:

### STDP Features

1. **Classic Pairwise STDP**
   - Pre-before-post: Long-term potentiation (LTP)
   - Post-before-pre: Long-term depression (LTD)
   - Exponential decay with configurable time constants

2. **Advanced STDP Models**
   - **Pairwise**: Simple, well-understood model
   - **Vilnius**: Incorporates synaptic tags and capture
   - **Ruvinen**: Includes triplet interactions
   - **Tsodyks**: Uses conductance-based approach
   - **Combined**: Integrates multiple mechanisms

3. **Neuromodulation Integration**
   - Dopamine modulation of STDP learning rates
   - Reward prediction error effects
   - Eligibility traces for reward-modulated learning

4. **Biological Plausibility**
   - NMDA receptor dynamics
   - Synaptic weight bounds and normalization
   - Activity-dependent plasticity
   - Competition mechanisms

### STDP Configuration
```bash
# Configure STDP parameters
./nlm \
  --stdp-ltp-weight=0.02 \
  --stdp-ltd-weight=0.015 \
  --stdp-time-constant=20.0
```

## Advanced User Commands and Utilities (Phase 2)

### Brain State Management

#### Save Checkpoint
```bash
./nlm --save-brain=checkpoint1.nbt --benchmark
```
Saves the complete brain state including:
- Neuron membrane potentials
- Synaptic weights
- Memory traces
- Neuromodulation states
- Developmental stage

#### Load Checkpoint
```bash
./nlm --load-brain=checkpoint1.nbt --output=results.txt
```
Loads a previously saved brain state and continues simulation from that state.

### Benchmarking and Performance Analysis

#### Comprehensive Benchmark Suite
```bash
./nlm --benchmark
```
Runs the complete benchmark suite including:
- **Spike Propagation Benchmark**: Measures spike transmission speed
- **Plasticity Benchmark**: Tests learning speed and accuracy
- **Memory Operations Benchmark**: Evaluates memory system performance
- **Stress Test**: Tests performance under maximum load

#### Benchmark Output Format
```
=== Comprehensive Benchmark Results ===
Total benchmark duration: 12.5s
Benchmark phases completed: 4

Phase: Spike Propagation
  Duration: 3.2s
  Operations/sec: 31250
  Spikes generated: 15000
  Average firing rate: 4687.5 Hz
  Memory usage: 45236 bytes

Phase: Plasticity Learning
  Duration: 4.1s
  Operations/sec: 24390
  Spikes generated: 8500
  Average firing rate: 2073.2 Hz

Phase: Memory Operations
  Duration: 3.8s
  Operations/sec: 21053
  Memory usage: 12456 bytes

Phase: Stress Test
  Duration: 1.4s
  Operations/sec: 71429
  Spikes generated: 100000
  Average firing rate: 71428.6 Hz

Performance grade: Excellent
```

## Integrated Experiments and Testing

### Phase 6 Integration Experiment

The NLM now includes a comprehensive Phase 6 integration experiment that tests all brain systems:

#### Experiment Features
1. **Basic Connectivity Test**
   - Verifies neuron connectivity and spike propagation
   - Tests basic neural dynamics

2. **Plasticity Learning Experiment**
   - Demonstrates measurable synaptic changes
   - Tests STDP and Hebbian learning
   - Validates memory encoding

3. **STDP Verification Test**
   - Confirms STDP mechanism functionality
   - Tests potentiation and depression
   - Validates spike-timing dependence

#### Run Experiments
```bash
./nlm --experiments --experiment-steps=2000 --visualization
```

This runs all three experiments and displays detailed results for each:

## Visualization and Debugging

### Visualization Mode
```bash
./nlm --visualization
```
Enables visualization mode for real-time monitoring of:
- Neural activity patterns
- Brain state evolution
- Learning dynamics
- Memory formation

### Debug Mode
```bash
./nlm --debug
```
Provides detailed diagnostic information for:
- Configuration validation
- System initialization
- Performance profiling
- Error detection

## Performance Optimizations

### Built-in Performance Tools

1. **Memory Management**
   - Memory pools for efficient allocation
   - Event queues for spike delivery
   - Optimized data structures

2. **Parallel Processing**
   - SIMD vectorization for neuron operations
   - Multi-threading for spike processing
   - Optimized memory access patterns

3. **Performance Monitoring**
   - Real-time performance metrics
   - Memory usage tracking
   - Benchmark suites
   - Profile analysis tools

### Performance Benchmarking

The NLM includes comprehensive benchmarking tools:

#### Key Performance Metrics
- **Steps per second**: Simulation throughput
- **Memory usage**: RAM consumption
- **Spike rate**: Neural activity levels
- **Firing rate**: Average neuron firing frequency
- **Performance grade**: Overall system evaluation

#### Performance Optimization
```bash
# Run performance analysis
./nlm --benchmark --output=performance_report.json

# Profile memory usage
./nlm --visualization --output=memory_profile.csv

# Analyze spike patterns
./nlm --debug --export-format=csv --export=spike_analysis.csv
```

## Python Bindings and Integration

NLM provides Python bindings for easy integration with Python workflows:

### Basic Python Usage
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Advanced Python Features
```python
import pynlm

# Create agent with world simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run complete simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Building and Installation

### Prerequisites
- C++ Compiler: GCC 9+, Clang 9+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+ (for Python bindings)
- pybind11: Version 2.11.0+ (for Python bindings)

### Build Commands
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Python Installation
```bash
# Install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Testing and Validation

### Unit Tests
NLM includes comprehensive unit tests covering:
- Core neuron dynamics
- Synaptic plasticity mechanisms
- Memory systems
- Neuromodulation
- Development processes

### Integration Tests
- Phase 1: Core functionality
- Phase 2: Real neural computation
- Phase 3: World interaction
- Phase 4: Advanced cognition
- Phase 5: Performance optimizations
- Phase 6: Final integration

### Performance Validation
- Benchmarking suites
- Memory usage analysis
- Stress testing
- Scalability testing

## Scientific and Technical Background

### Research Focus
NLM is a research project investigating computational brain-like systems. Key research questions include:

1. **Memory Integration**
   - Can working memory, episodic memory, and associative memory integrate with neural dynamics?
   - How do memory systems interact with plasticity?

2. **Neuromodulation Coordination**
   - Can dopamine, curiosity, and novelty work together effectively?
   - How do neuromodulators affect learning in a coordinated way?

3. **Prediction as Organizing Principle**
   - Can prediction become a central organizing principle of the brain?
   - How does prediction error signal learning?

4. **Developmental Stages**
   - How do developmental stages affect plasticity rates?
   - Can age-dependent neural dynamics be modeled?

5. **Memory Replay and Consolidation**
   - Does episodic memory replay reinforce learning?
   - How does consolidation work with replay?

6. **Complete Brain Loop**
   - Can all brain systems work together coherently?
   - Does the closed-loop architecture function correctly?

### Scientific Limitations

NLM is a research project and makes NO claims about biological accuracy:

- **Simplified Neuron Model**: Leaky Integrate-and-Fire (not Hodgkin-Huxley)
- **No Ion Channel Dynamics**: Simplified electrical behavior
- **No Dendritic Complexity**: Single-compartment neurons
- **No Realistic Architecture**: Simplified connectivity
- **No Consciousness Claims**: Investigates mechanisms, not subjective experience

### Acknowledgements

This project builds on decades of neural computation research and represents a comprehensive effort to create an integrated artificial brain system with biological plausibility.

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background and technical details.

### Key Contributors
- **Phase 1-6 Integration Team**: All contributors to the NLM project
- **Performance Optimization Team**: Memory pools, SIMD vectorization, parallel processing
- **Biological Modeling Team**: Realistic neuron dynamics, STDP, neuromodulation
- **Integration Testing Team**: Comprehensive validation and benchmarking

---

*For the latest documentation, examples, and technical details, visit: https://github.com/neural-learning-machine/nlm*

## License

MIT

## Authors

Research project — See docs for scientific background.
