#!/usr/bin/env python3
"""
NLM Python Interface
Comprehensive Python bindings for the NLM (Neural Learning Machine) framework
Provides both high-level APIs for beginners and advanced functionality for researchers
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pybind11
from pybind11 import cpp_function, cppclass
import numpy as np

# Import the main C++ modules
import pynlm

def create_advanced_brain(config_overrides=None, **kwargs):
    """
    Create a brain with advanced configuration options
    
    Args:
        config_overrides: Dictionary of configuration overrides
        **kwargs: Individual configuration parameters
    
    Returns:
        Brain object with configured parameters
    """
    config = pynlm.createDefaultConfig()
    
    # Apply configuration overrides
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value, pynlm.ConfigSource.Runtime)
    
    # Apply keyword arguments
    for key, value in kwargs.items():
        config.set(key, value, pynlm.ConfigSource.Runtime)
    
    return pynlm.createBrain(config)

def create_experiment_brain(base_config=None, **parameters):
    """
    Create a brain optimized for specific experiments
    
    Args:
        base_config: Base configuration (optional)
        **parameters: Experiment-specific parameters
    
    Returns:
        Configured brain object
    """
    if base_config is None:
        base_config = pynlm.createDefaultConfig()
    
    # Experiment-specific defaults
    experiment_defaults = {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "simulation_timestep": 0.0001,
        "max_steps": 10000,
        "enable_plasticity": True,
        "enable_learning": True,
        "random_seed": 42
    }
    
    # Apply experiment defaults
    for key, value in experiment_defaults.items():
        if not base_config.has(key):
            base_config.set(key, value, pynlm.ConfigSource.Default)
    
    # Override with provided parameters
    for key, value in parameters.items():
        base_config.set(key, value, pynlm.ConfigSource.Runtime)
    
    return pynlm.createBrain(base_config)

def run_learning_experiment(brain, world=None, agent=None, steps=1000, 
                           enable_logging=True, save_state=False):
    """
    Run a complete learning experiment with the brain
    
    Args:
        brain: The brain object to simulate
        world: Optional world object for interaction
        agent: Optional agent for brain-world interaction
        steps: Number of simulation steps
        enable_logging: Enable/disable logging
        save_state: Save brain state at intervals
    
    Returns:
        Dictionary with experiment results
    """
    results = {
        "total_spikes": 0,
        "firing_neurons": 0,
        "average_firing_rate": 0.0,
        "learning_detected": False,
        "plasticity_enabled": False,
        "development_stage": None
    }
    
    # Initialize logging
    if enable_logging:
        import logging
        logging.basicConfig(level=logging.INFO)
        logger = logging.getLogger("NLM_Experiment")
    
    try:
        # Initialize brain if not already initialized
        if not brain.is_initialized():
            brain.initialize()
        
        # Main simulation loop
        for step in range(steps):
            if world and agent:
                # Full agent-world interaction
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain thinks
                brain.step(step)
                
                # Produce action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            else:
                # Simple brain-only simulation
                brain.step(step)
            
            # Log progress periodically
            if enable_logging and step % 100 == 0:
                if logger:
                    logger.info(f"Step {step}: "
                              f"Spikes={brain.getTotalSpikeCount()}, "
                              f"Firing={brain.getFiringNeuronCount()}, "
                              f"Rate={brain.getAverageFiringRate():.2f}Hz")
            
            # Save state periodically
            if save_state and step % 1000 == 0 and step > 0:
                state_file = f"nlm_checkpoint_step_{step}.nlm"
                brain.save(state_file)
                if logger:
                    logger.info(f"Saved checkpoint: {state_file}")
        
        # Collect final statistics
        results["total_spikes"] = brain.getTotalSpikeCount()
        results["firing_neurons"] = brain.getFiringNeuronCount()
        results["average_firing_rate"] = brain.getAverageFiringRate()
        results["development_stage"] = brain.getDevelopmentalStage()
        results["plasticity_enabled"] = brain.isPlasticityEnabled()
        
        # Check if learning occurred
        results["learning_detected"] = (
            results["total_spikes"] > 0 and 
            results["average_firing_rate"] > 0.1
        )
        
    except Exception as e:
        if enable_logging and logger:
            logger.error(f"Experiment failed at step {step}: {e}")
        raise
    
    return results

def create_training_scenario(scenario_type="random_walk", **params):
    """
    Create a predefined training scenario
    
    Args:
        scenario_type: Type of scenario ("random_walk", "grid_world", "maze", "predator_prey")
        **params: Scenario-specific parameters
    
    Returns:
        Tuple of (brain, world, agent) configured for the scenario
    """
    # Create brain
    brain = create_experiment_brain(
        neuron_count=params.get("neuron_count", 500),
        region_count=params.get("region_count", 1),
        connection_probability=params.get("connection_probability", 0.15)
    )
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=params.get("world_width", 20),
        height=params.get("world_height", 20),
        visionWidth=params.get("vision_width", 8),
        visionHeight=params.get("vision_height", 8)
    )
    world.setAgentStart(
        params.get("start_x", 10),
        params.get("start_y", 10)
    )
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable specific features based on scenario
    if scenario_type == "random_walk":
        agent.enableCuriosity(True)
        agent.enableNoveltyDetection(True)
    elif scenario_type == "grid_world":
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
    elif scenario_type == "maze":
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    elif scenario_type == "predator_prey":
        agent.enableRewardModulation(True)
        agent.enableNoveltyDetection(True)
    
    return brain, world, agent

def analyze_brain_performance(brain, window_size=100):
    """
    Analyze brain performance statistics
    
    Args:
        brain: Brain object to analyze
        window_size: Size of analysis window (steps)
    
    Returns:
        Dictionary with performance metrics
    """
    metrics = {
        "current_spike_count": brain.getTotalSpikeCount(),
        "current_firing_neurons": brain.getFiringNeuronCount(),
        "current_average_firing_rate": brain.getAverageFiringRate(),
        "current_excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "current_developmental_stage": brain.getDevelopmentalStage(),
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "active_regions": brain.getRegionCount(),
    }
    
    # Add historical metrics if available
    if hasattr(brain, 'getSpikeHistory'):
        history = brain.getSpikeHistory()
        if len(history) > 1:
            # Calculate performance trends
            recent_spikes = sum(h.step for h in history[-window_size:])
            metrics["recent_average_spike_rate"] = recent_spikes / min(window_size, len(history))
            metrics["spike_history_length"] = len(history)
    
    return metrics

def benchmark_brain_performance(brain, test_scenarios=None, duration_seconds=60):
    """
    Benchmark brain performance across different scenarios
    
    Args:
        brain: Brain object to benchmark
        test_scenarios: List of scenario types to test
        duration_seconds: Duration of benchmark test
    
    Returns:
        Dictionary with benchmark results
    """
    if test_scenarios is None:
        test_scenarios = ["simple", "with_agent", "complex"]
    
    results = {
        "scenarios_tested": [],
        "performance_metrics": {},
        "benchmark_duration": duration_seconds,
        "speedups": {},
        "memory_usage": {}
    }
    
    start_time = time.time()
    
    for scenario in test_scenarios:
        scenario_start = time.time()
        
        if scenario == "simple":
            # Simple brain simulation
            steps = int(duration_seconds / 0.1)  # Assuming 0.1s timesteps
            for step in range(steps):
                brain.step(step)
                
        elif scenario == "with_agent":
            # Create a world and agent for interaction
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            steps = int(duration_seconds / 0.1)
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
        elif scenario == "complex":
            # Complex scenario with all features enabled
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            steps = int(duration_seconds / 0.1)
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simulate some reward-based learning
                if step % 10 == 0:
                    agent.applyRewardModulation(1.0, 0.5)
        
        scenario_duration = time.time() - scenario_start
        results["scenarios_tested"].append(scenario)
        results["performance_metrics"][scenario] = {
            "steps_performed": steps,
            "duration_seconds": scenario_duration,
            "steps_per_second": steps / scenario_duration if scenario_duration > 0 else 0,
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate()
        }
    
    total_duration = time.time() - start_time
    results["benchmark_duration"] = total_duration
    
    return results

def export_brain_for_research(brain, filepath, format="json"):
    """
    Export brain state for research purposes
    
    Args:
        brain: Brain object to export
        filepath: Output file path
        format: Export format ("json", "csv", "binary")
    """
    if format == "json":
        data = {
            "timestamp": time.time(),
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "developmental_stage": brain.getDevelopmentalStage().value,
            "configuration": dict(brain.getConfig().getKeys()) if hasattr(brain.getConfig(), 'getKeys') else {}
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
            
    elif format == "csv":
        import csv
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['metric', 'value'])
            writer.writerow(['neuron_count', brain.getTotalNeuronCount()])
            writer.writerow(['synapse_count', brain.getTotalSynapseCount()])
            writer.writerow(['total_spikes', brain.getTotalSpikeCount()])
            writer.writerow(['firing_rate', brain.getAverageFiringRate()])
            
    elif format == "binary":
        brain.save(filepath)
    
    return filepath

def import_brain_from_research(filepath, format="json"):
    """
    Import brain state from research data
    
    Args:
        filepath: Input file path
        format: Import format ("json", "csv", "binary")
    
    Returns:
        Config object with loaded parameters
    """
    config = pynlm.createDefaultConfig()
    
    if format == "json":
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Load parameters from JSON
        if "neuron_count" in data:
            config.set("neuron_count", data["neuron_count"], pynlm.ConfigSource.Runtime)
        if "synapse_count" in data:
            config.set("synapse_count", data["synapse_count"], pynlm.ConfigSource.Runtime)
            
    elif format == "csv":
        import csv
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                if row['metric'] in config.getKeys() or row['metric'] not in ["metric", "value"]:
                    try:
                        if row['metric'].isdigit():
                            config.set(row['metric'], int(row['value']), pynlm.ConfigSource.Runtime)
                        else:
                            config.set(row['metric'], float(row['value']), pynlm.ConfigSource.Runtime)
                    except (ValueError, KeyError):
                        pass
    
    return config

# Convenience functions for quick testing
def quick_test():
    """Quick test of NLM functionality"""
    print("=== Quick NLM Test ===")
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Created and tested brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"✓ Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

def interactive_demo():
    """Interactive demonstration with agent and world"""
    print("=== Interactive NLM Demo ===")
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Created brain, world, and agent")
    print("✓ Enabled learning features")
    
    # Run interaction loop
    for step in range(20):
        world.update(0.1)
        
        # Get what agent sees
        percept = world.getSensoryPercept()
        
        # Tell brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print status
        if step % 5 == 0:
            print(f"Step {step}: "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Action={action}")
    
    print("✓ Interactive demo completed")
    return brain, world, agent

# Additional advanced utilities
def create_neural_network_topology(topology_type="fully_connected", **params):
    """
    Create a specific neural network topology
    
    Args:
        topology_type: Type of topology
        **params: Topology parameters
    
    Returns:
        Config object for the specified topology
    """
    config = pynlm.createDefaultConfig()
    
    if topology_type == "fully_connected":
        regions = params.get("regions", 1)
        neurons_per_region = params.get("neurons_per_region", 100)
        config.set("region_count", regions, pynlm.ConfigSource.Default)
        config.set("neuron_count", regions * neurons_per_region, pynlm.ConfigSource.Default)
        config.set("connection_probability", 1.0, pynlm.ConfigSource.Default)  # Fully connected
        
    elif topology_type == "sparse_network":
        config.set("connection_probability", 0.1, pynlm.ConfigSource.Default)
        config.set("region_count", params.get("regions", 2), pynlm.ConfigSource.Default)
        
    elif topology_type == "hierarchical":
        config.set("region_count", params.get("regions", 3), pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.2, pynlm.ConfigSource.Default)
        
    elif topology_type == "modular":
        config.set("region_count", params.get("modules", 4), pynlm.ConfigSource.Default)
        config.set("connection_probability", 0.05, pynlm.ConfigSource.Default)
    
    return config

def optimize_brain_parameters(brain, optimization_goals=None):
    """
    Optimize brain parameters for specific goals
    
    Args:
        brain: Brain object to optimize
        optimization_goals: Dictionary of goals
    
    Returns:
        Dictionary with optimization results
    """
    if optimization_goals is None:
        optimization_goals = {
            "max_firing_rate": 50.0,  # Hz
            "min_spike_count": 100,
            "stable_behavior": True
        }
    
    results = {
        "current_metrics": analyze_brain_performance(brain),
        "optimization_goals": optimization_goals,
        "recommendations": [],
        "optimization_complete": False
    }
    
    # Check goals and provide recommendations
    current = results["current_metrics"]
    
    if current["current_average_firing_rate"] > optimization_goals["max_firing_rate"]:
        results["recommendations"].append(
            f"Reduce firing rate: current {current['current_average_firing_rate']:.1f} > goal {optimization_goals['max_firing_rate']}"
        )
        results["recommendations"].append("  -> Reduce input current or increase inhibition")
    
    if current["current_spike_count"] < optimization_goals["min_spike_count"]:
        results["recommendations"].append(
            f"Increase spike count: current {current['current_spike_count']} < goal {optimization_goals['min_spike_count']}"
        )
        results["recommendations"].append("  -> Increase excitatory input or neuron excitability")
    
    results["optimization_complete"] = len(results["recommendations"]) == 0
    
    return results

# Version and system information
__version__ = "1.0.0"
__author__ = "NLM Project"
__description__ = "Python interface for NLM (Neural Learning Machine)"

# Export key functions
__all__ = [
    'create_advanced_brain',
    'create_experiment_brain',
    'run_learning_experiment',
    'create_training_scenario',
    'analyze_brain_performance',
    'benchmark_brain_performance',
    'export_brain_for_research',
    'import_brain_from_research',
    'quick_test',
    'interactive_demo',
    'create_neural_network_topology',
    'optimize_brain_parameters',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain'
]

if __name__ == "__main__":
    print(f"NLM Python Interface v{__version__}")
    print(__description__)
    print("\nAvailable functions:")
    for func in __all__:
        if not func.startswith('_'):
            print(f"  - {func}")
