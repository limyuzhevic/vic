# Improved Python API for NLM (pynlm)
# 
# This file provides an enhanced Python API for the NLM neural simulation framework
# with better user experience and more advanced functionality.
#
# Key improvements:
# 1. Preset configuration profiles for different use cases
# 2. Convenience functions for common tasks
# 3. Better error handling and documentation
# 4. Backward compatibility with original API
# 5. Advanced configuration methods for learning scenarios
#
# Usage examples:
#
# # Create brain with preset configuration
# import pynlm
# config = pynlm.createConfigFromPreset("research")
# world = pynlm.createWorldFromPreset("large")
# agent = pynlm.createCompleteAgentSetup(config, world)
#
# # Configure for specific learning scenarios
# pynlm.configureBrainForLearning(config)
# pynlm.configureBrainForExploration(config)
# pynlm.configureBrainForMemory(config)
#
# # Run simulations easily
# pynlm.runSimulation(brain, world, agent, 100)
# pynlm.printBrainSummary(brain)
# pynlm.estimateMemoryUsage(brain)
#
# # Access constants for IDs
# neuron_id = pynlm.INVALID_NEURON_ID

# Common preset configurations for different use cases

# Minimal configuration - fast testing and prototyping
MINIMAL = "minimal"
BALANCED = "balanced"
HIGH_PERFORMANCE = "high_performance"
RESEARCH = "research"

# World presets for different environments
SMALL_WORLD = "small"
MEDIUM_WORLD = "medium"
LARGE_WORLD = "large"
OPEN_WORLD = "open_world"

# Configuration helper functions

def getConfigWithNeuronCount(neuron_count, region_count=2, connection_probability=0.1):
    """Create a custom configuration with specified neuron count."""
    import pynlm
    config = pynlm.createConfigFromPreset("balanced")
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    return config

def getConfigForTask(task_type, custom_neuron_count=None):
    """Create configuration based on task type.
    
    Args:
        task_type: "memory", "exploration", "cognition", or "adaptive"
        custom_neuron_count: Override neuron count if specified
        
    Returns:
        Configured brain object
    """
    import pynlm
    
    if task_type == "memory":
        config = pynlm.createConfigFromPreset("research")
        if custom_neuron_count:
            config.set("neuron_count", custom_neuron_count)
            
    elif task_type == "exploration":
        config = pynlm.createConfigFromPreset("high_performance")
        if custom_neuron_count:
            config.set("neuron_count", custom_neuron_count)
            
    elif task_type == "cognition":
        config = pynlm.createConfigFromPreset("balanced")
        if custom_neuron_count:
            config.set("neuron_count", custom_neuron_count)
            # Enhance cognition systems for cognitive tasks
            config.set("cognition.planning.enabled", True)
            config.set("cognition.concept_formation.enabled", True)
            
    elif task_type == "adaptive":
        config = pynlm.createConfigFromPreset("research")
        if custom_neuron_count:
            config.set("neuron_count", custom_neuron_count)
        # Enable all adaptive features
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
    else:
        raise ValueError(f"Unknown task type: {task_type}")
        
    return config

def runLearningEpisode(brain, world, agent, steps, with_reward=True, with_development=True):
    """Run a complete learning episode with optional features.
    
    Args:
        brain: Brain object
        world: World object  
        agent: AgentBrain object
        steps: Number of simulation steps
        with_reward: Whether to apply reward modulation
        with_development: Whether to update development system
        
    Returns:
        Dictionary with episode statistics
    """
    import pynlm
    
    episode_stats = {
        'steps_completed': 0,
        'total_reward': 0.0,
        'final_energy': 0.0,
        'novelty_level': 0.0,
        'curiosity_level': 0.0,
        'prediction_error': 0.0,
        'synapse_count': 0,
        'action_counts': {'move_forward': 0, 'move_backward': 0, 
                         'turn_left': 0, 'turn_right': 0, 
                         'interact': 0, 'wait': 0}
    }
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if enabled
        if with_reward:
            agent.applyRewardModulation(result.reward, 0.0)
            
        if with_development:
            agent.updateDevelopment(0.1)
            
        # Track statistics
        episode_stats['steps_completed'] = step + 1
        episode_stats['total_reward'] += result.reward
        
        # Count actions
        action_map = {
            pynlm.MotorCommand.MoveForward: 'move_forward',
            pynlm.MotorCommand.MoveBackward: 'move_backward',
            pynlm.MotorCommand.TurnLeft: 'turn_left',
            pynlm.MotorCommand.TurnRight: 'turn_right',
            pynlm.MotorCommand.Interact: 'interact',
            pynlm.MotorCommand.Wait: 'wait'
        }
        
        action_key = action_map.get(action, 'wait')
        episode_stats['action_counts'][action_key] += 1
    
    # Get final statistics
    episode_stats['final_energy'] = world.getAgentBody().energy
    episode_stats['novelty_level'] = agent.getNoveltyLevel()
    episode_stats['curiosity_level'] = agent.getCuriosityLevel()
    episode_stats['prediction_error'] = agent.getPredictionError()
    episode_stats['synapse_count'] = brain.getTotalSynapseCount()
    
    return episode_stats

def compareBrains(brain1, brain2, comparison_functions=None):
    """Compare two brains and return comparison results.
    
    Args:
        brain1: First brain to compare
        brain2: Second brain to compare
        comparison_functions: List of comparison functions to run
            or None for default comparisons
            
    Returns:
        Dictionary with comparison results
    """
    if comparison_functions is None:
        comparison_functions = [
            'basic_stats',
            'memory_systems',
            'plasticity_systems',
            'neuromodulation',
            'development'
        ]
    
    results = {}
    
    if 'basic_stats' in comparison_functions:
        results['basic_stats'] = {
            'neuron_count_diff': brain2.getTotalNeuronCount() - brain1.getTotalNeuronCount(),
            'synapse_count_diff': brain2.getTotalSynapseCount() - brain1.getTotalSynapseCount(),
            'firing_rate_diff': brain2.getAverageFiringRate() - brain1.getAverageFiringRate(),
            'total_spikes_diff': brain2.getTotalSpikeCount() - brain1.getTotalSpikeCount(),
            'e_i_ratio_diff': brain2.getExcitationInhibitionRatio() - brain1.getExcitationInhibitionRatio()
        }
    
    if 'memory_systems' in comparison_functions:
        results['memory_systems'] = {
            'working_memory_diff': (
                brain2.getWorkingMemory().getActiveTraces() if brain2.getWorkingMemory() else 0
            ) - (
                brain1.getWorkingMemory().getActiveTraces() if brain1.getWorkingMemory() else 0
            ),
            'episodic_memory_diff': (
                brain2.getEpisodicMemory().getEpisodeCount() if brain2.getEpisodicMemory() else 0
            ) - (
                brain1.getEpisodicMemory().getEpisodeCount() if brain1.getEpisodicMemory() else 0
            )
        }
    
    if 'plasticity_systems' in comparison_functions:
        results['plasticity_systems'] = {
            'stdp_ltp_diff': (
                brain2.getSTDP().getLTPWeight() if brain2.getSTDP() else 0
            ) - (
                brain1.getSTDP().getLTPWeight() if brain1.getSTDP() else 0
            ),
            'stdp_ltd_diff': (
                brain2.getSTDP().getLTDWeight() if brain2.getSTDP() else 0
            ) - (
                brain1.getSTDP().getLTDWeight() if brain1.getSTDP() else 0
            )
        }
    
    if 'neuromodulation' in comparison_functions:
        results['neuromodulation'] = {
            'dopamine_diff': (
                brain2.getDopamine().getLevel() if brain2.getDopamine() else 0
            ) - (
                brain1.getDopamine().getLevel() if brain1.getDopamine() else 0
            ),
            'curiosity_diff': (
                brain2.getCuriosity().getLevel() if brain2.getCuriosity() else 0
            ) - (
                brain1.getCuriosity().getLevel() if brain1.getCuriosity() else 0
            )
        }
    
    if 'development' in comparison_functions:
        results['development'] = {
            'developmental_stage_diff': (
                brain2.getDevelopmentalStage() != brain1.getDevelopmentalStage()
            ),
            'stage1': str(brain1.getDevelopmentalStage()),
            'stage2': str(brain2.getDevelopmentalStage())
        }
    
    return results

def createMultiAgentSetup(agent_configs):
    """Create multiple agents with different configurations.
    
    Args:
        agent_configs: List of dictionaries with config and world for each agent
            
    Returns:
        List of agent setups
    """
    import pynlm
    
    agent_setups = []
    
    for i, config_dict in enumerate(agent_configs):
        config = config_dict.get('config', pynlm.createConfigFromPreset("balanced"))
        world = config_dict.get('world', pynlm.createWorldFromPreset("medium"))
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on config
        if config.get('enable_reward_modulation', True):
            agent.enableRewardModulation(True)
        if config.get('enable_structural_plasticity', True):
            agent.enableStructuralPlasticity(True)
        if config.get('enable_development', True):
            agent.enableDevelopment(True)
        if config.get('enable_curiosity', True):
            agent.enableCuriosity(True)
        
        agent_setups.append({
            'id': i,
            'brain': brain,
            'world': world,
            'agent': agent,
            'config': config
        })
    
    return agent_setups

def runCompetitiveEpisode(agent_setups, steps):
    """Run a competitive episode with multiple agents.
    
    Args:
        agent_setups: List of agent setups (from createMultiAgentSetup)
        steps: Number of simulation steps
        
    Returns:
        Dictionary with episode results for each agent
    """
    import pynlm
    
    episode_results = {}
    
    for agent_setup in agent_setups:
        agent = agent_setup['agent']
        world = agent_setup['world']
        brain = agent_setup['brain']
        
        # Reset for this agent's episode
        world.reset()
        agent.reset()
        brain.reset()
        
        # Run episode
        for step in range(steps):
            world.update(0.1)
            
            # All agents get same percept
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Each agent processes independently
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Record results
        episode_results[agent_setup['id']] = {
            'total_reward': result.reward,
            'action': str(action),
            'energy': world.getAgentBody().energy,
            'novelty': agent.getNoveltyLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'synapses': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
    
    return episode_results

def saveBrainWithMetadata(brain, filepath, metadata=None):
    """Save brain state with metadata to a file.
    
    Args:
        brain: Brain object to save
        filepath: File path to save to
        metadata: Optional metadata dictionary to save with brain
        
    Returns:
        True if save successful, False otherwise
    """
    import json
    import os
    
    # Create save directory if it doesn't exist
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    # Prepare metadata
    save_metadata = {
        'timestamp': pynlm.getCurrentTimestamp(),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'config_summary': brain.getConfig().summary() if brain.getConfig() else "None"
    }
    
    if metadata:
        save_metadata.update(metadata)
    
    # Try to save brain checkpoint
    save_success = brain.save(filepath)
    
    if save_success:
        # Save metadata separately
        metadata_file = filepath.replace('.bin', '_meta.json')
        with open(metadata_file, 'w') as f:
            json.dump(save_metadata, f, indent=2)
        
        return True
    else:
        return False

def loadBrainWithMetadata(filepath):
    """Load brain state with metadata from a file.
    
    Args:
        filepath: File path to load from (should be brain checkpoint file)
        
    Returns:
        Tuple of (brain, metadata) if successful, (None, None) otherwise
    """
    import json
    
    # Try to load metadata first
    metadata_file = filepath.replace('.bin', '_meta.json')
    metadata = {}
    
    if os.path.exists(metadata_file):
        with open(metadata_file, 'r') as f:
            metadata = json.load(f)
    
    # Create a new brain with default config
    import pynlm
    config = pynlm.createConfigFromPreset("research")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Try to load brain state
    load_success = brain.load(filepath)
    
    if load_success:
        return brain, metadata
    else:
        return None, None

def getPerformanceMetrics(brain, world=None, agent=None):
    """Get comprehensive performance metrics for the brain/agent system.
    
    Args:
        brain: Brain object
        world: Optional world object
        agent: Optional agent object
        
    Returns:
        Dictionary with performance metrics
    """
    metrics = {
        'brain_metrics': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'pending_events': brain.getPendingSpikeEventCount()
        },
        'memory_metrics': {},
        'neuromodulation_metrics': {},
        'development_metrics': {}
    }
    
    # Add memory system metrics if available
    if brain.getWorkingMemory():
        metrics['memory_metrics']['working_memory_capacity'] = brain.getWorkingMemory().getCapacity()
        metrics['memory_metrics']['working_memory_utilization'] = brain.getWorkingMemory().getActiveTraces()
    
    if brain.getEpisodicMemory():
        metrics['memory_metrics']['episodic_memory_capacity'] = brain.getEpisodicMemory().getMaxEpisodes()
        metrics['memory_metrics']['episodic_memory_used'] = brain.getEpisodicMemory().getEpisodeCount()
    
    # Add neuromodulation metrics if available
    if brain.getDopamine():
        metrics['neuromodulation_metrics']['dopamine_level'] = brain.getDopamine().getLevel()
    
    if brain.getCuriosity():
        metrics['neuromodulation_metrics']['curiosity_level'] = brain.getCuriosity().getLevel()
    
    if brain.getNovelty():
        metrics['neuromodulation_metrics']['novelty_level'] = brain.getNovelty().getLevel()
    
    # Add development metrics if available
    if brain.getDevelopmentSystem():
        metrics['development_metrics']['developmental_stage'] = str(brain.getDevelopmentalStage())
    
    # Add world/agent metrics if provided
    if world and agent:
        metrics['environment_metrics'] = {
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health,
            'simulation_time': world.getSimulationTime(),
            'world_width': world.getWidth(),
            'world_height': world.getHeight()
        }
        
        metrics['agent_metrics'] = {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        }
    
    return metrics

def printDetailedMetrics(metrics):
    """Print detailed metrics in a human-readable format.
    
    Args:
        metrics: Dictionary of performance metrics (from getPerformanceMetrics)
    """
    print("=== DETAILED PERFORMANCE METRICS ===")
    print("\n--- Brain Metrics ---")
    brain_metrics = metrics['brain_metrics']
    print(f"Neurons: {brain_metrics['neuron_count']}")
    print(f"Synapses: {brain_metrics['synapse_count']}")
    print(f"Total spikes: {brain_metrics['total_spikes']}")
    print(f"Firing rate: {brain_metrics['firing_rate']:.2f} Hz")
    print(f"E/I ratio: {brain_metrics['e_i_ratio']:.2f}")
    print(f"Active neurons: {brain_metrics['active_neurons']}")
    print(f"Firing neurons: {brain_metrics['firing_neurons']}")
    print(f"Pending events: {brain_metrics['pending_events']}")
    
    if 'memory_metrics' in metrics and metrics['memory_metrics']:
        print("\n--- Memory Metrics ---")
        for key, value in metrics['memory_metrics'].items():
            print(f"{key}: {value}")
    
    if 'neuromodulation_metrics' in metrics and metrics['neuromodulation_metrics']:
        print("\n--- Neuromodulation Metrics ---")
        for key, value in metrics['neuromodulation_metrics'].items():
            print(f"{key}: {value}")
    
    if 'development_metrics' in metrics and metrics['development_metrics']:
        print("\n--- Development Metrics ---")
        for key, value in metrics['development_metrics'].items():
            print(f"{key}: {value}")
    
    if 'environment_metrics' in metrics and metrics['environment_metrics']:
        print("\n--- Environment Metrics ---")
        for key, value in metrics['environment_metrics'].items():
            print(f"{key}: {value}")
    
    if 'agent_metrics' in metrics and metrics['agent_metrics']:
        print("\n--- Agent Metrics ---")
        for key, value in metrics['agent_metrics'].items():
            print(f"{key}: {value}")
    
    print("=== END METRICS ===")

# Get current timestamp for metadata
def getCurrentTimestamp():
    """Get current timestamp as ISO format string."""
    from datetime import datetime
    return datetime.now().isoformat()