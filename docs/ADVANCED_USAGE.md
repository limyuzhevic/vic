# NLM - Advanced Usage Guide for Experienced Users

## Introduction

This guide covers advanced usage patterns, debugging techniques, and performance optimization for NLM (Neural Learning Machine) Python users. It assumes familiarity with the basic concepts from Easy Usage Guide.

## Advanced Brain Configuration

### Custom Neuron Populations

```python
import pynlm

# Create custom configuration with specific neuron types
config = pynlm.createDefaultConfig()

# Set custom neuron counts
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 4)

# Configure specific regions for different functions
config.set("region.0.type", "cognitive")  # Cortex-like region
config.set("region.1.type", "sensory")    # Sensory processing
config.set("region.2.type", "motor")      # Motor control
config.set("region.3.type", "memory")     # Memory consolidation

# Adjust synaptic properties
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.7)
config.set("brain.initial_weight_std", 0.15)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("synaptogenesis_rate", 0.0005)
config.set("pruning_rate", 0.00005)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Development Stage Management

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with development tracking
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all development features
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Custom development timeline
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Run simulation with detailed logging
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Log developmental progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Neuron count: {brain.getTotalNeuronCount()}")
        print(f"  Synapse count: {brain.getTotalSynapseCount()}")
        print(f"  Cur level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

## Memory System Integration

### Working Memory with Active Traces

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory system
working_memory = brain.getWorkingMemory()
if working_memory:
    working_memory.setCapacity(brain.getTotalNeuronCount() // 8)
    print(f"Working memory capacity set to: {working_memory.getCapacity()}")
    
    # Store important neural activities
    for step in range(100):
        brain.step(step)
        # Simulate storing neural patterns
        for neuron_id in brain.getActiveNeuronIds():
            if neuron_id:
                activation = brain.getNeuronActivation(neuron_id) / 10.0
                working_memory.storeToNeuron(neuron_id, activation)
    
    print(f"Active traces: {working_memory.getActiveTraces()}")
```

### Episodic Memory with Episode Management

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create simple episode tracking
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable episodic memory
agent.enableCuriosity(True)

episodes = []
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Create episode at key moments
    if step % 100 == 0:
        episode_data = {
            'step': step,
            'time': world.getSimulationTime(),
            'vision': world.getSensoryPercept().getVision(),
            'action': agent.decodeMotorCommand(),
            'reward': 0.0  # Would come from world
        }
        episodes.append(episode_data)
        
        # Store in episodic memory
        if brain.getEpisodicMemory():
            # Store neural pattern representation
            brain.getEpisodicMemory().recordExperience(
                brain.getActiveNeuronPatterns(),
                world.getSimulationTime()
            )

print(f"Created {len(episodes)} episodes")
```

## Prediction and Planning

### Neural Prediction System

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    prediction_system.setPredictionHorizon(20)
    prediction_system.setPredictionAccuracy(0.8)
    
    # Feed sequence data for prediction
    sequence_data = []
    for step in range(100):
        brain.step(step)
        # Extract sensory sequence
        if step % 10 == 0:
            sequence_data.append(world.getSensoryPercept())
    
    # Train prediction model
    if sequence_data:
        prediction_system.train(sequence_data)
        
        # Make predictions
        future_steps = prediction_system.predictNextSteps(5)
        for i, pred_step in enumerate(future_steps):
            print(f"Predicted step {i}: {pred_step}")
```

### Neural Planning

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

planner = brain.getPlanner()
if planner:
    planner.setPlanningDepth(3)
    planner.setActionSelectionTimeout(100)
    
    # Create action options
    action_options = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact
    ]
    
    for step in range(200):
        brain.step(step)
        
        # Plan sequence of actions
        if step % 50 == 0:
            planned_sequence = planner.planActionSequence(
                action_options,
                brain.getSensoryInput(),
                5
            )
            
            # Execute planned actions
            for action in planned_sequence:
                world.applyMotorCommand(action, world.getSimulationTime())
```

## Neuromodulation and Learning

### Advanced Neuromodulation

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Configure all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Custom neuromodulation parameters
brain.getDopamine().setLearningRate(0.01)
brain.getCuriosity().setNoveltyThreshold(0.5)
brain.getPredictionError().setErrorGain(2.0)
brain.getNovelty().setDetectionThreshold(0.3)

# Run learning simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Calculate reward and novelty
    reward = calculate_reward(world)
    novelty = calculate_novelty(world)
    
    # Apply all neuromodulation
    agent.applyRewardModulation(reward, reward * 0.9)  # Predicted reward
    agent.updateDevelopment(0.1)
    
    # Log neuromodulation levels
    if step % 100 == 0:
        print(f"Dopamine: {brain.getDopamine().getLevel():.3f}")
        print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction Error: {agent.getPredictionError():.3f}")
```

## Debugging and Monitoring

### Performance Monitoring

```python
import pynlm
import time
import matplotlib.pyplot as plt

class PerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.step_times = []
        self.spike_counts = []
        self.memory_usage = []
        self.neuromodulation_levels = []
    
    def start_monitoring(self, duration_steps=1000):
        for step in range(duration_steps):
            step_start = time.time()
            
            # Run simulation step
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            step_time = time.time() - step_start
            self.step_times.append(step_time)
            self.spike_counts.append(self.brain.getTotalSpikeCount())
            self.memory_usage.append(self.brain.getTotalNeuronCount())
            self.neuromodulation_levels.append({
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'reward': self.agent.getNeuromodulationLevel()
            })
    
    def generate_report(self):
        print("=== NLM Performance Report ===")
        print(f"Average step time: {sum(self.step_times)/len(self.step_times)*1000:.2f} ms")
        print(f"Max step time: {max(self.step_times)*1000:.2f} ms")
        print(f"Min step time: {min(self.step_times)*1000:.2f} ms")
        print(f"Total spikes: {self.spike_counts[-1] if self.spike_counts else 0}")
        print(f"Final neuron count: {self.brain.getTotalNeuronCount()}")
        print(f"Final synapse count: {self.brain.getTotalSynapseCount()}")
        
        # Generate performance plots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        axes[0, 0].plot(self.step_times)
        axes[0, 0].set_title('Step Time (seconds)')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Time (s)')
        
        axes[0, 1].plot(self.spike_counts)
        axes[0, 1].set_title('Cumulative Spike Count')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Spikes')
        
        axes[1, 0].plot([m['curiosity'] for m in self.neuromodulation_levels])
        axes[1, 0].set_title('Curiosity Level')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Level')
        
        axes[1, 1].plot([m['novelty'] for m in self.neuromodulation_levels])
        axes[1, 1].set_title('Novelty Level')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Level')
        
        plt.tight_layout()
        plt.savefig('performance_report.png')
        plt.show()

# Usage
monitor = PerformanceMonitor(brain, agent, world)
monitor.start_monitoring(500)
monitor.generate_report()
```

## Advanced Configuration Patterns

### Configuration Templates

```python
import pynlm
import json

class NLMConfigurationTemplates:
    @staticmethod
    def create_basic_config(neuron_count=1000):
        """Create a basic configuration for simple simulations"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 2)
        config.set("plasticity.stdp.enable", True)
        return config
    
    @staticmethod
    def create_advanced_config(neuron_count=5000, development=True):
        """Create an advanced configuration with all features enabled"""
        config = pynlm.createDefaultConfig()
        
        # Core parameters
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 4)
        config.set("brain.connection_probability", 0.05)
        config.set("brain.initial_weight_mean", 0.5)
        config.set("brain.initial_weight_std", 0.1)
        
        # Memory systems
        config.set("working_memory.capacity", neuron_count // 8)
        config.set("episodic_memory.max_episodes", 1000)
        config.set("associative_memory.pattern_count", 500)
        
        # Prediction and planning
        config.set("prediction.horizon", 20)
        config.set("planner.depth", 3)
        config.set("attention.inhibition_strength", 0.5)
        
        # Neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.threshold", 0.5)
        config.set("neuromod.novelty.threshold", 0.3)
        config.set("neuromod.prediction.error_gain", 2.0)
        
        # Development
        config.set("development.enable", development)
        config.set("synaptogenesis.rate", 0.0001)
        config.set("pruning.rate", 0.00001)
        
        # Performance
        config.set("checkpoint.interval", 100)
        config.set("simulation.timestep", 0.001)
        
        return config
    
    @staticmethod
    def create_research_config(neuron_count=10000, intensive_learning=True):
        """Create a research-grade configuration for intensive learning studies"""
        config = NLMConfigurationTemplates.create_advanced_config(neuron_count)
        
        # Intensive learning parameters
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("synaptogenesis.rate", 0.001)
        config.set("pruning.rate", 0.0001)
        
        # Enhanced memory
        config.set("working_memory.capacity", neuron_count // 5)
        config.set("episodic_memory.max_episodes", 5000)
        config.set("associative_memory.pattern_count", 2000)
        
        # Advanced prediction
        config.set("prediction.horizon", 50)
        config.set("prediction.accuracy", 0.95)
        config.set("planner.depth", 5)
        
        # Developmental focus
        config.set("development.enable", True)
        config.set("development.critical_period_duration", 1000)
        config.set("development.maturation_rate", 0.01)
        
        return config

# Example usage
basic_config = NLMConfigurationTemplates.create_basic_config(2000)
advanced_config = NLMConfigurationTemplates.create_advanced_config(10000)
research_config = NLMConfigurationTemplates.create_research_config()
```

### Environment Configuration

```python
import pynlm

class NLMWorldEnvironment:
    @staticmethod
    def create_simple_world(width=20, height=20, vision_width=8, vision_height=8):
        """Create a simple 2D world for NLM testing"""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        
        # Add resources
        for x in range(0, width, 5):
            for y in range(0, height, 5):
                world.addObject(pynlm.WorldObject(
                    x, y, pynlm.WorldObjectType.Resource, value=1.0
                ))
        
        # Add hazards
        for x in range(width//2, width, 3):
            world.addObject(pynlm.WorldObject(
                x, height//2, pynlm.WorldObjectType.Hazard, value=1.0
            ))
        
        return world
    
    @staticmethod
    def create_complex_environment():
        """Create a complex environment with multiple interacting elements"""
        world = pynlm.createSimpleWorld()
        world.configure(50, 50, 12, 12)
        
        # Add different types of objects
        object_types = [
            (pynlm.WorldObjectType.Resource, 1.0, 0.8),
            (pynlm.WorldObjectType.Hazard, 1.0, 0.3),
            (pynlm.WorldObjectType.Wall, 1.0, 0.5),
            (pynlm.WorldObjectType.Marker, 0.5, 0.2)
        ]
        
        for obj_type, value, probability in object_types:
            for x in range(world.getWidth()):
                for y in range(world.getHeight()):
                    if random.random() < probability:
                        world.addObject(pynlm.WorldObject(
                            x, y, obj_type, value=value
                        ))
        
        return world
```

## Error Handling and Recovery

### Robust Simulation Management

```python
import pynlm
import traceback

class RobustNLMSimulation:
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        self.step_count = 0
        self.max_steps = config.getOr<size_t>("simulation.max_steps", 10000)
        self.checkpoint_interval = config.getOr<size_t>("checkpoint.interval", 100)
        self.last_checkpoint = 0
        
    def initialize(self):
        """Initialize all NLM components with error handling"""
        try:
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            print("Brain initialized successfully")
        except Exception as e:
            print(f"Brain initialization failed: {e}")
            return False
        
        try:
            self.world = pynlm.createSimpleWorld()
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            print("Agent and world initialized successfully")
        except Exception as e:
            print(f"Agent initialization failed: {e}")
            return False
        
        return True
    
    def step(self):
        """Run a single simulation step with error recovery"""
        try:
            # Update world
            self.world.update(0.1)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(self.step_count)
            
            # Motor control
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            self.step_count += 1
            
            # Checkpoint management
            if self.step_count - self.last_checkpoint >= self.checkpoint_interval:
                self.save_checkpoint()
                self.last_checkpoint = self.step_count
            
            return True
            
        except Exception as e:
            print(f"Step {self.step_count} failed: {e}")
            traceback.print_exc()
            
            # Attempt recovery
            if self.step_count > self.checkpoint_interval:
                try:
                    self.load_checkpoint()
                    print("Recovery successful - loaded from checkpoint")
                    return True
                except Exception as load_error:
                    print(f"Recovery failed: {load_error}")
                    return False
            return False
    
    def save_checkpoint(self):
        """Save simulation state"""
        if self.brain:
            checkpoint_file = f"nlm_checkpoint_{self.step_count}.bin"
            if self.brain.save(checkpoint_file):
                print(f"Checkpoint saved: {checkpoint_file}")
                return True
            else:
                print(f"Failed to save checkpoint: {checkpoint_file}")
                return False
    
    def load_checkpoint(self):
        """Load simulation state"""
        if self.brain:
            # Try to load the most recent checkpoint
            import glob
            checkpoint_files = glob.glob("nlm_checkpoint_*.bin")
            if checkpoint_files:
                latest_checkpoint = max(checkpoint_files, key=lambda x: x.split('_')[1].split('.')[0])
                if self.brain.load(latest_checkpoint):
                    print(f"Loaded checkpoint: {latest_checkpoint}")
                    return True
            print("No checkpoint found to load")
            return False
    
    def run(self):
        """Run the complete simulation with robust error handling"""
        if not self.initialize():
            print("Failed to initialize simulation")
            return False
        
        print(f"Starting robust NLM simulation for up to {self.max_steps} steps")
        
        successful_steps = 0
        failed_steps = 0
        
        while successful_steps < self.max_steps:
            if self.step():
                successful_steps += 1
                
                # Progress reporting
                if successful_steps % 1000 == 0:
                    print(f"Progress: {successful_steps} steps completed")
                    
                    # Log system status
                    if self.brain:
                        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
                        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
                        print(f"  Active neurons: {self.brain.getActiveNeuronCount()}")
                        print(f"  Cur level: {self.agent.getCuriosityLevel():.3f}")
            else:
                failed_steps += 1
                print(f"Failed steps: {failed_steps}")
                
                # If too many consecutive failures, abort
                if failed_steps > 10:
                    print("Too many consecutive failures - aborting simulation")
                    break
        
        print(f"Simulation completed: {successful_steps} successful steps, {failed_steps} failures")
        
        # Final checkpoint
        self.save_checkpoint()
        
        return True

# Usage example
config = pynlm.createDefaultConfig()
simulation = RobustNLMSimulation(config)
simulation.run()
```

## API Reference for Advanced Users

### Core Classes

#### Brain Class
- **Methods**: initialize(), step(), receiveSensoryInput(), produceAction()
- **Properties**: neuron_count, synapse_count, firing_rate, developmental_stage
- **Access Methods**: getWorkingMemory(), getPredictionSystem(), getPlanner()

#### AgentBrain Class  
- **Methods**: processSensoryInput(), decodeMotorCommand(), applyRewardModulation()
- **Properties**: curiosity_level, novelty_level, neuromodulation_level
- **Controls**: enableRewardModulation(), enableCuriosity(), enableDevelopment()

#### SimpleWorld Class
- **Methods**: configure(), reset(), update(), applyMotorCommand()
- **Properties**: width, height, vision_width, vision_height
- **Access**: getSensoryPercept(), getAgentBody()

### Configuration Management

#### Config Class
- **Creation**: createDefaultConfig()
- **Operations**: set(), has(), getKeys(), loadFromFile(), saveToFile()
- **Common Keys**: brain.neuron_count, plasticity.stdp.enable, neuromod.dopamine.scale

### Enumerations

#### NeuronType
- Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

#### ActionType
- MoveForward, MoveBackward, TurnLeft, TurnRight, Look, Interact, Eat, Rest

#### DevelopmentalStage
- Initial, CriticalPeriod, Maturation, Adult, Aging

## Performance Optimization Tips

### Memory Efficiency
1. **Monitor memory usage**: Use built-in memory tracking and checkpoint periodically
2. **Configure appropriate capacities**: Set working memory capacity based on brain size
3. **Enable structural plasticity selectively**: Only for long-running simulations

### Computational Efficiency
1. **Batch processing**: Process multiple sensory inputs before brain steps
2. **Adaptive timestep**: Increase timestep during stable periods
3. **Region-specific processing**: Focus on active brain regions

### I/O Optimization
1. **Asynchronous checkpointing**: Save checkpoints in background threads
2. **Compressed storage**: Use checkpoint compression for large brains
3. **Selective serialization**: Only save critical neural states

## Troubleshooting Guide

### Common Issues and Solutions

**Problem**: "Brain isn't doing anything"
**Solution**: 
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that sensory neurons have connections
- Verify developmental stage is appropriate

**Problem**: "Agent isn't moving"
**Solution**:
- Check that `world.update()` is being called
- Verify `agent.decodeMotorCommand()` returns valid actions
- Ensure motor neurons have synaptic connections

**Problem**: "Everything is 0"
**Solution**:
- Increase simulation steps for "warm-up" period
- Inject initial sensory input to activate neurons
- Check random seed for reproducible results

**Problem**: "High memory usage"
**Solution**:
- Reduce brain size for initial testing
- Disable unnecessary plasticity mechanisms
- Implement periodic checkpointing

### Advanced Debugging Techniques

```python
# Debug neural activity patterns
brain = pynlm.createBrain(config)
brain.initialize()

# Log detailed neural state
for step in range(100):
    brain.step(step)
    
    # Check for silent neurons
    silent_count = brain.getNeuronCount() - brain.getActiveNeuronCount()
    print(f"Step {step}: {silent_count} silent neurons")
    
    # Check synaptic health
    healthy_synapses = brain.getHealthySynapseCount()
    total_synapses = brain.getTotalSynapseCount()
    print(f"  Synaptic health: {healthy_synapses/total_synapses:.2%}")

# Monitor memory system performance
working_memory = brain.getWorkingMemory()
if working_memory:
    for step in range(100):
        brain.step(step)
        print(f"Working memory capacity: {working_memory.getActiveTraces()}")
```

## Advanced Research Features

### Custom Neuromodulation

```python
import pynlm
from typing import Callable

class CustomNeuromodulator:
    def __init__(self, name, update_function):
        self.name = name
        self.update_function = update_function
        self.level = 0.0
    
    def update(self, prediction_error: float, reward: float):
        self.level = self.update_function(prediction_error * reward)
        return self.level

# Create custom neuromodulator for stress response
custom_stress = CustomNeuromodulator("stress", lambda x: max(0.0, min(1.0, x * 2.0 - 0.5)))

# Integrate into brain
brain.getDopamine().registerCustomModulator(custom_stress)

# Use in simulation
for step in range(100):
    # Calculate stress based on performance variability
    stress_level = custom_stress.update(
        brain.getPredictionError(),
        brain.getTotalSpikeCount() / step if step > 0 else 0.0
    )
    print(f"Stress level: {stress_level:.3f}")
```

### Experimental Protocols

```python
import pynlm
import random

class NLMSquareGenerationExperiment:
    """Experiment to test whether NLM can discover square generation rules"""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.trial_count = 0
        self.success_count = 0
        self.sequence_history = []
    
    def run_square_task(self, steps=200):
        """Run a single square generation task"""
        # Reset world and brain
        self.world.reset()
        self.brain.reset()
        
        # Generate random target square
        target_square = self._generate_square()
        
        # Place square in world
        self.world.addObject(pynlm.WorldObject(
            target_square['x'], target_square['y'], 
            pynlm.WorldObjectType.Marker, value=1.0
        ))
        
        # Store initial state
        initial_state = self._capture_world_state()
        
        # Run task
        for step in range(steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.brain.receiveSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Get action and execute
            action = self.brain.produceAction()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check if square was found
            if self._check_square_found(target_square):
                self.success_count += 1
                break
            
            # Store experience
            self.sequence_history.append({
                'step': step,
                'time': self.world.getSimulationTime(),
                'action': action,
                'vision': percept.getVision()
            })
        
        self.trial_count += 1
        return self._check_square_found(target_square)
    
    def _generate_square(self):
        """Generate random square parameters"""
        return {
            'x': random.randint(5, 20),
            'y': random.randint(5, 20),
            'size': random.randint(2, 5),
            'color': random.choice(['red', 'green', 'blue'])
        }
    
    def _capture_world_state(self):
        """Capture current world state for replay"""
        return {
            'time': self.world.getSimulationTime(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synaptic_strengths': self._get_synaptic_patterns()
        }
    
    def _get_synaptic_patterns(self):
        """Get current synaptic weight patterns"""
        patterns = []
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            for synapse in region.getSynapses():
                patterns.append({
                    'source': synapse.getSourceNeuron().index(),
                    'destination': synapse.getDestinationNeuron().index(),
                    'weight': synapse.getWeight(),
                    'type': synapse.getType()
                })
        return patterns
    
    def _check_square_found(self, target_square):
        """Check if brain has identified the target square"""
        # Simple heuristic: check if brain's internal representation matches target
        # In practice, this would involve more sophisticated pattern matching
        return random.random() < 0.1  # Placeholder
    
    def run_experiment(self, trials=10):
        """Run complete experiment"""
        print("=== NLM Square Generation Experiment ===")
        
        for trial in range(trials):
            print(f"\nTrial {trial + 1}/{trials}")
            success = self.run_square_task()
            print(f"Result: {'SUCCESS' if success else 'FAILED'}")
        
        print(f"\n=== Experiment Results ===")
        print(f"Total trials: {self.trial_count}")
        print(f"Successful trials: {self.success_count}")
        print(f"Success rate: {self.success_count/self.trial_count:.1%}")
        
        return self.success_count / self.trial_count