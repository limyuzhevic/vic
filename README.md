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

**PHASE 2: Real Neural Computation**

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 2 Achievements

#### Neural Core
- Real LIF neuron dynamics implemented
- Event-driven spike propagation working
- STDP and Hebbian plasticity functional
- Structural plasticity for synapse creation/removal

#### Configuration
- Simple key=value configuration parser
- Command-line argument override
- Configuration validation and error handling

#### Development Foundations
- Neuron and synapse classes with proper PIMPL idiom
- Memory-safe implementation using smart pointers
- Modular architecture for future expansion

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 2 Demo
```bash
./nlm_phase2_demo
```

This runs a comprehensive integration test verifying:
- Basic neural connectivity
- Plasticity learning experiments
- Spike Timing Dependent Plasticity (STDP) verification

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
│   └── PHASE2_FINAL_AUDIT.md    # Phase 2 audit
├── src/
│   ├── core/           # Core utilities (config, logging, random)
│   ├── brain/          # Neural components (Neuron, Synapse)
│   ├── dynamics/       # Neural dynamics (LIF, synaptic)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian)
│   ├── development/    # Developmental system (synaptogenesis, pruning)
│   ├── neuromodulation/ # Neuromodulators (dopamine, reward)
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 2)
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

### Phase 3 (Planned)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Planned)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Planned)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Planned)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Current Implementation Status

### Working Components:
- ✅ Real LIF neuron dynamics (integrate-and-fire with adaptation)
- ✅ Spike propagation through network
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Basic configuration system
- ✅ Simulation clock and timing
- ✅ Learning experiments (demonstrates measurable synaptic changes)

### Known Limitations:
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- Limited visualization capabilities
- Configuration parser is simple key=value format
- Most advanced features planned for Phases 3-6

### Scientific Limitations

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
- Whether neurons can implement integrate-and-fire dynamics
- Whether plasticity rules (STDP, Hebbian) produce learning
- Whether structural plasticity can create and prune connections
- Whether the basic neural substrate can support information processing

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
