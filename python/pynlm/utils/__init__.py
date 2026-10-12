"""
Utility functions for NLM Python package

This module contains helper functions and utilities for:
- Configuration management
- File I/O operations
- Data processing and transformation
- Progress tracking and reporting
- Error handling and validation
"""

import json
import os
import time
from pathlib import Path
from typing import Dict, Any, Optional, Union


def save_config(config_dict: Dict[str, Any], filepath: str) -> bool:
    """
    Save configuration to JSON file.

    Args:
        config_dict: Configuration dictionary
        filepath: Output file path

    Returns:
        bool: True if successful, False otherwise
    """
    try:
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        return True
    except Exception as e:
        print(f"Error saving config: {e}")
        return False


def load_config(filepath: str) -> Optional[Dict[str, Any]]:
    """
    Load configuration from JSON file.

    Args:
        filepath: Input file path

    Returns:
        dict or None: Configuration dictionary if successful, None otherwise
    """
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading config: {e}")
        return None


def ensure_directory(path: str) -> bool:
    """
    Ensure directory exists, create if necessary.

    Args:
        path: Directory path

    Returns:
        bool: True if successful, False otherwise
    """
    try:
        Path(path).mkdir(parents=True, exist_ok=True)
        return True
    except Exception as e:
        print(f"Error creating directory {path}: {e}")
        return False


def format_time(seconds: float) -> str:
    """
    Format time in human-readable format.

    Args:
        seconds: Time in seconds

    Returns:
        str: Formatted time string
    """
    if seconds < 60:
        return f"{seconds:.2f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"


def get_memory_usage() -> Dict[str, float]:
    """
    Get memory usage information.

    Returns:
        dict: Memory usage statistics
    """
    try:
        import psutil
        process = psutil.Process()
        mem_info = process.memory_info()
        return {
            'rss_mb': mem_info.rss / 1024 / 1024,
            'vms_mb': mem_info.vms / 1024 / 1024,
        }
    except ImportError:
        return {'error': 'psutil not available'}
    except Exception as e:
        return {'error': str(e)}


def validate_config(config: Dict[str, Any]) -> tuple[bool, str]:
    """
    Validate configuration dictionary.

    Args:
        config: Configuration dictionary

    Returns:
        tuple: (is_valid, error_message)
    """
    required_keys = ['random_seed', 'simulation_timestep', 'neuron_count']

    for key in required_keys:
        if key not in config:
            return False, f"Missing required configuration key: {key}"

    if config['neuron_count'] <= 0:
        return False, "neuron_count must be positive"

    if config['simulation_timestep'] <= 0:
        return False, "simulation_timestep must be positive"

    return True, "Configuration valid"


def run_with_progress(func, *args, description: str = "Running", **kwargs):
    """
    Run a function with progress reporting.

    Args:
        func: Function to run
        *args: Positional arguments for function
        description: Description for progress reporting
        **kwargs: Keyword arguments for function

    Returns:
        Result of function call
    """
    print(f"{description}...")
    start_time = time.time()

    try:
        result = func(*args, **kwargs)
        elapsed = time.time() - start_time
        print(f"{description} completed in {format_time(elapsed)}")
        return result
    except Exception as e:
        elapsed = time.time() - start_time
        print(f"{description} failed after {format_time(elapsed)}: {e}")
        raise
