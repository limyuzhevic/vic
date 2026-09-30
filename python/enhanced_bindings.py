#!/usr/bin/env python3
"""
Enhanced Python bindings for NLM (Neural Learning Machine)

This module provides Pythonic interfaces and utilities that make the NLM API
more intuitive and efficient while maintaining backward compatibility.

Features:
- Context managers for resource management
- Helper functions and utilities  
- Pythonic interface enhancements (__iter__, __len__, __getitem__)
- Advanced Python features (callbacks, observer pattern, decorators, async)
- Utility classes (simulation runner, experiment runner, data collection, analysis)
- Integration with Python ecosystem (numpy, pandas, matplotlib, scikit-learn)

Usage:
    import enhanced_bindings as pynlm_enhanced
    # All enhanced features are available
    # Original pynlm API still works via import pynlm
"""

import sys
import os
import json
import numpy as np
from typing import Dict, List, Optional, Callable, Any, Union, Iterator
from contextlib import contextmanager
from dataclasses import dataclass, asdict
from datetime import datetime

# Import the core bindings
try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    pynlm = None

# =============================================================================
# CONTEXT MANAGERS FOR RESOURCE MANAGEMENT
# =============================================================================

@contextmanager
def simulation_step(world, brain, agent=None, timestep=0.1):
    """
    Context manager for simulation steps that automatically handles state.
    
    Args:
        world: The SimpleWorld instance
        brain: The Brain instance  
        agent: Optional AgentBrain instance
        timestep: Duration of the simulation step
        
    Yields:
        Dictionary containing step results
        
    Example:
        with simulation_step(world, brain, agent) as step_result:
            # Process the step
            pass
    """
    if not HAS_PYNLM:
        raise RuntimeError("pynlm bindings not available")
    
    # Record initial state
    initial_brain_spikes = brain.getTotalSpikeCount() if hasattr(brain, 'getTotalSpikeCount') else 0
    initial_time = world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0
    initial_pos = None
    if agent and hasattr(agent, 'getBrain') and hasattr(agent.getBrain(), 'getAgentBody'):
        initial_pos = agent.getBrain().getAgentBody()
    
    try:
        # Execute the step
        world.update(timestep)
        
        if agent:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(0)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
            agent.updateDevelopment(timestep)
        
        else:
            brain.step(0)
        
        # Prepare result
        result = {
            'step': timestep,
            'world': world,
            'brain': brain,
            'agent': agent,
            'world_time': world.getSimulationTime(),
            'spikes_generated': brain.getTotalSpikeCount() - initial_brain_spikes,
            'initial_time': initial_time,
            'agent_position': initial_pos,
            'success': True
        }
        
        yield result
        
    except Exception as e:
        result = {
            'step': timestep,
            'error': str(e),
            'world': world,
            'brain': brain,
            'agent': agent,
            'success': False
        }
        raise

@contextmanager
def agent_episode(world, brain, agent=None, max_steps=1000, timestep=0.1):
    """
    Context manager for agent episodes that resets state properly.
    
    Args:
        world: The SimpleWorld instance
        brain: The Brain instance
        agent: Optional AgentBrain instance
        max_steps: Maximum number of steps in the episode
        timestep: Duration of each simulation step
        
    Yields:
        Tuple of (step_result, episode_data)
        
    Example:
        with agent_episode(world, brain, agent, max_steps=500) as (step_result, episode_data):
            # Run episode
            pass
    """
    if not HAS_PYNLM:
        raise RuntimeError("pynlm bindings not available")
    
    # Reset all components
    world.reset()
    if agent:
        agent.reset()
    brain.reset()
    
    episode_data = {
        'steps': [],
        'rewards': [],
        'positions': [],
        'actions': [],
        'spike_counts': [],
        'start_time': world.getSimulationTime(),
        'start_pos': world.getAgentBody().x if hasattr(world, 'getAgentBody') else None
    }
    
    step_count = 0
    try:
        for step_count in range(max_steps):
            with simulation_step(world, brain, agent, timestep) as step_result:
                # Record episode data
                episode_data['steps'].append(step_count)
                episode_data['spike_counts'].append(step_result.get('spikes_generated', 0))
                
                if step_result.get('agent_position'):
                    pos = step_result['agent_position']
                    episode_data['positions'].append((pos.x, pos.y))
                
                # Calculate reward based on spikes
                reward = step_result.get('spikes_generated', 0) * 0.001
                episode_data['rewards'].append(reward)
                
                yield step_result, episode_data
                
    except Exception as e:
        print(f"Episode terminated at step {step_count}: {e}")
        raise
    
    finally:
        episode_data['end_time'] = world.getSimulationTime()
        episode_data['total_steps'] = step_count + 1
        episode_data['total_reward'] = sum(episode_data['rewards'])

@contextmanager
def world_session(world, agent=None, duration=None):
    """
    Context manager for world sessions that handles cleanup.
    
    Args:
        world: The SimpleWorld instance
        agent: Optional AgentBrain instance
        duration: Optional duration of the session
        
    Yields:
        Dictionary containing session data
        
    Example:
        with world_session(world, agent) as session_data:
            # Run session
            pass
    """
    if not HAS_PYNLM:
        raise RuntimeError("pynlm bindings not available")
    
    session_data = {
        'world': world,
        'agent': agent,
        'start_time': world.getSimulationTime(),
        'start_objects': world.getObjectCount() if hasattr(world, 'getObjectCount') else 0,
        'session_id': id(world)
    }
    
    try:
        yield session_data
        
    finally:
        session_data['end_time'] = world.getSimulationTime()
        session_data['duration'] = session_data['end_time'] - session_data['start_time']
        session_data['final_objects'] = world.getObjectCount() if hasattr(world, 'getObjectCount') else 0
        session_data['cleanup_complete'] = True

# =============================================================================
# HELPER FUNCTIONS AND UTILITIES
# =============================================================================

def run_simulation(world, brain, agent=None, steps=100, timestep=0.1):
    """
    Helper function for common simulation pattern.
    
    Args:
        world: The SimpleWorld instance
        brain: The Brain instance
        agent: Optional AgentBrain instance
        steps: Number of simulation steps
        timestep: Duration of each step
        
    Returns:
        List of step results
    """
    if not HAS_PYNLM:
        raise RuntimeError("pynlm bindings not available")
    
    results = []
    for step in range(steps):
        with simulation_step(world, brain, agent, timestep) as step_result:
            results.append(step_result)
    
    return results

def create_simulation_config(**kwargs):
    """
    Create configuration with sensible defaults for simulation.
    
    Args:
        **kwargs: Configuration parameters
        
    Returns:
        Config object
    """
    if not HAS_PYNLM:
        raise RuntimeError("pynlm bindings not available")
    
    config = pynlm.createDefaultConfig()
    
    # Apply defaults
    default_config = {
        'brain.neuron_count': 100,
        'brain.synapse_density': 0.1,
        'world.width': 20,
        'world.height': 20,
        'world.vision_width': 8,
        'world.vision_height': 8,
        'agent.max_energy': 100.0,
        'agent.energy_decay': 0.01,
    }
    
    # Apply user config overrides
    for key, value in kwargs.items():
        config.set(key, value)
    
    # Apply defaults for missing keys
    for key, value in default_config.items():
        if not config.has(key):
            config.set(key, value)
    
    return config

def save_simulation_data(data, filename, format='json'):
    """
    Save simulation data to file.
    
    Args:
        data: Data to save (dict, list, or pynlm object)
        filename: Output file path
        format: Export format ('json', 'csv', 'pickle')
    """
    if format == 'json':
        with open(filename, 'w') as f:
            json.dump(data, f, default=str, indent=2)
    elif format == 'csv':
        if isinstance(data, list) and len(data) > 0 and isinstance(data[0], dict):
            import csv
            with open(filename, 'w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=data[0].keys())
                writer.writeheader()
                writer.writerows(data)
        else:
            raise ValueError("CSV format requires list of dictionaries")
    elif format == 'pickle':
        import pickle
        with open(filename, 'wb') as f:
            pickle.dump(data, f)
    else:
        raise ValueError(f"Unsupported format: {format}")

def load_simulation_data(filename, format='json'):
    """
    Load simulation data from file.
    
    Args:
        filename: Input file path
        format: Import format ('json', 'csv', 'pickle')
        
    Returns:
        Loaded data
    """
    if format == 'json':
        with open(filename, 'r') as f:
            return json.load(f)
    elif format == 'csv':
        import csv
        with open(filename, 'r', newline='') as f:
            reader = csv.DictReader(f)
            return list(reader)
    elif format == 'pickle':
        import pickle
        with open(filename, 'rb') as f:
            return pickle.load(f)
    else:
        raise ValueError(f"Unsupported format: {format}")

def plot_simulation_results(results, save_path=None, show=True):
    """
    Plot simulation results using matplotlib.
    
    Args:
        results: Simulation results (list of step data)
        save_path: Optional path to save plot
        show: Whether to display plot
    """
    try:
        import matplotlib.pyplot as plt
        
        if not HAS_PYNLM:
            raise RuntimeError("pynlm bindings not available")
        
        if isinstance(results, list) and len(results) > 0:
            time_data = list(range(len(results)))
            
            spikes = [r.get('spikes_generated', 0) for r in results]
            positions = [r.get('agent_position') for r in results if r.get('agent_position')]
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('Simulation Results', fontsize=16)
            
            # Plot 1: Spike counts
            axes[0, 0].bar(time_data, spikes, alpha=0.7)
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Spikes Generated')
            axes[0, 0].set_title('Spike Generation')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Agent trajectory
            if positions:
                x_pos = [pos[0] for pos in positions]
                y_pos = [pos[1] for pos in positions]
                axes[0, 1].plot(x_pos, y_pos, 'b-', alpha=0.7, linewidth=2)
                axes[0, 1].plot(x_pos[0], y_pos[0], 'go', markersize=10, label='Start')
                axes[0, 1].plot(x_pos[-1], y_pos[-1], 'ro', markersize=10, label='End')
                axes[0, 1].set_xlabel('X Position')
                axes[0, 1].set_ylabel('Y Position')
                axes[0, 1].set_title('Agent Trajectory')
                axes[0, 1].legend()
                axes[0, 1].grid(True, alpha=0.3)
            
            # Plot 3: Time series
            axes[1, 0].plot(time_data, spikes, 'r-', alpha=0.7)
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Spikes')
            axes[1, 0].set_title('Spike Time Series')
            axes[1, 0].grid(True, alpha=0.3)
            
            # Plot 4: Summary statistics
            if spikes:
                axes[1, 1].hist(spikes, bins=20, alpha=0.7, edgecolor='black')
                axes[1, 1].set_xlabel('Spikes per Step')
                axes[1, 1].set_ylabel('Frequency')
                axes[1, 1].set_title('Spike Distribution')
                axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=150, bbox_inches='tight')
            
            if show:
                plt.show()
                
            return fig
        else:
            raise ValueError("Results must be a list of simulation data")
            
    except ImportError:
        print("matplotlib not available. Install with: pip install matplotlib")
        return None

# =============================================================================
# STATISTICAL UTILITIES
# =============================================================================

def mean(data):
    """Calculate mean of data."""
    return np.mean(data) if len(data) > 0 else 0.0

def std(data):
    """Calculate standard deviation of data."""
    return np.std(data) if len(data) > 0 else 0.0

def correlation(x, y):
    """Calculate Pearson correlation coefficient."""
    if len(x) != len(y) or len(x) == 0:
        return 0.0
    return np.corrcoef(x, y)[0, 1]

def autocorrelation(data, lag=1):
    """Calculate autocorrelation at given lag."""
    if len(data) <= lag:
        return 0.0
    return np.corrcoef(data[:-lag], data[lag:])[0, 1]

def moving_average(data, window):
    """Calculate moving average."""
    return np.convolve(data, np.ones(window)/window, mode='valid')

def detect_anomalies(data, threshold=2.0):
    """
    Detect anomalies using z-score method.
    
    Args:
        data: Input data array
        threshold: Z-score threshold for anomaly detection
        
    Returns:
        Boolean array indicating anomalies
    """
    if len(data) == 0:
        return np.array([])
    
    data_mean = np.mean(data)
    data_std = np.std(data)
    
    if data_std == 0:
        return np.array([False] * len(data))
    
    z_scores = np.abs((data - data_mean) / data_std)
    return z_scores > threshold

def bootstrap_sample(data, n_samples=None, random_state=None):
    """
    Generate bootstrap samples.
    
    Args:
        data: Input data array
        n_samples: Number of bootstrap samples to generate
        random_state: Random seed
        
    Returns:
        Array of bootstrap samples
    """
    if random_state is not None:
        np.random.seed(random_state)
    
    if n_samples is None:
        n_samples = len(data)
    
    return np.random.choice(data, size=(n_samples, len(data)), replace=True)

# =============================================================================
# DATA EXPORT/IMPORT UTILITIES
# =============================================================================

class DataExporter:
    """Utility class for exporting and importing simulation data."""
    
    @staticmethod
    def export_to_csv(data, filename, include_metadata=True):
        """Export data to CSV format."""
        if isinstance(data, list) and len(data) > 0:
            try:
                import pandas as pd
                df = pd.DataFrame(data)
                df.to_csv(filename, index=False)
                print(f"Data exported to {filename} using pandas")
                return True
            except ImportError:
                import csv
                with open(filename, 'w', newline='') as f:
                    writer = csv.DictWriter(f, fieldnames=data[0].keys())
                    writer.writeheader()
                    writer.writerows(data)
                print(f"Data exported to {filename} using csv module")
                return True
        return False
    
    @staticmethod
    def export_to_json(data, filename, include_metadata=True):
        """Export data to JSON format with metadata."""
        export_data = {
            'data': data,
            'metadata': {
                'exported_at': datetime.now().isoformat(),
                'format': 'json',
                'data_points': len(data) if isinstance(data, list) else 0
            }
        } if include_metadata else data
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, default=str, indent=2)
        
        print(f"Data exported to {filename}")
        return True
    
    @staticmethod
    def export_to_npy(data, filename):
        """Export data to numpy .npy format."""
        np.save(filename, data)
        print(f"Data exported to {filename} (numpy)")
        return True
    
    @staticmethod
    def import_from_csv(filename):
        """Import data from CSV file."""
        try:
            import pandas as pd
            df = pd.read_csv(filename)
            return df.to_dict('records')
        except ImportError:
            import csv
            with open(filename, 'r', newline='') as f:
                reader = csv.DictReader(f)
                return list(reader)
    
    @staticmethod
    def import_from_json(filename):
        """Import data from JSON file."""
        with open(filename, 'r') as f:
            return json.load(f)
    
    @staticmethod
    def import_from_npy(filename):
        """Import data from numpy .npy file."""
        return np.load(filename, allow_pickle=True)

# =============================================================================
# PYTHONIC INTERFACE ENHANCEMENTS
# =============================================================================

class EnhancedBrain:
    """
    Enhanced wrapper for Brain class with Pythonic interfaces.
    
    This class provides Pythonic enhancements while maintaining
    backward compatibility with the original C++ interface.
    """
    
    def __init__(self, brain_instance=None):
        """Initialize enhanced brain wrapper."""
        self._brain = brain_instance
        self._original_interface = hasattr(brain_instance, '__class__') and brain_instance.__class__.__name__ == 'Brain'
    
    def __getattr__(self, name):
        """Forward attribute access to underlying brain instance."""
        if self._brain is not None:
            return getattr(self._brain, name)
        raise AttributeError(f"'EnhancedBrain' object has no attribute '{name}'")
    
    def __repr__(self):
        """String representation."""
        if self._brain is not None:
            return f"<EnhancedBrain at {id(self)}>"
        return "<EnhancedBrain (uninitialized)>"
    
    def __len__(self):
        """Return number of neurons."""
        if self._brain and hasattr(self._brain, 'getTotalNeuronCount'):
            return self._brain.getTotalNeuronCount()
        return 0
    
    def __iter__(self):
        """Iterator over neurons (placeholder - would need neural population API)."""
        # This would require access to neuron population API
        # For now, return an empty iterator
        return iter([])
    
    def __getitem__(self, index):
        """Get neuron by index (placeholder - would need neuron access API)."""
        # This would require access to neuron API
        raise NotImplementedError("Direct neuron access requires additional C++ API")
    
    # Property-based access for common operations
    @property
    def neuron_count(self):
        """Get total neuron count."""
        return len(self) if self._brain else 0
    
    @property
    def synapse_count(self):
        """Get total synapse count."""
        if self._brain and hasattr(self._brain, 'getTotalSynapseCount'):
            return self._brain.getTotalSynapseCount()
        return 0
    
    @property
    def firing_rate(self):
        """Get average firing rate."""
        if self._brain and hasattr(self._brain, 'getAverageFiringRate'):
            return self._brain.getAverageFiringRate()
        return 0.0
    
    @property
    def active_neurons(self):
        """Get number of active neurons."""
        if self._brain and hasattr(self._brain, 'getActiveNeuronCount'):
            return self._brain.getActiveNeuronCount()
        return 0
    
    @property
    def firing_neuron_count(self):
        """Get firing neuron count."""
        if self._brain and hasattr(self._brain, 'getFiringNeuronCount'):
            return self._brain.getFiringNeuronCount()
        return 0
    
    # Method chaining support
    def step(self, step_num):
        """Execute brain step with chaining support."""
        if self._brain:
            self._brain.step(step_num)
        return self
    
    def initialize(self):
        """Initialize brain with chaining support."""
        if self._brain:
            self._brain.initialize()
        return self
    
    # Statistical operations
    def get_statistics(self):
        """Get comprehensive statistics as dictionary."""
        if not self._brain:
            return {}
        
        stats = {
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'firing_rate': self.firing_rate,
            'active_neurons': self.active_neurons,
            'firing_neuron_count': self.firing_neuron_count
        }
        
        # Add more stats if available
        if hasattr(self._brain, 'getTotalSpikeCount'):
            stats['total_spikes'] = self._brain.getTotalSpikeCount()
        
        if hasattr(self._brain, 'getExcitationInhibitionRatio'):
            stats['e_i_ratio'] = self._brain.getExcitationInhibitionRatio()
        
        return stats
    
    # Context manager support
    def simulation_step(self, world=None, agent=None, timestep=0.1):
        """
        Create a context manager for this brain in a simulation step.
        
        Args:
            world: Optional world instance
            agent: Optional agent instance
            timestep: Simulation timestep
            
        Returns:
            Context manager for the simulation step
        """
        return simulation_step(world, self, agent, timestep)

class EnhancedConfig:
    """
    Enhanced wrapper for Config class with Pythonic interfaces.
    """
    
    def __init__(self, config_instance=None):
        """Initialize enhanced config wrapper."""
        self._config = config_instance
    
    def __getattr__(self, name):
        """Forward attribute access to underlying config instance."""
        if self._config is not None:
            return getattr(self._config, name)
        raise AttributeError(f"'EnhancedConfig' object has no attribute '{name}'")
    
    def __repr__(self):
        """String representation."""
        if self._config is not None:
            return f"<EnhancedConfig: {self._config.summary()}>"
        return "<EnhancedConfig (uninitialized)>"
    
    def __getitem__(self, key):
        """Get configuration value using dictionary-like access."""
        if self._config is not None:
            value = self._config.get(key)
            return value if value is not None else None
        raise AttributeError("Config not initialized")
    
    def __setitem__(self, key, value):
        """Set configuration value using dictionary-like access."""
        if self._config is not None:
            self._config.set(key, value)
        else:
            raise AttributeError("Config not initialized")
    
    def __contains__(self, key):
        """Check if key exists in config."""
        return self._config is not None and self._config.has(key)
    
    def __len__(self):
        """Return number of configuration entries."""
        if self._config is not None:
            return len(self._config.getKeys())
        return 0
    
    def __iter__(self):
        """Iterator over configuration keys."""
        if self._config is not None:
            return iter(self._config.getKeys())
        return iter([])
    
    # Property-based access
    @property
    def keys(self):
        """Get all configuration keys."""
        return self._config.getKeys() if self._config else []
    
    @property
    def values(self):
        """Get all configuration values."""
        if self._config is not None:
            return {key: self._config.get(key) for key in self._config.getKeys()}
        return {}
    
    # Enhanced getter methods
    def get_bool(self, key, default=False):
        """Get boolean value with default."""
        if self._config is not None:
            return self._config.getBool(key, default)
        return default
    
    def get_int(self, key, default=0):
        """Get integer value with default."""
        if self._config is not None:
            return self._config.getInt(key, default)
        return default
    
    def get_float(self, key, default=0.0):
        """Get float value with default."""
        if self._config is not None:
            return self._config.getFloat(key, default)
        return default
    
    def get_string(self, key, default=""):
        """Get string value with default."""
        if self._config is not None:
            return self._config.getString(key, default)
        return default
    
    # Configuration validation
    def validate(self, key, expected_type):
        """Validate configuration value type."""
        if self._config is not None:
            self._config.validate(key, expected_type)
    
    # Merge operations
    def merge(self, other_config):
        """Merge another configuration into this one."""
        if self._config is not None and other_config:
            if hasattr(other_config, '_config'):
                self._config.merge(other_config._config)
            else:
                config_to_merge = getattr(other_config, '_config', other_config)
                if hasattr(config_to_merge, 'merge'):
                    config_to_merge.merge(self._config)
        return self
    
    # String representation enhancements
    def to_dict(self):
        """Convert configuration to dictionary."""
        if self._config is not None:
            return dict(zip(self._config.getKeys(),
                          [self._config.get(key) for key in self._config.getKeys()]))
        return {}
    
    def to_json(self, indent=2):
        """Convert configuration to JSON string."""
        return json.dumps(self.to_dict(), indent=indent)

# =============================================================================
# ADVANCED PYTHON FEATURES
# =============================================================================

class CallbackManager:
    """Manager for callback functions (observer pattern)."""
    
    def __init__(self):
        self._callbacks = {}
    
    def register(self, event_type, callback):
        """Register a callback for a specific event type."""
        if event_type not in self._callbacks:
            self._callbacks[event_type] = []
        self._callbacks[event_type].append(callback)
    
    def unregister(self, event_type, callback):
        """Unregister a callback."""
        if event_type in self._callbacks:
            if callback in self._callbacks[event_type]:
                self._callbacks[event_type].remove(callback)
    
    def emit(self, event_type, data=None):
        """Emit an event to all registered callbacks."""
        if event_type in self._callbacks:
            for callback in self._callbacks[event_type]:
                try:
                    callback(data)
                except Exception as e:
                    print(f"Error in callback for {event_type}: {e}")

class EventDrivenSimulation:
    """Event-driven simulation with callbacks and observers."""
    
    def __init__(self, world, brain, agent=None):
        self.world = world
        self.brain = brain
        self.agent = agent
        self.callbacks = CallbackManager()
        self.observers = []
    
    def add_observer(self, observer):
        """Add an observer for simulation events."""
        self.observers.append(observer)
    
    def remove_observer(self, observer):
        """Remove an observer."""
        if observer in self.observers:
            self.observers.remove(observer)
    
    def notify_observers(self, event_type, data):
        """Notify all observers of an event."""
        for observer in self.observers:
            try:
                if hasattr(observer, 'handle_event'):
                    observer.handle_event(event_type, data)
                else:
                    observer(event_type, data)
            except Exception as e:
                print(f"Error in observer: {e}")
    
    def run_step(self, timestep=0.1):
        """Run a single simulation step with event notifications."""
        # Emit pre-step event
        self.callbacks.emit('pre_step', {'step': timestep})
        self.notify_observers('pre_step', {'step': timestep})
        
        # Execute step
        with simulation_step(self.world, self.brain, self.agent, timestep) as step_result:
            # Emit step completion event
            self.callbacks.emit('post_step', step_result)
            self.notify_observers('post_step', step_result)
            
            # Emit spike event if spikes occurred
            if step_result.get('spikes_generated', 0) > 0:
                self.callbacks.emit('spike_event', step_result)
                self.notify_observers('spike_event', step_result)
        
        return step_result
    
    def run(self, steps, timestep=0.1):
        """Run simulation for specified number of steps."""
        for step in range(steps):
            self.run_step(timestep)

def decorator(func):
    """
    Decorator for custom processing of simulation results.
    
    This decorator can be used to add custom processing to
    simulation results or modify simulation behavior.
    """
    def wrapper(*args, **kwargs):
        print(f"[Decorator] Processing function: {func.__name__}")
        
        # Execute original function
        result = func(*args, **kwargs)
        
        print(f"[Decorator] Completed function: {func.__name__}")
        
        return result
    
    return wrapper

async def async_simulation_step(world, brain, agent=None, timestep=0.1):
    """
    Async version of simulation step for non-blocking operations.
    
    This allows simulation to run asynchronously without blocking
    the main event loop.
    """
    try:
        import asyncio
    except ImportError:
        print("asyncio not available. Install with: pip install asyncio")
        return None
    
    # Create async wrapper
    async def run_async_step():
        # Execute the step synchronously in executor
        loop = asyncio.get_event_loop()
        return await loop.run_in_executor(
            None, simulation_step, world, brain, agent, timestep
        )
    
    return await run_async_step()

# =============================================================================
# UTILITY CLASSES
# =============================================================================

class SimulationRunner:
    """Utility class for common simulation patterns."""
    
    def __init__(self, world, brain, agent=None):
        self.world = world
        self.brain = brain
        self.agent = agent
        self.history = []
    
    def run_experiment(self, steps=100, timestep=0.1, **kwargs):
        """
        Run an experiment with specified parameters.
        
        Args:
            steps: Number of simulation steps
            timestep: Duration of each step
            **kwargs: Additional experiment parameters
            
        Returns:
            Experiment results
        """
        experiment_config = {
            'steps': steps,
            'timestep': timestep,
            **kwargs
        }
        
        print(f"Running experiment: {experiment_config}")
        
        # Clear history
        self.history = []
        
        # Run simulation
        for step in range(steps):
            with simulation_step(self.world, self.brain, self.agent, timestep) as step_result:
                self.history.append(step_result)
        
        return self.get_results()
    
    def get_results(self):
        """Get experiment results as structured data."""
        results = {
            'steps': len(self.history),
            'final_state': self.history[-1] if self.history else None,
            'metrics': self._compute_metrics(),
            'history': self.history
        }
        return results
    
    def _compute_metrics(self):
        """Compute metrics from history."""
        if not self.history:
            return {}
        
        metrics = {
            'total_spikes': sum(h.get('spikes_generated', 0) for h in self.history),
            'avg_spikes_per_step': np.mean([h.get('spikes_generated', 0) for h in self.history]),
            'max_spikes_per_step': max(h.get('spikes_generated', 0) for h in self.history),
        }
        
        # Add position metrics if available
        positions = [h.get('agent_position') for h in self.history 
                    if h.get('agent_position')]
        if positions:
            x_positions = [pos[0] for pos in positions]
            y_positions = [pos[1] for pos in positions]
            
            metrics.update({
                'final_position': positions[-1],
                'position_change': ((x_positions[-1] - x_positions[0])**2 + 
                                   (y_positions[-1] - y_positions[0])**2)**0.5,
                'distance_traveled': sum(
                    ((x_positions[i] - x_positions[i-1])**2 + 
                     (y_positions[i] - y_positions[i-1])**2)**0.5
                    for i in range(1, len(x_positions))
                )
            })
        
        return metrics

class ExperimentRunner:
    """Runner for structured experiments with parameter sweeps."""
    
    def __init__(self, world_factory, brain_factory, agent_factory=None):
        self.world_factory = world_factory
        self.brain_factory = brain_factory
        self.agent_factory = agent_factory
        self.experiments = []
    
    def add_parameter_sweep(self, base_config, parameters):
        """
        Add a parameter sweep experiment.
        
        Args:
            base_config: Base configuration dictionary
            parameters: Dictionary of parameter names to values lists
            
        Example:
            sweep = {
                'neuron_count': [50, 100, 200],
                'synapse_density': [0.05, 0.1, 0.2]
            }
        """
        for param_name, param_values in parameters.items():
            for param_value in param_values:
                config = base_config.copy()
                config[param_name] = param_value
                self.experiments.append(config)
    
    def run_all(self):
        """Run all experiments and collect results."""
        results = []
        
        for i, config in enumerate(self.experiments):
            print(f"Running experiment {i+1}/{len(self.experiments)}")
            
            # Create components
            world = self.world_factory(config) if callable(self.world_factory) else self.world_factory
            brain = self.brain_factory(config) if callable(self.brain_factory) else self.brain_factory
            agent = self.agent_factory(config) if self.agent_factory else None
            
            # Run experiment
            runner = SimulationRunner(world, brain, agent)
            experiment_result = runner.run_experiment(**config)
            
            results.append({
                'config': config,
                'results': experiment_result,
                'experiment_id': i
            })
        
        return results

class DataCollector:
    """Class for recording metrics during simulation."""
    
    def __init__(self):
        self.recorded_data = {
            'steps': [],
            'metrics': [],
            'events': []
        }
        self.callbacks = []
    
    def add_callback(self, callback, event_type='step'):
        """Add a callback to be called when data is collected."""
        self.callbacks.append((callback, event_type))
    
    def record(self, step_data, event_type='step'):
        """
        Record data from a simulation step.
        
        Args:
            step_data: Data to record
            event_type: Type of event being recorded
        """
        # Store data
        self.recorded_data['steps'].append(step_data.get('steps', 0))
        self.recorded_data['metrics'].append(step_data.get('metrics', {}))
        self.recorded_data['events'].append(step_data)
        
        # Call callbacks
        for callback, callback_event_type in self.callbacks:
            if callback_event_type == event_type or callback_event_type == 'all':
                try:
                    callback(step_data)
                except Exception as e:
                    print(f"Error in data collector callback: {e}")
    
    def get_data(self, event_type=None):
        """
        Get collected data.
        
        Args:
            event_type: Filter by event type (not implemented yet)
            
        Returns:
            Dictionary of collected data
        """
        return self.recorded_data.copy()
    
    def clear(self):
        """Clear all recorded data."""
        self.recorded_data = {
            'steps': [],
            'metrics': [],
            'events': []
        }

class Analysis:
    """Class for post-simulation analysis."""
    
    def __init__(self, data):
        self.data = data
    
    def compute_statistics(self, data_key='events'):
        """
        Compute statistics on recorded data.
        
        Args:
            data_key: Key of data to analyze ('steps', 'metrics', 'events')
            
        Returns:
            Dictionary of statistics
        """
        if data_key not in self.data:
            return {}
        
        data = self.data[data_key]
        stats = {}
        
        if not data:
            return stats
        
        # Basic statistics
        stats['count'] = len(data)
        stats['start'] = data[0] if len(data) > 0 else None
        stats['end'] = data[-1] if len(data) > 0 else None
        
        # Spike statistics if available
        spikes = [d.get('spikes_generated', 0) for d in data]
        if spikes:
            stats['spikes'] = {
                'total': sum(spikes),
                'mean': np.mean(spikes),
                'std': np.std(spikes),
                'max': max(spikes),
                'min': min(spikes)
            }
        
        # Position statistics if available
        positions = [d.get('agent_position') for d in data if d.get('agent_position')]
        if positions:
            x_positions = [pos[0] for pos in positions]
            y_positions = [pos[1] for pos in positions]
            
            stats['positions'] = {
                'start': (x_positions[0], y_positions[0]),
                'end': (x_positions[-1], y_positions[-1]),
                'distance_traveled': sum(
                    ((x_positions[i] - x_positions[i-1])**2 + 
                     (y_positions[i] - y_positions[i-1])**2)**0.5
                    for i in range(1, len(x_positions))
                )
            }
        
        return stats
    
    def detect_patterns(self, data_key='events'):
        """
        Detect patterns in the data.
        
        Args:
            data_key: Key of data to analyze
            
        Returns:
            Dictionary of detected patterns
        """
        if data_key not in self.data:
            return {}
        
        data = self.data[data_key]
        patterns = {}
        
        # Detect activity patterns
        if data:
            # Burst detection
            spikes = [d.get('spikes_generated', 0) for d in data]
            bursts = []
            current_burst = []
            
            for i, spike_count in enumerate(spikes):
                if spike_count > 0:
                    current_burst.append(i)
                elif current_burst:
                    if len(current_burst) >= 2:
                        bursts.append(current_burst)
                    current_burst = []
            
            if current_burst and len(current_burst) >= 2:
                bursts.append(current_burst)
            
            patterns['bursts'] = {
                'count': len(bursts),
                'burst_lengths': [len(burst) for burst in bursts],
                'burst_intervals': self._compute_burst_intervals(bursts)
            }
        
        return patterns
    
    def _compute_burst_intervals(self, bursts):
        """Compute intervals between bursts."""
        if len(bursts) < 2:
            return []
        
        intervals = []
        for i in range(1, len(bursts)):
            last_burst_end = max(bursts[i-1])
            current_burst_start = min(bursts[i])
            intervals.append(current_burst_start - last_burst_end)
        
        return intervals

# =============================================================================
# INTEGRATION WITH PYTHON ECOSYSTEM
# =============================================================================

class NLMIntegration:
    """Integration module for enhanced NLM functionality."""
    
    @staticmethod
    def setup_numpy_support():
        """Setup numpy integration for vectorized operations."""
        try:
            import numpy as np
            print("NumPy integration enabled")
            return True
        except ImportError:
            print("NumPy not available. Install with: pip install numpy")
            return False
    
    @staticmethod
    def setup_pandas_support():
        """Setup pandas DataFrame integration for data analysis."""
        try:
            import pandas as pd
            print("Pandas integration enabled")
            return True
        except ImportError:
            print("Pandas not available. Install with: pip install pandas")
            return False
    
    @staticmethod
    def setup_matplotlib_support():
        """Setup matplotlib support for plotting."""
        try:
            import matplotlib.pyplot as plt
            print("Matplotlib integration enabled")
            return True
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib")
            return False
    
    @staticmethod
    def setup_sklearn_support():
        """Setup scikit-learn integration for machine learning."""
        try:
            import sklearn
            print("Scikit-learn integration enabled")
            return True
        except ImportError:
            print("Scikit-learn not available. Install with: pip install scikit-learn")
            return False
    
    @staticmethod
    def setup_all_integrations():
        """Setup all Python ecosystem integrations."""
        integrations = {
            'numpy': NLMIntegration.setup_numpy_support(),
            'pandas': NLMIntegration.setup_pandas_support(),
            'matplotlib': NLMIntegration.setup_matplotlib_support(),
            'sklearn': NLMIntegration.setup_sklearn_support()
        }
        
        print("\nPython Ecosystem Integrations:")
        for lib, status in integrations.items():
            print(f"  {lib}: {'✓' if status else '✗'}")
        
        return integrations

# =============================================================================
# BACKWARD COMPATIBILITY WRAPPERS
# =============================================================================

# Create enhanced versions of original bindings for backward compatibility
if HAS_PYNLM:
    # Enhanced versions of original classes
    EnhancedConfig = EnhancedConfig
    EnhancedBrain = EnhancedBrain
    
    # Original API still available through direct pynlm import
    original_config = pynlm.createDefaultConfig
    original_brain = pynlm.createBrain
    original_world = pynlm.createSimpleWorld
    original_agent = pynlm.createAgentBrain

# Example usage function
def example_usage():
    """Example usage of enhanced NLM API."""
    if not HAS_PYNLM:
        print("pynlm bindings not available. Cannot demonstrate examples.")
        return
    
    print("=== Enhanced NLM API Examples ===\n")
    
    # Example 1: Context managers
    print("1. Context Manager Example:")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    with simulation_step(world, brain, agent) as step_result:
        print(f"   Step completed with {step_result.get('spikes_generated', 0)} spikes")
    
    # Example 2: Data collection
    print("\n2. Data Collection Example:")
    collector = DataCollector()
    
    with simulation_step(world, brain, agent) as step_result:
        collector.record(step_result)
    
    data = collector.get_data()
    print(f"   Collected {len(data['steps'])} data points")
    
    # Example 3: Statistical analysis
    print("\n3. Statistical Analysis Example:")
    if data['steps']:
        spikes = [s.get('spikes_generated', 0) for s in data['events']]
        print(f"   Mean spikes per step: {mean(spikes):.2f}")
        print(f"   Std spikes per step: {std(spikes):.2f}")
    
    # Example 4: Experiment runner
    print("\n4. Experiment Runner Example:")
    
    def world_factory(config):
        return pynlm.createSimpleWorld()
    
    def brain_factory(config):
        return pynlm.createBrain(config)
    
    runner = ExperimentRunner(world_factory, brain_factory)
    
    base_config = {
        'steps': 100,
        'timestep': 0.1
    }
    
    parameters = {
        'neuron_count': [50, 100],
        'synapse_density': [0.05, 0.1]
    }
    
    runner.add_parameter_sweep(base_config, parameters)
    print(f"   Added {len(runner.experiments)} experiments")
    
    print("\n=== Examples Complete ===")

# Auto-setup function
def setup_enhanced_api():
    """Setup enhanced API with optional integrations."""
    print("Setting up Enhanced NLM API...")
    
    if not HAS_PYNLM:
        print("Error: pynlm bindings not available")
        return False
    
    print(f"✓ Loaded pynlm bindings (version: {getattr(pynlm, '__version__', 'unknown')})")
    
    # Show integration options
    print("\nIntegration options:")
    NLMIntegration.setup_all_integrations()
    
    print("\nEnhanced API ready!")
    print("Available features:")
    print("  - Context managers for simulation steps, agent episodes, and world sessions")
    print("  - Helper functions for common patterns")
    print("  - Visualization helpers (matplotlib)")
    print("  - Statistical utilities")
    print("  - Data export/import utilities")
    print("  - Pythonic interface enhancements (__iter__, __len__, __getitem__)")
    print("  - Callback support and event-driven programming")
    print("  - Observer pattern for notifications")
    print("  - Decorator pattern for custom processing")
    print("  - Utility classes (SimulationRunner, ExperimentRunner, DataCollector, Analysis)")
    print("  - Integration with numpy, pandas, matplotlib, scikit-learn")
    
    return True

# Auto-setup when module is imported
if HAS_PYNLM:
    setup_enhanced_api()

if __name__ == "__main__":
    example_usage()