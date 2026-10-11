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

### Build Notes

The project has been refactored with improved code organization:

**Modular Source Structure:**
- **Python bindings**: Split from monolithic `python/bindings.cpp` into focused files
  - `python/bindings.cpp` (45 lines) - Main orchestrator
  - `python/bindings/binding_headers.h` (22 lines) - Header file
  - `python/bindings/` directory - 8 focused binding files

- **C++ main**: Split from monolithic `src/main.cpp` into focused files
  - `src/main.cpp` (160 lines) - Clean main coordination
  - `src/banner.*` files for banner functionality
  - `src/learning_experiment.h` - LearningExperiment struct
  - `src/connectivity_test.*` files for basic connectivity test
  - `src/plasticity_test.*` files for plasticity experiment
  - `src/stdp_test.*` files for STDP verification

This refactoring significantly improves maintainability and allows for easier testing and modification of individual components while maintaining full backward compatibility.

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Code Organization Improvements

### Python Bindings Refactoring
The monolithic `python/bindings.cpp` (425 lines) has been refactored into a modular structure:

- **`python/bindings.cpp`** (45 lines) - Main orchestrator file
- **`python/bindings/binding_headers.h`** (22 lines) - Centralized header file
- **`python/bindings/` directory** - 8 focused binding files:
  - `action.cpp` - Action-related Python bindings
  - `action_result.cpp` - Action result bindings
  - `brain.cpp` - Brain interface bindings
  - `config.cpp` - Configuration bindings
  - `enums.cpp` - Enum type bindings
  - `module_functions.cpp` - Module initialization functions
  - `neuron_types.cpp` - Neuron type bindings
  - `perception.cpp` - Perception-related bindings
  - `sensory_input.cpp` - Sensory input bindings
  - `simple_world.cpp` - Simple world bindings
  - `world_objects.cpp` - World object bindings

**Benefits:**
- Improved maintainability with smaller, focused files
- Easier debugging and testing of individual bindings
- Better code organization and readability
- Separation of concerns

### C++ Main.cpp Refactoring
The monolithic `src/main.cpp` (449 lines) has been refactored into a modular structure:

- **`src/main.cpp`** (160 lines) - Clean main coordination
- **`src/banner.*`** - Banner functionality files
- **`src/learning_experiment.h`** - LearningExperiment struct
- **`src/connectivity_test.*`** - Basic connectivity test
- **`src/plasticity_test.*`** - Plasticity experiment
- **`src/stdp_test.*`** - STDP verification

**Benefits:**
- Reduced complexity in main entry point
- Isolated functionality in dedicated files
- Easier testing and modification of individual experiments
- Better code organization and reusability

This refactoring significantly improves code maintainability, testability, and developer productivity by breaking down large, monolithic files into smaller, focused components that can be easily understood, modified, and tested independently.

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
├── python/
│   ├── bindings.cpp               # Main Python bindings orchestrator (45 lines)
│   ├── bindings/                  # Modular Python bindings (8 files)
│   │   ├── binding_headers.h       # Headers for Python bindings
│   │   ├── action.cpp
│   │   ├── action_result.cpp
│   │   ├── brain.cpp
│   │   ├── config.cpp
│   │   ├── enums.cpp
│   │   ├── module_functions.cpp
│   │   ├── neuron_types.cpp
│   │   ├── perception.cpp
│   │   ├── sensory_input.cpp
│   │   ├── simple_world.cpp
│   │   └── world_objects.cpp
│   └── ... (more Python files)
├── src/
│   ├── main.cpp                    # Clean main orchestration (160 lines)
│   ├── banner.*                    # Banner functionality
│   ├── connectivity_test.*          # Basic connectivity test
│   ├── learning_experiment.h        # LearningExperiment struct
│   ├── plasticity_test.*            # Plasticity experiment
│   ├── stdp_test.*                  # STDP verification
│   ├── core/                       # Core utilities
│   ├── brain/                      # Neural components (integrated)
│   ├── dynamics/                   # Neural dynamics
│   ├── plasticity/                 # Plasticity rules
│   ├── development/                # Developmental system
│   ├── neuromodulation/             # Neuromodulators (integrated)
│   ├── memory/                     # Memory systems (integrated)
│   ├── prediction/                  # Prediction systems (integrated)
│   ├── cognition/                  # Cognitive mechanisms (integrated)
│   ├── sensory/                    # Sensory processing
│   ├── motor/                      # Motor system
│   ├── environment/                # Environment interface
│   ├── experiments/                # Experiment framework (Phase 6)
│   └── visualization/               # Visualization
├── tests/
│   ├── test_*.cpp                   # Unit tests (split from monolithic tests)
│   └── ...
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
