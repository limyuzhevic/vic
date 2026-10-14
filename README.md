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

### Main NLM Executable (Phase 6 Integration Demo)

The primary entry point is now the **Phase 6 integration demo** with comprehensive command-line interface support:

```bash
./nlm [MODE] [OPTIONS]
```

### Available Demo Modes

```bash
./nlm                          # Phase 6 integration demo (default)
./nlm quick                    # Quick basic testing mode
./nlm demo                    # Full Phase 6 integration demonstration
./nlm advanced                 # Comprehensive analysis mode
./nlm benchmark               # Performance benchmarking mode
./nlm phase3                   # Phase 3 cognition and prediction tests
./nlm phase4                   # Phase 4 advanced cognitive integration tests
./nlm phase2                   # Basic Phase 2 neural computation tests
```

### Common Command-Line Options

```bash
--help                     # Show detailed help information
--quiet                    # Reduce logging output for performance
--config <file>            # Load configuration from file
--neurons <count>          # Set number of neurons (default: 500)
--steps <count>            # Set simulation steps (default: 1000)
--advanced                 # Enable advanced features and debugging
```

### Examples

```bash
./nlm                                      # Run Phase 6 integration demo
./nlm quick --steps 500                    # Run quick test with 500 steps
./nlm demo --neurons 2000 --quiet          # Run demo with 2000 neurons, quiet mode
./nlm advanced --steps 5000                 # Run comprehensive analysis
./nlm benchmark --neurons 2000 --steps 5000 # Performance benchmark
```

### Legacy Executables (For Backward Compatibility)

The following executables are also available for specific demonstrations:

```bash
./nlm_legacy_phase2        # Legacy Phase 2 demo (for compatibility)
./nlm_legacy_phase3        # Legacy Phase 3 demo (for compatibility)
./nlm_legacy_phase4        # Legacy Phase 4 demo (for compatibility)
./nlm_phase6_demo          # Legacy Phase 6 demo (for compatibility)
```

**Note:** The main `nlm` executable now provides the enhanced Phase 6 integration demo with command-line interface. Use legacy executables only if you need specific older functionality.

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
│   ├── core/           # Core utilities (Config, Logger, etc.)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics (LIF neurons, spike propagation)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, structural)
│   ├── development/    # Developmental system (maturation, critical periods)
│   ├── neuromodulation/ # Neuromodulation systems (dopamine, curiosity)
│   ├── memory/         # Memory systems (working, episodic, associative)
│   ├── prediction/     # Prediction systems with error signals
│   ├── cognition/      # Cognitive mechanisms (planner, concept formation)
│   ├── sensory/        # Sensory processing (vision, audio, internal signals)
│   ├── motor/          # Motor system and action selection
│   ├── environment/    # Environment interface (world simulation)
│   ├── experiments/    # Experiment framework (Phase 3-6)
│   └── visualization/  # Visualization interfaces
├── tests/              # Unit tests for all components
└── configs/            # Default configuration files
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
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 4 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 5 (Complete)
- All Phase 5 experiments completed
- Phase 5 advanced experiments with complex cognitive tasks

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

## Additional Resources

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 final audit report

## Configuration Options

The NLM system supports extensive configuration through command-line arguments and configuration files. Key options include:

### Brain Configuration
- `brain.neuron_count`: Number of neurons (default: 500)
- `brain.synapse_density`: Synapse connectivity density (default: 0.1)
- `brain.connection_probability`: Probability of connection between neurons (default: 0.05)

### Memory Configuration
- `memory.working.enable`: Enable working memory (default: true)
- `memory.episodic.enable`: Enable episodic memory (default: true)
- `memory.associative.enable`: Enable associative memory (default: true)
- `memory.capacity`: Memory capacity (default: 1000)

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP (default: true)
- `plasticity.hebbian.enable`: Enable Hebbian learning (default: true)
- `plasticity.structural.enable`: Enable structural plasticity (default: true)
- `plasticity.stdp.learning_rate`: STDP learning rate (default: 0.001)

### Neuromodulation Configuration
- `neuromod.dopamine.enable`: Enable dopamine modulation (default: true)
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration (default: true)
- `neuromod.novelty.enable`: Enable novelty detection (default: true)

### Development Configuration
- `development.enabled`: Enable development system (default: true)
- `development.stages`: Number of developmental stages (default: 5)

### Advanced Features
The Phase 6 integration demo includes advanced features:
- **Checkpointing**: Save and restore brain state
- **Replay**: Re-experience important episodes
- **Development**: Brain maturation and adaptation
- **Comprehensive Metrics**: Detailed performance statistics
- **Error Recovery**: Enhanced error handling and recovery

## Performance Notes

- The Phase 6 integration demo is computationally intensive
- Recommended: 8+ CPU cores, 16+ GB RAM
- Memory usage scales with neuron count (typical: 1000-5000 neurons)
- Checkpointing requires additional disk space for saving brain state

## Getting Help

1. **Documentation**: Read the docs/ directory for detailed information
2. **Examples**: Use `./nlm help` for command-line help
3. **Source Code**: Study the src/ directory for implementation details
4. **Community**: Join the NLM research community for discussions

## Quick Start for Research Users

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration demo
./nlm demo --neurons 1000 --steps 2000

# Run quick test for rapid validation
./nlm quick --steps 500

# Run advanced analysis
./nlm advanced --neurons 2000 --steps 5000

# Run performance benchmark
./nlm benchmark --neurons 2000 --steps 5000 --quiet
```

The NLM project is continuously evolving. Check the documentation for the latest features and improvements.