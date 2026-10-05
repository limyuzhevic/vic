#!/usr/bin/env python3
"""
NLM Advanced Interactive Agent System

A comprehensive interactive agent system for NLM (Neural Learning Machine) with:
- Enhanced agent creation and management
- Advanced simulation control
- Performance monitoring and analysis
- Experiment management
- Data visualization and export
- Interactive CLI interface
"""

import json
import time
import sys
import os
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
import threading
import logging

# Try to import NLM
if __name__ != "__main__":
    try:
        import pynlm
        PYNLM_AVAILABLE = True
    except ImportError:
        PYNLM_AVAILABLE = False
else:
    PYNLM_AVAILABLE = False

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
class AgentType(Enum):
    BASIC = "basic"
    EXPLORER = "explorer"
    LEARNER = "learner"
    ADAPTIVE = "adaptive"
    SOCIAL = "social"
    CREATIVE = "creative"
class SimulationMode(Enum):
    REALTIME = "realtime"
    ACCELERATED = "accelerated"
    STEP_BY_STEP = "step_by_step"
    BATCH = "batch"
class DataFormat(Enum):
    JSON = "json"
    CSV = "csv"
    PICKLE = "pickle"
    BINARY = "binary"
@dataclass
class AgentProfile:
    """Agent personality and behavioral profile"""
    name: str = "default_agent"
    type: AgentType = AgentType.LEARNER
    curiosity_threshold: float = 0.5
    exploration_rate: float = 0.3
    learning_rate: float = 0.01
    risk_tolerance: float = 0.5
    social_presence: float = 0.0
    creativity_level: float = 0.5
    persistence: float = 0.8
    adaptation_speed: float = 0.1
    tags: List[str] = field(default_factory=list)
    metadata: Dict = field(default_factory=dict)
@dataclass
class EnvironmentProfile:
    """Environment characteristics"""
    width: int = 20
    height: int = 20
    vision_range: int = 8
    complexity: float = 0.5
    reward_density: float = 0.3
    hazard_probability: float = 0.1
    resource_availability: float = 0.7
    social_interaction: float = 0.0
@dataclass
class ExperimentSetup:
    """Complete experiment setup"""
    name: str
    description: str = ""
    agent_profile: AgentProfile = field(default_factory=AgentProfile)
    environment_profile: EnvironmentProfile = field(default_factory=EnvironmentProfile)
    duration: float = 1000.0
    timestep: float = 0.1
    max_steps: int = 10000
    save_frequency: int = 100
    parallel_runs: int = 1
    seed: int = 42
    output_dir: str = "experiments"
    tags: List[str] = field(default_factory=list)
    constraints: Dict = field(default_factory=dict)
class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations"""

    def __init__(self, sample_rate: float = 0.1):
        self.sample_rate = sample_rate
        self.metrics = {}
        self.history = []
        self.start_time = None
        self.last_sample = 0.0
        self.monitoring = False
        self.monitor_thread = None

    def start_monitoring(self):
        """Start performance monitoring"""
        self.monitoring = True
        self.start_time = time.time()
        self.last_sample = 0.0
        self.metrics.clear()
        self.history.clear()

        # Start monitoring thread
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        logger.info("Performance monitoring started")

    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        logger.info("Performance monitoring stopped")

    def _monitor_loop(self):
        """Continuous monitoring loop"""
        while self.monitoring:
            try:
                current_time = time.time()
                if current_time - self.last_sample >= self.sample_rate:
                    self._collect_metrics(current_time)
                    self.last_sample = current_time
                time.sleep(0.01)
            except Exception as e:
                logger.error(f"Monitoring error: {e}")
                break

    def _collect_metrics(self, current_time: float):
        """Collect current metrics"""
        try:
            metrics = {
                'timestamp': current_time,
                'uptime': current_time - self.start_time,
                'memory_usage': self._get_memory_usage(),
                'cpu_usage': self._get_cpu_usage(),
                'thread_count': self._get_thread_count(),
                'gc_stats': self._get_gc_stats()
            }
            self.history.append(metrics)
            self.metrics = metrics
        except Exception as e:
            logger.debug(f"Failed to collect metrics: {e}")

    def _get_memory_usage(self) -> Dict[str, float]:
        """Get memory usage statistics"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            return {
                'rss_mb': memory_info.rss / 1024 / 1024,
                'vms_mb': memory_info.vms / 1024 / 1024,
                'shared_mb': getattr(memory_info, 'shared', 0) / 1024 / 1024,
                'text_mb': getattr(memory_info, 'text', 0) / 1024 / 1024
            }
        except ImportError:
            return {'rss_mb': 0.0, 'vms_mb': 0.0}
        except Exception:
            return {'rss_mb': 0.0, 'vms_mb': 0.0}

    def _get_cpu_usage(self) -> float:
        """Get CPU usage percentage"""
        try:
            import psutil
            return psutil.cpu_percent(interval=0.0)
        except ImportError:
            return 0.0
        except Exception:
            return 0.0

    def _get_thread_count(self) -> int:
        """Get current thread count"""
        try:
            import threading
            return threading.active_count()
        except Exception:
            return 0

    def _get_gc_stats(self) -> Dict[str, Any]:
        """Get garbage collection statistics"""
        try:
            import gc
            gc.collect()
            return {
                'count': len(gc.get_objects()),
                'generations': [len(gc.get_objects()) for g in gc.get_stats()],
                'threshold': gc.get_threshold()
            }
        except Exception:
            return {'count': 0, 'generations': [], 'threshold': []}

    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current metrics"""
        return self.metrics.copy()

    def get_history(self) -> List[Dict[str, Any]]:
        """Get monitoring history"""
        return self.history.copy()

    def get_statistics(self) -> Dict[str, Any]:
        """Get performance statistics"""
        if not self.history:
            return {}

        try:
            memory_rss = [m['memory_usage']['rss_mb'] for m in self.history if 'memory_usage' in m]
            cpu_usage = [m['cpu_usage'] for m in self.history if 'cpu_usage' in m]

            stats = {
                'duration': self.metrics.get('uptime', 0),
                'memory_avg_mb': sum(memory_rss) / len(memory_rss) if memory_rss else 0,
                'memory_max_mb': max(memory_rss) if memory_rss else 0,
                'cpu_avg_percent': sum(cpu_usage) / len(cpu_usage) if cpu_usage else 0,
                'cpu_max_percent': max(cpu_usage) if cpu_usage else 0,
                'sample_count': len(self.history)
            }
            return stats
        except Exception:
            return {}
class InteractiveAgentSystem:
    """
    Interactive agent system for NLM with advanced features.
    Provides CLI interface, monitoring, and experiment management.
    """

    def __init__(self, output_dir: str = "nlm_experiments"):
        """Initialize the interactive agent system."""
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)

        # Current experiment state
        self.current_experiment: Optional[ExperimentSetup] = None
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.current_mode = SimulationMode.BATCH
        self.step_count = 0
        self.simulation_start_time = None

        # Monitoring
        self.performance_monitor = PerformanceMonitor()

        # Experiment results
        self.experiment_results: List[Dict] = []

        # Callbacks and hooks
        self.callbacks = {
            'step_start': [],
            'step_end': [],
            'episode_start': [],
            'episode_end': [],
            'simulation_start': [],
            'simulation_end': []
        }

        logger.info("InteractiveAgentSystem initialized")

    def create_advanced_agent(self, agent_profile: AgentProfile) -> Any:
        """
        Create an advanced agent based on the profile.

        Args:
            agent_profile: Agent personality and behavior profile

        Returns:
            Configured agent object
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock implementation")
            return self._create_mock_agent(agent_profile)

        try:
            # Create default config
            config = pynlm.createDefaultConfig()

            # Configure based on agent type
            if agent_profile.type == AgentType.EXPLORER:
                self._configure_explorer_agent(config, agent_profile)
            elif agent_profile.type == AgentType.LEARNER:
                self._configure_learner_agent(config, agent_profile)
            elif agent_profile.type == AgentType.ADAPTIVE:
                self._configure_adaptive_agent(config, agent_profile)
            elif agent_profile.type == AgentType.SOCIAL:
                self._configure_social_agent(config, agent_profile)
            elif agent_profile.type == AgentType.CREATIVE:
                self._configure_creative_agent(config, agent_profile)
            else:
                self._configure_basic_agent(config, agent_profile)

            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()

            logger.info(f"Advanced agent created: {agent_profile.name} ({agent_profile.type.value})")
            return brain

        except Exception as e:
            logger.error(f"Failed to create advanced agent: {e}")
            return self._create_mock_agent(agent_profile)

    def create_advanced_environment(self, env_profile: EnvironmentProfile) -> Any:
        """
        Create an advanced environment based on the profile.

        Args:
            env_profile: Environment characteristics profile

        Returns:
            Configured world object
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock implementation")
            return self._create_mock_world(env_profile)

        try:
            world = pynlm.createSimpleWorld()
            world.configure(
                width=env_profile.width,
                height=env_profile.height,
                visionWidth=env_profile.vision_range,
                visionHeight=env_profile.vision_range
            )

            # Configure environment characteristics
            world.setMaxEnergy(100.0 * env_profile.resource_availability)
            world.setEnergyDecayRate(0.99 - env_profile.complexity * 0.09)
            world.setRandomSeed(self.current_experiment.seed if self.current_experiment else 42)

            logger.info(f"Advanced environment created: {env_profile.width}x{env_profile.height}")
            return world

        except Exception as e:
            logger.error(f"Failed to create advanced environment: {e}")
            return self._create_mock_world(env_profile)

    def create_agent_interface(self, brain: Any, world: Any, agent_profile: AgentProfile) -> Any:
        """
        Create an advanced agent interface connecting brain to world.

        Args:
            brain: The brain object
            world: The world object
            agent_profile: Agent personality profile

        Returns:
            Configured agent interface
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock implementation")
            return self._create_mock_agent_interface(brain, world, agent_profile)

        try:
            # Create agent brain interface
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)

            # Configure based on agent profile
            self._configure_agent_interface(agent, agent_profile)

            logger.info(f"Agent interface created for {agent_profile.name}")
            return agent

        except Exception as e:
            logger.error(f"Failed to create agent interface: {e}")
            return self._create_mock_agent_interface(brain, world, agent_profile)

    def run_advanced_simulation(self, experiment: ExperimentSetup) -> Dict:
        """
        Run an advanced simulation with comprehensive monitoring and control.

        Args:
            experiment: Complete experiment setup

        Returns:
            Dictionary with simulation results
        """
        self.current_experiment = experiment
        self.step_count = 0
        self.simulation_start_time = time.time()

        logger.info(f"Starting advanced simulation: {experiment.name}")
        logger.info(f"Agent: {experiment.agent_profile.name} ({experiment.agent_profile.type.value})")
        logger.info(f"Environment: {experiment.environment_profile.width}x{experiment.environment_profile.height}")
        logger.info(f"Duration: {experiment.duration}s ({experiment.max_steps} steps)")

        try:
            # Create components
            self.current_brain = self.create_advanced_agent(experiment.agent_profile)
            self.current_world = self.create_advanced_environment(experiment.environment_profile)
            self.current_agent = self.create_agent_interface(self.current_brain, self.current_world, experiment.agent_profile)

            # Start performance monitoring
            self.performance_monitor.start_monitoring()

            # Trigger simulation start callbacks
            self._trigger_callbacks('simulation_start', {
                'experiment': experiment,
                'timestamp': time.time()
            })

            # Run simulation
            for step in range(experiment.max_steps):
                if self.current_mode == SimulationMode.PAUSED:
                    time.sleep(0.1)
                    continue

                if self.current_mode == SimulationMode.STOPPED:
                    break

                # Record step start
                step_start_time = time.time()

                # Trigger step start callbacks
                self._trigger_callbacks('step_start', {
                    'step': step,
                    'time': step * experiment.timestep,
                    'agent_state': self._get_current_agent_state()
                })

                # Update world
                self.current_world.update(experiment.timestep)

                # Get sensory input
                percept = self.current_world.getSensoryPercept()

                # Process sensory input
                self.current_agent.processSensoryInput(percept)

                # Run brain step
                self.current_brain.step(step)

                # Get agent action
                action = self.current_agent.decodeMotorCommand()

                # Apply action to world
                self.current_world.applyMotorCommand(action, step * experiment.timestep)

                # Apply learning if enabled
                if self.current_agent.isRewardModulationEnabled():
                    # Calculate reward based on environment
                    reward = self._calculate_reward(percept, action)
                    self.current_agent.applyRewardModulation(reward, 0.0)

                # Update development if enabled
                if self.current_agent.isDevelopmentEnabled():
                    self.current_agent.updateDevelopment(experiment.timestep)

                # Record step end
                step_end_time = time.time()
                step_duration = step_end_time - step_start_time

                # Trigger step end callbacks
                self._trigger_callbacks('step_end', {
                    'step': step,
                    'time': step * experiment.timestep,
                    'duration': step_duration,
                    'action': action,
                    'percept': percept,
                    'agent_state': self._get_current_agent_state(),
                    'metrics': self.performance_monitor.get_current_metrics()
                })

                # Save checkpoint periodically
                if step % experiment.save_frequency == 0:
                    self._save_checkpoint(step, experiment)

                # Update step count
                self.step_count = step

            # Stop monitoring
            self.performance_monitor.stop_monitoring()

            # Calculate final results
            end_time = time.time()
            results = self._calculate_final_results(experiment, end_time)

            # Trigger simulation end callbacks
            self._trigger_callbacks('simulation_end', {
                'results': results,
                'timestamp': end_time
            })

            # Save experiment results
            self._save_experiment_results(results, experiment)

            logger.info(f"Advanced simulation completed: {experiment.name}")
            return results

        except Exception as e:
            logger.error(f"Advanced simulation failed: {e}")
            return {
                'status': 'error',
                'experiment': experiment.name,
                'error': str(e),
                'steps_completed': self.step_count
            }

    def run_experiment_series(self, experiments: List[ExperimentSetup]) -> List[Dict]:
        """
        Run a series of experiments sequentially or in parallel.

        Args:
            experiments: List of experiment setups

        Returns:
            List of experiment results
        """
        results = []

        for i, experiment in enumerate(experiments):
            logger.info(f"Running experiment {i+1}/{len(experiments)}: {experiment.name}")

            # Configure for parallel runs
            if experiment.parallel_runs > 1:
                # For now, run sequentially - parallel implementation would be more complex
                logger.warning(f"Parallel runs not fully implemented, running sequentially for {experiment.name}")

            # Run experiment
            result = self.run_advanced_simulation(experiment)
            results.append(result)

            # Add small delay between experiments
            time.sleep(0.5)

        return results

    def add_callback(self, event_type: str, callback: Callable):
        """
        Add a callback for simulation events.

        Args:
            event_type: Type of event ('step_start', 'step_end', etc.)
            callback: Function to call when event occurs
        """
        if event_type in self.callbacks:
            self.callbacks[event_type].append(callback)
            logger.debug(f"Added callback for {event_type}")
        else:
            logger.warning(f"Unknown event type: {event_type}")

    def remove_callback(self, event_type: str, callback: Callable):
        """
        Remove a callback.

        Args:
            event_type: Type of event
            callback: Function to remove
        """
        if event_type in self.callbacks:
            try:
                self.callbacks[event_type].remove(callback)
                logger.debug(f"Removed callback for {event_type}")
            except ValueError:
                pass

    def _configure_explorer_agent(self, config: Any, profile: AgentProfile):
        """Configure an explorer-type agent."""
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)

    def _configure_learner_agent(self, config: Any, profile: AgentProfile):
        """Configure a learner-type agent."""
        config.set("brain.neuron_count", 1200)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0015)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.reward_modulation.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)

    def _configure_adaptive_agent(self, config: Any, profile: AgentProfile):
        """Configure an adaptive-type agent."""
        config.set("brain.neuron_count", 1300)
        config.set("brain.synapse_density", 0.18)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0015)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.reward_modulation.enable", True)
        config.set("development.enable", True)

    def _configure_social_agent(self, config: Any, profile: AgentProfile):
        """Configure a social-type agent."""
        config.set("brain.neuron_count", 1400)
        config.set("brain.synapse_density", 0.25)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0012)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 1.3)
        config.set("neuromod.social_learning.enable", True)

    def _configure_creative_agent(self, config: Any, profile: AgentProfile):
        """Configure a creative-type agent."""
        config.set("brain.neuron_count", 1600)
        config.set("brain.synapse_density", 0.3)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0018)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0012)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("development.enable", True)
        config.set("development.maturation_speed", 0.8)

    def _configure_basic_agent(self, config: Any, profile: AgentProfile):
        """Configure a basic agent."""
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.reward_modulation.enable", False)

    def _configure_agent_interface(self, agent: Any, profile: AgentProfile):
        """Configure the agent interface based on profile."""
        # Enable features based on agent type
        if profile.type in [AgentType.EXPLORER, AgentType.ADAPTIVE, AgentType.SOCIAL, AgentType.CREATIVE]:
            agent.enableCuriosity(True)

        if profile.type in [AgentType.LEARNER, AgentType.ADAPTIVE, AgentType.SOCIAL]:
            agent.enableRewardModulation(True)

        if profile.type in [AgentType.ADAPTIVE, AgentType.CREATIVE]:
            agent.enableDevelopment(True)

        if profile.type == AgentType.CREATIVE:
            agent.enableStructuralPlasticity(True)

    def _calculate_reward(self, percept: Any, action: Any) -> float:
        """Calculate reward based on percept and action."""
        try:
            # Simple reward calculation based on internal signals
            internal = percept.getInternal() if hasattr(percept, 'getInternal') else []
            if internal:
                return internal[0] if len(internal) > 0 else 0.0
            return 0.0
        except:
            return 0.0

    def _get_current_agent_state(self) -> Dict[str, Any]:
        """Get current agent state."""
        if not self.current_agent:
            return {}

        try:
            state = {
                'curiosity': self.current_agent.getCuriosityLevel() if hasattr(self.current_agent, 'getCuriosityLevel') else 0.0,
                'novelty': self.current_agent.getNoveltyLevel() if hasattr(self.current_agent, 'getNoveltyLevel') else 0.0,
                'prediction_error': self.current_agent.getPredictionError() if hasattr(self.current_agent, 'getPredictionError') else 0.0,
                'neuromodulation': self.current_agent.getNeuromodulationLevel() if hasattr(self.current_agent, 'getNeuromodulationLevel') else 0.0,
                'developmental_stage': self.current_agent.getDevelopmentalStage().value if hasattr(self.current_agent, 'getDevelopmentalStage') else 'Unknown'
            }
            return state
        except:
            return {}

    def _calculate_final_results(self, experiment: ExperimentSetup, end_time: float) -> Dict:
        """Calculate final simulation results."""
        duration = end_time - self.simulation_start_time
        steps_completed = self.step_count

        # Get brain statistics
        brain_stats = {}
        if self.current_brain:
            brain_stats = {
                'total_neurons': self.current_brain.getTotalNeuronCount(),
                'total_synapses': self.current_brain.getTotalSynapseCount(),
                'firing_neurons': self.current_brain.getFiringNeuronCount(),
                'total_spikes': self.current_brain.getTotalSpikeCount(),
                'average_firing_rate': self.current_brain.getAverageFiringRate(),
                'e_i_ratio': self.current_brain.getExcitationInhibitionRatio(),
                'developmental_stage': self.current_brain.getDevelopmentalStage().value
            }

        # Get agent statistics
        agent_stats = {}
        if self.current_agent:
            agent_stats = {
                'curiosity_level': self.current_agent.getCuriosityLevel(),
                'novelty_level': self.current_agent.getNoveltyLevel(),
                'prediction_error': self.current_agent.getPredictionError(),
                'neuromodulation_level': self.current_agent.getNeuromodulationLevel(),
                'developmental_stage': self.current_agent.getDevelopmentalStage().value,
                'reward_modulation_enabled': self.current_agent.isRewardModulationEnabled(),
                'structural_plasticity_enabled': self.current_agent.isStructuralPlasticityEnabled(),
                'development_enabled': self.current_agent.isDevelopmentEnabled(),
                'curiosity_enabled': self.current_agent.isCuriosityEnabled()
            }

        # Get performance statistics
        performance_stats = self.performance_monitor.get_statistics()

        results = {
            'experiment_name': experiment.name,
            'status': 'completed',
            'steps_completed': steps_completed,
            'duration_seconds': duration,
            'timestep': experiment.timestep,
            'simulation_start_time': self.simulation_start_time,
            'simulation_end_time': end_time,
            'brain_statistics': brain_stats,
            'agent_statistics': agent_stats,
            'performance_statistics': performance_stats,
            'agent_profile': {
                'name': experiment.agent_profile.name,
                'type': experiment.agent_profile.type.value,
                'curiosity_threshold': experiment.agent_profile.curiosity_threshold,
                'exploration_rate': experiment.agent_profile.exploration_rate
            },
            'environment_profile': {
                'width': experiment.environment_profile.width,
                'height': experiment.environment_profile.height,
                'vision_range': experiment.environment_profile.vision_range,
                'complexity': experiment.environment_profile.complexity
            }
        }

        return results

    def _trigger_callbacks(self, event_type: str, data: Dict):
        """Trigger all callbacks for an event."""
        if event_type in self.callbacks:
            for callback in self.callbacks[event_type]:
                try:
                    callback(event_type, data)
                except Exception as e:
                    logger.error(f"Callback error for {event_type}: {e}")

    def _save_checkpoint(self, step: int, experiment: ExperimentSetup):
        """Save simulation checkpoint."""
        try:
            if self.current_brain and experiment.output_dir:
                checkpoint_dir = Path(experiment.output_dir) / "checkpoints"
                checkpoint_dir.mkdir(parents=True, exist_ok=True)

                checkpoint_file = checkpoint_dir / f"checkpoint_step_{step}.bin"
                self.current_brain.save(str(checkpoint_file))
                logger.debug(f"Checkpoint saved: {checkpoint_file}")

        except Exception as e:
            logger.debug(f"Failed to save checkpoint: {e}")

    def _save_experiment_results(self, results: Dict, experiment: ExperimentSetup):
        """Save experiment results to files."""
        try:
            output_dir = Path(experiment.output_dir)
            output_dir.mkdir(parents=True, exist_ok=True)

            # Save JSON results
            json_file = output_dir / f"{experiment.name}_results.json"
            with open(json_file, 'w') as f:
                json.dump(results, f, indent=2, default=str)

            # Save performance history
            if hasattr(self, 'performance_monitor'):
                history_file = output_dir / f"{experiment.name}_performance.json"
                with open(history_file, 'w') as f:
                    json.dump(self.performance_monitor.get_history(), f, indent=2, default=str)

            logger.info(f"Experiment results saved to: {output_dir}")

        except Exception as e:
            logger.error(f"Failed to save experiment results: {e}")

    def _create_mock_agent(self, profile: AgentProfile) -> Any:
        """Create a mock agent for testing when pynlm is not available."""
        class MockBrain:
            def __init__(self):
                self.neuron_count = profile.type.value * 300
                self.synapse_count = self.neuron_count * 3
                self.spike_count = 0
                self.firing_count = 0

            def initialize(self):
                pass

            def step(self, step):
                self.spike_count += 1
                self.firing_count = self.neuron_count * 0.3

            def getTotalNeuronCount(self):
                return self.neuron_count

            def getTotalSynapseCount(self):
                return self.synapse_count

            def getFiringNeuronCount(self):
                return int(self.firing_count)

            def getTotalSpikeCount(self):
                return self.spike_count

            def getAverageFiringRate(self):
                return 5.0

            def getExcitationInhibitionRatio(self):
                return 1.2

            def getDevelopmentalStage(self):
                class MockStage:
                    def __init__(self):
                        self.value = "Maturation"
                return MockStage()

            def save(self, filepath):
                pass

            def load(self, filepath):
                pass

        class MockAgent:
            def __init__(self):
                self.reward_modulation_enabled = profile.type in [AgentType.LEARNER, AgentType.ADAPTIVE, AgentType.SOCIAL, AgentType.CREATIVE]
                self.structural_plasticity_enabled = profile.type in [AgentType.ADAPTIVE, AgentType.CREATIVE]
                self.development_enabled = profile.type in [AgentType.ADAPTIVE, AgentType.CREATIVE]
                self.curiosity_enabled = profile.type in [AgentType.EXPLORER, AgentType.ADAPTIVE, AgentType.SOCIAL, AgentType.CREATIVE]

            def initialize(self, world):
                pass

            def processSensoryInput(self, percept):
                pass

            def decodeMotorCommand(self):
                return "MOVE_FORWARD"

            def applyRewardModulation(self, reward, predicted):
                pass

            def updateDevelopment(self, timestep):
                pass

            def getCuriosityLevel(self):
                return profile.curiosity_threshold

            def getNoveltyLevel(self):
                return profile.curiosity_threshold * 0.7

            def getPredictionError(self):
                return 0.1

            def getNeuromodulationLevel(self):
                return 0.2

            def getDevelopmentalStage(self):
                class MockStage:
                    def __init__(self):
                        self.value = "Maturation"
                return MockStage()

            def enableRewardModulation(self, enable):
                self.reward_modulation_enabled = enable

            def enableStructuralPlasticity(self, enable):
                self.structural_plasticity_enabled = enable

            def enableDevelopment(self, enable):
                self.development_enabled = enable

            def enableCuriosity(self, enable):
                self.curiosity_enabled = enable

            def isRewardModulationEnabled(self):
                return self.reward_modulation_enabled

            def isStructuralPlasticityEnabled(self):
                return self.structural_plasticity_enabled

            def isDevelopmentEnabled(self):
                return self.development_enabled

            def isCuriosityEnabled(self):
                return self.curiosity_enabled

        return MockBrain()

    def _create_mock_world(self, profile: EnvironmentProfile) -> Any:
        """Create a mock world for testing when pynlm is not available."""
        class MockWorld:
            def __init__(self):
                self.width = profile.width
                self.height = profile.height
                self.vision_width = profile.vision_range
                self.vision_height = profile.vision_range
                self.simulation_time = 0.0

            def configure(self, width, height, visionWidth, visionHeight):
                self.width = width
                self.height = height
                self.vision_width = visionWidth
                self.vision_height = visionHeight

            def reset(self):
                self.simulation_time = 0.0

            def update(self, timestep):
                self.simulation_time += timestep

            def applyMotorCommand(self, cmd, time):
                pass

            def getSensoryPercept(self):
                class MockPercept:
                    def getVision(self):
                        return [0.1] * (self.vision_width * self.vision_height)

                    def getTouch(self):
                        return [0.0] * 8

                    def getInternal(self):
                        return [0.0] * 4

                    def getProprioception(self):
                        return [0.0] * 6

                return MockPercept()

            def getAgentBody(self):
                class MockBody:
                    def __init__(self):
                        self.x = 0.0
                        self.y = 0.0
                        self.energy = 1.0

                return MockBody()

        return MockWorld()

    def _create_mock_agent_interface(self, brain, world, profile: AgentProfile) -> Any:
        """Create a mock agent interface for testing."""
        return MockAgent()

    def get_current_status(self) -> Dict:
        """Get current system status."""
        return {
            'current_experiment': self.current_experiment.name if self.current_experiment else None,
            'step_count': self.step_count,
            'current_mode': self.current_mode.value,
            'performance_monitor_active': self.performance_monitor.monitoring,
            'brain_available': self.current_brain is not None,
            'world_available': self.current_world is not None,
            'agent_available': self.current_agent is not None
        }

    def pause_simulation(self):
        """Pause the current simulation."""
        self.current_mode = SimulationMode.PAUSED
        logger.info("Simulation paused")

    def resume_simulation(self):
        """Resume the current simulation."""
        self.current_mode = SimulationMode.RUNNING
        logger.info("Simulation resumed")

    def stop_simulation(self):
        """Stop the current simulation."""
        self.current_mode = SimulationMode.STOPPED
        self.performance_monitor.stop_monitoring()
        logger.info("Simulation stopped")

    def reset_system(self):
        """Reset the entire system."""
        self.current_experiment = None
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.current_mode = SimulationMode.BATCH
        self.step_count = 0
        self.simulation_start_time = None
        self.performance_monitor.stop_monitoring()
        logger.info("System reset")

    def export_results(self, format: str = DataFormat.JSON) -> str:
        """Export experiment results in specified format."""
        if not self.experiment_results:
            return "No results to export"

        try:
            if format == DataFormat.JSON:
                return json.dumps(self.experiment_results, indent=2, default=str)
            elif format == DataFormat.CSV:
                import csv
                import io

                output = io.StringIO()
                writer = csv.writer(output)

                # Write headers
                writer.writerow(['experiment', 'steps', 'duration', 'neurons', 'firing_rate', 'curiosity'])

                # Write data
                for result in self.experiment_results:
                    writer.writerow([
                        result.get('experiment_name', ''),
                        result.get('steps_completed', 0),
                        result.get('duration_seconds', 0),
                        result.get('brain_statistics', {}).get('total_neurons', 0),
                        result.get('brain_statistics', {}).get('average_firing_rate', 0),
                        result.get('agent_statistics', {}).get('curiosity_level', 0)
                    ])

                return output.getvalue()
            else:
                raise ValueError(f"Unsupported format: {format}")

        except Exception as e:
            logger.error(f"Export failed: {e}")
            return f"Export failed: {e}"

    def save_results(self, filepath: str):
        """Save experiment results to file."""
        try:
            with open(filepath, 'w') as f:
                f.write(self.export_results())
            logger.info(f"Results saved to: {filepath}")
        except Exception as e:
            logger.error(f"Failed to save results: {e}")

    def load_results(self, filepath: str):
        """Load experiment results from file."""
        try:
            with open(filepath, 'r') as f:
                results = json.load(f)

            if isinstance(results, list):
                self.experiment_results.extend(results)
            else:
                self.experiment_results.append(results)

            logger.info(f"Results loaded from: {filepath}")

        except Exception as e:
            logger.error(f"Failed to load results: {e}")

    def print_status(self):
        """Print current system status."""
        status = self.get_current_status()

        print("\n" + "="*60)
        print("NLM INTERACTIVE AGENT SYSTEM STATUS")
        print("="*60)

        if status['current_experiment']:
            print(f"Current Experiment: {status['current_experiment']}")

        print(f"Step Count: {status['step_count']}")
        print(f"Mode: {status['current_mode']}")
        print(f"Brain Available: {status['brain_available']}")
        print(f"World Available: {status['world_available']}")
        print(f"Agent Available: {status['agent_available']}")
        print(f"Performance Monitor Active: {status['performance_monitor_active']}")

        if status['performance_monitor_active']:
            perf_stats = self.performance_monitor.get_statistics()
            print(f"\nPerformance Stats:")
            print(f"  Duration: {perf_stats.get('duration', 0):.2f}s")
            print(f"  Memory Avg: {perf_stats.get('memory_avg_mb', 0):.2f} MB")
            print(f"  CPU Avg: {perf_stats.get('cpu_avg_percent', 0):.1f}%")

        print("="*60 + "\n")

    def print_available_commands(self):
        """Print available CLI commands."""
        print("\n" + "="*60)
        print("AVAILABLE COMMANDS")
        print("="*60)

        commands = [
            ("create <name>", "Create a new experiment setup"),
            ("run", "Run the current experiment"),
            ("series", "Run a series of experiments"),
            ("pause", "Pause the current simulation"),
            ("resume", "Resume the paused simulation"),
            ("stop", "Stop the current simulation"),
            ("reset", "Reset the entire system"),
            ("status", "Print current system status"),
            ("help", "Show this help message"),
            ("quit", "Exit the system"),
            ("save <filepath>", "Save results to file"),
            ("load <filepath>", "Load results from file"),
            ("export <format>", "Export results in specified format"),
            ("add-callback <event> <function>", "Add a callback for events"),
            ("remove-callback <event> <function>", "Remove a callback"),
            ("show-experiments", "List saved experiments"),
            ("show-results", "Show experiment results")
        ]

        for cmd, desc in commands:
            print(f"{cmd:<25} - {desc}")

        print("="*60 + "\n")

    def interactive_session(self):
        """Start an interactive CLI session."""
        print("NLM Interactive Agent System")
        print("="*60)
        print("Type 'help' for available commands or 'quit' to exit.")

        while True:
            try:
                command = input("\n> ").strip()

                if not command:
                    continue

                parts = command.split()
                cmd = parts[0].lower()
                args = parts[1:]

                if cmd == 'help':
                    self.print_available_commands()

                elif cmd == 'create':
                    self._handle_create_command(args)

                elif cmd == 'run':
                    self._handle_run_command(args)

                elif cmd == 'series':
                    self._handle_series_command(args)

                elif cmd == 'pause':
                    self.pause_simulation()

                elif cmd == 'resume':
                    self.resume_simulation()

                elif cmd == 'stop':
                    self.stop_simulation()

                elif cmd == 'reset':
                    self.reset_system()

                elif cmd == 'status':
                    self.print_status()

                elif cmd == 'save':
                    if args:
                        self.save_results(args[0])
                    else:
                        print("Usage: save <filepath>")

                elif cmd == 'load':
                    if args:
                        self.load_results(args[0])
                    else:
                        print("Usage: load <filepath>")

                elif cmd == 'export':
                    if args:
                        try:
                            format_type = DataFormat(args[0])
                            print(self.export_results(format_type))
                        except ValueError:
                            print(f"Unsupported format: {args[0]}. Use: {[f.value for f in DataFormat]}")
                    else:
                        print("Usage: export <format>")

                elif cmd == 'quit':
                    print("Exiting...")
                    break

                elif cmd == 'show-experiments':
                    self._show_experiments()

                elif cmd == 'show-results':
                    self._show_results()

                elif cmd == 'add-callback':
                    self._handle_add_callback_command(args)

                elif cmd == 'remove-callback':
                    self._handle_remove_callback_command(args)

                else:
                    print(f"Unknown command: {cmd}. Type 'help' for available commands.")

            except KeyboardInterrupt:
                print("\nInterrupted. Type 'quit' to exit or continue.")
            except Exception as e:
                print(f"Error: {e}")

    def _run_demo(self):
        """Run a demonstration of the system."""
        print("\nRunning demonstration mode...")
        print("This would demonstrate various agent types and behaviors.")
        print("For now, please run in interactive mode or use the advanced_agent_controller.py script.")

    def _handle_create_command(self, args):
        """Handle create command."""
        if len(args) < 1:
            print("Usage: create <experiment_name>")
            return

        experiment_name = args[0]

        # Default profiles for different agent types
        agent_types = {
            'explorer': AgentProfile(name=f"{experiment_name}_explorer", type=AgentType.EXPLORER),
            'learner': AgentProfile(name=f"{experiment_name}_learner", type=AgentType.LEARNER),
            'adaptive': AgentProfile(name=f"{experiment_name}_adaptive", type=AgentType.ADAPTIVE),
            'social': AgentProfile(name=f"{experiment_name}_social", type=AgentType.SOCIAL),
            'creative': AgentProfile(name=f"{experiment_name}_creative", type=AgentType.CREATIVE)
        }

        # Ask which agent type to use
        print(f"Available agent types: {', '.join([a.value for a in AgentType])}")
        agent_type_input = input("Enter agent type (default: learner): ").strip() or "learner"

        try:
            agent_type = AgentType(agent_type_input)
            self.current_experiment = ExperimentSetup(
                name=experiment_name,
                description=f"Experiment created for {experiment_name}",
                agent_profile=agent_types.get(agent_type_input, agent_types['learner']),
                environment_profile=EnvironmentProfile(),
                duration=500.0,
                timestep=0.1,
                max_steps=5000,
                save_frequency=100,
                output_dir=f"experiments/{experiment_name}"
            )
            print(f"Created experiment: {self.current_experiment.name}")
            print(f"Agent: {self.current_experiment.agent_profile.name} ({self.current_experiment.agent_profile.type.value})")
            print(f"Environment: {self.current_experiment.environment_profile.width}x{self.current_experiment.environment_profile.height}")

        except ValueError:
            print(f"Invalid agent type: {agent_type_input}")

    def _handle_run_command(self, args):
        """Handle run command."""
        if not self.current_experiment:
            print("No experiment created. Use 'create <name>' first.")
            return

        if input(f"Run experiment '{self.current_experiment.name}'? (y/n): ").strip().lower() != 'y':
            print("Experiment not run.")
            return

        result = self.run_advanced_simulation(self.current_experiment)
        self.experiment_results.append(result)

        if result['status'] == 'completed':
            print(f"Experiment completed successfully!")
            print(f"Steps: {result['steps_completed']}")
            print(f"Duration: {result['duration_seconds']:.2f}s")
            print(f"Average firing rate: {result['brain_statistics']['average_firing_rate']:.2f}")
            print(f"Final curiosity: {result['agent_statistics']['curiosity_level']:.3f}")

            # Ask to save
            if input("Save results? (y/n): ").strip().lower() == 'y':
                save_path = input("Save path (default: experiments/): ").strip() or "experiments"
                self.save_results(f"{save_path}/{self.current_experiment.name}_results.json")
        else:
            print(f"Experiment failed: {result.get('error', 'Unknown error')}")

    def _handle_series_command(self, args):
        """Handle series command."""
        if len(args) < 1:
            print("Usage: series <number_of_experiments>")
            return

        try:
            num_experiments = int(args[0])
            experiments = []

            for i in range(num_experiments):
                exp_name = f"series_experiment_{i+1}"
                agent_type = AgentType.EXPLORER if i % 2 == 0 else AgentType.LEARNER
                agent_profile = AgentProfile(name=f"{exp_name}_{agent_type.value}", type=agent_type)

                experiment = ExperimentSetup(
                    name=exp_name,
                    agent_profile=agent_profile,
                    duration=200.0,
                    timestep=0.1,
                    max_steps=2000,
                    output_dir=f"experiments/series"
                )
                experiments.append(experiment)

            if input(f"Run {num_experiments} experiments? (y/n): ").strip().lower() != 'y':
                print("Series not run.")
                return

            results = self.run_experiment_series(experiments)
            self.experiment_results.extend(results)

            print(f"Series completed! {len(results)} experiments run.")

        except ValueError:
            print("Invalid number of experiments")

    def _handle_add_callback_command(self, args):
        """Handle add-callback command."""
        if len(args) < 2:
            print("Usage: add-callback <event> <function_name>")
            print("Available events: step_start, step_end, episode_start, episode_end,")
            print("                   simulation_start, simulation_end")
            return

        event_type = args[0]
        function_name = args[1]

        # Create a simple callback function
        def simple_callback(event, data):
            print(f"Callback triggered: {event} at step {data.get('step', 'unknown')}")

        try:
            self.add_callback(event_type, simple_callback)
            print(f"Added callback for {event_type}")
        except Exception as e:
            print(f"Failed to add callback: {e}")

    def _handle_remove_callback_command(self, args):
        """Handle remove-callback command."""
        if len(args) < 2:
            print("Usage: remove-callback <event> <function_name>")
            return

        event_type = args[0]
        function_name = args[1]

        # Find and remove callback (simplified implementation)
        callbacks = self.callbacks.get(event_type, [])
        if callbacks:
            self.callbacks[event_type] = [cb for cb in callbacks if cb.__name__ != function_name]
            print(f"Removed callback for {event_type}")
        else:
            print(f"No callbacks found for {event_type}")

    def _show_experiments(self):
        """Show saved experiments."""
        try:
            experiment_dir = Path("experiments")
            if not experiment_dir.exists():
                print("No experiments directory found.")
                return

            print("\nSaved experiments:")
            for item in experiment_dir.iterdir():
                if item.is_dir():
                    print(f"  {item.name}/")
                    for file in item.glob("*.json"):
                        print(f"    {file.name}")

        except Exception as e:
            print(f"Failed to list experiments: {e}")

    def _show_results(self):
        """Show experiment results."""
        if not self.experiment_results:
            print("No results available.")
            return

        print(f"\nExperiment Results ({len(self.experiment_results)} experiments):")
        print("-" * 80)

        for i, result in enumerate(self.experiment_results):
            print(f"Experiment {i+1}: {result.get('experiment_name', 'Unknown')}")
            print(f"  Status: {result.get('status', 'Unknown')}")
            print(f"  Steps: {result.get('steps_completed', 0)}")
            print(f"  Duration: {result.get('duration_seconds', 0):.2f}s")

            if result.get('brain_statistics'):
                brain = result['brain_statistics']
                print(f"  Neurons: {brain.get('total_neurons', 0)}")
                print(f"  Firing rate: {brain.get('average_firing_rate', 0):.2f}")

            if result.get('agent_statistics'):
                agent = result['agent_statistics']
                print(f"  Curiosity: {agent.get('curiosity_level', 0):.3f}")

            print()


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Interactive Agent System - Advanced NLM simulation platform"
    )
    parser.add_argument(
        '--output-dir', '-o',
        default="nlm_experiments",
        help='Output directory for experiments (default: nlm_experiments)'
    )
    parser.add_argument(
        '--headless', '-h',
        action='store_true',
        help='Run in headless mode (no interactive CLI)'
    )
    parser.add_argument(
        '--demo', '-d',
        action='store_true',
        help='Run a demonstration with example configurations'
    )

    args = parser.parse_args()

    print("NLM Interactive Agent System")
    print("="*60)
    print(f"Output directory: {args.output_dir}")
    print(f"Mode: {'Headless' if args.headless else 'Interactive'}")
    print(f"Demo: {'Enabled' if args.demo else 'Disabled'}")
    print("="*60)

    # Create interactive system
    system = InteractiveAgentSystem(output_dir=args.output_dir)

    if args.headless:
        # Run demonstration or basic tests
        if args.demo:
            print("\nRunning demonstration mode...")
            system._run_demo()
        else:
            print("\nRunning in headless mode - no interactive commands available.")
            print("Add --demo flag to run demonstration.")
    else:
        # Start interactive session
        system.interactive_session()


if __name__ == "__main__":
    main()
