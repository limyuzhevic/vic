#!/usr/bin/env python3
"""
NLM High-Level API Convenience Functions

This module provides high-level convenience functions for the NLM (Neural Learning Machine)
framework, simplifying common operations shown in the documentation examples.

The functions follow Pythonic naming conventions, provide sensible defaults,
handle common error cases, and offer both simple and advanced usage options.
"""

import os
import json
import time
from datetime import datetime
from typing import Dict, Any, Optional, Union

from pathlib import Path

# Import the pynlm module
import pynlm
def create_agent_brain_with_config(config_dict: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a brain with configuration in one step.
    
    This convenience function combines the manual steps of creating a config
    and then creating a brain, making it easier for beginners.
    
    Args:
        config_dict: Optional dictionary with configuration values.
                    If None, uses default configuration.
    
    Returns:
        Initialized brain object
    
    Examples:
        >>> # Simple usage with defaults
        >>> brain = create_agent_brain_with_config()
        >>> 
        >>> # Advanced usage with custom config
        >>> config = {
        ...     "brain": {
        ...         "neuron_count": 1000,
        ...         "synapse_density": 0.1
        ...     }
        ... }
        >>> brain = create_agent_brain_with_config(config)
    """
    try:
        # Create config from dictionary
        pynlm_config = pynlm.createDefaultConfig()
        
        if config_dict:
            # Flatten nested dictionary for pynlm config
            for section, values in config_dict.items():
                if isinstance(values, dict):
                    for key, value in values.items():
                        full_key = f"{section}.{key}"
                        pynlm_config.set(full_key, value)
                else:
                    pynlm_config.set(section, values)
        
        # Create and initialize brain
        brain = pynlm.createBrain(pynlm_config)
        brain.initialize()
        
        return brain
        
    except Exception as e:
        raise pynlm.NLMError(f"Failed to create agent brain with config: {e}")


def run_simulation_loop(brain: Any, world: Any, agent: Any, steps: int, verbose: bool = False) -> Dict[str, Any]:
    """
    Run a complete simulation loop with brain, world, and agent.
    
    This simplifies the repetitive simulation code pattern shown in examples,
    handling the common workflow of updating world, processing sensory input,
    running brain step, and applying motor commands.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of simulation steps to run
        verbose: Enable verbose output during simulation
    
    Returns:
        Dictionary with simulation results including statistics
    
    Examples:
        >>> brain = create_agent_brain_with_config()
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> results = run_simulation_loop(brain, world, agent, 1000, verbose=True)
        >>> print(f"Simulation completed: {results['steps']} steps")
    """
    results = {
        'steps': steps,
        'initial_spikes': brain.getTotalSpikeCount(),
        'initial_firing': brain.getFiringNeuronCount(),
        'final_spikes': 0,
        'final_firing': 0,
        'average_firing_rate': 0.0,
        'max_firing_rate': 0.0,
        'total_time': 0.0
    }
    
    start_time = time.time()
    
    try:
        for step in range(steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept and hasattr(percept, 'getInternal'):
                internal_data = percept.getInternal()
                if internal_data:
                    reward = internal_data[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            if verbose and step % 100 == 0:
                firing = brain.getFiringNeuronCount()
                print(f"Step {step}: {firing} firing neurons")
        
        # Collect final results
        results['final_spikes'] = brain.getTotalSpikeCount() - results['initial_spikes']
        results['final_firing'] = brain.getFiringNeuronCount()
        results['average_firing_rate'] = brain.getAverageFiringRate()
        
        # Calculate max firing rate (rough approximation)
        results['max_firing_rate'] = brain.getFiringNeuronCount()
        
        results['total_time'] = time.time() - start_time
        results['success'] = True
        
        return results
        
    except Exception as e:
        results['error'] = str(e)
        results['success'] = False
        raise pynlm.NLMError(f"Simulation loop failed: {e}")


def run_agent_simulation(config: Optional[Dict[str, Any]] = None, 
                         world_config: Optional[Dict[str, Any]] = None,
                         steps: int = 1000,
                         enable_features: bool = True) -> Dict[str, Any]:
    """
    Run a complete agent simulation with all configuration and setup.
    
    This is the high-level function that runs the complete example from the
    documentation, handling all the setup steps automatically.
    
    Args:
        config: Optional brain configuration dictionary
        world_config: Optional world configuration dictionary
        steps: Number of simulation steps (default: 1000)
        enable_features: Enable agent features like reward modulation,
                       structural plasticity, development, and curiosity
    
    Returns:
        Dictionary with simulation results and final state
    
    Examples:
        >>> # Basic usage
        >>> results = run_agent_simulation(steps=500)
        >>> 
        >>> # Advanced usage with custom config
        >>> config = {"brain": {"neuron_count": 2000}}
        >>> results = run_agent_simulation(config=config, steps=2000)
    """
    try:
        # 1. Create and configure brain
        brain = create_agent_brain_with_config(config)
        
        # 2. Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # 3. Create and configure world
        world = pynlm.createSimpleWorld()
        if world_config:
            for key, value in world_config.items():
                if hasattr(world, key):
                    setattr(world, key, value)
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # 4. Initialize agent with world
        agent.initialize(world)
        
        # 5. Enable subsystems if requested
        if enable_features:
            enable_brain_features(agent, reward_modulation=True, 
                                structural_plasticity=True, development=True,
                                curiosity=True)
        
        # 6. Run simulation loop
        results = run_simulation_loop(brain, world, agent, steps, verbose=True)
        
        # 7. Add final statistics
        results.update({
            'brain_config': config,
            'world_config': world_config,
            'enable_features': enable_features,
            'final_neuron_count': brain.getTotalNeuronCount(),
            'final_synapse_count': brain.getTotalSynapseCount(),
            'final_developmental_stage': brain.getDevelopmentalStage(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        })
        
        return results
        
    except Exception as e:
        raise pynlm.NLMError(f"Agent simulation failed: {e}")


def get_brain_stats(brain: Any) -> Dict[str, Any]:
    """
    Get formatted brain statistics as a dictionary.
    
    This provides a convenient way to access brain statistics in a structured
    format, making them easier to process programmatically.
    
    Args:
        brain: Brain object
    
    Returns:
        Dictionary containing brain statistics
    
    Examples:
        >>> brain = create_agent_brain_with_config()
        >>> stats = get_brain_stats(brain)
        >>> print(f"Neurons: {stats['neuron_count']}")
        >>> print(f"Firing rate: {stats['average_firing_rate']:.2f}")
    """
    try:
        stats = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'pending_spike_event_count': brain.getPendingSpikeEventCount(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'regions_count': brain.getRegionCount(),
            'region_ids': brain.getRegionIds()
        }
        
        return stats
        
    except Exception as e:
        raise pynlm.NLMError(f"Failed to get brain statistics: {e}")


def enable_brain_features(agent: Any, **features: bool) -> None:
    """
    Enable/disable brain subsystems in batch.
    
    This provides a convenient way to enable multiple features at once,
    avoiding repetitive enable/disable calls.
    
    Args:
        agent: AgentBrain object
        **features: Feature names and their enable/disable state:
            - reward_modulation: Reward modulation subsystem
            - structural_plasticity: Structural plasticity subsystem  
            - development: Development system
            - curiosity: Curiosity-driven exploration
    
    Examples:
        >>> agent = pynlm.createAgentBrain(brain)
        >>> 
        >>> # Enable all features
        >>> enable_brain_features(agent, 
        ...                       reward_modulation=True,
        ...                       structural_plasticity=True,
        ...                       development=True,
        ...                       curiosity=True)
        >>> 
        >>> # Disable specific features
        >>> enable_brain_features(agent, development=False, curiosity=False)
    """
    feature_mapping = {
        'reward_modulation': agent.enableRewardModulation,
        'structural_plasticity': agent.enableStructuralPlasticity,
        'development': agent.enableDevelopment,
        'curiosity': agent.enableCuriosity
    }
    
    for feature_name, setter_func in feature_mapping.items():
        if feature_name in features:
            setter_func(features[feature_name])
def save_brain_with_timestamp(brain: Any, filepath: Optional[str] = None) -> str:
    """
    Save brain state with automatic timestamp in filename.
    
    This provides a convenient way to save brain states with unique
    filenames that include timestamps for easy backup and organization.
    
    Args:
        brain: Brain object to save
        filepath: Optional output filepath. If None, uses timestamped default.
    
    Returns:
        The actual filepath where the brain state was saved
    
    Examples:
        >>> brain = create_agent_brain_with_config()
        >>> # Auto-generated filename with timestamp
        >>> filepath = save_brain_with_timestamp(brain)
        >>> print(f"Brain saved to: {filepath}")
        >>> 
        >>> # Custom filename
        >>> filepath = save_brain_with_timestamp(brain, "my_brain_state.bin")
    """
    if filepath is None:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filepath = f"brain_state_{timestamp}.bin"
    
    try:
        brain.save(filepath)
        return filepath
    except Exception as e:
        raise pynlm.NLMError(f"Failed to save brain with timestamp: {e}")


def load_brain_with_backup(brain: Any, filepath: str, backup_if_failed: bool = True) -> bool:
    """
    Load brain state with automatic backup on failure.
    
    This provides a safe way to load brain states with automatic backup
    creation if the load fails, preventing data loss.
    
    Args:
        brain: Brain object to load state into
        filepath: Path to brain state file
        backup_if_failed: Create backup of current brain state if load fails
    
    Returns:
        True if load successful, False if failed (and backup was created)
    
    Examples:
        >>> brain = create_agent_brain_with_config()
        >>> # Safe load with backup
        >>> success = load_brain_with_backup(brain, "checkpoint.bin")
        >>> if success:
        ...     print("Brain state loaded successfully")
        >>> else:
        ...     print("Brain state load failed, backup created")
    """
    if not os.path.exists(filepath):
        raise pynlm.NLMError(f"Brain state file not found: {filepath}")
    
    try:
        # Try to load
        brain.load(filepath)
        return True
        
    except Exception as e:
        if backup_if_failed:
            # Create backup before attempting recovery
            backup_file = filepath.replace('.bin', f'_backup_{datetime.now().strftime("%Y%m%d_%H%M%S")}.bin')
            try:
                brain.save(backup_file)
                print(f"Brain state backup created: {backup_file}")
            except Exception as backup_error:
                print(f"Warning: Could not create backup: {backup_error}")
        
        raise pynlm.NLMError(f"Failed to load brain state: {e}")


def configure_brain_from_dict(brain: Any, config_dict: Dict[str, Any]) -> None:
    """
    Configure brain from dictionary configuration.
    
    This provides a convenient way to apply configuration changes to an
    existing brain without recreating it.
    
    Args:
        brain: Brain object to configure
        config_dict: Configuration dictionary with nested sections
    
    Examples:
        >>> brain = create_agent_brain_with_config()
        >>> # Update configuration
        >>> config_updates = {
        ...     "brain": {"neuron_count": 2000},
        ...     "plasticity": {"stdp": {"enable": True}}
        ... }
        >>> configure_brain_from_dict(brain, config_updates)
    """
    try:
        config = brain.getConfig()
        
        for section, values in config_dict.items():
            if isinstance(values, dict):
                for key, value in values.items():
                    full_key = f"{section}.{key}"
                    config.set(full_key, value)
            else:
                config.set(section, values)
                
    except Exception as e:
        raise pynlm.NLMError(f"Failed to configure brain from dict: {e}")


def create_world_from_config(world_config_dict: Dict[str, Any]) -> Any:
    """
    Create and configure a world from dictionary configuration.
    
    This provides a convenient way to create worlds with specific
    configuration without manual setup.
    
    Args:
        world_config_dict: World configuration dictionary
    
    Returns:
        Configured world object
    
    Examples:
        >>> # Basic world configuration
        >>> world_config = {
        ...     "width": 30,
        ...     "height": 30,
        ...     "visionWidth": 12,
        ...     "visionHeight": 12
        ... }
        >>> world = create_world_from_config(world_config)
        >>> 
        >>> # Advanced world with agent start position
        >>> world_config = {
        ...     "width": 50,
        ...     "height": 50,
        ...     "visionWidth": 16,
        ...     "visionHeight": 16,
        ...     "agent_start": (25.0, 25.0)
        ... }
        >>> world = create_world_from_config(world_config)
    """
    try:
        world = pynlm.createSimpleWorld()
        
        # Configure basic properties
        if "width" in world_config_dict:
            world.configure(width=world_config_dict["width"])
        if "height" in world_config_dict:
            world.configure(height=world_config_dict["height"])
        if "visionWidth" in world_config_dict:
            world.configure(visionWidth=world_config_dict["visionWidth"])
        if "visionHeight" in world_config_dict:
            world.configure(visionHeight=world_config_dict["visionHeight"])
        
        # Handle agent start position
        if "agent_start" in world_config_dict:
            x, y = world_config_dict["agent_start"]
            world.setAgentStart(float(x), float(y))
        
        # Reset world to initial state
        world.reset()
        
        return world
        
    except Exception as e:
        raise pynlm.NLMError(f"Failed to create world from config: {e}")
