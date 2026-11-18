# Advanced NLM Usage Guide

## Introduction

This guide covers advanced usage of the NLM (Neural Learning Machine) framework for creating sophisticated neural simulations, experimental setups, and real-world applications. It builds upon the basic usage guide with more complex patterns, configuration options, and performance optimizations.

## Advanced Configuration Management

### Phase 6 Configuration

NLM Phase 6 provides comprehensive configuration options for advanced neural simulations. Here's how to create sophisticated configurations:

```python
import pynlm

# Create advanced configuration with Phase 6 defaults
config = pynlm.createAdvancedConfig()

# Override specific parameters
config.set("brain.neuron_count", 2000)  # More neurons for complex tasks
config.set("brain.synapse_density", 0.15)  # Higher connectivity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("prediction.enabled", True)
config.set("development.enable", True)
config.set("working_memory.enabled", True)
config.set("episodic_memory.enabled", True)
config.set("self_model.enable", True)
config.set("social_learning.enable", True)

# Load from configuration file
config.loadFromFile("advanced_config.json")

# Save configuration to file
config.saveToFile("my_config.json")

# Get configuration summary
print(config.summaryWithValidation())
```

### Configuration Validation

NLM provides comprehensive configuration validation to catch errors early:

```python
# Create a configuration
config = pynlm.createBrain(config)

# Valid configuration
config.set("brain.neuron_count", 1000)  # Positive integer
config.set("plasticity.stdp.learning_rate", 0.001)  # Small positive
config.set("simulation_timestep", 0.001)  # Small timestep

# Configuration validation function
def validate_config(config):
    validators = {
        "brain.neuron_count": pynlm.Config.validate_positive,
        "brain.synapse_density": lambda v: pynlm.Config.validate_range(v, 0.0, 1.0),
        "simulation_timestep": lambda v: pynlm.Config.validate_range(v, 0.0001, 1.0),
        "plasticity.stdp.learning_rate": pynlm.Config.validate_positive,
    }
    return config.validate(validators)

# Validate configuration
if not validate_config(config):
    print("Configuration validation failed!")
```

## Advanced Brain Configuration

### Custom Neuron Parameters

Create neurons with specific biological properties:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Custom brain configuration
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.12)
config.set("brain.v_thresh", -50.0)  # Higher threshold
config.set("brain.v_rest", -65.0)    # Different resting potential
config.set("brain.tau_mem", 25.0)    # Slower membrane time constant
config.set("brain.tau_ref", 5.0)     # Longer refractory period

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

### Structural Plasticity Configuration

Configure synapse formation and pruning:

```python
# Configure structural plasticity
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
config.set("plasticity.structural.pruning_rate", 0.00005)
config.set("plasticity.structural.min_weight", 0.01)
config.set("plasticity.structural.max_weight", 2.0)

# Configure STDP parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.stdp.max_weight", 2.0)
config.set("plasticity.stdp.min_weight", 0.01)

# Configure Hebbian learning
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.association_strength", 0.05)
config.set("plasticity.hebbian.stability", 0.95)
```

## Advanced World and Agent Configuration

### Complex World Setup

Create sophisticated world environments:

```python
import pynlm

# Create advanced world configuration
world = pynlm.createSimpleWorld()
world.configure(
    width=100,           # Larger world
    height=100,          # Larger world
    visionWidth=16,      # Wider field of view
    visionHeight=16,     # Higher resolution vision
    energyDecayRate=0.05, # Slower energy decay
    maxEnergy=200.0      # Higher maximum energy
)

# Set agent starting position and orientation
world.setAgentStart(50.0, 50.0)

# Add interesting world objects
# Resources (positive value)
resource1 = pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=10.0)
resource2 = pynlm.WorldObject(75.0, 75.0, pynlm.WorldObjectType.Resource, value=15.0)

# Hazards (negative value)
hazard1 = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=-5.0)
hazard2 = pynlm.WorldObject(90.0, 90.0, pynlm.WorldObjectType.Hazard, value=-8.0)

world.addObject(resource1)
world.addObject(resource2)
world.addObject(hazard1)
world.addObject(hazard2)
```

### Advanced Agent Configuration

Configure agent with learning capabilities:

```python
import pynlm

# Create brain and world
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
pynlm.initializeAgent(agent, world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Configure agent-specific parameters
agent.enableReflexBonus(True)  # Vision-gated reflex bonus
agent.enableHazardSuppression(True)  # Hazard detection
agent.enableWinnerTakeAll(True)  # Lateral inhibition

# Check agent capabilities
print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
print(f"Agent motor output size: {agent.getMotorOutputSize()}")
print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
```

## Advanced Simulation Patterns

### Learning Pattern

Implement complex learning scenarios:

```python
def runAdvancedLearningSimulation(brain, world, agent, num_steps):
    """Run a simulation with advanced learning capabilities."""
    
    # Initialize world
    world.reset()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Enable advanced learning features
    agent.enableSocialLearning(True)
    agent.enableConceptFormation(True)
    agent.enableSelfModelLearning(True)
    
    # Track performance metrics
    metrics = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'firing_rates': [],
        'rewards': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = result.reward
        predicted_reward = agent.getExpectedReward()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        metrics['steps'].append(step)
        metrics['rewards'].append(reward)
        metrics['curiosity'].append(agent.getCuriosityLevel())
        metrics['novelty'].append(agent.getNoveltyLevel())
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.1f} Hz")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    return metrics

# Run advanced learning simulation
metrics = runAdvancedLearningSimulation(brain, world, agent, 2000)
```

### Memory Integration Pattern

Demonstrate memory and prediction integration:

```python
def runMemoryIntegrationSimulation(brain, world, agent, num_steps):
    """Run simulation demonstrating memory-prediction integration."""
    
    # Initialize
    world.reset()
    agent.initialize(world)
    
    # Enable memory systems
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enableAssociativeMemory(True)
    
    # Enable prediction system
    agent.enablePredictionSystem(True)
    
    # Track memory metrics
    memory_metrics = {
        'episodes_stored': 0,
        'prediction_accuracy': 0.0,
        'concept_count': 0,
        'working_memory_usage': 0.0
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with memory integration
        agent.processSensoryInput(percept)
        
        # Predict next state
        prediction = agent.getPrediction()
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update prediction based on outcome
        agent.updatePrediction(prediction, result)
        
        # Store in episodic memory
        agent.storeEpisodicMemory(percept, action, result.reward)
        
        # Apply neuromodulation
        reward = result.reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record memory metrics
        memory_metrics['episodes_stored'] = agent.getEpisodicMemoryCount()
        memory_metrics['prediction_accuracy'] = agent.getPredictionAccuracy()
        memory_metrics['concept_count'] = agent.getConceptCount()
        memory_metrics['working_memory_usage'] = agent.getWorkingMemoryUsage()
    
    return memory_metrics
```

### Advanced Experiment Pattern

Create comprehensive experimental setups:

```python
def runAdvancedExperiment(brain, world, agent, phases):
    """Run a multi-phase advanced experiment."""
    
    results = {}
    
    for phase_name, phase_config in phases.items():
        print(f"Running phase: {phase_name}")
        
        # Configure phase-specific settings
        world.configure(
            width=phase_config.get('world_width', 50),
            height=phase_config.get('world_height', 50),
            visionWidth=phase_config.get('vision_width', 16),
            visionHeight=phase_config.get('vision_height', 16)
        )
        
        # Configure agent for phase
        agent.enableRewardModulation(phase_config.get('reward_modulation', True))
        agent.enableCuriosity(phase_config.get('curiosity', True))
        agent.enableDevelopment(phase_config.get('development', True))
        
        # Run phase simulation
        phase_steps = phase_config.get('steps', 500)
        phase_results = runSimulation(brain, world, agent, phase_steps)
        
        results[phase_name] 'results' for phase_name, phase_results in phases.items()
```

## Performance Optimization

### Simulation Optimization

Tips for high-performance simulations:

```python
# Optimize brain configuration for performance
config = pynlm.createAdvancedConfig()

# Use larger regions for better spatial organization
config.set("brain.region_count", 4)
config.set("brain.neuron_count_per_region", 500)

# Optimize connectivity for performance
config.set("brain.connection_probability", 0.08)  # Fewer connections
config.set("brain.synapse_delay", 1)  # Minimal delay

# Configure memory systems for performance
config.set("working_memory.max_size", 100)
config.set("episodic_memory.max_episodes", 1000)
config.set("prediction.max_sequence_length", 50)

# Create optimized brain
brain = pynlm.createBrain(config)

# Use optimized world for performance
world = pynlm.createAdvancedWorld()  # Larger but more efficient

# Optimize agent configuration
agent = pynlm.createAgentBrain(brain)
agent.enablePerformanceMode(True)  # Enable performance optimizations
```

### Memory Management

Best practices for memory-efficient simulations:

```python
# Monitor memory usage
import tracemalloc

tracemalloc.start()

# Run simulation
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Take memory snapshot before simulation
snapshot1 = tracemalloc.take_snapshot()

# Run simulation for some time
for step in range(1000):
    # Simulation code here
    pass

# Take memory snapshot after simulation
snapshot2 = tracemalloc.take_snapshot()

# Compare memory usage
top_stats = snapshot2.compare_to(snapshot1, 'lineno')

print("Memory usage changes:")
for stat in top_stats[:10]:
    print(stat)

tracemalloc.stop()
```

## Debugging and Troubleshooting

### Debugging Tools

Use these debugging techniques for complex simulations:

```python
# Enable debug logging
import pynlm
pynlm.Logger.setGlobalLevel(pynlm.LogLevel.Debug)

# Create brain with debug configuration
config = pynlm.createAdvancedConfig()
config.set("logger.level", "debug")
brain = pynlm.createBrain(config)

# Add debug agents for monitoring
class DebugAgent:
    def __init__(self, agent):
        self.agent = agent
        self.step_count = 0
        self.metrics = {}
    
    def processSensoryInput(self, percept):
        # Log sensory input details
        print(f"Debug: Step {self.step_count}")
        print(f"  Vision: {percept.getVision()[:10]}...")
        print(f"  Touch: {percept.getTouch()[:10]}...")
        print(f"  Internal: {percept.getInternal()[:10]}...")
        
        self.agent.processSensoryInput(percept)
        self.step_count += 1

# Use debug agent
import pynlm
debug_agent = DebugAgent(agent)
```

### Error Handling

Robust error handling for production simulations:

```python
import pynlm
import traceback

class SafeSimulation:
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.world = None
        self.agent = None
    
    def setup(self):
        try:
            self.brain = pynlm.createBrain(self.config)
            self.world = pynlm.createSimpleWorld()
            self.agent = pynlm.createAgentBrain(self.brain)
            return True
        except Exception as e:
            print(f"Failed to setup simulation: {e}")
            traceback.print_exc()
            return False
    
    def run(self, num_steps):
        try:
            if not self.setup():
                return None
            
            for step in range(num_steps):
                try:
                    self.world.update(0.1)
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    self.brain.step(step)
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                except Exception as step_error:
                    print(f"Error at step {step}: {step_error}")
                    # Continue simulation despite errors
                    continue
            
            return self.get_results()
            
        except Exception as e:
            print(f"Simulation failed: {e}")
            traceback.print_exc()
            return None
    
    def get_results(self):
        # Return simulation results
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel()
        }

# Run safe simulation
simulation = SafeSimulation(config)
results = simulation.run(1000)
if results:
    print("Simulation completed successfully!")
    print(f"Final results: {results}")
else:
    print("Simulation failed.")
```

## Batch Processing and Automation

### Batch Simulation Runner

Run multiple simulations efficiently:

```python
import pynlm
import json
from pathlib import Path

class BatchSimulator:
    def __init__(self, output_dir="simulation_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
    
    def run_simulation(self, config, simulation_id):
        """Run a single simulation with given configuration."""
        print(f"Running simulation {simulation_id}")
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize agent
        pynlm.initializeAgent(agent, world)
        
        # Run simulation
        results = {
            'simulation_id': simulation_id,
            'steps': [],
            'metrics': {
                'total_spikes': 0,
                'firing_rate': 0.0,
                'curiosity': 0.0,
                'novelty': 0.0,
                'reward': 0.0,
                'concept_count': 0,
                'episodes_stored': 0
            }
        }
        
        for step in range(config.get('steps', 1000)):
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record results
            results['steps'].append(step)
            results['metrics']['total_spikes'] = brain.getTotalSpikeCount()
            results['metrics']['firing_rate'] = brain.getAverageFiringRate()
            results['metrics']['curiosity'] = agent.getCuriosityLevel()
            results['metrics']['novelty'] = agent.getNoveltyLevel()
            results['metrics']['reward'] = result.reward
            results['metrics']['concept_count'] = agent.getConceptCount()
            results['metrics']['episodes_stored'] = agent.getEpisodicMemoryCount()
            
            # Print progress
            if step % 100 == 0:
                print(f"  Step {step}: Reward={result.reward:.3f}, Curiosity={agent.getCuriosityLevel():.3f}")
        
        return results
    
    def run_batch(self, configs):
        """Run a batch of simulations."""
        all_results = []
        
        for i, config in enumerate(configs):
            results = self.run_simulation(config, i)
            all_results.append(results)
            
            # Save individual results
            output_file = self.output_dir / f"simulation_{i}.json"
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"  Saved results to {output_file}")
        
        # Save combined results
        combined_file = self.output_dir / "combined_results.json"
        with open(combined_file, 'w') as f:
            json.dump(all_results, f, indent=2)
        
        print(f"Saved combined results to {combined_file}")
        
        return all_results

# Define batch configurations
batch_configs = []

# Configuration 1: High learning
config1 = pynlm.createAdvancedConfig()
config1.set("brain.neuron_count", 1000)
config1.set("plasticity.stdp.learning_rate", 0.01)
config1.set("neuromod.curiosity.enable", True)
config1.set("development.enable", True)
batch_configs.append(config1)

# Configuration 2: Fast learning
config2 = pynlm.createAdvancedConfig()
config2.set("brain.neuron_count", 500)
config2.set("plasticity.stdp.learning_rate", 0.005)
config2.set("neuromod.curiosity.enable", False)
config2.set("development.enable", False)
batch_configs.append(config2)

# Configuration 3: Complex task
config3 = pynlm.createAdvancedConfig()
config3.set("brain.neuron_count", 2000)
config3.set("plasticity.stdp.learning_rate", 0.001)
config3.set("neuromod.curiosity.enable", True)
config3.set("neuromod.novelty.enable", True)
config3.set("development.enable", True)
config3.set("working_memory.enabled", True)
config3.set("episodic_memory.enabled", True)
batch_configs.append(config3)

# Run batch simulation
batch_simulator = BatchSimulator()
all_results = batch_simulator.run_batch(batch_configs)
```

## Custom Extensions

### Adding Custom Neuron Dynamics

Extend NLM with custom neuron models:

```cpp
// Custom neuron implementation (in your own source file)
class CustomLIFNeuron : public Neuron {
public:
    CustomLIFNeuron(NeuronId id) : Neuron(id) {}
    
    // Override stepLIF with custom dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom dynamics implementation
        // ...
    }
};

// Register custom neuron with Python bindings
PYBIND11_MODULE(custom_neuron, m) {
    py::class_<CustomLIFNeuron, std::shared_ptr<CustomLIFNeuron>>(m, "CustomLIFNeuron")
        .def(py::init<NeuronId>())
        .def("stepLIF", &CustomLIFNeuron::stepLIF);
}
```

### Custom Plasticity Rules

Add custom learning rules:

```cpp
// Custom plasticity rule
class CustomPlasticityRule : public PlasticityRule {
public:
    void update(Neuron* pre, Neuron* post, Timestamp time, double dt) override {
        // Custom plasticity implementation
        // ...
    }
};

// Register with NLM
void registerCustomPlasticity() {
    // Register custom rule with NLM's plasticity system
    // ...
}
```

## Best Practices

### Development Best Practices

1. **Configuration Validation**: Always validate configuration before using it
2. **Memory Management**: Monitor memory usage in long-running simulations
3. **Error Handling**: Implement robust error handling for production use
4. **Logging**: Use structured logging for debugging and monitoring
5. **Testing**: Write comprehensive tests for custom configurations

### Performance Best Practices

1. **Profile Before Optimizing**: Identify bottlenecks before making changes
2. **Use Appropriate Data Types**: Choose correct data types for performance
3. **Minimize Object Creation**: Reuse objects when possible
4. **Parallel Processing**: Use available parallel processing capabilities
5. **Cache Frequently Accessed Data**: Cache data that is accessed repeatedly

### Design Best Practices

1. **Separation of Concerns**: Keep different responsibilities in separate modules
2. **Clear Interfaces**: Design clear interfaces between components
3. **Determinism**: Use explicit random seeds for reproducible experiments
4. **Extensibility**: Design components to be easily extended
5. **Documentation**: Document all public APIs and configuration options

## Troubleshooting Common Issues

### Simulation Runs Too Slowly

```python
# Optimization tips
config.set("brain.connection_probability", 0.05)  # Fewer synapses
config.set("brain.region_count", 2)  # Fewer regions
config.set("plasticity.stdp.enable", False)  # Disable STDP if not needed
config.set("development.enable", False)  # Disable development if not needed
```

### High Memory Usage

```python
# Memory optimization
config.set("working_memory.max_size", 50)
config.set("episodic_memory.max_episodes", 100)
config.set("prediction.max_sequence_length", 20)
```

### Learning Does Not Occur

```python
# Check learning configuration
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Check plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

### Simulation Instability

```python
# Stabilize simulation
config.set("brain.v_thresh", -55.0)
config.set("brain.v_rest", -70.0)
config.set("simulation_timestep", 0.001)
```

## Conclusion

This advanced usage guide provides comprehensive information for working with NLM at a professional level. The framework offers extensive customization options for neural simulation, learning, and experimentation. By following these best practices and using the advanced features outlined in this guide, you can create sophisticated neural simulations that push the boundaries of artificial intelligence research.

The key to mastering NLM is understanding the balance between biological plausibility and computational efficiency, and this guide provides the tools and knowledge needed to achieve that balance.