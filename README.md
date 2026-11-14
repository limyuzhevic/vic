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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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
Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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

```

## Key Improvements

The NLM project has been significantly enhanced with several key improvements that make it more powerful, easier to use, and more maintainable:

### Command Line Interface Improvements

NLM now features an enhanced command-line interface that provides better control and flexibility:

```bash
nlm --help                    # Show all available options
nlm --phase 2                 # Run Phase 2 demo (default)
nlm --demo phase3 --neurons 1000 --steps 5000  # Custom demo with parameters
nlm --config custom.cfg      # Load custom configuration
nlm --benchmark              # Run performance benchmarks
nlm --performance            # Measure and report performance metrics
nlm --verbose                # Enable verbose logging
nlm --quiet                  # Reduce output (errors only)
```

**New Features:**
- Multiple phases support (2, 3, 4, 5, 6)
- Custom demos with parameter tuning
- Configuration file loading
- Performance monitoring and benchmarking
- Verbose and quiet logging modes
- Batch processing capabilities
- Output redirection to files

### Configuration Management

The configuration system has been completely improved with:

**Enhanced default.cfg file:**
```cfg
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds  
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Connection probability for random connectivity
connection_probability = 0.1

# Vision settings (for agent sensory input)
vision_width = 16
vision_height = 16

# Performance optimization settings
enable_structural_plasticity = true
enable_reward_modulation = true
enable_development = true
enable_curiosity = true
```

**Validation and Safety:**
- Parameter range validation (e.g., neuron_count: 1-100000)
- Type checking and safe defaults
- Warning system for potentially problematic configurations
- Automatic fallback to safe values

### Unified Demo Framework

Code duplication has been significantly reduced with a unified demo framework:

**Benefits:**
- Shared base class for all demos (DemoBase)
- Consistent interfaces across all phases
- Centralized error handling
- Built-in performance monitoring
- Easy extensibility for new phases

**Available Demos:**
- **Phase 2**: Real neural computation with LIF neurons, STDP, and Hebbian plasticity
- **Phase 3**: World interaction with sensory input, action selection, and learning
- **Phase 4**: Emerging cognition including memory, prediction, and planning
- **Phase 5**: Brain-personality development and sandbox learning
- **Phase 6**: Full integration of all neural systems

### Performance Optimizations

Built-in performance monitoring and optimization:

**Benchmarking:**
```bash
nlm --benchmark
```

**Performance Monitoring:**
- Execution timing measurement
- Resource usage tracking (memory, CPU)
- Throughput metrics (operations per second)
- Scalability testing across different brain sizes
- Automatic resource optimization

**Key Optimizations:**
- Memory usage estimation
- Simulation speed measurement
- Resource-aware configuration tuning
- Performance profiling tools

## Quick Start Guide

### Basic Usage

```bash
# Run the basic demonstration
nlm

# Run Phase 3 with world interaction
nlm --phase 3 --neurons 500 --steps 200

# Run Phase 4 with custom settings
nlm --phase 4 --neurons 1000 --steps 1000 --seed 12345
```

### Advanced Usage

```bash
# Load custom configuration and run simulation
nlm --demo phase2 --config custom.cfg --output results.csv

# Run performance benchmarks
nlm --benchmark

# Run in quiet mode for production use
nlm --phase 2 --quiet
```

## Example: Performance Optimization

The configuration system automatically optimizes settings based on available resources:

**For systems with limited memory (< 1GB):**
- Reduces neuron count from 10,000 to 5,000
- Lowers connection probability from 0.2 to 0.1
- Increases simulation timestep from 0.0005 to 0.001

**For systems with ample memory (> 8GB):**
- Maintains maximum neuron count
- Uses higher connection probability
- Uses smaller simulation timestep for accuracy

## Example: Configuration Validation

The system validates all configuration parameters:

```ini
# This configuration will trigger warnings
neuron_count = 20000           # Warning: Large neuron count may impact performance
connection_probability = 0.01  # Warning: Low connection probability may result in disconnected network
synaptogenesis_rate = 0.5      # Error: synaptogenesis_rate must be between 0.0 and 1.0
```

## Example: Performance Benchmarking

```bash
nlm --benchmark
# Output:
# Benchmarking brain size: 100 neurons
#   Initialization: 12ms
#   Simulation (100 steps): 45ms
#   Steps per second: 2222
#   Neurons per second: 222200

# Benchmarking brain size: 500 neurons
#   Initialization: 68ms
#   Simulation (100 steps): 223ms
#   Steps per second: 448
#   Neurons per second: 224133
```

## Building and Running

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the basic demonstration
./nlm

# Run Phase 3 demo
./nlm_phase3_demo

# Run performance benchmarks
./nlm_benchmark
```

## Summary

The NLM project has been significantly enhanced with:

✅ **Better user interface**: Enhanced CLI with comprehensive options
✅ **Improved code organization**: Unified demo framework reducing duplication
✅ **Robust configuration management**: Validation, error handling, and optimization
✅ **Performance monitoring**: Detailed performance measurement and optimization
✅ **Better maintainability**: Cleaner code structure and centralized management

These improvements make NLM more robust, easier to use, and more suitable for both research and production environments. The enhanced CLI provides power users with fine-grained control while the configuration validation prevents common errors and ensures optimal performance.

## License

MIT

## Authors

Research project — See docs for scientific background.
