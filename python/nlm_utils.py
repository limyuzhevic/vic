# NLM Python Utilities
# Utility functions and helper classes for working with NLM

import time
import numpy as np
from typing import List, Dict, Any, Optional, Callable
from enum import Enum

class SimulationRunner:
    """Helper class for running NLM simulations with common patterns."""
    
    def __init__(self, brain, world=None, agent=None):
        """
        Initialize a simulation runner.
        
        Args:
            brain: NLM Brain object
            world: Optional SimpleWorld object
            agent: Optional AgentBrain object
        """
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def run_basic_simulation(self, steps: int, timestep: float = 0.1):
        """
        Run a basic simulation without world interaction.
        
        Args:
            steps: Number of simulation steps
            timestep: Time between steps
        """
        print(f"Running basic simulation for {steps} steps...")
        
        for step in range(steps):
            self.brain.step(step)
            if step % 100 == 0:
                self._log_progress(step)
        
        self._log_final_stats()
        return steps
    
    def run_world_simulation(self, steps: int, timestep: float = 0.1):
        """
        Run a simulation with world interaction.
        
        Args:
            steps: Number of simulation steps
            timestep: Time between steps
        """
        if not self.world or not self.agent:
            raise ValueError("World and agent required for world simulation")
        
        print(f"Running world simulation for {steps} steps...")
        self.world.reset()
        
        for step in range(steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            if step % 100 == 0:
                self._log_world_progress(step)
        
        self._log_world_final_stats()
        return steps
    
    def _log_progress(self, step: int):
        """Log progress for basic simulation."""
        stats = {
            'Step': step,
            'Neurons Firing': self.brain.getFiringNeuronCount(),
            'Total Spikes': self.brain.getTotalSpikeCount(),
            'Avg Firing Rate': self.brain.getAverageFiringRate(),
            'Development Stage': str(self.brain.getDevelopmentalStage()),
            'Neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0,
            'Curiosity': self.agent.getCuriosityLevel() if self.agent else 0,
            'Novelty': self.agent.getNoveltyLevel() if self.agent else 0,
            'Prediction Error': self.agent.getPredictionError() if self.agent else 0,
        }
        print(f"  {stats}")
    
    def _log_world_progress(self, step: int):
        """Log progress for world simulation."""
        stats = {
            'Step': step,
            'Neurons Firing': self.brain.getFiringNeuronCount(),
            'Total Spikes': self.brain.getTotalSpikeCount(),
            'Agent X': self.world.getAgentBody().x,
            'Agent Y': self.world.getAgentBody().y,
            'Agent Orientation': self.world.getAgentBody().orientation,
            'Agent Energy': self.world.getAgentBody().energy,
            'Energy Decay': self.world.getEnergyDecayRate(),
            'Max Energy': self.world.getMaxEnergy(),
        }
        print(f"  {stats}")
    
    def _log_final_stats(self):
        """Log final statistics for basic simulation."""
        print("\n=== Final Simulation Statistics ===")
        print(f"Total Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Total Spikes: {self.brain.getTotalSpikeCount()}")
        print(f"Average Firing Rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"Excitatory/Inhibitory Ratio: {self.brain.getExcitationInhibitionRatio():.2f}")
        print(f"Developmental Stage: {self.brain.getDevelopmentalStage()}")
    
    def _log_world_final_stats(self):
        """Log final statistics for world simulation."""
        print("\n=== Final World Simulation Statistics ===")
        if self.world:
            body = self.world.getAgentBody()
            print(f"Final Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"Final Orientation: {body.orientation:.1f}°")
            print(f"Final Energy: {body.energy:.1f}/{self.world.getMaxEnergy()}")
            print(f"Simulation Time: {self.world.getSimulationTime():.1f}s")
        
        print(f"\nBrain Statistics:")
        print(f"  Total Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Total Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Total Spikes: {self.brain.getTotalSpikeCount()}")

class ExperimentRunner:
    """Helper class for running NLM experiments with different configurations."""
    
    @staticmethod
    def create_experiment_config(base_config=None, **kwargs):
        """
        Create an experiment configuration by modifying defaults.
        
        Args:
            base_config: Base configuration object
            **kwargs: Configuration options to override
            
        Returns:
            Modified configuration object
        """
        if base_config is None:
            from . import createDefaultConfig
            base_config = createDefaultConfig()
        
        # Apply configuration overrides
        for key, value in kwargs.items():
            base_config.set(key, value, source="experiment")
        
        return base_config
    
    @staticmethod
    def run_learning_experiment(brain, world, agent, duration: int, 
                              enable_learning: bool = True):
        """
        Run a learning experiment with reward feedback.
        
        Args:
            brain: NLM Brain object
            world: SimpleWorld object
            agent: AgentBrain object
            duration: Simulation duration in steps
            enable_learning: Whether to enable neuromodulation
            
        Returns:
            Dictionary with experiment results
        """
        # Enable learning features
        if enable_learning:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        # Run experiment
        rewards = []
        curiosities = []
        novelties = []
        predictions = []
        spikes = []
        
        world.reset()
        
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            if step % 10 == 0:  # Sample every 10 steps
                rewards.append(world.getAgentBody().energy)
                if agent:
                    curiosities.append(agent.getCuriosityLevel())
                    novelties.append(agent.getNoveltyLevel())
                    predictions.append(agent.getPredictionError())
                spikes.append(brain.getFiringNeuronCount())
        
        # Compile results
        results = {
            'steps': duration,
            'final_energy': world.getAgentBody().energy,
            'max_energy': world.getMaxEnergy(),
            'energy_efficiency': world.getAgentBody().energy / world.getMaxEnergy(),
            'mean_reward': np.mean(rewards) if rewards else 0,
            'max_reward': np.max(rewards) if rewards else 0,
            'min_reward': np.min(rewards) if rewards else 0,
            'std_reward': np.std(rewards) if len(rewards) > 1 else 0,
            'mean_curiosity': np.mean(curiosities) if curiosities else 0,
            'mean_novelty': np.mean(novelties) if novelties else 0,
            'mean_prediction_error': np.mean(predictions) if predictions else 0,
            'mean_spikes': np.mean(spikes) if spikes else 0,
            'final_development_stage': brain.getDevelopmentalStage(),
            'final_neuromodulation': agent.getNeuromodulationLevel() if agent else 0,
        }
        
        return results

class NLMConfig:
    """Convenience class for NLM configuration presets."""
    
    class ConfigPresets:
        @staticmethod
        def create_exploratory():
            """Create a configuration optimized for exploration."""
            from . import createDefaultConfig
            config = createDefaultConfig()
            config.set("curiosity_rate", 0.1, "preset")
            config.set("novelty_threshold", 0.05, "preset")
            config.set("reward_curiosity_balance", 0.7, "preset")
            return config
        
        @staticmethod
        def create_exploitive():
            """Create a configuration optimized for exploitation."""
            from . import createDefaultConfig
            config = createDefaultConfig()
            config.set("curiosity_rate", 0.01, "preset")
            config.set("novelty_threshold", 0.1, "preset")
            config.set("reward_curiosity_balance", 0.9, "preset")
            return config
        
        @staticmethod
        def create_balanced():
            """Create a balanced exploration/exploitation configuration."""
            from . import createDefaultConfig
            config = createDefaultConfig()
            config.set("curiosity_rate", 0.03, "preset")
            config.set("novelty_threshold", 0.07, "preset")
            config.set("reward_curiosity_balance", 0.8, "preset")
            return config
        
        @staticmethod
        def create_reproductive():
            """Create a conservative configuration focused on reproduction/survival."""
            from . import createDefaultConfig
            config = createDefaultConfig()
            config.set("curiosity_rate", 0.005, "preset")
            config.set("novelty_threshold", 0.15, "preset")
            config.set("reward_curiosity_balance", 0.95, "preset")
            return config
        
        @staticmethod
        def create_playful():
            """Create a highly exploratory configuration."""
            from . import createDefaultConfig
            config = createDefaultConfig()
            config.set("curiosity_rate", 0.2, "preset")
            config.set("novelty_threshold", 0.02, "preset")
            config.set("reward_curiosity_balance", 0.6, "preset")
            return config
    
    # Expose all presets
    exploratory = ConfigPresets.create_exploratory
    exploitive = ConfigPresets.create_exploitive
    balanced = ConfigPresets.create_balanced
    reproductive = ConfigPresets.create_reproductive
    playful = ConfigPresets.create_playful

# Utility functions

def benchmark_simulation(brain, world, agent, steps: int, 
                        timestep: float = 0.1, warmup: int = 100):
    """
    Benchmark a simulation and return performance metrics.
    
    Args:
        brain: NLM Brain object
        world: SimpleWorld object
        agent: AgentBrain object
        steps: Number of steps to benchmark
        timestep: Time between steps
        warmup: Number of warmup steps
        
    Returns:
        Dictionary with performance metrics
    """
    import time
    
    # Warmup
    brain.reset()
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    for step in range(warmup):
        world.update(timestep)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Benchmark
    start_time = time.time()
    
    brain.reset()
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    for step in range(steps):
        world.update(timestep)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    elapsed_time = end_time - start_time
    steps_per_second = steps / elapsed_time
    
    return {
        'steps_benchmarked': steps,
        'elapsed_time_seconds': elapsed_time,
        'steps_per_second': steps_per_second,
        'simulation_timestep': timestep,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
    }

def validate_configuration(config):
    """
    Validate an NLM configuration and return warnings/errors.
    
    Args:
        config: Configuration object
        
    Returns:
        Dictionary with validation results
    """
    warnings = []
    errors = []
    
    # Check critical parameters
    try:
        neuron_count = config.getOr[int64_t]("neuron_count", 100)
        if neuron_count < 10:
            warnings.append("Very low neuron count may not produce interesting behavior")
        elif neuron_count > 10000:
            warnings.append("Very high neuron count may impact performance")
    except:
        errors.append("Invalid or missing neuron_count parameter")
    
    try:
        timestep = config.getOr<double]("simulation_timestep", 0.001)
        if timestep <= 0 or timestep > 1.0:
            errors.append("simulation_timestep must be between 0 and 1.0 seconds")
    except:
        errors.append("Invalid or missing simulation_timestep parameter")
    
    try:
        connection_prob = config.getOr<float>("connection_probability", 0.1)
        if connection_prob < 0 or connection_prob > 1:
            errors.append("connection_probability must be between 0 and 1")
    except:
        errors.append("Invalid or missing connection_probability parameter")
    
    return {
        'valid': len(errors) == 0,
        'warnings': warnings,
        'errors': errors,
    }

# Export public API
__all__ = [
    'SimulationRunner',
    'ExperimentRunner', 
    'NLMConfig',
    'benchmark_simulation',
    'validate_configuration',
]

# Set module description for help()
__doc__ = """NLM Python Utilities

Additional utilities and helper classes for working with the NLM (Neural Learning Machine) neural simulation framework.

This module provides:

- SimulationRunner: High-level simulation management
- ExperimentRunner: Experiment configuration and execution
- NLMConfig: Configuration presets for different behaviors
- benchmark_simulation: Performance benchmarking utilities
- validate_configuration: Configuration validation

These utilities simplify common patterns and provide additional functionality
beyond the basic NLM bindings.

Examples:

    import pynlm
    from pynlm import SimulationRunner
    
    # Create and run a simulation
    brain = pynlm.createBrain()
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    runner = SimulationRunner(brain, world, agent)
    runner.run_world_simulation(1000)
    
    # Or use experiment runner
    from pynlm import ExperimentRunner
    results = ExperimentRunner.run_learning_experiment(brain, world, agent, 500)
    print(f"Final energy: {results['final_energy']:.1f}")
"""
