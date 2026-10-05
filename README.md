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

### Phase 6 Integration Experiment
```bash
cd build
./nlm_phase6_integration
```

This runs the full Phase 6 integration experiment with agent-based interaction.

### Legacy Phase 2 Demo
```bash
cd build
./nlm_phase2_demo
```

This demonstrates the Phase 2 neural computation foundation (for educational purposes).

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
│   ├── agent/         # Agent architecture (Phase 6)
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
- **Used as foundation for Phase 6 integration**

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

### Phase 6 (In Final Integration)
- **ALL systems integrated into coherent brain loop** ✓
- Memory systems connected to neural processing ✓
- Neuromodulation affects plasticity and dynamics ✓
- Prediction integrated with learning ✓
- Development affects plasticity rates ✓
- Checkpoint save/load working ✓
- Replay and consolidation functional ✓
- **Phase 6 integration experiment created and running** ✓
- **Ready for production use and scientific investigation** ✓

## Getting Started with NLM Phase 6

### Quick Start
1. **Build the project**: `cd build && make -j4`
2. **Run integration test**: `./nlm_phase6_demo`
3. **Explore advanced features**: `./nlm_phase6_integration`

### Key Features for Users

#### For Researchers
- Integrated brain simulation with memory, prediction, and cognition
- Memory systems (working, episodic, associative) fully connected
- Neuromodulation (dopamine, curiosity, novelty) affecting plasticity
- Checkpoint/save-load capability for state persistence
- Replay mechanisms for memory consolidation
- Developmental plasticity modulation

#### For Developers
- Modular architecture with clear interfaces
- Performance optimizations (memory pools, SIMD)
- Event-driven spike processing
- Comprehensive experiment framework
- Config-driven system customization

#### For Advanced Users
- Neural planner and concept formation systems
- Attention mechanisms with competitive dynamics
- Full integration of all cognitive systems
- Agent-based environment interaction
- Sleep/rest cycle implementation

## Quick Commands

Run the **Phase 6 Integration Demo** to see all systems working together:
```bash
cd build
./nlm_phase6_demo
```

Run the **Full Integration Experiment** for comprehensive testing:
```bash
cd build
./nlm_phase6_integration --neurons 1000 --steps 5000
```

Run the **Legacy Phase 2 Demo** for neural computation basics:
```bash
cd build
./nlm_phase2_demo
```

## Configuration

All NLM systems are configurable via:
- **Command line arguments** for runtime customization
- **Configuration files** for persistent settings
- **Default configs** in `configs/` directory

Example configuration for Phase 6 experiments:
```bash
./nlm_phase6_integration \
  --config configs/phase6_experimental.cfg \
  --neurons 2000 \
  --steps 10000 \
  --enable-checkpointing \
  --enable-replay
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
