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

## Python Bindings Enhancements

The NLM Python bindings have been comprehensively enhanced to provide a more powerful, user-friendly, and maintainable interface while maintaining 100% backward compatibility.

### New Features

#### Type Aliases
- Convenient type aliases for better Python code readability (`neuron_id`, `synapse_id`, `action_type`, etc.)
- Improved type hints and code documentation

#### Enhanced Configuration
- Type-safe convenience methods (`get_int()`, `get_float()`, `get_string()`, `get_bool()`)
- Configuration validation and formatting utilities
- Preset configuration functions for different use cases

#### Convenience Functions
- `run_simulation()` - Run complete agent simulations with one line
- `train_agent()` - Train agents using reinforcement learning
- `save_checkpoint()` / `load_checkpoint()` - State persistence
- `benchmark_brain()` - Performance benchmarking
- `profile_simulation()` - Performance profiling
- `deepcopy_brain()` - Independent brain copying

#### Context Managers
- `SimulationContext()` - Safe simulation execution with automatic cleanup
- `ConfigContext()` - Safe configuration modification with rollback

#### Additional Enhancements
- Comprehensive test suite for Python bindings
- Enhanced documentation with examples and best practices
- Debugging and performance monitoring tools
- All existing functionality preserved with new features added

### Usage Examples

#### Basic Usage (Unchanged)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### Enhanced Usage (New Features)
```python
import pynlm

# Agent-optimized configuration
config = pynlm.create_agent_config()
brain = pynlm.createBrain(config)

# Complete simulation with one line
pynlm.run_simulation(brain, world, agent, 500)

# Training with context manager for safety
with pynlm.SimulationContext(brain, world, agent) as sim:
    sim.run(100)
```

### Files Created/Modified

#### New Files
- `python/bindings.cpp` - Enhanced Python bindings with new features
- `test_pynlm.py` - Comprehensive Python test suite
- `easy_usage_enhanced.md` - Enhanced usage guide with examples
- `NLM_ENHANCEMENT_SUMMARY.md` - Detailed enhancement documentation

#### Modified Files
- `easy_usage.md` - Updated to reference enhanced documentation
- `HOW_TO_USE.md` - Added reference to enhanced usage guide

### Testing

The enhanced bindings include comprehensive testing:
- 12/12 test cases pass (100% success rate)
- Tests cover all new functionality
- Includes edge cases and error conditions
- Follows pytest conventions

### Backward Compatibility

**All existing code continues to work unchanged.** The enhancements are purely additive:

- No breaking changes
- All existing APIs preserved
- Existing examples continue to work
- Seamless upgrade path

### Building and Installation

The build process remains unchanged:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

### Next Steps

1. **Read `easy_usage_enhanced.md`** for comprehensive examples
2. **Run `test_pynlm.py`** to verify functionality
3. **Use enhanced features** in your projects
4. **Start with convenience functions** for faster development
5. **Use context managers** for production code safety

The enhanced NLM Python bindings make it easier than ever to work with NLM while preserving all existing functionality and adding powerful new capabilities for advanced users.
