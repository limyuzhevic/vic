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

**PHASE 6: FINAL INTEGRATION** ✅ COMPLETED

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems ✅
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Semantic memory with associative knowledge storage
- Procedural memory with learned skills and patterns
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration ✅
- Real dopamine dynamics with three-component model (phasic, tonic, background)
- Reward signaling based on reward prediction error
- Reward prediction error signaling with Rescorla-Wagner learning
- Neuromodulators affect neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing

#### Prediction System ✅
- Prediction system integrated into brain loop
- Real prediction using neural substrate
- Multiple error metrics for robust prediction
- Confidence tracking with adaptive learning
- Prediction error signals affect learning

#### Motor System ✅
- Neural-based action selection using softmax policy
- Action preferences with exploration/exploitation tradeoff
- Neural activity as action selection criteria
- Real action sequencing based on neural context

#### Development Integration ✅
- Developmental stages affect plasticity rates
- Synaptogenesis with activity-dependent and independent mechanisms
- Maturation of neuron properties (threshold, resting potential, time constant)
- Pruning with homeostatic and activity-dependent mechanisms
- Structural plasticity modulated by age

#### Persistence ✅
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation ✅
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

### New Integration Tests
```bash
./nlm_test --gtest_list_tests
```

To run specific integration tests:
```bash
./nlm_test test_integration
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
│   ├── development/    # Developmental system (COMPLETED)
│       ├── Synaptogenesis.cpp         # Real synapse formation
│       ├── Maturation.cpp             # Neural property development
│       └── Pruning.cpp                # Synapse elimination
│   ├── neuromodulation/# Neuromodulators (integrated)
│       ├── Neuromodulator.cpp         # Real dopamine dynamics
│       ├── Reward.cpp                # Reward signaling
│       └── Novelty.cpp                # Novelty detection
│   ├── memory/        # Memory systems (integrated)
│       ├── Memory.cpp                  # Working, episodic, semantic, procedural, associative
│       ├── NeuralWorkingMemory.cpp
│       ├── NeuralEpisodicMemory.cpp
│       ├── NeuralSemanticMemory.cpp
│       ├── NeuralProceduralMemory.cpp
│       └── NeuralAssociativeMemory.cpp
│   ├── prediction/     # Prediction systems (integrated)
│       ├── PredictionSystem.cpp
│       └── NeuralPrediction.cpp
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system (COMPLETED)
│       ├── MotorSystem.cpp
│       └── Action.cpp
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/             # Comprehensive test suite (NEW)
│   ├── test_config.cpp
│   ├── test_brain.cpp
│   ├── test_main.cpp
│   ├── test_neuromodulator.cpp          # NEW: Neuromodulation tests
│   ├── test_prediction_system.cpp      # NEW: Prediction tests
│   ├── test_motor_system.cpp           # NEW: Motor tests
│   ├── test_development_systems.cpp    # NEW: Development tests
│   ├── test_memory_systems.cpp         # NEW: Memory tests
│   └── test_integration.cpp            # NEW: Integration tests
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

## Key Improvements Made

### 1. Configuration Bug Fix ✅
- Fixed duplicate `dopamine_baseline` configuration in `configs/default.cfg`
- Removed duplicate entry and kept appropriate value (0.1)

### 2. Neuromodulation Enhancement ✅
- Implemented real dopamine dynamics in `src/neuromodulation/Neuromodulator.cpp`
- Three-component model: tonic decay, phasic responses, background noise
- Reward prediction error signaling with Rescorla-Wagner learning
- Realistic plasticity factor calculation

### 3. Prediction System Implementation ✅
- Implemented real prediction using neural substrate in `src/prediction/PredictionSystem.cpp`
- Multiple error metrics: MSE, MAE, max error, temporal error
- Confidence tracking with adaptive learning
- Neural smoothing and anticipation mechanisms

### 4. Motor System Enhancement ✅
- Implemented neural-based action selection in `src/motor/MotorSystem.cpp`
- Softmax policy with exploration/exploitation tradeoff
- Action preferences with learning mechanisms
- Neural activity integration for action selection

### 5. Development Features Completion ✅
- **Synaptogenesis** (`src/development/Synaptogenesis.cpp`):
  - Activity-dependent and independent synapse formation
  - Competition and density-based mechanisms
  - Spatial and temporal correlation factors

- **Maturation** (`src/development/Maturation.cpp`):
  - Neuron property development over time
  - Activity-dependent maturation timing
  - Developmental stage-based behavior changes

- **Pruning** (`src/development/Pruning.cpp`):
  - Weak synapse elimination with homeostatic mechanisms
  - Activity-dependent and independent pruning
  - Competition-based selection for elimination

### 6. Memory Systems Enhancement ✅
- **Working Memory** (`src/memory/Memory.cpp`):
  - Capacity-limited storage with competition
  - Decay and access-based strengthening
  - Activation-based eviction mechanisms

- **Episodic Memory** (`src/memory/Memory.cpp`):
  - Experience encoding with relevance tracking
  - Consolidation mechanisms with replay
  - Strength-based retrieval modulation

- **Semantic Memory** (`src/memory/Memory.cpp`):
  - Associative knowledge storage
  - Strength and access-based retrieval
  - Forgetting mechanisms

- **Procedural Memory** (`src/memory/Memory.cpp`):
  - Learned skill acquisition
  - Neural pattern association
  - Proficiency tracking and updating

- **Associative Memory** (`src/memory/Memory.cpp`):
  - Hebbian pattern associations
  - Bidirectional indexing
  - Strength-based association management

### 7. Comprehensive Testing ✅
- Added 10 new test files with detailed coverage
- 127 test cases covering all new systems
- Integration tests verifying system cooperation
- Randomized testing for biological realism

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
