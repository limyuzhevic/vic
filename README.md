# NLM — 熙然

**An Experimental Artificial Developmental Brain**

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Improved Build System for Phase 6 Integration

### Modern CMake Configuration
The project now uses modern CMake practices with improved organization and optional targets for different phases.

### Available Executables

**Phase 6 Integration Demo (nlm_phase6_demo)**
- Comprehensive test of all integrated brain systems
- Verifies memory, neuromodulation, prediction, and development integration
- Tests checkpoint save/load functionality
- Tests memory replay and consolidation systems
- Recommended for initial system verification

**Basic Neural Computation (nlm)**
- Phase 2: Real spiking neural computation with LIF neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity

**World Interaction (nlm_phase3_demo)**
- Phase 3: World interaction and action execution
- Demonstrates experience -> neural activity -> action -> consequence -> learning

**Advanced Integration (nlm_phase4_demo)**
- Phase 4: Advanced cognition and prediction systems
- Includes concept formation, planning, and attention

**Testing (nlm_test)**
- Unit tests for all core components
- Validates neuron dynamics, synapses, and plasticity rules

### Build Instructions

#### Simple Build (Recommended for Phase 6)

```bash
# Create build directory
mkdir build
cd build

# Configure with Phase 6 focus
 cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE6_TESTS=ON

# Build all targets
 make -j4

# Run Phase 6 integration demo
 ./nlm_phase6_demo
```

#### Build with All Phases

```bash
mkdir build
cd build
cmake .. -DENABLE_ALL_PHASES=ON -DCMAKE_BUILD_TYPE=Release
make -j4

# Run integration tests
 ./nlm_phase6_demo

# Run basic neural computation tests
 ./nlm

# Run world interaction tests
 ./nlm_phase3_demo
```

#### Custom Configuration

```bash
# Build with custom configuration
mkdir build
cd build
cmake .. \
    -DNEURON_COUNT=1000 \
    -DREGION_COUNT=2 \
    -DPLASTICTY_MODE="enhanced" \
    -DLOG_LEVEL=DEBUG

make -j4
```

### Command Line Options for Phase 6 Demo

```bash
nlm_phase6_demo [options]

Options:
  --config <file>      Configuration file (default: configs/default.cfg)
  --neurons <count>    Number of neurons (default: 500)
  --steps <count>      Maximum simulation steps (default: 2000)
  --no-checkpoint      Disable checkpoint testing
  --no-replay          Disable replay testing
  --no-development     Disable development system
  --verbose            Enable verbose logging
  --quiet              Suppress non-essential output

Examples:
  nlm_phase6_demo --config custom.cfg --neurons 1000
  nlm_phase6_demo --no-checkpoint --no-replay
  nlm_phase6_demo --verbose --quiet  # Conflicting options (last wins)
```

### Configuration File Options

**Phase 6 Specific Settings**
- `phase6_neuron_count`: Number of neurons for Phase 6 (default: 500)
- `phase6_max_steps`: Maximum simulation steps (default: 2000)
- `phase6_enable_checkpointing`: Enable checkpoint testing (default: true)
- `phase6_enable_replay`: Enable replay testing (default: true)
- `phase6_enable_development`: Enable development system (default: true)
- `phase6_checkpoint_path`: Path for checkpoint files (default: ./checkpoint_test.bin)
- `phase6_replay_interval`: Memory replay interval (default: 100)
- `phase6_consolidation_interval`: Memory consolidation interval (default: 1000)

**System Integration**
The improved configuration file now properly defines the complete Phase 6 integration setup with:

1. **Memory Systems**
   - Working memory for transient active information
   - Episodic memory for experience storage
   - Associative memory for pattern associations

2. **Neuromodulation**
   - Dopamine for reward prediction error
   - Curiosity for exploration motivation
   - Novelty detection for surprise signals

3. **Prediction**
   - Forward models for sensory prediction
   - Prediction error computation
   - Confidence tracking

4. **Cognition**
   - Neural planner for action sequence evaluation
   - Concept formation from experience patterns
   - Attentional selection with competitive dynamics

5. **Development**
   - Developmental stages affecting plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

### Build Status Summary

**Phase 6: FINAL INTEGRATION** - ✅ COMPLETE
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

**Previous Phases:**
- Phase 1 (Complete): Project skeleton and interfaces
- Phase 2 (Complete): Real neural computation
- Phase 3 (Complete): World interaction
- Phase 4 (Complete): Advanced cognition
- Phase 5 (Complete): Performance optimizations

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

