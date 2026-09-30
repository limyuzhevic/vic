"""
High-level Python API core modules for NLM (Neural Learning Machine).

This module provides the implementation for high-level simulation abstractions
and enhanced Pythonic interfaces for the NLM framework.
"""

import json
import time
import threading
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, asdict
from enum import Enum

# Import NLM core classes
from ..pynlm import (
    Config, 
    Brain, 
    SimpleWorld, 
    Action,
    SensoryPercept,
    ActionType,
    NeuronType,
    DevelopmentalStage,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId
)

# Import existing modules
from ..profiling import (
    PerformanceProfiler,
    MemoryMonitor,
    CPUMonitor,
    SimulationProfiler,
    MemoryMetrics,
    CPUMetrics,
    SimulationMetrics
)

from ..experiments import (
    Experiment,
    ExperimentConfig,
    ExperimentResult,
    ExperimentStatus,
    ExperimentType,
    ExperimentSuite
)

from ..nlm.advanced import (
    AdvancedExperiment,
    CustomEnvironment
)
class SimulationStatus(Enum):
    """Simulation execution status."""
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    TERMINATED = "terminated"
    ERROR = "error"


@dataclass
class SimulationMetrics:
    """Simulation metrics data structure."""
    timestamp: float
    step: int
    elapsed_time: float
    brain_stats: Dict[str, Any]
    memory_stats: Dict[str, Any]
    performance_stats: Dict[str, Any]
    custom_metrics: Dict[str, Any] = None


def _create_default_config(config_file: Optional[str] = None,
                          config_dict: Optional[Dict[str, Any]] = None) -> Config:
    """Create default configuration."""
    config = Config()
    
    # Default values
    defaults = {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.12,
        "random.seed": 42,
        "energy.decay_rate": 0.01,
        "world.width": 100,
        "world.height": 100,
        "world.vision_width": 10,
        "world.vision_height": 10,
        "world.max_energy": 100.0,
        "world.energy_decay_rate": 0.01,
        "world.start_x": 50,
        "world.start_y": 50,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "simulation_timestep": 0.001,
        "replay_interval": 100,
        "consolidation_interval": 1000
    }
    
    # Apply defaults
    for key, value in defaults.items():
        if isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, str):
            config.set(key, value)
    
    # Load from file if specified
    if config_file:
        if not config.loadFromFile(config_file):
            raise RuntimeError(f"Failed to load configuration from {config_file}")
    
    # Override with config_dict
    if config_dict:
        for key, value in config_dict.items():
            if isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
    
    return config
class BrainSimulator:
    """
    High-level interface for brain simulations with easy setup and control.
    
    Provides a Pythonic API for creating, configuring, and running brain simulations
    with integrated memory systems, prediction, and cognition.
    
    Args:
        config_file (str, optional): Path to configuration file
        config_dict (dict, optional): Configuration dictionary
        simulation_id (str, optional): Unique identifier for simulation
    """
    
    def __init__(self, config_file: Optional[str] = None, 
                 config_dict: Optional[Dict[str, Any]] = None,
                 simulation_id: Optional[str] = None):
        self.simulation_id = simulation_id or f"sim_{int(time.time())}"
        self.status = SimulationStatus.INITIALIZING
        self.metrics_history: List[SimulationMetrics] = []
        self.current_step = 0
        self.start_time = None
        self.config_dict = config_dict or {}
        
        # Performance monitoring
        self.performance_profiler = PerformanceProfiler()
        self.memory_monitor = MemoryMonitor()
        self.cpu_monitor = CPUMonitor()
        
        # Core components
        self.config = None
        self.brain = None
        self.world = None
        
        # Initialize components
        self._initialize_components(config_file)
        
    def _initialize_components(self, config_file: Optional[str] = None):
        """Initialize simulation components from configuration."""
        # Create configuration
        self.config = _create_default_config(config_file, self.config_dict)
        
        # Create brain with configuration
        self.brain = Brain(self.config)
        
        # Initialize brain
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
            
        self.status = SimulationStatus.RUNNING
        self.start_time = time.time()
        
    def step(self, num_steps: int = 1) -> List[SimulationMetrics]:
        """
        Run simulation for specified number of steps.
        
        Args:
            num_steps (int): Number of simulation steps to run
            
        Returns:
            List[SimulationMetrics]: Metrics collected during simulation
        """
        if self.status != SimulationStatus.RUNNING:
            raise RuntimeError(f"Cannot step: simulation status is {self.status.value}")
        
        step_metrics = []
        step_start_time = time.time()
        
        for step in range(num_steps):
            step_num = self.current_step + step + 1
            
            # Profile step execution
            def execute_step():
                self.brain.step(step_num)
                
            step_result = self.performance_profiler.benchmark_function(
                execute_step, f"step_{step_num}"
            )
            
            # Collect metrics
            metrics = self._collect_metrics(step_num, step_start_time)
            step_metrics.append(metrics)
            
            self.current_step = step_num
        
        return step_metrics
        
    def _collect_metrics(self, step: int, step_start_time: float) -> SimulationMetrics:
        """Collect comprehensive metrics for current simulation state."""
        elapsed_time = time.time() - step_start_time
        
        # Get brain statistics
        brain_stats = {
            "total_neurons": self.brain.getTotalNeuronCount(),
            "total_synapses": self.brain.getTotalSynapseCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "e_i_ratio": self.brain.getExcitationInhibitionRatio(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "pending_events": self.brain.getPendingSpikeEventCount(),
            "developmental_stage": str(self.brain.getDevelopmentalStage())
        }
        
        # Get memory system statistics
        memory_stats = {}
        if hasattr(self.brain, 'getWorkingMemory') and self.brain.getWorkingMemory():
            memory_stats['working_memory'] = {
                "size": self.brain.getWorkingMemory().size(),
                "capacity": self.brain.getWorkingMemory().getCapacity(),
                "active_traces": self.brain.getWorkingMemory().getActiveTraces()
            }
        
        if hasattr(self.brain, 'getEpisodicMemory') and self.brain.getEpisodicMemory():
            memory_stats['episodic_memory'] = {
                "episode_count": self.brain.getEpisodicMemory().getEpisodeCount(),
                "max_episodes": self.brain.getEpisodicMemory().getMaxEpisodes()
            }
        
        # Get performance metrics
        performance_stats = {
            "cpu_usage": self.cpu_monitor.get_current_cpu().cpu_percent,
            "memory_usage_mb": self.memory_monitor.get_current_memory().rss_mb,
            "simulation_time_ms": self.performance_profiler.benchmark_results[-1].execution_time * 1000 if self.performance_profiler.benchmark_results else 0,
            "peak_memory_mb": self.memory_monitor.get_peak_memory().rss_mb
        }
        
        return SimulationMetrics(
            timestamp=time.time(),
            step=step,
            elapsed_time=elapsed_time,
            brain_stats=brain_stats,
            memory_stats=memory_stats,
            performance_stats=performance_stats,
            custom_metrics={}
        )
        
    def run_simulation(self, num_steps: int = 100, save_checkpoint: bool = False,
                      checkpoint_interval: int = 1000) -> Dict[str, Any]:
        """
        Run complete simulation with progress monitoring and optional checkpointing.
        
        Args:
            num_steps (int): Number of steps to simulate
            save_checkpoint (bool): Whether to save checkpoints
            checkpoint_interval (int): Steps between checkpoints
            
        Returns:
            Dict[str, Any]: Simulation results and metrics
        """
        print(f"Starting BrainSimulator simulation: {self.simulation_id}")
        print(f"Total steps: {num_steps}, Checkpoint interval: {checkpoint_interval}")
        
        # Start monitoring
        self.performance_profiler.start_profiling()
        self.memory_monitor.start_monitoring()
        self.cpu_monitor.start_monitoring()
        
        simulation_start_time = time.time()
        simulation_metrics = []
        
        try:
            for step in range(0, num_steps, checkpoint_interval):
                steps_to_run = min(checkpoint_interval, num_steps - step)
                
                print(f"Running steps {step + 1}-{step + steps_to_run}")
                
                # Run steps
                step_metrics = self.step(steps_to_run)
                simulation_metrics.extend(step_metrics)
                
                # Save checkpoint if requested
                if save_checkpoint and (step + steps_to_run) % checkpoint_interval == 0:
                    checkpoint_file = f"{self.simulation_id}_checkpoint_{step + steps_to_run}.json"
                    self.save_simulation_state(checkpoint_file)
                    print(f"Checkpoint saved: {checkpoint_file}")
                    
                # Print progress every 10% or at milestones
                if (step + steps_to_run) % max(1, num_steps // 10) == 0 or (step + steps_to_run) == num_steps:
                    progress = (step + steps_to_run) / num_steps * 100
                    print(f"Progress: {progress:.1f}%")
                    
        finally:
            # Stop monitoring
            self.performance_profiler.stop_profiling()
            self.memory_monitor.stop_monitoring()
            self.cpu_monitor.stop_monitoring()
            
        simulation_end_time = time.time()
        duration = simulation_end_time - simulation_start_time
        
        print(f"Simulation completed: {self.simulation_id}")
        print(f"Duration: {duration:.2f} seconds")
        print(f"Total steps executed: {self.current_step}")
        
        # Compile results
        results = {
            "simulation_id": self.simulation_id,
            "duration": duration,
            "total_steps": num_steps,
            "steps_executed": self.current_step,
            "metrics": asdict(simulation_metrics) if simulation_metrics else {},
            "performance_summary": self.performance_profiler.getSummary(),
            "memory_summary": {
                "peak_memory_mb": self.memory_monitor.get_peak_memory().rss_mb,
                "average_memory_mb": self.memory_monitor.get_average_memory().rss_mb
            }
        }
        
        self.status = SimulationStatus.COMPLETED
        return results
        
    def save_simulation_state(self, filepath: str):
        """
        Save current simulation state to file.
        
        Args:
            filepath (str): Path to save state file
        """
        state = {
            "simulation_id": self.simulation_id,
            "current_step": self.current_step,
            "config": self.config_dict,
            "metrics_history": asdict(self.metrics_history) if self.metrics_history else [],
            "performance_data": {
                "benchmark_results": asdict(self.performance_profiler.benchmark_results) if self.performance_profiler.benchmark_results else [],
                "profiling_data": self.performance_profiler.getProfilingData()
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2, default=str)
            
        print(f"Simulation state saved: {filepath}")
        
    def load_simulation_state(self, filepath: str):
        """
        Load simulation state from file.
        
        Args:
            filepath (str): Path to state file
        """
        with open(filepath, 'r') as f:
            state = json.load(f)
            
        self.simulation_id = state.get("simulation_id", self.simulation_id)
        self.current_step = state.get("current_step", 0)
        self.config_dict = state.get("config", {})
        self.metrics_history = state.get("metrics_history", [])
        
        # Re-initialize brain from config
        self._initialize_components()
        
        print(f"Simulation state loaded: {filepath}")
        
    def pause(self):
        """Pause simulation."""
        self.status = SimulationStatus.PAUSED
        
    def resume(self):
        """Resume simulation."""
        if self.status == SimulationStatus.PAUSED:
            self.status = SimulationStatus.RUNNING
            
    def stop(self):
        """Stop simulation."""
        self.status = SimulationStatus.TERMINATED
        self.pause()
        
    def get_current_metrics(self) -> Optional[SimulationMetrics]:
        """Get current simulation metrics."""
        if self.metrics_history:
            return self.metrics_history[-1]
        return None
        
    def reset(self):
        """Reset simulation to initial state."""
        self.stop()
        self.metrics_history.clear()
        self.current_step = 0
        self.start_time = time.time()
        self.performance_profiler.clearProfilingData()
        self.performance_profiler.benchmark_results.clear()
        
        # Re-initialize components
        self._initialize_components()
        
    def __enter__(self):
        """Context manager entry."""
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - clean up resources."""
        self.stop()
class WorldSimulator:
    """
    High-level interface for world/environment simulations.
    
    Provides easy setup and control of simulated environments with agents,
    objects, and dynamic behaviors.
    
    Args:
        config_dict (dict, optional): World configuration
        width (int): World width
        height (int): World height
        simulation_id (str, optional): Unique identifier
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None, width: int = 100, 
                 height: int = 100, simulation_id: Optional[str] = None):
        self.simulation_id = simulation_id or f"world_{int(time.time())}"
        self.width = width
        self.height = height
        self.config_dict = config_dict or {}
        self.status = SimulationStatus.INITIALIZING
        
        # Components
        self.config = Config()
        self.world = SimpleWorld()
        self.agent = None
        
        # Performance monitoring
        self.performance_profiler = PerformanceProfiler()
        self.simulation_profiler = SimulationProfiler()
        
        # Initialize
        self._initialize()
        
    def _initialize(self):
        """Initialize world simulation."""
        # Apply configuration
        for key, value in self.config_dict.items():
            if isinstance(value, int):
                self.config.set(key, value)
            elif isinstance(value, float):
                self.config.set(key, value)
            elif isinstance(value, bool):
                self.config.set(key, value)
            elif isinstance(value, str):
                self.config.set(key, value)
        
        # Configure world
        world_width = self.config.getOr("world.width", self.width)
        world_height = self.config.getOr("world.height", self.height)
        vision_width = self.config.getOr("world.vision_width", 10)
        vision_height = self.config.getOr("world.vision_height", 10)
        
        self.world.configure(world_width, world_height, vision_width, vision_height)
        
        # Set starting position
        start_x = self.config.getOr("world.start_x", world_width // 2)
        start_y = self.config.getOr("world.start_y", world_height // 2)
        self.world.setAgentStart(start_x, start_y)
        
        # Set energy parameters
        self.world.setMaxEnergy(self.config.getOr("world.max_energy", 100.0))
        self.world.setEnergyDecayRate(self.config.getOr("world.energy_decay_rate", 0.01))
        
        # Set random seed
        seed = self.config.getOr("random.seed", 42)
        self.world.setRandomSeed(seed)
        
        self.status = SimulationStatus.RUNNING
        
    def add_agent(self, brain: Brain) -> 'WorldSimulator':
        """
        Add an agent to the world.
        
        Args:
            brain (Brain): Agent's brain
            
        Returns:
            WorldSimulator: For method chaining
        """
        from ..pynlm import AgentBrain
        agent_brain = AgentBrain(brain)
        self.agent = Agent(agent_brain, self.world)
        return self
        
    def configure(self, **kwargs):
        """
        Configure world parameters.
        
        Args:
            **kwargs: Configuration parameters
        """
        for key, value in kwargs.items():
            self.config_dict[key] = value
            
        # Re-initialize with new configuration
        self._initialize()
        
    def add_world_object(self, obj) -> 'WorldSimulator':
        """
        Add a world object.
        
        Args:
            obj: World object
            
        Returns:
            WorldSimulator: For method chaining
        """
        self.world.addObject(obj)
        return self
        
    def step(self, timestep: float = 0.01) -> Dict[str, Any]:
        """
        Run one world simulation step.
        
        Args:
            timestep (float): Simulation timestep
            
        Returns:
            Dict[str, Any]: Step results and metrics
        """
        if self.status != SimulationStatus.RUNNING:
            raise RuntimeError(f"Cannot step: world status is {self.status.value}")
        
        # Profile the step
        def execute_step():
            self.world.update(timestep)
            
        step_result = self.performance_profiler.benchmark_function(
            execute_step, f"world_step_{self.current_step}"
        )
        
        # Get world state
        world_state = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Get brain metrics if agent exists
        brain_metrics = {}
        if self.agent:
            brain_metrics = {
                "total_neurons": self.agent.getBrain().getTotalNeuronCount(),
                "total_synapses": self.agent.getBrain().getTotalSynapseCount(),
                "active_neurons": self.agent.getBrain().getActiveNeuronCount(),
                "firing_neurons": self.agent.getBrain().getFiringNeuronCount(),
                "average_firing_rate": self.agent.getBrain().getAverageFiringRate(),
                "total_spikes": self.agent.getBrain().getTotalSpikeCount()
            }
        
        return {
            "step": self.current_step,
            "world_state": asdict(world_state) if world_state else {},
            "percept": str(percept),
            "brain_metrics": brain_metrics,
            "step_time_ms": step_result.execution_time * 1000,
            "success": True
        }
        
    def run_simulation(self, num_steps: int = 100) -> List[Dict[str, Any]]:
        """
        Run world simulation for specified number of steps.
        
        Args:
            num_steps (int): Number of steps to simulate
            
        Returns:
            List[Dict[str, Any]]: List of step results
        """
        print(f"Starting WorldSimulator: {self.simulation_id}")
        print(f"Running {num_steps} simulation steps")
        
        results = []
        
        for step in range(num_steps):
            step_result = self.step()
            results.append(step_result)
            
            # Print progress every 10%
            if (step + 1) % max(1, num_steps // 10) == 0 or (step + 1) == num_steps:
                progress = (step + 1) / num_steps * 100
                print(f"Progress: {progress:.1f}%")
        
        print(f"World simulation completed: {self.simulation_id}")
        return results
        
    def reset(self):
        """Reset world to initial state."""
        self.world.reset()
        
    def __repr__(self):
        return f"WorldSimulator(width={self.width}, height={self.height}, status={self.status.value})"


# Forward reference for Agent
class Agent:
    """Placeholder for Agent class - needs implementation."""
    pass
class AgentSimulator:
    """
    High-level interface for agent-environment interaction simulations.
    
    Manages agent behavior in environments with sensory input and motor output,
    providing advanced control and monitoring capabilities.
    
    Args:
        brain (Brain): Agent's brain
        world (WorldSimulator): Environment
        config_dict (dict, optional): Agent configuration
    """
    
    def __init__(self, brain: Brain, world: WorldSimulator,
                 config_dict: Optional[Dict[str, Any]] = None):
        self.brain = brain
        self.world = world
        self.config_dict = config_dict or {}
        self.status = SimulationStatus.INITIALIZING
        
        # Create agent brain interface
        from ..pynlm import AgentBrain
        self.agent_brain = AgentBrain(brain)
        
        # Performance monitoring
        self.performance_profiler = PerformanceProfiler()
        self.simulation_profiler = SimulationProfiler()
        self.memory_monitor = MemoryMonitor()
        
        # Episode management
        self.episode_results: List[Dict[str, Any]] = []
        
        # Initialize
        self._initialize()
        
    def _initialize(self):
        """Initialize agent simulator."""
        # Apply configuration
        for key, value in self.config_dict.items():
            if isinstance(value, int):
                self.brain.getConfig().set(key, value)
            elif isinstance(value, float):
                self.brain.getConfig().set(key, value)
            elif isinstance(value, bool):
                self.brain.getConfig().set(key, value)
            elif isinstance(value, str):
                self.brain.getConfig().set(key, value)
        
        self.status = SimulationStatus.RUNNING
        
    def step(self, timestep: float = 0.01) -> Dict[str, Any]:
        """
        Run one agent step in the environment.
        
        Args:
            timestep (float): Simulation timestep
            
        Returns:
            Dict[str, Any]: Step results including sensory input, action, and metrics
        """
        if self.status != SimulationStatus.RUNNING:
            raise RuntimeError(f"Cannot step: agent status is {self.status.value}")
        
        # Profile step execution
        def execute_step():
            # Get sensory input
            percept = self.world.world.getSensoryPercept()
            
            # Process input through agent brain
            self.agent_brain.initialize(self.world.world)
            self.agent_brain.processSensoryInput(percept)
            
            # Produce action
            action = self.agent_brain.decodeMotorCommand()
            
            # Apply action to world
            self.world.world.applyMotorCommand(action, self.world.world.getSimulationTime())
            
            # Update world
            self.world.world.update(timestep)
            
        step_result = self.performance_profiler.benchmark_function(
            execute_step, f"agent_step_{len(self.episode_results)}"
        )
        
        # Get step metrics
        metrics = self._collect_step_metrics()
        
        return {
            "step": len(self.episode_results),
            "sensory_input": str(percept),
            "action": str(action),
            "metrics": metrics,
            "step_time_ms": step_result.execution_time * 1000,
            "success": True
        }
        
    def _collect_step_metrics(self) -> Dict[str, Any]:
        """Collect metrics for current step."""
        return {
            "brain_stats": {
                "total_neurons": self.brain.getTotalNeuronCount(),
                "total_synapses": self.brain.getTotalSynapseCount(),
                "active_neurons": self.brain.getActiveNeuronCount(),
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "e_i_ratio": self.brain.getExcitationInhibitionRatio()
            },
            "memory_stats": {
                "working_memory_size": self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                "episodic_memory_count": self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0
            },
            "performance_stats": {
                "cpu_usage": self.memory_monitor.get_current_memory().rss_mb / 1024,
                "memory_usage_mb": self.memory_monitor.get_current_memory().rss_mb
            }
        }
        
    def run_episode(self, num_steps: int = 100, save_checkpoint: bool = True,
                   checkpoint_interval: int = 20) -> List[Dict[str, Any]]:
        """
        Run a complete agent episode.
        
        Args:
            num_steps (int): Number of steps in episode
            save_checkpoint (bool): Whether to save checkpoints
            checkpoint_interval (int): Steps between checkpoints
            
        Returns:
            List[Dict[str, Any]]: Episode results
        """
        print(f"Starting agent episode: {self.brain.getConfig().summary()}")
        
        # Reset agent and world for new episode
        self.brain.reset()
        self.world.reset()
        
        # Clear episode results
        self.episode_results.clear()
        
        # Start monitoring
        self.performance_profiler.start_profiling()
        self.memory_monitor.start_monitoring()
        
        episode_start_time = time.time()
        
        try:
            for step in range(num_steps):
                step_result = self.step()
                self.episode_results.append(step_result)
                
                # Save checkpoint
                if save_checkpoint and (step + 1) % checkpoint_interval == 0:
                    checkpoint_file = f"episode_checkpoint_step_{step + 1}.json"
                    self.save_checkpoint(step + 1, checkpoint_file)
                
                # Print progress
                if (step + 1) % max(1, num_steps // 10) == 0 or (step + 1) == num_steps:
                    progress = (step + 1) / num_steps * 100
                    print(f"Episode progress: {progress:.1f}%")
                    
        finally:
            # Stop monitoring
            self.performance_profiler.stop_profiling()
            self.memory_monitor.stop_monitoring()
            
        episode_end_time = time.time()
        duration = episode_end_time - episode_start_time
        
        print(f"Agent episode completed")
        print(f"Duration: {duration:.2f} seconds")
        print(f"Steps executed: {len(self.episode_results)}")
        
        return self.episode_results
        
    def save_checkpoint(self, step: int, filepath: str):
        """
        Save checkpoint for episode recovery.
        
        Args:
            step (int): Current step number
            filepath (str): Path to save checkpoint
        """
        checkpoint_data = {
            "step": step,
            "episode_results": self.episode_results,
            "brain_state": self.brain.logStatus(),
            "world_state": self.world.getAgentBody(),
            "performance_data": self.performance_profiler.getProfilingData()
        }
        
        with open(filepath, 'w') as f:
            json.dump(checkpoint_data, f, indent=2, default=str)
            
        print(f"Checkpoint saved: {filepath}")
        
    def load_checkpoint(self, filepath: str, step: int = 0):
        """
        Load checkpoint for episode recovery.
        
        Args:
            filepath (str): Path to checkpoint file
            step (int): Step number to resume at
        """
        with open(filepath, 'r') as f:
            checkpoint_data = json.load(f)
            
        self.episode_results = checkpoint_data.get("episode_results", [])
        
        # Note: In a real implementation, you would load the actual brain and world state
        # For now, we'll just log the checkpoint
        print(f"Checkpoint loaded: {filepath}")
        print(f"Resuming at step: {step}")
        
    def save_results(self, filepath: str):
        """
        Save episode results to file.
        
        Args:
            filepath (str): Path to save results
        """
        results_data = {
            "episode_results": self.episode_results,
            "brain_config": self.brain.getConfig().summary(),
            "performance_summary": self.performance_profiler.getSummary(),
            "timestamp": time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2, default=str)
            
        print(f"Episode results saved: {filepath}")
        
    def load_results(self, filepath: str):
        """
        Load episode results from file.
        
        Args:
            filepath (str): Path to results file
        """
        with open(filepath, 'r') as f:
            results_data = json.load(f)
            
        self.episode_results = results_data.get("episode_results", [])
        
        print(f"Episode results loaded: {filepath}")
        
    def analyze_episode(self) -> Dict[str, Any]:
        """
        Analyze episode results and generate statistics.
        
        Returns:
            Dict[str, Any]: Episode analysis
        """
        if not self.episode_results:
            return {"error": "No episode results to analyze"}
        
        analysis = {
            "total_steps": len(self.episode_results),
            "successful_steps": len([r for r in self.episode_results if r.get("success", False)]),
            "success_rate": len([r for r in self.episode_results if r.get("success", False)]) / len(self.episode_results) if self.episode_results else 0,
            "average_step_time_ms": 0,
            "total_execution_time": 0,
            "brain_stats": self._aggregate_brain_stats(),
            "memory_stats": self._aggregate_memory_stats()
        }
        
        # Calculate averages
        step_times = [r.get("step_time_ms", 0) for r in self.episode_results]
        if step_times:
            analysis["average_step_time_ms"] = sum(step_times) / len(step_times)
            
        return analysis
        
    def _aggregate_brain_stats(self) -> Dict[str, Any]:
        """Aggregate brain statistics across episode."""
        if not self.episode_results:
            return {}
        
        brain_stats = {
            "total_neurons": self.episode_results[0].get("metrics", {}).get("brain_stats", {}).get("total_neurons", 0),
            "total_synapses": self.episode_results[0].get("metrics", {}).get("brain_stats", {}).get("total_synapses", 0),
            "peak_active_neurons": 0,
            "peak_firing_neurons": 0,
            "peak_average_firing_rate": 0,
            "peak_total_spikes": 0
        }
        
        for result in self.episode_results:
            metrics = result.get("metrics", {})
            brain = metrics.get("brain_stats", {})
            
            brain_stats["peak_active_neurons"] = max(brain_stats["peak_active_neurons"], brain.get("active_neurons", 0))
            brain_stats["peak_firing_neurons"] = max(brain_stats["peak_firing_neurons"], brain.get("firing_neurons", 0))
            brain_stats["peak_average_firing_rate"] = max(brain_stats["peak_average_firing_rate"], brain.get("average_firing_rate", 0))
            brain_stats["peak_total_spikes"] = max(brain_stats["peak_total_spikes"], brain.get("total_spikes", 0))
        
        return brain_stats
        
    def _aggregate_memory_stats(self) -> Dict[str, Any]:
        """Aggregate memory statistics across episode."""
        memory_stats = {
            "peak_working_memory": 0,
            "peak_episodic_memory": 0
        }
        
        for result in self.episode_results:
            metrics = result.get("metrics", {})
            memory = metrics.get("memory_stats", {})
            
            memory_stats["peak_working_memory"] = max(memory_stats["peak_working_memory"], memory.get("working_memory_size", 0))
            memory_stats["peak_episodic_memory"] = max(memory_stats["peak_episodic_memory"], memory.get("episodic_memory_count", 0))
        
        return memory_stats
        
    def __enter__(self):
        """Context manager entry."""
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.status = SimulationStatus.TERMINATED
class MemoryManager:
    """
    Comprehensive memory management for NLM simulations.
    
    Provides high-level interface for managing working memory, episodic memory,
    and associative memory with advanced utilities for memory analysis,
    consolidation, and replay.
    
    Args:
        brain (Brain): The brain instance to manage memory for
        config_dict (dict, optional): Memory configuration
    """
    
    def __init__(self, brain: Brain, config_dict: Optional[Dict[str, Any]] = None):
        self.brain = brain
        self.config_dict = config_dict or {}
        self.memory_stats: Dict[str, Any] = {}
        self.memory_monitor = MemoryMonitor()
        
        # Initialize memory components
        self._initialize()
        
    def _initialize(self):
        """Initialize memory components."""
        # Configure working memory
        if "working_memory_capacity" in self.config_dict:
            self.brain.getWorkingMemory().setCapacity(self.config_dict["working_memory_capacity"])
        
        # Configure episodic memory
        if "episodic_memory_max_episodes" in self.config_dict:
            self.brain.getEpisodicMemory().setMaxEpisodes(self.config_dict["episodic_memory_max_episodes"])
        
        # Start memory monitoring
        self.memory_monitor.start_monitoring()
        
    def store_working_memory(self, key: str, value: str):
        """
        Store data in working memory.
        
        Args:
            key (str): Memory key
            value (str): Memory value
        """
        self.brain.getWorkingMemory().store(key, value)
        
    def get_working_memory(self, key: str) -> str:
        """
        Retrieve data from working memory.
        
        Args:
            key (str): Memory key
            
        Returns:
            str: Memory value or empty string if not found
        """
        return self.brain.getWorkingMemory().recall(key)
        
    def clear_working_memory(self):
        """Clear working memory."""
        self.brain.getWorkingMemory().clear()
        
    def store_episodic_memory(self, episode_key: str, events: List[str]):
        """
        Store an episode in episodic memory.
        
        Args:
            episode_key (str): Episode identifier
            events (List[str]): List of events in the episode
        """
        self.brain.getEpisodicMemory().storeEpisode(episode_key, events)
        
    def get_episodic_memory(self, episode_key: str) -> List[str]:
        """
        Recall an episode from episodic memory.
        
        Args:
            episode_key (str): Episode identifier
            
        Returns:
            List[str]: List of events or empty list if not found
        """
        return self.brain.getEpisodicMemory().recallEpisode(episode_key)
        
    def clear_episodic_memory(self):
        """Clear episodic memory."""
        self.brain.getEpisodicMemory().clear()
        
    def store_association(self, key1: str, key2: str, strength: float = 1.0):
        """
        Store an association between two memories.
        
        Args:
            key1 (str): First memory key
            key2 (str): Second memory key
            strength (float): Association strength (0.0 to 1.0)
        """
        self.brain.getAssociativeMemory().associate(key1, key2, strength)
        
    def get_association(self, key1: str, key2: str) -> float:
        """
        Get association strength between two memories.
        
        Args:
            key1 (str): First memory key
            key2 (str): Second memory key
            
        Returns:
            float: Association strength or 0.0 if not found
        """
        return self.brain.getAssociativeMemory().getStrength(key1, key2)
        
    def consolidate_memory(self):
        """Consolidate episodic memories."""
        self.brain.getEpisodicMemory().consolidate()
        
    def replay_memory(self, episode_key: str):
        """
        Replay a specific episode.
        
        Args:
            episode_key (str): Episode identifier
        """
        self.brain.getEpisodicMemory().replayEpisode(episode_key)
        
    def get_memory_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive memory statistics.
        
        Returns:
            Dict[str, Any]: Memory statistics
        """
        self.memory_stats = {
            "working_memory": {
                "size": self.brain.getWorkingMemory().size(),
                "capacity": self.brain.getWorkingMemory().getCapacity(),
                "active_traces": self.brain.getWorkingMemory().getActiveTraces()
            },
            "episodic_memory": {
                "episode_count": self.brain.getEpisodicMemory().getEpisodeCount(),
                "max_episodes": self.brain.getEpisodicMemory().getMaxEpisodes()
            },
            "associative_memory": {
                # Note: This would require additional API methods
                "note": "Associative memory statistics not available"
            },
            "performance": {
                "memory_usage_mb": self.memory_monitor.get_current_memory().rss_mb,
                "peak_memory_mb": self.memory_monitor.get_peak_memory().rss_mb,
                "average_memory_mb": self.memory_monitor.get_average_memory().rss_mb
            }
        }
        
        return self.memory_stats
        
    def get_memory_usage(self) -> Dict[str, float]:
        """
        Get current memory usage.
        
        Returns:
            Dict[str, float]: Memory usage metrics
        """
        current_memory = self.memory_monitor.get_current_memory()
        return {
            "rss_mb": current_memory.rss_mb,
            "vms_mb": current_memory.vms_mb,
            "python_objects": current_memory.python_objects,
            "numpy_arrays": current_memory.numpy_arrays
        }
        
    def save_memory_state(self, filepath: str):
        """
        Save memory state to file.
        
        Args:
            filepath (str): Path to save memory state
        """
        memory_state = {
            "working_memory_entries": dict(self.brain.getWorkingMemory().getAllEntries()),
            "episodic_memory_episodes": self.brain.getEpisodicMemory().getAllEpisodes(),
            "associative_memory_associations": self.brain.getAssociativeMemory().getAllAssociations(),
            "memory_stats": self.get_memory_stats(),
            "timestamp": time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(memory_state, f, indent=2, default=str)
            
        print(f"Memory state saved: {filepath}")
        
    def load_memory_state(self, filepath: str):
        """
        Load memory state from file.
        
        Args:
            filepath (str): Path to memory state file
        """
        with open(filepath, 'r') as f:
            memory_state = json.load(f)
            
        # Note: This would require additional API methods to restore memory state
        # For now, we'll just log the action
        print(f"Memory state loaded from: {filepath}")
        print(f"Note: Actual memory restoration would require additional API methods")
        
    def stop_monitoring(self):
        """Stop memory monitoring."""
        self.memory_monitor.stop_monitoring()
        
    def __enter__(self):
        """Context manager entry."""
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - clean up."""
        self.stop_monitoring()
