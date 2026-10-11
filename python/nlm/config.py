"""Configuration management for NLM Python API."""

import json
import os
from typing import Any, Dict, List, Optional, Union
from pathlib import Path
import yaml
import jsonschema
from dataclasses import dataclass, asdict
import copy

from ..core.Config.Config import Config
from ..brain.Brain.hpp:4> # noqa: F401


def nlm_config() -> Config:
    """
    Create a high-level configuration manager class.
    
    Returns:
        Config: A new configuration object
        
    Example:
        >>> config = nlm.nlm_config()
        >>> config.set_value("brain.region_count", 10)
        >>> config.set_value("learning.rate", 0.01)
    """
    return Config()


def load_config_file(filepath: str, auto_detect: bool = True) -> Dict[str, Any]:
    """
    Load config with automatic format detection.
    
    Args:
        filepath: Path to configuration file
        auto_detect: Whether to auto-detect file format by extension
        
    Returns:
        Dict[str, Any]: Loaded configuration as dictionary
        
    Raises:
        ValueError: If file format cannot be detected
        FileNotFoundError: If file does not exist
        
    Example:
        >>> config = nlm.load_config_file("config.json")
        >>> config = nlm.load_config_file("config.yaml", auto_detect=True)
    """
    filepath = Path(filepath)
    if not filepath.exists():
        raise FileNotFoundError(f"Configuration file not found: {filepath}")
    
    if auto_detect:
        if filepath.suffix.lower() in ['.json', '.yaml', '.yml']:
            with open(filepath, 'r') as f:
                return json.load(f) if filepath.suffix.lower() == '.json' else yaml.safe_load(f)
        else:
            raise ValueError(f"Cannot auto-detect format for file: {filepath}")
    else:
        with open(filepath, 'r') as f:
            return json.load(f)


def save_config(config: Union[Dict[str, Any], Config], filepath: str, format: str = "auto") -> None:
    """
    Save config with format choice.
    
    Args:
        config: Configuration to save (dict or Config object)
        filepath: Output file path
        format: Output format ("auto", "json", or "yaml")
        
    Example:
        >>> nlm.save_config(config, "output.json", format="json")
        >>> nlm.save_config(config, "output.yaml", format="yaml")
    """
    filepath = Path(filepath)
    filepath.parent.mkdir(parents=True, exist_ok=True)
    
    if isinstance(config, Config):
        config_dict = {}
        for key in config.getKeys():
            config_dict[key] = config.getValue(key)
    else:
        config_dict = config
    
    if format == "auto" or format == "json":
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
    elif format in ["yaml", "yml"]:
        with open(filepath, 'w') as f:
            yaml.dump(config_dict, f, default_flow_style=False)
    else:
        raise ValueError(f"Unsupported format: {format}")


def validate_config(config: Union[Dict[str, Any], Config], schema: Optional[Dict] = None) -> List[str]:
    """
    Validate config against schemas.
    
    Args:
        config: Configuration to validate
        schema: JSON schema to validate against (uses default if None)
        
    Returns:
        List[str]: List of validation errors (empty if valid)
        
    Example:
        >>> errors = nlm.validate_config(config)
        >>> if errors:
        ...     print("Validation errors:", errors)
    """
    errors = []
    
    # Convert Config to dict if needed
    if isinstance(config, Config):
        config_dict = {}
        for key in config.getKeys():
            config_dict[key] = config.getValue(key)
    else:
        config_dict = config
    
    # Default schema for basic validation
    if schema is None:
        schema = {
            "type": "object",
            "properties": {
                "brain": {"type": "object"},
                "learning": {"type": "object"},
                "memory": {"type": "object"},
                "environment": {"type": "object"},
                "neuromodulation": {"type": "object"}
            },
            "required": ["brain"]
        }
    
    try:
        jsonschema.validate(instance=config_dict, schema=schema)
    except jsonschema.ValidationError as e:
        errors.append(str(e))
    
    return errors


def compare_configs(config1: Union[Dict[str, Any], Config], 
                   config2: Union[Dict[str, Any], Config], 
                   tolerance: float = 0.001) -> Dict[str, Any]:
    """
    Compare two configs.
    
    Args:
        config1: First configuration
        config2: Second configuration
        tolerance: Tolerance for floating point comparisons
        
    Returns:
        Dict[str, Any]: Comparison results including differences
        
    Example:
        >>> diff = nlm.compare_configs(config1, config2)
        >>> print("Differences:", diff['differences'])
    """
    # Convert to dict for comparison
    if isinstance(config1, Config):
        config1_dict = {}
        for key in config1.getKeys():
            config1_dict[key] = config1.getValue(key)
    else:
        config1_dict = config1
        
    if isinstance(config2, Config):
        config2_dict = {}
        for key in config2.getKeys():
            config2_dict[key] = config2.getValue(key)
    else:
        config2_dict = config2
    
    differences = {}
    all_keys = set(config1_dict.keys()) | set(config2_dict.keys())
    
    for key in all_keys:
        val1 = config1_dict.get(key)
        val2 = config2_dict.get(key)
        
        if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
            if abs(val1 - val2) > tolerance:
                differences[key] = {"config1": val1, "config2": val2}
        elif val1 != val2:
            differences[key] = {"config1": val1, "config2": val2}
    
    return {
        "identical": len(differences) == 0,
        "differences": differences,
        "only_in_config1": set(config1_dict.keys()) - set(config2_dict.keys()),
        "only_in_config2": set(config2_dict.keys()) - set(config1_dict.keys())
    }