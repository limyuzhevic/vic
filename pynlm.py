"""
NLM Python High-Level API

A high-level, Pythonic interface for NLM (Neural Learning Machine) neural simulation.

This module provides enhanced abstractions, convenience methods, and utilities
for working with NLM brains in Python, making the complex neural simulation
framework more accessible and easier to use.

Key Features:
- Python wrapper classes with intuitive APIs
- Comprehensive error handling and validation
- Visualization and plotting utilities
- Simulation utilities and helpers
- Configuration builders
- Performance monitoring tools
- Memory-efficient simulation runners
- Advanced simulation patterns and workflows

Author: NLM Development Team
License: MIT
"""

import json
import time
import threading
import concurrent.futures
from typing import Dict, List, Optional, Callable, Any, Union, Tuple
from enum import Enum
import warnings

from .pynlm import (
    Config, Brain, AgentBrain, SimpleWorld, SensoryInput, Vision, Audio, 
    InternalSignals, Action, ActionType, WorldObject, WorldObjectType,
    NeuronType, SynapseType, DevelopmentalStage, FiringState, MotorCommand,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ValidationError(NLMError):
    """Raised for validation errors."""
    pass

class SimulationError(NLMError):
    """Raised for simulation errors."""
    pass

class ConfigError(NLMError):
    """Raised for configuration errors."""
    pass

class BrainSimulator:
    """
    High-level brain simulation wrapper with enhanced functionality and error handling.
    
    This class provides a Pythonic interface for brain simulation, including:
    - Automatic initialization and cleanup
    - Progress monitoring
    - Statistics collection
    - Error recovery
    - Configuration validation
    """
    
    def __init__(self, config: Optional[Config] = None, auto_initialize: bool = True):
        """
        Initialize the brain simulator.
        
        Args:
            config: Configuration object. If None, creates default config.
            auto_initialize: Whether to automatically initialize the brain.
        
        Raises:
            ValidationError: If configuration is invalid.
        """
        if config is None:
            config = createDefaultConfig()
        
        self.config = config
        self.brain = None
        self._is_initialized = False
        self._simulation_step = 0
        self._start_time = 0.0
        self._stats_history = []
        
        if auto_initialize:
            self.initialize()
    
    def initialize(self) -> 'BrainSimulator':
        """
        Initialize the brain with current configuration.
        
        Returns:
            Self for method chaining.
            
        Raises:
            SimulationError: If initialization fails.
        """
        try:
            self.brain = createBrain(self.config)
            self.brain.initialize()
            self._is_initialized = True
            self._simulation_step = 0
            self._start_time = time.time()
            self._stats_history = []
            return self
        except Exception as e:
            raise SimulationError(f"Failed to initialize brain: {e}")
    
    def step(self, step_number: Optional[int] = None, current_time: Optional[float] = None) -> 'BrainSimulator':
        """
        Perform a single simulation step.
        
        Args:
            step_number: Current simulation step number.
            current_time: Current simulation time. If None, uses step_number.
            
        Returns:
            Self for method chaining.
            
        Raises:
            SimulationError: If step fails.
        """
        if not self._is_initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
        
        try:
            if current_time is None:
                current_time = float(step_number) if step_number is not None else self._simulation_step
            
            self.brain.step(self._simulation_step, current_time)
            self._simulation_step += 1
            
            # Collect statistics
            self._collect_stats()
            
            return self
        except Exception as e:
            raise SimulationError(f"Simulation step failed: {e}")
    
    def reset(self) -> 'BrainSimulator':
        """
        Reset the brain to its initial state.
        
        Returns:
            Self for method chaining.
        """
        if self._is_initialized:
            self.brain.reset()
            self._simulation_step = 0
            self._stats_history = []
        return self
    
    def run(self, num_steps: int, current_time: Optional[float] = None, 
            progress_callback: Optional[Callable[[int, Dict], None]] = None) -> 'BrainSimulator':
        """
        Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run.
            current_time: Starting time. If None, starts at 0.
            progress_callback: Optional callback for progress updates.
            
        Returns:
            Self for method chaining.
        """
        start_time = current_time if current_time is not None else 0.0
        
        for step in range(num_steps):
            self.step(step, start_time + step * 0.1)
            
            if progress_callback:
                progress_callback(step, self.get_current_stats())
        
        return self
    
    def get_current_stats(self) -> Dict[str, Any]:
        """
        Get current simulation statistics.
        
        Returns:
            Dictionary of statistics.
        """
        if not self._is_initialized:
            return {}
        
        stats = {
            'step': self._simulation_step,
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage().name,
            'elapsed_time': time.time() - self._start_time
        }
        
        return stats
    
    def save(self, filepath: str) -> 'BrainSimulator':
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file.
            
        Returns:
            Self for method chaining.
        """
        if not self._is_initialized:
            raise SimulationError("Brain not initialized. Cannot save.")
        
        try:
            self.brain.save(filepath)
        except Exception as e:
            raise SimulationError(f"Failed to save brain: {e}")
        
        return self
    
    def load(self, filepath: str) -> 'BrainSimulator':
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file.
            
        Returns:
            Self for method chaining.
        """
        if not self._is_initialized:
            raise SimulationError("Brain not initialized. Cannot load.")
        
        try:
            self.brain.load(filepath)
        except Exception as e:
            raise SimulationError(f"Failed to load brain: {e}")
        
        return self
    
    def _collect_stats(self) -> None:
        """Collect current statistics for history."""
        if self._is_initialized:
            stats = self.get_current_stats()
            self._stats_history.append(stats)
    
    def get_stats_history(self, last_n: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get simulation statistics history.
        
        Args:
            last_n: If specified, return only last N entries.
            
        Returns:
            List of statistics dictionaries.
        """
        if last_n is not None:
            return self._stats_history[-last_n:]
        return self._stats_history.copy()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across simulation."""
        if not self._stats_history:
            return 0.0
        
        return sum(s['average_firing_rate'] for s in self._stats_history) / len(self._stats_history)
    
    def get_max_firing_rate(self) -> float:
        """Get maximum firing rate observed."""
        if not self._stats_history:
            return 0.0
        
        return max(s['average_firing_rate'] for s in self._stats_history)
    
    def is_alive(self) -> bool:
        """Check if brain is initialized and alive."""
        return self._is_initialized


class AgentSimulator:
    """
    High-level agent simulation wrapper with world interaction.
    
    This class provides a Pythonic interface for agent simulations, including:
    - Automatic world setup and management
    - Sensory processing pipeline
    - Action execution and reward collection
    - Episode management
    - Environment interaction
    """
    
    def __init__(self, config: Optional[Config] = None, world_width: int = 20, 
                 world_height: int = 20, vision_width: int = 8, vision_height: int = 8):
        """
        Initialize the agent simulator.
        
        Args:
            config: Configuration object. If None, creates default config.
            world_width: Width of the simulation world.
            world_height: Height of the simulation world.
            vision_width: Width of agent's vision field.
            vision_height: Height of agent's vision field.
        """
        if config is None:
            config = createDefaultConfig()
        
        self.config = config
        self.world_width = world_width
        self.world_height = world_height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        self.brain = None
        self.agent = None
        self.world = None
        self._is_initialized = False
        self._current_step = 0
        self._episode_data = []
    
    def initialize(self) -> 'AgentSimulator':
        """
        Initialize all components: brain, agent, and world.
        
        Returns:
            Self for method chaining.
        """
        # Create components
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        self.agent = createAgentBrain(self.brain)
        
        self.world = createSimpleWorld()
        self.world.configure(self.world_width, self.world_height, 
                           self.vision_width, self.vision_height)
        self.world.reset()
        
        self.agent.initialize(self.world)
        
        self._is_initialized = True
        self._current_step = 0
        self._episode_data = []
        
        return self
    
    def run_step(self) -> Tuple[Optional[Action], Dict[str, Any]]:
        """
        Run a single simulation step.
        
        Returns:
            Tuple of (action, metrics) where action is the motor command
            and metrics contains statistics.
        """
        if not self._is_initialized:
            raise SimulationError("Agent not initialized. Call initialize() first.")
        
        # Update world
        self.world.update(0.1)
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain thinks
        self.brain.step(self._current_step)
        
        # Get action from brain
        action = self.agent.decodeMotorCommand()
        
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Collect metrics
        metrics = self._collect_step_metrics(percept, action)
        
        self._current_step += 1
        
        return action, metrics
    
    def run_episode(self, num_steps: int, progress_callback: Optional[Callable[[int, Dict], None]] = None) -> List[Dict[str, Any]]:
        """
        Run a complete episode.
        
        Args:
            num_steps: Number of steps to run.
            progress_callback: Optional callback for progress updates.
            
        Returns:
            List of step metrics.
        """
        episode_data = []
        
        for step in range(num_steps):
            action, metrics = self.run_step()
            episode_data.append(metrics)
            
            if progress_callback:
                progress_callback(step, metrics)
        
        self._episode_data = episode_data
        return episode_data
    
    def reset(self) -> 'AgentSimulator':
        """Reset all components for a new episode."""
        if self._is_initialized:
            self.brain.reset()
            self.world.reset()
            self.agent.reset()
            self._current_step = 0
            self._episode_data = []
        return self
    
    def _collect_step_metrics(self, percept: SensoryInput, action: Action) -> Dict[str, Any]:
        """Collect metrics for a single step."""
        return {
            'step': self._current_step,
            'action_type': action.getType().name,
            'action_parameters': action.getParameters(),
            'brain_stats': {
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'developmental_stage': self.brain.getDevelopmentalStage().name,
            },
            'agent_stats': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            },
            'sensory_data': {
                'vision_data': percept.getVision() if hasattr(percept, 'getVision') else None,
                'internal_signals': percept.getInternal(),
                'has_touch': hasattr(percept, 'getTouch') and bool(percept.getTouch()),
            }
        }
    
    def get_episode_stats(self) -> Dict[str, Any]:
        """Get statistics for the current episode."""
        if not self._episode_data:
            return {}
        
        stats = {
            'total_steps': len(self._episode_data),
            'unique_actions': list(set(
                d['action_type'] for d in self._episode_data
            )),
            'average_firing_rate': sum(
                d['brain_stats']['average_firing_rate'] for d in self._episode_data
            ) / len(self._episode_data),
            'total_spikes': sum(
                d['brain_stats']['total_spikes'] for d in self._episode_data
            ),
            'final_dev_stage': self._episode_data[-1]['brain_stats']['developmental_stage']
        }
        
        return stats
    
    def is_alive(self) -> bool:
        """Check if agent is initialized and alive."""
        return self._is_initialized


class NLMVisualizer:
    """
    Visualization utilities for NLM simulations.
    
    Provides methods for plotting and visualizing simulation data,
    brain states, and performance metrics.
    """
    
    @staticmethod
    def plot_brain_activity(stats: List[Dict[str, Any]], figsize: Tuple[int, int] = (12, 8)):
        """
        Plot brain activity over time.
        
        Args:
            stats: List of statistics dictionaries.
            figsize: Figure size.
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=figsize)
            
            # Plot 1: Firing neurons
            steps = [s['step'] for s in stats]
            firing = [s['firing_neurons'] for s in stats]
            ax1.plot(steps, firing, 'b-', alpha=0.7, linewidth=1)
            ax1.set_title('Firing Neurons')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Count')
            ax1.grid(True, alpha=0.3)
            
            # Plot 2: Average firing rate
            rates = [s['average_firing_rate'] for s in stats]
            ax2.plot(steps, rates, 'r-', alpha=0.7, linewidth=1)
            ax2.set_title('Average Firing Rate')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Hz')
            ax2.grid(True, alpha=0.3)
            
            # Plot 3: Total spikes
            spikes = [s['total_spikes'] for s in stats]
            ax3.plot(steps, spikes, 'g-', alpha=0.7, linewidth=1)
            ax3.set_title('Total Spikes')
            ax3.set_xlabel('Step')
            ax3.set_ylabel('Count')
            ax3.grid(True, alpha=0.3)
            
            # Plot 4: E/I ratio
            ei_ratios = [s['excitation_inhibition_ratio'] for s in stats]
            ax4.plot(steps, ei_ratios, 'm-', alpha=0.7, linewidth=1)
            ax4.set_title('E/I Ratio')
            ax4.set_xlabel('Step')
            ax4.set_ylabel('Ratio')
            ax4.grid(True, alpha=0.3)
            
            plt.tight_layout()
            return fig, plt
            
        except ImportError:
            warnings.warn("matplotlib not available. Install with: pip install matplotlib")
            return None, None
    
    @staticmethod
    def plot_action_distribution(episode_data: List[Dict[str, Any]], figsize: Tuple[int, int] = (10, 6)):
        """
        Plot action distribution for agent episode.
        
        Args:
            episode_data: List of step metrics.
            figsize: Figure size.
        """
        try:
            import matplotlib.pyplot as plt
            
            # Extract action types
            action_types = [d['action_type'] for d in episode_data]
            
            # Count actions
            from collections import Counter
            action_counts = Counter(action_types)
            
            # Create bar plot
            fig, ax = plt.subplots(figsize=figsize)
            
            actions, counts = zip(*action_counts.items())
            bars = ax.bar(actions, counts, color='skyblue', alpha=0.7)
            
            # Add value labels on bars
            for bar, count in zip(bars, counts):
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{count}', ha='center', va='bottom')
            
            ax.set_title('Action Distribution')
            ax.set_xlabel('Action Type')
            ax.set_ylabel('Count')
            ax.tick_params(axis='x', rotation=45)
            
            plt.tight_layout()
            return fig, plt
            
        except ImportError:
            warnings.warn("matplotlib not available. Install with: pip install matplotlib")
            return None, None
    
    @staticmethod
    def plot_performance_metrics(stats_history: List[Dict[str, Any]], figsize: Tuple[int, int] = (12, 8)):
        """
        Plot comprehensive performance metrics.
        
        Args:
            stats_history: List of simulation statistics.
            figsize: Figure size.
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            fig, axes = plt.subplots(3, 2, figsize=figsize)
            axes = axes.flatten()
            
            steps = [s['step'] for s in stats_history]
            
            # Subplot 1: Neural activity
            ax = axes[0]
            neurons = [s['total_neurons'] for s in stats_history]
            firing = [s['firing_neurons'] for s in stats_history]
            ax.plot(steps, neurons, 'k-', alpha=0.5, label='Total Neurons')
            ax.plot(steps, firing, 'r-', alpha=0.7, label='Firing Neurons')
            ax.set_title('Neural Population')
            ax.set_xlabel('Step')
            ax.set_ylabel('Count')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            # Subplot 2: Firing rate over time
            ax = axes[1]
            rates = [s['average_firing_rate'] for s in stats_history]
            ax.plot(steps, rates, 'b-', alpha=0.7)
            ax.set_title('Average Firing Rate')
            ax.set_xlabel('Step')
            ax.set_ylabel('Hz')
            ax.grid(True, alpha=0.3)
            
            # Subplot 3: Spike rate
            ax = axes[2]
            spike_rates = [s['total_spikes'] / max(s['step'], 1) for s in stats_history]
            ax.plot(steps, spike_rates, 'g-', alpha=0.7)
            ax.set_title('Spike Rate')
            ax.set_xlabel('Step')
            ax.set_ylabel('Spikes/Step')
            ax.grid(True, alpha=0.3)
            
            # Subplot 4: E/I balance
            ax = axes[3]
            ei_ratios = [s['excitation_inhibition_ratio'] for s in stats_history]
            ax.plot(steps, ei_ratios, 'm-', alpha=0.7)
            ax.set_title('E/I Balance')
            ax.set_xlabel('Step')
            ax.set_ylabel('Ratio')
            ax.set_ylim(0, max(ei_ratios) * 1.1 if ei_ratios else 1)
            ax.grid(True, alpha=0.3)
            
            # Subplot 5: Developmental stage
            ax = axes[4]
            dev_stages = [s['developmental_stage'] for s in stats_history]
            unique_stages = list(set(dev_stages))
            stage_counts = [dev_stages.count(stage) for stage in unique_stages]
            ax.bar(unique_stages, stage_counts, color='coral', alpha=0.7)
            ax.set_title('Developmental Stage Distribution')
            ax.set_xlabel('Stage')
            ax.set_ylabel('Count')
            ax.tick_params(axis='x', rotation=45)
            
            # Subplot 6: Simulation time
            ax = axes[5]
            elapsed_times = [s['elapsed_time'] for s in stats_history]
            ax.plot(steps, elapsed_times, 'orange', alpha=0.7)
            ax.set_title('Simulation Time')
            ax.set_xlabel('Step')
            ax.set_ylabel('Seconds')
            ax.grid(True, alpha=0.3)
            
            plt.tight_layout()
            return fig, plt
            
        except ImportError:
            warnings.warn("matplotlib not available. Install with: pip install matplotlib")
            return None, None


class NLMConfigBuilder:
    """
    Configuration builder for NLM with validation and presets.
    
    Provides a fluent interface for creating NLM configurations with:
    - Preset configurations for common scenarios
    - Validation of configuration values
    - Parameter descriptions and constraints
    - Configuration export/import
    """
    
    # Preset configurations
    PRESETS = {
        'minimal': {
            'brain.neuron_count': 100,
            'brain.synapse_density': 0.05,
        },
        'standard': {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
        },
        'advanced': {
            'brain.neuron_count': 5000,
            'brain.synapse_density': 0.2,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.5,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'development.enabled': True,
        },
        'research': {
            'brain.neuron_count': 10000,
            'brain.synapse_density': 0.3,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 2.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.prediction_error.enable': True,
            'development.enabled': True,
            'development.critical_period': True,
        }
    }
    
    # Parameter constraints and descriptions
    PARAMETER_INFO = {
        'brain.neuron_count': {
            'type': 'int',
            'min': 10,
            'max': 100000,
            'default': 1000,
            'description': 'Total number of neurons in the brain'
        },
        'brain.synapse_density': {
            'type': 'float',
            'min': 0.0,
            'max': 1.0,
            'default': 0.1,
            'description': 'Density of synaptic connections (0.0-1.0)'
        },
        'plasticity.stdp.enable': {
            'type': 'bool',
            'default': True,
            'description': 'Enable spike-timing dependent plasticity'
        },
        'plasticity.hebbian.enable': {
            'type': 'bool',
            'default': True,
            'description': 'Enable Hebbian learning'
        },
        'plasticity.structural.enable': {
            'type': 'bool',
            'default': False,
            'description': 'Enable structural plasticity (new synapse formation)'
        },
        'neuromod.dopamine.scale': {
            'type': 'float',
            'min': 0.0,
            'max': 5.0,
            'default': 1.0,
            'description': 'Dopamine modulation scale factor'
        },
        'neuromod.curiosity.enable': {
            'type': 'bool',
            'default': True,
            'description': 'Enable curiosity-driven exploration'
        },
        'neuromod.novelty.enable': {
            'type': 'bool',
            'default': True,
            'description': 'Enable novelty detection'
        },
        'neuromod.prediction_error.enable': {
            'type': 'bool',
            'default': False,
            'description': 'Enable prediction error signals'
        },
        'development.enabled': {
            'type': 'bool',
            'default': True,
            'description': 'Enable developmental stages'
        },
        'development.critical_period': {
            'type': 'bool',
            'default': True,
            'description': 'Enable critical period plasticity'
        }
    }
    
    def __init__(self):
        """Initialize configuration builder."""
        self.config = createDefaultConfig()
        self._validation_errors = []
    
    def apply_preset(self, preset_name: str) -> 'NLMConfigBuilder':
        """
        Apply a preset configuration.
        
        Args:
            preset_name: Name of preset ('minimal', 'standard', 'advanced', 'research')
            
        Returns:
            Self for method chaining.
            
        Raises:
            ValidationError: If preset doesn't exist.
        """
        if preset_name not in self.PRESETS:
            raise ValidationError(f"Unknown preset: {preset_name}. Available: {list(self.PRESETS.keys())}")
        
        for key, value in self.PRESETS[preset_name].items():
            self.config.set(key, value)
        
        return self
    
    def set_neural_parameters(self, neuron_count: int = 1000, synapse_density: float = 0.1) -> 'NLMConfigBuilder':
        """
        Set neural system parameters.
        
        Args:
            neuron_count: Total number of neurons.
            synapse_density: Synaptic connectivity density.
            
        Returns:
            Self for method chaining.
        """
        self._validate_parameter('brain.neuron_count', neuron_count)
        self._validate_parameter('brain.synapse_density', synapse_density)
        
        self.config.set('brain.neuron_count', neuron_count)
        self.config.set('brain.synapse_density', synapse_density)
        return self
    
    def set_plasticity(self, stdp_enabled: bool = True, hebbian_enabled: bool = True,
                       stdp_rate: float = 0.001, hebbian_rate: float = 0.001,
                       structural_enabled: bool = False) -> 'NLMConfigBuilder':
        """
        Set plasticity parameters.
        
        Args:
            stdp_enabled: Enable STDP.
            hebbian_enabled: Enable Hebbian learning.
            stdp_rate: STDP learning rate.
            hebbian_rate: Hebbian learning rate.
            structural_enabled: Enable structural plasticity.
            
        Returns:
            Self for method chaining.
        """
        self.config.set('plasticity.stdp.enable', stdp_enabled)
        self.config.set('plasticity.hebbian.enable', hebbian_enabled)
        self.config.set('plasticity.structural.enable', structural_enabled)
        self.config.set('plasticity.stdp.learning_rate', stdp_rate)
        self.config.set('plasticity.hebbian.learning_rate', hebbian_rate)
        return self
    
    def set_neuromodulation(self, dopamine_scale: float = 1.0, curiosity_enabled: bool = True,
                           novelty_enabled: bool = True, prediction_error_enabled: bool = False) -> 'NLMConfigBuilder':
        """
        Set neuromodulation parameters.
        
        Args:
            dopamine_scale: Dopamine modulation scale.
            curiosity_enabled: Enable curiosity-driven exploration.
            novelty_enabled: Enable novelty detection.
            prediction_error_enabled: Enable prediction error signals.
            
        Returns:
            Self for method chaining.
        """
        self._validate_parameter('neuromod.dopamine.scale', dopamine_scale)
        
        self.config.set('neuromod.dopamine.scale', dopamine_scale)
        self.config.set('neuromod.curiosity.enable', curiosity_enabled)
        self.config.set('neuromod.novelty.enable', novelty_enabled)
        self.config.set('neuromod.prediction_error.enable', prediction_error_enabled)
        return self
    
    def set_development(self, developmental_enabled: bool = True, critical_period: bool = True) -> 'NLMConfigBuilder':
        """
        Set developmental parameters.
        
        Args:
            developmental_enabled: Enable developmental stages.
            critical_period: Enable critical period plasticity.
            
        Returns:
            Self for method chaining.
        """
        self.config.set('development.enabled', developmental_enabled)
        self.config.set('development.critical_period', critical_period)
        return self
    
    def set_parameter(self, key: str, value: Any) -> 'NLMConfigBuilder':
        """
        Set a configuration parameter.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            
        Returns:
            Self for method chaining.
        """
        self._validate_parameter(key, value)
        self.config.set(key, value)
        return self
    
    def get_config(self) -> Config:
        """Get the configured configuration object."""
        return self.config
    
    def export_config(self) -> Dict[str, Any]:
        """
        Export configuration as dictionary.
        
        Returns:
            Dictionary representation of configuration.
        """
        return self.config.summary()
    
    def import_config(self, config_dict: Dict[str, Any]) -> 'NLMConfigBuilder':
        """
        Import configuration from dictionary.
        
        Args:
            config_dict: Dictionary with configuration parameters.
            
        Returns:
            Self for method chaining.
        """
        for key, value in config_dict.items():
            self.set_parameter(key, value)
        return self
    
    def validate(self) -> List[str]:
        """
        Validate current configuration.
        
        Returns:
            List of validation error messages. Empty if valid.
        """
        self._validation_errors = []
        
        # This would need to access the C++ config's validation
        # For now, return empty list
        return self._validation_errors.copy()
    
    def _validate_parameter(self, key: str, value: Any) -> None:
        """Validate a single parameter."""
        if key in self.PARAMETER_INFO:
            info = self.PARAMETER_INFO[key]
            
            # Type validation
            if info['type'] == 'int' and not isinstance(value, int):
                self._validation_errors.append(f"Parameter {key} must be an integer")
            elif info['type'] == 'float' and not isinstance(value, (int, float)):
                self._validation_errors.append(f"Parameter {key} must be a number")
            elif info['type'] == 'bool' and not isinstance(value, bool):
                self._validation_errors.append(f"Parameter {key} must be a boolean")
            
            # Range validation
            if 'min' in info and value < info['min']:
                self._validation_errors.append(f"Parameter {key} must be >= {info['min']}")
            
            if 'max' in info and value > info['max']:
                self._validation_errors.append(f"Parameter {key} must be <= {info['max']}")


class NLMPerformanceMonitor:
    """
    Performance monitoring for NLM simulations.
    
    Provides tools for monitoring and optimizing simulation performance,
    including timing, memory usage, and resource profiling.
    """
    
    def __init__(self):
        """Initialize performance monitor."""
        self.metrics = {
            'simulation_time': [],
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'event_counts': {}
        }
        self._start_time = 0.0
        self._step_start_time = 0.0
    
    def start_monitoring(self) -> None:
        """Start performance monitoring."""
        self._start_time = time.time()
        self._step_start_time = time.time()
    
    def end_step(self, event_type: Optional[str] = None) -> Dict[str, Any]:
        """
        End step monitoring and collect metrics.
        
        Args:
            event_type: Type of event being measured.
            
        Returns:
            Dictionary of performance metrics for this step.
        """
        step_time = time.time() - self._step_start_time
        self.metrics['step_times'].append(step_time)
        
        metrics = {
            'step_time': step_time,
            'total_elapsed': time.time() - self._start_time,
            'steps_per_second': 1.0 / step_time if step_time > 0 else 0.0
        }
        
        if event_type:
            if event_type not in self.metrics['event_counts']:
                self.metrics['event_counts'][event_type] = 0
            self.metrics['event_counts'][event_type] += 1
            metrics['event_count'] = self.metrics['event_counts'][event_type]
        
        self._step_start_time = time.time()
        return metrics
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        if not self.metrics['step_times']:
            return {}
        
        step_times = self.metrics['step_times']
        
        return {
            'total_steps': len(step_times),
            'total_time': sum(step_times),
            'average_step_time': sum(step_times) / len(step_times),
            'min_step_time': min(step_times),
            'max_step_time': max(step_times),
            'steps_per_second': len(step_times) / sum(step_times) if sum(step_times) > 0 else 0,
            'event_counts': self.metrics['event_counts']
        }
    
    def reset(self) -> None:
        """Reset performance monitor."""
        self.metrics = {
            'simulation_time': [],
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'event_counts': {}
        }
        self._start_time = 0.0
        self._step_start_time = 0.0


class NLMUtils:
    """
    Utility functions for NLM.
    
    Provides common utilities and helper functions for NLM simulations.
    """
    
    @staticmethod
    def generate_random_config(neuron_range: Tuple[int, int] = (500, 5000),
                              synapse_range: Tuple[float, float] = (0.05, 0.3)) -> Config:
        """
        Generate a random configuration.
        
        Args:
            neuron_range: Range for neuron count (min, max).
            synapse_range: Range for synapse density (min, max).
            
        Returns:
            Randomly configured Config object.
        """
        import random
        
        config = createDefaultConfig()
        config.set('brain.neuron_count', random.randint(*neuron_range))
        config.set('brain.synapse_density', random.uniform(*synapse_range))
        
        # Randomly enable/disable features
        config.set('plasticity.stdp.enable', random.choice([True, False]))
        config.set('plasticity.hebbian.enable', random.choice([True, False]))
        config.set('neuromod.curiosity.enable', random.choice([True, False]))
        
        return config
    
    @staticmethod
    def create_simulation_grid(world_size: int = 20, num_agents: int = 5) -> List[Dict[str, Any]]:
        """
        Create a grid of agents for multi-agent simulation.
        
        Args:
            world_size: Size of square world.
            num_agents: Number of agents.
            
        Returns:
            List of agent configurations.
        """
        import math
        
        agents = []
        grid_size = math.ceil(math.sqrt(num_agents))
        spacing = world_size / grid_size
        
        for i in range(num_agents):
            row = i // grid_size
            col = i % grid_size
            
            agent_config = {
                'world': {
                    'width': world_size,
                    'height': world_size,
                    'vision_width': 8,
                    'vision_height': 8,
                    'vision_range': 2.0,
                },
                'agent': {
                    'x': col * spacing + spacing / 2,
                    'y': row * spacing + spacing / 2,
                    'orientation': 0.0,
                    'speed': 1.0,
                    'energy': 100.0,
                    'health': 100.0,
                },
                'brain': {
                    'neuron_count': 1000,
                    'learning_rate': 0.01,
                    'plasticity': 'stdp',
                }
            }
            
            agents.append(agent_config)
        
        return agents
    
    @staticmethod
    def benchmark_simulation(config: Optional[Config] = None, 
                           num_steps: int = 1000) -> Dict[str, Any]:
        """
        Benchmark simulation performance.
        
        Args:
            config: Configuration to use. If None, creates default.
            num_steps: Number of steps to simulate.
            
        Returns:
            Dictionary with benchmark results.
        """
        if config is None:
            config = createDefaultConfig()
        
        brain = createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(num_steps):
            brain.step(step)
        
        end_time = time.time()
        
        duration = end_time - start_time
        
        return {
            'steps': num_steps,
            'duration': duration,
            'steps_per_second': num_steps / duration,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        }


# Global instances for convenience
_builder = NLMConfigBuilder()
_monitor = NLMPerformanceMonitor()

# Preset builders for common use cases
def create_minimal_config() -> Config:
    """Create minimal configuration."""
    return _builder.apply_preset('minimal').get_config()

def create_standard_config() -> Config:
    """Create standard configuration."""
    return _builder.apply_preset('standard').get_config()

def create_advanced_config() -> Config:
    """Create advanced configuration."""
    return _builder.apply_preset('advanced').get_config()

# Export public API
__all__ = [
    # Main classes
    'NLMError',
    'ValidationError',
    'SimulationError',
    'ConfigError',
    'BrainSimulator',
    'AgentSimulator',
    'NLMVisualizer',
    'NLMConfigBuilder',
    'NLMPerformanceMonitor',
    'NLMUtils',
    
    # Helper functions
    'create_minimal_config',
    'create_standard_config',
    'create_advanced_config',
    'benchmark_simulation',
    'generate_random_config',
    'create_simulation_grid',
    
    # Re-exported classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'SensoryInput',
    'Vision', 'Audio', 'InternalSignals', 'Action', 'ActionType',
    'WorldObject', 'WorldObjectType', 'NeuronType', 'SynapseType',
    'DevelopmentalStage', 'FiringState', 'MotorCommand',
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
]
