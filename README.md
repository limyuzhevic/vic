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

## Enhanced Python Bindings

NLM now includes **enhanced Python bindings** with a complete Pythonic API that exposes all neural systems and provides advanced features:

### Key Enhancements

1. **Pythonic API**: Snake_case naming and properties for intuitive usage
2. **Full System Access**: Direct access to memory, prediction, cognition, neuromodulation, and plasticity systems
3. **Custom Exception Handling**: Better error messages and context for debugging
4. **Performance Optimizations**: Batch operations for efficient processing
5. **Memory Management**: Smart reference handling and cloning utilities

### Enhanced Brain API

```python
# Create and initialize
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Access all neural systems
working_memory = brain.get_working_memory()
prediction_system = brain.get_prediction_system()
planner = brain.get_planner()
curiosity = brain.get_curiosity()
dopamine = brain.get_dopamine()
stdp = brain.get_stdp()

# Pythonic properties
print(brain.neuron_count)
print(brain.synapse_count)
print(brain.firing_count)
print(brain.spike_count)
```

### Pythonic Naming Convention

The enhanced bindings use Pythonic snake_case naming while maintaining backward compatibility:

**Original (snake_case):** Available for backward compatibility
**New (camelCase):** For new code

Examples:
- `brain.getFiringNeuronCount()` → `brain.firing_count`
- `brain.getTotalNeuronCount()` → `brain.neuron_count`
- `world.setAgentStart()` → `world.set_agent_position()`
- `agent.isRewardModulationEnabled()` → `agent.reward_modulation_enabled`

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
# Basic build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Enhanced build with Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python)
make -j4

# Install Python bindings
pip install -e .
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Examples

See the `easy_usage.md` and `HOW_TO_USE.md` files for comprehensive examples, including:

- **Basic usage**: Simple brain creation and simulation
- **Agent-based simulation**: Complete agent with world interaction
- **Advanced features**: Memory systems, neuromodulation, prediction
- **Performance optimization**: Batch operations for efficiency
- **Error handling**: Custom exceptions and contextual debugging

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── easy_usage.md          # Beginner-friendly usage guide
├── HOW_TO_USE.md          # Comprehensive advanced documentation
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
├── python/             # Python bindings
│   └── bindings.cpp    # Enhanced Python bindings (Phase 6)
├── tests/
│   ├── test_*.cpp      # Unit tests
│   └── test_*_py.py    # Python binding tests
└── configs/
```

## Enhanced Python Bindings (Phase 6)

The Python bindings now include:

### 1. Brain Class - Complete System Access

```python
# Memory systems
brain.get_working_memory()
brain.get_episodic_memory()
brain.get_associative_memory()

# Prediction system
brain.get_prediction_system()

# Cognitive systems
brain.get_planner()
brain.get_concept_formation()
brain.get_attention()

# Neuromodulation systems
brain.get_dopamine()
brain.get_curiosity()
brain.get_novelty()
brain.get_prediction_error_signal()

# Plasticity systems
brain.get_stdp()
brain.get_hebbian()
brain.get_structural_plasticity()
```

### 2. AgentBrain Class - Enhanced Interface

```python
# Enhanced neuromodulation controls
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Pythonic property access
print(agent.reward_modulation_enabled)
print(agent.curiosity_enabled)
```

### 3. Configuration Class - Type-Safe API

```python
# Type-safe getters
config.get("brain.neuron_count")      # Returns Optional[int64_t]
config.get("brain.connection_probability")  # Returns Optional[float]
config.get("brain.enable_learning")  # Returns Optional[bool]

# Pythonic get_or methods
config.get_or_int("brain.neuron_count", 1000)
config.get_or_float("brain.learning_rate", 0.001)
```

### 4. Performance Optimizations

```python
# Batch operations for performance
pynlm.batch_step(brain, list(range(1000)))
pynlm.batch_step_with_time(brain, [(i, i * 0.001) for i in range(1000)])
```

### 5. Error Handling

```python
# Custom exceptions with context
try:
    brain.step(0)
except pynlm.InvalidOperationError as e:
    context_error = pynlm.create_error_context("operation", "details")
    print(f"Error: {e}")
```

### 6. Serialization and State Management

```python
# Enhanced state management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Memory management
brain.clone()  # Create copy of brain
world.clone()  # Create copy of world
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
- **Enhanced Python bindings with full system access**
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **Comprehensive documentation and examples**
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

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 6 Key Innovations

### Enhanced Python Bindings
- Full access to all neural subsystems
- Pythonic API for better developer experience
- Custom exception handling for robust debugging
- Performance optimizations through batch operations
- Comprehensive documentation and examples

### Enhanced Documentation
- Beginner-friendly `easy_usage.md`
- Comprehensive advanced `HOW_TO_USE.md`
- Complete API reference and examples
- Troubleshooting guides
- Migration paths for existing users

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Started

1. **Build from source**: Follow the build instructions above
2. **Install Python bindings**: `pip install -e .`
3. **Try basic examples**: See `easy_usage.md` for simple examples
4. **Explore advanced features**: See `HOW_TO_USE.md` for comprehensive documentation
5. **Run integration tests**: Use `./nlm_phase6_demo` for a full system test

The enhanced NLM Phase 6 provides a complete, accessible interface for experimenting with artificial developmental brains while maintaining the scientific rigor and biological inspiration of the original project.
