#!/usr/bin/env python3
"""
NLM Advanced Agent Controller

An advanced tool for controlling and managing NLM (Neural Learning Machine) simulations.
Provides enhanced functionality, debugging tools, and advanced agent behaviors.

Features:
- Advanced agent configuration and control
- Simulation monitoring and debugging
- Experiment management
- Performance profiling
- Advanced agent behaviors
"""

import json
import time
import subprocess
import sys
import threading
from pathlib import Path
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
import logging

# Try to import NLM (will work after build)
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Advanced features will use mock implementations.")

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
class SimulationState(Enum):
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    ERROR = "error"
class AgentMode(Enum):
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    CONSOLIDATION = "consolidation"
    ADAPTATION = "adaptation"
    CUSTOM = "custom"
@dataclass
class ExperimentConfig:
    """Configuration for NLM experiments"""
    name: str = "default_experiment"
    brain_config: Dict = field(default_factory=dict)
    world_config: Dict = field(default_factory=dict)
    agent_config: Dict = field(default_factory=dict)
    duration: float = 1000.0
    timestep: float = 0.1
    save_frequency: int = 100
    log_frequency: int = 50
    random_seed: int = 42
    checkpoint_path: Optional[str] = None
    tags: List[str] = field(default_factory=list)
    metadata: Dict = field(default_factory=dict)
@dataclass
class PerformanceMetrics:
    """Performance metrics for simulation monitoring"""
    simulation_time: float = 0.0
    steps_per_second: float = 0.0
    memory_usage_mb: float = 0.0
    cpu_usage_percent: float = 0.0
    energy_consumption: float = 0.0
    spike_rate: float = 0.0
    synaptic_activity: float = 0.0
    learning_rate: float = 0.0
    exploration_rate: float = 0.0
class AdvancedAgentController:
    """
    Advanced controller for NLM simulations with enhanced functionality.
    Provides better control, monitoring, and advanced agent behaviors.
    """

    def __init__(self, log_file: Optional[str] = None):
        """Initialize the advanced agent controller."""
        self.state = SimulationState.IDLE
        self.agent_mode = AgentMode.EXPLORATION
        self.config = ExperimentConfig()
        self.metrics = PerformanceMetrics()
        self.step_count = 0
        self.start_time = None
        self.checkpoint_interval = self.config.save_frequency
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.callbacks: Dict[str, List[Callable]] = {}
        self.performance_history: List[PerformanceMetrics] = []
        self.action_history: List[Dict] = []
        self.observation_history: List[Dict] = []

        # Setup logging
        if log_file:
            file_handler = logging.FileHandler(log_file)
            file_handler.setLevel(logging.DEBUG)
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            file_handler.setFormatter(formatter)
            logger.addHandler(file_handler)

        logger.info("AdvancedAgentController initialized")

    def create_enhanced_brain(self, config_dict: Optional[Dict] = None) -> Any:
        """
        Create an enhanced brain configuration with advanced settings.

        Args:
            config_dict: Optional dictionary with configuration parameters

        Returns:
            Configured brain object
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock configuration")
            return self._create_mock_brain(config_dict)

        try:
            # Create base config
            config = pynlm.createDefaultConfig()

            # Apply enhanced configuration
            enhanced_config = config_dict or {}

            # Advanced brain settings
            if "neuron_count" in enhanced_config:
                config.set("brain.neuron_count", enhanced_config["neuron_count"])

            if "synapse_density" in enhanced_config:
                config.set("brain.synapse_density", enhanced_config["synapse_density"])

            if "connection_probability" in enhanced_config:
                config.set("brain.connection_probability", enhanced_config["connection_probability"])

            # Advanced plasticity settings
            if "plasticity" in enhanced_config:
                plasticity_config = enhanced_config["plasticity"]
                for key, value in plasticity_config.items():
                    config.set(f"plasticity.{key}", value)

            # Neuromodulation settings
            if "neuromod" in enhanced_config:
                neuromod_config = enhanced_config["neuromod"]
                for key, value in neuromod_config.items():
                    config.set(f"neuromod.{key}", value)

            # Development settings
            if "development" in enhanced_config:
                dev_config = enhanced_config["development"]
                for key, value in dev_config.items():
                    config.set(f"development.{key}", value)

            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()

            logger.info(f"Enhanced brain created with {brain.getTotalNeuronCount()} neurons")
            return brain

        except Exception as e:
            logger.error(f"Failed to create enhanced brain: {e}")
            return self._create_mock_brain(config_dict)

    def create_advanced_world(self, config_dict: Optional[Dict] = None) -> Any:
        """
        Create an advanced world simulation with enhanced features.

        Args:
            config_dict: Optional dictionary with world configuration

        Returns:
            Configured world object
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock world")
            return self._create_mock_world(config_dict)

        try:
            world = pynlm.createSimpleWorld()

            # Advanced world configuration
            enhanced_config = config_dict or {}

            # Environment dimensions
            if "width" in enhanced_config:
                width = enhanced_config["width"]
            else:
                width = enhanced_config.get("size", 20)

            if "height" in enhanced_config:
                height = enhanced_config["height"]
            else:
                height = enhanced_config.get("size", 20)

            if "visionWidth" in enhanced_config:
                vision_width = enhanced_config["visionWidth"]
            else:
                vision_width = enhanced_config.get("vision_size", 8)

            if "visionHeight" in enhanced_config:
                vision_height = enhanced_config["visionHeight"]
            else:
                vision_height = enhanced_config.get("vision_size", 8)

            world.configure(width=width, height=height,
                           visionWidth=vision_width, visionHeight=vision_height)

            # Advanced world settings
            if "max_energy" in enhanced_config:
                world.setMaxEnergy(enhanced_config["max_energy"])

            if "energy_decay_rate" in enhanced_config:
                world.setEnergyDecayRate(enhanced_config["energy_decay_rate"])

            if "random_seed" in enhanced_config:
                world.setRandomSeed(enhanced_config["random_seed"])

            # Add environmental objects if specified
            if "objects" in enhanced_config:
                for obj_config in enhanced_config["objects"]:
                    # Create world object (simplified - would need full object creation)
                    x = obj_config.get("x", 0.0)
                    y = obj_config.get("y", 0.0)
                    obj_type = obj_config.get("type", "Resource")
                    value = obj_config.get("value", 1.0)

                    # Note: This is a simplified version - full implementation would need
                    # to create proper world objects from pynlm
                    logger.debug(f"Would add object at ({x}, {y}) type {obj_type} value {value}")

            logger.info(f"Advanced world created: {width}x{height} with {vision_width}x{vision_height} vision")
            return world

        except Exception as e:
            logger.error(f"Failed to create advanced world: {e}")
            return self._create_mock_world(config_dict)

    def create_advanced_agent(self, brain: Any, world: Any, config_dict: Optional[Dict] = None) -> Any:
        """
        Create an advanced agent with enhanced behaviors and capabilities.

        Args:
            brain: The brain object
            world: The world object
            config_dict: Optional dictionary with agent configuration

        Returns:
            Configured agent object
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock agent")
            return self._create_mock_agent(brain, world, config_dict)

        try:
            # Create agent brain interface
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)

            # Enhanced agent configuration
            enhanced_config = config_dict or {}

            # Enable advanced agent features
            agent_features = enhanced_config.get("features", {})

            if agent_features.get("reward_modulation", True):
                agent.enableRewardModulation(True)

            if agent_features.get("structural_plasticity", True):
                agent.enableStructuralPlasticity(True)

            if agent_features.get("development", True):
                agent.enableDevelopment(True)

            if agent_features.get("curiosity", True):
                agent.enableCuriosity(True)

            # Advanced agent behaviors
            agent_mode_config = enhanced_config.get("mode", {})
            self.agent_mode = AgentMode(agent_mode_config.get("name", "exploration"))

            # Store configuration
            self.current_agent = agent
            self.current_brain = brain
            self.current_world = world

            logger.info(f"Advanced agent created with mode: {self.agent_mode.value}")
            return agent

        except Exception as e:
            logger.error(f"Failed to create advanced agent: {e}")
            return self._create_mock_agent(brain, world, config_dict)

    def run_enhanced_simulation(self, duration: Optional[float] = None, headless: bool = True) -> Dict:
        """
        Run an enhanced simulation with monitoring and advanced features.

        Args:
            duration: Optional duration to run (overrides config duration)
            headless: If True, run without GUI output

        Returns:
            Dictionary with simulation results and metrics
        """
        if not PYNLM_AVAILABLE:
            logger.warning("pynlm not available, using mock simulation")
            return self._run_mock_simulation(duration, headless)

        try:
            # Setup simulation
            self.start_time = time.time()
            self.state = SimulationState.RUNNING

            simulation_duration = duration or self.config.duration
            timestep = self.config.timestep
            steps = int(simulation_duration / timestep)

            logger.info(f"Starting enhanced simulation: {steps} steps, {simulation_duration}s duration")

            # Clear history
            self.action_history.clear()
            self.observation_history.clear()
            self.performance_history.clear()

            # Run simulation with enhanced monitoring
            for step in range(steps):
                if self.state == SimulationState.PAUSED:
                    time.sleep(0.1)
                    continue

                if self.state == SimulationState.COMPLETED:
                    break

                step_start_time = time.time()

                # Update world
                self.current_world.update(timestep)

                # Get sensory percept
                percept = self.current_world.getSensoryPercept()

                # Process sensory input
                self.current_agent.processSensoryInput(percept)

                # Record observation
                observation = {
                    "step": step,
                    "time": step * timestep,
                    "percept": percept,
                    "agent_state": self._get_agent_state()
                }
                self.observation_history.append(observation)

                # Run brain step
                self.current_brain.step(step)

                # Get action from agent
                action = self.current_agent.decodeMotorCommand()

                # Record action
                recorded_action = {
                    "step": step,
                    "time": step * timestep,
                    "action": action,
                    "agent_state": self._get_agent_state()
                }
                self.action_history.append(recorded_action)

                # Apply action to world
                self.current_world.applyMotorCommand(action, step * timestep)

                # Apply reward modulation if enabled
                if self.current_agent.isRewardModulationEnabled():
                    # Simple reward based on internal signals
                    reward = 0.0
                    if percept.getInternal():
                        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                    self.current_agent.applyRewardModulation(reward, 0.0)

                # Update development if enabled
                if self.current_agent.isDevelopmentEnabled():
                    self.current_agent.updateDevelopment(timestep)

                # Update performance metrics
                self._update_performance_metrics(step, timestep)

                # Log progress periodically
                if step % self.config.log_frequency == 0 and not headless:
                    self._log_simulation_progress(step, steps)

                # Save checkpoint periodically
                if step % self.checkpoint_interval == 0:
                    self._save_checkpoint(step)

                # Trigger callbacks
                self._trigger_callbacks("step", {
                    "step": step,
                    "time": step * timestep,
                    "action": action,
                    "percept": percept,
                    "metrics": self.metrics
                })

            # Simulation completed
            self.state = SimulationState.COMPLETED
            end_time = time.time()

            # Calculate final metrics
            total_time = end_time - self.start_time
            final_metrics = self._calculate_final_metrics(total_time, steps)

            # Save final results
            results = {
                "status": "completed",
                "steps": steps,
                "duration": total_time,
                "agent_mode": self.agent_mode.value,
                "final_metrics": final_metrics,
                "performance_history": self.performance_history,
                "action_history": self.action_history,
                "observation_history": self.observation_history,
                "config": self.config.__dict__
            }

            logger.info(f"Enhanced simulation completed: {steps} steps in {total_time:.2f}s")
            return results

        except Exception as e:
            logger.error(f"Enhanced simulation failed: {e}")
            self.state = SimulationState.ERROR
            return {
                "status": "error",
                "error": str(e),
                "steps_completed": self.step_count
            }

    def run_experiment(self, experiment_config: ExperimentConfig) -> Dict:
        """
        Run a complete experiment with the given configuration.

        Args:
            experiment_config: Experiment configuration

        Returns:
            Dictionary with experiment results
        """
        try:
            logger.info(f"Starting experiment: {experiment_config.name}")

            # Update configuration
            self.config = experiment_config

            # Create enhanced components
            brain = self.create_enhanced_brain(experiment_config.brain_config)
            world = self.create_advanced_world(experiment_config.world_config)
            agent = self.create_advanced_agent(brain, world, experiment_config.agent_config)

            # Run simulation
            results = self.run_enhanced_simulation(experiment_config.duration)

            # Add experiment metadata
            results["experiment_name"] = experiment_config.name
            results["tags"] = experiment_config.tags
            results["metadata"] = experiment_config.metadata

            logger.info(f"Experiment completed: {experiment_config.name}")
            return results

        except Exception as e:
            logger.error(f"Experiment failed: {e}")
            return {
                "status": "error",
                "experiment_name": experiment_config.name,
                "error": str(e)
            }

    def add_callback(self, event_type: str, callback: Callable):
        """
        Add a callback function for simulation events.

        Args:
            event_type: Type of event ('step', 'start', 'end', 'error')
            callback: Function to call when event occurs
        """
        if event_type not in self.callbacks:
            self.callbacks[event_type] = []
        self.callbacks[event_type].append(callback)
        logger.debug(f"Added callback for event: {event_type}")

    def remove_callback(self, event_type: str, callback: Callable):
        """
        Remove a callback function.

        Args:
            event_type: Type of event
            callback: Function to remove
        """
        if event_type in self.callbacks:
            try:
                self.callbacks[event_type].remove(callback)
                logger.debug(f"Removed callback for event: {event_type}")
            except ValueError:
                pass

    def _get_agent_state(self) -> Dict:
        """Get current agent state for recording."""
        if not self.current_agent or not PYNLM_AVAILABLE:
            return {}

        try:
            state = {
                "curiosity": self.current_agent.getCuriosityLevel(),
                "novelty": self.current_agent.getNoveltyLevel(),
                "prediction_error": self.current_agent.getPredictionError(),
                "neuromodulation": self.current_agent.getNeuromodulationLevel(),
                "developmental_stage": self.current_agent.getDevelopmentalStage().value
            }
            return state
        except:
            return {}

    def _update_performance_metrics(self, step: int, timestep: float):
        """Update performance metrics for current step."""
        if not PYNLM_AVAILABLE or not self.current_brain:
            return

        try:
            # Get brain metrics
            self.metrics.steps_per_second = 1.0 / timestep
            self.metrics.spike_rate = self.current_brain.getFiringNeuronCount() / max(1, step)
            self.metrics.synaptic_activity = self.current_brain.getTotalSynapseCount() / max(1, step)
            self.metrics.energy_consumption = step * 0.001  # Mock calculation

            # Get agent metrics
            if self.current_agent:
                self.metrics.exploration_rate = self.current_agent.getCuriosityLevel()
                self.metrics.learning_rate = abs(self.current_agent.getPredictionError())

            # Record metrics
            self.performance_history.append(self.metrics.copy())

        except Exception as e:
            logger.debug(f"Failed to update metrics: {e}")

    def _calculate_final_metrics(self, total_time: float, steps: int) -> PerformanceMetrics:
        """Calculate final performance metrics."""
        final_metrics = PerformanceMetrics()
        final_metrics.simulation_time = total_time
        final_metrics.steps_per_second = steps / total_time if total_time > 0 else 0

        # Calculate averages from history
        if self.performance_history:
            avg_metrics = PerformanceMetrics()
            count = len(self.performance_history)

            for metrics in self.performance_history:
                avg_metrics.simulation_time += metrics.simulation_time
                avg_metrics.steps_per_second += metrics.steps_per_second
                avg_metrics.spike_rate += metrics.spike_rate
                avg_metrics.synaptic_activity += metrics.synaptic_activity
                avg_metrics.energy_consumption += metrics.energy_consumption
                avg_metrics.exploration_rate += metrics.exploration_rate
                avg_metrics.learning_rate += metrics.learning_rate

            avg_metrics.simulation_time /= count
            avg_metrics.steps_per_second /= count
            avg_metrics.spike_rate /= count
            avg_metrics.synaptic_activity /= count
            avg_metrics.energy_consumption /= count
            avg_metrics.exploration_rate /= count
            avg_metrics.learning_rate /= count

            final_metrics = avg_metrics

        return final_metrics

    def _log_simulation_progress(self, step: int, total_steps: int):
        """Log simulation progress."""
        progress = (step / total_steps) * 100
        logger.info(f"Simulation progress: {step}/{total_steps} ({progress:.1f}%)")
        logger.info(f"Current metrics: spikes={self.current_brain.getFiringNeuronCount() if self.current_brain else 0}, "
                   f"curiosity={self.current_agent.getCuriosityLevel() if self.current_agent else 0:.3f}")

    def _save_checkpoint(self, step: int):
        """Save simulation checkpoint."""
        try:
            if self.current_brain and self.config.checkpoint_path:
                checkpoint_file = f"{self.config.checkpoint_path}_step_{step}.bin"
                self.current_brain.save(checkpoint_file)
                logger.debug(f"Checkpoint saved: {checkpoint_file}")
        except Exception as e:
            logger.debug(f"Failed to save checkpoint: {e}")

    def _trigger_callbacks(self, event_type: str, data: Dict):
        """Trigger all callbacks for an event."""
        if event_type in self.callbacks:
            for callback in self.callbacks[event_type]:
                try:
                    callback(event_type, data)
                except Exception as e:
                    logger.error(f"Callback error: {e}")

    def _create_mock_brain(self, config_dict: Optional[Dict]) -> Any:
        """Create a mock brain for testing when pynlm is not available."""
        class MockBrain:
            def __init__(self):
                self.total_neuron_count = config_dict.get("neuron_count", 100) if config_dict else 100
                self.total_synapse_count = 500
                self.spike_count = 0
                self.firing_count = 0

            def initialize(self):
                pass

            def step(self, step):
                self.spike_count += 1
                self.firing_count = self.total_neuron_count * 0.3

            def getTotalNeuronCount(self):
                return self.total_neuron_count

            def getTotalSynapseCount(self):
                return self.total_synapse_count

            def getFiringNeuronCount(self):
                return int(self.firing_count)

            def getTotalSpikeCount(self):
                return self.spike_count

            def save(self, filepath):
                pass

            def load(self, filepath):
                pass

        return MockBrain()

    def _create_mock_world(self, config_dict: Optional[Dict]) -> Any:
        """Create a mock world for testing when pynlm is not available."""
        class MockWorld:
            def __init__(self):
                self.width = config_dict.get("width", 20) if config_dict else 20
                self.height = config_dict.get("height", 20) if config_dict else 20
                self.vision_width = config_dict.get("visionWidth", 8) if config_dict else 8
                self.vision_height = config_dict.get("visionHeight", 8) if config_dict else 8
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

            def getWidth(self):
                return self.width

            def getHeight(self):
                return self.height

            def getSimulationTime(self):
                return self.simulation_time

        return MockWorld()

    def _create_mock_agent(self, brain, world, config_dict: Optional[Dict]) -> Any:
        """Create a mock agent for testing when pynlm is not available."""
        class MockAgent:
            def __init__(self):
                self.reward_modulation_enabled = True
                self.structural_plasticity_enabled = True
                self.development_enabled = True
                self.curiosity_enabled = True

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
                return 0.5

            def getNoveltyLevel(self):
                return 0.3

            def getPredictionError(self):
                return 0.1

            def getNeuromodulationLevel(self):
                return 0.2

            def getDevelopmentalStage(self):
                class MockStage:
                    def __init__(self):
                        self.value = "Initial"

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

        return MockAgent()

    def save_results(self, results: Dict, filepath: str):
        """Save simulation results to a file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2, default=str)
            logger.info(f"Results saved to: {filepath}")
        except Exception as e:
            logger.error(f"Failed to save results: {e}")

    def load_results(self, filepath: str) -> Dict:
        """Load simulation results from a file."""
        try:
            with open(filepath, 'r') as f:
                results = json.load(f)
            logger.info(f"Results loaded from: {filepath}")
            return results
        except Exception as e:
            logger.error(f"Failed to load results: {e}")
            return {}

    def export_data(self, results: Dict, format: str = "json"):
        """Export simulation data in various formats."""
        if format.lower() == "json":
            return json.dumps(results, indent=2, default=str)
        elif format.lower() == "csv":
            # Convert to CSV format (simplified)
            import csv
            import io

            output = io.StringIO()
            writer = csv.writer(output)

            # Write headers
            writer.writerow(['step', 'time', 'action', 'spike_rate', 'curiosity'])

            # Write data
            for i, (action, obs) in enumerate(zip(self.action_history, self.observation_history)):
                writer.writerow([
                    action['step'],
                    action['time'],
                    action['action'],
                    obs.get('metrics', {}).get('spike_rate', 0),
                    obs.get('agent_state', {}).get('curiosity', 0)
                ])

            return output.getvalue()
        else:
            raise ValueError(f"Unsupported format: {format}")

    def print_summary(self, results: Dict):
        """Print a summary of simulation results."""
        print("\n" + "="*60)
        print("NLM SIMULATION RESULTS SUMMARY")
        print("="*60)

        if 'experiment_name' in results:
            print(f"Experiment: {results['experiment_name']}")

        if 'steps' in results:
            print(f"Steps: {results['steps']}")

        if 'duration' in results:
            print(f"Duration: {results['duration']:.2f} seconds")

        if 'agent_mode' in results:
            print(f"Agent Mode: {results['agent_mode']}")

        final_metrics = results.get('final_metrics', {})
        if final_metrics:
            print(f"\nFinal Performance Metrics:")
            print(f"  Steps per second: {final_metrics.get('steps_per_second', 0):.2f}")
            print(f"  Spike rate: {final_metrics.get('spike_rate', 0):.4f}")
            print(f"  Synaptic activity: {final_metrics.get('synaptic_activity', 0):.4f}")
            print(f"  Curiosity level: {final_metrics.get('exploration_rate', 0):.4f}")
            print(f"  Learning rate: {final_metrics.get('learning_rate', 0):.4f}")

        print("\n" + "="*60 + "\n")

    def pause_simulation(self):
        """Pause the current simulation."""
        self.state = SimulationState.PAUSED
        logger.info("Simulation paused")

    def resume_simulation(self):
        """Resume the current simulation."""
        self.state = SimulationState.RUNNING
        logger.info("Simulation resumed")

    def stop_simulation(self):
        """Stop the current simulation."""
        self.state = SimulationState.COMPLETED
        logger.info("Simulation stopped")

    def reset_simulation(self):
        """Reset the simulation state."""
        self.state = SimulationState.IDLE
        self.step_count = 0
        self.action_history.clear()
        self.observation_history.clear()
        self.performance_history.clear()
        logger.info("Simulation reset")


def run_advanced_demo():
    """Run an advanced demonstration of the NLM system."""
    print("NLM Advanced Agent Controller Demo")
    print("=" * 50)

    # Create controller
    controller = AdvancedAgentController(log_file="nlm_simulation.log")

    # Define advanced experiment configuration
    experiment_config = ExperimentConfig(
        name="advanced_agent_demo",
        brain_config={
            "neuron_count": 1000,
            "synapse_density": 0.15,
            "connection_probability": 0.2,
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.001},
                "hebbian": {"enable": True, "learning_rate": 0.0005}
            },
            "neuromod": {
                "dopamine": {"scale": 1.0},
                "curiosity": {"enable": True},
                "novelty": {"enable": True}
            },
            "development": {
                "initial_plasticity": 1.0,
                "critical_period_multiplier": 0.8
            }
        },
        world_config={
            "width": 30,
            "height": 30,
            "visionWidth": 12,
            "visionHeight": 12,
            "max_energy": 100.0,
            "energy_decay_rate": 0.995,
            "random_seed": 12345
        },
        agent_config={
            "features": {
                "reward_modulation": True,
                "structural_plasticity": True,
                "development": True,
                "curiosity": True
            },
            "mode": {
                "name": "exploration"
            }
        },
        duration=500.0,
        timestep=0.1,
        save_frequency=100,
        log_frequency=50,
        tags=["advanced", "demonstration", "agent-based"],
        metadata={"description": "Advanced NLM agent demonstration with enhanced features"}
    )

    # Add callback for step monitoring
    def step_callback(event_type, data):
        if event_type == "step" and data["step"] % 100 == 0:
            print(f"Step {data['step']}: Action={data['action']}, "
                  f"SpikeRate={data['metrics'].spike_rate:.4f}, "
                  f"Curiosity={data['metrics'].exploration_rate:.4f}")

    controller.add_callback("step", step_callback)

    # Run experiment
    print(f"\nRunning advanced experiment: {experiment_config.name}")
    print(f"Brain: {experiment_config.brain_config['neuron_count']} neurons")
    print(f"World: {experiment_config.world_config['width']}x{experiment_config.world_config['height']}")
    print(f"Duration: {experiment_config.duration}s ({experiment_config.duration/experiment_config.timestep:.0f} steps)")
    print()

    results = controller.run_experiment(experiment_config)

    if results["status"] == "completed":
        print("\nExperiment completed successfully!")
        controller.print_summary(results)

        # Save results
        controller.save_results(results, "advanced_experiment_results.json")

        # Export to CSV
        csv_data = controller.export_data(results, "csv")
        with open("advanced_experiment_results.csv", "w") as f:
            f.write(csv_data)
        print("Results saved to advanced_experiment_results.json and advanced_experiment_results.csv")

        # Print some statistics
        print(f"\nKey Statistics:")
        print(f"- Total steps: {results['steps']}")
        print(f"- Total time: {results['duration']:.2f} seconds")
        print(f"- Steps per second: {results['final_metrics'].get('steps_per_second', 0):.2f}")
        print(f"- Average spike rate: {results['final_metrics'].get('spike_rate', 0):.4f}")
        print(f"- Average curiosity: {results['final_metrics'].get('exploration_rate', 0):.4f}")
    else:
        print(f"\nExperiment failed: {results.get('error', 'Unknown error')}")

    print("\n" + "=" * 50)
    print("Advanced demo completed!")


if __name__ == "__main__":
    run_advanced_demo()
