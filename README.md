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

## Key Improvements

### Enhanced Configuration System

- **JSON/YAML Support**: `Config::loadFromFile()` now supports JSON format in addition to the simple key=value format
- **Better Serialization**: `Config::saveToFile()` provides readable output with source information and proper type formatting
- **Flexible Value Types**: Supports int, double, string, bool, and arrays of these types

### Advanced AgentBrain Features

The AgentBrain now includes enhanced functionality:

#### Configurable Parameters:
- `maxCuriosityThreshold`: Controls when curiosity drives exploration (default: 0.8)
- `explorationRate`: Base rate of random exploration (default: 0.1)
- `neuromodulationGain`: Scales neuromodulatory signals (default: 1.0)
- `validationEnabled`: Whether to validate sensory input dimensions (default: true)

#### Enhanced Capabilities:
- **Look Commands**: Added `LookLeft` and `LookRight` motor commands for sensor panning
- **Audio Processing**: Support for auditory sensory input
- **Better Motor Distribution**: 8 motor groups instead of 6 for more flexible control
- **Enhanced Sensory Categorization**: 5 sensory groups instead of 4 (adding audio)
- **Advanced Novelty Detection**: Spatially weighted novelty computation
- **Better Error Handling**: Validation and logging of input mismatches
- **Adaptive Scaling**: Novelty amplifies sensory sensitivity

#### New Methods:
- `getOptimalMotorGroupCount()`: Returns expected motor groups
- `setNeuromodulationGain()`: Configure neuromodulation strength
- `setExplorationRate()`: Configure exploration behavior
- `setMaxCuriosityThreshold()`: Set curiosity threshold
- `setVisionDimensions()`: Configure vision grid for coordinate weighting
- `enableValidation()`: Toggle input validation

### Improved Documentation

The `easy_usage.md` file has been completely rewritten with:

1. **Enhanced Examples**: Including error handling, advanced configurations, and debugging patterns
2. **Complete Pattern Library**: 3 comprehensive patterns for different use cases
3. **Mini Projects**: 3 practical examples from simple to complex
4. **Advanced Features**: Development simulation, reward learning, and neuro-debugging
5. **Configuration Examples**: Custom brain setup and state persistence
6. **Performance Monitoring**: Brain performance metrics and debugging tools
7. **Comprehensive Troubleshooting**: Common issues and solutions

### Better Error Handling

- **Null Pointer Checks**: All methods check for null brain pointers
- **Input Validation**: Validate sensory input dimensions when enabled
- **Comprehensive Logging**: Detailed logging for debugging and monitoring
- **Graceful Degradation**: Fail gracefully with informative error messages
- **Range Clamping**: All parameters are clamped to valid ranges

### Python Bindings Enhancements

The `bindings.cpp` provides:

- **Complete API**: Full access to all AgentBrain configuration and state methods
- **Error Handling**: Better error propagation from C++ to Python
- **Documentation**: Enhanced docstrings for all bindings
- **Type Safety**: Proper type conversions and safety checks

## Performance Improvements

1. **Reduced Copying**: Using move semantics for brain pointer
2. **Optimized Memory Access**: Better locality with grouped neuron vectors
3. **Adaptive Processing**: Weighted novelties and adaptive scaling
4. **Configurable Validation**: Can disable validation for performance-critical scenarios

## Usage Examples

### Basic Usage

```python
import pynlm

# Simple setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
```

### Advanced Configuration

```python
import pynlm

# Create and configure agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

# Configure behavior
agent.setNeuromodulationGain(1.5f)  # Stronger neuromodulation
agent.setExplorationRate(0.2f)      # More exploration
agent.setMaxCuriosityThreshold(0.9f) # Higher curiosity threshold
agent.enableValidation(True)        # Enable input validation

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)
```

### Debugging and Monitoring

```python
import pynlm

# Create agent with debugging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
agent.enableValidation(True)  # Enable input validation
agent.enableDevelopment(True)

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
agent.initialize(world)

# Debug output
print(f"Initial state: {brain.getDevelopmentalStage()}")
print(f"Expected input size: {agent.getSensoryInputSize()}")
print(f"Expected output size: {agent.getMotorOutputSize()}")

# Run with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.4f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.4f}")
```

## Testing and Validation

The system includes comprehensive validation:

1. **Input Size Validation**: Ensures sensory input matches expected dimensions
2. **Range Checking**: All parameters are clamped to valid ranges
3. **Null Pointer Protection**: Safe handling of null brain pointers
4. **State Validation**: Brain state consistency checks
5. **Performance Monitoring**: Brain activity metrics and statistics

## Future Enhancements

The improved codebase provides a solid foundation for:

1. **More Complex Sensory Modalities**: Additional sensory types (smell, taste, etc.)
2. **Advanced Motor Control**: More nuanced action selection and sequencing
3. **Better Memory Integration**: Enhanced memory systems with temporal dynamics
4. **Learning Algorithms**: More sophisticated learning rules and plasticity
5. **Multi-Agent Systems**: Coordination between multiple agent brains
6. **External Interfaces**: Integration with other AI systems and environments
7. **Visualization Tools**: Better debugging and visualization capabilities

The NLM Phase 6 system now provides a more robust, configurable, and extensible platform for research in brain-like architectures and artificial developmental systems.


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
