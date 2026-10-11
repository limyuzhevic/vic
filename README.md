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
├── python/                     # Enhanced Python bindings
│   ├── enhanced_bindings.cpp   # Enhanced Python API with improved error handling
│   ├── CMakeLists.txt          # Python bindings build system
│   └── ENHANCED_API.md         # Comprehensive documentation
├── docs/                       # Scientific documentation
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
├── tests/               # Test suite
└── configs/             # Configuration examples
```

## Python Interface

NLM provides a Python interface through **enhanced bindings** with improved error handling, more intuitive APIs, and comprehensive utilities for both beginners and advanced users.

### Quick Start with Enhanced Python API

```python
import pynlm

# 1. Create enhanced configuration
config = pynlm.createDevelopmentConfig()  # Specialized for learning

# 2. Create enhanced brain interface
brain = pynlm.PythonBrain(config)

# 3. Initialize with error handling
if not brain.initialize():
    raise pynlm.SimulationError("Brain initialization failed")

# 4. Create agent and world
world = pynlm.PythonWorld()
agent = pynlm.PythonAgentBrain(brain)

world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# 5. Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 6. Run enhanced simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = calculate_reward()
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

# 7. Get comprehensive analysis
print(brain.getStats())
```

### Key Features of Enhanced Python API

1. **Better Error Handling**: Custom exception types (`NLMException`, `SimulationError`, `InvalidConfigException`)

2. **Enhanced Configuration**: `PythonConfig` class with source control and better API

3. **Advanced Brain Interface**: `PythonBrain` with enhanced statistics and development control

4. **Power User Tools**: Batch processing (`runMultipleEpisodes`), analysis utilities, configuration factories

5. **Comprehensive Documentation**: `ENHANCED_API.md` with detailed examples

6. **Backward Compatibility**: Original API preserved for existing code

### Enhanced Configuration Options

```python
# Specialized configurations for different use cases
config = pynlm.createMinimalConfig()        # Fast startup
config = pynlm.createHighPerformanceConfig() # Maximum capabilities
config = pynlm.createDevelopmentConfig()    # Focus on learning
config = pynlm.createDefaultConfig()        # General purpose
```

### Advanced Simulation Utilities

```python
# Batch processing for research and testing
pynlm.runMultipleEpisodes(
    numEpisodes=100,
    config=pynlm.createHighPerformanceConfig(),
    stepsPerEpisode=1000,
    enableLearning=True,
    verbose=True
)

# Analysis and debugging
print(brain.getStats())
print(world.getWorldStats())
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

## Python Package Information

### Enhanced API Benefits

1. **Improved Developer Experience**: Better error messages, more intuitive APIs
2. **Enhanced Debugging**: Comprehensive statistics and monitoring tools
3. **Research Support**: Batch processing utilities for reproducible experiments
4. **Learning Tools**: Specialized configurations for different use cases
5. **Documentation**: Complete API reference with examples

### Migration Guide

The enhanced API maintains full backward compatibility:

- Original classes (`Brain`, `Config`, `AgentBrain`, `SimpleWorld`) still available
- Original constructors preserved
- Original methods maintained
- Enhanced versions available with improved functionality

### Usage Examples

Beginner examples can be found in:
- `easy_usage.md` - Simple, beginner-friendly examples
- `HOW_TO_USE.md` - Comprehensive API documentation

Advanced examples and research utilities can be found in:
- `python/ENHANCED_API.md` - Complete enhanced API documentation
- `docs/` - Scientific and architectural documentation

## Support and Community

For questions about the enhanced Python API:
- Check `python/ENHANCED_API.md` for detailed documentation
- Refer to `easy_usage.md` and `HOW_TO_USE.md` for basic usage
- Scientific questions: See `docs/` directory
- Technical questions: Check project architecture documentation

The enhanced Python API is designed to support both beginners learning neural networks and researchers conducting advanced experiments with artificial developmental systems.