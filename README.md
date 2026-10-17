# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

> **NEW: Enhanced Python Bindings Available!**
> The project now includes `pynlm_enhanced` with a simplified, user-friendly API for rapid prototyping and machine learning tasks.

## What is NLM?

NLM is a brain simulator for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

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

### Building Enhanced Python Bindings

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4 pynlm_enhanced
```

The enhanced Python bindings are built as a separate module `pynlm_enhanced` alongside the original `pynlm`.

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Usage Examples

**Basic Python (Original Bindings)**

```python
import pynlm as nlm

# Create a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

**Enhanced Python (New Simplified API)**

```python
import pynlm_enhanced as nlm

# Create a brain easily
brain = nlm.createSimpleBrain(neuron_count=100)

# Run simulation easily
brain.simulate(50)

# Get status easily
status = brain.getStatus()
print(f"Your brain has {status['neuron_count']} neurons")
print(f"Total spikes: {status['total_spikes']}")
print(f"Average firing rate: {status['firing_rate']:.2f} Hz")
print(f"Learning detected: {status['learning_detected']}")
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

## Enhanced Features

### New Python API (pynlm_enhanced)

#### Simplified Brain Interface
- `createSimpleBrain()` - Easy brain creation with sensible defaults
- `brain.simulate(steps)` - Simple simulation without manual stepping
- `brain.getStatus()` - Comprehensive status information
- `brain.checkLearning()` - Simple learning detection

#### Neural Network Tools
- `createNeuralNetwork()` - Simple neural network implementation
- `trainAndEvaluate()` - Train and evaluate neural networks
- `batchSimulate()` - Run simulations on multiple brains

#### Enhanced Configuration
- Method chaining for configuration
- Type-safe getters and setters
- Preset configurations (basic/advanced)
- JSON export/import

### Code Quality Improvements

#### C++ Core
- Modernized code structure
- Fixed memory management issues
- Improved error handling
- Enhanced documentation
- Better testing coverage

#### Python Bindings
- Simplified API for common use cases
- Enhanced error handling with Python exceptions
- Type safety and validation
- Better documentation and examples

### Advanced Tools

#### Command Line Tools
- Brain configuration tools
- Simulation control tools
- Performance monitoring
- Checkpoint management

#### Visualization Tools
- Real-time brain state visualization
- Spike train visualization
- Network topology visualization
- Performance monitoring dashboards

## Enhanced API Documentation

For complete API documentation, see:
- `docs/ENHANCED_API.md` - Full enhanced API reference
- `python/bindings_enhanced.cpp` - Enhanced bindings source
- `easy_usage.md` - Easy usage guide (updated)
- `HOW_TO_USE.md` - Comprehensive usage guide

## License

MIT

## Authors

Research project — See docs for scientific background.
