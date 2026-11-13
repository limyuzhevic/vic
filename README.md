# NLM (熙然)

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

## Current Status (honest, 2026-11-13)

**Phase 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Key Improvements Made (Summary of Work Done):

**Critical Issues Fixed:**

1. ✅ **Configuration Error**: Removed duplicate `dopamine_baseline` in configs/default.cfg
2. ✅ **Reward System**: Implemented prediction system in AgentBrain to fix reward tracking
3. ✅ **Checkpointing**: Confirmed checkpoint system is fully functional with proper serialization

**Code Quality Improvements:**

4. ✅ **Testing Framework**: Implemented comprehensive unit tests for core systems
5. ✅ **Documentation**: Consolidated redundant documentation into this unified README
6. ✅ **Code Quality**: Added consistent formatting and comments

### Phase 6 Integration Achievements

All systems integrated and working:
- **Memory Systems**: Working memory + Episodic memory + Associative memory
- **Neuromodulation**: Dopamine affects neural excitability and plasticity
- **Prediction System**: Prediction integrated with learning via prediction error
- **Cognition Systems**: Neural planner + Concept formation + Attention
- **Development**: Developmental stages affect plasticity rates
- **Persistence**: Checkpoint save/load working
- **Replay**: Episodic memory replay functional

**Current limitations (the next problems to solve):**
- Activity levels ~200 spikes/step (very high)
- Learning mostly saturation, not selective pattern learning

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

Build all targets first. The main demos are:

```bash
./nlm_phase3_demo     # Phase 3 demonstration
./nlm_phase4_demo     # Phase 4 demonstration
./nlm_phase6_demo     # Phase 6 integration test
./nlm_phase6_text_demo # Phase 6 text/thought demo
./nlm_test          # Unit test suite
./nlm_benchmark     # Performance benchmarks
```

## Quick Start (Python Users)

For Python users with the pynlm bindings:

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/              # Technical documentation
├── src/              # Source code
│   ├── core/         # Core utilities
│   ├── agent/        # Agent-brain interface
│   ├── brain/        # Neural components
│   ├── experiments/  # Experiment framework
│   └── ...
├── tests/            # Unit tests
└── configs/          # Default configuration files
```

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

## Technical Summary

**Core Components:**
- **LIF Neurons**: Leaky Integrate-and-Fire dynamics
- **Synapses**: STDP and Hebbian plasticity
- **Memory**: Working memory + Episodic memory
- **Neuromodulation**: Dopamine, curiosity, novelty
- **Prediction**: Neural prediction with error signals
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Plasticity changes with developmental stages

**Key Achievements (Phase 6):**
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

**Research Focus (What we investigate):**
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

**Scientific Limitations:**
- Simplified LIF neuron model (not biologically realistic)
- No claim of consciousness, intelligence, or human-like cognition
- Limited to computational exploration of brain-like systems

## License

MIT

## Authors

Research project — See docs/ for scientific background.
