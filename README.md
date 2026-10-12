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

## Python Installation

The NLM Python bindings are packaged using scikit-build-core:

```bash
# Install from source (recommended for development)
pip install pybind11 scikit-build-core pytest numpy
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Verify installation
import pynlm
print(pynlm.__version__)
```

## Running Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Key Improvements

### Phase 2: Real Neural Computation ✅
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: Environment Interaction ✅
- World simulation with agent interface
- Sensory processing and motor decoding
- Reward prediction error and neuromodulation
- Developmental stages affecting plasticity

### Phase 4: Emerging Cognition ✅
- Neural prediction system
- Working memory with persistent activity
- Episodic memory with experience encoding
- Concept formation from experience patterns
- Neural attention with competitive selection
- Predictive planning
- Self-model development
- Social learning capabilities

### Phase 5: Performance Optimizations ✅
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration ✅
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Python API Highlights

### Enhanced Neuromodulation
- **Improved Dopamine dynamics**: Real reward signaling with prediction error response
- **Enhanced Curiosity**: Better novelty detection and exploration behavior
- **Prediction Error**: More accurate error signal computation for learning

### Advanced Configuration
- Comprehensive configuration system with JSON/YAML support
- Command-line argument parsing
- Runtime configuration modification
- Configuration validation and type safety

### Complete Agent Interface
- **SensoryTransduction**: Multi-modal input processing (vision, touch, internal, audio)
- **MotorDecoding**: Neural activity to motor command conversion
- **Neuromodulation**: Reward-based learning and adaptation
- **Development**: Age-dependent plasticity and maturation

### Memory Systems
- **Working Memory**: Persistent activity for temporary information storage
- **Episodic Memory**: Experience encoding and pattern completion
- **Associative Memory**: Experience-based associations
- **Concept Formation**: Pattern discovery without explicit labels

### Prediction System
- **Temporal Sequence Learning**: Pattern recognition in time sequences
- **ActionConsequencePrediction**: Predicting effects of actions
- **Multi-step Prediction**: Planning multiple steps ahead
- **Error-Driven Learning**: Refinement based on prediction errors

## Scientific Features

### Biological Inspiration
- **Synaptic Plasticity**: Hebbian and STDP mechanisms
- **Neural Dynamics**: LIF neuron models with leak and refractory periods
- **Developmental Plasticity**: Critical periods and maturation
- **Neuromodulation**: Dopamine, novelty, curiosity systems

### Computational Advances
- **Efficient Spike Processing**: Event-driven simulation with delays
- **Memory Integration**: Neural memory systems with persistent activity
- **Predictive Processing**: Forward models and error minimization
- **Competitive Attention**: Lateral inhibition and winner-take-all dynamics

### Research Capabilities
- **Developmental Learning**: Learning across lifespan stages
- **Social Learning**: Observation and imitation of other agents
- **Cognition Emergence**: Investigation of how complex cognition emerges from simple neural dynamics
- **Autonomous Development**: Systems that develop capabilities from experience

## Architecture Philosophy

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Usage Examples

### Minimal Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Complete Agent Example

```python
def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

## System Requirements

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Memory**: Minimum 1GB RAM for basic operation
- **Storage**: Checkpoint files may require several GB for large simulations

## Performance Considerations

- **Large Simulations**: Use Release builds for best performance
- **Memory Usage**: Monitor brain size and neuron count
- **Time Steps**: Balance simulation accuracy with performance
- **Parallel Processing**: Enable OpenMP for multi-core systems

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

## Testing

### Unit Tests

Run the comprehensive unit test suite:

```bash
python tests/test_nlm.py
```

The test suite covers:
- Configuration management
- Core brain operations
- Agent brain interface
- World simulation
- Integration scenarios

### Integration Tests

Run Phase 6 integration tests:

```bash
./nlm_phase6_demo
```

This tests the complete integrated system with all subsystems connected.

### Performance Benchmarks

Run performance benchmarks:

```bash
./nlm_benchmark
```

Benchmarks include:
- Neuron firing rate
- Simulation speed
- Memory usage
- Scalability testing

## Development Workflow

### Adding New Features

1. **Core Components**: Add new C++ classes in `src/` with appropriate headers
2. **Python Bindings**: Extend `python/bindings.cpp` with pybind11 bindings
3. **Documentation**: Update `docs/API_REFERENCE.md` and other documentation
4. **Testing**: Add tests to `tests/` directory
5. **Configuration**: Add new configuration keys to default configs

### Modifying Existing Systems

1. **Neuromodulators**: Enhance dopamine, curiosity, novelty systems
2. **Memory Systems**: Add new memory types or improve existing ones
3. **Plasticity Rules**: Implement new learning algorithms
4. **Development**: Add new developmental stages or mechanisms

## Code Quality Standards

### C++ Code Style
- C++20 with `-Wall -Wextra -Wpedantic`
- Pimpl idiom for implementation hiding
- Exception safety and resource management
- Clear, self-documenting code

### Python API Design
- Intuitive class interfaces
- Comprehensive docstrings
- Type safety and validation
- Consistent naming conventions

### Testing Standards
- Comprehensive unit test coverage
- Integration testing for system components
- Performance testing for scalability
- Error condition testing

## Project Roadmap

### Near Term (Next 3 months)
- ✅ Complete Phase 6 integration testing
- ✅ Finalize Python bindings documentation
- ✅ Implement advanced neuromodulation features
- ✅ Add performance monitoring tools

### Medium Term (6-12 months)
- Enhance memory systems with capacity management
- Implement more sophisticated developmental models
- Add social learning and interaction capabilities
- Optimize for larger-scale simulations

### Long Term (1-2 years)
- Implement cortical architectures
- Add cognitive modeling capabilities
- Develop educational and research tools
- Create cloud-based simulation services

## Contributing

### Code Contributions
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add comprehensive tests
5. Update documentation
6. Submit pull request

### Documentation Contributions
1. Improve existing documentation
2. Add examples and tutorials
3. Create API reference materials
4. Add scientific background information

### Research Contributions
- Submit research papers based on NLM findings
- Share configuration files and experimental results
- Contribute to the scientific community
- Participate in collaborative projects

## Acknowledgements

This project builds on the foundations of:
- Biological brain research and neural computation
- Spiking neural network theory
- Neuromodulation and learning systems
- The open-source community and its contributions
- Research in computational neuroscience

## Contact

For questions, issues, or collaboration:
- GitHub Issues: [repository issues]
- Documentation: [documentation links]
- Research Papers: [publication links]
- Community: [community links]

## License

MIT License. See `LICENSE` file for details.

## Authors

Research project — See docs for scientific background.

## Version Information

**Current Version**: 0.1.0 (Research Alpha)
**Phase**: Phase 6 Complete (Final Integration)
**Status**: Actively maintained and developed
