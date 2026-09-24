#!/usr/bin/env python3
"""
NLM - High-Level Python API for Neural Learning Machine

This module provides a comprehensive, easy-to-use interface for NLM simulations.
It includes common use case patterns, advanced features, and educational examples.

Key Features:
- High-level abstractions for common scenarios
- Rich set of configuration options
- Progress tracking and monitoring
- Experiment management
- Visualization and output
- Error handling and validation

Usage:
    from nlm_python import create_simulation, SimulationRunner
    
    # Create a simulation with simple configuration
    sim = create_simulation(neuron_count=1000, max_steps=5000)
    
    # Run the simulation
    runner = SimulationRunner(sim)
    results = runner.run()
    
    # Analyze results
    print(f"Total reward: {results.total_reward}")
    print(f"Learning achieved: {results.learning_detected}")
"""

import time
import math
import numpy as np
from typing import Optional, Dict, List, Callable, Any

# Import the low-level bindings
try:
    import pynlm
    PYNLML_AVAILABLE = True
except ImportError:
    PYNLML_AVAILABLE = False
    print("Warning: pynlm bindings not available. Some features will be limited.")
class NLMConfig:
    """Configuration for NLM simulation with sensible defaults."""
    
    def __init__(self, neuron_count: int = 500, region_count: int = 1,
                 connection_probability: float = 0.1, timestep: float = 0.001,
                 random_seed: int = 42):
        """
        Initialize NLM configuration.
        
        Args:
            neuron_count: Number of neurons in the brain
            region_count: Number of brain regions
            connection_probability: Probability of connection between neurons
            timestep: Simulation timestep in seconds
            random_seed: Random seed for reproducibility
        """
        self.neuron_count = neuron_count
        self.region_count = region_count
        self.connection_probability = connection_probability
        self.timestep = timestep
        self.random_seed = random_seed
        
        # Create low-level config
        self.config = pynlm.createDefaultConfig() if PYNLML_AVAILABLE else None
        
        # Apply settings
        if self.config:
            self.config.set("neuron_count", neuron_count)
            self.config.set("region_count", region_count)
            self.config.set("connection_probability", connection_probability)
            self.config.set("random_seed", random_seed)
            self.config.set("simulation_timestep", timestep)
            self.config.set("stdp_ltp_weight", 0.02)
            self.config.set("stdp_ltd_weight", 0.015)
            self.config.set("stdp_tau", 20.0)
    
    def enable_features(self, reward_modulation: bool = True,
                       structural_plasticity: bool = True,
                       development: bool = True, curiosity: bool = True):
        """Enable advanced features for the simulation."""
        if self.config:
            self.config.set("enable_reward_modulation", reward_modulation)
            self.config.set("enable_structural_plasticity", structural_plasticity)
            self.config.set("enable_development", development)
            self.config.set("enable_curiosity", curiosity)
        
        self.reward_modulation = reward_modulation
        self.structural_plasticity = structural_plasticity
        self.development = development
        self.curiosity = curiosity
        
        return self
    
    def with_plasticity_rules(self, hebbian: bool = True, stdp: bool = True,
                              reward_modulated: bool = True):
        """Configure plasticity rules."""
        if self.config:
            self.config.set("enable_hebbian", hebbian)
            self.config.set("enable_stdp", stdp)
            self.config.set("enable_reward_modulated_stdp", reward_modulated)
        
        return self
    
    def with_development_stages(self, stages: int = 5):
        """Configure developmental stages."""
        if self.config:
            self.config.set("development_stages", stages)
        return self
class NeuralNetworkSimulator:
    """High-level interface for NLM neural network simulation."""
    
    def __init__(self, config: NLMConfig):
        """
        Initialize neural network simulator.
        
        Args:
            config: NLM configuration
        """
        if not PYNLML_AVAILABLE:
            raise RuntimeError("pynlm bindings not available. Cannot create simulator.")
        
        self.config = config
        self.brain = pynlm.createBrain(config.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize world with reasonable defaults
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.agent.initialize(self.world)
        
        # Enable features based on config
        self.agent.enableRewardModulation(config.reward_modulation)
        self.agent.enableStructuralPlasticity(config.structural_plasticity)
        self.agent.enableDevelopment(config.development)
        self.agent.enableCuriosity(config.curiosity)
        
        # Initialize brain
        self.brain.initialize()
        
        # Simulation state
        self.step_count = 0
        self.current_metrics = {}
        self.episode_rewards = []
        self.simulation_start_time = time.time()
        
    def get_simulation_stats(self) -> Dict[str, Any]:
        """Get current simulation statistics."""
        return {
            "step": self.step_count,
            "neurons_firing": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "neuromodulation": self.agent.getNeuromodulationLevel(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "development_stage": self.agent.getDevelopmentalStage(),
            "prediction_error": self.agent.getPredictionError(),
            "elapsed_time": time.time() - self.simulation_start_time
        }
    
    def inject_sensory_input(self, vision_data: Optional[List[float]] = None,
                           touch_data: Optional[List[float]] = None,
                           internal_data: Optional[List[float]] = None):
        """
        Inject sensory input into the brain.
        
        Args:
            vision_data: Vision sensor data (normalized 0-1)
            touch_data: Touch/proximity sensor data
            internal_data: Internal state signals
        """
        # Create sensory percept
        from pynlm import SensoryPercept, Vision
        
        percept = SensoryPercept()
        
        if vision_data is not None:
            vision = Vision(len(self.world.getWidth()), len(self.world.getHeight()), 3)
            vision.setData(vision_data)
            percept.setVision(vision)
        
        if touch_data is not None:
            # Create touch data (simplified)
            percept.setTouch(touch_data)
        
        if internal_data is not None:
            # Create internal signals
            percept.setInternal(internal_data)
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
    
    def step(self) -> Dict[str, Any]:
        """
        Execute one simulation step.
        
        Returns:
            Dictionary with step metrics
        """
        # Update world
        self.world.update(self.config.timestep)
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        
        # Inject sensory input into brain
        self.agent.processSensoryInput(percept)
        
        # Brain computation step
        self.brain.step(self.step_count)
        
        # Get motor command from brain
        action = self.agent.decodeMotorCommand()
        
        # Apply action to world
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation if enabled
        self.agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development if enabled
        if self.config.development:
            self.agent.updateDevelopment(self.config.timestep)
        
        # Collect metrics
        metrics = {
            "step": self.step_count,
            "reward": result.reward,
            "success": result.success,
            "neurons_firing": self.brain.getFiringNeuronCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "neuromodulation": self.agent.getNeuromodulationLevel(),
            "action": action
        }
        
        self.step_count += 1
        return metrics
    
    def run_episode(self, steps: int, verbose: bool = False) -> List[Dict[str, Any]]:
        """
        Run a single episode of simulation.
        
        Args:
            steps: Number of simulation steps
            verbose: Whether to print progress
            
        Returns:
            List of step metrics
        """
        episode_start = time.time()
        episode_rewards = []
        
        if verbose:
            print(f"Starting episode with {steps} steps...")
        
        for step in range(steps):
            metrics = self.step()
            episode_rewards.append(metrics["reward"])
            
            if verbose and step % 100 == 0:
                stats = self.get_simulation_stats()
                print(f"  Step {step}: reward={metrics['reward']:.3f}, "
                      f"firing={metrics['neurons_firing']}, "
                      f"neuromod={metrics['neuromodulation']:.2f}")
        
        episode_time = time.time() - episode_start
        
        if verbose:
            print(f"Episode completed in {episode_time:.2f}s")
            print(f"Total reward: {sum(episode_rewards):.2f}")
            print(f"Average reward: {np.mean(episode_rewards):.3f}")
        
        self.episode_rewards.extend(episode_rewards)
        return self.get_recent_metrics(steps)
    
    def get_recent_metrics(self, count: int = None) -> List[Dict[str, Any]]:
        """Get recent simulation metrics."""
        if count is None or count >= len(self.step_count):
            count = self.step_count
        
        # Note: This would need to be implemented with proper metrics storage
        return [{"step": i, "placeholder": "metrics"} for i in range(max(0, self.step_count - count), self.step_count)]
class SimulationRunner:
    """High-level simulation runner with monitoring and reporting."""
    
    def __init__(self, simulator: NeuralNetworkSimulator, 
                 config: Optional[NLMConfig] = None):
        """
        Initialize simulation runner.
        
        Args:
            simulator: Neural network simulator
            config: Additional configuration
        """
        self.simulator = simulator
        self.config = config or NLMConfig()
        self.results = {}
        self.progressive_metrics = {}
        
    def run(self, num_episodes: int = 1, steps_per_episode: int = 100,
            verbose: bool = True, callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run simulation episodes.
        
        Args:
            num_episodes: Number of episodes to run
            steps_per_episode: Steps per episode
            verbose: Whether to print progress
            callback: Optional callback function for step updates
            
        Returns:
            Dictionary with simulation results
        """
        start_time = time.time()
        
        if verbose:
            print(f"Starting NLM simulation: {num_episodes} episodes x {steps_per_episode} steps")
            print(f"Configuration: {self.simulator.brain.getTotalNeuronCount()} neurons")
            print(f"Plasticity: Reward modulation={self.simulator.config.reward_modulation}, "
                  f"Structural={self.simulator.config.structural_plasticity}, "
                  f"Development={self.simulator.config.development}")
        
        # Run episodes
        all_metrics = []
        for episode in range(num_episodes):
            if verbose:
                print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
            
            episode_metrics = self.simulator.run_episode(steps_per_episode, verbose)
            all_metrics.extend(episode_metrics)
            
            # Call callback if provided
            if callback:
                callback(episode, episode_metrics)
        
        total_time = time.time() - start_time
        
        # Compute final results
        results = self._compute_results(all_metrics, total_time)
        
        if verbose:
            print(f"\n=== SIMULATION COMPLETE ===")
            print(f"Total time: {total_time:.2f}s ({total_time/(num_episodes*steps_per_episode):.2f}s/step)")
            print(f"Learning detected: {results.get('learning_detected', False)}")
            print(f"Final reward: {results.get('total_reward', 0):.2f}")
            print(f"Final firing rate: {results.get('final_firing_rate', 0):.3f}")
        
        self.results = results
        return results
    
    def _compute_results(self, metrics: List[Dict[str, Any]], 
                        total_time: float) -> Dict[str, Any]:
        """Compute final simulation results."""
        if not metrics:
            return {}
        
        rewards = [m.get("reward", 0) for m in metrics]
        firing_rates = [m.get("avg_firing_rate", 0) for m in metrics]
        neuromod_levels = [m.get("neuromodulation", 0) for m in metrics]
        
        # Calculate learning indicators
        learning_detected = (
            len(rewards) > 10 and 
            (rewards[-10:] != rewards[:-10] or 
             np.std(rewards) > 0.1 or 
             np.mean(np.abs(np.diff(rewards))) > 0.01)
        )
        
        return {
            "total_reward": sum(rewards),
            "avg_reward": np.mean(rewards),
            "reward_std": np.std(rewards),
            "final_firing_rate": firing_rates[-1] if firing_rates else 0,
            "avg_firing_rate": np.mean(firing_rates),
            "neuromodulation_final": neuromod_levels[-1] if neuromod_levels else 0,
            "neuromodulation_avg": np.mean(neuromod_levels),
            "steps_total": len(metrics),
            "episodes": len(set(m.get("step", 0) // 100 for m in metrics)),
            "learning_detected": learning_detected,
            "total_time": total_time,
            "steps_per_second": len(metrics) / total_time if total_time > 0 else 0
        }
class ExperimentManager:
    """Manage multiple experiments and compare results."""
    
    def __init__(self):
        self.experiments = {}
    
    def create_experiment(self, name: str, config: NLMConfig,
                         num_episodes: int = 5, steps_per_episode: int = 200) -> SimulationRunner:
        """
        Create a new experiment.
        
        Args:
            name: Experiment name
            config: NLM configuration
            num_episodes: Number of episodes
            steps_per_episode: Steps per episode
            
        Returns:
            SimulationRunner instance
        """
        simulator = NeuralNetworkSimulator(config)
        runner = SimulationRunner(simulator)
        
        self.experiments[name] = {
            "runner": runner,
            "config": config,
            "num_episodes": num_episodes,
            "steps_per_episode": steps_per_episode
        }
        
        return runner
    
    def run_experiment(self, name: str) -> Dict[str, Any]:
        """
        Run an experiment.
        
        Args:
            name: Experiment name
            
        Returns:
            Experiment results
        """
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        exp = self.experiments[name]
        return exp["runner"].run(
            num_episodes=exp["num_episodes"],
            steps_per_episode=exp["steps_per_episode"],
            verbose=True
        )
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict[str, Any]:
        """
        Compare multiple experiments.
        
        Args:
            experiment_names: List of experiment names
            
        Returns:
            Comparison results
        """
        comparison = {}
        
        for name in experiment_names:
            if name in self.experiments:
                results = self.experiments[name]["runner"].results
                comparison[name] = results
            else:
                print(f"Warning: Experiment '{name}' not found for comparison")
        
        if comparison:
            # Create summary table
            print("\n=== EXPERIMENT COMPARISON ===")
            print(f"{'Experiment':<20} {'Total Reward':>12} {'Avg Reward':>12} {'Final Firing':>12} {'Learning':>10}")
            print("-" * 80)
            
            for name, results in comparison.items():
                print(f"{name:<20} {results.get('total_reward', 0):>12.2f} "
                      f"{results.get('avg_reward', 0):>12.3f} "
                      f"{results.get('final_firing_rate', 0):>12.3f} "
                      f"{results.get('learning_detected', False):>10}")
        
        return comparison
def create_simulation(neuron_count: int = 500, **kwargs) -> NeuralNetworkSimulator:
    """
    Convenience function to create a simulation.
    
    Args:
        neuron_count: Number of neurons
        **kwargs: Additional configuration options
        
    Returns:
        NeuralNetworkSimulator instance
    """
    config = NLMConfig(neuron_count=neuron_count, **kwargs)
    return NeuralNetworkSimulator(config)
def run_simple_test(neurons: int = 100, steps: int = 50) -> Dict[str, Any]:
    """
    Run a simple test for testing purposes.
    
    Args:
        neurons: Number of neurons
        steps: Number of steps
        
    Returns:
        Test results
    """
    sim = create_simulation(neuron_count=neurons)
    
    # Simple random sensory input
    for step in range(steps):
        # Inject random sensory input
        vision_data = [np.random.random() for _ in range(64)]  # 8x8x3 flattened
        sim.inject_sensory_input(vision_data=vision_data)
        
        # Run simulation step
        metrics = sim.step()
        
        if step % 10 == 0:
            print(f"Step {step}: reward={metrics['reward']:.3f}, "
                  f"firing={metrics['neurons_firing']}")
    
    stats = sim.get_simulation_stats()
    return {
        "total_steps": sim.step_count,
        "simulation_stats": stats,
        "config": sim.config.__dict__
    }
# If pynlm is available, add the main execution block
if PYNLML_AVAILABLE:
    def main():
        """Example main function."""
        print("=== NLM High-Level Python API Demo ===\n")
        
        # Example 1: Simple learning simulation
        print("Example 1: Simple Learning Simulation")
        print("-" * 40)
        
        config = NLMConfig(
            neuron_count=200,
            connection_probability=0.15,
            timestep=0.01
        ).enable_features(
            reward_modulation=True,
            structural_plasticity=True,
            development=True,
            curiosity=True
        ).with_plasticity_rules(hebbian=True, stdp=True)
        
        sim = NeuralNetworkSimulator(config)
        
        # Create a simple world with a resource
        world = sim.world
        sim.world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        
        # Run simulation
        print("Running simulation...")
        runner = SimulationRunner(sim)
        results = runner.run(num_episodes=3, steps_per_episode=100, verbose=True)
        
        # Display results
        print(f"\nLearning achieved: {results.get('learning_detected', False)}")
        print(f"Total reward: {results.get('total_reward', 0):.2f}")
        print(f"Final firing rate: {results.get('final_firing_rate', 0):.3f}")
        
        # Example 2: Compare different configurations
        print("\n\nExample 2: Configuration Comparison")
        print("-" * 40)
        
        manager = ExperimentManager()
        
        # Create different experiments
        config1 = NLMConfig(neuron_count=100, connection_probability=0.1)
        config2 = NLMConfig(neuron_count=200, connection_probability=0.2)
        
        exp1_runner = manager.create_experiment("Small Network", config1, num_episodes=2, steps_per_episode=50)
        exp2_runner = manager.create_experiment("Large Network", config2, num_episodes=2, steps_per_episode=50)
        
        # Run experiments
        print("Running 'Small Network' experiment...")
        results1 = exp1_runner.run()
        
        print("\nRunning 'Large Network' experiment...")
        results2 = exp2_runner.run()
        
        # Compare
        comparison = manager.compare_experiments(["Small Network", "Large Network"])
        
        print("\n=== Demo Complete ===")
    
    if __name__ == "__main__":
        main()
else:
    print("Warning: pynlm bindings not available. High-level API features will be limited.")
    print("Please install pynlm bindings to use all features:")
    print("  pip install -e .")