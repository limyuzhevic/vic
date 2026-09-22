# NLM Python Library Utilities

"""Utility functions and advanced commands for the NLM Python API

This module provides enhanced functionality and convenience functions
for working with NLM brains, agents, and worlds in Python.
"""

import time
import json
import os
from pathlib import Path
from typing import Optional, Dict, List, Callable, Any
from datetime import datetime

import pynlm


def create_experiment_setup(
    brain_config: Optional[pynlm.Config] = None,
    world_width: float = 20.0,
    world_height: float = 20.0,
    vision_width: int = 16,
    vision_height: int = 16,
    seed: Optional[int] = None
) -> Dict[str, Any]:
    """Create a complete experiment setup with brain, world, and agent.
    
    This utility function sets up a complete NLM experiment environment,
    which is especially helpful for beginners and for running systematic
    tests.
    
    Args:
        brain_config: Optional brain configuration. If None, uses default.
        world_width: World width in meters.
        world_height: World height in meters.
        vision_width: Vision width in pixels.
        vision_height: Vision height in pixels.
        seed: Random seed for reproducibility.
        
    Returns:
        Dictionary containing 'brain', 'world', 'agent', and 'config'
        keys for easy access in experiments.
        
    Example:
        >>> setup = create_experiment_setup(world_width=30.0, world_height=30.0)
        >>> brain, world, agent = setup['brain'], setup['world'], setup['agent']
        >>> run_agent_simulation(brain, world, agent, 1000)
    """
    # Set random seed if provided
    if seed is not None:
        pynlm.set_random_seed(seed)
    
    # Create configuration
    if brain_config is None:
        brain_config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(world_width, world_height, vision_width, vision_height)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': brain_config,
        'seed': seed
    }


def run_agent_simulation(
    brain: pynlm.Brain,
    world: pynlm.SimpleWorld,
    agent: pynlm.AgentBrain,
    num_steps: int,
    enable_learning: bool = True,
    print_progress: bool = False,
    log_interval: int = 100
) -> Dict[str, Any]:
    """Run a complete agent simulation with brain-world interaction.
    
    This is a high-level utility that handles the complete simulation loop,
    including sensory processing, brain computation, action selection,
    and reward application. This abstracts away the low-level details and
    makes it easy to run complex simulations.
    
    Args:
        brain: The brain instance.
        world: The world instance.
        agent: The agent interface connecting brain to world.
        num_steps: Number of simulation steps to run.
        enable_learning: Whether to enable reward-based learning.
        print_progress: Whether to print progress messages.
        log_interval: Interval for logging progress.
        
    Returns:
        Dictionary containing simulation metrics:
        - 'total_reward': Total reward accumulated
        - 'avg_firing_rate': Average firing rate
        - 'final_energy': Final energy level
        - 'curiosity_level': Final curiosity level
        - 'novelty_level': Final novelty level
        - 'steps_completed': Number of steps actually completed
        
    Example:
        >>> setup = create_experiment_setup()
        >>> metrics = run_agent_simulation(
        ...     setup['brain'], setup['world'], setup['agent'],
        ...     num_steps=5000, enable_learning=True, print_progress=True
        ... )
        >>> print(f"Total reward: {metrics['total_reward']:.2f}")
    """
    # Enable learning features if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
    
    # Run simulation
    total_reward = 0.0
    total_firing_rate = 0.0
    steps_with_firing = 0
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Accumulate metrics
        total_reward += reward
        total_firing_rate += brain.getAverageFiringRate()
        if brain.getFiringNeuronCount() > 0:
            steps_with_firing += 1
        
        # Print progress
        if print_progress and step % log_interval == 0:
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Energy={world.getAgentBody().energy:.1f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Calculate metrics
    avg_firing_rate = total_firing_rate / num_steps if num_steps > 0 else 0.0
    
    return {
        'total_reward': total_reward,
        'avg_firing_rate': avg_firing_rate,
        'final_energy': world.getAgentBody().energy,
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'steps_completed': num_steps,
        'steps_with_firing': steps_with_firing
    }


def save_experiment_results(
    results: Dict[str, Any],
    filepath: str,
    brain: Optional[pynlm.Brain] = None,
    world: Optional[pynlm.SimpleWorld] = None,
    agent: Optional[pynlm.AgentBrain] = None
) -> bool:
    """Save experiment results to a file with metadata.
    
    This utility saves simulation results along with brain state,
    configuration, and timestamps for reproducibility and analysis.
    
    Args:
        results: Results dictionary from run_agent_simulation().
        filepath: Path to save the results (JSON format).
        brain: Optional brain instance to save state.
        world: Optional world instance.
        agent: Optional agent instance.
        
    Returns:
        True if successful, False otherwise.
        
    Example:
        >>> setup = create_experiment_setup()
        >>> metrics = run_agent_simulation(setup['brain'], setup['world'], setup['agent'], 1000)
        >>> save_experiment_results(metrics, "results.json", setup['brain'], setup['world'], setup['agent'])
    """
    try:
        # Create directory if it doesn't exist
        Path(filepath).parent.mkdir(parents=True, exist_ok=True)
        
        # Prepare metadata
        metadata = {
            'timestamp': datetime.now().isoformat(),
            'python_version': '3.8+',  # Update with actual version
            'pynlm_version': pynlm.__version__,
            'results': results
        }
        
        # Add brain state if provided
        if brain is not None:
            metadata['brain_state'] = {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            }
        
        # Add world state if provided
        if world is not None:
            metadata['world_state'] = {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'agent_x': world.getAgentBody().x,
                'agent_y': world.getAgentBody().y,
                'agent_energy': world.getAgentBody().energy,
                'agent_health': world.getAgentBody().health
            }
        
        # Add agent state if provided
        if agent is not None:
            metadata['agent_state'] = {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'development_stage': agent.getDevelopmentalStage()
            }
        
        # Save to file
        with open(filepath, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        print(f"Results saved to {filepath}")
        
        return True
        
    except Exception as e:
        print(f"Error saving results: {e}")
        return False


def print_experiment_summary(
    results: Dict[str, Any],
    brain: Optional[pynlm.Brain] = None,
    world: Optional[pynlm.SimpleWorld] = None,
    agent: Optional[pynlm.AgentBrain] = None,
    show_detailed: bool = False
):
    """Print a formatted summary of experiment results.
    
    This utility provides a clean, readable summary of experiment results
    with optional detailed information. This is useful for quickly
    reviewing experiment outcomes.
    
    Args:
        results: Results dictionary from run_agent_simulation().
        brain: Optional brain instance for detailed info.
        world: Optional world instance for detailed info.
        agent: Optional agent instance for detailed info.
        show_detailed: Whether to show detailed information.
        
    Example:
        >>> setup = create_experiment_setup()
        >>> metrics = run_agent_simulation(setup['brain'], setup['world'], setup['agent'], 1000)
        >>> print_experiment_summary(metrics, setup['brain'], setup['world'], setup['agent'])
    """
    print("=" * 60)
    print("NLM EXPERIMENT SUMMARY")
    print("=" * 60)
    
    print(f"Total Reward:           {results['total_reward']:8.2f}")
    print(f"Average Firing Rate:    {results['avg_firing_rate']:8.3f} Hz")
    print(f"Final Energy:            {results['final_energy']:8.1f}")
    print(f"Curiosity Level:        {results['curiosity_level']:8.3f}")
    print(f"Novelty Level:           {results['novelty_level']:8.3f}")
    print(f"Steps Completed:        {results['steps_completed']:8d}")
    print(f"Steps with Activity:    {results['steps_with_firing']:8d}")
    
    if show_detailed:
        print("\n" + "-" * 60)
        print("DETAILED INFORMATION")
        print("-" * 60)
        
        if brain:
            print(f"Brain State:")
            print(f"  Neurons:              {brain.getTotalNeuronCount():8d}")
            print(f"  Synapses:             {brain.getTotalSynapseCount():8d}")
            print(f"  Total Spikes:         {brain.getTotalSpikeCount():8d}")
            print(f"  Active Neurons:       {brain.getActiveNeuronCount():8d}")
            print(f"  Firing Neurons:       {brain.getFiringNeuronCount():8d}")
            print(f"  E/I Ratio:            {brain.getExcitationInhibitionRatio():8.3f}")
        
        if world:
            print(f"\nWorld State:")
            print(f"  Width:                {world.getWidth():8.1f} m")
            print(f"  Height:               {world.getHeight():8.1f} m")
            print(f"  Simulation Time:      {world.getSimulationTime():8.2f} s")
            print(f"  Agent Position:       ({world.getAgentBody().x:5.2f}, {world.getAgentBody().y:5.2f})")
            print(f"  Agent Energy:         {world.getAgentBody().energy:8.1f}")
            print(f"  Agent Health:         {world.getAgentBody().health:8.2f}")
        
        if agent:
            print(f"\nAgent State:")
            print(f"  Curiosity:            {agent.getCuriosityLevel():8.3f}")
            print(f"  Novelty:              {agent.getNoveltyLevel():8.3f}")
            print(f"  Neuromodulation:      {agent.getNeuromodulationLevel():8.3f}")
            print(f"  Prediction Error:    {agent.getPredictionError():8.3f}")
            print(f"  Development Stage:   {agent.getDevelopmentalStage():8d}")
    
    print("=" * 60)


def set_random_seed(seed: int):
    """Set random seed for reproducible experiments.
    
    This utility function sets the random seed for both the simulation
    and any random number generators, ensuring reproducible results.
    
    Args:
        seed: Integer seed value.
        
    Example:
        >>> set_random_seed(42)
        >>> setup = create_experiment_setup()
        >>> metrics1 = run_agent_simulation(...)
        >>> setup2 = create_experiment_setup()
        >>> metrics2 = run_agent_simulation(...)
        # metrics1 and metrics2 should be identical
    """
    pynlm.set_random_seed(seed)
