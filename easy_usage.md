# NLM - Comprehensive Usage Guide for All Skill Levels

## Overview

NLM (熙然, meaning "serene flow") is a brain simulator that lets you create virtual brains capable of learning, memory, and decision-making. This guide covers everything from basic usage to advanced techniques for building complex artificial cognitive systems.

---

## Quick Start

### Installation & Setup
```bash
# Clone the repository
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run demo
./nlm_phase6_demo
```

### Python Bindings
```python
# If using Python bindings
import pynlm

# Create a basic brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simple test - let the brain process for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain processed {brain.getTotalSpikeCount()} spikes")
```

---

## The 3 Core Components

### 1. Brain
The virtual neural substrate that thinks, learns, and stores memories.
- Contains neurons, synapses, and neural populations
- Implements LIF dynamics, plasticity, and neuromodulation
- Manages working memory, episodic memory, and conceptual knowledge

### 2. World
The environment where the brain operates and learns.
- Provides sensory inputs (vision, touch, internal states)
- Handles motor outputs and action execution
- Manages objects, resources, and hazards
- Tracks time and provides feedback

### 3. Agent
The interface that connects brain to world.
- Processes sensory inputs and converts to neural signals
- Decodes neural activity into motor commands
- Applies reward modulation and learning
- Manages curiosity and exploration behavior

---

## Basic Examples

### Example 1: The Simplest Brain
```python
import pynlm

# Create brain with default configuration (500 neurons)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize - this sets up neurons and connections
brain.initialize()

# Run brain processing for 100 time steps
for i in range(100):
    brain.step(i)  # Each step simulates neural dynamics

print(f"✓ Brain active with {brain.getTotalSpikeCount()} spikes")
print(f"✓ {brain.getFiringNeuronCount()} neurons currently firing")
print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Basic Agent-WORLD Interaction
```python
import pynlm

# Setup the components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run the agent-world loop
for step in range(50):
    # 1. Update the world (time passes)
    world.update(0.1)
    
    # 2. Get sensory perception from world
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # 4. Brain processes and computes
    brain.step(step)
    
    # 5. Decode motor command from brain's motor neurons
    action = agent.decodeMotorCommand()
    
    # 6. Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("✓ Simulation completed successfully")
print(f"✓ Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

---

## Advanced Usage

### Advanced Example 1: Learning Agent with Reward Modulation
```python
import pynlm

class LearningAgent:
    def __init__(self, neuron_count=1000, enable_curiosity=True):
        # Create custom configuration for advanced control
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", neuron_count)
        self.config.set("connection_probability", 0.15)
        self.config.set("curiosity_enabled", enable_curiosity)
        self.config.set("structural_plasticity_enabled", True)
        
        # Initialize brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=20, 
            height=20, 
            visionWidth=12, 
            visionHeight=12
        )
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Statistics tracking
        self.stats = {
            'total_reward': 0.0,
            'max_reward': 0.0,
            'steps': 0,
            'curiosity_history': [],
            'reward_history': []
        }
    
    def run_episode(self, max_steps=1000):
        """Run a complete learning episode"""
        print(f"Starting learning episode (max {max_steps} steps)...")
        
        for self.stats['steps'] in range(max_steps):
            # Update world and get sensory input
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process through agent and brain
            self.agent.processSensoryInput(percept)
            self.brain.step(self.stats['steps'])
            
            # Get action and execute it
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from experience
            self.agent.applyRewardModulation(result.reward, 0.0f)
            
            # Track statistics
            self.stats['total_reward'] += result.reward
            self.stats['reward_history'].append(result.reward)
            self.stats['curiosity_history'].append(self.agent.getCuriosityLevel())
            
            # Log progress every 100 steps
            if self.stats['steps'] % 100 == 0:
                self._log_progress()
            
            # Check termination conditions
            if self._should_terminate():
                print(f"Episode terminated at step {self.stats['steps']}")
                break
        
        self._log_final_stats()
        return self._get_summary()
    
    def _log_progress(self):
        """Log current episode progress"""
        avg_reward = self.stats['total_reward'] / (self.stats['steps'] + 1)
        curiosity = self.agent.getCuriosityLevel()
        
        print(f"  Step {self.stats['steps']}:")
        print(f"    Avg reward: {avg_reward:.3f}")
        print(f"    Current curiosity: {curiosity:.3f}")
        print(f"    Neurons firing: {self.brain.getFiringNeuronCount()}")
        print(f"    Development stage: {self.agent.getDevelopmentalStage()}")
    
    def _should_terminate(self):
        """Determine if episode should end"""
        # End if adult development reached
        if self.agent.getDevelopmentalStage() == nlm.DevelopmentalStage::Adult:
            return True
        
        # End if no learning for extended period
        if self.stats['steps'] > 500 and self.agent.getCuriosityLevel() < 0.01:
            return True
            
        return False
    
    def _log_final_stats(self):
        """Log episode summary"""
        avg_reward = self.stats['total_reward'] / self.stats['steps']
        max_curiosity = max(self.stats['curiosity_history'])
        
        print(f"\nEpisode completed!")
        print(f"  Total steps: {self.stats['steps']}")
        print(f"  Total reward: {self.stats['total_reward']:.3f}")
        print(f"  Average reward: {avg_reward:.3f}")
        print(f"  Maximum curiosity: {max_curiosity:.3f}")
        print(f"  Final development: {self.agent.getDevelopmentalStage()}")
    
    def _get_summary(self):
        """Get episode statistics as dictionary"""
        avg_reward = self.stats['total_reward'] / max(1, self.stats['steps'])
        max_curiosity = max(self.stats['curiosity_history'])
        
        return {
            'steps': self.stats['steps'],
            'total_reward': self.stats['total_reward'],
            'avg_reward': avg_reward,
            'max_curiosity': max_curiosity,
            'final_stage': self.agent.getDevelopmentalStage(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate()
        }

# Usage example
if __name__ == "__main__":
    import nlm
    
    print("Creating advanced learning agent...")
    agent = LearningAgent(neuron_count=800)
    
    # Run multiple episodes for comparison
    all_summaries = []
    for episode in range(3):
        print(f"\n{'='*60}")
        print(f"EPISODE {episode + 1}")
        print(f"{'='*60}")
        
        summary = agent.run_episode(max_steps=1000)
        all_summaries.append(summary)
        
        # Reset for next episode
        agent.brain.reset()
        agent.brain.initialize()
        agent.world.reset()
        agent.agent.reset()
    
    # Compare episodes
    print(f"\n{'='*60}")
    print("EPISODE COMPARISON")
    print(f"{'='*60}")
    
    best_episode = max(all_summaries, key=lambda x: x['avg_reward'])
    
    for i, summary in enumerate(all_summaries):
        print(f"Episode {i + 1}:")
        print(f"  Average reward: {summary['avg_reward']:.3f}")
        print(f"  Total reward: {summary['total_reward']:.3f}")
        print(f"  Max curiosity: {summary['max_curiosity']:.3f}")
        print(f"  Final stage: {summary['final_stage']}")
        print()
    
    print("BEST PERFORMING EPISODE:")
    print(f"  Average reward: {best_episode['avg_reward']:.3f}")
    print(f"  Total reward: {best_episode['total_reward']:.3f}")
    print(f"  Steps: {best_episode['steps']}")
    print(f"  Development stage: {best_episode['final_stage']}")
```

---

## Quick Start

### Installation & Setup
```bash
# Clone the repository
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run demo
./nlm_phase6_demo
```

### Python Bindings
```python
# If using Python bindings
import pynlm

# Create a basic brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simple test - let the brain process for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain processed {brain.getTotalSpikeCount()} spikes")
```

---

## The 3 Core Components

### 1. Brain
The virtual neural substrate that thinks, learns, and stores memories.
- Contains neurons, synapses, and neural populations
- Implements LIF dynamics, plasticity, and neuromodulation
- Manages working memory, episodic memory, and conceptual knowledge

### 2. World
The environment where the brain operates and learns.
- Provides sensory inputs (vision, touch, internal states)
- Handles motor outputs and action execution
- Manages objects, resources, and hazards
- Tracks time and provides feedback

### 3. Agent
The interface that connects brain to world.
- Processes sensory inputs and converts to neural signals
- Decodes neural activity into motor commands
- Applies reward modulation and learning
- Manages curiosity and exploration behavior

---

## Basic Examples

### Example 1: The Simplest Brain
```python
import pynlm

# Create brain with default configuration (500 neurons)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize - this sets up neurons and connections
brain.initialize()

# Run brain processing for 100 time steps
for i in range(100):
    brain.step(i)  # Each step simulates neural dynamics

print(f"✓ Brain active with {brain.getTotalSpikeCount()} spikes")
print(f"✓ {brain.getFiringNeuronCount()} neurons currently firing")
print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Basic Agent-WORLD Interaction
```python
import pynlm

# Setup the components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run the agent-world loop
for step in range(50):
    # 1. Update the world (time passes)
    world.update(0.1)
    
    # 2. Get sensory perception from world
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # 4. Brain processes and computes
    brain.step(step)
    
    # 5. Decode motor command from brain's motor neurons
    action = agent.decodeMotorCommand()
    
    # 6. Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("✓ Simulation completed successfully")
print(f"✓ Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

---

## Advanced Usage

### Advanced Example 1: Learning Agent with Reward Modulation
```python
import pynlm

class LearningAgent:
    def __init__(self, neuron_count=1000, enable_curiosity=True):
        # Create custom configuration for advanced control
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", neuron_count)
        self.config.set("connection_probability", 0.15)
        self.config.set("curiosity_enabled", enable_curiosity)
        self.config.set("structural_plasticity_enabled", True)
        
        # Initialize brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=20, 
            height=20, 
            visionWidth=12, 
            visionHeight=12
        )
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Statistics tracking
        self.stats = {
            'total_reward': 0.0,
            'max_reward': 0.0,
            'steps': 0,
            'curiosity_history': [],
            'reward_history': []
        }
    
    def run_episode(self, max_steps=1000):
        """Run a complete learning episode"""
        print(f"Starting learning episode (max {max_steps} steps)...")
        
        for self.stats['steps'] in range(max_steps):
            # Update world and get sensory input
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process through agent and brain
            self.agent.processSensoryInput(percept)
            self.brain.step(self.stats['steps'])
            
            # Get action and execute it
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from experience
            self.agent.applyRewardModulation(result.reward, 0.0f)
            
            # Track statistics
            self.stats['total_reward'] += result.reward
            self.stats['reward_history'].append(result.reward)
            self.stats['curiosity_history'].append(self.agent.getCuriosityLevel())
            
            # Log progress every 100 steps
            if self.stats['steps'] % 100 == 0:
                self._log_progress()
            
            # Check termination conditions
            if self._should_terminate():
                print(f"Episode terminated at step {self.stats['steps']}")
                break
        
        self._log_final_stats()
        return self._get_summary()
    
    def _log_progress(self):
        """Log current episode progress"""
        avg_reward = self.stats['total_reward'] / (self.stats['steps'] + 1)
        curiosity = self.agent.getCuriosityLevel()
        
        print(f"  Step {self.stats['steps']}:")
        print(f"    Avg reward: {avg_reward:.3f}")
        print(f"    Current curiosity: {curiosity:.3f}")
        print(f"    Neurons firing: {self.brain.getFiringNeuronCount()}")
        print(f"    Development stage: {self.agent.getDevelopmentalStage()}")
    
    def _should_terminate(self):
        """Determine if episode should end"""
        # End if adult development reached
        if self.agent.getDevelopmentalStage() == nlm.DevelopmentalStage::Adult:
            return True
        
        # End if no learning for extended period
        if self.stats['steps'] > 500 and self.agent.getCuriosityLevel() < 0.01:
            return True
            
        return False
    
    def _log_final_stats(self):
        """Log episode summary"""
        avg_reward = self.stats['total_reward'] / self.stats['steps']
        max_curiosity = max(self.stats['curiosity_history'])
        
        print(f"\nEpisode completed!")
        print(f"  Total steps: {self.stats['steps']}")
        print(f"  Total reward: {self.stats['total_reward']:.3f}")
        print(f"  Average reward: {avg_reward:.3f}")
        print(f"  Maximum curiosity: {max_curiosity:.3f}")
        print(f"  Final development: {self.agent.getDevelopmentalStage()}")
    
    def _get_summary(self):
        """Get episode statistics as dictionary"""
        avg_reward = self.stats['total_reward'] / max(1, self.stats['steps'])
        max_curiosity = max(self.stats['curiosity_history'])
        
        return {
            'steps': self.stats['steps'],
            'total_reward': self.stats['total_reward'],
            'avg_reward': avg_reward,
            'max_curiosity': max_curiosity,
            'final_stage': self.agent.getDevelopmentalStage(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate()
        }

# Usage example
if __name__ == "__main__":
    import nlm
    
    print("Creating advanced learning agent...")
    agent = LearningAgent(neuron_count=800)
    
    # Run multiple episodes for comparison
    all_summaries = []
    for episode in range(3):
        print(f"\n{'='*60}")
        print(f"EPISODE {episode + 1}")
        print(f"{'='*60}")
        
        summary = agent.run_episode(max_steps=1000)
        all_summaries.append(summary)
        
        # Reset for next episode
        agent.brain.reset()
        agent.brain.initialize()
        agent.world.reset()
        agent.agent.reset()
    
    # Compare episodes
    print(f"\n{'='*60}")
    print("EPISODE COMPARISON")
    print(f"{'='*60}")
    
    best_episode = max(all_summaries, key=lambda x: x['avg_reward'])
    
    for i, summary in enumerate(all_summaries):
        print(f"Episode {i + 1}:")
        print(f"  Average reward: {summary['avg_reward']:.3f}")
        print(f"  Total reward: {summary['total_reward']:.3f}")
        print(f"  Max curiosity: {summary['max_curiosity']:.3f}")
        print(f"  Final stage: {summary['final_stage']}")
        print()
    
    print("BEST PERFORMING EPISODE:")
    print(f"  Average reward: {best_episode['avg_reward']:.3f}")
    print(f"  Total reward: {best_episode['total_reward']:.3f}")
    print(f"  Steps: {best_episode['steps']}")
    print(f"  Development stage: {best_episode['final_stage']}")
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
