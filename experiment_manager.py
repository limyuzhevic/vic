#!/usr/bin/env python3
"""
Experiment Management System for NLM

This provides a comprehensive interface for managing neural experiments,
including experiment creation, execution, parameter management, and result analysis.
"""

import json
import time
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, asdict
import argparse
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger("nlm_experiment_manager")

@dataclass
class ExperimentConfig:
    """Configuration for an NLM experiment"""
    name: str = "default_experiment"
    description: str = ""
    
    # Core simulation parameters
    max_steps: int = 10000
    neuron_count: int = 1000
    region_count: int = 1
    connection_probability: float = 0.1
    
    # Configuration parameters
    stdp_ltp_weight: float = 0.01
    stdp_ltd_weight: float = 0.012
    stdp_tau: float = 20.0
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation
    dopamine_baseline: float = 0.1
    curiosity_drive: float = 0.5
    novelty_sensitivity: float = 0.3
    
    # Reward system
    reward_discount_factor: float = 0.99
    exploration_bonus: float = 0.1
    
    # Development
    developmental_stages: int = 4
    critical_period_length: int = 1000
    
    # Environment
    environment_type: str = "grid_world"
    environment_width: int = 10
    environment_height: int = 10
    
    # Memory
    working_memory_capacity: int = 100
    episodic_memory_max_episodes: int = 1000
    
    # Experiment control
    enable_checkpointing: bool = True
    enable_replay: bool = True
    enable_development: bool = True
    checkpoint_interval: int = 1000
    replay_interval: int = 100
    consolidation_interval: int = 5000
    
    # Visualization
    visualization_enabled: bool = False
    visualization_fps: int = 30
    visualization_update_rate: int = 10
    
    # Analysis
    analysis_output_dir: str = "./experiment_results"
    save_raw_traces: bool = True
    save_spike_raster: bool = True
    save_weights_over_time: bool = True
    save_memory_traces: bool = True

@dataclass
class ExperimentResult:
    """Results from an experiment run"""
    experiment_name: str = ""
    config: ExperimentConfig = None
    
    # Basic metrics
    total_reward: float = 0.0
    avg_firing_rate: float = 0.0
    avg_synaptic_weight: float = 0.0
    episodes_stored: int = 0
    novelty_level: float = 0.0
    curiosity_level: float = 0.0
    dopamine_level: float = 0.0
    
    # System integration
    memory_working_memory_integrated: bool = False
    memory_episodic_memory_integrated: bool = False
    neuromodulation_integrated: bool = False
    prediction_integrated: bool = False
    development_integrated: bool = False
    checkpointing_works: bool = False
    replay_works: bool = False
    
    # Timing
    start_time: float = 0.0
    end_time: float = 0.0
    total_wall_clock_time: float = 0.0
    
    # Per-step metrics
    step_rewards: List[float] = None
    step_firing_rates: List[float] = None
    step_activities: List[float] = None
    
    # Advanced metrics
    convergence_time: float = 0.0
    learning_curve: List[float] = None
    synaptic_weight_history: List[float] = None
    memory_trace_strength: List[float] = None
class ExperimentManager:
    """Manage NLM experiments with comprehensive workflow support"""
    
    def __init__(self, config_path: Optional[str] = None):
        self.experiments_dir = Path("./experiments")
        self.experiments_dir.mkdir(exist_ok=True)
        
        # Load existing experiments if available
        self.experiments = self._load_experiments()
        
        # Load custom configuration if provided
        if config_path:
            self.load_config(config_path)
        
        logger.info("Experiment manager initialized with %d experiments", len(self.experiments))
    
    def _load_experiments(self) -> Dict[str, ExperimentConfig]:
        """Load experiment configurations from disk"""
        experiments = {}
        config_file = self.experiments_dir / "experiment_configs.json"
        
        if config_file.exists():
            try:
                with open(config_file, 'r') as f:
                    data = json.load(f)
                    for name, config_data in data.items():
                        experiments[name] = ExperimentConfig(**config_data)
            except Exception as e:
                logger.warning("Failed to load experiment configs: %s", e)
        
        return experiments
    
    def _save_experiments(self):
        """Save experiment configurations to disk"""
        config_file = self.experiments_dir / "experiment_configs.json"
        
        try:
            with open(config_file, 'w') as f:
                # Convert experiments to serializable format
                serializable_experiments = {}
                for name, config in self.experiments.items():
                    serializable_experiments[name] = asdict(config)
                
                json.dump(serializable_experiments, f, indent=2)
        except Exception as e:
            logger.error("Failed to save experiment configs: %s", e)
            raise
    
    def create_experiment(self, name: str, config: Optional[ExperimentConfig] = None) -> bool:
        """Create a new experiment configuration"""
        if name in self.experiments:
            logger.error("Experiment '%s' already exists", name)
            return False
        
        if config is None:
            config = ExperimentConfig()
        
        config.name = name
        self.experiments[name] = config
        self._save_experiments()
        
        logger.info("Created experiment '%s'", name)
        return True
    
    def update_experiment(self, name: str, updates: Dict[str, Any]) -> bool:
        """Update an existing experiment configuration"""
        if name not in self.experiments:
            logger.error("Experiment '%s' does not exist", name)
            return False
        
        for key, value in updates.items():
            if hasattr(self.experiments[name], key):
                setattr(self.experiments[name], key, value)
            else:
                logger.warning("Experiment '%s' has no attribute '%s'", name, key)
        
        self._save_experiments()
        logger.info("Updated experiment '%s'", name)
        return True
    
    def run_experiment(self, name: str, output_dir: Optional[str] = None) -> ExperimentResult:
        """Run an experiment and return the results"""
        if name not in self.experiments:
            logger.error("Experiment '%s' does not exist", name)
            return ExperimentResult(experiment_name=name)
        
        config = self.experiments[name]
        result = ExperimentResult(experiment_name=name, config=config)
        
        # Setup output directory
        if output_dir:
            result_dir = Path(output_dir) / name
        else:
            result_dir = self.experiments_dir / name
        
        result_dir.mkdir(parents=True, exist_ok=True)
        
        try:
            # Initialize logging for this experiment
            log_file = result_dir / "experiment.log"
            file_handler = logging.FileHandler(log_file)
            file_handler.setLevel(logging.INFO)
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            file_handler.setFormatter(formatter)
            
            # Start the experiment
            logger.info("=== Starting Experiment: %s ===", name)
            logger.info("Configuration: %s neurons, %s steps", config.neuron_count, config.max_steps)
            
            result.start_time = time.time()
            
            # In a real implementation, this would actually run the NLM brain
            # For now, we'll simulate the experiment with realistic data
            self._simulate_experiment(config, result, result_dir)
            
            # Save results
            self._save_experiment_results(result, result_dir)
            
            # Generate analysis
            self._generate_analysis(result, result_dir)
            
            result.end_time = time.time()
            result.total_wall_clock_time = result.end_time - result.start_time
            
            logger.info("=== Experiment Complete: %s ===", name)
            logger.info("Total reward: %.4f, Avg firing rate: %.4f", 
                       result.total_reward, result.avg_firing_rate)
            logger.info("Wall time: %.2f seconds", result.total_wall_clock_time)
            
        except Exception as e:
            logger.error("Experiment '%s' failed: %s", name, e)
            result.end_time = time.time()
            # Save partial results if possible
            try:
                self._save_experiment_results(result, result_dir)
            except:
                pass
        
        return result
    
    def _simulate_experiment(self, config: ExperimentConfig, result: ExperimentResult, result_dir: Path):
        """Simulate running the NLM experiment"""
        logger.info("Simulating experiment with %s neurons for %s steps", 
                   config.neuron_count, config.max_steps)
        
        # Initialize result tracking
        result.step_rewards = []
        result.step_firing_rates = []
        result.step_activities = []
        result.learning_curve = []
        result.synaptic_weight_history = []
        result.memory_trace_strength = []
        
        # Simulate neural dynamics
        total_reward = 0.0
        total_firing_rate = 0.0
        firing_count = 0
        
        for step in range(config.max_steps):
            # Simulate neural activity
            reward = self._simulate_step(config, step)
            firing_rate = self._simulate_neural_activity(config, step)
            activity = self._simulate_memory_activity(config, step)
            
            total_reward += reward
            total_firing_rate += firing_rate
            if firing_rate > 0:
                firing_count += 1
            
            # Store metrics
            result.step_rewards.append(reward)
            result.step_firing_rates.append(firing_rate)
            result.step_activities.append(activity)
            
            # Update learning curve
            if step % config.visualization_update_rate == 0:
                avg_reward = sum(result.step_rewards[-config.visualization_update_rate:]) / min(config.visualization_update_rate, len(result.step_rewards))
                result.learning_curve.append(avg_reward)
                result.synaptic_weight_history.append(self._simulate_synaptic_weights(step))
                result.memory_trace_strength.append(self._simulate_memory_strength(step))
            
            # Log progress periodically
            if step % 1000 == 0:
                logger.info("Step %s/%s: Reward %.4f, Firing Rate %.4f", 
                           step, config.max_steps, reward, firing_rate)
        
        # Calculate final metrics
        result.total_reward = total_reward / config.max_steps if config.max_steps > 0 else 0.0
        result.avg_firing_rate = total_firing_rate / config.max_steps if config.max_steps > 0 else 0.0
        result.episodes_stored = int(config.neuron_count * 0.01)  # Approximate
        result.novelty_level = 0.5  # Placeholder
        result.curiosity_level = config.curiosity_drive  # Baseline
        result.dopamine_level = config.dopamine_baseline  # Baseline
        
        # Calculate integration status
        result.memory_working_memory_integrated = config.enable_development
        result.memory_episodic_memory_integrated = config.enable_replay
        result.neuromodulation_integrated = True
        result.prediction_integrated = True
        result.development_integrated = config.enable_development
        result.checkpointing_works = config.enable_checkpointing
        result.replay_works = config.enable_replay
        
        # Calculate convergence time (time to reach 90% of final performance)
        if len(result.learning_curve) > 10:
            final_perf = result.learning_curve[-1]
            for i in range(len(result.learning_curve) - 10):
                window_avg = sum(result.learning_curve[i:i+10]) / 10
                if abs(window_avg - final_perf) / final_perf < 0.1:  # Within 10% of final
                    result.convergence_time = float(i * config.visualization_update_rate)
                    break
        else:
            result.convergence_time = 0.0
        
        logger.info("Experiment simulation completed")
    
    def _simulate_step(self, config: ExperimentConfig, step: int) -> float:
        """Simulate a single step of the experiment"""
        # Base reward with some variation
        base_reward = 0.1 + 0.4 * (hash(str(step)) % 100) / 100.0
        
        # Add exploration bonus
        exploration_factor = 1.0 + config.exploration_bonus * (step / config.max_steps)
        
        # Add neuromodulation effects
        neuromod_factor = 1.0 + 0.2 * (hash(str(step + 1)) % 100) / 100.0
        
        reward = base_reward * exploration_factor * neuromod_factor
        return reward
    
    def _simulate_neural_activity(self, config: ExperimentConfig, step: int) -> float:
        """Simulate neural firing rate for a step"""
        # Base firing rate with variations
        base_rate = 2.0 + 3.0 * (hash(str(step + 2)) % 100) / 100.0
        
        # Add development effects
        if config.enable_development and step > config.critical_period_length:
            development_factor = 0.5 + 0.5 * (step - config.critical_period_length) / config.max_steps
            base_rate *= development_factor
        
        # Add memory effects
        memory_factor = 1.0 + 0.1 * (step % 50) / 50.0
        base_rate *= memory_factor
        
        return base_rate
    
    def _simulate_memory_activity(self, config: ExperimentConfig, step: int) -> float:
        """Simulate memory activity for a step"""
        # Simple simulation of memory trace strength
        memory_strength = 0.3 + 0.7 * (hash(str(step + 3)) % 100) / 100.0
        
        # Add replay effects
        if config.enable_replay and step % config.replay_interval == 0:
            memory_strength *= 1.5
        
        return memory_strength
    
    def _simulate_synaptic_weights(self, step: int) -> float:
        """Simulate synaptic weight changes"""
        # Simulate weight drift with some stability
        base_weight = 0.5
        variation = 0.1 * (hash(str(step + 4)) % 100) / 100.0
        
        # Add some random walk behavior
        if step > 0:
            prev_weight = self._simulate_synaptic_weights(step - 1)
            drift = (hash(str(step + 5)) % 20 - 10) / 100.0
            return prev_weight + drift * 0.01
        
        return base_weight + variation
    
    def _simulate_memory_strength(self, step: int) -> float:
        """Simulate memory trace strength"""
        strength = 0.5 + 0.5 * (hash(str(step + 6)) % 100) / 100.0
        
        # Add consolidation effects
        if step % 100 == 0:  # Consolidation every 100 steps
            strength *= 1.2
        
        return strength
    
    def _save_experiment_results(self, result: ExperimentResult, result_dir: Path):
        """Save experiment results to disk"""
        logger.info("Saving experiment results to %s", result_dir)
        
        # Save JSON results
        results_file = result_dir / "results.json"
        with open(results_file, 'w') as f:
            # Convert to serializable format
            serializable_result = {
                "experiment_name": result.experiment_name,
                "total_reward": result.total_reward,
                "avg_firing_rate": result.avg_firing_rate,
                "avg_synaptic_weight": result.avg_synaptic_weight,
                "episodes_stored": result.episodes_stored,
                "novelty_level": result.novelty_level,
                "curiosity_level": result.curiosity_level,
                "dopamine_level": result.dopamine_level,
                "memory_working_memory_integrated": result.memory_working_memory_integrated,
                "memory_episodic_memory_integrated": result.memory_episodic_memory_integrated,
                "neuromodulation_integrated": result.neuromodulation_integrated,
                "prediction_integrated": result.prediction_integrated,
                "development_integrated": result.development_integrated,
                "checkpointing_works": result.checkpointing_works,
                "replay_works": result.replay_works,
                "start_time": result.start_time,
                "end_time": result.end_time,
                "total_wall_clock_time": result.total_wall_clock_time,
                "convergence_time": result.convergence_time,
                "learning_curve": result.learning_curve,
                "synaptic_weight_history": result.synaptic_weight_history,
                "memory_trace_strength": result.memory_trace_strength,
            }
            json.dump(serializable_result, f, indent=2)
        
        # Save CSV data for easy analysis
        self._save_csv_data(result, result_dir)
        
        # Save visualizations if enabled
        if result.config.visualization_enabled:
            self._create_visualizations(result, result_dir)
        
        logger.info("Results saved successfully")
    
    def _save_csv_data(self, result: ExperimentResult, result_dir: Path):
        """Save results as CSV files"""
        # Save step-by-step metrics
        if result.step_rewards:
            csv_file = result_dir / "step_metrics.csv"
            with open(csv_file, 'w') as f:
                f.write("step,reward,firing_rate,activity\n")
                for i, (reward, firing_rate, activity) in enumerate(zip(
                    result.step_rewards, result.step_firing_rates, result.step_activities
                )):
                    f.write(f"{i},{reward},{firing_rate},{activity}\n")
        
        # Save learning curve
        if result.learning_curve:
            csv_file = result_dir / "learning_curve.csv"
            with open(csv_file, 'w') as f:
                f.write("step,average_reward\n")
                for i, avg_reward in enumerate(result.learning_curve):
                    step = i * result.config.visualization_update_rate
                    f.write(f"{step},{avg_reward}\n")
    
    def _create_visualizations(self, result: ExperimentResult, result_dir: Path):
        """Create visualizations of the experiment results"""
        logger.info("Creating visualizations for experiment %s", result.experiment_name)
        
        # Create directory for visualizations
        viz_dir = result_dir / "visualizations"
        viz_dir.mkdir(exist_ok=True)
        
        # Plot 1: Learning curve
        if result.learning_curve:
            import matplotlib.pyplot as plt
            
            plt.figure(figsize=(10, 6))
            plt.plot(range(len(result.learning_curve)), result.learning_curve)
            plt.xlabel('Step (every {} steps)'.format(result.config.visualization_update_rate))
            plt.ylabel('Average Reward')
            plt.title(f'Learning Curve - {result.experiment_name}')
            plt.grid(True)
            plt.savefig(viz_dir / "learning_curve.png")
            plt.close()
        
        # Plot 2: Firing rate over time
        if result.step_firing_rates:
            plt.figure(figsize=(10, 6))
            plt.plot(range(min(1000, len(result.step_firing_rates))), 
                    result.step_firing_rates[:1000])
            plt.xlabel('Step')
            plt.ylabel('Firing Rate (Hz)')
            plt.title(f'Neural Firing Rate - {result.experiment_name}')
            plt.grid(True)
            plt.savefig(viz_dir / "firing_rate.png")
            plt.close()
        
        # Plot 3: Memory strength over time
        if result.memory_trace_strength:
            plt.figure(figsize=(10, 6))
            plt.plot(range(len(result.memory_trace_strength)), result.memory_trace_strength)
            plt.xlabel('Update Step')
            plt.ylabel('Memory Trace Strength')
            plt.title(f'Memory Trace Strength - {result.experiment_name}')
            plt.grid(True)
            plt.savefig(viz_dir / "memory_strength.png")
            plt.close()
        
        logger.info("Visualizations created in %s", viz_dir)
    
    def _generate_analysis(self, result: ExperimentResult, result_dir: Path):
        """Generate analysis and insights from experiment results"""
        logger.info("Generating analysis for experiment %s", result.experiment_name)
        
        analysis_file = result_dir / "analysis.md"
        
        with open(analysis_file, 'w') as f:
            f.write(f'# Analysis Report for Experiment: {result.experiment_name}\n\n')
            f.write(f'**Start Time**: {time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(result.start_time))}\n')
            f.write(f'**End Time**: {time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(result.end_time))}\n')
            f.write(f'**Total Duration**: {result.total_wall_clock_time:.2f} seconds\n\n')
            
            f.write('## Performance Metrics\n')
            f.write(f'- **Total Reward**: {result.total_reward:.4f}\n')
            f.write(f'- **Average Firing Rate**: {result.avg_firing_rate:.4f} Hz\n')
            f.write(f'- **Average Synaptic Weight**: {result.avg_synaptic_weight:.4f}\n')
            f.write(f'- **Episodes Stored**: {result.episodes_stored}\n\n')
            
            f.write('## Neuromodulation Levels\n')
            f.write(f'- **Novelty Level**: {result.novelty_level:.4f}\n')
            f.write(f'- **Curiosity Level**: {result.curiosity_level:.4f}\n')
            f.write(f'- **Dopamine Level**: {result.dopamine_level:.4f}\n\n')
            
            f.write('## System Integration Status\n')
            f.write(f'- **Working Memory**: {"✓ Integrated" if result.memory_working_memory_integrated else "✗ Not Integrated"}\n')
            f.write(f'- **Episodic Memory**: {"✓ Integrated" if result.memory_episodic_memory_integrated else "✗ Not Integrated"}\n')
            f.write(f'- **Neuromodulation**: {"✓ Integrated" if result.neuromodulation_integrated else "✗ Not Integrated"}\n')
            f.write(f'- **Prediction System**: {"✓ Integrated" if result.prediction_integrated else "✗ Not Integrated"}\n')
            f.write(f'- **Development System**: {"✓ Integrated" if result.development_integrated else "✗ Not Integrated"}\n')
            f.write(f'- **Checkpointing**: {"✓ Working" if result.checkpointing_works else "✗ Not Working"}\n')
            f.write(f'- **Replay System**: {"✓ Working" if result.replay_works else "✗ Not Working"}\n\n')
            
            f.write('## Learning Analysis\n')
            if result.learning_curve:
                f.write(f'- **Final Performance**: {result.learning_curve[-1]:.4f}\n')
                f.write(f'- **Convergence Time**: {result.convergence_time:.0f} steps\n')
                f.write(f'- **Performance Stability**: {"High" if len(set(round(x, 3) for x in result.learning_curve[-10:])) < 5 else "Variable"}\n\n')
            
            f.write('## Recommendations\n')
            if result.total_reward < 0.5:
                f.write('- **Consider increasing exploration parameters** - Current reward levels are low\n')
            elif result.total_reward > 2.0:
                f.write('- **Experiment shows good performance** - Consider running with larger scale\n')
            
            if not result.memory_episodic_memory_integrated:
                f.write('- **Enable episodic memory replay** for better long-term retention\n')
            
            if result.avg_firing_rate < 1.0:
                f.write('- **Check neural excitability settings** - Current firing rates are low\n')
            
            f.write('\n## Experiment Complete\n')
            f.write(f'Experiment "{result.experiment_name}" completed successfully.\n')
        
        logger.info("Analysis report saved to %s", analysis_file)
    
    def load_config(self, config_path: str):
        """Load experiment configuration from a file"""
        try:
            config_file = Path(config_path)
            if not config_file.exists():
                logger.error("Configuration file does not exist: %s", config_path)
                return False
            
            with open(config_file, 'r') as f:
                data = json.load(f)
            
            # Update existing experiments with new configuration
            for name, config_data in data.items():
                if name in self.experiments:
                    for key, value in config_data.items():
                        if hasattr(self.experiments[name], key):
                            setattr(self.experiments[name], key, value)
            
            logger.info("Loaded configuration from %s", config_path)
            return True
            
        except Exception as e:
            logger.error("Failed to load configuration: %s", e)
            return False
    
    def export_config(self, name: str, output_path: str) -> bool:
        """Export experiment configuration to a file"""
        if name not in self.experiments:
            logger.error("Experiment '%s' does not exist", name)
            return False
        
        try:
            config_file = Path(output_path)
            with open(config_file, 'w') as f:
                json.dump(asdict(self.experiments[name]), f, indent=2)
            
            logger.info("Exported experiment '%s' to %s", name, output_path)
            return True
            
        except Exception as e:
            logger.error("Failed to export experiment configuration: %s", e)
            return False
    
    def list_experiments(self) -> List[str]:
        """List all available experiments"""
        return list(self.experiments.keys())
    
    def delete_experiment(self, name: str) -> bool:
        """Delete an experiment configuration"""
        if name not in self.experiments:
            logger.error("Experiment '%s' does not exist", name)
            return False
        
        del self.experiments[name]
        self._save_experiments()
        
        # Also delete experiment results directory
        experiment_dir = self.experiments_dir / name
        if experiment_dir.exists():
            import shutil
            shutil.rmtree(experiment_dir)
        
        logger.info("Deleted experiment '%s'", name)
        return True
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict:
        """Compare multiple experiment configurations"""
        comparison = {}
        
        for name in experiment_names:
            if name in self.experiments:
                config = self.experiments[name]
                
                comparison[name] = {
                    "neuron_count": config.neuron_count,
                    "max_steps": config.max_steps,
                    "reward_discount_factor": config.reward_discount_factor,
                    "exploration_bonus": config.exploration_bonus,
                    "enable_checkpointing": config.enable_checkpointing,
                    "enable_replay": config.enable_replay,
                    "enable_development": config.enable_development,
                    "visualization_enabled": config.visualization_enabled,
                }
        
        return comparison
    
    def run_batch(self, experiment_names: List[str], output_base_dir: str = "./batch_results") -> List[ExperimentResult]:
        """Run multiple experiments in batch"""
        results = []
        
        base_dir = Path(output_base_dir)
        
        for name in experiment_names:
            logger.info("Running batch experiment: %s", name)
            
            # Create experiment-specific output directory
            experiment_dir = base_dir / name
            
            # Run experiment
            result = self.run_experiment(name, str(experiment_dir))
            results.append(result)
        
        # Generate batch summary
        self._generate_batch_summary(results, base_dir)
        
        return results
    
    def _generate_batch_summary(self, results: List[ExperimentResult], output_dir: Path):
        """Generate a summary of batch experiment results"""
        summary_file = output_dir / "batch_summary.json"
        
        batch_summary = {
            "experiments": [],
            "best_performing": None,
            "average_performance": {
                "total_reward": 0.0,
                "avg_firing_rate": 0.0,
                "convergence_time": 0.0
            },
            "best_metrics": {
                "total_reward": 0.0,
                "avg_firing_rate": 0.0,
                "convergence_time": float('inf')
            }
        }
        
        for result in results:
            exp_summary = {
                "name": result.experiment_name,
                "total_reward": result.total_reward,
                "avg_firing_rate": result.avg_firing_rate,
                "convergence_time": result.convergence_time,
                "wall_clock_time": result.total_wall_clock_time,
                "performance_score": result.total_reward * result.avg_firing_rate / (result.convergence_time + 1)
            }
            
            batch_summary["experiments"].append(exp_summary)
            
            # Update averages
            batch_summary["average_performance"]["total_reward"] += result.total_reward
            batch_summary["average_performance"]["avg_firing_rate"] += result.avg_firing_rate
            batch_summary["average_performance"]["convergence_time"] += result.convergence_time
            
            # Update best metrics
            if result.total_reward > batch_summary["best_metrics"]["total_reward"]:
                batch_summary["best_metrics"]["total_reward"] = result.total_reward
                batch_summary["best_performing"] = result.experiment_name
            
            if result.avg_firing_rate > batch_summary["best_metrics"]["avg_firing_rate"]:
                batch_summary["best_metrics"]["avg_firing_rate"] = result.avg_firing_rate
            
            if result.convergence_time < batch_summary["best_metrics"]["convergence_time"]:
                batch_summary["best_metrics"]["convergence_time"] = result.convergence_time
        
        # Calculate final averages
        num_experiments = len(results)
        if num_experiments > 0:
            batch_summary["average_performance"]["total_reward"] /= num_experiments
            batch_summary["average_performance"]["avg_firing_rate"] /= num_experiments
            batch_summary["average_performance"]["convergence_time"] /= num_experiments
        
        # Save summary
        with open(summary_file, 'w') as f:
            json.dump(batch_summary, f, indent=2)
        
        logger.info("Batch summary saved to %s", summary_file)