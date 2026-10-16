# NLM - Expert Usage Guide

## Overview

This guide covers advanced NLM usage for researchers, developers, and power users. It assumes familiarity with basic NLM concepts and focuses on:

- Advanced configuration and optimization
- Research-grade simulation setup
- Performance tuning and debugging
- Custom agent development
- Integration with external systems
- Advanced plasticity and learning mechanisms

## Table of Contents

1. [Architecture Deep Dive](#architecture-deep-dive)
2. [Research-Grade Brain Setup](#research-grade-brain-setup)
3. [Performance Optimization](#performance-optimization)
4. [Advanced Debugging](#advanced-debugging)
5. [Custom Agent Development](#custom-agent-development)
6. [Integration with External Systems](#integration-with-external-systems)
7. [Advanced Plasticity Rules](#advanced-plasticity-rules)
8. [Simulation Best Practices](#simulation-best-practices)
9. [Advanced Configuration Patterns](#advanced-configuration-patterns)
10. [Troubleshooting](#troubleshooting)

---

## Architecture Deep Dive

### Brain Components

The NLM brain consists of several integrated systems:

#### Core Processing Units
- **Neurons**: LIF neurons with event-driven spike propagation
- **Synapses**: Chemical and electrical synapses with dynamic weights
- **Regions**: Hierarchical organization of neural populations
- **Populations**: Groups of neurons with similar properties

#### Memory Systems
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern association and recall
- **Prediction Systems**: Forward and inverse models

#### Cognitive Systems
- **Neural Planner**: Action sequence evaluation and selection
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive focus dynamics
- **Prediction Error**: Learning signal generation

#### Neuromodulation Systems
- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation and intrinsic drive
- **Novelty**: Change detection and surprise
- **Prediction Error**: Learning signal generation

#### Developmental System
- **Plasticity Modulation**: Age-dependent learning rates
- **Structural Plasticity**: Synaptogenesis and pruning
- **Maturation**: Developmental stage progression

### Integration Architecture

The brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Research-Grade Brain Setup

### Recommended Configuration

For research-grade simulations, use this configuration:

```python
import pynlm

# Create research-grade brain
brain = pynlm.createBrainForTask(pynlm.TaskType.PredictionAndPlanning, 8000)

# Apply research configuration
brain = pynlm.configureForResearch(brain)

# Set up world and agent
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=32, visionHeight=32)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning mechanisms
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

### Research Configuration Parameters

#### Neural Scale
- **Neuron Count**: 5,000-50,000 for standard research
- **Region Count**: 2-10 for complex behaviors
- **Population Size**: 50-500 per region

#### Plasticity Configuration
```python
# Enable all plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.02)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("plasticity.reward_modulated.enable", True)
```

#### Neuromodulation Configuration
```python
# Configure all neuromodulators
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.sensitivity", 1.5)
config.set("neuromod.novelty.threshold", 0.1)
config.set("neuromod.prediction_error.scale", 1.5)
```

#### Memory Systems
```python
# Configure all memory systems
config.set("memory.working.capacity", 100)
config.set("memory.episodic.duration", 3600)  # 1 hour
config.set("memory.associative.patterns", 50)
config.set("prediction.enabled", True)
config.set("prediction.horizon", 10)
```

#### Developmental Configuration
```python
# Configure development
config.set("development.enabled", True)
config.set("development.start_age", 0.0)
config.set("development.end_age", 3600.0)  # 1 hour
config.set("development.plasticity_rate", 1.0)
```

### Simulation Best Practices for Research

#### Scale Considerations
- **Small Scale** (500-2,000 neurons): Quick prototyping
- **Medium Scale** (2,000-10,000 neurons): Standard research
- **Large Scale** (10,000-50,000 neurons): Complex behaviors
- **Very Large Scale** (50,000+ neurons): System-level analysis

#### Time Scale
- **Fast Processes**: Synaptic plasticity (ms scale)
- **Medium Processes**: Working memory (seconds scale)
- **Slow Processes**: Development (minutes scale)
- **Very Slow Processes**: Evolution (hours scale)

#### Multi-Scale Simulation
```python
# Run simulation at multiple time scales
for time_scale in [0.001, 0.01, 0.1]:
    config = pynlm.createDefaultConfig()
    config.set("simulation.timestep", time_scale)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation at this time scale
    for step in range(1000):
        brain.step(step)
```

## Performance Optimization

### Performance Monitoring

#### Basic Performance Stats
```python
# Get basic performance statistics
stats = brain.getPerformanceStats()
print(f"Neurons processed: {stats.neuronsPerSecond:.0f}/sec")
print(f"Spikes processed: {stats.spikesPerSecond:.0f}/sec")
print(f"Real-time factor: {stats.realTimeFactor:.2f}")
print(f"Memory usage: {stats.memoryUsageMB:.1f} MB")
```

#### Advanced Performance Profiling
```python
import time

# Start performance profiling
brain.startPerformanceProfiler()

# Run performance-critical section
start_time = time.time()
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        # Get intermediate statistics
        stats = brain.getPerformanceStats()
        elapsed = time.time() - start_time
        print(f"Step {step}:")
        print(f"  Rate: {stats.neuronsPerSecond:.0f} neurons/sec")
        print(f"  Memory: {stats.memoryUsageMB:.1f} MB")
        print(f"  Time factor: {elapsed / (step * 0.001):.1f}")

brain.stopPerformanceProfiler()
```

### Optimization Strategies

#### Memory Optimization
```python
# Use memory-efficient configurations
brain = pynlm.createBrainForTask(pynlm.TaskType.SimpleControl, 1000)

# Enable memory pools and optimizations
config.set("performance.memory_pools", True)
config.set("performance.soa_layout", True)
config.set("performance.sparse_connectivity", True)
```

#### Multithreading Configuration
```python
# Configure multithreading
config.set("performance.multithreading", True)
config.set("performance.thread_count", 4)
config.set("performance.work_stealing", True)
config.set("performance.thread_local_state", True)
```

#### SIMD Vectorization
```python
# Enable SIMD acceleration
config.set("performance.simd.enable", True)
config.set("performance.simd.instruction_set", "avx512")
config.set("performance.vectorization.width", 16)  # 16 float32
```

### Performance Benchmarking

#### Simple Brain Benchmark
```python
def benchmark_brain_config(config_name, neuron_count, steps):
    import time
    import pynlm
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    # Measure creation time
    start_time = time.time()
    brain = pynlm.createBrain(config)
    brain.initialize()
    creation_time = time.time() - start_time
    
    # Measure simulation time
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    simulation_time = time.time() - start_time
    
    # Get performance statistics
    stats = brain.getPerformanceStats()
    
    return {
        "config": config_name,
        "neurons": neuron_count,
        "steps": steps,
        "creation_time": creation_time,
        "simulation_time": simulation_time,
        "neurons_per_second": stats.neuronsPerSecond,
        "spikes_per_second": stats.spikesPerSecond
    }

# Run benchmarks
benchmarks = []
for neurons in [1000, 5000, 10000]:
    for steps in [1000, 5000, 10000]:
        benchmark = benchmark_brain_config(f"config_{neurons}_{steps}", neurons, steps)
        benchmarks.append(benchmark)
        print(f"Benchmark complete: {benchmark}")
```

## Advanced Debugging

### Debug Neuron Connections
```python
# Debug connections for a specific neuron
neuron_id = brain.getRegion(RegionId(1)).getAllNeurons()[0].getId()
connections = brain.debugNeuronConnections(neuron_id, include_weights=True)

print(f"Connections for neuron {neuron_id}:")
for conn in connections:
    print(f"  -> Neuron {conn['target']}: weight={conn['weight']}, delay={conn['delay']}")
```

### Memory Profiling
```python
# Profile memory usage
memory_profile = brain.profileMemoryUsage()
print(f"Total memory usage: {memory_profile.totalMemoryMB:.1f} MB")
print(f"Breakdown:")
print(f"  Neurons: {memory_profile.neuronMemoryMB:.1f} MB")
print(f"  Synapses: {memory_profile.synapseMemoryMB:.1f} MB")
print(f"  Memory pools: {memory_profile.poolMemoryMB:.1f} MB")
print(f"  Other: {memory_profile.otherMemoryMB:.1f} MB")
```

### Event-Driven Debugging
```python
# Enable event-driven processing
brain.enableEventDriven(True)

# Debug spike events
spike_events = brain.getSpikeSystem().getRecentSpikes(100)
print(f"Recent spike events: {len(spike_events)}")
for event in spike_events[-10:]:  # Show last 10
    print(f"  Neuron {event.source_neuron.index()} at t={event.timestamp}")
```

### Network Analysis
```python
# Export network graph
network_graph = brain.exportNetworkGraph("graphml")

# Analyze network properties
analysis = {
    "node_count": len(network_graph.nodes),
    "edge_count": len(network_graph.edges),
    "average_degree": network_graph.degree().mean(),
    "clustering_coefficient": network_graph.cluster_coefficient(),
    "diameter": network_graph.diameter(),
    "components": network_graph.number_of_connected_components()
}

print("Network analysis:")
for key, value in analysis.items():
    print(f"  {key}: {value}")
```

## Custom Agent Development

### Custom Sensory Transduction
```python
import numpy as np

# Custom sensory processing function
def custom_sensory_transduction(percept):
    # Enhanced processing with custom features
    vision = percept.getVision()
    
    # Apply custom transformations
    processed_vision = []
    for i in range(len(vision)):
        # Custom feature: emphasize edges and corners
        if i % 3 == 0:  # Every third pixel
            processed_vision.append(vision[i] * 1.5)
        else:
            processed_vision.append(vision[i] * 0.8)
    
    # Normalize
    max_val = max(processed_vision)
    if max_val > 0:
        processed_vision = [v / max_val for v in processed_vision]
    
    return processed_vision

# Create custom agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createCustomAgent(
    brain=brain,
    name="CustomAgent",
    sensory_transduction=custom_sensory_transduction,
    motor_decoding=lambda activity: max(activity)  # Custom decoding
)

# Test the custom agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)
```

### Custom Motor Control
```python
# Custom motor control with learned policies
class CustomMotorController:
    def __init__(self):
        self.action_history = []
        self.reward_history = []
    
    def custom_motor_decoding(self, activity):
        # Custom policy based on learned behavior
        if len(self.action_history) > 0:
            # Use reinforcement learning to select actions
            avg_reward = np.mean(self.reward_history[-10:])
            if avg_reward > 0.5:
                # Exploit: use known good action
                return self.action_history[np.argmax(self.reward_history)]
            else:
                # Explore: try new actions
                return np.random.choice([0, 1, 2])  # Random action
        
        # Default: use maximum activity
        return np.argmax(activity)
    
    def update_rewards(self, rewards):
        self.reward_history.extend(rewards)
        if len(self.reward_history) > 100:
            self.reward_history = self.reward_history[-100:]

# Create agent with custom motor controller
motor_controller = CustomMotorController()
agent = pynlm.createCustomAgent(
    brain=brain,
    name="RLAgent",
    sensory_transduction=None,  # Use default
    motor_decoding=motor_controller.custom_motor_decoding
)
```

### Custom Plasticity Rules
```python
# Custom STDP rule with different parameters
def custom_stdp_rule(pre_spike_time, post_spike_time, current_weight):
    # Custom STDP with asymmetric learning rates
    delta_t = post_spike_time - pre_spike_time
    
    if delta_t > 0:  # Post before pre: LTP
        weight_change = 0.03 * np.exp(-delta_t / 20.0)
    else:  # Pre before post: LTD
        weight_change = -0.025 * np.exp(delta_t / 20.0)
    
    # Add voltage dependence
    voltage_factor = min(max(current_weight, 0), 1)
    weight_change *= voltage_factor
    
    return weight_change

# Add custom plasticity rule
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.addCustomPlasticityRule("custom_stdp", custom_stdp_rule)
```

## Integration with External Systems

### ROS Integration
```python
# ROS integration for robotics
import rospy
from sensor_msgs.msg import Image
from geometry_msgs.msg import Twist

class ROSIntegratedAgent:
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # ROS subscribers and publishers
        rospy.Subscriber("/camera/image", Image, self.image_callback)
        rospy.Publisher("/cmd_vel", Twist, queue_size=10)
    
    def image_callback(self, msg):
        # Convert ROS image to NLM vision input
        vision = self.process_ros_image(msg)
        self.agent.processSensoryInput(vision)
        
        # Get action and publish to ROS
        action = self.agent.decodeMotorCommand()
        twist = self.action_to_twist(action)
        self.cmd_vel_pub.publish(twist)

# Initialize and run
if __name__ == "__main__":
    rospy.init_node('nlm_ros_integration')
    agent = ROSIntegratedAgent()
    rospy.spin()
```

### WebSocket Integration
```python
import websockets
import asyncio
import json

class WebSocketNLMInterface:
    def __init__(self, brain):
        self.brain = brain
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(brain)
    
    async def handle_client(self, websocket, path):
        await websocket.send(json.dumps({"status": "connected"}))
        
        while True:
            try:
                # Receive commands from client
                data = await websocket.recv()
                command = json.loads(data)
                
                if command["type"] == "step":
                    # Execute simulation step
                    self.world.update(0.1)
                    if "sensory" in command:
                        self.agent.processSensoryInput(command["sensory"])
                    self.brain.step(command.get("step", 0))
                    
                    # Send state back to client
                    state = {
                        "neurons_firing": self.brain.getFiringNeuronCount(),
                        "spike_count": self.brain.getTotalSpikeCount(),
                        "action": self.agent.decodeMotorCommand().value
                    }
                    await websocket.send(json.dumps(state))
                
                elif command["type"] == "reset":
                    # Reset simulation
                    self.world.reset()
                    self.agent.reset()
                    self.brain.reset()
                    await websocket.send(json.dumps({"status": "reset"}))
                
            except websockets.ConnectionClosed:
                break
    
    async def run_server(self, host="localhost", port=8765):
        async with websockets.serve(self.handle_client, host, port):
            await asyncio.Future()  # Run forever
```

### Database Integration
```python
import sqlite3

class NLMDatabase:
    def __init__(self, db_path=":memory:"):
        self.conn = sqlite3.connect(db_path)
        self.create_tables()
    
    def create_tables(self):
        cursor = self.conn.cursor()
        
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS simulations (
                id INTEGER PRIMARY KEY,
                start_time REAL,
                end_time REAL,
                neuron_count INTEGER,
                steps INTEGER,
                final_spike_count INTEGER
            )
        """)
        
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS brain_states (
                id INTEGER PRIMARY KEY,
                simulation_id INTEGER,
                step INTEGER,
                firing_neurons INTEGER,
                spike_count INTEGER,
                avg_firing_rate REAL,
                timestamp REAL,
                FOREIGN KEY (simulation_id) REFERENCES simulations(id)
            )
        """)
        
        self.conn.commit()
    
    def log_simulation(self, simulation_info):
        cursor = self.conn.cursor()
        
        cursor.execute("""
            INSERT INTO simulations 
            (start_time, end_time, neuron_count, steps, final_spike_count)
            VALUES (?, ?, ?, ?, ?)
        """, (
            simulation_info["start_time"],
            simulation_info["end_time"],
            simulation_info["neuron_count"],
            simulation_info["steps"],
            simulation_info["final_spike_count"]
        ))
        
        simulation_id = cursor.lastrowid
        
        # Log brain states for each step
        for state in simulation_info["brain_states"]:
            cursor.execute("""
                INSERT INTO brain_states 
                (simulation_id, step, firing_neurons, spike_count, avg_firing_rate, timestamp)
                VALUES (?, ?, ?, ?, ?, ?)
            """, (
                simulation_id,
                state["step"],
                state["firing_neurons"],
                state["spike_count"],
                state["avg_firing_rate"],
                state["timestamp"]
            ))
        
        self.conn.commit()
        return simulation_id
    
    def query_simulation(self, simulation_id):
        cursor = self.conn.cursor()
        
        cursor.execute("SELECT * FROM simulations WHERE id = ?", (simulation_id,))
        simulation = cursor.fetchone()
        
        if simulation:
            cursor.execute("SELECT * FROM brain_states WHERE simulation_id = ? ORDER BY step", (simulation_id,))
            states = cursor.fetchall()
            
            return {
                "id": simulation[0],
                "start_time": simulation[1],
                "end_time": simulation[2],
                "neuron_count": simulation[3],
                "steps": simulation[4],
                "final_spike_count": simulation[5],
                "brain_states": [
                    {
                        "step": row[2],
                        "firing_neurons": row[3],
                        "spike_count": row[4],
                        "avg_firing_rate": row[5],
                        "timestamp": row[6]
                    }
                    for row in states
                ]
            }
        
        return None
```

## Advanced Plasticity Rules

### Custom STDP Variants
```python
# Multiple learning rules for different scenarios
def synaptic_tagging(strength, pre_trace, post_trace):
    """Synaptic tagging and capture for lasting LTP"""
    # Trace dynamics
    pre_trace *= 0.95  # Decay
    post_trace *= 0.95  # Decay
    
    # Tag when presynaptic neuron spikes
    if pre_trace > 0:
        post_trace = min(post_trace + 0.1 * strength, 1.0)
    
    # Capture when postsynaptic neuron spikes
    if post_trace > 0:
        long_term_change = post_trace * strength
        return long_term_change
    
    return 0.0

# Apply synaptic tagging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.addCustomPlasticityRule("synaptic_tagging", synaptic_tagging)
```

### Homeostatic Plasticity
```python
# Homeostatic regulation of firing rates
def homeostatic_plasticity(target_rate, current_rate, current_weight):
    """Maintain stable firing rates through homeostatic mechanisms"""
    error = target_rate - current_rate
    
    # Synaptic scaling
    if abs(error) > 0.1:
        scale_factor = 1.0 + 0.01 * error
        return current_weight * scale_factor
    
    return 0.0

# Configure homeostatic plasticity
config.set("plasticity.homeostatic.enable", True)
config.set("plasticity.homeostatic.target_rate", 10.0)  # Hz
```

### Neuromodulated Plasticity
```python
# Plasticity modulated by neuromodulators
def neuromodulated_plasticity(weight, dopamine, curiosity, novelty):
    """Plasticity changes based on multiple neuromodulators"""
    # Dopamine: reward prediction error modulation
    dopamine_factor = 1.0 + 0.5 * dopamine
    
    # Curiosity: exploration-driven plasticity
    curiosity_factor = 1.0 + 0.2 * curiosity
    
    # Novelty: change detection enhancement
    novelty_factor = 1.0 + 0.3 * novelty
    
    # Combined effect
    plasticity_strength = dopamine_factor * curiosity_factor * novelty_factor
    
    return weight * plasticity_strength * 0.01

# Apply neuromodulated plasticity
brain.addCustomPlasticityRule("neuromodulated", neuromodulated_plasticity)
```

## Simulation Best Practices

### Common Pitfalls and Solutions

#### 1. Spike Event Overflow
```python
# Monitor spike events to prevent overflow
max_pending_spikes = 10000
if brain.getPendingSpikeEventCount() > max_pending_spikes:
    print("Warning: Spike event count approaching limit")
    # Reduce input or increase processing speed
```

#### 2. Memory Exhaustion
```python
# Monitor memory usage during long simulations
if brain.profileMemoryUsage().totalMemoryMB > 1000:  # 1GB limit
    print("Warning: Memory usage approaching limit")
    # Consider saving checkpoints more frequently
    # or reducing simulation scale
```

#### 3. Time Step Instability
```python
# Ensure stable time steps
if config.get("simulation.timestep", 0.001) < 0.0001:
    print("Warning: Very small time step may cause instability")
if config.get("simulation.timestep", 0.001) > 0.1:
    print("Warning: Large time step may miss events")
```

### Simulation Monitoring

#### Real-Time Monitoring
```python
import time
import threading
import matplotlib.pyplot as plt

class SimulationMonitor:
    def __init__(self, brain, update_interval=1.0):
        self.brain = brain
        self.update_interval = update_interval
        self.metrics = []
        self.running = False
        self.monitor_thread = None
    
    def collect_metrics(self):
        """Collect current simulation metrics"""
        return {
            "step": self.brain.getTotalStepCount(),
            "neurons_firing": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "timestamp": time.time()
        }
    
    def monitor_loop(self):
        """Continuous monitoring loop"""
        while self.running:
            metrics = self.collect_metrics()
            self.metrics.append(metrics)
            time.sleep(self.update_interval)
    
    def start_monitoring(self):
        self.running = True
        self.monitor_thread = threading.Thread(target=self.monitor_loop)
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join()
        
        # Generate plots
        self.plot_metrics()
    
    def plot_metrics(self):
        """Generate metrics plots"""
        if len(self.metrics) < 2:
            return
        
        steps = [m["step"] for m in self.metrics]
        firing_rates = [m["avg_firing_rate"] for m in self.metrics]
        
        plt.figure(figsize=(12, 8))
        
        plt.subplot(2, 1, 1)
        plt.plot(steps, firing_rates)
        plt.xlabel("Step")
        plt.ylabel("Average Firing Rate (Hz)")
        plt.title("Simulation Metrics")
        plt.grid(True)
        
        plt.subplot(2, 1, 2)
        spikes = [m["total_spikes"] for m in self.metrics]
        plt.plot(steps, spikes)
        plt.xlabel("Step")
        plt.ylabel("Total Spikes")
        plt.grid(True)
        
        plt.tight_layout()
        plt.savefig(f"simulation_metrics_{int(time.time())}.png")
        plt.show()
```

#### Automated Checkpointing
```python
import os
import pickle

class CheckpointManager:
    def __init__(self, checkpoint_dir="checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
        self.checkpoint_id = 0
    
    def save_checkpoint(self, brain, step):
        """Save brain state to checkpoint file"""
        checkpoint_file = os.path.join(
            self.checkpoint_dir, 
            f"checkpoint_{self.checkpoint_id:06d}.bin"
        )
        
        brain.save(checkpoint_file)
        
        # Also save metadata
        metadata = {
            "checkpoint_id": self.checkpoint_id,
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "timestamp": time.time()
        }
        
        metadata_file = os.path.join(
            self.checkpoint_dir,
            f"checkpoint_{self.checkpoint_id:06d}.meta"
        )
        
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f)
        
        self.checkpoint_id += 1
        return checkpoint_file
    
    def load_checkpoint(self, checkpoint_file):
        """Load brain state from checkpoint file"""
        if os.path.exists(checkpoint_file):
            return brain.load(checkpoint_file)
        return False
    
    def cleanup_old_checkpoints(self, max_checkpoints=10):
        """Remove old checkpoint files"""
        checkpoints = sorted([
            f for f in os.listdir(self.checkpoint_dir)
            if f.startswith("checkpoint_") and f.endswith(".bin")
        ])
        
        if len(checkpoints) > max_checkpoints:
            for old_checkpoint in checkpoints[:-max_checkpoints]:
                os.remove(os.path.join(self.checkpoint_dir, old_checkpoint))
                # Also remove corresponding metadata file
                metadata_file = old_checkpoint.replace(".bin", ".meta")
                if os.path.exists(os.path.join(self.checkpoint_dir, metadata_file)):
                    os.remove(os.path.join(self.checkpoint_dir, metadata_file))
```

### Performance Profiling

#### Memory Profiling
```python
import tracemalloc

def profile_memory_usage(brain, steps):
    """Profile memory usage during simulation"""
    tracemalloc.start()
    
    # Take initial snapshot
    snapshot1 = tracemalloc.take_snapshot()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Take periodic snapshots
        if step % 1000 == 0:
            snapshot = tracemalloc.take_snapshot()
            top_stats = snapshot.compare_to(snapshot1, 'lineno')
            
            print(f"Memory usage at step {step}:")
            for stat in top_stats[:5]:
                print(f"  {stat}")
    
    # Final snapshot
    snapshot2 = tracemalloc.take_snapshot()
    top_stats = snapshot2.compare_to(snapshot1, 'lineno')
    
    print(f"Total memory allocated: {sum(stat.size_diff for stat in top_stats)} bytes")
    
    tracemalloc.stop()
```

#### CPU Profiling
```python
import cProfile
import pstats
import io

def profile_cpu_usage(brain, steps):
    """Profile CPU usage during simulation"""
    pr = cProfile.Profile()
    
    # Start profiling
    pr.enable()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    # Stop profiling
    pr.disable()
    
    # Generate report
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats()
    
    print(s.getvalue())
```

## Advanced Configuration Patterns

### Configuration Templates
```python
# Define reusable configuration templates
def get_template_config(template_name):
    templates = {
        "simple": {
            "brain": {"neuron_count": 1000, "region_count": 1},
            "plasticity": {"stdp": {"enable": True}},
            "simulation": {"timestep": 0.001}
        },
        "complex": {
            "brain": {"neuron_count": 10000, "region_count": 5},
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.02},
                "hebbian": {"enable": True},
                "structural": {"enable": True}
            },
            "memory": {"working": {"capacity": 100}},
            "neuromod": {"dopamine": {"scale": 2.0}},
            "simulation": {"timestep": 0.001}
        },
        "research": {
            "brain": {"neuron_count": 50000, "region_count": 10},
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.03},
                "hebbian": {"enable": True},
                "structural": {"enable": True},
                "reward_modulated": {"enable": True}
            },
            "memory": {
                "working": {"capacity": 1000},
                "episodic": {"duration": 3600},
                "associative": {"patterns": 100}
            },
            "neuromod": {
                "dopamine": {"scale": 2.0},
                "curiosity": {"sensitivity": 1.5},
                "novelty": {"threshold": 0.1}
            },
            "development": {"enabled": True, "plasticity_rate": 1.0},
            "performance": {"optimization": True}
        }
    }
    
    return templates.get(template_name, templates["simple"])

# Use template
config = pynlm.createDefaultConfig()
template = get_template_config("research")
# Apply template configuration (implementation depends on Config class)
```

### Configuration Inheritance
```python
# Create base configuration with inheritance
class InheritedConfig:
    def __init__(self):
        self.base_config = pynlm.createDefaultConfig()
        self.overrides = {}
    
    def set_value(self, key, value):
        """Set configuration value with inheritance"""
        self.overrides[key] = value
    
    def get_config(self):
        """Get configuration with applied overrides"""
        config = pynlm.createDefaultConfig()
        
        # Apply base configuration
        for key in self.base_config.getKeys():
            if self.base_config.has(key):
                config.set(key, self.base_config.get(key))
        
        # Apply overrides (overrides base values)
        for key, value in self.overrides.items():
            config.set(key, value)
        
        return config
    
    def merge_configs(self, other_config):
        """Merge with another inherited configuration"""
        merged = InheritedConfig()
        
        # Merge base configurations
        merged.base_config = pynlm.createDefaultConfig()
        for key in self.base_config.getKeys():
            if self.base_config.has(key):
                merged.base_config.set(key, self.base_config.get(key))
        
        for key in other_config.base_config.getKeys():
            if other_config.base_config.has(key):
                merged.base_config.set(key, other_config.base_config.get(key))
        
        # Merge overrides
        merged.overrides = dict(self.overrides)
        merged.overrides.update(other_config.overrides)
        
        return merged

# Use inheritance
base = InheritedConfig()
base.set_value("brain.neuron_count", 5000)
base.set_value("plasticity.stdp.enable", True)

complex_config = InheritedConfig()
complex_config.set_value("brain.neuron_count", 20000)
complex_config.set_value("brain.region_count", 5)

# Merge configurations
research_config = base.merge_configs(complex_config)
final_config = research_config.get_config()
```

### Configuration Validation and Optimization
```python
# Comprehensive configuration validation and optimization
def optimize_configuration(config):
    """Optimize configuration for best performance"""
    optimized = pynlm.createDefaultConfig()
    
    # Copy all settings
    for key in config.getKeys():
        optimized.set(key, config.get(key))
    
    # Apply optimization rules
    if optimized.get("brain.neuron_count", 1000) > 50000:
        print("Warning: Large network may require significant memory")
    
    if optimized.get("simulation.timestep", 0.001) < 0.0001:
        print("Warning: Very small time step may cause instability")
        optimized.set("simulation.timestep", 0.001)
    
    if optimized.get("brain.connection_probability", 0.15) > 0.5:
        print("Warning: High connection probability may cause excessive activity")
        optimized.set("brain.connection_probability", 0.3)
    
    # Normalize configuration
    optimized = pynlm.normalizeConfig(optimized)
    
    # Validate configuration
    if not pynlm.validateConfig(optimized):
        print("Warning: Configuration failed validation")
        # Apply default valid configuration
        optimized = pynlm.createDefaultConfig()
    
    return optimized

# Use optimization
original_config = pynlm.createDefaultConfig()
optimized_config = optimize_configuration(original_config)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: No Neurons Firing
```python
# Problem: Brain initialized but no neurons are firing
# Solution: Check configuration and initialization

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Verify brain parameters
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")

# Check if any neurons are active
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Try increasing input
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Issue 2: Memory Usage Too High
```python
# Problem: Memory usage exceeds available memory
# Solution: Reduce simulation scale and optimize configuration

import psutil
import os

process = psutil.Process(os.getpid())
memory_mb = process.memory_info().rss / 1024 / 1024
print(f"Current memory usage: {memory_mb:.1f} MB")

# Reduce network size
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Reduce from 10000
config.set("brain.region_count", 1)    # Reduce from 5

# Enable memory optimization
config.set("performance.memory_pools", True)
config.set("performance.soa_layout", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory during simulation
for step in range(1000):
    brain.step(step)
    current_memory = process.memory_info().rss / 1024 / 1024
    if current_memory > memory_mb + 500:  # 500MB increase limit
        print(f"Warning: High memory usage at step {step}: {current_memory:.1f} MB")
        break
```

#### Issue 3: Simulation Too Slow
```python
# Problem: Simulation runs too slowly
# Solution: Optimize performance and reduce complexity

import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Time simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

elapsed = end_time - start_time
print(f"Simulation time: {elapsed:.2f} seconds")
print(f"Steps per second: {1000/elapsed:.0f}")

# Get performance statistics
stats = brain.getPerformanceStats()
print(f"Performance stats: {stats}")

# Optimize for speed
config = pynlm.createDefaultConfig()
config.set("performance.optimization", True)
config.set("performance.multithreading", True)
config.set("performance.thread_count", 4)

# Use batch processing for better performance
steps = list(range(1000))
times = [i * 0.001 for i in steps]
brain.batchStep(steps, times)
```

#### Issue 4: Incorrect Actions
```python
# Problem: Agent produces incorrect or unexpected actions
# Solution: Debug agent configuration and check sensory input

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check agent configuration
print(f"Agent enabled features:")
print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")

# Test agent with known input
test_vision = [0.0] * (world.getVisionWidth() * world.getVisionHeight())
test_percept = pynlm.SensoryPercept()
test_percept.setVision(test_vision)

agent.processSensoryInput(test_percept)
action = agent.decodeMotorCommand()

print(f"Action from test input: {action}")
```

### Error Messages and Fixes

#### "Failed to initialize brain"
```python
# Problem: Brain initialization fails
# Solution: Check configuration and dependencies

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Verify configuration
if not brain.getConfig().has("brain.neuron_count"):
    print("Error: Missing brain.neuron_count configuration")
    # Set default value
    config = brain.getConfig()
    config.set("brain.neuron_count", 1000)

if not brain.getConfig().has("plasticity.stdp.enable"):
    print("Error: Missing plasticity.stdp.enable configuration")
    config.set("plasticity.stdp.enable", True)

brain.initialize()
```

#### "Simulation step failed"
```python
# Problem: Simulation step fails
# Solution: Check brain state and step parameters

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check brain state before stepping
if brain.getTotalNeuronCount() == 0:
    print("Error: No neurons in brain")
    # Add neurons
    brain.addRegion("main")
    # Reinitialize
    brain.initialize()

# Try stepping with valid parameters
for step in range(100):
    try:
        brain.step(step)
        print(f"Step {step} completed successfully")
    except Exception as e:
        print(f"Error at step {step}: {e}")
        # Try with simpler configuration
        break
```

#### "Memory usage exceeded"
```python
# Problem: Memory usage exceeds system limits
# Solution: Reduce simulation scale and optimize memory usage

import psutil

process = psutil.Process()
memory_info = process.memory_info()
memory_mb = memory_info.rss / 1024 / 1024

print(f"Available memory: {psutil.virtual_memory().available / 1024 / 1024:.1f} MB")
print(f"Current memory usage: {memory_mb:.1f} MB")

# Check if we're running out of memory
if memory_mb > psutil.virtual_memory().available * 0.8:
    print("Warning: High memory usage - consider reducing simulation scale")

# Create smaller configuration for memory efficiency
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 1)
config.set("performance.memory_pools", True)
config.set("performance.soa_layout", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with memory monitoring
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        current_memory = process.memory_info().rss / 1024 / 1024
        print(f"Step {step}: {current_memory:.1f} MB")
        
        if current_memory > psutil.virtual_memory().available * 0.9:
            print("Error: Memory usage too high - simulation stopped")
            break
```

### Getting Help

#### Documentation Resources
- **Basic Usage**: `easy_usage.md` - Quick start guide
- **Advanced Usage**: This file (`expert_usage.md`) - Comprehensive advanced guide
- **Architecture**: `docs/ARCHITECTURE.md` - Detailed architecture information
- **Configuration**: `HOW_TO_USE.md` - Configuration guide

#### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions

#### Community Support
```python
# Join NLM community and get help
import webbrowser

# Open documentation
webbrowser.open("https://nlm.readthedocs.io")

# Check GitHub issues
webbrowser.open("https://github.com/nlm-project/nlm/issues")

# Join discussion forum
webbrowser.open("https://github.com/nlm-project/nlm/discussions")
```

#### Technical Support
```python
# Report bugs and request features
import requests

def report_issue(title, description, severity="medium"):
    """Report an issue to the NLM team"""
    issue_data = {
        "title": title,
        "description": description,
        "severity": severity,
        "timestamp": time.time()
    }
    
    # Send to issue tracker (example implementation)
    # response = requests.post("https://api.github.com/repos/nlm-project/nlm/issues", 
    #                         json=issue_data, headers={"Authorization": "token YOUR_TOKEN"})
    
    print(f"Issue reported: {title}")
    print(f"Description: {description}")
    print("The NLM team will review and respond to your issue.")

# Report a problem
report_issue(
    title="Performance issue with large network",
    description="Simulation runs too slowly with 10,000 neurons",
    severity="high"
)
```

This comprehensive expert usage guide provides detailed information for working with advanced NLM configurations and scenarios. For more specific questions or issues, refer to the official documentation and community resources.