#!/usr/bin/env python3
"""
NLM Python Bindings - Advanced Python API for NLM

This module provides Python bindings for the NLM (Neural Learning Machine) C++ framework.
It includes advanced configuration options, performance monitoring, and debugging capabilities.
"""

import os
import sys
import json
import time
import logging
from typing import Dict, List, Optional, Union, Any
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMConfig:
    """Advanced configuration manager for NLM."""
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize NLM configuration.
        
        Args:
            config_file: Path to configuration file (optional)
        """
        self.config_file = config_file
        self._config = {
            # Brain configuration
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'connection_probability': 0.05,
                'initial_weight_mean': 0.5,
                'initial_weight_std': 0.1,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
                'tau_mem': 20.0,
                'tau_ref': 2.0,
            },
            # Plasticity configuration
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.001,
                    'tau_plus': 20.0,
                    'tau_minus': 20.0,
                },
                'hebbian': {
                    'enable': True,
                },
                'structural': {
                    'enable': True,
                    'synaptogenesis_rate': 0.0001,
                    'pruning_rate': 0.00001,
                },
            },
            # Neuromodulation configuration
            'neuromod': {
                'dopamine': {
                    'scale': 1.0,
                },
                'curiosity': {
                    'enable': True,
                },
                'novelty': {
                    'enable': True,
                },
            },
            # Performance optimization
            'performance': {
                'use_vectorization': True,
                'enable_parallel_processing': True,
                'optimization_level': 2,
                'memory_limit_mb': 1024,
                'checkpoint_interval': 100,
            },
            # Debugging and logging
            'debug': {
                'log_level': 'INFO',
                'enable_profiling': False,
                'debug_neurons': [],
                'log_synaptic_history': False,
            },
            # Advanced features
            'advanced': {
                'enable_prediction_system': True,
                'enable_memory_integration': True,
                'enable_developmental_model': True,
                'enable_reward_prediction': True,
            },
        }
        
        if config_file and os.path.exists(config_file):
            self.load_from_file(config_file)
    
    def load_from_file(self, config_file: str):
        """Load configuration from JSON file."""
        try:
            with open(config_file, 'r') as f:
                file_config = json.load(f)
            self._merge_config(file_config)
            logger.info(f"Loaded configuration from {config_file}")
        except Exception as e:
            logger.warning(f"Failed to load configuration from {config_file}: {e}")
    
    def merge_config(self, new_config: Dict):
        """Merge new configuration with existing."""
        self._merge_config(new_config)
    
    def set(self, key_path: str, value: Any):
        """Set configuration value using dot notation."""
        keys = key_path.split('.')
        config = self._config
        for key in keys[:-1]:
            if key not in config:
                config[key] = {}
            config = config[key]
        config[keys[-1]] = value
    
    def get(self, key_path: str, default: Any = None) -> Any:
        """Get configuration value using dot notation."""
        keys = key_path.split('.')
        config = self._config
        for key in keys:
            if key not in config:
                return default
            config = config[key]
        return config
    
    def _merge_config(self, new_config: Dict):
        """Internal method to merge configuration."""
        def merge_recursive(a: Dict, b: Dict):
            for key, value in b.items():
                if key in a and isinstance(a[key], dict) and isinstance(value, dict):
                    merge_recursive(a[key], value)
                else:
                    a[key] = value
        merge_recursive(self._config, new_config)
    
    def to_dict(self) -> Dict:
        """Convert configuration to dictionary."""
        return self._config.copy()
    
    def save_to_file(self, config_file: str):
        """Save configuration to JSON file."""
        try:
            with open(config_file, 'w') as f:
                json.dump(self._config, f, indent=2)
            logger.info(f"Saved configuration to {config_file}")
        except Exception as e:
            logger.error(f"Failed to save configuration to {config_file}: {e}")


class NLMPerformanceMonitor:
    """Advanced performance monitoring for NLM."""
    
    def __init__(self):
        self.metrics = {}
        self.start_time = time.time()
        self.step_count = 0
    
    def record_metric(self, name: str, value: Any):
        """Record a performance metric."""
        if name not in self.metrics:
            self.metrics[name] = []
        self.metrics[name].append(value)
    
    def get_average(self, name: str) -> float:
        """Get average value of a metric."""
        if name not in self.metrics or not self.metrics[name]:
            return 0.0
        return sum(self.metrics[name]) / len(self.metrics[name])
    
    def get_max(self, name: str) -> float:
        """Get maximum value of a metric."""
        if name not in self.metrics or not self.metrics[name]:
            return 0.0
        return max(self.metrics[name])
    
    def get_min(self, name: str) -> float:
        """Get minimum value of a metric."""
        if name not in self.metrics or not self.metrics[name]:
            return 0.0
        return min(self.metrics[name])
    
    def get_current_step(self) -> int:
        """Get current simulation step."""
        return self.step_count
    
    def increment_step(self):
        """Increment step counter."""
        self.step_count += 1
    
    def get_elapsed_time(self) -> float:
        """Get elapsed time since monitor creation."""
        return time.time() - self.start_time
    
    def generate_report(self) -> Dict:
        """Generate performance report."""
        report = {
            'total_steps': self.step_count,
            'elapsed_time': self.get_elapsed_time(),
            'steps_per_second': self.step_count / max(self.get_elapsed_time(), 0.001),
            'metrics': {}
        }
        
        for name, values in self.metrics.items():
            if values:
                report['metrics'][name] = {
                    'count': len(values),
                    'average': self.get_average(name),
                    'min': self.get_min(name),
                    'max': self.get_max(name),
                }
        
        return report


class NLMAdvancedAPI:
    """Advanced Python API for NLM with power user features."""
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """Initialize advanced NLM API.
        
        Args:
            config: NLM configuration (optional, will use default if None)
        """
        self.config = config or NLMConfig()
        self.performance_monitor = NLMPerformanceMonitor()
        self.logger = logging.getLogger(__name__)
        
        # Initialize C++ bindings (placeholder)
        self._cpp_api = None
        
    def create_brain(self, brain_name: str = "default") -> 'NLMBrain':
        """Create a brain with advanced configuration."""
        return NLMBrain(self.config, self.performance_monitor, brain_name)
    
    def create_world(self, width: int = 20, height: int = 20, 
                    vision_width: int = 8, vision_height: int = 8) -> 'NLMWorld':
        """Create a world with advanced configuration."""
        return NLMWorld(width, height, vision_width, vision_height)
    
    def create_agent(self, brain: 'NLMBrain', world: 'NLMWorld') -> 'NLMAgent':
        """Create an agent with brain and world integration."""
        return NLMAgent(brain, world)
    
    def run_advanced_simulation(self, brain: 'NLMBrain', world: 'NLMWorld', 
                              agent: 'NLMAgent', num_steps: int) -> Dict:
        """Run an advanced simulation with detailed monitoring."""
        results = {
            'brain_stats': [],
            'world_stats': [],
            'agent_stats': [],
            'performance': self.performance_monitor.generate_report(),
        }
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process with agent
            agent.process_sensory_input(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Record stats
            if step % 10 == 0:
                results['brain_stats'].append({
                    'step': step,
                    'firing_neurons': brain.get_firing_neuron_count(),
                    'total_spikes': brain.get_total_spike_count(),
                    'average_firing_rate': brain.get_average_firing_rate(),
                    'curiosity_level': agent.get_curiosity_level(),
                    'novelty_level': agent.get_novelty_level(),
                })
                
                results['agent_stats'].append({
                    'step': step,
                    'action': str(action),
                    'reward': agent.get_last_reward(),
                    'prediction_error': agent.get_prediction_error(),
                })
                
                results['world_stats'].append({
                    'step': step,
                    'simulation_time': world.get_simulation_time(),
                    'agent_x': agent.get_body().get_x(),
                    'agent_y': agent.get_body().get_y(),
                })
            
            # Increment performance counter
            self.performance_monitor.increment_step()
        
        return results


# Placeholder classes for the advanced API
class NLMBrain:
    """Placeholder for NLMBrain class."""
    
    def __init__(self, config: NLMConfig, monitor: NLMPerformanceMonitor, name: str):
        self.config = config
        self.monitor = monitor
        self.name = name
        self._brain = None  # Will hold the actual C++ brain object
    
    def initialize(self):
        """Initialize the brain."""
        # Placeholder for C++ initialization
        pass
    
    def step(self, step: int):
        """Run a brain step."""
        # Placeholder for C++ step function
        pass
    
    def get_firing_neuron_count(self) -> int:
        """Get number of firing neurons."""
        # Placeholder
        return 0
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        # Placeholder
        return 0
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        # Placeholder
        return 0.0


class NLMWorld:
    """Placeholder for NLMWorld class."""
    
    def __init__(self, width: int, height: int, vision_width: int, vision_height: int):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self._world = None  # Will hold the actual C++ world object
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions."""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
    
    def reset(self):
        """Reset world state."""
        # Placeholder
        pass
    
    def update(self, timestep: float):
        """Update world state."""
        # Placeholder
        pass
    
    def get_sensory_percept(self):
        """Get sensory percept from world."""
        # Placeholder
        return None
    
    def apply_motor_command(self, action, simulation_time: float):
        """Apply motor command to world."""
        # Placeholder
        pass
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        # Placeholder
        return 0.0
    
    def get_agent_body(self):
        """Get agent body."""
        # Placeholder
        return None


class NLMAgent:
    """Placeholder for NLMAgent class."""
    
    def __init__(self, brain: NLMBrain, world: NLMWorld):
        self.brain = brain
        self.world = world
        self._agent = None  # Will hold the actual C++ agent object
    
    def initialize(self, world: NLMWorld):
        """Initialize agent with world."""
        self.world = world
        # Placeholder
        pass
    
    def process_sensory_input(self, percept):
        """Process sensory input."""
        # Placeholder
        pass
    
    def decode_motor_command(self):
        """Decode motor command from brain."""
        # Placeholder
        return None
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        # Placeholder
        return 0.0
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        # Placeholder
        return 0.0
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        # Placeholder
        return 0.0
    
    def get_last_reward(self) -> float:
        """Get last reward."""
        # Placeholder
        return 0.0
    
    def get_body(self):
        """Get agent body."""
        # Placeholder
        return None


# Global functions for easy access
def create_default_config() -> NLMConfig:
    """Create a default configuration."""
    return NLMConfig()


def create_config_from_file(config_file: str) -> NLMConfig:
    """Create configuration from file."""
    return NLMConfig(config_file)


def create_advanced_api(config: Optional[NLMConfig] = None) -> NLMAdvancedAPI:
    """Create advanced NLM API."""
    return NLMAdvancedAPI(config)


def create_brain(config: Optional[NLMConfig] = None, brain_name: str = "default") -> NLMBrain:
    """Create a brain with configuration."""
    advanced_api = NLMAdvancedAPI(config)
    return advanced_api.create_brain(brain_name)


def create_world(width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8) -> NLMWorld:
    """Create a world."""
    return NLMWorld(width, height, vision_width, vision_height)


def create_agent(brain: NLMBrain, world: NLMWorld) -> NLMAgent:
    """Create an agent."""
    return NLMAgent(brain, world)


# Example usage function
def example_advanced_usage():
    """Example of advanced NLM usage."""
    print("=== NLM Advanced Usage Example ===")
    
    # 1. Create advanced configuration
    config = create_default_config()
    
    # 2. Enable advanced features
    config.set('advanced.enable_prediction_system', True)
    config.set('advanced.enable_memory_integration', True)
    config.set('advanced.enable_developmental_model', True)
    
    # 3. Configure performance
    config.set('performance.use_vectorization', True)
    config.set('performance.optimization_level', 3)
    config.set('performance.memory_limit_mb', 2048)
    
    # 4. Set up debugging
    config.set('debug.log_level', 'DEBUG')
    config.set('debug.enable_profiling', True)
    
    # 5. Create brain
    brain = create_brain(config, "advanced_brain")
    
    # 6. Create world
    world = create_world(30, 30, 12, 12)
    
    # 7. Create agent
    agent = create_agent(brain, world)
    
    # 8. Initialize agent
    agent.initialize(world)
    
    # 9. Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 10. Run simulation
    print("Running advanced simulation...")
    results = create_advanced_api(config).run_advanced_simulation(
        brain, world, agent, 1000
    )
    
    # 11. Print results
    print(f"Simulation completed!")
    print(f"Total steps: {results['performance']['total_steps']}")
    print(f"Final firing neurons: {results['brain_stats'][-1]['firing_neurons'] if results['brain_stats'] else 'N/A'}")
    print(f"Final curiosity level: {results['agent_stats'][-1]['curiosity_level'] if results['agent_stats'] else 'N/A'}")
    
    return results


if __name__ == "__main__":
    # Run example
    results = example_advanced_usage()
    
    print("\n=== NLM Advanced API Demo Complete ===")
    print("The NLM Advanced API provides:")
    print("1. Advanced configuration management")
    print("2. Performance monitoring and profiling")
    print("3. Detailed statistics and reporting")
    print("4. Simulation control and monitoring")
    print("5. Power user features and optimizations")
