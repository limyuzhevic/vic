# NLM Tutorials

This document provides comprehensive step-by-step tutorials for using NLM, from beginner to advanced levels. Each tutorial includes clear instructions, code examples, and expected outcomes.

## Table of Contents

1. [Beginner Tutorials](#beginner-tutorials)
   1.1 [Getting Started: Simple Brain Simulation](#11-getting-started-simple-brain-simulation)
   1.2 [Brain in a World](#12-brain-in-a-world)
   1.3 [Complete Agent Example](#13-complete-agent-example)
   1.4 [Basic Learning](#14-basic-learning)

2. [Intermediate Tutorials](#intermediate-tutorials)
   2.1 [Neural Development](#21-neural-development)
   2.2 [Sensory Processing](#22-sensory-processing)
   2.3 [Motor Control](#23-motor-control)
   2.4 [Reward Learning](#24-reward-learning)

3. [Advanced Tutorials](#advanced-tutorials)
   3.1 [Parameter Sweeps](#31-parameter-sweeps)
   3.2 [Batch Processing](#32-batch-processing)
   3.3 [Performance Monitoring](#33-performance-monitoring)
   3.4 [Plugin Development](#34-plugin-development)
   3.5 [Configuration Management](#35-configuration-management)

4. [Real-world Use Cases](#real-world-use-cases)
   4.1 [Exploration Behavior](#41-exploration-behavior)
   4.2 [Memory Formation](#42-memory-formation)
   4.3 [Decision Making](#43-decision-making)
   4.4 [Adaptive Control](#44-adaptive-control)

## Beginner Tutorials

### 1.1 Getting Started: Simple Brain Simulation

#### Objective
Create and run a basic brain simulation with NLM.

#### What You'll Learn
- How to create a brain in NLM
- How to initialize and step through a simulation
- How to read brain statistics

#### Materials
- NLM Python package (pynlm)
- Basic text editor

#### Step-by-Step Instructions

**Step 1: Setup Environment**
```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Print NLM version
print(f"NLM Python bindings version: {pynlm.__version__}")
```

**Step 2: Create a Basic Brain**
```python
# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure for a small brain
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial synapses: {brain.getTotalSynapseCount()}")
```

**Step 3: Run Basic Simulation**
```python
# Record initial state
firing_rates = []
steps = 100

# Run simulation
for step in range(steps):
    brain.step(step)
    
    # Record statistics every 10 steps
    if step % 10 == 0:
        firing_rate = brain.getAverageFiringRate()
        firing_rates.append(firing_rate)
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"avg rate: {firing_rate:.2f} Hz")

print(f"\nSimulation complete!")
print(f"Average firing rate: {np.mean(firing_rates):.2f} Hz")
print(f"Final firing rate: {firing_rates[-1]:.2f} Hz")
```

**Step 4: Visualize Results**
```python
# Create a simple plot of firing rates
plt.figure(figsize=(10, 6))
plt.plot(range(0, steps, 10), firing_rates, 'b-', linewidth=2, label='Average Firing Rate')
plt.xlabel('Simulation Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Brain Simulation: Firing Rate Over Time')
plt.grid(True, alpha=0.3)
plt.legend()
plt.savefig('firing_rate_plot.png', dpi=300, bbox_inches='tight')
plt.show()
```

**Expected Outcome**
- Brain should start with low firing activity
- Firing rates should stabilize over time
- Network should show emergent properties

**Files Created**
- `firing_rate_plot.png`: Visualization of firing rate over time

### 1.2 Brain in a World

#### Objective
Create a brain that interacts with and learns from a simple environment.

#### What You'll Learn
- How to create a simple world in NLM
- How sensory and motor systems work
- How to implement basic behavior

#### Materials
- NLM Python package
- Simple world implementation

#### Step-by-Step Instructions

**Step 1: Setup Environment**
```python
import pynlm
import numpy as np

print("=== NLM Brain-World Simulation ===")
```

**Step 2: Create Brain, World, and Agent**
```python
# Create brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()

# Set up agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent position: ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
```

**Step 3: Enable Learning Subsystems**
```python
# Enable all learning subsystems for this simulation
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("\nEnabled learning subsystems:")
print("- Reward modulation: Enabled")
print("- Structural plasticity: Enabled")
print("- Development: Enabled")
print("- Curiosity: Enabled")
```

**Step 4: Run Simulation Loop**
```python
# Simulation parameters
num_steps = 200
sensor_history = []
action_history = []
reward_history = []

# Run simulation
for step in range(num_steps):
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    sensor_history.append(percept)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    action_history.append(action)
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = agent.getCurrentReward()
    reward_history.append(reward)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 50 == 0:
        print(f"\nStep {step}:")
        print(f"  Agent position: ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
        print(f"  Reward: {reward:.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
```

**Step 5: Analyze Results**
```python
print("\n=== Simulation Results ===")
print(f"Total simulation steps: {num_steps}")
print(f"Total rewards received: {len([r for r in reward_history if r > 0])}")
print(f"Average curiosity: {np.mean([agent.getCuriosityLevel() for _ in range(num_steps)]):.3f}")
print(f"Final development stage: {brain.getDevelopmentalStage()}")

# Convert action counts to readable format
from enum import Enum
class ActionType(Enum):
    MoveForward = 0
    TurnLeft = 1
    TurnRight = 2
    Interact = 3

# Count actions
action_counts = {}
for action in action_history:
    action_type = ActionType(action).name
    action_counts[action_type] = action_counts.get(action_type, 0) + 1

print("\nAction distribution:")
for action_type, count in action_counts.items():
    print(f"  {action_type}: {count} times ({count/num_steps*100:.1f}%)")
```

**Expected Outcome**
- Brain should learn to navigate the environment
- Agent should explore and find rewards
- Learning subsystems should adapt over time

### 1.3 Complete Agent Example

#### Objective
Implement a complete agent that learns to perform a specific task.

#### What You'll Learn
- How to implement goal-directed behavior
- How to use reward signals effectively
- How to monitor and evaluate agent performance

#### Materials
- NLM Python package
- Performance monitoring tools

#### Step-by-Step Instructions

**Step 1: Design the Task**
```python
class NavigationTask:
    def __init__(self, world_size=15, goal_x=12.0, goal_y=12.0):
        self.world_size = world_size
        self.goal_x = goal_x
        self.goal_y = goal_y
        self.task_complete = False
        
    def get_distance_to_goal(self, x, y):
        return np.sqrt((x - self.goal_x)**2 + (y - self.goal_y)**2)
        
    def is_task_complete(self, x, y):
        distance = self.get_distance_to_goal(x, y)
        return distance < 1.0  # Within 1 unit of goal
```

**Step 2: Create and Configure the Agent**
```python
import pynlm
import numpy as np
import time

class NavigationAgent:
    def __init__(self, task):
        self.task = task
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.setup_agent()
        
    def setup_agent(self):
        # Configure world
        self.world.configure(
            width=self.task.world_size,
            height=self.task.world_size,
            visionWidth=6,
            visionHeight=6
        )
        self.world.reset()
        self.world.setAgentStart(2.0, 2.0)
        
        # Configure brain
        config = self.brain.getConfig()
        config.set("brain.neuron_count", 300)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
        # Initialize components
        self.brain.initialize()
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
    def run_episode(self, max_steps=100):
        episode_reward = 0.0
        steps_remaining = max_steps
        
        for step in range(max_steps):
            # Update world
            self.world.update(timestep=0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processes input
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate task reward
            x, y = self.world.getAgentPosition()
            distance = self.task.get_distance_to_goal(x, y)
            task_reward = max(0, 10.0 - distance)  # Higher reward near goal
            
            # Apply reward
            self.agent.applyRewardModulation(task_reward, 0.0)
            episode_reward += task_reward
            
            # Check if task complete
            if self.task.is_task_complete(x, y):
                self.task.task_complete = True
                print(f"Task completed at step {step}!")
                break
                
            # Update development
            self.agent.updateDevelopment(0.1)
            
            steps_remaining -= 1
            
        return episode_reward, step + 1
```

**Step 3: Create Navigation Task and Run Episodes**
```python
def run_navigation_training(num_episodes=20, max_steps=100):
    """Train an agent to navigate to a goal in a world."""
    
    # Create navigation task
    task = NavigationTask(world_size=15, goal_x=12.0, goal_y=12.0)
    
    # Create agent
    agent = NavigationAgent(task)
    
    # Training statistics
    episode_rewards = []
    episode_lengths = []
    completion_rates = []
    
    print("=== Navigation Task Training ===")
    print(f"World size: {task.world_size}x{task.world_size}")
    print(f"Goal position: ({task.goal_x}, {task.goal_y})")
    print(f"Training episodes: {num_episodes}")
    print()
    
    # Training loop
    for episode in range(num_episodes):
        # Run episode
        episode_reward, steps_taken = agent.run_episode(max_steps)
        
        # Track statistics
        episode_rewards.append(episode_reward)
        episode_lengths.append(steps_taken)
        completion_rates.append(1 if task.task_complete else 0)
        
        # Print episode summary
        print(f"Episode {episode + 1}/{num_episodes}:")
        print(f"  Reward: {episode_reward:.2f}")
        print(f"  Steps: {steps_taken}")
        print(f"  Completed: {'Yes' if task.task_complete else 'No'}")
        print(f"  Average reward per step: {episode_reward/steps_taken:.3f}")
        
        # Reset task for next episode
        task.task_complete = False
        task = NavigationTask(world_size=15, goal_x=12.0, goal_y=12.0)
        agent = NavigationAgent(task)
        
        print()
    
    # Calculate and print training statistics
    avg_reward = np.mean(episode_rewards)
    std_reward = np.std(episode_rewards)
    avg_length = np.mean(episode_lengths)
    completion_rate = np.mean(completion_rates)
    
    print("=== Training Statistics ===")
    print(f"Average reward per episode: {avg_reward:.2f} ± {std_reward:.2f}")
    print(f"Average episode length: {avg_length:.1f} steps")
    print(f"Task completion rate: {completion_rate*100:.1f}%")
    print(f"Best reward: {np.max(episode_rewards):.2f}")
    print(f"Worst reward: {np.min(episode_rewards):.2f}")
    
    return episode_rewards, episode_lengths, completion_rates

# Run training
episode_rewards, episode_lengths, completion_rates = run_navigation_training(
    num_episodes=10,
    max_steps=100
)
```

### 1.4 Basic Learning

#### Objective
Demonstrate basic learning mechanisms in NLM.

#### What You'll Learn
- How reward modulation works
- How curiosity-driven exploration functions
- How neural development occurs over time

#### Materials
- NLM Python package
- Understanding of basic neural concepts

#### Step-by-Step Instructions

**Step 1: Setup Learning Environment**
```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

print("=== Basic Learning Demonstration ===")
```

**Step 2: Create Brain with Learning Capabilities**
```python
# Create configuration optimized for learning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 150)
config.set("brain.v_thresh", -55.0)  # Lower threshold for easier firing
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

**Step 3: Implement Exploration Task**
```python
class ExplorationTask:
    def __init__(self, world_size=10):
        self.world_size = world_size
        self.target_found = False
        self.episode_rewards = []
        
    def get_environment_features(self):
        # Get current state features
        x, y = world.getAgentPosition()
        # Create simple feature vector
        features = [
            x / self.world_size,
            y / self.world_size,
            world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        ]
        return features
        
    def calculate_reward(self):
        # Simple reward: proximity to center
        x, y = world.getAgentPosition()
        center_x, center_y = self.world_size / 2, self.world_size / 2
        distance = np.sqrt((x - center_x)**2 + (y - center_y)**2)
        return max(0, 1.0 - distance / (self.world_size / 2))
```

**Step 4: Run Exploration Learning**
```python
# Initialize task
task = ExplorationTask(world_size=10)

# Learning parameters
num_episodes = 50
task_duration = 100

# Track learning progress
rewards = []
curiosity_levels = []
novelty_levels = []
development_stages = []

print("Starting exploration learning...")
print(f"Episodes: {num_episodes}, Steps per episode: {task_duration}")
print()

for episode in range(num_episodes):
    episode_reward = 0.0
    
    for step in range(task_duration):
        # Update world
        world.update(timestep=0.1)
        
        # Get state features
        features = task.get_environment_features()
        
        # Process sensory input
        agent.processSensoryInput(features)
        
        # Brain processes
        brain.step(step)
        
        # Choose action (simple exploration strategy)
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.7:  # Explore when curious
            action = 0  # Move forward
        else:  # Exploit when settled
            action = np.random.choice([0, 1, 2])  # Random direction
            
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        step_reward = task.calculate_reward()
        agent.applyRewardModulation(step_reward, 0.0)
        episode_reward += step_reward
        
        # Track learning
        rewards.append(episode_reward)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        development_stages.append(brain.getDevelopmentalStage())
        
        # Update development
        agent.updateDevelopment(0.1)
    
    task.episode_rewards.append(episode_reward)
    
    if (episode + 1) % 10 == 0:
        print(f"Episode {episode + 1}: Average reward = {np.mean(rewards[-task_duration:]):.3f}, "
              f"Curiosity = {np.mean(curiosity_levels[-task_duration:]):.3f}")
```

**Step 5: Analyze Learning Progress**
```python
print("\n=== Learning Analysis ===")

# Calculate statistics
final_reward = rewards[-1]
avg_reward = np.mean(rewards)
reward_improvement = final_reward - np.mean(rewards[:len(rewards)//2])

print(f"Final reward: {final_reward:.3f}")
print(f"Average reward: {avg_reward:.3f}")
print(f"Reward improvement: {reward_improvement:.3f}")
print(f"Final curiosity level: {curiosity_levels[-1]:.3f}")
print(f"Final novelty level: {novelty_levels[-1]:.3f}")
print(f"Final development stage: {development_stages[-1]}")

# Plot learning curves
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Reward curve
axes[0, 0].plot(rewards)
axes[0, 0].set_title('Episode Rewards')
axes[0, 0].set_xlabel('Episode')
axes[0, 0].set_ylabel('Reward')
axes[0, 0].grid(True, alpha=0.3)

# Curiosity curve
axes[0, 1].plot(curiosity_levels)
axes[0, 1].set_title('Curiosity Levels')
axes[0, 1].set_xlabel('Step')
axes[0, 1].set_ylabel('Curiosity')
axes[0, 1].grid(True, alpha=0.3)

# Novelty curve
axes[1, 0].plot(novelty_levels)
axes[1, 0].set_title('Novelty Levels')
axes[1, 0].set_xlabel('Step')
axes[1, 0].set_ylabel('Novelty')
axes[1, 0].grid(True, alpha=0.3)

# Development stages
unique_stages, counts = np.unique(development_stages, return_counts=True)
axes[1, 1].bar(unique_stages, counts)
axes[1, 1].set_title('Development Stage Distribution')
axes[1, 1].set_xlabel('Development Stage')
axes[1, 1].set_ylabel('Count')
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('learning_curves.png', dpi=300, bbox_inches='tight')
plt.show()

print("\nLearning curves saved to 'learning_curves.png'")
```

**Expected Outcome**
- Agent should learn to explore and find rewards
- Curiosity should increase with novel experiences
- Development stages should progress over time
- Network should show learning dynamics

## Intermediate Tutorials

### 2.1 Neural Development

This tutorial covers how neural development works in NLM, including synaptogenesis, pruning, and maturation.

### 2.2 Sensory Processing

Learn how sensory information is processed and represented in the brain.

### 2.3 Motor Control

Explore how motor commands are generated and executed.

### 2.4 Reward Learning

Understand how reward signals drive learning and behavior.

## Advanced Tutorials

### 3.1 Parameter Sweeps

Learn how to run parameter sweeps for experimental optimization.

### 3.2 Batch Processing

Implement batch processing for running multiple experiments efficiently.

### 3.3 Performance Monitoring

Set up real-time performance monitoring and visualization.

### 3.4 Plugin Development

Extend NLM with custom neural modules and plasticity rules.

### 3.5 Configuration Management

Master advanced configuration management and validation.

## Real-world Use Cases

### 4.1 Exploration Behavior

Implement exploration strategies for unknown environments.

### 4.2 Memory Formation

Create agents that learn and retain information.

### 4.3 Decision Making

Build agents that make optimal choices under uncertainty.

### 4.4 Adaptive Control

Develop systems that adapt to changing environments.

## Conclusion

These tutorials provide a comprehensive learning path for using NLM effectively. Start with the beginner tutorials to understand the basics, then progress to intermediate and advanced topics as your skills develop.

Each tutorial builds on previous knowledge, creating a solid foundation for neural network simulation and research.