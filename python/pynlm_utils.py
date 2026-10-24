"""
Pythonic utilities and helper functions for NLM (Neural Learning Machine).

This module provides convenient helper functions and utilities to make
working with NLM easier, more Pythonic, and more productive.
"""

import time
import json
from pathlib import Path
from typing import List, Dict, Any, Optional, Callable, Union, Iterator
import threading

import pynlm
def run_simulation(brain, steps: int, callback: Optional[Callable] = None) -> Dict[str, Any]:
    """
    Run a brain simulation with optional callback for progress updates.
    
    Args:
        brain: The NLM brain to simulate
        steps: Number of simulation steps to run
        callback: Optional function called every 10% of completion with progress info
    
    Returns:
        Dictionary with simulation results
    """
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if callback and (step + 1) % (steps // 10) == 0:
            progress = (step + 1) / steps
            callback({
                "step": step + 1,
                "progress": progress,
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "time_elapsed": time.time() - start_time,
            })
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    return {
        "total_steps": steps,
        "final_step": steps - 1,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "time_elapsed": elapsed,
        "steps_per_second": steps / elapsed if elapsed > 0 else 0,
    }
def run_agent_episode(agent, world, steps: int, callback: Optional[Callable] = None) -> Dict[str, Any]:
    """
    Run a complete agent-world episode.
    
    Args:
        agent: The AgentBrain instance
        world: The SimpleWorld instance
        steps: Number of steps to simulate
        callback: Optional function called every 10% of completion
    
    Returns:
        Dictionary with episode results
    """
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent.getBrain().step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (default 0.0, can be modified)
        reward = 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if callback and (step + 1) % (steps // 10) == 0:
            progress = (step + 1) / steps
            callback({
                "step": step + 1,
                "progress": progress,
                "firing_neurons": agent.getBrain().getFiringNeuronCount(),
                "total_spikes": agent.getBrain().getTotalSpikeCount(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "agent_stage": agent.getDevelopmentalStage(),
                "time_elapsed": time.time() - start_time,
            })
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    return {
        "total_steps": steps,
        "final_step": steps - 1,
        "total_spikes": agent.getBrain().getTotalSpikeCount(),
        "firing_neurons": agent.getBrain().getFiringNeuronCount(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError(),
        "agent_stage": agent.getDevelopmentalStage(),
        "time_elapsed": elapsed,
        "steps_per_second": steps / elapsed if elapsed > 0 else 0,
    }
def create_development_profile(base_config: Optional[pynlm.Config] = None, 
                              development_stage: Any = None) -> Dict[str, Any]:
    """
    Create a development profile with stage-specific settings.
    
    Args:
        base_config: Optional base configuration to modify
        development_stage: Target developmental stage (uses current if None)
    
    Returns:
        Dictionary with development settings
    """
    if base_config is None:
        base_config = pynlm.createDefaultConfig()
    
    # Create development profile based on current or specified stage
    stage = development_stage or pynlm.DevelopmentalStage.Initial
    
    profile = {
        "development_stage": stage,
        "plasticity_modifiers": {},
        "memory_settings": {},
        "behavior_settings": {},
    }
    
    # Stage-specific settings
    if stage == pynlm.DevelopmentalStage.Initial:
        profile["plasticity_modifiers"] = {"synaptogenesis": 1.0, "pruning": 0.1}
        profile["memory_settings"] = {"working_memory_capacity": 0.1, "episode_limit": 100}
        profile["behavior_settings"] = {"exploration": 0.8, "exploitation": 0.2}
    
    elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
        profile["plasticity_modifiers"] = {"synaptogenesis": 0.8, "pruning": 0.2}
        profile["memory_settings"] = {"working_memory_capacity": 0.2, "episode_limit": 200}
        profile["behavior_settings"] = {"exploration": 0.7, "exploitation": 0.3}
    
    elif stage == pynlm.DevelopmentalStage.Maturation:
        profile["plasticity_modifiers"] = {"synaptogenesis": 0.5, "pruning": 0.4}
        profile["memory_settings"] = {"working_memory_capacity": 0.3, "episode_limit": 300}
        profile["behavior_settings"] = {"exploration": 0.5, "exploitation": 0.5}
    
    elif stage == pynlm.DevelopmentalStage.Adult:
        profile["plasticity_modifiers"] = {"synaptogenesis": 0.2, "pruning": 0.6}
        profile["memory_settings"] = {"working_memory_capacity": 0.4, "episode_limit": 500}
        profile["behavior_settings"] = {"exploration": 0.3, "exploitation": 0.7}
    
    elif stage == pynlm.DevelopmentalStage.Aging:
        profile["plasticity_modifiers"] = {"synaptogenesis": 0.1, "pruning": 0.8}
        profile["memory_settings"] = {"working_memory_capacity": 0.3, "episode_limit": 200}
        profile["behavior_settings"] = {"exploration": 0.2, "exploitation": 0.8}
    
    return profile
def create_brain_from_preset(preset_name: str, custom_config: Optional[pynlm.Config] = None) -> pynlm.Brain:
    """
    Create a brain with preset configuration.
    
    Args:
        preset_name: Name of the preset ("simple", "advanced", "expert")
        custom_config: Optional custom configuration to override preset
    
    Returns:
        Configured NLM brain
    """
    presets = {
        "simple": {
            "brain.neuron_count": 100,
            "brain.synapse_density": 0.05,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "simulation_timestep": 0.001,
        },
        "advanced": {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development": True,
            "simulation_timestep": 0.001,
            "replay_interval": 100,
            "consolidation_interval": 1000,
        },
        "expert": {
            "brain.neuron_count": 5000,
            "brain.synapse_density": 0.15,
            "brain.connection_probability": 0.08,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.002,
            "plasticity.stdp.tau_plus": 20.0,
            "plasticity.stdp.tau_minus": 20.0,
            "plasticity.hebbian.enable": True,
            "plasticity.hebbian.learning_rate": 0.001,
            "plasticity.structural.enable": True,
            "plasticity.structural.synaptogenesis_rate": 0.0001,
            "plasticity.structural.pruning_rate": 0.00001,
            "neuromod.dopamine.scale": 2.0,
            "neuromod.curiosity.enable": True,
            "neuromod.curiosity.exploration_strength": 0.8,
            "neuromod.novelty.enable": True,
            "neuromod.novelty.sensitivity": 1.2,
            "development": True,
            "simulation_timestep": 0.001,
            "replay_interval": 50,
            "consolidation_interval": 500,
            "checkpoint.enabled": True,
            "checkpoint.path": "./checkpoints",
            "checkpoint.max_files": 10,
        },
    }
    
    if preset_name not in presets:
        raise ValueError(f"Unknown preset: {preset_name}. Available presets: {list(presets.keys())}")
    
    preset = presets[preset_name]
    
    if custom_config is None:
        config = pynlm.createDefaultConfig()
    else:
        config = custom_config
    
    # Apply preset settings
    for key, value in preset.items():
        config.set(key, value)
    
    return pynlm.createBrain(config)
def save_config_to_file(config: pynlm.Config, filepath: Union[str, Path], format: str = "json") -> bool:
    """
    Save configuration to file in various formats.
    
    Args:
        config: Configuration to save
        filepath: Path to save file
        format: Output format ("json", "yaml", "toml", "simple")
    
    Returns:
        True if successful, False otherwise
    """
    filepath = Path(filepath)
    
    try:
        if format == "simple":
            # Simple key=value format
            with open(filepath, 'w') as f:
                for key in config.getKeys():
                    value = config.getOr(key, "")
                    # Handle different value types
                    if isinstance(value, str):
                        f.write(f'{key} = "{value}"\n')
                    else:
                        f.write(f'{key} = {value}\n')
        
        elif format == "json":
            # JSON format
            data = {}
            for key in config.getKeys():
                value = config.getOr(key, "")
                # Convert to JSON-serializable type
                data[key] = value
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
        
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return True
        
    except Exception as e:
        print(f"Error saving config to {filepath}: {e}")
        return False
def load_config_from_file(filepath: Union[str, Path], format: str = "json") -> Optional[pynlm.Config]:
    """
    Load configuration from file in various formats.
    
    Args:
        filepath: Path to load file
        format: Input format ("json", "yaml", "toml", "simple")
    
    Returns:
        Loaded configuration or None if failed
    """
    filepath = Path(filepath)
    
    try:
        if not filepath.exists():
            print(f"File not found: {filepath}")
            return None
        
        config = pynlm.createDefaultConfig()
        
        if format == "simple":
            # Simple key=value format
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Remove quotes
                        if value.startswith('"') and value.endswith('"'):
                            value = value[1:-1]
                        elif value.startswith("'") and value.endswith("'"):
                            value = value[1:-1]
                        
                        config.set(key, value)
        
        elif format == "json":
            # JSON format
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            for key, value in data.items():
                config.set(key, value)
        
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return config
        
    except Exception as e:
        print(f"Error loading config from {filepath}: {e}")
        return None
class SimulationManager:
    """
    Manager for running and tracking simulations with advanced features.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                 agent: Optional[pynlm.AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.simulation_history: List[Dict[str, Any]] = []
        self.is_running = False
        self.current_thread: Optional[threading.Thread] = None
    
    def run_simulation(self, steps: int, callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run a simulation with tracking.
        
        Args:
            steps: Number of steps to run
            callback: Optional function called after each step
        
        Returns:
            Dictionary with simulation results
        """
        if self.is_running:
            raise RuntimeError("Simulation already running")
        
        self.is_running = True
        start_time = time.time()
        self.simulation_history = []
        
        try:
            for step in range(steps):
                self.brain.step(step)
                
                step_info = {
                    "step": step,
                    "time": step * self.brain.getConfig().getOr("simulation_timestep", 0.001),
                    "firing_neurons": self.brain.getFiringNeuronCount(),
                    "total_spikes": self.brain.getTotalSpikeCount(),
                    "avg_firing_rate": self.brain.getAverageFiringRate(),
                }
                
                self.simulation_history.append(step_info)
                
                if callback:
                    callback(step_info)
                
                # Small delay to prevent overwhelming the system
                time.sleep(0.001)
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            return {
                "total_steps": steps,
                "elapsed_time": elapsed,
                "steps_per_second": steps / elapsed if elapsed > 0 else 0,
                "total_spikes": self.brain.getTotalSpikeCount(),
                "final_firing_rate": self.brain.getAverageFiringRate(),
            }
            
        finally:
            self.is_running = False
    
    def get_simulation_stats(self) -> Dict[str, Any]:
        """
        Get statistics from the last simulation.
        
        Returns:
            Dictionary with simulation statistics
        """
        if not self.simulation_history:
            return {}
        
        firing_rates = [h["firing_neurons"] for h in self.simulation_history]
        
        return {
            "total_steps": len(self.simulation_history),
            "max_firing_neurons": max(firing_rates) if firing_rates else 0,
            "min_firing_neurons": min(firing_rates) if firing_rates else 0,
            "avg_firing_neurons": sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            "total_spikes": self.brain.getTotalSpikeCount(),
        }
    
    def reset_simulation_history(self):
        """Reset the simulation history."""
        self.simulation_history = []
class BrainProfiler:
    """
    Profiling utilities for brain performance analysis.
    """
    
    @staticmethod
    def benchmark_brain_creation(num_iterations: int = 100) -> Dict[str, float]:
        """
        Benchmark brain creation performance.
        
        Args:
            num_iterations: Number of iterations to run
        
        Returns:
            Dictionary with benchmark results
        """
        times = []
        
        for _ in range(num_iterations):
            start_time = time.perf_counter()
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            end_time = time.perf_counter()
            times.append(end_time - start_time)
        
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        
        return {
            "avg_creation_time_ms": avg_time * 1000,
            "min_creation_time_ms": min_time * 1000,
            "max_creation_time_ms": max_time * 1000,
            "std_dev_ms": (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5 * 1000,
        }
    
    @staticmethod
    def benchmark_simulation_steps(brain: pynlm.Brain, num_steps: int, 
                                  num_iterations: int = 10) -> Dict[str, float]:
        """
        Benchmark simulation step performance.
        
        Args:
            brain: Brain to benchmark
            num_steps: Number of steps per iteration
            num_iterations: Number of iterations
        
        Returns:
            Dictionary with benchmark results
        """
        times = []
        
        for _ in range(num_iterations):
            brain.reset()
            brain.initialize()
            
            start_time = time.perf_counter()
            for step in range(num_steps):
                brain.step(step)
            end_time = time.perf_counter()
            
            times.append(end_time - start_time)
        
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        
        steps_per_second = num_steps / avg_time
        
        return {
            "avg_time_seconds": avg_time,
            "min_time_seconds": min_time,
            "max_time_seconds": max_time,
            "steps_per_second": steps_per_second,
            "time_per_step_ms": avg_time * 1000 / num_steps,
        }
    
    @staticmethod
    def get_memory_usage() -> Dict[str, float]:
        """
        Get memory usage information.
        
        Returns:
            Dictionary with memory usage information (if psutil available)
        """
        try:
            import psutil
            import os
            
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            
            return {
                "rss_mb": memory_info.rss / 1024 / 1024,
                "vms_mb": memory_info.vms / 1024 / 1024,
                "shared_mb": memory_info.shared / 1024 / 1024,
                "text_mb": memory_info.text / 1024 / 1024,
                "data_mb": memory_info.data / 1024 / 1024,
                "dirty_mb": memory_info.dirty / 1024 / 1024,
            }
        except ImportError:
            return {"psutil_not_available": True}
class ConfigurationTemplates:
    """
    Advanced configuration templates for different use cases.
    """
    
    @staticmethod
    def create_research_config(experiment_name: str, 
                               neuron_count: int = 1000,
                               max_steps: int = 5000,
                               checkpoint_frequency: int = 100) -> pynlm.Config:
        """
        Create a configuration optimized for research experiments.
        
        Args:
            experiment_name: Name of the experiment
            neuron_count: Number of neurons
            max_steps: Maximum simulation steps
            checkpoint_frequency: Frequency of checkpoint saves
        
        Returns:
            Research-oriented configuration
        """
        config = pynlm.createDefaultConfig()
        
        # Research-focused settings
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.15)
        config.set("brain.connection_probability", 0.08)
        config.set("brain.initial_weight_mean", 0.5)
        config.set("brain.initial_weight_std", 0.2)
        
        # Plasticity settings
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
        config.set("plasticity.structural.pruning_rate", 0.00002)
        
        # Neuromodulation
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_strength", 0.7)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 1.0)
        
        # Development
        config.set("development", True)
        config.set("development.initial_plasticity", 1.0)
        config.set("development.critical_period_multiplier", 1.5)
        
        # Memory
        config.set("memory.working_memory.capacity", neuron_count // 10)
        config.set("memory.episodic.max_episodes", 1000)
        config.set("memory.associative.max_associations", 500)
        
        # Prediction
        config.set("prediction.enabled", True)
        config.set("prediction.error_threshold", 0.1)
        
        # Cognition
        config.set("cognition.planner.planning_depth", 5)
        config.set("cognition.concept.formation_threshold", 0.7)
        config.set("cognition.attention.inhibition_strength", 0.6)
        config.set("cognition.attention.excitation_strength", 1.4)
        
        # Simulation
        config.set("simulation_timestep", 0.001)
        config.set("max_steps", max_steps)
        config.set("checkpoint.enabled", True)
        config.set("checkpoint.path", f"./checkpoints/{experiment_name}")
        config.set("checkpoint.frequency", checkpoint_frequency)
        config.set("checkpoint.max_files", 20)
        
        # Replay and consolidation
        config.set("replay.enabled", True)
        config.set("replay.interval", 200)
        config.set("replay.consolidation_threshold", 0.3)
        config.set("consolidation.enabled", True)
        config.set("consolidation.interval", 2000)
        
        # Logging
        config.set("logging.level", "info")
        config.set("logging.file", f"./logs/{experiment_name}.log")
        config.set("logging.max_size_mb", 100)
        config.set("logging.backup_count", 5)
        
        # Experiment metadata
        config.set("experiment.name", experiment_name)
        config.set("experiment.author", "NLM Research Team")
        config.set("experiment.date", time.strftime("%Y-%m-%d %H:%M:%S"))
        config.set("experiment.version", "0.1.0")
        
        return config
    
    @staticmethod
    def create_real_time_config() -> pynlm.Config:
        """
        Create a configuration optimized for real-time applications.
        
        Returns:
            Real-time optimized configuration
        """
        config = pynlm.createDefaultConfig()
        
        # Real-time focused settings
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.08)
        config.set("simulation_timestep", 0.0005)  # Faster updates
        config.set("max_steps", 10000)
        
        # Fast plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.structural.enable", False)  # Skip structural for speed
        
        # Lightweight neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", False)  # Skip novelty for speed
        
        # Minimal development
        config.set("development", False)
        
        # Lightweight memory
        config.set("memory.working_memory.capacity", 100)
        config.set("memory.episodic.max_episodes", 50)
        config.set("replay.enabled", False)
        
        # Fast action selection
        config.set("action.selection.mode", "fast")
        config.set("action.planning.depth", 2)
        
        # Performance optimization
        config.set("performance.optimization", True)
        config.set("performance.use_gpu", False)  # No GPU for cross-platform
        config.set("performance.thread_count", 4)
        
        return config
    
    @staticmethod
    def create_education_config() -> pynlm.Config:
        """
        Create a configuration optimized for educational demonstrations.
        
        Returns:
            Education-oriented configuration
        """
        config = pynlm.createDefaultConfig()
        
        # Education-focused settings
        config.set("brain.neuron_count", 200)  # Smaller brain for easier understanding
        config.set("brain.synapse_density", 0.05)
        config.set("simulation_timestep", 0.01)  # Slower for better visibility
        config.set("max_steps", 2000)
        
        # Educational plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)  # Higher for visibility
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", False)  # Too complex for education
        
        # Simple neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # No development for simplicity
        config.set("development", False)
        
        # Simple memory
        config.set("memory.working_memory.capacity", 20)
        config.set("memory.episodic.max_episodes", 10)
        config.set("replay.enabled", False)
        
        # Educational features
        config.set("education.mode", True)
        config.set("education.highlight_neurons", True)
        config.set("education.show_plasticity", True)
        config.set("education.show_memory_tracing", True)
        config.set("education.slow_mode", True)
        
        return config
    
    @staticmethod
    def create_production_config() -> pynlm.Config:
        """
        Create a production-ready configuration for stable operation.
        
        Returns:
            Production-oriented configuration
        """
        config = pynlm.createDefaultConfig()
        
        # Production-focused settings
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.12)
        config.set("simulation_timestep", 0.001)
        config.set("max_steps", 50000)
        
        # Stable plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)  # Conservative
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Professional neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.8)
        
        # Controlled development
        config.set("development", True)
        config.set("development.plasticity_decay_rate", 0.05)
        
        # Production memory
        config.set("memory.working_memory.capacity", 500)
        config.set("memory.episodic.max_episodes", 1000)
        config.set("replay.enabled", True)
        config.set("replay.interval", 100)
        config.set("consolidation.enabled", True)
        config.set("consolidation.interval", 1000)
        
        # Production checkpointing
        config.set("checkpoint.enabled", True)
        config.set("checkpoint.path", "./production_checkpoints")
        config.set("checkpoint.frequency", 500)
        config.set("checkpoint.max_files", 100)
        
        # Production logging
        config.set("logging.level", "warning")  # Less verbose
        config.set("logging.file", "./logs/production.log")
        config.set("logging.max_size_mb", 500)
        config.set("logging.backup_count", 10)
        config.set("logging.performance_tracking", True)
        
        # Production security
        config.set("security.enable", True)
        config.set("security.checkpoint_encryption", True)
        config.set("security.access_control", True)
        
        return config
