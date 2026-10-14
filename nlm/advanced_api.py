# Advanced Python API for NLM (Neural Learning Machine)

from __future__ import annotations

import json
import time
import numpy as np
from typing import Any, Dict, List, Optional, Union, Callable
from dataclasses import dataclass
from enum import Enum

from .core import (
    Brain,
    Config,
    SimpleWorld,
    AgentBrain,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    ActionResult,
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

__all__ = [
    # Advanced simulation classes
    "NeuralSimulation",
    "BrainExperiment",
    "WorldInteractionExperiment",
    "CognitiveTest",
    "MemoryTest",
    "PlasticityTest",
    
    # Advanced utilities
    "run_simulation",
    "save_brain_state",
    "load_brain_state",
    "benchmark_simulation",
    "visualize_brain_state",
    "analyze_brain_dynamics",
    
    # Advanced configuration
    "AdvancedConfig",
    "SimulationPresets",
    
    # Advanced sensory/motor utilities
    "SensoryData",
    "MotorData",
    "WorldState",
    "AgentState",
    
    # Analysis and debugging
    "BrainAnalyzer",
    "DynamicsProfiler",
    "ConnectivityInspector",
    
    # Advanced helper functions
    "create_exploratory_simulation",
    "create_dev_stage_simulation",
    "create_plasticity_experiment",
    "create_memory_test",
]

# Advanced configuration classes
class AdvancedConfig:
    """Advanced configuration with preset support and validation."""
    
    def __init__(self, base_config: Optional[Config] = None, preset: str = "default"):
        self.base = base_config or create_default_config()
        self.preset = preset
        self._apply_preset()
    
    def _apply_preset(self) -> None:
        """Apply preset configuration settings."""
        presets = {
            "default": {
                "neuron_count": 1000,
                "region_count": 2,
                "plasticity_learning_rate": 0.01,
                "dopamine_baseline": 0.1,
                "visualization_enabled": False,
            },
            "research": {
                "neuron_count": 5000,
                "region_count": 5,
                "plasticity_learning_rate": 0.005,
                "dopamine_baseline": 0.2,
                "visualization_enabled": True,
                "log_level": "DEBUG",
            },
            "development": {
                "neuron_count": 2000,
                "region_count": 3,
                "plasticity_learning_rate": 0.02,
                "dopamine_baseline": 0.05,
                "development_synaptogenesis_rate": 0.002,
                "development_pruning_rate": 0.0005,
                "log_level": "INFO",
            },
            "production": {
                "neuron_count": 100,
                "region_count": 1,
                "plasticity_learning_rate": 0.001,
                "dopamine_baseline": 0.0,
                "max_simulation_steps": 50000,
                "log_level": "WARNING",
            },
        }
        
        if self.preset in presets:
            for key, value in presets[self.preset].items():
                if hasattr(self.base, key):
                    setattr(self.base, key, value)
    
    def get_config(self) -> Config:
        """Get the underlying configuration."""
        return self.base
    
    def update_config(self, **kwargs) -> None:
        """Update configuration parameters."""
        for key, value in kwargs.items():
            if hasattr(self.base, key):
                setattr(self.base, key, value)
            else:
                raise ValueError(f"Unknown configuration key: {key}")
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of issues."""
        issues = []
        
        if self.base.neuron_count <= 0:
            issues.append("neuron_count must be positive")
        if self.base.region_count <= 0:
            issues.append("region_count must be positive")
        if not 0.0 <= self.base.connection_probability <= 1.0:
            issues.append("connection_probability must be between 0.0 and 1.0")
        if self.base.simulation_timestep <= 0:
            issues.append("simulation_timestep must be positive")
        
        return issues
    
    def save_to_json(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
        return self.base.save_to_file(filepath)
    
    def load_from_json(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        return self.base.load_from_file(filepath)
    
    def __str__(self) -> str:
        return f"AdvancedConfig(preset='{self.preset}', neurons={self.base.neuron_count})"
class SimulationPresets:
    """Common simulation presets for different research scenarios."""
    
    @staticmethod
    def create_exploratory_config() -> AdvancedConfig:
        """Create configuration for exploratory simulations."""
        config = AdvancedConfig(preset="research")
        config.update_config(
            connection_probability=0.15,
            plasticity_learning_rate=0.008,
            visualization_enabled=True,
            log_level="DEBUG",
            max_simulation_steps=20000,
        )
        return config
    
    @staticmethod
    def create_dev_stage_config() -> AdvancedConfig:
        """Create configuration for development stage simulations."""
        config = AdvancedConfig(preset="development")
        config.update_config(
            development_synaptogenesis_rate=0.003,
            development_pruning_rate=0.001,
            visualization_enabled=False,
            log_level="INFO",
            max_simulation_steps=30000,
        )
        return config
    
    @staticmethod
    def create_production_config() -> AdvancedConfig:
        """Create configuration for production/simulated environment."""
        config = AdvancedConfig(preset="production")
        config.update_config(
            visualization_enabled=False,
            log_level="WARNING",
            max_simulation_steps=50000,
        )
        return config
    
    @staticmethod
    def create_memory_integrated_config() -> AdvancedConfig:
        """Create configuration optimized for memory integration experiments."""
        config = AdvancedConfig()
        config.update_config(
            neuron_count=3000,
            region_count=4,
            plasticity_learning_rate=0.015,
            dopamine_baseline=0.15,
            visualization_enabled=False,
            log_level="INFO",
            max_simulation_steps=50000,
        )
        return config
# Advanced data structures for simulation state
@dataclass
class SensoryData:
    """Advanced sensory data structure."""
    
    vision: Optional[np.ndarray] = None
    touch: Optional[np.ndarray] = None
    internal: Optional[np.ndarray] = None
    proprioception: Optional[np.ndarray] = None
    audio: Optional[np.ndarray] = None
    timestamp: float = 0.0
    source: str = "unknown"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "vision": self.vision.tolist() if self.vision is not None else None,
            "touch": self.touch.tolist() if self.touch is not None else None,
            "internal": self.internal.tolist() if self.internal is not None else None,
            "proprioception": self.proprioception.tolist() if self.proprioception is not None else None,
            "audio": self.audio.tolist() if self.audio is not None else None,
            "timestamp": self.timestamp,
            "source": self.source,
        }
class MotorData:
    """Advanced motor data structure."""
    
    action_type: ActionType = ActionType.WAIT
    parameters: np.ndarray = None
    confidence: float = 1.0
    timestamp: float = 0.0
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: Optional[np.ndarray] = None, confidence: float = 1.0):
        self.action_type = action_type
        self.parameters = parameters or np.array([])
        self.confidence = confidence
        self.timestamp = time.time()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "action_type": self.action_type.value,
            "parameters": self.parameters.tolist() if self.parameters is not None else [],
            "confidence": self.confidence,
            "timestamp": self.timestamp,
        }
class WorldState:
    """Advanced world state structure."""
    
    def __init__(self, width: int = 10, height: int = 10):
        self.width = width
        self.height = height
        self.objects = []
        self.agents = []
        self.resources = []
        self.hazards = []
        self.time_step = 0
        
    def add_object(self, x: float, y: float, obj_type: WorldObjectType, 
                   value: float = 0.0, radius: float = 0.5) -> None:
        """Add object to world."""
        self.objects.append({
            "x": x,
            "y": y,
            "type": obj_type.value,
            "value": value,
            "radius": radius,
            "active": True,
        })
    
    def remove_object(self, x: float, y: float) -> bool:
        """Remove object from world."""
        for i, obj in enumerate(self.objects):
            if abs(obj["x"] - x) < 0.1 and abs(obj["y"] - y) < 0.1:
                self.objects.pop(i)
                return True
        return False
class AgentState:
    """Advanced agent state structure."""
    
    def __init__(self):
        self.position = np.array([0.0, 0.0])
        self.orientation = 0.0
        self.velocity = np.array([0.0, 0.0])
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
class NeuralSimulation:
    """Advanced neural simulation wrapper with high-level API."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or AdvancedConfig()
        self.world = SimpleWorld()
        self.brain = Brain(self.config.get_config())
        self.agent_brain = AgentBrain(self.brain)
        self.current_step = 0
        self.simulation_time = 0.0
        self.episode_reward = 0.0
        self.predicted_reward = 0.0
        self.start_time = time.time()
        
        # Initialize world with agent
        self.world.configure(
            self.config.get_config().environment_width,
            self.config.get_config().environment_height,
            7,  # vision width
            7,  # vision height
        )
        self.world.setAgentStart(5, 5)
        
        self.agent = self.world.getAgentBody()
        self.agent_brain.initialize(self.world)
        self.brain.initialize()
        
    def step(self) -> Dict[str, Any]:
        """Advance simulation by one step."""
        # Get current sensory percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent_brain.processSensoryInput(percept)
        
        # Brain step
        self.brain.step(self.current_step, self.simulation_time)
        
        # Decode motor command
        motor_command = self.agent_brain.decodeMotorCommand()
        
        # Apply action to world
        self.world.applyMotorCommand(motor_command, self.simulation_time)
        
        # Compute reward
        reward = self.world.computeReward(self.agent)
        
        # Apply reward modulation
        self.agent_brain.applyRewardModulation(reward, self.predicted_reward)
        
        # Update development
        self.agent_brain.updateDevelopment(self.config.get_config().simulation_timestep)
        
        # Update statistics
        self.current_step += 1
        self.simulation_time += self.config.get_config().simulation_timestep
        self.episode_reward += reward
        
        # Return step information
        return {
            "step": self.current_step,
            "time": self.simulation_time,
            "reward": reward,
            "episode_reward": self.episode_reward,
            "brain_state": self._get_brain_state(),
            "world_state": self._get_world_state(),
            "agent_state": self._get_agent_state(),
        }
    
    def _get_brain_state(self) -> Dict[str, Any]:
        """Get current brain state."""
        return {
            "neuron_count": self.brain.getTotalNeuronCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": self.brain.getExcitationInhibitionRatio(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "developmental_stage": self.brain.getDevelopmentalStage().value,
            "neuromodulation_level": self.agent_brain.getNeuromodulationLevel(),
            "curiosity_level": self.agent_brain.getCuriosityLevel(),
            "novelty_level": self.agent_brain.getNoveltyLevel(),
            "prediction_error": self.agent_brain.getPredictionError(),
        }
    
    def _get_world_state(self) -> Dict[str, Any]:
        """Get current world state."""
        return {
            "width": self.world.getWidth(),
            "height": self.world.getHeight(),
            "max_energy": self.world.getMaxEnergy(),
            "energy_decay_rate": self.world.getEnergyDecayRate(),
            "simulation_time": self.world.getSimulationTime(),
        }
    
    def _get_agent_state(self) -> Dict[str, Any]:
        """Get current agent state."""
        return {
            "x": self.agent.x,
            "y": self.agent.y,
            "orientation": self.agent.orientation,
            "velocity_x": self.agent.velocityX,
            "velocity_y": self.agent.velocityY,
            "angular_velocity": self.agent.angularVelocity,
            "energy": self.agent.energy,
            "health": self.agent.health,
            "age": self.agent.age,
            "is_moving": self.agent.isMoving,
            "is_turning": self.agent.isTurning,
            "last_action_time": self.agent.lastActionTime,
        }
    
    def run_episode(self, max_steps: Optional[int] = None) -> List[Dict[str, Any]]:
        """Run a complete episode."""
        max_steps = max_steps or self.config.get_config().max_simulation_steps
        
        episode_data = []
        for step in range(max_steps):
            step_data = self.step()
            episode_data.append(step_data)
            
            # Check for termination conditions
            if self.agent.health <= 0 or step >= max_steps - 1:
                break
        
        return episode_data
    
    def reset(self) -> None:
        """Reset simulation."""
        self.current_step = 0
        self.simulation_time = 0.0
        self.episode_reward = 0.0
        self.predicted_reward = 0.0
        self.start_time = time.time()
        
        # Reset world and brain
        self.world.reset()
        self.world.setAgentStart(5, 5)
        self.agent = self.world.getAgentBody()
        
        # Reset agent brain
        self.agent_brain.reset()
        
        # Reset brain
        self.brain.reset()
    
    def save(self, filepath: str) -> bool:
        """Save simulation state."""
        try:
            # Save brain state
            brain_file = filepath.replace('.json', '_brain.npz')
            np.savez(brain_file, 
                    step=self.current_step,
                    time=self.simulation_time,
                    episode_reward=self.episode_reward,
                    config=self.config.get_config().to_dict())
            
            # Save world state
            world_file = filepath.replace('.json', '_world.json')
            with open(world_file, 'w') as f:
                json.dump({
                    "width": self.world.getWidth(),
                    "height": self.world.getHeight(),
                    "max_energy": self.world.getMaxEnergy(),
                    "energy_decay_rate": self.world.getEnergyDecayRate(),
                }, f, indent=2)
            
            # Save episode summary
            summary_file = filepath
            with open(summary_file, 'w') as f:
                json.dump({
                    "episode_data": self.run_episode(),
                    "final_brain_state": self._get_brain_state(),
                    "final_agent_state": self._get_agent_state(),
                }, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving simulation: {e}")
            return False
    
    def load(self, filepath: str) -> bool:
        """Load simulation state."""
        try:
            # This would need more sophisticated implementation
            # For now, just reset
            self.reset()
            return True
        except Exception as e:
            print(f"Error loading simulation: {e}")
            return False
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        elapsed_time = time.time() - self.start_time
        steps_per_second = self.current_step / elapsed_time if elapsed_time > 0 else 0
        
        return {
            "total_steps": self.current_step,
            "simulation_time": self.simulation_time,
            "episode_reward": self.episode_reward,
            "elapsed_time": elapsed_time,
            "steps_per_second": steps_per_second,
            "average_reward_per_step": self.episode_reward / self.current_step if self.current_step > 0 else 0,
            "final_brain_state": self._get_brain_state(),
        }
class BrainExperiment:
    """High-level experiment runner."""
    
    def __init__(self, name: str, config: Optional[AdvancedConfig] = None):
        self.name = name
        self.config = config or AdvancedConfig()
        self.simulations = []
        self.results = []
        
    def add_simulation(self, simulation: NeuralSimulation) -> None:
        """Add a simulation to the experiment."""
        self.simulations.append(simulation)
    
    def run_experiment(self, episodes_per_simulation: int = 100) -> Dict[str, Any]:
        """Run the complete experiment."""
        experiment_results = {
            "name": self.name,
            "config": self.config.get_config().to_dict(),
            "simulations_run": len(self.simulations),
            "total_episodes": len(self.simulations) * episodes_per_simulation,
            "results": [],
        }
        
        for i, simulation in enumerate(self.simulations):
            print(f"Running simulation {i+1}/{len(self.simulations)} for experiment '{self.name}'")
            
            simulation_results = []
            for episode in range(episodes_per_simulation):
                episode_data = simulation.run_episode()
                simulation_results.append(episode_data)
                simulation.reset()
            
            experiment_results["results"].append({
                "simulation_index": i,
                "episodes": simulation_results,
                "final_stats": simulation.get_performance_stats(),
            })
        
        self.results = experiment_results
        return experiment_results
    
    def analyze_results(self) -> Dict[str, Any]:
        """Analyze experiment results."""
        if not self.results:
            return {"error": "No results to analyze"}
        
        analysis = {
            "name": self.name,
            "summary": {
                "total_simulations": len(self.simulations),
                "total_episodes": sum(len(result["episodes"]) for result in self.results["results"]),
            },
            "performance_metrics": [],
            "brain_dynamics": [],
            "recommendations": [],
        }
        
        # Calculate performance metrics
        for result in self.results["results"]:
            sim_stats = result["final_stats"]
            analysis["performance_metrics"].append({
                "simulation": result["simulation_index"],
                "steps_per_second": sim_stats["steps_per_second"],
                "average_reward": sim_stats["average_reward_per_step"],
                "total_steps": sim_stats["total_steps"],
            })
        
        # Generate recommendations
        if analysis["performance_metrics"]:
            avg_steps_per_second = np.mean([m["steps_per_second"] for m in analysis["performance_metrics"]])
            if avg_steps_per_second < 100:
                analysis["recommendations"].append(
                    "Consider optimizing neural core for better performance"
                )
            
            avg_reward = np.mean([m["average_reward"] for m in analysis["performance_metrics"]])
            if avg_reward < 0.1:
                analysis["recommendations"].append(
                    "Consider adjusting reward function or neuromodulation parameters"
                )
        
        return analysis
# Advanced utility functions
def run_simulation(config: Optional[AdvancedConfig] = None, 
                  duration: int = 1000,
                  callbacks: Optional[List[Callable]] = None) -> NeuralSimulation:
    """Run a simulation with advanced utilities."""
    simulation = NeuralSimulation(config)
    callbacks = callbacks or []
    
    for step in range(duration):
        step_data = simulation.step()
        
        # Execute callbacks
        for callback in callbacks:
            try:
                callback(step_data)
            except Exception as e:
                print(f"Callback error: {e}")
        
        # Check for early termination
        if simulation.agent.health <= 0:
            break
    
    return simulation
def save_brain_state(brain: Brain, filepath: str) -> bool:
    """Save brain state to file."""
    try:
        brain.save(filepath)
        return True
    except Exception as e:
        print(f"Error saving brain state: {e}")
        return False
def load_brain_state(brain: Brain, filepath: str) -> bool:
    """Load brain state from file."""
    try:
        brain.load(filepath)
        return True
    except Exception as e:
        print(f"Error loading brain state: {e}")
        return False
def benchmark_simulation(config: Optional[AdvancedConfig] = None, 
                         steps: int = 1000) -> Dict[str, Any]:
    """Benchmark simulation performance."""
    simulation = NeuralSimulation(config)
    
    start_time = time.time()
    for _ in range(steps):
        simulation.step()
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = steps / elapsed if elapsed > 0 else 0
    
    return {
        "steps": steps,
        "elapsed_time": elapsed,
        "steps_per_second": steps_per_second,
        "simulation_stats": simulation.get_performance_stats(),
    }
def visualize_brain_state(brain: Brain) -> None:
    """Create simple visualization of brain state."""
    print("Brain State Visualization:")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Active Neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.4f}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage().value}")
    print()
def analyze_brain_dynamics(brain: Brain, window_size: int = 100) -> Dict[str, Any]:
    """Analyze brain dynamics over time."""
    # This would need to track history over time
    # For now, return current state analysis
    return {
        "current_state": {
            "neuron_count": brain.getTotalNeuronCount(),
            "activity_level": brain.getActiveNeuronCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
            "firing_rate": brain.getAverageFiringRate(),
            "plasticity": brain.getExcitationInhibitionRatio(),
        },
        "analysis": {
            "stability": "stable",  # This would require history
            "learning_rate": "moderate",  # This would require history
            "balance": "balanced",  # This would require history
        },
    }
# Advanced helper functions
def create_exploratory_simulation() -> NeuralSimulation:
    """Create an exploratory simulation with optimal settings."""
    config = SimulationPresets.create_exploratory_config()
    return NeuralSimulation(config)
def create_dev_stage_simulation() -> NeuralSimulation:
    """Create a development stage simulation."""
    config = SimulationPresets.create_dev_stage_config()
    return NeuralSimulation(config)
def create_plasticity_experiment(neuron_count: int = 2000) -> NeuralSimulation:
    """Create a plasticity-focused experiment."""
    config = AdvancedConfig()
    config.update_config(
        neuron_count=neuron_count,
        region_count=3,
        plasticity_learning_rate=0.02,
        dopamine_baseline=0.15,
        visualization_enabled=False,
        max_simulation_steps=50000,
    )
    return NeuralSimulation(config)
def create_memory_test() -> NeuralSimulation:
    """Create a memory-focused test."""
    config = SimulationPresets.create_memory_integrated_config()
    return NeuralSimulation(config)
# Create specialized test classes for advanced users
class CognitiveTest:
    """High-level cognitive tests."""
    
    @staticmethod
    def test_memory_retention(simulation: NeuralSimulation, duration: int = 1000) -> Dict[str, Any]:
        """Test memory retention over time."""
        simulation.reset()
        
        memory_data = []
        for step in range(duration):
            step_data = simulation.step()
            
            # Extract memory-related metrics
            memory_metrics = {
                "step": step,
                "active_neurons": step_data["brain_state"]["active_neurons"],
                "firing_rate": step_data["brain_state"]["average_firing_rate"],
                "neuromodulation": step_data["brain_state"]["neuromodulation_level"],
            }
            memory_data.append(memory_metrics)
        
        # Analyze retention
        initial_activity = memory_data[0]["active_neurons"]
        final_activity = memory_data[-1]["active_neurons"]
        activity_retention = (final_activity / initial_activity) if initial_activity > 0 else 0
        
        return {
            "test": "memory_retention",
            "duration": duration,
            "data": memory_data,
            "retention_rate": activity_retention,
            "conclusion": "good" if activity_retention > 0.5 else "poor" if activity_retention < 0.1 else "moderate",
        }
    
    @staticmethod
    def test_prediction_accuracy(simulation: NeuralSimulation, duration: int = 500) -> Dict[str, Any]:
        """Test prediction accuracy and learning."""
        simulation.reset()
        
        prediction_data = []
        for step in range(duration):
            step_data = simulation.step()
            
            prediction_metrics = {
                "step": step,
                "prediction_error": step_data["brain_state"]["prediction_error"],
                "curiosity": step_data["brain_state"]["curiosity_level"],
                "novelty": step_data["brain_state"]["novelty_level"],
                "reward": step_data["reward"],
            }
            prediction_data.append(prediction_metrics)
        
        # Analyze prediction improvement
        initial_error = prediction_data[0]["prediction_error"]
        final_error = prediction_data[-1]["prediction_error"]
        error_improvement = (initial_error - final_error) / initial_error if initial_error > 0 else 0
        
        return {
            "test": "prediction_accuracy",
            "duration": duration,
            "data": prediction_data,
            "error_improvement": error_improvement,
            "conclusion": "good" if error_improvement > 0.5 else "poor" if error_improvement < 0.1 else "moderate",
        }
class MemoryTest:
    """Memory-focused tests."""
    
    @staticmethod
    def test_working_memory(simulation: NeuralSimulation, duration: int = 1000) -> Dict[str, Any]:
        """Test working memory capacity."""
        simulation.reset()
        
        working_memory_data = []
        for step in range(duration):
            step_data = simulation.step()
            
            # Working memory is measured by active neuron proportion
            active_proportion = step_data["brain_state"]["active_neurons"] / simulation.brain.getTotalNeuronCount()
            
            working_memory_data.append({
                "step": step,
                "active_proportion": active_proportion,
                "firing_rate": step_data["brain_state"]["average_firing_rate"],
                "stability": np.std([d["active_proportion"] for d in working_memory_data[-10:]]) if len(working_memory_data) >= 10 else 0,
            })
        
        # Analyze working memory stability
        proportions = [d["active_proportion"] for d in working_memory_data]
        mean_proportion = np.mean(proportions)
        stability = 1 - np.std(proportions) / mean_proportion if mean_proportion > 0 else 0
        
        return {
            "test": "working_memory",
            "duration": duration,
            "data": working_memory_data,
            "stability_score": stability,
            "mean_activity": mean_proportion,
            "conclusion": "stable" if stability > 0.7 else "unstable" if stability < 0.3 else "moderate",
        }
class PlasticityTest:
    """Plasticity-focused tests."""
    
    @staticmethod
    def test_stdp_effectiveness(simulation: NeuralSimulation, duration: int = 2000) -> Dict[str, Any]:
        """Test STDP effectiveness."""
        simulation.reset()
        
        plasticity_data = []
        for step in range(duration):
            step_data = simulation.step()
            
            plasticity_metrics = {
                "step": step,
                "excitatory_inhibitory_ratio": step_data["brain_state"]["excitation_inhibition_ratio"],
                "total_spikes": step_data["brain_state"]["total_spikes"],
                "neuromodulation": step_data["brain_state"]["neuromodulation_level"],
            }
            plasticity_data.append(plasticity_metrics)
        
        # Analyze STDP effectiveness
        ratios = [d["excitatory_inhibitory_ratio"] for d in plasticity_data]
        spike_growth = plasticity_data[-1]["total_spikes"] / plasticity_data[0]["total_spikes"] if plasticity_data[0]["total_spikes"] > 0 else 1
        
        return {
            "test": "stdp_effectiveness",
            "duration": duration,
            "data": plasticity_data,
            "spike_growth_rate": spike_growth,
            "ratio_stability": np.std(ratios) if len(ratios) > 1 else 0,
            "conclusion": "effective" if spike_growth > 1.5 else "moderate" if spike_growth > 1.1 else "ineffective",
        }
# Advanced analysis classes
class BrainAnalyzer:
    """Advanced brain state analysis."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.history = []
    
    def analyze_current_state(self) -> Dict[str, Any]:
        """Analyze current brain state."""
        return {
            "overall_health": self._calculate_health(),
            "activity_patterns": self._analyze_activity(),
            "plasticity_indicators": self._analyze_plasticity(),
            "development_status": self._analyze_development(),
        }
    
    def _calculate_health(self) -> Dict[str, Any]:
        """Calculate brain health metrics."""
        total_neurons = self.brain.getTotalNeuronCount()
        if total_neurons == 0:
            return {"status": "unknown", "score": 0}
        
        active_ratio = self.brain.getActiveNeuronCount() / total_neurons
        firing_rate = self.brain.getAverageFiringRate()
        ratio = self.brain.getExcitationInhibitionRatio()
        
        # Health score calculation
        health_score = (active_ratio * 0.4 + 
                       min(firing_rate, 1.0) * 0.3 + 
                       max(0, 1 - abs(ratio - 1)) * 0.3)
        
        return {
            "status": "healthy" if health_score > 0.7 else "warning" if health_score > 0.4 else "critical",
            "score": health_score,
            "active_ratio": active_ratio,
            "firing_rate": firing_rate,
            "excitation_inhibition_ratio": ratio,
        }
    
    def _analyze_activity(self) -> Dict[str, Any]:
        """Analyze activity patterns."""
        # This would need historical data
        return {
            "stability": "stable",  # Would require history
            "burst_patterns": "none",  # Would require history
            "rhythmicity": "none",  # Would require history
        }
    
    def _analyze_plasticity(self) -> Dict[str, Any]:
        """Analyze plasticity indicators."""
        # This would require tracking synaptic changes over time
        return {
            "plasticity_active": True,  # STDP is always active
            "learning_rate": "moderate",  # Would require history
            "adaptation": "present",  # Structural plasticity
        }
    
    def _analyze_development(self) -> Dict[str, Any]:
        """Analyze developmental status."""
        stage = self.brain.getDevelopmentalStage()
        return {
            "stage": stage.value,
            "stage_progress": stage.value in ["CriticalPeriod", "Maturation", "Adult"],
            "development_effects": "active",
        }
class DynamicsProfiler:
    """Profile brain dynamics over time."""
    
    def __init__(self, simulation: NeuralSimulation):
        self.simulation = simulation
        self.time_series_data = []
    
    def profile_dynamics(self, steps: int = 1000) -> Dict[str, Any]:
        """Profile brain dynamics."""
        for step in range(steps):
            step_data = self.simulation.step()
            self.time_series_data.append(step_data)
        
        if not self.time_series_data:
            return {"error": "No data collected"}
        
        # Calculate dynamics metrics
        metrics = self._calculate_dynamics_metrics()
        return {
            "profile_data": self.time_series_data,
            "dynamics_metrics": metrics,
            "summary": self._generate_summary(metrics),
        }
    
    def _calculate_dynamics_metrics(self) -> Dict[str, Any]:
        """Calculate dynamics metrics."""
        steps = len(self.time_series_data)
        
        # Extract time series
        activity_levels = [d["brain_state"]["active_neurons"] for d in self.time_series_data]
        firing_rates = [d["brain_state"]["average_firing_rate"] for d in self.time_series_data]
        neuromodulation_levels = [d["brain_state"]["neuromodulation_level"] for d in self.time_series_data]
        prediction_errors = [d["brain_state"]["prediction_error"] for d in self.time_series_data]
        
        # Calculate metrics
        return {
            "activity_trend": self._calculate_trend(activity_levels),
            "firing_rate_trend": self._calculate_trend(firing_rates),
            "neuromodulation_stability": np.std(neuromodulation_levels) if len(neuromodulation_levels) > 1 else 0,
            "prediction_error_trend": self._calculate_trend(prediction_errors),
            "coherence_metrics": self._calculate_coherence(activity_levels, firing_rates),
            "burst_detection": self._detect_bursts(activity_levels),
        }
    
    def _calculate_trend(self, data: List[float]) -> Dict[str, Any]:
        """Calculate trend for a data series."""
        if len(data) < 2:
            return {"trend": "insufficient_data", "slope": 0}
        
        x = np.arange(len(data))
        slope = np.polyfit(x, data, 1)[0]
        
        if abs(slope) < 0.01:
            trend = "stable"
        elif slope > 0.01:
            trend = "increasing"
        else:
            trend = "decreasing"
        
        return {"trend": trend, "slope": slope}
    
    def _calculate_coherence(self, data1: List[float], data2: List[float]) -> Dict[str, Any]:
        """Calculate coherence between two data series."""
        if len(data1) != len(data2) or len(data1) < 2:
            return {"coherence": 0, "interpretation": "insufficient_data"}
        
        coherence = np.corrcoef(data1, data2)[0, 1]
        
        if abs(coherence) < 0.3:
            interpretation = "weak"
        elif abs(coherence) < 0.7:
            interpretation = "moderate"
        else:
            interpretation = "strong"
        
        return {"coherence": coherence, "interpretation": interpretation}
    
    def _detect_bursts(self, data: List[float]) -> Dict[str, Any]:
        """Detect burst patterns in data."""
        if len(data) < 10:
            return {"bursts": 0, "intensity": 0}
        
        # Simple burst detection: threshold-based
        threshold = np.mean(data) + np.std(data)
        bursts = sum(1 for x in data if x > threshold)
        burst_intensity = bursts / len(data)
        
        return {"bursts": bursts, "intensity": burst_intensity, "threshold": threshold}
    
    def _generate_summary(self, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Generate summary of dynamics profile."""
        return {
            "overall_stability": self._assess_stability(metrics),
            "key_patterns": self._identify_key_patterns(metrics),
            "recommendations": self._generate_recommendations(metrics),
        }
    
    def _assess_stability(self, metrics: Dict[str, Any]) -> str:
        """Assess overall stability."""
        stability_score = 0
        
        if metrics["activity_trend"]["trend"] == "stable":
            stability_score += 0.3
        
        if metrics["neuromodulation_stability"] < 0.1:
            stability_score += 0.3
        
        if abs(metrics["prediction_error_trend"]["slope"]) < 0.001:
            stability_score += 0.4
        
        if stability_score > 0.8:
            return "high"
        elif stability_score > 0.5:
            return "moderate"
        else:
            return "low"
    
    def _identify_key_patterns(self, metrics: Dict[str, Any]) -> List[str]:n        """Identify key patterns."""
        patterns = []
        
        if metrics["activity_trend"]["trend"] == "increasing":
            patterns.append("increasing_activity")
        
        if metrics["firing_rate_trend"]["trend"] == "decreasing":
            patterns.append("decreasing_firing_rate")
        
        if metrics["prediction_error_trend"]["trend"] == "decreasing":
            patterns.append("improving_predictions")
        
        if metrics["burst_detection"]["intensity"] > 0.1:
            patterns.append("burst_activity")
        
        return patterns
    
    def _generate_recommendations(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on profile."""
        recommendations = []
        
        if metrics["activity_trend"]["trend"] == "increasing":
            recommendations.append("Consider reducing excitation or increasing inhibition")
        
        if metrics["neuromodulation_stability"] > 0.2:
            recommendations.append("Neuromodulation levels are unstable - check parameters")
        
        if "burst_activity" in self._identify_key_patterns(metrics):
            recommendations.append("Monitor for potential instability from burst patterns")
        
        if "improving_predictions" in self._identify_key_patterns(metrics):
            recommendations.append("Prediction system is learning effectively")
        
        return recommendations
class ConnectivityInspector:
    """Inspect brain connectivity."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
    
    def analyze_connectivity(self) -> Dict[str, Any]:
        """Analyze brain connectivity patterns."""
        total_neurons = self.brain.getTotalNeuronCount()
        if total_neurons == 0:
            return {"error": "No neurons to analyze"}
        
        active_neurons = self.brain.getActiveNeuronCount()
        firing_rate = self.brain.getAverageFiringRate()
        ratio = self.brain.getExcitationInhibitionRatio()
        total_spikes = self.brain.getTotalSpikeCount()
        
        return {
            "overall_connectivity": {
                "activity_level": active_neurons / total_neurons,
                "firing_rate": firing_rate,
                "excitation_inhibition_balance": ratio,
                "total_activity": total_spikes,
            },
            "network_dynamics": {
                "network_size": total_neurons,
                "effective_size": active_neurons,
                "network_efficiency": self._calculate_efficiency(active_neurons, firing_rate),
                "network_stability": self._calculate_stability(total_spikes),
            },
            "regional_patterns": self._analyze_regional_patterns(),
            "recommendations": self._generate_connectivity_recommendations(),
        }
    
    def _calculate_efficiency(self, active_neurons: int, firing_rate: float) -> float:
        """Calculate network efficiency."""
        if active_neurons == 0:
            return 0
        
        return min(1.0, (active_neurons / 100) * (firing_rate / 10))
    
    def _calculate_stability(self, total_spikes: int) -> str:
        """Calculate network stability based on spike count."""
        if total_spikes < 100:
            return "stable"
        elif total_spikes < 1000:
            return "moderate"
        else:
            return "unstable"
    
    def _analyze_regional_patterns(self) -> Dict[str, Any]:
        """Analyze regional patterns."""
        # This would require access to regional data
        return {
            "pattern": "needs_regional_access",
            "regions": 0,
            "activity_distribution": "unknown",
        }
    
    def _generate_connectivity_recommendations(self) -> List[str]:
        """Generate connectivity recommendations."""
        recommendations = []
        
        # Would require analysis of actual connectivity patterns
        recommendations.append("Monitor excitation/inhibition balance")
        recommendations.append("Track activity levels over time")
        recommendations.append("Consider regional connectivity analysis")
        
        return recommendations

__all__.extend([
    "AdvancedConfig",
    "SimulationPresets",
    "SensoryData",
    "MotorData",
    "WorldState",
    "AgentState",
    "NeuralSimulation",
    "BrainExperiment",
    "run_simulation",
    "save_brain_state",
    "load_brain_state",
    "benchmark_simulation",
    "visualize_brain_state",
    "analyze_brain_dynamics",
    "create_exploratory_simulation",
    "create_dev_stage_simulation",
    "create_plasticity_experiment",
    "create_memory_test",
    "CognitiveTest",
    "MemoryTest",
    "PlasticityTest",
    "BrainAnalyzer",
    "DynamicsProfiler",
    "ConnectivityInspector",
])