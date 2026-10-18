# NLM - Enhanced Easy Usage Guide for Beginners

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

### Example 3: Enhanced Brain with Learning

```python
import pynlm

# Create a custom brain with enhanced learning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run learning episode
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent position
    agent_body = world.getAgentBody()
    reward = 0.1 if agent_body.isMoving else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 50 == 0:
        stats = agent.get_agent_stats()
        print(f"Step {step}: Curiosity={stats['curiosity']:.3f}, "
              f"Novelty={stats['novelty']:.3f}, "
              f"Neuromod={stats['neuromodulation']:.3f}")

print("Learning episode completed!")
```

### Example 4: Configuration Validation and Error Handling

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Validate configuration
validation = pynlm.validate_configuration(config)
if not validation["valid"]:
    print("Configuration errors:", validation["errors"])
    exit(1)

if validation["warnings"]:
    print("Warnings:", validation["warnings"])

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get enhanced stats
stats = brain.get_stats()
print(f"Brain configured: {stats['total_neurons']} neurons, "
      f"{stats['total_synapses']} synapses")
print(f"E/I ratio: {stats['e_i_ratio']:.3f}")
```

### Example 5: Benchmarking and Performance Monitoring

```python
import pynlm
import time

# Create brain for benchmarking
config = pynlm.create_preset_config("standard")
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark different brain sizes
presets = ["minimal", "standard", "large"]
for preset in presets:
    print(f"\nBenchmarking {preset} brain...")
    
    config = pynlm.create_preset_config(preset)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    benchmark_results = pynlm.benchmark_brain(brain, 500)
    
    print(f"  Steps: {benchmark_results['steps']}")
    print(f"  Time: {benchmark_results['time_ms']:.2f} ms")
    print(f"  Time/step: {benchmark_results['time_per_step_ms']:.4f} ms")
    print(f"  Spikes/sec: {benchmark_results['spikes_per_second']:.0f}")
```

---

## Mini Projects

### Mini Project 1: Basic Learning Agent

```python
import pynlm

class SimpleLearningAgent:
    def __init__(self, brain_config=None):
        # Setup brain with learning capabilities
        if brain_config is None:
            brain_config = pynlm.create_preset_config("standard")
        
        self.brain = pynlm.createBrain(brain_config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enable_all_systems()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        self.world.reset()
        self.agent.initialize(self.world)
        
        self.steps_completed = 0
    
    def run_episode(self, max_steps=1000):
        """Run complete learning episode"""
        for step in range(max_steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process and learn
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Act
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from experience
            reward = self.calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            self.steps_completed += 1
            
            # Print progress
            if step % 100 == 0:
                agent_stats = self.agent.get_agent_stats()
                print(f"Step {step}: "
                      f"Curiosity={agent_stats['curiosity']:.3f}, "
                      f"Novelty={agent_stats['novelty']:.3f}, "
                      f"Firing neurons={self.brain.getFiringNeuronCount()}")
                
                # Early stopping if not learning
                if step > 100 and agent_stats['curiosity'] < 0.01:
                    print("Agent stopped exploring - episode complete")
                    break
        
        return self.get_episode_summary()
    
    def calculate_reward(self):
        """Calculate reward based on agent's behavior"""
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Reward for moving (exploration)
        reward = 0.1 if body.isMoving else 0.0
        
        # Bonus for discovering resources
        vision = percept.getVision()
        if vision.empty():
            reward += 0.0
        else:
            avg_intensity = sum(vision.begin(), vision.end()) / vision.size();
            if avg_intensity > 0.5:  # Interesting visual input
                reward += 0.5
        
        return reward
    
    def get_episode_summary(self):
        """Get summary of learning episode"""
        agent_stats = self.agent.get_agent_stats()
        brain_stats = self.brain.get_stats()
        
        summary = {
            'steps': self.steps_completed,
            'neurons': brain_stats['total_neurons'],
            'synapses': brain_stats['total_synapses'],
            'spikes': brain_stats['total_spikes'],
            'firing_rate': brain_stats['avg_firing_rate'],
            'final_curiosity': agent_stats['curiosity'],
            'final_novelty': agent_stats['novelty'],
            'final_neuromodulation': agent_stats['neuromodulation']
        }
        
        return summary

# Use the agent
if __name__ == "__main__":
    agent = SimpleLearningAgent()
    
    print("Starting learning episode...")
    summary = agent.run_episode(max_steps=500)
    
    print("\n=== Learning Episode Complete ===")
    print(f"Steps: {summary['steps']}")
    print(f"Final curiosity: {summary['final_curiosity']:.3f}")
    print(f"Final novelty: {summary['final_novelty']:.3f}")
    print(f"Final neuromodulation: {summary['final_neuromodulation']:.3f}")
    print(f"Final firing rate: {summary['firing_rate']:.3f} Hz")
```

### Mini Project 2: Configuration Explorer

```python
import pynlm

def explore_configurations():
    """Explore how different configurations affect brain behavior"""
    
    test_configs = {
        "minimal": {"brain.neuron_count": 100, "brain.synapse_density": 0.05},
        "standard": {"brain.neuron_count": 1000, "brain.synapse_density": 0.1},
        "dense": {"brain.neuron_count": 5000, "brain.synapse_density": 0.3},
    }
    
    results = {}
    
    for name, params in test_configs.items():
        print(f"\nTesting {name} configuration...")
        
        # Create config
        config = pynlm.createDefaultConfig()
        for key, value in params.items():
            config.set(key, value)
        
        # Validate
        validation = pynlm.validate_configuration(config)
        if not validation["valid"]:
            print(f"  Invalid config: {validation['errors']}")
            continue
        
        # Create and test brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        benchmark = pynlm.benchmark_brain(brain, 200)
        
        results[name] = {
            'neuron_count': params["brain.neuron_count"],
            'synapse_density': params["brain.synapse_density"],
            'time_per_step_ms': benchmark['time_per_step_ms'],
            'spikes_per_second': benchmark['spikes_per_second'],
            'performance_score': benchmark['spikes_per_second'] / benchmark['time_per_step_ms']
        }
        
        print(f"  Performance: {benchmark['time_per_step_ms']:.4f} ms/step, "
              f"{benchmark['spikes_per_second']:.0f} spikes/sec")
    
    # Find best configuration
    if results:
        best_config = max(results.items(), key=lambda x: x[1]['performance_score'])
        print(f"\n=== Best Configuration: {best_config[0]} ===")
        print(f"Performance score: {best_config[1]['performance_score']:.2f}")
        print(f"Neurons: {best_config[1]['neuron_count']}, "
              f"Density: {best_config[1]['synapse_density']}")
    
    return results

if __name__ == "__main__":
    explore_configurations()
```

### Mini Project 3: Visualization and Analysis

```python
import pynlm
import math

def analyze_brain_activity(brain, world, steps=100):
    """Analyze brain activity patterns over time"""
    
    activity_data = {
        'steps': [],
        'spike_rates': [],
        'firing_rates': [],
        'e_i_ratios': [],
        'agent_positions': [],
        'agent_orientations': []
    }
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent = pynlm.createAgentBrain(brain)
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Collect data
        activity_data['steps'].append(step)
        activity_data['spike_rates'].append(brain.getTotalSpikeCount() / (step + 1))
        activity_data['firing_rates'].append(brain.getFiringNeuronCount() / 
                                            brain.getTotalNeuronCount())
        activity_data['e_i_ratios'].append(brain.getExcitationInhibitionRatio())
        
        # Get agent position
        body = world.getAgentBody()
        activity_data['agent_positions'].append((body.x, body.y))
        activity_data['agent_orientations'].append(body.orientation)
    
    return activity_data

def visualize_activity(activity_data):
    """Generate simple text-based visualization"""
    
    print("=== Brain Activity Analysis ===")
    print(f"Total steps: {len(activity_data['steps'])}")
    
    # Calculate statistics
    avg_spike_rate = sum(activity_data['spike_rates']) / len(activity_data['spike_rates'])
    avg_firing_rate = sum(activity_data['firing_rates']) / len(activity_data['firing_rates'])
    avg_ei_ratio = sum(activity_data['e_i_ratios']) / len(activity_data['e_i_ratios'])
    
    print(f"Average spike rate: {avg_spike_rate:.2f} spikes/step")
    print(f"Average firing rate: {avg_firing_rate:.2%}")
    print(f"Average E/I ratio: {avg_ei_ratio:.3f}")
    
    # Analyze movement
    positions = activity_data['agent_positions']
    if len(positions) > 1:
        start_x, start_y = positions[0]
        end_x, end_y = positions[-1]
        distance = math.sqrt((end_x - start_x)**2 + (end_y - start_y)**2)
        print(f"Agent traveled: {distance:.2f} units")
        
        # Calculate exploration
        unique_positions = set(positions)
        exploration_score = len(unique_positions) / len(positions)
        print(f"Exploration score: {exploration_score:.3f}")
    
    return {
        'avg_spike_rate': avg_spike_rate,
        'avg_firing_rate': avg_firing_rate,
        'avg_ei_ratio': avg_ei_ratio,
        'total_distance': distance if len(positions) > 1 else 0.0,
        'exploration_score': exploration_score if len(positions) > 1 else 0.0
    }

# Example usage
if __name__ == "__main__":
    # Setup
    config = pynlm.create_preset_config("standard")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enable_all_systems()
    agent.initialize(world)
    
    # Run analysis
    print("Running brain activity analysis...")
    activity_data = analyze_brain_activity(brain, world, 200)
    
    # Visualize
    stats = visualize_activity(activity_data)
    
    print(f"\n=== Analysis Complete ===")
    print(f"Performance summary: {stats}")
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

That's it! You're now ready to use NLM with the enhanced Python API.
