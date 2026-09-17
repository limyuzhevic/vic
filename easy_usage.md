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

### Project 3: Advanced Learning Agent

```python
import pynlm
import numpy as np
import time

def advanced_learning_agent(world_size=20, neuron_count=3000, num_steps=2000):
    """Advanced learning agent with curiosity, development, and memory."""
    
    # Configure brain for complex learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", 0.0005)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("plasticity.structural.plasticity_rate", 0.03)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.threshold", 0.05)
    config.set("neuromod.novelty.decay_rate", 0.95)
    config.set("development.enable_critical_period", True)
    
    # Create and initialize components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=8,
        visionHeight=8
    )
    world.setMaxEnergy(80.0)
    world.reset()
    world.setAgentStart(world_size/2, world_size/2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting advanced learning agent simulation...")
    print(f"  - Brain: {neuron_count} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  - World: {world_size}x{world_size} with sensory range 8x8")
    print(f"  - Steps: {num_steps}")
    print(f"  - Features: Reward modulation, Curiosity, Development, Structural plasticity")
    print()
    
    # Performance tracking
    performance_data = {
        'step': 0,
        'spikes': 0,
        'rewards': 0,
        'novelty_detections': 0,
        'curiosity_spikes': 0
    }
    
    start_time = time.time()
    
    # Main simulation loop with advanced learning
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation with prediction error
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development (brain maturation)
        agent.updateDevelopment(0.1)
        
        # Track performance metrics
        performance_data['step'] = step
        performance_data['spikes'] = brain.getTotalSpikeCount()
        performance_data['rewards'] += reward if reward > 0 else 0
        performance_data['novelty_detections'] += agent.getNoveltyLevel() > 0.7
        performance_data['curiosity_spikes'] += agent.getCuriosityLevel() > 0.5
        
        # Print progress every 200 steps
        if step % 200 == 0:
            elapsed = time.time() - start_time
            steps_per_sec = step / elapsed if elapsed > 0 else 0
            
            print(f"Step {step:4d} | "
                  f"Firing={brain.getFiringNeuronCount():5d} | "
                  f"Total Spikes={brain.getTotalSpikeCount():8d} | "
                  f"Curiosity={agent.getCuriosityLevel():.3f} | "
                  f"Novelty={agent.getNoveltyLevel():.3f} | "
                  f"Dev={brain.getDevelopmentalStage().name:12s} | "
                  f"Neuromod={agent.getNeuromodulationLevel():.3f} | "
                  f"Perf={steps_per_sec:.1f} steps/s")
    
    # Calculate final statistics
    avg_firing_rate = performance_data['spikes'] / num_steps
    spike_efficiency = performance_data['spikes'] / config.get("brain.neuron_count")
    reward_efficiency = performance_data['rewards'] / num_steps if num_steps > 0 else 0
    
    print("\n" + "="*60)
    print("ADVANCED LEARNING AGENT RESULTS")
    print("="*60)
    print(f"  Total simulation steps: {performance_data['step'] + 1}")
    print(f"  Total spikes generated: {performance_data['spikes']:,}")
    print(f"  Average firing rate: {avg_firing_rate:.2f} Hz/neuron")
    print(f"  Spike efficiency: {spike_efficiency:.4f} spikes/synapse")
    print(f"  Reward efficiency: {reward_efficiency:.4f} rewards/step")
    print(f"  Novelty detections: {performance_data['novelty_detections']}")
    print(f"  High curiosity moments: {performance_data['curiosity_spikes']}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return {
        'total_spikes': performance_data['spikes'],
        'final_dev_stage': brain.getDevelopmentalStage(),
        'avg_firing_rate': avg_firing_rate,
        'reward_efficiency': reward_efficiency,
        'novelty_detections': performance_data['novelty_detections'],
        'simulation_steps': performance_data['step'] + 1
    }

# Run advanced learning agent simulation
print("\n" + "="*60)
print("RUNNING ADVANCED LEARNING AGENT SIMULATION")
print("="*60)

advanced_results = advanced_learning_agent(
    world_size=15,
    neuron_count=2500,
    num_steps=1500
)

print(f"\nAdvanced Learning Agent completed successfully!")
print(f"Results: {advanced_results}")
```

### Performance-Optimized Agent

```python
import pynlm
import gc
import time
import psutil
import os

def optimized_agent_simulation(neuron_count=2000, memory_limit_mb=500):
    """Performance-optimized agent with memory management."""
    
    # Monitor system memory
    process = psutil.Process(os.getpid())
    
    def get_memory_mb():
        return process.memory_info().rss / 1024 / 1024
    
    # Configure for optimal performance
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("plasticity.structural.plasticity_rate", 0.02)
    config.set("brain.tau_mem", 15.0)    # Longer membrane time for stability
    config.set("brain.tau_ref", 3.0)     # Longer refractory for efficiency
    
    # Create components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize with performance monitoring
    agent.initialize(world)
    
    initial_memory = get_memory_mb()
    print(f"Initial memory usage: {initial_memory:.1f} MB")
    print(f"Target memory limit: {memory_limit_mb:.1f} MB")
    print(f"Starting optimized simulation with {neuron_count:,} neurons")
    print()
    
    # Run simulation with memory management
    for step in range(1000):
        # Check memory usage
        if step % 50 == 0:
            current_memory = get_memory_mb()
            memory_used = current_memory - initial_memory
            
            print(f"Step {step:4d} | "
                  f"Memory={current_memory:6.1f}MB (+{memory_used:5.1f}) | "
                  f"Firing={brain.getFiringNeuronCount():5d} | "
                  f"Spikes={brain.getTotalSpikeCount():8d} | "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
            
            # Memory management
            if memory_used > memory_limit_mb * 0.8:  # 80% of limit
                print(f"  WARNING: Approaching memory limit!")
                print(f"  Running garbage collection...")
                gc.collect()
                
                # Check again after GC
                after_gc_memory = get_memory_mb()
                gc_reduction = current_memory - after_gc_memory
                print(f"  Memory reduced by {gc_reduction:.1f} MB after GC")
                
                if after_gc_memory > memory_limit_mb:
                    print(f"  ERROR: Still above memory limit!")
                    break
        
        # Optimized simulation loop
        world.update(0.05)  # Faster time steps for performance
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.5)  # Simulate reward
        agent.updateDevelopment(0.05)
    
    # Final performance report
    final_memory = get_memory_mb()
    total_memory = final_memory - initial_memory
    
    print("\n" + "="*60)
    print("OPTIMIZED AGENT PERFORMANCE REPORT")
    print("="*60)
    print(f"  Total memory used: {total_memory:.1f} MB")
    print(f"  Final memory usage: {final_memory:.1f} MB")
    print(f"  Simulation efficiency: {step/10:.0f} steps/second (rough estimate)")
    print(f"  Total neurons created: {brain.getTotalNeuronCount():,}")
    print(f"  Total connections created: {brain.getTotalSynapseCount():,}")
    print(f"  Average firing rate: {brain.getTotalSpikeCount()/1000:.1f} Hz")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage().name}")
    
    return {
        'memory_used_mb': total_memory,
        'final_memory_mb': final_memory,
        'neurons_created': brain.getTotalNeuronCount(),
        'connections_created': brain.getTotalSynapseCount(),
        'steps_completed': step + 1
    }

# Run optimized simulation
print("\n" + "="*60)
print("RUNNING OPTIMIZED PERFORMANCE AGENT")
print("="*60)

optimized_results = optimized_agent_simulation(neuron_count=1500, memory_limit_mb=300)
print(f"\nOptimized agent completed successfully!")
```

### Complex Multi-Agent Environment

```python
import pynlm
import threading
import queue
import time

def multi_agent_simulation(agent_count=3, shared_world_size=30, steps_per_agent=500):
    """Multi-agent simulation with shared world and inter-agent communication."""
    
    print("="*70)
    print("COMPLEX MULTI-AGENT ENVIRONMENT SIMULATION")
    print("="*70)
    
    # Create shared world
    print(f"\nCreating shared world: {shared_world_size}x{shared_world_size}")
    shared_world = pynlm.createSimpleWorld()
    shared_world.configure(
        width=shared_world_size,
        height=shared_world_size,
        visionWidth=6,
        visionHeight=6
    )
    shared_world.setAgentStart(shared_world_size/2, shared_world_size/2)
    
    # Create agents with different configurations
    agents = []
    agent_results = []
    result_queue = queue.Queue()
    
    print(f"\nCreating {agent_count} agents with different personalities:")
    
    agent_configs = [
        {"name": "Explorer", "neurons": 1500, "curiosity": 2.0, "memory": 0.8},
        {"name": "Conservative", "neurons": 1000, "curiosity": 0.5, "memory": 0.3},
        {"name": "Adaptive", "neurons": 2000, "curiosity": 1.5, "memory": 0.6},
    ]
    
    for i in range(min(agent_count, len(agent_configs))):
        config = agent_configs[i]
        print(f"  Agent {i+1}: {config['name']} ({config['neurons']} neurons, "
              f"curiosity={config['curiosity']}, memory={config['memory']})")
        
        # Create agent
        agent_config = pynlm.createDefaultConfig()
        agent_config.set("brain.neuron_count", config["neurons"])
        agent_config.set("neuromod.curiosity.scale", config["curiosity"])
        agent_config.set("neuromod.dopamine.scale", 1.0)
        
        brain = pynlm.createBrain(agent_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(shared_world)
        
        agent.name = config["name"]
        agent.neurons = config["neurons"]
        agent.curiosity = config["curiosity"]
        agent.memory = config["memory"]
        
        agents.append(agent)
    
    # Shared communication queue
    communication_queue = queue.Queue()
    
    def agent_worker(agent_obj, agent_id):
        """Worker function for each agent thread."""
        try:
            for step in range(steps_per_agent):
                # Update shared world
                shared_world.update(0.1)
                
                # Get shared sensory input
                percept = shared_world.getSensoryPercept()
                
                # Process with agent
                agent_obj.processSensoryInput(percept)
                
                # Check for inter-agent communication
                if communication_queue.empty():
                    communication_queue.put({
                        'step': step,
                        'agent_id': agent_id,
                        'position': shared_world.getAgentBody().x,  # Simplified
                        'energy': shared_world.getAgentBody().energy
                    })
                
                # Get action and apply
                action = agent_obj.decodeMotorCommand()
                shared_world.applyMotorCommand(action, shared_world.getSimulationTime())
                
                # Apply reward with shared learning
                reward = shared_world.getSensoryPercept().getInternal()[0] if shared_world.getSensoryPercept().getInternal() else 0.0
                predicted_reward = agent_obj.getPredictionError()
                agent_obj.applyRewardModulation(reward, predicted_reward)
                
                # Update development
                agent_obj.updateDevelopment(0.1)
                
                if step % 100 == 0:
                    print(f"    Agent {agent_obj.name}: Step {step}, "
                          f"Energy={shared_world.getAgentBody().energy:.1f}")
            
            # Put result in queue
            result_queue.put({
                'agent_id': agent_id,
                'name': agent_obj.name,
                'neurons': agent_obj.neurons,
                'curiosity': agent_obj.curiosity,
                'memory': agent_obj.memory,
                'steps_completed': steps_per_agent,
                'final_energy': shared_world.getAgentBody().energy,
                'success': True
            })
            
        except Exception as e:
            result_queue.put({
                'agent_id': agent_id,
                'name': agent_obj.name,
                'error': str(e),
                'success': False
            })
    
    # Start all agent threads
    threads = []
    for i, agent_obj in enumerate(agents):
        thread = threading.Thread(target=agent_worker, args=(agent_obj, i))
        threads.append(thread)
        thread.start()
        time.sleep(0.1)  # Stagger thread starts
    
    # Monitor shared world
    print(f"\nMonitoring shared world with {agent_count} interacting agents:")
    
    for step in range(max(300, steps_per_agent // 2)):  # Run shorter for demo
        shared_world.update(0.1)
        
        # Check for inter-agent communication
        if not communication_queue.empty():
            comm_data = communication_queue.get()
            print(f"      Step {comm_data['step']:4d}: Agent {comm_data['agent_id']} "
                  f"at pos {comm_data['position']:.1f}, "
                  f"energy={comm_data['energy']:.1f}")
        
        # Simulate shared events
        shared_world.addObject(
            pynlm.WorldObject(
                shared_world_size/4, shared_world_size/4,
                pynlm.WorldObjectType.Resource, value=10.0
            )
        )
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join(timeout=300)  # 5 minute timeout
    
    # Collect results
    while not result_queue.empty():
        result = result_queue.get()
        agent_results.append(result)
    
    # Summary
    print("\n" + "="*70)
    print("MULTI-AGENT SIMULATION RESULTS")
    print("="*70)
    
    successful_agents = [r for r in agent_results if r['success']]
    print(f"  Total agents: {agent_count}")
    print(f"  Successful agents: {len(successful_agents)}")
    print(f"  Failed agents: {agent_count - len(successful_agents)}")
    print(f"  Total steps completed: {steps_per_agent * len(successful_agents)}")
    
    if successful_agents:
        total_neurons = sum(r['neurons'] for r in successful_agents)
        avg_curiosity = sum(r['curiosity'] for r in successful_agents) / len(successful_agents)
        avg_memory = sum(r['memory'] for r in successful_agents) / len(successful_agents)
        
        print(f"\n  Agent Statistics:")
        print(f"    Total neurons across all agents: {total_neurons:,}")
        print(f"    Average curiosity level: {avg_curiosity:.2f}")
        print(f"    Average memory factor: {avg_memory:.2f}")
        print(f"    Average final energy: {sum(r['final_energy'] for r in successful_agents) / len(successful_agents):.1f}")
    
    return agent_results

# Run multi-agent simulation
multi_agent_results = multi_agent_simulation(agent_count=3, steps_per_agent=300)

print(f"\nMulti-agent simulation completed successfully!")
print(f"Results: {len(multi_agent_results)} agents processed")
```

---

## Quick Reference

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
