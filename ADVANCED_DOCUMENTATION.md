# Advanced NLM Documentation

## Overview

This document provides comprehensive documentation for the advanced features and usage patterns of the Neural Learning Machine (NLM) Python API.

## Advanced Python API

### 1. Enhanced Configuration Management

#### Configuration Presets

NLM provides several predefined configuration presets for different use cases:

```python
import pynlm

# Expert preset for maximum capability
expert_config = pynlm.createConfigFromDict({
    'brain.neuron_count': 5000,
    'brain.synapse_density': 0.15,
    'plasticity.stdp.enable': True,
    'plasticity.hebbian.enable': True,
    'plasticity.structural.enable': True,
    'neuromod.dopamine.scale': 1.5,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True
})

# Research preset for scientific investigation
research_config = pynlm.createConfigFromDict({
    'brain.neuron_count': 10000,
    'brain.synapse_density': 0.2,
    'plasticity.stdp.enable': True,
    'plasticity.stdp.learning_rate': 0.01,
    'plasticity.hebbian.enable': True,
    'plasticity.structural.enable': True,
    'neuromod.dopamine.scale': 2.0,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True,
    'prediction.enable': True,
    'cognition.enable': True
})

# Educational preset for learning and teaching
educational_config = pynlm.createConfigFromDict({
    'brain.neuron_count': 1000,
    'brain.synapse_density': 0.05,
    'plasticity.stdp.enable': True,
    'plasticity.hebbian.enable': True,
    'neuromod.dopamine.scale': 1.0,
    'neuromod.curiosity.enable': True
})
```

#### Configuration File Management

```python
import pynlm

# Save configuration to JSON
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 5000)
config.set('brain.synapse_density', 0.15)
config.set('plasticity.stdp.enable', True)

# Save as JSON file
pynlm.saveConfigAsJson(config, 'my_config.json')

# Load configuration from JSON file
loaded_config = pynlm.loadConfigFromJson('my_config.json')
brain = pynlm.createBrain(loaded_config)
```

### 2. Advanced Brain Management

#### Brain Statistics and Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive brain statistics
stats = pynlm.getBrainStats(brain)

print(f"Total neurons: {stats['total_neuron_count']}")
print(f"Total synapses: {stats['total_synapse_count']}")
print(f"Active neurons: {stats['active_neuron_count']}")
print(f"Firing neurons: {stats['firing_neuron_count']}")
print(f"Total spikes: {stats['total_spike_count']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Development stage: {stats['developmental_stage']}")

# Monitor memory systems
if 'working_memory_capacity' in stats:
    print(f"Working memory: {stats['working_memory_usage']}/{stats['working_memory_capacity']}")

if 'episodic_memory_episodes' in stats:
    print(f"Episodic memory episodes: {stats['episodic_memory_episodes']}")
```

#### Benchmarking

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Benchmark simulation performance
benchmark_results = pynlm.benchmarkBrainStep(brain, 1000, 1.0)

print(f"Benchmark Results:")
print(f"  Total steps: {benchmark_results['total_steps']}")
print(f"  Elapsed time: {benchmark_results['elapsed_time_microseconds'] / 1000000:.2f} seconds")
print(f"  Steps per second: {benchmark_results['steps_per_second']:.0f}")
print(f"  Average step time: {benchmark_results['average_step_time_microseconds']:.2f} μs")
```

### 3. Context Management

#### Controlled Brain Simulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use context manager for controlled simulation
with pynlm.brainSimulationContext(brain, 100, 1.0) as sim:
    print(f"Simulation started with {sim['total_neuron_count']} neurons")
    
    # Access state information during simulation
    if 'total_spike_count' in sim:
        print(f"Current spikes: {sim['total_spike_count']}")
    
    # Simulation running...
    # ...
    
    # Access state again during simulation
    if 'firing_neuron_count' in sim:
        print(f"Current firing neurons: {sim['firing_neuron_count']}")

print("Simulation completed")
```

### 4. NumPy Integration

#### Sensory Data Import

```python
import pynlm
import numpy as np

# Create NumPy arrays for sensory input
vision_data = np.random.rand(8, 8, 3).astype(np.float32)  # 8x8 RGB image
audio_data = np.random.rand(16000).astype(np.float32)  # 1-second audio
internal_data = np.array([0.1, 0.5, 0.3, -0.2], dtype=np.float32)  # Internal signals

# Import sensory data
vision_sensory = pynlm.importSensoryData(vision_data, "vision")
audio_sensory = pynlm.importSensoryData(audio_data, "audio")
internal_sensory = pynlm.importSensoryData(internal_data, "internal")

print(f"Vision data shape: {vision_sensory.getDimensions()}")
print(f"Audio data sample rate: {audio_sensory.getSampleRate()}")
print(f"Internal signals count: {internal_sensory.getData().size()}")
```

#### Neural Activity Export

```python
import pynlm
import numpy as np

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for i in range(100):
    brain.step(i)

# Export neural activity
activity_data = pynlm.exportNeuralActivity(brain, "numpy")

print(f"Exported activity: {activity_data}")
print(f"Neuron count in exported data: {activity_data['neuron_count']}")
```

### 5. Learning Experiments

#### Advanced Learning Experiment

```python
import pynlm
import numpy as np

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Experiment configuration
experiment_config = {
    'num_steps': 1000,
    'enable_logging': True,
    'save_checkpoints': True,
    'curiosity_threshold': 0.3,
    'reward_gamma': 0.9,
    'learning_rate': 0.01
}

# Run advanced learning experiment
results = pynlm.runLearningExperiment(brain, world, agent, experiment_config)

print(f"Experiment completed in {results['steps_completed']} steps")
print(f"Final performance: {results['final_performance']:.3f}")
print(f"Learning rate achieved: {results['learning_rate']:.3f}")
```

#### Learning Trajectory Analysis

```python
import pynlm
import matplotlib.pyplot as plt

# Get learning trajectory from experiment results
# (Assuming results contains trajectory data)
learning_data = {
    'steps': list(range(100)),
    'performance': np.random.rand(100).cumsum() / 100,
    'curiosity': np.random.rand(100),
    'novelty': np.random.rand(100),
    'prediction_error': np.random.rand(100)
}

# Analyze learning trajectory
analysis = pynlm.analyzeLearningTrajectory(learning_data)

print("Learning Analysis:")
for key, value in analysis.items():
    print(f"  {key}: {value:.3f}")

# Create visualization
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(learning_data['steps'], learning_data['performance'])
plt.title('Learning Performance')
plt.xlabel('Step')
plt.ylabel('Performance')

plt.subplot(2, 2, 2)
plt.plot(learning_data['steps'], learning_data['curiosity'])
plt.title('Curiosity Level')
plt.xlabel('Step')
plt.ylabel('Curiosity')

plt.subplot(2, 2, 3)
plt.plot(learning_data['steps'], learning_data['novelty'])
plt.title('Novelty Detection')
plt.xlabel('Step')
plt.ylabel('Novelty')

plt.subplot(2, 2, 4)
plt.plot(learning_data['steps'], learning_data['prediction_error'])
plt.title('Prediction Error')
plt.xlabel('Step')
plt.ylabel('Error')

plt.tight_layout()
plt.show()
```

### 6. Model Management

#### Brain Model Export/Import

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Export brain model as JSON
pynlm.exportBrainModel(brain, 'json', 'my_brain_model.json')

# Import brain model
imported_brain = pynlm.importBrainModel('json', 'my_brain_model.json')
```

#### Model Formats

NLM supports multiple model export formats:

- **JSON**: Human-readable, structured format
- **Binary**: Compact, efficient format
- **HDF5**: Scientific data storage
- **Custom**: Plugin-based format support

### 7. Visualization

#### Visualization Data Creation

```python
import pynlm
import pynlm.advanced_python_api as api

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Run simulation
for i in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()

# Create visualization data
viz_data = pynlm.createVisualizationData(brain, world, agent)

print("Visualization data created:")
print(f"  Neural activity: {viz_data['neural_activity']}")
print(f"  World state: {viz_data['world_state']}")
print(f"  Agent behavior: {viz_data['agent_behavior']}")
```

### 8. Development Pipeline

#### Development Pipeline Creation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Define development stages
stages = [
    {'name': 'initial', 'duration': 10, 'plasticity': 1.0},
    {'name': 'critical_period', 'duration': 20, 'plasticity': 0.8},
    {'name': 'maturation', 'duration': 30, 'plasticity': 0.5},
    {'name': 'adult', 'duration': float('inf'), 'plasticity': 0.2}
]

# Create development pipeline
pynlm.createDevelopmentPipeline(brain, stages)

# Get detailed development report
report = pynlm.getDevelopmentReport(brain, include_details=True)

print("Development Report:")
for key, value in report.items():
    print(f"  {key}: {value}")
```

### 9. Research Environment

#### Research Environment Setup

```python
import pynlm

# Create specialized research environment
research_env = pynlm.createResearchEnvironment()

# Configure environment for research
research_env.configure({
    'world': {
        'type': 'multi_agent',
        'agents': 5,
        'resources': 10,
        'hazards': 2
    },
    'brain': {
        'type': 'hybrid',
        'plasticity': 'full',
        'neuromodulation': 'complete'
    },
    'experiment': {
        'type': 'social_learning',
        'duration': 1000,
        'record_video': True,
        'save_data': True
    }
})

# Run research protocol
protocol = "social_learning_experiment"
parameters = {
    'learning_rate': 0.01,
    'discount_factor': 0.9,
    'exploration_rate': 0.1,
    'population_size': 100,
    'generations': 50
}

results = pynlm.runResearchProtocol(research_env, protocol, parameters)

print(f"Research protocol completed:")
print(f"  Best fitness: {results['best_fitness']:.3f}")
print(f"  Average fitness: {results['average_fitness']:.3f}")
print(f"  Convergence generation: {results['convergence_generation']}")
```

### 10. Advanced Factory Functions

#### Specialized Brain Creation

```python
import pynlm

# Create expert brain (maximum capability)
expert_brain = pynlm.createExpertBrain({
    'brain.neuron_count': 10000,
    'plasticity.stdp.enable': True,
    'neuromod.dopamine.scale': 2.0
})

# Create research brain (optimized for scientific investigation)
research_brain = pynlm.createResearchBrain({
    'brain.neuron_count': 20000,
    'prediction.enable': True,
    'cognition.enable': True,
    'development.enable': True
})

# Create educational brain (appropriate difficulty level)
educational_brain = pynlm.createEducationalBrain(difficulty_level=3)

print(f"Created expert brain with {expert_brain.getTotalNeuronCount()} neurons")
print(f"Created research brain with {research_brain.getTotalNeuronCount()} neurons")
print(f"Created educational brain with {educational_brain.getTotalNeuronCount()} neurons")
```

### 11. Validation and Testing

#### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 1000)
config.set('brain.synapse_density', 0.1)

# Validate configuration
validation_results = pynlm.validateBrainConfiguration(config)

print("Configuration Validation Results:")
print(f"  Valid: {validation_results['valid']}")
if 'errors' in validation_results:
    print(f"  Errors: {validation_results['errors']}")
if 'warnings' in validation_results:
    print(f"  Warnings: {validation_results['warnings']}")
```

#### Simulation Profiling

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Profile simulation performance
profile_results = pynlm.profileBrainSimulation(brain, 1000, 1.0)

print("Simulation Profile Results:")
print(f"  Execution time: {profile_results['execution_time']:.2f} seconds")
print(f"  Memory usage: {profile_results['memory_usage']:.2f} MB")
print(f"  Steps per second: {profile_results['steps_per_second']:.0f}")
print(f"  Peak memory: {profile_results['peak_memory']:.2f} MB")
```

## Advanced Usage Patterns

### 1. Multi-Agent Environment

```python
import pynlm

# Create world with multiple agents
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)

# Create multiple agents with different brains
agents = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set('brain.neuron_count', 2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agents.append(agent)

# Run multi-agent simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input for each agent
    percepts = [world.getSensoryPercept() for _ in agents]
    
    # Process sensory input
    for agent, percept in zip(agents, percepts):
        agent.processSensoryInput(percept)
    
    # Simulate brain steps
    for agent in agents:
        brain = agent.getBrain()
        brain.step(step)
    
    # Decode motor commands
    actions = [agent.decodeMotorCommand() for agent in agents]
    
    # Apply actions to world
    for action in actions:
        world.applyMotorCommand(action, world.getSimulationTime())
```

### 2. Learning with Curriculum

```python
import pynlm

# Create learning curriculum
class LearningCurriculum:
    def __init__(self):
        self.stages = [
            {'difficulty': 1, 'steps': 100, 'reward_threshold': 0.1},
            {'difficulty': 2, 'steps': 200, 'reward_threshold': 0.3},
            {'difficulty': 3, 'steps': 300, 'reward_threshold': 0.5},
            {'difficulty': 4, 'steps': 400, 'reward_threshold': 0.7},
            {'difficulty': 5, 'steps': 500, 'reward_threshold': 0.9}
        ]
        self.current_stage = 0
    
    def get_current_config(self):
        config = pynlm.createDefaultConfig()
        stage = self.stages[self.current_stage]
        
        # Scale neuron count based on difficulty
        config.set('brain.neuron_count', 500 * (stage['difficulty'] + 1))
        config.set('brain.synapse_density', 0.05 * stage['difficulty'])
        
        return config
    
    def is_stage_complete(self, brain):
        # Check if current stage's reward threshold is met
        # (Implementation depends on specific metrics)
        return True

# Create curriculum and run learning
curriculum = LearningCurriculum()
brain = pynlm.createBrain(curriculum.get_current_config())
brain.initialize()

for stage in curriculum.stages:
    print(f"Running stage {stage['difficulty']}...")
    
    for step in range(stage['steps']):
        brain.step(step)
        
        # Check for stage completion
        if curriculum.is_stage_complete(brain):
            curriculum.current_stage += 1
            if curriculum.current_stage < len(curriculum.stages):
                brain.reset()
                brain = pynlm.createBrain(curriculum.get_current_config())
                brain.initialize()
                break
```

### 3. Transfer Learning

```python
import pynlm

# Create source brain (pre-trained)
source_config = pynlm.createDefaultConfig()
source_config.set('brain.neuron_count', 5000)
source_config.set('plasticity.stdp.enable', True)

source_brain = pynlm.createBrain(source_config)
source_brain.initialize()

# Train source brain on source task
for i in range(1000):
    source_brain.step(i)

# Save source brain model
source_brain.save('source_brain.model')

# Create target brain (for new task)
target_config = pynlm.createDefaultConfig()
target_config.set('brain.neuron_count', 3000)  # Smaller for target
target_config.set('plasticity.stdp.enable', True)
target_config.set('plasticity.stdp.learning_rate', 0.01)  # Different learning rate

target_brain = pynlm.createBrain(target_config)
target_brain.initialize()

# Import source weights as initialization
target_brain.load('source_brain.model')

# Fine-tune target brain on target task
for i in range(500):
    target_brain.step(i)

# Continue with target-only training (unfreeze source if needed)
for i in range(1000):
    target_brain.step(i)
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

1. **Configuration Errors**
   ```python
   try:
       config = pynlm.createConfigFromDict({'invalid.key': 123})
   except pynlm.ConfigError as e:
       print(f"Configuration error: {e}")
   ```

2. **Brain Simulation Errors**
   ```python
   try:
       brain = pynlm.createBrain(config)
       brain.initialize()
       for i in range(100):
           brain.step(i)
   except pynlm.BrainError as e:
       print(f"Brain simulation error: {e}")
   ```

3. **World Simulation Errors**
   ```python
   try:
       world = pynlm.createSimpleWorld()
       world.configure(width=20, height=20)
       world.update(0.1)
   except pynlm.WorldError as e:
       print(f"World simulation error: {e}")
   ```

## Performance Optimization

### Tips for Better Performance

1. **Use Appropriate Configuration Presets**
   ```python
   # For maximum performance
   brain = pynlm.createExpertBrain({'brain.neuron_count': 5000})
   
   # For research (higher accuracy)
   brain = pynlm.createResearchBrain({'brain.neuron_count': 10000})
   
   # For quick testing
   brain = pynlm.createEducationalBrain(difficulty_level=1)
   ```

2. **Batch Processing**
   ```python
   # Process multiple steps in batch for better performance
   for batch_start in range(0, total_steps, batch_size):
       batch_end = min(batch_start + batch_size, total_steps)
       for step in range(batch_start, batch_end):
           brain.step(step)
   ```

3. **Memory Management**
   ```python
   # Clear unused memory
   import gc
   gc.collect()
   
   # Use context managers to ensure cleanup
   with pynlm.brainSimulationContext(brain, 100) as sim:
       # Simulation code here
       pass
   ```

## Integration with Other Systems

### Integration with Machine Learning Libraries

```python
import pynlm
import numpy as np
import torch

# Create NLM brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create PyTorch model for policy learning
class NLMPolicy(torch.nn.Module):
    def __init__(self, brain):
        super().__init__()
        self.brain = brain
        # Extract weights from brain neurons
        self.neural_weights = torch.nn.Parameter(
            torch.tensor([n.getMembranePotential() for n in brain.getNeurons()],
                        dtype=torch.float32)
        )
    
    def forward(self, state):
        # Convert NLM state to PyTorch
        policy = self.brain.produceAction()
        return policy

# Create hybrid model
hybrid_model = NLMPolicy(brain)
optimizer = torch.optim.Adam(hybrid_model.parameters(), lr=0.001)

# Training loop
for epoch in range(100):
    for step in range(50):
        brain.step(step)
        action = hybrid_model(None)  # Forward pass
        reward = calculate_reward(action)  # Your reward function
        
        optimizer.zero_grad()
        loss = -torch.log(action + 1e-8) * reward
        loss.backward()
        optimizer.step()
```

## Conclusion

The NLM Python API provides powerful tools for advanced brain simulation and learning. By leveraging these advanced features, you can create sophisticated neural systems for research, education, and practical applications.

Key advantages of the advanced API:

1. **Enhanced Configuration Management**: Pre-defined presets and file-based configuration
2. **Comprehensive Brain Monitoring**: Detailed statistics and real-time monitoring
3. **Advanced Simulation Control**: Context managers and batch processing
4. **NumPy Integration**: Seamless integration with scientific Python ecosystem
5. **Learning Experiment Framework**: Built-in support for advanced learning experiments
6. **Model Management**: Export/import capabilities for different formats
7. **Development Tools**: Development pipeline and detailed reporting
8. **Research Environment**: Specialized environment for scientific research
9. **Validation and Testing**: Comprehensive validation and profiling tools
10. **Integration Support**: Easy integration with other Python libraries

These features make NLM a powerful tool for neural simulation and learning research.