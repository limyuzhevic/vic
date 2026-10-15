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

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

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
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
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

## Documentation

NLM documentation is organized by experience level and use case:

### Getting Started

**Beginner Level**
- `easy_usage.md` - Simple examples for beginners
- `HOW_TO_USE.md` - Quick reference and common patterns

**Intermediate Level**
- `HOW_TO_USE.md` - Advanced integration examples
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration details
- `docs/EXPERIMENTS.md` - Experiment framework documentation

**Advanced Level**
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific methods and principles
- `docs/SCALING.md` - Performance optimization and scaling
- `docs/PERFORMANCE.md` - Performance benchmarks and metrics

**Research Level**
- `docs/RESEARCH_METHODS.md` - Research methodology
- `docs/BIOLOGICAL_FIDELITY.md` - Biological plausibility discussions
- `docs/LIMITATIONS.md` - Current limitations and future directions
- `docs/FINAL_LIMITATIONS.md` - Phase 6 specific limitations
- `docs/FAILURES.md` - What didn't work and why

### Documentation Structure

| Documentation Type | Purpose | Key Topics |
|-------------------|---------|------------|
| **Integration Examples** | How to use and connect systems | `HOW_TO_USE.md` |
| **System Architecture** | How the brain is designed | `docs/ARCHITECTURE.md` |
| **Scientific Background** | Why it works the way it does | `docs/SCIENCE.md` |
| **Phase 6 Results** | What Phase 6 achieved | `docs/PHASE6_FINAL_AUDIT.md` |
| **Experiments** | How to run and interpret experiments | `docs/EXPERIMENTS.md` |
| **Performance** | Speed and scalability | `docs/PERFORMANCE.md` |
| **Scaling** | Optimization techniques | `docs/SCALING.md` |
| **Limitations** | Current boundaries | `docs/LIMITATIONS.md` |
| **Research Methods** | How to study it | `docs/RESEARCH_METHODS.md` |

### Additional Resources

#### Advanced Usage
- **Working with Memory Systems**: `HOW_TO_USE.md` - Memory storage, retrieval, replay
- **Neuromodulation Control**: `HOW_TO_USE.md` - Dopamine, curiosity, novelty control
- **Prediction and Planning**: `HOW_TO_USE.md` - Using prediction for intelligent behavior
- **Development and Maturation**: `HOW_TO_USE.md` - Understanding developmental stages
- **Sleep and Memory Consolidation**: `HOW_TO_USE.md` - Replay mechanisms
- **Performance Optimization**: `HOW_TO_USE.md` - Speed and scalability
- **Multi-Agent Systems**: `HOW_TO_USE.md` - Creating competing agents

#### Configuration Files
- `configs/default.cfg` - Default NLM configuration
- `configs/large_brain.cfg` - Configuration for larger simulations
- `configs/learning.cfg` - Configuration optimized for learning
- `configs/fast.cfg` - Configuration for faster simulation

#### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Running Experiments

```bash
# Run the basic integration demo
./nlm_phase6_demo

# Run a learning experiment  
./nlm_learning_demo

# Run a sleep experiment
./nlm_sleep_demo

# Run a neuromodulation experiment
./nlm_neuromodulation_demo

# Run a prediction experiment
./nlm_prediction_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── easy_usage.md              # Simple examples for beginners
├── HOW_TO_USE.md              # Integration examples
├── docs/                     # Detailed documentation
│   ├── ARCHITECTURE.md       # System architecture
│   ├── SCIENCE.md            # Scientific background
│   ├── ROADMAP.md            # Development roadmap
│   ├── EXPERIMENTS.md        # Experiment framework
│   ├── PERFORMANCE.md        # Performance benchmarks
│   ├── SCALING.md            # Performance optimization
│   ├── LIMITATIONS.md        # Current limitations
│   ├── BIOLOGICAL_FIDELITY.md # Biological plausibility
│   ├── RESEARCH_METHODS.md   # Research methodology
│   ├── FAILURES.md           # What didn't work
│   ├── FINAL_LIMITATIONS.md  # Phase 6 limitations
│   └── PHASE6_FINAL_AUDIT.md  # Phase 6 integration results
├── src/                     # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators
│   ├── memory/             # Memory systems
│   ├── prediction/          # Prediction systems
│   ├── cognition/          # Cognitive mechanisms
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework
│   └── visualization/       # Visualization
├── tests/                   # Unit tests
└── configs/                 # Configuration files
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
