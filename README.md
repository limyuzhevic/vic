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

**PHASE 2: REAL NEURAL COMPUTATION**

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

This is the primary demonstration showing real neural computation.

### Phase 2 Achievements

#### Neural Core
- Real LIF neuron dynamics implementation
- Event-driven spike propagation
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

## System Components

### 1. Neural Core (Phase 2 - main.cpp)
- **Neuron class**: LIF dynamics implementation
- **Synapse class**: Synaptic transmission
- **Spike system**: Event-driven spike processing
- **Plasticity rules**: STDP, Hebbian, structural plasticity

### 2. Integrated Artificial Brain (Phase 6 - Brain.cpp)
The complete integrated brain with all systems:
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Prediction System**: Predictive processing and error tracking
- **Cognition Systems**: Neural planner, concept formation, attention
- **Development**: Developmental stages affecting plasticity
- **Checkpoint System**: Save/load functionality
- **Replay and Consolidation**: Memory replay and strengthening

### 3. World Interaction
- **Sensory Processing**: Vision, audio, internal signals
- **Motor Control**: Action selection and execution
- **Environment**: Grid world simulation

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 2 Demo (Neural Computation)
```bash
./nlm
```

This runs a comprehensive neural computation demo showing:
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment demonstrating measurable changes

### Phase 6 Integration Demo
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected:
- Working Memory: Connected
- Episodic Memory: Connected
- Neuromodulation: Connected
- Prediction: Connected
- Development: Connected
- Checkpointing: Working
- Replay: Working

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
│   ├── brain/          # Neural components (Phase 2 & 6)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (Phase 6)
│   ├── memory/        # Memory systems (Phase 6)
│   ├── prediction/     # Prediction systems (Phase 6)
│   ├── cognition/      # Cognitive mechanisms (Phase 6)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
├── configs/
├── python/
│   ├── bindings.cpp    # Python bindings
│   └── CMakeLists.txt
├── experiment_manager.py    # Experiment management system
└── DOCUMENTATION.md
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete - DEMONSTRATION)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 6 (Complete - FINAL INTEGRATION)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
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

Phase 2 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 2 DOES investigate:
- Whether real neural computation works correctly
- Whether spiking neurons can implement plasticity rules
- Whether structural plasticity can modify network connectivity

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

## Python Interface

See `python/bindings.cpp` for the complete Python API, or see `DOCUMENTATION.md` for detailed usage.

## Experiment Management

See `experiment_manager.py` for comprehensive experiment management capabilities.

## License

MIT

## Authors

Research project — See docs for scientific background.
