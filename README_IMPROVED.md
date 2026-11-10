# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

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

## Quick Start Guide

### Building the System

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running the Phase 2 Real Neural Computation Demo

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

## Key Features

### 1. Real Spiking Neural Computation
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### 2. Integrated Brain Systems
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction Systems**: Sensory prediction and error computation
- **Cognition Systems**: Neural planner, concept formation, attention
- **Development**: Developmental stages affecting plasticity rates

### 3. Advanced Learning Mechanisms
- **Enhanced STDP**: Multiple time windows for different spike intervals
- **Competitive Hebbian Learning**: Covariance-based learning with homeostatic control
- **Reward-Modulated Learning**: Eligibility traces for credit assignment
- **Pattern Separation**: Mechanisms to distinguish similar experiences

### 4. Development and Adaptation
- **Structural Plasticity**: Synapse creation and pruning based on activity
- **Developmental Stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Spike-Frequency Adaptation**: Automatic firing rate regulation
- **Homeostatic Plasticity**: Maintenance of network stability

## Advanced Features for Expert Users

### Command-Line Interface

NLM now supports advanced command-line options for expert users:

```bash
./nlm --help                    # Show advanced usage options
./nlm --verbose                 # Enable detailed logging
./nlm --benchmark              # Run performance benchmarks
./nlm --analyze-network        # Analyze network properties
./nlm --plasticity-type stdp   # Configure plasticity systems
./nlm --neuron_count 1000      # Customize neuron count
./nlm --output results.json    # Save experiment results
```

### Advanced Configuration

Create custom configuration files in the `configs/` directory:

```json
{
  "neuron_count": 500,
  "region_count": 2,
  "connection_probability": 0.1,
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "enable_stdp": true,
  "enable_hebbian": true,
  "enable_reward_modulation": true,
  "enable_curiosity": true,
  "enable_development": true
}
```

### Python Bindings

NLM provides Python bindings for programmatic access:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## Phase-Specific Features

### Phase 2: Real Neural Computation
- Core spiking neural dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3: World Interaction
- Experience → Neural Activity → Action → Consequence → Learning
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4: Advanced Cognition
- Neural prediction systems
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning

### Phase 5: Performance Optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

## Scientific Background

### Biological Inspiration

NLM is inspired by several key biological principles:

1. **Spiking Neurons**: Leaky Integrate-and-Fire model approximates real neuron dynamics
2. **Spike-Timing-Dependent Plasticity**: STDP implements Hebbian learning with precise timing
3. **Neuromodulation**: Dopamine and other neuromodulators regulate learning
4. **Developmental Trajectories**: Brain development from immature to mature states
5. **Memory Systems**: Working memory, episodic memory, and associative memory

### Key Research Questions Addressed

1. **Can neural systems develop selective pattern learning?**
2. **Do neuromodulators effectively regulate plasticity?**
3. **Can memory systems integrate with neural dynamics?**
4. **Does prediction become a central organizing principle?**
5. **Can developmental stages modulate learning appropriately?**

## Example: Phase 3 World Interaction

This example demonstrates how NLM learns from interaction with a simple world:

```cpp
// Phase 3 Demo: World Interaction
// Shows: experience -> neural activity -> action -> consequence -> learning

// Create world with resources and hazards
SimpleWorld world;
world.configure(20.0f, 20.0f, 16, 16);
world.setRandomSeed(42);

// Add objects for interaction
world.addObject(WorldObject(5.0f, 5.0f, WorldObjectType::Resource, 1.0f));
world.addObject(WorldObject(15.0f, 15.0f, WorldObjectType::Resource, 1.0f));
world.addObject(WorldObject(10.0f, 10.0f, WorldObjectType::Hazard, -2.0f));

// Create brain and agent
AgentBrain agentBrain(brain);
agentBrain.initialize(world);
agentBrain.enableRewardModulation(true);
agentBrain.enableStructuralPlasticity(true);
agentBrain.enableDevelopment(true);
agentBrain.enableCuriosity(true);

// Run episodes
for (int episode = 0; episode < 5; ++episode) {
    world.reset();
    agentBrain.reset();
    brain->reset();
    
    // Agent perceives world, acts, receives rewards, and modifies synapses
    SensoryPercept percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    // Brain processes input and produces action
    MotorCommand cmd = agentBrain.decodeMotorCommand();
    ActionResult result = world.applyMotorCommand(cmd);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(result.reward, 0.0f);
}
```

## Testing and Validation

### Running Tests

The project includes several test suites:

```bash
# Run all tests
cmake --build . --target test

# Run specific demos
./nlm_phase3_demo
./nlm_phase6_demo
./nlm_benchmark
```

### Test Categories

1. **Unit Tests**: Test individual components (neurons, synapses, plasticity)
2. **Integration Tests**: Test system interactions (memory, prediction, cognition)
3. **Performance Tests**: Benchmark system performance and scalability
4. **Learning Tests**: Verify learning mechanisms work correctly
5. **Validation Tests**: Ensure system outputs are biologically plausible

## Troubleshooting

### Common Issues and Solutions

#### Issue: 0 reward in Phase 3/Phase 6 demos
**Problem**: The demos report 0 reward because no objects are in the world for the agent to interact with.
**Solution**: Objects (resources and hazards) have been added to the world in both demos.

#### Issue: High spike rates (~200 spikes/step)
**Problem**: Excessive neural firing leading to saturation.
**Solution**: Enhanced neuron dynamics with improved firing rate limiting and refractory periods.

#### Issue: Learning saturation instead of selective pattern learning
**Problem**: All synapses change in the same direction, losing selectivity.
**Solution**: Enhanced STDP and Hebbian learning with competitive normalization.

#### Issue: Checkpointing problems
**Problem**: Save/load functionality not working correctly.
**Solution**: Implemented comprehensive checkpoint system with multiple section types.

## Technical Specifications

### System Architecture
- **Language**: C++20 with Python bindings
- **Build System**: CMake
- **Logging**: Custom logging framework
- **Memory Management**: Custom allocators and pools
- **Parallel Processing**: Thread-safe for concurrent operations

### Performance Characteristics
- **Simulation Speed**: 1ms timesteps for real-time simulation
- **Scalability**: Tested with 100-1000 neurons
- **Memory Usage**: Efficient with custom memory pools
- **Plasticity**: Real-time synaptic updates

### Configuration Options
- **Neural Parameters**: Neuron count, region count, connection probability
- **Plasticity Parameters**: STDP weights, learning rates, time constants
- **Development Parameters**: Developmental stages, pruning rates
- **Memory Parameters**: Working memory capacity, episodic memory parameters
- **Neuromodulation**: Dopamine levels, curiosity settings, novelty detection

## Future Development

### Planned Improvements

1. **Enhanced Learning Algorithms**
   - More sophisticated STDP variants
   - Spike-based learning rules
   - Reward-modulated plasticity

2. **Advanced Cognitive Systems**
   - Theory of mind capabilities
   - Causal reasoning
   - Meta-learning algorithms

3. **Development and Maturation**
   - More detailed developmental trajectories
   - Experience-dependent development
   - Ageing and senescence models

4. **Integration and Applications**
   - Sensor integration
   - Motor control
   - Social learning
   - Transfer learning

### Research Directions

1. **Closed-Loop Learning**: Real-world interaction and adaptation
2. **Transfer Learning**: Knowledge transfer between tasks
3. **Multi-Agent Systems**: Social learning and cooperation
4. **Developmental Psychology**: Modeling cognitive development
5. **Neuroscience**: Testing hypotheses about brain function

## Conclusion

NLM represents a significant step toward understanding how neural systems can develop, learn, and adapt through interaction with their environment. While this is an experimental system with limitations, it provides valuable insights into the principles of artificial brain development and learning.

The project successfully addresses many of the challenges mentioned in the README:

- ✅ **Fixed reward generation**: Added objects to Phase 3 and Phase 6 demos
- ✅ **Enhanced neuron dynamics**: Improved firing rate limiting and refractory periods
- ✅ **Improved learning mechanisms**: Enhanced STDP and Hebbian with competitive learning
- ✅ **Advanced features**: Added comprehensive command-line interface and configuration options
- ✅ **Better documentation**: Comprehensive documentation and examples

NLM continues to evolve as a research platform for exploring artificial developmental brains and their learning capabilities.
