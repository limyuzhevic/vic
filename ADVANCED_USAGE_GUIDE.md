# NLM Advanced Usage Guide for Power Users

This guide provides comprehensive information for advanced users who want to get the most out of the NLM Python bindings.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Performance Optimization](#performance-optimization)
3. [Advanced Experiment Modes](#advanced-experiment-modes)
4. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
5. [Integration with External Systems](#integration-with-external-systems)
6. [Custom Brain Architectures](#custom-brain-architectures)
7. [Memory Management and Resource Control](#memory-management-and-resource-control)
8. [Advanced Simulation Control](#advanced-simulation-control)
9. [Statistical Analysis and Data Processing](#statistical-analysis-and-data-processing)
10. [Best Practices and Common Pitfalls](#best-practices-and-common-pitfalls)
11. [API Reference](#api-reference)

---

## Advanced Configuration Management

### Configuration Format Detection

NLM automatically detects the configuration file format:

```python
import pynlm

# JSON configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# YAML configuration  
config = pynlm.createDefaultConfig()
config.loadFromFile("config.yaml")

# Legacy simple format
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")
```

### Advanced Configuration Options

NLM provides extensive configuration options for advanced users:

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Network topology
config.set("neuron_count", 2000)  # Larger network
config.set("region_count", 4)     # Multiple regions
config.set("connection_probability", 0.05)  # Sparse connectivity

# Plasticity parameters
config.set("stdp_ltp_weight", 0.01)
config.set("stdp_ltd_weight", 0.012)
config.set("stdp_time_constant", 30.0)
config.set("plasticity_learning_rate", 0.05)

# Neuromodulation
config.set("dopamine_baseline", 0.1)
config.set("novelty_threshold", 0.05)
config.set("curiosity_multiplier", 3.0)

# Development
config.set("development_synaptogenesis_rate", 0.001)
config.set("development_pruning_rate", 0.0001)

# Advanced simulation parameters
config.set("basic_connectivity_injection_current", 100.0)
config.set("plasticity_injection_current", 50.0)
config.set("stdp_injection_current", 80.0)
config.set("weight_change_threshold", 0.05)
config.set("learning_threshold", 0.005)

# Memory and processing
config.set("working_memory_capacity", 500)
config.set("episodic_memory_capacity", 5000)
config.set("replay_interval", 50)
config.set("consolidation_interval", 500)

# Get configuration summary
print(config.summary())
```

### Configuration Validation and Quality Assurance

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("advanced_config.json")

# Validate configuration
issues = config.validateConfig()

if issues:
    print("Configuration issues found:")
    for issue in issues:
        print(f"  - {issue}")
else:
    print("Configuration is valid!")

# Check for specific issues
if "neuron_count" in config:
    neuron_count = config.get("neuron_count")
    if neuron_count and neuron_count.value() < 100:
        print("Warning: Small network may not show interesting dynamics")

# Get all available configuration options
all_config = config.get_advanced_config()
for key, description in all_config.items():
    print(f"{key}: {description}")
```

### Configuration Templates

#### High-Performance Simulation
```python
def get_high_performance_config():
    config = pynlm.createDefaultConfig()
    
    # Optimize for speed
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("simulation_timestep", 0.0005)  # Smaller timesteps
    config.set("connection_probability", 0.08)
    config.set("max_simulation_steps", 50000)
    
    # Balance plasticity and stability
    config.set("stdp_ltp_weight", 0.008)
    config.set("stdp_ltd_weight", 0.012)
    config.set("plasticity_learning_rate", 0.02)
    
    # Enable all features
    config.set("synaptogenesis_rate", 0.0005)
    config.set("pruning_rate", 0.0001)
    
    return config
```

#### Cognitive Development Simulation
```python
def get_development_config():
    config = pynlm.createDefaultConfig()
    
    # Large network for development
    config.set("neuron_count", 5000)
    config.set("region_count", 4)
    
    # Strong developmental plasticity
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0005)
    config.set("plasticity_learning_rate", 0.1)
    
    # Extended developmental timeline
    config.set("simulation_time_limit", 300.0)  # 5 minutes
    
    # Learning parameters
    config.set("basic_connectivity_injection_current", 30.0)
    config.set("plasticity_injection_current", 20.0)
    config.set("stdp_injection_current", 40.0)
    
    return config
```

---

## Performance Optimization

### Performance Monitoring

```python
import pynlm
import time
import psutil
import os

def monitor_performance(brain, world, agent, num_steps):
    """Monitor and record performance metrics during simulation"""
    
    start_time = time.time()
    start_memory = psutil.virtual_memory().used
    
    # Run simulation with monitoring
    results = brain.run_advanced_simulation(num_steps, verbose=True)
    
    end_time = time.time()
    end_memory = psutil.virtual_memory().used
    
    # Calculate metrics
    duration = end_time - start_time
    memory_increase = end_memory - start_memory
    steps_per_second = num_steps / duration
    
    # Get brain statistics
    stats = brain.get_brain_stats()
    
    # Performance report
    report = {
        "duration_seconds": duration,
        "steps_per_second": steps_per_second,
        "memory_increase_mb": memory_increase / (1024 * 1024),
        "final_statistics": stats,
        "simulation_results": results
    }
    
    return report

# Usage example
brain = pynlm.createBrain(config)
brain.initialize()

report = monitor_performance(brain, world, agent, 1000)
print(f"Simulation completed in {report['duration_seconds']:.2f} seconds")
print(f"Performance: {report['steps_per_second']:.0f} steps/second")
```

### Memory Management

```python
import pynlm
import gc
import tracemalloc

def optimized_memory_simulation(config, num_steps):
    """Run simulation with optimized memory management"""
    
    # Start memory tracing
    tracemalloc.start()
    
    # Create brain with garbage collection
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect initial memory
    snapshot1 = tracemalloc.take_snapshot()
    
    # Run simulation
    results = []
    for step in range(num_steps):
        brain.step(step)
        
        # Periodic garbage collection
        if step % 100 == 0:
            gc.collect()
            
        # Record statistics periodically
        if step % 500 == 0:
            results.append({
                "step": step,
                "neurons_firing": brain.get_firing_neuron_count(),
                "total_spikes": brain.get_total_spike_count(),
                "avg_firing_rate": brain.get_average_firing_rate()
            })
    
    # Collect final memory
    snapshot2 = tracemalloc.take_snapshot()
    
    # Calculate memory differences
    top_stats = snapshot2.compare_to(snapshot1, 'lineno')
    
    # Get memory report
    report = {
        "simulation_steps": num_steps,
        "final_statistics": brain.get_brain_stats(),
        "memory_usage": {
            "peak_memory": max(stat.size_diff for stat in top_stats),
            "total_allocations": sum(stat.count_diff for stat in top_stats)
        },
        "intermediate_results": results
    }
    
    tracemalloc.stop()
    return report
```

### Batch Processing

```python
import pynlm
import concurrent.futures
import multiprocessing

def run_batch_simulations(config, scenarios, max_workers=None):
    """Run multiple simulations in parallel"""
    
    def run_single_simulation(scenario):
        """Run a single simulation with given scenario"""
        scenario_config = pynlm.createDefaultConfig()
        
        # Apply scenario parameters
        for key, value in scenario.items():
            scenario_config.set(key, value)
        
        brain = pynlm.createBrain(scenario_config)
        brain.initialize()
        
        # Run simulation
        results = brain.run_advanced_simulation(scenario.get("steps", 100), 
                                               verbose=scenario.get("verbose", False))
        
        return {
            "scenario": scenario["name"],
            "results": results,
            "final_stats": brain.get_brain_stats()
        }
    
    # Run simulations in parallel
    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        futures = [executor.submit(run_single_simulation, scenario) 
                  for scenario in scenarios]
        
        # Collect results
        all_results = []
        for future in concurrent.futures.as_completed(futures):
            all_results.append(future.result())
    
    return all_results

# Example batch processing
scenarios = [
    {"name": "small_network", "neuron_count": 100, "steps": 500, "verbose": False},
    {"name": "medium_network", "neuron_count": 500, "steps": 1000, "verbose": False},
    {"name": "large_network", "neuron_count": 1000, "steps": 2000, "verbose": True}
]

batch_results = run_batch_simulations(config, scenarios, max_workers=2)

for result in batch_results:
    print(f"Scenario: {result['scenario']}")
    print(f"  Duration: {result['results']['duration_seconds']:.2f}s")
    print(f"  Final spikes: {result['final_stats']['total_spikes']}")
```

---

## Advanced Experiment Modes

### Learning Experiment Framework

```python
import pynlm
from typing import Dict, List, Callable

class AdvancedLearningExperiment:
    """Framework for advanced learning experiments"""
    
    def __init__(self, config, brain=None):
        self.config = config
        self.brain = brain or pynlm.createBrain(config)
        self.brain.initialize()
        self.results = []
        self.experiments_completed = 0
    
    def run_plasticity_experiment(self, 
                                 injection_currents: List[float],
                                 duration_per_current: int = 1000,
                                 reset_between_trials: bool = True) -> Dict:
        """Advanced plasticity experiment with multiple injection conditions"""
        
        experiment_data = {
            "injection_currents": injection_currents,
            "duration_per_current": duration_per_current,
            "trials_completed": 0,
            "weight_changes": [],
            "spike_patterns": [],
            "learning_curves": []
        }
        
        for trial, current in enumerate(injection_currents):
            print(f"Running trial {trial + 1}: injection current = {current}")
            
            # Reset brain if requested
            if reset_between_trials:
                self.brain.reset()
                self.brain.initialize()
            
            # Record initial state
            initial_weights = self.get_current_weights()
            initial_spikes = self.brain.get_total_spike_count()
            
            # Apply sustained input
            for step in range(duration_per_current):
                # Inject current based on experimental protocol
                self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, current)
                self.brain.step(step)
                
                # Record spike activity
                if step % 100 == 0:
                    experiment_data["spike_patterns"].append({
                        "step": step,
                        "spikes": self.brain.get_firing_neuron_count(),
                        "total_spikes": self.brain.get_total_spike_count()
                    })
            
            # Record final state
            final_weights = self.get_current_weights()
            final_spikes = self.brain.get_total_spike_count()
            
            # Calculate weight changes
            weight_changes = self.calculate_weight_changes(initial_weights, final_weights)
            experiment_data["weight_changes"].append(weight_changes)
            
            # Record learning curve
            learning_curve = self.calculate_learning_curve(
                initial_weights, final_weights, duration_per_current
            )
            experiment_data["learning_curves"].append(learning_curve)
            
            experiment_data["trials_completed"] += 1
        
        self.results.append(experiment_data)
        self.experiments_completed += 1
        
        return experiment_data
    
    def run_stdp_verification_protocol(self,
                                    pre_post_patterns: List[Dict],
                                    num_trials_per_pattern: int = 10) -> Dict:
        """Advanced STDP verification with multiple spike patterns"""
        
        stdp_results = {
            "patterns_tested": [],
            "total_trials": 0,
            "weight_changes": {},
            "potentiation_effects": {},
            "depression_effects": {}
        }
        
        for pattern in pre_post_patterns:
            pattern_name = pattern["name"]
            stdp_results["patterns_tested"].append(pattern_name)
            
            # Initialize weight storage
            stdp_results["weight_changes"][pattern_name] = []
            stdp_results["potentiation_effects"][pattern_name] = 0
            stdp_results["depression_effects"][pattern_name] = 0
            
            for trial in range(num_trials_per_pattern):
                # Reset and configure brain
                self.brain.reset()
                self.brain.initialize()
                
                # Run STDP protocol with this pattern
                self.run_stdp_pattern(pattern, trial)
                
                # Record weight changes
                current_weights = self.get_current_weights()
                if trial > 0:
                    prev_weights = stdp_results["weight_changes"][pattern_name][-1]
                    weight_change = self.calculate_weight_change(
                        prev_weights, current_weights
                    )
                    stdp_results["weight_changes"][pattern_name].append(weight_change)
                    
                    # Track potentiation and depression
                    if weight_change > 0.001:
                        stdp_results["potentiation_effects"][pattern_name] += 1
                    elif weight_change < -0.001:
                        stdp_results["depression_effects"][pattern_name] += 1
            
            stdp_results["total_trials"] += num_trials_per_pattern
        
        self.results.append(stdp_results)
        self.experiments_completed += 1
        
        return stdp_results
    
    def run_development_experiment(self,
                                 developmental_stages: List[Dict],
                                 duration_per_stage: int = 1000) -> Dict:
        """Advanced development experiment with multiple stages"""
        
        development_data = {
            "stages": [],
            "developmental_timeline": [],
            "plasticity_trajectory": [],
            "behavioral_emergence": []
        }
        
        for stage in developmental_stages:
            stage_name = stage["name"]
            stage_start = stage["start_time"]
            stage_duration = stage.get("duration", duration_per_stage)
            
            print(f"Simulating developmental stage: {stage_name}")
            
            # Configure developmental parameters
            self.brain.set_developmental_stage(stage["stage"])
            
            # Run stage simulation
            stage_data = {
                "name": stage_name,
                "start_time": stage_start,
                "duration": stage_duration,
                "start_stats": self.get_current_stats(),
                "end_stats": None,
                "changes": {}
            }
            
            for step in range(stage_duration):
                # Update development system
                self.brain.update_development(step * 0.001)
                
                # Run brain simulation
                self.brain.step(step)
                
                # Record development metrics
                if step % 100 == 0:
                    development_data["developmental_timeline"].append({
                        "step": step,
                        "stage": stage_name,
                        "neurons_firing": self.brain.get_firing_neuron_count(),
                        "plasticity_level": self.get_plasticity_level(),
                        "novelty": self.get_novelty_level()
                    })
            
            stage_data["end_stats"] = self.get_current_stats()
            stage_data["changes"] = self.calculate_changes(
                stage_data["start_stats"], stage_data["end_stats"]
            )
            
            development_data["stages"].append(stage_data)
        
        self.results.append(development_data)
        self.experiments_completed += 1
        
        return development_data
    
    # Helper methods
    def get_current_weights(self) -> List[float]:
        """Get current synaptic weights"""
        weights = []
        for region in self.brain.get_regions():
            for synapse in region.get_synapses():
                weights.append(synapse.get_weight())
        return weights
    
    def calculate_weight_changes(self, initial_weights: List[float], 
                                final_weights: List[float]) -> List[float]:
        """Calculate weight changes between two states"""
        if len(initial_weights) != len(final_weights):
            return []
        
        changes = []
        for initial, final in zip(initial_weights, final_weights):
            changes.append(final - initial)
        
        return changes
    
    def get_current_stats(self) -> Dict:
        """Get current brain statistics"""
        stats = self.brain.get_brain_stats()
        neuromod = {}
        if hasattr(self.brain, 'getNeuromodulationLevel'):
            neuromod = {
                "dopamine": self.brain.get_neuromodulation_level(),
                "curiosity": self.brain.get_curiosity_level(),
                "novelty": self.brain.get_novelty_level(),
                "prediction_error": self.brain.get_prediction_error()
            }
        
        return {
            "statistics": stats,
            "neuromodulation": neuromod,
            "timestamp": self.brain.get_total_spike_count()
        }
    
    def calculate_learning_curve(self, initial_weights: List[float],
                                final_weights: List[float],
                                duration: int) -> Dict:
        """Calculate learning curve for weight changes"""
        if not initial_weights or not final_weights:
            return {}
        
        # Calculate various learning metrics
        mean_initial = sum(initial_weights) / len(initial_weights)
        mean_final = sum(final_weights) / len(final_weights)
        mean_change = mean_final - mean_initial
        
        # Calculate weight distribution changes
        initial_positive = sum(1 for w in initial_weights if w > 0)
        final_positive = sum(1 for w in final_weights if w > 0)
        
        # Calculate learning rate (simplified)
        learning_rate = mean_change / duration if duration > 0 else 0
        
        return {
            "mean_initial_weight": mean_initial,
            "mean_final_weight": mean_final,
            "total_change": mean_change,
            "learning_rate": learning_rate,
            "positive_synapse_change": final_positive - initial_positive,
            "stability_score": 1.0 - (abs(mean_change) / max(abs(mean_initial), 0.001))
        }
    
    def run_stdp_pattern(self, pattern: Dict, trial: int):
        """Run specific STDP spike pattern"""
        # Implementation depends on pattern specification
        # This is a simplified example
        pre_neuron = pattern.get("pre_neuron")
        post_neuron = pattern.get("post_neuron")
        pre_time = pattern.get("pre_time", 0)
        post_time = pattern.get("post_time", 10)
        
        if pre_neuron and post_neuron:
            # Pre-synaptic spike
            if pre_time >= 0:
                # Simulate pre-synaptic neuron firing
                pass
            
            # Run simulation steps
            for step in range(20):
                self.brain.step(step)
            
            # Post-synaptic spike
            if post_time >= 0:
                # Simulate post-synaptic neuron firing
                pass
    
    def get_plasticity_level(self) -> float:
        """Get current plasticity level"""
        # This would be obtained from the brain's development system
        return 1.0
    
    def get_novelty_level(self) -> float:
        """Get current novelty level"""
        # This would be obtained from the agent's novelty detector
        return 0.0
    
    def calculate_weight_change(self, prev_weights: List[float], 
                               current_weights: List[float]) -> float:
        """Calculate overall weight change between two states"""
        if not prev_weights or not current_weights:
            return 0.0
        
        if len(prev_weights) != len(current_weights):
            return 0.0
        
        total_change = 0.0
        for prev, current in zip(prev_weights, current_weights):
            total_change += abs(current - prev)
        
        return total_change / len(prev_weights)
    
    def calculate_changes(self, start_stats: Dict, end_stats: Dict) -> Dict:
        """Calculate changes between two states"""
        changes = {}
        
        # Compare statistics
        start = start_stats.get("statistics", {})
        end = end_stats.get("statistics", {})
        
        for key in start:
            if key in end:
                changes[key] = end[key] - start[key]
        
        # Compare neuromodulation
        start_neuro = start_stats.get("neuromodulation", {})
        end_neuro = end_stats.get("neuromodulation", {})
        
        for key in start_neuro:
            if key in end_neuro:
                changes[f"neuromod_{key}"] = end_neuro[key] - start_neuro[key]
        
        return changes
```

### Experiment Results Analysis

```python
def analyze_experiment_results(experiment_data: Dict) -> Dict:
    """Analyze and visualize experiment results"""
    
    analysis = {
        "summary": {},
        "patterns": {},
        "statistics": {},
        "visualizations": {}
    }
    
    # Extract key metrics
    if "weight_changes" in experiment_data:
        all_changes = []
        for trial_changes in experiment_data["weight_changes"]:
            all_changes.extend(trial_changes)
        
        analysis["statistics"]["weight_change_stats"] = {
            "mean": sum(all_changes) / len(all_changes) if all_changes else 0,
            "std": (sum((x - sum(all_changes) / len(all_changes))**2 
                       for x in all_changes) / len(all_changes))**0.5 
                   if all_changes else 0,
            "min": min(all_changes) if all_changes else 0,
            "max": max(all_changes) if all_changes else 0
        }
    
    if "learning_curves" in experiment_data:
        learning_rates = [curve["learning_rate"] for curve in experiment_data["learning_curves"]]
        analysis["statistics"]["learning_stats"] = {
            "mean_learning_rate": sum(learning_rates) / len(learning_rates) if learning_rates else 0,
            "learning_stability": 1.0 - (max(abs(lr) for lr in learning_rates) if learning_rates else 0)
        }
    
    if "spike_patterns" in experiment_data:
        spike_patterns = experiment_data["spike_patterns"]
        analysis["patterns"]["spike_activity"] = {
            "mean_spikes_per_step": sum(p["spikes"] for p in spike_patterns) / len(spike_patterns) if spike_patterns else 0,
            "max_concurrent_firing": max(p["spikes"] for p in spike_patterns) if spike_patterns else 0
        }
    
    return analysis

# Usage example
experiment = AdvancedLearningExperiment(config)
plasticity_data = experiment.run_plasticity_experiment([10.0, 20.0, 30.0, 40.0])
analysis = analyze_experiment_results(plasticity_data)

print(f"Average weight change: {analysis['statistics']['weight_change_stats']['mean']:.4f}")
print(f"Learning rate: {analysis['statistics']['learning_stats']['mean_learning_rate']:.6f}")
```

---

This advanced guide provides comprehensive information for power users to:

1. **Master advanced configuration** with extensive parameter tuning
2. **Optimize performance** through memory management and batch processing
3. **Design complex experiments** with multiple trials and conditions
4. **Analyze results** with statistical analysis and visualization tools
5. **Implement custom brain architectures** and developmental models
6. **Integrate with external systems** for real-world applications
7. **Follow best practices** and avoid common pitfalls

The NLM framework offers extensive capabilities for advanced neural simulation research.
