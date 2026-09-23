#!/usr/bin/env python3
"""
Brain simulation for NLM Python bindings.

This module provides a Python interface to the NLM brain simulation system,
enabling users to create and interact with artificial neural substrates
that learn through experience and development.
"""

import time
import threading
from typing import List, Optional, Dict, Any, Tuple
from enum import Enum

# Import underlying C++ bindings
try:
    import pynlm
    has_pynlm = True
except ImportError:
    has_pynlm = False

class Brain:
    """
    NLM (熙然) Neural Learning Machine Brain.
    
    This class represents the central neural processing unit of the NLM system,
    implementing Phase 6 integration with all major neural systems working
    together in a coordinated loop.
    
    The brain implements:
    - Real LIF neuron dynamics
    - Event-driven spike propagation with delays
    - Memory systems (working, episodic, associative)
    - Neuromodulation (dopamine, curiosity, novelty)
    - Prediction systems
    - Cognitive mechanisms (planning, concept formation, attention)
    - Development system
    - Plasticity rules (STDP, Hebbian, structural)
    - Checkpoint save/load functionality
    
    Examples:
        >>> # Create a brain with configuration
        >>> config = Config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("random_seed", 42)
        >>> brain = Brain(config)
        >>> 
        >>> # Initialize the brain
        >>> brain.initialize()
        >>> 
        >>> # Run a simulation
        >>> for step in range(100):
        ...     brain.step(step)
    """
    
    def __init__(self, config = None):
        """
        Create a new NLM brain.
        
        Args:
            config: Configuration object. If None, a default configuration
                   will be created with Phase 6 integration parameters.
        """
        if config is None:
            config = Config()
        
        if has_pynlm:
            # Create the underlying C++ brain object
            self._c_brain = pynlm.Brain(config._c_config)
            self._config = config
            self._initialized = False
            
            # Simulation state
            self._current_step = 0
            self._current_time = 0.0
            self._step_lock = threading.Lock()
        else:
            raise ImportError(
                "pynlm module not available. "
                "Please ensure NLM Python bindings are properly installed."
            )
    
    def initialize(self) -> bool:
        """
        Initialize the brain with current configuration.
        
        This sets up all neural systems and prepares the brain for simulation.
        Initialization includes:
        - Creating neurons and synapses
        - Setting up memory systems
        - Initializing neuromodulation
        - Configuring plasticity rules
        
        Returns:
            True if initialization succeeded, False otherwise
        """
        if not has_pynlm:
            return False
            
        if self._initialized:
            return True
        
        try:
            # Initialize the C++ brain
            if not self._c_brain.initialize():
                raise RuntimeError("C++ brain initialization failed")
            
            self._initialized = True
            
            # Log initialization success
            self._c_brain.logStatus()
            
            return True
            
        except Exception as e:
            print(f"Brain initialization failed: {e}")
            self._initialized = False
            return False
    
    def step(self, current_step: int, current_time: Optional[float] = None) -> Dict[str, Any]:
        """
        Advance the brain by one simulation step.
        
        This is the main simulation loop that processes all neural systems:
        1. Processes pending spike events
        2. Updates neuron membrane potentials (LIF dynamics)
        3. Detects spikes and schedules outgoing spikes
        4. Updates working memory
        5. Applies neuromodulation effects
        6. Applies plasticity rules (STDP, Hebbian)
        7. Updates episodic memory
        8. Updates prediction system
        9. Updates attention system
        10. Applies structural plasticity
        11. Replays important memories
        12. Applies development effects
        
        Args:
            current_step: Current simulation step number
            current_time: Current simulation time in seconds.
                         If None, calculated as current_step * timestep
            
        Returns:
            Dictionary with simulation statistics for this step
        """
        if not has_pynlm:
            raise RuntimeError("pynlm module not available")
            
        if not self._initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
        
        with self._step_lock:
            # Set current step and time
            self._current_step = current_step
            if current_time is None:
                self._current_time = current_step * self._get_timestep()
            else:
                self._current_time = current_time
            
            # Store previous state for statistics
            prev_spikes = self.get_total_spike_count()
            prev_active = self.get_active_neuron_count()
            
            # Call C++ brain step function
            if len(self._c_brain.step.__self__.__class__.__name__.split('::')[-1]) > 10:
                # Two-argument version
                self._c_brain.step(current_step, current_time)
            else:
                # Single-argument version
                self._c_brain.step(current_step)
            
            # Calculate step statistics
            stats = self._calculate_step_statistics(prev_spikes, prev_active)
            
            return stats
    
    def _calculate_step_statistics(self, prev_spikes: int, prev_active: int) -> Dict[str, Any]:
        """Calculate statistics for the current simulation step."""
        current_spikes = self.get_total_spike_count()
        current_active = self.get_active_neuron_count()
        current_firing = self.get_firing_neuron_count()
        
        return {
            "step": self._current_step,
            "time": self._current_time,
            "spikes_generated": current_spikes - prev_spikes,
            "total_spikes": current_spikes,
            "active_neurons": current_active - prev_active,
            "total_active": current_active,
            "firing_neurons": current_firing,
            "average_firing_rate": self.get_average_firing_rate(),
            "excitatory_inhibition_ratio": self.get_excitatory_inhibition_ratio(),
        }
    
    def inject_current(self, neuron_id: int, current: float) -> None:
        """
        Inject current directly into a specific neuron.
        
        Args:
            neuron_id: ID of the neuron to receive current
            current: Current amount to inject (in nA)
        """
        if has_pynlm:
            self._c_brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type, current: float) -> None:
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to target
            current: Current amount to inject (in nA)
        """
        if has_pynlm:
            self._c_brain.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self):
        """
        Produce a motor action based on current neural activity.
        
        Returns:
            Action object representing the motor output
        """
        if has_pynlm:
            return self._c_brain.produceAction()
        return None
    
    def reset(self) -> None:
        """Reset the brain to its initial state."""
        if has_pynlm:
            self._c_brain.reset()
        
        self._current_step = 0
        self._current_time = 0.0
        self._initialized = True
    
    def save(self, filepath: str) -> bool:
        """
        Save the brain's current state to a checkpoint file.
        
        Args:
            filepath: Path where to save the checkpoint
            
        Returns:
            True if saving succeeded, False otherwise
        """
        if has_pynlm:
            return self._c_brain.save(filepath)
        return False
    
    def load(self, filepath: str) -> bool:
        """
        Load a brain state from a checkpoint file.
        
        Args:
            filepath: Path to the checkpoint file
            
        Returns:
            True if loading succeeded, False otherwise
        """
        if has_pynlm:
            success = self._c_brain.load(filepath)
            if success:
                self._initialized = True
            return success
        return False
    
    def get_config(self):
        """Get the brain's configuration."""
        return self._config
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons in the brain."""
        if has_pynlm:
            return self._c_brain.getTotalNeuronCount()
        return 0
    
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses in the brain."""
        if has_pynlm:
            return self._c_brain.getTotalSynapseCount()
        return 0
    
    def get_active_neuron_count(self) -> int:
        """Get current number of active neurons."""
        if has_pynlm:
            return self._c_brain.getActiveNeuronCount()
        return 0
    
    def get_firing_neuron_count(self) -> int:
        """Get current number of firing neurons (this step)."""
        if has_pynlm:
            return self._c_brain.getFiringNeuronCount()
        return 0
    
    def get_total_spike_count(self) -> int:
        """Get total number of spikes generated by the brain."""
        if has_pynlm:
            return self._c_brain.getTotalSpikeCount()
        return 0
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        if has_pynlm:
            return self._c_brain.getAverageFiringRate()
        return 0.0
    
    def get_excitatory_inhibition_ratio(self) -> float:
        """Get the excitation/inhibition balance ratio."""
        if has_pynlm:
            return self._c_brain.getExcitationInhibitionRatio()
        return 0.0
    
    def get_pending_spike_event_count(self) -> int:
        """Get number of pending spike events."""
        if has_pynlm:
            return self._c_brain.getPendingSpikeEventCount()
        return 0
    
    def get_developmental_stage(self) -> str:
        """Get current developmental stage."""
        if has_pynlm:
            stage = self._c_brain.getDevelopmentalStage()
            return str(stage)
        return "Unknown"
    
    def set_developmental_stage(self, stage: str) -> None:
        """Set developmental stage."""
        if has_pynlm:
            # Map string to enum
            stage_map = {
                "initial": pynlm.DevelopmentalStage.Initial,
                "critical_period": pynlm.DevelopmentalStage.CriticalPeriod,
                "maturation": pynlm.DevelopmentalStage.Maturation,
                "adult": pynlm.DevelopmentalStage.Adult,
                "aging": pynlm.DevelopmentalStage.Aging,
            }
            
            if stage.lower() in stage_map:
                self._c_brain.setDevelopmentalStage(stage_map[stage.lower()])
    
    def get_spike_system(self):
        """Get the spike system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getSpikeSystem()
        return None
    
    def get_stdp(self):
        """Get the STDP plasticity system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getSTDP()
        return None
    
    def get_hebbian(self):
        """Get the Hebbian plasticity system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getHebbian()
        return None
    
    def get_structural_plasticity(self):
        """Get the structural plasticity system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getStructuralPlasticity()
        return None
    
    def get_working_memory(self):
        """Get the working memory system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getWorkingMemory()
        return None
    
    def get_episodic_memory(self):
        """Get the episodic memory system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getEpisodicMemory()
        return None
    
    def get_associative_memory(self):
        """Get the associative memory system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getAssociativeMemory()
        return None
    
    def get_prediction_system(self):
        """Get the prediction system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getPredictionSystem()
        return None
    
    def get_planner(self):
        """Get the neural planner (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getPlanner()
        return None
    
    def get_concept_formation(self):
        """Get the concept formation system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getConceptFormation()
        return None
    
    def get_attention(self):
        """Get the attention system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getAttention()
        return None
    
    def get_development_system(self):
        """Get the development system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getDevelopmentSystem()
        return None
    
    def get_dopamine(self):
        """Get the dopamine neuromodulation system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getDopamine()
        return None
    
    def get_curiosity(self):
        """Get the curiosity neuromodulation system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getCuriosity()
        return None
    
    def get_novelty(self):
        """Get the novelty detection system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getNovelty()
        return None
    
    def get_prediction_error(self):
        """Get the prediction error system (for advanced usage)."""
        if has_pynlm:
            return self._c_brain.getPredictionErrorSignal()
        return None
    
    def log_status(self) -> None:
        """Log detailed status information about the brain."""
        if has_pynlm:
            self._c_brain.logStatus()
    
    def _get_timestep(self) -> float:
        """Get the simulation timestep from configuration."""
        return self._config.get_float("simulation_timestep", 0.001)
    
    def __repr__(self) -> str:
        """String representation of the brain."""
        status = "Initialized" if self._initialized else "Not initialized"
        return f"<Brain: {status}, {self.get_total_neuron_count()} neurons, {self.get_total_synapse_count()} synapses>"
    
    def __str__(self) -> str:
        """Detailed string representation of the brain."""
        if not self._initialized:
            return "Brain not initialized"
        
        return (f"NLM Brain (Phase 6 - Complete Integration)\n"
                f"  Neurons: {self.get_total_neuron_count()}\n"
                f"  Synapses: {self.get_total_synapse_count()}\n"
                f"  Total Spikes: {self.get_total_spike_count()}\n"
                f"  Active Neurons: {self.get_active_neuron_count()}\n"
                f"  Average Firing Rate: {self.get_average_firing_rate():.3f} Hz\n"
                f"  Developmental Stage: {self.get_developmental_stage()}\n"
                f"  E/I Ratio: {self.get_excitatory_inhibition_ratio():.3f}")
class _Internal:
    """Internal class to avoid circular imports."""
    pass