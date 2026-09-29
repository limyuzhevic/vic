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

## Enhanced Python Bindings

The NLM project now includes enhanced Python bindings with comprehensive APIs for quick and easy neural simulation development.

### Key Features

**Enhanced Python API:**
- Factory methods for quick setup (`Brain.create_default()`, `Config.from_preset()`)
- Context managers for automatic resource cleanup
- Comprehensive error handling with specific exception types
- Type safety with input validation and conversion utilities
- Extensive documentation with examples

**Convenience Methods:**
- Batch simulation execution (`brain.run_steps()`)
- Quick statistics (`brain.get_stats()`)
- Configuration presets (`'development'`, `'production'`, `'research'`, `'demo'`)
- World and action factory methods
- Global helper functions for common tasks

**Error Handling:**
- Specific exception types for different error scenarios
- Meaningful error messages for debugging
- Input validation with clear feedback

### Quick Start

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.Brain.create_default()
brain.initialize()

# Run a simple simulation
brain.run_steps(100)

# Get quick statistics
stats = brain.get_stats()
print(f"Total neurons: {stats['total_neuron_count']}")
```

### Advanced Usage

```python
import pynlm

# Use configuration presets
config = pynlm.Config.from_preset('demo')
brain = pynlm.Brain(config)
brain.initialize()

# Use context manager for automatic cleanup
with pynlm.Brain.create_default() as brain:
    brain.initialize()
    brain.run_steps(50)

# Create and configure a world
world = pynlm.SimpleWorld.create_standard(100, 100)
world.add_agent(10, 10)
world.add_resource(50, 50, 20.0)

# Create actions
move_action = pynlm.Action.create_move("forward", 1.0)
look_action = pynlm.Action.create_look(0.1, 0.0)
```

### Example Files

The enhanced bindings include comprehensive example files:

- **examples/python_examples.py** - 9 complete usage examples
- **tests/python_bindings_test.py** - Complete test suite

Run the examples to see all features in action:

```bash
python3 examples/python_examples.py
```

Run the tests to verify all functionality:

```bash
python3 -m pytest tests/python_bindings_test.py -v
```

## Building

### C++ Components

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings

The Python bindings are built using scikit-build and pybind11. They are automatically built when the C++ components are built.

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Bindings Testing

```bash
python3 -m pytest tests/python_bindings_test.py -v
```

### Python Examples

```bash
python3 examples/python_examples.py
```

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
├── python/             # Enhanced Python bindings
│   ├── bindings.cpp
│   └── CMakeLists.txt
├── examples/           # Python usage examples
│   └── python_examples.py
├── tests/              # Python test suite
│   └── python_bindings_test.py
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

## Enhanced Python Bindings Features

### 1. Custom Exception Hierarchy

Specific exception types for different error scenarios:
- `NLMException` - Base exception
- `BrainInitializationError` - Brain initialization errors
- `WorldConfigurationError` - World configuration errors
- `ActionValidationError` - Action validation errors
- `ConfigValidationError` - Configuration validation errors
- `MemoryError` - Memory-related errors
- `SimulationError` - Simulation runtime errors

### 2. Factory Methods and Convenience APIs

**Brain Class:**
- `Brain.createDefault()` - Quick brain creation
- `Brain.createPreset(preset_name)` - Preset-based creation
- `Brain.runSteps(steps)` - Efficient batch simulation
- `Brain.getStats()` - Comprehensive statistics
- `Brain.isReady()` - Readiness checking
- `Brain.waitForConvergence(target_rate, timeout)` - Convergence waiting

**Config Class:**
- `Config.fromPreset(preset_name)` - Load from presets
- `Config.validate()` - Configuration validation
- `Config.toDict()` - Convert to Python dictionary
- `Config.updateFromDict(data)` - Update from dictionary
- `Config.getWithDefault(key, default)` - Get with default

**SimpleWorld Class:**
- `SimpleWorld.createStandard(width, height, vision_width, vision_height)` - Standard world
- `SimpleWorld.createEmpty()` - Empty world
- `world.addAgent(x, y)` - Agent placement
- `world.addResource(x, y, value)` - Resource addition
- `world.addHazard(x, y, damage)` - Hazard addition
- `world.clearObjects()` - Clear objects

**Action Class:**
- `Action.createMove(direction, magnitude)` - Movement actions
- `Action.createLook(dx, dy)` - Looking actions
- `Action.createInteract()` - Interaction actions
- `Action.createWait(duration)` - Waiting actions
- `Action.createEat(nutrition)` - Eating actions
- `Action.createDrink(hydration)` - Drinking actions
- `Action.createRest()` - Resting actions
- `Action.createCustom(name, parameters)` - Custom actions

### 3. Context Manager Support

Automatic resource cleanup:
```python
with Brain.createDefault() as brain:
    brain.initialize()
    brain.runSteps(50)
```

### 4. Type Safety and Validation

Safe type conversion and input validation with meaningful error messages.

### 5. Global Factory Functions

Convenience functions for common tasks:
- `createDefaultConfig()` - Default configuration
- `createBrain(config)` - Brain factory
- `createSimpleWorld()` - World factory
- `createAgentBrain(brain)` - Agent brain factory
- `createDemoBrain()` - Demo brain
- `createDemoWorld()` - Demo world
- `createAgentForWorld(world)` - Agent for world
- `simulateEpisode(brain, world, max_steps, target_reward)` - Episode simulation
- `validateSetup(brain, world)` - Setup validation

### 6. Comprehensive Documentation

- Module docstring with examples
- Detailed docstrings for all new methods
- Usage examples and best practices

## Testing

Run the Python bindings test suite:

```bash
python3 -m pytest tests/python_bindings_test.py -v
```

The test suite includes:
- Basic bindings functionality
- Preset configuration loading
- World and action factory methods
- Context manager functionality
- Error handling scenarios
- Validation and type conversion
- Backward compatibility verification
- Brain convenience methods

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