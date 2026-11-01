# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

### Improved Neural Dynamics

NLM has been enhanced with:
- **Realistic LIF neuron dynamics** with membrane potentials and spike generation
- **Multiple plasticity rules** including STDP, Hebbian learning, and structural plasticity
- **Integrated neuromodulation** with dopamine, curiosity, and novelty systems
- **Complex memory systems** including working memory, episodic memory, and associative memory
- **Advanced development stages** from initial to aging phases

The reward and neuromodulation systems are now fully functional, providing meaningful feedback for learning.

---

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

## Advanced Features for Experienced Users

### Neural Dynamics Configuration

```python
# Custom neural parameters
config = pynlm.createDefaultConfig()
config.set("brain.v_thresh", -40.0)    # Higher threshold
config.set("brain.v_rest", -65.0)     # Less negative resting potential
config.set("brain.v_reset", -60.0)    # Higher reset potential
config.set("brain.tau_mem", 15.0)     # Faster membrane time constant
config.set("brain.tau_ref", 5.0)      # Shorter refractory period
```

### Advanced Plasticity Rules

```python
# Fine-tune plasticity mechanisms
config.set("plasticity.stdp.learning_rate", 0.005)    # Stronger STDP
config.set("plasticity.hebbian.enable", True)          # Enable Hebbian
config.set("plasticity.structural.enable", True)      # Enable structural
config.set("plasticity.structural.rate", 0.0001)      # Structural plasticity rate
```

### Neuromodulation Settings

```python
# Configure neuromodulatory systems
config.set("neuromod.dopamine.scale", 2.0)              # Stronger dopamine
config.set("neuromod.curiosity.enable", True)           # Enable curiosity
config.set("neuromod.novelty.enable", True)              # Enable novelty detection
```

### Memory System Configuration

```python
# Memory system parameters
config.set("memory.working.capacity", 50)               # Working memory size
config.set("memory.episodic.max_episodes", 1000)       # Episodic memory capacity
```

### Development and Learning

```python
# Advanced development settings
config.set("development.plasticity_rate", 0.01)         # Plasticity changes over time
config.set("development.stimulus_threshold", 0.1)       # When development accelerates
```

---

## Debugging and Advanced Usage Tips

### Monitoring Brain State

```python
# In your simulation loop
if step % 100 == 0:
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    print(f"  Development Stage: {brain.getDevelopmentalStage()}")
    print(f"  Working Memory Traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
    print(f"  Episodic Memory Count: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
```

### Custom Configuration Loading

```python
import json

# Load configuration from JSON
config = pynlm.createDefaultConfig()
with open("my_config.json", "r") as f:
    config_data = json.load(f)
    
for key, value in config_data.items():
    config.set(key, value)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Saving and Loading Checkpoints

```python
# Save brain state (checkpointing)
brain.save("brain_checkpoint.bin")

# Later, restore brain state
restored_brain = pynlm.createBrain(config)
restored_brain.load("brain_checkpoint.bin")

# Verify restoration
print(f"Original neurons: {brain.getTotalNeuronCount()}")
print(f"Restored neurons: {restored_brain.getTotalNeuronCount()}")
print(f"Synapse match: {brain.getTotalSynapseCount() == restored_brain.getTotalSynapseCount()}")
```

### Batch Simulation

```python
def run_batch_experiments(num_batches=5, steps_per_batch=1000):
    """Run multiple simulations with different random seeds."""
    results = []
    
    for batch in range(num_batches):
        print(f"\n=== Batch {batch + 1}/{num_batches} ===")
        
        # Create config with batch-specific seed
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + batch)
        config.set("brain.neuron_count", 500 + batch * 100)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run batch
        total_reward = 0.0
        for step in range(steps_per_batch):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            total_reward += reward
        
        # Store results
        results.append({
            'batch': batch,
            'total_reward': total_reward,
            'avg_reward': total_reward / steps_per_batch,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_dopamine': agent.getNeuromodulationLevel(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
        })
        
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Average reward: {total_reward / steps_per_batch:.4f}")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    
    return results

# Run batch experiments
batch_results = run_batch_experiments(3, 500)

# Analyze results
print("\n=== Batch Analysis ===")
total_rewards = [r['total_reward'] for r in batch_results]
print(f"Average total reward: {sum(total_rewards) / len(total_rewards):.2f}")
print(f"Best batch: Batch {max(batch_results, key=lambda x: x['avg_reward'])['batch']}")
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

## Advanced Simulation Control

### Multi-Agent Simulation

```python
import pynlm
from typing import List, Dict, Any

def run_multi_agent_simulation(
    num_agents: int,
    world_width: int = 30,
    world_height: int = 30,
    steps: int = 1000
) -> Dict[str, Any]:
    """Run simulation with multiple independent agents."""
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Create list of agents
    agents: List[pynlm.AgentBrain] = []
    brains: List[pynlm.Brain] = []
    
    for i in range(num_agents):
        # Create unique configuration for each agent
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + i * 1000)  # Different seed per agent
        config.set("brain.neuron_count", 300 + i * 50)  # Gradually increasing neurons
        
        brain = pynlm.createBrain(config)
        brains.append(brain)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
        agent.initialize(world)
        
        # Set different starting positions
        agent_x = (i + 1) * (world_width / (num_agents + 1))
        agent_y = (i + 1) * (world_height / (num_agents + 1))
        world.setAgentStart(agent_x, agent_y)
    
    # Run simulation
    agent_results = []
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Each agent senses and acts
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brains[i].step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Collect statistics every 100 steps
        if step % 100 == 0:
            step_stats = {
                'step': step,
                'world_time': world.getSimulationTime(),
                'agents': []
            }
            
            for i, agent in enumerate(agents):
                agent_stats = {
                    'agent_id': i,
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'dopamine': agent.getNeuromodulationLevel(),
                    'prediction_error': agent.getPredictionError(),
                    'development_stage': brains[i].getDevelopmentalStage(),
                    'firing_rate': brains[i].getAverageFiringRate(),
                    'active_neurons': brains[i].getFiringNeuronCount(),
                }
                step_stats['agents'].append(agent_stats)
            
            agent_results.append(step_stats)
    
    # Compile final results
    final_results = {
        'num_agents': num_agents,
        'total_steps': steps,
        'final_metrics': []
    }
    
    for i, agent in enumerate(agents):
        agent_metrics = {
            'agent_id': i,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_dopamine': agent.getNeuromodulationLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'development_stage': brains[i].getDevelopmentalStage(),
            'total_synapses': brains[i].getTotalSynapseCount(),
            'total_spikes': brains[i].getTotalSpikeCount(),
            'avg_firing_rate': brains[i].getAverageFiringRate(),
        }
        final_results['final_metrics'].append(agent_metrics)
    
    return final_results

# Run multi-agent simulation
print("Running multi-agent simulation with 3 agents...")
results = run_multi_agent_simulation(num_agents=3, steps=500)

print("\n=== Final Results ===")
for agent in results['final_metrics']:
    print(f"Agent {agent['agent_id']}:")
    print(f"  Final Curiosity: {agent['final_curiosity']:.3f}")
    print(f"  Final Novelty: {agent['final_novelty']:.3f}")
    print(f"  Final Dopamine: {agent['final_dopamine']:.3f}")
    print(f"  Final Prediction Error: {agent['final_prediction_error']:.3f}")
    print(f"  Development Stage: {agent['development_stage']}")
    print(f"  Total Synapses: {agent['total_synapses']}")
    print(f"  Total Spikes: {agent['total_spikes']}")
    print(f"  Average Firing Rate: {agent['avg_firing_rate']:.3f}")
    print()
```

### Complex Environment Simulation

```python
import pynlm
import math

class CustomWorld(pynlm.SimpleWorld):
    """Extended world with custom dynamics."""
    
    def __init__(self):
        super().__init__()
        self.resource_spawn_rate = 0.05  # Probability of new resource per step
        self.hazard_spawn_rate = 0.02     # Probability of new hazard per step
        self.max_resources = 15
        self.max_hazards = 8
    
    def update_with_spawn(self, timestep: float):
        """Update world and occasionally spawn new objects."""
        # Update world state
        super().update(timestep)
        
        # Randomly spawn resources
        for _ in range(int(self.resource_spawn_rate * 100)):  # Scale for performance
            if len([o for o in self.objects_ if o.type == pynlm.WorldObjectType.Resource]) < self.max_resources:
                x = self.random.uniform(1.0, self.width_ - 1.0)
                y = self.random.uniform(1.0, self.height_ - 1.0)
                if self.isValidPosition(x, y):
                    self.objects_.append(
                        pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 10.0)
                    )
        
        # Randomly spawn hazards
        for _ in range(int(self.hazard_spawn_rate * 100)):
            if len([o for o in self.objects_ if o.type == pynlm.WorldObjectType.Hazard]) < self.max_hazards:
                x = self.random.uniform(1.0, self.width_ - 1.0)
                y = self.random.uniform(1.0, self.height_ - 1.0)
                if self.isValidPosition(x, y):
                    self.objects_.append(
                        pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, -5.0)
                    )
    
    def get_environment_complexity(self) -> float:
        """Calculate current environmental complexity."""
        resources = len([o for o in self.objects_ if o.type == pynlm.WorldObjectType.Resource])
        hazards = len([o for o in self.objects_ if o.type == pynlm.WorldObjectType.Hazard])
        walls = len([o for o in self.objects_ if o.type == pynlm.WorldObjectType.Wall])
        
        return (resources * 0.5 + hazards * 0.8 + walls * 0.3) / (self.max_resources + self.max_hazards + 20)

def run_complex_environment_simulation(steps: int = 1000):
    """Run simulation with complex environment dynamics."""
    
    # Create custom world
    world = CustomWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    world.setRandomSeed(12345)
    
    # Create agent with enhanced learning capabilities
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 12345)
    config.set("brain.neuron_count", 800)  # Larger brain for complex task
    config.set("plasticity.stdp.learning_rate", 0.008)  # Faster STDP
    config.set("neuromod.dopamine.scale", 1.5)  # Stronger dopamine
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complex simulation
    complexity_history = []
    curiosity_history = []
    reward_history = []
    
    print("Running complex environment simulation...")
    print("Environment will dynamically change with random object spawning.")
    
    for step in range(steps):
        # Update custom world with spawning
        world.update_with_spawn(0.1)
        
        # Get agent percept
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record metrics
        complexity = world.get_environment_complexity()
        curiosity_history.append(agent.getCuriosityLevel())
        complexity_history.append(complexity)
        reward_history.append(reward)
        
        # Print progress and environment status
        if step % 100 == 0:
            resources = len([o for o in world.objects_ if o.type == pynlm.WorldObjectType.Resource])
            hazards = len([o for o in world.objects_ if o.type == pynlm.WorldObjectType.Hazard])
            
            print(f"Step {step}: Reward={reward:+.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Complexity={complexity:.3f}, "
                  f"Resources={resources}, Hazards={hazards}")
    
    # Calculate statistics
    avg_curiosity = sum(curiosity_history) / len(curiosity_history)
    avg_complexity = sum(complexity_history) / len(complexity_history)
    avg_reward = sum(reward_history) / len(reward_history)
    
    # Find peaks and trends
    max_curiosity = max(curiosity_history)
    max_complexity = max(complexity_history)
    
    print(f"\n=== Complex Environment Simulation Results ===")
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Average environmental complexity: {avg_complexity:.3f}")
    print(f"Average reward per step: {avg_reward:.3f}")
    print(f"Maximum curiosity achieved: {max_curiosity:.3f}")
    print(f"Maximum environmental complexity: {max_complexity:.3f}")
    
    # Check for learning patterns
    if len(curiosity_history) > 100:
        first_half = curiosity_history[:len(curiosity_history)//2]
        second_half = curiosity_history[len(curiosity_history)//2:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        if avg_second > avg_first * 1.2:
            print("✓ Curiosity increased significantly - learning successful!")
        elif avg_second < avg_first * 0.8:
            print("⚠ Curiosity decreased - possible over-specialization.")
        else:
            print("≈ Curiosity stabilized - consistent exploration behavior.")
    
    return {
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_dopamine': agent.getNeuromodulationLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'complexity_history': complexity_history,
        'curiosity_history': curiosity_history,
        'reward_history': reward_history,
    }

# Run complex environment simulation
complex_results = run_complex_environment_simulation(1000)
```

---

## Performance Optimization Tips

### Optimized Simulation Loop

```python
import time
import pynlm

def optimized_simulation_loop(brain, agent, world, steps: int, timestep: float = 0.1):
    """Optimized simulation loop for maximum performance."""
    
    # Pre-compile frequently used methods
    process_input = agent.processSensoryInput
    brain_step = brain.step
    decode_action = agent.decodeMotorCommand
    apply_command = world.applyMotorCommand
    update_agent = agent.updateDevelopment
    
    # Use local variables for faster access
    get_percept = world.getSensoryPercept
    get_time = world.getSimulationTime
    
    start_time = time.time()
    
    for step in range(steps):
        # Single percept retrieval
        percept = get_percept()
        
        # Process all subsystems
        process_input(percept)
        brain_step(step)
        action = decode_action()
        apply_command(action, get_time())
        
        # Apply neuromodulation (simplified - use cached values)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        update_agent(timestep)
        
        # Update world
        world.update(timestep)
    
    elapsed = time.time() - start_time
    print(f"Completed {steps} steps in {elapsed:.2f} seconds ({steps/elapsed:.0f} steps/sec)")
    
    return elapsed

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run optimized simulation
optimized_simulation_loop(brain, agent, world, 5000)
```

### Batch Processing

```python
import concurrent.futures
from typing import List, Dict

def run_single_experiment(experiment_id: int, params: Dict) -> Dict:
    """Run a single simulation experiment."""
    
    config = pynlm.createDefaultConfig()
    config.set("random_seed", params.get('random_seed', 42))
    config.set("brain.neuron_count", params.get('neuron_count', 500))
    config.set("plasticity.stdp.learning_rate", params.get('stdp_rate', 0.01))
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(**params.get('world_config', {'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8}))
    world.reset()
    agent.initialize(world)
    
    # Enable features based on parameters
    if params.get('enable_reward', True):
        agent.enableRewardModulation(True)
    if params.get('enable_curiosity', True):
        agent.enableCuriosity(True)
    if params.get('enable_development', True):
        agent.enableDevelopment(True)
    
    # Run experiment
    total_reward = 0.0
    steps = params.get('steps', 500)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        total_reward += reward
    
    return {
        'experiment_id': experiment_id,
        'total_reward': total_reward,
        'avg_reward': total_reward / steps,
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_dopamine': agent.getNeuromodulationLevel(),
        'steps': steps,
        **params
    }

def run_batch_experiments(experiments: List[Dict], max_workers: int = None):
    """Run multiple experiments in parallel."""
    
    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        # Submit all experiments
        future_to_exp = {
            executor.submit(run_single_experiment, i, exp): i 
            for i, exp in enumerate(experiments)
        }
        
        # Collect results
        results = []
        for future in concurrent.futures.as_completed(future_to_exp):
            exp_id = future_to_exp[future]
            try:
                result = future.result()
                results.append(result)
                print(f"Experiment {exp_id} completed")
            except Exception as e:
                print(f"Experiment {exp_id} failed: {e}")
    
    return results

# Define different experiment configurations
experiment_configs = [
    {'random_seed': 1000, 'neuron_count': 300, 'stdp_rate': 0.005, 'steps': 300},
    {'random_seed': 1001, 'neuron_count': 500, 'stdp_rate': 0.01, 'steps': 500},
    {'random_seed': 1002, 'neuron_count': 700, 'stdp_rate': 0.015, 'steps': 700},
    {'random_seed': 1003, 'neuron_count': 1000, 'stdp_rate': 0.02, 'steps': 1000},
]

# Run batch experiments
print("Running batch experiments...")
batch_results = run_batch_experiments(experiment_configs)

# Analyze results
print("\n=== Batch Experiment Results ===")
for result in batch_results:
    print(f"Experiment {result['experiment_id']}:")
    print(f"  Neurons: {result['neuron_count']}, STDP rate: {result['stdp_rate']}, Steps: {result['steps']}")
    print(f"  Average reward: {result['avg_reward']:.4f}")
    print(f"  Final curiosity: {result['final_curiosity']:.3f}")
    print()
```
