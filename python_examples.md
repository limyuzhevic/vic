# NLM Python Examples and Tutorials

This document provides comprehensive examples and tutorials for using the NLM Python API
at various skill levels, from beginner to advanced.

## Quick Start

If you're new to NLM, start with these beginner examples:

```python
# Example 1: Silent Brain
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

```python
# Example 2: Brain Watching a World
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

## Beginner Examples

### Example 1: Basic Neural Activity

```python
import pynlm

def basic_neural_simulation():
    """Demonstrate basic neural activity."""
    print("=== Basic Neural Activity ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Print statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

basic_neural_simulation()
```

### Example 2: Simple World Interaction

```python
import pynlm

def simple_world_interaction():
    """Demonstrate agent-world interaction."""
    print("=== Simple World Interaction ===")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable basic learning
    agent.enableRewardModulation(True)
    
    # Run interaction
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")

# Run the example
simple_world_interaction()
```

### Example 3: Context Manager Usage

```python
import pynlm

def context_manager_example():
    """Demonstrate context manager usage."""
    print("=== Context Manager Usage ===")
    
    # Create sensory inputs using context managers
    with pynlm.Vision.createImage(640, 480) as vision:
        print(f"Created vision: {vision.getWidth()}x{vision.getHeight()}")
        
        # Set some sample data
        data = [0.5] * (640 * 480 * 3)
        vision.setData(data)
        
        # Process the image
        avg_intensity = sum(data) / len(data)
        print(f"Average pixel intensity: {avg_intensity:.3f}")
    
    # Vision context manager has exited, but object still exists
    print(f"Vision object still accessible: {vision.getWidth()}x{vision.getHeight()}")
    
    # Use audio in context manager
    with pynlm.Audio.createAudio(44100, 1024) as audio:
        print(f"Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
        
        # Set audio data
        audio.setData([0.1] * 1024)
        
        # Process audio (just example)
        peak_amplitude = max(abs(x) for x in audio.getData())
        print(f"Peak amplitude: {peak_amplitude:.3f}")
    
    # Use internal signals
    with pynlm.InternalSignals.createSignals() as signals:
        signals.addSignal(1.0)
        signals.addSignal(2.0)
        signals.addSignal(-0.5)
        
        print(f"Processed signals: {signals.getData()}")
        print(f"Average signal: {sum(signals.getData()) / len(signals.getData()):.3f}")

context_manager_example()
```

## Intermediate Examples

### Example 1: Complete Agent Simulation

```python
import pynlm

def complete_agent_simulation(steps=1000):
    """Run a complete agent simulation with all systems."""
    print(f"=== Complete Agent Simulation ({steps} steps) ===")
    
    # Setup phase
    print("Setting up brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)  # Smaller for faster demo
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    
    print("Setting up world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable all learning systems
    print("Enabling learning systems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Simulation phase
    print("Running simulation...")
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Agent selects action
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning based on outcome
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    # Results phase
    print("=== Simulation Results ===")
    print(f"Total simulation steps: {steps}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

# Run the complete simulation
brain, agent, world = complete_agent_simulation(1000)
```

### Example 2: Configuration Management

```python
import pynlm

class NLMSimulationManager:
    """Manage NLM simulations with different configurations."""
    
    def __init__(self):
        self.simulations = {}
    
    def create_simulation(self, name, config_name="default", steps=100):
        """Create a new simulation with specified configuration."""
        print(f"Creating simulation '{name}'...")
        
        if config_name == "small":
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 100)
            config.set("region_count", 1)
        elif config_name == "medium":
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 1000)
            config.set("region_count", 2)
        elif config_name == "large":
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 5000)
            config.set("region_count", 4)
        else:  # default
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable different learning modes based on config
        if config_name in ["medium", "large"]:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        if config_name == "large":
            agent.enableDevelopment(True)
        
        self.simulations[name] = {
            'brain': brain,
            'agent': agent,
            'world': world,
            'config_name': config_name,
            'steps': steps
        }
        
        return self.simulations[name]
    
    def run_simulation(self, name, run_behavior=True):
        """Run a simulation by name."""
        if name not in self.simulations:
            raise ValueError(f"Simulation '{name}' not found")
        
        sim = self.simulations[name]
        brain = sim['brain']
        agent = sim['agent']
        world = sim['world']
        steps = sim['steps']
        
        print(f"Running simulation '{name}'...")
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if run_behavior:
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        print(f"Simulation '{name}' completed!")
        return sim
    
    def compare_simulations(self, *names):
        """Compare results from multiple simulations."""
        print("=== Simulation Comparison ===")
        
        results = []
        for name in names:
            if name in self.simulations:
                sim = self.simulations[name]
                results.append({
                    'name': name,
                    'config': sim['config_name'],
                    'steps': sim['steps'],
                    'final_firing_rate': sim['brain'].getAverageFiringRate(),
                    'final_spikes': sim['brain'].getTotalSpikeCount(),
                    'final_curiosity': sim['agent'].getCuriosityLevel(),
                    'final_novelty': sim['agent'].getNoveltyLevel()
                })
        
        # Print comparison table
        print(f"{'Simulation':<15} {'Config':<10} {'Steps':<8} {'Firing Rate':<15} {'Total Spikes':<12} {'Curiosity':<12} {'Novelty':<12}")
        print("-" * 85)
        for result in results:
            print(f"{result['name']:<15} {result['config']:<10} {result['steps']:<8} {result['final_firing_rate']:<15.3f} {result['final_spikes']:<12} {result['final_curiosity']:<12.3f} {result['final_novelty']:<12.3f}")
        
        return results

# Usage example
manager = NLMSimulationManager()

# Create different simulations
sim1 = manager.create_simulation("SmallScale", "small", 200)
sim2 = manager.create_simulation("MediumScale", "medium", 500)
sim3 = manager.create_simulation("LargeScale", "large", 1000)

# Run all simulations
manager.run_simulation("SmallScale")
manager.run_simulation("MediumScale")
manager.run_simulation("LargeScale")

# Compare results
comparison = manager.compare_simulations("SmallScale", "MediumScale", "LargeScale")
```

### Example 3: Advanced Sensor Processing

```python
import pynlm

def advanced_sensor_processing():
    """Demonstrate advanced sensor processing techniques."""
    print("=== Advanced Sensor Processing ===")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Process sensor data
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory inputs
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        # Advanced vision processing
        if step % 50 == 0:
            width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
            data = vision.getData()
            
            # Calculate image statistics
            avg_intensity = sum(data) / len(data)
            max_intensity = max(data)
            min_intensity = min(data)
            
            print(f"Step {step}: Vision stats - Avg: {avg_intensity:.3f}, Max: {max_intensity:.3f}, Min: {min_intensity:.3f}")
        
        # Advanced signal processing
        if step % 25 == 0:
            signals = percept.getInternal()
            if signals:
                current_signal = signals[0] if signals else 0.0
                processed_signal = current_signal * 0.5 + 0.1 * (step % 10) / 10.0
                print(f"Internal signal: {current_signal:.3f} -> processed: {processed_signal:.3f}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final prediction error: {agent.getPredictionError():.3f}")

advanced_sensor_processing()
```

## Advanced Examples

### Example 1: Multi-Agent Simulation

```python
import pynlm
import threading
import time

def create_agent(name, brain_config):
    """Create an agent with its own brain."""
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    return {
        'name': name,
        'brain': brain,
        'agent': pynlm.createAgentBrain(brain)
    }

def agent_simulation(agent_data, world, steps=100):
    """Run simulation for a single agent."""
    name = agent_data['name']
    brain = agent_data['brain']
    agent = agent_data['agent']
    
    print(f"Agent '{name}' starting simulation...")
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Different agents have different behaviors
        if agent_data['name'] == 'explorer':
            agent.enableCuriosity(True)
        elif agent_data['name'] == 'exploiter':
            agent.enableRewardModulation(True)
        elif agent_data['name'] == 'social':
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
    
    print(f"Agent '{name}' simulation completed!")
    return agent_data

def multi_agent_simulation():
    """Run multi-agent simulation with concurrent agents."""
    print("=== Multi-Agent Simulation ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Create different agent types
    small_config = pynlm.createDefaultConfig()
    small_config.set("neuron_count", 200)
    
    medium_config = pynlm.createDefaultConfig()
    medium_config.set("neuron_count", 500)
    
    large_config = pynlm.createDefaultConfig()
    large_config.set("neuron_count", 1000)
    
    agents = [
        create_agent("explorer", small_config),
        create_agent("exploiter", medium_config),
        create_agent("social", large_config)
    ]
    
    # Initialize agents
    for agent_data in agents:
        agent_data['agent'].initialize(world)
    
    # Run simulations in parallel (simplified version - in practice use proper threading)
    print("Running multi-agent simulation...")
    results = []
    
    for agent_data in agents:
        # Set different starting positions
        offset = len(results) * 10
        world.setAgentStart(25 + offset, 25)
        
        result = agent_simulation(agent_data, world, 200)
        results.append(result)
    
    # Compare agent performance
    print("\n=== Agent Performance Comparison ===")
    for result in results:
        agent_data = result
        brain = agent_data['brain']
        agent = agent_data['agent']
        
        print(f"Agent '{agent_data['name']}':")
        print(f"  Neurons: {brain.getTotalNeuronCount()}, Steps: {len(range(200))}")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
        print()
    
    return results

multi_agent_simulation()
```

### Example 2: Simulation with Learning and Development

```python
import pynlm

def developmental_simulation(total_episodes=50):
    """Simulation with developmental changes over time."""
    print(f"=== Developmental Simulation ({total_episodes} episodes) ===")
    
    # Create agent
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    # Enable all developmental systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Developmental stages
    developmental_stages = [
        (0, 10, "Initial"),
        (10, 25, "Critical Period"),
        (25, 40, "Maturation"),
        (40, total_episodes, "Adult")
    ]
    
    episode = 0
    stage_idx = 0
    current_stage = developmental_stages[stage_idx]
    
    while episode < total_episodes:
        # Update developmental stage if needed
        if episode >= current_stage[1]:
            stage_idx = min(stage_idx + 1, len(developmental_stages) - 1)
            current_stage = developmental_stages[stage_idx]
            print(f"\nStage changed to: {current_stage[2]} (episodes {current_stage[0]}-{current_stage[1]})")
        
        # Run episode
        print(f"\nEpisode {episode} ({current_stage[2]} stage):")
        
        for step in range(50):  # 50 steps per episode
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Developmental learning
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 10 == 0:
                print(f"  Step {step}: Cur={agent.getCuriosityLevel():.3f}, Nov={agent.getNoveltyLevel():.3f}, Dev={brain.getDevelopmentalStage()}")
        
        episode += 1
    
    # Final developmental assessment
    print(f"\n=== Final Developmental Assessment ===")
    print(f"Total episodes: {episode}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Save developmental trajectory
    developmental_trajectory = {
        'stages': developmental_stages,
        'final_stage': brain.getDevelopmentalStage(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel()
    }
    
    return developmental_trajectory

developmental_trajectory = developmental_simulation(50)
```

### Example 3: Performance Benchmarking

```python
import pynlm
import time

def benchmark_brain_performance():
    """Benchmark different brain configurations."""
    print("=== Brain Performance Benchmark ===")
    
    configs = [
        {"name": "Tiny", "neurons": 100, "regions": 1},
        {"name": "Small", "neurons": 500, "regions": 1},
        {"name": "Medium", "neurons": 1000, "regions": 2},
        {"name": "Large", "neurons": 5000, "regions": 4}
    ]
    
    results = []
    
    for config_info in configs:
        print(f"\nBenchmarking {config_info['name']} brain ({config_info['neurons']} neurons, {config_info['regions']} regions)...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", config_info['neurons'])
        config.set("region_count", config_info['regions'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark step performance
        start_time = time.time()
        steps = 100
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        elapsed_time = end_time - start_time
        steps_per_second = steps / elapsed_time
        
        result = {
            'name': config_info['name'],
            'neurons': config_info['neurons'],
            'regions': config_info['regions'],
            'steps_per_second': steps_per_second,
            'total_time': elapsed_time,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        results.append(result)
        
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Total time: {elapsed_time:.2f}s")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Compare results
    print(f"\n=== Performance Comparison ===")
    print(f"{'Config':<10} {'Neurons':<10} {'Regions':<10} {'Steps/s':<12} {'Time (s)':<12} {'Firing Rate':<15} {'Total Spikes':<12}")
    print("-" * 85)
    
    for result in results:
        print(f"{result['name']:<10} {result['neurons']:<10} {result['regions']:<10} {result['steps_per_second']:<12.1f} {result['total_time']:<12.2f} {result['final_firing_rate']:<15.3f} {result['total_spikes']:<12}")
    
    return results

benchmark_results = benchmark_brain_performance()
```

## Real-World Application Examples

### Example 1: Exploration vs Exploitation

```python
import pynlm

def exploration_exploitation_simulation():
    """Demonstrate exploration vs exploitation behavior."""
    print("=== Exploration vs Exploitation Simulation ===")
    
    # Create two agents with different behaviors
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    # Exploration agent (curiosity-driven)
    explorer_brain = pynlm.createBrain(config)
    explorer_brain.initialize()
    explorer_agent = pynlm.createAgentBrain(explorer_brain)
    explorer_agent.enableCuriosity(True)
    explorer_agent.enableRewardModulation(False)
    
    # Exploitation agent (reward-driven)
    exploiter_brain = pynlm.createBrain(config)
    exploiter_brain.initialize()
    exploiter_agent = pynlm.createAgentBrain(exploiter_brain)
    exploiter_agent.enableCuriosity(False)
    exploiter_agent.enableRewardModulation(True)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(5.0, 5.0)  # Explorer start
    
    explorer_agent.initialize(world)
    
    # Set exploiter start position
    exploiter_agent.initialize(world)
    world.setAgentStart(35.0, 35.0)  # Exploiter start
    
    # Run simulation
    for episode in range(100):
        if episode % 2 == 0:  # Explorer's turn
            agent = explorer_agent
            brain = explorer_brain
            position_name = "Explorer"
        else:  # Exploiter's turn
            agent = exploiter_agent
            brain = exploiter_brain
            position_name = "Exploiter"
        
        # Move to agent's position
        world.setAgentStart(world.getAgentBody().x, world.getAgentBody().y)
        
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Different reward signals for different agents
            if episode % 2 == 0:
                # Explorer gets reward for exploring new areas
                reward = 0.1 if step < 10 else 0.0
            else:
                # Exploiter gets reward for consistent performance
                reward = 1.0
            
            agent.applyRewardModulation(reward, 0.0)
        
        # Print results
        if episode % 10 == 0:
            print(f"\nEpisode {episode}:")
            print(f"  Agent: {position_name}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print(f"\n=== Final Results ===")
    print(f"Explorer - Final curiosity: {explorer_agent.getCuriosityLevel():.3f}")
    print(f"Exploiter - Final reward sensitivity: {exploiter_agent.getNeuromodulationLevel():.3f}")
    
    return explorer_agent, exploiter_agent

explorer, exploiter = exploration_exploitation_simulation()
```

### Example 2: Memory-Based Decision Making

```python
import pynlm

def memory_based_decision_making():
    """Demonstrate memory-based decision making."""
    print("=== Memory-Based Decision Making ===")
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create world with resources
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
    world.reset()
    agent.initialize(world)
    
    # Track memory of different locations
    location_memory = {}
    visited_locations = []
    
    for episode in range(200):
        for step in range(30):
            # Update world
            world.update(0.1)
            
            # Get current position
            current_pos = (world.getAgentBody().x, world.getAgentBody().y)
            
            # Check if this location is new
            if current_pos not in visited_locations:
                visited_locations.append(current_pos)
                location_memory[current_pos] = {
                    'explored': False,
                    'reward_history': [],
                    'curiosity_history': []
                }
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Update location memory
            if current_pos in location_memory:
                location_memory[current_pos]['explored'] = True
                location_memory[current_pos]['reward_history'].append(
                    percept.getInternal()[0] if percept.getInternal() else 0.0
                )
                location_memory[current_pos]['curiosity_history'].append(
                    agent.getCuriosityLevel()
                )
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Decision making based on memory
            # If exploring new areas, prioritize curiosity
            new_locations = [loc for loc in visited_locations if not location_memory[loc]['explored']]
            if len(new_locations) > 0 and agent.getCuriosityLevel() > 0.5:
                # Move to new location
                target_location = new_locations[0]
                action_type = pynlm.ActionType.MoveForward  # Simplified
                print(f"Exploring new location: {target_location}")
            else:
                # Continue exploring current area
                action_type = pynlm.ActionType.Look  # Simplified - look around
            
            # Apply action (simplified - would use actual action selection)
            # In real implementation, would use action = agent.decodeMotorCommand()
            world.applyMotorCommand(pynlm.MotorCommand.MoveForward, world.getSimulationTime())
            
            # Apply learning based on experience
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if episode % 20 == 0:
            print(f"\nEpisode {episode}:")
            print(f"  Visited locations: {len(visited_locations)}")
            print(f"  Average curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Average novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Memory locations: {len(location_memory)}")
    
    print(f"\n=== Memory Analysis ===")
    print(f"Total locations visited: {len(visited_locations)}")
    print(f"Memory utilization: {len([m for m in location_memory.values() if m['explored']])}/{len(location_memory)}")
    
    return location_memory, visited_locations

memory, locations = memory_based_decision_making()
```

## Best Practices and Tips

### 1. Use Context Managers for Resource Management

```python
# Good - Use context managers for proper resource management
with pynlm.Vision.createImage(640, 480) as vision:
    # Process vision data
    vision.setData([0.5] * (640 * 480 * 3))

# Good - Factory methods with validation
brain = pynlm.Brain.createQuickBrain()

# Good - Proper error handling
try:
    result = risky_operation()
except Exception as e:
    print(f"Handled error: {e}")
```

### 2. Configure Wisely

```python
# Good - Configure for performance
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Not too small, not too large
config.set("region_count", 2)    # Reasonable number of regions

# Good - Use appropriate timestep
config.set("simulation_timestep", 0.001)  # Balance accuracy and performance
```

### 3. Reuse Objects

```python
# Good - Reuse brain and agent objects
brain = pynlm.Brain.createQuickBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Reset and run multiple simulations
for episode in range(10):
    world.reset()
    agent.initialize(world)
    # Run episode logic...
```

### 4. Monitor and Log

```python
# Good - Monitor performance metrics
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
```

### 5. Error Handling

```python
# Good - Comprehensive error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate for many steps
    for step in range(10000):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation failed: {e}")
    # Recovery or cleanup logic
    brain.reset()
```

## Testing Your Code

Run the NLM test suite to verify your installation:

```bash
# Basic API tests
python test_basic_api.py

# Factory method tests  
python test_factory_methods.py

# Error handling tests
python test_error_handling.py

# Context manager tests
python test_context_managers.py
```

All tests should pass, confirming your NLM Python installation is working correctly.

## Getting Help

### Common Issues and Solutions

**Issue: "ImportError: No module named 'pynlm'"**
```bash
pip install pynlm
# or for development
pip install -e .
```

**Issue: "Brain.initialize() failed"**
```python
# Solution: Check configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Ensure valid configuration
brain = pynlm.createBrain(config)
brain.initialize()  # Should now succeed
```

**Issue: "Context manager doesn't work"**
```python
# Solution: Use factory methods
with pynlm.Vision.createImage(100, 100) as vision:
    # This should work with the new factory methods
    print(f"Vision: {vision.getWidth()}x{vision.getHeight()}")
```

### Online Resources

- **NLM Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **API Reference**: See `python_API.md` for detailed documentation
- **Examples**: See this file for comprehensive examples

## Conclusion

The NLM Python API provides a powerful interface to the Neural Learning Machine neural simulation framework. With enhanced Pythonic patterns, context manager support, and comprehensive factory methods, you can now easily create, configure, and run sophisticated neural simulations.

The examples in this document demonstrate real-world usage patterns, from simple neural activity to complex multi-agent simulations with memory and development systems.

Start with the beginner examples, progress to intermediate examples, and finally explore the advanced examples to make the most of the NLM Python API.