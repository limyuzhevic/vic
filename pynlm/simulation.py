#!/usr/bin/env python3
"""
NLM Simulation Manager

Advanced Python interface for NLM simulations with comprehensive monitoring,
control, and automation capabilities.
"""

import time
import threading
import logging
from typing import Dict, List, Optional, Callable, Any, Union, Iterator
from dataclasses import dataclass, field, asdict
from enum import Enum
from contextlib import contextmanager
import numpy as np
from pathlib import Path

from pynlm import _TypeValidator, get_config, set_config, NLMConfig

logger = logging.getLogger(__name__)


class SimulationStatus(Enum):
    """Simulation status enumeration."""
    INITIALIZED = "initialized"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    ERROR = "error"
    COMPLETED = "completed"


class EventType(Enum):
    """Event types for simulation monitoring."""
    STEP_COMPLETED = "step_completed"
    BRAIN_STATE_CHANGED = "brain_state_changed"
    MEMORY_UPDATED = "memory_updated"
    PLASTICITY_EVENT = "plasticity_event"
    DEVELOPMENT_EVENT = "development_event"
    NEUROMODULATION_EVENT = "neuromodulation_event"
    ERROR_OCCURRED = "error_occurred"
    SIMULATION_STARTED = "simulation_started"
    SIMULATION_STOPPED = "simulation_stopped"


@dataclass
class SimulationEvent:
    """Event occurring during simulation."""
    
    event_type: EventType
    timestamp: float
    step: int
    data: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event to dictionary."""
        return {
            "event_type": self.event_type.value,
            "timestamp": self.timestamp,
            "step": self.step,
            "data": self.data
        }


@dataclass
class SimulationMetrics:
    """Metrics collected during simulation."""
    
    steps_completed: int = 0
    total_spikes: int = 0
    firing_neurons: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 1.0
    learning_rate: float = 0.0
    memory_usage: float = 0.0
    energy_consumed: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    developmental_stage: str = "Unknown"
    simulation_time: float = 0.0
    
    def update(self, brain: Any) -> None:
        """
        Update metrics from brain state.
        
        Args:
            brain: NLM brain instance
        """
        self.steps_completed += 1
        self.total_spikes = brain.getTotalSpikeCount()
        self.firing_neurons = brain.getFiringNeuronCount()
        self.average_firing_rate = brain.getAverageFiringRate()
        self.excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
        self.learning_rate = brain.getSTDP().getLearningRate() if hasattr(brain, 'getSTDP') else 0.0
        self.curiosity_level = brain.getCuriosity().getLevel() if hasattr(brain, 'getCuriosity') else 0.0
        self.novelty_level = brain.getNovelty().getLevel() if hasattr(brain, 'getNovelty') else 0.0
        self.prediction_error = brain.getPredictionError().getError() if hasattr(brain, 'getPredictionError') else 0.0
        self.developmental_stage = brain.getDevelopmentalStage() if hasattr(brain, 'getDevelopmentalStage') else "Unknown"
        self.simulation_time = self.steps_completed * brain.getConfig().simulation_timestep
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return asdict(self)


class EventManager:
    """Event manager for simulation events."""
    
    def __init__(self):
        self._listeners: Dict[EventType, List[Callable]] = {}
        self._events: List[SimulationEvent] = []
        self._lock = threading.Lock()
    
    def add_listener(self, event_type: EventType, callback: Callable) -> None:
        """
        Add event listener.
        
        Args:
            event_type: Event type to listen for
            callback: Callback function
        """
        if event_type not in self._listeners:
            self._listeners[event_type] = []
        self._listeners[event_type].append(callback)
    
    def remove_listener(self, event_type: EventType, callback: Callable) -> None:
        """
        Remove event listener.
        
        Args:
            event_type: Event type
            callback: Callback function
        """
        if event_type in self._listeners:
            try:
                self._listeners[event_type].remove(callback)
            except ValueError:
                pass
    
    def emit(self, event_type: EventType, step: int, data: Dict[str, Any] = None) -> None:
        """
        Emit event.
        
        Args:
            event_type: Event type
            step: Current simulation step
            data: Event data
        """
        with self._lock:
            if data is None:
                data = {}
            
            event = SimulationEvent(
                event_type=event_type,
                timestamp=time.time(),
                step=step,
                data=data
            )
            
            self._events.append(event)
            
            # Call listeners
            if event_type in self._listeners:
                for callback in self._listeners[event_type]:
                    try:
                        callback(event)
                    except Exception as e:
                        logger.error(f"Event listener error: {e}")
    
    def get_events(self, event_type: Optional[EventType] = None, since_step: Optional[int] = None) -> List[SimulationEvent]:
        """
        Get events.
        
        Args:
            event_type: Filter by event type
            since_step: Filter by step number
            
        Returns:
            List of events
        """
        with self._lock:
            events = self._events
            
            if event_type is not None:
                events = [e for e in events if e.event_type == event_type]
            
            if since_step is not None:
                events = [e for e in events if e.step >= since_step]
            
            return events
    
    def clear(self) -> None:
        """Clear all events."""
        with self._lock:
            self._events.clear()


class MetricsCollector:
    """Metrics collector for simulation data."""
    
    def __init__(self, brain: Any):
        self.brain = brain
        self.metrics_history: List[SimulationMetrics] = []
        self._lock = threading.Lock()
    
    def collect_metrics(self) -> SimulationMetrics:
        """
        Collect current metrics.
        
        Returns:
            SimulationMetrics instance
        """
        metrics = SimulationMetrics()
        metrics.update(self.brain)
        
        with self._lock:
            self.metrics_history.append(metrics)
        
        return metrics
    
    def get_metrics(self, step: Optional[int] = None) -> Optional[SimulationMetrics]:
        """
        Get metrics at specific step.
        
        Args:
            step: Step number (None for latest)
            
        Returns:
            SimulationMetrics or None
        """
        with self._lock:
            if step is None:
                return self.metrics_history[-1] if self.metrics_history else None
            
            for metrics in self.metrics_history:
                if metrics.steps_completed == step:
                    return metrics
            
            return None
    
    def get_metrics_range(self, start_step: int, end_step: int) -> List[SimulationMetrics]:
        """
        Get metrics for range of steps.
        
        Args:
            start_step: Start step
            end_step: End step
            
        Returns:
            List of metrics
        """
        with self._lock:
            return [m for m in self.metrics_history 
                   if start_step <= m.steps_completed <= end_step]
    
    def get_statistics(self, metric_name: str) -> Dict[str, float]:
        """
        Get statistics for a metric.
        
        Args:
            metric_name: Name of metric
            
        Returns:
            Dictionary with statistics
        """
        with self._lock:
            values = []
            for metrics in self.metrics_history:
                value = getattr(metrics, metric_name, 0.0)
                values.append(value)
            
            if not values:
                return {}
            
            return {
                "mean": np.mean(values),
                "std": np.std(values),
                "min": np.min(values),
                "max": np.max(values),
                "median": np.median(values),
                "percentile_25": np.percentile(values, 25),
                "percentile_75": np.percentile(values, 75)
            }


class Simulation:
    """Advanced NLM simulation controller."""
    
    def __init__(self, brain: Any, config: Optional[Union[Dict, NLMConfig]] = None):
        """
        Initialize simulation.
        
        Args:
            brain: NLM brain instance
            config: Simulation configuration
        """
        self.brain = brain
        self.status = SimulationStatus.INITIALIZED
        self.event_manager = EventManager()
        self.metrics_collector = MetricsCollector(brain)
        
        # Simulation parameters
        self.max_steps = 1000
        self.current_step = 0
        self.timestep = 0.001
        self.target_fps = 60.0  # Target frames per second
        
        # Control flags
        self.paused = False
        self.auto_reset = False
        self.save_checkpoints = False
        
        # Monitoring
        self.monitoring_enabled = True
        self.progress_callback: Optional[Callable] = None
        
        # Setup event listeners
        self._setup_event_listeners()
        
        # Set simulation parameters from config if provided
        if config:
            if isinstance(config, dict):
                config_obj = NLMConfig(**config)
            else:
                config_obj = config
            
            self.max_steps = config_obj.neuron_count // 10  # Estimate
            self.timestep = config_obj.simulation_timestep
        
        logger.info("Simulation initialized")
    
    def _setup_event_listeners(self) -> None:
        """Setup default event listeners."""
        self.event_manager.add_listener(
            EventType.STEP_COMPLETED,
            self._on_step_completed
        )
        
        self.event_manager.add_listener(
            EventType.ERROR_OCCURRED,
            self._on_error_occurred
        )
    
    def _on_step_completed(self, event: SimulationEvent) -> None:
        """Handle step completed event."""
        # Collect metrics
        self.metrics_collector.collect_metrics()
        
        # Call progress callback
        if self.progress_callback:
            progress = event.step / self.max_steps * 100
            self.progress_callback(event.step, self.max_steps, progress)
    
    def _on_error_occurred(self, event: SimulationEvent) -> None:
        """Handle error event."""
        logger.error(f"Simulation error at step {event.step}: {event.data}")
        self.status = SimulationStatus.ERROR
    
    def run(self, num_steps: int, callback: Optional[Callable] = None) -> None:
        """
        Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of steps to run
            callback: Optional per-step callback
        """
        if self.status == SimulationStatus.ERROR:
            raise RuntimeError("Cannot run simulation in error state")
        
        self.status = SimulationStatus.RUNNING
        self.event_manager.emit(
            EventType.SIMULATION_STARTED,
            self.current_step,
            {"num_steps": num_steps}
        )
        
        start_time = time.time()
        
        try:
            for step in range(num_steps):
                if self.paused:
                    time.sleep(0.1)
                    continue
                
                # Execute simulation step
                self.brain.step(step)
                
                # Emit step completed event
                self.event_manager.emit(
                    EventType.STEP_COMPLETED,
                    step,
                    {
                        "spike_count": self.brain.getTotalSpikeCount(),
                        "firing_neurons": self.brain.getFiringNeuronCount()
                    }
                )
                
                # Call per-step callback
                if callback:
                    callback(step, self.brain)
                
                self.current_step = step
                
                # Auto-reset if enabled
                if self.auto_reset and step > 0 and step % self.max_steps == 0:
                    self.reset()
        
        except Exception as e:
            self.event_manager.emit(
                EventType.ERROR_OCCURRED,
                self.current_step,
                {"error": str(e), "step": self.current_step}
            )
            raise
        
        finally:
            elapsed_time = time.time() - start_time
            self.status = SimulationStatus.COMPLETED
            self.event_manager.emit(
                EventType.SIMULATION_STOPPED,
                self.current_step,
                {
                    "total_steps": num_steps,
                    "elapsed_time": elapsed_time,
                    "steps_per_second": num_steps / elapsed_time if elapsed_time > 0 else 0
                }
            )
            
            logger.info(f"Simulation completed in {elapsed_time:.2f}s ({num_steps/elapsed_time:.1f} steps/s)")
    
    def step(self) -> bool:
        """
        Execute one simulation step.
        
        Returns:
            True if step executed, False if simulation stopped
        """
        if self.status != SimulationStatus.RUNNING:
            return False
        
        if self.paused:
            return True
        
        # Execute simulation step
        self.brain.step(self.current_step)
        
        # Emit step completed event
        self.event_manager.emit(
            EventType.STEP_COMPLETED,
            self.current_step,
            {
                "spike_count": self.brain.getTotalSpikeCount(),
                "firing_neurons": self.brain.getFiringNeuronCount()
            }
        )
        
        self.metrics_collector.collect_metrics()
        self.current_step += 1
        
        return True
    
    def reset(self) -> None:
        """Reset simulation state."""
        self.brain.reset()
        self.current_step = 0
        self.metrics_collector.metrics_history.clear()
        self.event_manager.clear()
        
        if self.status == SimulationStatus.RUNNING:
            self.status = SimulationStatus.INITIALIZED
        
        logger.info("Simulation reset")
    
    def pause(self) -> None:
        """Pause simulation."""
        self.paused = True
        self.status = SimulationStatus.PAUSED
        logger.info("Simulation paused")
    
    def resume(self) -> None:
        """Resume simulation."""
        self.paused = False
        if self.status == SimulationStatus.PAUSED:
            self.status = SimulationStatus.RUNNING
        logger.info("Simulation resumed")
    
    def stop(self) -> None:
        """Stop simulation."""
        self.status = SimulationStatus.STOPPED
        logger.info("Simulation stopped")
    
    def add_event_listener(self, event_type: EventType, callback: Callable) -> None:
        """
        Add event listener.
        
        Args:
            event_type: Event type
            callback: Callback function
        """
        self.event_manager.add_listener(event_type, callback)
    
    def get_events(self, event_type: Optional[EventType] = None, since_step: Optional[int] = None) -> List[SimulationEvent]:
        """
        Get simulation events.
        
        Args:
            event_type: Filter by event type
            since_step: Filter by step number
            
        Returns:
            List of events
        """
        return self.event_manager.get_events(event_type, since_step)
    
    def get_metrics(self, step: Optional[int] = None) -> Optional[SimulationMetrics]:
        """
        Get simulation metrics.
        
        Args:
            step: Step number (None for latest)
            
        Returns:
            SimulationMetrics or None
        """
        return self.metrics_collector.get_metrics(step)
    
    def get_metrics_statistics(self, metric_name: str) -> Dict[str, float]:
        """
        Get statistics for a metric.
        
        Args:
            metric_name: Name of metric
            
        Returns:
            Dictionary with statistics
        """
        return self.metrics_collector.get_statistics(metric_name)
    
    def get_progress(self) -> Dict[str, Any]:
        """
        Get simulation progress.
        
        Returns:
            Dictionary with progress information
        """
        return {
            "status": self.status.value,
            "current_step": self.current_step,
            "max_steps": self.max_steps,
            "progress": self.current_step / self.max_steps * 100 if self.max_steps > 0 else 0,
            "paused": self.paused,
            "elapsed_time": self.current_step * self.timestep
        }
    
    def export_events(self, filepath: str) -> None:
        """
        Export events to file.
        
        Args:
            filepath: Output file path
        """
        events = self.event_manager.get_events()
        event_data = [event.to_dict() for event in events]
        
        with open(filepath, 'w') as f:
            json.dump(event_data, f, indent=2)
        
        logger.info(f"Events exported to {filepath}")
    
    def import_events(self, filepath: str) -> None:
        """
        Import events from file.
        
        Args:
            filepath: Input file path
        """
        with open(filepath, 'r') as f:
            event_data = json.load(f)
        
        for event_dict in event_data:
            event = SimulationEvent(
                event_type=EventType(event_dict["event_type"]),
                timestamp=event_dict["timestamp"],
                step=event_dict["step"],
                data=event_dict.get("data", {})
            )
            
            self.event_manager._events.append(event)
        
        logger.info(f"Events imported from {filepath}")
    
    def export_metrics(self, filepath: str) -> None:
        """
        Export metrics to file.
        
        Args:
            filepath: Output file path
        """
        metrics = self.metrics_collector.metrics_history
        metrics_data = [m.to_dict() for m in metrics]
        
        with open(filepath, 'w') as f:
            json.dump(metrics_data, f, indent=2)
        
        logger.info(f"Metrics exported to {filepath}")
    
    def import_metrics(self, filepath: str) -> None:
        """
        Import metrics from file.
        
        Args:
            filepath: Input file path
        """
        with open(filepath, 'r') as f:
            metrics_data = json.load(f)
        
        self.metrics_collector.metrics_history.clear()
        
        for metric_dict in metrics_data:
            # Create SimulationMetrics from dict
            metric = SimulationMetrics(**metric_dict)
            self.metrics_collector.metrics_history.append(metric)
        
        logger.info(f"Metrics imported from {filepath}")


@contextmanager
def simulation_context(brain: Any, **kwargs) -> Iterator[Simulation]:
    """
    Context manager for simulation.
    
    Args:
        brain: NLM brain instance
        **kwargs: Simulation configuration options
        
    Yields:
        Simulation instance
    """
    simulation = Simulation(brain, **kwargs)
    
    try:
        yield simulation
    finally:
        simulation.stop()