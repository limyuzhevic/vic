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

## New Enhanced Python API

NLM now includes an enhanced Python API that provides user-friendly abstractions and simplified interfaces for common use cases:

### Quick Start (Enhanced API)

```python
import enhanced_pynlm as nlm

# Create and initialize a brain (simple one-liner)
brain = nlm.Brain()
brain.initialize()

# Run simulation
brain.simulate(100)

# Get statistics
stats = brain.getStats()
print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Original API Still Available

```python
import nlm

# Original basic API (still available for advanced users)
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

## Enhanced API Features

### 1. High-Level Abstractions

| Feature | Description | Example |
|---------|-------------|---------|
| **Simple Brain** | One-line brain creation and initialization | `brain = nlm.Brain()` |
| **Quick Simulation** | Run multiple steps efficiently | `brain.simulate(100)` |
| **Statistics** | Get all brain stats in one call | `stats = brain.getStats()` |
| **Smart Save** | Automatic filename and format | `brain.save("my_brain")` |

### 2. Agent-World Integration

```python
import enhanced_pynlm as nlm

# Create environment
brain = nlm.Brain()
world = nlm.World()

# Create and initialize agent
agent = nlm.Agent(brain)
agent.initialize()
agent.enableAll()  # Enable all learning features

# Run exploration
results = agent.runEpisode(maxSteps=1000)
print(f"Average reward: {results['total_reward']}")
```

### 3. Command-Line Helpers

```python
import enhanced_pynlm as nlm

# Quick simulation with helpers
brain = nlm.createBrain("config.json")
world = nlm.createWorld(20, 20)
stats = nlm.runSimulation(brain, 100)
```

## Installation

### Install Enhanced API

```bash
pip install enhanced-pynlm
```

### Install Original API

```bash
pip install pynlm
```

### Install from Source

```bash
pip install -e .
```

## Usage Examples

### Example 1: Simple Brain Simulation

```python
import enhanced_pynlm as nlm

# Create and run a simple simulation
brain = nlm.Brain()
brain.initialize()
brain.simulate(1000)

# Save and display results
brain.save("simple_simulation")
stats = brain.getStats()
print(f"Simulation completed with {stats['total_neurons']} neurons")
```

### Example 2: Complete Agent-World Experiment

```python
import enhanced_pynlm as nlm

# Setup complete environment
brain = nlm.Brain()
world = nlm.World()
world.setup(width=25, height=25)

# Create and initialize agent
agent = nlm.Agent(brain)
agent.initialize()
agent.enableAll()

# Run exploration episode
print("Starting agent exploration...")
results = agent.runEpisode(maxSteps=1000)

# Display results
print(f"Exploration completed!")
print(f"Average reward per step: {results['total_reward']}")
print(f"Final position: ({results['final_x']:.1f}, {results['final_y']:.1f})")
```

### Example 3: Learning with Memory

```python
import enhanced_pynlm as nlm

# Create brain with memory support
brain = nlm.Brain()
brain.initialize()

# Simulate with learning
print("Running learning simulation...")
for step in range(500):
    sensory_input = [0.5 * (step % 10) / 10 for _ in range(8)]
    brain.step(step, sensory_input)
    
    if step % 100 == 0:
        stats = brain.getStats()
        print(f"Step {step}: {stats['firing_neurons']} firing neurons")

brain.save("learning_simulation")
```

## API Reference

### Brain Class

| Method | Description | Parameters |
|--------|-------------|------------|
| `Brain()` | Create new brain with default config | None |
| `initialize()` | Initialize brain with configuration | None |
| `step(step, input)` | Run one simulation step | `step: int, input: List[float]` |
| `simulate(steps)` | Run multiple simulation steps | `steps: int` |
| `getStats()` | Get brain statistics | None |
| `save(name)` | Save brain state | `name: str` |
| `getBrain()` | Get underlying brain | None |

### World Class

| Method | Description | Parameters |
|--------|-------------|------------|
| `World()` | Create new world | None |
| `setup(width, height, visionWidth, visionHeight)` | Configure world | `width, height, visionWidth, visionHeight` |
| `setAgentPosition(x, y)` | Set agent start | `x, y` |
| `update(timestep)` | Update world and get observation | `timestep: float` |
| `applyAction(actionType, currentTime)` | Apply action | `actionType: int, currentTime: float` |
| `getState()` | Get world state | None |
| `getWorld()` | Get underlying world | None |

### Agent Class

| Method | Description | Parameters |
|--------|-------------|------------|
| `Agent(brain)` | Create agent with brain | `brain: Brain` |
| `initialize()` | Initialize agent with world | None |
| `runEpisode(maxSteps)` | Run complete episode | `maxSteps: int` |
| `enableAll()` | Enable all learning | None |

## Migration Guide

### From Original API to Enhanced API

**Before (Original API):**
```python
import nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

**After (Enhanced API):**
```python
import enhanced_pynlm as nlm

brain = nlm.Brain()
brain.initialize()
brain.simulate(100)
```

### Performance

The enhanced API provides similar performance with much simpler syntax. For most use cases, there's no significant performance difference.

## Documentation

- **Original Documentation**: `docs/` (architecture, science, experiments)
- **Enhanced API Documentation**: `docs/ENHANCED_API.md`
- **Quick Usage Guide**: `easy_usage.md` (enhanced examples)
- **Complete Reference**: `HOW_TO_USE.md`

## Examples Directory

The project includes example scripts:

- `examples/simple_brain.py` - Basic brain simulation
- `examples/agent_exploration.py` - Complete agent-world experiment
- `examples/learning_simulation.py` - Advanced learning with memory

## Development

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Tests

```bash
./nlm_test
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo
```

### Python Bindings

```bash
pip install -e .
```

## Contributing

We welcome contributions to improve the NLM framework! Please follow the existing code patterns and add tests for new functionality.

## License

MIT License - See `LICENSE` file for details.

## Authors

Research project — See `docs/` for scientific background and methodology.

## Acknowledgments

The NLM project is an ongoing research effort in artificial developmental systems and computational neuroscience.

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
