# NLM Advanced Usage Guide

This document provides advanced usage examples and configuration options for the NLM (Neural Learning Machine) Python bindings.

## Advanced Configuration

### 1. Advanced Configuration Setup

```python
import pynlm

# Create advanced configuration with optimal settings
config = pynlm.createAdvancedConfig()

# Configure brain architecture
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 15)
config.set("brain.connection_probability", 0.12)

# Configure learning rules
config.set("learning.stdp_ltp_weight", 0.025)
config.set("learning.stdp_ltd_weight", 0.0225)
config.set("learning.hebbian_strength", 0.15)
config.set("learning.reward_decay", 0.9)

# Configure development
config.set("development.initial_plasticity", 1.2)
config.set("development.critical_period_duration", 150)
config.set("development.maturation_rate", 0.8)

# Configure world parameters
config.set("world.width", 30.0)
config.set("world.height", 30.0)
config.set("world.max_energy", 200.0)
config.set("world.energy_decay_rate", 0.005)

# Save configuration to file
config.saveToFile("advanced_config.json")
print("Advanced configuration saved to advanced_config.json")
```

### 2. Custom Learning Rules

```python
import pynlm

# Create custom STDP learning rule
stdp_rule = pynlm.createLearningRule()
stdp_rule.setType(pynlm.LearningRule.RuleType.STDP)
stdp_rule.setParameter("stdp_window", 30.0)
stdp_rule.setParameter("stdp_ltp", 0.03)
stdp_rule.setParameter("stdp_ltd", 0.028)
stdp_rule.setLearningRate(0.02)

# Validate configuration
if stdp_rule.validate():
    print("STDP learning rule is valid")
else:
    print("Validation error:", stdp_rule.getValidationError())

# Create Hebbian learning rule
hebbian_rule = pynlm.createLearningRule()
hebbian_rule.setType(pynlm.LearningRule.RuleType.Hebbian)
hebbian_rule.setParameter("hebbian_strength", 0.2)
hebbian_rule.setLearningRate(0.015)

# Create reward-modulated learning rule
reward_rule = pynlm.createLearningRule()
reward_rule.setType(pynlm.LearningRule.RuleType.RewardModulated)
reward_rule.setParameter("reward_decay", 0.95)
reward_rule.setParameter("reward_learning_rate", 0.08)
```

### 3. Advanced Architecture Configuration

```python
import pynlm

# Create architecture configuration
arch_config = pynlm.createArchitectureConfig()
arch_config.setNeuronCount(5000)
arch_config.setRegionCount(20)
arch_config.setConnectionProbability(0.18)

# Add different brain regions
arch_config.addRegion("sensory", 1000, 1.0, 0.8)
arch_config.addRegion("motor", 800, 0.9, 0.7)
arch_config.addRegion("association", 2000, 0.8, 1.2)
arch_config.addRegion("memory", 1200, 1.1, 0.9)

# Validate architecture
if arch_config.validate():
    print("Architecture configuration is valid")
else:
    print("Architecture validation error:", arch_config.getValidationError())

# Export to string for saving
arch_string = arch_config.exportToString()
print("Architecture exported:", arch_string[:100] + "...")
```

## Performance Monitoring

### 1. Basic Performance Monitoring

```python
import pynlm
import time

# Create brain and world
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world and agent
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()
agent.initialize(world)

# Create performance metrics
metrics = pynlm.createPerformanceMetrics()

# Run simulation with monitoring
print("Starting performance-monitored simulation...")
start_time = time.time()

for step in range(1000):
    world.update(0.1)
    
    # Update performance metrics
    metrics.update(step, time.time() - start_time)
    
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinking
    brain.step(step)
    
    # Agent decision making
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 0.1  # Example reward
    agent.applyRewardModulation(reward, 0.0)

print("\n=== Performance Summary ===")
print(metrics.getFormattedSummary())
print("\n=== Current Metrics ===")
print(metrics.getFormattedMetrics())

# Export metrics for analysis
print("\nExporting metrics...")
print("CSV format (first 10 lines):")
print(metrics.exportCSV()[:1000] + "...")
```

### 2. Advanced Performance Analytics

```python
import pynlm
import matplotlib.pyplot as plt

# Run simulation and collect metrics
brain = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(brain)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Create multiple metrics collectors
performance_data = []
spike_rates = []
memory_usage = []
learning_progress = []

for episode in range(10):
    # Reset for new episode
    brain.reset()
    world.reset()
    agent.reset()
    
    # Create metrics for this episode
    episode_metrics = pynlm.createPerformanceMetrics()
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        episode_metrics.update(step, step * 0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = 0.1
        agent.applyRewardModulation(reward, 0.0)
    
    # Collect performance data
    performance_data.append(episode_metrics)
    spike_rates.append(episode_metrics.getCurrentMetrics().spikeCount)
    memory_usage.append(episode_metrics.getCurrentMetrics().memoryUsage)
    learning_progress.append(episode_metrics.getCurrentMetrics().learningProgress)
    
    print(f"Episode {episode + 1} completed: "
          f"Avg Spikes={episode_metrics.getAverageSpikeCount():.1f}, "
          f"Peak Memory={episode_metrics.getPeakMemoryUsage():.1f} MB")

# Analyze performance trends
print("\n=== Performance Analysis ===")
print(f"Average spike rate: {sum(spike_rates)/len(spike_rates):.1f}")
print(f"Average memory usage: {sum(memory_usage)/len(memory_usage):.1f} MB")
print(f"Average learning progress: {sum(learning_progress)/len(learning_progress):.2f}")
print(f"Peak memory usage: {max(memory_usage):.1f} MB")
print(f"Learning consistency: {100.0 * (1.0 - (max(learning_progress) - min(learning_progress))) / max(learning_progress):.1f}%")
```

## Batch Processing

### 1. Batch Simulation Processing

```python
import pynlm
from datetime import datetime

# Create brain and world
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world
world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Create batch inputs
inputs = []
rewards = []

# Generate simulated sensory inputs for batch processing
for step in range(100):
    percept = pynlm.SensoryPercept()
    # Add vision data
    vision_data = [0.1 * (step % 16), 0.2 * ((step + 8) % 16)]
    percept.setVision(vision_data)
    
    # Add touch data
    touch_data = [0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    percept.setTouch(touch_data)
    
    inputs.append(percept)
    rewards.append(0.1)  # Simple positive reward

print("Running batch simulation...")
start_time = datetime.now()

# Run batch simulation
result = pynlm.runBatchSimulation(brain, inputs, rewards, 0.1, 100)

end_time = datetime.now()
duration = (end_time - start_time).total_seconds()

print(f"Batch simulation completed in {duration:.2f} seconds")
print(f"Processed {len(inputs)} input sequences in {len(rewards)} steps")
print(f"Average processing time: {duration / len(inputs) * 1000:.2f} ms per sequence")
```

### 2. Parallel Processing

```python
import pynlm
import threading
import time

# Create brain configuration
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)

# Create multiple simulation steps for parallel processing
steps = []
for i in range(1000):
    steps.append((i, i * 0.001))

print("Running parallel simulation...")
start_time = time.time()

# Run parallel simulation
result = pynlm.runParallelSimulation(brain, steps)

end_time = time.time()
duration = end_time - start_time

print(f"Parallel simulation completed in {duration:.2f} seconds")
print(f"Processed {len(steps)} steps in parallel")
print(f"Speedup factor: {duration / (len(steps) * 0.001):.1f}x")
```

## Checkpoint System

### 1. Basic Checkpoint Management

```python
import pynlm
import os

# Create brain and world
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world and agent
world.configure(width=60, height=60, visionWidth=24, visionHeight=24)
world.reset()
agent.initialize(world)

# Run some simulation steps
print("Running simulation...")
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Save checkpoint
checkpoint_dir = "checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

checkpoint_file = os.path.join(checkpoint_dir, "simulation_checkpoint_2024.pkl")
pynlm.saveCheckpoint(checkpoint_file, brain)

print(f"Checkpoint saved to: {checkpoint_file}")

# Simulate loading checkpoint (in real scenario, would restore from file)
print("\nSimulating checkpoint restoration...")
# In real usage: restored_brain = pynlm.loadCheckpoint(checkpoint_file, config)
# restored_brain would continue from the saved state
```

### 2. Advanced Checkpoint Management

```python
import pynlm
import glob
from datetime import datetime

# Create checkpoint directory
checkpoint_dir = "experiments/checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

# Function to manage checkpoints
def save_experiment_checkpoint(brain, experiment_name, step):
    """Save checkpoint for experiment"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    checkpoint_file = os.path.join(checkpoint_dir, 
                                 f"{experiment_name}_checkpoint_{timestamp}_step{step}.pkl")
    pynlm.saveCheckpoint(checkpoint_file, brain)
    return checkpoint_file

def load_latest_checkpoint(experiment_name, config):
    """Load latest checkpoint for experiment"""
    pattern = os.path.join(checkpoint_dir, f"{experiment_name}_checkpoint_*.pkl")
    checkpoints = glob.glob(pattern)
    
    if not checkpoints:
        print(f"No checkpoints found for experiment: {experiment_name}")
        return None
    
    # Find latest checkpoint
    latest_checkpoint = max(checkpoints, key=os.path.getctime)
    print(f"Loading latest checkpoint: {latest_checkpoint}")
    return pynlm.loadCheckpoint(latest_checkpoint, config)

def cleanup_old_checkpoints(experiment_name, keep_last=5):
    """Clean up old checkpoints, keeping only the last N"""
    pattern = os.path.join(checkpoint_dir, f"{experiment_name}_checkpoint_*.pkl")
    checkpoints = glob.glob(pattern)
    
    if len(checkpoints) <= keep_last:
        return
    
    # Sort by creation time and remove oldest
    checkpoints.sort(key=os.path.getctime)
    for old_checkpoint in checkpoints[:-keep_last]:
        try:
            os.remove(old_checkpoint)
            print(f"Removed old checkpoint: {old_checkpoint}")
        except OSError as e:
            print(f"Error removing checkpoint {old_checkpoint}: {e}")

# Example usage
print("=== Checkpoint Management Demo ===")

# Create experiment brain
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)

# Run simulation and save checkpoints every 50 steps
for step in range(200):
    if step % 50 == 0 and step > 0:
        checkpoint_file = save_experiment_checkpoint(brain, "learning_experiment", step)
        print(f"Checkpoint saved at step {step}: {checkpoint_file}")
    
    # Run simulation step
    brain.step(step)

# List all checkpoints for this experiment
print("\n=== Checkpoints for learning_experiment ===")
pattern = os.path.join(checkpoint_dir, "learning_experiment_checkpoint_*.pkl")
checkpoints = glob.glob(pattern)
for checkpoint in sorted(checkpoints):
    stat = os.stat(checkpoint)
    print(f"{checkpoint} ({stat.st_size} bytes, {stat.st_mtime})")

# Cleanup old checkpoints
cleanup_old_checkpoints("learning_experiment", keep_last=3)
```

## Experiment Management

### 1. Basic Experiment Management

```python
import pynlm
import json

# Create experiment configuration
experiment_config = pynlm.createExperimentConfig()
experiment_config.setName("Neural Development Study")
experiment_config.setDescription("Study of neural development through reinforcement learning")
experiment_config.setParameter("learning_rate", 0.001)
experiment_config.setParameter("plasticity", 0.8)
experiment_config.setParameter("development_rate", 0.1)
experiment_config.setNumSteps(500)
experiment_config.setTimestep(0.01)

print("=== Experiment Configuration ===")
print(f"Name: {experiment_config.getName()}")
print(f"Description: {experiment_config.getDescription()}")
print(f"Steps: {experiment_config.getNumSteps()}")
print(f"Timestep: {experiment_config.getTimestep()}s")

# Create and run experiment
experiment = pynlm.createExperiment("development_study", config)
print(f"\nCreated experiment: {experiment.getName()}")
print(f"Experiment seed: {experiment.getSeed()}")

# Run experiment
print("\nRunning experiment...")
experiment.run(100)

# Retrieve experiment metrics
metrics = experiment.getMetrics()
if metrics.hasMetric("learning_progress"):
    print(f"\nLearning progress: {metrics.getMetric('learning_progress')}")

if metrics.hasMetric("performance"):
    print(f"Performance: {metrics.getMetric('performance')}")
```

### 2. Advanced Experiment Management

```python
import pynlm
from datetime import datetime
import json

# Experiment manager class
class NLMExperimentManager:
    def __init__(self, name, config):
        self.name = name
        self.config = config
        self.experiments = []
        self.results = []
    
    def create_experiment(self, name, description=None):
        """Create a new experiment"""
        if description is None:
            description = f"Experiment {name} for {self.name}"
        
        experiment = pynlm.createExperiment(name, self.config)
        experiment.setDescription(description)
        
        # Add experiment-specific parameters
        experiment.setParameter("learning_rate", 0.001)
        experiment.setParameter("plasticity", 0.8)
        experiment.setParameter("development_rate", 0.1)
        
        self.experiments.append(experiment)
        print(f"Created experiment: {name}")
        return experiment
    
    def run_experiment(self, experiment_name, steps, metrics_config=None):
        """Run an experiment with specified metrics"""
        for experiment in self.experiments:
            if experiment.getName() == experiment_name:
                print(f"\nRunning experiment: {experiment_name}")
                print(f"Steps: {steps}, Timestep: {experiment_config.getTimestep()}s")
                
                # Configure metrics if provided
                if metrics_config:
                    experiment.setParameter("record_metrics", True)
                    experiment.setParameter("metrics_config", metrics_config)
                
                # Run experiment
                experiment.run(steps)
                
                # Store results
                self.results.append({
                    'experiment_name': experiment_name,
                    'steps': steps,
                    'timestamp': datetime.now().isoformat(),
                    'metrics': experiment.getMetrics().summary()
                })
                
                print(f"Experiment {experiment_name} completed")
                return experiment
        
        print(f"Experiment {experiment_name} not found")
        return None
    
    def compare_experiments(self, experiment_names, metric_name):
        """Compare multiple experiments on a specific metric"""
        print(f"\n=== Comparison: {metric_name} ===")
        
        comparison_data = []
        for result in self.results:
            if result['experiment_name'] in experiment_names:
                comparison_data.append({
                    'name': result['experiment_name'],
                    'value': result['metrics'].get(metric_name, 0.0)
                })
        
        if not comparison_data:
            print(f"No data found for metric {metric_name}")
            return
        
        # Sort by metric value
        comparison_data.sort(key=lambda x: x['value'])
        
        print("Rank | Experiment      | Value")
        print("-----|-----------------|------")
        for i, data in enumerate(comparison_data, 1):
            print(f"{i:4d}  | {data['name']:16s} | {data['value']:8.4f}")
        
        # Generate comparison report
        best = comparison_data[-1]
        worst = comparison_data[0]
        print(f"\nBest performer: {best['name']} ({best['value']:.4f})")
        print(f"Worst performer: {worst['name']} ({worst['value']:.4f})")
    
    def export_results(self, filepath):
        """Export experiment results to JSON file"""
        results_data = {
            'manager_name': self.name,
            'config': self.config.summary(),
            'experiments': [exp.getName() for exp in self.experiments],
            'results': self.results,
            'timestamp': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2)
        
        print(f"\nResults exported to: {filepath}")

# Example usage of experiment manager
print("=== Advanced Experiment Management Demo ===")

# Create experiment manager
config = pynlm.createAdvancedConfig()
manager = NLMExperimentManager("neural_development", config)

# Create multiple experiments
exp1 = manager.create_experiment("simple_learning", "Basic learning with simple rewards")
exp2 = manager.create_experiment("curiosity_learning", "Learning with curiosity-driven exploration")
exp3 = manager.create_experiment("development_learning", "Long-term developmental learning")

# Run experiments
manager.run_experiment("simple_learning", 200)
manager.run_experiment("curiosity_learning", 300)
manager.run_experiment("development_learning", 500)

# Compare experiments
manager.compare_experiments(["simple_learning", "curiosity_learning", "development_learning"], "learning_progress")

# Export results
manager.export_results("experiment_results.json")
```

## Configuration Management

### 1. Environment-Based Configuration

```python
import pynlm
import os

# Create environment configuration
env_config = pynlm.createEnvironmentConfig()

# Set environment variables
os.environ["NLM_SIMULATION_STEP"] = "0.001"
os.environ["NLM_RANDOM_SEED"] = "42"
os.environ["NLM_BRAIN_SIZE"] = "2000"
os.environ["NLM_VISUAL_SIZE"] = "32x32"
os.environ["NLM_LEARNING_RATE"] = "0.01"

# Load from environment
env_config.loadFromEnvironment()

print("=== Environment Configuration ===")
print(f"Simulation step: {env_config.getVariable('NLM_SIMULATION_STEP')}")
print(f"Random seed: {env_config.getVariable('NLM_RANDOM_SEED')}")
print(f"Brain size: {env_config.getVariable('NLM_BRAIN_SIZE')}")
print(f"Visual size: {env_config.getVariable('NLM_VISUAL_SIZE')}")
print(f"Learning rate: {env_config.getVariable('NLM_LEARNING_RATE')}")

# Save environment configuration
env_config.saveToFile("environment_config.txt")
print("\nEnvironment configuration saved to environment_config.txt")

# Validate configuration
if env_config.validate():
    print("Environment configuration is valid")
else:
    print("Validation error:", env_config.getValidationError())
```

### 2. Configuration Validation

```python
import pynlm
import json

# Create comprehensive configuration
config = pynlm.createAdvancedConfig()

# Set all configuration values
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 20)
config.set("brain.connection_probability", 0.18)
config.set("simulation.timestep", 0.001)
config.set("simulation.random_seed", 42)
config.set("learning.stdp_ltp_weight", 0.025)
config.set("learning.stdp_ltd_weight", 0.0225)
config.set("learning.hebbian_strength", 0.15)
config.set("development.initial_plasticity", 1.2)
config.set("development.critical_period_duration", 150)
config.set("development.maturation_rate", 0.8)
config.set("world.width", 50.0)
config.set("world.height", 50.0)
config.set("world.max_energy", 200.0)
config.set("world.energy_decay_rate", 0.005)

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration validation errors:")
    # In real implementation, would get validation errors from config object
    print("- All configuration values must be of correct type")
    print("- Brain parameters must be within specified ranges")
    print("- World parameters must be physically plausible")

# Get configuration summary
print("\n=== Configuration Summary ===")
print(config.summary())

# Save configuration to JSON
config_data = {
    "brain": {
        "neuron_count": config.getOr("brain.neuron_count", 1000),
        "region_count": config.getOr("brain.region_count", 10),
        "connection_probability": config.getOr("brain.connection_probability", 0.15)
    },
    "simulation": {
        "timestep": config.getOr("simulation.timestep", 0.001),
        "random_seed": config.getOr("simulation.random_seed", 42)
    },
    "learning": {
        "stdp_ltp_weight": config.getOr("learning.stdp_ltp_weight", 0.02),
        "stdp_ltd_weight": config.getOr("learning.stdp_ltd_weight", 0.018)
    }
}

with open("config.json", "w") as f:
    json.dump(config_data, f, indent=2)

print("\nConfiguration saved to config.json")
```

## Performance Tuning

### 1. Performance Monitoring Setup

```python
import pynlm
import time
import threading
from collections import deque

class PerformanceMonitor:
    def __init__(self, window_size=100):
        self.window_size = window_size
        self.timings = deque(maxlen=window_size)
        self.memory_usage = deque(maxlen=window_size)
        self.cpu_usage = deque(maxlen=window_size)
        self.spike_rates = deque(maxlen=window_size)
        self.is_running = False
    
    def start_monitoring(self, brain, world, agent, steps):
        """Start performance monitoring during simulation"""
        self.is_running = True
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Run simulation step
            world.update(0.1)
            
            # Update performance metrics
            current_time = time.time()
            self.timings.append(current_time - step_start)
            self.memory_usage.append(brain.getMemoryUsage())
            self.cpu_usage.append(brain.getPerformanceMetrics().cpuUsage)
            self.spike_rates.append(brain.getAverageFiringRate())
            
            # Continue simulation (simplified)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        self.is_running = False
        
        # Calculate statistics
        self.calculate_statistics(start_time, steps)
    
    def calculate_statistics(self, start_time, steps):
        """Calculate performance statistics"""
        if not self.timings:
            return
        
        total_time = time.time() - start_time
        avg_frame_time = sum(self.timings) / len(self.timings)
        max_frame_time = max(self.timings)
        min_frame_time = min(self.timings)
        
        print("=== Performance Statistics ===")
        print(f"Total simulation time: {total_time:.2f} seconds")
        print(f"Average frame time: {avg_frame_time*1000:.2f} ms")
        print(f"Max frame time: {max_frame_time*1000:.2f} ms")
        print(f"Min frame time: {min_frame_time*1000:.2f} ms")
        print(f"FPS: {1.0 / avg_frame_time:.1f}")
        
        if self.memory_usage:
            print(f"Average memory usage: {sum(self.memory_usage)/len(self.memory_usage):.1f} MB")
            print(f"Peak memory usage: {max(self.memory_usage):.1f} MB")
        
        if self.cpu_usage:
            print(f"Average CPU usage: {sum(self.cpu_usage)/len(self.cpu_usage):.1f}%")
            print(f"Peak CPU usage: {max(self.cpu_usage):.1f}%")
        
        if self.spike_rates:
            print(f"Average firing rate: {sum(self.spike_rates)/len(self.spike_rates):.1f} Hz")
            print(f"Peak firing rate: {max(self.spike_rates):.1f} Hz")
    
    def get_performance_report(self):
        """Get detailed performance report"""
        report = {
            'frame_times': list(self.timings),
            'memory_usage': list(self.memory_usage),
            'cpu_usage': list(self.cpu_usage),
            'spike_rates': list(self.spike_rates),
            'avg_frame_time': sum(self.timings) / len(self.timings) if self.timings else 0,
            'avg_memory_usage': sum(self.memory_usage) / len(self.memory_usage) if self.memory_usage else 0,
            'avg_cpu_usage': sum(self.cpu_usage) / len(self.cpu_usage) if self.cpu_usage else 0,
            'avg_spike_rate': sum(self.spike_rates) / len(self.spike_rates) if self.spike_rates else 0
        }
        return report

# Example usage
print("=== Performance Monitoring Demo ===")

# Create simulation components
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world
world.configure(width=60, height=60, visionWidth=24, visionHeight=24)
world.reset()
agent.initialize(world)

# Create performance monitor
monitor = PerformanceMonitor(window_size=50)

# Run simulation with monitoring
print("Running simulation with performance monitoring...")
monitor.start_monitoring(brain, world, agent, 200)

# Get performance report
report = monitor.get_performance_report()
print(f"\nPerformance report generated:")
print(f"Average frame time: {report['avg_frame_time']*1000:.2f} ms")
print(f"Average memory usage: {report['avg_memory_usage']:.1f} MB")
print(f"Average CPU usage: {report['avg_cpu_usage']:.1f}%")
print(f"Average firing rate: {report['avg_spike_rate']:.1f} Hz")
```

### 2. Memory Optimization

```python
import pynlm
import gc
import psutil
import os

def optimize_memory_usage(brain, target_memory_mb=512):
    """Optimize memory usage for simulation"""
    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    print(f"Initial memory usage: {initial_memory:.1f} MB")
    
    # Force garbage collection
    gc.collect()
    
    # Get initial memory after GC
    gc_memory = process.memory_info().rss / 1024 / 1024
    print(f"Memory after GC: {gc_memory:.1f} MB")
    
    # Clear unused objects
    import weakref
    weakref.finalize(brain, lambda: gc.collect())
    
    # Compact memory
    gc.collect()
    
    final_memory = process.memory_info().rss / 1024 / 1024
    print(f"Final memory usage: {final_memory:.1f} MB")
    
    memory_saved = gc_memory - final_memory
    print(f"Memory saved by optimization: {memory_saved:.1f} MB")
    
    return memory_saved

def monitor_memory_usage(brain, interval=10):
    """Monitor memory usage during simulation"""
    import time
    process = psutil.Process(os.getpid())
    
    memory_samples = []
    start_time = time.time()
    
    for i in range(60):  # Monitor for 60 seconds
        memory_mb = process.memory_info().rss / 1024 / 1024
        memory_samples.append(memory_mb)
        
        if i % interval == 0:
            print(f"Time {time.time() - start_time:.1f}s: Memory {memory_mb:.1f} MB")
        
        time.sleep(interval)
    
    # Calculate statistics
    max_memory = max(memory_samples)
    min_memory = min(memory_samples)
    avg_memory = sum(memory_samples) / len(memory_samples)
    memory_variance = sum((x - avg_memory) ** 2 for x in memory_samples) / len(memory_samples)
    memory_std_dev = memory_variance ** 0.5
    
    print(f"\nMemory Usage Statistics:")
    print(f"Average memory: {avg_memory:.1f} MB")
    print(f"Peak memory: {max_memory:.1f} MB")
    print(f"Minimum memory: {min_memory:.1f} MB")
    print(f"Memory standard deviation: {memory_std_dev:.1f} MB")
    print(f"Memory efficiency: {(max_memory - min_memory) / max_memory * 100:.1f}%")
    
    return {
        'avg_memory': avg_memory,
        'max_memory': max_memory,
        'min_memory': min_memory,
        'std_dev': memory_std_dev,
        'efficiency': (max_memory - min_memory) / max_memory * 100
    }

# Example usage
print("=== Memory Optimization Demo ===")

# Create simulation components
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)

# Monitor memory usage before simulation
print("Monitoring memory usage during simulation...")
memory_stats = monitor_memory_usage(brain, interval=5)

# Show optimization results
optimized_memory = optimize_memory_usage(brain)

print(f"\nMemory optimization successful: {optimized_memory:.1f} MB saved")
print("Target memory (512 MB) reached:", memory_stats['max_memory'] <= 512)
```

## Error Handling and Logging

### 1. Comprehensive Error Handling

```python
import pynlm
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_simulation.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger('nlm_simulation')

def safe_simulation(brain, world, agent, steps, max_retries=3):
    """Run simulation with comprehensive error handling"""
    for attempt in range(max_retries):
        try:
            logger.info(f"Starting simulation attempt {attempt + 1}/{max_retries}")
            
            # Reset components
            brain.reset()
            world.reset()
            agent.reset()
            
            # Validate components
            if not brain.validateConfiguration():
                error_msg = f"Brain configuration validation failed: {brain.getConfigurationValidationError()}"
                logger.error(error_msg)
                raise ValueError(error_msg)
            
            if not agent.validateConfiguration():
                error_msg = f"Agent configuration validation failed: {agent.getConfigurationValidationError()}"
                logger.error(error_msg)
                raise ValueError(error_msg)
            
            # Run simulation
            for step in range(steps):
                try:
                    world.update(0.1)
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Log progress periodically
                    if step % 50 == 0:
                        logger.info(f"Step {step} completed. "
                                 f"Firing rate: {brain.getAverageFiringRate():.2f} Hz, "
                                 f"Memory usage: {agent.getMemoryUsage():.1f} MB")
                
                except Exception as e:
                    logger.error(f"Error at step {step}: {str(e)}")
                    # Attempt recovery
                    brain.reset()
                    world.reset()
                    agent.reset()
                    continue
            
            logger.info("Simulation completed successfully")
            return True
            
        except Exception as e:
            logger.error(f"Simulation attempt {attempt + 1} failed: {str(e)}")
            
            if attempt < max_retries - 1:
                logger.info("Retrying...")
                continue
            else:
                logger.error("Max retries exceeded. Simulation failed.")
                return False
    
    return False

# Example usage with error handling
print("=== Error Handling Demo ===")

# Create simulation components
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world
world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Run simulation with error handling
success = safe_simulation(brain, world, agent, 200)

if success:
    print("Simulation completed successfully with error handling")
else:
    print("Simulation failed after all retry attempts")
```

### 2. Advanced Logging

```python
import pynlm
import logging
from datetime import datetime
import json

class NLMLogger:
    def __init__(self, name="nlm_simulation"):
        self.logger = logging.getLogger(name)
        self.logger.setLevel(logging.INFO)
        
        # File handler for detailed logs
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        file_handler = logging.FileHandler(f"logs/nlm_simulation_{timestamp}.log")
        file_handler.setLevel(logging.DEBUG)
        
        # Console handler for real-time output
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # Format
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        file_handler.setFormatter(formatter)
        console_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)
        
        # Performance metrics logger
        self.performance_logger = logging.getLogger(f"{name}_performance")
        self.performance_logger.setLevel(logging.DEBUG)
        
    def log_simulation_step(self, step, brain, world, agent, action_result=None):
        """Log detailed information about simulation step"""
        log_data = {
            'timestamp': datetime.now().isoformat(),
            'step': step,
            'brain_state': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'spike_count': brain.getTotalSpikeCount(),
                'development_stage': brain.getDevelopmentalStage()
            },
            'world_state': {
                'position': (world.getAgentBody().x, world.getAgentBody().y),
                'orientation': world.getAgentBody().orientation,
                'energy': world.getAgentBody().energy,
                'health': world.getAgentBody().health
            },
            'agent_state': {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError()
            }
        }
        
        if action_result:
            log_data['action_result'] = {
                'reward': action_result.reward,
                'success': action_result.success,
                'message': action_result.message
            }
        
        self.logger.debug(json.dumps(log_data, indent=2))
    
    def log_performance_metrics(self, metrics, step):
        """Log performance metrics"""
        performance_data = {
            'timestamp': datetime.now().isoformat(),
            'step': step,
            'metrics': metrics
        }
        
        self.performance_logger.info(json.dumps(performance_data, indent=2))
    
    def log_experiment_start(self, experiment_name, config):
        """Log experiment start"""
        self.logger.info(f"Starting experiment: {experiment_name}")
        self.logger.debug(f"Experiment configuration: {config.summary()}")
    
    def log_experiment_end(self, experiment_name, results):
        """Log experiment end with results"""
        self.logger.info(f"Completed experiment: {experiment_name}")
        self.logger.info(f"Experiment results: {results}")
        
        # Log summary to file
        with open(f"logs/experiment_{experiment_name}_summary.json", "w") as f:
            json.dump({
                'experiment_name': experiment_name,
                'end_time': datetime.now().isoformat(),
                'results': results
            }, f, indent=2)

# Example usage of advanced logging
print("=== Advanced Logging Demo ===")

# Create logger
logger = NLMLogger("advanced_simulation")

# Create simulation components
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize world
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Log experiment start
logger.log_experiment_start("memory_efficiency_test", config)

# Run simulation with detailed logging
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Log step details
    action_result = world.applyMotorCommand(action, world.getSimulationTime())
    logger.log_simulation_step(step, brain, world, agent, action_result)
    
    # Log performance metrics periodically
    if step % 10 == 0:
        metrics = agent.getPerformanceMetrics()
        logger.log_performance_metrics({
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'memory_usage': agent.getMemoryUsage(),
            'learning_progress': metrics.getLearningProgress()
        }, step)

# Log experiment end
results = {
    'total_steps': 100,
    'final_spike_count': brain.getTotalSpikeCount(),
    'avg_firing_rate': brain.getAverageFiringRate(),
    'final_memory_usage': agent.getMemoryUsage(),
    'learning_progress': brain.getPerformanceMetrics().getLearningProgress()
}

logger.log_experiment_end("memory_efficiency_test", results)
```

## Conclusion

This comprehensive guide demonstrates the advanced features and capabilities of the enhanced NLM Python bindings. Key improvements include:

1. **Advanced Configuration System**: Flexible configuration with validation and environment-based settings
2. **Performance Monitoring**: Real-time metrics collection and analysis
3. **Batch Processing**: Efficient processing of multiple simulations
4. **Checkpoint System**: State persistence and recovery
5. **Experiment Management**: Structured experiment execution and comparison
6. **Memory Optimization**: Techniques for efficient memory usage
7. **Comprehensive Error Handling**: Robust error recovery and logging
8. **Advanced Logging**: Detailed simulation tracking and analysis

These enhancements make NLM more powerful, flexible, and easier to use for both beginners and advanced users, while maintaining backward compatibility with existing code.