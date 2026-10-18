# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

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

## Advanced Usage

When you're ready to go beyond the basics, here are more advanced patterns and features:

### Advanced Agent Behavior

```python
import pynlm

def create_advanced_agent(config_preset="aggressive_explorer"):
    """Create an agent with specialized behavior profile."""
    
    if config_preset == "aggressive_explorer":
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
    elif config_preset == "conservative_learner":
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0001)
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", False)
        
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent

# Create specialized agent
brain, agent = create_advanced_agent("aggressive_explorer")

# Advanced simulation with custom features
world = pynlm.createComplexWorld()
agent.initialize(world)

for episode in range(10):
    episode_reward = 0.0
    for step in range(200):
        world.update(0.05)  # Faster simulation
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Custom reward shaping
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        curiosity_bonus = agent.getCuriosityLevel() * 0.1
        episode_reward += reward + curiosity_bonus
        
        agent.applyRewardModulation(reward + curiosity_bonus, agent.getPredictionError())
        agent.updateDevelopment(0.05)
    
    print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}, "
          f"Firing rate = {brain.getAverageFiringRate():.3f} Hz")
```

### Performance Monitoring and Optimization

```python
import pynlm
import time

def performance_test(brain, world, agent, steps=1000):
    """Run performance test and return statistics."""
    
    # Warmup
    for i in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        agent.decodeMotorCommand()
    
    # Benchmark
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    end_time = time.time()
    
    # Collect statistics
    stats = pynlm.getSimulationStats(brain, world, agent)
    duration = end_time - start_time
    
    print(f"\n=== Performance Test Results ===")
    print(f"Duration: {duration:.2f} seconds")
    print(f"Steps per second: {steps/duration:.2f}")
    print(f"Firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    for key, value in stats.items():
        print(f"{key}: {value}")
    
    return {
        'duration': duration,
        'steps_per_second': steps/duration,
        'firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount()
    }

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
agent.initialize(world)

results = performance_test(brain, world, agent, steps=500)
```

### Advanced Configuration Management

```python
import pynlm
import json
import time

def save_experiment_config(config, base_name, experiment_id):
    """Save configuration with metadata for reproducibility."""
    config_path = f"configs/{base_name}_{experiment_id}.json"
    config.saveToFile(config_path)
    
    # Also save metadata
    metadata = {
        "experiment_id": experiment_id,
        "config_path": config_path,
        "timestamp": time.time(),
        "pynlm_version": pynlm.__version__
    }
    
    metadata_path = f"configs/{base_name}_{experiment_id}_meta.json"
    with open(metadata_path, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    return config_path, metadata_path

def load_experiment_config(base_name, experiment_id):
    """Load experiment configuration."""
    config_path = f"configs/{base_name}_{experiment_id}.json"
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_path)
    return config

# Example usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)

exp_id = "exp_001"
config_path, meta_path = save_experiment_config(config, "nlm_experiment", exp_id)
print(f"Configuration saved to {config_path}")

# Later...
loaded_config = load_experiment_config("nlm_experiment", exp_id)
brain = pynlm.createBrain(loaded_config)
```

### Advanced Learning and Plasticity Control

```python
import pynlm

def fine_tune_brain(brain, agent, world, steps=500):
    """Advanced fine-tuning with explicit plasticity control."""
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_weights = get_synapse_weights(brain)
    
    print("Starting fine-tuning...")
    
    for step in range(steps):
        world.update(0.1)
        
        # Step through sensory-motor loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Adaptive reward modulation based on prediction error
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        prediction_error = agent.getPredictionError()
        
        # Scale reward by prediction error (Bayesian updating)
        adaptive_reward = reward * (1.0 + abs(prediction_error))
        agent.applyRewardModulation(adaptive_reward, prediction_error)
        
        # Developmental updates
        if step % 50 == 0:
            agent.updateDevelopment(0.1)
            print(f"Step {step}: Dev stage = {brain.getDevelopmentalStage()}")
    
    # Analyze learning outcomes
    final_spikes = brain.getTotalSpikeCount()
    final_weights = get_synapse_weights(brain)
    
    print(f"\nLearning Analysis:")
    print(f"Total spike increase: {final_spikes - initial_spikes}")
    print(f"Average weight change: {calculate_weight_change(initial_weights, final_weights):.4f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return {
        'spike_increase': final_spikes - initial_spikes,
        'weight_change': calculate_weight_change(initial_weights, final_weights)
    }

def get_synapse_weights(brain):
    """Extract synapse weights for analysis."""
    weights = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            weights.append(synapse.getWeight())
    return weights

def calculate_weight_change(initial, final):
    """Calculate average weight change."""
    if len(initial) == 0 or len(final) == 0:
        return 0.0
    
    total_change = 0.0
    count = 0
    for i in range(min(len(initial), len(final))):
        total_change += final[i] - initial[i]
        count += 1
    
    return total_change / count if count > 0 else 0.0
```

### Multi-Agent Coordination

```python
import pynlm

def create_multi_agent_team(config):
    """Create a team of collaborating agents."""
    
    # Team leader with higher capacity
    leader_config = config.copy() if hasattr(config, 'copy') else config
    leader_config.set("brain.neuron_count", config.getOr<int64_t>("brain.neuron_count", 1000) * 2)
    leader_config.set("neuromod.curiosity.enable", True)
    
    # Team members with specialized roles
    member_configs = []
    for i in range(3):
        member_config = config.copy() if hasattr(config, 'copy') else config
        member_config.set("brain.neuron_count", config.getOr<int64_t>("brain.neuron_count", 1000) // 2)
        member_config.set("neuromod.curiosity.enable", False)
        member_configs.append(member_config)
    
    # Create team members
    team_members = []
    for member_config in member_configs:
        member_brain = pynlm.createBrain(member_config)
        member_brain.initialize()
        member_agent = pynlm.createAgentBrain(member_brain)
        team_members.append((member_brain, member_agent))
    
    # Create leader
    leader_brain = pynlm.createBrain(leader_config)
    leader_brain.initialize()
    leader_agent = pynlm.createAgentBrain(leader_brain)
    
    return leader_brain, leader_agent, team_members

def collaborative_learning(leader_brain, leader_agent, team_members, world, steps=300):
    """Train team with collaborative learning."""
    
    for step in range(steps):
        # Leader processes first (guidance)
        world.update(0.1)
        percept = world.getSensoryPercept()
        leader_agent.processSensoryInput(percept)
        leader_brain.step(step)
        
        # Team members exchange information (simplified)
        for member_brain, member_agent in team_members:
            member_agent.processSensoryInput(percept)  # Share perception
            member_brain.step(step)
        
        # Leader makes coordinated decision
        leader_action = leader_agent.decodeMotorCommand()
        world.applyMotorCommand(leader_action, world.getSimulationTime())
        
        # All agents learn from team outcome
        team_reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Leader receives full reward
        leader_agent.applyRewardModulation(team_reward, 0.0)
        
        # Team members receive scaled reward (collaborative learning)
        scaled_reward = team_reward * 0.3  # Distributed reward
        for member_brain, member_agent in team_members:
            member_agent.applyRewardModulation(scaled_reward, 0.0)
        
        if step % 50 == 0:
            print(f"Step {step}: Team reward = {team_reward:.3f}")
    
    return {
        'leader_spikes': leader_brain.getTotalSpikeCount(),
        'team_spikes': sum(m[0].getTotalSpikeCount() for m in team_members)
    }
```

### Production-Ready Simulation Wrapper

```python
import pynlm
import json
import time
from typing import Dict, Any, Callable

class NLMSimulation:
    """Production-ready simulation wrapper with advanced features."""
    
    def __init__(self, config=None, enable_monitoring=True):
        self.config = config or pynlm.createDefaultConfig()
        self.enable_monitoring = enable_monitoring
        self.brain = None
        self.agent = None
        self.world = None
        self.step_count = 0
        self.episode_reward = 0.0
        self.training_metrics = {
            'spike_counts': [],
            'reward_history': [],
            'firing_rates': []
        }
        
    def setup(self, world_size=30, vision_size=(12, 12)):
        """Initialize simulation components."""
        # Create and initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(world_size, world_size, vision_size[0], vision_size[1])
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        if self.enable_monitoring:
            print("Simulation initialized successfully")
            self.log_setup_info()
        
        return self
    
    def log_setup_info(self):
        """Log simulation setup information."""
        stats = pynlm.getSimulationStats(self.brain, self.world, self.agent)
        print("\n=== Simulation Setup ===")
        print(f"Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"Vision: {stats.get('Brain: Total Neurons', '?')} neurons, "
              f"E/I ratio: {stats.get('Brain: E/I Ratio', '?')}")
    
    def step(self, callback: Callable = None):
        """Run one simulation step with optional callback."""
        # Update world
        self.world.update(0.1)
        
        # Get perception
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain computation
        self.brain.step(self.step_count)
        
        # Decision making
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        prediction_error = self.agent.getPredictionError()
        self.agent.applyRewardModulation(reward, prediction_error)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Collect metrics
        self.episode_reward += reward
        self.training_metrics['spike_counts'].append(self.brain.getTotalSpikeCount())
        self.training_metrics['reward_history'].append(reward)
        self.training_metrics['firing_rates'].append(self.brain.getAverageFiringRate())
        
        self.step_count += 1
        
        # Call user callback if provided
        if callback:
            callback(self)
        
        return {
            'step': self.step_count - 1,
            'reward': reward,
            'prediction_error': prediction_error,
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'dev_stage': self.brain.getDevelopmentalStage()
        }
    
    def run_episode(self, steps, callback=None):
        """Run complete episode."""
        episode_start = self.step_count
        
        for i in range(steps):
            result = self.step(callback)
            
            if self.enable_monitoring and (i % 50 == 0):
                print(f"Step {episode_start + i}: "
                      f"Reward = {result['reward']:.3f}, "
                      f"Firing = {result['firing_rate']:.3f} Hz")
        
        episode_end = self.step_count
        total_steps = episode_end - episode_start
        
        episode_result = {
            'steps_completed': total_steps,
            'total_reward': self.episode_reward,
            'average_reward': self.episode_reward / total_steps if total_steps > 0 else 0,
            'final_spike_count': self.brain.getTotalSpikeCount(),
            'spike_increase': self.brain.getTotalSpikeCount() - (self.training_metrics['spike_counts'][0] if self.training_metrics['spike_counts'] else 0)
        }
        
        if self.enable_monitoring:
            print(f"\n=== Episode Complete ===")
            print(f"Steps: {total_steps}")
            print(f"Total Reward: {episode_result['total_reward']:.2f}")
            print(f"Average Reward: {episode_result['average_reward']:.4f}")
        
        return episode_result
    
    def save_checkpoint(self, filepath):
        """Save simulation state."""
        if self.brain:
            self.brain.save(filepath)
            print(f"Checkpoint saved to {filepath}")
            return True
        return False
    
    def load_checkpoint(self, filepath):
        """Load simulation state."""
        if self.brain:
            if self.brain.load(filepath):
                print(f"Checkpoint loaded from {filepath}")
                return True
        return False
    
    def get_advanced_stats(self):
        """Get comprehensive simulation statistics."""
        if self.brain and self.agent and self.world:
            return pynlm.getSimulationStats(self.brain, self.world, self.agent)
        return None
    
    def reset(self):
        """Reset simulation state."""
        if self.world:
            self.world.reset()
        if self.brain:
            self.brain.reset()
            self.brain.initialize()
            self.agent.reset()
        
        self.step_count = 0
        self.episode_reward = 0.0
        
        # Reset metrics
        for key in self.training_metrics:
            self.training_metrics[key] = []
        
        if self.enable_monitoring:
            print("Simulation reset")
        
        return self

# Production example
if __name__ == "__main__":
    # Create simulation with monitoring
    sim = NLMSimulation(enable_monitoring=True)
    
    # Setup
    sim.setup(world_size=40, vision_size=(16, 16))
    
    # Run training episodes
    for episode in range(5):
        print(f"\n=== Training Episode {episode + 1} ===")
        
        # Define custom callback for advanced monitoring
        def monitoring_callback(simulation):
            if simulation.step_count % 100 == 0:
                stats = simulation.get_advanced_stats()
                if stats:
                    print(f"  Step {simulation.step_count}: "
                          f"Neurons={stats.get('Brain: Firing Neurons', '?')}, "
                          f"Curiosity={stats.get('Agent: Curiosity Level', '?')}")
        
        # Run episode
        results = sim.run_episode(200, monitoring_callback)
        
        # Save checkpoint every few episodes
        if episode % 2 == 0:
            sim.save_checkpoint(f"checkpoints/episode_{episode}_sim.pkl")
    
    print("\n=== Training Complete ===")
    print(f"Total steps: {sim.step_count}")
    print(f"Final developmental stage: {sim.brain.getDevelopmentalStage()}")
```

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more technical details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM and its advanced features.
