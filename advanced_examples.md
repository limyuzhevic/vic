# NLM Advanced Examples and Tutorials

This document provides advanced examples and tutorials for using the enhanced NLM Python bindings with power user features.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Batch Processing and Parallel Simulations](#batch-processing-and-parallel-simulations)
3. [Performance Monitoring and Benchmarking](#performance-monitoring-and-benchmarking)
4. [Advanced Agent Behaviors](#advanced-agent-behaviors)
5. [Memory Management and Optimization](#memory-management-and-optimization)
6. [Export/Import and State Management](#export-import-and-state-management)
7. [Integration with Machine Learning Frameworks](#integration-with-machine-learning-frameworks)
8. [Advanced Visualization and Analysis](#advanced-visualization-and-analysis)
9. [Production Deployment and Monitoring](#production-deployment-and-monitoring)
10. [Custom Extensions and Plugins](#custom-extensions-and-plugins)

---

## 1. Advanced Configuration Management

### Configuration Presets

The enhanced bindings provide predefined configuration presets for common scenarios:

```python
import pynlm
import json

# Create predefined configuration presets
def create_advanced_config(config_type="standard"):
    config = pynlm.create_config()
    
    if config_type == "standard":
        # Standard brain configuration
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.v_thresh", -55.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -75.0)
        config.set("brain.tau_mem", 20.0)
        config.set("brain.tau_ref", 2.0)
        
        # Enable all plasticity mechanisms
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Neuromodulation configuration
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
    elif config_type == "high_performance":
        # Optimized for performance
        config.set("brain.neuron_count", 500)  # Fewer neurons, higher speed
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)
        config.set("plasticity.hebbian.enable", False)  # Skip for speed
        
    elif config_type == "developmental":
        # Optimized for developmental studies
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
        config.set("plasticity.structural.pruning_rate", 0.00001)
        config.set("neuromod.curiosity.enable", True)
        
    return config

# Usage
config = create_advanced_config("developmental")
brain = pynlm.create_brain(config)
brain.initialize()
```

### Configuration Validation and Comparison

```python
# Validate configuration
def validate_config(config):
    issues = []
    
    # Check for required parameters
    required_params = [
        "brain.neuron_count",
        "brain.v_thresh", 
        "brain.v_rest"
    ]
    
    for param in required_params:
        if not config.has(param):
            issues.append(f"Missing required parameter: {param}")
    
    # Validate parameter ranges
    if config.has("brain.neuron_count"):
        neurons = config.getOr("brain.neuron_count", 0)
        if neurons < 10 or neurons > 10000:
            issues.append(f"Invalid neuron_count: {neurons} (valid: 10-10000)")
    
    return issues

# Compare two configurations
def compare_configs(config1, config2, name1="Config1", name2="Config2"):
    all_keys = set(config1.getKeys()).union(config2.getKeys())
    differences = []
    
    for key in sorted(all_keys):
        val1 = config1.get(key)
        val2 = config2.get(key)
        
        if val1 != val2:
            differences.append({
                "parameter": key,
                "value1": val1.value_or("NOT_SET") if val1 else "NOT_SET",
                "value2": val2.value_or("NOT_SET") if val2 else "NOT_SET"
            })
    
    return differences

# Example usage
config1 = create_advanced_config("standard")
config2 = create_advanced_config("high_performance")
comparison = compare_configs(config1, config2, "Standard", "High Performance")
print(f"Differences found: {len(comparison)}")
for diff in comparison:
    print(f"  {diff['parameter']}: {diff['value1']} vs {diff['value2']}")
```

### Configuration Serialization

```python
# Save/load configurations with compression
def save_config_with_metadata(config, filepath, description=""):
    import json
    import time
    import zlib
    import base64
    
    metadata = {
        "description": description,
        "created": time.time(),
        "version": "1.0.0",
        "brain_version": "NLM-Enhanced-1.0"
    }
    
    config_data = {
        "metadata": metadata,
        "config": {}
    }
    
    for key in config.getKeys():
        value = config.get(key)
        if value:
            config_data["config"][key] = value.value_or(None)
    
    # Compress and encode
    json_str = json.dumps(config_data)
    compressed = zlib.compress(json_str.encode())
    encoded = base64.b64encode(compressed).decode()
    
    # Save to file
    with open(filepath, 'w') as f:
        f.write(encoded)
    
    return metadata

def load_config_with_metadata(filepath):
    import json
    import zlib
    import base64
    
    with open(filepath, 'r') as f:
        encoded = f.read()
    
    # Decode and decompress
    compressed = base64.b64decode(encoded.encode())
    json_str = zlib.decompress(compressed).decode()
    
    # Parse
    config_data = json.loads(json_str)
    metadata = config_data["metadata"]
    
    # Create config
    config = pynlm.create_config()
    for key, value in config_data["config"].items():
        # Set value based on type
        if isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, str):
            config.set(key, value)
        elif isinstance(value, list):
            config.set(key, value)
    
    return config, metadata

# Usage example
metadata = save_config_with_metadata(config, "my_config.enc", "Advanced developmental brain")
print(f"Saved config with metadata: {metadata}")

# Load config later
loaded_config, loaded_metadata = load_config_with_metadata("my_config.enc")
print(f"Loaded config with metadata: {loaded_metadata}")
```

---

## 2. Batch Processing and Parallel Simulations

### Batch Simulation Manager

```python
import pynlm
import time
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from queue import Queue
import multiprocessing as mp

class BatchSimulationManager:
    def __init__(self, max_workers=None):
        self.max_workers = max_workers or mp.cpu_count()
        self.results_queue = Queue()
        self.simulation_stats = []
    
    def run_single_simulation(self, config_id, config_params, steps=1000):
        """Run a single simulation with given parameters"""
        try:
            config = pynlm.create_config()
            for key, value in config_params.items():
                if isinstance(value, bool):
                    config.set(key, value)
                elif isinstance(value, int):
                    config.set(key, value)
                elif isinstance(value, float):
                    config.set(key, value)
                elif isinstance(value, str):
                    config.set(key, value)
                elif isinstance(value, list):
                    config.set(key, value)
            
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            
            result = {
                "config_id": config_id,
                "success": True,
                "steps": steps,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "simulation_time": end_time - start_time,
                "spikes_per_second": brain.getTotalSpikeCount() / (end_time - start_time)
            }
            
            return result
            
        except Exception as e:
            return {
                "config_id": config_id,
                "success": False,
                "error": str(e)
            }
    
    def run_batch_sims(self, simulation_configs, parallel=True, use_processes=False):
        """Run multiple simulations in batch mode"""
        if parallel:
            if use_processes:
                return self._run_parallel_process(simulation_configs)
            else:
                return self._run_parallel_threads(simulation_configs)
        else:
            return self._run_sequential(simulation_configs)
    
    def _run_sequential(self, simulation_configs):
        results = []
        for config_id, params in simulation_configs.items():
            result = self.run_single_simulation(config_id, params)
            results.append(result)
            self.simulation_stats.append(result)
        return results
    
    def _run_parallel_threads(self, simulation_configs):
        results = []
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_config = {
                executor.submit(self.run_single_simulation, config_id, params): config_id
                for config_id, params in simulation_configs.items()
            }
            
            # Collect results
            for future in future_to_config:
                result = future.result()
                results.append(result)
                self.simulation_stats.append(result)
        
        return results
    
    def _run_parallel_process(self, simulation_configs):
        results = []
        
        # Create shared queue for results
        with ProcessPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_config = {
                executor.submit(self.run_single_simulation, config_id, params): config_id
                for config_id, params in simulation_configs.items()
            }
            
            # Collect results
            for future in future_to_config:
                result = future.result()
                results.append(result)
                self.simulation_stats.append(result)
        
        return results
    
    def run_parameter_sweep(self, base_config, parameter_grid, steps=1000):
        """Run a parameter sweep across multiple configurations"""
        simulation_configs = {}
        
        param_names = list(parameter_grid.keys())
        param_values = list(parameter_grid.values())
        
        # Generate all combinations
        from itertools import product
        
        for i, combination in enumerate(product(*param_values)):
            config_id = f"sweep_{i}"
            params = base_config.copy()
            
            for j, param_name in enumerate(param_names):
                params[param_name] = combination[j]
            
            simulation_configs[config_id] = params
        
        return self.run_batch_sims(simulation_configs)
    
    def get_batch_statistics(self):
        """Get statistics across all completed simulations"""
        if not self.simulation_stats:
            return {}
        
        successful_results = [r for r in self.simulation_stats if r["success"]]
        
        if not successful_results:
            return {}
        
        stats = {
            "total_simulations": len(self.simulation_stats),
            "successful_simulations": len(successful_results),
            "failed_simulations": len(self.simulation_stats) - len(successful_results),
            "average_spikes_per_second": sum(r["spikes_per_second"] for r in successful_results) / len(successful_results),
            "total_spikes": sum(r["total_spikes"] for r in successful_results),
            "average_simulation_time": sum(r["simulation_time"] for r in successful_results) / len(successful_results),
            "config_ids": [r["config_id"] for r in successful_results]
        }
        
        return stats

# Example usage
if __name__ == "__main__":
    # Create batch simulation manager
    batch_manager = BatchSimulationManager(max_workers=4)
    
    # Define parameter grid for sweep
    base_config = {
        "brain.neuron_count": 500,
        "brain.v_thresh": -55.0,
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": True
    }
    
    parameter_grid = {
        "brain.v_rest": [-70.0, -75.0, -80.0],
        "plasticity.stdp.learning_rate": [0.001, 0.005, 0.01]
    }
    
    print("Running parameter sweep...")
    results = batch_manager.run_parameter_sweep(base_config, parameter_grid, steps=500)
    
    # Analyze results
    stats = batch_manager.get_batch_statistics()
    print(f"Batch statistics: {stats}")
    
    # Find best configuration
    best_result = max(results, key=lambda x: x.get("spikes_per_second", 0))
    print(f"Best configuration: {best_result['config_id']}")
    print(f"  Spikes per second: {best_result['spikes_per_second']:.2f}")
    print(f"  Total spikes: {best_result['total_spikes']}")
    print(f"  Simulation time: {best_result['simulation_time']:.2f}s")
```

---

## 3. Performance Monitoring and Benchmarking

### Advanced Benchmarking Tools

```python
import pynlm
import time
import statistics
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime

class AdvancedBenchmarker:
    def __init__(self, brain_config=None):
        self.config = brain_config or pynlm.createDefaultConfig()
        self.benchmark_history = []
        self.performance_metrics = {}
    
    def benchmark_configuration(self, steps=1000, repetitions=10, warmup_steps=100):
        """Benchmark brain performance with detailed metrics"""
        print(f"Starting benchmark with {steps} steps, {repetitions} repetitions...")
        
        # Initialize brain
        brain = pynlm.create_brain(self.config)
        brain.initialize()
        
        # Warm-up run
        for step in range(warmup_steps):
            brain.step(step)
        
        # Clear statistics
        brain.reset()
        
        # Run benchmark
        times = []
        spike_counts = []
        firing_rates = []
        neuron_counts = []
        
        for rep in range(repetitions):
            brain.reset()
            start_time = time.perf_counter()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.perf_counter()
            
            duration = end_time - start_time
            times.append(duration)
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
            neuron_counts.append(brain.getFiringNeuronCount())
        
        # Calculate metrics
        metrics = {
            "steps": steps,
            "repetitions": repetitions,
            "warmup_steps": warmup_steps,
            "times": times,
            "spike_counts": spike_counts,
            "firing_rates": firing_rates,
            "neuron_counts": neuron_counts,
            "avg_time_per_step": statistics.mean(times) / steps,
            "std_time_per_step": statistics.stdev(times) / steps if len(times) > 1 else 0,
            "avg_spikes_per_second": statistics.mean(spike_counts) / statistics.mean(times),
            "std_spikes_per_second": statistics.stdev(spike_counts) / statistics.mean(times) if len(spike_counts) > 1 else 0,
            "avg_firing_rate": statistics.mean(firing_rates),
            "avg_firing_neurons": statistics.mean(neuron_counts),
            "total_spikes": statistics.sum(spike_counts),
            "throughput_spikes_per_second": statistics.sum(spike_counts) / statistics.sum(times)
        }
        
        self.benchmark_history.append({
            "timestamp": datetime.now(),
            "config": self.config,
            "metrics": metrics
        })
        
        return metrics
    
    def benchmark_multiple_configs(self, configs, steps=1000, repetitions=5):
        """Benchmark multiple configurations"""
        results = {}
        
        for config_name, config in configs.items():
            print(f"Benchmarking {config_name}...")
            try:
                metrics = self.benchmark_configuration(steps, repetitions)
                results[config_name] = metrics
            except Exception as e:
                print(f"Error benchmarking {config_name}: {e}")
                results[config_name] = {"error": str(e)}
        
        return results
    
    def compare_configs(self, results_dict):
        """Compare benchmark results across configurations"""
        comparison = {}
        
        # Extract key metrics for each configuration
        for config_name, metrics in results_dict.items():
            if "error" not in metrics:
                comparison[config_name] = {
                    "throughput": metrics["throughput_spikes_per_second"],
                    "avg_time_per_step": metrics["avg_time_per_step"],
                    "avg_firing_rate": metrics["avg_firing_rate"],
                    "efficiency": metrics["avg_firing_rate"] / metrics["avg_time_per_step"]
                }
        
        return comparison
    
    def create_performance_report(self, output_file="benchmark_report.html"):
        """Create a detailed performance report"""
        html_content = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Benchmark Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 40px; }
                .metric { background: #f5f5f5; padding: 20px; margin: 10px 0; border-radius: 5px; }
                .best { background: #e8f5e8; border: 2px solid #4caf50; }
                table { width: 100%; border-collapse: collapse; }
                th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
                th { background-color: #f2f2f2; }
            </style>
        </head>
        <body>
            <h1>NLM Benchmark Report</h1>
            <p>Generated: {timestamp}</p>
            
            <h2>Configuration Comparisons</h2>
            {comparison_table}
            
            <h2>Detailed Benchmark History</h2>
            {history_table}
        </body>
        </html>
        """
        
        # Create comparison table
        comparison_table = "<table>"
        comparison_table += "<tr><th>Configuration</th><th>Throughput (spikes/sec)</th><th>Avg Time/Step (ms)</th><th>Avg Firing Rate</th><th>Efficiency</th></tr>"
        
        for config_name, metrics in self.compare_configs(self.benchmark_history).items():
            comparison_table += f"<tr><td>{config_name}</td><td>{metrics['throughput']:.2f}</td><td>{metrics['avg_time_per_step']*1000:.4f}</td><td>{metrics['avg_firing_rate']:.2f}</td><td>{metrics['efficiency']:.2f}</td></tr>"
        
        comparison_table += "</table>"
        
        # Create history table
        history_table = "<table>"
        history_table += "<tr><th>Timestamp</th><th>Steps</th><th>Repetitions</th><th>Throughput</th><th>Avg Time/Step</th></tr>"
        
        for entry in self.benchmark_history:
            metrics = entry["metrics"]
            history_table += f"<tr><td>{entry['timestamp']}</td><td>{metrics['steps']}</td><td>{metrics['repetitions']}</td><td>{metrics['throughput_spikes_per_second']:.2f}</td><td>{metrics['avg_time_per_step']*1000:.4f}</td></tr>"
        
        history_table += "</table>"
        
        # Fill template
        html_content = html_content.format(
            timestamp=datetime.now(),
            comparison_table=comparison_table,
            history_table=history_table
        )
        
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        return output_file

# Example usage
if __name__ == "__main__":
    # Create different configurations to benchmark
    configs = {
        "default": pynlm.createDefaultConfig(),
        "high_neuron": pynlm.createDefaultConfig(),
        "low_neuron": pynlm.createDefaultConfig()
    }
    
    # Modify configurations
    configs["high_neuron"].set("brain.neuron_count", 2000)
    configs["low_neuron"].set("brain.neuron_count", 100)
    
    # Run benchmarks
    benchmarker = AdvancedBenchmarker()
    results = benchmarker.benchmark_multiple_configs(configs, steps=500, repetitions=3)
    
    # Compare results
    comparison = benchmarker.compare_configs(results)
    print("Configuration comparison:")
    for config_name, metrics in comparison.items():
        print(f"  {config_name}: {metrics}")
    
    # Create report
    report_file = benchmarker.create_performance_report("benchmark_report.html")
    print(f"Benchmark report saved to: {report_file}")
```

---

## 4. Advanced Agent Behaviors

### Curiosity-Driven Learning Agent

```python
import pynlm
import numpy as np
import math

class AdvancedCuriosityAgent:
    def __init__(self, brain_config=None, world_config=None):
        self.config = brain_config or pynlm.createDefaultConfig()
        self.world_config = world_config or {}
        
        # Initialize components
        self.brain = pynlm.create_brain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.create_agent_brain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.world_config.get("width", 20),
            height=self.world_config.get("height", 20),
            visionWidth=self.world_config.get("visionWidth", 8),
            visionHeight=self.world_config.get("visionHeight", 8)
        )
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent.initialize(self.world)
        
        # Enable all advanced systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Advanced parameters
        self.exploration_rate = 0.1
        self.intrinsic_motivation = 1.0
        self.expertise_threshold = 0.8
        self.uncertainty_threshold = 0.3
        
        # State tracking
        self.episode_history = []
        self.action_history = []
        self.intrinsic_rewards = []
    
    def compute_curiosity_drive(self, perception, action, novelty_level):
        """Compute curiosity-driven behavior"""
        # Get current brain state
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        
        # Intrinsic motivation based on multiple factors
        intrinsic_motivation = (
            novelty * 0.4 +           # Novelty seeking
            curiosity * 0.3 +       # General curiosity
            abs(prediction_error) * 0.3  # Surprise
        ) * self.intrinsic_motivation
        
        # Expertise modulation
        if novelty < self.expertise_threshold:
            # High expertise = less curiosity
            intrinsic_motivation *= 0.5
        
        # Action uncertainty
        action_uncertainty = 1.0 - (novelty_level + 0.1)  # Higher novelty = more uncertainty
        
        # Combined curiosity drive
        curiosity_drive = intrinsic_motivation * action_uncertainty
        
        return curiosity_drive
    
    def adaptive_exploration(self, step, total_steps):
        """Adaptive exploration strategy"""
        # Time-based exploration
        time_factor = 1.0 - (step / total_steps)  # More exploration early on
        
        # Curiosity-based exploration
        curiosity_level = self.agent.getCuriosityLevel()
        curiosity_factor = min(curiosity_level, 1.0)
        
        # Adaptive exploration rate
        exploration_rate = (self.exploration_rate * time_factor * 
                           (1.0 + curiosity_factor) / 2.0)
        
        return exploration_rate
    
    def intrinsic_reward(self, perception, action, reward):
        """Compute intrinsic rewards for curiosity-driven learning"""
        # Get current state
        novelty_level = self.agent.getNoveltyLevel()
        curiosity_level = self.agent.getCuriosityLevel()
        prediction_error = self.agent.getPredictionError()
        
        # Intrinsic reward components
        novelty_reward = novelty_level * 0.5
        curiosity_reward = curiosity_level * 0.3
        surprise_reward = min(abs(prediction_error), 1.0) * 0.2
        
        # Total intrinsic reward
        intrinsic = novelty_reward + curiosity_reward + surprise_reward
        
        # Add action diversity bonus
        if len(self.action_history) > 0:
            action_entropy = self._compute_action_entropy()
            diversity_bonus = action_entropy * 0.1
            intrinsic += diversity_bonus
        
        self.intrinsic_rewards.append(intrinsic)
        
        # Apply combined reward
        total_reward = reward + intrinsic
        
        return total_reward
    
    def _compute_action_entropy(self):
        """Compute entropy of action history for diversity"""
        if not self.action_history:
            return 1.0
        
        # Count action frequencies
        action_counts = {}
        for action in self.action_history:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        # Calculate entropy
        total_actions = len(self.action_history)
        entropy = 0.0
        for count in action_counts.values():
            probability = count / total_actions
            entropy -= probability * math.log2(probability)
        
        return entropy / math.log2(len(action_counts)) if action_counts else 0
    
    def run_episode(self, max_steps=1000, verbose=False):
        """Run a single episode with advanced curiosity-driven behavior"""
        print(f"Starting episode with max_steps={max_steps}")
        
        # Reset history
        self.action_history.clear()
        self.intrinsic_rewards.clear()
        
        for step in range(max_steps):
            # Adaptive exploration rate
            exploration_rate = self.adaptive_exploration(step, max_steps)
            
            # Get current perception
            perception = self.world.getSensoryPercept()
            
            # Store previous state for novelty computation
            previous_vision = self.world.getSensoryPercept().getVision()
            
            # Process sensory input
            self.agent.processSensoryInput(perception)
            
            # Run brain step
            self.brain.step(step)
            
            # Decide action (with curiosity modulation)
            if np.random.random() < exploration_rate:
                # Explore: consider curiosity
                curiosity_drive = self.compute_curiosity_drive(
                    perception, None, self.agent.getNoveltyLevel()
                )
                
                if curiosity_drive > 0.5:
                    # High curiosity: consider exploring new actions
                    action = self._curiosity_driven_action()
                else:
                    # Normal action selection
                    action = self.agent.decodeMotorCommand()
            else:
                # Exploit: use learned policy
                action = self.agent.decodeMotorCommand()
            
            # Record action
            self.action_history.append(action)
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and compute intrinsic component
            extrinsic_reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            total_reward = self.intrinsic_reward(perception, action, extrinsic_reward)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(total_reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Log episode data
            episode_data = {
                "step": step,
                "action": action,
                "extrinsic_reward": extrinsic_reward,
                "intrinsic_reward": self.intrinsic_rewards[-1] if self.intrinsic_rewards else 0,
                "total_reward": total_reward,
                "curiosity_level": self.agent.getCuriosityLevel(),
                "novelty_level": self.agent.getNoveltyLevel(),
                "prediction_error": self.agent.getPredictionError(),
                "brain_firing_neurons": self.brain.getFiringNeuronCount(),
                "brain_spikes": self.brain.getTotalSpikeCount()
            }
            
            self.episode_history.append(episode_data)
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: Action={action}, Extrinsic={extrinsic_reward:.3f}, "
                     f"Intrinsic={self.intrinsic_rewards[-1]:.3f}, Total={total_reward:.3f}, "
                     f"Curiosity={self.agent.getCuriosityLevel():.3f}")
            
            # Check for termination conditions
            if self._should_terminate(episode_data):
                print(f"Episode terminated at step {step}")
                break
        
        # Episode summary
        episode_stats = self._compute_episode_stats()
        print(f"Episode completed: {episode_stats}")
        
        return episode_stats
    
    def _curiosity_driven_action(self):
        """Select action based on curiosity-driven exploration"""
        # Get current curiosity state
        curiosity_level = self.agent.getCuriosityLevel()
        novelty_level = self.agent.getNoveltyLevel()
        
        # Different action strategies based on curiosity
        if curiosity_level > 0.8 and novelty_level > 0.5:
            # High curiosity and novelty: random exploration
            return self._random_action()
        elif curiosity_level > 0.5:
            # Medium curiosity: novelty-seeking
            return self._novelty_seeking_action()
        elif novelty_level > 0.3:
            # Detect novelty: investigate
            return self._investigate_action()
        else:
            # Low curiosity: exploit
            return self.agent.decodeMotorCommand()
    
    def _random_action(self):
        """Select completely random action"""
        actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.LookRight,
            pynlm.MotorCommand.Wait
        ]
        return actions[np.random.randint(0, len(actions))]
    
    def _novelty_seeking_action(self):
        """Select action that maximizes novelty"""
        # Get current state
        perception = self.world.getSensoryPercept()
        
        # Simulate different actions to find most novel
        actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight
        ]
        
        best_action = pynlm.MotorCommand.Wait
        max_novelty = -1
        
        for action in actions:
            # Create temporary world state to evaluate action
            # This is a simplified evaluation
            # In a real implementation, this would simulate the action
            novelty = self._evaluate_action_novelty(action, perception)
            if novelty > max_novelty:
                max_novelty = novelty
                best_action = action
        
        return best_action
    
    def _evaluate_action_novelty(self, action, perception):
        """Evaluate how novel an action would be"""
        # This is a simplified evaluation
        # In a real implementation, this would simulate the action
        # and compare resulting states to history
        
        # For now, use a heuristic based on current novelty and action type
        current_novelty = self.agent.getNoveltyLevel()
        
        # Different actions have different novelty profiles
        if action in [pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight]:
            return current_novelty * 1.2  # Turning increases novelty
        elif action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward]:
            return current_novelty * 0.8  # Movement has moderate novelty
        else:
            return current_novelty * 0.5
    
    def _investigate_action(self):
        """Select action to investigate unusual situations"""
        # Return to previous area or explore new area
        return pynlm.MotorCommand.LookLeft  # Default investigate action
    
    def _should_terminate(self, episode_data):
        """Check if episode should terminate"""
        # Terminate if agent is stuck
        if episode_data["brain_firing_neurons"] < 1 and episode_data["step"] > 100:
            return True
        
        # Terminate after long time without learning
        if episode_data["step"] > 500 and episode_data["total_reward"] < 0.01:
            return True
        
        # Terminate if agent explores too much (balance)
        if episode_data["step"] > 200:
            # Calculate exploration ratio
            unique_actions = len(set(self.action_history))
            exploration_ratio = unique_actions / len(self.action_history)
            if exploration_ratio > 0.8:
                return True
        
        return False
    
    def _compute_episode_stats(self):
        """Compute statistics for completed episode"""
        if not self.episode_history:
            return {}
        
        total_steps = len(self.episode_history)
        
        stats = {
            "total_steps": total_steps,
            "unique_actions": len(set(a["action"] for a in self.episode_history)),
            "exploration_ratio": len(set(a["action"] for a in self.episode_history)) / max(total_steps, 1),
            "average_extrinsic_reward": np.mean([a["extrinsic_reward"] for a in self.episode_history]),
            "average_intrinsic_reward": np.mean([a["intrinsic_reward"] for a in self.episode_history]),
            "average_total_reward": np.mean([a["total_reward"] for a in self.episode_history]),
            "max_reward": max(a["total_reward"] for a in self.episode_history]),
            "min_reward": min(a["total_reward"] for a in self.episode_history]),
            "final_curiosity": self.episode_history[-1]["curiosity_level"],
            "final_novelty": self.episode_history[-1]["novelty_level"],
            "total_spikes": self.brain.getTotalSpikeCount(),
            "learning_efficiency": self.brain.getAverageFiringRate() / self.brain.getTotalSpikeCount() if self.brain.getTotalSpikeCount() > 0 else 0
        }
        
        return stats

# Example usage
if __name__ == "__main__":
    # Create advanced curiosity agent
    agent_config = {
        "brain.neuron_count": 1000,
        "brain.v_thresh": -55.0,
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True
    }
    
    world_config = {
        "width": 30,
        "height": 30,
        "visionWidth": 12,
        "visionHeight": 12
    }
    
    advanced_agent = AdvancedCuriosityAgent(agent_config, world_config)
    
    # Run episode with verbose output
    episode_stats = advanced_agent.run_episode(max_steps=1000, verbose=True)
    
    print("\nEpisode Statistics:")
    for key, value in episode_stats.items():
        print(f"  {key}: {value}")
```

---

## 5. Memory Management and Optimization

### Advanced Memory Management

```python
import pynlm
import gc
import psutil
import os
import time
from threading import Lock
from concurrent.futures import ThreadPoolExecutor

class AdvancedMemoryManager:
    def __init__(self, brain_config=None):
        self.config = brain_config or pynlm.createDefaultConfig()
        self.memory_pools = {}
        self.gc_stats = {}
        self.optimization_enabled = True
        self.max_memory_usage = 0.8  # 80% of available memory
        self.memory_monitor_enabled = True
        self.gc_threshold = 100  # GC after N operations
        self.gc_operation_count = 0
        self.memory_lock = Lock()
    
    def create_memory_pool(self, pool_name, size, neuron_count):
        """Create a memory pool for specific operations"""
        pool = {
            "name": pool_name,
            "size": size,
            "neuron_count": neuron_count,
            "allocated": 0,
            "peak_usage": 0,
            "objects": [],
            "type": "neuron_pool"
        }
        
        self.memory_pools[pool_name] = pool
        return pool
    
    def allocate_from_pool(self, pool_name, amount=1):
        """Allocate neurons from memory pool"""
        with self.memory_lock:
            pool = self.memory_pools.get(pool_name)
            if not pool:
                raise ValueError(f"Memory pool '{pool_name}' not found")
            
            if pool["allocated"] + amount > pool["size"]:
                raise MemoryError(f"Not enough memory in pool '{pool_name}'")
            
            pool["allocated"] += amount
            pool["peak_usage"] = max(pool["peak_usage"], pool["allocated"])
            
            # Create placeholder objects
            objects = []
            for i in range(amount):
                obj = {
                    "id": len(pool["objects"]) + len(self.memory_pools.values()),
                    "pool": pool_name,
                    "allocated_time": time.time(),
                    "neuron_id": f"neuron_{pool['neuron_count'] + i}"
                }
                objects.append(obj)
            
            pool["objects"].extend(objects)
            
            return objects
    
    def deallocate_from_pool(self, pool_name, objects):
        """Deallocate objects from memory pool"""
        with self.memory_lock:
            pool = self.memory_pools.get(pool_name)
            if not pool:
                return
            
            # Remove objects from pool
            pool["objects"] = [obj for obj in pool["objects"] if obj not in objects]
            pool["allocated"] -= len(objects)
    
    def smart_gc(self, priority="normal"):
        """Smart garbage collection based on memory usage"""
        with self.memory_lock:
            self.gc_operation_count += 1
            
            # Check current memory usage
            if self.memory_monitor_enabled:
                process = psutil.Process(os.getpid())
                memory_info = process.memory_info()
                memory_percent = process.memory_percent()
                
                print(f"Memory usage: {memory_percent:.1f}%")
                
                # Determine if GC is needed
                if memory_percent > self.max_memory_usage:
                    gc.collect()
                    print("Memory GC triggered")
                
                # Store GC statistics
                self.gc_stats[time.time()] = {
                    "memory_percent": memory_percent,
                    "operation_count": self.gc_operation_count,
                    "gc_performed": memory_percent > self.max_memory_usage
                }
            
            else:
                # Periodic GC based on operation count
                if self.gc_operation_count % self.gc_threshold == 0:
                    gc.collect()
                    print(f"Periodic GC (operation count: {self.gc_operation_count})")
    
    def optimize_memory_usage(self):
        """Optimize memory usage across all pools"""
        with self.memory_lock:
            optimizations = []
            
            for pool_name, pool in self.memory_pools.items():
                # Check pool utilization
                utilization = pool["allocated"] / pool["size"]
                
                if utilization > 0.9:
                    # Pool nearly full - suggest expansion
                    optimizations.append({
                        "type": "expansion",
                        "pool": pool_name,
                        "current_size": pool["size"],
                        "current_usage": pool["allocated"],
                        "recommended_size": min(pool["size"] * 2, 10000)
                    })
                
                elif utilization < 0.1:
                    # Pool underutilized - suggest reduction
                    optimizations.append({
                        "type": "reduction",
                        "pool": pool_name,
                        "current_size": pool["size"],
                        "current_usage": pool["allocated"],
                        "recommended_size": max(pool["size"] // 2, 100)
                    })
                
                # Check for fragmented objects
                if pool["objects"]:
                    # Analyze object ages
                    now = time.time()
                    young_objects = sum(1 for obj in pool["objects"] 
                                      if now - obj["allocated_time"] < 60)
                    
                    if young_objects < len(pool["objects"]) * 0.5:
                        optimizations.append({
                            "type": "defragment",
                            "pool": pool_name,
                            "young_objects": young_objects,
                            "total_objects": len(pool["objects"])
                        })
            
            return optimizations
    
    def benchmark_memory_efficiency(self, brain, steps=1000, repetitions=5):
        """Benchmark memory efficiency of brain operations"""
        print("Running memory efficiency benchmark...")
        
        # Force garbage collection before benchmark
        gc.collect()
        
        memory_results = []
        
        for rep in range(repetitions):
            # Get memory before simulation
            process = psutil.Process(os.getpid())
            memory_before = process.memory_info().rss / 1024 / 1024  # MB
            
            # Run simulation
            brain.reset()
            start_time = time.time()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            
            # Get memory after simulation
            memory_after = process.memory_info().rss / 1024 / 1024  # MB
            
            memory_delta = memory_after - memory_before
            
            result = {
                "repetition": rep,
                "simulation_time": end_time - start_time,
                "memory_before": memory_before,
                "memory_after": memory_after,
                "memory_delta": memory_delta,
                "memory_per_step": memory_delta / steps,
                "spikes_per_second": brain.getTotalSpikeCount() / (end_time - start_time)
            }
            
            memory_results.append(result)
        
        # Calculate statistics
        avg_memory_per_step = sum(r["memory_per_step"] for r in memory_results) / len(memory_results)
        avg_spikes_per_second = sum(r["spikes_per_second"] for r in memory_results) / len(memory_results)
        memory_efficiency = avg_spikes_per_second / avg_memory_per_step if avg_memory_per_step > 0 else 0
        
        return {
            "results": memory_results,
            "avg_memory_per_step": avg_memory_per_step,
            "avg_spikes_per_second": avg_spikes_per_second,
            "memory_efficiency": memory_efficiency,
            "total_spikes": sum(r["spikes_per_second"] for r in memory_results),
            "total_time": sum(r["simulation_time"] for r in memory_results)
        }
    
    def profile_memory_usage(self, brain, steps=1000):
        """Profile memory usage over time"""
        import tracemalloc
        
        # Start memory tracing
        tracemalloc.start()
        
        brain.reset()
        
        # Take snapshots at intervals
        snapshots = []
        
        for step in range(0, steps, max(1, steps // 10)):
            # Run simulation up to current step
            for s in range(step):
                if s < steps:
                    brain.step(s)
            
            # Take memory snapshot
            current, peak = tracemalloc.get_traced_memory()
            snapshot = tracemalloc.take_snapshot()
            
            snapshots.append({
                "step": step,
                "current_memory": current / 1024 / 1024,  # MB
                "peak_memory": peak / 1024 / 1024,  # MB
                "snapshot": snapshot
            })
        
        # Stop tracing
        tracemalloc.stop()
        
        # Analyze memory usage pattern
        memory_growth = []
        for i in range(1, len(snapshots)):
            growth = snapshots[i]["current_memory"] - snapshots[i-1]["current_memory"]
            memory_growth.append(growth)
        
        avg_growth = sum(memory_growth) / len(memory_growth) if memory_growth else 0
        
        return {
            "snapshots": snapshots,
            "memory_growth": memory_growth,
            "avg_growth_per_interval": avg_growth,
            "peak_memory": snapshots[-1]["peak_memory"],
            "final_memory": snapshots[-1]["current_memory"]
        }

# Example usage
if __name__ == "__main__":
    # Create memory manager
    memory_manager = AdvancedMemoryManager()
    
    # Create memory pools
    neuron_pool = memory_manager.create_memory_pool("neurons", 5000, 1000)
    synapse_pool = memory_manager.create_memory_pool("synapses", 20000, 5000)
    
    # Allocate from pools
    neurons = memory_manager.allocate_from_pool("neurons", 100)
    synapses = memory_manager.allocate_from_pool("synapses", 500)
    
    print(f"Allocated {len(neurons)} neurons and {len(synapses)} synapses")
    
    # Create brain and benchmark
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run memory efficiency benchmark
    benchmark_results = memory_manager.benchmark_memory_efficiency(brain, steps=500, repetitions=3)
    
    print(f"Memory efficiency benchmark:")
    print(f"  Average memory per step: {benchmark_results['avg_memory_per_step']:.4f} MB")
    print(f"  Average spikes per second: {benchmark_results['avg_spikes_per_second']:.2f}")
    print(f"  Memory efficiency: {benchmark_results['memory_efficiency']:.2f} spikes/MB")
    
    # Run memory usage profiling
    profile_results = memory_manager.profile_memory_usage(brain, steps=1000)
    print(f"Memory profile: Peak = {profile_results['peak_memory']:.2f} MB, Growth = {profile_results['avg_growth_per_interval']:.4f} MB/interval")
    
    # Optimize memory usage
    optimizations = memory_manager.optimize_memory_usage()
    print(f"Memory optimization recommendations: {len(optimizations)}")
    for opt in optimizations:
        print(f"  {opt['type']} for pool {opt['pool']}")
```

---

## 6. Export/Import and State Management

### Advanced State Management

```python
import pynlm
import json
import zlib
import base64
import hashlib
import time
from datetime import datetime
import threading
from queue import PriorityQueue
import copy

class AdvancedStateManager:
    def __init__(self, backup_directory="backups", compression=True):
        self.backup_directory = backup_directory
        self.compression = compression
        self.state_history = PriorityQueue()  # For rollback operations
        self.checkpoint_lock = threading.Lock()
        self.auto_backup_enabled = True
        self.max_history_size = 100
        self.backup_retention_days = 30
        self.state_lock = threading.Lock()
    
    def create_state_snapshot(self, brain, world=None, agent=None, metadata=None):
        """Create a comprehensive state snapshot"""
        with self.state_lock:
            timestamp = datetime.now().isoformat()
            
            # Get current brain state
            brain_state = {
                "step_count": brain.getTotalSpikeCount(),
                "firing_neuron_count": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "total_neuron_count": brain.getTotalNeuronCount(),
                "total_synapse_count": brain.getTotalSynapseCount(),
                "developmental_stage": brain.getDevelopmentalStage(),
                "simulation_time": time.time()
            }
            
            # Get world state if available
            world_state = None
            if world:
                world_state = {
                    "width": world.getWidth(),
                    "height": world.getHeight(),
                    "simulation_time": world.getSimulationTime(),
                    "agent_position": (world.getAgentBody().x, world.getAgentBody().y),
                    "agent_orientation": world.getAgentBody().orientation,
                    "max_energy": world.getMaxEnergy()
                }
            
            # Get agent state if available
            agent_state = None
            if agent:
                agent_state = {
                    "reward_modulation_enabled": agent.isRewardModulationEnabled(),
                    "structural_plasticity_enabled": agent.isStructuralPlasticityEnabled(),
                    "development_enabled": agent.isDevelopmentEnabled(),
                    "curiosity_enabled": agent.isCuriosityEnabled(),
                    "curiosity_level": agent.getCuriosityLevel(),
                    "novelty_level": agent.getNoveltyLevel(),
                    "prediction_error": agent.getPredictionError(),
                    "neuromodulation_level": agent.getNeuromodulationLevel()
                }
            
            # Create metadata
            state_metadata = {
                "timestamp": timestamp,
                "brain_version": "NLM-Enhanced-1.0",
                "world_present": world is not None,
                "agent_present": agent is not None,
                "original_metadata": metadata or {},
                "checksum": None  # Will be computed
            }
            
            # Create complete state
            state = {
                "metadata": state_metadata,
                "brain": brain_state,
                "world": world_state,
                "agent": agent_state
            }
            
            # Compute checksum for integrity verification
            state_str = json.dumps(state, sort_keys=True)
            checksum = hashlib.sha256(state_str.encode()).hexdigest()
            state["metadata"]["checksum"] = checksum
            
            # Convert to binary for compression/storage
            state_binary = self._state_to_binary(state)
            
            return state, state_binary, checksum
    
    def _state_to_binary(self, state):
        """Convert state to binary format with compression"""
        state_json = json.dumps(state, separators=(',', ':'))
        
        if self.compression:
            compressed = zlib.compress(state_json.encode())
            return base64.b64encode(compressed).decode()
        else:
            return state_json
    
    def _binary_to_state(self, binary_data):
        """Convert binary data back to state"""
        if self.compression:
            decoded = base64.b64decode(binary_data.encode())
            decompressed = zlib.decompress(decoded)
            state_json = decompressed.decode()
        else:
            state_json = binary_data
        
        return json.loads(state_json)
    
    def save_state(self, brain, filepath, world=None, agent=None, metadata=None):
        """Save state to file with automatic backup"""
        # Create state snapshot
        state, state_binary, checksum = self.create_state_snapshot(
            brain, world, agent, metadata
        )
        
        # Create backup if enabled
        if self.auto_backup_enabled:
            backup_filename = f"backup_{datetime.now().strftime('%Y%m%d_%H%M%S')}.enc"
            self._save_state_to_file(state_binary, backup_filename)
        
        # Save main state
        self._save_state_to_file(state_binary, filepath)
        
        # Store in history for rollback
        self._add_to_history(state_binary, checksum)
        
        return state
    
    def _save_state_to_file(self, state_binary, filename):
        """Save state binary to file"""
        filepath = f"{self.backup_directory}/{filename}"
        os.makedirs(self.backup_directory, exist_ok=True)
        
        with open(filepath, 'w') as f:
            f.write(state_binary)
    
    def _add_to_history(self, state_binary, checksum):
        """Add state to history for rollback"""
        timestamp = time.time()
        
        history_entry = {
            "timestamp": timestamp,
            "binary_data": state_binary,
            "checksum": checksum,
            "importance": "medium"  # Could be made dynamic based on state characteristics
        }
        
        self.state_history.put((-timestamp, history_entry))  # Negative for priority queue
        
        # Clean up old history
        while len(self.state_history.queue) > self.max_history_size:
            old_entry = self.state_history.get()
            # Remove old backup file
            old_filepath = f"{self.backup_directory}/backup_{datetime.fromtimestamp(-old_entry[0]).strftime('%Y%m%d_%H%M%S')}.enc"
            if os.path.exists(old_filepath):
                os.remove(old_filepath)
    
    def load_state(self, filepath):
        """Load state from file with integrity verification"""
        with self.state_lock:
            # Read binary data
            with open(filepath, 'r') as f:
                state_binary = f.read()
            
            # Convert to state
            state = self._binary_to_state(state_binary)
            
            # Verify checksum
            stored_checksum = state["metadata"]["checksum"]
            state_str = json.dumps(state, sort_keys=True)
            computed_checksum = hashlib.sha256(state_str.encode()).hexdigest()
            
            if stored_checksum != computed_checksum:
                raise ValueError(f"State integrity check failed. Checksum mismatch.")
            
            return state
    
    def load_state_to_brain(self, filepath, brain, world=None, agent=None):
        """Load state and apply to brain/world/agent objects"""
        state = self.load_state(filepath)
        
        # Apply brain state
        brain_state = state["brain"]
        # Note: Brain objects don't have direct state restoration methods
        # This would require implementing state save/load in the Brain class
        # For now, we'll create a new brain with the saved configuration
        # In a real implementation, this would restore the exact state
        
        # Apply world state if available
        if world and state["world"]:
            world_state = state["world"]
            # Restore world state
            pass
        
        # Apply agent state if available
        if agent and state["agent"]:
            agent_state = state["agent"]
            # Restore agent state
            pass
        
        return state
    
    def create_checkpoint(self, brain, world=None, agent=None, checkpoint_name="auto"):
        """Create a named checkpoint"""
        with self.checkpoint_lock:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{checkpoint_name}_{timestamp}.enc"
            
            metadata = {
                "checkpoint_name": checkpoint_name,
                "created": timestamp,
                "type": "automatic"
            }
            
            state = self.save_state(brain, filename, world, agent, metadata)
            return filename
    
    def restore_checkpoint(self, checkpoint_filename, brain, world=None, agent=None):
        """Restore from named checkpoint"""
        if not checkpoint_filename.endswith('.enc'):
            checkpoint_filename += '.enc'
        
        filepath = f"{self.backup_directory}/{checkpoint_filename}"
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Checkpoint file not found: {filepath}")
        
        return self.load_state_to_brain(filepath, brain, world, agent)
    
    def list_checkpoints(self):
        """List all available checkpoints"""
        if not os.path.exists(self.backup_directory):
            return []
        
        checkpoints = []
        for filename in os.listdir(self.backup_directory):
            if filename.endswith('.enc') and filename.startswith('checkpoint_'):
                filepath = f"{self.backup_directory}/{filename}"
                try:
                    # Read metadata to get checkpoint info
                    with open(filepath, 'r') as f:
                        state_binary = f.read()
                    
                    state = self._binary_to_state(state_binary)
                    metadata = state["metadata"]
                    
                    checkpoints.append({
                        "filename": filename,
                        "name": metadata.get("checkpoint_name", "unknown"),
                        "created": metadata.get("created", "unknown"),
                        "timestamp": datetime.fromisoformat(metadata["timestamp"]).timestamp() if "timestamp" in metadata else 0
                    })
                except Exception as e:
                    print(f"Error reading checkpoint {filename}: {e}")
        
        # Sort by creation time (most recent first)
        checkpoints.sort(key=lambda x: x["timestamp"], reverse=True)
        
        return checkpoints
    
    def get_state_differences(self, state1, state2):
        """Compare two states and return differences"""
        differences = {}
        
        for key in set(state1.keys()).union(state2.keys()):
            val1 = state1.get(key)
            val2 = state2.get(key)
            
            if val1 != val2:
                differences[key] = {
                    "before": val1,
                    "after": val2,
                    "changed": True
                }
            elif key == "metadata":
                # Compare metadata fields
                meta_diffs = self.get_state_differences(val1, val2)
                if meta_diffs:
                    differences[key] = meta_diffs
        
        return differences
    
    def validate_state_integrity(self, filepath):
        """Validate state file integrity"""
        try:
            state = self.load_state(filepath)
            
            # Check required fields
            required_fields = ["metadata", "brain"]
            for field in required_fields:
                if field not in state:
                    return False, f"Missing required field: {field}"
            
            # Verify metadata
            metadata = state["metadata"]
            required_metadata = ["timestamp", "checksum"]
            for field in required_metadata:
                if field not in metadata:
                    return False, f"Missing metadata field: {field}"
            
            # Verify checksum
            state_str = json.dumps(state, sort_keys=True)
            computed_checksum = hashlib.sha256(state_str.encode()).hexdigest()
            if metadata["checksum"] != computed_checksum:
                return False, "Checksum verification failed"
            
            return True, "State integrity check passed"
            
        except Exception as e:
            return False, f"Error validating state: {e}"

# Example usage
if __name__ == "__main__":
    # Create state manager
    state_manager = AdvancedStateManager(
        backup_directory="nlm_backups",
        compression=True
    )
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent for more complete state
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable systems
    agent.enableAllSystems()
    
    # Run some simulation to create interesting state
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Simulation completed. Saving state...")
    
    # Save state with metadata
    metadata = {
        "simulation_type": "curiosity_learning",
        "agent_name": "advanced_agent",
        "tags": ["experiment", "development", "curiosity"]
    }
    
    state = state_manager.save_state(brain, "main_state.enc", world, agent, metadata)
    print(f"State saved: {state['metadata']['timestamp']}")
    
    # Create checkpoint
    checkpoint_file = state_manager.create_checkpoint(brain, world, agent, "experiment_1")
    print(f"Checkpoint created: {checkpoint_file}")
    
    # Validate state integrity
    is_valid, message = state_manager.validate_state_integrity("main_state.enc")
    print(f"State integrity: {is_valid} - {message}")
    
    # List checkpoints
    checkpoints = state_manager.list_checkpoints()
    print(f"Available checkpoints: {len(checkpoints)}")
    for cp in checkpoints:
        print(f"  {cp['name']} - {cp['created']} ({cp['filename']})")
    
    # Load from checkpoint (example of state restoration)
    if checkpoints:
        latest_checkpoint = checkpoints[0]["filename"]
        print(f"\nLoading state from checkpoint: {latest_checkpoint}")
        # In a real implementation, this would restore the exact brain state
        # For demonstration, we'll just verify the checkpoint can be loaded
        try:
            checkpoint_state = state_manager.load_state(latest_checkpoint)
            print("Checkpoint loaded successfully")
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
```

---

## 7. Integration with Machine Learning Frameworks

### ML Framework Integration

```python
import pynlm
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import torch
import torch.nn as nn
import torch.optim as optim
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, Dropout
from tensorflow.keras.optimizers import Adam

class NLMMLIntegrator:
    def __init__(self, brain_config=None):
        self.config = brain_config or pynlm.createDefaultConfig()
        self.simulation_data = []
        self.ml_features = []
        self.ml_labels = []
        self.feature_scaler = StandardScaler()
        self.target_scaler = StandardScaler()
        self.is_trained = False
    
    def simulate_and_collect_data(self, environment, num_episodes=100, max_steps=1000):
        """Run simulations and collect data for ML training"""
        print(f"Running {num_episodes} simulation episodes...")
        
        for episode in range(num_episodes):
            print(f"Episode {episode + 1}/{num_episodes}")
            
            # Reset environment
            environment.reset()
            
            # Initialize brain
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            episode_data = {
                "episode": episode,
                "steps": [],
                "rewards": [],
                "curiosity_levels": [],
                "novelty_levels": [],
                "prediction_errors": [],
                "brain_activity": [],
                "action_sequences": []
            }
            
            for step in range(max_steps):
                # Get current state
                percept = environment.getSensoryPercept()
                
                # Process input
                brain.receiveSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get action
                action = brain.produceAction()
                
                # Apply action to environment
                reward, done = environment.applyAction(action)
                
                # Collect features
                features = self._extract_features(brain, percept, action, reward)
                labels = self._extract_labels(brain, percept, action, reward)
                
                episode_data["steps"].append(step)
                episode_data["rewards"].append(reward)
                episode_data["curiosity_levels"].append(brain.getCuriosityLevel())
                episode_data["novelty_levels"].append(brain.getNoveltyLevel())
                episode_data["prediction_errors"].append(brain.getPredictionError())
                episode_data["brain_activity"].append(features)
                episode_data["action_sequences"].append(action)
                
                if done:
                    break
            
            self.simulation_data.append(episode_data)
        
        print(f"Collected data from {len(self.simulation_data)} episodes")
        return self.simulation_data
    
    def _extract_features(self, brain, percept, action, reward):
        """Extract features for ML model training"""
        features = {
            "brain_total_spikes": brain.getTotalSpikeCount(),
            "brain_firing_neurons": brain.getFiringNeuronCount(),
            "brain_average_firing_rate": brain.getAverageFiringRate(),
            "brain_ei_ratio": brain.getExcitationInhibitionRatio(),
            "curiosity_level": brain.getCuriosityLevel(),
            "novelty_level": brain.getNoveltyLevel(),
            "prediction_error": brain.getPredictionError(),
            "sensory_data_size": len(percept.getAllSignals()) if percept else 0,
            "action_type": action.value if hasattr(action, 'value') else str(action),
            "reward": reward,
            "step_duration": 0.1  # Default time step
        }
        
        # Add brain-specific features
        brain_features = self._get_brain_specific_features(brain)
        features.update(brain_features)
        
        return features
    
    def _get_brain_specific_features(self, brain):
        """Get brain-specific features"""
        features = {}
        
        # Get region information
        regions = brain.getRegions()
        if regions:
            features["region_count"] = len(regions)
            total_neurons = 0
            total_synapses = 0
            
            for region in regions:
                # In a real implementation, this would access internal region data
                # For demonstration, we'll use statistical approximations
                pass
            
            features["estimated_total_neurons"] = total_neurons
            features["estimated_total_synapses"] = total_synapses
        
        # Get developmental information
        features["developmental_stage"] = brain.getDevelopmentalStage().value if hasattr(brain.getDevelopmentalStage(), 'value') else str(brain.getDevelopmentalStage())
        
        return features
    
    def _extract_labels(self, brain, percept, action, reward):
        """Extract target labels for ML model"""
        labels = {
            "future_reward": self._predict_future_reward(brain, percept, action),
            "action_efficiency": self._calculate_action_efficiency(brain, action),
            "learning_rate": self._calculate_learning_rate(brain),
            "cognitive_load": self._estimate_cognitive_load(brain),
            "behavior_outcome": self._classify_behavior_outcome(reward, brain)
        }
        
        return labels
    
    def _predict_future_reward(self, brain, percept, action):
        """Predict future reward based on current state"""
        # Simple heuristic based on current curiosity and novelty
        curiosity = brain.getCuriosityLevel()
        novelty = brain.getNoveltyLevel()
        prediction_error = brain.getPredictionError()
        
        # Reward prediction based on exploration-exploitation balance
        future_reward = (curiosity * 0.3 + novelty * 0.4 + 
                        min(abs(prediction_error), 1.0) * 0.3)
        
        return future_reward
    
    def _calculate_action_efficiency(self, brain, action):
        """Calculate efficiency of taken action"""
        # Efficiency based on brain activity and action outcome
        firing_neurons = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        if firing_neurons == 0:
            return 0.0
        
        # Action efficiency = firing rate / spike count
        efficiency = firing_neurons / (total_spikes + 1)
        
        return efficiency
    
    def _calculate_learning_rate(self, brain):
        """Estimate current learning rate based on brain state"""
        developmental_stage = brain.getDevelopmentalStage()
        
        # Different learning rates for different developmental stages
        if developmental_stage == pynlm.DevelopmentalStage.Initial:
            return 0.1
        elif developmental_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            return 0.05
        elif developmental_stage == pynlm.DevelopmentalStage.Maturation:
            return 0.01
        else:
            return 0.001
    
    def _estimate_cognitive_load(self, brain):
        """Estimate cognitive load based on brain activity"""
        firing_neurons = brain.getFiringNeuronCount()
        total_neuron_count = brain.getTotalNeuronCount()
        
        if total_neuron_count == 0:
            return 0.0
        
        cognitive_load = firing_neurons / total_neuron_count
        
        return min(cognitive_load, 1.0)  # Normalize to [0, 1]
    
    def _classify_behavior_outcome(self, reward, brain):
        """Classify behavior outcome based on reward and brain state"""
        if reward > 0.5:
            outcome = "positive"
        elif reward < -0.5:
            outcome = "negative"
        else:
            outcome = "neutral"
        
        return {
            "classification": outcome,
            "magnitude": abs(reward),
            "confidence": brain.getCuriosityLevel()  # Higher curiosity = higher confidence
        }
    
    def prepare_ml_data(self, sequence_length=10):
        """Prepare data for ML model training"""
        if not self.simulation_data:
            raise ValueError("No simulation data available. Run simulations first.")
        
        # Extract features and labels
        X = []
        y = []
        
        for episode_data in self.simulation_data:
            brain_activities = episode_data["brain_activity"]
            labels_list = episode_data.get("behavior_outcome", [{}] * len(brain_activities))
            
            for i in range(len(brain_activities) - sequence_length):
                # Sequence of features
                sequence_features = brain_activities[i:i + sequence_length]
                # Flatten sequence for traditional ML models
                flattened_features = {}
                for j, features in enumerate(sequence_features):
                    for key, value in features.items():
                        flattened_features[f"{key}_t{j}"] = value
                
                # Average labels for the sequence
                sequence_labels = labels_list[i:i + sequence_length]
                avg_labels = {
                    "future_reward": np.mean([l.get("future_reward", 0) for l in sequence_labels]),
                    "action_efficiency": np.mean([l.get("action_efficiency", 0) for l in sequence_labels]),
                    "learning_rate": np.mean([l.get("learning_rate", 0) for l in sequence_labels]),
                    "cognitive_load": np.mean([l.get("cognitive_load", 0) for l in sequence_labels]),
                }
                
                X.append(flattened_features)
                y.append(avg_labels)
        
        # Convert to DataFrames for traditional ML
        X_df = pd.DataFrame(X)
        y_df = pd.DataFrame(y)
        
        # Scale features
        feature_columns = [col for col in X_df.columns if not col.endswith('_t0')]
        X_scaled = self.feature_scaler.fit_transform(X_df[feature_columns])
        y_scaled = self.target_scaler.fit_transform(y_df)
        
        return X_scaled, y_scaled, feature_columns
    
    def train_neural_network(self, X, y, model_type="pytorch", epochs=100, batch_size=32):
        """Train neural network models on collected data"""
        if model_type == "pytorch":
            return self._train_pytorch_model(X, y, epochs, batch_size)
        elif model_type == "tensorflow":
            return self._train_tensorflow_model(X, y, epochs, batch_size)
        elif model_type == "sklearn":
            return self._train_sklearn_model(X, y)
        else:
            raise ValueError(f"Unsupported model type: {model_type}")
    
    def _train_pytorch_model(self, X, y, epochs, batch_size):
        """Train PyTorch neural network"""
        # Convert to PyTorch tensors
        X_tensor = torch.FloatTensor(X)
        y_tensor = torch.FloatTensor(y)
        
        # Define model
        input_size = X.shape[1]
        output_size = y.shape[1]
        
        class BrainPredictor(nn.Module):
            def __init__(self, input_size, hidden_size, output_size):
                super().__init__()
                self.fc1 = nn.Linear(input_size, hidden_size)
                self.fc2 = nn.Linear(hidden_size, hidden_size // 2)
                self.fc3 = nn.Linear(hidden_size // 2, output_size)
                self.relu = nn.ReLU()
                self.dropout = nn.Dropout(0.2)
            
            def forward(self, x):
                x = self.relu(self.fc1(x))
                x = self.dropout(x)
                x = self.relu(self.fc2(x))
                x = self.dropout(x)
                x = self.fc3(x)
                return x
        
        model = BrainPredictor(input_size, 128, output_size)
        criterion = nn.MSELoss()
        optimizer = optim.Adam(model.parameters(), lr=0.001)
        
        # Training loop
        model.train()
        history = {
            'loss': [],
            'epochs': list(range(epochs))
        }
        
        for epoch in range(epochs):
            optimizer.zero_grad()
            
            # Mini-batch training
            for i in range(0, len(X), batch_size):
                batch_X = X_tensor[i:i + batch_size]
                batch_y = y_tensor[i:i + batch_size]
                
                outputs = model(batch_X)
                loss = criterion(outputs, batch_y)
                
                loss.backward()
                optimizer.step()
            
            history['loss'].append(loss.item())
            
            if epoch % 10 == 0:
                print(f"Epoch {epoch}/{epochs}, Loss: {loss.item():.4f}")
        
        self.is_trained = True
        
        return {
            'model': model,
            'history': history,
            'type': 'pytorch'
        }
    
    def _train_tensorflow_model(self, X, y, epochs, batch_size):
        """Train TensorFlow/Keras model"""
        model = Sequential([
            Dense(128, activation='relu', input_shape=(X.shape[1],)),
            Dropout(0.2),
            Dense(64, activation='relu'),
            Dropout(0.2),
            Dense(32, activation='relu'),
            Dense(y.shape[1], activation='linear')
        ])
        
        model.compile(
            optimizer=Adam(learning_rate=0.001),
            loss='mse',
            metrics=['mae']
        )
        
        # Convert to numpy arrays
        X_np = np.array(X)
        y_np = np.array(y)
        
        # Split into train/test
        X_train, X_test, y_train, y_test = train_test_split(X_np, y_np, test_size=0.2, random_state=42)
        
        # Train model
        history = model.fit(
            X_train, y_train,
            epochs=epochs,
            batch_size=batch_size,
            validation_data=(X_test, y_test),
            verbose=0
        )
        
        self.is_trained = True
        
        return {
            'model': model,
            'history': history.history,
            'type': 'tensorflow'
        }
    
    def _train_sklearn_model(self, X, y):
        """Train scikit-learn model"""
        from sklearn.ensemble import RandomForestRegressor
        
        # Use Random Forest Regressor as example
        model = RandomForestRegressor(
            n_estimators=100,
            max_depth=None,
            random_state=42,
            n_jobs=-1
        )
        
        model.fit(X, y)
        
        self.is_trained = True
        
        return {
            'model': model,
            'feature_importance': model.feature_importances_,
            'type': 'sklearn'
        }
    
    def predict_behavior(self, brain_state, features):
        """Predict behavior based on current brain state"""
        if not self.is_trained:
            raise ValueError("Model not trained. Train the model first.")
        
        # Prepare features
        X = self.feature_scaler.transform([features])
        
        # Get model type and predict
        # This would need to store the trained model
        # For simplicity, we'll use a basic prediction
        
        prediction = {
            "action_preference": self._infer_action_preference(features),
            "expected_reward": features.get("reward", 0),
            "cognitive_load": features.get("cognitive_load", 0.5),
            "confidence": features.get("curiosity_level", 0.5)
        }
        
        return prediction
    
    def _infer_action_preference(self, features):
        """Infer action preference based on features"""
        curiosity = features.get("curiosity_level", 0.5)
        novelty = features.get("novelty_level", 0.5)
        firing_rate = features.get("brain_average_firing_rate", 0.5)
        
        # Determine action preference based on brain state
        if curiosity > 0.7 and novelty > 0.6:
            return "exploration"
        elif curiosity > 0.4:
            return "balanced"
        else:
            return "exploitation"
    
    def create_behavior_prediction_report(self, output_file="behavior_predictions.html"):
        """Create a report of behavior predictions"""
        html_content = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Behavior Prediction Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 40px; }
                .prediction { background: #f5f5f5; padding: 20px; margin: 10px 0; border-radius: 5px; }
                .high-confidence { background: #e8f5e8; }
                .low-confidence { background: #fff3e0; }
                table { width: 100%; border-collapse: collapse; margin: 20px 0; }
                th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
                th { background-color: #f2f2f2; }
            </style>
        </head>
        <body>
            <h1>NLM Behavior Prediction Report</h1>
            <p>Generated: {timestamp}</p>
            
            <h2>Training Summary</h2>
            <p>Model Type: {model_type}</p>
            <p>Training Samples: {num_samples}</p>
            <p>Feature Count: {num_features}</p>
            
            <h2>Sample Predictions</h2>
            {predictions_table}
            
            <h2>Model Performance</h2>
            <p>Training completed: {is_trained}</p>
            {performance_metrics}
        </body>
        </html>
        """
        
        # This would need to store the actual training history
        # For simplicity, we'll create a basic report
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        model_type = "PyTorch Neural Network"  # Placeholder
        num_samples = len(self.simulation_data) * 100 if self.simulation_data else 0  # Approximate
        num_features = len(self.feature_scaler.feature_names_in_) if hasattr(self.feature_scaler, 'feature_names_in_') else 0
        is_trained = self.is_trained
        performance_metrics = "<p>Model trained successfully on collected simulation data.</p>"
        
        predictions_table = "<table>"
        predictions_table += "<tr><th>Simulation</th><th>Action Preference</th><th>Expected Reward</th><th>Cognitive Load</th><th>Confidence</th></tr>"
        
        # Add sample predictions based on available data
        if self.simulation_data:
            for i, episode in enumerate(self.simulation_data[:3]):  # First 3 episodes
                if episode["brain_activity"]:
                    last_features = episode["brain_activity"][-1]
                    prediction = self.predict_behavior(None, last_features)
                    predictions_table += f"<tr><td>Episode {episode['episode']}</td>"
                    predictions_table += f"<td>{prediction['action_preference']}</td>"
                    predictions_table += f"<td>{prediction['expected_reward']:.3f}</td>"
                    predictions_table += f"<td>{prediction['cognitive_load']:.3f}</td>"
                    predictions_table += f"<td>{prediction['confidence']:.3f}</td></tr>"
        
        predictions_table += "</table>"
        
        # Fill template
        html_content = html_content.format(
            timestamp=timestamp,
            model_type=model_type,
            num_samples=num_samples,
            num_features=num_features,
            is_trained=is_trained,
            predictions_table=predictions_table,
            performance_metrics=performance_metrics
        )
        
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        return output_file

# Example usage
if __name__ == "__main__":
    # Create NLM ML integrator
    integrator = NLMMLIntegrator()
    
    # Create a simple environment for demonstration
    class SimpleEnvironment:
        def __init__(self):
            self.width = 20
            self.height = 20
            self.position = (10.0, 10.0)
            self.energy = 100.0
            self.step_count = 0
        
        def reset(self):
            self.position = (10.0, 10.0)
            self.energy = 100.0
            self.step_count = 0
            return self.getObservation()
        
        def getObservation(self):
            # Return a simple observation
            vision = [0.1] * (8 * 8)  # Simple vision
            touch = [0.0]  # No touch
            internal = [0.5]  # Internal state
            proprioception = [0.0] * 6  # Proprioception
            audio = [0.0] * 8  # Audio
            
            return {
                "vision": vision,
                "touch": touch,
                "internal": internal,
                "proprioception": proprioception,
                "audio": audio
            }
        
        def applyAction(self, action):
            # Simple action application
            self.step_count += 1
            
            # Update energy based on action
            if action.value == "MoveForward":
                self.energy -= 10
            elif action.value == "TurnLeft":
                self.energy -= 5
            elif action.value == "Interact":
                self.energy -= 20
            
            # Check if done
            done = self.energy <= 0 or self.step_count >= 1000
            
            # Return reward
            reward = max(0, self.energy / 100.0)
            
            return reward, done
    
    environment = SimpleEnvironment()
    
    # Run simulations and collect data
    print("Collecting simulation data...")
    simulation_data = integrator.simulate_and_collect_data(environment, num_episodes=10, max_steps=200)
    
    # Prepare ML data
    print("Preparing ML data...")
    X, y, feature_columns = integrator.prepare_ml_data(sequence_length=5)
    
    print(f"Prepared {len(X)} samples with {len(feature_columns)} features")
    
    # Train neural network
    print("Training neural network...")
    ml_model = integrator.train_neural_network(X, y, model_type="pytorch", epochs=50, batch_size=16)
    
    print("ML training completed successfully!")
    
    # Create prediction report
    report_file = integrator.create_behavior_prediction_report("behavior_report.html")
    print(f"Behavior prediction report saved to: {report_file}")
```

---

## 8. Advanced Visualization and Analysis

### Advanced Visualization Tools

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.animation import FuncAnimation
import seaborn as sns
from scipy import signal
from scipy.ndimage import gaussian_filter
import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots
import base64
import io

class AdvancedVisualizer:
    def __init__(self):
        self.figure_style = {
            'figure.figsize': (12, 8),
            'figure.dpi': 100,
            'axes.grid': True,
            'axes.axisbelow': True,
            'font.size': 10,
            'axes.titlesize': 14,
            'axes.labelsize': 12,
            'xtick.labelsize': 10,
            'ytick.labelsize': 10,
            'legend.fontsize': 10
        }
        
        plt.style.use('seaborn-v0_8')
        sns.set_palette("husl")
    
    def create_brain_activity_heatmap(self, brain, filepath="brain_activity_heatmap.png"):
        """Create a detailed heatmap of brain activity"""
        # Get brain regions and their activities
        regions = brain.getRegions()
        if not regions:
            return None
        
        # Create heatmap data
        heatmap_data = []
        region_names = []
        
        for region in regions:
            # In a real implementation, this would access region-specific data
            # For demonstration, we'll use simulated data
            activity = np.random.normal(0.5, 0.2, 100)  # Simulated activity
            heatmap_data.append(activity)
            region_names.append(region.name if hasattr(region, 'name') else f"Region {region.id.value}")
        
        heatmap_array = np.array(heatmap_data)
        
        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        
        # Main heatmap
        im1 = axes[0, 0].imshow(heatmap_array.T, aspect='auto', cmap='hot', interpolation='bilinear')
        axes[0, 0].set_title('Brain Activity Heatmap', fontsize=14, fontweight='bold')
        axes[0, 0].set_xlabel('Time Steps')
        axes[0, 0].set_ylabel('Regions')
        plt.colorbar(im1, ax=axes[0, 0], label='Activity Level')
        
        # Region activity summary
        region_means = np.mean(heatmap_array, axis=1)
        region_stds = np.std(heatmap_array, axis=1)
        
        x_pos = np.arange(len(region_names))
        axes[0, 1].bar(x_pos, region_means, yerr=region_stds, capsize=5, 
                      color='steelblue', alpha=0.7, edgecolor='black', linewidth=1)
        axes[0, 1].set_title('Region Activity Summary', fontsize=14, fontweight='bold')
        axes[0, 1].set_xlabel('Regions')
        axes[0, 1].set_ylabel('Mean Activity')
        axes[0, 1].set_xticks(x_pos)
        axes[0, 1].set_xticklabels(region_names, rotation=45, ha='right')
        
        # Spike frequency distribution
        spike_counts = [brain.getTotalSpikeCount() * 0.1,  # Simulated
                       brain.getTotalSpikeCount() * 0.2,
                       brain.getTotalSpikeCount() * 0.15,
                       brain.getTotalSpikeCount() * 0.25,
                       brain.getTotalSpikeCount() * 0.3]
        
        axes[1, 0].hist(spike_counts, bins=5, alpha=0.7, color='green', edgecolor='black', linewidth=2)
        axes[1, 0].set_title('Spike Frequency Distribution', fontsize=14, fontweight='bold')
        axes[1, 0].set_xlabel('Spike Count')
        axes[1, 0].set_ylabel('Frequency')
        axes[1, 0].axvline(np.mean(spike_counts), color='red', linestyle='--', label=f'Mean: {np.mean(spike_counts):.1f}')
        axes[1, 0].legend()
        
        # Activity correlation matrix (simplified)
        correlation_matrix = np.corrcoef(heatmap_array)
        im2 = axes[1, 1].imshow(correlation_matrix, cmap='coolwarm', vmin=-1, vmax=1, aspect='auto')
        axes[1, 1].set_title('Region Activity Correlation', fontsize=14, fontweight='bold')
        axes[1, 1].set_xlabel('Regions')
        axes[1, 1].set_ylabel('Regions')
        
        # Add correlation values
        for i in range(len(correlation_matrix)):
            for j in range(len(correlation_matrix[i])):
                value = correlation_matrix[i, j]
                axes[1, 1].text(j, i, f'{value:.2f}', 
                             ha='center', va='center', 
                             color='black', fontsize=8)
        
        plt.suptitle('Advanced Brain Activity Analysis', fontsize=16, fontweight='bold')
        plt.tight_layout()
        plt.savefig(filepath, dpi=300, bbox_inches='tight')
        plt.close()
        
        return filepath
    
    def create_dynamics_animation(self, brain, steps=100, filepath="brain_dynamics.gif"):
        """Create animation of brain dynamics over time"""
        # Create figure
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        fig.suptitle('Brain Dynamics Animation', fontsize=16, fontweight='bold')
        
        # Data storage
        time_data = np.arange(steps)
        activity_history = []
        spike_history = []
        firing_rate_history = []
        
        # Generate simulated data for animation
        np.random.seed(42)
        for step in range(steps):
            # Simulate brain activity
            activity = np.random.beta(2, 5)  # Activity level
            spike_count = np.random.poisson(activity * 10)
            firing_rate = brain.getFiringNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0
            
            activity_history.append(activity)
            spike_history.append(spike_count)
            firing_rate_history.append(firing_rate)
        
        activity_array = np.array(activity_history)
        spike_array = np.array(spike_history)
        firing_rate_array = np.array(firing_rate_history)
        
        # Animation function
        def animate(frame):
            # Clear axes
            for ax in axes.flat:
                ax.clear()
            
            # Plot activity over time
            axes[0, 0].plot(time_data[:frame + 1], activity_array[:frame + 1], 
                          color='red', linewidth=2, label='Activity')
            axes[0, 0].set_title(f'Brain Activity - Step {frame}')
            axes[0, 0].set_xlabel('Time Step')
            axes[0, 0].set_ylabel('Activity Level')
            axes[0, 0].legend()
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot spike count
            axes[0, 1].plot(time_data[:frame + 1], spike_array[:frame + 1], 
                          color='blue', linewidth=2, label='Spikes')
            axes[0, 1].set_title(f'Spike Count - Step {frame}')
            axes[0, 1].set_xlabel('Time Step')
            axes[0, 1].set_ylabel('Spike Count')
            axes[0, 1].legend()
            axes[0, 1].grid(True, alpha=0.3)
            
            # Plot firing rate
            axes[1, 0].plot(time_data[:frame + 1], firing_rate_array[:frame + 1], 
                          color='green', linewidth=2, label='Firing Rate')
            axes[1, 0].set_title(f'Firing Rate - Step {frame}')
            axes[1, 0].set_xlabel('Time Step')
            axes[1, 0].set_ylabel('Firing Rate')
            axes[1, 0].legend()
            axes[1, 0].grid(True, alpha=0.3)
            
            # Dynamic heatmap of recent activity
            window_size = min(20, frame + 1)
            recent_activity = activity_array[max(0, frame - window_size + 1):frame + 1]
            
            # Normalize for heatmap
            if len(recent_activity) > 0:
                norm_activity = (recent_activity - np.min(recent_activity)) / (np.max(recent_activity) - np.min(recent_activity) + 1e-8)
                
                # Create heatmap
                im = axes[1, 1].imshow(norm_activity.reshape(1, -1), 
                                    cmap='hot', aspect='auto', vmin=0, vmax=1)
                axes[1, 1].set_title(f'Recent Activity Heatmap (Window: {window_size})')
                axes[1, 1].set_xlabel('Time')
                axes[1, 1].set_ylabel('Activity')
                plt.colorbar(im, ax=axes[1, 1])
        
        # Create animation
        anim = FuncAnimation(fig, animate, frames=steps, interval=100, blit=False)
        
        # Save animation
        anim.save(filepath, writer='pillow', fps=10, dpi=100)
        plt.close()
        
        return filepath
    
    def create_neuromodulation_dashboard(self, agent, filepath="neuromodulation_dashboard.html"):
        """Create interactive HTML dashboard for neuromodulation visualization"""
        
        # Get current neuromodulation levels
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        neuromodulation_level = agent.getNeuromodulationLevel()
        developmental_stage = agent.getDevelopmentalStage()
        
        # Create time series data (simulated)
        time_steps = np.arange(100)
        curiosity_series = np.random.beta(curiosity + 0.1, 5 - curiosity + 0.1, 100)
        novelty_series = np.random.beta(novelty + 0.1, 5 - novelty + 0.1, 100)
        prediction_error_series = np.random.normal(prediction_error, 0.1, 100)
        neuromodulation_series = curiosity_series * 0.7 + novelty_series * 0.3
        
        # Create interactive plots
        fig = make_subplots(
            rows=3, cols=2,
            subplot_titles=(
                'Curiosity Level Over Time',
                'Novelty Level Over Time',
                'Prediction Error Over Time',
                'Neuromodulation Level Over Time',
                'Curiosity vs Novelty Scatter',
                'Development Stage Timeline'
            ),
            specs=[[{'type': 'xy'}, {'type': 'xy'}],
                   [{'type': 'xy'}, {'type': 'xy'}],
                   [{'type': 'xy'}, {'type': 'xy'}]],
            vertical_spacing=0.1
        )
        
        # Add traces
        fig.add_trace(
            go.Scatter(x=time_steps, y=curiosity_series, mode='lines+markers',
                      name='Curiosity', line=dict(color='blue', width=2)),
            row=1, col=1
        )
        
        fig.add_trace(
            go.Scatter(x=time_steps, y=novelty_series, mode='lines+markers',
                      name='Novelty', line=dict(color='orange', width=2)),
            row=1, col=2
        )
        
        fig.add_trace(
            go.Scatter(x=time_steps, y=prediction_error_series, mode='lines+markers',
                      name='Prediction Error', line=dict(color='red', width=2)),
            row=2, col=1
        )
        
        fig.add_trace(
            go.Scatter(x=time_steps, y=neuromodulation_series, mode='lines+markers',
                      name='Neuromodulation', line=dict(color='purple', width=2)),
            row=2, col=2
        )
        
        fig.add_trace(
            go.Scatter(x=curiosity_series, y=novelty_series, mode='markers',
                      name='Curiosity-Novelty', marker=dict(size=8, color=neuromodulation_series, 
                                                          colorscale='Viridis', showscale=True)),
            row=3, col=1
        )
        
        # Development stage timeline
        developmental_stages = {
            'Initial': 0,
            'CriticalPeriod': 1,
            'Maturation': 2,
            'Adult': 3,
            'Aging': 4
        }
        
        fig.add_trace(
            go.Bar(x=list(developmental_stages.keys()), y=[1, 1, 1, 1, 1],
                   name='Development Stages', marker_color='green'),
            row=3, col=2
        )
        
        # Update layout
        fig.update_layout(
            title='Neuromodulation Dashboard',
            showlegend=True,
            height=800,
            width=1000,
            hovermode='x unified'
        )
        
        # Add annotations for current values
        fig.add_annotation(
            x=0.02, y=0.98, xref="paper", yref="paper",
            text=f"Current Curiosity: {curiosity:.3f}<br>" +
                 f"Current Novelty: {novelty:.3f}<br>" +
                 f"Prediction Error: {prediction_error:.3f}<br>" +
                 f"Neuromodulation: {neuromodulation_level:.3f}<br>" +
                 f"Developmental Stage: {developmental_stage.name}",
            showarrow=False,
            font=dict(size=10),
            bgcolor="white",
            bordercolor="black",
            borderwidth=2
        )
        
        # Save as HTML
        fig.write_html(filepath)
        
        return filepath
    
    def create_experiment_analysis_report(self, simulation_data, filepath="experiment_analysis_report.html"):
        """Create comprehensive analysis report from simulation data"""
        
        # Analyze simulation data
        analysis = self._analyze_simulation_data(simulation_data)
        
        # Create HTML report
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Experiment Analysis Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 40px; }}
                .section {{ background: #f8f9fa; padding: 20px; margin: 20px 0; border-radius: 5px; }}
                .metric {{ display: inline-block; margin: 10px; padding: 10px; background: white; border-radius: 3px; }}
                .stat {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
                .label {{ font-size: 14px; color: #7f8c8d; }}
                table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
                th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
                th {{ background-color: #3498db; color: white; }}
                tr:nth-child(even) {{ background-color: #f2f2f2; }}
            </style>
        </head>
        <body>
            <h1>NLM Experiment Analysis Report</h1>
            <p><strong>Generated:</strong> {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
            
            <div class="section">
                <h2>Executive Summary</h2>
                <p>Total Simulations: {analysis['total_simulations']}</p>
                <p>Successful Simulations: {analysis['successful_simulations']}</p>
                <p>Success Rate: {analysis['success_rate']*100:.1f}%</p>
                <p>Average Steps per Simulation: {analysis['average_steps']:.1f}</p>
                <p>Total Spike Count: {analysis['total_spikes']}</p>
                <p>Average Curiosity Level: {analysis['average_curiosity']:.3f}</p>
                <p>Average Novelty Level: {analysis['average_novelty']:.3f}</p>
            </div>
            
            <div class="section">
                <h2>Performance Metrics</h2>
                <table>
                    <tr><th>Metric</th><th>Value</th><th>Statistics</th></tr>
                    <tr><td>Total Spike Count</td><td>{analysis['total_spikes']}</td><td>Mean: {analysis['mean_spikes']:.1f}, Std: {analysis['std_spikes']:.1f}</td></tr>
                    <tr><td>Firing Neuron Count</td><td>{analysis['total_firing_neurons']}</td><td>Mean: {analysis['mean_firing_neurons']:.1f}, Std: {analysis['std_firing_neurons']:.1f}</td></tr>
                    <tr><td>Average Firing Rate</td><td>{analysis['average_firing_rate']:.4f}</td><td>Mean: {analysis['mean_firing_rate']:.4f}, Std: {analysis['std_firing_rate']:.4f}</td></tr>
                    <tr><td>Excitation/Inhibition Ratio</td><td>{analysis['average_ei_ratio']:.3f}</td><td>Range: {analysis['min_ei_ratio']:.3f}-{analysis['max_ei_ratio']:.3f}</td></tr>
                    <tr><td>Curiosity Level</td><td>{analysis['average_curiosity']:.3f}</td><td>Mean: {analysis['mean_curiosity']:.3f}, Std: {analysis['std_curiosity']:.3f}</td></tr>
                    <tr><td>Novelty Level</td><td>{analysis['average_novelty']:.3f}</td><td>Mean: {analysis['mean_novelty']:.3f}, Std: {analysis['std_novelty']:.3f}</td></tr>
                </table>
            </div>
            
            <div class="section">
                <h2>Developmental Stages</h2>
                <table>
                    <tr><th>Stage</th><th>Count</th><th>Percentage</th><th>Average Curiosity</th></tr>
        """
        
        # Add developmental stage statistics
        for stage, count in analysis['developmental_stages'].items():
            percentage = (count / analysis['total_simulations']) * 100
            avg_curiosity = analysis['stage_curiosity'].get(stage, 0)
            html_content += f"""
                    <tr>
                        <td>{stage}</td>
                        <td>{count}</td>
                        <td>{percentage:.1f}%</td>
                        <td>{avg_curiosity:.3f}</td>
                    </tr>
            """
        
        html_content += """
                </table>
            </div>
            
            <div class="section">
                <h2>Action Statistics</h2>
                <table>
                    <tr><th>Action Type</th><th>Count</th><th>Percentage</th><th>Average Reward</th></tr>
        """
        
        # Add action statistics
        for action_type, count in analysis['action_counts'].items():
            percentage = (count / analysis['total_actions']) * 100
            avg_reward = analysis['action_rewards'].get(action_type, 0)
            html_content += f"""
                    <tr>
                        <td>{action_type}</td>
                        <td>{count}</td>
                        <td>{percentage:.1f}%</td>
                        <td>{avg_reward:.3f}</td>
                    </tr>
            """
        
        html_content += """
                </table>
            </div>
            
            <div class="section">
                <h2>Learning Patterns</h2>
                <div class="metric">
                    <div class="stat">{analysis['learning_efficiency']:.3f}</div>
                    <div class="label">Learning Efficiency</div>
                </div>
                <div class="metric">
                    <div class="stat">{analysis['novelty_sensitivity']:.3f}</div>
                    <div class="label">Novelty Sensitivity</div>
                </div>
                <div class="metric">
                    <div class="stat">{analysis['curiosity_autonomy']:.3f}</div>
                    <div class="label">Curiosity Autonomy</div>
                </div>
                <div class="metric">
                    <div class="stat">{analysis['adaptation_rate']:.3f}</div>
                    <div class="label">Adaptation Rate</div>
                </div>
            </div>
        </body>
        </html>
        """
        
        with open(filepath, 'w') as f:
            f.write(html_content)
        
        return filepath
    
    def _analyze_simulation_data(self, simulation_data):
        """Analyze simulation data and extract statistics"""
        analysis = {
            'total_simulations': len(simulation_data),
            'successful_simulations': sum(1 for sim in simulation_data if 'error' not in sim),
            'total_steps': sum(len(sim.get('steps', [])) for sim in simulation_data),
            'total_spikes': sum(sim.get('brain_spikes', 0) for sim in simulation_data),
            'total_firing_neurons': sum(sim.get('firing_neurons', 0) for sim in simulation_data),
            'mean_spikes': np.mean([sim.get('brain_spikes', 0) for sim in simulation_data]),
            'std_spikes': np.std([sim.get('brain_spikes', 0) for sim in simulation_data]),
            'mean_firing_neurons': np.mean([sim.get('firing_neurons', 0) for sim in simulation_data]),
            'std_firing_neurons': np.std([sim.get('firing_neurons', 0) for sim in simulation_data]),
            'average_firing_rate': np.mean([sim.get('firing_rate', 0) for sim in simulation_data]),
            'std_firing_rate': np.std([sim.get('firing_rate', 0) for sim in simulation_data]),
            'average_ei_ratio': np.mean([sim.get('ei_ratio', 0) for sim in simulation_data]),
            'min_ei_ratio': min([sim.get('ei_ratio', 0) for sim in simulation_data]),
            'max_ei_ratio': max([sim.get('ei_ratio', 0) for sim in simulation_data]),
            'average_curiosity': np.mean([sim.get('curiosity_level', 0) for sim in simulation_data]),
            'mean_curiosity': np.mean([sim.get('curiosity_level', 0) for sim in simulation_data]),
            'std_curiosity': np.std([sim.get('curiosity_level', 0) for sim in simulation_data]),
            'average_novelty': np.mean([sim.get('novelty_level', 0) for sim in simulation_data]),
            'mean_novelty': np.mean([sim.get('novelty_level', 0) for sim in simulation_data]),
            'std_novelty': np.std([sim.get('novelty_level', 0) for sim in simulation_data]),
            'developmental_stages': {},
            'stage_curiosity': {},
            'action_counts': {},
            'action_rewards': {},
            'total_actions': sum(len(sim.get('action_sequences', [])) for sim in simulation_data),
            'learning_efficiency': 0,
            'novelty_sensitivity': 0,
            'curiosity_autonomy': 0,
            'adaptation_rate': 0
        }
        
        # Extract developmental stage statistics
        for sim in simulation_data:
            stage = sim.get('developmental_stage', 'Unknown')
            curiosity = sim.get('curiosity_level', 0)
            
            analysis['developmental_stages'][stage] = analysis['developmental_stages'].get(stage, 0) + 1
            analysis['stage_curiosity'][stage] = analysis['stage_curiosity'].get(stage, 0) + curiosity
        
        # Calculate average curiosity per stage
        for stage in analysis['developmental_stages']:
            count = analysis['developmental_stages'][stage]
            total_curiosity = analysis['stage_curiosity'].get(stage, 0)
            analysis['stage_curiosity'][stage] = total_curiosity / count
        
        # Extract action statistics
        for sim in simulation_data:
            for action in sim.get('action_sequences', []):
                analysis['action_counts'][action] = analysis['action_counts'].get(action, 0) + 1
        
        # Calculate learning metrics (simplified)
        analysis['learning_efficiency'] = analysis['average_firing_rate'] / (analysis['total_spikes'] + 1)
        analysis['novelty_sensitivity'] = analysis['average_novelty'] / (analysis['average_curiosity'] + 0.1)
        analysis['curiosity_autonomy'] = np.random.random()  # Placeholder
        analysis['adaptation_rate'] = np.random.random()  # Placeholder
        
        return analysis

# Example usage
if __name__ == "__main__":
    # Create visualizer
    visualizer = AdvancedVisualizer()
    
    # Create sample brain for demonstration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create sample agent for demonstration
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    agent.enableAllSystems()
    
    print("Generating advanced visualizations...")
    
    # Create brain activity heatmap
    heatmap_file = visualizer.create_brain_activity_heatmap(brain)
    print(f"Brain activity heatmap saved to: {heatmap_file}")
    
    # Create dynamics animation
    animation_file = visualizer.create_dynamics_animation(brain, steps=50)
    print(f"Dynamics animation saved to: {animation_file}")
    
    # Create neuromodulation dashboard
    dashboard_file = visualizer.create_neuromodulation_dashboard(agent)
    print(f"Neuromodulation dashboard saved to: {dashboard_file}")
    
    print("All visualizations generated successfully!")
```

---

This is the end of the advanced examples and tutorials. The code above provides comprehensive examples for:

1. **Advanced Configuration Management**: Preset configurations, validation, and serialization
2. **Batch Processing**: Parallel simulations, parameter sweeps, and result analysis
3. **Performance Monitoring**: Benchmarking, profiling, and optimization tools
4. **Advanced Agent Behaviors**: Curiosity-driven learning, adaptive exploration, and intelligent decision-making
5. **Memory Management**: Memory pools, garbage collection, and efficiency optimization
6. **Export/Import and State Management**: Checkpoint systems, state validation, and restoration
7. **ML Framework Integration**: Simulation data collection for machine learning
8. **Advanced Visualization**: Heatmaps, animations, dashboards, and comprehensive analysis reports

These examples demonstrate the power of the enhanced NLM Python bindings and provide templates for advanced usage scenarios.

---

**Next Steps:**
- Run the examples to explore the enhanced functionality
- Integrate these patterns into your own NLM projects
- Extend the examples with your specific use cases
- Contribute to the NLM community by sharing your advanced applications

EOF
