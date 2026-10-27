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

### Python Module

NLM now includes comprehensive Python bindings:

```bash
python3 examples.py
```

Run the examples to see the Python API in action.

## Python Module

NLM now includes comprehensive Python bindings for easier experimentation and research:

### Quick Start

```python
from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain

# Create brain
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

# Create world and agent
world = createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Key Features

- **Brain API** - Create and control neural brains
- **World API** - Simulate environments
- **Agent API** - Bridge between brain and world
- **Learning API** - Enable reward, curiosity, development
- **Configuration API** - Customize brain parameters
- **Checkpointing** - Save/load brain states

### Examples

- `examples.py` - Comprehensive Python examples
- `PYTHON_SETUP.md` - Python module setup guide
- `easy_usage.md` - Beginner-friendly tutorial

### Installation

```bash
pip install -e .
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── PYTHON_SETUP.md          # Python module documentation
├── easy_usage.md           # Beginner-friendly tutorial
├── examples.py             # Comprehensive Python examples
├── docs/
│   ├── ARCHITECTURE.md     # Technical architecture
│   ├── SCIENCE.md          # Scientific background
│   ├── ROADMAP.md          # Development roadmap
│   ├── EXPERIMENTS.md      # Experiment documentation
│   ├── LIMITATIONS.md      # Known limitations
│   └── PHASE6_FINAL_AUDIT.md  # Phase 6 integration status
├── src/
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── memory/            # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
├── python/                 # Python bindings
│   └── bindings.cpp        # PyBind11 bindings
├── tests/                  # Unit tests
└── build/                  # Build artifacts (if generated)
```

## Python Module (pynlm)

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
