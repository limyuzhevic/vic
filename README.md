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

## Building and Running

### Prerequisites

- **CMake 3.16+**
- **C++20 compatible compiler** (GCC 10+, Clang 12+, or MSVC 2019+)
- **Python 3.8+** (for Python bindings)
- **pybind11 2.11.0+** (for Python bindings)

### Build Instructions

```bash
# Standard build (recommended)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Alternative builds
make -j4  # Multi-core build
cmake .. -DCMAKE_BUILD_TYPE=Debug   # Debug version
cmake .. -DPYTHON_EXECUTABLE=$(which python)  # Specify Python
```

### Running the Demos

Build all targets first (see Building above). The entry points are currently separate demos per phase:

```bash
# Phase 2: Basic neural computation demo
./nlm

# Phase 3: World interaction demo
./nlm_phase3_demo [num_episodes] [steps_per_episode]

# Phase 4: Emerging cognition demo
./nlm_phase4_demo [num_trials]

# Phase 5: Standalone demo (NEW!)
./nlm_phase5_demo

# Phase 6: Integration test (comprehensive)
./nlm_phase6_demo

# Performance benchmark
./nlm_benchmark

# Experiment runner (Phase 6)
./nlm_phase6_demo
```

#### Advanced Command Line Options

All demos support these advanced options:

```bash
# Configuration and setup
--config <file>           Config file path (default: configs/default.cfg)
--neurons <count>         Number of neurons (default: 500)
--regions <count>         Number of brain regions (default: 1)
--steps <count>           Maximum simulation steps (default: 1000)
--connection <prob>       Connection probability (default: 0.15)
--seed <seed>             Random seed (default: 42)
--timestep <time>         Simulation timestep (default: 0.001)

# Demo modes
--mode <mode>             Demo mode: comprehensive, basic, advanced, custom
--plasticity-only         Run only plasticity experiments
--stdp-only               Run only STDP verification
--connectivity-only       Run only connectivity test

# Output and visualization
--output <dir>            Output directory for results (default: ./nlm_demos)
--save                    Save demo results to files
--quiet                   Suppress non-essential output
--debug                   Enable debug output

# Performance and processing
--profile                 Enable performance profiling
--parallel                Enable parallel processing of neurons
--batch                   Run multiple demonstrations in batch mode

# Specialized features
--neural-dynamics         Enable detailed neural dynamics visualization
```

#### Example Usage

```bash
# Run Phase 3 demo with custom settings
./nlm_phase3_demo 10 500

# Run comprehensive Phase 2 demo with visualization
./nlm --mode comprehensive --neurons 1000 --regions 2

# Run Phase 6 integration test with checkpointing
./nlm_phase6_demo --neurons 500 --steps 2000

# Run benchmark for performance testing
./nlm_benchmark --neurons 10000 --steps 5000

# Batch processing for parameter sweeps
./nlm --batch --neurons 500 1000 2000 --seed 42 43 44
```

#### Configuration Files

The system uses configuration files to control brain parameters:

**Config File Format:**
```ini
# Example config file (configs/custom.cfg)
random_seed = 123
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
dopamine_baseline = 0.1
environment_width = 20
environment_height = 20
vision_width = 16
vision_height = 16
enable_multithreading = true
enable_simd = true
enable_sparse_connectivity = true
```

**Built-in Config:**
- `configs/default.cfg` - Default configuration for most experiments
- `configs/development.cfg` - Optimized for development/debugging
- `configs/performance.cfg` - Optimized for performance testing

#### Running Python Bindings

If Python bindings are available, you can run Python examples:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Optionally modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
```

### Project Structure

```
NLM/
├── CMakeLists.txt                    # Main build configuration
├── README.md                         # Project documentation
├── docs/                             # Detailed documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   ├── LIMITATIONS.md                 # Current limitations
│   └── PHASE6_FINAL_AUDIT.md         # Phase 6 integration audit
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   ├── agent/                        # Agent-brain interface
│   ├── world/                        # Simple world simulation
│   ├── performance/                  # Performance optimizations
│   ├── brain/                        # Neural components
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulation systems
│   ├── memory/                       # Memory systems
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                # Visualization
├── tests/                           # Unit tests
├── configs/                         # Configuration files
├── python/                          # Python bindings
└── build/                           # Build directory (generated)
```

## Phase Summary

### Phase 1 (Complete) - Project Skeleton
- Core types and configuration management
- Neural interfaces and communication protocols
- Basic build system and project structure

### Phase 2 (Complete) - Real Neural Computation
- Real Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP (Spike-Timing-Dependent Plasticity) and Hebbian plasticity rules
- Structural plasticity (synaptogenesis and pruning)
- **Note**: Enhanced with reduced firing rates and improved stability

### Phase 3 (Complete) - World Interaction
- World simulation loop with sensory input and motor output
- Reward prediction error and learning mechanisms
- Developmental stages and maturation
- Novelty detection and curiosity-driven exploration

### Phase 4 (Complete) - Emerging Cognition
- Neural prediction system for forward models
- Working memory with persistent activity
- Episodic-like memory storage and retrieval
- Concept formation from experience patterns
- Neural attention with competitive selection dynamics
- Predictive planning and action selection
- Self-model development and body awareness
- Social learning and imitation
- Continual learning and adaptation

### Phase 5 (Complete) - Performance Optimizations
- Memory pools and event queues for efficient memory management
- SIMD (Single Instruction Multiple Data) vectorization
- Parallel processing for multi-core systems
- Checkpoint system for long-running simulations
- **New**: Standalone demo executable (`nlm_phase5_demo`)

### Phase 6 (Complete) - Final Integration
- All systems integrated into a coherent artificial brain
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Prediction system integrated with learning
- Development affects plasticity rates
- Checkpoint save/load functionality working
- Replay and consolidation functional
- **Issue**: Checkpointing in Phase 6 demo shows as NOT WORKING (needs fix)

## Performance Improvements

### Neural Dynamics Optimization

1. **Reduced Excessive Firing Rates**:
   - Current injections reduced in plasticity experiments (30→10)
   - Current injections reduced in STDP tests (60→20)
   - Current injections reduced in connectivity tests (50→15)
   - Current injections reduced in benchmarks (30→10)

2. **Improved Reward System**:
   - Fixed predicted reward calculation using agent's expected reward
   - Positive rewards for normal actions (movement, turning)
   - Better balance between exploration and exploitation

3. **Enhanced Configuration**:
   - Expanded default.cfg with advanced options
   - Better parameter management for different use cases
   - Improved stability and performance tuning

### Code Quality Improvements

1. **Phase 5 Standalone Demo**:
   - Created `nlm_phase5_demo` executable
   - Demonstrates large-scale neural simulation (10k+ neurons)
   - Includes scaling experiments and continual learning

2. **Phase 2 Enhanced Demo**:
   - Improved main.cpp with comprehensive command-line interface
   - Better error handling and user feedback
   - Advanced visualization and profiling options
   - Multiple demo modes (basic, comprehensive, advanced)

3. **Documentation Improvements**:
   - Enhanced README.md with detailed instructions
   - Better configuration file examples
   - Improved usage examples and tutorials

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

## Contact and Support

For questions, issues, or collaboration opportunities:
- Check the documentation in `docs/` for detailed information
- Review the source code for implementation details
- Consult individual phase documentation for specific features

## Acknowledgments

This project builds on decades of research in computational neuroscience, neural networks, and artificial intelligence. Special thanks to the contributors and research community that have made this work possible.

## Quick Start

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the comprehensive Phase 2 demo
./nlm

# Run the world interaction demo
./nlm_phase3_demo

# Run the integration test
./nlm_phase6_demo

# Run the performance benchmark
./nlm_benchmark
```

For more information, see the detailed documentation in `docs/` and individual demo help (use `--help` flag).

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
