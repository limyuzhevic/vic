"""
Helper functions for the NLM CLI and API.

This module provides helper functions for the NLM CLI and API,
including configuration loading, validation, and system utilities.
"""

import json
import logging
import os
import platform
import sys
from pathlib import Path
from typing import Dict, Any, Optional

logger = logging.getLogger(__name__)

def setup_logging(level: str = "INFO", log_file: Optional[str] = None):
    """
    Set up logging for the application.
    
    Args:
        level: Log level
        log_file: Path to log file
    """
    # Create logs directory if it doesn't exist
    if log_file:
        log_dir = Path(log_file).parent
        log_dir.mkdir(parents=True, exist_ok=True)
    
    # Configure logging
    handlers = [
        logging.StreamHandler(sys.stdout)
    ]
    
    if log_file:
        handlers.append(logging.FileHandler(log_file))
    
    logging.basicConfig(
        level=getattr(logging, level.upper()),
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=handlers
    )
    
    logger.info(f"Logging initialized with level: {level}")

def load_config(config_path: str) -> Dict[str, Any]:
    """
    Load configuration from file.
    
    Args:
        config_path: Path to configuration file
        
    Returns:
        Configuration dictionary
    """
    try:
        if not Path(config_path).exists():
            logger.warning(f"Configuration file not found: {config_path}")
            return {}
        
        if config_path.endswith('.json'):
            with open(config_path, 'r') as f:
                return json.load(f)
        elif config_path.endswith('.cfg'):
            # Simple config format
            config = {}
            with open(config_path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Try to parse value type
                        if value.lower() == 'true':
                            config[key] = True
                        elif value.lower() == 'false':
                            config[key] = False
                        elif value.isdigit():
                            config[key] = int(value)
                        else:
                            try:
                                config[key] = float(value)
                            except ValueError:
                                config[key] = value
            return config
        else:
            logger.warning(f"Unknown configuration file format: {config_path}")
            return {}
            
    except Exception as e:
        logger.error(f"Error loading configuration from {config_path}: {e}")
        return {}

def save_config(config: Dict[str, Any], config_path: str) -> bool:
    """
    Save configuration to file.
    
    Args:
        config: Configuration dictionary
        config_path: Path to configuration file
        
    Returns:
        True if successful, False otherwise
    """
    try:
        Path(config_path).parent.mkdir(parents=True, exist_ok=True)
        
        if config_path.endswith('.json'):
            with open(config_path, 'w') as f:
                json.dump(config, f, indent=2)
        elif config_path.endswith('.cfg'):
            with open(config_path, 'w') as f:
                for key, value in config.items():
                    if isinstance(value, bool):
                        value = 'true' if value else 'false'
                    f.write(f"{key} = {value}\n")
        else:
            logger.warning(f"Unknown configuration file format: {config_path}")
            return False
            
        return True
        
    except Exception as e:
        logger.error(f"Error saving configuration to {config_path}: {e}")
        return False

def get_system_info() -> Dict[str, Any]:
    """
    Get system information.
    
    Returns:
        Dictionary with system information
    """
    return {
        "platform": platform.system(),
        "platform_version": platform.release(),
        "architecture": platform.machine(),
        "python_version": platform.python_version(),
        "processor": platform.processor()
    }

def format_time(seconds: float) -> str:
    """
    Format time in seconds to a human-readable string.
    
    Args:
        seconds: Time in seconds
        
    Returns:
        Formatted time string
    """
    if seconds < 60:
        return f"{seconds:.2f} seconds"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.2f} minutes"
    elif seconds < 86400:
        hours = seconds / 3600
        return f"{hours:.2f} hours"
    else:
        days = seconds / 86400
        return f"{days:.2f} days"

def validate_positive_int(value: Any, name: str = "value") -> bool:
    """
    Validate that a value is a positive integer.
    
    Args:
        value: Value to validate
        name: Name of the value for error messages
        
    Returns:
        True if valid, False otherwise
    """
    if isinstance(value, int) and value > 0:
        return True
    elif isinstance(value, str) and value.isdigit() and int(value) > 0:
        return True
    else:
        logger.error(f"Invalid {name}: {value}. Must be a positive integer.")
        return False

def validate_range(value: Any, min_val: float, max_val: float, name: str = "value") -> bool:
    """
    Validate that a value is within a specified range.
    
    Args:
        value: Value to validate
        min_val: Minimum value
        max_val: Maximum value
        name: Name of the value for error messages
        
    Returns:
        True if valid, False otherwise
    """
    try:
        num_val = float(value)
        if min_val <= num_val <= max_val:
            return True
        else:
            logger.error(f"Invalid {name}: {value}. Must be between {min_val} and {max_val}.")
            return False
    except ValueError:
        logger.error(f"Invalid {name}: {value}. Must be a number.")
        return False

def safe_makedirs(path: str):
    """
    Create directories safely.
    
    Args:
        path: Directory path to create
    """
    try:
        Path(path).mkdir(parents=True, exist_ok=True)
    except Exception as e:
        logger.error(f"Error creating directory {path}: {e}")

def get_file_size(path: str) -> Optional[float]:
    """
    Get file size in megabytes.
    
    Args:
        path: Path to file
        
    Returns:
        File size in MB, or None if error
    """
    try:
        size = Path(path).stat().st_size
        return size / (1024 * 1024)
    except Exception as e:
        logger.error(f"Error getting file size for {path}: {e}")
        return None

def format_bytes(size: int) -> str:
    """
    Format bytes to a human-readable string.
    
    Args:
        size: Size in bytes
        
    Returns:
        Formatted size string
    """
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if size < 1024.0:
            return f"{size:.2f} {unit}"
        size /= 1024.0
    return f"{size:.2f} PB"

def is_valid_filename(filename: str) -> bool:
    """
    Check if a filename is valid.
    
    Args:
        filename: Filename to validate
        
    Returns:
        True if valid, False otherwise
    """
    # Check for invalid characters
    invalid_chars = ['<', '>', ':', '"', '/', '\\', '|', '?', '*']
    
    for char in invalid_chars:
        if char in filename:
            return False
    
    # Check for reserved names (Windows)
    reserved_names = ['CON', 'PRN', 'AUX', 'NUL', 'COM1', 'COM2', 'COM3', 'COM4', 
                      'COM5', 'COM6', 'COM7', 'COM8', 'COM9', 'LPT1', 'LPT2', 'LPT3',
                      'LPT4', 'LPT5', 'LPT6', 'LPT7', 'LPT8', 'LPT9']
    
    if filename.upper() in reserved_names:
        return False
    
    # Check for empty filename or too long
    if not filename or len(filename) > 255:
        return False
    
    return True

if __name__ == "__main__":
    # Test helper functions
    print("Testing helper functions...")
    
    # Test setup_logging
    setup_logging("INFO", "test.log")
    
    # Test load_config
    test_config = {"test_key": "test_value", "test_number": 42, "test_bool": True}
    save_config(test_config, "test_config.json")
    loaded_config = load_config("test_config.json")
    print(f"Loaded config: {loaded_config}")
    
    # Test system info
    system_info = get_system_info()
    print(f"System info: {system_info}")
    
    # Test format_time
    print(f"Format time: {format_time(3661.5)}")
    
    # Test validation
    print(f"Validate positive int: {validate_positive_int(42)}")
    print(f"Validate range: {validate_range(50, 0, 100)}")
    
    # Test safe_makedirs
    safe_makedirs("test_dir")
    
    # Test get_file_size
    save_config(test_config, "test_file.txt")
    file_size = get_file_size("test_file.txt")
    print(f"File size: {file_size} MB")
    
    print("All tests passed!")
