#!/usr/bin/env python3
"""
NLM CLI Utilities

Common utilities for the NLM Command Line Interface.
Includes configuration handling, brain management, and formatting functions.
"""

import json
import yaml
import os
from pathlib import Path
from typing import Dict, Any, Optional, Union
from datetime import datetime

try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False

class NLMCLIError(Exception):
    """Base exception for NLM CLI errors"""
    pass

def load_config(filepath: str) -> Dict[str, Any]:
    """
    Load configuration from file.
    Supports JSON, YAML, and key=value format.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Dictionary of configuration values
        
    Raises:
        NLMCLIError: If file cannot be read or parsed
    """
    path = Path(filepath)
    
    if not path.exists():
        raise NLMCLIError(f"Config file not found: {filepath}")
    
    try:
        if path.suffix.lower() == '.json':
            with open(path, 'r') as f:
                return json.load(f)
        
        elif path.suffix.lower() in ['.yml', '.yaml']:
            with open(path, 'r') as f:
                return yaml.safe_load(f)
        
        elif path.suffix.lower() == '.cfg':
            # Key=value format
            config = {}
            with open(path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#'):
                        if '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to convert to appropriate type
                            if value.lower() in ['true', 'false']:
                                value = value.lower() == 'true'
                            elif value.isdigit():
                                value = int(value)
                            else:
                                try:
                                    value = float(value)
                                except ValueError:
                                    pass
                            
                            config[key] = value
            return config
        
        else:
            raise NLMCLIError(f"Unsupported config file format: {path.suffix}")
    
    except (json.JSONDecodeError, yaml.YAMLError, UnicodeDecodeError) as e:
        raise NLMCLIError(f"Failed to parse config file {filepath}: {e}")
    except Exception as e:
        raise NLMCLIError(f"Error reading config file {filepath}: {e}")

def save_config(config: Dict[str, Any], filepath: str) -> bool:
    """
    Save configuration to file.
    Supports JSON and YAML formats (prefers YAML for readability).
    
    Args:
        config: Configuration dictionary
        filepath: Path to save configuration
        
    Returns:
        True if successful
        
    Raises:
        NLMCLIError: If file cannot be written
    """
    path = Path(filepath)
    
    # Create parent directory if it doesn't exist
    path.parent.mkdir(parents=True, exist_ok=True)
    
    try:
        if path.suffix.lower() in ['.yml', '.yaml']:
            with open(path, 'w') as f:
                yaml.dump(config, f, default_flow_style=False, sort_keys=False)
        
        elif path.suffix.lower() == '.json':
            with open(path, 'w') as f:
                json.dump(config, f, indent=2, sort_keys=False)
        
        else:
            # Default to YAML
            path = path.with_suffix('.yaml')
            with open(path, 'w') as f:
                yaml.dump(config, f, default_flow_style=False, sort_keys=False)
        
        return True
    
    except Exception as e:
        raise NLMCLIError(f"Failed to save config to {filepath}: {e}")

def format_number(num: Union[int, float]) -> str:
    """Format a number for display"""
    if isinstance(num, float):
        if num < 1000:
            return f"{num:.2f}"
        elif num < 1000000:
            return f"{num/1000:.1f}k"
        else:
            return f"{num/1000000:.1f}M"
    else:
        if num < 1000:
            return str(num)
        elif num < 1000000:
            return f"{num/1000}k"
        else:
            return f"{num/1000000}M"

def format_duration(seconds: float) -> str:
    """Format duration in seconds to human-readable string"""
    if seconds < 60:
        return f"{seconds:.1f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"

def format_percentage(value: float) -> str:
    """Format a value as percentage"""
    return f"{value:.1f}%"

def create_brain(config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create and initialize a brain with given configuration.
    
    Args:
        config: Configuration dictionary. If None, uses default config.
        
    Returns:
        Initialized brain object
        
    Raises:
        NLMCLIError: If brain cannot be created or initialized
    """
    if not NLM_AVAILABLE:
        raise NLMCLIError("pynlm module is not available")
    
    try:
        # Create config from dictionary if provided
        if config:
            pynlm_config = pynlm.createDefaultConfig()
            # Convert config dict to pynlm config
            # This is a simplified conversion - pynlm config API might be different
            for key, value in config.items():
                if isinstance(value, bool):
                    pynlm_config.set(key, value)
                elif isinstance(value, int):
                    pynlm_config.set(key, value)
                elif isinstance(value, float):
                    pynlm_config.set(key, value)
                elif isinstance(value, str):
                    pynlm_config.set(key, value)
        else:
            pynlm_config = pynlm.createDefaultConfig()
        
        # Create and initialize brain
        brain = pynlm.createBrain(pynlm_config)
        brain.initialize()
        
        return brain
    
    except Exception as e:
        raise NLMCLIError(f"Failed to create brain: {e}")

def run_simulation(brain: Any, steps: int, verbose: bool = False) -> Dict[str, Any]:
    """
    Run a simulation with the given brain for specified steps.
    
    Args:
        brain: Brain object
        steps: Number of steps to run
        verbose: Enable verbose output
        
    Returns:
        Dictionary with simulation results
        
    Raises:
        NLMCLIError: If simulation fails
    """
    if not NLM_AVAILABLE:
        raise NLMCLIError("pynlm module is not available")
    
    try:
        # Store initial state
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
            if verbose and step % 100 == 0:
                firing = brain.getFiringNeuronCount()
                spikes = brain.getTotalSpikeCount() - initial_spikes
                print(f"Step {step}: {firing} firing, {spikes} spikes")
        
        # Collect results
        final_spikes = brain.getTotalSpikeCount() - initial_spikes
        final_firing = brain.getFiringNeuronCount()
        avg_firing = brain.getAverageFiringRate()
        
        return {
            'steps': steps,
            'initial_spikes': initial_spikes,
            'final_spikes': final_spikes,
            'spike_increase': final_spikes - initial_spikes,
            'final_firing': final_firing,
            'average_firing_rate': avg_firing,
            'success': True
        }
    
    except Exception as e:
        raise NLMCLIError(f"Simulation failed: {e}")

def format_stats(brain: Any) -> str:
    """
    Format brain statistics as a readable string.
    
    Args:
        brain: Brain object
        
    Returns:
        Formatted statistics string
    """
    try:
        stats = [
            "=== NLM Brain Statistics ===",
            f"Total Neurons: {brain.getTotalNeuronCount()}",
            f"Total Synapses: {brain.getTotalSynapseCount()}",
            f"Active Neurons: {brain.getActiveNeuronCount()}",
            f"Firing Neurons: {brain.getFiringNeuronCount()}",
            f"Total Spikes: {brain.getTotalSpikeCount()}",
            f"Average Firing Rate: {brain.getAverageFiringRate():.2f}",
            f"E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}",
            f"Pending Spike Events: {brain.getPendingSpikeEventCount()}",
            f"Developmental Stage: {brain.getDevelopmentalStage()}"
        ]
        
        return "\n".join(stats)
    
    except Exception as e:
        return f"Error formatting stats: {e}"

def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
    """
    Validate and normalize configuration.
    
    Args:
        config: Configuration dictionary
        
    Returns:
        Validated and normalized configuration
        
    Raises:
        NLMCLIError: If configuration is invalid
    """
    # Default values for required configuration keys
    defaults = {
        'neuron_count': 500,
        'region_count': 1,
        'connection_probability': 0.1,
        'simulation_timestep': 0.001,
        'random_seed': 42,
        'enable_plasticity': True,
        'enable_development': True,
        'enable_neuromodulation': True
    }
    
    # Merge with defaults
    validated_config = defaults.copy()
    validated_config.update(config)
    
    # Validate values
    if not isinstance(validated_config['neuron_count'], int) or validated_config['neuron_count'] <= 0:
        raise NLMCLIError("neuron_count must be a positive integer")
    
    if not isinstance(validated_config['region_count'], int) or validated_config['region_count'] <= 0:
        raise NLMCLIError("region_count must be a positive integer")
    
    if not isinstance(validated_config['connection_probability'], float) or \
       not (0.0 <= validated_config['connection_probability'] <= 1.0):
        raise NLMCLIError("connection_probability must be a float between 0 and 1")
    
    return validated_config

def create_markdown_table(headers: list, rows: list) -> str:
    """
    Create a markdown-formatted table.
    
    Args:
        headers: List of column headers
        rows: List of rows (each row is a list of values)
        
    Returns:
        Markdown table string
    """
    # Calculate column widths
    col_widths = [len(str(h)) for h in headers]
    for row in rows:
        for i, value in enumerate(row):
            col_widths[i] = max(col_widths[i], len(str(value)))
    
    # Create the table
    lines = []
    
    # Header
    header_line = "| " + " | ".join(
        str(h).ljust(col_widths[i]) for i, h in enumerate(headers)
    ) + " |"
    lines.append(header_line)
    
    # Separator
    sep_line = "| " + " | ".join(
        "-" * col_widths[i] for i in range(len(headers))
    ) + " |"
    lines.append(sep_line)
    
    # Rows
    for row in rows:
        row_line = "| " + " | ".join(
            str(row[i]).ljust(col_widths[i]) if i < len(row) else ""
            for i in range(len(headers))
        ) + " |"
        lines.append(row_line)
    
    return "\n".join(lines)

# Create simple YAML support if not available
if not NLM_AVAILABLE or 'yaml' not in locals():
    try:
        import yaml
    except ImportError:
        # Add a minimal yaml-like function
        def safe_load(content):
            # Very basic YAML parser for simple key=value pairs
            result = {}
            for line in content.split('\n'):
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    result[key.strip()] = value.strip()
            return result
        
        class YamlDumper:
            def __init__(self, file):
                self.file = file
            
            def dump(self, data, **kwargs):
                for key, value in data.items():
                    self.file.write(f"{key}: {value}\n")
        
        class YamlLoader:
            def safe_load(self, content):
                return safe_load(content)
        
        yaml = type('yaml', (), {
            'safe_load': YamlLoader().safe_load,
            'dump': YamlDumper
        })()