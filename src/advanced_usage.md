# Advanced NLM Usage Guide

## Overview

This guide provides advanced usage patterns, configuration options, and optimization techniques for power users working with the Neural Learning Machine (NLM) Phase 6 Final Integration.

## Advanced Configuration Management

### Configuration File Formats

NLM now supports multiple configuration file formats:

#### 1. Key-Value Format (Backward Compatible)
```
# configs/advanced.cfg
brain.neuron_count = 5000
plasticity.stdp.enable = true
neuromod.dopamine.scale = 2.0
```

#### 2. JSON Format (Phase 4 Enhancement)
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 2.0,
            "sensitivity": 1.5
        }
    }
}
```

#### 3. YAML Format (Future Support)
```yaml
brain:
  neuron_count: 5000
  synapse_density: 0.15

plasticity:
  stdp:
    enable: true
    learning_rate: 0.002

neuromod:
  dopamine:
    scale: 2.0
    sensitivity: 1.5
```

### Configuration Management Best Practices

#### 1. Configuration Layering
```python
import pynlm

# Load default configuration
config = pynlm.createDefaultConfig()

# Override with project-specific settings
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)

# Load from JSON file for complex configurations
config.loadFromFile("configs/production.json")

# Command line overrides
# ./nlm --config production.json --neuron_count=10000
```

#### 2. Environment-Specific Configurations
```python
# Development configuration
def get_dev_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)  # Small brain for rapid testing
    config.set("enable_development", True)
    config.set("enable_debug_logging", True)
    return config

# Production configuration
def get_prod_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 10000)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", False)
    config.set("checkpoint_interval", 1000.0)
    return config
```

## Advanced Simulation Scenarios

### 1. Memory-Intensive Learning Simulation
```python
import pynlm
import time

def run_memory_learning_simulation(steps=5000, memory_capacity_multiplier=2.0):
    """Run a simulation that focuses on memory system integration."""
    
    # Create enhanced configuration for memory-intensive tasks
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("working_memory_capacity", int(50 * memory_capacity_multiplier))
    config.set("episodic_memory_capacity", int(100 * memory_capacity_multiplier))
    config.set("semantic_memory_capacity", int(200 * memory_capacity_multiplier))
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Create world with rich environment
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Performance metrics tracking
    start_time = time.time()
    memory_events = []
    
    # Run simulation with advanced logging
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development stage
        agent.updateDevelopment(0.1)
        
        # Log memory events
        if brain.getTotalSpikeCount() % 100 == 0:
            memory_events.append({
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'memory_usage': len(brain.getSpikeSystem().getActiveNeurons()),
                'development_stage': brain.getDevelopmentalStage(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel()
            })
        
        # Performance checkpoint
        if step % 500 == 0 and step > 0:
            elapsed = time.time() - start_time
            steps_per_second = step / elapsed
            print(f"Performance: {steps_per_second:.1f} steps/sec, "
                  f"Memory events: {len(memory_events)}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\nSimulation completed in {total_time:.1f} seconds")
    print(f"Final memory usage: {len(memory_events)} events recorded")
    
    return {
        'total_steps': steps,
        'total_time': total_time,
        'steps_per_second': steps / total_time,
        'memory_events': memory_events,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount()
    }
```

### 2. Multi-Agent Collaborative Learning
```python
import pynlm
import threading
import queue

def create_collaborative_agents(config_template, num_agents=3):
    """Create multiple agents that collaborate in the same environment."""
    
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Create unique configuration for each agent
        agent_config = pynlm.createDefaultConfig()
        
        # Modify base config for this agent
        agent_config.set("brain.neuron_count", config_template.getOr<int64_t>("brain.neuron_count", 1000) // num_agents)
        agent_config.set("agent.id", i)
        agent_config.set("agent.collaboration_mode", True)
        
        # Create brain
        brain = pynlm.createBrain(agent_config)
        brain.initialize()
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # Enable different subsystems for diversity
        agent.enableRewardModulation(True)
        agent.enableCuriosity(i % 2 == 0)  # Alternate curiosity enable
        agent.enableStructuralPlasticity(i % 3 == 0)  # Every third agent has structural plasticity
        
        agents.append(agent)
        brains.append(brain)
    
    return agents, brains

def run_collaborative_simulation(agents, world, steps=1000):
    """Run simulation with multiple collaborating agents."""
    
    # Shared communication queue
    agent_messages = queue.Queue()
    
    def agent_worker(agent_id, agent, world, messages):
        """Worker function for individual agent."""
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Add agent ID to percept for collaboration
            percept.setAgentId(agent_id)
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            agent.brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check for messages
            while not messages.empty():
                message = messages.get()
                # Process message and influence decision
                if message['type'] == 'reward_boost':
                    agent.applyRewardModulation(message['value'], 0.0)
    
    # Create threads for each agent
    threads = []
    for i, (agent_id, agent) in enumerate(zip(range(len(agents)), agents)):
        thread = threading.Thread(
            target=agent_worker,
            args=(agent_id, agent, world, agent_messages)
        )
        threads.append(thread)
    
    # Start all agents
    start_time = time.time()
    for thread in threads:
        thread.start()
    
    # Wait for all agents to complete
    for thread in threads:
        thread.join()
    
    end_time = time.time()
    
    print(f"Collaborative simulation completed in {end_time - start_time:.1f} seconds")
    
    return {
        'total_steps': steps,
        'num_agents': len(agents),
        'total_time': end_time - start_time
    }
```

### 3. Real-Time Visualization with Performance Monitoring
```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np
import time

def run_real_time_simulation_with_visualization(steps=2000):
    """Run simulation with real-time visualization and performance monitoring."""
    
    # Create performance monitoring system
    performance_data = {
        'steps': [],
        'firing_rates': [],
        'memory_usage': [],
        'curiosity_levels': [],
        'times': []
    }
    
    # Create brain with visualization enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("visualization_enabled", True)
    config.set("visualization_update_rate", 30.0)
    config.set("enable_performance_logging", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Set up plotting
    plt.ion()
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
    
    start_time = time.time()
    
    for step in range(steps):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect performance data
        current_time = time.time()
        performance_data['steps'].append(step)
        performance_data['firing_rates'].append(brain.getAverageFiringRate())
        performance_data['memory_usage'].append(len(brain.getSpikeSystem().getActiveNeurons()))
        performance_data['curiosity_levels'].append(agent.getCuriosityLevel())
        performance_data['times'].append(current_time - start_time)
        
        # Update plots every 50 steps
        if step % 50 == 0:
            ax1.clear()
            ax1.plot(performance_data['steps'], performance_data['firing_rates'])
            ax1.set_title('Firing Rate Over Time')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Firing Rate')
            
            ax2.clear()
            ax2.plot(performance_data['steps'], performance_data['memory_usage'])
            ax2.set_title('Memory Usage Over Time')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Memory Usage')
            
            ax3.clear()
            ax3.plot(performance_data['steps'], performance_data['curiosity_levels'])
            ax3.set_title('Curiosity Level Over Time')
            ax3.set_xlabel('Step')
            ax3.set_ylabel('Curiosity Level')
            
            ax4.clear()
            ax4.plot(performance_data['times'], performance_data['firing_rates'])
            ax4.set_title('Performance Over Time')
            ax4.set_xlabel('Time (s)')
            ax4.set_ylabel('Firing Rate')
            
            plt.tight_layout()
            plt.pause(0.01)
        
        # Performance monitoring
        step_time = time.time() - step_start
        if step % 100 == 0:
            print(f"Step {step}/{steps} - "
                  f"Firing Rate: {brain.getAverageFiringRate():.3f} - "
                  f"Step Time: {step_time*1000:.1f}ms - "
                  f"Memory: {len(brain.getSpikeSystem().getActiveNeurons())}")
    
    plt.ioff()
    plt.show()
    
    return performance_data
```

## Performance Optimization Guide

### 1. Memory Optimization
```python
# Configure brain for memory-constrained environments
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain
config.set("working_memory_capacity", 25)
config.set("episodic_memory_capacity", 50)
config.set("enable_structural_plasticity", False)  # Disable to save memory
config.set("enable_development", False)  # Disable development

# Enable memory-efficient processing
brain = pynlm.createBrain(config)
brain.initialize()

# Use memory-efficient stepping for long simulations
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        brain.reset()  # Periodically reset to conserve memory
```

### 2. Speed Optimization
```python
# Configuration for maximum performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("enable_development", False)
config.set("enable_structural_plasticity", False)
config.set("visualization_enabled", False)
config.set("enable_curiosity", False)
config.set("enable_reward_modulation", False)

# Use parallel processing for multi-core systems
import multiprocessing

def process_batch(steps, config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(steps):
        brain.step(step)
    return brain

# Create pool of processes
with multiprocessing.Pool(processes=4) as pool:
    results = pool.map(process_batch, [(1000, config)] * 4)
```

### 3. Checkpointing and Recovery
```python
import pynlm
import os

def run_simulation_with_checkpointing(steps=5000, checkpoint_interval=100):
    """Run simulation with automatic checkpointing and recovery."""
    
    config = pynlm.createDefaultConfig()
    config.set("enable_checkpointing", True)
    config.set("checkpoint_interval", float(checkpoint_interval))
    
    # Check if there's a saved checkpoint
    checkpoint_file = "simulation_checkpoint.bin"
    if os.path.exists(checkpoint_file):
        print("Found saved checkpoint, loading...")
        brain = pynlm.createBrain(config)
        brain.load(checkpoint_file)
    else:
        # Create new simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run simulation with checkpointing
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Save checkpoint periodically
        if step > 0 and step % checkpoint_interval == 0:
            brain.save(f"simulation_checkpoint_{step}.bin")
            print(f"Checkpoint saved at step {step}")
    
    return brain
```

## Advanced Configuration Options

### Configuration Schema

```python
# Advanced configuration options for power users
ADVANCED_CONFIG_OPTIONS = {
    "brain": {
        "neuron_count": {"type": "int", "min": 100, "max": 100000, "default": 1000},
        "synapse_density": {"type": "float", "min": 0.01, "max": 1.0, "default": 0.1},
        "connection_probability": {"type": "float", "min": 0.01, "max": 1.0, "default": 0.05},
        "v_thresh": {"type": "float", "min": -100, "max": 0, "default": -50},
        "v_rest": {"type": "float", "min": -100, "max": 0, "default": -70},
        "tau_mem": {"type": "float", "min": 1, "max": 100, "default": 20},
        "tau_ref": {"type": "float", "min": 0.1, "max": 10, "default": 2}
    },
    "plasticity": {
        "stdp": {
            "enable": {"type": "bool", "default": True},
            "learning_rate": {"type": "float", "min": 0.0001, "max": 0.1, "default": 0.001},
            "tau_plus": {"type": "float", "min": 1, "max": 100, "default": 20},
            "tau_minus": {"type": "float", "min": 1, "max": 100, "default": 20}
        },
        "hebbian": {
            "enable": {"type": "bool", "default": True},
            "learning_rate": {"type": "float", "min": 0.0001, "max": 0.1, "default": 0.001}
        },
        "structural": {
            "enable": {"type": "bool", "default": True},
            "synaptogenesis_rate": {"type": "float", "min": 0.00001, "max": 0.1, "default": 0.0001},
            "pruning_rate": {"type": "float", "min": 0.000001, "max": 0.01, "default": 0.00001}
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": {"type": "float", "min": 0.1, "max": 10, "default": 1.0},
            "baseline": {"type": "float", "min": 0, "max": 1, "default": 0.0}
        },
        "curiosity": {
            "enable": {"type": "bool", "default": True},
            "threshold": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.5},
            "decay_rate": {"type": "float", "min": 0.001, "max": 0.1, "default": 0.01}
        },
        "novelty": {
            "enable": {"type": "bool", "default": True},
            "detection_threshold": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.3}
        }
    },
    "memory": {
        "working_capacity": {"type": "int", "min": 10, "max": 1000, "default": 50},
        "episodic_capacity": {"type": "int", "min": 10, "max": 1000, "default": 100},
        "semantic_capacity": {"type": "int", "min": 10, "max": 1000, "default": 200},
        "consolidation_threshold": {"type": "float", "min": 0.1, "max": 1.0, "default": 0.8}
    }
}
```

### Configuration Validation
```python
class ConfigValidator:
    def __init__(self, config):
        self.config = config
    
    def validate(self):
        errors = []
        
        # Validate neuron count
        neuron_count = self.config.getOr<int64_t>("brain.neuron_count", 1000)
        if neuron_count < 100 or neuron_count > 100000:
            errors.append(f"Invalid neuron_count: {neuron_count}. Must be between 100 and 100000")
        
        # Validate synaptic connections
        synapse_density = self.config.getOr<float>("brain.synapse_density", 0.1)
        if synapse_density <= 0 or synapse_density > 1.0:
            errors.append(f"Invalid synapse_density: {synapse_density}. Must be between 0.01 and 1.0")
        
        # Validate plasticity settings
        if self.config.getOr<bool>("plasticity.stdp.enable", True):
            stdp_rate = self.config.getOr<float>("plasticity.stdp.learning_rate", 0.001)
            if stdp_rate <= 0 or stdp_rate > 0.1:
                errors.append(f"Invalid STDP learning rate: {stdp_rate}")
        
        return errors
    
    def get_suggestions(self, key, value):
        suggestions = []
        if key == "brain.neuron_count" and value < 500:
            suggestions.append("Consider increasing neuron_count for more complex behavior")
        elif key == "plasticity.stdp.enable" and not value:
            suggestions.append("STDP is important for realistic learning - consider enabling")
        return suggestions
```

## Debugging and Troubleshooting

### 1. Common Issues and Solutions

```python
def debug_simulation(config, world, agent, steps=100):
    """Debug simulation with enhanced logging and diagnostics."""
    
    print("=== Starting Debug Mode ===")
    
    # Enable debug logging
    config.set("enable_debug_logging", True)
    config.set("log_level", "DEBUG")
    
    # Create brain with debug mode
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with debug subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run debug simulation
    for step in range(steps):
        print(f"\n--- Step {step} ---")
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        print(f"Percept: {percept}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Debug output
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
        print(f"Curiosity level: {agent.getCuriosityLevel()}")
        print(f"Novelty level: {agent.getNoveltyLevel()}")
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        print(f"Action: {action}")
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for issues
        if brain.getFiringNeuronCount() == 0:
            print("WARNING: No neurons firing - check brain initialization")
        
        if agent.getCuriosityLevel() < 0.1:
            print("INFO: Low curiosity - consider adjusting threshold")
        
        if step % 50 == 0:
            # Save checkpoint for analysis
            brain.save(f"debug_checkpoint_{step}.bin")
    
    return brain
```

### 2. Performance Profiling
```python
import cProfile
import pstats
import io

def profile_simulation(steps=1000):
    """Profile simulation performance."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Start profiling
    pr = cProfile.Profile()
    pr.enable()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    pr.disable()
    
    # Print profiling results
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats()
    
    print("=== Performance Profile ===")
    print(s.getvalue())
    
    return pr
```

### 3. Memory Leak Detection
```python
import tracemalloc
import pynlm

def detect_memory_leaks(steps=1000):
    """Detect memory leaks in simulation."""
    
    # Start memory tracing
    tracemalloc.start()
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    # Take initial snapshot
    snapshot1 = tracemalloc.take_snapshot()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        if step % 200 == 0:
            # Take snapshot and compare
            snapshot2 = tracemalloc.take_snapshot()
            
            # Compare snapshots
            top_stats = snapshot2.compare_to(snapshot1, 'lineno')
            
            print(f"\nMemory usage at step {step}:")
            for stat in top_stats[:5]:
                print(stat)
            
            snapshot1 = snapshot2
    
    tracemalloc.stop()
    return brain
```

## Integration with External Systems

### 1. Remote Simulation Control
```python
import pynlm
import json
import requests

class RemoteSimulationController:
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.base_url = f"http://{host}:{port}"
    
    def create_simulation(self, config, simulation_id):
        """Create a simulation on remote server."""
        response = requests.post(
            f"{self.base_url}/simulations",
            json={
                'config': config.to_dict(),
                'simulation_id': simulation_id
            }
        )
        return response.json()
    
    def run_simulation(self, simulation_id, steps):
        """Run simulation on remote server."""
        response = requests.post(
            f"{self.base_url}/simulations/{simulation_id}/run",
            json={'steps': steps}
        )
        return response.json()
    
    def get_results(self, simulation_id):
        """Get simulation results from remote server."""
        response = requests.get(f"{self.base_url}/simulations/{simulation_id}/results")
        return response.json()

# Usage
controller = RemoteSimulationController()
result = controller.create_simulation(config, "memory_learning_001")
controller.run_simulation("memory_learning_001", 5000)
results = controller.get_results("memory_learning_001")
```

### 2. Database Integration
```python
def save_simulation_to_database(brain, simulation_id, timestamp):
    """Save simulation results to database."""
    import sqlite3
    import json
    
    # Create database connection
    conn = sqlite3.connect('nlm_simulations.db')
    
    # Create tables if they don't exist
    conn.execute('''
        CREATE TABLE IF NOT EXISTS simulations (
            id TEXT PRIMARY KEY,
            timestamp TEXT,
            neuron_count INTEGER,
            total_spikes INTEGER,
            avg_firing_rate REAL,
            config TEXT
        )
    ''')
    
    conn.execute('''
        CREATE TABLE IF NOT EXISTS simulation_steps (
            simulation_id TEXT,
            step INTEGER,
            firing_neurons INTEGER,
            active_neurons INTEGER,
            FOREIGN KEY(simulation_id) REFERENCES simulations(id)
        )
    ''')
    
    # Save simulation metadata
    config_data = json.dumps({
        'neuron_count': brain.getTotalNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    })
    
    conn.execute(
        'INSERT INTO simulations (id, timestamp, neuron_count, total_spikes, avg_firing_rate, config) VALUES (?, ?, ?, ?, ?, ?)',
        (simulation_id, timestamp, brain.getTotalNeuronCount(),
         brain.getTotalSpikeCount(), brain.getAverageFiringRate(), config_data)
    )
    
    # Save step data
    for step in range(0, 1000, 10):  # Save every 10 steps
        conn.execute(
            'INSERT INTO simulation_steps (simulation_id, step, firing_neurons, active_neurons) VALUES (?, ?, ?, ?)',
            (simulation_id, step, brain.getFiringNeuronCount(),
             len(brain.getSpikeSystem().getActiveNeurons()))
        )
    
    conn.commit()
    conn.close()
```

### 3. Web Interface Integration
```python
def create_web_simulation_dashboard(brain, port=8050):
    """Create web interface for monitoring simulation."""
    
    from flask import Flask, render_template, jsonify
    import threading
    import time
    
    app = Flask(__name__)
    
    # Global simulation state
    simulation_state = {
        'running': False,
        'current_step': 0,
        'total_steps': 1000,
        'firing_rate': 0,
        'total_spikes': 0,
        'curiosity_level': 0,
        'development_stage': 0
    }
    
    @app.route('/')
    def index():
        return render_template('dashboard.html', state=simulation_state)
    
    @app.route('/api/state')
    def get_state():
        return jsonify(simulation_state)
    
    @app.route('/api/control/<action>')
    def control_simulation(action):
        if action == 'start':
            simulation_state['running'] = True
        elif action == 'stop':
            simulation_state['running'] = False
        elif action == 'reset':
            simulation_state['current_step'] = 0
            simulation_state['firing_rate'] = 0
            simulation_state['total_spikes'] = 0
        return jsonify({'success': True})
    
    def run_simulation_loop():
        """Run simulation in background thread."""
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        while simulation_state['running'] and simulation_state['current_step'] < simulation_state['total_steps']:
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(simulation_state['current_step'])
            
            # Update state
            simulation_state['current_step'] += 1
            simulation_state['firing_rate'] = brain.getAverageFiringRate()
            simulation_state['total_spikes'] = brain.getTotalSpikeCount()
            simulation_state['curiosity_level'] = agent.getCuriosityLevel()
            simulation_state['development_stage'] = brain.getDevelopmentalStage()
            
            time.sleep(0.1)  # Control update rate
    
    # Start simulation thread
    simulation_thread = threading.Thread(target=run_simulation_loop)
    simulation_thread.daemon = True
    simulation_thread.start()
    
    return app
```

## Next Steps for Advanced Users

### 1. Extend NLM with Custom Components
```python
# Example: Add custom neuron type
class CustomNeuron(pynlm.Neuron):
    def __init__(self):
        super().__init__()
        self.custom_param = 1.0
    
    def update(self, dt):
        # Custom update logic
        super().update(dt)
        self.custom_param *= 0.99  # Decay
    
    def spike(self):
        # Custom spike handling
        super().spike()
        # Additional custom logic
```

### 2. Create Custom Plasticity Rules
```python
def create_custom_plasticity_rule(pre_neuron, post_neuron, time_diff):
    """Example custom plasticity rule combining STDP and Hebbian."""
    
    # STDP component
    stdp_weight = 0.001 * np.exp(-abs(time_diff) / 20.0)
    
    # Hebbian component (co-activation)
    hebbian_weight = 0.01 * pre_neuron.getFiringHistory() * post_neuron.getFiringHistory()
    
    # Combined rule
    weight_change = stdp_weight + hebbian_weight
    
    return weight_change
```

### 3. Implement Custom Learning Objectives
```python
class CustomLearningObjective:
    def __init__(self):
        self.target_behavior = None
        self.reward_function = None
    
    def calculate_reward(self, brain, world, action):
        """Custom reward function based on specific behavior."""
        
        # Example: Reward exploration of novel areas
        if self.target_behavior == 'exploration':
            reward = self.calculate_exploration_reward(brain, world, action)
        
        # Example: Reward efficient problem solving
        elif self.target_behavior == 'efficiency':
            reward = self.calculate_efficiency_reward(brain, world, action)
        
        # Example: Reward social cooperation
        elif self.target_behavior == 'cooperation':
            reward = self.calculate_cooperation_reward(brain, world, action)
        
        return reward
    
    def calculate_exploration_reward(self, brain, world, action):
        """Reward exploration of new areas."""
        # Implementation would depend on world state and action
        return 1.0  # Placeholder
    
    def calculate_efficiency_reward(self, brain, world, action):
        """Reward efficient task completion."""
        # Implementation would depend on task metrics
        return 0.5  # Placeholder
    
    def calculate_cooperation_reward(self, brain, world, action):
        """Reward cooperative behavior."""
        # Implementation would depend on multi-agent interactions
        return 0.8  # Placeholder
```

## Conclusion

The NLM Phase 6 Final Integration provides a powerful platform for advanced neural network simulation. By leveraging the advanced configuration options, comprehensive documentation, and sophisticated example scenarios, power users can:

1. **Deploy complex, memory-intensive learning simulations**
2. **Implement multi-agent collaborative learning systems**
3. **Create real-time visualization dashboards**
4. **Optimize performance for specific use cases**
5. **Debug and troubleshoot complex simulation issues**
6. **Integrate with external systems and databases**

The combination of enhanced configuration management, advanced simulation scenarios, and comprehensive documentation makes NLM an excellent choice for research, development, and production environments requiring sophisticated neural network simulation capabilities.

Remember to always start with simpler configurations and gradually increase complexity as you become familiar with the system's capabilities.

---

*For the latest updates, documentation, and community support, visit the NLM project documentation and forums.*
