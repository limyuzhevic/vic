"""
High-level wrapper classes for NLM - Python-first API for neural simulation
"""

import time
import math
import random
from typing import List, Optional, Dict, Any, Union, Callable
from enum import Enum

from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, ActionType, MotorCommand as MotorCommandEnum,
    WorldObjectType, DevelopmentalStage, FiringState,
    SynapseType, PlasticityFlags, FiringRate,
    MembranePotential, FiringRate, Timestamp, SimulationStep,
)

from .core import Brain, Config, AgentBrain, SimpleWorld
from .sensory import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, MotorCommand
from .agent import AgentBody
from .world import WorldObject
class NLMException(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigurationError(NLMException):
    """Configuration error"""
    pass

class SimulationError(NLMException):
    """Simulation error"""
    pass

class ValidationError(NLMException):
    """Validation error"""
    pass
class NLMSimulation:
    """
    High-level neural simulation manager with Python-friendly interface.
    Provides an intuitive way to run complex neural simulations with minimal code.
    """
    
    def __init__(self, config: Optional[Config] = None, world: Optional[SimpleWorld] = None):
        self.brain = Brain(config) if config else Brain()
        self.world = world if world else SimpleWorld()
        self.agent_brain = AgentBrain(self.brain) if config else None
        self._simulation_history = []
        self._episode_count = 0
        self._learning_metrics = []
    
    def quick_start(self, neurons: int = 1000, width: int = 800, height: int = 600,
                   timestep: float = 0.001, seed: int = 42) -> 'NLMSimulation':
        """
        Quick start with common parameters.
        
        Args:
            neurons: Number of neurons in brain
            width, height: World dimensions
            timestep: Simulation timestep
            seed: Random seed
            
        Returns:
            Self for method chaining
        """
        config = Config()
        config.set("random_seed", seed, None)
        config.set("simulation_timestep", timestep, None)
        config.set("neuron_count", neurons, None)
        config.set("region_count", 1, None)
        config.set("connection_probability", 0.15, None)
        
        self.brain = Brain(config)
        self.world = SimpleWorld()
        self.world.configure(width, height, width // 10, height // 10)
        self.agent_brain = AgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
        
        return self
    
    def run_episode(self, steps: int, verbose: bool = False) -> Dict[str, Any]:
        """
        Run a single simulation episode.
        
        Args:
            steps: Number of simulation steps
            verbose: Print progress information
            
        Returns:
            Dictionary with episode statistics
        """
        if verbose:
            print(f"Starting episode {self._episode_count + 1} ({steps} steps)")
        
        episode_start_time = time.time()
        step_metrics = []
        
        for step in range(steps):
            # Get current perception
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            
            # Get action and apply to world
            action = self.agent_brain.decodeMotorCommand()
            self.world.applyMotorCommand(action, step * 0.001)
            
            # Update development
            self.agent_brain.updateDevelopment(0.001)
            
            # Record metrics
            metrics = {
                "step": step,
                "time": step * 0.001,
                "spike_count": self.brain.getTotalSpikeCount(),
                "active_neurons": self.brain.getActiveNeuronCount(),
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "vision_center": percept.get_vision_center(),
                "total_strength": percept.get_total_signal_strength()["total_strength"],
            }
            step_metrics.append(metrics)
            
            if verbose and step % 100 == 0:
                print(f"  Step {step}: "
                      f"Spikes={metrics['spike_count']}, "
                      f"Active={metrics['active_neurons']}, "
                      f"Firing={metrics['firing_neurons']}")
        
        episode_time = time.time() - episode_start_time
        
        # Calculate episode statistics
        final_spikes = self.brain.getTotalSpikeCount()
        total_firing = sum(m["firing_neurons"] for m in step_metrics)
        avg_firing_rate = total_firing / steps if steps > 0 else 0.0
        
        episode_stats = {
            "episode": self._episode_count + 1,
            "steps": steps,
            "duration": episode_time,
            "final_spike_count": final_spikes,
            "total_firing": total_firing,
            "average_firing_rate": avg_firing_rate,
            "steps_per_second": steps / episode_time if episode_time > 0 else 0.0,
            "final_metrics": step_metrics[-1] if step_metrics else None,
        }
        
        self._episode_count += 1
        self._simulation_history.append({
            "episode": episode_stats,
            "metrics": step_metrics
        })
        
        if verbose:
            print(f"Episode completed in {episode_time:.2f}s: "
                  f"Final spikes={final_spikes}, "
                  f"Avg firing rate={avg_firing_rate:.2f} Hz")
        
        return episode_stats
    
    def run_simulation(self, total_steps: int, steps_per_episode: int = 100,
                      verbose: bool = False) -> Dict[str, Any]:
        """
        Run a complete simulation with multiple episodes.
        
        Args:
            total_steps: Total simulation steps
            steps_per_episode: Steps per episode
            verbose: Print progress information
            
        Returns:
            Dictionary with simulation results
        """
        episodes = total_steps // steps_per_episode
        remaining_steps = total_steps % steps_per_episode
        
        if verbose:
            print(f"Running simulation: {total_steps} steps "
                  f"({episodes} episodes + {remaining_steps} steps)")
        
        simulation_start = time.time()
        
        for episode in range(episodes):
            if remaining_steps > 0 and episode == episodes - 1:
                # Last episode with remaining steps
                self.run_episode(remaining_steps, verbose)
            else:
                self.run_episode(steps_per_episode, verbose)
        
        simulation_time = time.time() - simulation_start
        
        results = {
            "total_steps": total_steps,
            "episodes": self._episode_count,
            "duration": simulation_time,
            "steps_per_second": total_steps / simulation_time if simulation_time > 0 else 0.0,
            "final_spike_count": self.brain.getTotalSpikeCount(),
            "final_firing_rate": self.brain.getAverageFiringRate(),
            "simulation_history": self._simulation_history,
        }
        
        if verbose:
            print(f"Simulation completed in {simulation_time:.2f}s: "
                  f"Final spikes={results['final_spike_count']}, "
                  f"Final firing rate={results['final_firing_rate']:.2f} Hz")
        
        return results
    
    def run_learning_experiment(self, experiences: int, steps_per_experience: int = 200,
                               sensory_patterns: bool = True) -> Dict[str, Any]:
        """
        Run a learning experiment with multiple experiences.
        
        Args:
            experiences: Number of learning experiences
            steps_per_experience: Steps per experience
            sensory_patterns: Use varied sensory patterns
            
        Returns:
            Dictionary with learning results
        """
        print(f"Starting learning experiment: {experiences} experiences")
        
        # Enable plasticity
        if region := self.brain.getRegion(RegionId(1)):
            for syn in region.getSynapses():
                syn.enablePlasticity(true, true, false)
        
        # Run experiences
        learning_results = []
        
        for experience in range(experiences):
            print(f"\n--- Experience {experience + 1}/{experiences} ---")
            
            # Create experience-specific pattern
            base_strength = 20.0 + experience * 5.0
            pattern_offset = 10.0 * math.sin(experience * 0.5)
            
            experience_start = time.time()
            
            for step in range(steps_per_experience):
                # Create experience-specific input
                if sensory_patterns:
                    # Complex pattern based on experience and step
                    current = (base_strength + 
                             pattern_offset + 
                             15.0 * math.sin(step * 0.05) +
                             10.0 * math.cos(step * 0.03) +
                             random.uniform(-5, 5))
                else:
                    # Simple constant input
                    current = base_strength
                
                self.brain.injectCurrentToNeurons(NeuronType.Sensory, current)
                self.brain.step(step, step * 0.001)
                
                if step % 50 == 0:
                    print(f"  Step {step}: Spikes={self.brain.getTotalSpikeCount()}, "
                          f"Firing={self.brain.getFiringNeuronCount()}")
            
            experience_time = time.time() - experience_start
            
            # Record experience results
            experience_stats = {
                "experience": experience + 1,
                "steps": steps_per_experience,
                "duration": experience_time,
                "final_spike_count": self.brain.getTotalSpikeCount(),
                "final_firing_rate": self.brain.getAverageFiringRate(),
                "base_strength": base_strength,
                "pattern_offset": pattern_offset,
            }
            
            learning_results.append(experience_stats)
        
        # Calculate overall learning
        total_spikes = sum(r["final_spike_count"] for r in learning_results)
        avg_final_spikes = total_spikes / experiences
        
        # Check if learning occurred (weights changed)
        learning_detected = False
        
        if len(learning_results) >= 2:
            first_exp = learning_results[0]["final_spike_count"]
            last_exp = learning_results[-1]["final_spike_count"]
            learning_change = (last_exp - first_exp) / first_exp if first_exp > 0 else 0.0
            learning_detected = abs(learning_change) > 0.1
        
        simulation_results = {
            "experiences": experiences,
            "steps_per_experience": steps_per_experience,
            "total_steps": experiences * steps_per_experience,
            "learning_detected": learning_detected,
            "total_spikes": total_spikes,
            "average_final_spikes": avg_final_spikes,
            "learning_results": learning_results,
            "final_brain_state": self.get_brain_summary(),
        }
        
        print(f"\nLearning experiment completed:")
        print(f"  Learning detected: {learning_detected}")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Average final spikes: {avg_final_spikes:.1f}")
        
        return simulation_results
    
    def get_brain_summary(self) -> Dict[str, Any]:
        """Get comprehensive brain summary"""
        return {
            "stats": self.brain.get_connectivity_stats(),
            "developmental": self.brain.get_developmental_summary(),
            "spike_system": {
                "total_spikes": self.brain.getTotalSpikeCount(),
                "pending_events": self.brain.getPendingSpikeEventCount(),
            },
            "plasticity": self.brain.getPlasticityFlags(),
        }
    
    def get_simulation_summary(self) -> Dict[str, Any]:
        """Get simulation summary"""
        return {
            "episodes_run": self._episode_count,
            "total_steps": sum(e["episode"]["steps"] for e in self._simulation_history),
            "simulation_history": self._simulation_history,
            "world_state": self.world.get_simulation_summary(),
        }
    
    def reset(self):
        """Reset simulation to initial state"""
        self.brain.reset()
        self.brain.initialize()
        self._episode_count = 0
        self._simulation_history = []
        self._learning_metrics = []
        
        if self.agent_brain:
            self.agent_brain.reset()
    
    def save_state(self, filepath: str):
        """Save simulation state to file"""
        self.brain.save(filepath)
    
    def load_state(self, filepath: str):
        """Load simulation state from file"""
        if self.brain.load(filepath):
            self.reset()
            return True
        return False
class NLMExperiment:
    """
    High-level experiment manager for systematic testing and research.
    Provides tools for designing, running, and analyzing neural experiments.
    """
    
    def __init__(self, name: str):
        self.name = name
        self.config = Config()
        self.results = []
        self.experiments = []
    
    def add_brain(self, brain: Brain) -> 'NLMExperiment':
        """Add a brain to the experiment"""
        self.experiments.append({
            "brain": brain,
            "config": self.config,
            "results": None,
            "name": self.name
        })
        return self
    
    def add_parameter_set(self, param_set: Dict[str, Any]) -> 'NLMExperiment':
        """Add a parameter set for testing"""
        self.config.clear()
        for key, value in param_set.items():
            self.config.set(key, value, None)
        return self
    
    def run_experiment(self, steps: int, experiment_type: str = "standard") -> Dict[str, Any]:
        """
        Run a single experiment.
        
        Args:
            steps: Number of simulation steps
            experiment_type: Type of experiment (standard, plasticity, development, etc.)
            
        Returns:
            Experiment results
        """
        if not self.experiments:
            raise ValidationError("No brain configured for experiment")
        
        experiment_results = {
            "experiment_name": self.name,
            "experiment_type": experiment_type,
            "steps": steps,
            "start_time": time.time(),
            "metrics": [],
            "brain_state": None,
        }
        
        for exp_data in self.experiments:
            brain = exp_data["brain"]
            
            if experiment_type == "plasticity":
                # Run with plasticity enabled
                if region := brain.getRegion(RegionId(1)):
                    for syn in region.getSynapses():
                        syn.enablePlasticity(true, true, false)
                
                # Run plasticity experiment
                result = brain.simulate_plasticity_experiment(steps)
                experiment_results["plasticity_results"] = result
            
            elif experiment_type == "development":
                # Run development experiment
                brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
                for step in range(steps):
                    brain.injectCurrentToNeurons(NeuronType.Sensory, 20.0)
                    brain.step(step, step * 0.001)
                
                experiment_results["developmental_results"] = {
                    "stage": brain.getDevelopmentalStage(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "final_firing_rate": brain.getAverageFiringRate(),
                }
            
            else:  # standard experiment
                for step in range(steps):
                    brain.injectCurrentToNeurons(NeuronType.Sensory, 20.0 + step % 50)
                    brain.step(step, step * 0.001)
                    
                    # Record metrics
                    metrics = {
                        "step": step,
                        "spike_count": brain.getTotalSpikeCount(),
                        "active_neurons": brain.getActiveNeuronCount(),
                        "firing_neurons": brain.getFiringNeuronCount(),
                        "firing_rate": brain.getAverageFiringRate(),
                    }
                    experiment_results["metrics"].append(metrics)
                
                experiment_results["standard_results"] = {
                    "final_spike_count": brain.getTotalSpikeCount(),
                    "final_firing_rate": brain.getAverageFiringRate(),
                    "total_steps": steps,
                }
            
            experiment_results["brain_state"] = brain.get_brain_summary()
        
        experiment_results["end_time"] = time.time()
        experiment_results["duration"] = experiment_results["end_time"] - experiment_results["start_time"]
        
        self.results.append(experiment_results)
        return experiment_results
    
    def run_parameter_sweep(self, param_sets: List[Dict[str, Any]],
                           steps: int) -> List[Dict[str, Any]]:
        """
        Run a parameter sweep across multiple configurations.
        
        Args:
            param_sets: List of parameter sets to test
            steps: Number of steps per configuration
            
        Returns:
            List of results for each parameter set
        """
        results = []
        
        for i, param_set in enumerate(param_sets):
            print(f"Running parameter set {i + 1}/{len(param_sets)}")
            
            # Create new brain for each parameter set
            test_config = Config()
            for key, value in param_set.items():
                test_config.set(key, value, None)
            
            brain = Brain(test_config)
            brain.quick_initialize()
            
            # Create experiment with this brain
            exp = NLMExperiment(f"{self.name}_param_{i}")
            exp.add_brain(brain)
            exp.add_parameter_set(param_set)
            
            result = exp.run_experiment(steps, "standard")
            result["parameter_set"] = param_set
            results.append(result)
        
        return results
class NLMPlotting:
    """
    High-level plotting and visualization tools for NLM results.
    Provides easy-to-use plotting functions for neural simulation data.
    """
    
    def __init__(self):
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            self.plt = plt
            self.np = np
            self.available = True
        except ImportError:
            self.plt = None
            self.np = None
            self.available = False
            print("Warning: matplotlib not available. Plotting functionality disabled.")
    
    def plot_spike_train(self, spike_data: List[int], title: str = "Spike Train"):
        """Plot spike train from spike count data"""
        if not self.available:
            raise RuntimeError("Plotting requires matplotlib")
        
        self.plt.figure(figsize=(12, 4))
        self.plt.plot(spike_data)
        self.plt.title(title)
        self.plt.xlabel("Step")
        self.plt.ylabel("Spike Count")
        self.plt.grid(True, alpha=0.3)
        self.plt.show()
    
    def plot_firing_rate(self, steps: int, firing_rates: List[float], title: str = "Firing Rate"):
        """Plot firing rate over time"""
        if not self.available:
            raise RuntimeError("Plotting requires matplotlib")
        
        self.plt.figure(figsize=(12, 4))
        self.plt.plot(range(steps), firing_rates)
        self.plt.title(title)
        self.plt.xlabel("Step")
        self.plt.ylabel("Firing Rate (Hz)")
        self.plt.grid(True, alpha=0.3)
        self.plt.show()
    
    def plot_learning_curves(self, learning_results: List[Dict[str, Any]],
                            metric: str = "spike_count"):
        """Plot learning curves from multiple experiments"""
        if not self.available:
            raise RuntimeError("Plotting requires matplotlib")
        
        if not learning_results:
            print("No learning results to plot")
            return
        
        self.plt.figure(figsize=(12, 8))
        
        # Plot multiple learning curves
        for i, result in enumerate(learning_results):
            steps = result.get("steps", len(result.get("spike_counts", [])))
            if metric == "spike_count":
                data = result.get("spike_counts", [])
            elif metric == "firing_rate":
                data = result.get("firing_rates", [])
            elif metric == "weight_change":
                data = result.get("weight_changes", [])
            else:
                continue
            
            self.plt.plot(range(steps), data, label=f"Experiment {i + 1}", alpha=0.7)
        
        self.plt.title(f"Learning Curves - {metric.replace('_', ' ').title()}")
        self.plt.xlabel("Step")
        self.plt.ylabel(metric.replace('_', ' ').title())
        self.plt.legend()
        self.plt.grid(True, alpha=0.3)
        self.plt.show()
    
    def plot_2d_world(self, world: SimpleWorld, title: str = "World State"):
        """Plot 2D world visualization"""
        if not self.available:
            raise RuntimeError("Plotting requires matplotlib")
        
        self.plt.figure(figsize=(10, 8))
        
        # Plot walls
        walls = world.get_wall_objects()
        for wall in walls:
            self.plt.add_patch(
                self.plt.Circle((wall.x, wall.y), wall.radius, 
                              color='gray', alpha=0.7)
            )
        
        # Plot resources
        resources = world.get_resource_objects()
        for resource in resources:
            self.plt.add_patch(
                self.plt.Circle((resource.x, resource.y), resource.radius,
                              color='green', alpha=0.7)
            )
        
        # Plot hazards
        hazards = world.get_hazard_objects()
        for hazard in hazards:
            self.plt.add_patch(
                self.plt.Circle((hazard.x, hazard.y), hazard.radius,
                              color='red', alpha=0.7)
            )
        
        # Plot agent
        agent = world.getAgentBody()
        self.plt.add_patch(
            self.plt.Circle((agent.x, agent.y), 0.3,
                          color='blue', alpha=0.9)
        )
        
        self.plt.xlim(0, world.getWidth())
        self.plt.ylim(0, world.getHeight())
        self.plt.title(title)
        self.plt.xlabel("X")
        self.plt.ylabel("Y")
        self.plt.grid(True, alpha=0.3)
        self.plt.gca().set_aspect('equal')
        self.plt.show()
    
    def create_animation(self, simulation: 'NLMSimulation', fps: int = 10):
        """Create animation of simulation"""
        if not self.available:
            raise RuntimeError("Plotting requires matplotlib")
        
        try:
            import matplotlib.animation as animation
            
            # Create frames
            frames = []
            for history in simulation._simulation_history:
                # Create frame for each episode
                fig, ax = self.plt.subplots(figsize=(10, 8))
                
                # Plot world state
                self.plot_2d_world(simulation.world, "")
                
                # Add episode text
                episode = history["episode"]
                ax.text(0.02, 0.98, f"Episode {episode['episode']}: "
                       f"Spikes={episode['final_spike_count']}, "
                       f"Duration={episode['duration']:.2f}s",
                       transform=ax.transAxes, fontsize=12,
                       bbox=dict(boxstyle="round", facecolor="white", alpha=0.8))
                
                frames.append(fig)
            
            # Create animation
            ani = animation.ArtistAnimation(figs=frames, 
                                           artists=[[ax] for ax in frames[0].axes.flat],
                                           interval=1000/fps, repeat=False)
            
            self.plt.show()
            
        except ImportError:
            print("Warning: matplotlib.animation not available")
class NLMDeveloper:
    """
    High-level development tools for NLM research and experimentation.
    Provides tools for development, testing, and research workflows.
    """
    
    def __init__(self):
        self.project_name = "NLM Project"
        self.version = "0.1.0"
        self.config_templates = {}
        self.benchmark_results = []
    
    def create_config_template(self, name: str, params: Dict[str, Any]):
        """Create a configuration template"""
        self.config_templates[name] = params
        print(f"Created config template: {name}")
        return self
    
    def get_config_template(self, name: str) -> Optional[Dict[str, Any]]:
        """Get a configuration template"""
        return self.config_templates.get(name)
    
    def create_experiment_template(self, name: str, config_name: str,
                                  experiment_type: str, steps: int) -> Dict[str, Any]:
        """Create an experiment template"""
        template = {
            "name": name,
            "config_template": config_name,
            "experiment_type": experiment_type,
            "steps": steps,
            "created_at": time.time(),
        }
        
        print(f"Created experiment template: {name}")
        return template
    
    def validate_config(self, config: Config) -> List[str]:
        """Validate configuration and return error messages"""
        errors = config.validate()
        if errors:
            print(f"Configuration validation failed: {errors}")
        else:
            print("Configuration validation passed")
        return errors
    
    def benchmark_brain(self, brain: Brain, steps: int, iterations: int = 5) -> Dict[str, Any]:
        """
        Benchmark brain performance.
        
        Args:
            brain: Brain to benchmark
            steps: Steps per benchmark run
            iterations: Number of benchmark runs
            
        Returns:
            Benchmark results
        """
        print(f"Benchmarking brain: {iterations} iterations of {steps} steps each")
        
        times = []
        spike_counts = []
        firing_rates = []
        
        for iteration in range(iterations):
            start_time = time.time()
            
            # Run simulation
            for step in range(steps):
                brain.injectCurrentToNeurons(NeuronType.Sensory, 20.0 + step % 50)
                brain.step(step, step * 0.001)
            
            end_time = time.time()
            
            times.append(end_time - start_time)
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
            
            # Reset brain for next iteration
            brain.reset()
            brain.initialize()
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        avg_spikes = sum(spike_counts) / len(spike_counts)
        avg_firing_rate = sum(firing_rates) / len(firing_rates)
        
        benchmark_result = {
            "brain_type": "NLM",
            "parameters": brain.getConfig().summary_dict(),
            "iterations": iterations,
            "steps_per_iteration": steps,
            "average_time_per_iteration": avg_time,
            "steps_per_second": steps / avg_time if avg_time > 0 else 0,
            "average_spike_count": avg_spikes,
            "average_firing_rate": avg_firing_rate,
            "spike_count_std": (sum((x - avg_spikes) ** 2 for x in spike_counts) / len(spike_counts)) ** 0.5
            if len(spike_counts) > 1 else 0.0,
            "times": times,
            "spike_counts": spike_counts,
            "firing_rates": firing_rates,
        }
        
        self.benchmark_results.append(benchmark_result)
        
        print(f"Benchmark completed:")
        print(f"  Average time per iteration: {avg_time:.3f}s")
        print(f"  Steps per second: {steps / avg_time:.1f}")
        print(f"  Average spike count: {avg_spikes:.1f}")
        print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
        
        return benchmark_result
    
    def compare_benchmarks(self, brain1: Brain, brain2: Brain, 
                          steps: int, iterations: int = 3) -> Dict[str, Any]:
        """
        Compare two brains with benchmark.
        
        Args:
            brain1, brain2: Brains to compare
            steps: Steps per benchmark run
            iterations: Number of benchmark runs
            
        Returns:
            Comparison results
        """
        print(f"Comparing two brains over {iterations} iterations of {steps} steps")
        
        benchmark1 = self.benchmark_brain(brain1, steps, iterations)
        benchmark2 = self.benchmark_brain(brain2, steps, iterations)
        
        comparison = {
            "brain1": benchmark1,
            "brain2": benchmark2,
            "comparison": {
                "time_ratio": benchmark2["average_time_per_iteration"] / benchmark1["average_time_per_iteration"],
                "spike_count_ratio": benchmark2["average_spike_count"] / benchmark1["average_spike_count"] if benchmark1["average_spike_count"] > 0 else 0,
                "firing_rate_ratio": benchmark2["average_firing_rate"] / benchmark1["average_firing_rate"] if benchmark1["average_firing_rate"] > 0 else 0,
            }
        }
        
        print(f"\nComparison results:")
        print(f"  Time ratio (B2/B1): {comparison['comparison']['time_ratio']:.2f}")
        print(f"  Spike count ratio (B2/B1): {comparison['comparison']['spike_count_ratio']:.2f}")
        print(f"  Firing rate ratio (B2/B1): {comparison['comparison']['firing_rate_ratio']:.2f}")
        
        return comparison
    
    def create_research_template(self, experiment_name: str,
                                 hypothesis: str, variables: Dict[str, Any]) -> Dict[str, Any]:
        """
        Create a research template for scientific experiments.
        
        Args:
            experiment_name: Name of experiment
            hypothesis: Research hypothesis
            variables: Variables to manipulate
            
        Returns:
            Research template
        """
        template = {
            "experiment_name": experiment_name,
            "hypothesis": hypothesis,
            "variables": variables,
            "created_at": time.time(),
            "status": "ready",
        }
        
        print(f"Created research template: {experiment_name}")
        return template
class Factory:
    """Factory class for creating common NLM objects"""
    
    @staticmethod
    def create_default_config() -> Config:
        """Create default configuration"""
        return Config()
    
    @staticmethod
    def create_brain(config: Optional[Config] = None) -> Brain:
        """Create brain with optional configuration"""
        return Brain(config) if config else Brain()
    
    @staticmethod
    def create_world(width: int = 800, height: int = 600,
                    vision_width: int = 80, vision_height: int = 80) -> SimpleWorld:
        """Create simple world with configuration"""
        world = SimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        return world
    
    @staticmethod
    def create_agent_brain(brain: Brain) -> AgentBrain:
        """Create agent brain interface"""
        return AgentBrain(brain)
class CommandFactory:
    """Command factory for creating complex actions"""
    
    @staticmethod
    def create_move_command(x: float, y: float, speed: float = 1.0) -> Action:
        """Create move action"""
        action = Action()
        action.set_named_parameter("x", x)
        action.set_named_parameter("y", y)
        action.set_named_parameter("speed", speed)
        return action
    
    @staticmethod
    def create_look_command(direction: float) -> Action:
        """Create look action"""
        action = Action()
        action.set_named_parameter("angle", direction)
        return action
    
    @staticmethod
    def create_custom_command(**kwargs) -> Action:
        """Create custom action with parameters"""
        action = Action()
        for key, value in kwargs.items():
            action.set_named_parameter(key, value)
        return action
# Global factory instances
factory = Factory()
commands = CommandFactory()
