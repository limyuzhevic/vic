# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Advanced Usage Overview

This guide starts with basic usage and progresses to advanced features. For the most comprehensive documentation, also read:

- `HOW_TO_USE.md` - Detailed Python API usage
- `docs/ARCHITECTURE.md` - System architecture
- `INTEGATION.md` - How to connect systems
- `PERFORMANCE.md` - Parameter tuning
- `TROUBLESHOOTING.md` - Common problems
- `PYTHON_ADVANCED.md` - Advanced Python usage

## Basic Structure

The three main components work together in a closed-loop system:

1. **Brain** - Neural processing with LIF neurons, plasticity, memory
2. **World** - Simulated environment with sensory input and motor output  
3. **Agent** - Interface that connects brain to world

## Advanced Features

### Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.08)
config.set("brain.v_thresh", -55.0)
config.set("brain.tau_mem", 30.0)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.0005)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.noise_level", 0.2)
config.set("neuromod.novelty.enable", True)

# Configure memory systems
config.set("working.memory.capacity", 300)
config.set("working.memory.decay_rate", 0.02)
```

### Advanced Memory Systems

```python
# Create agent with memory management
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced memory features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Access and manipulate memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    working_memory.setCapacity(500)
    working_memory.setDecayRate(0.01)
    
    # Store and retrieve patterns
    pattern = np.random.randn(100)
    working_memory.store(pattern, 1.0)
    
    # Competition for memory access
    working_memory.runCompetition()
    
    # Get memory statistics
    active_traces = working_memory.getActiveTraces()
    memory_activity = working_memory.getMemoryActivity()
```

### Advanced Neuromodulation

```python
# Create custom neuromodulation
class CustomNeuromodulator:
    def __init__(self):
        self.level = 0.5
        self.target = 0.5
        self.time_constant = 100.0
    
    def update(self, dt):
        # Smooth transition toward target
        self.level += (self.target - self.level) * dt / self.time_constant
    
    def set_target(self, target):
        self.target = target

# Apply custom neuromodulation
neuromod = CustomNeuromodulator()
brain.applyNeuromodulation(neuromod)
```

### Development System

```python
# Advanced development control
brain.initializeDevelopment()

# Set development stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Monitor development progress
dev_stage = brain.getDevelopmentalStage()
stage_progress = brain.getStageProgress()
critical_period = brain.isCriticalPeriodActive()

# Force development transitions
brain.startNextStage()
```

### Performance Monitoring

```python
# Create performance profiler
profiler = pynlm.createPerformanceProfiler()
profiler.startProfiling()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    
    # Log performance metrics periodically
    if step % 100 == 0:
        metrics = profiler.getPerformanceMetrics()
        print(f"Step {step}: {metrics}")
        
profiler.stopProfiling()
final_metrics = profiler.getPerformanceMetrics()
print(f"Final performance: {final_metrics}")
```

### Checkpoint and Persistence

```python
# Create checkpoint manager
checkpoint_manager = pynlm.createCheckpointManager()
checkpoint_manager.setAutoSaveInterval(100)

# Save brain state
checkpoint_manager.saveCheckpoint("simulation_1", brain)

# Load brain state later
checkpoint_manager.loadCheckpoint("simulation_1", brain)

# Manage checkpoints
checkpoints = checkpoint_manager.getCheckpointList()
for checkpoint_name in checkpoints:
    info = checkpoint_manager.getCheckpointInfo(checkpoint_name)
    print(f"{checkpoint_name}: {info}")
```

### Advanced Training Techniques

```python
class AdvancedTrainer:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def run_training_session(self, num_episodes, curriculum=None):
        """Run advanced training with curriculum or meta-learning."""
        
        episode_rewards = []
        
        for episode in range(num_episodes):
            # Curriculum learning or meta-task sampling
            task_config = self.sample_task() if curriculum else None
            
            # Run episode
            episode_reward = self.run_episode(task_config)
            episode_rewards.append(episode_reward)
            
            # Adaptive learning rate
            self.adapt_learning_rate(episode_reward)
            
            # Print progress
            if episode % 50 == 0:
                avg_reward = np.mean(episode_rewards[-10:])
                print(f"Episode {episode}: Avg reward (last 10) = {avg_reward:.3f}")
        
        return episode_rewards
    
    def sample_task(self):
        """Sample meta-task for meta-learning."""
        
        task_types = ['exploration', 'exploitation', 'memory', 'prediction']
        task_type = np.random.choice(task_types)
        
        # Create task-specific configuration
        if task_type == 'exploration':
            return {
                'world_config': {'visionWidth': 16, 'visionHeight': 16},
                'target_objective': 'maximize_curiosity'
            }
        elif task_type == 'exploitation':
            return {
                'world_config': {'visionWidth': 8, 'visionHeight': 8},
                'target_objective': 'maximize_reward'
            }
        
        return None
```

### Error Handling and Validation

```python
import traceback
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("nlm_advanced")

def safe_simulation(brain, world, agent, steps):
    """Run simulation with comprehensive error handling."""
    
    try:
        # Validate components
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        if not agent.initialize(world):
            raise RuntimeError("Agent initialization failed")
        
        # Run simulation
        for step in range(steps):
            try:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            except Exception as e:
                logger.error(f"Step {step} failed: {e}")
                # Continue or break based on error severity
                continue
        
        return True
        
    except Exception as e:
        logger.error(f"Simulation failed: {e}")
        logger.debug(traceback.format_exc())
        return False
    
    finally:
        # Cleanup
        if hasattr(world, 'cleanup'):
            world.cleanup()
        if hasattr(agent, 'cleanup'):
            agent.cleanup()
```

### Advanced Python Integration

```python
# Integrate with external libraries
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def advanced_analysis(brain, world, agent, steps):
    """Advanced analysis of brain performance."""
    
    # Collect data
    data = {
        'spike_count': [],
        'firing_rate': [],
        'reward': [],
        'curiosity': [],
        'novelty': []
    }
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Collect metrics
        data['spike_count'].append(brain.getTotalSpikeCount())
        data['firing_rate'].append(brain.getAverageFiringRate())
        data['reward'].append(getattr(world, 'last_reward', 0.0))
        data['curiosity'].append(agent.getCuriosityLevel())
        data['novelty'].append(agent.getNoveltyLevel())
    
    # Convert to DataFrame for analysis
    df = pd.DataFrame(data)
    
    # Generate visualizations
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Spike count over time
    axes[0, 0].plot(df['spike_count'].rolling(window=10).mean())
    axes[0, 0].set_title('Cumulative Spike Count (smoothed)')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Spike Count')
    
    # Firing rate over time
    axes[0, 1].plot(df['firing_rate'])
    axes[0, 1].set_title('Firing Rate Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Firing Rate')
    
    # Reward and curiosity correlation
    axes[1, 0].scatter(df['reward'], df['curiosity'])
    axes[1, 0].set_title('Reward vs Curiosity')
    axes[1, 0].set_xlabel('Reward')
    axes[1, 0].set_ylabel('Curiosity Level')
    
    # Novelty over time
    axes[1, 1].plot(df['novelty'])
    axes[1, 1].set_title('Novelty Detection Over Time')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Novelty Level')
    
    plt.tight_layout()
    plt.savefig('advanced_analysis.png')
    plt.show()
    
    # Statistical analysis
    print(f"\n=== Advanced Analysis ===")
    print(f"Spike count - Mean: {df['spike_count'].mean():.1f}, Std: {df['spike_count'].std():.1f}")
    print(f"Firing rate - Mean: {df['firing_rate'].mean():.3f}, Std: {df['firing_rate'].std():.3f}")
    print(f"Reward - Mean: {df['reward'].mean():.3f}, Max: {df['reward'].max():.3f}")
    print(f"Curiosity - Mean: {df['curiosity'].mean():.3f}, Max: {df['curiosity'].max():.3f}")
    print(f"Novelty - Mean: {df['novelty'].mean():.3f}, Max: {df['novelty'].max():.3f}")
    
    return df
```

## Common Advanced Patterns

### Pattern 4: Multi-Agent Training

```python
class MultiAgentTrainer:
    """Train multiple agents with shared brain."""
    
    def __init__(self, brain):
        self.brain = brain
        self.agents = []
        self.worlds = []
    
    def add_agent(self, agent, world):
        """Add agent and world to trainer."""
        self.agents.append(agent)
        self.worlds.append(world)
        agent.initialize(world)
    
    def run_multi_agent_training(self, num_episodes):
        """Run training with multiple agents."""
        
        for episode in range(num_episodes):
            # Train each agent
            for i, (agent, world) in enumerate(zip(self.agents, self.worlds)):
                self.train_agent_agent(agent, world, episode)
            
            # Exchange experiences between agents
            self.exchange_experiences()
    
    def train_agent_agent(self, agent, world, episode):
        """Train single agent."""
        
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            self.brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    def exchange_experiences(self):
        """Exchange experiences between agents."""
        
        # Collect experiences from all agents
        experiences = []
        for agent in self.agents:
            experiences.extend(agent.get_experiences())
        
        # Share experiences (simplified)
        for agent in self.agents:
            agent.receive_experiences(experiences)
```

### Pattern 5: Hierarchical Task Learning

```python
class HierarchicalTaskLearner:
    """Learn tasks hierarchically."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.task_hierarchy = self.build_task_hierarchy()
    
    def build_task_hierarchy(self):
        """Build hierarchical task structure."""
        
        hierarchy = {
            'level_1': [
                {'name': 'basic_navigation', 'difficulty': 1},
                {'name': 'object_interaction', 'difficulty': 2},
                {'name': 'goal_achievement', 'difficulty': 3}
            ],
            'level_2': [
                {'name': 'complex_navigation', 'difficulty': 4},
                {'name': 'multi_step_tasks', 'difficulty': 5},
                {'name': 'strategy_planning', 'difficulty': 6}
            ]
        }
        
        return hierarchy
    
    def run_hierarchical_training(self, num_epochs):
        """Run hierarchical task learning."""
        
        for epoch in range(num_epochs):
            # Start from appropriate difficulty level
            current_difficulty = self.calculate_current_difficulty(epoch)
            
            # Select tasks based on difficulty
            tasks = self.select_tasks(current_difficulty)
            
            # Train on selected tasks
            for task in tasks:
                self.train_on_task(task, epoch)
    
    def train_on_task(self, task, epoch):
        """Train on specific task."""
        
        task_rewards = []
        
        for step in range(task['steps_per_epoch']):
            # Execute task-specific behavior
            action = self.execute_task_action(task)
            
            # Get task-specific reward
            reward = self.get_task_reward(task, action)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            task_rewards.append(reward)
        
        # Update task difficulty based on performance
        task_performance = np.mean(task_rewards)
        task['difficulty'] = self.adjust_task_difficulty(task, task_performance)
    
    def execute_task_action(self, task):
        """Execute action for specific task."""
        
        # Get action from brain
        self.brain.step(0)  # Simplified
        action = self.agent.decodeMotorCommand()
        
        return action
```

## Advanced Configuration Templates

### Template 1: Research Configuration

```python
research_config = pynlm.createDefaultConfig()

# Optimized for research experiments
research_config.set("brain.neuron_count", 5000)
research_config.set("brain.synapse_density", 0.05)
research_config.set("plasticity.stdp.learning_rate", 0.001)

# Enable all subsystems for comprehensive study
research_config.set("neuromod.curiosity.enable", True)
research_config.set("neuromod.curiosity.noise_level", 0.1)
research_config.set("neuromod.novelty.enable", True)
research_config.set("plasticity.structural.enable", True)

# Enhanced memory for research
research_config.set("working.memory.capacity", 1000)
research_config.set("working.memory.decay_rate", 0.005)
```

### Template 2: Performance Configuration

```python
performance_config = pynlm.createDefaultConfig()

# Optimized for speed
performance_config.set("brain.neuron_count", 1000)
performance_config.set("brain.synapse_density", 0.02)
performance_config.set("brain.tau_mem", 10.0)
performance_config.set("brain.tau_ref", 1.0)

# Simplified for speed
performance_config.set("plasticity.stdp.enable", True)
performance_config.set("plasticity.hebbian.enable", False)
performance_config.set("plasticity.structural.enable", False)
performance_config.set("neuromod.curiosity.enable", False)
performance_config.set("neuromod.novelty.enable", False)

# Minimal memory for speed
performance_config.set("working.memory.capacity", 100)
performance_config.set("working.memory.decay_rate", 0.1)
```

### Template 3: Developmental Configuration

```python
developmental_config = pynlm.createDefaultConfig()

# Support developmental processes
developmental_config.set("brain.neuron_count", 3000)
developmental_config.set("brain.synapse_density", 0.06)

# Enable developmental effects
developmental_config.set("plasticity.stdp.enable", True)
developmental_config.set("plasticity.structural.enable", True)

# Strong neuromodulation for development
developmental_config.set("neuromod.curiosity.enable", True)
developmental_config.set("neuromod.curiosity.noise_level", 0.3)
developmental_config.set("neuromod.novelty.enable", True)

# Rich memory for development
developmental_config.set("working.memory.capacity", 500)
developmental_config.set("working.memory.decay_rate", 0.01)
```

## Error Recovery and Resilience

### Recovery Strategies

```python
def resilient_simulation(brain, world, agent, steps):
    """Run simulation with recovery strategies."""
    
    recovery_strategies = {
        'brain_reset': lambda: brain.reset() and brain.initialize(),
        'agent_reinit': lambda: agent.initialize(world),
        'world_reset': lambda: world.reset(),
        'memory_clear': lambda: brain.getWorkingMemory().clear() if brain.getWorkingMemory() else None
    }
    
    for step in range(steps):
        try:
            # Try to run step
            run_step(step, brain, world, agent)
            
        except Exception as e:
            # Determine appropriate recovery strategy
            recovery_action = select_recovery_strategy(e, step)
            
            if recovery_action:
                print(f"Recovery: {recovery_action.__name__}")
                recovery_action()
                
                # Retry step after recovery
                if step > 0:
                    run_step(step - 1, brain, world, agent)
            else:
                # Critical error, stop simulation
                print(f"Critical error at step {step}: {e}")
                break
    
    return True

def select_recovery_strategy(error, step):
    """Select appropriate recovery strategy."""
    
    error_type = type(error).__name__
    
    if error_type in ['MemoryError', 'RuntimeError']:
        return recovery_strategies['memory_clear']
    elif error_type == 'ValueError':
        return recovery_strategies['brain_reset']
    elif error_type == 'AttributeError':
        return recovery_strategies['agent_reinit']
    elif error_type == 'KeyError':
        return recovery_strategies['world_reset']
    
    return None
```

## Summary

This enhanced `easy_usage.md` provides:

1. **Advanced configuration** options for customizing brain behavior
2. **Advanced memory system** manipulation and control
3. **Neuromodulation** techniques for custom control
4. **Development system** integration and monitoring
5. **Performance monitoring** and profiling tools
6. **Checkpoint and persistence** for long-running simulations
7. **Advanced training** techniques (curriculum learning, meta-learning)
8. **Error handling** and recovery strategies
9. **Advanced Python integration** with external libraries
10. **Common advanced patterns** for complex applications

This guide serves as a bridge from basic usage to professional-level NLM development and research.

**Next Steps:**

For even more advanced usage, refer to:
- `HOW_TO_USE.md` - Complete Python API documentation
- `PYTHON_ADVANCED.md` - Advanced Python techniques
- `INTEGATION.md` - System integration details
- `PERFORMANCE.md` - Performance optimization
- `TROUBLESHOOTING.md` - Advanced problem solving

That's it! You're now ready for advanced NLM usage. Good luck with your brain simulation projects!

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

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

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
