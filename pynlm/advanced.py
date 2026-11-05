#!/usr/bin/env python3
"""
Advanced NLM Python Interface

High-level Python interface for NLM with enhanced functionality,
comprehensive error handling, and modern Python APIs.
"""

import time
import logging
from typing import Dict, List, Optional, Union, Any, Callable, Iterator
from dataclasses import dataclass, field, asdict
from enum import Enum
from contextlib import contextmanager
import numpy as np
from pathlib import Path

from pynlm import _TypeValidator, get_config, set_config, NLMConfig
from pynlm.simulation import Simulation, SimulationStatus, EventType

logger = logging.getLogger(__name__)


class EnhancedBrainWrapper:
    """Enhanced wrapper for NLM brain with improved Python interface."""
    
    def __init__(self, brain: Any):
        """
        Initialize enhanced brain wrapper.
        
        Args:
            brain: NLM brain instance
        """
        self._brain = brain
        self._config = brain.getConfig()
        self._neuron_cache = None
        self._synapse_cache = None
    
    def get_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    def get_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get firing neuron count."""
        return self._brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    def get_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    def get_developmental_stage(self) -> str:
        """Get developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def get_config_summary(self) -> Dict[str, Any]:
        """Get configuration summary."""
        return {
            "neuron_count": self.get_neuron_count(),
            "synapse_count": self.get_synapse_count(),
            "region_count": self._brain.getRegionCount(),
            "simulation_timestep": self._config.simulation_timestep,
            "random_seed": self._config.random_seed,
        }
    
    def get_performance_stats(self) -> Dict[str, float]:
        """Get performance statistics."""
        return {
            "steps_per_second": 1.0 / self._config.simulation_timestep,
            "neurons_per_second": self.get_neuron_count() / self._config.simulation_timestep,
            "synapses_per_neuron": self.get_synapse_count() / self.get_neuron_count() if self.get_neuron_count() > 0 else 0,
        }
    
    def run_steps(self, num_steps: int) -> List[Dict[str, Any]]:
        """
        Run multiple steps and return statistics.
        
        Args:
            num_steps: Number of steps to run
            
        Returns:
            List of statistics for each step
        """
        stats = []
        
        for step in range(num_steps):
            self._brain.step(step)
            
            step_stats = {
                "step": step,
                "spike_count": self.get_spike_count(),
                "firing_neurons": self.get_firing_neuron_count(),
                "firing_rate": self.get_average_firing_rate(),
                "e_i_ratio": self.get_excitation_inhibition_ratio(),
                "developmental_stage": self.get_developmental_stage(),
            }
            
            stats.append(step_stats)
        
        return stats
    
    def get_activity_patterns(self, num_steps: int = 100) -> Dict[str, np.ndarray]:
        """
        Get activity patterns over time.
        
        Args:
            num_steps: Number of steps to analyze
            
        Returns:
            Dictionary of activity patterns
        """
        spike_history = []
        firing_rates = []
        e_i_ratios = []
        
        for step in range(num_steps):
            self._brain.step(step)
            spike_history.append(self.get_spike_count())
            firing_rates.append(self.get_average_firing_rate())
            e_i_ratios.append(self.get_excitation_inhibition_ratio())
        
        return {
            "spike_count": np.array(spike_history),
            "firing_rate": np.array(firing_rates),
            "e_i_ratio": np.array(e_i_ratios),
            "step": np.arange(num_steps)
        }
    
    def find_active_regions(self, threshold: float = 0.1) -> List[Dict[str, Any]]:
        """
        Find active brain regions.
        
        Args:
            threshold: Activity threshold for region activation
            
        Returns:
            List of active regions with statistics
        """
        regions = []
        
        for region_id in self._brain.getRegionIds():
            region = self._brain.getRegion(region_id)
            
            if region:
                region_neuron_count = region.getAllNeurons()
                firing_count = sum(1 for neuron in region_neuron_count if neuron.isFiring())
                total_count = len(region_neuron_count)
                
                if total_count > 0:
                    firing_rate = firing_count / total_count
                    
                    if firing_rate >= threshold:
                        regions.append({
                            "region_id": region_id,
                            "neuron_count": total_count,
                            "firing_count": firing_count,
                            "firing_rate": firing_rate,
                            "activity": "high" if firing_rate > 0.5 else "medium" if firing_rate > 0.1 else "low"
                        })
        
        return regions
    
    def analyze_stability(self, num_steps: int = 100) -> Dict[str, Any]:
        """
        Analyze simulation stability.
        
        Args:
            num_steps: Number of steps to analyze for stability
            
        Returns:
            Stability analysis results
        """
        firing_rates = []
        spike_counts = []
        
        for step in range(num_steps):
            self._brain.step(step)
            firing_rates.append(self.get_average_firing_rate())
            spike_counts.append(self.get_spike_count())
        
        # Calculate stability metrics
        firing_rate_std = np.std(firing_rates) if firing_rates else 0
        firing_rate_mean = np.mean(firing_rates) if firing_rates else 0
        spike_count_std = np.std(spike_counts) if spike_counts else 0
        spike_count_mean = np.mean(spike_counts) if spike_counts else 0
        
        # Stability classification
        if firing_rate_std < 0.1 * firing_rate_mean and spike_count_std < 0.1 * spike_count_mean:
            stability = "stable"
        elif firing_rate_std < 0.5 * firing_rate_mean and spike_count_std < 0.5 * spike_count_mean:
            stability = "moderate"
        else:
            stability = "unstable"
        
        return {
            "stability": stability,
            "firing_rate": {
                "mean": firing_rate_mean,
                "std": firing_rate_std,
                "cv": firing_rate_std / firing_rate_mean if firing_rate_mean > 0 else 0
            },
            "spike_count": {
                "mean": spike_count_mean,
                "std": spike_count_std,
                "cv": spike_count_std / spike_count_mean if spike_count_mean > 0 else 0
            },
            "recommendations": self._generate_stability_recommendations(stability, firing_rate_std, spike_count_std)
        }
    
    def _generate_stability_recommendations(self, stability: str, firing_rate_std: float, spike_count_std: float) -> List[str]:
        """Generate stability recommendations."""
        recommendations = []
        
        if stability == "unstable":
            recommendations.extend([
                "Reduce network size for more stable dynamics",
                "Increase synaptic decay rates",
                "Add more inhibitory connections",
                "Reduce initial weight variance"
            ])
        
        elif stability == "moderate":
            recommendations.extend([
                "Consider adjusting neuron parameters",
                "Review connectivity patterns",
                "Check neuromodulation settings"
            ])
        
        else:  # stable
            recommendations.extend([
                "Good stability achieved - maintain current parameters",
                "Consider exploring new learning scenarios",
                "Experiment with different connectivity patterns"
            ])
        
        return recommendations
    
    def get_plasticity_summary(self) -> Dict[str, Any]:
        """Get plasticity system summary."""
        return {
            "stdp_enabled": self._config.stdp_enable,
            "hebbian_enabled": self._config.hebbian_enable,
            "structural_enabled": self._config.structural_enable,
            "reward_modulation_enabled": self._config.reward_modulation_enabled,
        }


class AdvancedNLM:
    """Advanced NLM interface with high-level abstractions."""
    
    def __init__(self, config: Optional[Union[Dict, NLMConfig]] = None):
        """
        Initialize advanced NLM interface.
        
        Args:
            config: Configuration (dict or NLMConfig)
        """
        # Set global configuration
        if config:
            if isinstance(config, dict):
                config_obj = NLMConfig(**config)
            else:
                config_obj = config
            
            set_config(config_obj)
        
        # Initialize brain
        self.brain = self._create_brain()
        self.enhanced_brain = EnhancedBrainWrapper(self.brain)
        self.simulations: Dict[str, Simulation] = {}
        
        logger.info("Advanced NLM interface initialized")
    
    def _create_brain(self) -> Any:
        """Create NLM brain."""
        from pynlm import _pynlm
        return _pynlm.createBrain(get_config().get_config())
    
    def create_simulation(self, name: str, **kwargs) -> Simulation:
        """
        Create simulation.
        
        Args:
            name: Simulation name
            **kwargs: Simulation configuration
            
        Returns:
            Simulation instance
        """
        simulation = Simulation(self.brain, **kwargs)
        self.simulations[name] = simulation
        return simulation
    
    def run_experiment(self, experiment_name: str, steps: int, 
                      enable_learning: bool = True,
                      enable_development: bool = True) -> Dict[str, Any]:
        """
        Run a complete experiment.
        
        Args:
            experiment_name: Experiment name
            steps: Number of steps to run
            enable_learning: Enable learning mechanisms
            enable_development: Enable development mechanisms
            
        Returns:
            Experiment results
        """
        # Configure experiment
        if enable_learning:
            self.brain.enableRewardModulation(True)
            self.brain.enableStructuralPlasticity(True)
        
        if enable_development:
            self.brain.enableDevelopment(True)
        
        # Create simulation
        simulation = self.create_simulation(experiment_name, max_steps=steps)
        
        # Run experiment
        start_time = time.time()
        simulation.run(steps)
        elapsed_time = time.time() - start_time
        
        # Get results
        results = {
            "experiment_name": experiment_name,
            "steps_completed": simulation.current_step,
            "elapsed_time": elapsed_time,
            "steps_per_second": steps / elapsed_time if elapsed_time > 0 else 0,
            "final_brain_state": self.enhanced_brain.get_config_summary(),
            "plasticity_summary": self.enhanced_brain.get_plasticity_summary(),
            "activity_analysis": self.enhanced_brain.analyze_stability(steps),
        }
        
        # Export data
        simulation.export_events(f"{experiment_name}_events.json")
        simulation.export_metrics(f"{experiment_name}_metrics.json")
        
        return results
    
    def train_brain(self, num_episodes: int, max_steps_per_episode: int,
                    reward_threshold: float = 0.5) -> Dict[str, Any]:
        """
        Train brain with reinforcement learning.
        
        Args:
            num_episodes: Number of training episodes
            max_steps_per_episode: Maximum steps per episode
            reward_threshold: Reward threshold for learning
            
        Returns:
            Training results
        """
        training_history = []
        best_reward = 0.0
        
        for episode in range(num_episodes):
            episode_start_time = time.time()
            
            # Reset brain for new episode
            self.brain.reset()
            self.brain.initialize()
            
            # Run episode
            episode_reward = 0.0
            episode_steps = 0
            
            for step in range(max_steps_per_episode):
                # Process sensory input (placeholder - implement based on specific environment)
                # For now, just step the brain
                self.brain.step(step)
                
                # Simulate reward (placeholder)
                if step % 100 == 0:
                    episode_reward += reward_threshold * 0.1
                
                episode_steps += 1
                
                # Check if episode should end
                if self.brain.getFiringNeuronCount() == 0:
                    break
            
            episode_time = time.time() - episode_start_time
            
            episode_result = {
                "episode": episode,
                "steps_completed": episode_steps,
                "reward": episode_reward,
                "episode_time": episode_time,
                "brain_state": self.enhanced_brain.get_config_summary(),
            }
            
            training_history.append(episode_result)
            
            # Update best reward
            if episode_reward > best_reward:
                best_reward = episode_reward
                
                # Save best brain state
                self.brain.save(f"best_brain_{episode}.bin")
            
            # Log progress
            logger.info(f"Episode {episode + 1}/{num_episodes}: "
                       f"Reward: {episode_reward:.3f}, "
                       f"Time: {episode_time:.2f}s")
        
        # Calculate training statistics
        total_reward = sum(episode["reward"] for episode in training_history)
        avg_reward = total_reward / num_episodes
        avg_steps = sum(episode["steps_completed"] for episode in training_history) / num_episodes
        
        return {
            "total_episodes": num_episodes,
            "best_reward": best_reward,
            "average_reward": avg_reward,
            "average_steps_per_episode": avg_steps,
            "training_history": training_history,
            "final_brain_state": self.enhanced_brain.get_config_summary()
        }
    
    def evaluate_brain(self, test_scenarios: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Evaluate brain on test scenarios.
        
        Args:
            test_scenarios: List of test scenarios
            
        Returns:
            Evaluation results
        """
        evaluation_results = []
        
        for scenario in test_scenarios:
            scenario_name = scenario.get("name", "Unnamed Scenario")
            steps = scenario.get("steps", 100)
            
            logger.info(f"Evaluating scenario: {scenario_name}")
            
            # Reset and initialize
            self.brain.reset()
            self.brain.initialize()
            
            # Run scenario
            scenario_start_time = time.time()
            
            for step in range(steps):
                self.brain.step(step)
            
            scenario_time = time.time() - scenario_start_time
            
            # Calculate performance metrics
            final_spike_count = self.brain.getTotalSpikeCount()
            final_firing_rate = self.brain.getAverageFiringRate()
            final_e_i_ratio = self.brain.getExcitationInhibitionRatio()
            
            scenario_result = {
                "scenario_name": scenario_name,
                "steps_completed": steps,
                "scenario_time": scenario_time,
                "performance": {
                    "final_spike_count": final_spike_count,
                    "final_firing_rate": final_firing_rate,
                    "final_e_i_ratio": final_e_i_ratio,
                    "stability": self.enhanced_brain.analyze_stability(steps)["stability"],
                },
                "brain_state": self.enhanced_brain.get_config_summary(),
            }
            
            evaluation_results.append(scenario_result)
        
        # Calculate overall evaluation statistics
        avg_final_spike_count = np.mean([r["performance"]["final_spike_count"] for r in evaluation_results])
        avg_final_firing_rate = np.mean([r["performance"]["final_firing_rate"] for r in evaluation_results])
        avg_final_e_i_ratio = np.mean([r["performance"]["final_e_i_ratio"] for r in evaluation_results])
        
        return {
            "total_scenarios": len(test_scenarios),
            "average_performance": {
                "final_spike_count": avg_final_spike_count,
                "final_firing_rate": avg_final_firing_rate,
                "final_e_i_ratio": avg_final_e_i_ratio,
            },
            "evaluation_results": evaluation_results,
        }
    
    def save_brain(self, filepath: str) -> bool:
        """
        Save brain state to file.
        
        Args:
            filepath: Output file path
            
        Returns:
            True if successful, False otherwise
        """
        try:
            success = self.brain.save(filepath)
            if success:
                logger.info(f"Brain saved to {filepath}")
            else:
                logger.error(f"Failed to save brain to {filepath}")
            return success
        except Exception as e:
            logger.error(f"Error saving brain: {e}")
            return False
    
    def load_brain(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Input file path
            
        Returns:
            True if successful, False otherwise
        """
        try:
            success = self.brain.load(filepath)
            if success:
                logger.info(f"Brain loaded from {filepath}")
                self.enhanced_brain = EnhancedBrainWrapper(self.brain)
            else:
                logger.error(f"Failed to load brain from {filepath}")
            return success
        except Exception as e:
            logger.error(f"Error loading brain: {e}")
            return False
    
    def get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        return {
            "rss_mb": memory_info.rss / 1024 / 1024,
            "vms_mb": memory_info.vms / 1024 / 1024,
            "percent": process.memory_percent(),
        }
    
    def optimize_performance(self, target_memory_mb: int = 512) -> Dict[str, Any]:
        """
        Optimize brain performance for memory constraints.
        
        Args:
            target_memory_mb: Target memory usage in MB
            
        Returns:
            Optimization results
        """
        initial_memory = self.get_memory_usage()["rss_mb"]
        
        # Reduce neuron count if memory usage is high
        if initial_memory > target_memory_mb:
            current_neuron_count = self.brain.getTotalNeuronCount()
            reduction_factor = target_memory_mb / initial_memory
            new_neuron_count = max(100, int(current_neuron_count * reduction_factor))
            
            # Update configuration
            self.brain.getConfig().neuron_count = new_neuron_count
            
            # Reinitialize brain
            self.brain.reset()
            self.brain.initialize()
            
            logger.info(f"Optimized brain: {current_neuron_count} -> {new_neuron_count} neurons")
        
        final_memory = self.get_memory_usage()["rss_mb"]
        
        return {
            "initial_memory_mb": initial_memory,
            "final_memory_mb": final_memory,
            "memory_reduction": initial_memory - final_memory,
            "optimization_successful": final_memory <= target_memory_mb
        }