# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an advanced experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

**PHASE 6: FINAL INTEGRATION (ENHANCED VERSION)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system with enhanced functionality.

### Enhanced Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competitive selection
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
- Enhanced prediction with multiple temporal scales

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Enhanced attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- Enhanced developmental trajectory modeling

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints
- Automatic checkpoint management

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle
- Replay-based learning enhancement

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### With Advanced Features
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENHANCED_FEATURES=ON
make -j4
```

### Build Options
- `NLM_ENABLE_CHECKPOINTING`: Enable checkpoint system (default: ON)
- `NLM_ENABLE_REPLAY`: Enable memory replay (default: ON)
- `NLM_ENABLE_DEVELOPMENT`: Enable developmental system (default: ON)
- `NLM_ENABLE_NEUROMODULATION`: Enable neuromodulation (default: ON)
- `NLM_ENABLE_VISUALIZATION`: Enable visualization interface (default: OFF)

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Enhanced Phase 6 Commands
```bash
./nlm_phase6 --help                    # Show help information
./nlm_phase6 --version                 # Show version and build information
./nlm_phase6 --integration             # Run integration verification only
./nlm_phase6 --memory                  # Run memory integration test only
./nlm_phase6 --neuromodulation          # Run neuromodulation integration test only
./nlm_phase6 --checkpoint              # Run checkpoint test only
./nlm_phase6 --replay                  # Run replay test only
./nlm_phase6 --neurons=1000 --steps=5000  # Custom experiment size
./nlm_phase6 --config=experiment.cfg   # Load custom configuration
./nlm_phase6 --checkpoint-dir=./results # Specify checkpoint directory
```

### Configuration Files
Create configuration files in `configs/` directory:
- `default.cfg`: Default configuration
- `experiment_a.cfg`: Small-scale experiment
- `experiment_b.cfg`: Large-scale experiment
- `development.cfg`: Development-focused experiment

Example configuration file:
```
# NLM Configuration File
#neuron_count = 500
#max_steps = 2000
#region_count = 2
#connection_probability = 0.1
#stdp_ltp_weight = 0.01
#stdp_ltd_weight = 0.012
#synaptogenesis_rate = 0.0001
#pruning_rate = 0.00001
#replay_interval = 100
#consolidation_interval = 1000
#enable_checkpointing = true
#enable_replay = true
#enable_development = true
```

### Advanced Usage
```bash
# Run specific system tests
./nlm_phase6 --integration --memory --neuromodulation

# Run with verbose output for debugging
./nlm_phase6 --verbose --neurons=2000

# Run experiment with custom checkpoint path
./nlm_phase6 --checkpoint-dir=./experiment_results --steps=10000
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (types, config, logger, random)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics (PHASE 6 enhanced)
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   ├── visualization/ # Visualization
│   └── nlm_phase6_cli.cpp  # Enhanced CLI interface
├── configs/            # Configuration files
├── docs/               # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit report
├── tests/              # Test suite
└── CMakeLists.txt      # Build system
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Enhanced)
- Real LIF neuron dynamics (improved)
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

### Phase 5 (Enhanced)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system (enhanced)

### Phase 6 (Enhanced - Final Integration)
- All systems integrated into coherent brain loop
- Enhanced memory systems with working/episodic/associative memory
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Enhanced checkpoint save/load with management
- Enhanced replay and consolidation functional
- Phase 6 integration experiment created with CLI

## Enhanced Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Enhanced limitations include:

- Simplified LIF neuron model with stochastic channel noise (not Hodgkin-Huxley)
- No realistic ion channel dynamics (but modeled stochastically)
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 ENHANCED investigates:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently
- Whether enhanced dynamics improve learning efficiency

## Enhanced Architecture Philosophy

The Enhanced NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
ENHANCED NEURAL PROCESSING (LIF dynamics with stochastic channels, spikes)
  ↓
INTERNAL STATE (working memory, attention, concept formation)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity, novelty)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural, short-term)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Enhanced Neural Dynamics Models

Phase 6 now supports three enhanced dynamics models:

### 1. Integrate-And-Fire (Enhanced)
- Standard LIF with stochastic channel noise
- Synaptic adaptation during refractory periods
- Short-term plasticity with depression/facilitation

### 2. Hodgkin-Huxley (Enhanced)
- Full biophysical model of ion channels
- Sodium, potassium, and leak conductances
- Gating variable dynamics

### 3. FitzHugh-Nagumo (Enhanced)
- Simplified excitable system for pattern generation
- Bistable switch behavior
- Oscillatory dynamics

## Enhanced Configuration Options

### Basic Configuration
- `neuron_count`: Number of neurons (default: 500)
- `max_steps`: Maximum simulation steps (default: 2000)
- `region_count`: Number of neural regions (default: 1)
- `connection_probability`: Connection probability (default: 0.1)

### Plasticity Configuration
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.01)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.012)
- `stdp_tau`: STDP time constant (default: 20.0)
- `synaptogenesis_rate`: Structural growth rate (default: 0.0001)
- `pruning_rate`: Structural pruning rate (default: 0.00001)

### Memory Configuration
- `replay_interval`: Memory replay frequency (default: 100)
- `consolidation_interval`: Memory consolidation frequency (default: 1000)
- `working_memory_capacity`: Working memory capacity (default: neuron_count/10)
- `max_episodes`: Maximum episodic memory episodes (default: 1000)

### Development Configuration
- `enable_development`: Enable developmental system (default: true)
- `development_rate`: Overall development rate (default: 1.0)

### Enhanced Checkpointing
- `checkpoint_dir`: Directory for checkpoints (default: ./checkpoints)
- `checkpoint_interval`: Checkpoint save frequency (default: 1000)
- `max_checkpoints`: Maximum checkpoints to retain (default: 10)
- `compress_checkpoints`: Enable checkpoint compression (default: true)

## Enhanced Experiment Framework

Phase 6 includes an enhanced experiment framework with:

### Built-in Tests
- Integration verification (all systems connected)
- Memory integration testing
- Neuromodulation integration testing
- Checkpoint save/load testing
- Memory replay testing

### Configurable Experiments
- Small-scale experiments (100-500 neurons)
- Medium-scale experiments (500-2000 neurons)
- Large-scale experiments (2000-10000 neurons)
- Development-focused experiments
- Learning-focused experiments

### Results Analysis
- Detailed performance metrics
- System integration status
- Learning effectiveness measurements
- Memory efficiency analysis

## Enhanced Development Tools

### Command Line Interface Features
- Interactive experiment selection
- Configuration file support
- Custom parameter overrides
- Progress monitoring
- Result visualization

### Development Commands
```bash
# Build with specific features
make -j4 NLM_ENABLE_CHECKPOINTING=ON NLM_ENABLE_REPLAY=ON

# Run specific tests
./nlm_phase6 --integration --memory

# Profile performance
./nlm_phase6 --verbose --neurons=1000 --steps=5000

# Export experiment results
./nlm_phase6 --config=experiment.cfg --checkpoint-dir=./export
```

### Enhanced Documentation
- Comprehensive API documentation
- Usage examples for all systems
- Configuration parameter descriptions
- Performance optimization guidelines

## Enhanced License

MIT License

## Enhanced Authors

Research project — Enhanced documentation and Phase 6 improvements by AI assistant. See docs for scientific background.

## Quick Start

1. **Build**: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4`
2. **Run Demo**: `./nlm_phase6 --help` to see all options
3. **Run Basic Test**: `./nlm_phase6 --integration`
4. **Run Full Experiment**: `./nlm_phase6 --neurons=1000 --steps=5000`

## Enhanced Notes

- The enhanced Phase 6 implementation provides a more comprehensive and biologically realistic neural simulation framework
- All systems are now properly integrated and tested through the enhanced CLI
- Configuration files can be created in the `configs/` directory for custom experiments
- The enhanced framework supports both basic and advanced usage scenarios
- Performance optimizations include SIMD vectorization and parallel processing where available