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

**IMPORTANT**: The Python bindings now include advanced features like simulation helpers, performance benchmarking, and data export capabilities!

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

### Pattern 4: Advanced Simulation Control

```python
def advanced_simulation(brain, world, agent, num_steps=1000, 
                         save_interval=100, benchmark=True):
    import time
    import os
    
    print("Starting advanced simulation...")
    
    # Configure world parameters
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Track performance metrics
    performance_data = []
    spike_history = []
    
    start_time = time.time()
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track metrics
        spike_history.append(brain.getFiringNeuronCount())
        performance_data.append({
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate()
        })
        
        # Save checkpoint periodically
        if step % save_interval == 0 and step > 0:
            filename = f"checkpoint_step_{step}.bin"
            brain.save(filename)
            print(f"Checkpoint saved: {filename}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Simulation completed in {duration:.2f} seconds")
    print(f"Average performance: {duration/num_steps*1000:.2f} ms/step")
    
    # Benchmark performance
    if benchmark:
        benchmark_result = brain.benchmarkPerformance(1000)
        print(f"Benchmark results: {benchmark_result}")
    
    # Export performance data
    pynlm.exportBrainData(brain, "performance_data.csv")
    
    # Analyze activity
    analysis = pynlm.analyzeBrainActivity(brain)
    print("Activity Analysis:")
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    return performance_data, spike_history

# Use the advanced simulation
def main():
    import pynlm
    
    # Create brain with advanced configuration
    config = pynlm.createAdvancedConfig(
        neuronCount=1000,
        connectionProb=0.2,
        stdpLTP=0.03,
        stdpLTD=0.02,
        enableDevelopment=True,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run the advanced simulation
    perf_data, spike_data = advanced_simulation(brain, world, agent, num_steps=500)
    
    print("✓ Advanced simulation completed successfully!")
```

### Pattern 5: Training and Learning Simulation

```python
def create_training_simulation(config_file="training_config.json"):
    import pynlm
    import os
    import json
    
    # Load configuration or create default
    try:
        config = pynlm.createDefaultConfig()
        if os.path.exists(config_file):
            config.loadFromFile(config_file)
            print(f"Loaded config from {config_file}")
    except:
        print("Using default configuration")
        config = pynlm.createDefaultConfig()
    
    # Create training world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.95)
    world.reset()
    
    return config, world

def run_training_episode(config, world, episode_id, max_steps=200):
    import pynlm
    
    # Create fresh brain for each episode
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"\nStarting Training Episode {episode_id}")
    print(f"Initial neurons: {brain.getTotalNeuronCount()}")
    print(f"Initial synapses: {brain.getTotalSynapseCount()}")
    
    episode_data = {
        'episode': episode_id,
        'steps': [],
        'total_spikes': 0,
        'final_firing_rate': 0,
        'reward_history': [],
        'performance_metrics': {}
    }
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = result.reward if hasattr(result, 'reward') else 0.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Record data
        episode_data['steps'].append({
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'reward': reward,
            'action': action.getType() if hasattr(action, 'getType') else None
        })
        
        episode_data['total_spikes'] = brain.getTotalSpikeCount()
        
        # End episode if brain dies or time limit reached
        if brain.getFiringNeuronCount() == 0 or step >= max_steps - 1:
            break
    
    episode_data['final_firing_rate'] = brain.getAverageFiringRate()
    episode_data['performance_metrics'] = pynlm.analyzeBrainActivity(brain)
    
    # Save brain snapshot
    snapshot_file = f"training_episode_{episode_id}_brain.bin"
    pynlm.saveBrainSnapshot(brain, snapshot_file, f"Training episode {episode_id}")
    
    print(f"Episode {episode_id} completed:")
    print(f"  Total spikes: {episode_data['total_spikes']}")
    print(f"  Final firing rate: {episode_data['final_firing_rate']:.2f}")
    print(f"  Saved to: {snapshot_file}")
    
    return episode_data, brain

def run_training_session(num_episodes=10, episodes_per_checkpoint=5):
    import pynlm
    import os
    
    # Create training environment
    config, world = create_training_environment()
    
    all_episodes = []
    
    for episode in range(num_episodes):
        episode_data, brain = run_training_episode(config, world, episode)
        all_episodes.append(episode_data)
        
        # Save checkpoint
        if (episode + 1) % episodes_per_checkpoint == 0:
            checkpoint_file = f"training_checkpoint_episode_{episode + 1}.json"
            with open(checkpoint_file, 'w') as f:
                json.dump({
                    'episode': episode + 1,
                    'config_summary': config.summary(),
                    'episodes_data': all_episodes[-episodes_per_checkpoint:]
                }, f, indent=2)
            print(f"Checkpoint saved: {checkpoint_file}")
    
    # Analyze training results
    avg_firing_rate = sum(ep['final_firing_rate'] for ep in all_episodes) / len(all_episodes)
    total_spikes = sum(ep['total_spikes'] for ep in all_episodes)
    
    print(f"\nTraining Complete!")
    print(f"Average firing rate: {avg_firing_rate:.2f}")
    print(f"Total spikes across all episodes: {total_spikes}")
    
    return all_episodes

# Main training example
def main():
    print("=== NLM Training Example ===")
    
    # Run training session
    all_episodes = run_training_session(num_episodes=5, episodes_per_checkpoint=2)
    
    print("\n✓ Training example completed successfully!")
```

### Pattern 6: Visualization and Data Export

```python
def create_viz_config_and_export(brain, world, output_dir="viz_data"):
    import pynlm
    import os
    import json
    
    # Create visualization configuration
    viz_config = pynlm.createVisualizationConfig(
        width=800,
        height=600,
        showNeurons=True,
        showSynapses=True,
        showActivity=True
    )
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Export brain data for visualization
    pynlm.exportBrainData(brain, f"{output_dir}/brain_data.csv")
    
    # Create plotting data
    spike_history, firing_rate_history = pynlm.createBrainPlotData(brain)
    
    # Save plot data
    plot_data = {
       'agriculture_history': spike_history,
        'firing_rate_history': firing_rate_history,
        'timestamp': 'current_time'
    }
    
    with open(f"{output_dir}/plot_data.json", 'w') as f:
        json.dump(plot_data, f)
    
    # Create brain snapshot for visualization tools
    pynlm.saveBrainSnapshot(brain, f"{output_dir}/brain_snapshot.bin", 
                           "Brain state for visualization")
    
    print(f"Visualization data exported to {output_dir}/")
    
    return viz_config, spike_history, firing_rate_history

def main():
    import pynlm
    
    # Setup simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation and collect visualization data
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Create visualization data
    viz_config, spikes, rates = create_viz_config_and_export(brain, world)
    
    print("✓ Visualization data exported successfully!")
```

### Pattern 7: Benchmarking and Performance Analysis

```python
def comprehensive_benchmark(brain, test_scenarios, warmup_steps=100):
    import pynlm
    import time
    
    results = {}
    
    for scenario_name, scenario_config in test_scenarios.items():
        print(f"\nBenchmarking scenario: {scenario_name}")
        
        # Configure brain for this scenario
        brain.configure(scenario_config)
        brain.reset()
        brain.initialize()
        
        # Warmup
        for step in range(warmup_steps):
            brain.step(step)
        
        # Benchmark the scenario
        start_time = time.time()
        scenario_steps = scenario_config.get('steps', 1000)
        
        for step in range(warmup_steps, warmup_steps + scenario_steps):
            brain.step(step)
            
            # Inject current if specified
            if 'current_injection' in scenario_config:
                brain.injectCurrentToNeurons(
                    scenario_config['current_injection']['type'],
                    scenario_config['current_injection']['amount']
                )
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Collect metrics
        metrics = {
            'total_time_ms': duration * 1000,
            'avg_time_per_step_ms': (duration * 1000) / scenario_steps,
            crop_rate, 'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'spikes_per_second': brain.getTotalSpikeCount() / duration,
            'neurons_per_second': brain.getTotalNeuronCount() / duration
        }
        
        results[scenario_name] = metrics
        
        print(f"  Time per step: {metrics['avg_time_per_step_ms']:.2f} ms")
        print(f"  Total spikes: {metrics['total_spikes']}")
        print(f"  Avg firing rate: {metrics['avg_firing_rate']:.2f}")
        
        # Export results
        pynlm.exportBrainData(brain, f"{scenario_name}_results.csv")
    
    return results

def main():
    import pynlm
    
    # Create brain for benchmarking
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Define test scenarios
    test_scenarios = {
        'baseline': {
            'steps': 1000
        },
        'high_activity': {
            'steps': 1000,
            'current_injection': {
                'type': 'Excitatory',
                'amount': 50.0
            }
        },
        'learning': {
            'steps': 2000,
            'enable_plasticity': True
        }
    }
    
    # Run comprehensive benchmark
    results = comprehensive_benchmark(brain, test_scenarios)
    
    print("\n✓ Comprehensive benchmark completed!")
    print("\nBenchmark Summary:")
    for scenario, metrics in results.items():
        print(f"\n{scenario.upper()}:")
        for key, value in metrics.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.2f}")
            else:
                print(f"  {key}: {value}")
```

### Pattern 4: Advanced Simulation Control

```python
def advanced_simulation(brain, world, agent, num_steps=1000, 
                         save_interval=100, benchmark=True):
    import time
    import os
    
    print("Starting advanced simulation...")
    
    # Configure world parameters
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Track performance metrics
    performance_data = []
    spike_history = []
    
    start_time = time.time()
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track metrics
        spike_history.append(brain.getFiringNeuronCount())
        performance_data.append({
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate()
        })
        
        # Save checkpoint periodically
        if step % save_interval == 0 and step > 0:
            filename = f"checkpoint_step_{step}.bin"
            brain.save(filename)
            print(f"Checkpoint saved: {filename}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Simulation completed in {duration:.2f} seconds")
    print(f"Average performance: {duration/num_steps*1000:.2f} ms/step")
    
    # Benchmark performance
    if benchmark:
        benchmark_result = brain.benchmarkPerformance(1000)
        print(f"Benchmark results: {benchmark_result}")
    
    # Export performance data
    pynlm.exportBrainData(brain, "performance_data.csv")
    
    # Analyze activity
    analysis = pynlm.analyzeBrainActivity(brain)
    print("Activity Analysis:")
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    return performance_data, spike_history

# Use the advanced simulation
def main():
    import pynlm
    
    # Create brain with advanced configuration
    config = pynlm.createAdvancedConfig(
        neuronCount=1000,
        connectionProb=0.2,
        stdpLTP=0.03,
        stdpLTD=0.02,
        enableDevelopment=True,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run the advanced simulation
    perf_data, spike_data = advanced_simulation(brain, world, agent, num_steps=500)
    
    print("✓ Advanced simulation completed successfully!")
```

### Pattern 5: Training and Learning Simulation

```python
def create_training_environment(config_file="training_config.json"):
    import pynlm
    import json
    
    # Load configuration or create default
    try:
        config = pynlm.createDefaultConfig()
        if os.path.exists(config_file):
            config.loadFromFile(config_file)
            print(f"Loaded config from {config_file}")
    except:
        print("Using default configuration")
        config = pynlm.createDefaultConfig()
    
    # Create training world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.95)
    world.reset()
    
    return config, world

def run_training_episode(config, world, episode_id, max_steps=200):
    import pynlm
    
    # Create fresh brain for each episode
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"\nStarting Training Episode {episode_id}")
    print(f"Initial neurons: {brain.getTotalNeuronCount()}")
    print(f"Initial synapses: {brain.getTotalSynapseCount()}")
    
    episode_data = {
        'episode': episode_id,
        'steps': [],
        'total_spikes': 0,
        'final_firing_rate': 0,
        'reward_history': [],
        'performance_metrics': {}
    }
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = result.reward if hasattr(result, 'reward') else 0.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Record data
        episode_data['steps'].append({
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'reward': reward,
            'action': action.getType() if hasattr(action, 'getType') else None
        })
        
        episode_data['total_spikes'] = brain.getTotalSpikeCount()
        
        # End episode if brain dies or time limit reached
        if brain.getFiringNeuronCount() == 0 or step >= max_steps - 1:
            break
    
    episode_data['final_firing_rate'] = brain.getAverageFiringRate()
    episode_data['performance_metrics'] = pynlm.analyzeBrainActivity(brain)
    
    # Save brain snapshot
    snapshot_file = f"training_episode_{episode_id}_brain.bin"
    pynlm.saveBrainSnapshot(brain, snapshot_file, f"Training episode {episode_id}")
    
    print(f"Episode {episode_id} completed:")
    print(f"  Total spikes: {episode_data['total_spikes']}")
    print(f"  Final firing rate: {episode_data['final_firing_rate']:.2f}")
    print(f"  Saved to: {snapshot_file}")
    
    return episode_data, brain

def run_training_session(num_episodes=10, episodes_per_checkpoint=5):
    import pynlm
    import os
    
    # Create training environment
    config, world = create_training_environment()
    
    all_episodes = []
    
    for episode in range(num_episodes):
        episode_data, brain = run_training_episode(config, world, episode)
        all_episodes.append(episode_data)
        
        # Save checkpoint
        if (episode + 1) % episodes_per_checkpoint == 0:
            checkpoint_file = f"training_checkpoint_episode_{episode + 1}.json"
            with open(checkpoint_file, 'w') as f:
                json.dump({
                    'episode': episode + 1,
                    'config_summary': config.summary(),
                    'episodes_data': all_episodes[-episodes_per_checkpoint:]
                }, f, indent=2)
            print(f"Checkpoint saved: {checkpoint_file}")
    
    # Analyze training results
    avg_firing_rate = sum(ep['final_firing_rate'] for ep in all_episodes) / len(all_episodes)
    total_spikes = sum(ep['total_spikes'] for ep in all_episodes)
    
    print(f"\nTraining Complete!")
    print(f"Average firing rate: {avg_firing_rate:.2f}")
    print(f"Total spikes across all episodes: {total_spikes}")
    
    return all_episodes

# Main training example
def main():
    print("=== NLM Training Example ===")
    
    # Run training session
    all_episodes = run_training_session(num_episodes=5, episodes_per_checkpoint=2)
    
    print("\n✓ Training example completed successfully!")
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
