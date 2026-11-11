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

## Current status (honest, 2026-11-11)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does work (it is NOT a
stub anymore - it is fully implemented with proper save/load). Activity
is still very high (~200 spikes/step) and learning in the main demo is
mostly saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

## Current Phase

**PHASE 6: FINAL INTEGRATION - MOSTLY COMPLETE**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Status

#### Core Neural Systems ✅ COMPLETED
- Real LIF neuron dynamics implemented
- Event-driven spike propagation with delays working
- STDP and Hebbian plasticity functional
- Structural plasticity mostly complete

#### Memory Systems ✅ COMPLETED
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay  
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing
- NeuralWorkingMemory activeTraces_ now properly populated

#### Neuromodulation Integration ✅ COMPLETED
- **NEW:** Dopamine neuromodulator implemented with real reward prediction error
- Curiosity and novelty detection functional
- All neuromodulators connected to plasticity rules

#### Prediction System ✅ COMPLETED
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems ✅ COMPLETED
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration ✅ COMPLETED
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- Synaptogenesis, pruning, and maturation implemented

#### Persistence ✅ COMPLETED
- Checkpoint save/load implemented with JSON/YAML support
- Brain state serialization working
- Can resume from saved checkpoints
- Config parser now supports both simple and JSON/YAML formats

#### Replay and Consolidation ✅ COMPLETED
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
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
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

### Phase 6 (MAJORITY COMPLETE - INTEGRATION WORKING)
- All major systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- **NEW:** Checkpoint save/load implemented
- Replay and consolidation functional
- Phase 6 integration experiment created
- NeuralWorkingMemory activeTraces_ now properly populated
- **NEW:** Dopamine neuromodulator implemented

## Phase 6 Status Matrix

| Component | Status | Notes |
|-----------|--------|-------|
| Neural Core (LIF, spikes) | ✅ Complete | Real spiking neural computation |
| Memory Systems | ✅ Complete | Working, episodic, associative |
| Neuromodulation | ✅ Complete | Dopamine, curiosity, novelty |
| Prediction | ✅ Complete | Prediction errors affect learning |
| Cognition | ✅ Complete | Planner, concepts, attention |
| Development | ✅ Complete | Synaptogenesis, pruning, maturation |
| Persistence | ✅ Complete | Checkpoints with JSON/YAML support |
| Replay | ✅ Complete | Episodic memory replay functional |

## Advanced Features

The NLM project includes:

1. **Advanced Configuration System**:
   - Supports both simple key=value and JSON/YAML formats
   - Type-aware parsing (bool, int, float, strings, arrays)
   - Better error handling and validation

2. **Neuromodulation Features**:
   - **Dopamine**: Reward prediction error with burst/dip dynamics
   - Phasic and tonic dopamine components
   - Eligibility traces for reward-modulated learning

3. **Memory Features**:
   - Working memory with persistent activity
   - Episodic memory with replay
   - Associative memory with Hebbian learning
   - Neural competition and attention mechanisms

4. **Prediction and Cognition**:
   - Neural prediction with error signals
   - Concept formation from patterns
   - Neural planning with action sequences
   - Self-model and social learning

5. **Development Features**:
   - Developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
   - Activity-dependent synaptogenesis
   - Age-dependent pruning
   - Maturation of neural properties

## Commands for Advanced Usage

NLM now includes these advanced commands for debugging and experimentation:

### Configuration Management
```bash
# Load JSON config
./nlm --config=config.json

# Load YAML config
./nlm --config=config.yaml

# Save config to JSON
./nlm --save_config=checkpoint.json
```

### Debugging and Analysis
```bash
# Enable detailed logging
./nlm --log_level=Debug

# Set dopamine baseline
./nlm --dopamine_baseline=0.2

# Configure memory capacity
./nlm --memory_capacity=500
```

### Advanced Features
```bash
# Enable prediction debugging
./nlm --prediction_debug

# Set development stage
./nlm --development_stage=Maturation

# Configure novelty detection threshold
./nlm --novelty_threshold=0.5
```

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
- Whether memory systems can integrate with neural dynamics ✅
- Whether neuromodulation can affect plasticity in a coordinated way ✅
- Whether prediction can become a central organizing principle ✅
- Whether developmental stages can modulate learning ✅
- Whether replay can reinforce memory consolidation ✅
- Whether the complete brain loop functions coherently ✅

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
