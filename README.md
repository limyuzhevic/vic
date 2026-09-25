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

## Python Bindings Enhancement

NLM now includes enhanced Python bindings (`pynlm`) that make it much easier to work with the neural simulation framework. The Python bindings provide a user-friendly interface with convenient convenience methods and factory functions.

### Key Python Enhancements

#### 1. Enhanced Config Class
- **Type-safe methods**: `getFloat()`, `getInt()`, `setFloat()`, `setInt()`, `setString()` for working with configuration values
- **Configuration helpers**: `configureWorld()`, `getSystemStats()` for common setup patterns
- **Better serialization**: Fixed `saveToFile()` to properly save actual configuration values (previously had placeholder values)

#### 2. Factory Functions
- **Quick starts**: `createDefaultConfig()`, `createBrain()`, `createSimpleWorld()`, `createAgentBrain()`
- **Helper functions**: `quickStartExperiment()`, `resetConfigToDefaults()`, `normalizeConfigForExperiment()`
- **World setup**: `createWorldWithObjects()`, `createStandardTestWorld()`

#### 3. Enhanced AgentBrain
- **Feature management**: `enableAllFeatures()`, `disableAllFeatures()`, `getNeuromodulationStatus()`
- **Batch operations**: `updateAllNeuromodulators(reward, prediction_error, novelty, curiosity)`

#### 4. Convenience Methods
- **Type conversion**: Automatic type conversion when getting configuration values
- **Validation**: Better error handling and type safety
- **Documentation**: Comprehensive docstrings for all methods

### Python Usage Examples

#### Basic Usage (Unchanged)
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

#### Enhanced Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
config.configureWorld(100, 100, 16, 16)  # Set up world dimensions
config.setFloat("learning_rate", 0.5)   # Set float parameter
config.setInt("max_steps", 1000)       # Set integer parameter

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Quick Start Helper
```python
import pynlm

# One-liner for common experiment setup
brain = pynlm.quickStartExperiment()  # New method!
brain.initialize()
```

#### Advanced Setup
```python
import pynlm

# Create world with objects
world = pynlm.createStandardTestWorld()  # New method!
world.configure(50, 50, 8, 8)

# Create brain with experiment-specific config
config = pynlm.normalizeConfigForExperiment(pynlm.createDefaultConfig(), "phase2")  # New method!
brain = pynlm.createBrain(config)
brain.initialize()

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllFeatures()  # New method!
```

### Installation

The Python bindings are included in the main package. To use the enhanced bindings:

```bash
pip install -e .  # Install in development mode
```

### Enhanced Convenience Methods Reference

| Method | Description | Example |
|--------|-------------|---------|
| `config.getFloat(key, default)` | Get config as float with type conversion | `rate = config.getFloat("learning_rate", 0.01)` |
| `config.getInt(key, default)` | Get config as int with type conversion | `steps = config.getInt("max_steps", 1000)` |
| `config.setFloat(key, value)` | Set config from float | `config.setFloat("param", 0.5)` |
| `config.setInt(key, value)` | Set config from int | `config.setInt("count", 42)` |
| `config.configureWorld(width, height, visionWidth, visionHeight)` | Configure world dimensions | `config.configureWorld(100, 100, 16, 16)` |
| `config.getSystemStats()` | Get configuration statistics | `stats = config.getSystemStats()` |
| `pynlm.quickStartExperiment()` | Quick start for common experiments | `brain = pynlm.quickStartExperiment()` |
| `agent.enableAllFeatures()` | Enable all neuromodulation | `agent.enableAllFeatures()` |

### What's New in This Version

This version includes significant improvements to the Python bindings:

1. **Fixed Critical Bug**: `Config.saveToFile()` now properly saves actual configuration values instead of hardcoded "PLACEHOLDER_VALUE"
2. **Enhanced Performance**: Optimized `AgentBrain` constructor from O(n²) to O(n) complexity
3. **Improved Action Selection**: Enhanced `Brain.produceAction()` to use intelligent motor neuron activity patterns
4. **Better Python Interface**: Added 15+ convenience methods for common Python operations
5. **Factory Functions**: Added helper functions for common setup patterns
6. **World Helpers**: Added convenient world setup functions
7. **Improved Documentation**: Enhanced examples and documentation in `easy_usage.md`

These improvements make NLM much more accessible to Python users while maintaining full backward compatibility with existing code.

### Migration Guide

#### Existing Code (No Changes Required)

All existing Python code continues to work without any modifications:

```python
# This still works exactly the same
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

#### Using New Features (Optional)

To take advantage of the new features, simply add the enhanced convenience methods to your code:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.configureWorld(100, 100, 16, 16)  # New method!

brain = pynlm.createBrain(config)
brain.initialize()

# Use new helper functions
brain = pynlm.quickStartExperiment()  # Alternative quick start
```

All new methods are optional and do not affect existing code.

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

## License

MIT

## Authors

Research project — See docs for scientific background.
