# Enhanced NLM Documentation

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that simulates spiking neural networks. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

## Quick Start Guide

### Basic Brain Simulation

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Simulation

```python
def run_complete_simulation(steps=1000):
    """Run a complete agent simulation with world interaction."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
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
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            
    print(f"Simulation complete after {steps} steps")
    return brain, agent, world

# Run the simulation
run_complete_simulation(1000)
```

### Working Memory Example

```python
import pynlm

# Create brain with working memory
config = pynlm.createDefaultConfig()
config.set("working_memory.capacity", 50)
config.set("working_memory.similarity_threshold", 0.7)

brain = pynlm.createBrain(config)
brain.initialize()

# Store and retrieve experiences
working_memory = brain.getWorkingMemory()

# Simulate experience encoding
test_experience = [0.5, 0.8, 0.3, 0.9, 0.2]
working_memory.storeExperience(test_experience, 0.8)

# Retrieve similar experiences
retrieved = working_memory.retrieveSimilar(test_experience, 0.6)
print(f"Retrieved {len(retrieved)} similar experiences")
for i, exp in enumerate(retrieved):
    print(f"  Experience {i}: {exp}")
```

### Concept Formation Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

concept_formation = brain.getConceptFormation()

# Simulate pattern exposure
patterns = [
    [0.1, 0.2, 0.3, 0.4, 0.5],
    [0.15, 0.25, 0.35, 0.45, 0.55],  # Similar to first
    [0.9, 0.8, 0.7, 0.6, 0.5],      # Similar to second
    [0.95, 0.85, 0.75, 0.65, 0.55], # Similar to third
    [0.01, 0.02, 0.03, 0.04, 0.05], # Distinct pattern
]

# Process patterns
for i, pattern in enumerate(patterns):
    # Create sensory input (simplified)
    vision = pynlm.Vision(width=5, height=1)
    vision.setData(pattern)
    
    brain.receiveSensoryInput(vision)
    brain.step(i)
    
    # Get current concepts
    current_concepts = concept_formation.getCurrentConcepts()
    print(f"Step {i}: Found {len(current_concepts)} concepts")

# Analyze formed concepts
all_concepts = concept_formation.getAllConcepts()
print(f"\nTotal concepts formed: {len(all_concepts)}")
print("Sample concepts:")
for i, concept in enumerate(list(all_concepts.items())[:3]):
    print(f"  Concept {i}: similarity={concept.similarity:.3f}, stability={concept.stability:.3f}")
```

### Prediction and Planning Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()

# Set up prediction
prediction_system.setPredictionHorizon(10)

# Run prediction cycle
for step in range(50):
    # Get current state
    current_state = brain.produceAction()
    
    # Generate prediction
    prediction = prediction_system.generatePrediction(current_state, step)
    
    # Create action sequence
    action_sequence = []
    for t in range(prediction.prediction_horizon):
        if t < len(prediction.future_actions):
            action_sequence.append(prediction.future_actions[t])
    
    # Evaluate sequence
    action_quality = planner.evaluateActionSequence(action_sequence)
    
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Prediction error: {prediction.error:.3f}")
        print(f"  Action sequence length: {len(action_sequence)}")
        print(f"  Action quality: {action_quality:.3f}")
```

### Developmental Stage Simulation

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

development_system = brain.getDevelopmentSystem()

# Monitor development over time
print("Development Progress:")
for step in range(0, 100, 10):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    age = brain.getDevelopmentalStageAge()
    plasticity = development_system.getCurrentPlasticityRate()
    
    print(f"Step {step}: {stage.name} (age: {age:.2f}s)")
    print(f"  Plasticity rate: {plasticity:.3f}")
    print(f"  Neuronal growth: {development_system.getGrowthRate():.3f}")
    print(f"  Synaptogenesis rate: {development_system.getSynaptogenesisRate():.3f}")
    print()
```

### Checkpoint and Persistence Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

print("Brain state before save:")
brain.logStatus()

# Save checkpoint
checkpoint_file = "brain_checkpoint.bin"
save_success = brain.save(checkpoint_file)
print(f"\nSave successful: {save_success}")

# Simulate more learning
for i in range(100, 200):
    brain.step(i)

print("\nBrain state after additional learning:")
brain.logStatus()

# Load from checkpoint
load_success = brain.load(checkpoint_file)
print(f"\nLoad successful: {load_success}")

print("\nBrain state after loading checkpoint:")
brain.logStatus()
```

### Performance Monitoring Example

```python
import pynlm
import time

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor performance
print("Performance Monitoring:")
print("Step, Time(ms), Neurons, Spikes, Firing Rate, E/I Ratio")
print("-" * 70)

for step in range(100):
    start_time = time.time()
    brain.step(step)
    end_time = time.time()
    
    step_time = (end_time - start_time) * 1000  # Convert to ms
    total_neurons = brain.getTotalNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    firing_rate = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    
    if step % 10 == 0:
        print(f"{step:4d}, {step_time:8.2f}, {total_neurons:7d}, {total_spikes:6d}, "
              f"{firing_rate:10.2f}, {ei_ratio:8.3f}")
```

### Error Handling and Recovery Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Safe initialization
try:
    if not brain.initialize():
        print("Error: Brain initialization failed!")
        # Create new brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("Created new brain successfully")
except Exception as e:
    print(f"Exception during initialization: {e}")
    # Handle error appropriately
    raise

# Safe simulation with error handling
for step in range(100):
    try:
        brain.step(step)
        
        # Check for critical errors
        if brain.getTotalSpikeCount() > 1000000:  # Spike limit
            print(f"Warning: High spike count at step {step}")
            brain.reset()
            brain.initialize()
            
    except RuntimeError as e:
        print(f"Runtime error at step {step}: {e}")
        brain.reset()
        brain.initialize()
    except Exception as e:
        print(f"Unexpected error at step {step}: {e}")
        raise

print("Simulation completed successfully")
```

### Advanced Configuration Example

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Custom neuron parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.08)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0f)
config.set("plasticity.stdp.tau_minus", 25.0f)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.growth_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00005)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_rate", 0.3)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.8)

# Development
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_length", 50.0)
config.set("development.maturation_rate", 0.02)

# Memory systems
config.set("working_memory.capacity", 100)
config.set("working_memory.similarity_threshold", 0.6)

config.set("episodic_memory.capacity", 1000)
config.set("episodic_memory.consolidation_rate", 0.01)

# Print configuration summary
print("Custom Configuration Summary:")
for key in config.getKeys():
    value = config.getOr[float](key, 0.0f)
    print(f"  {key}: {value}")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
print(f"\nCreated brain with {brain.getTotalNeuronCount()} neurons")
```

## Configuration Options

### Neuron Configuration
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Connection density
- `brain.connection_probability`: Probability of connection
- `brain.initial_weight_mean`: Initial synaptic weight mean
- `brain.initial_weight_std`: Initial synaptic weight standard deviation

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP potentiation time constant
- `plasticity.stdp.tau_minus`: STDP depression time constant

### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection

### Development Configuration
- `development.initial_plasticity`: Initial plasticity rate
- `development.critical_period_length`: Critical period duration
- `development.maturation_rate`: Maturation rate

### Memory Configuration
- `working_memory.capacity`: Working memory capacity
- `working_memory.similarity_threshold`: Similarity threshold
- `episodic_memory.capacity`: Episodic memory capacity
- `episodic_memory.consolidation_rate`: Consolidation rate

## Performance Tips

### For Large Networks
```python
# Use these settings for larger networks
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)  # Lower density
config.set("plasticity.structural.enable", False)  # Disable structural plasticity
config.set("development.enable", False)  # Disable development
```

### For Fast Training
```python
# Optimize for training speed
config.set("simulation_timestep", 0.01)  # Larger timestep
config.set("plasticity.stdp.enable", False)  # Disable STDP
config.set("plasticity.hebbian.enable", True)  # Use simpler Hebbian learning
```

### For Detailed Analysis
```python
# For detailed analysis and debugging
config.set("brain.neuron_count", 100)  # Smaller network
config.set("visualization.enable", True)  # Enable visualization
config.set("logging.level", "Debug")  # Enable debug logging
```

## Troubleshooting Guide

### Common Issues and Solutions

#### "Brain is not firing"
**Problem**: No neurons are firing.
**Solution**: 
- Check that `brain.initialize()` was called
- Ensure `brain.step()` is being called
- Check configuration values (threshold, current injection)
- Verify that sensory input is being provided

#### "High memory usage"
**Problem**: Memory usage grows too quickly.
**Solution**:
- Reduce `brain.neuron_count`
- Disable structural plasticity
- Disable development system
- Use smaller timesteps

#### "Simulation is slow"
**Problem**: Simulation runs too slowly.
**Solution**:
- Use larger timesteps
- Reduce plasticity calculations
- Disable development system
- Use compiled version with optimizations

#### "NaN values"
**Problem**: Simulation produces NaN values.
**Solution**:
- Check configuration for invalid values
- Ensure all parameters are positive
- Add boundary checks in neural dynamics
- Use smaller timesteps

### Debug Scripts

```python
def debug_brain(brain, steps=100):
    """Debug brain state and look for issues."""
    print("Starting brain debug...")
    
    for step in range(steps):
        brain.step(step)
        
        # Check for common issues
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > 1000:  # Unrealistic firing rate
            print(f"Warning: High firing rate at step {step}: {firing_rate}")
            
        ei_ratio = brain.getExcitationInhibitionRatio()
        if ei_ratio < 0.1 or ei_ratio > 10:  # Unrealistic E/I ratio
            print(f"Warning: Unrealistic E/I ratio at step {step}: {ei_ratio}")
            
        # Check for NaN values
        if any(x != x for x in [firing_rate, ei_ratio]):  # NaN check
            print(f"Error: NaN values detected at step {step}")
            break
            
    print("Debug complete")
```

## Advanced Features

### Neural Architecture Search

```python
import pynlm

def create_brain_architecture(neuron_count, connection_density):
    """Create brain with specific architecture."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", connection_density)
    
    # Add regions based on architecture
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

# Test different architectures
architectures = [
    (100, 0.1),   # Small, sparse
    (1000, 0.05), # Medium, sparse
    (500, 0.2),   # Medium, dense
    (2000, 0.1),  # Large, sparse
]

for neurons, density in architectures:
    brain = create_brain_architecture(neurons, density)
    print(f"Created architecture: {neurons} neurons, {density} density")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
```

### Transfer Learning

```python
import pynlm
import shutil
import os

def save_brain_as_template(brain, filepath):
    """Save brain state as learning template."""
    # Save configuration
    config = brain.getConfig()
    config.saveToFile(f"{filepath}/config.json")
    
    # Save brain state
    brain.save(f"{filepath}/brain.bin")
    
    print(f"Saved brain template to {filepath}")

def load_brain_template(filepath):
    """Load brain as learning template."""
    # Load configuration
    config = pynlm.createDefaultConfig()
    if config.loadFromFile(f"{filepath}/config.json"):
        print(f"Loaded configuration from {filepath}")
    
    # Load brain state
    brain = pynlm.createBrain(config)
    if brain.load(f"{filepath}/brain.bin"):
        print(f"Loaded brain state from {filepath}")
    
    return brain

# Create and save a pre-trained brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate pre-training
for i in range(500):
    brain.step(i)

# Save as template
save_brain_as_template(brain, "pretrained_brain")

# Load and continue training
pretrained_brain = load_brain_template("pretrained_brain")
for i in range(500):
    pretrained_brain.step(i)

print("Transfer learning completed")
```

### Multi-Agent System

```python
import pynlm

class MultiAgentSystem:
    def __init__(self, num_agents, shared_brain_config):
        self.agents = []
        self.shared_brain = pynlm.createBrain(shared_brain_config)
        self.shared_brain.initialize()
        
        # Create agents that share the same brain
        for i in range(num_agents):
            agent = pynlm.createAgentBrain(self.shared_brain)
            agent.initialize(None)  # Will use world later
            self.agents.append(agent)
            
        print(f"Created multi-agent system with {num_agents} agents")
    
    def run_synchronized_training(self, steps=100):
        """Run synchronized training across all agents."""
        for step in range(steps):
            # Each agent processes its own input (in real system)
            for agent in self.agents:
                agent.updateDevelopment(0.1)
                
                # Apply learning signals
                reward = 0.1  # Simulated reward
                agent.applyRewardModulation(reward, 0.0)
            
            # Shared brain advances
            self.shared_brain.step(step)
            
            if step % 50 == 0:
                firing_rates = [agent.getCuriosityLevel() for agent in self.agents]
                print(f"Step {step}: Average curiosity: {sum(firing_rates)/len(firing_rates):.3f}")

# Create and run multi-agent system
config = pynlm.createDefaultConfig()
mas = MultiAgentSystem(5, config)
mas.run_synchronized_training(200)
print("Multi-agent training completed")
```

## Migration Guide

### From v0.1.0 to v0.2.0

1. **API Changes**:
   - `brain.getTotalSpikeCount()` added (was missing)
   - `brain.getPendingSpikeEventCount()` added (was missing)
   - `agent.enableRewardModulation(True/False)` added (was missing)
   - `agent.isRewardModulationEnabled()` added (was missing)

2. **Configuration Changes**:
   - `plasticity.structural.enable` renamed to `plasticity.structural.growth_enable`
   - `plasticity.structural.pruning_rate` added
   - `development.initial_plasticity` renamed to `development.plasticity_rate`

3. **New Methods Added**:
   - `brain.getActiveNeuronCount()`
   - `brain.getTotalSpikeCount()`
   - `brain.getPendingSpikeEventCount()`
   - `agent.enableStructuralPlasticity(True/False)`
   - `agent.isStructuralPlasticityEnabled()`
   - `world.getEnergyDecayRate()`
   - `world.setEnergyDecayRate()`
   - `Action.getName()`
   - `Action.clone()`

### Upgrading Tips

1. **Update your imports**: No changes needed, but check for new modules
2. **Update configuration**: Add new configuration keys
3. **Update method calls**: Add missing method calls
4. **Test thoroughly**: Run your existing code with the updated library
5. **Check documentation**: Review examples for new API

## Future Enhancements

### Planned Features

1. **New Sensory Modalities**: Touch, proprioception, vestibular sensing
2. **Advanced Plasticity Rules**: Homeostatic plasticity, reward prediction errors
3. **Memory Architectures**: Hierarchical memory, semantic networks
4. **Cognitive Architectures**: Working memory, attention systems, decision making
5. **Social Learning**: Imitation, cultural transmission, collaboration
6. **Developmental Stages**: Detailed developmental trajectory modeling
7. **Explainable AI**: Interpretability, explainability, transparency
8. **Hardware Acceleration**: GPU support, neuromorphic hardware interfaces

### Contribution Guidelines

1. **Report Issues**: Use GitHub Issues for bug reports and feature requests
2. **Code Contributions**: Fork and submit pull requests
3. **Documentation**: Improve examples, add new tutorials
4. **Testing**: Add new tests for edge cases
5. **Performance**: Optimize critical paths
6. **Compatibility**: Maintain backward compatibility

## References

1. ","Computational Brain Research,", NLM Project Documentation
2. ","Neural Networks and Learning Systems,", Research Papers
3. ","Spiking Neural Networks,", Technical Specifications
4. ","Python Bindings for C++ Libraries,", Pybind11 Documentation
5. ","Machine Learning for Neuroscience,", Research References

## License

MIT License - See LICENSE file for details

---

*Last updated: 2026-10-17*
*Version: 2.0.0*