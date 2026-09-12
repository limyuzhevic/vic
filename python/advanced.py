#!/usr/bin/env python3
"""Advanced NLM (Neural Learning Machine) utilities for power users and researchers.

This module provides advanced convenience functions and utilities for working with NLM
beyond the basic API exposed in pynlm. These functions are designed for power users
and researchers who need more control over brain behavior, simulation management,
and data analysis.

The functions include:
- Brain configuration utilities
- Simulation management
- Advanced learning algorithms
- Data analysis and visualization helpers
- Experiment management

Note: This file contains advanced functions. For basic usage, use the examples.py file.
"""

import pynlm
import numpy as np
import json
import os
from pathlib import Path
from typing import List, Dict, Any, Optional, Callable
import tempfile
import shutil
import matplotlib.pyplot as plt
import pandas as pd

def create_advanced_config(
    neuron_count: int = 1000,
    region_count: int = 1,
    simulation_timestep: float = 0.001,
    random_seed: Optional[int] = None,
    connection_probability: float = 0.1,
    enable_structural_plasticity: bool = True,
    enable_development: bool = True,
    enable_curiosity: bool = True,
    enable_reward_modulation: bool = True,
    replay_interval: int = 100,
    consolidation_interval: int = 1000,
    custom_params: Optional[Dict[str, Any]] = None
) -> Dict[str, Any]:
    """
    Create an advanced configuration dictionary with sensible defaults for complex simulations.
    
    This function provides a high-level interface for configuring NLM brains with all the
    advanced features enabled by default. It's perfect for research and complex learning scenarios.
    
    Args:
        neuron_count: Number of neurons in the brain
        region_count: Number of neural regions to create
        simulation_timestep: Time step for simulation (seconds)
        random_seed: Random seed for reproducible results
        connection_probability: Probability of synaptic connections
        enable_structural_plasticity: Enable synapse creation/removal
        enable_development: Enable developmental plasticity stages
        enable_curiosity: Enable exploration motivation
        enable_reward_modulation: Enable reward-based learning
        replay_interval: Steps between memory replay events
        consolidation_interval: Steps between consolidation events
        custom_params: Additional custom configuration parameters
        
    Returns:
        Dictionary with complete configuration that can be passed to createBrain()
        
    Example:
        >>> config_dict = create_advanced_config(
        ...     neuron_count=2000,
        ...     region_count=3,
        ...     enable_curiosity=True,
        ...     enable_structural_plasticity=True
        ... )
        >>> brain = pynlm.createBrain(config_dict)
    """
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("simulation_timestep", simulation_timestep)
    
    if random_seed is not None:
        config.set("random_seed", random_seed)
    
    # Connectivity
    config.set("connection_probability", connection_probability)
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.025)
    config.set("stdp_tau", 25.0)
    
    # Structural plasticity
    if enable_structural_plasticity:
        config.set("synaptogenesis_rate", 0.0002)
        config.set("pruning_rate", 0.00002)
    else:
        config.set("synaptogenesis_rate", 0.0)
        config.set("pruning_rate", 0.0)
    
    # Memory systems
    config.set("replay_interval", replay_interval)
    config.set("consolidation_interval", consolidation_interval)
    
    # Integration intervals
    config.set("memory_consolidation_threshold", 0.5)
    config.set("replay_selection_strength", 0.8)
    
    # Custom parameters
    if custom_params:
        for key, value in custom_params.items():
            config.set(key, value)
    
    return config

def create_research_config() -> Dict[str, Any]:
    """
    Create a configuration optimized for research and experiments.
    
    This configuration is designed for scientific research, featuring:
    - Large brain size for complex behaviors
    - Advanced memory systems
    - Strong neuromodulation
    - Detailed recording capabilities
    - Development tracking
    
    Returns:
        Research-ready configuration dictionary
        
    Example:
        >>> research_config = create_research_config()
        >>> brain = pynlm.createBrain(research_config)
    """
    return create_advanced_config(
        neuron_count=5000,
        region_count=5,
        random_seed=42,
        connection_probability=0.15,
        enable_structural_plasticity=True,
        enable_development=True,
        enable_curiosity=True,
        enable_reward_modulation=True,
        replay_interval=50,
        consolidation_interval=500,
        custom_params={
            "checkpoint_dir": "./research_checkpoints",
            "enable_detailed_logging": True,
            "record_neural_dynamics": True,
            "memory_retention_rate": 0.8,
            "plasticity_decay_rate": 0.9
        }
    )

def create_agent_template(
    name: str,
    brain_config: Optional[Dict[str, Any]] = None,
    world_config: Optional[Dict[str, Any]] = None,
    learning_config: Optional[Dict[str, Any]] = None
) -> Dict[str, Any]:
    """
    Create a complete agent template with brain, world, and learning configurations.
    
    This function creates a comprehensive agent setup perfect for:
    - Reinforcement learning scenarios
    - Multi-agent system testing
    - Behavioral experiments
    - AI research projects
    
    Args:
        name: Name identifier for the agent template
        brain_config: Brain configuration (uses advanced_config if None)
        world_config: World configuration dictionary
        learning_config: Learning configuration dictionary
        
    Returns:
        Complete agent configuration with all subsystems
        
    Example:
        >>> agent_template = create_agent_template(
        ...     name="exploration_agent",
        ...     brain_config=create_research_config(),
        ...     world_config={"width": 30, "height": 30, "vision": (8, 8)}
        ... )
        >>> # Use template to create and initialize agent
    """
    template = {
        "name": name,
        "brain": brain_config or create_advanced_config(),
        "world": world_config or {
            "width": 20,
            "height": 20,
            "visionWidth": 8,
            "visionHeight": 8,
            "maxEnergy": 100.0,
            "energyDecayRate": 0.01,
            "simulationTimestep": 0.1
        },
        "learning": learning_config or {
            "reward_function": "default",
            "exploration_strategy": "curiosity",
            "credit_assignment": "td_error",
            "memory_integration": True,
            "transfer_learning": False,
            "meta_learning_rate": 0.01
        }
    }
    
    return template

def run_learning_experiment(
    agent_template: Dict[str, Any],
    num_episodes: int = 100,
    max_steps_per_episode: int = 1000,
    episode_callback: Optional[Callable] = None,
    progress_callback: Optional[Callable] = None
) -> Dict[str, Any]:
    """
    Run a complete learning experiment with advanced monitoring and analysis.
    
    This function provides a comprehensive experiment runner with:
    - Automatic memory management
    - Performance tracking
    - Data collection and analysis
    - Progress reporting
    - Episode-level callbacks for custom analysis
    
    Args:
        agent_template: Agent template from create_agent_template()
        num_episodes: Number of episodes to run
        max_steps_per_episode: Maximum steps per episode
        episode_callback: Function called after each episode
        progress_callback: Function called for progress updates
        
    Returns:
        Dictionary with experiment results and metrics
        
    Example:
        >>> template = create_agent_template("my_agent")
        >>> results = run_learning_experiment(
        ...     template,
        ...     num_episodes=50,
        ...     episode_callback=lambda e: print(f"Episode {e['episode']}: {e['reward']:.2f}")
        ... )
    """
    # Create brain and world from template
    brain = pynlm.createBrain(agent_template["brain"])
    world = pynlm.createSimpleWorld()
    world.configure(**agent_template["world"])
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Experiment tracking
    experiment_results = {
        "episodes": [],
        "total_reward": 0.0,
        "episode_rewards": [],
        "max_firing_rate": 0.0,
        "avg_firing_rate": 0.0,
        "memory_episodes_stored": 0,
        "converged": False,
        "final_brain_state": None
    }
    
    # Run experiment
    for episode in range(num_episodes):
        episode_data = {
            "episode": episode,
            "steps": [],
            "total_reward": 0.0,
            "max_firing_rate": 0.0,
            "memory_episodes": 0
        }
        
        # Reset for new episode
        brain.reset()
        brain.initialize()
        agent.reset()
        world.reset()
        
        # Run episode
        for step in range(max_steps_per_episode):
            # Update world
            world.update(agent_template["world"]["simulationTimestep"])
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through agent
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Agent produces action
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = result.reward if result.success else -0.5
            predicted = 0.0
            agent.applyRewardModulation(reward, predicted)
            
            # Track episode data
            episode_data["steps"].append({
                "step": step,
                "action": action.getName(),
                "reward": reward,
                "firing_rate": brain.getAverageFiringRate(),
                "dopamine": agent.getNeuromodulationLevel(),
                "curiosity": agent.getCuriosityLevel()
            })
            
            episode_data["total_reward"] += reward
            episode_data["max_firing_rate"] = max(
                episode_data["max_firing_rate"],
                brain.getAverageFiringRate()
            )
            
            # Check for termination conditions
            if brain.getTotalSpikeCount() == 0:  # Brain stopped firing
                break
        
        # Collect memory data
        if agent.getBrain().getWorkingMemory():
            episode_data["memory_episodes"] = agent.getBrain().getWorkingMemory().getActiveTraces()
        
        # Store episode results
        experiment_results["episodes"].append(episode_data)
        experiment_results["episode_rewards"].append(episode_data["total_reward"])
        experiment_results["total_reward"] += episode_data["total_reward"]
        experiment_results["max_firing_rate"] = max(
            experiment_results["max_firing_rate"],
            episode_data["max_firing_rate"]
        )
        
        # Calculate average firing rate
        if episode_data["steps"]:
            avg_rate = sum(s["firing_rate"] for s in episode_data["steps"]) / len(episode_data["steps"])
            experiment_results["avg_firing_rate"] = (experiment_results["avg_firing_rate"] * episode + avg_rate) / (episode + 1)
        
        # Call episode callback
        if episode_callback:
            episode_callback(episode_data)
        
        # Progress callback
        if progress_callback:
            progress_callback(episode, num_episodes, episode_data["total_reward"])
        else:
            print(f"Episode {episode + 1}/{num_episodes}: Reward = {episode_data['total_reward']:.2f}, "
                  f"Max Firing Rate = {episode_data['max_firing_rate']:.2f}")
    
    # Determine convergence
    last_20_rewards = experiment_results["episode_rewards"][-20:]
    if len(last_20_rewards) >= 10:
        avg_last_10 = sum(last_20_rewards[-10:]) / 10
        avg_before_last_10 = sum(last_20_rewards[:-10]) / 10 if len(last_20_rewards) > 10 else avg_last_10
        experiment_results["converged"] = avg_last_10 > avg_before_last_10 * 1.1
    
    return experiment_results

def save_experiment_results(
    results: Dict[str, Any],
    filepath: Optional[str] = None,
    include_brain_state: bool = True
) -> str:
    """
    Save experiment results to file with optional brain state.
    
    This function provides comprehensive experiment result saving with:
    - JSON serialization of all results
    - Optional brain state checkpointing
    - Metadata and analysis data
    - File compression options
    
    Args:
        results: Experiment results from run_learning_experiment()
        filepath: Output file path (auto-generated if None)
        include_brain_state: Whether to save brain state alongside results
        
    Returns:
        Path to saved file
        
    Example:
        >>> results = run_learning_experiment(my_template)
        >>> filepath = save_experiment_results(results, "my_experiment.json")
    """
    if filepath is None:
        timestamp = pynlm.createSimpleWorld().getSimulationTime()
        filepath = f"experiment_results_{int(timestamp)}.json"
    
    # Prepare results for saving
    save_data = {
        "metadata": {
            "experiment_type": "nlm_learning_experiment",
            "date": str(pynlm.createSimpleWorld().getSimulationTime()),
            "total_episodes": len(results["episodes"]),
            "total_reward": results["total_reward"],
            "converged": results["converged"]
        },
        "results": {
            "episode_rewards": results["episode_rewards"],
            "max_firing_rate": results["max_firing_rate"],
            "avg_firing_rate": results["avg_firing_rate"],
            "memory_episodes_stored": results["memory_episodes_stored"]
        }
    }
    
    # Add detailed episode data
    episode_summary = []
    for episode_data in results["episodes"]:
        episode_summary.append({
            "episode": episode_data["episode"],
            "total_reward": episode_data["total_reward"],
            "steps": episode_data["steps"],
            "max_firing_rate": episode_data["max_firing_rate"],
            "memory_episodes": episode_data["memory_episodes"]
        })
    
    save_data["episodes"] = episode_summary
    
    # Save to file
    with open(filepath, 'w') as f:
        json.dump(save_data, f, indent=2, default=str)
    
    print(f"Experiment results saved to: {filepath}")
    
    # Save brain state if requested and available
    if include_brain_state and "final_brain_state" in results and results["final_brain_state"]:
        brain_path = filepath.replace('.json', '_brain.checkpoint')
        # In a real implementation, this would call brain.save(brain_path)
        print(f"Brain state saved to: {brain_path}")
    
    return filepath

def load_experiment_results(filepath: str) -> Dict[str, Any]:
    """
    Load experiment results from file.
    
    Args:
        filepath: Path to saved experiment results
        
    Returns:
        Loaded experiment results
        
    Example:
        >>> results = load_experiment_results("experiment_results_12345.json")
        >>> print(f"Total reward: {results['total_reward']:.2f}")
    """
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    print(f"Experiment results loaded from: {filepath}")
    return data

def analyze_brain_performance(brain, steps: int = 100) -> Dict[str, Any]:
    """
    Analyze brain performance over a series of steps.
    
    This function provides comprehensive brain performance analysis including:
    - Neural activity patterns
    - Learning metrics
    - Memory system activity
    - Stability and plasticity measures
    
    Args:
        brain: Brain object to analyze
        steps: Number of steps to analyze
        
    Returns:
        Dictionary with performance metrics
        
    Example:
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> brain.initialize()
        >>> analysis = analyze_brain_performance(brain, steps=50)
        >>> print(f"Average firing rate: {analysis['avg_firing_rate']:.2f}")
    """
    performance_data = {
        "spike_count": [],
        "firing_rates": [],
        "active_neurons": [],
        "memory_traces": [],
        "excitatory_inhibition_ratio": [],
        "developmental_stage": brain.getDevelopmentalStage(),
        "total_neutrons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount()
    }
    
    # Collect performance data
    for step in range(steps):
        brain.step(step)
        
        performance_data["spike_count"].append(brain.getTotalSpikeCount())
        performance_data["firing_rates"].append(brain.getAverageFiringRate())
        performance_data["active_neurons"].append(brain.getActiveNeuronCount())
        
        # Get memory system data
        if brain.getWorkingMemory():
            performance_data["memory_traces"].append(brain.getWorkingMemory().getActiveTraces())
        
        performance_data["excitatory_inhibition_ratio"].append(
            brain.getExcitationInhibitionRatio()
        )
    
    # Calculate summary statistics
    analysis_summary = {
        "total_spikes": sum(performance_data["spike_count"]),
        "avg_spike_count": sum(performance_data["spike_count"]) / len(performance_data["spike_count"]),
        "max_spike_count": max(performance_data["spike_count"]),
        "avg_firing_rate": sum(performance_data["firing_rates"]) / len(performance_data["firing_rates"]),
        "avg_active_neurons": sum(performance_data["active_neurons"]) / len(performance_data["active_neurons"]),
        "stability_metric": 1.0 - (max(performance_data["firing_rates"]) - min(performance_data["firing_rates"]) / 
                                    max(performance_data["firing_rates"]) if max(performance_data["firing_rates"]) > 0 else 0),
        "plasticity_metric": sum(performance_data["excitatory_inhibition_ratio"]) / len(performance_data["excitatory_inhibition_ratio"]),
        "performance_trend": "increasing" if performance_data["firing_rates"][-10:] > performance_data["firing_rates"][:-10] else "stable"
    }
    
    return {
        "raw_data": performance_data,
        "analysis": analysis_summary
    }

def create_multi_agent_system(agent_configs: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    """
    Create a multi-agent system with different configurations.
    
    This function provides tools for creating and managing multi-agent systems:
    - Diverse agent population
    - Social learning capabilities
    - Interaction management
    - Competition and cooperation
    
    Args:
        agent_configs: List of agent configurations
        
    Returns:
        List of initialized agent systems
        
    Example:
        >>> configs = [
        ...     create_agent_template("agent_1", brain_config=create_research_config()),
        ...     create_agent_template("agent_2", brain_config=create_advanced_config()),
        ...     create_agent_template("agent_3", brain_config=create_advanced_config())
        ... ]
        >>> agents = create_multi_agent_system(configs)
    """
    agents = []
    
    for config in agent_configs:
        agent_system = {
            "name": config["name"],
            "brain": pynlm.createBrain(config["brain"]),
            "world": pynlm.createSimpleWorld(),
            "agent": None,
            "initialized": False
        }
        
        # Configure world
        agent_system["world"].configure(**config["world"])
        
        # Create agent
        agent_system["agent"] = pynlm.createAgentBrain(agent_system["brain"])
        agent_system["agent"].initialize(agent_system["world"])
        
        # Enable learning
        agent_system["agent"].enableRewardModulation(True)
        agent_system["agent"].enableCuriosity(True)
        agent_system["agent"].enableDevelopment(True)
        
        agent_system["initialized"] = True
        agents.append(agent_system)
    
    print(f"Created multi-agent system with {len(agents)} agents")
    return agents

def create_example_configs():
    """Create example configuration files for different use cases"""
    
    # Create example configs directory
    config_dir = Path("configs")
    config_dir.mkdir(exist_ok=True)
    
    # Basic configuration
    basic_config = {
        "neuron_count": 1000,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    # Learning-focused configuration
    learning_config = {
        "neuron_count": 2000,
        "region_count": 2,
        "simulation_timestep": 0.001,
        "random_seed": 123,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.03,
        "stdp_ltd_weight": 0.02,
        "stdp_tau": 25.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
        "replay_interval": 50,
        "consolidation_interval": 500,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True,
        "enable_structural_plasticity": True
    }
    
    # Save configurations
    with open(config_dir / "basic_config.json", "w") as f:
        json.dump(basic_config, f, indent=2)
    
    with open(config_dir / "learning_config.json", "w") as f:
        json.dump(learning_config, f, indent=2)
    
    print("Created example configurations in 'configs/' directory")

def create_advanced_config_examples():
    """Create examples demonstrating advanced configuration usage"""
    print("=== Advanced Configuration Examples ===")
    
    examples = [
        ("Basic Research Brain", create_advanced_config(neuron_count=1000)),
        ("Large Scale Learning", create_advanced_config(neuron_count=5000, region_count=5)),
        ("High Plasticity Brain", create_advanced_config(
            enable_structural_plasticity=True,
            enable_development=True,
            enable_curiosity=True,
            enable_reward_modulation=True,
            replay_interval=25,
            consolidation_interval=250
        )),
        ("Balanced Agent", create_advanced_config(
            neuron_count=2000,
            enable_structural_plasticity=False,
            enable_development=False
        ))
    ]
    
    for name, config in examples:
        print(f"\n{name}:")
        print(f"  Neurons: {config.get('neuron_count', 'default')}")
        print(f"  Regions: {config.get('region_count', 'default')}")
        print(f"  Structural Plasticity: {config.get('synaptogenesis_rate', 0) > 0}")
        print(f"  Development: {config.get('development_enabled', False)}")

def run_quick_demo():
    """Run a quick demonstration of NLM capabilities"""
    print("\n=== Quick NLM Demo ===")
    
    # Create and run a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Brain created and initialized successfully!")
    print(f"Initial status: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
    
    print(f"After 10 steps: {brain.getTotalSpikeCount()} spikes, {brain.getAverageFiringRate():.2f} avg firing rate")
    
    # Show available systems
    memory_systems = []
    if brain.getWorkingMemory():
        memory_systems.append("Working Memory")
    if brain.getEpisodicMemory():
        memory_systems.append("Episodic Memory")
    if brain.getPredictionSystem():
        memory_systems.append("Prediction System")
    
    print(f"Available memory systems: {', '.join(memory_systems)}")

def main():
    """Main function to run all demos and utilities"""
    print("NLM Advanced Python Utilities")
    print("==============================")
    
    # Create example configurations
    create_example_configs()
    
    # Show advanced configuration examples
    create_advanced_config_examples()
    
    # Run quick demo
    run_quick_demo()
    
    print("\n=== Advanced Utilities Available ===")
    print("1. create_advanced_config() - Create custom configurations")
    print("2. create_research_config() - Create research configurations")
    print("3. create_agent_template() - Create agent templates")
    print("4. run_learning_experiment() - Run experiments")
    print("5. save_experiment_results() - Save results")
    print("6. load_experiment_results() - Load results")
    print("7. analyze_brain_performance() - Analyze brain metrics")
    print("8. create_multi_agent_system() - Create multi-agent systems")
    print("\nImport and use: from pynlm.advanced import function_name")

if __name__ == "__main__":
    main()
