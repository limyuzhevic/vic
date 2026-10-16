#!/usr/bin/env python3
"""
NLM Research Script

This script provides research-oriented functionality for NLM simulations,
including experiment management, data collection, and analysis.
"""

import sys
import os
import argparse
import json
import time
import numpy as np
import pandas as pd
from pathlib import Path
from typing import Dict, List, Any, Optional

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not found.")
    print("Please build the project first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make pynlm")
    sys.exit(1)
def create_argument_parser():
    """Create and configure the argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM Research Script - Advanced NLM Simulation Research",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python research.py --experiment learning --steps 1000
  python research.py --experiment plasticity --parameters exp_params.json
  python research.py --experiment comparison --experiments baseline,high_plasticity
  python research.py --experiment neural_network --sizes 100,500,1000,2000
  python research.py --experiment timing --durations 100,500,1000,5000
  python research.py --experiment development --stages initial,critical,adult
  python research.py --experiment social --agents 2,5,10
        """
    )
    
    # Experiment types
    parser.add_argument(
        "--experiment", "-e",
        type=str,
        required=True,
        choices=["learning", "plasticity", "comparison", "neural_network", "timing", "development", "social", "custom"],
        help="Type of experiment to run",
    )
    
    # Basic experiment parameters
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of simulation steps (default: 1000)",
    )
    
    parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=1000,
        help="Number of neurons in simulation (default: 1000)",
    )
    
    parser.add_argument(
        "--timestep", "-t",
        type=float,
        default=0.01,
        help="Simulation timestep (default: 0.01)",
    )
    
    # Experiment-specific arguments
    parser.add_argument(
        "--parameters", "-P",
        type=str,
        help="Path to parameter file for custom experiments",
    )
    
    parser.add_argument(
        "--experiments", "-E",
        type=str,
        help="Comma-separated list of experiment names for comparison",
    )
    
    parser.add_argument(
        "--sizes", "-S",
        type=str,
        help="Comma-separated list of network sizes for neural network experiment",
    )
    
    parser.add_argument(
        "--durations", "-D",
        type=str,
        help="Comma-separated list of simulation durations for timing experiment",
    )
    
    parser.add_argument(
        "--stages", "-g",
        type=str,
        help="Comma-separated list of developmental stages for development experiment",
    )
    
    parser.add_argument(
        "--agents", "-a",
        type=str,
        help="Comma-separated list of agent counts for social experiment",
    )
    
    # Learning experiment specific arguments
    parser.add_argument(
        "--learning-rate", "-lr",
        type=float,
        default=0.1,
        help="Learning rate for learning experiment (default: 0.1)",
    )
    
    parser.add_argument(
        "--plasticity-type", "-pt",
        type=str,
        choices=["stdp", "hebbian", "both"],
        default="both",
        help="Type of plasticity to use (default: both)",
    )
    
    # Output options
    parser.add_argument(
        "--output", "-o",
        type=str,
        help="Output file for results (JSON format)",
    )
    
    parser.add_argument(
        "--data-dir", "-data",
        type=str,
        default="research_data",
        help="Directory for data storage (default: research_data)",
    )
    
    parser.add_argument(
        "--verbose", "-V",
        action="store_true",
        help="Enable verbose output",
    )
    
    parser.add_argument(
        "--quiet", "-q",
        action="store_true",
        help="Suppress non-essential output",
    )
    
    # Analysis options
    parser.add_argument(
        "--analyze", "-a",
        action="store_true",
        help="Perform analysis on results",
    )
    
    parser.add_argument(
        "--plot", "-p",
        action="store_true",
        help="Generate plots (requires matplotlib)",
    )
    
    parser.add_argument(
        "--statistics", "-stat",
        nargs="+",
        default=["spike_count", "firing_rate", "e_i_ratio", "learning_rate"],
        help="Statistics to collect (default: spike_count firing_rate e_i_ratio learning_rate)",
    )
    
    return parser
def load_parameter_file(param_file):
    """Load parameter variations from file."""
    if not os.path.exists(param_file):
        raise FileNotFoundError(f"Parameter file not found: {param_file}")
    
    with open(param_file, 'r') as f:
        return json.load(f)
def create_learning_experiment():
    """Create learning experiment to study synaptic plasticity."""
    print("Creating learning experiment...")
    
    # Create base configuration
    config = nlm.create_demo_config()
    
    # Configure for learning experiment
    config.set_int("brain.neuron_count", 1000)
    config.set_int("brain.simulation_steps", 1000)
    config.set_float("plasticity.learning_rate", 0.1)
    config.set_float("brain.connection_probability", 0.15)
    
    # Create brain and world
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.create_simple_demo_world()
    
    # Run learning experiment
    print("Running learning experiment...")
    
    # Collect data over time
    learning_data = {
        "step": [],
        "total_spikes": [],
        "active_neurons": [],
        "firing_rate": [],
        "e_i_ratio": [],
        "learning_rate": [],
    }
    
    for step in range(1000):
        # Inject correlated pre-post activity to induce learning
        brain.injectCurrentToNeurons(nlm.NEURON_TYPE_EXCITATORY, 30.0)
        
        # Step simulation
        world.update(0.01)
        brain.step(step, step * 0.001)
        
        # Collect data
        learning_data["step"].append(step)
        learning_data["total_spikes"].append(brain.getTotalSpikeCount())
        learning_data["active_neurons"].append(brain.getActiveNeuronCount())
        learning_data["firing_rate"].append(brain.getAverageFiringRate())
        learning_data["e_i_ratio"].append(brain.getExcitationInhibitionRatio())
        learning_data["learning_rate"].append(0.1)  # Fixed for this experiment
        
        # Print progress
        if step % 100 == 0:
            print(f"  Step {step}: Spikes = {brain.getTotalSpikeCount()}, Firing rate = {brain.getAverageFiringRate():.2f} Hz")
    
    print("Learning experiment completed!")
    return learning_data
def create_plasticity_experiment():
    """Create plasticity experiment to study different plasticity rules."""
    print("Creating plasticity experiment...")
    
    # Different plasticity configurations
    plasticity_configs = [
        {
            "name": "STDP only",
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "hebbian_weight": 0.0,
        },
        {
            "name": "Hebbian only",
            "stdp_ltp_weight": 0.0,
            "stdp_ltd_weight": 0.0,
            "hebbian_weight": 0.01,
        },
        {
            "name": "Both STDP and Hebbian",
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "hebbian_weight": 0.01,
        },
    ]
    
    all_results = {}
    
    for config in plasticity_configs:
        print(f"\nRunning {config['name']} experiment...")
        
        # Create configuration
        exp_config = nlm.create_demo_config()
        exp_config.set_int("brain.neuron_count", 1000)
        exp_config.set_int("brain.simulation_steps", 500)
        
        # Set plasticity parameters
        exp_config.set_float("stdp.ltp_weight", config["stdp_ltp_weight"])
        exp_config.set_float("stdp.ltd_weight", config["stdp_ltd_weight"])
        exp_config.set_float("hebbian.weight", config["hebbian_weight"])
        
        # Create brain and world
        brain = nlm.createBrain(exp_config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run experiment
        print(f"  Running {config['name']} for 500 steps...")
        
        for step in range(500):
            brain.injectCurrentToNeurons(nlm.NEURON_TYPE_EXCITATORY, 30.0)
            world.update(0.01)
            brain.step(step, step * 0.001)
        
        # Collect results
        results = {
            "name": config["name"],
            "statistics": {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "plasticity_config": config,
            },
            "timestamp": time.time(),
        }
        
        all_results[config["name"]] = results
        
        print(f"  - Total spikes: {results['statistics']['total_spikes']}")
        print(f"  - Firing rate: {results['statistics']['firing_rate']:.2f} Hz")
        print(f"  - E/I ratio: {results['statistics']['e_i_ratio']:.4f}")
    
    print("Plasticity experiment completed!")
    return all_results
def create_comparison_experiment():
    """Create comparison experiment to compare different configurations."""
    print("Creating comparison experiment...")
    
    # Define experiment configurations to compare
    if not hasattr(sys.argv, 'experiments') or not sys.argv.experiments:
        # Default comparison experiments
        experiment_configs = [
            {"name": "Baseline", "neurons": 1000, "plasticity": 0.01, "connection_prob": 0.1},
            {"name": "High Plasticity", "neurons": 1000, "plasticity": 0.1, "connection_prob": 0.1},
            {"name": "High Connectivity", "neurons": 1000, "plasticity": 0.01, "connection_prob": 0.2},
            {"name": "Small Network", "neurons": 500, "plasticity": 0.01, "connection_prob": 0.1},
            {"name": "Large Network", "neurons": 2000, "plasticity": 0.01, "connection_prob": 0.05},
        ]
    else:
        # Use command-line experiment list
        experiment_names = getattr(sys.argv, 'experiments', '').split(',')
        experiment_configs = []
        
        for name in experiment_names:
            name = name.strip()
            # Define configurations for common experiment names
            if name == "baseline":
                experiment_configs.append({"name": "Baseline", "neurons": 1000, "plasticity": 0.01, "connection_prob": 0.1})
            elif name == "high_plasticity":
                experiment_configs.append({"name": "High Plasticity", "neurons": 1000, "plasticity": 0.1, "connection_prob": 0.1})
            elif name == "low_connectivity":
                experiment_configs.append({"name": "Low Connectivity", "neurons": 1000, "plasticity": 0.01, "connection_prob": 0.01})
            else:
                experiment_configs.append({"name": name, "neurons": 1000, "plasticity": 0.01, "connection_prob": 0.1})
    
    comparison_results = {}
    
    for exp_config in experiment_configs:
        print(f"\nRunning comparison experiment: {exp_config['name']}...")
        
        # Create configuration
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", exp_config["neurons"])
        config.set_float("plasticity.learning_rate", exp_config["plasticity"])
        config.set_float("brain.connection_probability", exp_config["connection_prob"])
        config.set_int("brain.simulation_steps", 1000)
        
        # Create brain and world
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run experiment
        nlm.run_simulation(brain, world, 1000)
        
        # Collect results
        results = {
            "name": exp_config["name"],
            "parameters": exp_config,
            "statistics": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
            },
            "timestamp": time.time(),
        }
        
        comparison_results[exp_config["name"]] = results
        
        print(f"  - Neurons: {results['statistics']['total_neurons']}")
        print(f"  - Synapses: {results['statistics']['total_synapses']}")
        print(f"  - Spikes: {results['statistics']['total_spikes']}")
        print(f"  - Firing rate: {results['statistics']['firing_rate']:.2f} Hz")
        print(f"  - E/I ratio: {results['statistics']['e_i_ratio']:.4f}")
    
    print("Comparison experiment completed!")
    return comparison_results
def create_neural_network_experiment():
    """Create neural network experiment to study network properties."""
    print("Creating neural network experiment...")
    
    # Parse network sizes from command line or use default
    if not hasattr(sys.argv, 'sizes') or not sys.argv.sizes:
        network_sizes = [100, 500, 1000, 2000, 5000]
    else:
        network_sizes = [int(size.strip()) for size in sys.argv.sizes.split(',')]
    
    network_results = {}
    
    for size in network_sizes:
        print(f"\nRunning neural network experiment with {size} neurons...")
        
        # Create configuration
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", size)
        config.set_int("brain.simulation_steps", 500)
        
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Get network properties
        stats = brain.get_statistics()
        
        # Calculate network metrics
        metrics = {
            "network_size": size,
            "synapse_density": stats.get("total_synapses", 0) / max(size, 1),
            "firing_efficiency": stats.get("total_spikes", 0) / 500,  # spikes per step
            "excitation_inhibition_balance": stats.get("e_i_ratio", 1.0),
            "activity_level": stats.get("active_neurons", 0) / max(size, 1),
            "spike_count": stats.get("total_spikes", 0),
            "firing_rate": stats.get("avg_firing_rate", 0),
        }
        
        network_results[size] = {
            "name": f"Network with {size} neurons",
            "parameters": {
                "neuron_count": size,
                "simulation_steps": 500,
            },
            "metrics": metrics,
            "timestamp": time.time(),
        }
        
        print(f"  - Network size: {size} neurons")
        print(f"  - Synapse density: {metrics['synapse_density']:.4f} synapses/neuron")
        print(f"  - Firing efficiency: {metrics['firing_efficiency']:.4f} spikes/step")
        print(f"  - Activity level: {metrics['activity_level']:.4f}")
        print(f"  - E/I balance: {metrics['excitation_inhibition_balance']:.4f}")
    
    print("Neural network experiment completed!")
    return network_results
def create_timing_experiment():
    """Create timing experiment to study temporal dynamics."""
    print("Creating timing experiment...")
    
    # Parse simulation durations from command line or use default
    if not hasattr(sys.argv, 'durations') or not sys.argv.durations:
        simulation_durations = [100, 500, 1000, 5000, 10000]
    else:
        simulation_durations = [int(duration.strip()) for duration in sys.argv.durations.split(',')]
    
    timing_results = {}
    
    for duration in simulation_durations:
        print(f"\nRunning timing experiment with {duration} simulation steps...")
        
        # Create configuration
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", 1000)
        config.set_int("brain.simulation_steps", duration)
        
        # Create brain and world
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run experiment and collect timing data
        start_time = time.time()
        
        nlm.run_simulation(brain, world, duration)
        
        end_time = time.time()
        elapsed_ms = (end_time - start_time) * 1000
        
        # Calculate temporal metrics
        metrics = {
            "simulation_duration": duration,
            "elapsed_time_ms": elapsed_ms,
            "time_per_step_ms": elapsed_ms / duration,
            "steps_per_second": duration / (elapsed_ms / 1000),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "final_activity": brain.getActiveNeuronCount(),
            "final_firing": brain.getFiringNeuronCount(),
        }
        
        timing_results[duration] = {
            "name": f"Simulation with {duration} steps",
            "parameters": {
                "neuron_count": 1000,
                "simulation_steps": duration,
            },
            "metrics": metrics,
            "timestamp": time.time(),
        }
        
        print(f"  - Duration: {duration} steps")
        print(f"  - Elapsed time: {elapsed_ms:.2f} ms")
        print(f"  - Time per step: {metrics['time_per_step_ms']:.4f} ms")
        print(f"  - Steps per second: {metrics['steps_per_second']:.2f}")
        print(f"  - Total spikes: {metrics['total_spikes']}")
        print(f"  - Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
    
    print("Timing experiment completed!")
    return timing_results
def create_development_experiment():
    """Create development experiment to study developmental stages."""
    print("Creating development experiment...")
    
    # Parse developmental stages from command line or use default
    if not hasattr(sys.argv, 'stages') or not sys.argv.stages:
        developmental_stages = ["initial", "critical", "maturation", "adult", "aging"]
    else:
        developmental_stages = [stage.strip() for stage in sys.argv.stages.split(',')]
    
    development_results = {}
    
    for stage in developmental_stages:
        print(f"\nRunning development experiment for {stage} stage...")
        
        # Create configuration with stage-specific parameters
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", 1000)
        config.set_int("brain.simulation_steps", 200)
        
        # Set stage-specific parameters
        if stage == "initial":
            config.set_float("development.synaptogenesis_rate", 0.001)
            config.set_float("development.pruning_rate", 0.0001)
        elif stage == "critical":
            config.set_float("development.synaptogenesis_rate", 0.0005)
            config.set_float("development.pruning_rate", 0.0002)
        elif stage == "maturation":
            config.set_float("development.synaptogenesis_rate", 0.0001)
            config.set_float("development.pruning_rate", 0.0005)
        elif stage == "adult":
            config.set_float("development.synaptogenesis_rate", 0.00001)
            config.set_float("development.pruning_rate", 0.00001)
        elif stage == "aging":
            config.set_float("development.synaptogenesis_rate", 0.000001)
            config.set_float("development.pruning_rate", 0.00005)
        
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Set developmental stage
        if stage == "initial":
            brain.setDevelopmentalStage(nlm.DEVELOPMENTAL_STAGE_INITIAL)
        elif stage == "critical":
            brain.setDevelopmentalStage(nlm.DEVELOPMENTAL_STAGE_CRITICAL_PERIOD)
        elif stage == "maturation":
            brain.setDevelopmentalStage(nlm.DEVELOPMENTAL_STAGE_MATURATION)
        elif stage == "adult":
            brain.setDevelopmentalStage(nlm.DEVELOPMENTAL_STAGE_ADULT)
        elif stage == "aging":
            brain.setDevelopmentalStage(nlm.DEVELOPMENTAL_STAGE_AGING)
        
        # Create world
        world = nlm.create_simple_demo_world()
        
        # Run experiment
        nlm.run_simulation(brain, world, 200)
        
        # Collect development-specific results
        results = {
            "name": f"Development stage: {stage}",
            "stage": stage,
            "parameters": {
                "neuron_count": 1000,
                "simulation_steps": 200,
                "synaptogenesis_rate": config.get_float("development.synaptogenesis_rate", 0.0),
                "pruning_rate": config.get_float("development.pruning_rate", 0.0),
            },
            "statistics": {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": brain.getDevelopmentalStage(),
            },
            "timestamp": time.time(),
        }
        
        development_results[stage] = results
        
        print(f"  - Stage: {stage}")
        print(f"  - Total spikes: {results['statistics']['total_spikes']}")
        print(f"  - Firing rate: {results['statistics']['firing_rate']:.2f} Hz")
        print(f"  - E/I ratio: {results['statistics']['e_i_ratio']:.4f}")
        print(f"  - Development stage: {results['statistics']['developmental_stage']}")
    
    print("Development experiment completed!")
    return development_results
def create_social_experiment():
    """Create social experiment to study interaction effects."""
    print("Creating social experiment...")
    
    # Parse agent counts from command line or use default
    if not hasattr(sys.argv, 'agents') or not sys.argv.agents:
        agent_counts = [1, 2, 5, 10]
    else:
        agent_counts = [int(count.strip()) for count in sys.argv.agents.split(',')]
    
    social_results = {}
    
    for agent_count in agent_counts:
        print(f"\nRunning social experiment with {agent_count} agents...")
        
        # Create configuration
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", 1000)
        config.set_int("brain.simulation_steps", 300)
        
        # Create world and agents
        world = nlm.create_simple_demo_world()
        
        # Create multiple agents
        agents = []
        for i in range(agent_count):
            brain = nlm.createBrain(config)
            brain.initialize()
            
            agent = nlm.create_agent(brain, world)
            agent.initialize(world)
            
            # Set different starting positions for each agent
            agent_start_x = 50.0 + (i * 10) % 80
            agent_start_y = 50.0 + (i * 15) % 80
            world.setAgentStart(agent_start_x, agent_start_y)
            
            agents.append({
                "id": i,
                "brain": brain,
                "agent": agent,
            })
        
        # Run social simulation
        print(f"  Running social simulation with {agent_count} agents...")
        
        for step in range(300):
            # Each agent processes sensory input
            for agent_info in agents:
                agent_info["agent"].processSensoryInput(world.getSensoryPercept())
            
            # Simulate world step
            world.update(0.01)
            
            # Each agent produces action
            for agent_info in agents:
                action = agent_info["agent"].getBrain().produceAction()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 50 == 0:
                total_spikes = sum(agent_info["agent"].getBrain().getTotalSpikeCount() for agent_info in agents)
                print(f"    Step {step}: Total spikes = {total_spikes}")
        
        # Collect social results
        social_results[agent_count] = {
            "name": f"Social experiment with {agent_count} agents",
            "parameters": {
                "neuron_count": 1000,
                "simulation_steps": 300,
                "agent_count": agent_count,
            },
            "agent_statistics": [],
            "world_statistics": {
                "total_steps": 300,
                "final_world_time": world.getSimulationTime(),
            },
            "timestamp": time.time(),
        }
        
        # Collect individual agent statistics
        for agent_info in agents:
            agent_stats = {
                "agent_id": agent_info["id"],
                "neuron_count": agent_info["brain"].getTotalNeuronCount(),
                "total_spikes": agent_info["brain"].getTotalSpikeCount(),
                "firing_rate": agent_info["brain"].getAverageFiringRate(),
                "final_active_neurons": agent_info["brain"].getActiveNeuronCount(),
            }
            
            social_results[agent_count]["agent_statistics"].append(agent_stats)
        
        print(f"  - Total agents: {agent_count}")
        print(f"  - Total spikes: {sum(agent_info['brain'].getTotalSpikeCount() for agent_info in agents)}")
        print(f"  - Average firing rate: {np.mean([agent_info['brain'].getAverageFiringRate() for agent_info in agents]):.2f} Hz")
    
    print("Social experiment completed!")
    return social_results
def create_custom_experiment():
    """Create custom experiment from parameter file."""
    print("Creating custom experiment...")
    
    if not hasattr(sys.argv, 'parameters') or not sys.argv.parameters:
        raise ValueError("Custom experiment requires parameter file (--parameters)")
    
    # Load parameter variations
    param_variations = load_parameter_file(sys.argv.parameters)
    
    if not isinstance(param_variations, list):
        param_variations = [param_variations]
    
    custom_results = {}
    
    for i, variation in enumerate(param_variations):
        print(f"\nRunning custom variation {i + 1}/{len(param_variations)}...")
        
        # Create configuration
        config = nlm.create_demo_config()
        
        # Apply parameter variations
        for key, value in variation.items():
            if isinstance(value, int):
                config.set_int(key, value)
            elif isinstance(value, float):
                config.set_float(key, value)
            elif isinstance(value, str):
                config.set_string(key, value)
            elif isinstance(value, bool):
                config.set_bool(key, value)
        
        # Ensure required parameters are set
        if not config.has("brain.neuron_count"):
            config.set_int("brain.neuron_count", 1000)
        
        if not config.has("brain.simulation_steps"):
            config.set_int("brain.simulation_steps", 1000)
        
        # Create brain and world
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run custom experiment
        nlm.run_simulation(brain, world, config.get_int("brain.simulation_steps", 1000))
        
        # Collect results
        results = {
            "name": f"Custom variation {i + 1}",
            "parameters": variation,
            "statistics": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
            },
            "timestamp": time.time(),
        }
        
        custom_results[f"variation_{i + 1}"] = results
        
        print(f"  - Parameters: {variation}")
        print(f"  - Statistics: {results['statistics']}")
    
    print("Custom experiment completed!")
    return custom_results
def save_results(results, output_file):
    """Save experiment results to file."""
    print(f"\nSaving results to {output_file}...")
    
    # Prepare results for saving
    save_data = {
        "experiment_type": sys.argv.experiment,
        "timestamp": time.time(),
        "results": results,
    }
    
    # Save to file
    with open(output_file, 'w') as f:
        json.dump(save_data, f, indent=2, default=str)
    
    print(f"  Results saved to: {output_file}")
    return save_data
def create_analysis_report(results, analysis_file):
    """Create analysis report from experiment results."""
    print(f"\nCreating analysis report in {analysis_file}...")
    
    # Generate summary statistics
    report = {
        "title": f"NLM {sys.argv.experiment} Experiment Analysis Report",
        "timestamp": time.time(),
        "experiment_type": sys.argv.experiment,
        "summary": {
            "total_experiments": len(results) if isinstance(results, dict) else 1,
            "date": time.strftime("%Y-%m-%d %H:%M:%S"),
            "python_version": sys.version,
        },
        "detailed_results": [],
    }
    
    # Process results based on experiment type
    if isinstance(results, dict):
        for experiment_name, experiment_data in results.items():
            analysis_entry = {
                "experiment_name": experiment_name,
                "parameters": experiment_data.get("parameters", {}),
                "statistics": experiment_data.get("statistics", {}),
                "metrics": calculate_metrics(experiment_data),
            }
            
            report["detailed_results"].append(analysis_entry)
    else:
        # Single experiment result
        analysis_entry = {
            "experiment_name": f"Experiment {time.time()}",
            "parameters": {},
            "statistics": results,
            "metrics": calculate_metrics(results),
        }
        
        report["detailed_results"].append(analysis_entry)
    
    # Add statistical summaries
    report["statistical_summary"] = {
        "total_spikes_range": calculate_spike_range(report["detailed_results"]),
        "firing_rate_range": calculate_firing_rate_range(report["detailed_results"]),
        "e_i_ratio_range": calculate_ei_ratio_range(report["detailed_results"]),
        "performance_summary": calculate_performance_summary(report["detailed_results"]),
    }
    
    # Save report
    with open(analysis_file, 'w') as f:
        json.dump(report, f, indent=2, default=str)
    
    print(f"  Analysis report saved to: {analysis_file}")
    return report
def calculate_metrics(experiment_data):
    """Calculate additional metrics from experiment data."""
    metrics = {}
    
    if "statistics" not in experiment_data:
        return metrics
    
    stats = experiment_data["statistics"]
    
    # Calculate efficiency metrics
    if "total_spikes" in stats and "total_neurons" in stats:
        metrics["spike_efficiency"] = stats["total_spikes"] / max(stats["total_neurons"], 1)
    
    if "firing_rate" in stats:
        metrics["activity_level"] = stats["firing_rate"] / 100  # Normalize to 0-1
    
    if "e_i_ratio" in stats:
        metrics["balance_score"] = 1.0 / (1.0 + abs(stats["e_i_ratio"] - 1.0))
    
    return metrics
def calculate_spike_range(experiment_results):
    """Calculate range of spike counts across experiments."""
    spike_counts = []
    
    for result in experiment_results:
        stats = result.get("statistics", {})
        if "total_spikes" in stats:
            spike_counts.append(stats["total_spikes"])
    
    if spike_counts:
        return {
            "min": min(spike_counts),
            "max": max(spike_counts),
            "mean": sum(spike_counts) / len(spike_counts),
            "range": max(spike_counts) - min(spike_counts),
        }
    
    return {}
def calculate_firing_rate_range(experiment_results):
    """Calculate range of firing rates across experiments."""
    firing_rates = []
    
    for result in experiment_results:
        stats = result.get("statistics", {})
        if "firing_rate" in stats:
            firing_rates.append(stats["firing_rate"])
    
    if firing_rates:
        return {
            "min": min(firing_rates),
            "max": max(firing_rates),
            "mean": sum(firing_rates) / len(firing_rates),
            "range": max(firing_rates) - min(firing_rates),
        }
    
    return {}
def calculate_ei_ratio_range(experiment_results):
    """Calculate range of E/I ratios across experiments."""
    e_i_ratios = []
    
    for result in experiment_results:
        stats = result.get("statistics", {})
        if "e_i_ratio" in stats:
            e_i_ratios.append(stats["e_i_ratio"])
    
    if e_i_ratios:
        return {
            "min": min(e_i_ratios),
            "max": max(e_i_ratios),
            "mean": sum(e_i_ratios) / len(e_i_ratios),
            "range": max(e_i_ratios) - min(e_i_ratios),
        }
    
    return {}
def calculate_performance_summary(experiment_results):
    """Calculate performance summary across experiments."""
    performance_metrics = {}
    
    for result in experiment_results:
        stats = result.get("statistics", {})
        
        if "total_spikes" in stats and "total_neurons" in stats:
            efficiency = stats["total_spikes"] / max(stats["total_neurons"], 1)
            performance_metrics.setdefault("efficiency_scores", []).append(efficiency)
    
    if performance_metrics:
        return {
            "mean_efficiency": sum(performance_metrics["efficiency_scores"]) / len(performance_metrics["efficiency_scores"]),
            "max_efficiency": max(performance_metrics["efficiency_scores"]),
            "min_efficiency": min(performance_metrics["efficiency_scores"]),
        }
    
    return {}
def main():
    """Main function."""
    parser = create_argument_parser()
    args = parser.parse_args()
    
    # Print banner
    print("=" * 80)
    print("NLM Research Script - Advanced NLM Simulation Research")
    print("=" * 80)
    print()
    
    # Validate NLM availability
    if not NLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        sys.exit(1)
    
    try:
        # Run research experiment based on type
        print(f"\nStarting {args.experiment} experiment...")
        
        if args.experiment == "learning":
            results = create_learning_experiment()
        elif args.experiment == "plasticity":
            results = create_plasticity_experiment()
        elif args.experiment == "comparison":
            results = create_comparison_experiment()
        elif args.experiment == "neural_network":
            results = create_neural_network_experiment()
        elif args.experiment == "timing":
            results = create_timing_experiment()
        elif args.experiment == "development":
            results = create_development_experiment()
        elif args.experiment == "social":
            results = create_social_experiment()
        elif args.experiment == "custom":
            results = create_custom_experiment()
        else:
            raise ValueError(f"Unknown experiment type: {args.experiment}")
        
        # Save results if requested
        if args.output:
            save_results(results, args.output)
        
        # Create analysis report if requested
        if args.analyze:
            analysis_file = args.output.replace('.json', '_analysis.json') if args.output else "research_analysis.json"
            create_analysis_report(results, analysis_file)
        
        # Print summary
        print("\n" + "=" * 80)
        print("Research experiment completed successfully!")
        print("=" * 80)
        
        if not args.quiet:
            print(f"\nExperiment Summary:")
            print(f"  Type: {args.experiment}")
            print(f"  Steps: {args.steps}")
            print(f"  Neurons: {args.neurons}")
            
            if isinstance(results, dict):
                print(f"  Results: {len(results)} experiments")
                
                for name, result in results.items():
                    print(f"    - {name}")
            else:
                print(f"  Results: Single experiment completed")
            
            if args.output:
                print(f"  Results saved to: {args.output}")
            
            if args.analyze:
                print(f"  Analysis report generated")
        
        print("\nResearch experiment completed successfully!")
        
    except Exception as e:
        print(f"\nError during research experiment: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()
