#!/usr/bin/env python3
"""
Brain interface for NLM Python module

This module provides high-level abstractions for working with NLM brains,
including context managers, simulation control, and common patterns.
"""

import time
import threading
from typing import Optional, List, Dict, Any, Callable
from contextlib import contextmanager

from .core import Brain as BrainCore
from .world import SimpleWorld
from .agent import AgentBrain
from .sensory import SensoryProcessor
from .motor import MotorController
from .neuromodulation import NeuromodulationSystem
from .exceptions import NLMError, SimulationError, ConfigurationError
from .utils import (
    validate_config, 
    create_named_timer,
    format_neuron_id,
    parse_synaptic_weight
)

# Re-export core types
from .core import createBrain, createDefaultConfig
from .world import WorldObject, WorldObjectType
from .sensory import Vision, Audio, InternalSignals
from .motor import Action, ActionType
from .neuromodulation import Reward, Hebbian, STDP

# Re-export neuron types from C++ bindings
class NeuronType:
    """Neuron type enumeration."""
    Excitatory = "Excitatory"
    Inhibitory = "Inhibitory"
    Modulatory = "Modulatory"
    Sensory = "Sensory"
    Motor = "Motor"
    Internal = "Internal"


class DevelopmentalStage:
    """Developmental stage enumeration."""
    Initial = "Initial"
    CriticalPeriod = "CriticalPeriod"
    Maturation = "Maturation"
    Adult = "Adult"
    Aging = "Aging"


class BrainInterface:
    """
    High-level interface for NLM brain operations with context management.
    
    This class provides a Pythonic wrapper around the core Brain class with
    enhanced functionality including:
    - Context managers for resource management
    - High-level simulation control
    - Statistics collection and analysis
    - Development monitoring
    - Error handling and recovery
    """
    
    def __init__(self, config: Optional[Any] = None):
        """
        Initialize a BrainInterface.
        
        Args:
            config: Configuration object. If None, a default config is created.
        """
        self._config = config if config is not None else createDefaultConfig()
        self._brain: Optional[BrainCore] = None
        self._agent: Optional[AgentBrain] = None
        self._world: Optional[SimpleWorld] = None
        self._neuromodulation = NeuromodulationSystem()
        self._simulation_running = False
        self._simulation_thread: Optional[threading.Thread] = None
        self._event_handlers: List[Callable] = []
    
    def __enter__(self):
        """Context manager entry."""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.shutdown()
    
    def initialize(self) -> bool:
        """
        Initialize the brain with the current configuration.
        
        Returns:
            True if initialization succeeded, False otherwise.
        """
        try:
            self._brain = createBrain(self._config)
            result = self._brain.initialize()
            if not result:
                raise InitializationError("Failed to initialize brain")
            return True
        except Exception as e:
            raise InitializationError(f"Brain initialization failed: {e}")
    
    def step(self, current_step: int, current_time: Optional[float] = None) -> Dict[str, Any]:
        """
        Perform a single simulation step.
        
        Args:
            current_step: The current simulation step number.
            current_time: The current simulation time. If None, calculated from step.
        
        Returns:
            Dictionary containing step statistics and events.
        """
        if self._brain is None:
            raise InitializationError("Brain not initialized. Call initialize() first.")
        
        events = []
        
        # Update neuromodulation
        neuromodulation_data = self._neuromodulation.update(current_step)
        
        # Record pre-step statistics
        pre_stats = {
            'neurons_active': self._brain.getActiveNeuronCount(),
            'spike_count': self._brain.getTotalSpikeCount(),
            'development_stage': self._brain.getDevelopmentalStage(),
            'neuromodulation_level': neuromodulation_data['overall_level']
        }
        
        # Perform brain step
        self._brain.step(current_step, current_time or current_step * 0.001)
        
        # Record post-step statistics
        post_stats = {
            'neurons_active': self._brain.getActiveNeuronCount(),
            'spike_count': self._brain.getTotalSpikeCount(),
            'development_stage': self._brain.getDevelopmentalStage(),
            'neuromodulation_level': neuromodulation_data['overall_level']
        }
        
        # Check for events
        step_events = self._check_step_events(pre_stats, post_stats)
        events.extend(step_events)
        
        # Notify event handlers
        self._notify_event_handlers('step_completed', {
            'step': current_step,
            'pre_stats': pre_stats,
            'post_stats': post_stats,
            'events': step_events
        })
        
        return {
            'step': current_step,
            'pre_stats': pre_stats,
            'post_stats': post_stats,
            'events': step_events,
            'neuromodulation': neuromodulation_data
        }
    
    def run_simulation(self, num_steps: int, timestep: float = 0.001,
                      callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run a complete simulation for the specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run.
            timestep: Time duration of each step.
            callback: Optional callback function called each step with step data.
        
        Returns:
            Dictionary containing simulation results and statistics.
        """
        if self._brain is None:
            raise InitializationError("Brain not initialized. Call initialize() first.")
        
        self._simulation_running = True
        results = {
            'steps': [],
            'total_time': 0.0,
            'start_time': time.time(),
            'end_time': 0.0,
            'statistics': {},
            'events': []
        }
        
        try:
            for step in range(num_steps):
                if not self._simulation_running:
                    break
                
                step_data = self.step(step, step * timestep)
                results['steps'].append(step_data)
                
                if callback:
                    callback(step_data)
                
                # Small delay to simulate real-time if needed
                if timestep > 0.001:
                    time.sleep(timestep)
            
            results['end_time'] = time.time()
            results['total_time'] = results['end_time'] - results['start_time']
            
            # Compute final statistics
            results['statistics'] = self._compute_simulation_statistics(results['steps'])
            
            return results
            
        except Exception as e:
            raise SimulationError(f"Simulation failed: {e}")
        finally:
            self._simulation_running = False
    
    def attach_world(self, world: SimpleWorld) -> 'AgentBrainInterface':
        """
        Attach a world to this brain and return an agent interface.
        
        Args:
            world: The world to attach.
        
        Returns:
            An AgentBrainInterface for interacting with the brain-world system.
        """
        self._world = world
        self._agent = AgentBrainInterface(self._brain, world)
        return self._agent
    
    def reset(self) -> bool:
        """
        Reset the brain to its initial state.
        
        Returns:
            True if reset succeeded, False otherwise.
        """
        if self._brain is None:
            return False
        
        result = self._brain.reset()
        if result:
            self._brain.initialize()
        return result
    
    def shutdown(self) -> bool:
        """
        Shutdown the brain and clean up resources.
        
        Returns:
            True if shutdown succeeded, False otherwise.
        """
        try:
            if self._brain:
                # Brain doesn't have an explicit shutdown method,
                # but we clean up our references
                pass
            
            self._brain = None
            self._agent = None
            self._world = None
            self._simulation_running = False
            
            return True
        except Exception:
            return False
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get current brain statistics.
        
        Returns:
            Dictionary containing brain statistics.
        """
        if self._brain is None:
            return {}
        
        return {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'developmental_stage': self._brain.getDevelopmentalStage(),
            'neuromodulation_level': self._neuromodulation.get_overall_level()
        }
    
    def enable_features(self, **features) -> None:
        """
        Enable various brain features.
        
        Args:
            **features: Feature flags to enable (reward_modulation, structural_plasticity,
                      development, curiosity).
        """
        if self._agent:
            if features.get('reward_modulation', False):
                self._agent.enable_reward_modulation(True)
            if features.get('structural_plasticity', False):
                self._agent.enable_structural_plasticity(True)
            if features.get('development', False):
                self._agent.enable_development(True)
            if features.get('curiosity', False):
                self._agent.enable_curiosity(True)
    
    def add_event_handler(self, event_type: str, handler: Callable) -> None:
        """
        Add an event handler.
        
        Args:
            event_type: Type of event to handle ('step_completed', 'simulation_started',
                       'simulation_completed', 'error').
            handler: Callable that handles the event.
        """
        self._event_handlers.append((event_type, handler))
    
    def _check_step_events(self, pre_stats: Dict, post_stats: Dict) -> List[Dict]:
        """Check for step events based on pre/post statistics."""
        events = []
        
        # Check for significant changes
        if post_stats['neurons_active'] > pre_stats['neurons_active'] * 1.5:
            events.append({
                'type': 'neural_activity_spike',
                'description': f"Neural activity increased by {(post_stats['neurons_active'] / pre_stats['neurons_active'] - 1) * 100:.1f}%",
                'severity': 'medium'
            })
        
        if post_stats['spike_count'] > pre_stats['spike_count'] * 1.2:
            events.append({
                'type': 'high_spiking',
                'description': f"Spike rate increased significantly",
                'severity': 'low'
            })
        
        # Check developmental stage changes
        if post_stats['development_stage'] != pre_stats['development_stage']:
            events.append({
                'type': 'developmental_stage_change',
                'description': f"Brain moved to {post_stats['development_stage']} stage",
                'severity': 'info'
            })
        
        return events
    
    def _notify_event_handlers(self, event_type: str, data: Dict) -> None:
        """Notify registered event handlers."""
        for event_filter, handler in self._event_handlers:
            if event_filter == event_type or event_filter == 'all':
                try:
                    handler(event_type, data)
                except Exception:
                    pass  # Don't let handler errors break the simulation
    
    def _compute_simulation_statistics(self, steps: List[Dict]) -> Dict[str, Any]:
        """Compute overall statistics from simulation steps."""
        if not steps:
            return {}
        
        spike_counts = [step['post_stats']['spike_count'] for step in steps]
        neuron_counts = [step['post_stats']['neurons_active'] for step in steps]
        
        return {
            'total_steps': len(steps),
            'max_spike_count': max(spike_counts),
            'min_spike_count': min(spike_counts),
            'avg_spike_count': sum(spike_counts) / len(spike_counts),
            'max_neural_activity': max(neuron_counts),
            'min_neural_activity': min(neuron_counts),
            'avg_neural_activity': sum(neuron_counts) / len(neuron_counts),
            'spike_rate_variation': (max(spike_counts) - min(spike_counts)) / max(1, spike_counts[0]),
            'activity_variation': (max(neuron_counts) - min(neuron_counts)) / max(1, neuron_counts[0])
        }


class AgentBrainInterface:
    """
    High-level interface for agent brain operations with world integration.
    
    This class provides enhanced functionality for the brain-world-agent
    integration pattern with support for context managers, simulation control,
    and common interaction patterns.
    """
    
    def __init__(self, brain: BrainCore, world: SimpleWorld):
        """
        Initialize an AgentBrainInterface.
        
        Args:
            brain: The brain to interface with.
            world: The world to interact with.
        """
        self._brain = brain
        self._world = world
        self._neuromodulation = NeuromodulationSystem()
        self._simulation_thread: Optional[threading.Thread] = None
    
    def __enter__(self):
        """Context manager entry."""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.shutdown()
    
    def initialize(self, world: Optional[SimpleWorld] = None) -> bool:
        """
        Initialize the agent with the brain and world.
        
        Args:
            world: The world to initialize with. If None, uses the world from
                   initialization.
        
        Returns:
            True if initialization succeeded, False otherwise.
        """
        if world is not None:
            self._world = world
        
        if self._world is None:
            raise InitializationError("No world provided for agent initialization")
        
        # The actual agent brain is managed by the BrainInterface
        return True
    
    def process_percept(self, percept: Any) -> Dict[str, Any]:
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: The sensory percept to process.
        
        Returns:
            Dictionary containing processing results.
        """
        # This would interface with the actual agent brain if we had it
        # For now, we provide the high-level interface
        
        # Inject percept into brain (if we have the actual agent brain)
        # This is a placeholder for the high-level abstraction
        
        return {
            'success': True,
            'neurons_active': self._brain.getActiveNeuronCount(),
            'spike_count': self._brain.getTotalSpikeCount()
        }
    
    def decode_motor_command(self) -> Dict[str, Any]:
        """
        Decode motor command from brain activity.
        
        Returns:
            Dictionary containing motor command and related data.
        """
        # This would interface with the actual agent brain
        # For now, we provide the high-level interface
        
        return {
            'action': None,  # Placeholder
            'confidence': 0.0,  # Placeholder
            'neurons_active': self._brain.getActiveNeuronCount(),
            'motor_region_active': self._brain.getActiveNeuronCount() * 0.3  # Estimate
        }
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> Dict[str, Any]:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: The actual reward received.
            predicted_reward: The predicted reward.
        
        Returns:
            Dictionary containing neuromodulation results.
        """
        # Update neuromodulation system
        neuromodulation_data = self._neuromodulation.update(0)  # Step 0 as placeholder
        
        return {
            'reward': reward,
            'predicted_reward': predicted_reward,
            'error': reward - predicted_reward,
            'neuromodulation_level': neuromodulation_data['overall_level'],
            'curiosity': neuromodulation_data['curiosity'],
            'novelty': neuromodulation_data['novelty']
        }
    
    def update_development(self, timestep: float) -> Dict[str, Any]:
        """
        Update development system.
        
        Args:
            timestep: Time duration of the development update.
        
        Returns:
            Dictionary containing development results.
        """
        # Update development (placeholder for high-level abstraction)
        current_stage = self._brain.getDevelopmentalStage()
        
        # Simple development progression logic
        stage_order = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
        try:
            current_idx = stage_order.index(current_stage)
            if current_idx < len(stage_order) - 1:
                next_stage = stage_order[current_idx + 1]
                # In a real implementation, this would update the brain
                pass
        except ValueError:
            pass
        
        return {
            'development_stage': self._brain.getDevelopmentalStage(),
            'timestep': timestep,
            'plasticity_rate': 1.0  # Placeholder
        }
    
    def shutdown(self) -> bool:
        """
        Shutdown the agent and clean up resources.
        
        Returns:
            True if shutdown succeeded, False otherwise.
        """
        self._simulation_thread = None
        return True
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get current agent status.
        
        Returns:
            Dictionary containing agent status.
        """
        neuromodulation_data = self._neuromodulation.update(0)  # Step 0 as placeholder
        
        return {
            'brain': self._brain,
            'world': self._world,
            'neuromodulation': neuromodulation_data,
            'development_stage': self._brain.getDevelopmentalStage(),
            'simulation_running': self._simulation_thread is not None and self._simulation_thread.is_alive()
        }


# Create an alias for backward compatibility
Brain = BrainInterface


@contextmanager
def brain_simulation(config: Optional[Any] = None, **kwargs):
    """
    Context manager for brain simulation.
    
    Provides a clean interface for brain simulation with automatic cleanup.
    
    Args:
        config: Configuration object for the brain.
        **kwargs: Additional arguments for simulation configuration.
    
    Yields:
        BrainInterface: A configured brain interface.
    """
    brain = BrainInterface(config)
    try:
        brain.initialize()
        yield brain
    finally:
        brain.shutdown()


@contextmanager
def brain_world_simulation(config: Optional[Any] = None, world_config: Optional[Dict] = None,
                         **kwargs):
    """
    Context manager for brain-world simulation.
    
    Provides a clean interface for brain-world-agent simulation with
    automatic cleanup and common patterns.
    
    Args:
        config: Configuration object for the brain.
        world_config: Configuration dictionary for the world.
        **kwargs: Additional arguments for simulation configuration.
    
    Yields:
        tuple: (BrainInterface, AgentBrainInterface) for brain and agent control.
    """
    brain = BrainInterface(config)
    world = SimpleWorld()
    
    if world_config:
        for key, value in world_config.items():
            if hasattr(world, f'set_{key}'):
                getattr(world, f'set_{key}')(value)
    
    try:
        brain.initialize()
        agent = brain.attach_world(world)
        yield brain, agent
    finally:
        brain.shutdown()