#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Core Config Module

This module provides configuration management for the NLM neural simulation framework.
It includes configuration validation, parameter management, preset configurations,
and configuration export/import functionality.

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

import json
import time
import threading
from typing import Dict, List, Any, Optional, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
import logging

# Import core types
from .types import *

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Version
__version__ = "0.1.0"

# Preset configurations
_PRESET_CONFIGS = {
    "minimal": {
        "neuron_count": 100,
        "max_steps": 100,
        "enable_visualization": False,
        "enable_performance_monitoring": False,
        "enable_profiling": False,
    },
    "standard": {
        "neuron_count": 1000,
        "max_steps": 1000,
        "enable_visualization": True,
        "enable_performance_monitoring": True,
        "enable_profiling": False,
    },
    "high_performance": {
        "neuron_count": 10000,
        "max_steps": 5000,
        "enable_visualization": True,
        "enable_performance_monitoring": True,
        "enable_profiling": True,
        "thread_safe": True,
        "precision_mode": "double",
    },
    "development": {
        "neuron_count": 500,
        "max_steps": 2000,
        "enable_development": True,
        "developmental_stage": DevelopmentalStage.JUVENILE,
        "enable_visualization": True,
        "enable_performance_monitoring": True,
        "enable_curiosity": True,
    },
}

# Create Config class with all methods
@dataclass
class Config:
    """Configuration for NLM simulation."""
    
    # Basic simulation parameters
    max_steps: int = 1000
    timestep: float = 0.1
    simulation_time: float = 100.0
    
    # Neural parameters
    neuron_count: int = 1000
    region_count: int = 10
    connection_probability: float = 0.1
    average_degree: float = 10.0
    threshold: float = -55.0
    resting_potential: float = -70.0
    
    # Plasticity parameters
    enable_stdp: bool = True
    enable_hebbian: bool = True
    stdp_learning_rate: float = 0.01
    hebbian_learning_rate: float = 0.005
    
    # Neuromodulation parameters
    enable_reward_modulation: bool = True
    enable_curiosity: bool = False
    enable_development: bool = True
    neuromodulation_strength: float = 1.0
    
    # Memory parameters
    enable_working_memory: bool = True
    enable_episodic_memory: bool = True
    enable_associative_memory: bool = True
    memory_capacity: int = 1000
    
    # Performance parameters
    thread_safe: bool = False
    precision_mode: str = "single"
    memory_pool_enabled: bool = False
    enable_performance_monitoring: bool = True
    enable_profiling: bool = False
    enable_visualization: bool = True
    
    # Development parameters
    enable_development: bool = True
    developmental_stage: DevelopmentalStage = DevelopmentalStage.ADULT
    developmental_speed: float = 1.0
    
    # Environment parameters
    environment_type: str = "simple"
    world_width: float = 100.0
    world_height: float = 100.0
    vision_width: int = 20
    vision_height: int = 20
    vision_range: float = 10.0
    
    # Learning parameters
    curiosity_threshold: float = 0.5
    reward_scale: float = 1.0
    novelty_threshold: float = 0.3
    learning_rate: float = 0.01
    discount_factor: float = 0.99
    
    def __post_init__(self):
        """Initialize configuration."""
        self._validate_config()
        
    def _validate_config(self) -> None:
        """Validate configuration parameters."""
        # Validate basic parameters
        if self.max_steps <= 0:
            raise ValueError("max_steps must be positive")
            
        if self.timestep <= 0:
            raise ValueError("timestep must be positive")
            
        if self.simulation_time <= 0:
            raise ValueError("simulation_time must be positive")
            
        if self.neuron_count <= 0:
            raise ValueError("neuron_count must be positive")
            
        if self.region_count <= 0:
            raise ValueError("region_count must be positive")
            
        if not (0.0 <= self.connection_probability <= 1.0):
            raise ValueError("connection_probability must be between 0 and 1")
            
        if self.threshold >= self.resting_potential:
            raise ValueError("threshold must be less than resting_potential")
            
        # Validate plasticity parameters
        if not (0.0 <= self.stdp_learning_rate <= 1.0):
            raise ValueError("stdp_learning_rate must be between 0 and 1")
            
        if not (0.0 <= self.hebbian_learning_rate <= 1.0):
            raise ValueError("hebbian_learning_rate must be between 0 and 1")
            
        # Validate neuromodulation parameters
        if not (0.0 <= self.neuromodulation_strength <= 10.0):
            raise ValueError("neuromodulation_strength must be between 0 and 10")
            
        # Validate memory parameters
        if self.memory_capacity <= 0:
            raise ValueError("memory_capacity must be positive")
            
        # Validate environment parameters
        if self.world_width <= 0:
            raise ValueError("world_width must be positive")
            
        if self.world_height <= 0:
            raise ValueError("world_height must be positive")
            
        if self.vision_width <= 0:
            raise ValueError("vision_width must be positive")
            
        if self.vision_height <= 0:
            raise ValueError("vision_height must be positive")
            
        # Validate precision mode
        if self.precision_mode not in ["single", "double", "half"]:
            raise ValueError("precision_mode must be 'single', 'double', or 'half'")
            
        # Validate development parameters
        if not (0.0 <= self.developmental_speed <= 10.0):
            raise ValueError("developmental_speed must be between 0 and 10")
            
        # Validate learning parameters
        if not (0.0 <= self.curiosity_threshold <= 1.0):
            raise ValueError("curiosity_threshold must be between 0 and 1")
            
        if not (0.0 <= self.novelty_threshold <= 1.0):
            raise ValueError("novelty_threshold must be between 0 and 1")
            
        if not (0.0 <= self.learning_rate <= 1.0):
            raise ValueError("learning_rate must be between 0 and 1")
            
        if not (0.0 <= self.discount_factor <= 1.0):
            raise ValueError("discount_factor must be between 0 and 1")
            
    def set(self, key: str, value: Any) -> None:
        """Set configuration parameter."""
        if hasattr(self, key):
            setattr(self, key, value)
            self._validate_config()
        else:
            raise ValueError(f"Unknown configuration parameter: {key}")
            
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration parameter."""
        return getattr(self, key, default)
        
    def validate(self) -> List[str]:
        """Validate configuration and return any errors found."""
        errors = []
        
        # Check required parameters
        required_params = ['max_steps', 'timestep', 'neuron_count']
        for param in required_params:
            if getattr(self, param, None) is None:
                errors.append(f"Missing required parameter: {param}")
                
        # Check parameter ranges
        if self.max_steps <= 0:
            errors.append("max_steps must be positive")
            
        if self.timestep <= 0:
            errors.append("timestep must be positive")
            
        if self.neuron_count <= 0:
            errors.append("neuron_count must be positive")
            
        return errors
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        config_dict = {}
        for key, value in self.__dict__.items():
            if not key.startswith('_'):
                if hasattr(value, 'value'):  # Enum value
                    config_dict[key] = value.value
                else:
                    config_dict[key] = value
        return config_dict
        
    def from_dict(self, config_dict: Dict[str, Any]) -> None:
        """Load configuration from dictionary."""
        for key, value in config_dict.items():
            if hasattr(self, key):
                # Convert string values for enums
                if key == 'developmental_stage' and isinstance(value, str):
                    setattr(self, key, DevelopmentalStage(value))
                else:
                    setattr(self, key, value)
                
        self._validate_config()
        
    def copy(self) -> 'Config':
        """Create a copy of the configuration."""
        new_config = Config()
        for key, value in self.__dict__.items():
            if not key.startswith('_'):
                setattr(new_config, key, value)
        return new_config
        
    def merge(self, other_config: 'Config') -> None:
        """Merge another configuration into this one."""
        for key, value in other_config.__dict__.items():
            if not key.startswith('_'):
                setattr(self, key, value)
        
        self._validate_config()
        
    def __str__(self) -> str:
        """String representation of configuration."""
        return f"Config(neuron_count={self.neuron_count}, " \
               f"max_steps={self.max_steps}, " \
               f"timestep={self.timestep}, " \
               f"development_stage={self.developmental_stage.value})"
        
    def __repr__(self) -> str:
        """Detailed string representation of configuration."""
        return self.__str__()