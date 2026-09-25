"""
Comprehensive metrics collector for NLM (Neural Learning Machine) experiments.

This module provides enhanced metrics collection for NLM experiments,
powering advanced artificial brain capabilities through comprehensive
experiment management and analysis.

Key features:
- Comprehensive metrics collection for all NLM systems
- Real-time metrics monitoring and analysis
- Metrics history tracking and analysis
- Metrics visualization and reporting
- Integration with experiment framework
- Custom metrics support
- Metrics validation and quality control

The Metrics Collector is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
experiment management and analysis system.

Integration with:
- Experiment Framework (experiment management)
- Brain (neural coordination metrics)
- AgentBrain (agent performance metrics)
- World (environmental interaction metrics)
- Memory systems (memory system metrics)
- Prediction systems (prediction accuracy metrics)
- Cognitive systems (cognitive ability metrics)
- Neuromodulation systems (neuromodulation effect metrics)
- Development systems (development progress metrics)
- Plasticity systems (learning rate metrics)
- Visualization (metrics visualization)

See src/experiments/README.md for detailed documentation.
"""

from typing import Dict, Any, Optional, List, Union, Callable
from dataclasses import dataclass, field
from datetime import datetime, timedelta
import time
import json
import logging
from pathlib import Path
import threading
from collections import defaultdict, deque
from enum import Enum

from .brain import Brain
from .agent_brain import AgentBrain
from .world import SimpleWorld


class MetricType(Enum):
    """Metric type enumeration."""
    
    PERFORMANCE = "performance"
    COGNITIVE = "cognitive"
    MEMORY = "memory"
    PREDICTION = "prediction"
    NEUROMODULATION = "neuromodulation"
    DEVELOPMENT = "development"
    PLASTICITY = "plasticity"
    SOCIAL = "social"
    ENVIRONMENTAL = "environmental"
    CUSTOM = "custom"


@dataclass
class Metric:
    """Metric dataclass."""
    
    # Basic information
    name: str = ""
    description: str = ""
    type: MetricType = MetricType.CUSTOM
    unit: str = ""
    
    # Value
    value: float = 0.0
    
    # History
    history: List[float] = field(default_factory=list)
    max_history: int = 1000
    
    # Statistics
    min: float = float('inf')
    max: float = float('-inf')
    mean: float = 0.0
    std: float = 0.0
    
    # Metadata
    timestamp: float = 0.0
    step: int = 0
    
    # Quality
    valid: bool = True
    quality_score: float = 1.0
    
    # Custom
    custom_data: Dict[str, Any] = field(default_factory=dict)


@dataclass
class MetricStatistics:
    """Metric statistics dataclass."""
    
    # Basic statistics
    count: int = 0
    min: float = float('inf')
    max: float = float('-inf')
    mean: float = 0.0
    std: float = 0.0
    median: float = 0.0
    variance: float = 0.0
    
    # Percentile statistics
    percentile_25: float = 0.0
    percentile_50: float = float('inf')
    percentile_75: float = 0.0
    percentile_90: float = 0.0
    percentile_95: float = 0.0
    
    # Advanced statistics
    skewness: float = 0.0
    kurtosis: float = 0.0
    
    # Quality
    quality_score: float = 1.0
    confidence: float = 1.0


class MetricsCollector:
    """Comprehensive metrics collector for NLM experiments."""
    
    def __init__(self):
        """Initialize metrics collector."""
        self.metrics: Dict[str, Metric] = {}
        self.metrics_by_type: Dict[MetricType, List[Metric]] = defaultdict(list)
        self.metrics_by_name: Dict[str, List[Metric]] = defaultdict(list)
        
        # Configuration
        self.max_history = 1000
        self.auto_update_stats = True
        self.enable_validation = True
        self.validation_threshold = 0.5
        
        # State
        self.is_running = False
        self.current_step = 0
        self.start_time = 0.0
        self.last_update_time = 0.0
        
        # Threading
        self.update_thread: Optional[threading.Thread] = None
        self.stop_event = threading.Event()
        
        # Event handling
        self.event_handlers: Dict[str, List[Callable]] = defaultdict(list)
        
        # Logging
        self.logger = logging.getLogger(__name__)
    
    def start(self):
        """Start metrics collection."""
        if self.is_running:
            return
        
        self.is_running = True
        self.start_time = time.time()
        
        # Start update thread
        self.update_thread = threading.Thread(target=self._update_loop)
        self.update_thread.daemon = True
        self.update_thread.start()
        
        self.logger.info("Metrics collector started")
    
    def stop(self):
        """Stop metrics collection."""
        if not self.is_running:
            return
        
        self.is_running = False
        self.stop_event.set()
        
        if self.update_thread:
            self.update_thread.join(timeout=1.0)
        
        # Update final statistics
        self._update_statistics()
        
        self.logger.info("Metrics collector stopped")
    
    def is_running(self) -> bool:
        """Check if metrics collector is running."""
        return self.is_running
    
    def add_metric(self, name: str, description: str = "", 
                  metric_type: MetricType = MetricType.CUSTOM,
                  unit: str = "", default_value: float = 0.0) -> bool:
        """
        Add metric.
        
        Args:
            name: Metric name
            description: Metric description
            metric_type: Metric type
            unit: Metric unit
            default_value: Default metric value
            
        Returns:
            True if metric added successfully, False otherwise
        """
        try:
            if name in self.metrics:
                self.logger.warning(f"Metric '{name}' already exists")
                return False
            
            metric = Metric(
                name=name,
                description=description,
                type=metric_type,
                unit=unit,
                value=default_value,
                timestamp=time.time(),
                step=self.current_step
            )
            
            self.metrics[name] = metric
            self.metrics_by_type[metric_type].append(metric)
            self.metrics_by_name[name].append(metric)
            
            self.logger.debug(f"Metric '{name}' added")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to add metric '{name}': {e}")
            return False
    
    def remove_metric(self, name: str) -> bool:
        """
        Remove metric.
        
        Args:
            name: Metric name
            
        Returns:
            True if metric removed successfully, False otherwise
        """
        try:
            if name not in self.metrics:
                self.logger.warning(f"Metric '{name}' not found")
                return False
            
            metric = self.metrics[name]
            
            # Remove from all indices
            self.metrics_by_type[metric.type].remove(metric)
            self.metrics_by_name[name].remove(metric)
            
            del self.metrics[name]
            
            self.logger.debug(f"Metric '{name}' removed")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to remove metric '{name}': {e}")
            return False
    
    def set_metric_value(self, name: str, value: float, step: Optional[int] = None) -> bool:
        """
        Set metric value.
        
        Args:
            name: Metric name
            value: Metric value
            step: Step number (optional)
            
        Returns:
            True if metric value set successfully, False otherwise
        """
        try:
            if name not in self.metrics:
                self.logger.warning(f"Metric '{name}' not found")
                return False
            
            metric = self.metrics[name]
            
            # Store previous value
            old_value = metric.value
            
            # Update metric
            metric.value = value
            metric.timestamp = time.time()
            if step is not None:
                metric.step = step
            else:
                metric.step = self.current_step
            
            # Add to history
            metric.history.append(value)
            
            # Trim history
            if len(metric.history) > metric.max_history:
                metric.history.pop(0)
            
            # Update statistics
            if self.auto_update_stats:
                self._update_metric_statistics(metric)
            
            # Emit event
            self.emit_event("metric_updated", {
                "name": name,
                "value": value,
                "old_value": old_value,
                "step": metric.step,
                "timestamp": metric.timestamp
            })
            
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to set metric value '{name}': {e}")
            return False
    
    def get_metric_value(self, name: str) -> Optional[float]:
        """
        Get metric value.
        
        Args:
            name: Metric name
            
        Returns:
            Metric value if found, None otherwise
        """
        if name not in self.metrics:
            return None
        
        return self.metrics[name].value
    
    def get_metric(self, name: str) -> Optional[Metric]:
        """
        Get metric.
        
        Args:
            name: Metric name
            
        Returns:
            Metric if found, None otherwise
        """
        return self.metrics.get(name)
    
    def get_all_metrics(self) -> Dict[str, Metric]:
        """
        Get all metrics.
        
        Returns:
            Dictionary of all metrics
        """
        return self.metrics.copy()
    
    def get_metrics_by_type(self, metric_type: MetricType) -> List[Metric]:
        """
        Get metrics by type.
        
        Args:
            metric_type: Metric type
            
        Returns:
            List of metrics
        """
        return self.metrics_by_type[metric_type].copy()
    
    def get_current_metrics(self) -> Dict[str, float]:
        """
        Get current metrics.
        
        Returns:
            Dictionary of current metrics
        """
        return {name: metric.value for name, metric in self.metrics.items()}
    
    def collect(self, step: int, agents: List[AgentBrain], world: SimpleWorld, 
                brain: Brain, config: Any) -> bool:
        """
        Collect metrics.
        
        Args:
            step: Current step
            agents: List of agents
            world: World object
            brain: Brain object
            config: Experiment configuration
            
        Returns:
            True if metrics collected successfully, False otherwise
        """
        try:
            self.current_step = step
            
            # Collect agent metrics
            self._collect_agent_metrics(agents, config)
            
            # Collect brain metrics
            self._collect_brain_metrics(brain, config)
            
            # Collect world metrics
            self._collect_world_metrics(world, config)
            
            # Collect system metrics
            self._collect_system_metrics(config)
            
            # Update statistics
            if self.auto_update_stats:
                self._update_statistics()
            
            # Emit event
            self.emit_event("metrics_collected", {
                "step": step,
                "timestamp": time.time(),
                "num_metrics": len(self.metrics)
            })
            
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to collect metrics: {e}")
            return False
    
    def _collect_agent_metrics(self, agents: List[AgentBrain], config: Any):
        """
        Collect agent metrics.
        
        Args:
            agents: List of agents
            config: Experiment configuration
        """
        for i, agent in enumerate(agents):
            # Agent statistics
            agent_name = f"agent_{i}"
            
            # Basic metrics
            self.set_metric_value(f"{agent_name}_firing_rate", 
                                agent.get_average_firing_rate())
            self.set_metric_value(f"{agent_name}_total_spikes", 
                                agent.get_total_spike_count())
            self.set_metric_value(f"{agent_name}_neuron_count", 
                                agent.get_total_neuron_count())
            
            # Novelty and curiosity
            self.set_metric_value(f"{agent_name}_novelty_level", 
                                agent.get_novelty_level())
            self.set_metric_value(f"{agent_name}_curiosity_level", 
                                agent.get_curiosity_level())
            
            # Reward and prediction
            self.set_metric_value(f"{agent_name}_dopamine_level", 
                                agent.get_dopamine_level())
            self.set_metric_value(f"{agent_name}_prediction_error", 
                                agent.get_prediction_error())
            
            # Development
            self.set_metric_value(f"{agent_name}_development_stage", 
                                agent.get_developmental_stage())
            self.set_metric_value(f"{agent_name}_plasticity_modifier", 
                                agent.get_plasticity_modifier())
    
    def _collect_brain_metrics(self, brain: Brain, config: Any):
        """
        Collect brain metrics.
        
        Args:
            brain: Brain object
            config: Experiment configuration
        """
        # Brain statistics
        self.set_metric_value("brain_total_neurons", brain.get_total_neuron_count())
        self.set_metric_value("brain_total_synapses", brain.get_total_synapse_count())
        self.set_metric_value("brain_firing_neurons", brain.get_firing_neuron_count())
        self.set_metric_value("brain_average_firing_rate", brain.get_average_firing_rate())
        self.set_metric_value("brain_excitation_inhibition_ratio", 
                            brain.get_excitation_inhibition_ratio())
        self.set_metric_value("brain_total_spikes", brain.get_total_spike_count())
        self.set_metric_value("brain_pending_spike_events", brain.get_pending_spike_event_count())
        
        # Development
        self.set_metric_value("brain_development_stage", brain.get_developmental_stage())
        
        # Memory systems
        if brain.get_working_memory():
            self.set_metric_value("brain_working_memory_active_neurons", 
                                brain.get_working_memory().get_active_neuron_count())
        
        if brain.get_episodic_memory():
            self.set_metric_value("brain_episodic_memory_episodes", 
                                brain.get_episodic_memory().get_episode_count())
        
        # Prediction system
        if brain.get_prediction_system():
            self.set_metric_value("brain_prediction_error", 
                                brain.get_prediction_system().get_prediction_error())
            self.set_metric_value("brain_prediction_confidence", 
                                brain.get_prediction_system().get_confidence())
    
    def _collect_world_metrics(self, world: SimpleWorld, config: Any):
        """
        Collect world metrics.
        
        Args:
            world: World object
            config: Experiment configuration
        """
        # World statistics
        self.set_metric_value("world_time", world.get_simulation_time())
        self.set_metric_value("world_width", world.get_width())
        self.set_metric_value("world_height", world.get_height())
        self.set_metric_value("world_vision_width", world.get_vision_width())
        self.set_metric_value("world_vision_height", world.get_vision_height())
        
        # Environment statistics
        self.set_metric_value("world_object_count", world.get_object_count())
        self.set_metric_value("world_resource_count", world.get_resource_count())
        self.set_metric_value("world_hazard_count", world.get_hazard_count())
    
    def _collect_system_metrics(self, config: Any):
        """
        Collect system metrics.
        
        Args:
            config: Experiment configuration
        """
        # System statistics
        self.set_metric_value("system_metrics_collector_runtime", 
                            time.time() - self.start_time)
        self.set_metric_value("system_metrics_collector_step", self.current_step)
        
        # Configuration metrics
        if config:
            self.set_metric_value("system_config_max_steps", config.max_steps)
            self.set_metric_value("system_config_time_step", config.time_step)
            self.set_metric_value("system_config_num_agents", config.num_agents)
    
    def _update_metric_statistics(self, metric: Metric):
        """
        Update metric statistics.
        
        Args:
            metric: Metric object
        """
        history = metric.history
        if len(history) < 2:
            return
        
        # Calculate statistics
        metric.min = min(history)
        metric.max = max(history)
        metric.mean = sum(history) / len(history)
        
        if len(history) > 1:
            variance = sum((x - metric.mean) ** 2 for x in history) / (len(history) - 1)
            metric.std = variance ** 0.5
    
    def _update_statistics(self):
        """Update all metric statistics."""
        for metric in self.metrics.values():
            self._update_metric_statistics(metric)
    
    def _update_loop(self):
        """Update loop."""
        while self.is_running:
            try:
                time.sleep(0.1)  # Update every 100ms
                self._update_statistics()
            except Exception as e:
                self.logger.error(f"Error in metrics collector update loop: {e}")
    
    def emit_event(self, event_type: str, data: Dict[str, Any]):
        """
        Emit event.
        
        Args:
            event_type: Event type
            data: Event data
        """
        for handler in self.event_handlers.get(event_type, []):
            try:
                handler(data)
            except Exception as e:
                self.logger.error(f"Error in event handler for '{event_type}': {e}")
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """
        Add event handler.
        
        Args:
            event_type: Event type
            handler: Event handler function
        """
        self.event_handlers[event_type].append(handler)
    
    def remove_event_handler(self, event_type: str, handler: Callable):
        """
        Remove event handler.
        
        Args:
            event_type: Event type
            handler: Event handler function
        """
        if event_type in self.event_handlers:
            try:
                self.event_handlers[event_type].remove(handler)
            except ValueError:
                pass
    
    def get_metric_statistics(self, name: str) -> Optional[MetricStatistics]:
        """
        Get metric statistics.
        
        Args:
            name: Metric name
            
        Returns:
            Metric statistics if found, None otherwise
        """
        metric = self.metrics.get(name)
        if not metric:
            return None
        
        history = metric.history
        if len(history) < 2:
            return None
        
        # Calculate statistics
        stats = MetricStatistics()
        
        stats.count = len(history)
        stats.min = min(history)
        stats.max = max(history)
        stats.mean = sum(history) / len(history)
        
        if len(history) > 1:
            variance = sum((x - stats.mean) ** 2 for x in history) / (len(history) - 1)
            stats.std = variance ** 0.5
            stats.variance = variance
        
        # Calculate percentiles
        sorted_history = sorted(history)
        stats.percentile_25 = sorted_history[int(len(sorted_history) * 0.25)]
        stats.percentile_50 = sorted_history[int(len(sorted_history) * 0.5)]
        stats.percentile_75 = sorted_history[int(len(sorted_history) * 0.75)]
        stats.percentile_90 = sorted_history[int(len(sorted_history) * 0.9)]
        stats.percentile_95 = sorted_history[int(len(sorted_history) * 0.95)]
        
        return stats
    
    def get_metric_history(self, name: str) -> Optional[List[float]]:
        """
        Get metric history.
        
        Args:
            name: Metric name
            
        Returns:
            Metric history if found, None otherwise
        """
        if name not in self.metrics:
            return None
        
        return self.metrics[name].history.copy()
    
    def validate_metrics(self) -> Dict[str, bool]:
        """
        Validate metrics.
        
        Returns:
            Dictionary of validation results
        """
        validation_results = {}
        
        for name, metric in self.metrics.items():
            is_valid = self._validate_metric(metric)
            validation_results[name] = is_valid
        
        return validation_results
    
    def _validate_metric(self, metric: Metric) -> bool:
        """
        Validate metric.
        
        Args:
            metric: Metric object
            
        Returns:
            True if metric is valid, False otherwise
        """
        try:
            # Check if metric has enough history
            if len(metric.history) < 1:
                return False
            
            # Check if metric value is reasonable
            if not isinstance(metric.value, (int, float)):
                return False
            
            # Check if metric statistics are reasonable
            if not isinstance(metric.min, (int, float)) or not isinstance(metric.max, (int, float)):
                return False
            
            # Check if metric quality is reasonable
            if not isinstance(metric.quality_score, (int, float)) or metric.quality_score < 0 or metric.quality_score > 1:
                return False
            
            return True
            
        except Exception as e:
            self.logger.error(f"Error validating metric '{metric.name}': {e}")
            return False
    
    def export_metrics(self, filepath: str):
        """
        Export metrics to file.
        
        Args:
            filepath: File path
        """
        try:
            # Convert metrics to serializable format
            metrics_data = {}
            for name, metric in self.metrics.items():
                metrics_data[name] = {
                    "name": metric.name,
                    "description": metric.description,
                    "type": metric.type.value,
                    "unit": metric.unit,
                    "value": metric.value,
                    "history": metric.history,
                    "min": metric.min,
                    "max": metric.max,
                    "mean": metric.mean,
                    "std": metric.std,
                    "timestamp": metric.timestamp,
                    "step": metric.step,
                    "valid": metric.valid,
                    "quality_score": metric.quality_score
                }
            
            # Write to file
            with open(filepath, 'w') as f:
                json.dump(metrics_data, f, indent=2)
            
            self.logger.info(f"Metrics exported to '{filepath}'")
            
        except Exception as e:
            self.logger.error(f"Failed to export metrics to '{filepath}': {e}")
    
    def import_metrics(self, filepath: str) -> bool:
        """
        Import metrics from file.
        
        Args:
            filepath: File path
            
        Returns:
            True if metrics imported successfully, False otherwise
        """
        try:
            # Read metrics from file
            with open(filepath, 'r') as f:
                metrics_data = json.load(f)
            
            # Clear existing metrics
            self.metrics.clear()
            self.metrics_by_type.clear()
            self.metrics_by_name.clear()
            
            # Import metrics
            for name, data in metrics_data.items():
                metric = Metric(
                    name=data.get("name", name),
                    description=data.get("description", ""),
                    type=MetricType(data.get("type", "custom")),
                    unit=data.get("unit", ""),
                    value=data.get("value", 0.0),
                    history=data.get("history", []),
                    min=data.get("min", 0.0),
                    max=data.get("max", 0.0),
                    mean=data.get("mean", 0.0),
                    std=data.get("std", 0.0),
                    timestamp=data.get("timestamp", time.time()),
                    step=data.get("step", 0),
                    valid=data.get("valid", True),
                    quality_score=data.get("quality_score", 1.0)
                )
                
                self.metrics[name] = metric
                self.metrics_by_type[metric.type].append(metric)
                self.metrics_by_name[name].append(metric)
            
            self.logger.info(f"Metrics imported from '{filepath}'")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to import metrics from '{filepath}': {e}")
            return False


# Enum definition
from enum import Enum