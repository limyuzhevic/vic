"""
Core NLM Python bindings and utilities.

This module contains the main Python bindings for NLM, including factory functions
and constants that are not directly exposed in the C++ bindings.
"""

import json
import tempfile
from pathlib import Path
from typing import Any, Dict, List, Optional, Union

# These will be populated by pybind11 at runtime
try:
    from .pynlm_ext import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Main classes
        Config, Brain, AgentBrain, SimpleWorld,
        SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult, SensoryPercept,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
        INVALID_REGION_ID, INVALID_POPULATION_ID,
    )
except ImportError:
    # Fallback for development/testing without C++ extension
    NeuronId = SynapseId = RegionId = PopulationId = None
    NeuronType = SynapseType = DevelopmentalStage = FiringState = None
    ActionType = MotorCommand = WorldObjectType = None
    Config = Brain = AgentBrain = SimpleWorld = None
    SensoryInput = Vision = Audio = InternalSignals = None
    Action = WorldObject = AgentBody = ActionResult = SensoryPercept = None
    createDefaultConfig = createBrain = createSimpleWorld = createAgentBrain = None
    INVALID_NEURON_ID = INVALID_SYNAPSE_ID = INVALID_REGION_ID = INVALID_POPULATION_ID = None

__version__ = "0.1.0"


class NLMError(Exception):
    """Base exception for NLM errors."""
    pass


class ConfigError(NLMError):
    """Error related to configuration operations."""
    pass


class SimulationError(NLMError):
    """Error related to simulation operations."""
    pass


class ResourceError(NLMError):
    """Error related to resource management."""
    pass


def load_config_from_file(filepath: Union[str, Path], format: str = "auto") -> Config:
    """
    Load configuration from a file with automatic format detection.
    
    Args:
        filepath: Path to configuration file
        format: File format ('auto', 'json', 'yaml', 'ini', 'toml')
        
    Returns:
        Config object with loaded settings
        
    Raises:
        ConfigError: If file cannot be loaded or parsed
        ValueError: If format is not supported
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise ConfigError(f"Configuration file not found: {filepath}")
    
    if format == "auto":
        # Auto-detect format based on file extension
        ext = filepath.suffix.lower()
        if ext == ".json":
            format = "json"
        elif ext in (".yaml", ".yml"):
            format = "yaml"
        elif ext == ".ini":
            format = "ini"
        elif ext == ".toml":
            format = "toml"
        else:
            # Default to simple key=value format
            format = "simple"
    
    config = createDefaultConfig()
    
    try:
        if format == "json":
            with open(filepath, 'r') as f:
                data = json.load(f)
            _apply_json_config(config, data)
        elif format == "yaml":
            try:
                import yaml
                with open(filepath, 'r') as f:
                    data = yaml.safe_load(f)
                _apply_yaml_config(config, data)
            except ImportError:
                raise ConfigError("YAML support requires pyyaml package. Install with: pip install pyyaml")
        elif format == "ini":
            import configparser
            parser = configparser.ConfigParser()
            parser.read(filepath)
            _apply_ini_config(config, parser)
        elif format == "toml":
            try:
                import tomli
                with open(filepath, 'rb') as f:
                    data = tomli.load(f)
                _apply_toml_config(config, data)
            except ImportError:
                raise ConfigError("TOML support requires tomli package. Install with: pip install tomli")
        elif format == "simple":
            config.loadFromFile(str(filepath))
        else:
            raise ValueError(f"Unsupported format: {format}")
            
    except Exception as e:
        raise ConfigError(f"Failed to load configuration from {filepath}: {e}")
    
    return config


def save_config_to_file(config: Config, filepath: Union[str, Path], 
                       format: str = "auto", pretty: bool = False) -> None:
    """
    Save configuration to a file with format preservation.
    
    Args:
        config: Configuration to save
        filepath: Output file path
        format: File format ('auto', 'json', 'yaml', 'ini', 'toml')
        pretty: Whether to format output for human readability
        
    Raises:
        ConfigError: If file cannot be written
    """
    filepath = Path(filepath)
    filepath.parent.mkdir(parents=True, exist_ok=True)
    
    try:
        if format == "auto":
            # Try to preserve existing format or default to JSON
            if filepath.exists():
                ext = filepath.suffix.lower()
                if ext == ".json":
                    format = "json"
                elif ext in (".yaml", ".yml"):
                    format = "yaml"
                elif ext == ".ini":
                    format = "ini"
                elif ext == ".toml":
                    format = "toml"
                else:
                    format = "json"
            else:
                format = "json"
        
        if format == "json":
            data = _extract_json_config(config)
            with open(filepath, 'w') as f:
                if pretty:
                    json.dump(data, f, indent=2, default=_json_default)
                else:
                    json.dump(data, f, default=_json_default)
        elif format == "yaml":
            try:
                import yaml
                data = _extract_yaml_config(config)
                with open(filepath, 'w') as f:
                    if pretty:
                        yaml.dump(data, f, default_flow_style=False)
                    else:
                        yaml.dump(data, f, default_flow_style=True)
            except ImportError:
                raise ConfigError("YAML support requires pyyaml package. Install with: pip install pyyaml")
        elif format == "ini":
            import configparser
            parser = configparser.ConfigParser()
            data = _extract_ini_config(config)
            for section, values in data.items():
                parser[section] = values
            with open(filepath, 'w') as f:
                parser.write(f)
        elif format == "toml":
            try:
                import tomli_w
                data = _extract_toml_config(config)
                with open(filepath, 'wb') as f:
                    if pretty:
                        tomli_w.dump(data, f)
                    else:
                        # For compact output, write without pretty formatting
                        tomli_w.dump(data, f)
            except ImportError:
                raise ConfigError("TOML support requires tomli-w package. Install with: pip install tomli-w")
        else:
            raise ValueError(f"Unsupported format: {format}")
            
    except Exception as e:
        raise ConfigError(f"Failed to save configuration to {filepath}: {e}")


def validate_config(config: Config) -> List[str]:
    """
    Validate configuration and return list of errors (empty if valid).
    
    Args:
        config: Configuration to validate
        
    Returns:
        List of error messages (empty if valid)
    """
    errors = []
    
    # Check for required configuration keys
    required_keys = ["brain.neuron_count"]
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required configuration key: {key}")
    
    # Validate specific values
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 1)
        if neuron_count <= 0:
            errors.append("brain.neuron_count must be positive")
        elif neuron_count > 1000000:
            errors.append("brain.neuron_count too large (max 1,000,000)")
    
    if config.has("simulation_timestep"):
        timestep = config.getOr("simulation_timestep", 0.001)
        if timestep <= 0:
            errors.append("simulation_timestep must be positive")
        elif timestep > 1.0:
            errors.append("simulation_timestep too large (max 1.0)")
    
    return errors


class ContextManager:
    """
    Context manager for NLM resource management.
    
    Usage:
        with ContextManager(config, world) as (brain, agent):
            # Use brain and agent
            for step in range(100):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
    """
    
    def __init__(self, config: Optional[Config] = None, world: Optional[SimpleWorld] = None):
        self.config = config
        self.world = world
        self.brain = None
        self.agent = None
        
    def __enter__(self):
        if self.config is None:
            self.config = createDefaultConfig()
        
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        if self.world is None:
            self.world = createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
        
        if self.brain and self.world:
            self.agent = createAgentBrain(self.brain)
            self.agent.initialize(self.world)
        
        return self.brain, self.agent, self.world
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Clean up resources
        if self.agent:
            self.agent.reset()
        if self.brain:
            self.brain.reset()
        
        self.agent = None
        self.brain = None
        self.world = None


def run_simulation_episode(brain: Brain, world: SimpleWorld, agent: AgentBrain,
                           num_steps: int, enable_learning: bool = True) -> Dict[str, Any]:
    """
    Run a complete simulation episode with automatic resource management.
    
    Args:
        brain: Neural brain
        world: Simulation world
        agent: Agent interface
        num_steps: Number of simulation steps
        enable_learning: Whether to enable learning subsystems
        
    Returns:
        Dictionary with simulation statistics
    """
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    stats = {
        "steps": num_steps,
        "final_firing_rate": 0.0,
        "max_firing_rate": 0.0,
        "total_spikes": 0,
        "curiosity_levels": [],
        "novelty_levels": [],
        "development_stages": [],
        "reward_modulation_enabled": False,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if enable_learning:
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Collect statistics
        firing_rate = brain.getAverageFiringRate()
        stats["curiosity_levels"].append(agent.getCuriosityLevel())
        stats["novelty_levels"].append(agent.getNoveltyLevel())
        stats["development_stages"].append(brain.getDevelopmentalStage().value)
        
        stats["final_firing_rate"] = firing_rate
        stats["max_firing_rate"] = max(stats["max_firing_rate"], firing_rate)
        stats["total_spikes"] = brain.getTotalSpikeCount()
        
        if enable_learning:
            stats["reward_modulation_enabled"] = True
    
    return stats


def _apply_json_config(config: Config, data: Dict[str, Any]) -> None:
    """Apply JSON configuration data to config object."""
    if "brain" in data:
        brain_config = data["brain"]
        for key, value in brain_config.items():
            config.set(f"brain.{key}", value)
    
    # Apply top-level configuration
    top_level_keys = ["random_seed", "simulation_timestep", "neuron_count", 
                     "region_count", "connection_probability"]
    for key in top_level_keys:
        if key in data:
            config.set(key, data[key])


def _extract_json_config(config: Config) -> Dict[str, Any]:
    """Extract configuration data as JSON."""
    data = {}
    
    # Extract brain configuration
    brain_config = {}
    brain_keys = ["neuron_count", "region_count", "connection_probability"]
    for key in brain_keys:
        if config.has(f"brain.{key}"):
            brain_config[key] = config.getOr(f"brain.{key}")
    
    if brain_config:
        data["brain"] = brain_config
    
    # Extract top-level configuration
    top_level_keys = ["random_seed", "simulation_timestep"]
    for key in top_level_keys:
        if config.has(key):
            data[key] = config.getOr(key)
    
    return data


def _apply_yaml_config(config: Config, data: Dict[str, Any]) -> None:
    """Apply YAML configuration data to config object."""
    _apply_json_config(config, data)  # YAML structure similar to JSON


def _extract_yaml_config(config: Config) -> Dict[str, Any]:
    """Extract configuration data as YAML."""
    return _extract_json_config(config)


def _apply_ini_config(config: Config, parser: Any) -> None:
    """Apply INI configuration data to config object."""
    for section in parser.sections():
        for key, value in parser.items(section):
            config.set(f"{section}.{key}", value)


def _extract_ini_config(config: Config) -> Dict[str, Dict[str, str]]:
    """Extract configuration data as INI."""
    data = {}
    for section_name in config.getKeys():
        if "." in section_name:
            section, key = section_name.split(".", 1)
            if section not in data:
                data[section] = {}
            data[section][key] = str(config.getOr(section_name))
    return data


def _apply_toml_config(config: Config, data: Dict[str, Any]) -> None:
    """Apply TOML configuration data to config object."""
    _apply_json_config(config, data)  # TOML structure similar to JSON


def _extract_toml_config(config: Config) -> Dict[str, Any]:
    """Extract configuration data as TOML."""
    return _extract_json_config(config)


def _json_default(obj: Any) -> Any:
    """JSON serializer for objects not serializable by default json code."""
    if hasattr(obj, '__dict__'):
        return obj.__dict__
    elif hasattr(obj, 'value'):  # For enums and IDs
        return obj.value
    raise TypeError(f"Object of type {type(obj)} is not JSON serializable")
