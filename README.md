# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is a computational brain research project exploring how complex cognition can emerge from spiking neural dynamics, developmental learning, and embodied interaction. Rather than implementing specific AI capabilities, NLM investigates whether brain-like principles can give rise to cognitive functions through experience and development.

## Key Differentiators

### What NLM IS
- A **neural system** built from spiking neurons with biologically inspired dynamics
- A **developmental architecture** that matures and adapts over time
- A **closed-loop cognitive system** where perception, memory, and action continuously interact
- A **testbed for emergent cognition** studying how prediction, memory, and planning can arise from neural processes

### What NLM IS NOT
- **NOT** a transformer or large language model (LLM)
- **NOT** a pretrained AI system with fixed capabilities
- **NOT** a deep learning model in the conventional sense
- **NOT** a chatbot or conversational agent
- **NOT** a claim about human-level intelligence or consciousness

## Current Status: Phase 6 Integration

**PHASE 6: FINAL INTEGRATION** (In Progress)

Phase 6 represents the culmination of five years of development, focusing on connecting all subsystems into a unified artificial brain. The goal is to demonstrate whether the individual components—neural dynamics, memory systems, prediction, neuromodulation, and cognition—can function coherently together.

### Core Integration Achievements

#### 1. Memory Systems Integration
| System | Function | Integration Status |
|--------|----------|-------------------|
| **Working Memory** | Persistent activity for information maintenance | Connected to neural processing |
| **Episodic Memory** | Experience storage and replay | Integrated with consolidation |
| **Associative Memory** | Pattern-based associations | Linked to neural dynamics |

#### 2. Neuromodulation Integration
- **Dopamine**: Scales STDP plasticity based on prediction errors
- **Curiosity**: Drives exploration through novelty prediction
- **Novelty Detection**: Integrates with sensory processing to identify unexpected events
- **Prediction Error**: Provides learning signals for memory and plasticity

#### 3. Prediction System Integration
- **Temporal Prediction**: Learns sequences from sensory input
- **Action Consequences**: Predicts outcomes of motor commands
- **Confidence Tracking**: Estimates reliability of predictions
- **Multi-step Planning**: Evaluates future action sequences

#### 4. Cognitive Systems Integration
- **Attention**: Competitive selection of neural resources
- **Concept Formation**: Discovers stable patterns from experience
- **Neural Planning**: Uses predictions to select optimal actions
- **Self-Model**: Learns sensorimotor relationships for agency

#### 5. Development Integration
- **Age-Dependent Plasticity**: Learning rates change across developmental stages
- **Critical Period**: Enhanced learning early in development
- **Maturation**: Gradual acquisition of adult-level capabilities
- **Structural Plasticity**: Ongoing synapse formation and pruning

## System Architecture

NLM implements a hierarchical architecture where all components feed into and modulate each other:

```
┌─────────────────────────────────────────────────────────────┐
│                      WORLD (Environment)                   │
├─────────────────────────────────────────────────────────────┤
│                     SENSORY INPUT                           │
├─────────────────────────────────────────────────────────────┤
│                    NEURAL PROCESSING                        │
│  ┌─────────────────────┐ ┌──────────────────────┐        │
│  │   WORKING MEMORY    │ │   EPISODIC MEMORY    │        │
│  └─────────────────────┘ └──────────────────────┘        │
│  ┌─────────────────────┐ ┌──────────────────────┐        │
│  │    PREDICTION      │ │   CONCEPT FORMATION │        │
│  └─────────────────────┘ └──────────────────────┘        │
│  ┌─────────────────────┐ ┌──────────────────────┐        │
│  │    ATTENTION       │ │    NEURAL PLANNER   │        │
│  └─────────────────────┘ └──────────────────────┘        │
│  ┌─────────────────────┐ ┌──────────────────────┐        │
│  │     SELF-MODEL     │ │   SOCIAL LEARNING   │        │
│  └─────────────────────┘ └──────────────────────┘        │
├─────────────────────────────────────────────────────────────┤
│                    NEUROMODULATION                          │
├─────────────────────────────────────────────────────────────┤
│                    DEVELOPMENT                              │
├─────────────────────────────────────────────────────────────┤
│                    PLASTICITY                               │
└─────────────────────────────────────────────────────────────┘
```

## Learning Loop

The NLM operates through a continuous cycle:

1. **Sensory Processing**: Environmental input through vision, touch, and internal signals
2. **Neural Integration**: Sensory data interacts with working memory and predictions
3. **Action Selection**: Motor commands selected based on predictions and goals
4. **Environmental Interaction**: Actions produce consequences and rewards
5. **Learning**: Prediction errors and rewards update synaptic weights and memory
6. **Development**: Age-related changes modify learning rates and capabilities

## Getting Started

### Installation

```bash
# Clone the repository
mkdir nlm-workspace
cd nlm-workspace
git clone <repository-url>
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Quick Test (Phase 6 Integration Demo)

```bash
# Run the integration test
./build/nlm_phase6_demo

# This tests the complete brain loop with all integrated systems
```

## Basic Python API Usage

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# 2. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create agent and world for embodied interaction
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 4. Initialize agent with world
agent.initialize(world)

# 5. Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore novel stimuli

# 6. Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")

print("Simulation complete!")
```

## Running Experiments

### Phase 4 Cognitive Experiments

NLM includes comprehensive cognitive experiments to test emergent abilities:

```bash
# Run temporal prediction experiment
./build/nlm_phase4_demo 100

# Run working memory experiment
./build/nlm_phase4_demo --working-memory 100

# Run episodic recall experiment  
./build/nlm_phase4_demo --episodic 100

# Run concept formation experiment
./build/nlm_phase4_demo --concept-formation 100
```

### Performance Experiments

```bash
# Run scaling benchmark
./build/nlm --scaling-benchmark

# Test memory usage at different scales
./build/nlm --benchmark --neurons=10000 --steps=10000
```

## Key Components and APIs

### Brain Class (`pynlm.Brain`)
Core neural processing system implementing LIF dynamics and plasticity.

**Methods:**
- `initialize()`: Initialize brain state and neural connections
- `step(step_number)`: Process one simulation step with spike propagation
- `reset()`: Reset brain to initial developmental state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

**Properties:**
- `getTotalNeuronCount()`: Total neurons in brain
- `getTotalSynapseCount()`: Total synaptic connections
- `getFiringNeuronCount()`: Currently active neurons
- `getAverageFiringRate()`: Neural activity statistics
- `getDevelopmentalStage()`: Current developmental phase

### AgentBrain Class (`pynlm.AgentBrain`)
Integration layer connecting brain to world perception and action.

**Key Methods:**
- `processSensoryInput(percept)`: Convert world observations to neural input
- `decodeMotorCommand()`: Convert neural activity to motor commands
- `applyRewardModulation(reward, prediction)`: Apply learning signals
- `updateDevelopment(timestep)`: Advance developmental stage

**Subsystems:**
- Reward modulation (dopamine-like learning)
- Structural plasticity (synaptogenesis/pruning)
- Development (age-dependent learning)
- Curiosity (exploration motivation)

### World Class (`pynlm.SimpleWorld`)
Embodied environment for agent interaction.

**Core Functions:**
- `createSimpleWorld()`: Create 2D environment with objects and energy
- `configure(width, height, visionWidth, visionHeight)`: Set world dimensions
- `update(timestep)`: Advance environment state
- `getSensoryPercept()`: Get agent's sensory observation
- `applyMotorCommand(action, time)`: Execute agent's action

## Development Roadmap

| Phase | Status | Focus Areas |
|-------|--------|-------------|
| **Phase 1** | ✅ Complete | Core infrastructure, types, interfaces |
| **Phase 2** | ✅ Complete | Neural dynamics, spikes, plasticity |
| **Phase 3** | ✅ Complete | World interaction, reward, development |
| **Phase 4** | ✅ Complete | Cognition: prediction, memory, attention |
| **Phase 5** | ✅ Complete | Performance, scaling, research platform |
| **Phase 6** | 🔄 Integration | Coherent artificial brain loop |

## Scientific Motivation

NLM addresses fundamental questions about the nature of cognition:

1. **Can prediction become a central organizing principle** for cognition?
2. **Can working memory emerge** from spiking neural dynamics?
3. **Can concepts form** without predefined categories?
4. **Can planning arise** from predictive neural mechanisms?
5. **Can self-models develop** through sensorimotor interaction?

## Ethical Considerations

NLM is a research tool, not an AI system. We:

- Make no claims about consciousness, sentience, or human-like intelligence
- Explicitly reject any application as an intelligent agent or decision-maker
- Focus on understanding computational principles rather than creating capable systems
- Maintain transparent limitations and avoid misleading descriptions

## Citing NLM

For research use, please cite:

```bibtex
@misc{nlm2026manual,
  title={NLM — 熙然: An Experimental Artificial Developmental Brain},
  author={NLM Project},
  year={2026},
  url={https://github.com/nlm-project/nlm}
}
```

## Documentation Resources

- **[HOW_TO_USE.md](HOW_TO_USE.md)**: Detailed technical guide with examples
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)**: System architecture diagrams
- **[docs/SCIENCE.md](docs/SCIENCE.md)**: Scientific background and biological inspiration
- **[docs/EXPERIMENTS.md](docs/EXPERIMENTS.md)**: Experimental protocols and results
- **[docs/LIMITATIONS.md](docs/LIMITATIONS.md)**: Known limitations and approximations

## Community and Support

For questions, issues, or collaboration:
- Check [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines
- Report issues at [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- Join discussions at [Discord](https://discord.gg/nlm)

---

**Phase 6 Integration Complete**: NLM now functions as a coherent artificial brain where perception, memory, prediction, and action continuously interact through neural dynamics and developmental learning.
