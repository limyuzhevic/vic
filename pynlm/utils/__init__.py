"""
pynlm.utils - General utility functions for NLM project

This module provides general utility functions including:
- Configuration helpers
- Simulation utilities
- Data analysis helpers
- Error handling and validation
- File I/O utilities (checkpoint management)
"""

import json
import os
import sys
import time
from typing import Dict, Any, List, Optional, Union
import numpy as np
import logging
from datetime import datetime
import warnings
from pathlib import Path

# Optional dependency handling
try:
    import yaml
    HAS_YAML = True
except ImportError:
    HAS_YAML = False
    yaml = None
    warnings.warn("PyYAML is not installed. YAML configuration loading is disabled.")

# Configure logger
logger = logging.getLogger(__name__)


def load_config(config_path: str) -> Dict[str, Any]:
    """
    Load configuration from JSON or YAML file.
    
    Parameters
    ----------
    config_path : str
        Path to configuration file (.json or .yaml/.yml)
        
    Returns
    -------
    Dict[str, Any]
        Configuration dictionary
        
    Examples
    --------
    >>> config = load_config("config.json")
    >>> config = load_config("config.yaml")
    """
    config_path = Path(config_path)
    
    if not config_path.exists():
        raise FileNotFoundError(f"Configuration file not found: {config_path}")
    
    if config_path.suffix.lower() == '.json':
        with open(config_path, 'r') as f:
            config = json.load(f)
    elif config_path.suffix.lower() in ['.yaml', '.yml']:
        if not HAS_YAML:
            raise ImportError("YAML file loading requires PyYAML. Install it with 'pip install pyyaml'.")
        with open(config_path, 'r') as f:
            config = yaml.safe_load(f)
    else:
        raise ValueError(f"Unsupported configuration file format: {config_path.suffix}")
    
    logger.info(f"Loaded configuration from {config_path}")
    return config


def save_config(config: Dict[str, Any], config_path: str, format: str = 'json') -> None:
    """
    Save configuration to JSON or YAML file.
    
    Parameters
    ----------
    config : Dict[str, Any]
        Configuration dictionary
    config_path : str
        Path to save configuration file
    format : str, optional
        Output format ('json' or 'yaml'), by default 'json'
        
    Examples
    --------
    >>> save_config(config, "config.json", "json")
    >>> save_config(config, "config.yaml", "yaml")
    """
    config_path = Path(config_path)
    config_path.parent.mkdir(parents=True, exist_ok=True)
    
    if format.lower() == 'json':
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
    elif format.lower() == 'yaml':
        if not HAS_YAML:
            raise ImportError("YAML file saving requires PyYAML. Install it with 'pip install pyyaml'.")
        with open(config_path, 'w') as f:
            yaml.dump(config, f, default_flow_style=False)
    else:
        raise ValueError(f"Unsupported output format: {format}")
    
    logger.info(f"Saved configuration to {config_path}")


def create_preset(base_config: Dict[str, Any], **kwargs) -> Dict[str, Any]:
    """
    Create a configuration preset by merging base configuration with overrides.
    
    Parameters
    ----------
    base_config : Dict[str, Any]
        Base configuration dictionary
    **kwargs
        Configuration overrides
        
    Returns
    -------
    Dict[str, Any]
        Merged configuration dictionary
        
    Examples
    --------
    >>> base = load_config("base_config.json")
    >>> preset = create_preset(base, learning_rate=0.01, batch_size=32)
    """
    preset = base_config.copy()
    
    def recursive_merge(dict1: Dict[str, Any], dict2: Dict[str, Any]) -> Dict[str, Any]:
        for key, value in dict2.items():
            if key in dict1 and isinstance(dict1[key], dict) and isinstance(value, dict):
                recursive_merge(dict1[key], value)
            else:
                dict1[key] = value
        return dict1
    
    return recursive_merge(preset, kwargs)


def merge_configs(*configs: Dict[str, Any]) -> Dict[str, Any]:
    """
    Merge multiple configuration dictionaries.
    
    Parameters
    ----------
    *configs : Dict[str, Any]
        Configuration dictionaries to merge (later ones override earlier ones)
        
    Returns
    -------
    Dict[str, Any]
        Merged configuration dictionary
        
    Examples
    --------
    >>> config1 = {"learning_rate": 0.01}
    >>> config2 = {"batch_size": 32, "learning_rate": 0.001}
    >>> config = merge_configs(config1, config2)
    """
    result = {}
    for config in configs:
        result = create_preset(result, **config)
    return result


class Timer:
    """
    Context manager for timing code execution.
    
    Examples
    --------
    >>> with Timer("Training"):
    ...     train_model()
    Training: 2.34s
    """
    
    def __init__(self, name: str = None, verbose: bool = True):
        self.name = name
        self.verbose = verbose
        self.start_time = None
        
    def __enter__(self):
        self.start_time = time.time()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        elapsed = time.time() - self.start_time
        if self.verbose:
            name = f" {self.name}" if self.name else ""
            print(f"{name}: {elapsed:.2f}s")
        
        return False


def format_time(seconds: float) -> str:
    """
    Format seconds into human-readable time string.
    
    Parameters
    ----------
    seconds : float
        Time in seconds
        
    Returns
    -------
    str
        Formatted time string (e.g., "2.34s", "1:23.4m", "2:15:32h")
        
    Examples
    --------
    >>> format_time(45.67)
    '45.67s'
    >>> format_time(3723.45)
    '1:02:03.45'
    """
    if seconds < 60:
        return f"{seconds:.2f}s"
    elif seconds < 3600:
        minutes = int(seconds // 60)
        secs = seconds % 60
        return f"{minutes}:{secs:04.1f}m"
    else:
        hours = int(seconds // 3600)
        minutes = int((seconds % 3600) // 60)
        secs = seconds % 60
        return f"{hours}:{minutes:02d}:{secs:04.1f}h"


class ProgressBar:
    """
    Progress bar for long-running operations.
    
    Examples
    --------
    >>> items = range(100)
    >>> pb = ProgressBar(len(items), "Processing")
    >>> for item in items:
    ...     process(item)
    ...     pb.update()
    Processing: 100%|██████████| 100/100 [12:34:56, 12.34it/s]
    """
    
    def __init__(self, total: int, prefix: str = "", suffix: str = "", length: int = 50):
        self.total = total
        self.prefix = prefix
        self.suffix = suffix
        self.length = length
        self.current = 0
        self.start_time = time.time()
        
    def update(self, increment: int = 1) -> None:
        """Update progress bar."""
        self.current += increment
        self._display()
        
    def _display(self) -> None:
        if self.current > self.total:
            self.current = self.total
            
        filled_length = int(self.length * self.current // self.total)
        bar = '█' * filled_length + '-' * (self.length - filled_length)
        
        percent = 100 * self.current // self.total
        elapsed = time.time() - self.start_time
        
        rate = self.current / elapsed if elapsed > 0 else 0
        eta = (self.total - self.current) / rate if rate > 0 else 0
        
        time_str = format_time(elapsed)
        eta_str = format_time(eta)
        
        print(f"\r{self.prefix} {percent:3d}%|{bar}| {self.current}/{self.total} ({rate:.2f}it/s, {eta_str} remaining){self.suffix}", 
              end='', flush=True)
              
        if self.current >= self.total:
            print()


def validate_parameters(params: Dict[str, Any], schema: Dict[str, Any]) -> bool:
    """
    Validate parameters against a schema.
    
    Parameters
    ----------
    params : Dict[str, Any]
        Parameters to validate
    schema : Dict[str, Any]
        Schema with 'type', 'required', 'range', 'choices' keys
        
    Returns
    -------
    bool
        True if parameters are valid, False otherwise
        
    Raises
    ------
    ValueError
        If validation fails
        
    Examples
    --------
    >>> schema = {
    ...     "learning_rate": {"type": "float", "range": (0.0, 1.0)},
    ...     "batch_size": {"type": "int", "range": (1, 100)}
    ... }
    >>> validate_parameters({"learning_rate": 0.01, "batch_size": 32}, schema)
    True
    """
    errors = []
    
    for param_name, param_value in params.items():
        if param_name not in schema:
            continue
            
        param_schema = schema[param_name]
        
        if 'required' in param_schema and param_schema['required']:
            if param_name not in params or params[param_name] is None:
                errors.append(f"Missing required parameter: {param_name}")
                
        if param_name in params and params[param_name] is not None:
            value = params[param_name]
            
            if 'type' in param_schema:
                expected_type = param_schema['type']
                if expected_type == 'int' and not isinstance(value, (int, np.integer)):
                    errors.append(f"Parameter {param_name} must be an integer, got {type(value)}")
                elif expected_type == 'float' and not isinstance(value, (float, int, np.floating, np.integer)):
                    errors.append(f"Parameter {param_name} must be a float, got {type(value)}")
                elif expected_type == 'bool' and not isinstance(value, bool):
                    errors.append(f"Parameter {param_name} must be a boolean, got {type(value)}")
                elif expected_type == 'str' and not isinstance(value, str):
                    errors.append(f"Parameter {param_name} must be a string, got {type(value)}")
                
            if 'range' in param_schema:
                min_val, max_val = param_schema['range']
                if isinstance(value, (int, float, np.number)):
                    if value < min_val or value > max_val:
                        errors.append(f"Parameter {param_name} must be in range [{min_val}, {max_val}], got {value}")
                elif isinstance(value, (list, tuple, np.ndarray)):
                    if len(value) < min_val or len(value) > max_val:
                        errors.append(f"Parameter {param_name} length must be in range [{min_val}, {max_val}], got {len(value)}")
                        
            if 'choices' in param_schema:
                if value not in param_schema['choices']:
                    errors.append(f"Parameter {param_name} must be one of {param_schema['choices']}, got {value}")
    
    if errors:
        error_msg = "Validation errors:\n" + "\n".join(f"  - {error}" for error in errors)
        raise ValueError(error_msg)
    
    return True


def safe_divide(a: float, b: float, default: float = 0.0) -> float:
    """
    Safely divide two numbers, returning default value if division by zero.
    
    Parameters
    ----------
    a : float
        Numerator
    b : float
        Denominator
    default : float, optional
        Value to return if denominator is zero, by default 0.0
        
    Returns
    -------
    float
        Result of division or default value
        
    Examples
    --------
    >>> safe_divide(10, 2)
    5.0
    >>> safe_divide(10, 0)
    0.0
    """
    return a / b if b != 0 else default


def safe_mean(data: List[float], default: float = 0.0) -> float:
    """
    Safely compute mean of a list, returning default if list is empty.
    
    Parameters
    ----------
    data : List[float]
        Data list
    default : float, optional
        Value to return if data is empty, by default 0.0
        
    Returns
    -------
    float
        Mean of data or default value
        
    Examples
    --------
    >>> safe_mean([1, 2, 3, 4, 5])
    3.0
    >>> safe_mean([])
    0.0
    """
    return np.mean(data) if data else default


def safe_std(data: List[float], default: float = 0.0) -> float:
    """
    Safely compute standard deviation of a list, returning default if empty.
    
    Parameters
    ----------
    data : List[float]
        Data list
    default : float, optional
        Value to return if data is empty, by default 0.0
        
    Returns
    -------
    float
        Standard deviation of data or default value
        
    Examples
    --------
    >>> safe_std([1, 2, 3, 4, 5])
    1.4142135623730951
    >>> safe_std([])
    0.0
    """
    return np.std(data) if data else default


class CheckpointManager:
    """
    Manage checkpoint files for saving and restoring simulation state.
    
    Examples
    --------
    >>> from pynlm.checkpoint import CheckpointManager
    >>> checkpoint = CheckpointManager("model_checkpoint")
    >>> checkpoint.save({"epoch": 10, "model_state": model.state_dict()})
    >>> state = checkpoint.load()
    """
    
    def __init__(self, name: str, checkpoint_dir: str = "./checkpoints"):
        """
        Initialize checkpoint manager.
        
        Parameters
        ----------
        name : str
            Name of the checkpoint
        checkpoint_dir : str, optional
            Directory to store checkpoints, by default "./checkpoints"
        """
        self.name = name
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
        
    def save(self, state: Dict[str, Any], filename: Optional[str] = None) -> None:
        """
        Save state to checkpoint file.
        
        Parameters
        ----------
        state : Dict[str, Any]
            State dictionary to save
        filename : str, optional
            Checkpoint filename, by default {name}_{timestamp}.json
            
        Examples
        --------
        >>> checkpoint.save({"epoch": 10, "model_state": model.state_dict()})
        """
        if filename is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"{self.name}_{timestamp}.json"
            
        filepath = self.checkpoint_dir / filename
        
        # Save metadata
        metadata = {
            "name": self.name,
            "timestamp": datetime.now().isoformat(),
            "state": state
        }
        
        with open(filepath, 'w') as f:
            json.dump(metadata, f, indent=2)
            
        logger.info(f"Saved checkpoint to {filepath}")
        
    def load(self, filename: Optional[str] = None) -> Dict[str, Any]:
        """
        Load state from checkpoint file.
        
        Parameters
        ----------
        filename : str, optional
            Checkpoint filename, by default loads the latest
            
        Returns
        -------
        Dict[str, Any]
            Loaded state dictionary
            
        Examples
        --------
        >>> state = checkpoint.load()
        """
        if filename is None:
            # Find latest checkpoint
            checkpoint_files = list(self.checkpoint_dir.glob(f"{self.name}_*.json"))
            if not checkpoint_files:
                raise FileNotFoundError(f"No checkpoints found for {self.name}")
            filename = max(checkpoint_files, key=lambda p: p.stat().st_mtime).name
            
        filepath = self.checkpoint_dir / filename
        
        with open(filepath, 'r') as f:
            metadata = json.load(f)
            
        state = metadata.get('state', {})
        logger.info(f"Loaded checkpoint from {filepath}")
        
        return state
        
    def list_checkpoints(self) -> List[str]:
        """
        List all available checkpoints.
        
        Returns
        -------
        List[str]
            List of checkpoint filenames
            
        Examples
        --------
        >>> checkpoints = checkpoint.list_checkpoints()
        """
        checkpoint_files = list(self.checkpoint_dir.glob(f"{self.name}_*.json"))
        return [f.name for f in checkpoint_files]
        
    def delete(self, filename: str) -> None:
        """
        Delete a checkpoint file.
        
        Parameters
        ----------
        filename : str
            Checkpoint filename to delete
            
        Examples
        --------
        >>> checkpoint.delete("model_20240101_120000.json")
        """
        filepath = self.checkpoint_dir / filename
        if filepath.exists():
            filepath.unlink()
            logger.info(f"Deleted checkpoint {filepath}")
        else:
            raise FileNotFoundError(f"Checkpoint not found: {filename}")


def format_bytes(bytes_count: int) -> str:
    """
    Format bytes into human-readable format.
    
    Parameters
    ----------
    bytes_count : int
        Number of bytes
        
    Returns
    -------
    str
        Formatted bytes string (e.g., "1.23MB", "456KB", "1.23GB")
        
    Examples
    --------
    >>> format_bytes(1024)
    '1.00KB'
    >>> format_bytes(1024 * 1024)
    '1.00MB'
    """
    if bytes_count < 1024:
        return f"{bytes_count}B"
    elif bytes_count < 1024**2:
        return f"{bytes_count/1024:.2f}KB"
    elif bytes_count < 1024**3:
        return f"{bytes_count/1024**2:.2f}MB"
    elif bytes_count < 1024**4:
        return f"{bytes_count/1024**3:.2f}GB"
    else:
        return f"{bytes_count/1024**4:.2f}TB"


def sanitize_filename(filename: str) -> str:
    """
    Sanitize filename by removing invalid characters.
    
    Parameters
    ----------
    filename : str
        Input filename
        
    Returns
    -------
    str
        Sanitized filename
        
    Examples
    --------
    >>> sanitize_filename("model@name#1.txt")
    'model_name_1.txt'
    """
    invalid_chars = ['<', '>', ':', '"', '/', '\\', '|', '?', '*', '[', ']', ',', ';']
    
    for char in invalid_chars:
        filename = filename.replace(char, '_')
    
    return filename


def setup_logging(log_file: Optional[str] = None, level: str = 'INFO', 
                  format: Optional[str] = None) -> logging.Logger:
    """
    Setup logging configuration.
    
    Parameters
    ----------
    log_file : str, optional
        Path to log file, by default None (console only)
    level : str, optional
        Logging level, by default 'INFO'
    format : str, optional
        Log message format, by default None (standard format)
        
    Returns
    -------
    logging.Logger
        Configured logger
        
    Examples
    --------
    >>> logger = setup_logging("simulation.log", "DEBUG")
    """
    if format is None:
        format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    
    logging.basicConfig(
        level=getattr(logging, level.upper()),
        format=format,
        handlers=[
            logging.FileHandler(log_file) if log_file else logging.StreamHandler(),
            logging.StreamHandler()
        ]
    )
    
    return logging.getLogger()


def get_package_info() -> Dict[str, Any]:
    """
    Get package information (version, dependencies, etc.).
    
    Returns
    -------
    Dict[str, Any]
        Package information dictionary
        
    Examples
    --------
    >>> info = get_package_info()
    """
    import pkg_resources
    
    return {
        "version": pkg_resources.get_distribution("pynlm").version,
        "author": "NLM Team",
        "description": "Python utilities for Neural Language Modeling",
        "dependencies": [
            "numpy>=1.18.0",
            "scipy>=1.4.0",
            "matplotlib>=3.0.0",
            "torch>=1.6.0",
            "tqdm>=4.40.0"
        ]
    }