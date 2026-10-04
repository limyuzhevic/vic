# NLM Configuration Manager
# Enhanced configuration system for NLM Phase 6

import json
import os
from typing import Dict, Any, Optional, Union
from enum import Enum
import logging
from pathlib import Path

class LogLevel(Enum):
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

class Environment(Enum):
    DEVELOPMENT = "development"
    TESTING = "testing"
    PRODUCTION = "production"
    BENCHMARK = "benchmark"

class NLMConfig:
    """
    Comprehensive configuration manager for NLM (Neural Learning Machine).
    
    This class provides a centralized configuration system for all NLM subsystems,
    with support for file-based configuration, environment variables, and runtime
    configuration management.
    """
    
    DEFAULT_CONFIG = {
        # Brain configuration
        "brain": {
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "random_seed": 42,
            "checkpoint_dir": "./checkpoints",
            "developmental_stage": "Initial",
            "working_memory_capacity": 100,
            "episodic_memory_max_episodes": 1000
        },
        
        # Plasticity configuration
        "plasticity": {
            "stdp": {
                "enable": True,
                "ltp_weight": 0.01,
                "ltd_weight": 0.012,
                "tau": 20.0,
                "learning_rate": 0.001
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.001
            },
            "structural": {
                "enable": True,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        },
        
        # Neuromodulation configuration
        "neuromod": {
            "dopamine": {
                "scale": 1.0,
                "plasticity_factor": 1.0
            },
            "curiosity": {
                "enable": True,
                "exploration_strength": 1.0
            },
            "novelty": {
                "enable": True,
                "novelty_sensitivity": 1.0
            }
        },
        
        # Memory system configuration
        "memory": {
            "working": {
                "capacity": 100,
                "decay_rate": 0.01
            },
            "episodic": {
                "max_episodes": 1000,
                "consolidation_interval": 1000,
                "replay_interval": 100
            },
            "associative": {
                "max_associations": 500,
                "similarity_threshold": 0.7
            }
        },
        
        # Cognition system configuration
        "cognition": {
            "planner": {
                "planning_depth": 5,
                "exploration_weight": 0.3
            },
            "concept_formation": {
                "min_patterns": 10,
                "similarity_threshold": 0.8
            },
            "attention": {
                "inhibition_strength": 0.5,
                "excitation_strength": 1.5,
                "selection_threshold": 0.3
            }
        },
        
        # Development system configuration
        "development": {
            "initial_stage": "Initial",
            "critical_period_duration": 100,
            "plasticity_decay_rate": 0.95,
            "maturation_rate": 0.01
        },
        
        # Performance and optimization
        "performance": {
            "enable_multithreading": False,
            "enable_simd": True,
            "memory_pool_size": 1024,
            "event_queue_size": 4096,
            "checkpoint_compression": "balanced"
        },
        
        # Experiment configuration
        "experiment": {
            "default_neuron_count": 500,
            "default_max_steps": 2000,
            "enable_replay": True,
            "enable_development": True,
            "enable_checkpointing": True
        },
        
        # Logging configuration
        "logging": {
            "level": "INFO",
            "file": "nlm.log",
            "max_file_size": 10485760,  # 10MB
            "backup_count": 5,
            "format": "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        },
        
        # System configuration
        "system": {
            "max_memory_usage": 2147483648,  # 2GB
            "timeout": 300,  # 5 minutes
            "retry_attempts": 3,
            "enable_validation": True
        }
    }
    
    def __init__(self, config_file: Optional[str] = None, environment: Environment = Environment.DEVELOPMENT):
        """
        Initialize NLM configuration.
        
        Args:
            config_file: Path to configuration file
            environment: Environment to load configuration for
        """
        self.config = self.DEFAULT_CONFIG.copy()
        self.environment = environment
        self.logger = self._setup_logging()
        
        # Load configuration from file if provided
        if config_file and os.path.exists(config_file):
            self.load_config_file(config_file)
        
        # Override with environment-specific configuration
        self._load_environment_config()
        
        # Validate configuration
        self._validate_config()
        
        self.logger.info(f"NLM configuration loaded for {environment.value}")
    
    def _setup_logging(self) -> logging.Logger:
        """Setup logging system."""
        logger = logging.getLogger("nlm")
        logger.setLevel(logging.DEBUG)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        # Create console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.DEBUG)
        console_handler.setFormatter(formatter)
        
        # Create file handler
        file_handler = logging.FileHandler("nlm.log")
        file_handler.setLevel(logging.INFO)
        file_handler.setFormatter(formatter)
        
        # Add handlers to logger
        logger.addHandler(console_handler)
        logger.addHandler(file_handler)
        
        return logger
    
    def load_config_file(self, config_file: str):
        """
        Load configuration from file.
        
        Args:
            config_file: Path to configuration file
        """
        try:
            with open(config_file, 'r') as f:
                file_config = json.load(f)
            
            self._merge_config(file_config)
            self.logger.info(f"Configuration loaded from {config_file}")
            
        except Exception as e:
            self.logger.error(f"Failed to load configuration from {config_file}: {e}")
            raise
    
    def _merge_config(self, file_config: Dict[str, Any]):
        """Merge file configuration with existing configuration."""
        def merge_dicts(dest, src):
            for key, value in src.items():
                if key in dest and isinstance(dest[key], dict) and isinstance(value, dict):
                    merge_dicts(dest[key], value)
                else:
                    dest[key] = value
        
        merge_dicts(self.config, file_config)
    
    def _load_environment_config(self):
        """Load environment-specific configuration."""
        env_config = {
            Environment.DEVELOPMENT: {
                "logging": {"level": "DEBUG"},
                "performance": {"enable_multithreading": False}
            },
            Environment.TESTING: {
                "logging": {"level": "INFO"},
                "performance": {"enable_multithreading": False}
            },
            Environment.PRODUCTION: {
                "logging": {"level": "WARNING"},
                "performance": {"enable_multithreading": True, "enable_simd": True}
            },
            Environment.BENCHMARK: {
                "logging": {"level": "ERROR"},
                "performance": {"enable_multithreading": True, "enable_simd": True}
            }
        }
        
        if self.environment in env_config:
            self._merge_config(env_config[self.environment])
    
    def _validate_config(self):
        """Validate configuration for consistency and correctness."""
        self.logger.info("Validating configuration...")
        
        # Validate brain configuration
        brain_config = self.config.get("brain", {})
        if brain_config.get("neuron_count", 0) <= 0:
            raise ValueError("neuron_count must be > 0")
        
        if brain_config.get("region_count", 0) <= 0:
            raise ValueError("region_count must be > 0")
        
        connection_prob = brain_config.get("connection_probability", 0)
        if connection_prob < 0 or connection_prob > 1:
            raise ValueError("connection_probability must be between 0.0 and 1.0")
        
        # Validate plasticity configuration
        plasticity_config = self.config.get("plasticity", {})
        
        if plasticity_config.get("stdp", {}).get("enable"):
            stdp = plasticity_config["stdp"]
            if stdp.get("ltp_weight", 0) <= 0:
                raise ValueError("stdp.ltp_weight must be > 0")
            if stdp.get("ltd_weight", 0) <= 0:
                raise ValueError("stdp.ltd_weight must be > 0")
        
        # Validate memory configuration
        memory_config = self.config.get("memory", {})
        
        if memory_config.get("working", {}).get("capacity", 0) <= 0:
            raise ValueError("working.memory.capacity must be > 0")
        
        # Validate performance configuration
        performance_config = self.config.get("performance", {})
        
        if performance_config.get("memory_pool_size", 0) <= 0:
            raise ValueError("performance.memory_pool_size must be > 0")
        
        self.logger.info("Configuration validation completed successfully")
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value by key.
        
        Args:
            key: Configuration key (can be nested with dot notation)
            default: Default value if key not found
            
        Returns:
            Configuration value
        """
        # Handle nested keys with dot notation (e.g., "brain.neuron_count")
        keys = key.split('.')
        value = self.config
        
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        
        return value
    
    def set(self, key: str, value: Any):
        """
        Set configuration value by key.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        keys = key.split('.')
        config = self.config
        
        # Navigate to the parent of the target key
        for k in keys[:-1]:
            if k not in config:
                config[k] = {}
            config = config[k]
        
        config[keys[-1]] = value
        
        # Validate after setting
        self._validate_config()
        
        self.logger.info(f"Configuration updated: {key} = {value}")
    
    def get_brain_config(self) -> Dict[str, Any]:
        """Get brain-specific configuration."""
        return self.config.get("brain", {}).copy()
    
    def get_plasticity_config(self) -> Dict[str, Any]:
        """Get plasticity-specific configuration."""
        return self.config.get("plasticity", {}).copy()
    
    def get_neuromod_config(self) -> Dict[str, Any]:
        """Get neuromodulation-specific configuration."""
        return self.config.get("neuromod", {}).copy()
    
    def get_memory_config(self) -> Dict[str, Any]:
        """Get memory-specific configuration."""
        return self.config.get("memory", {}).copy()
    
    def get_cognition_config(self) -> Dict[str, Any]:
        """Get cognition-specific configuration."""
        return self.config.get("cognition", {}).copy()
    
    def get_development_config(self) -> Dict[str, Any]:
        """Get development-specific configuration."""
        return self.config.get("development", {}).copy()
    
    def get_performance_config(self) -> Dict[str, Any]:
        """Get performance-specific configuration."""
        return self.config.get("performance", {}).copy()
    
    def get_experiment_config(self) -> Dict[str, Any]:
        """Get experiment-specific configuration."""
        return self.config.get("experiment", {}).copy()
    
    def get_logging_config(self) -> Dict[str, Any]:
        """Get logging-specific configuration."""
        return self.config.get("logging", {}).copy()
    
    def get_system_config(self) -> Dict[str, Any]:
        """Get system-specific configuration."""
        return self.config.get("system", {}).copy()
    
    def save_config(self, config_file: str):
        """
        Save configuration to file.
        
        Args:
            config_file: Path to save configuration file
        """
        try:
            with open(config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
            
            self.logger.info(f"Configuration saved to {config_file}")
            
        except Exception as e:
            self.logger.error(f"Failed to save configuration to {config_file}: {e}")
            raise
    
    def reset_to_defaults(self):
        """Reset configuration to default values."""
        self.config = self.DEFAULT_CONFIG.copy()
        self.logger.info("Configuration reset to defaults")
    
    def get_config_summary(self) -> str:
        """
        Get configuration summary as string.
        
        Returns:
            Configuration summary
        """
        summary = "=== NLM CONFIGURATION SUMMARY ===\n"
        
        # Brain configuration
        brain = self.config.get("brain", {})
        summary += f"Brain: {brain.get('neuron_count', 0)} neurons, {brain.get('region_count', 0)} regions\n"
        
        # Plasticity configuration
        plasticity = self.config.get("plasticity", {})
        stdp_enabled = plasticity.get("stdp", {}).get("enable", False)
        summary += f"Plasticity: STDP={'Enabled' if stdp_enabled else 'Disabled'}, "
        summary += f"Hebbian={'Enabled' if plasticity.get('hebbian', {}).get('enable') else 'Disabled'}, "
        summary += f"Structural={'Enabled' if plasticity.get('structural', {}).get('enable') else 'Disabled'}\n"
        
        # Memory configuration
        memory = self.config.get("memory", {})
        working_capacity = memory.get("working", {}).get("capacity", 0)
        episodic_capacity = memory.get("episodic", {}).get("max_episodes", 0)
        summary += f"Memory: Working={working_capacity}, Episodic={episodic_capacity}\n"
        
        # Performance configuration
        performance = self.config.get("performance", {})
        threading = performance.get("enable_multithreading", False)
        simd = performance.get("enable_simd", False)
        summary += f"Performance: Multi-threading={'Enabled' if threading else 'Disabled'}, SIMD={'Enabled' if simd else 'Disabled'}\n"
        
        # Environment
        summary += f"Environment: {self.environment.value}\n"
        
        return summary
    
    def validate_config_section(self, section: str) -> bool:
        """
        Validate a specific configuration section.
        
        Args:
            section: Configuration section to validate
            
        Returns:
            True if section is valid, False otherwise
        """
        if section not in self.config:
            self.logger.error(f"Configuration section '{section}' not found")
            return False
        
        # Perform section-specific validation
        if section == "brain":
            brain = self.config[section]
            if brain.get("neuron_count", 0) <= 0:
                return False
            if brain.get("region_count", 0) <= 0:
                return False
        
        elif section == "memory":
            memory = self.config[section]
            working = memory.get("working", {})
            if working.get("capacity", 0) <= 0:
                return False
        
        elif section == "performance":
            performance = self.config[section]
            if performance.get("memory_pool_size", 0) <= 0:
                return False
        
        return True

# Global configuration instance
_config_instance = None

def get_config() -> NLMConfig:
    """
    Get global NLM configuration instance.
    
    Returns:
        Global NLM configuration instance
    """
    global _config_instance
    if _config_instance is None:
        _config_instance = NLMConfig()
    return _config_instance

def initialize_config(config_file: str = None, environment: Environment = Environment.DEVELOPMENT):
    """
    Initialize global NLM configuration.
    
    Args:
        config_file: Path to configuration file
        environment: Environment to load configuration for
    """
    global _config_instance
    _config_instance = NLMConfig(config_file, environment)
    return _config_instance

# Example usage
if __name__ == "__main__":
    # Initialize configuration
    config = NLMConfig()
    
    # Print configuration summary
    print(config.get_config_summary())
    
    # Get specific configuration values
    neuron_count = config.get("brain.neuron_count")
    connection_prob = config.get("brain.connection_probability")
    
    print(f"Neuron count: {neuron_count}")
    print(f"Connection probability: {connection_prob}")
    
    # Set configuration values
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.learning_rate", 0.002)
    
    # Save configuration
    config.save_config("nlm_config.json")
    
    print("\nConfiguration updated and saved successfully!")
