#!/usr/bin/env python3
"""
Enhanced NLM Python API - Advanced Features for NLM Users

This module provides an enhanced interface to the NLM neural simulation framework
with additional functionality for advanced users and easier usage for beginners.

Features:
- Enhanced configuration with validation
- Advanced performance monitoring
- Simplified world creation and management
- Comprehensive error handling
- Development tools and debugging utilities
- Performance optimization helpers
"""

import sys
import json
import time
from typing import Optional, Dict, List, Any, Callable, Union
from enum import Enum

# Try to import NLM bindings, with helpful error messages
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm module not available: {e}")
    print("Please install the NLM Python bindings with: pip install -e .")

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors"""
    pass

class PerformanceError(NLMError):
    """Performance-related errors"""
    pass

class EnvironmentError(NLMError):
    """Environment-related errors"""
    pass

class NLMConfig:
    """Enhanced configuration manager with validation and convenience methods"""
    
    def __init__(self, config=None):
        if not PYNLM_AVAILABLE:
            raise NLMError("NLM Python bindings not available")
        
        if config is None:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = config
        
        self._setup_default_config()
    
    def _setup_default_config(self):
        """Set up sensible defaults for various use cases"""
        # Get current config to avoid overwriting existing values
        if not self.config.has("brain.neuron_count"):
            self.config.set("brain.neuron_count", 1000)
        
        if not self.config.has("simulation_timestep"):
            self.config.set("simulation_timestep", 0.001)
        
        # Enhanced configuration for better performance
        self.config.set("performance.enable_memory_pools", True)
        self.config.set("performance.enable_simd", True)
        self.config.set("performance.thread_count", 1)
        
        # Configuration for learning and development
        self.config.set("development.enable_deformation_adaptation", False)
        self.config.set("development.deformation_sensitivity", 0.1)
        self.config.set("development.development_sensitivity", 0.5)
        
        # Configuration for exploration and curiosity
        self.config.set("neuromodulation.curiosity_threshold", 0.5)
        self.config.set("neuromodulation.novelty_decay_rate", 0.99)
    
    def set_brain_config(self, neuron_count: int = 1000, regions: int = 1,
                         connection_probability: float = 0.1):
        """Configure basic brain parameters"""
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.region_count", regions)
        self.config.set("brain.connection_probability", connection_probability)
        return self
    
    def set_plasticity_config(self, learning_rate: float = 0.01,
                               stdp_ltp: float = 0.01, stdp_ltd: float = 0.012,
                               stdp_tau: float = 20.0):
        """Configure plasticity parameters"""
        self.config.set("plasticity.learning_rate", learning_rate)
        self.config.set("plasticity.stdp_ltp_weight", stdp_ltp)
        self.config.set("plasticity.stdp_ltd_weight", stdp_ltd)
        self.config.set("plasticity.stdp_time_constant", stdp_tau)
        return self
    
    def set_development_config(self, synaptogenesis_rate: float = 0.001,
                               pruning_rate: float = 0.0001,
                               sensitivity: float = 0.5):
        """Configure development parameters"""
        self.config.set("development.synaptogenesis_rate", synaptogenesis_rate)
        self.config.set("development.pruning_rate", pruning_rate)
        self.config.set("development.development_sensitivity", sensitivity)
        return self
    
    def set_neuromodulation_config(self, dopamine_baseline: float = 0.1,
                                   dopamine_scale: float = 1.0,
                                   curiosity_threshold: float = 0.5):
        """Configure neuromodulation parameters"""
        self.config.set("neuromodulation.dopamine_baseline", dopamine_baseline)
        self.config.set("neuromodulation.dopamine_scale", dopamine_scale)
        self.config.set("neuromodulation.curiosity_threshold", curiosity_threshold)
        return self
    
    def set_environment_config(self, width: int = 10, height: int = 10,
                              vision_width: int = 8, vision_height: int = 8):
        """Configure environment parameters"""
        self.config.set("environment.width", width)
        self.config.set("environment.height", height)
        self.config.set("environment.vision_width", vision_width)
        self.config.set("environment.vision_height", vision_height)
        return self
    
    def set_performance_config(self, enable_memory_pools: bool = True,
                               enable_simd: bool = True,
                               thread_count: int = 1):
        """Configure performance parameters"""
        self.config.set("performance.enable_memory_pools", enable_memory_pools)
        self.config.set("performance.enable_simd", enable_simd)
        self.config.set("performance.thread_count", thread_count)
        return self
    
    def load_from_file(self, filepath: str):
        """Load configuration from JSON file with validation"""
        try:
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            
            # Apply configuration
            for key, value in file_config.items():
                self.config.set(key, value)
            
            print(f"Configuration loaded from {filepath}")
            return self
            
        except FileNotFoundError:
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        except json.JSONDecodeError as e:
            raise ConfigurationError(f"Invalid JSON in configuration file: {e}")
        except Exception as e:
            raise ConfigurationError(f"Error loading configuration: {e}")
    
    def save_to_file(self, filepath: str):
        """Save configuration to JSON file"""
        try:
            # Extract configuration as dictionary
            config_dict = self.get_config_dict()
            
            with open(filepath, 'w') as f:
                json.dump(config_dict, f, indent=2)
            
            print(f"Configuration saved to {filepath}")
            return self
            
        except Exception as e:
            raise ConfigurationError(f"Error saving configuration: {e}")
    
    def get_config_dict(self) -> Dict[str, Any]:
        """Get configuration as dictionary"""
        return {
            'brain.neuron_count': self.config.getOr<int64_t>('brain.neuron_count', 1000),
            'brain.region_count': self.config.getOr<int64_t>('brain.region_count', 1),
            'brain.connection_probability': self.config.getOr<float>('brain.connection_probability', 0.1),
            'simulation_timestep': self.config.getOr<double>('simulation_timestep', 0.001),
            'plasticity.learning_rate': self.config.getOr<float>('plasticity.learning_rate', 0.01),
            'plasticity.stdp_ltp_weight': self.config.getOr<float>('plasticity.stdp_ltp_weight', 0.01),
            'plasticity.stdp_ltd_weight': self.config.getOr<float>('plasticity.stdp_ltd_weight', 0.012),
            'plasticity.stdp_time_constant': self.config.getOr<float>('plasticity.stdp_time_constant', 20.0),
            'development.synaptogenesis_rate': self.config.getOr<float>('development.synaptogenesis_rate', 0.001),
            'development.pruning_rate': self.config.getOr<float>('development.pruning_rate', 0.0001),
            'development.development_sensitivity': self.config.getOr<float>('development.development_sensitivity', 0.5),
            'neuromodulation.dopamine_baseline': self.config.getOr<float>('neuromodulation.dopamine_baseline', 0.1),
            'neuromodulation.dopamine_scale': self.config.getOr<float>('neuromodulation.dopamine_scale', 1.0),
            'neuromodulation.curiosity_threshold': self.config.getOr<float>('neuromodulation.curiosity_threshold', 0.5),
            'neuromodulation.novelty_decay_rate': self.config.getOr<float>('neuromodulation.novelty_decay_rate', 0.99),
            'environment.width': self.config.getOr<int64_t>('environment.width', 10),
            'environment.height': self.config.getOr<int64_t>('environment.height', 10),
            'performance.enable_memory_pools': self.config.getOr<bool>('performance.enable_memory_pools', True),
            'performance.enable_simd': self.config.getOr<bool>('performance.enable_simd', True),
            'performance.thread_count': self.config.getOr<int64_t>('performance.thread_count', 1),
        }
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors (empty if valid)"""
        errors = []
        
        # Validate neuron count
        neuron_count = self.config.getOr<int64_t>('brain.neuron_count', 0)
        if neuron_count < 10 or neuron_count > 100000:
            errors.append(f"Neuron count {neuron_count} is out of valid range [10, 100000]")
        
        # Validate region count
        region_count = self.config.getOr<int64_t>('brain.region_count', 0)
        if region_count < 1 or region_count > 100:
            errors.append(f"Region count {region_count} is out of valid range [1, 100]")
        
        # Validate connection probability
        conn_prob = self.config.getOr<float>('brain.connection_probability', 0.0)
        if conn_prob < 0.0 or conn_prob > 1.0:
            errors.append(f"Connection probability {conn_prob} is out of valid range [0.0, 1.0]")
        
        # Validate timestep
        timestep = self.config.getOr<double>('simulation_timestep', 0.0)
        if timestep <= 0.0 or timestep > 1.0:
            errors.append(f"Simulation timestep {timestep} is out of valid range (0, 1.0]")
        
        return errors
    
    def __repr__(self):
        return f"NLMConfig(neuron_count={self.config.getOr<int64_t>('brain.neuron_count', 1000)})"


class EnhancedNLMWorld:
    """Enhanced world interface with convenience methods"""
    
    def __init__(self, brain=None, config=None):
        if not PYNLM_AVAILABLE:
            raise NLMError("NLM Python bindings not available")
        
        # Create brain if not provided
        if brain is None:
            if config is None:
                config = NLMConfig()
            brain = pynlm.createBrain(config.config)
            brain.initialize()
        
        self.brain = brain
        self.agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        self.world = pynlm.createSimpleWorld()
        self.config = config
        
        # Apply environment configuration if available
        if config:
            env_config = config.get_config_dict()
            width = env_config.get('environment.width', 10)
            height = env_config.get('environment.height', 10)
            vision_width = env_config.get('environment.vision_width', 8)
            vision_height = env_config.get('environment.vision_height', 8)
            
            self.world.configure(width, height, vision_width, vision_height)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
    
    def setup_agent(self, x: float = None, y: float = None,
                    enable_reward_modulation: bool = True,
                    enable_structural_plasticity: bool = True,
                    enable_development: bool = True,
                    enable_curiosity: bool = True):
        """Setup agent with world"""
        if x is not None and y is not None:
            self.world.setAgentStart(x, y)
        
        # Enable subsystems
        if enable_reward_modulation:
            self.agent.enableRewardModulation(True)
        
        if enable_structural_plasticity:
            self.agent.enableStructuralPlasticity(True)
        
        if enable_development:
            self.agent.enableDevelopment(True)
        
        if enable_curiosity:
            self.agent.enableCuriosity(True)
        
        return self
    
    def run_episode(self, num_steps: int = 1000, visualize: bool = False,
                    callback: Optional[Callable] = None):
        """Run a complete episode with the agent"""
        if not PYNLM_AVAILABLE:
            raise NLMError("NLM Python bindings not available")
        
        # Reset agent for new episode
        self.agent.reset()
        
        print(f"Starting episode with {num_steps} steps")
        
        for step in range(num_steps):
            try:
                # Update world
                self.world.update(0.1)
                
                # Get sensory input from world
                percept = self.world.getSensoryPercept()
                
                # Process sensory input in brain
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode motor command from brain activity
                motor_cmd = self.agent.decodeMotorCommand()
                
                # Apply motor command to world
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = 0.0
                if percept.getInternal() and len(percept.getInternal()) > 0:
                    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Call callback if provided
                if callback:
                    callback(self, step)
                
                # Print progress every 100 steps
                if step % 100 == 0:
                    stats = self.get_stats()
                    print(f"Step {step}: {stats}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                break
        
        print(f"Episode completed with {step + 1} steps")
        return step + 1
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current statistics from brain and agent"""
        return {
            'step': int(self.world.getSimulationTime() / 0.1),  # Approximate
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'development_stage': int(self.agent.getDevelopmentalStage()),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'world_time': self.world.getSimulationTime(),
            'agent_x': self.world.getAgentBody().x,
            'agent_y': self.world.getAgentBody().y,
            'agent_energy': self.world.getAgentBody().energy,
            'agent_health': self.world.getAgentBody().health,
        }
    
    def save_state(self, filepath: str):
        """Save brain state to file"""
        return self.brain.save(filepath)
    
    def load_state(self, filepath: str):
        """Load brain state from file"""
        return self.brain.load(filepath)
    
    def __repr__(self):
        return f"EnhancedNLMWorld(neurons={self.brain.getTotalNeuronCount()}, firing={self.brain.getFiringNeuronCount()})"


class PerformanceMonitor:
    """Performance monitoring and optimization tools"""
    
    def __init__(self, brain=None):
        if brain is None and PYNLM_AVAILABLE:
            raise NLMError("Brain instance required for performance monitoring")
        
        self.brain = brain
        self.metrics = {
            'steps': 0,
            'start_time': time.time(),
            'neuron_count': 0,
            'synapse_count': 0,
            'total_spikes': 0,
            'max_firing_rate': 0.0,
            'memory_usage': 0,
            'cpu_usage': 0.0,
        }
        
        if self.brain:
            self.metrics['neuron_count'] = self.brain.getTotalNeuronCount()
            self.metrics['synapse_count'] = self.brain.getTotalSynapseCount()
    
    def record_step(self):
        """Record a step for performance tracking"""
        self.metrics['steps'] += 1
        self.metrics['total_spikes'] += self.brain.getFiringNeuronCount()
        
        # Calculate firing rate
        if self.metrics['steps'] > 0:
            self.metrics['max_firing_rate'] = max(
                self.metrics['max_firing_rate'],
                self.brain.getAverageFiringRate()
            )
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics"""
        elapsed_time = time.time() - self.metrics['start_time']
        
        if elapsed_time > 0:
            steps_per_second = self.metrics['steps'] / elapsed_time
            spikes_per_second = self.metrics['total_spikes'] / elapsed_time
        else:
            steps_per_second = 0.0
            spikes_per_second = 0.0
        
        return {
            'steps_completed': self.metrics['steps'],
            'elapsed_time_seconds': elapsed_time,
            'steps_per_second': steps_per_second,
            'spikes_per_second': spikes_per_second,
            'neurons': self.metrics['neuron_count'],
            'synapses': self.metrics['synapse_count'],
            'total_spikes': self.metrics['total_spikes'],
            'max_firing_rate': self.metrics['max_firing_rate'],
            'memory_usage_mb': self.metrics['memory_usage'] / (1024 * 1024),
            'cpu_usage_percent': self.metrics['cpu_usage'],
        }
    
    def export_metrics(self, filepath: str):
        """Export metrics to file"""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.get_performance_stats(), f, indent=2)
            print(f"Performance metrics saved to {filepath}")
        except Exception as e:
            raise PerformanceError(f"Error exporting metrics: {e}")


# Create enhanced convenience functions for common use cases

def create_quick_brain(neuron_count: int = 500, regions: int = 1):
    """Create a brain with common settings for quick experimentation"""
    config = NLMConfig()
    config.set_brain_config(neuron_count, regions)
    config.set_plasticity_config()
    
    brain = pynlm.createBrain(config.config)
    brain.initialize()
    return brain

def create_simple_experiment(brain=None, world_width: int = 10,
                            world_height: int = 10, num_steps: int = 1000):
    """Create a simple experiment setup"""
    if brain is None:
        brain = create_quick_brain()
    
    world = EnhancedNLMWorld(brain)
    world.setup_agent(enable_reward_modulation=True,
                     enable_structural_plasticity=False,
                     enable_development=False,
                     enable_curiosity=True)
    
    return world

def run_simple_experiment(world: EnhancedNLMWorld, num_steps: int = 1000):
    """Run a simple experiment with the given world"""
    print(f"Running simple experiment with {num_steps} steps")
    print(f"World configuration: {world}")
    
    start_time = time.time()
    steps_completed = world.run_episode(num_steps)
    
    elapsed_time = time.time() - start_time
    print(f"Experiment completed in {elapsed_time:.2f} seconds")
    print(f"Steps per second: {steps_completed / elapsed_time:.2f}")
    
    return world.get_stats()


# Initialize enhanced API

def initialize_enhanced_api():
    """Initialize the enhanced NLM API with all components"""
    global PYNLM_AVAILABLE
    
    if not PYNLM_AVAILABLE:
        print("Warning: Enhanced NLM API initialized without core bindings")
        return
    
    print("Enhanced NLM API initialized successfully")
    print("Available features:")
    print("  - NLMConfig: Enhanced configuration management")
    print("  - EnhancedNLMWorld: Simplified world interface")
    print("  - PerformanceMonitor: Performance tracking")
    print("  - Convenience functions for quick setup")
    
    return True

# Initialize on import
initialize_enhanced_api()

__all__ = [
    'NLMConfig',
    'EnhancedNLMWorld', 
    'PerformanceMonitor',
    'create_quick_brain',
    'create_simple_experiment',
    'run_simple_experiment',
    'NLMError',
    'ConfigurationError',
    'PerformanceError',
    'EnvironmentError',
]