# NLM User Guide - Intermediate

## Welcome to Intermediate NLM!

This guide builds on the beginner guide and introduces more sophisticated concepts. You should be comfortable with the basic NLM workflow before diving into this guide.

## Intermediate Concepts

### 1. Configuration Management

The brain configuration controls how the brain works. Let's explore different approaches:

```python
import pynlm

# Method 1: Default configuration
config1 = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config1)

# Method 2: Standard configuration (better for experiments)
config2 = pynlm.createStandardConfig()
brain2 = pynlm.createBrain(config2)

# Method 3: Custom exploration configuration
config3 = pynlm.createExplorationConfig()
brain3 = pynlm.createBrain(config3)
```

### 2. Brain Configuration Parameters

Key configuration options for advanced usage:

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Modify key parameters
config.set("neuron_count", 1000)              # More neurons = more processing
config.set("region_count", 3)                 # Multiple brain regions
config.set("connection_probability", 0.08)   # Dense connectivity
config.set("stdp_ltp_weight", 0.01)           # Hebbian learning rate
config.set("stdp_ltd_weight", 0.012)          # STDP learning rate
config.set("synaptogenesis_rate", 0.0001)    # New synapse formation
config.set("pruning_rate", 0.00001)          # Synapse removal
config.set("random_seed", 42)                 # Reproducible results
config.set("simulation_timestep", 0.001)      # Time resolution

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Multi-Region Brain Architecture

Creating brains with multiple regions mimics the complexity of real brains:

```python
import pynlm

# Create multi-region brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 4)  # 4 regions like cortex, hippocampus, amygdala, etc.

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain has {brain.getRegionCount()} regions")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: properties here")
```

### 4. Advanced Sensory Processing

Working with different sensory modalities:

```python
import pynlm

# Create agent system
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Advanced simulation with detailed monitoring
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get complete sensory input
    percept = world.getSensoryPercept()
    
    # Process each sensory modality
    if hasattr(percept, 'getVision'):
        vision = percept.getVision()
        # Process visual data
        
    if hasattr(percept, 'getInternal'):
        internal = percept.getInternal()
        # Process internal signals
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply sophisticated reward
    reward = calculate_reward(world, brain, agent, step)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Detailed logging
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### 5. Learning System Integration

Enable and understand the different learning subsystems:

```python
def setup_advanced_learning(agent):
    """Setup comprehensive learning capabilities."""
    # Core learning systems
    agent.enableRewardModulation(True)        # Reward-based learning
    agent.enableStructuralPlasticity(True)    # Create/remove connections
    agent.enableDevelopment(True)             # Brain maturation
    agent.enableCuriosity(True)               # Exploration motivation
    
    return {
        'reward_modulation': agent.isRewardModulationEnabled(),
        'structural_plasticity': agent.isStructuralPlasticityEnabled(),
        'development': agent.isDevelopmentEnabled(),
        'curiosity': agent.isCuriosityEnabled()
    }

# Usage
agent = pynlm.createAgentBrain(brain)
learning_systems = setup_advanced_learning(agent)

print("Learning systems enabled:")
for system, enabled in learning_systems.items():
    print(f"  {system}: {'✓' if enabled else '✗'}")
```

### 6. Development Stages

Understanding and working with brain development:

```python
import pynlm

# Create brain for developmental study
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()
agent.initialize(world)

agent.enableDevelopment(True)

# Track development over time
development_log = []
for step in range(200):
    world.update(0.1)
    
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log development stage
    stage = brain.getDevelopmentalStage()
    development_log.append({
        'step': step,
        'stage': stage,
        'neuromodulation': agent.getNeuromodulationLevel()
    })
    
    if step % 20 == 0:
        print(f"Step {step}: {stage}, Neuromod={agent.getNeuromodulationLevel():.2f}")

# Analyze development
print("\nDevelopment Analysis:")
stages_reached = set(entry['stage'] for entry in development_log)
print(f"Stages reached: {stages_reached}")
print(f"Final stage: {development_log[-1]['stage']}")
```

### 7. Neuromodulation Systems

Working with dopamine, curiosity, and novelty detection:

```python
import pynlm

# Create comprehensive neuromodulation system
config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

world = pynlm.createSimpleWorld()
world.configure(25, 25, 10, 10)
world.reset()
agent.initialize(world)

# Run neuromodulation experiment
for step in range(300):
    # Update world and get perception
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate and apply sophisticated reward
    reward = calculate_advanced_reward(world, brain, agent, step)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log neuromodulation levels
    if step % 30 == 0:
        print(f"Step {step}:")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
        print(f"  Prediction Error: {agent.getPredictionError():.2f}")
```

### 8. World Configuration and Interaction

Advanced world setup and object interaction:

```python
import pynlm

# Create sophisticated world
world = pynlm.createSimpleWorld()

# Configure with specific parameters
world.configure(
    width=50,                    # Large world
    height=50,
    visionWidth=20,              # Good visual range
    visionHeight=20
)

# Set various world properties
world.setMaxEnergy(200.0)        # High energy capacity
world.setEnergyDecayRate(0.01)    # Slower energy decay
world.setRandomSeed(12345)        # Reproducible randomness

# Reset and start
world.reset()
world.setAgentStart(25.0, 25.0)

# Create agent system
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Add environmental objects
add_environment_objects(world)

# Run exploration simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get perception and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for special events
    handle_environment_events(world, brain, agent, step)
    
    # Apply sophisticated reward system
    reward = calculate_comprehensive_reward(world, brain, agent, step)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log key metrics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
        print(f"  Energy: {world.getAgentBody().energy:.1f}")
        print(f"  Age: {world.getAgentBody().age}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
```

### 9. Performance Monitoring

Track and analyze simulation performance:

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self):
        self.timings = []
        self.metrics = []
        
    def monitor_step(self, brain, agent, world, step):
        """Monitor a single simulation step."""
        start_time = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = calculate_reward(world, brain, agent, step)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Measure time
        end_time = time.time()
        step_time = end_time - start_time
        self.timings.append(step_time)
        
        # Collect metrics
        self.metrics.append({
            'step': step,
            'time': step_time,
            'reward': reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        return step_time

# Usage
monitor = PerformanceMonitor()
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()
agent.initialize(world)

# Run with monitoring
for step in range(500):
    monitor.monitor_step(brain, agent, world, step)
    
    if step % 100 == 0:
        avg_time = sum(m['time'] for m in monitor.metrics) / len(monitor.metrics)
        print(f"Step {step}: Avg time {avg_time*1000:.1f}ms")

# Performance analysis
print("\nPerformance Analysis:")
print(f"Total steps: {len(monitor.timings)}")
print(f"Average step time: {sum(monitor.timings)/len(monitor.timings)*1000:.1f}ms")
print(f"Min step time: {min(monitor.timings)*1000:.1f}ms")
print(f"Max step time: {max(monitor.timings)*1000:.1f}ms")
print(f"Steps per second: {len(monitor.timings)/sum(monitor.timings):.0f}")
```

### 10. State Management and Persistence

Save and load brain states:

```python
import pynlm

# Create and run brain
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

print("Running simulation...")
print(f"Final spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Save brain state
brain.save("experiment_brain.bin")
print("Brain state saved to experiment_brain.bin")

# Create new brain and load saved state
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
brain2.initialize()

success = brain2.load("experiment_brain.bin")
if success:
    print("Brain state loaded successfully!")
    print(f"Loaded neurons: {brain2.getTotalNeuronCount()}")
    print(f"Loaded synapses: {brain2.getTotalSynapseCount()}")
else:
    print("Failed to load brain state")
```

### 11. Configuration File I/O

Working with configuration files:

```python
import pynlm
import json

# Create configuration
config = pynlm.createStandardConfig()
config.set("custom_param", "test_value")

# Save to JSON file
config.saveToFile("my_config.json")

# Load from JSON file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Save brain to checkpoint
brain.save("checkpoint.bin")

# Load brain from checkpoint
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("checkpoint.bin")
```

### 12. Experiment Framework

Creating structured experiments:

```python
import pynlm

class NLMExperiment:
    def __init__(self, name, description):
        self.name = name
        self.description = description
        self.results = []
        self.config = pynlm.createStandardConfig()
        self.brain = None
        self.agent = None
        self.world = None
        
    def setup(self):
        """Setup experiment components."""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def run(self, num_steps):
        """Run experiment."""
        for step in range(num_steps):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            reward = self.calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            self.record_step(step, reward)
            
    def calculate_reward(self):
        """Calculate experimental reward."""
        body = self.world.getAgentBody()
        x, y = body.x, body.y
        width, height = self.world.getWidth(), self.world.getHeight()
        
        # Simple reward for staying in bounds
        return 1.0 if (0 <= x < width and 0 <= y < height) else -1.0
        
    def record_step(self, step, reward):
        """Record step data."""
        self.results.append({
            'step': step,
            'reward': reward,
            'curiosity': self.agent.getCuriosityLevel(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'development_stage': self.brain.getDevelopmentalStage()
        })

# Use experiment framework
experiment = NLMExperiment("Exploration Study", "Study how agents explore environments")
experiment.setup()
experiment.run(1000)

# Analyze results
print(f"Experiment completed {len(experiment.results)} steps")
print(f"Average reward: {sum(r['reward'] for r in experiment.results)/len(experiment.results):.2f}")
print(f"Final curiosity: {experiment.results[-1]['curiosity']:.2f}")
```

### 13. Visualization and Data Collection

Collecting data for visualization and analysis:

```python
import pynlm
import json

class VisualizationDataCollector:
    def __init__(self):
        self.data = []
        
    def collect_step_data(self, brain, agent, world, step):
        """Collect comprehensive data for visualization."""
        step_data = {
            'step': step,
            'timestamp': world.getSimulationTime(),
            
            # Brain metrics
            'brain_metrics': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'developmental_stage': brain.getDevelopmentalStage()
            },
            
            # Agent metrics
            'agent_metrics': {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError(),
                'development_stage': agent.getDevelopmentalStage()
            },
            
            # World state
            'world_metrics': {
                'simulation_time': world.getSimulationTime(),
                'agent_x': world.getAgentBody().x,
                'agent_y': world.getAgentBody().y,
                'agent_energy': world.getAgentBody().energy,
                'agent_age': world.getAgentBody().age,
                'agent_is_moving': world.getAgentBody().isMoving,
                'agent_is_turning': world.getAgentBody().isTurning
            },
            
            # Reward components
            'reward_breakdown': {
                'position_reward': 0.0,  # Would be calculated based on position
                'activity_reward': 0.0,  # Based on neural activity
                'curiosity_reward': 0.0,  # Based on curiosity level
                'total_reward': 0.0
            }
        }
        
        self.data.append(step_data)
        return step_data

# Usage example
data_collector = VisualizationDataCollector()

config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()
agent.initialize(world)

# Collect data for visualization
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    data = data_collector.collect_step_data(brain, agent, world, step)
    
    if step % 50 == 0:
        print(f"Collected data for step {step}")

# Save data for visualization
save_visualization_data(data_collector.data, "simulation_data.json")
print("Data saved for visualization")
```

## Integration Examples

### 1. Complete Agent with All Learning Systems

```python
import pynlm

def run_complete_agent_experiment(config_overrides=None):
    """Run experiment with all learning systems enabled."""
    
    # Create configuration
    config = pynlm.createExplorationConfig()
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run comprehensive simulation
    for step in range(1000):
        # Update environment
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate sophisticated reward
        reward = calculate_advanced_reward(world, brain, agent, step)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.2f}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'config': config
    }

# Usage
results = run_complete_agent_experiment({
    "neuron_count": 1500,
    "region_count": 3,
    "random_seed": 42
})

print("Experiment completed successfully!")
print(f"Final curiosity level: {results['agent'].getCuriosityLevel():.2f}")
print(f"Final novelty level: {results['agent'].getNoveltyLevel():.2f}")
print(f"Total spikes generated: {results['brain'].getTotalSpikeCount()}")
```

### 2. Performance Benchmark

```python
import pynlm
import time

def benchmark_performance(config, steps):
    """Benchmark NLM performance with given configuration."""
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed_time = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / elapsed_time
    neurons_per_second = config.get("neuron_count", 1000) * steps_per_second
    
    return {
        'elapsed_time': elapsed_time,
        'steps_per_second': steps_per_second,
        'neurons_per_second': neurons_per_second,
        'final_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate()
    }

# Benchmark with different configurations
configs = {
    'small': pynlm.createDefaultConfig(),
    'medium': pynlm.createStandardConfig(),
    'large': pynlm.createExplorationConfig()
}

for name, config in configs.items():
    # Adjust neuron count for scaling
    config.set("neuron_count", 1000 if name == 'small' else 
                                 10000 if name == 'medium' else 
                                 100000)
    
    print(f"\nBenchmarking {name} configuration...")
    results = benchmark_performance(config, 100)
    
    print(f"  Steps per second: {results['steps_per_second']:.0f}")
    print(f"  Neurons per second: {results['neurons_per_second']:.0f}")
    print(f"  Final firing rate: {results['final_firing_rate']:.2f}")
```

### 3. Complex Experiment with State Management

```python
import pynlm
import json

def run_advanced_experiment(experiment_config):
    """Run advanced experiment with comprehensive state management."""
    
    # Load or create configuration
    if "config_file" in experiment_config:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(experiment_config["config_file"])
    else:
        config = pynlm.createStandardConfig()
        update_config_with_experiment_settings(config, experiment_config)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=experiment_config.get("world_width", 50),
        height=experiment_config.get("world_height", 50),
        visionWidth=experiment_config.get("vision_width", 20),
        visionHeight=experiment_config.get("vision_height", 20)
    )
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning based on experiment config
    enable_learning_systems(agent, experiment_config)
    
    # Add environmental features
    add_environment_features(world, experiment_config)
    
    # Run experiment with checkpoints
    experiment_results = {
        'steps_completed': 0,
        'rewards_collected': [],
        'learning_metrics': [],
        'checkpoints': []
    }
    
    for step in range(experiment_config.get("total_steps", 1000)):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and compute
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        reward = calculate_experiment_reward(
            world, brain, agent, step, experiment_config
        )
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        record_experiment_metrics(
            experiment_results, step, brain, agent, world, reward
        )
        
        # Save checkpoint periodically
        if step % 100 == 0:
            save_experiment_checkpoint(
                brain, config, f"checkpoint_step_{step}.bin"
            )
            experiment_results['checkpoints'].append(step)
        
        # Log progress
        if step % 100 == 0:
            print(f"Experiment step {step}/{experiment_config.get('total_steps', 1000)}")
    
    # Save final results
    save_experiment_results(experiment_results, "final_results.json")
    
    # Save final brain state
    brain.save("final_brain.bin")
    
    return experiment_results

# Helper functions

def enable_learning_systems(agent, config):
    """Enable learning systems based on configuration."""
    if config.get("enable_reward_learning", True):
        agent.enableRewardModulation(True)
    if config.get("enable_structural_plasticity", False):
        agent.enableStructuralPlasticity(True)
    if config.get("enable_development", True):
        agent.enableDevelopment(True)
    if config.get("enable_curiosity", True):
        agent.enableCuriosity(True)

def add_environment_features(world, config):
    """Add environmental features for experiments."""
    # Add resources
    world.addResource(10.0, 10.0, 10.0)
    world.addResource(40.0, 40.0, 15.0)
    world.addResource(20.0, 30.0, 5.0)
    
    # Set energy parameters
    if "max_energy" in config:
        world.setMaxEnergy(config["max_energy"])

def calculate_experiment_reward(world, brain, agent, step, config):
    """Calculate reward for experiment."""
    body = world.getAgentBody()
    x, y = body.x, body.y
    width, height = world.getWidth(), world.getHeight()
    
    # Base reward for being in bounds
    position_reward = 1.0 if (0 <= x < width and 0 <= y < height) else -1.0
    
    # Additional rewards based on experiment type
    if "reward_resources" in config:
        # Check if agent is near resources (simplified)
        position_reward += 2.0 if (15 <= x <= 25 and 15 <= y <= 25) else 0.0
    
    if "reward_activity" in config:
        # Reward for neural activity
        position_reward += brain.getFiringNeuronCount() * 0.001
    
    if "reward_curiosity" in config:
        # Reward for exploration
        position_reward += agent.getCuriosityLevel() * 0.1
    
    return position_reward

def record_experiment_metrics(results, step, brain, agent, world, reward):
    """Record experiment metrics."""
    results['steps_completed'] = step + 1
    results['rewards_collected'].append(reward)
    
    results['learning_metrics'].append({
        'step': step,
        'reward': reward,
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'neuromodulation': agent.getNeuromodulationLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'agent_x': world.getAgentBody().x,
        'agent_y': world.getAgentBody().y,
        'agent_energy': world.getAgentBody().energy
    })

def save_experiment_checkpoint(brain, config, filename):
    """Save experiment checkpoint."""
    brain.save(filename)
    config.saveToFile(filename.replace('.bin', '.json'))

def save_experiment_results(results, filename):
    """Save experiment results."""
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)

# Usage example
experiment_config = {
    "world_width": 50,
    "world_height": 50,
    "vision_width": 20,
    "vision_height": 20,
    "total_steps": 1000,
    "enable_reward_learning": True,
    "enable_structural_plasticity": False,
    "enable_development": True,
    "enable_curiosity": True,
    "reward_resources": True,
    "reward_activity": True,
    "reward_curiosity": True
}

print("Starting advanced experiment...")
experimental_results = run_advanced_experiment(experiment_config)
print("Experiment completed successfully!")
print(f"Total steps: {experimental_results['steps_completed']}")
print(f"Final checkpoint at step: {experimental_results['checkpoints'][-1] if experimental_results['checkpoints'] else 'N/A'}")
```

## Best Practices

### 1. Configuration Management

```python
# Use named configuration functions for different use cases
config = pynlm.createDefaultConfig()      # Quick prototyping
config = pynlm.createStandardConfig()    # Standard experiments  
config = pynlm.createExplorationConfig() # Learning and exploration

# Always validate configuration before use
assert config.has("neuron_count"), "neuron_count not set"
assert config.has("connection_probability"), "connection_probability not set"
```

### 2. Error Handling

```python
import pynlm

# Always handle potential errors
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
        
except RuntimeError as e:
    print(f"Simulation error: {e}")
    # Attempt recovery or graceful shutdown
    
except KeyboardInterrupt:
    print("\nSimulation interrupted by user")
    # Save current state if needed
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log error and exit
```

### 3. Performance Considerations

```python
import pynlm
import time

# Profile your simulation
brain = pynlm.createBrain(pynlm.createStandardConfig())
brain.initialize()

# Measure performance
start_time = time.time()
for step in range(10000):
    brain.step(step)
end_time = time.time()

print(f"Simulated 10000 steps in {end_time-start_time:.2f} seconds")
print(f"Performance: {(10000/(end_time-start_time)):.0f} steps/second")
```

### 4. Reproducibility

```python
import pynlm

# For reproducible experiments
def run_reproducible_experiment(seed=42):
    config = pynlm.createStandardConfig()
    config.set("random_seed", seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
    
    return brain

# Run with same seed for reproducible results
brain1 = run_reproducible_experiment(42)
brain2 = run_reproducible_experiment(42)

# Compare results (should be identical)
print("Brains are identical:", brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount())
```

### 5. Memory Management

```python
import pynlm
import gc

# Monitor memory usage
brain = pynlm.createBrain(pynlm.createStandardConfig())
brain.initialize()

# Simulate and check memory
for step in range(1000):
    brain.step(step)
    
    # Force garbage collection periodically
    if step % 100 == 0:
        gc.collect()
        print(f"Step {step}: Memory check")

# Clear references when done
del brain
gc.collect()
print("Memory cleaned up")
```

## Advanced Topics

### 1. Multi-Experiment Framework

```python
import pynlm

class NLMExperimentFramework:
    def __init__(self):
        self.experiments = []
        self.global_config = pynlm.createStandardConfig()
        
    def add_experiment(self, name, config_overrides, steps):
        """Add experiment to framework."""
        experiment = {
            'name': name,
            'config': self.create_experiment_config(config_overrides),
            'steps': steps,
            'results': None
        }
        self.experiments.append(experiment)
        
    def run_experiment(self, experiment):
        """Run single experiment."""
        brain = pynlm.createBrain(experiment['config'])
        brain.initialize()
        
        for step in range(experiment['steps']):
            brain.step(step)
            
        return brain
        
    def run_all_experiments(self):
        """Run all experiments."""
        for experiment in self.experiments:
            print(f"Running {experiment['name']}...")
            experiment['results'] = self.run_experiment(experiment)
            print(f"Completed {experiment['name']}")
            
        return self.experiments

# Usage
framework = NLMExperimentFramework()
framework.add_experiment("Small Brain", {"neuron_count": 500}, 1000)
framework.add_experiment("Medium Brain", {"neuron_count": 5000}, 1000)
framework.add_experiment("Large Brain", {"neuron_count": 50000}, 100)

results = framework.run_all_experiments()
```

### 2. Data Analysis and Visualization

```python
import pynlm
import json
import matplotlib.pyplot as plt

class NLMDataAnalyzer:
    def __init__(self):
        self.step_data = []
        
    def collect_data(self, brain, agent, world, step):
        """Collect comprehensive data for analysis."""
        data_point = {
            'step': step,
            'timestamp': world.getSimulationTime(),
            'brain_metrics': {
                'neurons': brain.getTotalNeuronCount(),
                'firing': brain.getFiringNeuronCount(),
                'rate': brain.getAverageFiringRate(),
                'spikes': brain.getTotalSpikeCount(),
                'development': brain.getDevelopmentalStage()
            },
            'agent_metrics': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromod': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError()
            },
            'world_state': {
                'x': world.getAgentBody().x,
                'y': world.getAgentBody().y,
                'energy': world.getAgentBody().energy,
                'age': world.getAgentBody().age
            }
        }
        self.step_data.append(data_point)
        
    def analyze_development(self):
        """Analyze development patterns."""
        stages = {}
        for data in self.step_data:
            stage = data['brain_metrics']['development']
            if stage not in stages:
                stages[stage] = []
            stages[stage].append(data)
            
        return {stage: len(data_points) for stage, data_points in stages.items()}
        
    def analyze_learning(self):
        """Analyze learning patterns."""
        curiosity_trend = [d['agent_metrics']['curiosity'] for d in self.step_data]
        novelty_trend = [d['agent_metrics']['novelty'] for d in self.step_data]
        
        return {
            'final_curiosity': curiosity_trend[-1],
            'final_novelty': novelty_trend[-1],
            'curiosity_change': curiosity_trend[-1] - curiosity_trend[0],
            'novelty_change': novelty_trend[-1] - novelty_trend[0]
        }

# Usage
analyzer = NLMDataAnalyzer()
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()
agent.initialize(world)

# Collect and analyze data
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    analyzer.collect_data(brain, agent, world, step)

# Analyze results
development_analysis = analyzer.analyze_development()
learning_analysis = analyzer.analyze_learning()

print("Development Analysis:", development_analysis)
print("Learning Analysis:", learning_analysis)

# Plot results (requires matplotlib)
try:
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    steps = [d['step'] for d in analyzer.step_data]
    firing_rates = [d['brain_metrics']['rate'] for d in analyzer.step_data]
    plt.plot(steps, firing_rates)
    plt.title('Firing Rate Over Time')
    plt.xlabel('Step')
    plt.ylabel('Firing Rate')
    
    plt.subplot(2, 2, 2)
    curiosity = [d['agent_metrics']['curiosity'] for d in analyzer.step_data]
    novelty = [d['agent_metrics']['novelty'] for d in analyzer.step_data]
    plt.plot(steps, curiosity, label='Curiosity')
    plt.plot(steps, novelty, label='Novelty')
    plt.title('Neuromodulation Levels')
    plt.xlabel('Step')
    plt.ylabel('Level')
    plt.legend()
    
    plt.subplot(2, 2, 3)
    rewards = [d.get('reward', 0) for d in analyzer.step_data]
    plt.plot(steps, rewards)
    plt.title('Rewards Over Time')
    plt.xlabel('Step')
    plt.ylabel('Reward')
    
    plt.subplot(2, 2, 4)
    agent_positions = [(d['world_state']['x'], d['world_state']['y']) for d in analyzer.step_data]
    if agent_positions:
        xs, ys = zip(*agent_positions)
        plt.scatter(xs, ys, c=steps, cmap='viridis', alpha=0.7)
        plt.title('Agent Trajectory')
        plt.xlabel('X Position')
        plt.ylabel('Y Position')
        plt.colorbar(label='Step')
    
    plt.tight_layout()
    plt.savefig('nlm_analysis.png')
    print("Analysis plot saved to nlm_analysis.png")
    
except ImportError:
    print("matplotlib not available, skipping plot generation")
```

### 3. Advanced Configuration Strategies

```python
import pynlm
import json

class ConfigurationManager:
    def __init__(self):
        self.config_templates = {}
        self.load_default_templates()
        
    def load_default_templates(self):
        """Load default configuration templates."""
        self.config_templates = {
            'quick_test': {
                "neuron_count": 100,
                "region_count": 1,
                "connection_probability": 0.1,
                "random_seed": 42
            },
            'standard_experiment': {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.15,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "random_seed": 12345
            },
            'exploration_study': {
                "neuron_count": 500,
                "region_count": 3,
                "connection_probability": 0.2,
                "stdp_ltp_weight": 0.005,
                "stdp_ltd_weight": 0.006,
                "synaptogenesis_rate": 0.0005,
                "random_seed": 999
            },
            'learning_optimization': {
                "neuron_count": 2000,
                "region_count": 4,
                "connection_probability": 0.08,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.022,
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001,
                "random_seed": 777
            }
        }
        
    def create_config(self, template_name, custom_overrides=None):
        """Create configuration from template."""
        if template_name not in self.config_templates:
            raise ValueError(f"Unknown template: {template_name}")
            
        base_config = self.config_templates[template_name].copy()
        
        if custom_overrides:
            base_config.update(custom_overrides)
            
        # Create and return pynlm configuration
        pynlm_config = pynlm.createDefaultConfig()
        for key, value in base_config.items():
            pynlm_config.set(key, value)
            
        return pynlm_config
        
    def save_template(self, name, config):
        """Save configuration as template."""
        template_data = {}
        keys = config.getKeys()
        for key in keys:
            template_data[key] = config.get(key)
            
        self.config_templates[name] = template_data
        
    def export_templates(self, filename):
        """Export templates to file."""
        with open(filename, 'w') as f:
            json.dump(self.config_templates, f, indent=2)
            
    def import_templates(self, filename):
        """Import templates from file."""
        with open(filename, 'r') as f:
            imported = json.load(f)
            self.config_templates.update(imported)

# Usage
config_manager = ConfigurationManager()

# Use built-in templates
quick_config = config_manager.create_config('quick_test')
standard_config = config_manager.create_config('standard_experiment')
exploration_config = config_manager.create_config('exploration_study')

# Create custom configuration with template overrides
custom_config = config_manager.create_config(
    'standard_experiment',
    {"neuron_count": 2000, "random_seed": 12345}
)

# Save custom configuration as new template
config_manager.save_template('my_experiment', custom_config)

# Export all templates to file
config_manager.export_templates("all_templates.json")
```

## Summary

This intermediate guide covered:

### Core Topics:
1. **Configuration Management** - Different config templates and parameter tuning
2. **Advanced Brain Architecture** - Multi-region brains and complex configurations
3. **Learning System Integration** - Neuromodulation, curiosity, development
4. **Sophisticated World Interaction** - Environmental features and advanced reward systems
5. **Performance Monitoring** - Comprehensive data collection and analysis
6. **State Management** - Save/load functionality and checkpoints
7. **Configuration File I/O** - JSON configuration management

### Framework Examples:
1. **Experiment Framework** - Structured experiment design and execution
2. **Performance Benchmark** - Measuring and comparing performance
3. **Advanced Experiment** - Complex experiments with state management
4. **Complete Agent** - Full integration of all learning systems

### Best Practices:
1. **Configuration Management** - Use templates and validate settings
2. **Error Handling** - Comprehensive exception handling and recovery
3. **Performance Considerations** - Profiling and optimization
4. **Reproducibility** - Seed management and result comparison
5. **Memory Management** - Proper cleanup and garbage collection

### Advanced Techniques:
1. **Multi-Experiment Framework** - Running multiple experiments efficiently
2. **Data Analysis and Visualization** - Comprehensive analysis tools
3. **Advanced Configuration Strategies** - Template management and customization

This guide builds on the beginner guide and provides the foundation for more advanced NLM research and development. The concepts and techniques covered here enable users to create sophisticated neural simulations for research, education, and application development.

For even more advanced usage:
- Read the complete API reference in `docs/REFERENCE.md`
- Explore the example scripts in `nlm_examples.py`
- Check the CLI tool in `nlm_cli.py`
- Review the documentation in the `docs/` directory

You're now well-equipped to work with NLM at an intermediate level. These skills will serve you well in more advanced research and development projects using the NLM framework.
