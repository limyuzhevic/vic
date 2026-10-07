# NLM - Advanced User Guide

## Overview

This guide covers advanced usage patterns, debugging techniques, and performance optimization for the NLM (Neural Learning Machine) Python bindings. It builds upon the basics covered in the Easy Usage Guide and targets experienced users who want to leverage the full power of the NLM simulation framework.

## Table of Contents

1. [Advanced Brain Configuration](#advanced-brain-configuration)
2. [Performance Optimization](#performance-optimization)
3. [Advanced Simulation Control](#advanced-simulation-control)
4. [Debugging and Monitoring](#debugging-and-monitoring)
5. [Memory Management and Resource Control](#memory-management-and-resource-control)
6. [Advanced Agent Systems](#advanced-agent-systems)
7. [Neuromodulation Deep Dive](#neuromodulation-deep-dive)
8. [Custom Sensory Processing](#custom-sensory-processing)
9. [Advanced Motor Commands](#advanced-motor-commands)
10. [Experiment Design and Analysis](#experiment-design-and-analysis)
11. [Integration with External Systems](#integration-with-external-systems)
12. [Best Practices](#best-practices)

---

## Advanced Brain Configuration

### Custom Brain Initialization

```python
import pynlm
import json

# Load configuration from JSON file
config = pynlm.createDefaultConfig()
with open("advanced_config.json", "r") as f:
    config_data = json.load(f)

# Apply configuration
for key, value in config_data.items():
    config.set(key, value)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Configuration Parameters

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `brain.neuron_count` | int | 100-10000 | Total number of neurons |
| `brain.synapse_density` | float | 0.01-0.5 | Connection probability |
| `brain.initial_weight_mean` | float | -1.0 to 1.0 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -60.0 to -30.0 | Neuron threshold potential |
| `plasticity.stdp.enable` | bool | true/false | Enable STDP learning |
| `neuromod.dopamine.scale` | float | 0.0 to 10.0 | Dopamine modulation strength |

### Developmental Stages Control

```python
# Set specific developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Monitor developmental progress
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Force stage transition
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
```

---

## Performance Optimization

### Large-Scale Brain Configuration

```python
import time
import pynlm

# Optimized configuration for large brains
def create_large_brain(neuron_count=5000):
    config = pynlm.createDefaultConfig()
    
    # Optimize neuron count
    config.set("brain.neuron_count", neuron_count)
    
    # Optimize synapse density for scale
    if neuron_count > 1000:
        config.set("brain.synapse_density", 0.05)  # Lower density for large brains
    else:
        config.set("brain.synapse_density", 0.1)
    
    # Optimize plasticity for speed
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", False)  # Disable for speed
    config.set("plasticity.structural.enable", True)  # Keep structural for development
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

# Test performance
brain = create_large_brain(2000)
start_time = time.time()

for step in range(100):
    brain.step(step)

end_time = time.time()
print(f"Processed 100 steps in {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 100:.4f} seconds")
```

### Batch Processing Optimization

```python
import pynlm
import numpy as np

def batch_simulation(num_brains=10, steps=50):
    """Efficient batch simulation using parallel processing."""
    
    # Create brains with different configurations
    brains = []
    for i in range(num_brains):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500 + i * 100)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Run batch simulation
    start_time = time.time()
    
    for step in range(steps):
        for brain in brains:
            brain.step(step)
    
    end_time = time.time()
    
    # Collect statistics
    total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
    avg_firing = sum(brain.getAverageFiringRate() for brain in brains) / num_brains
    
    return {
        "total_time": end_time - start_time,
        "avg_time_per_brain": (end_time - start_time) / num_brains,
        "total_spikes": total_spikes,
        "avg_firing_rate": avg_firing,
        "brains": brains
    }

# Run batch simulation
results = batch_simulation(5, 100)
print(f"Batch simulation completed in {results['total_time']:.2f} seconds")
print(f"Average per brain: {results['avg_time_per_brain']:.4f} seconds")
print(f"Total spikes across all brains: {results['total_spikes']}")
```

---

## Advanced Simulation Control

### Simulation with Timestamps

```python
import pynlm
import time

def time_stamped_simulation(num_steps=1000, timestep=0.1):
    """Simulation with precise timestamping."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world for environment interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    simulation_time = 0.0
    step_data = []
    
    for step in range(num_steps):
        # Update world with precise timing
        world.update(timestep)
        simulation_time += timestep
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with brain
        agent.processSensoryInput(percept)
        brain.step(step, simulation_time)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, simulation_time)
        
        # Get reward and apply neuromodulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Record step data
        step_data.append({
            "step": step,
            "time": simulation_time,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "reward": reward,
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "dev_stage": brain.getDevelopmentalStage()
        })
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}, Time: {simulation_time:.1f}, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return step_data, brain, agent, world

# Run time-stamped simulation
step_data, brain, agent, world = time_stamped_simulation(500, 0.05)
```

### Checkpoint and Recovery

```python
import pynlm
import os

def checkpointed_simulation(checkpoint_dir="checkpoints"):
    """Simulation with automatic checkpointing and recovery."""
    
    # Create checkpoint directory
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    episode = 0
    max_episodes = 10
    steps_per_episode = 200
    
    while episode < max_episodes:
        # Try to load from checkpoint
        checkpoint_file = os.path.join(checkpoint_dir, f"episode_{episode:03d}.bin")
        if os.path.exists(checkpoint_file):
            print(f"Loading checkpoint from {checkpoint_file}")
            brain.load(checkpoint_file)
        else:
            print(f"Starting new episode {episode}")
        
        # Run episode
        episode_start = brain.getTotalSpikeCount()
        
        for step in range(steps_per_episode):
            # Episode logic here...
            brain.step(step)
            
            # Save checkpoint periodically
            if step % 50 == 0:
                checkpoint_file = os.path.join(checkpoint_dir, f"episode_{episode:03d}_step_{step:03d}.bin")
                brain.save(checkpoint_file)
                print(f"Checkpoint saved: {checkpoint_file}")
        
        # Save final episode checkpoint
        final_checkpoint = os.path.join(checkpoint_dir, f"episode_{episode:03d}_final.bin")
        brain.save(final_checkpoint)
        
        episode += 1
        
        print(f"Episode {episode-1} completed. Spikes: {brain.getTotalSpikeCount() - episode_start}")
    
    return brain, agent, world

# Run checkpointed simulation
brain, agent, world = checkpointed_simulation()
```

---

## Debugging and Monitoring

### Brain State Monitoring

```python
import pynlm
import time
import matplotlib.pyplot as plt

def monitor_brain_dynamics(brain, world, agent, num_steps=1000):
    """Monitor and visualize brain dynamics in real-time."""
    
    # Initialize monitoring data
    monitoring_data = {
        "time": [],
        "firing_rate": [],
        "spike_count": [],
        "curiosity": [],
        "novelty": [],
        "prediction_error": [],
        "dev_stage": [],
        "energy": world.getAgentBody().energy if world else 0.0
    }
    
    for step in range(num_steps):
        # Step simulation
        brain.step(step)
        
        # Collect metrics
        monitoring_data["time"].append(step)
        monitoring_data["firing_rate"].append(brain.getAverageFiringRate())
        monitoring_data["spike_count"].append(brain.getTotalSpikeCount())
        
        # Agent metrics (if available)
        if agent:
            monitoring_data["curiosity"].append(agent.getCuriosityLevel())
            monitoring_data["novelty"].append(agent.getNoveltyLevel())
            monitoring_data["prediction_error"].append(agent.getPredictionError())
        
        monitoring_data["dev_stage"].append(brain.getDevelopmentalStage())
        
        # Print debug info
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            
            if agent:
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    return monitoring_data

# Run monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

monitoring_data = monitor_brain_dynamics(brain, world, agent, 500)

# Visualization
plt.figure(figsize=(15, 10))

plt.subplot(3, 2, 1)
plt.plot(monitoring_data["time"], monitoring_data["firing_rate"])
plt.xlabel("Step")
plt.ylabel("Firing Rate (Hz)")
plt.title("Brain Firing Rate Over Time")
plt.grid(True)

plt.subplot(3, 2, 2)
plt.plot(monitoring_data["time"], monitoring_data["spike_count"])
plt.xlabel("Step")
plt.ylabel("Total Spike Count")
plt.title("Cumulative Spike Count")
plt.grid(True)

if monitoring_data["curiosity"]:
    plt.subplot(3, 2, 3)
    plt.plot(monitoring_data["time"], monitoring_data["curiosity"])
    plt.xlabel("Step")
    plt.ylabel("Curiosity Level")
    plt.title("Curiosity Dynamics")
    plt.grid(True)

if monitoring_data["novelty"]:
    plt.subplot(3, 2, 4)
    plt.plot(monitoring_data["time"], monitoring_data["novelty"])
    plt.xlabel("Step")
    plt.ylabel("Novelty Level")
    plt.title("Novelty Dynamics")
    plt.grid(True)

plt.subplot(3, 2, 5)
stages = [str(stage) for stage in monitoring_data["dev_stage"]]
stage_counts = {}
for stage in stages:
    stage_counts[stage] = stage_counts.get(stage, 0) + 1

plt.bar(stage_counts.keys(), stage_counts.values())
plt.xlabel("Developmental Stage")
plt.ylabel("Number of Steps")
plt.title("Developmental Stage Distribution")

plt.tight_layout()
plt.show()
```

### Debug Mode

```python
import pynlm

def debug_brain_simulation(brain, world, agent, num_steps=200):
    """Run simulation with enhanced debugging output."""
    
    print("=== DEBUG MODE ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    debug_info = {
        "step": [],
        "neuron_states": [],
        "synapse_states": [],
        "errors": []
    }
    
    for step in range(num_steps):
        # Check for potential issues
        firing_count = brain.getFiringNeuronCount()
        if firing_count == 0:
            print(f"WARNING: No neurons firing at step {step}")
        
        if firing_count > brain.getTotalNeuronCount() * 0.8:
            print(f"WARNING: Very high firing rate at step {step}")
        
        # Step brain
        brain.step(step)
        
        # Record debug info
        debug_info["step"].append(step)
        debug_info["neuron_states"].append({
            "total": brain.getTotalNeuronCount(),
            "active": brain.getActiveNeuronCount(),
            "firing": firing_count,
            "rate": brain.getAverageFiringRate()
        })
        
        # Debug agent if available
        if agent:
            action = agent.decodeMotorCommand()
            print(f"Step {step}: Action = {action}")
            
            debug_info["neuron_states"][-1].update({
                "action": action,
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel()
            })
        
        # Debug world if available
        if world:
            world.update(0.1)
            body = world.getAgentBody()
            print(f"Step {step}: Position ({body.x:.2f}, {body.y:.2f}), Energy: {body.energy:.2f}")
            
            debug_info["neuron_states"][-1].update({
                "position_x": body.x,
                "position_y": body.y,
                "energy": body.energy
            })
    
    print("=== DEBUG SUMMARY ===")
    total_steps = len(debug_info["step"])
    avg_firing_rate = sum(item["rate"] for item in debug_info["neuron_states"]) / total_steps
    max_firing = max(item["firing"] for item in debug_info["neuron_states"])
    
    print(f"Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"Maximum neurons firing: {max_firing} out of {brain.getTotalNeuronCount()}")
    
    return debug_info

# Run debug simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

debug_results = debug_brain_simulation(brain, world, agent, 100)
```

---

## Memory Management and Resource Control

### Brain Resource Management

```python
import pynlm
import gc
import psutil
import os

def optimize_brain_memory(brain, world=None, agent=None):
    """Optimize memory usage for brain simulation."""
    
    print("=== Memory Optimization ===")
    
    # Get current memory usage
    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Clear any cached references
    if world:
        world = None
    
    if agent:
        agent = None
    
    # Force garbage collection
    gc.collect()
    
    # Get memory after cleanup
    after_gc_memory = process.memory_info().rss / 1024 / 1024
    
    print(f"Initial memory: {initial_memory:.2f} MB")
    print(f"After GC: {after_gc_memory:.2f} MB")
    print(f"Memory freed: {initial_memory - after_gc_memory:.2f} MB")
    
    return brain

def brain_memory_profiler(brain, num_steps=500):
    """Profile memory usage during brain simulation."""
    
    print("=== Brain Memory Profiling ===")
    
    process = psutil.Process(os.getpid())
    
    # Record initial state
    initial_neurons = brain.getTotalNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    initial_memory = process.memory_info().rss / 1024 / 1024
    
    memory_samples = []
    neuron_samples = []
    spike_samples = []
    
    for step in range(num_steps):
        brain.step(step)
        
        # Sample memory every 50 steps
        if step % 50 == 0:
            current_memory = process.memory_info().rss / 1024 / 1024
            memory_samples.append(current_memory)
            neuron_samples.append(brain.getTotalNeuronCount())
            spike_samples.append(brain.getTotalSpikeCount())
            
            print(f"Step {step}: Memory={current_memory:.2f}MB, "
                  f"Neurons={brain.getTotalNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    # Calculate memory growth
    final_memory = process.memory_info().rss / 1024 / 1024
    memory_growth = final_memory - initial_memory
    
    print(f"\nMemory Growth: {memory_growth:.2f} MB")
    print(f"Growth rate: {memory_growth / num_steps:.4f} MB per step")
    
    # Memory efficiency metrics
    memory_per_neuron = (final_memory - initial_memory) / (initial_neurons - initial_spikes)
    print(f"Memory per neuron: {memory_per_neuron:.6f} MB")
    
    return {
        "initial_memory": initial_memory,
        "final_memory": final_memory,
        "memory_growth": memory_growth,
        "memory_samples": memory_samples,
        "neuron_samples": neuron_samples,
        "spike_samples": spike_samples
    }

# Test memory optimization and profiling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run memory optimization
optimized_brain = optimize_brain_memory(brain)

# Run memory profiling
memory_profile = brain_memory_profiler(optimized_brain, 200)
```

### Resource Usage Monitoring

```python
import pynlm
import time
import threading
from datetime import datetime

class ResourceMonitor:
    """Monitor resource usage during brain simulation."""
    
    def __init__(self):
        self.samples = []
        self.monitoring = False
        
    def monitor_resources(self, brain, interval=0.1):
        """Continuously monitor resource usage."""
        
        self.monitoring = True
        
        while self.monitoring:
            timestamp = datetime.now()
            
            # Get system metrics
            import psutil
            cpu_percent = psutil.cpu_percent()
            memory_info = psutil.virtual_memory()
            memory_percent = memory_info.percent
            
            # Get brain metrics
            brain_info = {
                "neurons": brain.getTotalNeuronCount(),
                "synapses": brain.getTotalSynapseCount(),
                "firing": brain.getFiringNeuronCount(),
                "rate": brain.getAverageFiringRate(),
                "spikes": brain.getTotalSpikeCount()
            }
            
            # Store sample
            sample = {
                "timestamp": timestamp,
                "cpu_percent": cpu_percent,
                "memory_percent": memory_percent,
                "brain": brain_info
            }
            
            self.samples.append(sample)
            
            time.sleep(interval)
        
    def stop_monitoring(self):
        """Stop resource monitoring."""
        self.monitoring = False
    
    def get_summary(self):
        """Get summary of resource usage."""
        if not self.samples:
            return None
        
        cpu_samples = [s["cpu_percent"] for s in self.samples]
        memory_samples = [s["memory_percent"] for s in self.samples]
        
        return {
            "samples_count": len(self.samples),
            "avg_cpu": sum(cpu_samples) / len(cpu_samples),
            "max_cpu": max(cpu_samples),
            "avg_memory": sum(memory_samples) / len(memory_samples),
            "max_memory": max(memory_samples)
        }

def run_with_monitoring(brain, duration=60, timestep=0.1):
    """Run simulation with resource monitoring."""
    
    monitor = ResourceMonitor()
    
    # Start monitoring in background thread
    monitor_thread = threading.Thread(
        target=monitor.monitor_resources, 
        args=(brain, 0.1)
    )
    monitor_thread.daemon = True
    monitor_thread.start()
    
    # Run simulation
    num_steps = int(duration / timestep)
    
    for step in range(num_steps):
        brain.step(step)
        
        # Print resource usage every 10 seconds
        if step % (int(10 / timestep)) == 0:
            summary = monitor.get_summary()
            if summary:
                print(f"Step {step}: CPU={summary['avg_cpu']:.1f}%, "
                      f"Memory={summary['avg_memory']:.1f}%")
    
    # Stop monitoring
    monitor.stop_monitoring()
    monitor_thread.join(timeout=1.0)
    
    # Print final summary
    summary = monitor.get_summary()
    if summary:
        print(f"\nResource Usage Summary:")
        print(f"Duration: {duration} seconds")
        print(f"Average CPU: {summary['avg_cpu']:.1f}%")
        print(f"Max CPU: {summary['max_cpu']:.1f}%")
        print(f"Average Memory: {summary['avg_memory']:.1f}%")
        print(f"Max Memory: {summary['max_memory']:.1f}%")
        print(f"Total samples: {summary['samples_count']}")
    
    return monitor

# Run simulation with resource monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Starting simulation with resource monitoring...")
resource_monitor = run_with_monitoring(brain, duration=30, timestep=0.05)
```

---

## Advanced Agent Systems

### Multi-Agent Coordination

```python
import pynlm
import numpy as np

class MultiAgentSystem:
    """System of multiple interacting agents."""
    
    def __init__(self, num_agents, shared_world, config=None):
        self.agents = []
        self.world = shared_world
        self.num_agents = num_agents
        
        if config is None:
            config = pynlm.createDefaultConfig()
        
        # Create agents with slight variations
        for i in range(num_agents):
            # Modify configuration for each agent
            agent_config = pynlm.createDefaultConfig()
            agent_config.set("brain.neuron_count", config.get("brain.neuron_count", 1000))
            agent_config.set("brain.synapse_density", config.get("brain.synapse_density", 0.1))
            
            brain = pynlm.createBrain(agent_config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(shared_world)
            
            # Enable different features for different agents
            agent.enableRewardModulation(i % 2 == 0)  # Even agents get reward modulation
            agent.enableCuriosity(i % 3 == 0)  # Every 3rd agent gets curiosity
            agent.enableDevelopment(i % 4 == 0)  # Every 4th agent develops
            
            self.agents.append(agent)
        
        print(f"Created multi-agent system with {num_agents} agents")
    
    def run_coordination_episode(self, steps=200):
        """Run episode with multi-agent coordination."""
        
        print("=== Multi-Agent Coordination Episode ===")
        
        # Initialize tracking
        agent_metrics = []
        for i, agent in enumerate(self.agents):
            agent_metrics.append({
                "agent_id": i,
                "spikes": agent.getBrain().getTotalSpikeCount(),
                "firing_rate": agent.getBrain().getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError()
            })
        
        # Run episode
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Each agent processes sensory input
            percept = self.world.getSensoryPercept()
            
            for agent in self.agents:
                agent.processSensoryInput(percept)
                agent.getBrain().step(step)
                
                # Each agent makes decisions independently
                action = agent.decodeMotorCommand()
                
                # Apply all agent actions to world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply neuromodulation
                reward = 0.0
                if percept.getInternal():
                    reward = percept.getInternal()[0]
                
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Print coordination metrics every 50 steps
            if step % 50 == 0:
                print(f"Step {step}:")
                for i, agent in enumerate(self.agents):
                    brain = agent.getBrain()
                    print(f"  Agent {i}: Firing={brain.getFiringNeuronCount()}, "
                          f"Curiosity={agent.getCuriosityLevel():.3f}")
        
        # Collect final metrics
        final_metrics = []
        for i, agent in enumerate(self.agents):
            brain = agent.getBrain()
            final_metrics.append({
                "agent_id": i,
                "spikes": brain.getTotalSpikeCount() - agent_metrics[i]["spikes"],
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError()
            })
        
        return final_metrics
    
    def analyze_coordination(self, metrics):
        """Analyze coordination patterns between agents."""
        
        print("=== Coordination Analysis ===")
        
        # Extract metrics for analysis
        curiosity_levels = [m["curiosity"] for m in metrics]
        novelty_levels = [m["novelty"] for m in metrics]
        prediction_errors = [m["prediction_error"] for m in metrics]
        
        # Calculate coordination statistics
        avg_curiosity = np.mean(curiosity_levels)
        avg_novelty = np.mean(novelty_levels)
        avg_prediction_error = np.mean(prediction_errors)
        
        print(f"Average curiosity: {avg_curiosity:.3f}")
        print(f"Average novelty: {avg_novelty:.3f}")
        print(f"Average prediction error: {avg_prediction_error:.3f}")
        
        # Coordination patterns
        coordination_score = avg_curiosity * avg_novelty / (avg_prediction_error + 0.001)
        
        print(f"Coordination score: {coordination_score:.3f}")
        
        # Determine coordination type
        if coordination_score > 1.0:
            print("Coordination: High - Agents working together effectively")
        elif coordination_score > 0.5:
            print("Coordination: Moderate - Some collaboration observed")
        else:
            print("Coordination: Low - Agents mostly independent")
        
        return {
            "avg_curiosity": avg_curiosity,
            "avg_novelty": avg_novelty,
            "avg_prediction_error": avg_prediction_error,
            "coordination_score": coordination_score
        }

# Example usage
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

multi_agent = MultiAgentSystem(3, world)
metrics = multi_agent.run_coordination_episode(100)
analysis = multi_agent.analyze_coordination(metrics)
```

### Hierarchical Agent Systems

```python
import pynlm

class HierarchicalAgent:
    """Hierarchical agent with multiple levels of control."""
    
    def __init__(self, base_config):
        self.base_config = base_config
        self.create_hierarchy()
    
    def create_hierarchy(self):
        """Create hierarchical agent structure."""
        
        # Level 1: Primary controller
        self.primary_config = pynlm.createDefaultConfig()
        self.primary_config.set("brain.neuron_count", self.base_config.get("brain.neuron_count", 500))
        self.primary_config.set("brain.synapse_density", 0.2)
        
        self.primary_brain = pynlm.createBrain(self.primary_config)
        self.primary_brain.initialize()
        
        self.primary_agent = pynlm.createAgentBrain(self.primary_brain)
        
        # Level 2: Secondary controller (subordinate)
        self.secondary_config = pynlm.createDefaultConfig()
        self.secondary_config.set("brain.neuron_count", self.base_config.get("brain.neuron_count", 200))
        self.secondary_config.set("brain.synapse_density", 0.15)
        
        self.secondary_brain = pynlm.createBrain(self.secondary_config)
        self.secondary_brain.initialize()
        
        self.secondary_agent = pynlm.createAgentBrain(self.secondary_brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        
        # Initialize agents
        self.primary_agent.initialize(self.world)
        self.secondary_agent.initialize(self.world)
        
        # Enable different features
        self.primary_agent.enableRewardModulation(True)
        self.primary_agent.enableCuriosity(True)
        
        self.secondary_agent.enableRewardModulation(True)
        self.secondary_agent.enableDevelopment(True)
        
        # Set up hierarchical control
        self.setup_hierarchical_control()
    
    def setup_hierarchical_control(self):
        """Set up hierarchical control relationships."""
        
        # Primary agent controls main actions
        self.primary_agent.enableStructuralPlasticity(True)
        
        # Secondary agent handles specialized tasks
        self.secondary_agent.enableCuriosity(True)
        self.secondary_agent.enableDevelopment(True)
        
        # Create coordination between agents
        self.coordination_level = 0.5  # Initial coordination level
    
    def run_hierarchical_episode(self, steps=300):
        """Run episode with hierarchical control."""
        
        print("=== Hierarchical Agent Episode ===")
        
        # Reset world
        self.world.reset()
        
        # Episode tracking
        episode_data = {
            "steps": [],
            "primary_metrics": [],
            "secondary_metrics": []
        }
        
        for step in range(steps):
            # Update world
            self.world.update(0.05)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Primary agent processes high-level decisions
            self.primary_agent.processSensoryInput(percept)
            self.primary_brain.step(step)
            
            # Hierarchical decision making
            primary_action = self.primary_agent.decodeMotorCommand()
            secondary_action = self.decide_secondary_action(step, percept)
            
            # Combine actions
            combined_action = self.combine_actions(primary_action, secondary_action)
            
            # Apply combined action
            self.world.applyMotorCommand(combined_action, self.world.getSimulationTime())
            
            # Update secondary agent
            self.secondary_agent.processSensoryInput(percept)
            self.secondary_brain.step(step)
            
            # Secondary agent can override primary decisions
            if self.should_override_primary(step):
                self.world.applyMotorCommand(secondary_action, self.world.getSimulationTime())
            
            # Apply neuromodulation
            self.primary_agent.applyRewardModulation(0.0, 0.0)
            self.secondary_agent.applyRewardModulation(0.0, 0.0)
            
            # Update development
            self.primary_agent.updateDevelopment(0.05)
            self.secondary_agent.updateDevelopment(0.05)
            
            # Record metrics
            episode_data["steps"].append(step)
            
            primary_metric = {
                "step": step,
                "firing_rate": self.primary_brain.getAverageFiringRate(),
                "spike_count": self.primary_brain.getTotalSpikeCount(),
                "curiosity": self.primary_agent.getCuriosityLevel()
            }
            episode_data["primary_metrics"].append(primary_metric)
            
            secondary_metric = {
                "step": step,
                "firing_rate": self.secondary_brain.getAverageFiringRate(),
                "spike_count": self.secondary_brain.getTotalSpikeCount(),
                "novelty": self.secondary_agent.getNoveltyLevel()
            }
            episode_data["secondary_metrics"].append(secondary_metric)
            
            # Print hierarchical metrics every 50 steps
            if step % 50 == 0:
                print(f"Step {step}:")
                print(f"  Primary - Firing: {self.primary_brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.primary_agent.getCuriosityLevel():.3f}")
                print(f"  Secondary - Firing: {self.secondary_brain.getFiringNeuronCount()}, "
                      f"Novelty: {self.secondary_agent.getNoveltyLevel():.3f}")
        
        return episode_data
    
    def decide_secondary_action(self, step, percept):
        """Secondary agent makes specialized decisions."""
        
        # Secondary agent focuses on exploration and novelty
        novelty_threshold = 0.3 + 0.2 * np.sin(step * 0.1)
        
        if self.secondary_agent.getNoveltyLevel() > novelty_threshold:
            # High novelty = exploration action
            return pynlm.MotorCommand.LookLeft  # Example exploration action
        else:
            # Follow primary guidance
            return pynlm.MotorCommand.Wait
    
    def combine_actions(self, primary_action, secondary_action):
        """Combine actions from different hierarchical levels."""
        
        # Simple combination: use secondary if coordination is high, otherwise primary
        if self.coordination_level > 0.7:
            return secondary_action
        else:
            return primary_action
    
    def should_override_primary(self, step):
        """Determine if secondary agent should override primary."""
        
        # Override when novelty is high and coordination is low
        novelty = self.secondary_agent.getNoveltyLevel()
        return novelty > 0.5 and self.coordination_level < 0.3

# Example usage with hierarchical agents
base_config = pynlm.createDefaultConfig()
hierarchical_agent = HierarchicalAgent(base_config)

# Run hierarchical episode
hierarchical_data = hierarchical_agent.run_hierarchical_episode(200)
```

---

## Neuromodulation Deep Dive

### Custom Neuromodulation Systems

```python
import pynlm
import numpy as np

class CustomNeuromodulation:
    """Custom neuromodulation system for specialized learning."""
    
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.initialize_systems()
    
    def initialize_systems(self):
        """Initialize custom neuromodulation components."""
        
        # Dopamine system with custom dynamics
        self.dopamine_system = {
            "baseline": 0.1,
            "learning_rate": 0.01,
            "adaptation_rate": 0.001,
            "history": []
        }
        
        # Curiosity system with exploration bonus
        self.curiosity_system = {
            "baseline": 0.05,
            "novelty_sensitivity": 1.0,
            "prediction_error_weight": 0.8,
            "exploration_bonus": 0.2,
            "history": []
        }
        
        # Novelty detection with hysteresis
        self.novelty_system = {
            "threshold": 0.5,
            "hysteresis": 0.1,
            "memory_size": 100,
            "novelty_history": []
        }
        
        # Reward prediction error with custom learning
        self.rpe_system = {
            "learning_rate": 0.05,
            "value_estimate": 0.0,
            "prediction_error_history": []
        }
    
    def compute_custom_dopamine(self, reward, prediction_error):
        """Compute custom dopamine signal."""
        
        # Modified dopamine response
        dopamine = (
            self.dopamine_system["baseline"] +
            self.dopamine_system["learning_rate"] * prediction_error - 
            self.dopamine_system["adaptation_rate"] * self.dopamine_system["baseline"]
        )
        
        # Clip to reasonable range
        dopamine = max(0.0, min(1.0, dopamine))
        
        # Update history
        self.dopamine_system["history"].append(dopamine)
        if len(self.dopamine_system["history"]) > 1000:
            self.dopamine_system["history"].pop(0)
        
        return dopamine
    
    def compute_custom_curiosity(self, novelty, prediction_error):
        """Compute custom curiosity signal."""
        
        # Enhanced curiosity calculation
        curiosity = (
            self.curiosity_system["baseline"] +
            self.curiosity_system["novelty_sensitivity"] * novelty +
            self.curiosity_system["prediction_error_weight"] * prediction_error
        )
        
        # Add exploration bonus for sustained novelty
        if novelty > 0.7 and len(self.novelty_system["novelty_history"]) > 10:
            recent_novelty = np.mean(self.novelty_system["novelty_history"][-10:])
            if recent_novelty > 0.6:
                curiosity += self.curiosity_system["exploration_bonus"]
        
        # Clip to reasonable range
        curiosity = max(0.0, min(1.0, curiosity))
        
        # Update history
        self.curiosity_system["history"].append(curiosity)
        if len(self.curiosity_system["history"]) > 1000:
            self.curiosity_system["history"].pop(0)
        
        return curiosity
    
    def detect_novelty_with_hysteresis(self, current_value):
        """Novelty detection with hysteresis."""
        
        # Add to novelty history
        self.novelty_system["novelty_history"].append(current_value)
        if len(self.novelty_system["novelty_history"]) > self.novelty_system["memory_size"]:
            self.novelty_system["novelty_history"].pop(0)
        
        # Calculate current novelty using moving average
        recent_values = self.novelty_system["novelty_history"][-10:]
        current_novelty = np.mean(recent_values)
        
        # Apply hysteresis
        threshold_high = self.novelty_system["threshold"] + self.novelty_system["hysteresis"]
        threshold_low = self.novelty_system["threshold"] - self.novelty_system["hysteresis"]
        
        if current_novelty > threshold_high:
            self.novelty_system["current_novelty"] = current_novelty
        elif current_novelty < threshold_low:
            self.novelty_system["current_novelty"] = current_novelty
        # else: maintain previous value
        
        return self.novelty_system["current_novelty"]
    
    def compute_reward_prediction_error(self, actual_reward, predicted_reward):
        """Compute reward prediction error with custom learning."""
        
        # Standard RPE calculation
        rpe = actual_reward - self.rpe_system["value_estimate"]
        
        # Update value estimate
        self.rpe_system["value_estimate"] += self.rpe_system["learning_rate"] * rpe
        
        # Apply eligibility traces
        self.rpe_system["value_estimate"] *= 0.99  # Discount
        
        # Update history
        self.rpe_system["prediction_error_history"].append(rpe)
        if len(self.rpe_system["prediction_error_history"]) > 1000:
            self.rpe_system["prediction_error_history"].pop(0)
        
        return rpe
    
    def run_custom_neuromodulation_loop(self, steps=200):
        """Run simulation with custom neuromodulation."""
        
        print("=== Custom Neuromodulation Loop ===")
        
        neuromodulation_data = {
            "steps": [],
            "dopamine": [],
            "curiosity": [],
            "novelty": [],
            "rpe": [],
            "behavior": []
        }
        
        for step in range(steps):
            # Get current state
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            prediction_error = self.agent.getPredictionError()
            
            # Simulate reward (in real scenario, this would come from environment)
            reward = 0.1 if step % 20 == 0 else 0.0  # Small sparse rewards
            
            # Apply custom neuromodulation
            dopamine = self.compute_custom_dopamine(reward, prediction_error)
            custom_curiosity = self.compute_custom_curiosity(novelty, prediction_error)
            custom_novelty = self.detect_novelty_with_hysteresis(reward)
            rpe = self.compute_reward_prediction_error(reward, self.rpe_system["value_estimate"])
            
            # Store neuromodulation data
            neuromodulation_data["steps"].append(step)
            neuromodulation_data["dopamine"].append(dopamine)
            neuromodulation_data["curiosity"].append(custom_curiosity)
            neuromodulation_data["novelty"].append(custom_novelty)
            neuromodulation_data["rpe"].append(rpe)
            
            # Determine behavior based on neuromodulation
            behavior = self.determine_behavior(dopamine, custom_curiosity, custom_novelty)
            neuromodulation_data["behavior"].append(behavior)
            
            # Print progress
            if step % 50 == 0:
                print(f"Step {step}: Dopamine={dopamine:.3f}, "
                      f"Curiosity={custom_curiosity:.3f}, Novelty={custom_novelty:.3f}, "
                      f"RPE={rpe:.3f}, Behavior={behavior}")
        
        return neuromodulation_data
    
    def determine_behavior(self, dopamine, curiosity, novelty):
        """Determine behavior based on neuromodulation signals."""
        
        # Behavior selection based on neuromodulation
        if dopamine > 0.7 and curiosity > 0.5:
            return "Exploitative"  # High confidence, exploit
        elif novelty > 0.6:
            return "Exploratory"   # High novelty, explore
        elif curiosity > 0.3:
            return "Balanced"     # Moderate curiosity, balance
        else:
            return "Passive"      # Low neuromodulation, passive

# Example usage with custom neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable standard neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Create custom neuromodulation system
custom_neuro = CustomNeuromodulation(brain, agent)

# Run custom neuromodulation simulation
neuro_data = custom_neuro.run_custom_neuromodulation_loop(150)
```

---

## Integration with External Systems

### External Data Integration

```python
import pynlm
import numpy as np
import json
from datetime import datetime

class ExternalDataIntegrator:
    """Integrate external data sources with NLM simulation."""
    
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.data_sources = {}
        self.integrated_data = []
    
    def add_data_source(self, name, data_source, update_frequency=1.0):
        """Add external data source to integration system."""
        
        self.data_sources[name] = {
            "source": data_source,
            "frequency": update_frequency,
            "last_update": 0.0,
            "data_type": "numeric"  # or "categorical", "temporal"
        }
        
        print(f"Added data source: {name}")
    
    def integrate_external_data(self, data_type, value, timestamp):
        """Integrate external data into brain simulation."""
        
        # Create sensory input from external data
        sensory_input = pynlm.SensoryInput()
        
        if data_type == "numeric":
            # For numeric data, create internal signals
            internal_signal = pynlm.InternalSignals()
            internal_signal.addSignal(value)
            sensory_input.setInternal(internal_signal)
            
        elif data_type == "categorical":
            # For categorical data, create vision input
            vision_data = np.zeros((8, 8, 3))  # 8x8 RGB image
            if value == "resource":
                vision_data[3:6, 3:6] = [1.0, 1.0, 0.0]  # Yellow resource
            elif value == "hazard":
                vision_data[0:3, 0:3] = [1.0, 0.0, 0.0]  # Red hazard
            
            vision = pynlm.Vision(8, 8, 3)
            vision.setData(vision_data.flatten())
            sensory_input.setVision(vision)
            
        elif data_type == "temporal":
            # For temporal data, create audio input
            audio = pynlm.Audio(sample_rate=100, num_samples=50)
            audio_data = np.zeros(50)
            audio_data[::5] = value  # Sparse signal
            audio.setData(audio_data)
            sensory_input.setAudio(audio)
        
        # Set timestamp
        sensory_input.setTimestamp(timestamp)
        
        # Apply sensory input to brain
        self.brain.receiveSensoryInput(sensory_input)
        
        # Store integrated data
        integrated_entry = {
            "timestamp": timestamp,
            "data_type": data_type,
            "value": value,
            "brain_state": {
                "step": len(self.integrated_data),
                "firing_rate": self.brain.getAverageFiringRate(),
                "spike_count": self.brain.getTotalSpikeCount()
            }
        }
        
        self.integrated_data.append(integrated_entry)
        
        return sensory_input
    
    def run_external_data_simulation(self, external_data_stream, duration=300):
        """Run simulation with continuous external data integration."""
        
        print("=== External Data Integration Simulation ===")
        
        simulation_time = 0.0
        timestep = 0.1
        steps = int(duration / timestep)
        
        # Initialize external data sources
        for name, config in self.data_sources.items():
            config["last_update"] = 0.0
        
        for step in range(steps):
            simulation_time += timestep
            
            # Process external data stream
            external_data = external_data_stream(step, simulation_time)
            
            # Integrate external data
            for data_entry in external_data:
                self.integrate_external_data(
                    data_entry["type"], 
                    data_entry["value"], 
                    data_entry["timestamp"]
                )
            
            # Run brain step
            self.brain.step(step, simulation_time)
            
            # Process with agent if available
            if self.agent and self.world:
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, simulation_time)
                
                # Apply neuromodulation based on integrated data
                reward = self.calculate_reward_from_external_data(external_data)
                self.agent.applyRewardModulation(reward, 0.0)
            
            # Print integration status
            if step % (int(10 / timestep)) == 0:
                print(f"Step {step} ({simulation_time:.1f}s): "
                      f"Integrated {len(external_data)} data points, "
                      f"Firing rate: {self.brain.getAverageFiringRate():.3f} Hz")
        
        return self.integrated_data
    
    def calculate_reward_from_external_data(self, external_data):
        """Calculate reward based on integrated external data."""
        
        reward = 0.0
        
        for data in external_data:
            if data["type"] == "numeric":
                # Positive numeric values give positive reward
                reward += max(0.0, data["value"] * 0.1)
                
            elif data["type"] == "categorical":
                # Resource gives positive reward, hazard gives negative
                if data["value"] == "resource":
                    reward += 1.0
                elif data["value"] == "hazard":
                    reward -= 0.5
                    
            elif data["type"] == "temporal":
                # Regular temporal patterns give moderate reward
                if data["value"] > 0.5:
                    reward += 0.2
        
        return reward
    
    def analyze_external_data_impact(self):
        """Analyze impact of external data on brain behavior."""
        
        print("=== External Data Impact Analysis ===")
        
        if not self.integrated_data:
            print("No integrated data to analyze")
            return None
        
        # Analyze data by type
        data_by_type = {}
        for entry in self.integrated_data:
            data_type = entry["data_type"]
            if data_type not in data_by_type:
                data_by_type[data_type] = []
            data_by_type[data_type].append(entry)
        
        # Analyze impact on brain metrics
        analysis_results = {}
        
        for data_type, entries in data_by_type.items():
            print(f"\nAnalyzing {data_type} data impact:")
            
            # Calculate statistics
            firing_rates = [entry["brain_state"]["firing_rate"] for entry in entries]
            spike_counts = [entry["brain_state"]["spike_count"] for entry in entries]
            
            analysis_results[data_type] = {
                "count": len(entries),
                "avg_firing_rate": np.mean(firing_rates),
                "avg_spike_count": np.mean(spike_counts),
                "max_firing_rate": np.max(firing_rates),
                "min_firing_rate": np.min(firing_rates),
                "firing_rate_std": np.std(firing_rates)
            }
            
            print(f"  Data points: {len(entries)}")
            print(f"  Average firing rate: {analysis_results[data_type]['avg_firing_rate']:.3f} Hz")
            print(f"  Average spike count: {analysis_results[data_type]['avg_spike_count']:.1f}")
            print(f"  Firing rate std: {analysis_results[data_type]['firing_rate_std']:.3f} Hz")
        
        # Cross-data type correlations
        if len(data_by_type) > 1:
            print(f"\nCross-data type correlations:")
            for data_type1 in data_by_type.keys():
                for data_type2 in data_by_type.keys():
                    if data_type1 < data_type2:
                        correlation = self.calculate_correlation(data_type1, data_type2, data_by_type)
                        print(f"  {data_type1} vs {data_type2}: {correlation:.3f}")
        
        return analysis_results
    
    def calculate_correlation(self, type1, type2, data_by_type):
        """Calculate correlation between two data types."""
        
        # Get brain metrics for each data type
        metrics1 = [entry["brain_state"]["firing_rate"] for entry in data_by_type[type1]]
        metrics2 = [entry["brain_state"]["firing_rate"] for entry in data_by_type[type2]]
        
        # Ensure same length by pairing with timestamps
        # For simplicity, just use available data
        if len(metrics1) != len(metrics2):
            min_len = min(len(metrics1), len(metrics2))
            metrics1 = metrics1[:min_len]
            metrics2 = metrics2[:min_len]
        
        if len(metrics1) < 2:
            return 0.0
        
        return np.corrcoef(metrics1, metrics2)[0, 1]

# Example usage with external data integration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create integrator
integrator = ExternalDataIntegrator(brain, agent, world)

# Define external data source (simulated sensor data)
def sensor_data_stream(step, timestamp):
    """Simulate sensor data stream with external events."""
    
    data = []
    
    # Add random sensor readings
    if np.random.random() < 0.3:  # 30% chance of external event
        event_type = np.random.choice(["numeric", "categorical", "temporal"])
        
        if event_type == "numeric":
            value = np.random.normal(0.5, 0.2)
            data.append({
                "type": "numeric",
                "value": value,
                "timestamp": timestamp
            })
            
        elif event_type == "categorical":
            value = np.random.choice(["resource", "hazard", "marker"])
            data.append({
                "type": "categorical",
                "value": value,
                "timestamp": timestamp
            })
            
        elif event_type == "temporal":
            value = np.random.uniform(0, 1.0)
            data.append({
                "type": "temporal",
                "value": value,
                "timestamp": timestamp
            })
    
    return data

# Run integration simulation
print("Starting external data integration simulation...")
integrated_data = integrator.run_external_data_simulation(
    sensor_data_stream, 
    duration=60
)

# Analyze impact
analysis = integrator.analyze_external_data_impact()
```

---

## Best Practices

### Recommended Development Workflow

1. **Start Simple**
   - Begin with basic brain configuration
   - Use default parameters when unsure
   - Validate with simple test cases

2. **Incremental Development**
   - Build functionality step by step
   - Test each feature before adding dependencies
   - Use checkpointing for recovery

3. **Memory Management**
   - Use context managers for resource cleanup
   - Clear references when no longer needed
   - Monitor memory usage during long simulations

4. **Configuration Management**
   - Use configuration files for complex setups
   - Implement parameter sweeps for experimentation
   - Document configuration options

5. **Testing and Validation**
   - Use debugging mode for issue identification
   - Implement unit tests for custom components
   - Monitor brain metrics during development

### Performance Guidelines

1. **Brain Size Optimization**
   - Use appropriate neuron counts for your task
   - Adjust synapse density for computational efficiency
   - Consider plasticity rules based on performance needs

2. **Simulation Control**
   - Use time-stamped simulations for precision
   - Implement checkpointing for long-running experiments
   - Monitor resource usage regularly

3. **Agent Configuration**
   - Enable only necessary neuromodulation systems
   - Balance exploration vs. exploitation
   - Use developmental stages appropriately

### Common Pitfalls and Solutions

1. **Problem**: Brain doesn't fire neurons
   - **Solution**: Check if `brain.initialize()` was called
   - **Solution**: Increase simulation steps for warm-up
   - **Solution**: Verify sensory input is connected

2. **Problem**: Memory usage grows uncontrollably
   - **Solution**: Use garbage collection periodically
   - **Solution**: Clear unnecessary references
   - **Solution**: Monitor memory with `psutil`

3. **Problem**: Simulation runs too slowly
   - **Solution**: Reduce neuron count or synapse density
   - **Solution**: Disable unnecessary plasticity rules
   - **Solution**: Use batch processing for multiple simulations

4. **Problem**: Agent doesn't learn effectively
   - **Solution**: Enable appropriate neuromodulation systems
   - **Solution**: Adjust learning rates in configuration
   - **Solution**: Provide consistent reward signals

### Code Style Recommendations

1. **Python Code Style**
   - Use descriptive variable names
   - Follow PEP 8 guidelines
   - Include docstrings for functions
   - Use type hints for better code documentation

2. **Configuration Management**
   - Store configuration in JSON files
   - Use parameter validation
   - Implement configuration inheritance

3. **Error Handling**
   - Use try-catch blocks for external data
   - Validate input parameters
   - Provide meaningful error messages

4. **Testing**
   - Write unit tests for custom components
   - Use logging for debugging
   - Implement performance benchmarks

---

## Advanced Examples

### Example 1: Learning with Dynamic Environment

```python
import pynlm
import numpy as np
import time

def dynamic_environment_learning():
    """Learn in an environment that changes over time."""
    
    # Create brain with adaptive configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Track learning over time
    learning_metrics = {
        "steps": [],
        "firing_rate": [],
        "curiosity": [],
        "novelty": [],
        "environment_changes": []
    }
    
    # Simulate dynamic environment
    for step in range(500):
        # Environment changes every 100 steps
        if step % 100 == 0:
            environment_change = step // 100
            learning_metrics["environment_changes"].append({
                "step": step,
                "change_type": "difficulty_increase" if environment_change % 2 == 0 else "novelty_boost"
            })
            print(f"Environment change at step {step}: {learning_metrics['environment_changes'][-1]['change_type']}")
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record metrics
        learning_metrics["steps"].append(step)
        learning_metrics["firing_rate"].append(brain.getAverageFiringRate())
        learning_metrics["curiosity"].append(agent.getCuriosityLevel())
        learning_metrics["novelty"].append(agent.getNoveltyLevel())
        
        # Print learning progress
        if step % 100 == 0:
            print(f"Step {step}: Firing rate={brain.getAverageFiringRate():.3f} Hz, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    return learning_metrics

# Run dynamic learning simulation
learning_metrics = dynamic_environment_learning()
```

### Example 2: Multi-Phase Learning

```python
import pynlm
import json

def multi_phase_learning():
    """Learn across multiple developmental phases."""
    
    # Phase 1: Rapid learning (high plasticity)
    phase1_config = pynlm.createDefaultConfig()
    phase1_config.set("brain.neuron_count", 500)
    phase1_config.set("plasticity.stdp.enable", True)
    phase1_config.set("plasticity.hebbian.enable", True)
    phase1_config.set("plasticity.structural.enable", True)
    
    # Phase 2: Stable performance (moderate plasticity)
    phase2_config = pynlm.createDefaultConfig()
    phase2_config.set("brain.neuron_count", 800)
    phase2_config.set("plasticity.stdp.enable", True)
    phase2_config.set("plasticity.hebbian.enable", False)
    phase2_config.set("plasticity.structural.enable", True)
    
    # Phase 3: Specialization (low plasticity, high performance)
    phase3_config = pynlm.createDefaultConfig()
    phase3_config.set("brain.neuron_count", 1200)
    phase3_config.set("plasticity.stdp.enable", False)
    phase3_config.set("plasticity.hebbian.enable", False)
    phase3_config.set("plasticity.structural.enable", True)
    
    phases = [
        {"config": phase1_config, "steps": 200, "stage": "exploration"},
        {"config": phase2_config, "steps": 300, "stage": "refinement"},
        {"config": phase3_config, "steps": 500, "stage": "specialization"}
    ]
    
    all_metrics = []
    
    for phase_info in phases:
        print(f"\n=== Phase: {phase_info['stage'].upper()} ===")
        
        # Create brain for this phase
        brain = pynlm.createBrain(phase_info["config"])
        brain.initialize()
        
        # Set developmental stage
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable appropriate learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(phase_info["stage"] == "exploration")
        agent.enableDevelopment(True)
        
        # Run phase
        phase_metrics = {
            "phase": phase_info["stage"],
            "steps": [],
            "firing_rate": [],
            "spike_count": [],
            "curiosity": [],
            "novelty": []
        }
        
        for step in range(phase_info["steps"]):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Update developmental stage for phase 3
            if phase_info["stage"] == "specialization":
                if step > 100:
                    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
                if step > 300:
                    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            
            # Record metrics
            phase_metrics["steps"].append(step)
            phase_metrics["firing_rate"].append(brain.getAverageFiringRate())
            phase_metrics["spike_count"].append(brain.getTotalSpikeCount())
            phase_metrics["curiosity"].append(agent.getCuriosityLevel())
            phase_metrics["novelty"].append(agent.getNoveltyLevel())
            
            # Print phase progress
            if step % 100 == 0:
                print(f"  Phase {phase_info['stage']}: Step {step}, "
                      f"Firing rate: {brain.getAverageFiringRate():.3f} Hz, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Save phase results
        all_metrics.append(phase_metrics)
        
        # Save checkpoint for continuity
        checkpoint_file = f"phase_{phase_info['stage']}_checkpoint.bin"
        brain.save(checkpoint_file)
        print(f"Phase checkpoint saved: {checkpoint_file}")
    
    return all_metrics

# Run multi-phase learning
phase_metrics = multi_phase_learning()
```

---

## Conclusion

This Advanced User Guide provides comprehensive coverage of NLM's advanced features for experienced users. The techniques covered include:

- **Advanced brain configuration** for specific research tasks
- **Performance optimization** for large-scale simulations
- **Debugging and monitoring** tools for development and troubleshooting
- **Memory management** strategies for resource-constrained environments
- **Advanced agent systems** for complex behavioral studies
- **Neuromodulation deep dive** for custom learning systems
- **External data integration** for real-world applications
- **Best practices** for robust and efficient code

By mastering these advanced techniques, you can leverage the full power of NLM to create sophisticated neural simulations, conduct advanced research, and develop complex adaptive systems.

The examples provided demonstrate practical applications of these advanced features and can serve as starting points for your own research projects. Adapt and extend these examples to meet your specific requirements and experimental goals.

---

## References

- NLM Documentation: https://nlm.readthedocs.io
- Architecture Guide: docs/ARCHITECTURE.md
- Scientific Background: docs/SCIENCE.md
- Experiment Examples: docs/EXPERIMENTS.md

---

## See Also

- [Easy Usage Guide](easy_usage.md) - Basic usage patterns
- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete API reference
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and protocols

---

*(Document Version: 1.0 - Advanced User Guide)*
*(Last Updated: 2026-10-07)*
*(For NLM Version 0.1.0+)*

---

## See Also

- [Easy Usage Guide](easy_usage.md) - Basic usage patterns
- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete API reference
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and protocols

---

*End of Advanced User Guide*