# NLM - Advanced Usage Guide for Experienced Users

## Overview

This guide covers advanced usage patterns, new features, and sophisticated techniques for working with NLM (Neural Learning Machine). It builds upon the basic concepts from `easy_usage.md` and introduces powerful new capabilities for experienced users.

## Table of Contents

1. [Advanced Object Creation and Configuration](#advanced-object-creation-and-configuration)
2. [Enhanced Sensory Processing](#enhanced-sensory-processing)
3. [Advanced Action and Behavior Control](#advanced-action-and-behavior-control)
4. [Sophisticated Simulation Control](#sophisticated-simulation-control)
5. [Performance Monitoring and Debugging](#performance-monitoring-and-debugging)
6. [Memory Management and Persistence](#memory-management-and-persistence)
7. [Training and Learning Advanced Techniques](#training-and-learning-advanced-techniques)
8. [Neural Topology Analysis](#neural-topology-analysis)
9. [Custom Simulation Strategies](#custom-simulation-strategies)
10. [Integration with External Systems](#integration-with-external-systems)

---

## Advanced Object Creation and Configuration

### Enhanced Configuration Presets

The NLM Python API now provides sophisticated configuration presets for different use cases:

```python
import pynlm

# Advanced configuration for complex tasks
advanced_config = pynlm.configureDefaults()
print(f"Advanced config has {advanced_config.getKeys()}")

# Minimal configuration for rapid testing
minimal_config = pynlm.configureMinimal()
print(f"Minimal config neuron count: {minimal_config.getOr('brain.neuron_count', 0)}")

# Custom simulation configuration
custom_config = pynlm.configureSimulation(
    neuronCount=3000,
    density=0.12f,
    connectionProb=0.06f,
    simulationSteps=50000
)

# Save and load configurations
pynlm.saveConfigAdvanced(custom_config, "my_simulation.cfg")
loaded_config = pynlm.loadConfigAdvanced("my_simulation.cfg")
```

### Advanced Object Factories

Create sophisticated objects with precise control:

```python
import pynlm

# Create advanced world objects
resource = pynlm.createResourceObject(x=5.0, y=5.0, value=2.5)
hazard = pynlm.createHazardObject(x=10.0, y=10.0, radius=2.0)
wall = pynlm.createWallObject(x=0.0, y=0.0, width=3.0, height=2.0)
world_obj = pynlm.createWorldObject(x=3.0, y=3.0, type=pynlm.WorldObjectType.Resource, value=1.5)

# Create advanced sensory inputs with custom data
vision_data = [0.1, 0.3, 0.7, 0.9] * 64  # 256 values for 16x16 vision
vision = pynlm.createVisionInput(16, 16, 3, vision_data)

audio_data = [0.2, 0.4, 0.6, 0.8] * 100  # 400 samples
audio = pynlm.createAudioInput(sampleRate=44100, samples=audio_data)

internal_signals = pynlm.createInternalSignals([0.5, -0.3, 0.8, -0.1])
```

### Advanced Action Creation

Create sophisticated actions with complex parameter structures:

```python
import pynlm

# Create basic action with parameters
basic_action = pynlm.createAction(pynlm.ActionType.Interact, [0.5, 0.8, 1.2])

# Create movement actions with precise control
move_forward = pynlm.createMovementAction(pynlm.ActionType.MoveForward, speed=2.5, angle=45.0)
turn_left = pynlm.createMovementAction(pynlm.ActionType.TurnLeft, speed=1.0, angle=30.0)
move_backward = pynlm.createMovementAction(pynlm.ActionType.MoveBackward, speed=1.5, angle=180.0)

# Create interaction actions with target objects
interact_with_door = pynlm.createInteractionAction("door_01", "open")
interact_with_resource = pynlm.createInteractionAction("resource_05", "collect")
```

---

## Enhanced Sensory Processing

### Advanced Sensory Input Processing

Process complex sensory inputs with advanced techniques:

```python
import pynlm

# Create comprehensive sensory percepts
vision = pynlm.createVisionInput(32, 32, 3, [0.5] * 1024)
touch = [0.0] * 8  # Touch sensor readings
internal = [0.3, -0.2, 0.8, -0.5]  # Internal signals
proprioception = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6]  # Body position signals
audio = pynlm.createAudioInput(22050, [0.1, 0.2, 0.3] * 100)

# Create advanced sensory percept
percept = pynlm.SensoryPercept()
percept.setVision(vision.getData())
percept.setTouch(touch)
percept.setInternal(internal)
percept.setProprioception(proprioception)
percept.setAudio(audio.getData())
percept.setTimestamp(0.0)

# Get all processed signals as a single array
all_signals = percept.getAllSignals()
print(f"Total sensory input size: {len(all_signals)}")
```

### Custom Sensory Processing Functions

```python
def custom_sensory_processor(percept, world):
    """Custom sensory processing with advanced feature extraction"""
    vision = percept.getVision()
    internal = percept.getInternal()
    
    # Extract advanced features from vision
    center_intensity = sum(vision[i] for i in range(256)) / 256.0  # Center of vision
    edge_intensity = sum(vision[i] for i in range(0, 256, 4)) / 64.0  # Edge detection
    
    # Extract temporal features from internal signals
    temporal_gradient = sum(internal[i] - internal[i-1] for i in range(1, len(internal)))
    
    # Enhanced novelty detection
    novelty_score = calculate_novelty(vision, internal)
    
    return {
        'center_intensity': center_intensity,
        'edge_intensity': edge_intensity,
        'temporal_gradient': temporal_gradient,
        'novelty_score': novelty_score,
        'timestamp': percept.getTimestamp()
    }

def calculate_novelty(vision, internal):
    """Advanced novelty detection algorithm"""
    # Implementation of novelty calculation
    avg_vision = sum(vision) / len(vision)
    avg_internal = sum(abs(x) for x in internal) / len(internal)
    return (sum(abs(v - avg_vision) for v in vision) + avg_internal) / 2.0
```

---

## Advanced Action and Behavior Control

### Sophisticated Agent Configuration

Configure agents with advanced settings for specific behaviors:

```python
import pynlm

# Create brain and world
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createWorldAdvanced(50, 50, 16, 16)
world.reset()

# Create advanced agent with customized settings
agent = pynlm.createAdvancedAgent(brain)

# Configure agent with advanced settings
pynlm.configureAgentAdvanced(
    agent=agent,
    rewardModulation=True,           # Enable reward-based learning
    structuralPlasticity=True,       # Enable structural plasticity
    development=True,                # Enable developmental processes
    curiosity=True,                 # Enable curiosity-driven exploration
    noveltyDecay=0.95f               # Decay rate for novelty detection
)

# Set up world and agent
agent.initialize(world)
world.setAgentStart(25.0, 25.0)
```

### Advanced Action Decoding Strategies

Implement custom action decoding strategies:

```python
def curiosity_based_action_decoder(agent, default_action):
    """Advanced action decoding with curiosity modulation"""
    curiosity_level = agent.getCuriosityLevel()
    prediction_error = agent.getPredictionError()
    
    if curiosity_level > 0.7:
        # High curiosity: increase exploration
        exploration_prob = 0.3 + (curiosity_level - 0.7) * 0.5
        if agent.getBrain().getRandomGenerator().uniformReal(0.0, 1.0) < exploration_prob:
            return select_random_action(agent)
    
    if abs(prediction_error) > 0.5:
        # High prediction error: adapt behavior
        return adapt_behavior_for_error(default_action, prediction_error)
    
    return default_action

def select_random_action(agent):
    """Select random action from available motor neurons"""
    actions = [pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward,
               pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight,
               pynlm.ActionType.Interact, pynlm.ActionType.Wait]
    return actions[agent.getBrain().getRandomGenerator().uniformInt(0, len(actions)-1)]

def adapt_behavior_for_error(action, error):
    """Adapt behavior based on prediction error"""
    # Implementation of behavior adaptation
    if error > 0:  # Positive error (better than expected)
        return enhance_successful_behavior(action)
    else:  # Negative error (worse than expected)
        return modify_unsuccessful_behavior(action)
```

---

## Sophisticated Simulation Control

### Advanced Simulation with Custom Control Flow

Implement sophisticated simulation control with custom stop conditions and monitoring:

```python
import pynlm
import time
from typing import Callable, Optional

class AdvancedSimulationController:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.start_time = time.time()
        self.step_count = 0
        self.total_reward = 0.0
        self.max_reward = 0.0
    
    def run_with_advanced_features(self, max_steps=10000, 
                                  stop_condition=None,
                                  monitoring_interval=100,
                                  reward_function=None):
        """Run advanced simulation with comprehensive monitoring"""
        print("Starting advanced simulation...")
        
        for step in range(max_steps):
            self.step_count = step
            
            # Update world state
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input with advanced features
            self.agent.processSensoryInput(percept)
            
            # Run brain computation
            self.brain.step(step)
            
            # Decode motor command with advanced decision making
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward using custom reward function
            reward = self.calculate_advanced_reward(percept, action, reward_function)
            
            # Apply reward modulation with advanced features
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Update statistics
            self.total_reward += reward
            if reward > self.max_reward:
                self.max_reward = reward
            
            # Custom stop condition
            if stop_condition and stop_condition(step, percept, action, reward):
                print(f"Custom stop condition triggered at step {step}")
                break
            
            # Periodic monitoring and statistics
            if step % monitoring_interval == 0:
                self.print_simulation_status(step, reward)
                
                # Advanced performance monitoring
                self.perform_performance_monitoring()
        
        # Simulation complete
        self.print_final_statistics()
        return self.get_simulation_results()
    
    def calculate_advanced_reward(self, percept, action, custom_reward_function):
        """Calculate advanced reward using multiple factors"""
        base_reward = 0.0
        
        # Reward from internal signals
        internal = percept.getInternal()
        if internal:
            base_reward += internal[0] * 2.0
        
        # Reward from action success
        if action.getType() == pynlm.ActionType.Interact:
            base_reward += 1.0
        
        # Novelty-based reward
        novelty = self.agent.getNoveltyLevel()
        base_reward += novelty * 0.5
        
        # Curiosity-based reward
        curiosity = self.agent.getCuriosityLevel()
        base_reward += curiosity * 0.3
        
        # Custom reward function
        if custom_reward_function:
            custom_reward = custom_reward_function(percept, action)
            base_reward += custom_reward
        
        return base_reward
    
    def perform_performance_monitoring(self):
        """Perform advanced performance monitoring"""
        stats = pynlm.analyzeBrainStats(self.brain)
        
        print(f"  Advanced Stats:")
        print(f"    Neural Activity: {stats['firing_neurons']} firing neurons")
        print(f"    Learning Rate: {stats['total_spikes'] / max(1, self.step_count):.2f} spikes/step")
        print(f"    Neuromodulation: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"    Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {self.agent.getNoveltyLevel():.3f}")
    
    def print_simulation_status(self, step, reward):
        """Print current simulation status"""
        elapsed = time.time() - self.start_time
        steps_per_sec = step / elapsed if elapsed > 0 else 0
        
        print(f"Step {step:5d} | "
              f"Reward: {reward:+.3f} | "
              f"Total: {self.total_reward:+.3f} | "
              f"Max: {self.max_reward:.3f} | "
              f"Rate: {steps_per_sec:5.1f} steps/sec | "
              f"Firing: {self.brain.getFiringNeuronCount()} | "
              f"Time: {elapsed:.1f}s")
    
    def print_final_statistics(self):
        """Print final simulation statistics"""
        print("\n=== Final Simulation Statistics ===")
        print(f"Total Steps: {self.step_count}")
        print(f"Total Reward: {self.total_reward:.3f}")
        print(f"Maximum Reward: {self.max_reward:.3f}")
        print(f"Average Reward: {self.total_reward / max(1, self.step_count):.3f}")
        print(f"Simulation Time: {time.time() - self.start_time:.2f} seconds")
        print(f"Steps per Second: {(self.step_count / (time.time() - self.start_time)):.2f}")
    
    def get_simulation_results(self):
        """Get comprehensive simulation results"""
        return {
            'steps_completed': self.step_count,
            'total_reward': self.total_reward,
            'max_reward': self.max_reward,
            'avg_reward': self.total_reward / max(1, self.step_count),
            'simulation_time': time.time() - self.start_time,
            'steps_per_second': (self.step_count / (time.time() - self.start_time)),
            'final_brain_stats': pynlm.analyzeBrainStats(self.brain),
            'agent_state': {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation': self.agent.getNeuromodulationLevel()
            }
        }

# Example usage
advanced_controller = AdvancedSimulationController(brain, world, agent)
results = advanced_controller.run_with_advanced_features(
    max_steps=5000,
    monitoring_interval=500,
    reward_function=lambda percept, action: 0.0  # Custom reward function
)

print(f"\nAdvanced simulation completed!")
print(f"Results: {results}")
```

---

## Performance Monitoring and Debugging

### Comprehensive Debug Reporting

Generate detailed debug reports for brain analysis:

```python
import pynlm

# Create and initialize brain
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Generate comprehensive debug report
print("Generating comprehensive debug report...")
brain_debug_report = pynlm.brainDebugReport(brain)

# Analyze performance
print("\nPerformance Analysis:")
perf_stats = pynlm.analyzeBrainStats(brain)
for key, value in perf_stats.items():
    print(f"  {key}: {value}")

# Print detailed statistics
print("\nDetailed Statistics:")
pynlm.printDetailedStats(brain)
```

### Advanced Performance Profiling

Profile brain performance with detailed metrics:

```python
import pynlm
import time

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run performance profiling
print("Starting performance profiling...")
avg_steps_per_second = pynlm.profileBrainPerformance(brain, 2000)

print(f"\nPerformance Profile:")
print(f"Average Steps per Second: {avg_steps_per_second:.2f}")
print(f"Expected Performance: {avg_steps_per_second * 60:.2f} steps/minute")
```

---

## Memory Management and Persistence

### Advanced Brain Backup and Restore

Manage brain memory with advanced backup and restore capabilities:

```python
import pynlm
import os

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Run some simulation to generate interesting brain state
for step in range(1000):
    brain.step(step)

# Create backup directory
backup_dir = "brain_backups"
os.makedirs(backup_dir, exist_ok=True)

# Create timestamped backup files
timestamp = int(time.time())
backup_file = f"{backup_dir}/brain_backup_{timestamp}.bin"

# Create brain backup
print(f"Creating brain backup: {backup_file}")
if pynlm.backupBrainMemory(brain, backup_file):
    print("✓ Brain backup successful")
else:
    print("✗ Brain backup failed")

# List available backups
print(f"\nAvailable backups:")
for filename in os.listdir(backup_dir):
    if filename.endswith('.bin'):
        filepath = os.path.join(backup_dir, filename)
        size = os.path.getsize(filepath)
        print(f"  {filename} ({size:,} bytes)")

# Restore from backup
latest_backup = f"{backup_dir}/brain_backup_{timestamp}.bin"
if os.path.exists(latest_backup):
    print(f"\nRestoring brain from backup: {latest_backup}")
    if pynlm.restoreBrainMemory(brain, latest_backup):
        print("✓ Brain restore successful")
        print(f"Brain after restore - Neurons: {brain.getTotalNeuronCount()}, "
              f"Synapses: {brain.getTotalSynapseCount()}")
    else:
        print("✗ Brain restore failed")
```

### Data Export and Import

Export brain state and simulation data for analysis:

```python
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Export brain data for analysis
export_file = "brain_data_export.json"
print(f"Exporting brain data to: {export_file}")
if pynlm.exportBrainData(brain, export_file):
    print("✓ Brain data export successful")
    
    # Load exported data for analysis
    loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    loaded_brain.load(export_file)
    print(f"✓ Brain data import successful")
    print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")
else:
    print("✗ Brain data export failed")
```

---

## Training and Learning Advanced Techniques

### Advanced Agent Training

Train agents with sophisticated learning algorithms:

```python
import pynlm

# Create training environment
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createWorldAdvanced(30, 30, 12, 12)
world.reset()
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAdvancedAgentWithLearning(brain, world)

# Define custom reward function
def custom_reward_function(percept, action, world):
    """Custom reward function for training"""
    reward = 0.0
    
    # Resource collection reward
    if action.getType() == pynlm.ActionType.Interact:
        nearby_objects = world.getNearbyObjects(world.getAgentBody().x, world.getAgentBody().y, 2.0)
        for obj in nearby_objects:
            if obj.type == pynlm.WorldObjectType.Resource:
                reward += obj.value * 2.0
    
    # Hazard avoidance penalty
    if action.getType() == pynlm.ActionType.Interact:
        nearby_objects = world.getNearbyObjects(world.getAgentBody().x, world.getAgentBody().y, 2.0)
        for obj in nearby_objects:
            if obj.type == pynlm.WorldObjectType.Hazard:
                reward -= 5.0  # Penalty for interacting with hazards
    
    # Novelty-based reward
    novelty = agent.getNoveltyLevel()
    reward += novelty * 0.5
    
    # Curiosity-based reward
    curiosity = agent.getCuriosityLevel()
    reward += curiosity * 0.3
    
    return reward

# Train agent with advanced settings
print("Starting advanced agent training...")
trained_agent = pynlm.trainAgentAdvanced(
    brain=brain,
    world=world,
    episodes=100,
    stepsPerEpisode=200,
    rewardFunction=custom_reward_function
)

print(f"\nTraining complete!")
print(f"Final agent curiosity: {trained_agent.getCuriosityLevel():.3f}")
print(f"Final agent novelty: {trained_agent.getNoveltyLevel():.3f}")
```

### Continuous Learning and Adaptation

Implement continuous learning strategies:

```python
import pynlm
import threading
import time

class ContinuousLearningSystem:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.is_learning = False
        self.learning_thread = None
    
    def start_continuous_learning(self, episodes=100, steps_per_episode=100):
        """Start continuous learning in background thread"""
        self.is_learning = True
        self.learning_thread = threading.Thread(
            target=self._continuous_learning_loop,
            args=(episodes, steps_per_episode)
        )
        self.learning_thread.daemon = True
        self.learning_thread.start()
        print(f"Started continuous learning with {episodes} episodes")
    
    def _continuous_learning_loop(self, episodes, steps_per_episode):
        """Background learning loop"""
        for episode in range(episodes):
            if not self.is_learning:
                break
                
            print(f"Continuous learning episode {episode + 1}/{episodes}")
            
            # Reset environment
            self.world.reset()
            self.world.setAgentStart(15.0, 15.0)
            
            # Run episode
            for step in range(steps_per_episode):
                if not self.is_learning:
                    break
                
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Brain computation
                self.brain.step(step)
                
                # Decode and apply action
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward
                reward = self.calculate_continuous_reward(percept, action)
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Small delay to prevent overwhelming
                time.sleep(0.001)
            
            # Brief pause between episodes
            time.sleep(0.1)
    
    def calculate_continuous_reward(self, percept, action):
        """Calculate reward for continuous learning"""
        reward = 0.0
        
        # Base reward from internal signals
        internal = percept.getInternal()
        if internal:
            reward += internal[0] * 1.5
        
        # Curiosity bonus
        curiosity = self.agent.getCuriosityLevel()
        reward += curiosity * 0.5
        
        # Novelty bonus
        novelty = self.agent.getNoveltyLevel()
        reward += novelty * 0.3
        
        return reward
    
    def stop_continuous_learning(self):
        """Stop continuous learning"""
        self.is_learning = False
        if self.learning_thread:
            self.learning_thread.join(timeout=5.0)
        print("Continuous learning stopped")

# Example usage
cls = ContinuousLearningSystem(brain, world, agent)

# Start continuous learning in background
cls.start_continuous_learning(episodes=20, steps_per_episode=50)

# Do other work while learning continues
for step in range(100):
    print(f"Main thread: Processing step {step}")
    time.sleep(0.1)

# Stop learning when done
cls.stop_continuous_learning()
```

---

## Neural Topology Analysis

### Analyze Brain Structure and Connectivity

Analyze the complex structure of neural networks:

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Run simulation to develop interesting structure
for step in range(500):
    brain.step(step)

# Perform comprehensive neural topology analysis
print("Performing neural topology analysis...")
pynlm.analyzeNeuralTopology(brain)

# Get detailed connectivity statistics
print("\nDetailed Connectivity Analysis:")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    neuron_count = region.getNeuronCount()
    population_count = region.getPopulationCount()
    synapse_count = region.getSynapseCount()
    
    print(f"  Region {region_id}:")
    print(f"    Neurons: {neuron_count}")
    print(f"    Populations: {population_count}")
    print(f"    Synapses: {synapse_count}")
    print(f"    Neurons per Population: {neuron_count / max(1, population_count):.1f}")
    print(f"    Synapses per Neuron: {synapse_count / max(1, neuron_count):.1f}")

# Analyze neuron type distribution
print("\nNeuron Type Distribution:")
spike_system = brain.getSpikeSystem()
if spike_system:
    populations = spike_system.getAllPopulations()
    neuron_type_counts = {}
    for pop in populations:
        neuron_type = pop.getNeuronType()
        if neuron_type not in neuron_type_counts:
            neuron_type_counts[neuron_type] = 0
        neuron_type_counts[neuron_type] += pop.getNeuronCount()
    
    for neuron_type, count in neuron_type_counts.items():
        print(f"  {neuron_type}: {count} neurons ({count/brain.getTotalNeuronCount()*100:.1f}%)")
```

---

## Custom Simulation Strategies

### Implement Custom Simulation Control Loops

Create custom simulation strategies with advanced control logic:

```python
import pynlm
from typing import List, Dict, Any

class CustomSimulationStrategy:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.strategy_history = []
        self.performance_metrics = {}
    
    def execute_custom_strategy(self, strategy_name: str, parameters: Dict[str, Any]):
        """Execute custom simulation strategy"""
        print(f"Executing custom strategy: {strategy_name}")
        
        if strategy_name == "curiosity_driven_exploration":
            return self._curiosity_driven_exploration(parameters)
        elif strategy_name == "reward_based_optimization":
            return self._reward_based_optimization(parameters)
        elif strategy_name == "developmentally_guided_learning":
            return self._developmentally_guided_learning(parameters)
        elif strategy_name == "adaptive_environment_interaction":
            return self._adaptive_environment_interaction(parameters)
        else:
            raise ValueError(f"Unknown strategy: {strategy_name}")
    
    def _curiosity_driven_exploration(self, parameters: Dict[str, Any]):
        """Strategy focused on curiosity-driven exploration"""
        exploration_radius = parameters.get("exploration_radius", 10.0)
        max_curiosity_threshold = parameters.get("max_curiosity_threshold", 0.8)
        min_reward_threshold = parameters.get("min_reward_threshold", 0.0)
        
        steps_completed = 0
        total_curiosity = 0.0
        successful_explorations = 0
        
        for step in range(parameters.get("max_steps", 1000)):
            # Check curiosity level
            curiosity = self.agent.getCuriosityLevel()
            total_curiosity += curiosity
            
            if curiosity < max_curiosity_threshold:
                # High curiosity: encourage exploration
                action = self._select_exploration_action(exploration_radius)
            elif curiosity > max_curiosity_threshold:
                # Low curiosity: encourage exploitation
                action = self._select_exploitation_action()
            else:
                # Balanced: use agent's normal decision making
                action = self.agent.decodeMotorCommand()
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check if exploration was successful
            if self._is_successful_exploration(action):
                successful_explorations += 1
            
            steps_completed += 1
            
            # Check stop conditions
            if self._should_stop_early(curiosity, min_reward_threshold):
                break
        
        # Calculate performance metrics
        avg_curiosity = total_curiosity / max(1, steps_completed)
        exploration_efficiency = successful_explorations / max(1, steps_completed)
        
        self.performance_metrics[strategy_name] = {
            'steps_completed': steps_completed,
            'average_curiosity': avg_curiosity,
            'exploration_efficiency': exploration_efficiency,
            'successful_explorations': successful_explorations
        }
        
        return self.performance_metrics[strategy_name]
    
    def _reward_based_optimization(self, parameters: Dict[str, Any]):
        """Strategy focused on reward-based optimization"""
        target_reward = parameters.get("target_reward", 1.0)
        exploration_rate = parameters.get("exploration_rate", 0.1)
        exploitation_rate = 1.0 - exploration_rate
        
        steps_completed = 0
        total_reward = 0.0
        rewards_above_target = 0
        
        for step in range(parameters.get("max_steps", 1000)):
            # Choose action based on reward strategy
            if self.agent.getRandomGenerator().uniformReal(0.0, 1.0) < exploration_rate:
                # Exploration: try new actions
                action = self._select_random_action()
            else:
                # Exploitation: use learned actions
                action = self.agent.decodeMotorCommand()
            
            # Execute action and get reward
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Simulate reward (in real implementation, this would come from environment)
            current_reward = self._calculate_current_reward(action)
            total_reward += current_reward
            
            if current_reward >= target_reward:
                rewards_above_target += 1
            
            steps_completed += 1
        
        # Calculate performance metrics
        avg_reward = total_reward / max(1, steps_completed)
        success_rate = rewards_above_target / max(1, steps_completed)
        
        self.performance_metrics[strategy_name] = {
            'steps_completed': steps_completed,
            'average_reward': avg_reward,
            'success_rate': success_rate,
            'rewards_above_target': rewards_above_target
        }
        
        return self.performance_metrics[strategy_name]
    
    def _select_exploration_action(self, exploration_radius: float):
        """Select action for exploration"""
        # Choose random movement within exploration radius
        angle = self.agent.getBrain().getRandomGenerator().uniformReal(0.0, 2 * 3.14159)
        distance = self.agent.getBrain().getRandomGenerator().uniformReal(0.0, exploration_radius)
        
        # Calculate target position
        current_x = self.world.getAgentBody().x
        current_y = self.world.getAgentBody().y
        target_x = current_x + distance * math.cos(angle)
        target_y = current_y + distance * math.sin(angle)
        
        # Check if target is valid
        if self.world.isValidPosition(target_x, target_y):
            return pynlm.ActionType.Interact  # In real implementation, this would move to target
        else:
            return pynlm.ActionType.Wait
    
    def _select_exploitation_action(self):
        """Select action for exploitation"""
        # Use agent's learned preferences
        return self.agent.decodeMotorCommand()
    
    def _is_successful_exploration(self, action):
        """Check if exploration was successful"""
        # Implementation depends on specific environment and action semantics
        return action.getType() != pynlm.ActionType.Wait
    
    def _should_stop_early(self, curiosity: float, min_reward_threshold: float):
        """Check if simulation should stop early"""
        # Example stop conditions
        if curiosity < 0.01:  # No curiosity left
            return True
        if curiosity > 0.99:  # Too much curiosity (over-exploration)
            return True
        return False
    
    def _select_random_action(self):
        """Select completely random action"""
        actions = list(pynlm.ActionType)
        return actions[self.agent.getBrain().getRandomGenerator().uniformInt(0, len(actions)-1)]
    
    def _calculate_current_reward(self, action):
        """Calculate current reward for action"""
        # Simple reward calculation for demonstration
        if action.getType() == pynlm.ActionType.Interact:
            return 1.0
        elif action.getType() == pynlm.ActionType.Wait:
            return 0.0
        else:
            return 0.1

# Example usage
strategy = CustomSimulationStrategy(brain, world, agent)

# Run curiosity-driven exploration
print("Running curiosity-driven exploration strategy...")
results1 = strategy.execute_custom_strategy(
    "curiosity_driven_exploration",
    {
        "exploration_radius": 15.0,
        "max_curiosity_threshold": 0.7,
        "min_reward_threshold": 0.0,
        "max_steps": 500
    }
)

print(f"Results: {results1}")

# Run reward-based optimization
print("\nRunning reward-based optimization strategy...")
results2 = strategy.execute_custom_strategy(
    "reward_based_optimization",
    {
        "target_reward": 1.5,
        "exploration_rate": 0.2,
        "max_steps": 500
    }
)

print(f"Results: {results2}")

# Display comprehensive results
print("\n=== Custom Strategy Results Summary ===")
for strategy_name, metrics in strategy.performance_metrics.items():
    print(f"\n{strategy_name}:")
    for metric, value in metrics.items():
        print(f"  {metric}: {value}")
```

---

## Integration with External Systems

### Connect NLM with External Frameworks and Tools

Integrate NLM with external systems for advanced applications:

```python
import pynlm
import numpy as np
import json
from typing import Dict, Any, Callable

class NLMExternalIntegration:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.external_systems = {}
        self.data_handlers = {}
    
    def register_external_system(self, name: str, system: Any, 
                                data_handler: Callable = None):
        """Register external system for integration"""
        self.external_systems[name] = system
        if data_handler:
            self.data_handlers[name] = data_handler
        print(f"Registered external system: {name}")
    
    def exchange_brain_data(self, target_system: str, data_format: str = "json"):
        """Exchange brain data with external system"""
        if target_system not in self.external_systems:
            raise ValueError(f"External system not registered: {target_system}")
        
        # Extract brain data
        brain_data = self._extract_brain_data()
        
        # Convert to requested format
        if data_format == "json":
            data = json.dumps(brain_data, indent=2)
        elif data_format == "numpy":
            data = self._convert_to_numpy(brain_data)
        elif data_format == "csv":
            data = self._convert_to_csv(brain_data)
        else:
            raise ValueError(f"Unsupported data format: {data_format}")
        
        # Send data to external system
        external_system = self.external_systems[target_system]
        if hasattr(external_system, 'receive_data'):
            external_system.receive_data(data, data_format)
        
        return brain_data
    
    def receive_external_data(self, source_system: str, data: Any, 
                             data_format: str = "json"):
        """Receive data from external system"""
        if source_system not in self.data_handlers:
            raise ValueError(f"No data handler registered for: {source_system}")
        
        # Process data using registered handler
        processed_data = self.data_handlers[source_system](data, data_format)
        
        # Apply external data to brain/world
        self._apply_external_data(processed_data, source_system)
        
        return processed_data
    
    def _extract_brain_data(self) -> Dict[str, Any]:
        """Extract comprehensive brain data"""
        return {
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'exc_inh_ratio': self.brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(self.brain.getDevelopmentalStage())
            },
            'agent_state': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled(),
                'development_enabled': self.agent.isDevelopmentEnabled(),
                'curiosity_enabled': self.agent.isCuriosityEnabled()
            },
            'world_state': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'vision_width': self.world.getVisionWidth(),
                'vision_height': self.world.getVisionHeight(),
                'simulation_time': self.world.getSimulationTime(),
                'max_energy': self.world.getMaxEnergy()
            },
            'performance_metrics': pynlm.analyzeBrainStats(self.brain)
        }
    
    def _convert_to_numpy(self, brain_data: Dict[str, Any]) -> Dict[str, Any]:
        """Convert brain data to numpy format"""
        numpy_data = {}
        for category, data in brain_data.items():
            if isinstance(data, dict):
                numpy_data[category] = {k: np.array(v) if isinstance(v, list) else v
                                       for k, v in data.items()}
            else:
                numpy_data[category] = np.array(data)
        return numpy_data
    
    def _convert_to_csv(self, brain_data: Dict[str, Any]) -> str:
        """Convert brain data to CSV format"""
        import pandas as pd
        
        # Flatten nested data for CSV export
        csv_data = {}
        for category, data in brain_data.items():
            if isinstance(data, dict):
                for key, value in data.items():
                    csv_data[f"{category}_{key}"] = value
            else:
                csv_data[category] = data
        
        df = pd.DataFrame([csv_data])
        return df.to_csv(index=False)
    
    def _apply_external_data(self, data: Any, source_system: str):
        """Apply external data to brain/world system"""
        # Example implementations for different data sources
        if source_system == "environment_monitor":
            self._apply_environment_data(data)
        elif source_system == "sensor_array":
            self._apply_sensor_data(data)
        elif source_system == "external_knowledge_base":
            self._apply_knowledge_data(data)
        elif source_system == "user_interface":
            self._apply_ui_data(data)
    
    def _apply_environment_data(self, env_data: Dict[str, Any]):
        """Apply environmental data"""
        # Update world based on environmental conditions
        if 'temperature' in env_data:
            # Example: Adjust neural excitability based on temperature
            temp_factor = 1.0 + (env_data['temperature'] - 20.0) * 0.01
            # This would modify brain parameters in real implementation
        
        if 'light_level' in env_data:
            # Example: Adjust sensory processing based on light
            # This would modify world properties in real implementation
            pass
    
    def _apply_sensor_data(self, sensor_data: Dict[str, Any]):
        """Apply sensor data"""
        # Process sensor inputs and feed to brain
        if 'vision_data' in sensor_data:
            vision_input = pynlm.createVisionInput(
                sensor_data['vision_width'],
                sensor_data['vision_height'],
                3,
                sensor_data['vision_data']
            )
            # Inject into brain
        
        if 'audio_data' in sensor_data:
            audio_input = pynlm.createAudioInput(
                sensor_data['sample_rate'],
                sensor_data['audio_data']
            )
            # Inject into brain
    
    def _apply_knowledge_data(self, knowledge_data: Dict[str, Any]):
        """Apply knowledge base data"""
        # Update brain configuration based on external knowledge
        if 'optimal_parameters' in knowledge_data:
            # Apply optimal neural parameters
            pass
        
        if 'learning_strategies' in knowledge_data:
            # Update learning strategies
            pass
    
    def _apply_ui_data(self, ui_data: Dict[str, Any]):
        """Apply user interface data"""
        # Update brain/world based on user interactions
        if 'user_preferences' in ui_data:
            # Update agent preferences
            pass
        
        if 'simulation_controls' in ui_data:
            # Update simulation controls
            pass

# Example usage
integration = NLMExternalIntegration(brain, world, agent)

# Register external systems
integration.register_external_system("environment_monitor", None,
                                    lambda data, fmt: {
                                        'temperature': data.get('temperature', 20.0),
                                        'humidity': data.get('humidity', 50.0)
                                    })

integration.register_external_system("sensor_array", None,
                                    lambda data, fmt: {
                                        'vision_data': data.get('vision', []),
                                        'audio_data': data.get('audio', [])
                                    })

integration.register_external_system("user_interface", None,
                                    lambda data, fmt: {
                                        'user_preferences': data.get('preferences', {}),
                                        'simulation_controls': data.get('controls', {})
                                    })

# Example data exchange
print("Exchanging data with external systems...")

# Send brain data to environment monitor
env_data = integration.exchange_brain_data("environment_monitor", "json")
print(f"Environment data: {json.dumps(env_data, indent=2)}")

# Send brain data to sensor array
sensor_data = integration.exchange_brain_data("sensor_array", "numpy")
print(f"Sensor data shape: {sensor_data}")

# Simulate receiving data from sensor array
print("\nSimulating external sensor input...")
new_sensor_data = {
    'vision_width': 32,
    'vision_height': 32,
    'vision_data': [0.5] * 1024,
    'sample_rate': 22050,
    'audio_data': [0.3, 0.7] * 50
}
integration.receive_external_data("sensor_array", new_sensor_data, "json")

# Simulate receiving data from user interface
print("\nSimulating external user interface input...")
ui_data = {
    'preferences': {
        'learning_rate': 0.01,
        'exploration_weight': 0.7
    },
    'controls': {
        'speed': 2.0,
        'complexity': 0.8
    }
}
integration.receive_external_data("user_interface", ui_data, "json")

print("\nExternal integration complete!")
```

---

## Advanced Python API Reference

This section provides detailed documentation for all advanced Python API functions and classes added to NLM.

### Advanced Configuration Functions

| Function | Description | Example |
|----------|-------------|---------|
| `configureDefaults()` | Create advanced default configuration | `config = pynlm.configureDefaults()` |
| `configureMinimal()` | Create minimal configuration for testing | `config = pynlm.configureMinimal()` |
| `configureSimulation()` | Create custom simulation configuration | `config = pynlm.configureSimulation(1000, 0.1, 0.05, 10000)` |

### Advanced Object Creation Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createWorldObject()` | Create world objects with parameters | `obj = pynlm.createWorldObject(1.0, 2.0, Resource, 1.0)` |
| `createResourceObject()` | Create resource objects | `res = pynlm.createResourceObject(5.0, 5.0, 2.5)` |
| `createHazardObject()` | Create hazard objects | `haz = pynlm.createHazardObject(10.0, 10.0, 1.0)` |
| `createWallObject()` | Create wall objects | `wall = pynlm.createWallObject(0.0, 0.0, 2.0, 3.0)` |
| `createVisionInput()` | Create vision sensory input | `vision = pynlm.createVisionInput(16, 16, 3, data)` |
| `createAudioInput()` | Create audio sensory input | `audio = pynlm.createAudioInput(44100, samples)` |
| `createInternalSignals()` | Create internal signals | `internal = pynlm.createInternalSignals([0.5, -0.3])` |
| `createAction()` | Create actions with parameters | `action = pynlm.createAction(MoveForward, [1.0, 0.5])` |
| `createMovementAction()` | Create movement actions | `move = pynlm.createMovementAction(MoveForward, 2.0, 45.0)` |
| `createInteractionAction()` | Create interaction actions | `interact = pynlm.createInteractionAction("door", "open")` |

### Agent Configuration Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createAdvancedAgent()` | Create advanced agent | `agent = pynlm.createAdvancedAgent(brain)` |
| `createAdvancedAgentWithLearning()` | Create agent with all learning features | `agent = pynlm.createAdvancedAgentWithLearning(brain, world)` |
| `configureAgentAdvanced()` | Configure agent with advanced settings | `configureAgentAdvanced(agent, True, True, True, True, 0.95)` |
| `createWorldAdvanced()` | Create advanced world | `world = pynlm.createWorldAdvanced(20, 20, 8, 8)` |

### Simulation Control Functions

| Function | Description | Example |
|----------|-------------|---------|
| `runSimulationAdvanced()` | Run advanced simulation with stop conditions | `runSimulationAdvanced(brain, world, agent, 1000, stop_condition)` |
| `createRandomSimulation()` | Create and run random simulation | `brain = pynlm.createRandomSimulation(config, 1000)` |
| `analyzeBrainStats()` | Analyze and return brain statistics | `stats = pynlm.analyzeBrainStats(brain)` |
| `printDetailedStats()` | Print detailed brain statistics | `pynlm.printDetailedStats(brain)` |
| `brainDebugReport()` | Generate comprehensive debug report | `pynlm.brainDebugReport(brain)` |

### Data Management Functions

| Function | Description | Example |
|----------|-------------|---------|
| `exportBrainData()` | Export brain data to file | `pynlm.exportBrainData(brain, "data.json")` |
| `saveConfigAdvanced()` | Save configuration to advanced format | `saveConfigAdvanced(config, "config.json")` |
| `loadConfigAdvanced()` | Load configuration from advanced format | `config = pynlm.loadConfigAdvanced("config.json")` |
| `backupBrainMemory()` | Create backup of brain memory | `backupBrainMemory(brain, "backup.bin")` |
| `restoreBrainMemory()` | Restore brain memory from backup | `restoreBrainMemory(brain, "backup.bin")` |

### Performance Monitoring Functions

| Function | Description | Example |
|----------|-------------|---------|
| `monitorBrainPerformance()` | Monitor brain performance with callback | `monitorBrainPerformance(brain, 100, callback)` |
| `profileBrainPerformance()` | Profile brain performance metrics | `avg_speed = profileBrainPerformance(brain, 1000)` |
| `analyzeNeuralTopology()` | Analyze neural topology | `pynlm.analyzeNeuralTopology(brain)` |

### Training Functions

| Function | Description | Example |
|----------|-------------|---------|
| `trainAgentAdvanced()` | Train agent with reward function | `agent = trainAgentAdvanced(brain, world, 10, 200, reward_func)` |
| `analyzeBrainStats()` | Analyze brain statistics during training | `stats = analyzeBrainStats(brain)` |

### Logger Classes

| Class | Description | Example |
|-------|-------------|---------|
| `Logger` | Global logger class | `logger = pynlm.Logger()` |
| `ConsoleLogger` | Console logger with colors | `console_logger = pynlm.ConsoleLogger(pynlm.LogLevel.Info)` |

### Additional Classes and Types

| Class/Type | Description |
|------------|-------------|
| `LogLevel` | Log level enumeration (Debug, Info, Warning, Error, Critical) |
| `WorldObjectType` | World object type enumeration |
| `ActionType` | Action type enumeration |
| `NeuronType` | Neuron type enumeration |
| `SensoryInput` | Base sensory input class |
| `Vision` | Vision sensory input class |
| `Audio` | Audio sensory input class |
| `InternalSignals` | Internal signals class |
| `Action` | Action representation class |
| `WorldObject` | World object representation class |
| `AgentBody` | Agent body state class |
| `ActionResult` | Action result class |
| `SensoryPercept` | Sensory percept data class |

---

## Quick Reference: Advanced NLM Usage Patterns

### Pattern 1: Advanced Simulation with Custom Control

```python
def advanced_simulation_example():
    # Create advanced components
    config = pynlm.configureAdvanced()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorldAdvanced(50, 50, 16, 16)
    world.reset()
    
    agent = pynlm.createAdvancedAgentWithLearning(brain, world)
    agent.initialize(world)
    
    # Custom stop condition
    def custom_stop_condition(step, percept):
        # Stop when agent has learned enough
        curiosity = agent.getCuriosityLevel()
        return step > 500 and curiosity < 0.1
    
    # Run advanced simulation
    results = pynlm.runSimulationAdvanced(
        brain, world, agent, 1000,
        stop_condition=custom_stop_condition
    )
    
    return results
```

### Pattern 2: Sophisticated Agent Training

```python
def sophisticated_training_example():
    # Setup training environment
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 12, 12)
    world.reset()
    
    agent = pynlm.createAdvancedAgent(brain)
    
    # Define custom reward function
    def custom_reward(percept, action, world):
        reward = 0.0
        
        # Resource collection reward
        if action.getType() == pynlm.ActionType.Interact:
            nearby = world.getNearbyObjects(world.getAgentBody().x, world.getAgentBody().y, 2.0)
            for obj in nearby:
                if obj.type == pynlm.WorldObjectType.Resource:
                    reward += obj.value * 2.0
        
        # Hazard avoidance penalty
        if action.getType() == pynlm.ActionType.Interact:
            nearby = world.getNearbyObjects(world.getAgentBody().x, world.getAgentBody().y, 2.0)
            for obj in nearby:
                if obj.type == pynlm.WorldObjectType.Hazard:
                    reward -= 5.0
        
        return reward
    
    # Train agent
    trained_agent = pynlm.trainAgentAdvanced(
        brain=brain,
        world=world,
        episodes=100,
        stepsPerEpisode=200,
        rewardFunction=custom_reward
    )
    
    return trained_agent
```

### Pattern 3: Advanced Performance Monitoring

```python
def performance_monitoring_example():
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create monitoring callback
    def performance_callback(step, firing_rate, active_neurons, firing_neurons):
        print(f"Step {step}: FR={firing_rate:.2f}, "
              f"Active={active_neurons}, "
              f"Firing={firing_neurons}")
    
    # Monitor performance
    pynlm.monitorBrainPerformance(brain, 200, performance_callback)
    
    # Generate performance profile
    avg_speed = pynlm.profileBrainPerformance(brain, 1000)
    print(f"Average performance: {avg_speed:.2f} steps/second")
    
    # Analyze neural topology
    pynlm.analyzeNeuralTopology(brain)
    
    # Generate comprehensive debug report
    pynlm.brainDebugReport(brain)
```

### Pattern 4: Advanced Data Management

```python
def advanced_data_management_example():
    # Create and run simulation
    brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    brain.initialize()
    
    # Run simulation to generate interesting data
    for step in range(1000):
        brain.step(step)
    
    # Export brain data for analysis
    if pynlm.exportBrainData(brain, "simulation_results.json"):
        print("Brain data exported successfully")
    
    # Create backup
    if pynlm.backupBrainMemory(brain, "brain_backup.bin"):
        print("Brain backup created successfully")
    
    # Restore from backup if needed
    if pynlm.restoreBrainMemory(brain, "brain_backup.bin"):
        print("Brain restored from backup successfully")
    
    return brain
```

### Pattern 5: Advanced External Integration

```python
def external_integration_example():
    # Create NLM integration system
    integration = NLMExternalIntegration(brain, world, agent)
    
    # Register external systems
    integration.register_external_system("environment_monitor", None,
                                        lambda data, fmt: {
                                            'temperature': data.get('temperature', 20.0),
                                            'humidity': data.get('humidity', 50.0)
                                        })
    
    integration.register_external_system("sensor_array", None,
                                        lambda data, fmt: {
                                            'vision_data': data.get('vision', []),
                                            'audio_data': data.get('audio', [])
                                        })
    
    # Exchange data with external systems
    env_data = integration.exchange_brain_data("environment_monitor", "json")
    sensor_data = integration.exchange_brain_data("sensor_array", "numpy")
    
    # Receive data from external systems
    integration.receive_external_data("sensor_array", sensor_data, "json")
    
    return integration
```

---

## Migration Guide

### From Basic to Advanced NLM Usage

This guide helps you migrate from basic NLM usage to advanced patterns:

#### Step 1: Basic to Enhanced Configuration

**Before:**
```python
# Basic configuration
config = pynlm.createDefaultConfig()
```

**After:**
```python
# Advanced configuration
config = pynlm.configureAdvanced()
```

#### Step 2: Basic to Advanced Object Creation

**Before:**
```python
# Basic world creation
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
```

**After:**
```python
# Advanced world creation
world = pynlm.createWorldAdvanced(50, 50, 16, 16)
```

#### Step 3: Basic to Advanced Agent Configuration

**Before:**
```python
# Basic agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

** After:**
```python
# Advanced agent
agent = pynlm.createAdvancedAgentWithLearning(brain, world)
agent.initialize(world)
```

#### Step 4: Basic to Advanced Simulation Control

**Before:**
```python
# Basic simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**After:**
```python
# Advanced simulation with monitoring
results = pynlm.runSimulationAdvanced(
    brain, world, agent, 1000,
    monitoring_interval=100,
    stop_condition=custom_stop_condition
)
```

#### Step 5: Basic to Advanced Data Management

**Before:**
```python
# Basic brain saving
brain.save("brain.bin")
```

**After:**
```python
# Advanced brain backup
pynlm.backupBrainMemory(brain, "advanced_backup.bin")
```

---

## Troubleshooting Advanced Usage

### Common Issues and Solutions

#### Issue 1: Advanced Functions Not Available

**Symptoms:** Advanced functions like `configureAdvanced()` or `runSimulationAdvanced()` are not found.

**Solutions:**
1. Ensure you're using the latest version of pynlm
2. Check that the module was imported correctly:
```python
import pynlm
# Verify all advanced functions are available
print(dir(pynlm))  # Should include advanced functions
```

#### Issue 2: Advanced Simulation Performance Issues

**Symptoms:** Advanced simulation runs slowly or consumes excessive resources.

**Solutions:**
1. Use `configureMinimal()` for faster testing
2. Reduce simulation steps with `stepsPerEpisode`
3. Use `configureSimulation(neuronCount, density, connectionProb, simulationSteps)` for optimized configurations
4. Enable performance monitoring to identify bottlenecks:
```python
avg_speed = pynlm.profileBrainPerformance(brain, 100)
```

#### Issue 3: Advanced Agent Training Issues

**Symptoms:** Advanced agent training fails or produces poor results.

**Solutions:**
1. Ensure all learning features are enabled:
```python
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```
2. Use custom reward functions to guide learning:
```python
def custom_reward(percept, action, world):
    # Implement task-specific reward logic
    return calculated_reward
```
3. Monitor training progress:
```python
trained_agent = pynlm.trainAgentAdvanced(brain, world, episodes=50, stepsPerEpisode=200)
```

#### Issue 4: Advanced Data Export/Import Issues

**Symptoms:** Advanced data export/import operations fail.

**Solutions:**
1. Check file permissions and disk space
2. Verify file paths are correct:
```python
pynlm.exportBrainData(brain, "path/to/data.json")
```
3. Use backup functions for critical data:
```python
pynlm.backupBrainMemory(brain, "backup/path/brain_backup.bin")
```

#### Issue 5: Advanced External Integration Issues

**Symptoms:** External system integration fails.

**Solutions:**
1. Ensure external systems are properly registered:
```python
integration.register_external_system("system_name", system_instance)
```
2. Verify data format compatibility:
```python
integration.exchange_brain_data("system_name", "json")
```
3. Check data handler implementations:
```python
def data_handler(data, format):
    # Process data appropriately
    return processed_data
```

### Advanced Error Handling

Implement comprehensive error handling for advanced NLM usage:

```python
import pynlm
import logging

# Setup logging for advanced debugging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger("nlm_advanced")

def safe_advanced_simulation():
    """Advanced simulation with comprehensive error handling"""
    try:
        # Create advanced components
        config = pynlm.configureAdvanced()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createWorldAdvanced(30, 30, 12, 12)
        world.reset()
        
        agent = pynlm.createAdvancedAgentWithLearning(brain, world)
        agent.initialize(world)
        
        # Run advanced simulation with error handling
        results = pynlm.runSimulationAdvanced(
            brain, world, agent, 1000,
            stop_condition=lambda step, percept: step > 500
        )
        
        return results
        
    except Exception as e:
        logger.error(f"Advanced simulation failed: {e}")
        # Attempt recovery
        if 'brain' in locals():
            try:
                pynlm.backupBrainMemory(brain, "error_recovery_backup.bin")
                print("Error recovery backup created")
            except Exception as backup_error:
                logger.error(f"Error recovery failed: {backup_error}")
        
        raise
```

---

## Conclusion

This advanced usage guide provides comprehensive documentation for working with NLM's advanced features. The enhanced Python API now includes:

1. **Sophisticated configuration presets** for different use cases
2. **Advanced object creation utilities** with precise control
3. **Enhanced simulation control** with custom stop conditions and monitoring
4. **Performance monitoring and debugging** tools for optimization
5. **Memory management and persistence** features for data safety
6. **Advanced training and learning** techniques for agent development
7. **Neural topology analysis** for understanding brain structure
8. **Custom simulation strategies** for specialized applications
9. **External system integration** for connecting with other frameworks
10. **Comprehensive error handling** for robust operation

These advanced features enable users to leverage the full power of the NLM framework for complex, real-world neural simulation and learning applications. The enhanced Python API provides both the flexibility of the underlying C++ implementation and the convenience of Python for rapid prototyping and deployment.

---

**Next Steps:**
1. Start with `configureAdvanced()` for most use cases
2. Use `runSimulationAdvanced()` for sophisticated simulations
3. Implement custom reward functions for specific learning tasks
4. Use `profileBrainPerformance()` to optimize performance
5. Consider external integration for multi-system applications

The NLM framework is now equipped with comprehensive advanced features that support complex neural simulation, learning, and analysis tasks across a wide range of applications.