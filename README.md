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

## Python Bindings

The NLM framework includes enhanced Python bindings that provide a comprehensive interface to the neural simulation system.

### Installation

The enhanced Python bindings are built as part of the main NLM build system:

```bash
cd build
# After building the main NLM library, build the Python bindings
cd ..
cmake . -DCMAKE_BUILD_TYPE=Release
make pynlm
```

### Quick Start

```python
import nlm

# Create a complete simulation setup
world, brain, agent_brain, config = nlm.createSimulationSetup()

# Configure the agent
config.set("agent.name", "demo_agent")
config.set("agent.max_energy", 200.0)

# Run simulation with context manager (safe execution)
with nlm.SimulationContext(brain, max_steps=1000) as sim:
    world.setAgentStart(50.0, 50.0)
    results = sim.runSteps(500)
    
print(f"Simulation completed: {results['steps_completed']} steps")
print(f"Average firing rate: {results['average_firing_rate']}")
```

### Features

- **Enhanced Error Handling**: Comprehensive exception handling with custom NLMError classes
- **Pythonic APIs**: Context managers, property access, and Python-style initialization
- **Utility Classes**: VisionHelper, AudioHelper, SimulationUtils for common operations
- **Safe Operations**: Safe save/load functions with error handling
- **Statistical Analysis**: Built-in statistics collection and analysis
- **Backward Compatibility**: All existing APIs continue to work

### Documentation

For comprehensive documentation, including examples and API reference:
- [Enhanced Python Bindings Documentation](python/README.md)
- [Python Example Code](examples/)

### API Reference

Key classes and functions:

- `nlm.Config`: Enhanced configuration management
- `nlm.Brain`: Neural simulation brain with enhanced Python interface
- `nlm.AgentBrain`: Agent interface with neural processing
- `nlm.SimpleWorld`: 3D world simulation with sensory processing
- `nlm.SimulationContext`: Context manager for safe simulation execution

### Additional Resources

- [Python Bindings Documentation](python/README.md)
- [Phase 6 Integration Guide](docs/PHASE6_FINAL_AUDIT.md)
- [Examples and Tutorials](examples/)

The enhanced Python bindings provide a comprehensive, user-friendly interface to the sophisticated NLM neural simulation framework, making it easy to create, run, and analyze complex neural simulations.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/                    # Enhanced Python bindings
│   ├── bindings.cpp           # Enhanced C++/Python interface
│   ├── CMakeLists.txt         # Python bindings build configuration
│   ├── README.md              # Python bindings documentation
│   └── examples/              # Python examples and tutorials
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

### Phase 7 (In Progress - Enhanced Python Bindings)
- Comprehensive Python bindings with enhanced error handling
- Pythonic APIs with context managers and utilities
- Vision/Audio helper classes for data processing
- Safe save/load operations with error handling
- Statistical analysis and monitoring tools
- Comprehensive documentation and examples
- Backward compatibility maintained

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 7 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 7 DOES investigate:
- Whether Python bindings can provide intuitive, Pythonic interface to neural simulation
- Whether enhanced error handling improves usability and debugging
- Whether utility classes simplify common operations
- Whether context managers make simulation safety accessible
- Whether comprehensive documentation lowers barriers to entry

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
