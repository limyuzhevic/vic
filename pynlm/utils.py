"""
Utility functions for NLM Python bindings.

This module provides common utility functions for working with NLM,
including file operations, data validation, and helper functions.
"""

import sys
import json
import os
from typing import Any, Dict, List, Optional, Union, Callable
import numpy as np

# Add parent directory to path for imports
if len(sys.path) == 1:
    sys.path.append("..")

from nlm import Config, WorldObject, WorldObjectType

class NLMUtils:
    """
    Utility class for common NLM operations.
    
    Provides utility functions for file operations, data validation,
    and helper functions commonly used with NLM simulations.
    """
    
    @staticmethod
    def validate_config(config: Dict[str, Any]) -> bool:
        """
        Validate configuration dictionary.
        
        Args:
            config: Configuration dictionary to validate
        
        Returns:
            True if valid, False otherwise
        """
        required_keys = ['brain.neuron_count', 'brain.region_count']
        
        for key in required_keys:
            if key not in config:
                return False
        
        # Validate value types
        if not isinstance(config['brain.neuron_count'], (int, float)):
            return False
        
        if not isinstance(config['brain.region_count'], (int, float)):
            return False
        
        return True
    
    @staticmethod
    def create_default_config_from_dict(config_dict: Dict[str, Any]) -> Config:
        """
        Create NLM Config from dictionary.
        
        Args:
            config_dict: Configuration dictionary
        
        Returns:
            Configured NLM Config object
        
        Raises:
            ValueError: If configuration is invalid
        """
        if not NLMUtils.validate_config(config_dict):
            raise ValueError("Invalid configuration dictionary")
        
        config = Config()
        
        # Set configuration values
        if 'brain.neuron_count' in config_dict:
            config.set('brain.neuron_count', config_dict['brain.neuron_count'])
        
        if 'brain.region_count' in config_dict:
            config.set('brain.region_count', config_dict['brain.region_count'])
        
        if 'brain.connection_probability' in config_dict:
            config.set('brain.connection_probability', config_dict['brain.connection_probability'])
        
        if 'brain.random_seed' in config_dict:
            config.set('brain.random_seed', config_dict['brain.random_seed'])
        
        if 'simulation.learning_rate' in config_dict:
            config.set('simulation.learning_rate', config_dict['simulation.learning_rate'])
        
        # Set any additional configuration
        for key, value in config_dict.items():
            if key not in ['brain.neuron_count', 'brain.region_count', 
                          'brain.connection_probability', 'brain.random_seed',
                          'simulation.learning_rate']:
                config.set(key, value)
        
        return config
    
    @staticmethod
    def load_config_from_json(filepath: str) -> Config:
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
        
        Returns:
            Loaded NLM Config object
        
        Raises:
            IOError: If file cannot be read
            ValueError: If JSON is invalid
        """
        try:
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            
            return NLMUtils.create_default_config_from_dict(config_dict)
            
        except IOError as e:
            raise IOError(f"Cannot read configuration file '{filepath}': {e}")
        except json.JSONDecodeError as e:
            raise ValueError(f"Invalid JSON in configuration file '{filepath}': {e}")
    
    @staticmethod
    def save_config_to_json(config: Config, filepath: str) -> bool:
        """
        Save configuration to JSON file.
        
        Args:
            config: NLM Config object
            filepath: Path to save JSON file
        
        Returns:
            True if successful, False otherwise
        """
        try:
            # Convert config to dictionary (this is a simplified version)
            config_dict = {}
            keys = config.get_keys()
            
            for key in keys:
                value = config.get_any(key)
                if value is not None:
                    config_dict[key] = value
            
            with open(filepath, 'w') as f:
                json.dump(config_dict, f, indent=2)
            
            return True
            
        except Exception as e:
            print(f"Warning: Failed to save configuration to JSON: {e}")
            return False
    
    @staticmethod
    def create_world_objects_from_dict(objects_dict: List[Dict[str, Any]]) -> List[WorldObject]:
        """
        Create world objects from dictionary list.
        
        Args:
            objects_dict: List of object dictionaries
        
        Returns:
            List of WorldObject objects
        """
        objects = []
        
        for obj_dict in objects_dict:
            x = obj_dict.get('x', 0.0)
            y = obj_dict.get('y', 0.0)
            radius = obj_dict.get('radius', 0.5)
            
            # Convert string type to WorldObjectType
            type_str = obj_dict.get('type', 'Empty')
            try:
                obj_type = WorldObjectType[type_str]
            except KeyError:
                obj_type = WorldObjectType.Empty
            
            value = obj_dict.get('value', 0.0)
            active = obj_dict.get('active', True)
            
            obj = WorldObject(x, y, obj_type, value, radius)
            obj.active = active
            objects.append(obj)
        
        return objects
    
    @staticmethod
    def validate_world_config(width: float, height: float,
                             vision_width: int, vision_height: int) -> bool:
        """
        Validate world configuration.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        
        Returns:
            True if valid, False otherwise
        """
        return (width > 0 and height > 0 and
                vision_width > 0 and vision_height > 0 and
                width >= vision_width and height >= vision_height)
    
    @staticmethod
    def run_simulation_with_validation(brain, world, num_steps: int,
                                     validate_callback: Optional[Callable] = None) -> bool:
        """
        Run simulation with optional validation callback.
        
        Args:
            brain: Brain object
            world: World object
            num_steps: Number of steps to simulate
            validate_callback: Optional validation callback function
        
        Returns:
            True if simulation completed successfully, False otherwise
        """
        try:
            for step in range(num_steps):
                # Update world
                world.update(0.1)
                
                # Get and process sensory input
                percept = world.getSensoryPercept()
                
                # Note: In real usage, you would need an AgentBrain object
                # This is a simplified version
                
                # Brain step
                brain.step(step)
                
                # Validation callback
                if validate_callback:
                    if not validate_callback(step, brain, world):
                        print(f"Validation failed at step {step}")
                        return False
            
            return True
            
        except Exception as e:
            print(f"Simulation error: {e}")
            return False
    
    @staticmethod
    def calculate_statistics(brain, world) -> Dict[str, Any]:
        """
        Calculate simulation statistics.
        
        Args:
            brain: Brain object
            world: World object
        
        Returns:
            Dictionary of statistics
        """
        stats = {
            'total_neurons': brain.get_total_neurons(),
            'total_synapses': brain.get_total_synapses(),
            'active_neurons': brain.get_active_neurons(),
            'firing_neurons': brain.get_firing_neurons(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'total_spikes': brain.get_total_spikes(),
            'pending_spikes': brain.get_pending_spikes(),
            'world_width': world.get_width(),
            'world_height': world.get_height(),
            'simulation_time': world.get_simulation_time()
        }
        
        return stats
    
    @staticmethod
    def format_statistics(stats: Dict[str, Any]) -> str:
        """
        Format statistics for display.
        
        Args:
            stats: Statistics dictionary
        
        Returns:
            Formatted statistics string
        """
        lines = ["Simulation Statistics:"]
        lines.append(f"  Neurons: {stats['total_neurons']}")
        lines.append(f"  Synapses: {stats['total_synapses']}")
        lines.append(f"  Active neurons: {stats['active_neurons']}")
        lines.append(f"  Firing neurons: {stats['firing_neurons']}")
        lines.append(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
        lines.append(f"  Total spikes: {stats['total_spikes']}")
        lines.append(f"  Pending spikes: {stats['pending_spikes']}")
        lines.append(f"  World size: {stats['world_width']}x{stats['world_height']}")
        lines.append(f"  Simulation time: {stats['simulation_time']:.2f}s")
        
        return "\n".join(lines)
    
    @staticmethod
    def validate_action(action: Any) -> bool:
        """
        Validate action object.
        
        Args:
            action: Action object to validate
        
        Returns:
            True if valid, False otherwise
        """
        return action is not None
    
    @staticmethod
    def create_experiment_summary(results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Create experiment summary from multiple simulation results.
        
        Args:
            results: List of simulation result dictionaries
        
        Returns:
            Experiment summary dictionary
        """
        if not results:
            return {}
        
        summary = {
            'total_simulations': len(results),
            'average_neurons': sum(r.get('total_neurons', 0) for r in results) / len(results),
            'average_firing_rate': sum(r.get('avg_firing_rate', 0) for r in results) / len(results),
            'average_spikes': sum(r.get('total_spikes', 0) for r in results) / len(results),
            'min_neurons': min(r.get('total_neurons', 0) for r in results),
            'max_neurons': max(r.get('total_neurons', 0) for r in results),
            'best_configuration': max(results, key=lambda x: x.get('total_spikes', 0))
        }
        
        return summary
    
    @staticmethod
    def print_experiment_summary(summary: Dict[str, Any]) -> None:
        """
        Print experiment summary.
        
        Args:
            summary: Experiment summary dictionary
        """
        if not summary:
            print("No experiment data to summarize.")
            return
        
        print("\nExperiment Summary:")
        print(f"  Total simulations: {summary['total_simulations']}")
        print(f"  Average neurons: {summary['average_neurons']:.1f}")
        print(f"  Average firing rate: {summary['average_firing_rate']:.2f} Hz")
        print(f"  Average spikes: {summary['average_spikes']:.1f}")
        print(f"  Min neurons: {summary['min_neurons']}")
        print(f"  Max neurons: {summary['max_neurons']}")
        
        if 'best_configuration' in summary:
            best = summary['best_configuration']
            print(f"  Best configuration: {best.get('config', 'Unknown')}")
            print(f"  Best spikes: {best.get('total_spikes', 0)}")

class NLMBenchmark:
    """
    Benchmark class for NLM performance testing.
    
    Provides benchmarking capabilities for measuring simulation performance
    and resource usage.
    """
    
    def __init__(self):
        self.results = []
    
    def run_benchmark(self, brain, world, steps: int, name: str = "") -> Dict[str, Any]:
        """
        Run benchmark test.
        
        Args:
            brain: Brain object
            world: World object
            steps: Number of steps to simulate
            name: Benchmark name
        """
        import time
        
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            # Note: In real usage, you would need an AgentBrain object
            brain.step(step)
        
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = steps / duration if duration > 0 else 0
        
        result = {
            'name': name,
            'steps': steps,
            'duration': duration,
            'steps_per_second': steps_per_second,
            'final_neurons': brain.get_total_neurons(),
            'final_spikes': brain.get_total_spikes()
        }
        
        self.results.append(result)
        return result
    
    def print_results(self) -> None:
        """Print benchmark results."""
        if not self.results:
            print("No benchmark results available.")
            return
        
        print("\nBenchmark Results:")
        for i, result in enumerate(self.results):
            print(f"  [{i+1}] {result['name']}:")
            print(f"      Steps: {result['steps']}")
            print(f"      Duration: {result['duration']:.2f}s")
            print(f"      Steps/sec: {result['steps_per_second']:.1f}")
            print(f"      Final neurons: {result['final_neurons']}")
            print(f"      Final spikes: {result['final_spikes']}")
    
    def get_average_steps_per_second(self) -> float:
        """Get average steps per second across all benchmarks."""
        if not self.results:
            return 0.0
        
        return sum(r['steps_per_second'] for r in self.results) / len(self.results)

# Convenience functions

def create_nlm_config(config_dict: Optional[Dict[str, Any]] = None) -> Config:
    """Create NLM configuration from dictionary."""
    if config_dict is None:
        return Config()
    return NLMUtils.create_default_config_from_dict(config_dict)

def create_nlm_brain(config: Optional[Config] = None) -> Any:
    """Create NLM brain with optional configuration."""
    if config is None:
        config = Config()
    from nlm import Brain
    return Brain(config)

def create_nlm_world() -> Any:
    """Create NLM world."""
    from nlm import SimpleWorld
    return SimpleWorld()

def run_simple_simulation(steps: int = 100, config: Optional[Config] = None) -> Dict[str, Any]:
    """
    Run a simple simulation with default settings.
    
    Args:
        steps: Number of simulation steps
        config: Optional configuration
    
    Returns:
        Dictionary with simulation results
    """
    if config is None:
        config = Config()
    
    brain = create_nlm_brain(config)
    world = create_nlm_world()
    
    world.configure(20.0, 20.0, 8, 8)
    world.reset()
    
    for step in range(steps):
        world.update(0.1)
        brain.step(step)
    
    stats = NLMUtils.calculate_statistics(brain, world)
    return stats