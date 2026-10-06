# Visualization and Debugging Tools

## Overview

This document describes the visualization and debugging tools available for NLM, including:

- Brain state visualization
- Network graph visualization
- Performance monitoring
- Debug output and logging
- Checkpoint management visualization

## Available Visualization Interfaces

### 1. Simple Console Visualization

A simple console-based visualization system that provides real-time monitoring of brain state.

```python
import pynlm
import time

def console_visualizer(brain, agent=None, world=None, update_interval=1.0):
    """Simple console-based visualization of NLM brain state."""
    
    print("=== NLM Brain Visualization ===")
    print("Press Ctrl+C to exit")
    print()
    
    try:
        while True:
            # Clear screen (for Unix/Linux/Mac)
            print("\033[H\033[2J", end="")  # ANSI escape codes for clear screen
            
            # Display brain statistics
            print("=== BRAIN STATISTICS ===")
            print(f"Neurons: {brain.getTotalNeuronCount():6d}")
            print(f"Synapses: {brain.getTotalSynapseCount():6d}")
            print(f"Firing neurons: {brain.getFiringNeuronCount():6d}")
            print(f"Total spikes: {brain.getTotalSpikeCount():6d}")
            print(f"Active neurons: {brain.getActiveNeuronCount():6d}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
            print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print()
            
            # Display development stage
            print(f"Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"Current time: {brain.getConfig().getOr_double('simulation.timestep', 0.001) * brain.getConfig().getOr_int('simulation.max_steps', 1000):.3f} s")
            print()
            
            # Display neuromodulation levels (if agent is provided)
            if agent:
                print("=== NEUROMODULATION ===")
                print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
                print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
                print(f"Prediction error: {agent.getPredictionError():.3f}")
                print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
                print()
            
            # Display memory system stats (if available)
            if agent and hasattr(agent, 'getEpisodicMemory'):
                episodic_memory = agent.getEpisodicMemory()
                if episodic_memory:
                    print("=== MEMORY SYSTEMS ===")
                    print(f"Episodic memory episodes: {episodic_memory->getEpisodeCount():6d}")
                    working_memory = agent.getWorkingMemory()
                    if working_memory:
                        print(f"Working memory traces: {working_memory->getActiveTraces():6d}")
                    print()
            
            # Display recent activity (if available)
            print("=== RECENT ACTIVITY ===")
            print(f"Steps since last checkpoint: {brain.getTotalSpikeCount() % 1000:6d}")
            print(f"Last spike time: {brain.getTotalSpikeCount() > 0 ? 'Recent' : 'None'}")
            print()
            
            # Wait for update interval
            time.sleep(update_interval)
            
    except KeyboardInterrupt:
        print("\nVisualization stopped by user")
    
    return False

# Usage example
if __name__ == "__main__":
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set_default_brain_config()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable debugging features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation with visualization
    step = 0
    while step < 100:
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update agent learning
        reward = 0.0
        if not world.getSensoryPercept().getInternal().empty():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        step += 1
    
    # Run visualization
    console_visualizer(brain, agent, world, update_interval=1.0)
```

### 2. Brain State Dump Tool

A tool for dumping brain state information for analysis.

```python
import pynlm
import json
from datetime import datetime

def dump_brain_state(brain, filename=None, include_memory=True):
    """Dump brain state information to a JSON file for analysis."""
    
    if filename is None:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"brain_dump_{timestamp}.json"
    
    dump_data = {
        "timestamp": datetime.now().isoformat(),
        "config": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "region_count": brain.getRegionCount(),
            "simulation_step": brain.getTotalSpikeCount(),
        },
        "statistics": {
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        },
        "development": {
            "stage": brain.getDevelopmentalStage(),
        },
        "memory": {},
        "regions": []
    }
    
    # Add memory system information
    if include_memory:
        working_memory = brain.getWorkingMemory()
        if working_memory:
            dump_data["memory"]["working_memory"] = {
                "capacity": working_memory->getCapacity(),
                "active_traces": working_memory->getActiveTraces(),
            }
        
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            dump_data["memory"]["episodic_memory"] = {
                "max_episodes": episodic_memory->getMaxEpisodes(),
                "episode_count": episodic_memory->getEpisodeCount(),
            }
        
        associative_memory = brain.getAssociativeMemory()
        if associative_memory:
            dump_data["memory"]["associative_memory"] = {
                "pattern_count": associative_memory->getPatternCount(),
            }
    
    # Add region information
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            region_info = {
                "id": region_id.index(),
                "name": region->getName(),
                "neuron_count": region->getTotalNeuronCount(),
                "synapse_count": region->getSynapseCount(),
                "average_synaptic_weight": region->getAverageSynapticWeight(),
                "population_count": region->getPopulationCount(),
            }
            dump_data["regions"].append(region_info)
    
    # Write to file
    with open(filename, 'w') as f:
        json.dump(dump_data, f, indent=2)
    
    print(f"Brain state dumped to {filename}")
    return dump_data

# Usage example
if __name__ == "__main__":
    # Create and run brain simulation
    config = pynlm.createDefaultConfig()
    config.set_default_brain_config()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
    
    # Dump brain state
    dump_data = dump_brain_state(brain, "brain_analysis.json", include_memory=True)
    
    # Print summary
    print(f"\nBrain state summary:")
    print(f"  Total spikes: {dump_data['statistics']['total_spikes']}")
    print(f"  Firing neurons: {dump_data['statistics']['firing_neurons']}")
    print(f"  Average firing rate: {dump_data['statistics']['average_firing_rate']:.3f} Hz")
    print(f"  Development stage: {dump_data['development']['stage']}")
    print(f"  Regions: {len(dump_data['regions'])}")
    if 'memory' in dump_data:
        print(f"  Memory systems: {list(dump_data['memory'].keys())}")
```

### 3. Performance Profiler

A performance monitoring tool for NLM simulations.

```python
import pynlm
import time
import statistics

def profile_performance(brain_factory, config_factory, simulation_steps=100, profile_interval=10):
    """Profile performance of NLM simulation with detailed metrics."""
    
    print("=== NLM Performance Profile ===")
    print(f"Simulation steps: {simulation_steps}")
    print(f"Profile interval: {profile_interval} steps")
    print()
    
    # Create brain
    config = config_factory()
    brain = brain_factory(config)
    brain.initialize()
    
    # Performance metrics
    step_times = []
    spike_rates = []
    memory_usage = []
    cpu_usage = []
    
    start_time = time.time()
    total_steps = 0
    
    for step in range(simulation_steps):
        step_start = time.time()
        
        # Run simulation step
        brain.step(step)
        
        step_end = time.time()
        step_time = step_end - step_start
        step_times.append(step_time)
        
        # Calculate metrics
        spike_rate = brain.getFiringNeuronCount() / (step_time + 1e-6)  # spikes per second
        spike_rates.append(spike_rate)
        
        # Memory usage (approximate)
        import psutil
        process = psutil.Process()
        memory_usage.append(process.memory_info().rss / 1024 / 1024)  # MB
        
        total_steps += 1
        
        # Print profile every interval
        if (step + 1) % profile_interval == 0:
            print(f"--- Step {step + 1}/{simulation_steps} ---")
            print(f"  Step time: {step_time*1000:.2f} ms")
            print(f"  Spike rate: {spike_rate:.1f} Hz")
            print(f"  Memory usage: {memory_usage[-1]:.1f} MB")
            print(f"  Total elapsed: {time.time() - start_time:.2f} s")
            print()
    
    total_time = time.time() - start_time
    
    # Calculate statistics
    print("=== Performance Statistics ===")
    print(f"Total simulation time: {total_time:.2f} s")
    print(f"Average step time: {statistics.mean(step_times)*1000:.2f} ± {statistics.stdev(step_times)*1000:.2f} ms")
    print(f"Steps per second: {simulation_steps/total_time:.1f}")
    print(f"Average spike rate: {statistics.mean(spike_rates):.1f} ± {statistics.stdev(spike_rates):.1f} Hz")
    print(f"Memory usage: {statistics.mean(memory_usage):.1f} ± {statistics.stdev(memory_usage):.1f} MB")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    # Performance score (higher is better)
    steps_per_sec = simulation_steps / total_time
    spike_rate_score = min(100, brain.getAverageFiringRate() / 10.0)  # Scale to 100
    memory_score = max(0, 100 - (statistics.mean(memory_usage) - 50))  # Better if < 50MB
    
    performance_score = (steps_per_sec * 0.4 + spike_rate_score * 0.4 + memory_score * 0.2) / 100 * 100
    
    print(f"\nOverall performance score: {performance_score:.1f}/100")
    
    # Return metrics for further analysis
    return {
        "step_times": step_times,
        "spike_rates": spike_rates,
        "memory_usage": memory_usage,
        "total_time": total_time,
        "total_spikes": brain.getTotalSpikeCount(),
        "performance_score": performance_score,
    }

# Usage example with different configurations
if __name__ == "__main__":
    def small_config():
        config = pynlm.createDefaultConfig()
        config.set_default_brain_config()
        config.set_int("brain.neuron_count", 500)
        return config
    
    def large_config():
        config = pynlm.createDefaultConfig()
        config.set_default_brain_config()
        config.set_int("brain.neuron_count", 5000)
        config.set_int("brain.region_count", 4)
        return config
    
    # Profile small brain
    print("Profiling small brain...")
    small_metrics = profile_performance(
        lambda c: pynlm.createBrain(c),
        small_config,
        simulation_steps=200,
        profile_interval=50
    )
    
    # Profile large brain
    print("\n\nProfiling large brain...")
    large_metrics = profile_performance(
        lambda c: pynlm.createBrain(c),
        large_config,
        simulation_steps=200,
        profile_interval=50
    )
    
    print(f"\n=== Comparison ===")
    print(f"Small brain score: {small_metrics['performance_score']:.1f}/100")
    print(f"Large brain score: {large_metrics['performance_score']:.1f}/100")
```

### 4. Debug Log Viewer

A tool for viewing and analyzing debug logs from NLM simulations.

```python
import pynlm
import json
from datetime import datetime
from typing import List, Dict, Any

class DebugLogViewer:
    """Tool for viewing and analyzing NLM debug logs."""
    
    def __init__(self):
        self.logs = []
        self.current_session = []
        self.session_start = None
    
    def start_session(self, config_name="default"):
        """Start a new logging session."""
        self.session_start = datetime.now()
        self.current_session = []
        print(f"Starting debug session: {config_name} at {self.session_start}")
    
    def log_step(self, step: int, brain_stats: Dict[str, Any], agent_stats: Dict[str, Any] = None):
        """Log step information."""
        log_entry = {
            "timestamp": datetime.now().isoformat(),
            "step": step,
            "brain_stats": brain_stats,
            "agent_stats": agent_stats or {},
        }
        self.current_session.append(log_entry)
        self.logs.append(log_entry)
    
    def end_session(self):
        """End current session and save logs."""
        if self.current_session:
            filename = f"debug_log_{self.session_start.strftime('%Y%m%d_%H%M%S')}.json"
            with open(filename, 'w') as f:
                json.dump({
                    "session_info": {
                        "start_time": self.session_start.isoformat(),
                        "end_time": datetime.now().isoformat(),
                        "duration_seconds": (datetime.now() - self.session_start).total_seconds(),
                        "step_count": len(self.current_session),
                    },
                    "logs": self.current_session,
                }, f, indent=2)
            
            print(f"Debug session saved to {filename}")
            print(f"Logged {len(self.current_session)} steps")
        
        self.current_session = []
        self.session_start = None
    
    def analyze_session(self, filename: str = None):
        """Analyze and display session statistics."""
        if filename is None:
            if not self.logs:
                print("No logs available for analysis")
                return
            data = self.logs
        else:
            with open(filename, 'r') as f:
                data = json.load(f)["logs"]
        
        if not data:
            print("No data to analyze")
            return
        
        print("=== Debug Log Analysis ===")
        print(f"Total steps: {len(data)}")
        
        # Time analysis
        timestamps = [log["timestamp"] for log in data]
        start_time = datetime.fromisoformat(timestamps[0])
        end_time = datetime.fromisoformat(timestamps[-1])
        duration = (end_time - start_time).total_seconds()
        
        print(f"Duration: {duration:.2f} seconds")
        print(f"Average step time: {duration / len(data) * 1000:.2f} ms")
        
        # Brain statistics analysis
        firing_counts = [log["brain_stats"].get("firing_neuron_count", 0) for log in data]
        spike_counts = [log["brain_stats"].get("total_spikes", 0) for log in data]
        
        print(f"\nBrain activity:")
        print(f"  Firing neurons: {min(firing_counts)} - {max(firing_counts)}")
        print(f"  Average: {sum(firing_counts) / len(firing_counts):.1f}")
        print(f"  Total spikes: {sum(spike_counts)}")
        
        # Agent statistics analysis (if available)
        agent_firing_rates = []
        novelty_levels = []
        curiosity_levels = []
        
        for log in data:
            if "agent_stats" in log:
                agent_firing_rates.append(log["agent_stats"].get("firing_rate", 0))
                novelty_levels.append(log["agent_stats"].get("novelty_level", 0))
                curiosity_levels.append(log["agent_stats"].get("curiosity_level", 0))
        
        if agent_firing_rates:
            print(f"\nAgent activity:")
            print(f"  Average firing rate: {sum(agent_firing_rates) / len(agent_firing_rates):.3f}")
            print(f"  Average novelty: {sum(novelty_levels) / len(novelty_levels):.3f}")
            print(f"  Average curiosity: {sum(curiosity_levels) / len(curiosity_levels):.3f}")
        
        # Event analysis
        step_changes = [firing_counts[i] - firing_counts[i-1] for i in range(1, len(firing_counts))]
        significant_changes = [change for change in step_changes if abs(change) > 10]
        
        print(f"\nActivity changes:")
        print(f"  Significant changes (>10 neurons): {len(significant_changes)}")
        if significant_changes:
            print(f"  Average change magnitude: {sum(abs(c) for c in significant_changes) / len(significant_changes):.1f}")

# Usage example
if __name__ == "__main__":
    viewer = DebugLogViewer()
    
    # Create simulation
    config = pynlm.createDefaultConfig()
    config.set_default_brain_config()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Start logging
    viewer.start_session("learning_simulation")
    
    # Run simulation with logging
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log step
        brain_stats = {
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "active_neuron_count": brain.getActiveNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        }
        
        agent_stats = {
            "firing_rate": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "curiosity_level": agent.getCuriosityLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation_level": agent.getNeuromodulationLevel(),
        }
        
        viewer.log_step(step, brain_stats, agent_stats)
        
        # Update learning
        reward = 0.0
        if not world.getSensoryPercept().getInternal().empty():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # End session and analyze
    viewer.end_session()
    viewer.analyze_session()
```

## Installation and Usage

### Prerequisites

```bash
# Install dependencies
pip install psutil pandas matplotlib seaborn

# Install NLM
pip install pynlm
```

### Quick Start

```python
import pynlm

# Create brain and visualize
config = pynlm.createDefaultConfig()
config.set_default_brain_config()

brain = pynlm.createBrain(config)
brain.initialize()

# Quick visualization
print("Brain State:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simple simulation
for step in range(10):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("\nSimulation completed successfully!")
```

### Advanced Usage

```python
import pynlm
from nlm_tools import (
    console_visualizer,
    dump_brain_state,
    profile_performance,
    DebugLogViewer,
)

# Create advanced simulation
config = pynlm.createDefaultConfig()
config.set_research_config()  # Use research configuration

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run advanced simulation with visualization
print("Starting advanced simulation with visualization...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = 0.0
    if not world.getSensoryPercept().getInternal().empty():
        reward = world.getSensoryPercept().getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

# Dump brain state for analysis
dump_brain_state(brain, "research_simulation.json")

# Run performance profile
metrics = profile_performance(
    lambda c: pynlm.createBrain(c),
    lambda: config,
    simulation_steps=100,
    profile_interval=20
)

# Start debug logging and analysis
viewer = DebugLogViewer()
viewer.start_session("advanced_research")
# ... log simulation steps ...
viewer.end_session()
viewer.analyze_session()

print("Advanced simulation complete with full analysis!")
```

## Integration with Other Tools

### Jupyter Notebook Support

```python
# Create interactive visualization for Jupyter
import pynlm
import matplotlib.pyplot as plt
import seaborn as sns

# Enable inline plotting
%matplotlib inline

# Create brain and collect data
config = pynlm.createDefaultConfig()
config.set_default_brain_config()

brain = pynlm.createBrain(config)
brain.initialize()

# Collect data for plotting
steps = []
firing_rates = []
spike_counts = []
curiosity_levels = []

for step in range(50):
    brain.step(step)
    steps.append(step)
    firing_rates.append(brain.getFiringNeuronCount())
    spike_counts.append(brain.getTotalSpikeCount())
    if hasattr(brain, 'getCuriosityLevel'):
        curiosity_levels.append(brain.getCuriosityLevel())

# Create plots
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Firing neurons over time
axes[0, 0].plot(steps, firing_rates, 'b-', linewidth=2)
axes[0, 0].set_title('Firing Neurons Over Time')
axes[0, 0].set_xlabel('Step')
axes[0, 0].set_ylabel('Number of Firing Neurons')
axes[0, 0].grid(True, alpha=0.3)

# Total spikes over time
axes[0, 1].plot(steps, spike_counts, 'r-', linewidth=2)
axes[0, 1].set_title('Total Spikes Over Time')
axes[0, 1].set_xlabel('Step')
axes[0, 1].set_ylabel('Total Spikes')
axes[0, 1].grid(True, alpha=0.3)

# Firing rate distribution
axes[1, 0].hist(firing_rates, bins=20, alpha=0.7, color='green')
axes[1, 0].set_title('Firing Rate Distribution')
axes[1, 0].set_xlabel('Firing Neurons')
axes[1, 0].set_ylabel('Frequency')
axes[1, 0].grid(True, alpha=0.3)

# Curiosity levels (if available)
if curiosity_levels:
    axes[1, 1].plot(steps, curiosity_levels, 'purple', linewidth=2)
    axes[1, 1].set_title('Curiosity Levels Over Time')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Curiosity Level')
    axes[1, 1].grid(True, alpha=0.3)
else:
    axes[1, 1].text(0.5, 0.5, 'Curiosity data not available', 
                   ha='center', va='center', transform=axes[1, 1].transAxes)
    axes[1, 1].set_title('Curiosity Levels')

plt.tight_layout()
plt.show()
```

### Integration with Model Training

```python
# Export brain state for machine learning
import pynlm
import numpy as np
from sklearn.preprocessing import StandardScaler

# Create brain and collect training data
config = pynlm.createDefaultConfig()
config.set_default_brain_config()

brain = pynlm.createBrain(config)
brain.initialize()

# Collect training features
features = []
labels = []

for step in range(100):
    brain.step(step)
    
    # Extract features for ML training
    feature_vector = [
        brain.getTotalNeuronCount(),
        brain.getTotalSynapseCount(),
        brain.getFiringNeuronCount(),
        brain.getActiveNeuronCount(),
        brain.getAverageFiringRate(),
        brain.getExcitationInhibitionRatio(),
        brain.getDevelopmentalStage(),
    ]
    
    # Add agent features if available
    agent = pynlm.createAgentBrain(brain)
    agent_features = [
        agent.getCuriosityLevel(),
        agent.getNoveltyLevel(),
        agent.getPredictionError(),
        agent.getNeuromodulationLevel(),
    ]
    
    feature_vector.extend(agent_features)
    features.append(feature_vector)
    
    # Simple label (e.g., learning phase)
    labels.append(step % 10)  # Cyclic label

# Convert to numpy arrays
X = np.array(features)
y = np.array(labels)

# Scale features
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

print(f"Collected {len(features)} training samples")
print(f"Feature dimension: {X_scaled.shape[1]}")
print(f"Unique labels: {len(np.unique(y))}")

# Save for later use
np.save('brain_features.npy', X_scaled)
np.save('brain_labels.npy', y)

print("Training data saved for machine learning!")
```

## Troubleshooting

### Common Issues

1. **Memory Issues**
   - Use `profile_performance()` to identify memory bottlenecks
   - Try smaller brain configurations for debugging
   - Monitor memory usage during simulation

2. **Performance Issues**
   - Use `set_performance_config()` for faster simulations
   - Disable unnecessary features (structural plasticity, episodic memory)
   - Optimize for your specific use case

3. **Visualization Issues**
   - Ensure display environment is properly set up
   - Use console visualization for headless environments
   - Check terminal settings for ANSI escape codes

4. **Debug Logging Issues**
   - Use `DebugLogViewer()` for structured logging
   - Limit log verbosity for performance
   - Regularly analyze and export logs

### Debugging Commands

```python
# Quick performance check
from nlm_tools import profile_performance
metrics = profile_performance(
    lambda c: pynlm.createBrain(c),
    lambda: pynlm.createDefaultConfig(),
    simulation_steps=50,
    profile_interval=10
)

# Brain state dump
from nlm_tools import dump_brain_state
dump_brain_state(brain, "debug_brain.json")

# Interactive visualization
from nlm_tools import console_visualizer
console_visualizer(brain, agent, world, update_interval=1.0)

# Log analysis
from nlm_tools import DebugLogViewer
viewer = DebugLogViewer()
viewer.start_session("debug")
# ... simulation ...
viewer.end_session()
viewer.analyze_session()
```

## File Structure

The visualization and debugging tools are organized as follows:

```
nlm-tools/
├── __init__.py
├── visualization/
│   ├── console_visualizer.py
│   ├── graph_visualizer.py
│   └── performance_profiler.py
├── debugging/
│   ├── debug_log_viewer.py
│   ├── state_dumper.py
│   └── event_logger.py
├── analysis/
│   ├── pattern_analyzer.py
│   ├── performance_analyzer.py
│   └── visualization_generator.py
├── __main__.py
└── examples/
    ├── basic_usage.py
    ├── advanced_simulation.py
    └── machine_learning.py
```

## License

MIT License - See LICENSE file for details.