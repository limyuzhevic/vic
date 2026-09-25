"""
Enhanced Brain simulation with context managers, type hints, and error handling.

This module provides a Python wrapper around the C++ Brain implementation
with modern Python features like context managers, type hints, and
comprehensive error handling.
"""

from typing import Dict, Any, Optional, List, Union, Iterator, Callable
from pathlib import Path
import time
import warnings
from contextlib import contextmanager
from dataclasses import dataclass
from enum import Enum

from .._core import Brain as CBrain
from .._core import NeuronId, RegionId, NeuronType, DevelopmentalStage
from .exceptions import (
    NLMError,
    NLMInitializationError,
    NLMInvalidStateError,
    NLMSimulationError,
    NLMResourceError,
)


class BrainState(Enum):
    """Brain states for state management."""
    
    UNINITIALIZED = "uninitialized"
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    SAVING = "saving"
    LOADING = "loading"


@dataclass
class BrainStatistics:
    """Brain statistics dataclass with type hints."""
    
    total_neurons: int = 0
    total_synapses: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 1.0
    total_spikes: int = 0
    pending_spike_events: int = 0
    developmental_stage: str = "Initial"
    neuromodulation_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    energy: float = 100.0
    age: float = 0.0


@dataclass
class SimulationStepInfo:
    """Information about a simulation step."""
    
    step_number: int = 0
    simulation_time: float = 0.0
    neurons_firing: int = 0
    spikes_this_step: int = 0
    action_taken: Optional[str] = None
    reward_received: float = 0.0
    novelty_detected: float = 0.0
    development_progress: float = 0.0


class BrainSimulation:
    """Enhanced brain simulation with context management and statistics."""
    
    def __init__(
        self,
        brain: CBrain,
        timestep: float = 0.1,
        stats_window: int = 100,
    ):
        """Initialize brain simulation wrapper.
        
        Args:
            brain: Underlying C++ Brain instance
            timestep: Simulation time step duration
            stats_window: Window size for statistics calculation
            
        Raises:
            NLMInitializationError: If brain cannot be initialized
        """
        if not brain:
            raise NLMInitializationError("Brain cannot be None")
        
        self._brain = brain
        self._timestep = timestep
        self._stats_window = max(1, stats_window)
        
        # Internal state
        self._current_step = 0
        self._simulation_time = 0.0
        self._state = BrainState.UNINITIALIZED
        self._step_history: List[SimulationStepInfo] = []
        
        # Performance tracking
        self._step_times: List[float] = []
        self._spike_counts: List[int] = []
        
        try:
            self._initialize()
        except Exception as e:
            if not isinstance(e, (NLMError, NLMInitializationError)):
                raise NLMInitializationError(f"Failed to initialize brain: {e}")
            raise
    
    def _initialize(self) -> None:
        """Internal initialization."""
        self._brain.initialize()
        self._state = BrainState.RUNNING
        self._current_step = 0
        self._simulation_time = 0.0
        self._step_history.clear()
        self._step_times.clear()
        self._spike_counts.clear()
    
    def step(self, step_number: Optional[int] = None) -> SimulationStepInfo:
        """Execute one simulation step.
        
        Args:
            step_number: Optional step number (defaults to internal counter)
            
        Returns:
            Information about the executed step
            
        Raises:
            NLMInvalidStateError: If brain is not in a valid state
            NLMSimulationError: If step execution fails
        """
        if self._state not in (BrainState.RUNNING, BrainState.PAUSED):
            raise NLMInvalidStateError(
                f"Cannot execute step in state {self._state}. "
                f"Expected RUNNING or PAUSED."
            )
        
        step_num = step_number if step_number is not None else self._current_step
        
        try:
            start_time = time.time()
            
            # Execute the step
            self._brain.step(step_num, self._simulation_time)
            
            # Update simulation time
            self._simulation_time += self._timestep
            
            # Collect statistics
            step_info = self._collect_step_info(step_num)
            
            # Update history
            self._step_history.append(step_info)
            
            # Maintain windows
            self._maintain_windows(step_info)
            
            # Update internal state
            self._current_step = step_num + 1
            
            # Record performance
            self._step_times.append(time.time() - start_time)
            
            return step_info
            
        except Exception as e:
            if not isinstance(e, (NLMError, NLMSimulationError)):
                raise NLMSimulationError(f"Simulation step failed: {e}")
            raise
    
    def _collect_step_info(self, step_number: int) -> SimulationStepInfo:
        """Collect information about a completed step."""
        # Get brain statistics
        stats = self.get_statistics()
        
        # Get action if available
        action = None
        try:
            action_result = self._brain.produceAction()
            if action_result:
                action = action_result.getName() if hasattr(action_result, 'getName') else str(action_result)
        except Exception:
            pass  # Ignore action collection errors
        
        # Get reward (if available)
        reward = 0.0
        try:
            if hasattr(self._brain, 'getPredictionSystem'):
                # Try to get prediction error as proxy for reward
                pass
        except Exception:
            pass
        
        # Get novelty
        novelty = 0.0
        try:
            novelty = stats.novelty_level
        except Exception:
            pass
        
        # Get development progress
        development = 0.0
        try:
            if hasattr(self._brain, 'getDevelopmentalStage'):
                stage = self._brain.getDevelopmentalStage()
                if stage:
                    stage_value = stage.value if hasattr(stage, 'value') else stage
                    development = float(hash(stage_value) % 100) / 100.0
        except Exception:
            pass
        
        return SimulationStepInfo(
            step_number=step_number,
            simulation_time=self._simulation_time,
            neurons_firing=stats.firing_neurons,
            spikes_this_step=stats.firing_neurons,  # Approximate
            action_taken=action,
            reward_received=reward,
            novelty_detected=novelty,
            development_progress=development,
        )
    
    def _maintain_windows(self, step_info: SimulationStepInfo) -> None:
        """Maintain sliding windows of statistics."""
        # Keep only recent history
        if len(self._step_history) > self._stats_window * 2:
            self._step_history = self._step_history[-self._stats_window:]
        
        # Keep recent step times
        if len(self._step_times) > self._stats_window:
            self._step_times = self._step_times[-self._stats_window:]
        
        # Calculate rolling spike counts
        recent_steps = self._step_history[-self._stats_window:]
        self._spike_counts = [step.spikes_this_step for step in recent_steps]
    
    @property
    def current_step(self) -> int:
        """Get current simulation step."""
        return self._current_step
    
    @property
    def simulation_time(self) -> float:
        """Get current simulation time."""
        return self._simulation_time
    
    @property
    def state(self) -> BrainState:
        """Get current brain state."""
        return self._state
    
    @property
    def step_history(self) -> List[SimulationStepInfo]:
        """Get simulation step history."""
        return self._step_history.copy()
    
    @property
    def average_step_time(self) -> float:
        """Get average step execution time."""
        if not self._step_times:
            return 0.0
        return sum(self._step_times) / len(self._step_times)
    
    # Brain property accessors
    
    def get_total_neurons(self) -> int:
        """Get total number of neurons."""
        try:
            return self._brain.getTotalNeuronCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get neuron count: {e}")
    
    def get_total_synapses(self) -> int:
        """Get total number of synapses."""
        try:
            return self._brain.getTotalSynapseCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get synapse count: {e}")
    
    def get_active_neurons(self) -> int:
        """Get number of active neurons."""
        try:
            return self._brain.getActiveNeuronCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get active neuron count: {e}")
    
    def get_firing_neurons(self) -> int:
        """Get number of firing neurons."""
        try:
            return self._brain.getFiringNeuronCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get firing neuron count: {e}")
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        try:
            return self._brain.getAverageFiringRate()
        except Exception as e:
            raise NLMResourceError(f"Failed to get firing rate: {e}")
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        try:
            return self._brain.getExcitationInhibitionRatio()
        except Exception as e:
            raise NLMResourceError(f"Failed to get E/I ratio: {e}")
    
    def get_total_spikes(self) -> int:
        """Get total spike count."""
        try:
            return self._brain.getTotalSpikeCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get spike count: {e}")
    
    def get_pending_spike_events(self) -> int:
        """Get number of pending spike events."""
        try:
            return self._brain.getPendingSpikeEventCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get spike event count: {e}")
    
    def get_developmental_stage(self) -> str:
        """Get current developmental stage."""
        try:
            stage = self._brain.getDevelopmentalStage()
            return stage.value if hasattr(stage, 'value') else str(stage)
        except Exception as e:
            raise NLMResourceError(f"Failed to get developmental stage: {e}")
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        try:
            # Get from various neuromodulators
            neuromod = 0.0
            try:
                neuromod += self._brain.getDopamine()
            except Exception:
                pass
            
            try:
                neuromod += self._brain.getCuriosity()
            except Exception:
                pass
            
            try:
                neuromod += self._brain.getNovelty()
            except Exception:
                pass
            
            return neuromod
        except Exception as e:
            raise NLMResourceError(f"Failed to get neuromodulation level: {e}")
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        try:
            return self._brain.getCuriosity()
        except Exception as e:
            raise NLMResourceError(f"Failed to get curiosity level: {e}")
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        try:
            return self._brain.getNovelty()
        except Exception as e:
            raise NLMResourceError(f"Failed to get novelty level: {e}")
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        try:
            return self._brain.getPredictionError()
        except Exception as e:
            raise NLMResourceError(f"Failed to get prediction error: {e}")
    
    def get_energy(self) -> float:
        """Get current energy level."""
        # This might not be directly available, estimate from brain state
        return min(100.0, (self.get_firing_neurons() / max(1, self.get_total_neurons())) * 100.0)
    
    def get_age(self) -> float:
        """Get brain age."""
        return self._current_step * self._timestep
    
    def get_region_count(self) -> int:
        """Get number of neural regions."""
        try:
            return self._brain.getRegionCount()
        except Exception as e:
            raise NLMResourceError(f"Failed to get region count: {e}")
    
    # Configuration methods
    
    def reset(self) -> None:
        """Reset brain to initial state."""
        try:
            self._brain.reset()
            self._state = BrainState.RUNNING
            self._current_step = 0
            self._simulation_time = 0.0
            self._step_history.clear()
            self._step_times.clear()
            self._spike_counts.clear()
        except Exception as e:
            raise NLMInvalidStateError(f"Failed to reset brain: {e}")
    
    def pause(self) -> None:
        """Pause simulation."""
        if self._state == BrainState.RUNNING:
            self._state = BrainState.PAUSED
        else:
            raise NLMInvalidStateError(f"Cannot pause brain in state {self._state}")
    
    def resume(self) -> None:
        """Resume paused simulation."""
        if self._state == BrainState.PAUSED:
            self._state = BrainState.RUNNING
        else:
            raise NLMInvalidStateError(f"Cannot resume brain in state {self._state}")
    
    def save(self, filepath: Union[str, Path]) -> None:
        """Save brain state to file.
        
        Args:
            filepath: Path to save brain state
            
        Raises:
            NLMInvalidStateError: If save operation fails
        """
        if self._state not in (BrainState.RUNNING, BrainState.PAUSED):
            raise NLMInvalidStateError(f"Cannot save brain in state {self._state}")
        
        try:
            self._brain.save(str(filepath))
        except Exception as e:
            raise NLMResourceError(f"Failed to save brain state: {e}")
    
    def load(self, filepath: Union[str, Path]) -> None:
        """Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
            
        Raises:
            NLMInvalidStateError: If load operation fails
        """
        if not Path(filepath).exists():
            raise NLMResourceError(f"Brain state file not found: {filepath}")
        
        try:
            self._brain.load(str(filepath))
            self._state = BrainState.RUNNING
        except Exception as e:
            raise NLMResourceError(f"Failed to load brain state: {e}")
    
    def get_statistics(self) -> BrainStatistics:
        """Get current brain statistics."""
        return BrainStatistics(
            total_neurons=self.get_total_neurons(),
            total_synapses=self.get_total_synapses(),
            active_neurons=self.get_active_neurons(),
            firing_neurons=self.get_firing_neurons(),
            average_firing_rate=self.get_average_firing_rate(),
            excitation_inhibition_ratio=self.get_excitation_inhibition_ratio(),
            total_spikes=self.get_total_spikes(),
            pending_spike_events=self.get_pending_spike_events(),
            developmental_stage=self.get_developmental_stage(),
            neuromodulation_level=self.get_neuromodulation_level(),
            curiosity_level=self.get_curiosity_level(),
            novelty_level=self.get_novelty_level(),
            prediction_error=self.get_prediction_error(),
            energy=self.get_energy(),
            age=self.get_age(),
        )

    def run_steps(self, num_steps: int) -> List[SimulationStepInfo]:
        """Run multiple simulation steps.
        
        Args:
            num_steps: Number of steps to run
            
        Returns:
            List of step information
            
        Raises:
            NLMSimulationError: If simulation fails
        """
        if num_steps < 0:
            raise NLMInvalidStateError("Number of steps must be non-negative")
        
        results = []
        for i in range(num_steps):
            try:
                step_info = self.step(i)
                results.append(step_info)
            except Exception as e:
                raise NLMSimulationError(f"Simulation failed at step {i}: {e}")
        
        return results

    def analyze_patterns(self) -> Dict[str, Any]:
        """Analyze activity patterns in the brain.
        
        Returns:
            Dictionary with pattern analysis results
        """
        if not self._step_history:
            return {"error": "No step history available"}
        
        # Calculate various statistics
        firing_rates = [step.neurons_firing for step in self._step_history]
        spike_counts = [step.spikes_this_step for step in self._step_history]
        
        return {
            "total_steps": len(self._step_history),
            "avg_firing_rate": sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            "max_firing_rate": max(firing_rates) if firing_rates else 0,
            "min_firing_rate": min(firing_rates) if firing_rates else 0,
            "std_firing_rate": (sum((x - sum(firing_rates) / len(firing_rates)) ** 2 
                                   for x in firing_rates) / len(firing_rates)) ** 0.5 if firing_rates else 0,
            "total_spikes": sum(spike_counts),
            "avg_spikes_per_step": sum(spike_counts) / len(spike_counts) if spike_counts else 0,
            "activity_variance": self._calculate_variance(firing_rates) if firing_rates else 0,
            "trend": self._calculate_trend(firing_rates) if len(firing_rates) >= 2 else "stable",
        }
    
    def _calculate_variance(self, values: List[float]) -> float:
        """Calculate variance of values."""
        if len(values) <= 1:
            return 0.0
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
    
    def _calculate_trend(self, values: List[float]) -> str:
        """Calculate trend (increasing, decreasing, stable)."""
        if len(values) < 2:
            return "stable"
        
        # Simple trend calculation based on first and last third
        n = len(values)
        first_third = sum(values[:n//3]) / (n//3)
        last_third = sum(values[-n//3:]) / (n//3)
        
        if last_third > first_third * 1.1:
            return "increasing"
        elif last_third < first_third * 0.9:
            return "decreasing"
        else:
            return "stable"

    def __repr__(self) -> str:
        """Get string representation."""
        stats = self.get_statistics()
        return f"BrainSimulation(state={self._state.value}, " \
               f"step={self._current_step}, " \
               f"neurons={stats.total_neurons}, " \
               f"firing={stats.firing_neurons})"
