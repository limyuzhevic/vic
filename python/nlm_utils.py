#!/usr/bin/env python3
"""
NLM Python Utilities - Enhanced API for NLM (Neural Learning Machine)

This module provides enhanced Python utilities for the NLM brain simulation framework,
extending the basic C++ bindings with more Pythonic interfaces and additional functionality.
"""

import json
import time
import threading
import multiprocessing
import sys
from typing import Dict, List, Optional, Any, Union, Callable
from dataclasses import dataclass, field
from enum import Enum

# Try to import the core NLM module
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None
class ExperimentType(Enum):
    """Types of experiments that can be run"""
    SIMULATION = "simulation"
    LEARNING = "learning"
    REWARD = "reward"
    NOVELTY = "novelty"
    DEVELOPMENT = "development"
class ConfigPresets:
    """Common configuration presets for different use cases"""
    
    @staticmethod
    def create_development_brain(neuron_count: int = 500) -> Dict[str, Any]:
        """Create a configuration optimized for developmental studies"""
        return {
            "brain.neuron_count": neuron_count,
            "brain.region_count": 3,
            "brain.connection_probability": 0.08,
            "plasticity.structural.enable": True,
            "plasticity.structural.growth_rate": 0.001,
            "development.critical_period": 100.0,
            "development.maturation_rate": 0.01,
            "neuromod.dopamine.scale": 0.8,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True
        }
    
    @staticmethod
    def create_learning_agent(neuron_count: int = 1000) -> Dict[str, Any]:
        """Create a configuration optimized for learning agents"""
        return {
            "brain.neuron_count": neuron_count,
            "brain.region_count": 5,
            "brain.connection_probability": 0.05,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.enable": True,
            "neuromod.reward.enable": True,
            "neuromod.prediction_error.enable": True,
            "memory.working.capacity": 100,
            "memory.episodic.capacity": 50
        }
    
    @staticmethod
    def create_reproduction_brain(neuron_count: int = 200) -> Dict[str, Any]:
        """Create a configuration optimized for reproduction studies"""
        return {
            "brain.neuron_count": neuron_count,
            "brain.region_count": 2,
            "brain.connection_probability": 0.03,
            "plasticity.structural.enable": True,
            "plasticity.structural.pruning_rate": 0.001,
            "development.maturation_rate": 0.005,
            "neuromod.dopamine.scale": 1.2,
            "neuromod.curiosity.enable": False,
            "neuromod.novelty.enable": False
        }
class NLMExperiment:
    """Enhanced experiment runner with advanced features"""
    
    def __init__(self, name: str, experiment_type: ExperimentType = ExperimentType.SIMULATION):
        if not PYNLM_AVAILABLE:
            raise RuntimeError("NLM Python bindings not available")
        
        self.name = name
        self.type = experiment_type
        self.brain = None
        self.world = None
        self.agent = None
        self.config = None
        self.is_running = False
        self.start_time = None
        self.end_time = None
        self.step_count = 0
        self.performance_metrics = []
        
        # Event callbacks
        self.step_callbacks = []
        self.error_callbacks = []
    
    def setup_brain(self, config_dict: Optional[Dict[str, Any]] = None, 
                    preset: str = "default") -> 'NLMExperiment':
        """Setup brain with enhanced configuration options"""
        if preset == "default":
            config = pynlm.createDefaultConfig()
        elif preset == "development":
            config = pynlm.createDefaultConfig()
            for key, value in ConfigPresets.create_development_brain().items():
                config.set(key, value)
        elif preset == "learning":
            config = pynlm.createDefaultConfig()
            for key, value in ConfigPresets.create_learning_agent().items():
                config.set(key, value)
        elif preset == "reproduction":
            config = pynlm.createDefaultConfig()
            for key, value in ConfigPresets.create_reproduction_brain().items():
                config.set(key, value)
        else:
            raise ValueError(f"Unknown preset: {preset}")
        
        # Apply custom config if provided
        if config_dict:
            for key, value in config_dict.items():
                config.set(key, value)
        
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        return self
    
    def setup_world(self, width: int = 20, height: int = 20, 
                   vision_width: int = 8, vision_height: int = 8) -> 'NLMExperiment':
        """Setup simple world with enhanced parameters"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        
        # Set random seed for reproducibility
        self.world.setRandomSeed(int(time.time() * 1000) % 1000)
        
        return self
    
    def setup_agent(self) -> 'NLMExperiment':
        """Setup agent brain interface"""
        if not self.brain:
            raise RuntimeError("Brain must be set up before creating agent")
        
        self.agent = pynlm.createAgentBrain(self.brain)
        return self
    
    def enable_all_subsystems(self) -> 'NLMExperiment':
        """Enable all optional subsystems for comprehensive simulation"""
        if not self.agent:
            raise RuntimeError("Agent must be set up before enabling subsystems")
        
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        return self
    
    def add_step_callback(self, callback: Callable) -> 'NLMExperiment':
        """Add a callback function that will be called on each step"""
        self.step_callbacks.append(callback)
        return self
    
    def run(self, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """Run the experiment with enhanced monitoring and callbacks"""
        if not all([self.brain, self.agent, self.world]):
            raise RuntimeError("Experiment not fully configured. Setup brain, world, and agent first.")
        
        self.is_running = True
        self.start_time = time.time()
        self.step_count = 0
        self.performance_metrics.clear()
        
        try:
            for step in range(num_steps):
                # Update world
                self.world.update(timestep)
                
                # Get and process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Get motor command and apply
                motor_cmd = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply reward modulation if enabled
                if self.agent.isRewardModulationEnabled():
                    # Extract reward from internal signals if available
                    internal_signals = percept.getInternal() if hasattr(percept, 'getInternal') else []
                    reward = internal_signals[0] if internal_signals else 0.0
                    self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development if enabled
                if self.agent.isDevelopmentEnabled():
                    self.agent.updateDevelopment(timestep)
                
                # Record performance metrics
                metrics = {
                    'step': step,
                    'time': self.world.getSimulationTime(),
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'curiosity': self.agent.getCuriosityLevel() if self.agent.isCuriosityEnabled() else 0.0,
                    'novelty': self.agent.getNoveltyLevel() if hasattr(self.agent, 'getNoveltyLevel') else 0.0,
                    'development_stage': self.brain.getDevelopmentalStage(),
                    'energy': self.world.getAgentBody().energy if hasattr(self.world, 'getAgentBody') else 0.0
                }
                self.performance_metrics.append(metrics)
                
                # Call step callbacks
                for callback in self.step_callbacks:
                    callback(step, metrics)
                
                self.step_count = step + 1
                
        except Exception as e:
            # Call error callbacks
            for callback in self.error_callbacks:
                callback(e, step)
            raise
        
        finally:
            self.is_running = False
            self.end_time = time.time()
        
        # Return summary
        duration = self.end_time - self.start_time
        return {
            'name': self.name,
            'type': self.type.value,
            'steps': self.step_count,
            'duration': duration,
            'steps_per_second': self.step_count / duration if duration > 0 else 0,
            'final_metrics': self.performance_metrics[-1] if self.performance_metrics else {},
            'average_metrics': self._compute_average_metrics()
        }
    
    def _compute_average_metrics(self) -> Dict[str, float]:
        """Compute average metrics over the experiment"""
        if not self.performance_metrics:
            return {}
        
        metrics = {}
        for key in self.performance_metrics[0].keys():
            if key not in ['step', 'time']:  # Skip non-numeric keys
                values = [m[key] for m in self.performance_metrics if isinstance(m.get(key), (int, float))]
                if values:
                    metrics[key] = sum(values) / len(values)
        
        return metrics
    
    def save_results(self, filepath: str) -> 'NLMExperiment':
        """Save experiment results to file"""
        results = {
            'name': self.name,
            'type': self.type.value,
            'steps': self.step_count,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'performance_metrics': self.performance_metrics,
            'final_config': self.config.summary() if self.config else None
        }
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        return self
class NLMBatchRunner:
    """Batch experiment runner for running multiple experiments in parallel"""
    
    def __init__(self, num_workers: Optional[int] = None):
        self.num_workers = num_workers or multiprocessing.cpu_count()
        self.experiments = []
    
    def add_experiment(self, experiment: NLMExperiment) -> 'NLMBatchRunner':
        """Add an experiment to the batch"""
        self.experiments.append(experiment)
        return self
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all experiments in parallel"""
        with multiprocessing.Pool(self.num_workers) as pool:
            results = pool.map(lambda exp: exp.run(100), self.experiments)
        return results
class NLMProfiler:
    """Performance profiler for NLM simulations"""
    
    def __init__(self):
        self.timings = {}
        self.memory_samples = []
        self.cpu_samples = []
    
    def start_timing(self, operation_name: str):
        """Start timing an operation"""
        self.timings[operation_name] = {'start': time.time(), 'samples': []}
    
    def stop_timing(self, operation_name: str) -> float:
        """Stop timing an operation and return duration"""
        if operation_name in self.timings:
            start = self.timings[operation_name]['start']
            duration = time.time() - start
            
            # Store sample
            self.timings[operation_name]['samples'].append(duration)
            return duration
        return 0.0
    
    def sample_memory(self):
        """Sample current memory usage (if available)"""
        try:
            import psutil
            process = psutil.Process()
            self.memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
        except ImportError:
            pass
    
    def get_summary(self) -> Dict[str, Any]:
        """Get profiling summary"""
        summary = {}
        
        for op_name, data in self.timings.items():
            samples = data['samples']
            if samples:
                summary[op_name] = {
                    'count': len(samples),
                    'total': sum(samples),
                    'average': sum(samples) / len(samples),
                    'min': min(samples),
                    'max': max(samples)
                }
        
        if self.memory_samples:
            summary['memory'] = {
                'samples': self.memory_samples,
                'average': sum(self.memory_samples) / len(self.memory_samples),
                'max': max(self.memory_samples)
            }
        
        return summary
def create_config_from_file(filepath: str) -> pynlm.Config:
    """Create a config object from a JSON file"""
    config = pynlm.createDefaultConfig()
    config.loadFromFile(filepath)
    return config
def save_config_to_file(config: pynlm.Config, filepath: str) -> None:
    """Save a config object to a JSON file"""
    config.saveToFile(filepath)
def benchmark_brain(config_dict: Dict[str, Any], num_steps: int = 100) -> Dict[str, Any]:
    """Benchmark brain performance with given configuration"""
    if not PYNLM_AVAILABLE:
        return {"error": "NLM not available"}
    
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile brain step performance
    times = []
    for step in range(num_steps):
        start = time.time()
        brain.step(step)
        times.append(time.time() - start)
    
    return {
        'steps': num_steps,
        'total_time': sum(times),
        'average_time': sum(times) / len(times),
        'min_time': min(times),
        'max_time': max(times),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount()
    }
if __name__ == "__main__":
    # Quick test
    if PYNLM_AVAILABLE:
        print("NLM Python Utilities - Enhanced API")
        print(f"NLM available: {PYNLM_AVAILABLE}")
        print("Available classes: ConfigPresets, NLMExperiment, NLMBatchRunner, NLMProfiler")
    else:
        print("NLM Python bindings not available. Install pynlm to use this module.")
