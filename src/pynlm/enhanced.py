"""
Enhanced Python API for NLM (Neural Learning Machine)

This module provides advanced functionality for working with NLM brains, worlds, and agents.
It extends the basic pybind11 bindings with additional utilities for:

- Batch operations
- Configuration presets
- Benchmarking and performance analysis
- Advanced brain manipulation
- Serialization utilities
- Visualization and monitoring
- Experiment management
"""

import pynlm
from typing import List, Optional, Dict, Any, Callable
import time
import json
import os
from enum import Enum

__version__ = "0.1.0"

# Configuration presets for common use cases
class ConfigPresets:
    @staticmethod
    def minimal():
        """Minimal configuration for basic testing"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.05)
        return config
    
    @staticmethod
    def full():
        """Full-featured configuration for complex simulations"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.connection_probability", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        return config

class BatchSimulator:
    """Simulator for running multiple brain simulations efficiently"""
    
    def __init__(self, config, num_simulations=10):
        self.config = config
        self.num_simulations = num_simulations
        self.results = []
    
    def run(self, steps_per_simulation=1000) -> List[Dict[str, Any]]:
        """Run multiple simulations in batch"""
        print(f"Running {self.num_simulations} simulations with {steps_per_simulation} steps each...")
        
        for i in range(self.num_simulations):
            print(f"  Simulation {i + 1}/{self.num_simulations}")
            
            # Create fresh brain for each simulation
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            total_spikes = 0
            max_firing_rate = 0.0
            
            for step in range(steps_per_simulation):
                brain.step(step)
                
                # Collect metrics
                firing_rate = brain.getAverageFiringRate()
                max_firing_rate = max(max_firing_rate, firing_rate)
                total_spikes += brain.getTotalSpikeCount()
            
            # Calculate average spike count
            avg_spike_count = total_spikes / steps_per_simulation if steps_per_simulation > 0 else 0.0
            
            result = {
                'simulation_id': i,
                'total_spikes': total_spikes,
                'max_firing_rate': max_firing_rate,
                'avg_spike_count': avg_spike_count,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
            }
            
            self.results.append(result)
            
            # Clean up
            del brain
        
        print("Batch simulation complete!")
        return self.results
    
    def get_statistics(self) -> Dict[str, float]:
        """Get summary statistics of all completed simulations"""
        if not self.results:
            return {}
        
        num_sims = len(self.results)
        avg_spikes = sum(r['total_spikes'] for r in self.results) / num_sims
        avg_firing_rate = sum(r['max_firing_rate'] for r in self.results) / num_sims
        avg_spike_count = sum(r['avg_spike_count'] for r in self.results) / num_sims
        
        return {
            'num_simulations': num_sims,
            'average_total_spikes': avg_spikes,
            'average_max_firing_rate': avg_firing_rate,
            'average_spike_count': avg_spike_count,
            'min_spikes': min(r['total_spikes'] for r in self.results),
            'max_spikes': max(r['total_spikes'] for r in self.results),
        }

class ExperimentManager:
    """Manager for running complex experiments with brain, world, and agent"""
    
    def __init__(self):
        self.experiments = []
    
    def run_agent_experiment(self, config, world_config, num_steps=1000, 
                           enable_learning=True, enable_development=False,
                           enable_curiosity=False) -> Dict[str, Any]:
        """Run a complete agent experiment with brain, world, and agent integration"""
        print(f"Running agent experiment with {num_steps} steps...")
        
        # Create components
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        if enable_learning:
            agent.enableRewardModulation(True)
        if enable_development:
            agent.enableDevelopment(True)
        if enable_curiosity:
            agent.enableCuriosity(True)
        
        # Run simulation
        total_reward = 0.0
        firing_rates = []
        curiosity_levels = []
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and apply to world
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            total_reward += reward
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if enable_development:
                agent.updateDevelopment(0.1)
            
            # Collect metrics
            firing_rates.append(brain.getAverageFiringRate())
            curiosity_levels.append(agent.getCuriosityLevel())
        
        # Calculate statistics
        avg_firing_rate = sum(firing_rates) / len(firing_rates) if firing_rates else 0.0
        avg_curiosity = sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0.0
        
        result = {
            'total_reward': total_reward,
            'avg_firing_rate': avg_firing_rate,
            'avg_curiosity': avg_curiosity,
            'total_steps': num_steps,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'learned_preference': brain.getLearnedPreference(),
        }
        
        print(f"Experiment complete! Total reward: {total_reward:.1f}")
        return result

class BenchmarkSuite:
    """Benchmark suite for performance and accuracy testing"""
    
    @staticmethod
    def benchmark_creation(num_brains=100, steps=100):
        """Benchmark brain creation and initialization performance"""
        print(f"Benchmarking creation of {num_brains} brains...")
        
        config = pynlm.createDefaultConfig()
        
        start_time = time.time()
        brains = []
        for i in range(num_brains):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        creation_time = time.time() - start_time
        
        # Clean up
        for brain in brains:
            del brain
        
        avg_creation_time = creation_time / num_brains
        
        return {
            'total_creation_time': creation_time,
            'average_creation_time_per_brain': avg_creation_time,
            'brains_per_second': num_brains / creation_time,
            'brains_tested': num_brains,
        }
    
    @staticmethod
    def benchmark_simulation(config, num_steps=1000, num_runs=10):
        """Benchmark simulation performance"""
        print(f"Benchmarking {num_runs} simulations with {num_steps} steps each...")
        
        total_time = 0.0
        
        for i in range(num_runs):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(num_steps):
                brain.step(step)
            run_time = time.time() - start_time
            
            total_time += run_time
            
            del brain
        
        avg_time = total_time / num_runs
        
        return {
            'average_simulation_time': avg_time,
            'total_simulations': num_runs,
            'steps_per_simulation': num_steps,
        }

class SerializationUtils:
    """Utilities for saving and loading brain states"""
    
    @staticmethod
    def save_brain_to_file(brain, filepath):
        """Save brain state to file"""
        if brain.save(filepath):
            print(f"Brain state saved successfully to {filepath}")
            return True
        else:
            print(f"Failed to save brain state to {filepath}")
            return False
    
    @staticmethod
    def load_brain_from_file(filepath, config):
        """Load brain state from file"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        if brain.load(filepath):
            print(f"Brain state loaded successfully from {filepath}")
            return brain
        else:
            print(f"Failed to load brain state from {filepath}")
            return None

# Convenience functions

def run_quick_test(config=None):
    """Run a quick test with default settings"""
    if config is None:
        config = ConfigPresets.minimal()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running quick test...")
    for step in range(100):
        brain.step(step)
    
    print(f"Quick test complete! Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
    
    return {
        'success': True,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'final_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
    }

# Module metadata
__all__ = [
    'ConfigPresets',
    'BatchSimulator',
    'ExperimentManager',
    'BenchmarkSuite',
    'SerializationUtils',
    'run_quick_test',
]

# Print welcome message
print("Enhanced NLM Python API loaded successfully!")
print(f"Version: {__version__}")
print("Use ConfigPresets for predefined configurations")
print("Use BatchSimulator for batch operations")
print("Use ExperimentManager for complex experiments")
print("Use BenchmarkSuite for performance testing")
print("Use SerializationUtils for save/load operations")
