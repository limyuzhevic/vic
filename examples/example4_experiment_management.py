#!/usr/bin/env python3
"""
Example 4: Configuration Management and Experiment Tracking
===========================================================

This example demonstrates how to manage configurations, run experiments,
and track results systematically.
"""

import pynlm
import json
import os
import time
import argparse
from pathlib import Path

def create_experiment_config(base_config, modifications, experiment_name):
    """Create a new configuration based on a base configuration with modifications."""
    # Load base configuration
    with open(base_config, 'r') as f:
        config = json.load(f)
    
    # Apply modifications
    for key, value in modifications.items():
        config[key] = value
    
    # Add experiment metadata
    config["experiment_name"] = experiment_name
    config["creation_time"] = time.time()
    config["description"] = f"Experiment {experiment_name}"
    
    return config

def run_experiment(config, experiment_name, output_dir):
    """Run an experiment with the given configuration."""
    print(f"Running experiment: {experiment_name}")
    
    # Create brain from config
    pynlm_config = pynlm.createDefaultConfig()
    
    # Apply config parameters to pynlm config
    if "neuron_count" in config:
        pynlm_config.set("brain.neuron_count", config["neuron_count"])
    if "plasticity_learning_rate" in config:
        pynlm_config.set("plasticity.stdp.learning_rate", config["plasticity_learning_rate"])
    
    brain = pynlm.createBrain(pynlm_config)
    brain.initialize()
    
    # Run simulation
    steps = config.get("max_simulation_steps", 500)
    for step in range(steps):
        brain.step(step)
        
        # Log progress
        if step % 100 == 0:
            print(f"  Step {step}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Save results
    results = {
        "experiment_name": experiment_name,
        "config": config,
        "final_neuron_count": brain.getTotalNeuronCount(),
        "final_synapse_count": brain.getTotalSynapseCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "simulation_steps": steps,
        "completion_time": time.time()
    }
    
    # Save results file
    results_file = Path(output_dir) / f"{experiment_name}_results.json"
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"  Experiment completed. Results saved to: {results_file}")
    return results

def main():
    """Main experiment management function."""
    print("=== Configuration Management and Experiment Tracking ===")
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Manage experiments and configurations")
    parser.add_argument('--base-config', default='configs/default.cfg',
                       help='Base configuration file')
    parser.add_argument('--name', required=True, help='Experiment name')
    parser.add_argument('--output-dir', default='./experiments',
                       help='Output directory for experiment data')
    
    args = parser.parse_args()
    
    # Create output directory
    Path(args.output_dir).mkdir(exist_ok=True)
    
    # Define experiment parameters
    experiment_params = {
        "plasticity_learning_rate": 0.001,
        "connection_probability": 0.1,
        "development_synaptogenesis_rate": 0.001
    }
    
    # Create experiment configuration
    config = create_experiment_config(
        args.base_config, 
        experiment_params, 
        args.name
    )
    
    # Save configuration
    config_file = Path(args.output_dir) / f"{args.name}_config.json"
    with open(config_file, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Configuration saved to: {config_file}")
    print(f"Experiment parameters: {experiment_params}")
    
    # Run experiment
    results = run_experiment(config, args.name, args.output_dir)
    
    print(f"\nExperiment Summary:")
    print(f"  Name: {results['experiment_name']}")
    print(f"  Final neurons: {results['final_neuron_count']}")
    print(f"  Final synapses: {results['final_synapse_count']}")
    print(f"  Final firing rate: {results['final_firing_rate']:.3f}")
    print(f"  Steps simulated: {results['simulation_steps']}")
    
    # List all experiment files
    print(f"\nExperiment files in {args.output_dir}:")
    for file in Path(args.output_dir).glob("*"):
        if file.is_file():
            print(f"  {file.name}")

if __name__ == "__main__":
    main()