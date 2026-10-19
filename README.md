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

### Command Line Interface

NLM now includes a comprehensive command-line interface (`nlm-cli`):

```bash
./nlm-cli --help
```

**Available Commands:**

- **brain** - Brain management (create, initialize, step, save/load)
- **experiment** - Run experiments (demo, test, explore, lifelong, integrate)
- **config** - Configuration management (load, save, show, set)
- **log** - Logging control (level, file, status)
- **checkpoint** - Checkpoint operations (save, load, list)
- **explore** - Explore brain state and structure
- **batch** - Run batch operations (simulate, checkpoint, train, test)
- **export** - Export brain data to various formats

**Basic Examples:**

```bash
# Create and initialize a brain
./nlm-cli brain create
./nlm-cli brain initialize

# Run 1000 simulation steps
./nlm-cli brain step 1000

# Run Phase 6 integration demo
./nlm-cli experiment demo

# Load configuration from file
./nlm-cli config load my_settings.json

# Save brain state
./nlm-cli brain save brain_state.bin

# Load brain state
./nlm-cli brain load brain_state.bin

# Run exploration experiment
./nlm-cli experiment explore

# Run batch simulation
./nlm-cli batch simulate 500

# Export brain statistics
./nlm-cli brain status
./nlm-cli export stats brain_stats.txt
```

## Python Bindings

NLM provides Python bindings via `pynlm`:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Save brain state
brain.save("brain_state.bin")
```

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
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/         # Memory systems (integrated)
│   ├── prediction/      # Prediction systems (integrated)
│   ├── cognition/       # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── nlm-cli/            # Command line interface
│   ├── nlm-cli.cpp
│   └── *.hpp
├── tests/
└── configs/
```

## Quick Start

1. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the integration demo:**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Use the command-line interface:**
   ```bash
   ./nlm-cli --help
   ./nlm-cli experiment demo
   ```

4. **Run tests:**
   ```bash
   cd build
   ctest
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

## Key Improvements in Phase 6

### 1. Comprehensive Integration
- **Working Memory**: Transient active information storage with competitive dynamics
- **Episodic Memory**: Experience encoding with pattern completion and temporal indexing
- **Associative Memory**: Pattern-based associations and concept formation
- **Prediction System**: Temporal sequence learning and action consequence prediction

### 2. Neuromodulation Systems
- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation and intrinsic reward
- **Novelty**: Novelty detection for information seeking
- **Prediction Error**: Error signals for learning and adaptation

### 3. Cognitive Architecture
- **Attentional Selection**: Competitive attention with lateral inhibition
- **Neural Planner**: Predictive action selection using learned consequences
- **Concept Formation**: Pattern discovery without pre-labeled categories
- **Self-Model**: Sensorimotor self-awareness and body schema

### 4. Developmental System
- **Stage Progression**: Initial → Critical Period → Maturation → Adult → Aging
- **Plasticity Modulation**: Age-dependent changes in learning rates
- **Experience-Dependent Maturation**: Structural and functional changes over time

### 5. Advanced Features
- **Event-Driven Spike Processing**: Efficient neural computation
- **Memory Replay**: Consolidation during sleep/rest cycles
- **Checkpoint Persistence**: Save/load brain state for long-term studies
- **Parallel Processing**: Efficient use of multi-core architectures

## Command Line Interface (nlm-cli)

The new `nlm-cli` provides comprehensive control over the neural simulation:

### Brain Management
- Create and initialize brains with custom configurations
- Run simulation steps with various input patterns
- Save and load brain states for long-term experiments
- Monitor brain statistics and performance metrics

### Experiment Framework
- Run Phase 6 integration demo with comprehensive testing
- Execute specific test suites (connectivity, plasticity, STDP)
- Run exploration experiments with parameter sweeps
- Perform lifelong learning simulations
- Verify system integration and connections

### Configuration and Logging
- Load/save configuration files in JSON format
- Set log levels and add file logging
- Monitor system status and performance
- Configure simulation parameters and constraints

### Advanced Operations
- Batch processing for large-scale simulations
- Export brain data to various formats
- Explore brain structure and state
- Checkpoint management for long-running experiments

## Python Bindings (pynlm)

The Python bindings provide a high-level interface to the NLM framework:

- **Brain Control**: Create, initialize, and simulate neural systems
- **Sensory Integration**: Connect to world models and perception
- **Action Control**: Generate motor commands from neural activity
- **Memory Management**: Access working and episodic memory systems
- **Export/Import**: Save and load brain states for analysis

## Testing

The project includes comprehensive test suites:

- **Unit Tests**: Test individual components (neuron, synapse, plasticity)
- **Integration Tests**: Verify system connections and interactions
- **Performance Tests**: Benchmark different configurations
- **Regression Tests**: Ensure stability across updates

Run tests with:
```bash
cd build
ctest
```

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background.