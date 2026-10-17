#!/usr/bin/env python3
"""
NLM Python Convenience Functions

Enhance the pynlm Python API with convenience functions for common operations.
This module provides easier-to-use alternatives to the basic pynlm interface.
"""

import pynlm

def create_and_initialize_brain(config=None, neuron_count=1000, region_count=2):
    """
    Create and initialize a brain with a configuration.
    
    Args:
        config: Optional configuration object
        neuron_count: Number of neurons to create
        region_count: Number of brain regions to create
        
    Returns:
        tuple: (brain, config) initialized brain and configuration
        
    Example:
        >>> brain, config = create_and_initialize_brain(neuron_count=500)
        >>> brain.step(0)  # Start simulation
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain, config

def run_simulation(config=None, neurons=1000, steps=1000, timestep=0.001,
                   enable_reward=True, enable_curiosity=True,
                   enable_development=True):
    """
    Run a complete NLM simulation with a basic agent setup.
    
    Args:
        config: Optional configuration object
        neurons: Number of neurons (default: 1000)
        steps: Number of simulation steps (default: 1000)
        timestep: Simulation timestep in seconds (default: 0.001)
        enable_reward: Enable reward modulation (default: True)
        enable_curiosity: Enable curiosity-driven exploration (default: True)
        enable_development: Enable developmental learning (default: True)
        
    Returns:
        dict: Simulation results containing brain, agent, and world
        
    Example:
        >>> results = run_simulation(neurons=500, steps=500)
        >>> print(f"Final firing rate: {results['brain'].getAverageFiringRate():.2f}")
    """
    # Create configuration
    if config is None:
        config = pynlm.createDefaultConfig()
    
    config.set("brain.neuron_count", neurons)
    config.set("brain.region_count", 1)
    config.set("simulation_timestep", timestep)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(enable_reward)
    agent.enableStructuralPlasticity(enable_development)
    agent.enableDevelopment(enable_development)
    agent.enableCuriosity(enable_curiosity)
    
    # Run simulation
    for step in range(steps):
        world.update(timestep)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        if enable_reward:
            reward = 0.0
            if world.getSensoryPercept().getInternal():
                reward = world.getSensoryPercept().getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if enable_development:
            agent.updateDevelopment(timestep)
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'steps': steps,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'developmental_stage': brain.getDevelopmentalStage()
    }

def create_structured_brain(config=None):
    """
    Create a brain with organized structure (regions and populations).
    
    Args:
        config: Optional configuration object
        
    Returns:
        tuple: (brain, regions) brain with structured regions and regions list
        
    Example:
        >>> brain, regions = create_structured_brain()
        >>> sensory_region = brain.getRegion(regions[0])
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create specialized regions
    region_names = ["Sensory", "Motor", "Association", "Integration"]
    regions = []
    
    for i, name in enumerate(region_names):
        region_id = brain.addRegion(name)
        regions.append(region_id)
    
    return brain, regions

def create_grid_environment(width=20, height=20, vision_width=8, vision_height=8):
    """
    Create a standard grid environment.
    
    Args:
        width: World width in cells (default: 20)
        height: World height in cells (default: 20)
        vision_width: Vision width in cells (default: 8)
        vision_height: Vision height in cells (default: 8)
        
    Returns:
        SimpleWorld: Configured world object
        
    Example:
        >>> world = create_grid_environment(width=10, height=10)
        >>> world.setAgentStart(5, 5)
    """
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=vision_width, visionHeight=vision_height)
    world.reset()
    world.setAgentStart(width//2, height//2)  # Center start
    
    return world

def analyze_brain_activity(brain, steps_to_analyze=None):
    """
    Analyze brain activity patterns over time.
    
    Args:
        brain: Brain object to analyze
        steps_to_analyze: Number of recent steps to analyze (None for all)
        
    Returns:
        dict: Activity statistics including firing rate, spike count, etc.
        
    Example:
        >>> stats = analyze_brain_activity(brain, steps_to_analyze=100)
        >>> print(f"Average firing rate: {stats['avg_firing_rate']:.2f}")
    """
    # Get basic statistics
    stats = {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'total_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'ei_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage()
    }
    
    # Calculate additional metrics
    if brain.getTotalNeuronCount() > 0:
        stats['activity_percentage'] = (
            stats['firing_neuron_count'] / stats['neuron_count'] * 100
        )
    else:
        stats['activity_percentage'] = 0.0
    
    if stats['total_spike_count'] > 0:
        stats['spike_density'] = stats['total_spike_count'] / stats['neuron_count']
    else:
        stats['spike_density'] = 0.0
    
    return stats

def benchmark_performance(config=None, durations=[100, 500, 1000]):
    """
    Run performance benchmarks with different simulation durations.
    
    Args:
        config: Optional configuration object
        durations: List of step counts to benchmark (default: [100, 500, 1000])
        
    Returns:
        dict: Benchmark results with timing and performance metrics
        
    Example:
        >>> results = benchmark_performance(durations=[100, 500, 1000])
        >>> print(f"Speed at 1000 steps: {results[1000]['speed']:.1f} steps/sec")
    """
    import time
    
    results = {}
    
    for duration in durations:
        # Create fresh brain for each benchmark
        brain_config = pynlm.createDefaultConfig() if config is None else config
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for step in range(duration):
            brain.step(step)
        
        end_time = time.time()
        
        elapsed_time = end_time - start_time
        speed = duration / elapsed_time if elapsed_time > 0 else 0
        
        results[duration] = {
            'steps': duration,
            'time_seconds': elapsed_time,
            'speed_steps_per_second': speed,
            'neurons': brain.getTotalNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    return results

def export_results_to_csv(brain, agent, world, filename, format='simple'):
    """
    Export experiment results to CSV format.
    
    Args:
        brain: Final brain object
        agent: Final agent object
        world: Final world object
        filename: Output CSV filename
        format: Export format ('simple', 'detailed', or 'comprehensive')
        
    Returns:
        bool: True if successful, False otherwise
        
    Example:
        >>> export_results_to_csv(brain, agent, world, 'results.csv')
        >>> print(f"Results exported to results.csv")
    """
    try:
        with open(filename, 'w') as f:
            if format == 'simple':
                f.write("parameter,value\n")
                f.write(f"neuron_count,{brain.getTotalNeuronCount()}\n")
                f.write(f"synapse_count,{brain.getTotalSynapseCount()}\n")
                f.write(f"firing_neuron_count,{brain.getFiringNeuronCount()}\n")
                f.write(f"total_spike_count,{brain.getTotalSpikeCount()}\n")
                f.write(f"avg_firing_rate,{brain.getAverageFiringRate():.6f}\n")
                f.write(f"ei_ratio,{brain.getExcitationInhibitionRatio():.6f}\n")
                f.write(f"developmental_stage,{brain.getDevelopmentalStage()}\n")
                f.write(f"curiosity_level,{agent.getCuriosityLevel():.6f}\n")
                f.write(f"novelty_level,{agent.getNoveltyLevel():.6f}\n")
                f.write(f"simulation_time,{world.getSimulationTime():.6f}\n")
                
            elif format == 'detailed':
                f.write("parameter,value\n")
                # Add more detailed metrics
                f.write(f"neuron_count,{brain.getTotalNeuronCount()}\n")
                f.write(f"synapse_count,{brain.getTotalSynapseCount()}\n")
                f.write(f"active_neuron_count,{brain.getActiveNeuronCount()}\n")
                f.write(f"firing_neuron_count,{brain.getFiringNeuronCount()}\n")
                f.write(f"total_spike_count,{brain.getTotalSpikeCount()}\n")
                f.write(f"avg_firing_rate,{brain.getAverageFiringRate():.6f}\n")
                f.write(f"ei_ratio,{brain.getExcitationInhibitionRatio():.6f}\n")
                f.write(f"developmental_stage,{brain.getDevelopmentalStage()}\n")
                f.write(f"world_width,{world.getWidth()}\n")
                f.write(f"world_height,{world.getHeight()}\n")
                f.write(f"world_time,{world.getSimulationTime():.6f}\n")
                f.write(f"agent_x,{world.getAgentBody().x:.6f}\n")
                f.write(f"agent_y,{world.getAgentBody().y:.6f}\n")
                f.write(f"agent_orientation,{world.getAgentBody().orientation:.6f}\n")
                
            elif format == 'comprehensive':
                f.write("category,parameter,value\n")
                # Comprehensive export
                f.write(f"brain,neuron_count,{brain.getTotalNeuronCount()}\n")
                f.write(f"brain,synapse_count,{brain.getTotalSynapseCount()}\n")
                f.write(f"brain,active_neurons,{brain.getActiveNeuronCount()}\n")
                f.write(f"brain,firing_neurons,{brain.getFiringNeuronCount()}\n")
                f.write(f"brain,total_spikes,{brain.getTotalSpikeCount()}\n")
                f.write(f"brain,avg_firing_rate,{brain.getAverageFiringRate():.6f}\n")
                f.write(f"brain,ei_ratio,{brain.getExcitationInhibitionRatio():.6f}\n")
                f.write(f"brain,developmental_stage,{brain.getDevelopmentalStage()}\n")
                f.write(f"agent,curiosity_level,{agent.getCuriosityLevel():.6f}\n")
                f.write(f"agent,novelty_level,{agent.getNoveltyLevel():.6f}\n")
                f.write(f"agent,neuromodulation_level,{agent.getNeuromodulationLevel():.6f}\n")
                f.write(f"world,simulation_time,{world.getSimulationTime():.6f}\n")
                f.write(f"world,world_width,{world.getWidth()}\n")
                f.write(f"world,world_height,{world.getHeight()}\n")
                
                # Add state information
                body = world.getAgentBody()
                f.write(f"agent,agent_x,{body.x:.6f}\n")
                f.write(f"agent,agent_y,{body.y:.6f}\n")
                f.write(f"agent,orientation,{body.orientation:.6f}\n")
                f.write(f"agent,energy,{body.energy:.6f}\n")
                f.write(f"agent,health,{body.health:.6f}\n")
                f.write(f"agent,age,{body.age:.6f}\n")
        
        return True
        
    except Exception as e:
        print(f"Error exporting results to CSV: {e}")
        return False

# Make commonly used classes and functions available at module level
__all__ = [
    'create_and_initialize_brain',
    'run_simulation',
    'create_structured_brain',
    'create_grid_environment',
    'analyze_brain_activity',
    'benchmark_performance',
    'export_results_to_csv',
    'createSimpleWorld',
    'createBrain',
    'createAgentBrain',
    'createDefaultConfig'
]

# Import and re-export commonly used functions from pynlm
createSimpleWorld = pynlm.createSimpleWorld
createBrain = pynlm.createBrain
createAgentBrain = pynlm.createAgentBrain
createDefaultConfig = pynlm.createDefaultConfig

# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python utilities for NLM brain simulation"

print(f"NLM Python utilities v{__version__} loaded successfully")
print(f"Additional convenience functions available: {len(__all__)}")