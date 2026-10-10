# NLM Python API - Enhanced Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the experimental artificial developmental brain system. This enhanced documentation covers the complete API including core functionality, expert features, performance optimization, and practical examples for users of all skill levels.

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import nlm

# Create a default configuration
config = nlm.create_default_config()

# Create a brain
brain = nlm.create_brain(config)

# Create a world
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

# Create agent brain
agent_brain = nlm.create_agent_brain(brain)

# Run simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, step)
    world.update(0.001)
```

## Table of Contents

- [Installation Guide](#installation-guide)
- [API Reference](#api-reference)
- [Tutorials](#tutorials)
  - [Beginner Tutorial](#beginner-tutorial)
  - [Intermediate Tutorial](#intermediate-tutorial)
  - [Advanced Tutorial](#advanced-tutorial)
- [Examples](#examples)
  - [Basic Demo](#basic-demo)
  - [Expert Demo](#expert-demo)
  - [Performance Demo](#performance-demo)
  - [Visualization Demo](#visualization-demo)
  - [Research Demo](#research-demo)
  - [Production Demo](#production-demo)
- [Configuration Guide](#configuration-guide)
- [Performance Guide](#performance-guide)
- [Debugging Guide](#debugging-guide)
- [Migration Guide](#migration-guide)

## Installation Guide

### System Requirements

- Python 3.8+
- C++ compiler (for building from source)
- NumPy (optional, for expert features)

### Package Installation

#### pip Installation

```bash
pip install pynlm
```

#### Conda Installation

```bash
conda install pynlm
```

#### Development Setup

```bash
# Clone the repository
git clone <repository-url>
cd pynlm

# Install in development mode
pip install -e .

# Run tests
pytest tests/
```

### Build from Source

```bash
# Clone the repository
git clone <repository-url>
cd pynlm

# Configure with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Install Python bindings
pip install -e .
```

### Verification and Testing

```bash
# Run basic tests
python -m pytest tests/test_basic.py

# Run expert tests
python -m pytest tests/test_expert.py

# Run performance tests
python -m pytest tests/test_performance.py

# Run all tests
python -m pytest
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM simulation parameters.

```python
config = nlm.Config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
config.saveToFile('config.json')
```

#### Brain
Core neural network simulation.

```python
brain = nlm.Brain(config)
brain.initialize(world)
stats = brain.getStatistics()
```

#### AgentBrain
Interface for agent behavior control.

```python
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(brain, world)
agent_brain.processSensoryInput(percept)
action = agent_brain.decodeMotorCommand()
```

#### SimpleWorld
Simulation environment.

```python
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)
body = world.getAgentBody()
```

### Expert APIs

#### Performance Optimization

```python
from nlm import expert

# Memory pool for large simulations
pool = expert.MemoryPool(1024, 8192)

# SIMD operations
processor = expert.SIMDProcessor()

# Parallel processing
parallel = expert.ParallelProcessor(4)
```

#### Advanced Simulation

```python
# Custom plasticity rules
plasticity_rules = expert.CustomPlasticityRules()

# Advanced neuromodulation
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.8,
    'serotonin_influence': 0.6
})
```

#### Data Analysis

```python
# Neural signal processing
processor = expert.NeuralSignalProcessor()

# Pattern recognition
recognizer = expert.PatternRecognizer()

# Statistical analysis
analyzer = expert.StatisticalAnalyzer()
```

#### Custom Brain Architecture

```python
# Custom neuron models
neuron_model = expert.CustomNeuronModel("biophysical_lif")

# Synaptic dynamics
synaptic = expert.SynapticDynamics()
```

#### Experiment Management

```python
# Custom experiment loops
experiment = expert.CustomExperimentLoop(100, 0.001)

# Parallel experiment execution
runner = expert.ParallelExperimentRunner([experiment], 8)
```

### Exception Types

```python
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

### Configuration Presets

```python
# Default configuration
config = nlm.create_default_config()

# Minimal configuration
config = nlm.create_minimal_config()

# High performance configuration
config = nlm.create_high_performance_config()

# World presets
config = nlm.create_grid_world_config()
config = nlm.create_maze_world_config()
```

### Factory Functions

```python
# Core factory functions
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
agent_brain = nlm.create_agent_brain(brain)
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()

# Expert factory functions
batch = nlm.create_batch_processor("simulation_batch")
experiment_manager = nlm.create_experiment_manager()
```

## Tutorials

### Beginner Tutorial: Creating Your First Brain Simulation

This tutorial will guide you through creating your first NLM brain simulation.

#### Step 1: Setup and Configuration

```python
import nlm

# Create default configuration
config = nlm.create_default_config()

# Create brain and world
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

# Initialize systems
brain.initialize(world)
```

#### Step 2: Agent Brain Setup

```python
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(brain, world)

# Create monitoring
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()
```

#### Step 3: Running the Simulation

```python
# Run simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Generate and apply action
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    world.update(0.001)
    
    # Record performance
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

# Get performance report
print(monitor.getPerformanceReport())
```

### Intermediate Tutorial: Agent-Based Simulation with World Interaction

This tutorial focuses on creating agents that interact with their environment.

#### Building the Agent

```python
class BasicAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.sensory_inputs = []
        self.actions_taken = []
    
    def perceive(self):
        # Get all sensory inputs
        percept = self.world.getSensoryPercept()
        self.sensory_inputs.append(percept)
        return percept
    
    def decide(self, percept):
        # Process sensory input
        self.brain.processSensoryInput(percept)
        
        # Generate action
        action = self.brain.decodeMotorCommand()
        self.actions_taken.append(action)
        return action
    
    def act(self, action, step):
        # Apply action to world
        self.world.applyMotorCommand(action, step)
        self.world.update(0.001)
    
    def learn(self, percept):
        # Store experience for learning
        pass
    
    def run_step(self, step):
        # Complete agent step
        percept = self.perceive()
        action = self.decide(percept)
        self.act(action, step)
        self.learn(percept)
```

#### Using the Agent in Simulation

```python
# Create agent
agent = BasicAgent(agent_brain, world)

# Run simulation with agent
for step in range(200):
    agent.run_step(step)
    
    # Log performance
    if step % 10 == 0:
        energy = world.getAgentBody().energy
        print(f"Step {step}, Energy: {energy}")
```

### Advanced Tutorial: Performance Optimization and Expert Features

This tutorial demonstrates how to leverage expert APIs for high-performance simulations.

#### Performance Optimization Setup

```python
import nlm
from nlm import expert

# Create high-performance configuration
config = nlm.create_high_performance_config()

# Setup expert systems
memory_pool = expert.MemoryPool(8192, 65536)
simd_processor = expert.SIMDProcessor()
parallel_processor = expert.ParallelProcessor(8)

# Create performance monitor
monitor = nlm.create_performance_monitor()
```

#### Advanced Simulation with Expert Features

```python
# Create custom plasticity rules
custom_plasticity = expert.CustomPlasticityRules()

# Setup advanced neuromodulation
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.9,
    'curiosity_strength': 0.8,
    'novelty_threshold': 0.3
})

# Create custom connectivity patterns
connectivity = expert.CustomConnectivityPatterns()

# Initialize neural population manager
population_manager = expert.NeuralPopulationManager()
```

#### Performance Monitoring and Optimization

```python
# Run performance-optimized simulation
for step in range(1000):
    # Use SIMD for vectorized operations
    percept = world.getSensoryPercept()
    processed_percept = simd_processor.process_vector(
        percept, lambda x: [v * 0.5 for v in x]
    )
    
    # Process with agent brain
    agent_brain.processSensoryInput(processed_percept)
    
    # Generate action using parallel processing
    action = parallel_processor.distribute_work(
        agent_brain.decodeMotorCommand, [agent_brain]
    )[0]
    
    # Apply action
    world.applyMotorCommand(action, step)
    world.update(0.001)
    
    # Monitor performance
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

# Get detailed performance analysis
performance_report = monitor.getPerformanceReport()
print(f"Performance: {performance_report['simulation_speed']:.2f} steps/sec")
print(f"Memory usage: {performance_report['memory_usage']:.2f} MB")
```

## Examples

### Basic Demo: Simple Neural Network Simulation

This example demonstrates a basic neural network simulation.

```python
"""Basic NLM Demo - Simple Neural Network"""
import nlm
import time

def run_basic_demo():
    print("=== Basic NLM Demo ===")
    
    # Setup
    config = nlm.create_default_config()
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Simulation
    start_time = time.time()
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Generate action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
    
    end_time = time.time()
    
    # Results
    stats = brain.getStatistics()
    body = world.getAgentBody()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total neurons: {stats['total_neurons']}")
    print(f"Active neurons: {stats['active_neurons']}")
    print(f"Agent position: ({body.x}, {body.y})")
    print(f"Agent energy: {body.energy}")

if __name__ == "__main__":
    run_basic_demo()
```

### Expert Demo: Advanced Plasticity and Neuromodulation

This example demonstrates expert-level features with custom plasticity and neuromodulation.

```python
"""Expert NLM Demo - Advanced Plasticity and Neuromodulation"""
import nlm
from nlm import expert
import time

def run_expert_demo():
    print("=== Expert NLM Demo ===")
    
    # Setup with expert configuration
    config = nlm.create_high_performance_config()
    
    # Expert systems
    memory_pool = expert.MemoryPool(16384, 131072)
    simd_processor = expert.SIMDProcessor()
    parallel_processor = expert.ParallelProcessor(4)
    
    # Custom plasticity rules
    custom_plasticity = expert.CustomPlasticityRules()
    custom_plasticity.add_rule("hebbian", strength=0.7, window=10)
    custom_plasticity.add_rule("stdp", alpha=0.01, beta=0.005)
    
    # Advanced neuromodulation
    neuromodulation = expert.AdvancedNeuromodulation({
        'dopamine_strength': 0.9,
        'serotonin_influence': 0.7,
        'curiosity_strength': 0.8,
        'novelty_threshold': 0.3
    })
    
    # Create brain and world
    world = nlm.create_simple_world()
    world.configure(30, 30, 128, 128)
    world.setAgentStart(15, 15)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Simulation with expert features
    for step in range(500):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Apply expert processing
        processed_percept = simd_processor.process_vector(
            percept, lambda x: [v * 0.7 for v in x]
        )
        
        # Process with agent brain
        agent_brain.processSensoryInput(processed_percept)
        
        # Generate action with parallel processing
        action = parallel_processor.distribute_work(
            agent_brain.decodeMotorCommand, [agent_brain]
        )[0]
        
        # Apply neuromodulation
        action = neuromodulation.apply_modulation(action, step)
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Apply custom plasticity
        if step % 10 == 0:
            custom_plasticity.update_weights(brain, step)
    
    # Results
    stats = brain.getStatistics()
    body = world.getAgentBody()
    
    print(f"Expert simulation completed with {step + 1} steps")
    print(f"Total plasticity rules: {len(custom_plasticity.rules)}")
    print(f"Neuromodulation strength: {neuromodulation.current_strength}")
    print(f"Final performance stats: {stats}")

if __name__ == "__main__":
    run_expert_demo()
```

### Performance Demo: Large-Scale Simulation with Optimizations

This example demonstrates large-scale simulation with performance optimizations.

```python
"""Performance NLM Demo - Large-Scale Simulation"""
import nlm
from nlm import expert
import time
import tracemalloc

def run_performance_demo():
    print("=== Performance NLM Demo ===")
    
    # Memory tracking
    tracemalloc.start()
    
    # Setup with performance configuration
    config = nlm.create_high_performance_config()
    config.set('brain.simulated_neurons', 5000)
    
    # Expert performance systems
    memory_pool = expert.MemoryPool(32768, 262144)
    simd_processor = expert.SIMDProcessor(512)
    parallel_processor = expert.ParallelProcessor(8)
    
    # Create performance monitor
    monitor = nlm.create_performance_monitor()
    
    # Create world
    world = nlm.create_simple_world()
    world.configure(50, 50, 256, 256)
    world.setAgentStart(25, 25)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Large-scale simulation
    start_time = time.time()
    memory_snapshot1 = tracemalloc.take_snapshot()
    
    for step in range(2000):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Vectorized processing with SIMD
        processed_percept = simd_processor.process_vector(
            percept, lambda x: [v * 0.5 + (step % 100) * 0.005 for v in x]
        )
        
        # Process with agent brain
        agent_brain.processSensoryInput(processed_percept)
        
        # Generate action with parallel processing
        action = parallel_processor.distribute_work(
            agent_brain.decodeMotorCommand, [agent_brain]
        )[0]
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Record performance
        neurons_active = brain.getActiveNeuronCount()
        monitor.recordStep(neurons_active, 0, 0.001)
        
        # Progress output
        if step % 500 == 0:
            print(f"Completed {step + 1}/{2000} steps")
    
    end_time = time.time()
    memory_snapshot2 = tracemalloc.take_snapshot()
    
    # Performance analysis
    performance_report = monitor.getPerformanceReport()
    memory_stats = tracemalloc.get_traced_memory()
    
    tracemalloc.stop()
    
    # Results
    stats = brain.getStatistics()
    body = world.getAgentBody()
    
    print(f"\nPerformance Results:")
    print(f"Simulation time: {end_time - start_time:.2f} seconds")
    print(f"Steps per second: {performance_report['simulation_speed']:.2f}")
    print(f"Memory usage: {memory_stats[1] / 1024 / 1024:.2f} MB")
    print(f"Peak memory: {memory_stats[0] / 1024 / 1024:.2f} MB")
    print(f"Total neurons: {stats['total_neurons']}")
    print(f"Active neurons: {stats['active_neurons']}")
    print(f"Average iteration time: {performance_report['avg_iteration_time']:.6f} seconds")

if __name__ == "__main__":
    run_performance_demo()
```

### Visualization Demo: Advanced Neural Visualization

This example demonstrates advanced neural visualization capabilities.

```python
"""Visualization NLM Demo - Advanced Neural Visualization"""
import nlm
from nlm import expert
import matplotlib.pyplot as plt
import numpy as np

def run_visualization_demo():
    print("=== Visualization NLM Demo ===")
    
    # Setup
    config = nlm.create_default_config()
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Setup visualization pipeline
    pipeline = expert.VisualizationPipeline()
    real_time_viz = expert.RealTimeVisualizer(pipeline)
    
    # Simulation with visualization
    activity_data = []
    connection_data = []
    
    for step in range(300):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent brain
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Collect data for visualization
        neurons_active = brain.getActiveNeuronCount()
        activity_data.append(neurons_active)
        
        if step % 10 == 0:
            # Generate connection density plot
            density_plot = nlm.generate_connection_density_plot(brain)
            connection_data.append(density_plot)
    
    # Create visualization pipeline
    pipeline.add_step("neuron_activity", activity_data)
    pipeline.add_step("connection_density", connection_data)
    
    # Generate visualization
    viz_result = real_time_viz.render(
        "neuron_activity_plot",
        data=activity_data,
        title="Neural Activity Over Time"
    )
    
    # Show plots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    
    # Neuron activity plot
    ax1.plot(activity_data)
    ax1.set_title("Neural Activity Over Time")
    ax1.set_xlabel("Step")
    ax1.set_ylabel("Active Neurons")
    ax1.grid(True, alpha=0.3)
    
    # Connection density plot
    for i, conn_data in enumerate(connection_data):
        ax2.plot(conn_data, label=f"Step {i * 10}")
    
    ax2.set_title("Connection Density Over Time")
    ax2.set_xlabel("Step")
    ax2.set_ylabel("Connection Density")
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('neural_visualization.png', dpi=150)
    plt.show()
    
    print(f"Visualization completed with {len(activity_data)} data points")
    print("Plot saved as 'neural_visualization.png'")

if __name__ == "__main__":
    run_visualization_demo()
```

### Research Demo: Experiment Design and Analysis

This example demonstrates research-grade experiment design and analysis.

```python
"""Research NLM Demo - Experiment Design and Analysis"""
import nlm
from nlm import expert
import json
from datetime import datetime

def run_research_demo():
    print("=== Research NLM Demo ===")
    
    # Setup experiment manager
    experiment_manager = nlm.create_experiment_manager()
    
    # Define experimental conditions
    experimental_conditions = [
        {
            "name": "baseline",
            "config": nlm.create_default_config(),
            "description": "Default configuration with baseline parameters"
        },
        {
            "name": "high_plasticity",
            "config": nlm.create_minimal_config(),
            "description": "High plasticity rate configuration"
        },
        {
            "name": "high_performance",
            "config": nlm.create_high_performance_config(),
            "description": "Optimized for maximum performance"
        }
    ]
    
    # Create custom experiment loop
    experiment_loop = expert.CustomExperimentLoop(
        duration=200.0,
        time_step=0.001,
        loop_functions=[
            lambda state, iteration, dt: {"step": iteration, "time": iteration * dt},
            lambda state, iteration, dt: {"neurons": 1000 + iteration % 100},
            lambda state, iteration, dt: {"energy": 100 - iteration * 0.05}
        ]
    )
    
    # Add alert rules
    monitor = expert.ExperimentMonitor(0.5)
    monitor.add_alert_rule(
        "energy_depletion",
        lambda metrics: metrics.get("energy", 0) < 10,
        "Agent energy below critical threshold"
    )
    
    # Run experiments
    experiment_results = {}
    
    for condition in experimental_conditions:
        print(f"\nRunning experiment: {condition['name']}")
        
        # Create systems for this condition
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        brain = nlm.create_brain(condition["config"])
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Start monitoring
        experiment_id = f"{condition['name']}_{datetime.now().timestamp()}"
        monitor.start_monitoring(
            experiment_id,
            lambda metrics: {"active_neurons": brain.getActiveNeuronCount()}
        )
        
        # Run experiment
        result = experiment_loop.run()
        
        # Stop monitoring and get data
        monitoring_data = monitor.stop_monitoring(experiment_id)
        
        # Store results
        experiment_results[condition["name"]] = {
            "result": result,
            "monitoring": monitoring_data,
            "condition": condition,
            "brain_stats": brain.getStatistics()
        }
        
        print(f"Experiment completed with {result['total_iterations']} iterations")
    
    # Create result analyzer
    result_analyzer = expert.ResultAnalyzer(["statistical", "performance", "convergence"])
    
    # Analyze results
    analysis_report = {
        "timestamp": datetime.now().isoformat(),
        "conditions_tested": len(experimental_conditions),
        "total_experiments": len(experiment_results)
    }
    
    # Perform statistical analysis
    all_results = [exp["result"] for exp in experiment_results.values()]
    
    for analysis_type in ["statistical", "performance", "convergence", "comparative"]:
        try:
            analysis_result = result_analyzer.analyze_results(
                all_results, analysis_type=analysis_type
            )
            analysis_report[f"{analysis_type}_analysis"] = analysis_result
        except Exception as e:
            analysis_report[f"{analysis_type}_analysis_error"] = str(e)
    
    # Save results
    with open(f"research_results_{datetime.now().timestamp()}.json", "w") as f:
        json.dump(analysis_report, f, indent=2, default=str)
    
    print(f"\nResearch completed with {len(experiment_results)} experimental conditions")
    print(f"Results saved to: research_results_{datetime.now().timestamp()}.json")
    
    return analysis_report

if __name__ == "__main__":
    research_results = run_research_demo()
```

### Production Demo: Deployment and Monitoring

This example demonstrates production-grade deployment and monitoring.

```python
"""Production NLM Demo - Deployment and Monitoring"""
import nlm
from nlm import expert
import time
import threading
import socket
from datetime import datetime
import json

class NLMServer:
    """Production NLM server with monitoring and control"""
    
    def __init__(self, host="localhost", port=8080):
        self.host = host
        self.port = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((host, port))
        self.server_socket.listen(5)
        
        self.simulation_running = False
        self.simulation_thread = None
        self.monitoring_enabled = True
        self.checkpoint_manager = expert.CheckpointManager("./checkpoints", True, 300.0)
        
        # Production systems
        self.memory_pool = expert.MemoryPool(65536, 262144)
        self.simd_processor = expert.SIMDProcessor(256)
        self.parallel_processor = expert.ParallelProcessor(4)
        self.performance_profiler = expert.PerformanceProfiler()
        
    def create_production_config(self):
        """Create production-optimized configuration"""
        config = nlm.create_high_performance_config()
        config.set('brain.simulated_neurons', 3000)
        config.set('performance.monitoring_interval', 0.1)
        config.set('expert.performance_optimization.memory_pool_enabled', True)
        config.set('expert.performance_optimization.simd_processor_enabled', True)
        config.set('expert.performance_optimization.parallel_processor_enabled', True)
        return config
    
    def start_simulation(self):
        """Start production simulation"""
        if self.simulation_running:
            return False
        
        self.simulation_running = True
        self.simulation_thread = threading.Thread(target=self._run_production_simulation)
        self.simulation_thread.daemon = True
        self.simulation_thread.start()
        
        return True
    
    def _run_production_simulation(self):
        """Internal production simulation loop"""
        # Create production systems
        config = self.create_production_config()
        world = nlm.create_simple_world()
        world.configure(40, 40, 128, 128)
        world.setAgentStart(20, 20)
        
        brain = nlm.create_brain(config)
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        simulation_start_time = time.time()
        step_count = 0
        
        for step in range(10000):  # Large production run
            if not self.simulation_running:
                break
            
            step_start = time.time()
            self.performance_profiler.start_profiling("step_execution")
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Vectorized processing
            processed_percept = self.simd_processor.process_vector(
                percept, lambda x: [v * 0.6 for v in x]
            )
            
            # Process with agent brain
            agent_brain.processSensoryInput(processed_percept)
            
            # Generate action
            action = self.parallel_processor.distribute_work(
                agent_brain.decodeMotorCommand, [agent_brain]
            )[0]
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(0.001)
            
            # Record performance
            neurons_active = brain.getActiveNeuronCount()
            self.performance_profiler.recordStep(neurons_active, 0, 0.001)
            
            step_time = self.performance_profiler.stop_profiling("step_execution")
            step_count += 1
            
            # Create checkpoint periodically
            if step % 1000 == 0:
                checkpoint_name = self.checkpoint_manager.create_checkpoint(
                    f"production_checkpoint_{step}",
                    {
                        "step": step,
                        "time": time.time(),
                        "neurons_active": neurons_active,
                        "agent_position": (world.getAgentBody().x, world.getAgentBody().y)
                    }
                )
                print(f"Created checkpoint: {checkpoint_name}")
            
            # Log performance
            if step % 100 == 0 and self.monitoring_enabled:
                perf_stats = self.performance_profiler.get_stats("step_execution")
                print(f"Step {step}: {step_time:.6f}s, Active neurons: {neurons_active}")
        
        simulation_end_time = time.time()
        print(f"Production simulation completed in {simulation_end_time - simulation_start_time:.2f} seconds")
        print(f"Average step time: {perf_stats.get('total_time', 0) / perf_stats.get('calls', 1):.6f} seconds")
    
    def stop_simulation(self):
        """Stop production simulation"""
        self.simulation_running = False
        if self.simulation_thread:
            self.simulation_thread.join(timeout=30.0)
    
    def get_status(self):
        """Get current simulation status"""
        return {
            "simulation_running": self.simulation_running,
            "monitoring_enabled": self.monitoring_enabled,
            "performance_stats": self.performance_profiler.get_stats("step_execution"),
            "timestamp": datetime.now().isoformat()
        }
    
    def cleanup(self):
        """Cleanup resources"""
        self.stop_simulation()
        self.checkpoint_manager.cleanup_old_checkpoints(3600.0)  # Keep 1 hour

def run_production_demo():
    print("=== Production NLM Demo ===")
    
    # Create production server
    server = NLMServer()
    
    try:
        # Start simulation
        print("Starting production simulation...")
        if server.start_simulation():
            print("Simulation started successfully")
            
            # Monitor for 30 seconds
            start_time = time.time()
            while time.time() - start_time < 30:
                status = server.get_status()
                print(f"Status: Simulation running: {status['simulation_running']}, "
                      f"Avg step time: {status.get('performance_stats', {}).get('total_time', 0) / max(status.get('performance_stats', {}).get('calls', 1), 1):.6f}s")
                time.sleep(5)
            
            # Stop simulation
            print("\nStopping production simulation...")
            server.stop_simulation()
        else:
            print("Failed to start simulation")
        
        # Get final status
        final_status = server.get_status()
        print(f"\nProduction results:")
        print(f"Simulation status: {final_status['simulation_running']}")
        print(f"Performance stats available: {'step_execution' in final_status['performance_stats']}")
        
    finally:
        # Cleanup
        server.cleanup()
        print("Production demo completed successfully")

if __name__ == "__main__":
    run_production_demo()
```

## Configuration Guide

### Configuration Parameters

#### Core Configuration

- `brain.simulated_neurons`: Number of simulated neurons (int, default: 1000)
- `brain.min_plasticity_rate`: Minimum plasticity rate (float, default: 0.001)
- `brain.max_plasticity_rate`: Maximum plasticity rate (float, default: 0.1)
- `world.width`: World width in grid cells (int, default: 20)
- `world.height`: World height in grid cells (int, default: 20)
- `world.view_distance`: Sensory view distance (int, default: 64)

#### Neuromodulation Configuration

- `neuromodulation.dopamine_influence`: Dopamine influence strength (float, default: 0.8)
- `neuromodulation.curiosity_strength`: Curiosity drive strength (float, default: 0.6)
- `neuromodulation.novelty_threshold`: Novelty detection threshold (float, default: 0.5)

#### Performance Configuration

- `performance.memory_pool_size`: Memory pool size (int, default: 8192)
- `performance.enable_ssimd`: Enable SSE/SIMD instructions (bool, default: true)
- `performance.parallel_threads`: Number of parallel threads (int, default: 4)
- `performance.memory_profiling`: Enable memory profiling (bool, default: false)

### Configuration Presets

#### Default Configuration

```python
config = nlm.create_default_config()
# Includes all standard settings for basic simulations
```

#### Minimal Configuration

```python
config = nlm.create_minimal_config()
# Essential settings only for fast iteration
```

#### High Performance Configuration

```python
config = nlm.create_high_performance_config()
# Optimized for large-scale simulations with expert features
```

#### World Presets

```python
config = nlm.create_grid_world_config()
# Grid world for navigation tasks

config = nlm.create_maze_world_config()
# Maze world for problem-solving tasks
```

### Dynamic Configuration

```python
# Load configuration from JSON
config = nlm.Config()
config.loadFromFile('config.json')

# Update configuration dynamically
config.set('brain.simulated_neurons', 2000)
config.set('world.view_distance', 128)

# Validate configuration
config.validate()

# Save updated configuration
config.saveToFile('updated_config.json')
```

### Configuration Validation

```python
from nlm import ConfigurationError

try:
    config = nlm.Config()
    config.set('brain.simulated_neurons', 1000)
    config.validate()  # May raise ConfigurationError
    print("Configuration is valid")
except ConfigurationError as e:
    print(f"Configuration error: {e}")
```

## Performance Guide

### Memory Optimization

#### Memory Pool Management

```python
from nlm import expert

# Create memory pool for large simulations
pool = expert.MemoryPool(8192, 65536)

# Allocate memory from pool
memory_block = pool.allocate(1024)

# Free memory block
pool.free(memory_block)

# Monitor memory usage
profiler = expert.MemoryProfiler()
snapshot = profiler.take_snapshot("before_simulation")
```

#### Zero-Copy Interfaces

```python
from nlm import expert

# Create zero-copy interface
zcopy_interface = expert.ZeroCopyInterface()

# Create interface for data
interface_id = zcopy_interface.create_interface(
    data_ptr=sensor_data,
    size=len(sensor_data),
    dtype="float32"
)

# Read data without copying
data = zcopy_interface.read_data(interface_id)
```

### CPU Optimization

#### SIMD Operations

```python
from nlm import expert

# Create SIMD processor
processor = expert.SIMDProcessor(256)

# Process vector with SIMD
input_data = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0]
output_data = processor.process_vector(
    input_data,
    lambda x: [v * 0.5 for v in x]
)
```

#### Parallel Processing

```python
from nlm import expert

# Create parallel processor with 8 threads
processor = expert.ParallelProcessor(8)

# Distribute work across threads
results = processor.distribute_work(
    my_processing_function,
    list_of_data_items
)
```

### Simulation Scaling

#### Batch Processing

```python
from nlm import expert

# Create batch processor
batch = expert.BatchProcessor("simulation_batch")

# Add operations to batch
batch.addOperation(lambda: brain.reset())
batch.addOperation(lambda: world.reset())
batch.addOperation(lambda: agent_brain.initialize(brain, world))

# Execute all operations in batch
batch.execute()
```

#### Custom Allocator

```python
from nlm import expert

# Create custom allocator
allocator = expert.CustomAllocator("buddy", 64)

# Allocate memory with custom allocator
ptr = allocator.allocate(1024)

# Free memory
allocator.free(ptr)
```

### Monitoring and Profiling

#### Performance Monitoring

```python
from nlm import expert

# Create performance monitor
monitor = expert.PerformanceMonitor()

# Record simulation steps
for step in range(1000):
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

# Get performance report
report = monitor.getPerformanceReport()
print(f"Steps per second: {report['simulation_speed']}")
print(f"Average iteration time: {report['avg_iteration_time']:.6f}s")
```

#### Performance Profiling

```python
from nlm import expert

# Create performance profiler
profiler = expert.PerformanceProfiler()

# Profile specific operations
profiler.start_profiling("sensory_processing")
processed_percept = agent_brain.processSensoryInput(percept)
execution_time = profiler.stop_profiling("sensory_processing")

# Get profiling statistics
stats = profiler.get_stats("sensory_processing")
print(f"Execution time: {execution_time:.6f}s")
print(f"Average time: {stats['total_time'] / stats['calls']:.6f}s")
```

## Debugging Guide

### Common Errors

#### Configuration Errors

```python
from nlm import ConfigurationError

try:
    config = nlm.Config()
    config.set('brain.simulated_neurons', -100)  # Invalid value
    config.validate()
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    print("Check: brain.simulated_neurons must be a positive integer")
```

#### Simulation Errors

```python
from nlm import SimulationError

try:
    brain.initialize(world)
except SimulationError as e:
    print(f"Simulation error: {e}")
    print("Check: Ensure world and brain are properly configured")
```

#### Validation Errors

```python
from nlm import ValidationError

try:
    world.configure(20, 20, 64, 64)  # Invalid dimensions
except ValidationError as e:
    print(f"Validation error: {e}")
    print("Check: World dimensions must be positive integers")
```

### Memory Issues

#### Memory Leaks

```python
from nlm import expert

# Monitor memory usage
profiler = expert.MemoryProfiler()

# Take snapshots before and after simulation
snapshot1 = profiler.take_snapshot("before")
# ... run simulation ...
snapshot2 = profiler.take_snapshot("after")

# Analyze memory growth
analysis = profiler.analyze_growth(300.0)  # Analyze last 5 minutes
if analysis['growth_rate'] > 0.1:
    print("Warning: High memory growth rate detected")
```

#### Out of Memory

```python
from nlm import expert

# Create memory pool for large simulations
pool = expert.MemoryPool(32768, 131072)

# Check available memory
def check_memory_availability(pool, required_size):
    available = pool.max_size - pool.used
    if available < required_size:
        print(f"Not enough memory: {available} < {required_size}")
        return False
    return True

# Allocate memory safely
if check_memory_availability(pool, 1024):
    memory_block = pool.allocate(1024)
```

### Performance Bottlenecks

#### Identify Slow Operations

```python
from nlm import expert

# Create performance profiler
profiler = expert.PerformanceProfiler()

# Profile different operations
operations = [
    ("sensory_processing", lambda: agent_brain.processSensoryInput(percept)),
    ("motor_decoding", lambda: agent_brain.decodeMotorCommand()),
    ("world_update", lambda: world.update(0.001)),
    ("action_application", lambda: world.applyMotorCommand(action, step))
]

for op_name, op_func in operations:
    profiler.start_profiling(op_name)
    op_func()
    execution_time = profiler.stop_profiling(op_name)
    print(f"{op_name}: {execution_time:.6f}s")

# Get comprehensive performance report
report = profiler.getPerformanceReport()
print(f"Slowest operation: {report['slowest_operation']}")
print(f"Optimization opportunities: {report['optimization_recommendations']}")
```

### Error Reporting

```python
import traceback
from nlm import NLMError

def safe_simulation_run():
    try:
        # Run simulation
        for step in range(100):
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            world.update(0.001)
    
    except NLMError as e:
        # Log error details
        error_info = {
            "timestamp": datetime.now().isoformat(),
            "error_type": type(e).__name__,
            "error_message": str(e),
            "stack_trace": traceback.format_exc(),
            "step_count": step if 'step' in locals() else 0
        }
        
        # Save error to file
        with open(f"error_log_{datetime.now().timestamp()}.json", "w") as f:
            json.dump(error_info, f, indent=2)
        
        print(f"Simulation error logged: {error_info['error_type']}")
        raise
    
    except Exception as e:
        # Handle unexpected errors
        error_info = {
            "timestamp": datetime.now().isoformat(),
            "error_type": type(e).__name__,
            "error_message": str(e),
            "stack_trace": traceback.format_exc()
        }
        
        print(f"Unexpected error: {error_info['error_type']}")
        raise NLMError(f"Unexpected error in simulation: {e}") from e
```

## Migration Guide

### Version Differences

#### From NLM v1.0.0 to v2.0.0

**Breaking Changes:**

1. **New Exception Hierarchy**: All exceptions now inherit from `NLMError`
2. **Configuration System**: Enhanced `Config` class with JSON serialization
3. **Expert APIs**: New `expert` module with advanced features
4. **Factory Functions**: Added factory functions for common setups

#### Migration Steps

```python
# Old way (v1.0.0)
from nlm import Config, Brain

config = Config()
config.brain_simulated_neurons = 1000
brain = Brain(config)

# New way (v2.0.0)
from nlm import create_default_config, create_brain

config = create_default_config()
brain = create_brain(config)
```

### From Basic Bindings to Enhanced API

#### Old API (Basic)

```python
# Basic imports
from nlm import Config, Brain, create_default_config

# Simple configuration
config = Config()
config.set('brain.simulated_neurons', 1000)

# Basic brain
brain = Brain(config)

# Manual setup
world = create_simple_world()
world.configure(20, 20, 64, 64)
```

#### New API (Enhanced)

```python
# Enhanced imports
from nlm import create_default_config, create_brain, create_simple_world

# Enhanced configuration with preset
config = create_default_config()

# Enhanced brain with factory
brain = create_brain(config)

# Enhanced world setup
world = create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

# Additional expert features
from nlm import expert
memory_pool = expert.MemoryPool(1024, 8192)
simd_processor = expert.SIMDProcessor()
```

### Migration Examples

#### Example 1: Basic Migration

```python
# OLD (v1.0.0)
config = Config()
config.brain_simulated_neurons = 1000
config.world_width = 20
brain = Brain(config)
world = create_simple_world()
world.configure(20, 20, 64, 64)

# NEW (v2.0.0)
config = create_default_config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
brain = create_brain(config)
world = create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)
```

#### Example 2: Advanced Migration

```python
# OLD (v1.0.0)
from nlm import Config, Brain, Action, create_default_config

# Manual action creation
move_action = Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
look_action = Action.createLookAction('LookLeft')

# OLD factory
config = create_default_config()
brain = Brain(config)

# NEW (v2.0.0)
from nlm import create_brain

# Same actions (now available in enhanced API)
move_action = Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
look_action = Action.createLookAction('LookLeft')

# Enhanced factory
brain = create_brain(config)

# Additional expert features
from nlm import expert
batch = expert.BatchProcessor("my_batch")
batch.addOperation(lambda: brain.reset())
batch.execute()
```

### Backward Compatibility

#### Ensuring Compatibility

1. **Version Detection**: Use `nlm.__version__` to check current version
2. **Feature Detection**: Check for expert module availability
3. **Fallback Mechanisms**: Provide fallbacks for deprecated APIs
4. **Migration Tools**: Use migration helpers for automatic updates

```python
# Version check
import nlm
if nlm.__version__ == "2.0.0":
    print("Using enhanced NLM v2.0.0")

# Feature check
try:
    from nlm import expert
    print("Expert APIs available")
except ImportError:
    print("Expert APIs not available - using basic API")

# Migration helper
def migrate_config(old_config):
    """Migrate old configuration to new format"""
    new_config = create_default_config()
    # Migration logic here
    return new_config
```

### Documentation Index

- [Installation Guide](#installation-guide)
- [API Reference](#api-reference)
- [Tutorials](#tutorials)
  - [Beginner Tutorial](#beginner-tutorial)
  - [Intermediate Tutorial](#intermediate-tutorial)
  - [Advanced Tutorial](#advanced-tutorial)
- [Examples](#examples)
  - [Basic Demo](#basic-demo)
  - [Expert Demo](#expert-demo)
  - [Performance Demo](#performance-demo)
  - [Visualization Demo](#visualization-demo)
  - [Research Demo](#research-demo)
  - [Production Demo](#production-demo)
- [Configuration Guide](#configuration-guide)
- [Performance Guide](#performance-guide)
- [Debugging Guide](#debugging-guide)
- [Migration Guide](#migration-guide)

## Acknowledgments

This documentation was created for the enhanced NLM Python API, providing comprehensive support for users of all skill levels.

## License

MIT License. See LICENSE file for details.

## Author

NLM Team
