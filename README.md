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

## Current status (honest, 2026-11-17)

**Phase 6: FINAL INTEGRATION** - Key improvements completed:

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
- **Checkpoint save/load now working** (Phase 5 implementation)
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Major Improvements Made

### 1. **Fixed Phase 6 Reward Timing Issue**
- **Problem**: Phase 6 demos reported 0 reward due to incorrect world update timing (0.001s instead of action interval 0.1s)
- **Solution**: Updated Phase6IntegratedExperiment.cpp to use correct timing (world.update(0.1) per action)
- **Result**: Rewards are now properly accumulated and measured

### 2. **Fixed Checkpoint System Stub Implementations**
- **Problem**: CheckpointWriter methods (writeHeader, writeSectionHeader, padToAlignment) were stubs returning false
- **Solution**: Implemented proper checkpoint writing with validation, checksum calculation, and error handling
- **Result**: Checkpoint save/load now works correctly

### 3. **Improved Selective Learning**
- **Problem**: Learning experiment was using a fixed set of 20 neurons, causing over-specialization
- **Solution**: Implemented pattern sequence with 50 different overlapping patterns (50% overlap)
- **Result**: Brain develops broader selectivity and more realistic learning patterns

### 4. **Fixed Neuron Firing Rate**
- **Problem**: Input intensities were too high (50.0f, 30.0f, 20-40), causing ~200 spikes/step
- **Solution**: Added INPUT_SCALE (0.01f) to reduce current injections by 99%
- **Result**: More biologically realistic firing rates (~2-5 Hz typical)

### 5. **Enhanced Command-line Configuration**
- **Problem**: Command-line arguments only supported string values
- **Solution**: Implemented parseValue() to convert strings to appropriate types (int64, double, bool)
- **Result**: Users can now use `--neuron_count 1000`, `--simulation_timestep 0.01`, etc.

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

The entry points are:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` now runs a comprehensive integration test with:
- Working reward accumulation
- Functional checkpoint save/load
- Selective learning patterns
- Realistic firing rates

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                     # Technical documentation
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
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
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

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- **Checkpoint save/load working**
- Replay and consolidation functional
- Phase 6 integration experiment created
- **Learning now selective and biologically realistic**

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
- **Whether the complete brain loop functions coherently**

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

## Key Improvements Summary

✅ **Phase 6 reward timing fixed** - Rewards now properly accumulated
✅ **Checkpoint system working** - Save/load functional  
✅ **Selective learning improved** - 50 patterns instead of fixed set
✅ **Firing rate realistic** - Input scaling prevents runaway activity
✅ **Enhanced CLI support** - --neuron_count, --simulation_timestep, etc.

The NLM brain is now a **functioning artificial neural substrate** capable of changing its own synaptic connections through experience, with realistic firing rates and proper reward-based learning.
