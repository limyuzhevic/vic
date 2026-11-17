# NLM (Neural Learning Machine) - Phase 2

## Overview

NLM is a revolutionary artificial neural substrate designed to mimic the brain's learning and adaptation capabilities. This Phase 2 release implements real spiking neural computation with:

- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis/pruning)**

This neural simulation framework is implemented in C++ with Python bindings, making it accessible for both high-performance computing and interactive development.

## Key Features

### Core Neural Simulation
- Real spiking neuron models with biologically plausible dynamics
- Event-driven computation for efficient simulation
- Multi-region neural architectures
- Comprehensive plasticity mechanisms

### Advanced Learning Capabilities
- Spike-Timing-Dependent Plasticity (STDP)
- Hebbian learning rules
- Structural plasticity for synapse formation and elimination
- Reward-based neuromodulation

### Development and Adaptation
- Multi-stage developmental trajectory
- Curiosity-driven exploration
- Novelty detection and habituation
- Personality traits learned from experience

### Cognitive Systems
- Working memory for transient information
- Episodic memory for experience storage
- Associative memory for pattern recognition
- Prediction systems for forward planning
- Concept formation for abstract reasoning
- Attentional selection for focus management

## Architecture Overview

The NLM framework is organized into several key modules:

1. **Core Neural Engine**
   - Brain: Central coordinator and simulation engine
   - NeuralRegion: Container for neuron populations
   - Neuron: Individual spiking neurons
   - Synapse: Connection between neurons

2. **Memory Systems**
   - NeuralWorkingMemory: Transient active information
   - NeuralEpisodicMemory: Experience storage
   - NeuralAssociativeMemory: Pattern associations

3. **Sensory and Motor**
   - SensoryInput: Environmental data reception
   - Action: Motor output representation

4. **Agent Interface**
   - AgentBrain: Bridge between brain and world
   - AgentBody: Agent physical representation

5. **Supporting Systems**
   - Config: Configuration management
   - RandomGenerator: Deterministic randomness
   - Logger: Event logging and debugging

## Quick Start

### Python Installation

```bash
pip install nlm
```

### Basic Usage

```python
from nlm import Config, createBrain, createSimpleWorld, createAgentBrain

# Create a configured brain
brain = createBrain()

# Create a world
world = createSimpleWorld()

# Create agent with world
agent_brain = createAgentBrainWithWorld(brain, world)

# Initialize
agent_brain.initializeWithWorld(world)

print("NLM brain system initialized and ready for simulation!")
```

### Advanced Training Setup

```python
from nlm import createTrainingSetup

# Create complete training environment
env = createTrainingSetup()

# Configure for training
brain = env["brain"]
world = env["world"]
agent_brain = env["agentBrain"]

# Enable all features for advanced learning
agent_brain.enableAllFeatures()
```

### Running the Simulation

```python
# Run a simple simulation step
brain.step(current_step=100, current_time=0.1)

# Inject sensory input
percept = brain.createDefaultSensoryInput(64)
brain.receiveSensoryInput(percept)

# Generate action
action = brain.produceAction()
world.applyMotorCommand(action, current_time=0.1)
```

## Configuration

### Default Configuration

The system uses sensible defaults, but you can customize via configuration file or command line:

```bash
./nlm --config=custom.cfg
./nlm --neuron_count=500 --region_count=2
```

### Configuration File Format

```ini
# configs/default.cfg
# Simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

## Development Features

### Command Line Interface

```bash
# View version information
./nlm --version

# Run with custom configuration
./nlm --config=experiment.cfg

# View configuration summary
./nlm --help
```

### Python API

The Python bindings provide a complete interface to the NLM framework:

```python
# Configuration management
config = createDefaultConfig()
config.setInt("neuron_count", 1000)
config.setDouble("simulation_timestep", 0.0005)

# Brain management
brain = createBrain(config)
brain.initialize()

# Sensory processing
sensory = brain.createDefaultSensoryInput(128)
brain.receiveSensoryInput(sensory)

# Action generation
action = brain.produceAction()
```

## Advanced Features

### Agent-Based Learning

```python
# Create sophisticated agent
world = createSimpleWorld()
brain = createDefaultBrain()
agent = createAgentBrainWithWorld(brain, world)
agent.initializeWithWorld(world)
agent.enableAllFeatures()  # Enable reward modulation, plasticity, development, curiosity

# Run training episode
for step in range(1000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, step)
    
    # Apply reward modulation
    agent.applyModulation(result.reward, result.predicted_reward)
```

### Personality System

```python
# Export personality traits
personality = agent.exportPersonality()
print(f"Agent personality: {personality}")

# Import personality to new agent
new_agent = createAgentBrainWithWorld(createBrain(), createSimpleWorld())
new_agent.importPersonality(personality)
```

### Network Analysis

```python
# Analyze brain connectivity
connectivity = brain.analyzeNetworkConnectivity()
print(f"Connectivity statistics: {connectivity}")

# Get region information
regions = brain.getConnectedRegions(RegionId(0))
print(f"Connected regions: {len(regions)}")
```

## Example Applications

### 1. Classical Conditioning

```python
# Simulate Pavlovian conditioning
brain = createDefaultBrain()
world = createSimpleWorld()
agent = createAgentBrainWithWorld(brain, world)
agent.initializeWithWorld(world)

# Create conditioned stimulus (bell)
bell = brain.createDefaultSensoryInput(32)

# Create unconditioned stimulus (food)
food = brain.createDefaultSensoryInput(32)

# Present bell repeatedly (CS)
for step in range(100):
    brain.receiveSensoryInput(bell)
    brain.step(step)

# Present food (US) with reward
agent.applyModulation(1.0, 1.0)  # Strong reward
brain.receiveSensoryInput(food)
brain.step(step)
```

### 2. Spatial Navigation

```python
# Train agent to navigate maze
world = createSimpleWorld()
world.configure(200, 200, 80, 80)  # Larger world
brain = createDefaultBrain()
agent = createAgentBrainWithWorld(brain, world)
agent.initializeWithWorld(world)

# Train for navigation
for episode in range(10):
    # Reset agent and world
    agent.resetEpisode()
    world.reset()
    
    # Navigate to target
    while not agent.arrivedAtTarget():
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, episode)
```

### 3. Cognitive Development

```python
# Simulate cognitive development over time
agent = createAgentBrainWithWorld(createDefaultBrain(), createSimpleWorld())

# Evolve through developmental stages
for stage in [DevelopmentalStage::Initial, 
             DevelopmentalStage::CriticalPeriod,
             DevelopmentalStage::Maturation,
             DevelopmentalStage::Adult]:
    agent.getBrain().setDevelopmentalStage(stage)
    agent.updateDevelopmentSystem(0.1)  # 100ms development time
    
    # Observe increasing capabilities
    print(f"Development stage: {stage}")
    print(f"Curiosity level: {agent.getCuriosityLevel()}")
    print(f"Novelty detection: {agent.getNoveltyLevel()}")
```

## Performance Considerations

### Memory Management

- **Checkpoints**: Use `brain.save("checkpoint.bin")` to save state
- **Loading**: Load saved states with `brain.load("checkpoint.bin")`
- **Cleanup**: Regularly clear working memory with `brain.getWorkingMemory().clear()

### Simulation Speed

```python
# Optimize for speed
brain = createBrain()
brain.initialize()

# Use batch processing where possible
for step in range(1000):
    brain.step(step, step * 0.001)
```

### Parallel Processing

```python
# Create multiple brains for parallel training
brains = [createDefaultBrain() for _ in range(4)]
for brain in brains:
    brain.initialize()

# Run parallel simulations
for i, brain in enumerate(brains):
    brain.step(i * 10)
```

## Troubleshooting

### Common Issues

1. **Configuration Loading Errors**
```bash
# Ensure config files exist
ls configs/
# Check for syntax errors
nano configs/default.cfg
```

2. **Simulation Instability**
```python
# Reduce simulation complexity
brain = createBrain()
brain.getConfig().setInt("neuron_count", 100)  # Smaller network
brain.initialize()
```

3. **Memory Issues**
```python
# Monitor memory usage
brain = createBrain()
brain.initialize()
brain.logStatus()  # View memory statistics
```

4. **Performance Issues**
```python
# Profile performance
import time
start = time.time()
brain.step(100)
end = time.time()
print(f"Step time: {end - start:.4f} seconds")
```

### Getting Help

- **Documentation**: Check the detailed API documentation in the `docs/` directory
- **Examples**: Look at example code in the `examples/` directory
- **Issues**: Report bugs at the NLM GitHub repository
- **Support**: Join the NLM community for discussion and support

## Contributing

### Code Quality Standards

- **Documentation**: Maintain comprehensive docstrings and comments
- **Testing**: Ensure all changes have corresponding tests
- **Performance**: Profile and optimize critical paths
- **Memory Safety**: Use RAII and avoid raw pointers where possible

### Development Workflow

1. **Feature Development**: Implement new features in feature branches
2. **Code Review**: Submit pull requests for peer review
3. **Testing**: Run comprehensive test suites
4. **Documentation**: Update documentation and examples
5. **Release**: Create releases with appropriate versioning

## Future Directions

### Planned Enhancements

- **Multi-Modal Sensory Input**: Integrate vision, audio, touch, and proprioception
- **Complex Motor Control**: Advanced motor planning and execution
- **Social Learning**: Multi-agent cooperation and competition
- **Meta-Learning**: Learning how to learn more efficiently
- **Explainability**: Interpret neural activity and decision-making

### Research Opportunities

- **Neuromodulation**: Advanced reward systems and emotional modeling
- **Development**: Lifespan development and aging
- **Plasticity**: Activity-dependent structural changes
- **Cognition**: Higher-level cognitive processes and reasoning

## License

MIT License - See LICENSE file for details.

## Acknowledgments

The NLM framework builds on decades of neuroscience research and artificial intelligence development. Special thanks to the open-source community and research collaborators who have contributed to neural simulation and machine learning.

---

*Phase 2 Release - Real Neural Computation*
*Version 0.1.0*
*© 2026 NLM Authors*