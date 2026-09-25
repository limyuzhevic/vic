"""
Enhanced experiment framework for NLM (Neural Learning Machine).

This module provides enhanced experiment functionality for NLM systems,
powering advanced artificial brain capabilities through comprehensive
experiment management and analysis.

Key features:
- Comprehensive experiment lifecycle management
- Experiment state tracking and persistence
- Experiment configuration management
- Enhanced metrics collection
- Experiment validation and verification
- Multi-agent experiment support
- Experiment parameter tuning
- Experiment result analysis and visualization
- Integration with Component Registry
- Integration with NLM systems (Brain, AgentBrain, World)

The Enhanced Experiment Framework is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
experiment management and analysis system.

Integration with:
- Component Registry (component management)
- Brain (neural coordination)
- AgentBrain (agent interface)
- World (environmental interaction)
- Memory systems (working, episodic, semantic, procedural)
- Prediction systems (forward models)
- Cognitive systems (planning, attention, concept formation)
- Neuromodulation systems (dopamine, serotonin, norepinephrine, acetylcholine)
- Development systems (maturation, learning)
- Performance infrastructure (memory pools, event queues)

See src/experiments/README.md for detailed documentation.
"""

from typing import Dict, Any, Optional, List, Union, Callable, Iterator
from dataclasses import dataclass, field
from datetime import datetime, timedelta
import time
import json
import logging
from abc import ABC, abstractmethod
from pathlib import Path
import threading
import queue
from collections import defaultdict, deque
from enum import Enum

from .exceptions import (
    NLMError,
    NLMInitializationError,
    NLMInvalidStateError,
    NLMSimulationError,
    NLMResourceError,
    NLMSimulationError,
)
from .component_registry import ComponentMetadata, ComponentRegistry
from .plugin import PluginManager
from .brain import Brain
from .agent_brain import AgentBrain
from .world import SimpleWorld
from .memory import NeuralWorkingMemory, NeuralEpisodicMemory
from .prediction import PredictionSystem
from .cognition import NeuralPlanner, ConceptFormation, SelfModel, SocialLearning
from .neuromodulation import Dopamine, Serotonin, Norepinephrine, Acetylcholine
from .development import DevelopmentSystem
from .plasticity import STDP, Hebbian, StructuralPlasticity
from .visualization import VisualizationInterface
from .metrics import MetricsCollector


class ExperimentState(Enum):
    """Experiment state enumeration."""
    
    INITIALIZING = "initializing"
    CONFIGURING = "configuring"
    READY = "ready"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETING = "completing"
    COMPLETED = "completed"
    CANCELLED = "cancelled"
    ERROR = "error"
    STOPPED = "stopped"


@dataclass
class ExperimentMetadata:
    """Experiment metadata class."""
    
    # Basic information
    name: str = ""
    description: str = ""
    version: str = "1.0.0"
    author: str = ""
    tags: List[str] = field(default_factory=list)
    
    # Classification
    category: str = ""
    complexity: str = "medium"
    duration: Optional[float] = None
    
    # Metrics
    metrics: List[str] = field(default_factory=list)
    performance_targets: Dict[str, float] = field(default_factory=dict)
    
    # Configuration
    default_config: Dict[str, Any] = field(default_factory=dict)
    configurable_parameters: List[str] = field(default_factory=list)
    
    # Requirements
    requires_components: List[str] = field(default_factory=list)
    required_systems: List[str] = field(default_factory=list)
    
    # Documentation
    documentation: str = ""
    examples: List[str] = field(default_factory=list)
    references: List[str] = field(default_factory=list)


@dataclass
class ExperimentResult:
    """Experiment result dataclass."""
    
    # Basic information
    experiment_name: str = ""
    experiment_version: str = "1.0.0"
    start_time: float = 0.0
    end_time: float = 0.0
    duration: float = 0.0
    
    # Status
    success: bool = False
    error_message: str = ""
    
    # Metrics
    metrics: Dict[str, float] = field(default_factory=dict)
    metrics_history: Dict[str, List[float]] = field(default_factory=lambda: defaultdict(list))
    
    # Results
    results: Dict[str, Any] = field(default_factory=dict)
    intermediate_results: Dict[str, Any] = field(default_factory=dict)
    
    # Statistics
    agent_stats: Dict[str, Dict] = field(default_factory=dict)
    brain_stats: Dict[str, Dict] = field(default_factory=dict)
    world_stats: Dict[str, Dict] = field(default_factory=dict)
    
    # Components
    components_used: List[str] = field(default_factory=list)
    systems_used: List[str] = field(default_factory=list)


@dataclass
class ExperimentConfiguration:
    """Experiment configuration dataclass."""
    
    # Basic configuration
    name: str = ""
    description: str = ""
    version: str = "1.0.0"
    
    # Execution
    max_steps: int = 1000
    time_step: float = 0.1
    num_agents: int = 1
    random_seed: Optional[int] = None
    
    # Brain configuration
    brain_config: Dict[str, Any] = field(default_factory=dict)
    brain_neurons: int = 1000
    brain_connections: float = 0.1
    
    # World configuration
    world_config: Dict[str, Any] = field(default_factory=dict)
    world_width: int = 20
    world_height: int = 20
    world_vision_width: int = 8
    world_vision_height: int = 8
    
    # Agent configuration
    agent_config: Dict[str, Any] = field(default_factory=dict)
    enable_reward_modulation: bool = True
    enable_structural_plasticity: bool = True
    enable_development: bool = True
    enable_curiosity: bool = True
    
    # Learning configuration
    learning_config: Dict[str, Any] = field(default_factory=dict)
    learning_rate: float = 0.01
    discount_factor: float = 0.99
    exploration_rate: float = 0.1
    
    # Memory configuration
    memory_config: Dict[str, Any] = field(default_factory=dict)
    working_memory_capacity: int = 100
    episodic_memory_capacity: int = 1000
    
    # Custom configuration
    custom_config: Dict[str, Any] = field(default_factory=dict)


class Experiment(ABC):
    """Base experiment class for NLM experiments."""
    
    def __init__(self, name: str, version: str = "1.0.0"):
        """
        Initialize experiment.
        
        Args:
            name: Experiment name
            version: Experiment version
        """
        self.name = name
        self.version = version
        self.state = ExperimentState.INITIALIZING
        
        # Components
        self.registry = ComponentRegistry()
        self.plugin_manager = PluginManager()
        self.metrics_collector = MetricsCollector()
        
        # Configuration
        self.config = ExperimentConfiguration()
        self.config.name = name
        self.config.version = version
        
        # State
        self.start_time = 0.0
        self.end_time = 0.0
        self.current_step = 0
        self.max_steps = 1000
        
        # Results
        self.results = ExperimentResult()
        self.results.experiment_name = name
        self.results.experiment_version = version
        
        # Agents and systems
        self.agents: List[AgentBrain] = []
        self.world: Optional[SimpleWorld] = None
        self.brain: Optional[Brain] = None
        self.visualization: Optional[VisualizationInterface] = None
        
        # Logging
        self.logger = logging.getLogger(f"experiment.{name}")
        
        # Event handling
        self.event_handlers: Dict[str, List[Callable]] = defaultdict(list)
        
        # Threading
        self.event_queue = queue.Queue()
        self.worker_thread: Optional[threading.Thread] = None
        self.stop_event = threading.Event()
    
    def get_name(self) -> str:
        """Get experiment name."""
        return self.name
    
    def get_version(self) -> str:
        """Get experiment version."""
        return self.version
    
    def get_state(self) -> ExperimentState:
        """Get experiment state."""
        return self.state
    
    def initialize(self, config: Optional[ExperimentConfiguration] = None) -> bool:
        """
        Initialize experiment.
        
        Args:
            config: Experiment configuration (optional)
            
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            self.state = ExperimentState.INITIALIZING
            self.logger.info(f"Initializing experiment '{self.name}' version {self.version}")
            
            # Update configuration if provided
            if config:
                self.config = config
            
            # Initialize components
            self._initialize_components()
            
            # Configure systems
            self._configure_systems()
            
            # Set experiment state
            self.state = ExperimentState.READY
            
            self.logger.info(f"Experiment '{self.name}' initialized successfully")
            return True
            
        except Exception as e:
            self.state = ExperimentState.ERROR
            self.logger.error(f"Failed to initialize experiment '{self.name}': {e}")
            return False
    
    def configure(self, config: ExperimentConfiguration) -> bool:
        """
        Configure experiment.
        
        Args:
            config: Experiment configuration
            
        Returns:
            True if configuration successful, False otherwise
        """
        try:
            self.state = ExperimentState.CONFIGURING
            self.config = config
            
            # Apply configuration
            self._apply_configuration()
            
            # Validate configuration
            if not self._validate_configuration():
                self.state = ExperimentState.ERROR
                return False
            
            self.state = ExperimentState.READY
            self.logger.info(f"Experiment '{self.name}' configured successfully")
            return True
            
        except Exception as e:
            self.state = ExperimentState.ERROR
            self.logger.error(f"Failed to configure experiment '{self.name}': {e}")
            return False
    
    def start(self) -> bool:
        """
        Start experiment.
        
        Returns:
            True if experiment started successfully, False otherwise
        """
        try:
            if self.state != ExperimentState.READY:
                self.logger.error(f"Experiment '{self.name}' is not in READY state")
                return False
            
            self.state = ExperimentState.RUNNING
            self.start_time = time.time()
            
            # Initialize metrics collection
            self.metrics_collector.start()
            
            # Start agents
            self._start_agents()
            
            # Start simulation
            self._run_simulation()
            
            self.state = ExperimentState.COMPLETING
            return True
            
        except Exception as e:
            self.state = ExperimentState.ERROR
            self.logger.error(f"Failed to start experiment '{self.name}': {e}")
            return False
    
    def pause(self) -> bool:
        """
        Pause experiment.
        
        Returns:
            True if experiment paused successfully, False otherwise
        """
        try:
            if self.state != ExperimentState.RUNNING:
                return False
            
            self.state = ExperimentState.PAUSED
            self.logger.info(f"Experiment '{self.name}' paused")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to pause experiment '{self.name}': {e}")
            return False
    
    def resume(self) -> bool:
        """
        Resume experiment.
        
        Returns:
            True if experiment resumed successfully, False otherwise
        """
        try:
            if self.state != ExperimentState.PAUSED:
                return False
            
            self.state = ExperimentState.RUNNING
            self.logger.info(f"Experiment '{self.name}' resumed")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to resume experiment '{self.name}': {e}")
            return False
    
    def stop(self) -> bool:
        """
        Stop experiment.
        
        Returns:
            True if experiment stopped successfully, False otherwise
        """
        try:
            if self.state not in [ExperimentState.RUNNING, ExperimentState.PAUSED]:
                return False
            
            self.state = ExperimentState.STOPPED
            self._stop_agents()
            self.metrics_collector.stop()
            
            self.logger.info(f"Experiment '{self.name}' stopped")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to stop experiment '{self.name}': {e}")
            return False
    
    def complete(self) -> bool:
        """
        Complete experiment.
        
        Returns:
            True if experiment completed successfully, False otherwise
        """
        try:
            if self.state != ExperimentState.COMPLETING:
                return False
            
            self.end_time = time.time()
            self.duration = self.end_time - self.start_time
            
            # Stop agents
            self._stop_agents()
            
            # Stop metrics collection
            self.metrics_collector.stop()
            
            # Calculate final metrics
            self._calculate_final_metrics()
            
            # Store results
            self._store_results()
            
            self.state = ExperimentState.COMPLETED
            self.logger.info(f"Experiment '{self.name}' completed successfully")
            return True
            
        except Exception as e:
            self.state = ExperimentState.ERROR
            self.logger.error(f"Failed to complete experiment '{self.name}': {e}")
            return False
    
    def cancel(self) -> bool:
        """
        Cancel experiment.
        
        Returns:
            True if experiment cancelled successfully, False otherwise
        """
        try:
            self.state = ExperimentState.CANCELLED
            self._stop_agents()
            self.metrics_collector.stop()
            
            self.logger.info(f"Experiment '{self.name}' cancelled")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to cancel experiment '{self.name}': {e}")
            return False
    
    def is_running(self) -> bool:
        """Check if experiment is running."""
        return self.state == ExperimentState.RUNNING
    
    def is_paused(self) -> bool:
        """Check if experiment is paused."""
        return self.state == ExperimentState.PAUSED
    
    def is_completed(self) -> bool:
        """Check if experiment is completed."""
        return self.state == ExperimentState.COMPLETED
    
    def get_results(self) -> ExperimentResult:
        """Get experiment results."""
        return self.results
    
    def get_metrics(self) -> Dict[str, float]:
        """Get current experiment metrics."""
        return self.metrics_collector.get_current_metrics()
    
    def get_configuration(self) -> ExperimentConfiguration:
        """Get experiment configuration."""
        return self.config
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """
        Add event handler.
        
        Args:
            event_type: Event type
            handler: Event handler function
        """
        self.event_handlers[event_type].append(handler)
    
    def emit_event(self, event_type: str, data: Dict[str, Any]):
        """
        Emit event.
        
        Args:
            event_type: Event type
            data: Event data
        """
        # Add to event queue
        self.event_queue.put((event_type, data))
        
        # Handle event synchronously
        for handler in self.event_handlers.get(event_type, []):
            try:
                handler(data)
            except Exception as e:
                self.logger.error(f"Error in event handler for '{event_type}': {e}")
    
    @abstractmethod
    def _initialize_components(self):
        """Initialize experiment components (override in subclasses)."""
        pass
    
    @abstractmethod
    def _configure_systems(self):
        """Configure systems (override in subclasses)."""
        pass
    
    @abstractmethod
    def _apply_configuration(self):
        """Apply configuration (override in subclasses)."""
        pass
    
    @abstractmethod
    def _validate_configuration(self) -> bool:
        """
        Validate configuration (override in subclasses).
        
        Returns:
            True if configuration is valid, False otherwise
        """
        pass
    
    @abstractmethod
    def _start_agents(self):
        """Start agents (override in subclasses)."""
        pass
    
    @abstractmethod
    def _run_simulation(self):
        """Run simulation (override in subclasses)."""
        pass
    
    @abstractmethod
    def _stop_agents(self):
        """Stop agents (override in subclasses)."""
        pass
    
    @abstractmethod
    def _calculate_final_metrics(self):
        """Calculate final metrics (override in subclasses)."""
        pass
    
    @abstractmethod
    def _store_results(self):
        """Store results (override in subclasses)."""
        pass
    
    def _log_event(self, event_type: str, data: Dict[str, Any]):
        """
        Log event.
        
        Args:
            event_type: Event type
            data: Event data
        """
        self.logger.info(f"Event '{event_type}': {data}")
    
    def _collect_metrics(self, step: int, agents: List[AgentBrain], world: SimpleWorld, brain: Brain):
        """
        Collect metrics for current step.
        
        Args:
            step: Current step
            agents: List of agents
            world: World object
            brain: Brain object
        """
        self.metrics_collector.collect(
            step, agents, world, brain, self.config
        )
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        if exc_type is not None:
            self.cancel()
        elif self.state == ExperimentState.RUNNING:
            self.complete()
        else:
            self.stop()


class ExperimentRunner:
    """Enhanced experiment runner for NLM experiments."""
    
    def __init__(self):
        """Initialize experiment runner."""
        self.experiments: Dict[str, Experiment] = {}
        self.logger = logging.getLogger(__name__)
    
    def register_experiment(self, experiment: Experiment):
        """
        Register experiment.
        
        Args:
            experiment: Experiment object
        """
        self.experiments[experiment.get_name()] = experiment
        self.logger.info(f"Registered experiment '{experiment.get_name()}'")
    
    def get_experiment(self, name: str) -> Optional[Experiment]:
        """
        Get experiment by name.
        
        Args:
            name: Experiment name
            
        Returns:
            Experiment object if found, None otherwise
        """
        return self.experiments.get(name)
    
    def list_experiments(self) -> List[str]:
        """
        List all registered experiments.
        
        Returns:
            List of experiment names
        """
        return list(self.experiments.keys())
    
    def run_experiment(self, name: str, config: Optional[ExperimentConfiguration] = None) -> Optional[ExperimentResult]:
        """
        Run experiment.
        
        Args:
            name: Experiment name
            config: Experiment configuration (optional)
            
        Returns:
            Experiment results if successful, None otherwise
        """
        try:
            experiment = self.experiments.get(name)
            if not experiment:
                self.logger.error(f"Experiment '{name}' not found")
                return None
            
            if config:
                if not experiment.configure(config):
                    self.logger.error(f"Failed to configure experiment '{name}'")
                    return None
            
            if not experiment.start():
                self.logger.error(f"Failed to start experiment '{name}'")
                return None
            
            # Wait for experiment to complete
            while experiment.is_running():
                time.sleep(0.1)
            
            if experiment.is_completed():
                results = experiment.get_results()
                self.logger.info(f"Experiment '{name}' completed successfully")
                return results
            else:
                self.logger.error(f"Experiment '{name}' failed")
                return None
            
        except Exception as e:
            self.logger.error(f"Error running experiment '{name}': {e}")
            return None
    
    def run_experiments_parallel(self, experiment_configs: List[tuple]) -> List[ExperimentResult]:
        """
        Run multiple experiments in parallel.
        
        Args:
            experiment_configs: List of (experiment_name, config) tuples
            
        Returns:
            List of experiment results
        """
        results = []
        
        def run_single_experiment(experiment_name: str, config: Optional[ExperimentConfiguration]):
            result = self.run_experiment(experiment_name, config)
            results.append(result)
        
        threads = []
        for experiment_name, config in experiment_configs:
            thread = threading.Thread(
                target=run_single_experiment,
                args=(experiment_name, config)
            )
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        return results
    
    def stop_all_experiments(self):
        """Stop all running experiments."""
        for experiment in self.experiments.values():
            if experiment.is_running():
                experiment.stop()
    
    def cancel_all_experiments(self):
        """Cancel all running experiments."""
        for experiment in self.experiments.values():
            if experiment.is_running():
                experiment.cancel()