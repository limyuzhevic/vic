#!/usr/bin/env python3
"""
Advanced NLM Python API - Expert Features

This module provides advanced features for expert users working with NLM,
including performance monitoring, memory management, and advanced simulation control.
"""

import pynlm
import time
import threading
from typing import Dict, List, Callable, Optional, Any


class PerformanceMonitor:
    """Performance monitoring and profiling for NLM simulations."""
    
    def __init__(self):
        self.metrics = {}
        self.start_time = None
        self.step_count = 0
        self.lock = threading.Lock()
    
    def start(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        self.step_count = 0
        self.metrics = {}
    
    def step(self, brain, world, agent):
        """Record performance metrics for a simulation step."""
        with self.lock:
            self.step_count += 1
            
            # Record brain metrics
            if brain:
                self.metrics.setdefault('neurons', []).append(brain.getTotalNeuronCount())
                self.metrics.setdefault('synapses', []).append(brain.getTotalSynapseCount())
                self.metrics.setdefault('firing', []).append(brain.getFiringNeuronCount())
                self.metrics.setdefault('firing_rate', []).append(brain.getAverageFiringRate())
                self.metrics.setdefault('spikes', []).append(brain.getTotalSpikeCount())
            
            # Record world metrics
            if world:
                self.metrics.setdefault('energy', []).append(world.getMaxEnergy())
                self.metrics.setdefault('time', []).append(world.getSimulationTime())
            
            # Record agent metrics
            if agent:
                self.metrics.setdefault('curiosity', []).append(agent.getCuriosityLevel())
                self.metrics.setdefault('novelty', []).append(agent.getNoveltyLevel())
                self.metrics.setdefault('prediction', []).append(agent.getPredictionError())
                self.metrics.setdefault('neuromod', []).append(agent.getNeuromodulationLevel())
                self.metrics.setdefault('dev_stage', []).append(agent.getDevelopmentalStage())
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics."""
        with self.lock:
            result = {
                'steps': self.step_count,
                'duration': time.time() - self.start_time if self.start_time else 0.0
            }
            
            # Calculate statistics for each metric
            for key, values in self.metrics.items():
                if values:
                    result[f'{key}_mean'] = sum(values) / len(values)
                    result[f'{key}_min'] = min(values)
                    result[f'{key}_max'] = max(values)
            
            return result
    
    def get_detailed_metrics(self) -> Dict[str, List[float]]:
        """Get detailed time-series metrics."""
        with self.lock:
            return self.metrics.copy()


class MemoryManager:
    """Memory management utilities for NLM simulations."""
    
    def __init__(self, max_memory_mb: int = 1024):
        self.max_memory = max_memory_mb * 1024 * 1024  # Convert to bytes
        self.current_memory = 0
        self.allocation_callbacks = []
    
    def allocate_brain(self, config: pynlm.Config) -> pynlm.Brain:
        """Allocate a brain with memory tracking."""
        brain = pynlm.createBrain(config)
        
        # Estimate memory usage (rough approximation)
        neuron_estimate = config.get("brain.neuron_count", 1000) * 200  # ~200 bytes per neuron
        synapse_estimate = config.get("brain.neuron_count", 1000) * config.get("brain.synapse_density", 0.1) * 100  # ~100 bytes per synapse
        
        memory_estimate = neuron_estimate + synapse_estimate
        
        if self.current_memory + memory_estimate > self.max_memory:
            raise MemoryError(f"Memory limit exceeded: {self.current_memory + memory_estimate} > {self.max_memory}")
        
        self.current_memory += memory_estimate
        
        for callback in self.allocation_callbacks:
            callback('brain', memory_estimate, brain)
        
        return brain
    
    def deallocate_brain(self, brain: pynlm.Brain):
        """Deallocate a brain and free memory."""
        # Memory would be freed automatically by Python's GC
        # but we can track it here
        pass
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in bytes."""
        return self.current_memory
    
    def set_memory_limit(self, limit_mb: int):
        """Set memory limit in megabytes."""
        self.max_memory = limit_mb * 1024 * 1024


class SimulationController:
    """Advanced simulation control for expert users."""
    
    def __init__(self):
        self.simulation_threads = []
        self.stop_flags = []
    
    def run_parallel_simulations(self, configs: List[pynlm.Config], worlds: List[pynlm.SimpleWorld], 
                               steps: int, results_callback: Callable = None) -> List[Dict]:
        """Run multiple simulations in parallel."""
        import concurrent.futures
        
        def run_single_simulation(args):
            config, world, step = args
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentWithDefaults(brain, world)
            
            metrics = {
                'steps_completed': 0,
                'final_neurons': brain.getTotalNeuronCount(),
                'final_synapses': brain.getTotalSynapseCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage()
            }
            
            for i in range(step):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(i)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
                
                if agent.isRewardModulationEnabled():
                    agent.applyRewardModulation(1.0, 0.5)
                
                if results_callback:
                    results_callback(i, brain, world, agent, metrics)
            
            return metrics
        
        # Create simulation tasks
        tasks = list(zip(configs, worlds, [steps] * len(configs)))
        
        # Run in parallel
        with concurrent.futures.ThreadPoolExecutor(max_workers=len(configs)) as executor:
            futures = [executor.submit(run_single_simulation, task) for task in tasks]
            results = [future.result() for future in concurrent.futures.as_completed(futures)]
        
        return results
    
    def run_with_adaptation(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                          steps: int, adaptation_rules: Dict[str, Any]) -> Dict:
        """Run simulation with adaptive learning and parameter tuning."""
        metrics = {
            'step': 0,
            'curiosity': [],
            'novelty': [],
            'reward': [],
            'development': []
        }
        
        for step in range(steps):
            # Get current state
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction = agent.getPredictionError()
            dev_stage = agent.getDevelopmentalStage()
            
            metrics['step'] = step
            metrics['curiosity'].append(curiosity)
            metrics['novelty'].append(novelty)
            metrics['prediction'].append(prediction)
            metrics['development'].append(dev_stage)
            
            # Update world
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            agent.decodeMotorCommand()
            
            # Apply adaptation rules
            if adaptation_rules.get('adapt_curiosity', False):
                if curiosity > adaptation_rules.get('curiosity_threshold', 0.8):
                    agent.enableCuriosity(False)
            
            if adaptation_rules.get('adapt_development', False):
                if dev_stage == pynlm.DevelopmentalStage.Maturation:
                    agent.enableDevelopment(True)
            
            # Execute action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            if adaptation_rules.get('apply_reward', False):
                reward = adaptation_rules.get('base_reward', 1.0) * (1.0 + curiosity)
                agent.applyRewardModulation(reward, prediction)
                metrics['reward'].append(reward)
            
            # Update development
            if adaptation_rules.get('update_development', False):
                agent.updateDevelopment(0.1)
        
        return metrics
    
    def run_with_checkpoints(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                           steps: int, checkpoint_interval: int, checkpoint_dir: str = ".") -> List[Dict]:
        """Run simulation with automatic checkpointing."""
        import json
        import os
        
        checkpoints = []
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            agent.decodeMotorCommand()
            
            # Execute action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Checkpoint at intervals
            if step % checkpoint_interval == 0:
                checkpoint_data = {
                    'step': step,
                    'time': world.getSimulationTime(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'prediction': agent.getPredictionError(),
                    'dev_stage': agent.getDevelopmentalStage()
                }
                
                checkpoints.append(checkpoint_data)
                
                # Save to file
                filename = os.path.join(checkpoint_dir, f"checkpoint_step_{step}.json")
                with open(filename, 'w') as f:
                    json.dump(checkpoint_data, f, indent=2)
        
        return checkpoints


class ExpertTools:
    """Collection of expert tools for advanced NLM usage."""
    
    @staticmethod
    def create_parameter_search_space(base_config: pynlm.Config, parameter_ranges: Dict[str, tuple]) -> List[pynlm.Config]:
        """Create a grid of configurations for parameter search."""
        # This is a simplified implementation
        # In practice, you might want to use numpy or other libraries for this
        
        configs = []
        param_names = list(parameter_ranges.keys())
        
        # Generate all combinations (simplified)
        if param_names:
            param_name = param_names[0]
            min_val, max_val, step = parameter_ranges[param_name]
            
            values = []
            current = min_val
            while current <= max_val:
                values.append(current)
                current += step
            
            for value in values:
                config = pynlm.createConfigFromParams({param_name: value})
                config.set("brain.neuron_count", base_config.get("brain.neuron_count", 1000))
                configs.extend(ExpertTools.create_parameter_search_space(config, {})
        else:
            configs.append(base_config)
        
        return configs
    
    @staticmethod
    def benchmark_brain_creation(num_brains: int = 10, neurons: int = 1000) -> Dict[str, float]:
        """Benchmark brain creation performance."""
        import time
        
        start_time = time.time()
        
        for i in range(num_brains):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neurons)
            brain = pynlm.createBrain(config)
            brain.initialize()
        
        end_time = time.time()
        
        return {
            'total_time': end_time - start_time,
            'brains_per_second': num_brains / (end_time - start_time),
            'time_per_brain': (end_time - start_time) / num_brains
        }
    
    @staticmethod
    def validate_config_compatibility(config: pynlm.Config) -> List[str]:
        """Validate configuration and return warnings."""
        warnings = []
        
        # Check for incompatible combinations
        neuron_count = config.get("brain.neuron_count", 1000)
        synapse_density = config.get("brain.synapse_density", 0.1)
        
        expected_synapses = int(neuron_count * synapse_density)
        
        if expected_synapses > 100000:  # Arbitrary limit
            warnings.append("Large number of synapses may impact performance")
        
        # Check for extreme values
        v_thresh = config.get("brain.v_thresh", -50.0)
        if abs(v_thresh) > 100:
            warnings.append("Extreme threshold potential may affect stability")
        
        # Check development settings
        if config.get("development_enabled", True) and config.get("plasticity_rate", 0.01) > 0.1:
            warnings.append("High plasticity rate with development enabled may cause instability")
        
        return warnings
    
    @staticmethod
    def create_experiment_series(base_config: pynlm.Config, variations: List[Dict], 
                                world_template: pynlm.SimpleWorld) -> List[pynlm.Config]:
        """Create a series of configurations for experiments."""
        configs = []
        
        for variation in variations:
            config = pynlm.createConfigFromParams(variation)
            
            # Merge with base config (variation overrides)
            for key, value in base_config.getKeys():
                if not config.has(key):
                    config.set(key, value)
            
            configs.append(config)
        
        return configs


# Convenience functions for expert users

def create_advanced_simulation_setup(config_variations: List[Dict], 
                                    world_types: List[str],
                                    enable_all_features: bool = True):
    """Create multiple advanced simulation setups for comparison."""
    setups = []
    
    for config_var in config_variations:
        for world_type in world_types:
            brain, world, agent = pynlm.createCompleteSimulationSetup(world_type, 100, True)
            
            if enable_all_features:
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                agent.enableCuriosity(True)
            
            setups.append({
                'config': config_var,
                'world_type': world_type,
                'brain': brain,
                'world': world,
                'agent': agent,
                'setup_type': 'advanced'
            })
    
    return setups


def optimize_brain_for_task(brain: pynlm.Brain, task_characteristics: Dict[str, Any]) -> pynlm.Brain:
    """Optimize a brain for specific task characteristics."""
    # This is a placeholder for advanced optimization algorithms
    # In practice, this would involve more sophisticated techniques
    
    # Example optimization: adjust based on task complexity
    task_complexity = task_characteristics.get('complexity', 0.5)
    task_memory = task_characteristics.get('memory_requirement', 'medium')
    
    # Modify configuration
    config = brain.getConfig()
    
    # Adjust based on task
    if task_memory == 'high':
        config.set("brain.working_memory_size", 2000)
    elif task_memory == 'low':
        config.set("brain.working_memory_size", 100)
    
    if task_complexity > 0.7:
        config.set("brain.region_count", 4)
        config.set("plasticity_rate", 0.02)
    else:
        config.set("brain.region_count", 2)
        config.set("plasticity_rate", 0.005)
    
    return brain


def run_reproduction_experiment(baseline_brain: pynlm.Brain, 
                                baseline_world: pynlm.SimpleWorld,
                                baseline_agent: pynlm.AgentBrain,
                                num_steps: int) -> Dict:
    """Run a reproduction experiment for scientific validation."""
    results = {
        'baseline': {
            'neurons': baseline_brain.getTotalNeuronCount(),
            'synapses': baseline_brain.getTotalSynapseCount(),
            'final_firing_rate': baseline_brain.getAverageFiringRate(),
            'total_spikes': baseline_brain.getTotalSpikeCount()
        },
        'curiosity_trajectory': [],
        'novelty_trajectory': [],
        'prediction_trajectory': [],
        'development_trajectory': [],
        'reward_trajectory': []
    }
    
    for step in range(num_steps):
        # Run simulation
        baseline_world.update(0.1)
        baseline_agent.processSensoryInput(baseline_world.getSensoryPercept())
        baseline_brain.step(step)
        baseline_world.applyMotorCommand(baseline_agent.decodeMotorCommand(), baseline_world.getSimulationTime())
        
        # Record trajectories
        results['curiosity_trajectory'].append(baseline_agent.getCuriosityLevel())
        results['novelty_trajectory'].append(baseline_agent.getNoveltyLevel())
        results['prediction_trajectory'].append(baseline_agent.getPredictionError())
        results['development_trajectory'].append(baseline_agent.getDevelopmentalStage())
        
        if baseline_agent.isRewardModulationEnabled():
            baseline_agent.applyRewardModulation(1.0, 0.0)
            results['reward_trajectory'].append(1.0)
    
    return results

# Export advanced tools
__all__ = [
    'PerformanceMonitor',
    'MemoryManager', 
    'SimulationController',
    'ExpertTools',
    'create_advanced_simulation_setup',
    'optimize_brain_for_task',
    'run_reproduction_experiment'
]