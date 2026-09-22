# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start (Python Users)

If you're looking to use NLM, you're probably interested in the Python bindings. Here's the quickest way to get started:

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)
    
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

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

## Building (C++ Users)

If you want to build the C++ library or contribute to the core system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running (Python Users)

The Python bindings are the primary interface for users. No compilation needed:

```bash
# Install dependencies
pip install numpy pytest scikit-build-core

# Install pynlm
pip install .

# Run tests
pytest
```

## Project Structure

```
NLM/
├── Python bindings (pynlm)
├── C++ core library
├── Documentation
├── Configurations
└── Experiments
```

## Python API Overview

The main Python classes exposed via `pynlm`:

- **Brain**: Core neural simulation brain
- **AgentBrain**: Interface connecting brain to world
- **SimpleWorld**: Simulated environment
- **Config**: Configuration system
- **SensoryInput**: Sensory data (Vision, Audio, Internal signals)
- **Action**: Motor commands

## Getting Help

1. Read `docs/ARCHITECTURE.md` to understand the system architecture
2. Read `docs/SCIENCE.md` for scientific background
3. Read `docs/EXPERIMENTS.md` to see what's been implemented
4. Start with `easy_usage.md` for basic examples
5. Use `HOW_TO_USE.md` for comprehensive API reference

## License

MIT

## Authors

Research project — See docs for scientific background.
