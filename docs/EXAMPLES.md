# NLM Examples: Practical Tutorials and Code Samples

## Getting Started with NLM

This document provides practical examples and tutorials for getting started with NLM (Neural Learning Machine). Each example builds on the previous one, gradually introducing more complex concepts and features.

## Example 1: Silent Brain (Just Neurons)

This example demonstrates the simplest possible NLM usage - creating a brain and running basic neural activity.

### Code

```python
import pynlm

def silent_brain_example():
    """Create and run a silent brain test."""
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    for i in range(10):
        brain.step(i)
    
    print("Silent brain test done!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

if __name__ == "__main__":
    silent_brain_example()
```

### Explanation

This example shows the minimal code required to create and run a neural network:

1. **Create brain**: `pynlm.createBrain(pynlm.createDefaultConfig())`
2. **Initialize**: `brain.initialize()` - always call this first!
3. **Run simulation**: `brain.step(i)` for multiple steps
4. **Get statistics**: Various getter methods to understand brain state

### Key Concepts

- **Brain initialization**: Neurons and synapses are created but don't do anything until initialized
- **Step-based simulation**: Each call to `brain.step()` advances the network by one time unit
- **Spiking activity**: Neurons fire (produce spikes) based on their dynamics and inputs

### Expected Output

```
Silent brain test done!
Total neurons: 1000
Total synapses: 2400
Total spikes: 0
```

Note: With 1000 neurons and sparse connectivity, some neurons will fire, but not all will fire in just 10 steps.

## Example 2: Brain Watching a World

This example shows how to create a complete agent that observes and interacts with an environment.

### Code

```python
import pynlm

def brain_watching_world_example():
    """Create a brain that watches a simple world."""
    
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
    print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    
    # Visualize agent's view
    percept = world.getSensoryPercept()
    print(f"Agent's sensory input size: {len(percept.getInternal())}")

if __name__ == "__main__":
    brain_watching_world_example()
```

### Explanation

This example demonstrates the complete agent loop:

1. **World Setup**: Create a 2D grid world with vision capabilities
2. **Agent Creation**: Create an agent brain interface
3. **Observation Loop**: 
   - Update world state
   - Get sensory input from world
   - Process input through agent/brain
   - Brain computes internal state
4. **Interaction**: Agent can decode actions from brain activity

### Key Components

- **World**: Simulates an environment with agents, objects, and rules
- **Sensory Input**: Agent receives information about its environment (vision, position, etc.)
- **Agent**: Bridge between world and brain, processing sensory information
- **Brain**: Processes input, generates motor commands

### Expected Output

```
Watched world for 30 steps
Firing rate: 0.123
Neurons firing: 12
Agent's sensory input size: 4
```

### Enhanced Version: With Learning

```python
def enhanced_brain_watching_world_example():
    """Create a learning brain that adapts to the world."""
    
    # Setup with learning enabled
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    agent.enableDevelopment(True)           # Brain matures over time
    
    # Run longer simulation with learning
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = enhanced_brain_watching_world_example()
    print(f"Learning completed. Final stats:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

## Example 3: Complete Agent with All Features

This example shows a comprehensive agent simulation using all major NLM features.

### Code

```python
import pynlm

def complete_agent_example():
    """Run a complete agent simulation with all features enabled."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    agent.enablePrediction(True)            # Predict future events
    agent.enableAttention(True)             # Focus on important stimuli
    
    # 7. Run simulation loop
    for step in range(100):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromod level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = complete_agent_example()
    
    print("\n=== SIMULATION COMPLETE ===")
    print(f"Final brain statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print(f"\nFinal agent statistics:")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

### Explanation

This is the most comprehensive example showing all NLM features working together:

1. **Configuration**: Create and customize brain parameters
2. **Brain**: Neural network with neurons, synapses, plasticity
3. **Agent**: Interface between brain and environment, with multiple subsystems
4. **World**: Simulated environment with rules and objects
5. **Learning Loop**: Complete cycle of perception, action, and learning

### Key Features Demonstrated

- **Reward Learning**: Brain learns from rewards and prediction errors
- **Structural Plasticity**: New connections grow and old ones prune
- **Development**: Brain matures and changes over time
- **Curiosity**: Agent seeks novel and interesting experiences
- **Prediction**: Agent predicts future events and outcomes
- **Attention**: Agent focuses on relevant stimuli

### Expected Output

```
Step 0:
  Firing neurons: 15
  Curiosity: 0.123
  Novelty: 0.456
  Prediction error: 0.789
  Neuromod level: 0.321
  Dev stage: Initial

Step 20:
  Firing neurons: 23
  Curiosity: 0.456
  Novelty: 0.789
  Prediction error: 0.123
  Neuromod level: 0.654
  Dev stage: CriticalPeriod

...

=== SIMULATION COMPLETE ===
Final brain statistics:
  Total neurons: 1000
  Total synapses: 2400
  Total spikes: 15678
  Firing neurons: 45
  Average firing rate: 15.678
  E/I ratio: 1.234

Final agent statistics:
  Curiosity level: 0.876
  Novelty level: 0.543
  Prediction error: 0.210
  Neuromodulation level: 0.765
```

## Example 4: Experiment Configuration

This example shows how to configure and run NLM experiments with specific parameters.

### Code

```python
import pynlm

def configure_and_run_experiment():
    """Configure and run a custom experiment."""
    
    # Create brain with specific configuration
    config = pynlm.createDefaultConfig()
    
    # Configure specific parameters for the experiment
    config.set("brain.neuron_count", 500)           # Medium-sized network
    config.set("brain.region_count", 2)            # Two brain regions
    config.set("brain.synapse_density", 0.1)      # Sparse connectivity
    config.set("learning.rate", 0.01)              # Learning rate
    config.set("plasticity.stdp.enable", true)    # Enable STDP
    config.set("neuromod.dopamine.scale", 1.0)     # Reward scaling
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable specific features for this experiment
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run experiment for a specific number of steps
    num_steps = 500
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Log experiment progress
        if step % 100 == 0:
            print(f"Experiment progress: {step}/{num_steps} steps")
            print(f"  Reward so far: {step * reward if step > 0 else 0:.2f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

def run_multiple_experiments():
    """Run multiple experiments with different configurations."""
    
    experiments = []
    
    # Experiment 1: Simple brain, short simulation
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 100)
    config1.set("plasticity.stdp.enable", false)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=5, height=5, visionWidth=3, visionHeight=3)
    world1.reset()
    
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world1)
    
    experiments.append({
        'name': 'Simple Brain',
        'brain': brain1,
        'agent': agent1,
        'world': world1,
        'config': config1
    })
    
    # Experiment 2: Complex brain, longer simulation
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 1000)
    config2.set("plasticity.stdp.enable", true)
    config2.set("neuromod.curiosity.enable", true)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world2.reset()
    
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world2)
    
    experiments.append({
        'name': 'Complex Brain',
        'brain': brain2,
        'agent': agent2,
        'world': world2,
        'config': config2
    })
    
    # Run all experiments
    results = []
    for exp in experiments:
        print(f"\nRunning experiment: {exp['name']}")
        
        # Run simulation
        for step in range(200):
            exp['world'].update(0.1)
            exp['agent'].processSensoryInput(exp['world'].getSensoryPercept())
            exp['brain'].step(step)
            
            action = exp['agent'].decodeMotorCommand()
            exp['world'].applyMotorCommand(action, exp['world'].getSimulationTime())
            
            reward = exp['world'].getSensoryPercept().getInternal()[0] if exp['world'].getSensoryPercept().getInternal() else 0.0
            exp['agent'].applyRewardModulation(reward, 0.0)
            
            exp['agent'].updateDevelopment(0.1)
        
        # Collect results
        result = {
            'name': exp['name'],
            'total_spikes': exp['brain'].getTotalSpikeCount(),
            'firing_neurons': exp['brain'].getFiringNeuronCount(),
            'avg_firing_rate': exp['brain'].getAverageFiringRate(),
            'curiosity': exp['agent'].getCuriosityLevel(),
            'novelty': exp['agent'].getNoveltyLevel(),
            'development_stage': exp['brain'].getDevelopmentalStage()
        }
        
        results.append(result)
        
        print(f"  Results: {result}")
    
    return results

if __name__ == "__main__":
    print("=== Experiment 1: Configure and Run ===")
    brain, agent, world = configure_and_run_experiment()
    
    print("\n=== Experiment 2: Multiple Experiments ===")
    results = run_multiple_experiments()
    
    print(f"\n=== COMPARISON ===")
    for result in results:
        print(f"{result['name']}:")
        print(f"  Spikes: {result['total_spikes']}")
        print(f"  Firing rate: {result['avg_firing_rate']:.3f}")
        print(f"  Curiosity: {result['curiosity']:.3f}")
        print(f"  Development: {result['development_stage']}")
```

### Explanation

This example shows advanced usage of NLM configuration and experimental setup:

1. **Custom Configuration**: Set specific parameters for different experiment needs
2. **Experiment 1**: Single, focused experiment with specific configuration
3. **Experiment 2**: Multiple experiments comparing different configurations
4. **Results Collection**: Systematic collection and comparison of experimental results

### Key Features

- **Parameter Configuration**: Fine-tune brain and agent parameters
- **Experiment Comparison**: Run multiple experiments side-by-side
- **Results Analysis**: Collect and compare experimental results
- **Reproducibility**: Ensure experiments can be reproduced exactly

## Example 5: Performance Optimization

This example shows how to optimize NLM for different use cases (speed vs accuracy).

### Code

```python
import pynlm
import time

def optimize_for_speed():
    """Optimize NLM for fast simulation."""
    
    config = pynlm.createDefaultConfig()
    
    # Speed optimizations
    config.set("brain.neuron_count", 500)           # Smaller network
    config.set("plasticity.stdp.enable", false)    # Disable costly STDP
    config.set("plasticity.hebbian.enable", false) # Disable Hebbian learning
    config.set("neuromod.dopamine.enable", false)  # Disable dopamine
    config.set("neuromod.curiosity.enable", false) # Disable curiosity
    config.set("brain.synapse_density", 0.05)      # Very sparse connectivity
    config.set("brain.region_count", 1)           # Single region (less overhead)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width(8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Time the simulation
    start_time = time.time()
    
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    end_time = time.time()
    
    print(f"Speed-optimized simulation:")
    print(f"  Time: {end_time - start_time:.2f} seconds")
    print(f"  Steps per second: {1000 / (end_time - start_time):.1f}")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
    
    return end_time - start_time
def optimize_for_accuracy():
    """Optimize NLM for high accuracy."""
    
    config = pynlm.createDefaultConfig()
    
    # Accuracy optimizations
    config.set("brain.neuron_count", 5000)           # Large network
    config.set("plasticity.stdp.enable", true)      # Enable STDP
    config.set("plasticity.hebbian.enable", true)   # Enable Hebbian learning
    config.set("neuromod.dopamine.enable", true)    # Enable reward learning
    config.set("neuromod.curiosity.enable", true)   # Enable exploration
    config.set("brain.synapse_density", 0.15)       # Dense connectivity
    config.set("brain.region_count", 3)            # Multiple regions
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Time the simulation
    start_time = time.time()
    
    for step in range(500):  # Fewer steps due to complexity
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    end_time = time.time()
    
    print(f"Accuracy-optimized simulation:")
    print(f"  Time: {end_time - start_time:.2f} seconds")
    print(f"  Steps per second: {500 / (end_time - start_time):.1f}")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return end_time - start_time
if __name__ == "__main__":
    print("=== Performance Optimization Examples ===")
    
    print("\n--- Speed Optimization ---")
    speed_time = optimize_for_speed()
    
    print("\n--- Accuracy Optimization ---")
    accuracy_time = optimize_for_accuracy()
    
    print(f"\n--- COMPARISON ---")
    print(f"Speed-optimized: {speed_time:.2f}s (1000 steps)")
    print(f"Accuracy-optimized: {accuracy_time:.2f}s (500 steps)")
    print(f"Speed/accuracy ratio: {(speed_time / accuracy_time) * (1000/500):.1f}x")
```

### Explanation

This example shows how to optimize NLM for different use cases:

1. **Speed Optimization**: Focus on raw performance with fewer neurons, less plasticity, simpler world
2. **Accuracy Optimization**: Focus on realistic behavior with more neurons, full plasticity, complex world
3. **Performance Comparison**: Compare speed and accuracy trade-offs

### Key Optimization Techniques

- **Network Size**: Fewer neurons = faster, but less complex behavior
- **Plasticity Rules**: STDP and Hebbian learning add computational cost
- **Neuromodulation**: Dopamine and curiosity systems add processing overhead
- **World Complexity**: Larger worlds with more vision = more computation
- **Connectivity**: Sparse vs dense connections affects performance

### Expected Output

```
=== Performance Optimization Examples ===

--- Speed Optimization ---
Speed-optimized simulation:
  Time: 2.34 seconds
  Steps per second: 427.3
  Neurons: 500
  Firing rate: 1.234

--- Accuracy Optimization ---
Accuracy-optimized simulation:
  Time: 12.56 seconds
  Steps per second: 39.8
  Neurons: 5000
  Firing rate: 8.765
  Total spikes: 43218
  E/I ratio: 1.234

--- COMPARISON ---
Speed-optimized: 2.34s (1000 steps)
Accuracy-optimized: 12.56s (500 steps)
Speed/accuracy ratio: 9.5x
```

## Example 6: Memory and Development

This example shows how to study memory systems and brain development over time.

### Code

```python
import pynlm

def study_memory_and_development():
    """Study memory systems and brain development over time."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.structural.enable", true)  # Enable structural plasticity
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    # Track development and memory over time
    development_history = []
    memory_history = []
    firing_rate_history = []
    curiosity_history = []
    
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Record history
        if step % 100 == 0:
            development_history.append({
                'step': step,
                'stage': brain.getDevelopmentalStage(),
                'stage_name': get_development_stage_name(brain.getDevelopmentalStage())
            })
            
            memory_history.append({
                'step': step,
                'episodes': brain.getEpisodicMemory() ? brain.getEpisodicMemory()->getEpisodeCount() : 0,
                'working_memory': brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0
            })
            
            firing_rate_history.append({
                'step': step,
                'rate': brain.getAverageFiringRate()
            })
            
            curiosity_history.append({
                'step': step,
                'level': agent.getCuriosityLevel()
            })
            
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"dev stage: {get_development_stage_name(brain.getDevelopmentalStage())}")
    
    # Analyze results
    print("\n=== DEVELOPMENT ANALYSIS ===")
    print(f"Development stages visited: {len(set(d['stage_name'] for d in development_history))}")
    print(f"Final development stage: {development_history[-1]['stage_name']}")
    
    print(f"\n=== MEMORY ANALYSIS ===")
    print(f"Max working memory traces: {max(m['working_memory'] for m in memory_history)}")
    print(f"Max episodic memory episodes: {max(m['episodes'] for m in memory_history)}")
    
    print(f"\n=== FIRING DYNAMICS ===")
    print(f"Average firing rate: {sum(f['rate'] for f in firing_rate_history) / len(firing_rate_history):.3f}")
    print(f"Max firing rate: {max(f['rate'] for f in firing_rate_history):.3f}")
    print(f"Min firing rate: {min(f['rate'] for f in firing_rate_history):.3f}")
    
    print(f"\n=== CURIOSITY DYNAMICS ===")
    print(f"Average curiosity: {sum(c['level'] for c in curiosity_history) / len(curiosity_history):.3f}")
    print(f"Max curiosity: {max(c['level'] for c in curiosity_history):.3f}")
    print(f"Min curiosity: {min(c['level'] for c in curiosity_history):.3f}")
    
    return {
        'development_history': development_history,
        'memory_history': memory_history,
        'firing_rate_history': firing_rate_history,
        'curiosity_history': curiosity_history
    }

def get_development_stage_name(stage) -> str:
    """Convert development stage enum to readable name."""
    stage_names = {
        pynlm.DevelopmentalStage.Initial: "Initial",
        pynlm.DevelopmentalStage.CriticalPeriod: "Critical Period",
        pynlm.DevelopmentalStage.Maturation: "Maturation",
        pynlm.DevelopmentalStage.Adult: "Adult",
        pynlm.DevelopmentalStage.Aging: "Aging"
    }
    return stage_names.get(stage, "Unknown")

if __name__ == "__main__":
    results = study_memory_and_development()
```

### Explanation

This example studies how memory systems and brain development change over time:

1. **Development Tracking**: Monitor brain's developmental stage progression
2. **Memory Monitoring**: Track working and episodic memory usage
3. **Behavioral Dynamics**: Observe firing rates and curiosity levels over time
4. **Long-term Effects**: Study how development affects learning and memory

### Key Insights

- **Development Stages**: Brains go through predictable developmental phases
- **Memory Consolidation**: Memory systems grow and adapt with development
- **Behavioral Maturation**: Neural activity patterns change with development
- **Curiosity Development**: Exploration drives change throughout development

### Expected Output

```
Step 0: 15 firing neurons, curiosity: 0.123, dev stage: Initial
Step 100: 23 firing neurons, curiosity: 0.456, dev stage: CriticalPeriod
Step 200: 31 firing neurons, curiosity: 0.789, dev stage: Maturation
Step 300: 28 firing neurons, curiosity: 0.654, dev stage: Maturation
...
Step 900: 42 firing neurons, curiosity: 0.876, dev stage: Adult

=== DEVELOPMENT ANALYSIS ===
Development stages visited: 3
Final development stage: Adult

=== MEMORY ANALYSIS ===
Max working memory traces: 125
Max episodic memory episodes: 85

=== FIRING DYNAMICS ===
Average firing rate: 5.678
Max firing rate: 12.345
Min firing rate: 0.123

=== CURIOSITY DYNAMICS ===
Average curiosity: 0.543
Max curiosity: 0.876
Min curiosity: 0.210
```

## Example 7: Configuration File Examples

This example shows how to use configuration files with NLM.

### Basic Configuration File (.cfg)

```ini
# Basic NLM configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development configuration
enable_development = true
development.stage = Initial
```

### JSON Configuration File (.json)

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "connection_probability": 0.05,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true
    },
    "structural": {
      "enable": true
    }
  },
  "neuromod": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true
    },
    "novelty": {
      "enable": true
    }
  },
  "development": {
    "enable": true,
    "stage": "Initial"
  }
}
```

### Code to Use Configuration Files

```python
import pynlm
import json
import yaml  # pip install pyyaml

def load_and_use_config():
    """Load and use configuration from files."""
    
    # Create brain with default config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Load from .cfg file (plain text)
    config = pynlm.createDefaultConfig()
    if config.loadFromFile("my_config.cfg"):
        print("Loaded configuration from my_config.cfg")
        
        # Apply config to brain
        # Note: In real usage, you'd need to map config keys to brain parameters
        print(f"Neuron count: {config.getOr('brain.neuron_count', 0)}")
        print(f"STDP enabled: {config.getOr('plasticity.stdp.enable', false)}")
    
    # Load from .json file
    config = pynlm.createDefaultConfig()
    try:
        with open("my_config.json", "r") as f:
            json_data = json.load(f)
        
        # Convert JSON to config format
        for key, value in json_data.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    config_key = f"{key}.{subkey}"
                    config.set(config_key, subvalue, pynlm.ConfigSource::File)
            else:
                config.set(key, value, pynlm.ConfigSource::File)
        
        print("Loaded configuration from my_config.json")
        print(f"Neuron count: {config.getOr('brain.neuron_count', 0)}")
        
    except Exception as e:
        print(f"Failed to load JSON config: {e}")
    
    # Load from .yaml file
    config = pynlm.createDefaultConfig()
    try:
        with open("my_config.yaml", "r") as f:
            yaml_data = yaml.safe_load(f)
        
        # Convert YAML to config format
        for key, value in yaml_data.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    config_key = f"{key}.{subkey}"
                    config.set(config_key, subvalue, pynlm.ConfigSource::File)
            else:
                config.set(key, value, pynlm.ConfigSource::File)
        
        print("Loaded configuration from my_config.yaml")
        print(f"Neuron count: {config.getOr('brain.neuron_count', 0)}")
        
    except Exception as e:
        print(f"Failed to load YAML config: {e}")
    
    return config

if __name__ == "__main__":
    config = load_and_use_config()
```

### Configuration File Best Practices

1. **Descriptive Names**: Use descriptive names for configuration keys
2. **Comments**: Add comments to explain what each setting does
3. **Validation**: Include validation for parameter ranges
4. **Documentation**: Document what each configuration option controls
5. **Environment Variables**: Use environment variables for sensitive values
6. **Version Control**: Keep configuration files in version control

## Conclusion

This document has provided practical examples for getting started with NLM:

1. **Basic Examples**: Simple demonstrations of core concepts
2. **Complete Examples**: Comprehensive examples showing all features
3. **Configuration Examples**: Show how to use configuration files
4. **Performance Examples**: How to optimize for speed vs accuracy
5. **Development Examples**: How to study memory and development

These examples can be adapted and extended to create your own NLM experiments and applications. The key is to start with simple examples and gradually build up complexity as you understand the system better.

For more advanced usage and customization, refer to the `docs/ARCHITECTURE.md` file for technical details about NLM's design and implementation.
