"""
NLM Python Utilities - Helper functions for advanced NLM operations

This module provides utility functions to simplify common tasks when working with
the NLM Phase 6 integrated artificial brain system.
"""

import pynlm
import math
import json
from typing import List, Dict, Any, Optional, Callable
from enum import Enum

class SimulationMode(Enum):
    """Simulation modes for different behaviors"""
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    DEVELOPMENT = "development"

class BrainSimulator:
    """
    High-level simulator for NLM brains with predefined behaviors and monitoring.
    
    This class provides a convenient interface for running simulations with
    common patterns and tracking key metrics.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None, 
                 width: int = 20, height: int = 20):
        """
        Initialize the brain simulator.
        
        Args:
            config: NLM configuration (creates default if None)
            width: World width for visualization
            height: World height for visualization
        """
        if config is None:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = config
            
        # Create brain and world
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 8, 8)
        self.world.reset()
        self.world.setAgentStart(width // 2, height // 2)
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Simulation tracking
        self.step_count = 0
        self.episode_rewards = []
        self.brain_stats = []
        self.simulation_metrics = {}
        
        # Current episode
        self.current_episode = 0
        self.episode_start_step = 0
        
    def run_episode(self, max_steps: int = 1000, mode: SimulationMode = SimulationMode.BALANCED,
                   enable_monitoring: bool = True) -> Dict[str, Any]:
        """
        Run a single episode of simulation.
        
        Args:
            max_steps: Maximum steps per episode
            mode: Simulation behavior mode
            enable_monitoring: Whether to collect metrics
            
        Returns:
            Dictionary with episode results and metrics
        """
        self.current_episode += 1
        self.episode_start_step = self.step_count
        episode_reward = 0.0
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(self.step_count)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Get reward and apply reward modulation
            reward = self._extract_reward(percept)
            predicted_reward = self._get_predicted_reward()
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Update episode reward
            episode_reward += reward
            
            # Collect metrics
            if enable_monitoring:
                self._collect_metrics(step)
                
            # Adjust behavior based on mode
            self._adjust_behavior(mode, step)
            
            # Check for episode end
            if self._check_episode_end(step, max_steps):
                break
                
            self.step_count += 1
            
        # Store episode results
        self.episode_rewards.append(episode_reward)
        
        # Return episode results
        results = {
            'episode': self.current_episode,
            'steps': self.step_count - self.episode_start_step,
            'reward': episode_reward,
            'avg_reward_per_step': episode_reward / max(1, self.step_count - self.episode_start_step),
            'brain_stats': self.brain_stats[-1] if self.brain_stats else None,
            'simulation_metrics': self.simulation_metrics.copy()
        }
        
        return results
        
    def _extract_reward(self, percept) -> float:
        """Extract reward from sensory percept."""
        # Simple reward function: positive for resources, negative for hazards
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            return internal[0]  # First internal value as reward
        
        # Default reward based on vision
        vision = percept.getVision()
        if vision and len(vision) > 0:
            # Reward for seeing dark areas (resources in this example)
            return max(0.0, 1.0 - sum(vision) / len(vision))
            
        return 0.0
        
    def _get_predicted_reward(self) -> float:
        """Get predicted reward from agent's expectations."""
        return self.agent.getPredictionError() if hasattr(self.agent, 'getPredictionError') else 0.0
        
    def _collect_metrics(self, step: int) -> None:
        """Collect simulation metrics."""
        # Get brain statistics
        stats = {
            'step': self.step_count,
            'firing_rate': self.brain.getAverageFiringRate(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'neurons_active': self.brain.getActiveNeuronCount(),
            'development_stage': self.agent.getDevelopmentalStage() if hasattr(self.agent, 'getDevelopmentalStage') else None,
            'curiosity': self.agent.getCuriosityLevel() if hasattr(self.agent, 'getCuriosityLevel') else 0.0,
            'novelty': self.agent.getNoveltyLevel() if hasattr(self.agent, 'getNoveltyLevel') else 0.0,
            'prediction_error': self.agent.getPredictionError() if hasattr(self.agent, 'getPredictionError') else 0.0,
        }
        
        self.brain_stats.append(stats)
        
        # Update simulation metrics
        self.simulation_metrics['current_step'] = self.step_count
        self.simulation_metrics['avg_firing_rate'] = self._average('firing_rate')
        self.simulation_metrics['total_episodes'] = self.current_episode
        
    def _adjust_behavior(self, mode: SimulationMode, step: int) -> None:
        """Adjust agent behavior based on simulation mode."""
        if mode == SimulationMode.EXPLORATION:
            # Higher curiosity encourages exploration
            self.agent.enableCuriosity(True)
            if hasattr(self.agent, 'setCuriosityThreshold'):
                self.agent.setCuriosityThreshold(0.5)
                
        elif mode == SimulationMode.EXPLOITATION:
            # Lower curiosity encourages exploitation
            self.agent.enableCuriosity(False)
            if hasattr(self.agent, 'setCuriosityThreshold'):
                self.agent.setCuriosityThreshold(0.1)
                
        elif mode == SimulationMode.DEVELOPMENT:
            # Focus on development
            self.agent.enableDevelopment(True)
            if hasattr(self.agent, 'setDevelopmentSpeed'):
                self.agent.setDevelopmentSpeed(2.0)
                
        # Balanced mode uses default settings
        
    def _check_episode_end(self, step: int, max_steps: int) -> bool:
        """Check if episode should end."""
        # End if max steps reached
        if step >= max_steps - 1:
            return True
            
        # End if agent is stuck (no movement for some time)
        if hasattr(self, '_no_movement_count'):
            if self._no_movement_count > 50:  # 5 seconds at 0.1 timestep
                return True
        else:
            self._no_movement_count = 0
            
        # End if performance degrades too much (for long episodes)
        if self.current_episode > 1 and len(self.episode_rewards) >= 3:
            recent_rewards = self.episode_rewards[-3:]
            if all(r < 0.1 for r in recent_rewards):  # Very low rewards
                return True
                
        return False
        
    def _average(self, key: str) -> float:
        """Calculate average of a metric."""
        if not self.brain_stats:
            return 0.0
            
        values = [stats.get(key, 0.0) for stats in self.brain_stats if stats.get(key) is not None]
        return sum(values) / len(values) if values else 0.0
        
    def run_multiple_episodes(self, num_episodes: int, max_steps_per_episode: int = 1000,
                            mode: SimulationMode = SimulationMode.BALANCED) -> List[Dict[str, Any]]:
        """
        Run multiple episodes and return all results.
        
        Args:
            num_episodes: Number of episodes to run
            max_steps_per_episode: Maximum steps per episode
            mode: Simulation mode for all episodes
            
        Returns:
            List of episode results
        """
        all_results = []
        
        for episode in range(num_episodes):
            # Reset brain state between episodes
            self.reset()
            
            # Run episode
            results = self.run_episode(max_steps_per_episode, mode)
            all_results.append(results)
            
            # Print progress
            if (episode + 1) % 10 == 0:
                print(f"Completed {episode + 1}/{num_episodes} episodes")
                
        return all_results
        
    def reset(self) -> None:
        """Reset the simulator to initial state."""
        self.brain.reset()
        self.agent.reset()
        self.world.reset()
        
        # Reset tracking
        self.step_count = 0
        self.episode_start_step = 0
        self.brain_stats.clear()
        self.simulation_metrics.clear()
        
    def save_brain(self, filepath: str) -> bool:
        """Save brain state to file."""
        return self.brain.save(filepath)
        
    def load_brain(self, filepath: str) -> bool:
        """Load brain state from file."""
        return self.brain.load(filepath)
        
    def get_performance_report(self) -> Dict[str, Any]:
        """
        Generate performance report from collected metrics.
        
        Returns:
            Dictionary with performance analysis
        """
        if not self.brain_stats:
            return {'error': 'No metrics collected'}
            
        # Calculate performance statistics
        firing_rates = [stats.get('firing_rate', 0.0) for stats in self.brain_stats]
        curiosity_levels = [stats.get('curiosity', 0.0) for stats in self.brain_stats]
        prediction_errors = [stats.get('prediction_error', 0.0) for stats in self.brain_stats]
        
        report = {
            'total_steps': self.step_count,
            'total_episodes': self.current_episode,
            'avg_firing_rate': sum(firing_rates) / len(firing_rates) if firing_rates else 0.0,
            'max_firing_rate': max(firing_rates) if firing_rates else 0.0,
            'min_firing_rate': min(firing_rates) if firing_rates else 0.0,
            'avg_curiosity': sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0.0,
            'avg_prediction_error': sum(prediction_errors) / len(prediction_errors) if prediction_errors else 0.0,
            'episode_rewards': self.episode_rewards.copy(),
            'avg_episode_reward': sum(self.episode_rewards) / len(self.episode_rewards) if self.episode_rewards else 0.0,
            'simulation_metrics': self.simulation_metrics.copy()
        }
        
        return report

class ExperimentManager:
    """
    Manager for running and analyzing NLM experiments.
    
    This class provides functionality for setting up, running, and analyzing
    multiple NLM experiments with different configurations.
    """
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
        
    def add_experiment(self, name: str, simulator: BrainSimulator) -> None:
        """
        Add an experiment to the manager.
        
        Args:
            name: Unique name for the experiment
            simulator: BrainSimulator instance
        """
        self.experiments[name] = simulator
        
    def run_experiment(self, name: str, num_episodes: int = 10, 
                      max_steps_per_episode: int = 1000,
                      mode: SimulationMode = SimulationMode.BALANCED) -> Dict[str, Any]:
        """
        Run a specific experiment.
        
        Args:
            name: Experiment name
            num_episodes: Number of episodes to run
            max_steps_per_episode: Maximum steps per episode
            mode: Simulation mode
            
        Returns:
            Experiment results
        """
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
            
        simulator = self.experiments[name]
        results = simulator.run_multiple_episodes(num_episodes, max_steps_per_episode, mode)
        
        self.results[name] = results
        
        return {
            'experiment': name,
            'num_episodes': num_episodes,
            'total_steps': simulator.step_count,
            'results': results,
            'simulator': simulator
        }
        
    def compare_experiments(self, experiment_names: List[str]) -> Dict[str, Any]:
        """
        Compare multiple experiments.
        
        Args:
            experiment_names: List of experiment names to compare
            
        Returns:
            Comparison results
        """
        comparison = {}
        
        for name in experiment_names:
            if name not in self.results:
                print(f"Warning: Results for experiment '{name}' not available")
                continue
                
            results = self.results[name]
            simulator = self.experiments[name]
            
            performance_report = simulator.get_performance_report()
            
            comparison[name] = {
                'total_steps': performance_report['total_steps'],
                'num_episodes': performance_report['total_episodes'],
                'avg_firing_rate': performance_report['avg_firing_rate'],
                'avg_episode_reward': performance_report['avg_episode_reward'],
                'reward_std': self._calculate_std(performance_report['episode_rewards']),
                'performance_report': performance_report
            }
            
        return comparison
        
    def _calculate_std(self, values: List[float]) -> float:
        """Calculate standard deviation."""
        if len(values) < 2:
            return 0.0
            
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return math.sqrt(variance)
        
    def save_experiment_results(self, name: str, filepath: str) -> bool:
        """
        Save experiment results to file.
        
        Args:
            name: Experiment name
            filepath: Output file path
            
        Returns:
            True if successful
        """
        if name not in self.results:
            return False
            
        try:
            with open(filepath, 'w') as f:
                json.dump(self.results[name], f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving results: {e}")
            return False
            
    def load_experiment_results(self, name: str, filepath: str) -> bool:
        """
        Load experiment results from file.
        
        Args:
            name: Experiment name
            filepath: Input file path
            
        Returns:
            True if successful
        """
        try:
            with open(filepath, 'r') as f:
                self.results[name] = json.load(f)
            return True
        except Exception as e:
            print(f"Error loading results: {e}")
            return False

# Convenience functions for common tasks
def create_development_simulation(development_stages: int = 5, 
                                 neurons_per_stage: int = 200) -> BrainSimulator:
    """
    Create a brain simulator configured for development studies.
    
    Args:
        development_stages: Number of developmental stages to simulate
        neurons_per_stage: Number of neurons per developmental stage
        
    Returns:
        Configured BrainSimulator instance
    """
    config = pynlm.createAdvancedConfig()
    config.set("brain.neuron_count", neurons_per_stage * development_stages)
    config.set("brain.development.enabled", True)
    config.set("brain.enable_planner", True)
    config.set("brain.enable_concept_formation", True)
    config.set("brain.enable_attention", True)
    
    simulator = BrainSimulator(config)
    
    # Modify development timing
    if hasattr(simulator.agent, 'setDevelopmentSpeed'):
        simulator.agent.setDevelopmentSpeed(development_stages / 5.0)
        
    return simulator

def create_exploration_simulation(exploration_factor: float = 0.5) -> BrainSimulator:
    """
    Create a brain simulator optimized for exploration studies.
    
    Args:
        exploration_factor: Factor controlling exploration vs exploitation
        
    Returns:
        Configured BrainSimulator instance
    """
    config = pynlm.createAdvancedConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.development.enabled", True)
    config.set("brain.enable_planner", True)
    config.set("brain.enable_concept_formation", True)
    config.set("brain.enable_attention", True)
    
    simulator = BrainSimulator(config)
    
    # Configure for exploration
    if hasattr(simulator.agent, 'setCuriosityThreshold'):
        simulator.agent.setCuriosityThreshold(exploration_factor)
        
    return simulator

def create_exploitation_simulation(exploitation_factor: float = 0.3) -> BrainSimulator:
    """
    Create a brain simulator optimized for exploitation studies.
    
    Args:
        exploitation_factor: Factor controlling exploitation behavior
        
    Returns:
        Configured BrainSimulator instance
    """
    config = pynlm.createAdvancedConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.development.enabled", False)  # Stable for exploitation
    config.set("brain.enable_planner", True)
    config.set("brain.enable_concept_formation", True)
    config.set("brain.enable_attention", True)
    
    simulator = BrainSimulator(config)
    
    # Configure for exploitation
    if hasattr(simulator.agent, 'setCuriosityThreshold'):
        simulator.agent.setCuriosityThreshold(exploitation_factor * 0.5)  # Low exploration
        
    return simulator

def run_benchmark_suite(width: int = 20, height: int = 20) -> Dict[str, Any]:
    """
    Run a comprehensive benchmark suite.
    
    Args:
        width: World width
        height: World height
        
    Returns:
        Dictionary with benchmark results
    """
    manager = ExperimentManager()
    
    # Add development experiment
    dev_simulator = create_development_simulation()
    manager.add_experiment("development", dev_simulator)
    
    # Add exploration experiment
    explore_simulator = create_exploration_simulation(0.8)
    manager.add_experiment("exploration", explore_simulator)
    
    # Add exploitation experiment
    exploit_simulator = create_exploitation_simulation(0.2)
    manager.add_experiment("exploitation", exploit_simulator)
    
    # Run all experiments
    for name in manager.experiments:
        print(f"Running experiment: {name}")
        manager.run_experiment(name, num_episodes=5, max_steps_per_episode=500)
        
    # Compare experiments
    comparison = manager.compare_experiments(list(manager.experiments.keys()))
    
    return {
        'experiments': manager.experiments,
        'results': manager.results,
        'comparison': comparison
    }

# Visualization helper functions
def plot_simulation_metrics(simulator: BrainSimulator, metrics: List[str] = None):
    """
    Plot simulation metrics (placeholder for actual plotting).
    
    Args:
        simulator: BrainSimulator instance
        metrics: List of metrics to plot
    """
    if metrics is None:
        metrics = ['firing_rate', 'curiosity', 'prediction_error']
        
    print(f"Plotting metrics for simulation with {len(simulator.brain_stats)} data points")
    
    for metric in metrics:
        values = [stats.get(metric, 0.0) for stats in simulator.brain_stats]
        if values:
            print(f"  {metric}: min={min(values):.3f}, max={max(values):.3f}, avg={sum(values)/len(values):.3f}")
            
    return True

def save_simulation_data(simulator: BrainSimulator, filepath: str) -> bool:
    """
    Save simulation data to file.
    
    Args:
        simulator: BrainSimulator instance
        filepath: Output file path
        
    Returns:
        True if successful
    """
    try:
        data = {
            'brain_stats': simulator.brain_stats,
            'episode_rewards': simulator.episode_rewards,
            'simulation_metrics': simulator.simulation_metrics,
            'total_steps': simulator.step_count,
            'current_episode': simulator.current_episode
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
            
        return True
    except Exception as e:
        print(f"Error saving simulation data: {e}")
        return False

def load_simulation_data(filepath: str) -> Optional[Dict[str, Any]]:
    """
    Load simulation data from file.
    
    Args:
        filepath: Input file path
        
    Returns:
        Loaded data or None if error
    """
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading simulation data: {e}")
        return None

# Example usage functions
def example_development_simulation():
    """
    Example of running a development simulation.
    
    This example demonstrates how to use the BrainSimulator
    to study developmental processes in NLM brains.
    """
    print("Running development simulation example...")
    
    # Create development simulator
    simulator = create_development_simulation(development_stages=3, neurons_per_stage=100)
    
    # Run episodes
    results = simulator.run_multiple_episodes(3, max_steps_per_episode=200)
    
    # Print results
    for episode_result in results:
        print(f"Episode {episode_result['episode']}: {episode_result['steps']} steps, "
              f"reward={episode_result['reward']:.2f}, "
              f"avg_reward={episode_result['avg_reward_per_step']:.4f}")
    
    # Generate performance report
    report = simulator.get_performance_report()
    print(f"\nPerformance Report:")
    print(f"  Total steps: {report['total_steps']}")
    print(f"  Average firing rate: {report['avg_firing_rate']:.3f} Hz")
    print(f"  Average episode reward: {report['avg_episode_reward']:.3f}")
    
    return simulator, results

def example_exploration_vs_exploitation():
    """
    Example comparing exploration vs exploitation behaviors.
    
    This example demonstrates how to compare different simulation modes.
    """
    print("\nRunning exploration vs exploitation comparison...")
    
    # Create simulators with different modes
    explore_simulator = create_exploration_simulation(0.8)
    exploit_simulator = create_exploitation_simulation(0.2)
    
    # Run experiments with different modes
    explore_results = explore_simulator.run_multiple_episodes(3, max_steps_per_episode=300)
    exploit_results = exploit_simulator.run_multiple_episodes(3, max_steps_per_episode=300)
    
    # Compare results
    explore_rewards = [r['reward'] for r in explore_results]
    exploit_rewards = [r['reward'] for r in exploit_results]
    
    print(f"\nExploration results:")
    print(f"  Average reward: {sum(explore_rewards)/len(explore_rewards):.3f}")
    print(f"  Reward std: {sum((r - sum(explore_rewards)/len(explore_rewards))**2 for r in explore_rewards)/len(explore_rewards)**0.5:.3f}")
    
    print(f"\nExploitation results:")
    print(f"  Average reward: {sum(exploit_rewards)/len(exploit_rewards):.3f}")
    print(f"  Reward std: {sum((r - sum(exploit_rewards)/len(exploit_rewards))**2 for r in exploit_rewards)/len(exploit_rewards)**0.5:.3f}")
    
    return {
        'explore_simulator': explore_simulator,
        'exploit_simulator': exploit_simulator,
        'explore_results': explore_results,
        'exploit_results': exploit_results
    }

# Main block for demonstration
def main():
    """
    Demonstration of NLM Python utilities.
    
    This function shows examples of using the NLM utilities
    for different types of experiments and analyses.
    """
    print("NLM Python Utilities Demonstration")
    print("=" * 50)
    
    # Example 1: Development simulation
    simulator1, results1 = example_development_simulation()
    
    # Example 2: Exploration vs Exploitation comparison
    comparison_results = example_exploration_vs_exploitation()
    
    # Run comprehensive benchmark suite
    print("\n" + "=" * 50)
    print("Running comprehensive benchmark suite...")
    benchmark_results = run_benchmark_suite()
    
    print("\nBenchmark Results Summary:")
    for experiment_name, comparison_data in benchmark_results['comparison'].items():
        print(f"  {experiment_name}:")
        print(f"    Total steps: {comparison_data['total_steps']}")
        print(f"    Avg firing rate: {comparison_data['avg_firing_rate']:.3f} Hz")
        print(f"    Avg episode reward: {comparison_data['avg_episode_reward']:.3f}")
    
    print("\n" + "=" * 50)
    print("NLM Python utilities demonstration complete!")
    
    return benchmark_results

if __name__ == "__main__":
    # Run demonstration if executed as script
    main()