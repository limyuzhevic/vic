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

**PHASE 4: CORE BRAIN SYSTEMS IMPLEMENTED**

Phase 4 focuses on implementing the core computational brain systems that enable learning and adaptation. This phase implements:

### Core Neuromodulatory Systems
- **Dopamine**: Reward prediction error signaling and plasticity modulation
- **Reward**: Temporal difference learning and value estimation
- **Prediction Error**: Credit assignment for learning
- **Novelty**: Exploration and intrinsic motivation signals
- **Curiosity**: Adaptive exploration behavior

### Learning Systems
- **Plasticity Rules**: Hebbian covariance learning with synaptic normalization
- **Prediction System**: Hierarchical predictive coding and forward models
- **Memory System**: Experience storage and consolidation

### Neural Dynamics
- **Integrate-and-Fire Neurons**: Realistic LIF neuron dynamics
- **Synaptic Transmission**: Event-driven spike propagation with delays
- **Network Integration**: All systems connected into a coherent brain loop

## Phase 4 Integration Achievements

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Learning System Integration
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented
- Hebbian and STDP plasticity rules implemented
- Temporal difference learning for reward prediction

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 4 Demo (Core Integration Test)
```bash
./nlm
```

This runs the core integration test verifying all brain systems are connected.

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
│   └── PHASE4_CORE_AUDIT.md    # Phase 4 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules (implemented)
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (implemented)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (implemented)
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
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

### Phase 4 (IN PROGRESS)
- **Neuromodulation System**: Dopamine, Reward, PredictionError, Novelty, Curiosity
- **Learning Systems**: Hebbian covariance learning, Predictive coding
- **Memory Integration**: All memory systems connected to neural processing
- **Development Integration**: Age-dependent plasticity rates
- **Core Integration**: All systems working together as a unified brain

### Phase 5 (Planned)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Planned)
- Advanced cognitive capabilities
- Social learning and theory of mind
- Continual learning from lifelong experience
- Full biological realism

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

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
