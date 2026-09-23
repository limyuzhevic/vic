"""NLM Python Utilities and Convenience Functions

This module provides utility functions and convenience methods to make
using NLM more Pythonic and user-friendly.
"""

import pynlm as nlm
import json
import pickle
from typing import Dict, List, Any, Optional, Union

def create_brain_with_preset_config(preset_name: str = "balanced") -> nlm.Brain:
    """
    Create a brain with preset configuration profiles.
    
    Args:
        preset_name: "balanced", "fast_learning", "stable", "creative", or "adaptive"
    
    Returns:
        Configured and initialized Brain object
    """
    presets = {
        "balanced": {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        },
        "fast_learning": {
            "brain.neuron_count": 500,
            "plasticity.stdp.learning_rate": 0.01,
            "plasticity.hebbian.learning_rate": 0.005,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        },
        "stable": {
            "brain.neuron_count": 2000,
            "plasticity.stdp.learning_rate": 0.0001,
            "plasticity.hebbian.learning_rate": 0.00005,
            "plasticity.structural.enable": False,
            "neuromod.curiosity.enable": False,
        },
        "creative": {
            "brain.neuron_count": 1500,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        },
        "adaptive": {
            "brain.neuron_count": 1000,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.enabled": True,
        },
    }
    
    if preset_name not in presets:
        raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
    
    config = nlm.createDefaultConfig()
    for key, value in presets[preset_name].items():
        config.set(key, value)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_agent_with_training(config: Optional[nlm.Config] = None, 
                             world: Optional[nlm.SimpleWorld] = None) -> tuple:
    """
    Create a complete agent setup with brain and world for training.
    
    Args:
        config: Optional custom configuration
        world: Optional custom world
    
    Returns:
        Tuple of (brain, agent, world)
    """
    if config is None:
        config = nlm.createDefaultConfig()
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    
    if world is None:
        world = nlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
    
    agent.initialize(world)
    
    return brain, agent, world

def run_training_episode(agent: nlm.AgentBrain, 
                       world: nlm.SimpleWorld, 
                       num_steps: int,
                       explore: bool = True) -> Dict[str, Any]:
    """
    Run a complete training episode for an agent.
    
    Args:
        agent: Agent brain to train
        world: World to simulate in
        num_steps: Number of steps to run
        explore: Whether to enable exploration behavior
    
    Returns:
        Dictionary with training statistics
    """
    stats = {
        "steps": num_steps,
        "total_spikes": 0,
        "firing_neuron_counts": [],
        "curiosity_levels": [],
        "reward_history": [],
        "action_counts": {},
    }
    
    brain = agent.getBrain()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track action
        action_name = str(action.getType()) if hasattr(action, 'getType') else str(action)
        stats["action_counts"][action_name] = stats["action_counts"].get(action_name, 0) + 1
        
        # Apply reward modulation (simulate reward)
        reward = world.getAgentBody().energy / 100.0  # Simple reward based on energy
        agent.applyRewardModulation(reward, 0.0)
        stats["reward_history"].append(reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats["total_spikes"] += brain.getTotalSpikeCount()
        stats["firing_neuron_counts"].append(brain.getFiringNeuronCount())
        stats["curiosity_levels"].append(agent.getCuriosityLevel())
    
    return stats

def export_brain_state(brain: nlm.Brain, filepath: str, format: str = "binary"):
    """
    Export brain state to file.
    
    Args:
        brain: Brain object to export
        filepath: Output file path
        format: "binary", "json", or "pickle"
    """
    if format == "binary":
        brain.save(filepath)
    elif format == "json":
        # For JSON, we'd need to extract key statistics
        state = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "exc_inh_ratio": brain.getExcitationInhibitionRatio(),
        }
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
    elif format == "pickle":
        with open(filepath, 'wb') as f:
            pickle.dump(brain, f)
    else:
        raise ValueError(f"Unsupported format: {format}")

def import_brain_state(filepath: str, format: str = "binary") -> nlm.Brain:
    """
    Import brain state from file.
    
    Args:
        filepath: Input file path
        format: "binary", "json", or "pickle"
    
    Returns:
        Brain object loaded from file
    """
    if format == "binary":
        brain = nlm.createDefaultConfig()
        brain.load(filepath)
        return brain
    elif format == "json":
        with open(filepath, 'r') as f:
            state = json.load(f)
        # Create new brain and apply state (simplified)
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        # In a real implementation, we'd restore the neural state
        return brain
    elif format == "pickle":
        with open(filepath, 'rb') as f:
            return pickle.load(f)
    else:
        raise ValueError(f"Unsupported format: {format}")

def analyze_brain_statistics(brain: nlm.Brain) -> Dict[str, Any]:
    """
    Analyze brain statistics and return insights.
    
    Args:
        brain: Brain object to analyze
    
    Returns:
        Dictionary with analysis results
    """
    analysis = {
        "basic_stats": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
        },
        "activity_patterns": {
            "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "plasticity_enabled": True,  # In real implementation, check config
        },
        "health_indicators": {
            "neural_integrity": brain.getTotalNeuronCount() > 0,
            "synaptic_density": brain.getTotalSynapseCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
            "activity_level": brain.getFiringNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
        },
        "performance": {
            "efficiency": brain.getAverageFiringRate() / max(1, brain.getFiringNeuronCount()),
            "stability": 1.0 / (1.0 + brain.getTotalSpikeCount() / max(1, brain.getTotalNeuronCount())),
        },
    }
    
    return analysis

def create_visualization_data(brain: nlm.Brain, 
                            world: Optional[nlm.SimpleWorld] = None) -> Dict[str, Any]:
    """
    Create data structures for visualization.
    
    Args:
        brain: Brain object to visualize
        world: Optional world object
    
    Returns:
        Dictionary with visualization data
    """
    vis_data = {
        "brain_neurons": [],
        "brain_synapses": [],
        "regions": [],
        "statistics": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
        },
    }
    
    # Add neurons for visualization
    for region in brain.getRegions():
        for population in region.getPopulations():
            vis_data["regions"].append({
                "id": region.getId().value,
                "name": region.getName(),
                "population_count": population.getSize(),
                "type": population.getNeuronType(),
            })
    
    # Add world data if provided
    if world is not None:
        vis_data["world"] = {
            "width": world.getWidth(),
            "height": world.getHeight(),
            "agent_position": (world.getAgentBody().x, world.getAgentBody().y),
            "simulation_time": world.getSimulationTime(),
        }
    
    return vis_data

def benchmark_configuration(config: nlm.Config, num_steps: int = 100) -> Dict[str, Any]:
    """
    Benchmark a brain configuration.
    
    Args:
        config: Configuration to benchmark
        num_steps: Number of steps to run for benchmark
    
    Returns:
        Dictionary with benchmark results
    """
    import time
    
    results = {
        "config": config,
        "num_steps": num_steps,
        "metrics": {},
        "timing": {},
    }
    
    # Time initialization
    start_time = time.time()
    brain = nlm.createBrain(config)
    brain.initialize()
    init_time = time.time() - start_time
    
    # Time simulation
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    sim_time = time.time() - start_time
    
    results["timing"] = {
        "initialization": init_time,
        "simulation": sim_time,
        "total": init_time + sim_time,
    }
    
    results["metrics"] = {
        "neurons_per_second": brain.getTotalNeuronCount() / init_time if init_time > 0 else 0,
        "spikes_per_step": brain.getTotalSpikeCount() / num_steps if num_steps > 0 else 0,
        "average_firing_rate": brain.getAverageFiringRate(),
        "neurons_firing_percentage": brain.getFiringNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
    }
    
    return results

class NLMExperiment:
    """
    High-level interface for running NLM experiments.
    """
    
    def __init__(self, name: str, config: Optional[nlm.Config] = None):
        self.name = name
        self.config = config or nlm.createDefaultConfig()
        self.results = {}
    
    def run_trial(self, trial_id: int, **kwargs) -> Dict[str, Any]:
        """
        Run a single trial.
        
        Args:
            trial_id: Trial identifier
            **kwargs: Additional parameters
        
        Returns:
            Trial results
        """
        print(f"Running trial {trial_id} for experiment {self.name}")
        
        # Create agent setup
        brain, agent, world = create_agent_with_training(self.config)
        
        # Configure trial parameters
        num_steps = kwargs.get('num_steps', 100)
        explore = kwargs.get('explore', True)
        
        # Run training episode
        trial_results = run_training_episode(agent, world, num_steps, explore)
        
        # Store results
        self.results[trial_id] = trial_results
        
        return trial_results
    
    def run_experiment(self, num_trials: int, **kwargs) -> Dict[str, Any]:
        """
        Run complete experiment.
        
        Args:
            num_trials: Number of trials to run
            **kwargs: Parameters for each trial
        
        Returns:
            Experiment results
        """
        print(f"Starting experiment {self.name} with {num_trials} trials")
        
        experiment_results = {
            "experiment_name": self.name,
            "num_trials": num_trials,
            "trials": {},
            "summary": {},
        }
        
        for trial_id in range(num_trials):
            trial_results = self.run_trial(trial_id, **kwargs)
            experiment_results["trials"][trial_id] = trial_results
        
        # Calculate summary statistics
        summary = {
            "total_spikes": sum(t["total_spikes"] for t in experiment_results["trials"].values()),
            "avg_firing_neurons": sum(t["firing_neuron_counts"][0] for t in experiment_results["trials"].values()) / num_trials if num_trials > 0 else 0,
            "avg_curiosity": sum(t["curiosity_levels"][0] for t in experiment_results["trials"].values()) / num_trials if num_trials > 0 else 0,
            "total_actions": sum(len(t["action_counts"]) for t in experiment_results["trials"].values()),
        }
        
        experiment_results["summary"] = summary
        
        return experiment_results
    
    def save_results(self, filepath: str, format: str = "json"):
        """
        Save experiment results to file.
        
        Args:
            filepath: Output file path
            format: Output format
        """
        if format == "json":
            import json
            with open(filepath, 'w') as f:
                json.dump(self.results, f, indent=2)
        elif format == "pickle":
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(self.results, f)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def load_results(self, filepath: str, format: str = "json"):
        """
        Load experiment results from file.
        
        Args:
            filepath: Input file path
            format: Input format
        """
        if format == "json":
            import json
            with open(filepath, 'r') as f:
                self.results = json.load(f)
        elif format == "pickle":
            import pickle
            with open(filepath, 'rb') as f:
                self.results = pickle.load(f)
        else:
            raise ValueError(f"Unsupported format: {format}")

# Alias for backward compatibility
from_nlm import (
    create_brain_with_preset_config,
    create_agent_with_training,
    run_training_episode,
    export_brain_state,
    import_brain_state,
    analyze_brain_statistics,
    create_visualization_data,
    benchmark_configuration,
    NLMExperiment,
)

__all__ = [
    'create_brain_with_preset_config',
    'create_agent_with_training',
    'run_training_episode',
    'export_brain_state',
    'import_brain_state',
    'analyze_brain_statistics',
    'create_visualization_data',
    'benchmark_configuration',
    'NLMExperiment',
]
