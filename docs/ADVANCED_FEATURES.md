# NLM Advanced Features - User Guide

## Overview

This guide documents the advanced features and improvements made to the NLM (Neural Learning Machine) brain simulation framework. The Phase 6 release includes significant enhancements for both basic users and advanced developers.

## New Advanced Demo: Phase6AdvancedDemo

### What is it?

The `nlm_phase6_advanced_demo` executable is a new comprehensive demonstration of the complete NLM brain integration. It showcases:

- **Full Agent-Environment Interaction**: Complete loop between brain and world
- **Multi-step Learning**: Advanced learning through reward modulation and structural plasticity  
- **Development and Maturation**: Brain development over time with stage transitions
- **Memory Consolidation**: Working memory, episodic memory, and replay systems
- **Curiosity-Driven Exploration**: Intelligent exploration behavior
- **Advanced Configuration**: Command-line options for fine-tuning simulations

### Key Features

#### 1. Enhanced Command-Line Interface

```bash
# Basic usage
./nlm_phase6_advanced_demo

# Advanced configuration
./nlm_phase6_advanced_demo \
  --neurons=5000 \
  --steps=20000 \
  --timestep=0.01 \
  --seed=123 \
  --no-curiosity \
  --no-development
```

**Available Options:**
- `--neurons=N`: Number of neurons (default: 1000)
- `--steps=N`: Maximum simulation steps (default: 5000)  
- `--timestep=T`: Simulation timestep (default: 0.01)
- `--seed=N`: Random seed (default: 42)
- `--no-reward`: Disable reward modulation
- `--no-development`: Disable development system
- `--no-curiosity`: Disable curiosity-driven exploration
- `--no-plastic`: Disable structural plasticity

#### 2. Comprehensive Statistics Tracking

The demo provides detailed metrics on:
- **Learning Performance**: Total reward accumulation
- **Neural Activity**: Firing rates and patterns
- **Memory Systems**: Capacity and retrieval statistics
- **Neuromodulation**: Dopamine, curiosity, and novelty levels
- **Development**: Brain maturation stages
- **System Integration**: All subsystems connectivity status

#### 3. Advanced Learning Capabilities

The demo enables all major NLM subsystems:

##### Memory Systems Integration
- **Working Memory**: Transient active information processing
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and recall

##### Neuromodulation Systems
- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Novelty detection and adaptive learning

##### Cognitive Systems
- **Neural Planner**: Action planning and sequence evaluation
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Focus and competitive selection dynamics

##### Development System
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult
- **Structural Plasticity**: Synaptogenesis and pruning
- **Plasticity Modulation**: Age-dependent learning rates

## System Integration Verification

The advanced demo includes comprehensive integration tests that verify:

```
=== Integration Status ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction System: CONNECTED
Development System: CONNECTED
Cognition Systems: CONNECTED
```

## Usage Examples

### Example 1: Basic Advanced Simulation

```bash
# Run a basic advanced demo with default settings
./nlm_phase6_advanced_demo
```

### Example 2: Large Scale Learning Experiment

```bash
# Run a larger simulation for more complex learning
./nlm_phase6_advanced_demo \
  --neurons=5000 \
  --steps=20000 \
  --timestep=0.005 \
  --seed=42 \
  --no-curiosity
```

### Example 3: Development-Focused Experiment

```bash
# Focus on brain development and maturation
./nlm_phase6_advanced_demo \
  --neurons=2000 \
  --steps=10000 \
  --no-plastic \
  --no-curiosity
```

## Learning Assessment

The demo provides automatic learning assessment:

**Significant Learning Detected** when:
- High reward accumulation (>100.0)
- Active concept formation (>10 concepts)
- Strong curiosity/exploration (>0.5 level)

**Limited Learning** when:
- Low reward accumulation
- Minimal concept formation
- Low curiosity/exploration

## Technical Details

### File Structure

- `src/experiments/Phase6AdvancedDemo.cpp`: Main advanced demo implementation
- `CMakeLists.txt`: Updated build system with new demo target
- `docs/ADVANCED_FEATURES.md`: This comprehensive guide

### Build Requirements

```bash
# Clone the repository
 git clone <repository-url>
 cd nlm

# Create build directory and configure
 mkdir build
 cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
 make -j$(nproc)

# Run the advanced demo
 ./nlm_phase6_advanced_demo
```

### Python Bindings

The advanced features are also available through Python bindings:

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure for advanced learning
config.set("brain.neuron_count", 5000)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world for interaction
world = pynlm.createSimpleWorld()
world.configure(width=32, height=32, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable all subsystems for advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation
for step in range(10000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 1000 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

## Performance Considerations

### Memory Usage

- **Neuron Count**: Scales linearly with memory (approx. 2KB per neuron)
- **World Size**: Larger worlds require more memory for sensory input
- **Memory Systems**: Episodic memory storage can be significant for long simulations

### Simulation Speed

- **Timestep**: Smaller timesteps provide more accuracy but slower performance
- **Neuron Count**: More neurons = slower simulation (approximately linear relationship)
- **World Resolution**: Higher resolution vision processing adds overhead

### Recommended Settings

| Use Case | Neurons | Timestep | Duration | System Load |
|----------|---------|----------|----------|-------------|
| Basic Learning | 1000 | 0.01 | 5000 steps | Low-Medium |
| Standard Research | 2000 | 0.01 | 10000 steps | Medium |
| Large-Scale Study | 5000 | 0.005 | 20000 steps | High |
| Detailed Analysis | 1000 | 0.001 | 50000 steps | High |

## Troubleshooting

### Common Issues and Solutions

**"The simulation is too slow"**
- Reduce neuron count: `--neurons=1000`
- Increase timestep: `--timestep=0.02`
- Disable curiosity: `--no-curiosity`

**"No learning occurs"**
- Enable all subsystems: ensure reward, development, and curiosity are enabled
- Check reward signals from world
- Verify configuration parameters are appropriate

**"Memory usage is high"**
- Reduce episodic memory duration
- Monitor and manage simulation length
- Use checkpointing for long simulations

**"Agent doesn't move"**
- Check motor neuron connectivity
- Verify action decoding is working
- Ensure sensory input is being processed

## Future Enhancements

### Planned Improvements

1. **Advanced Visualization**: Real-time brain activity visualization
2. **Export/Import**: Save/load complete brain states with metadata
3. **Multi-Agent Systems**: Support for multiple interacting agents
4. **Cloud Integration**: Distributed computing for large-scale simulations
5. **Advanced Statistics**: Machine learning-based performance analysis
6. **Custom Environments**: User-defined world scenarios

### Contribution Guidelines

When contributing to NLM advanced features:

1. **Code Quality**: Follow existing code style and conventions
2. **Documentation**: Update this guide with new features
3. **Testing**: Add comprehensive test cases
4. **Performance**: Profile and optimize new functionality
5. **Backward Compatibility**: Ensure existing code continues to work

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Python API Reference](docs/PYTHON_API.md)
- [Configuration Options](docs/CONFIGURATION.md)

## Version Information

- **Version**: 0.1.0 (Phase 6 Advanced)
- **Build**: Release configuration
- **Dependencies**: C++20, CMake 3.16+, Python 3.8+
- **License**: MIT

For additional support, refer to the main NLM documentation or contact the development team.