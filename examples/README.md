"""
NLM Python Bindings Example Scripts

This directory contains example scripts demonstrating the improved NLM Python bindings.
These examples showcase the new Pythonic APIs, convenience methods, and advanced features.
"""

# This file is for package structure. The actual examples are in the main docs directory.

# This would be in the nlm package if it were properly structured
# For now, these are just for reference

__version__ = "0.1.0"
__author__ = "NLM Research Team"

# Example imports and basic usage patterns
# These are meant to be copied into the user's Python environment

# Example 1: Basic brain creation and simulation
"""
import nlm

# Create and initialize brain
config = nlm.create_default_config()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
world = nlm.create_simple_demo_world()
result = nlm.run_simulation(brain, world, 1000)

# Get statistics
stats = brain.get_statistics()
print(f"Simulation complete: {stats}")
"""

# Example 2: Action manipulation
"""
import nlm

# Create action with Pythonic APIs
action = nlm.Action(nlm.ACTION_TYPE_MOVE_FORWARD)
action.add_param(1.0)  # Distance
action.add_param(0.5)  # Speed

# Modify using convenient methods
action.set_param(0, 2.0)  # Change distance

# Convert to dictionary for analysis
action_dict = action.to_dict()
print(f"Action: {action_dict}")
"""

# Example 3: Configuration management
"""
import nlm

# Create and modify configuration
config = nlm.create_default_config()

# Use Pythonic convenience methods
config.set_int("brain.neuron_count", 2000)
config.set_float("world.width", 50.0)

# Get values with defaults
neuron_count = config.get_int("brain.neuron_count", 1000)
width = config.get_float("world.width", 10.0)

# Validate configuration
errors = config.validate(["brain.neuron_count", "world.width"])
if not errors:
    print("Configuration is valid!")
else:
    print(f"Errors: {errors}")
"""

# Example 4: Sensory data processing
"""
import nlm

# Create world and get percept
world = nlm.create_simple_demo_world()
percept = world.getSensoryPercept()

# Convert to dictionary for easy access
percept_dict = percept.to_dict()

# Process sensory data
vision_data = percept_dict['vision']
touch_data = percept_dict['touch']

print(f"Vision shape: {len(vision_data)}")
print(f"Touch data: {touch_data}")
"""

# Example 5: Advanced simulation with agent
"""
import nlm

# Create demo environment
config = nlm.create_demo_config()
brain = nlm.create_simple_demo_brain(config)
world = nlm.create_simple_demo_world()

# Create agent
agent = nlm.create_agent(brain, world)
agent.initialize(world)

# Run agent episode
for step in range(100):
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process and act
    agent.processSensoryInput(percept)
    world.update(0.01)
    
    # Get and execute action
    action = brain.produceAction()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent episode completed!")
"""

# Example 6: Research simulation template
"""
import nlm
import numpy as np

def run_research_simulation(param_sets):
    """Run multiple research simulations with different parameters."""
    
    results = []
    
    for i, params in enumerate(param_sets):
        print(f"Running simulation {i + 1}/{len(param_sets)}")
        
        # Create configuration
        config = nlm.create_demo_config()
        
        # Apply parameters
        for key, value in params.items():
            if isinstance(value, int):
                config.set_int(key, value)
            elif isinstance(value, float):
                config.set_float(key, value)
            elif isinstance(value, bool):
                config.set_bool(key, value)
        
        # Create and run brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        world = nlm.create_simple_demo_world()
        nlm.run_simulation(brain, world, params.get('steps', 500))
        
        # Collect results
        stats = brain.get_statistics()
        results.append({
            'parameters': params,
            'statistics': stats,
            'simulation_id': i
        })
    
    return results

# Define parameter sets for research
param_sets = [
    {'plasticity.learning_rate': 0.1, 'steps': 200},
    {'plasticity.learning_rate': 0.01, 'steps': 500},
    {'brain.connection_probability': 0.01, 'steps': 300},
    {'brain.connection_probability': 0.05, 'steps': 400},
]

# Run research
research_results = run_research_simulation(param_sets)
print(f"Completed {len(research_results)} research simulations")
"""

# Example 7: Performance analysis
"""
import nlm
import time

# Create brain
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

# Profile different simulation lengths
for steps in [100, 500, 1000]:
    print(f"Profiling {steps} steps...")
    start_time = time.time()
    
    nlm.profile_brain(brain, steps)
    
    end_time = time.time()
    print(f"Completed {steps} steps in {end_time - start_time:.2f} seconds")
"""

# Example 8: Data preparation for machine learning
"""
import nlm
import sklearn.preprocessing
import numpy as np

def extract_ml_features(brain, steps=100):
    """Extract features from brain simulation for ML analysis."""
    
    features = []
    
    for step in range(steps):
        # Get comprehensive statistics
        stats = brain.get_statistics()
        
        # Create feature vector
        feature_vector = [
            stats['total_neurons'],
            stats['total_synapses'],
            stats['active_neurons'],
            stats['firing_neurons'],
            stats['total_spikes'],
            stats['avg_firing_rate'],
            stats['e_i_ratio'],
            brain.getDevelopmentalStage(),
            step
        ]
        
        features.append(feature_vector)
        
        # Advance simulation
        brain.step(step)
    
    return np.array(features)

# Example usage
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

ml_data = extract_ml_features(brain, 200)
print(f"Extracted ML features: shape {ml_data.shape}")

# Normalize for ML
scaler = sklearn.preprocessing.StandardScaler()
normalized_data = scaler.fit_transform(ml_data)
print(f"Normalized data: shape {normalized_data.shape}")
"""

# Example 9: Complete experiment pipeline
"""
import nlm
import json
import os

class NLMSimulationPipeline:
    """Complete pipeline for running NLM simulations and analyzing results."""
    
    def __init__(self, output_dir="simulation_results"):
        self.output_dir = output_dir
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
    
    def run_simulation(self, config_overrides=None, description="simulation"):
        """Run a single simulation with optional configuration overrides."""
        
        # Create base configuration
        config = nlm.create_demo_config()
        
        # Apply overrides
        if config_overrides:
            for key, value in config_overrides.items():
                if isinstance(value, int):
                    config.set_int(key, value)
                elif isinstance(value, float):
                    config.set_float(key, value)
                elif isinstance(value, bool):
                    config.set_bool(key, value)
        
        # Create brain and world
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run simulation
        print(f"Running {description}...")
        nlm.run_simulation(brain, world, config.get_int("brain.simulation_steps", 500))
        
        # Collect results
        results = {
            'config': config.getKeys(),
            'statistics': brain.get_statistics(),
            'description': description,
            'timestamp': str(time.time())
        }
        
        return results
    
    def run_experiments(self, experiment_configs):
        """Run multiple experiments with different configurations."""
        
        all_results = []
        
        for i, (exp_name, config_overrides) in enumerate(experiment_configs.items()):
            results = self.run_simulation(config_overrides, exp_name)
            results['experiment_id'] = i
            all_results.append(results)
            
            # Save individual results
            with open(f"{self.output_dir}/{exp_name}.json", 'w') as f:
                json.dump(results, f, indent=2)
        
        # Save aggregated results
        with open(f"{self.output_dir}/all_results.json", 'w') as f:
            json.dump(all_results, f, indent=2)
        
        return all_results

# Example usage
pipeline = NLMSimulationPipeline()

experiment_configs = {
    "baseline": {"brain.neuron_count": 1000, "brain.simulation_steps": 500},
    "high_plasticity": {"plasticity.learning_rate": 0.2, "brain.simulation_steps": 300},
    "low_connectivity": {"brain.connection_probability": 0.01, "brain.simulation_steps": 700},
    "fast_development": {"brain.development_rate": 5, "brain.simulation_steps": 400}
}

# Run experiments
experiment_results = pipeline.run_experiments(experiment_configs)
print(f"Completed {len(experiment_results)} experiments")

# Analyze results
for result in experiment_results:
    print(f"\nExperiment: {result['description']}")
    stats = result['statistics']
    print(f"  Neurons: {stats['total_neurons']}")
    print(f"  Spikes: {stats['total_spikes']}")
    print(f"  Final firing rate: {stats['avg_firing_rate']:.2f} Hz")
"""

# Example 10: Visualization helper functions
"""
import nlm
import matplotlib.pyplot as plt
import numpy as np

def plot_brain_statistics(brain_stats_list, title="Brain Statistics"):
    """Plot statistics from multiple brain simulations."""
    
    if not brain_stats_list:
        return
    
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    axes = axes.flatten()
    
    # Plot 1: Neuron count
    neuron_counts = [stats.get('total_neurons', 0) for stats in brain_stats_list]
    axes[0].bar(range(len(neuron_counts)), neuron_counts)
    axes[0].set_title("Total Neurons")
    axes[0].set_ylabel("Count")
    
    # Plot 2: Synapse count
    synapse_counts = [stats.get('total_synapses', 0) for stats in brain_stats_list]
    axes[1].bar(range(len(synapse_counts)), synapse_counts)
    axes[1].set_title("Total Synapses")
    axes[1].set_ylabel("Count")
    
    # Plot 3: Spike count
    spike_counts = [stats.get('total_spikes', 0) for stats in brain_stats_list]
    axes[2].bar(range(len(spike_counts)), spike_counts)
    axes[2].set_title("Total Spikes")
    axes[2].set_ylabel("Count")
    
    # Plot 4: Firing rate
    firing_rates = [stats.get('avg_firing_rate', 0) for stats in brain_stats_list]
    axes[3].bar(range(len(firing_rates)), firing_rates)
    axes[3].set_title("Average Firing Rate")
    axes[3].set_ylabel("Hz")
    
    # Plot 5: E/I ratio
    e_i_ratios = [stats.get('e_i_ratio', 1.0) for stats in brain_stats_list]
    axes[4].bar(range(len(e_i_ratios)), e_i_ratios)
    axes[4].set_title("E/I Ratio")
    axes[4].set_ylabel("Ratio")
    
    # Plot 6: Developmental stage
    developmental_stages = [stats.get('developmental_stage', 0) for stats in brain_stats_list]
    axes[5].bar(range(len(developmental_stages)), developmental_stages)
    axes[5].set_title("Developmental Stage")
    axes[5].set_ylabel("Stage")
    
    plt.suptitle(title)
    plt.tight_layout()
    plt.show()

def compare_simulations(simulations, titles):
    """Compare multiple simulations side by side."""
    
    if len(simulations) != len(titles):
        raise ValueError("Number of simulations must match number of titles")
    
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    for i, (sim_data, title) in enumerate(zip(simulations, titles)):
        # Create subplots for each metric
        metrics = ['total_neurons', 'total_synapses', 'total_spikes', 
                  'avg_firing_rate', 'e_i_ratio', 'active_neurons']
        
        for j, metric in enumerate(metrics):
            if i == 0:  # Only create labels for first simulation
                axes[j].set_ylabel(metric.replace('_', ' ').title())
            
            values = [sim.get(metric, 0) for sim in sim_data]
            axes[j].plot(values, label=title)
            axes[j].set_title(f"{metric.replace('_', ' ').title()} vs Simulation")
            axes[j].legend()
    
    plt.tight_layout()
    plt.show()

# Example usage
print("Example visualization functions loaded.")
print("Use plot_brain_statistics() or compare_simulations() with your simulation data.")
"""

print("NLM Python Bindings Examples")
print("==============================")
print("This module contains example scripts demonstrating the improved NLM Python bindings.")
print("Copy the example blocks above into your Python scripts to get started!")
