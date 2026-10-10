# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Documentation

### Quick Start

For beginners, start with:
1. **[easy_usage.md](easy_usage.md)** - Get started quickly with simple examples
2. **[HOW_TO_USE.md](HOW_TO_USE.md)** - Complete technical documentation with all details
3. **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** - Understand how it all works

### What You'll Find

- **easy_usage.md**: Beginner-friendly guide with step-by-step examples
- **HOW_TO_USE.md**: Comprehensive documentation with code examples, configuration, and troubleshooting
- **docs/SCIENCE.md**: Scientific background and brain theory
- **docs/EXPERIMENTS.md**: Experiment descriptions and results

## Project Overview

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

## Documentation Quick Reference

### For New Users
- **Start here:** `easy_usage.md` - 15-minute quick start guide
- **Build it:** `HOW_TO_USE.md` - C++ and Python installation
- **Learn concepts:** `docs/SCIENCE.md` - Brain theory background
- **See results:** `docs/EXPERIMENTS.md` - What we've discovered

### For Developers
- **Architecture:** `docs/ARCHITECTURE.md` - System design
- **Code examples:** `HOW_TO_USE.md` - Complete API documentation
- **Troubleshooting:** `HOW_TO_USE.md` - Common issues and solutions

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── easy_usage.md              # Quick start guide
├── HOW_TO_USE.md              # Complete documentation
├── docs/
│   ├── ARCHITECTURE.md       # System architecture
│   ├── SCIENCE.md           # Scientific background
│   ├── ROADMAP.md           # Development roadmap
│   ├── EXPERIMENTS.md       # Experiment results
│   ├── LIMITATIONS.md       # Current limitations
│   └── PHASE6_FINAL_AUDIT.md # Phase 6 audit
├── src/
│   ├── core/                # Core utilities
│   ├── brain/               # Neural components (integrated)
│   ├── dynamics/            # Neural dynamics
│   ├── plasticity/          # Plasticity rules
│   ├── development/         # Developmental system
│   ├── neuromodulation/     # Neuromodulators (integrated)
│   ├── memory/             # Memory systems (integrated)
│   ├── prediction/          # Prediction systems (integrated)
│   ├── cognition/          # Cognitive mechanisms (integrated)
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework (Phase 6)
│   └── visualization/      # Visualization
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

## Learning Path

### For Absolute Beginners
1. Read `easy_usage.md` for quick start
2. Try the simple examples
3. Understand the brain-world-agent connection

### For Intermediate Users
1. Read `HOW_TO_USE.md` for complete documentation
2. Try the advanced examples
3. Experiment with different configurations

### For Advanced Users
1. Study `docs/ARCHITECTURE.md` for deep understanding
2. Read `docs/EXPERIMENTS.md` for research insights
3. Contribute to the project

## License

MIT

## Authors

Research project — See docs for scientific background.
