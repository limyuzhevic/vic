# Python Interface Improvements for NLM

# Enhanced NLM Python Interface

This document provides an improved, more comprehensive Python interface for the NLM (熙然) brain simulation project. The interface has been enhanced to provide better usability, more advanced features, and comprehensive documentation.

## Overview

The NLM Python interface (`pynlm`) provides Python bindings to the C++ NLM brain simulation system, enabling users to create, simulate, and interact with artificial brains without needing to understand the underlying C++ implementation.

## Installation

### Prerequisites

```bash
pip install pynlm
```

Or for development (with C++ source):

```bash
# Clone the repository
# Build and install
pip install -e .
```

## Enhanced Core Classes

### Brain Class - Enhanced Interface

The Brain class has been enhanced with additional methods for better control and monitoring:

```python
import pynlm
import time

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set('neuron_count', 2000)      # More neurons
config.set('region_count', 2)         # Multiple regions
config.set('simulation_timestep', 0.0005)  # Smaller timestep
config.set('connection_probability', 0.12)

brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced statistics and monitoring
print("=== Brain Statistics ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Advanced control methods
brain.enableDevelopment(True)           # Enable brain maturation
brain.enableCuriosity(True)             # Enable exploration
brain.enableRewardModulation(True)      # Enable learning
brain.enableStructuralPlasticity(True)  # Enable neural growth

# Get detailed system information
if brain.getWorkingMemory():
    print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")

if brain.getDopamine():
    print(f"Current dopamine level: {brain.getDopamine().getLevel():.3f}")
```

### Enhanced World Interface

The World class now supports more sophisticated environment interactions:

```python
import pynlm

# Create a more detailed world
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=15,
    visionHeight=15,
    audioChannels=8
)

# Enable sensory features
world.setLightLevel(0.7)
world.setTemperature(37.0)
world.addObstacle(25, 25)  # Add an obstacle at center

# Advanced sensory access
def advanced_perception(percept):
    # Enhanced perception processing
    visual_features = percept.getVisualFeatures()
    audio_features = percept.getAudioFeatures()
    internal_state = percept.getInternalState()
    
    # Process features for more complex behavior
    brightness = sum(visual_features) / len(visual_features)
    sound_energy = sum(audio_features)
    
    return {
        'brightness': brightness,
        'sound_energy': sound_energy,
        'complexity': len(visual_features) + len(audio_features)
    }

# Configure world with advanced processing
world.setPerceptionCallback(advanced_perception)
```

### Enhanced Agent Interface

The AgentBrain class now provides more sophisticated control:

```python
import pynlm

# Create a complete agent setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning capabilities
agent.enableRewardModulation(True)      # Reinforcement learning
agent.enableCuriosity(True)             # Intrinsic motivation
agent.enableStructuralPlasticity(True)  # Neural development
agent.enableDevelopment(True)           # Brain maturation
agent.enableAttention(True)              # Selective attention

# Advanced action selection strategies
def custom_action_selector(agent, world, percept):
    # Implement custom behavior strategies
    action = agent.decodeMotorCommand()
    
    # Modify action based on context
    if action.action_type == ActionType.MoveForward and percept.hasDanger():
        return pynlm.Action(ActionType.Wait)  # Avoid danger
    
    return action

agent.setActionSelector(custom_action_selector)
```

## Advanced Simulation Features

### Pattern Learning and Memory

```python
import pynlm

# Create a brain with pattern learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable advanced memory systems
working_memory = brain.getWorkingMemory()
working_memory.setCapacity(500)  # Larger working memory

episodic_memory = brain.getEpisodicMemory()
episodic_memory.setMaxEpisodes(5000)  # Store more experiences

# Pattern training function
def train_pattern_sequence(brain, steps=1000):
    pattern_strength = 0.0
    
    for step in range(steps):
        # Generate input pattern
        pattern = [0.5, 0.8, 0.3, 0.9, 0.2]  # Example pattern
        
        # Process pattern
        percept = pynlm.SensoryInput(pattern)
        brain.receiveSensoryInput(percept)
        
        # Learn pattern
        brain.step(step)
        
        # Store in working memory
        if working_memory:
            for neuron_id in range(min(10, brain.getTotalNeuronCount())):
                working_memory.storeToNeuron(neuron_id, pattern[step % len(pattern)])
    
    return pattern_strength

# Train and evaluate
strength = train_pattern_sequence(brain, 2000)
print(f"Pattern learning strength: {strength:.3f}")
```

### Neuromodulation Control

```python
import pynlm
import time

# Create brain with neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Advanced neuromodulation control
def neuromodulation_controller(step, reward=None, prediction_error=None):
    # Calculate neuromodulatory signals
    dopamine_level = 0.5  # Base level
    curiosity_level = 0.0
    novelty_level = 0.0
    
    # Adjust based on experience
    if reward is not None:
        dopamine_level += reward * 0.1
    
    if prediction_error is not None:
        dopamine_level += abs(prediction_error) * 0.05
        novelty_level += abs(prediction_error) * 0.02
    
    # Apply neuromodulation
    if dopamine:
        dopamine.setLevel(max(0.0, min(1.0, dopamine_level)))
    
    if novelty:
        novelty.setLevel(max(0.0, min(1.0, novelty_level)))
    
    # Curiosity drives exploration
    curiosity.setLevel(0.1 * (1.0 - dopamine_level if dopamine else 1.0))

# Apply neuromodulation during simulation
for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Process
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
    
    # Calculate reward and prediction error
    reward = calculate_reward(world, action)
    prediction_error = calculate_prediction_error(brain, percept)
    
    # Update neuromodulation
    neuromodulation_controller(step, reward, prediction_error)
    
    if step % 100 == 0:
        print(f"Step {step}: Dopamine={dopamine.getLevel():.3f}, "
              f"Curiosity={curiosity.getLevel():.3f}, "
              f"Novelty={novelty.getLevel():.3f}")
```

### Development and Maturation

```python
import pynlm
import time

def developmental_simulation(brain, total_steps=10000):
    print("Starting developmental simulation...")
    
    # Enable development
    brain.enableDevelopment(True)
    
    development_stages = []
    
    for step in range(total_steps):
        # Get current developmental stage
        stage = brain.getDevelopmentalStage()
        development_stages.append(stage)
        
        # Update brain with time factor
        brain.step(step, step * 0.001)  # Real time in seconds
        
        # Track key developmental metrics
        if step % 1000 == 0:
            firing_rate = brain.getAverageFiringRate()
            e_i_ratio = brain.getExcitationInhibitionRatio()
            plasticity = brain.getStructuralPlasticity().getPlasticityLevel()
            
            print(f"Step {step}: Stage={stage}, "
                  f"FiringRate={firing_rate:.3f}, "
                  f"E/I={e_i_ratio:.2f}, "
                  f"Plasticity={plasticity:.3f}")
        
        # Store developmental snapshots
        if step % 100 == 0:
            snapshot = {
                'step': step,
                'stage': stage,
                'neurons': brain.getTotalNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'synapses': brain.getTotalSynapseCount(),
                'plasticity': plasticity if 'plasticity' in locals() else 0.0
            }
            # Store snapshot for later analysis
    
    # Analyze development trajectory
    unique_stages = list(set(development_stages))
    print(f"Development completed in {len(unique_stages)} stages")
    print(f"Final stage: {development_stages[-1]}")
    
    return development_stages

# Run developmental simulation
stages = developmental_simulation(brain, 5000)
```

## Configuration System Enhancements

### Advanced Configuration

```python
import pynlm

class AdvancedNLMConfigurator:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.profiles = {}
    
    def create_profile(self, name, **kwargs):
        """Create a named configuration profile"""
        profile_config = self.config.copy()
        
        for key, value in kwargs.items():
            if hasattr(profile_config, f'set_{key}'):
                getattr(profile_config, f'set_{key}')(value)
            else:
                profile_config.set(key, value)
        
        self.profiles[name] = profile_config
        return profile_config
    
    def get_profile(self, name):
        """Get a configuration profile"""
        return self.profiles.get(name)
    
    def compare_profiles(self, *profile_names):
        """Compare multiple configuration profiles"""
        comparison = {}
        
        for name in profile_names:
            if name in self.profiles:
                config = self.profiles[name]
                comparison[name] = {
                    'neuron_count': config.get('neuron_count', 0),
                    'simulation_timestep': config.get('simulation_timestep', 0.001),
                    'connection_probability': config.get('connection_probability', 0.1),
                    'learning_rate': config.get('learning_rate', 0.01)
                }
        
        return comparison

# Create and compare profiles
configurator = AdvancedNLMConfigurator()

# Create different profiles for different use cases
explorer_profile = configurator.create_profile(
    'explorer',
    neuron_count=3000,
    connection_probability=0.15,
    curiosity=1.0,
    exploration_rate=0.8
)

learner_profile = configurator.create_profile(
    'learner',
    neuron_count=2000,
    connection_probability=0.12,
    reward_modulation=1.0,
    learning_rate=0.05
)

stable_profile = configurator.create_profile(
    'stable',
    neuron_count=1500,
    connection_probability=0.08,
    development_rate=0.3,
    exploration_rate=0.2
)

# Compare profiles
comparison = configurator.compare_profiles('explorer', 'learner', 'stable')
print("Profile comparison:")
for name, params in comparison.items():
    print(f"{name}: {params}")
```

## Performance Monitoring and Optimization

### Real-time Performance Monitoring

```python
import pynlm
import time
import matplotlib.pyplot as plt

class PerformanceMonitor:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.metrics = {
            'steps': [],
            'firing_rates': [],
            'rewards': [],
            'memory_usage': [],
            'neural_activity': []
        }
        self.start_time = time.time()
    
    def record_step(self, step, reward=0.0):
        """Record metrics for a single simulation step"""
        self.metrics['steps'].append(step)
        self.metrics['firing_rates'].append(self.brain.getAverageFiringRate())
        self.metrics['rewards'].append(reward)
        self.metrics['memory_usage'].append(self.get_memory_usage())
        self.metrics['neural_activity'].append(self.get_neural_activity())
    
    def get_memory_usage(self):
        """Estimate memory usage"""
        if self.brain.getWorkingMemory():
            return self.brain.getWorkingMemory().getActiveTraces()
        return 0
    
    def get_neural_activity(self):
        """Get current neural activity level"""
        return self.brain.getTotalSpikeCount()
    
    def optimize_parameters(self):
        """Analyze performance and suggest optimizations"""
        # Analyze firing rate patterns
        avg_firing_rate = sum(self.metrics['firing_rates']) / len(self.metrics['firing_rates'])
        
        # Check for optimal firing range (typically 1-10 Hz for stable operation)
        if avg_firing_rate > 20:
            return {'recommendation': 'reduce_neuron_excitability', 
                    'reason': 'Firing rate too high - risk of instability'}
        elif avg_firing_rate < 0.5:
            return {'recommendation': 'increase_neuron_excitability',
                    'reason': 'Firing rate too low - insufficient activation'}
        else:
            return {'recommendation': 'maintain_current_parameters',
                    'reason': 'Optimal firing rate range'}
    
    def generate_report(self):
        """Generate performance report"""
        total_time = time.time() - self.start_time
        steps_per_second = len(self.metrics['steps']) / total_time if total_time > 0 else 0
        
        report = {
            'total_steps': len(self.metrics['steps']),
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'average_firing_rate': sum(self.metrics['firing_rates']) / len(self.metrics['firing_rates']),
            'average_reward': sum(self.metrics['rewards']) / len(self.metrics['rewards']),
            'memory_efficiency': sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage'])
        }
        
        return report

# Use performance monitor in simulation
monitor = PerformanceMonitor(brain, world, agent)

for step in range(1000):
    # Run simulation step
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
    
    # Record metrics
    reward = calculate_reward(world, action)
    monitor.record_step(step, reward)

# Get optimization recommendations
optimization = monitor.optimize_parameters()
print("Optimization recommendations:", optimization)

# Generate performance report
report = monitor.generate_report()
print("Performance report:", report)
```

## Advanced Examples

### Example 1: Learning Maze Navigation

```python
import pynlm

def maze_navigation_learning():
    # Create brain optimized for spatial learning
    config = pynlm.createDefaultConfig()
    config.set('neuron_count', 3000)
    config.set('connection_probability', 0.12)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a maze environment
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=8, visionHeight=8)
    
    # Add maze structure
    for x in range(0, 100, 10):
        world.addWall(x, 0, x+9, 0)      # Top wall
        world.addWall(x, 99, x+9, 99)    # Bottom wall
        world.addWall(0, x, 0, x+9)      # Left wall
        world.addWall(99, x, 99, x+9)    # Right wall
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Learn maze navigation
    for episode in range(50):
        world.reset()
        agent.reset()
        
        for step in range(200):
            # Get perception
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Select action
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, step * 0.1)
            
            # Get reward
            reward = 0.0
            if world.isAtGoal():
                reward = 100.0
                break
            elif world.isNearWall():
                reward = -10.0
            
            # Apply reward modulation
            if agent.getRewardModulation():
                agent.applyRewardModulation(reward, brain.getPredictionErrorSignal())
    
    return brain, world, agent

# Run maze learning
brain, world, agent = maze_navigation_learning()
print("Maze learning completed!")
```

### Example 2: Social Learning Simulation

```python
import pynlm
import numpy as np

def social_learning_simulation(num_agents=5, steps=1000):
    # Create multiple agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set('neuron_count', 1500 + i * 500)
        config.set('connection_probability', 0.1 + i * 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(None)  # No shared world for social learning
        
        agents.append(agent)
        brains.append(brain)
    
    # Social learning simulation
    for step in range(steps):
        # Each agent experiences different environment
        for i, (agent, brain) in enumerate(zip(agents, brains)):
            # Generate random experience
            experience = np.random.normal(0, 1, 10)
            
            # Process experience
            percept = pynlm.SensoryInput(experience)
            brain.receiveSensoryInput(percept)
            
            # Learn from experience
            brain.step(step)
            
            # Share learned patterns
            if i > 0:
                # Social learning: observe and imitate successful strategies
                successful_neurons = agent.getSuccessfulNeurons()
                if successful_neurons:
                    # Imitate successful neural patterns
                    agent.imitate(brain, successful_neurons, imitation_strength=0.5)
        
        # Track social learning effects
        if step % 100 == 0:
            avg_performance = np.mean([agent.getPerformance() for agent in agents])
            print(f"Step {step}: Average performance = {avg_performance:.3f}")
    
    return agents, brains

# Run social learning
agents, brains = social_learning_simulation(num_agents=3, steps=500)
print("Social learning simulation completed!")
```

## Troubleshooting and Common Issues

### Common Issues and Solutions

1. **"Brain not responding to input"**
   - Ensure `brain.initialize()` is called before simulation
   - Check that sensory input is properly connected
   - Verify neuron count is adequate

2. **"Learning not occurring"**
   - Check that `agent.enableRewardModulation(True)` is set
   - Ensure `agent.enableCuriosity(True)` for intrinsic motivation
   - Verify that the brain has sufficient neuron diversity

3. **"Memory not storing experiences"**
   - Confirm `agent.enableDevelopment(True)` for memory formation
   - Check episodic memory capacity settings
   - Verify working memory size is appropriate

4. **"Performance too slow"**
   - Reduce neuron count for faster simulation
   - Decrease simulation timestep
   - Optimize configuration parameters

### Debugging Tools

```python
import pynlm

def debug_simulation(brain, world, agent):
    """Debug simulation with detailed logging"""
    print("=== NLM Simulation Debug ===")
    
    # Check system state
    print(f"Brain state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"World state: {world.getWidth()}x{world.getHeight()} grid")
    
    # Check memory systems
    if brain.getWorkingMemory():
        print(f"Working memory: {brain.getWorkingMemory().getActiveTraces()} active traces")
    
    if brain.getEpisodicMemory():
        print(f"Episodic memory: {brain.getEpisodicMemory().getEpisodeCount()} episodes")
    
    # Check neuromodulation
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
    
    # Check development
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Check action selection
    action = agent.decodeMotorCommand()
    print(f"Selected action: {action.action_type}")
    
    # Get brain statistics
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return True
```

## Advanced Configuration Profiles

### Predefined Configuration Profiles

```python
import pynlm

class NLMConfigurationProfiles:
    @staticmethod
    def get_explorer_profile():
        """Profile optimized for exploration and curiosity"""
        config = pynlm.createDefaultConfig()
        config.set('neuron_count', 4000)
        config.set('connection_probability', 0.18)
        config.set('curiosity', 1.0)
        config.set('exploration_rate', 0.8)
        config.set('development_rate', 0.5)
        return config
    
    @staticmethod
    def get_learner_profile():
        """Profile optimized for rapid learning"""
        config = pynlm.createDefaultConfig()
        config.set('neuron_count', 3000)
        config.set('connection_probability', 0.15)
        config.set('reward_modulation', 1.0)
        config.set('learning_rate', 0.08)
        config.set('memory_capacity', 1000)
        return config
    
    @staticmethod
    def get_balanced_profile():
        """Balanced profile for stable performance"""
        config = pynlm.createDefaultConfig()
        config.set('neuron_count', 2000)
        config.set('connection_probability', 0.12)
        config.set('development_rate', 0.3)
        config.set('exploration_rate', 0.4)
        config.set('reward_modulation', 0.7)
        return config
    
    @staticmethod
    def get_researcher_profile():
        """Profile optimized for research and experimentation"""
        config = pynlm.createDefaultConfig()
        config.set('neuron_count', 1000)
        config.set('connection_probability', 0.08)
        config.set('simulation_timestep', 0.0005)
        config.set('checkpoint_interval', 500)
        config.set('enable_development', True)
        config.set('enable_replay', True)
        return config

# Use profiles
profiles = NLMConfigurationProfiles()
explorer_config = profiles.get_explorer_profile()
learner_config = profiles.get_learner_profile()
balanced_config = profiles.get_balanced_profile()
researcher_config = profiles.get_researcher_profile()
```

## API Reference

### Core Classes

#### Brain
- `createBrain(config)`: Create a new brain
- `initialize()`: Initialize brain state
- `step(step)`: Run simulation step
- `receiveSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Select action
- `enableRewardModulation(bool)`: Enable learning
- `enableCuriosity(bool)`: Enable exploration
- `enableDevelopment(bool)`: Enable maturation
- `enableStructuralPlasticity(bool)`: Enable neural growth
- Various getter methods for system statistics

#### World
- `createSimpleWorld()`: Create simple environment
- `configure(width, height, visionWidth, visionHeight)`: Set world dimensions
- `update(dt)`: Update world state
- `getSensoryPercept()`: Get current perception
- `applyMotorCommand(action, time)`: Apply action to world
- `reset()`: Reset world to initial state

#### AgentBrain
- `createAgentBrain(brain)`: Create agent from brain
- `initialize(world)`: Initialize with environment
- `processSensoryInput(percept)`: Process sensory information
- `decodeMotorCommand()`: Select motor action
- `applyRewardModulation(reward, prediction_error)`: Learn from feedback
- Various enable methods for learning capabilities

### Configuration
- `createDefaultConfig()`: Create default configuration
- `Config` class with various setter/getter methods
- Profile management utilities

### Memory Systems
- Working memory for active information
- Episodic memory for experience storage
- Associative memory for pattern associations

### Neuromodulation Systems
- Dopamine for reward learning
- Curiosity for exploration motivation
- Novelty for change detection
- Prediction error for learning accuracy

## Performance Tips

### Optimization Strategies

1. **Neuron Count Optimization**
   - Start with 1000-2000 neurons for basic functionality
   - Increase to 3000-4000 for complex tasks
   - 5000+ neurons for advanced simulations

2. **Simulation Timestep**
   - Use 0.001s (1ms) for standard simulations
   - Use 0.0005s for higher precision
   - Use 0.002s for faster performance

3. **Memory Management**
   - Set appropriate working memory capacity
   - Configure episodic memory limits
   - Monitor memory usage during long simulations

4. **Learning Parameters**
   - Start with moderate learning rates
   - Adjust based on task requirements
   - Monitor learning progress

### Common Performance Issues and Solutions

| Issue | Solution |
|-------|----------|
| Low firing rates | Increase neuron excitability |
| High memory usage | Reduce memory capacity |
| Slow simulation | Decrease neuron count |
| Unstable learning | Adjust learning rates |

## Testing and Validation

### Basic Functionality Tests

```python
import pynlm

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Running basic functionality tests...")
    
    # Test 1: Brain creation and initialization
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain.initialize()
    print("✓ Brain creation and initialization passed")
    
    # Test 2: Simulation steps
    for step in range(10):
        brain.step(step)
    print("✓ Simulation steps passed")
    
    # Test 3: Sensory processing
    percept = pynlm.SensoryInput([0.5, 0.8, 0.3])
    brain.receiveSensoryInput(percept)
    print("✓ Sensory processing passed")
    
    # Test 4: Action selection
    action = brain.decodeMotorCommand()
    assert action is not None
    print("✓ Action selection passed")
    
    print("All basic functionality tests passed!")

# Run tests
test_basic_functionality()
```

### Integration Tests

```python
import pynlm

def test_integration():
    """Test integrated systems"""
    print("Running integration tests...")
    
    # Create complete agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run integration test
    for step in range(100):
        # Complete agent-environment loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Apply learning
        reward = 1.0 if world.isAtGoal() else 0.0
        agent.applyRewardModulation(reward, brain.getPredictionErrorSignal())
    
    print("✓ Integration test passed")
    return True

# Run integration test
test_integration()
```

## Future Enhancements

### Planned Improvements

1. **Advanced Neural Architectures**
   - Implement convolutional neural networks
   - Add recurrent network capabilities
   - Support for sparse connectivity patterns

2. **Enhanced Learning Mechanisms**
   - Transfer learning capabilities
   - Meta-learning for optimization
   - Few-shot learning support

3. **Advanced Memory Systems**
   - Long-term memory consolidation
   - Working memory optimization
   - Memory replay mechanisms

4. **Better Visualization Tools**
   - Real-time neural activity visualization
   - Memory usage tracking
   - Performance monitoring dashboards

### Research Features

1. **Experiment Control**
   - Experiment runner for systematic testing
   - Parameter sweep utilities
   - Statistical analysis tools

2. **Reproducible Research**
   - Checkpoint-based state saving
   - Experiment documentation
   - Results aggregation

3. **Simulation Frameworks**
   - Environment creation tools
   - Agent behavior testing
   - Comparative analysis

## Conclusion

The enhanced NLM Python interface provides a comprehensive, user-friendly way to work with artificial brain systems. With improved documentation, advanced features, and robust testing capabilities, users can now:

- Quickly create and experiment with neural systems
- Leverage advanced learning and development mechanisms
- Monitor and optimize system performance
- Conduct systematic research and experimentation
- Reproduce and share experimental results

The interface balances ease of use with advanced functionality, making it suitable for both beginners and experienced researchers working with artificial developmental systems.

---

*Note: This is a continuously improving interface. Future versions will include additional features, optimizations, and enhancements based on user feedback and research needs.*
