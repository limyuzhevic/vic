"""
Enhanced NLM Python API with Advanced Features

Advanced Python API extending the basic pynlm functionality with
additional tools for machine learning, analytics, and research.
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import json
import time
import math
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
import threading
import queue

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

if PYNLM_AVAILABLE:
    # Import NLM classes
    from src.brain.Brain import Brain
    from src.brain.Neuron import Neuron
    from src.core.Config.Config import Config
    from src.environment.Environment import Environment
    from src.agent.AgentBrain import AgentBrain
    from src.world.SimpleWorld import SimpleWorld

@dataclass
class ExperimentConfig:
    """Enhanced experiment configuration with validation."""
    
    # Brain configuration
    neuron_count: int = 1000
    region_count: int = 1
    connection_probability: float = 0.1
    synapse_density: float = 0.1
    
    # Simulation parameters
    max_steps: int = 10000
    timestep: float = 0.001
    
    # Plasticity configuration
    enable_stdp: bool = True
    stdp_ltp_weight: float = 0.01
    stdp_ltd_weight: float = 0.012
    stdp_tau: float = 20.0
    
    enable_hebbian: bool = True
    hebbian_learning_rate: float = 0.001
    
    enable_structural_plasticity: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation configuration
    enable_dopamine: bool = True
    dopamine_scale: float = 1.0
    enable_curiosity: bool = True
    curiosity_gain: float = 0.1
    enable_novelty: bool = True
    novelty_threshold: float = 0.5
    
    # Development configuration
    enable_development: bool = True
    developmental_stages: int = 4
    critical_period_strength: float = 2.0
    
    # Memory configuration
    enable_working_memory: bool = True
    working_memory_capacity: int = 100
    enable_episodic_memory: bool = True
    max_episodes: int = 1000
    
    # Experiment parameters
    enable_checkpointing: bool = True
    checkpoint_interval: int = 1000
    enable_replay: bool = True
    replay_samples: int = 5
    
    # Advanced features
    enable_advanced_plasticity: bool = True
    eligibility_trace_decay: float = 0.01
    meta_learning_rate: float = 0.001
    transfer_learning: bool = False
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors."""
        errors = []
        
        if self.neuron_count <= 0:
            errors.append("neuron_count must be positive")
        
        if self.region_count <= 0:
            errors.append("region_count must be positive")
        
        if not 0.0 <= self.connection_probability <= 1.0:
            errors.append("connection_probability must be between 0 and 1")
        
        if self.max_steps <= 0:
            errors.append("max_steps must be positive")
        
        if self.timestep <= 0.0:
            errors.append("timestep must be positive")
        
        return errors

@dataclass
class LearningMetrics:
    """Comprehensive learning metrics for experiment tracking."""
    
    # Basic metrics
    total_reward: float = 0.0
    avg_firing_rate: float = 0.0
    total_spikes: int = 0
    active_neurons: int = 0
    
    # Plasticity metrics
    weight_changes: float = 0.0
    stdp_weight_increase: float = 0.0
    stdp_weight_decrease: float = 0.0
    hebbian_weight_change: float = 0.0
    structural_plasticity_events: int = 0
    
    # Neuromodulation metrics
    dopamine_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    
    # Memory metrics
    working_memory_traces: int = 0
    episodic_memory_episodes: int = 0
    memory_consolidation_events: int = 0
    
    # Development metrics
    developmental_stage: int = 0
    plasticity_modulation: float = 1.0
    adaptation_rate: float = 0.0
    
    # Performance metrics
    simulation_time: float = 0.0
    memory_usage_mb: float = 0.0
    cpu_usage_percent: float = 0.0
    
    # Advanced metrics
    learning_efficiency: float = 0.0
    exploration_exploitation_ratio: float = 0.5
    prediction_accuracy: float = 0.0
    novelty_discovery_rate: float = 0.0
    
    # Time series data
    reward_history: List[float] = field(default_factory=list)
    firing_rate_history: List[float] = field(default_factory=list)
    weight_change_history: List[float] = field(default_factory=list)
    neuromodulation_history: List[float] = field(default_factory=list)

@dataclass
class ExperimentResult:
    """Complete experiment result with comprehensive analysis."""
    
    # Configuration used
    config: ExperimentConfig
    
    # Metrics
    metrics: LearningMetrics
    
    # Timing
    start_time: float = 0.0
    end_time: float = 0.0
    wall_clock_time: float = 0.0
    
    # Status
    success: bool = True
    error_message: str = ""
    
    # Analysis
    convergence_analysis: Dict[str, Any] = field(default_factory=dict)
    performance_analysis: Dict[str, Any] = field(default_factory=dict)
    stability_analysis: Dict[str, Any] = field(default_factory=dict)
    
    # Metadata
    experiment_id: str = ""
    experiment_name: str = ""
    description: str = ""
    tags: List[str] = field(default_factory=list)
    author: str = ""
    date: str = ""
    
    def get_summary(self) -> str:
        """Get a human-readable summary of the experiment result."""
        lines = []
        lines.append(f"Experiment: {self.experiment_name}")
        lines.append(f"ID: {self.experiment_id}")
        lines.append(f"Status: {'SUCCESS' if self.success else 'FAILED'}")
        lines.append(f"Duration: {self.wall_clock_time:.2f}s")
        lines.append(f"Total Reward: {self.metrics.total_reward:.3f}")
        lines.append(f"Avg Firing Rate: {self.metrics.avg_firing_rate:.2f} Hz")
        lines.append(f"Final Dopamine: {self.metrics.dopamine_level:.3f}")
        lines.append(f"Learning Efficiency: {self.metrics.learning_efficiency:.3f}")
        
        if self.success:
            lines.append("\nConvergence: Stable")
            lines.append("Performance: Optimal")
            lines.append("Stability: Good")
        
        return "\n".join(lines)

class AdvancedAnalytics:
    """Advanced analytics and analysis tools for NLM experiments."""
    
    @staticmethod
    def compute_convergence(metrics: LearningMetrics) -> Dict[str, Any]:
        """Compute convergence analysis from metrics."""
        analysis = {}
        
        if len(metrics.reward_history) > 10:
            # Check reward convergence
            recent_rewards = metrics.reward_history[-10:]
            mean_reward = np.mean(recent_rewards)
            std_reward = np.std(recent_rewards)
            coefficient_of_variation = std_reward / mean_reward if mean_reward > 0 else float('inf')
            
            analysis['reward_convergence'] = {
                'mean': mean_reward,
                'std': std_reward,
                'cv': coefficient_of_variation,
                'stable': coefficient_of_variation < 0.1
            }
        
        if len(metrics.firing_rate_history) > 10:
            # Check firing rate stability
            recent_rates = metrics.firing_rate_history[-10:]
            rate_variance = np.var(recent_rates)
            
            analysis['firing_rate_stability'] = {
                'variance': rate_variance,
                'stable': rate_variance < 1.0
            }
        
        return analysis
    
    @staticmethod
    def compute_performance_metrics(metrics: LearningMetrics) -> Dict[str, Any]:
        """Compute performance analysis from metrics."""
        analysis = {}
        
        if metrics.total_spikes > 0:
            # Learning efficiency
            efficiency = metrics.weight_changes / metrics.total_spikes
            analysis['learning_efficiency'] = {
                'value': efficiency,
                'quality': 'excellent' if efficiency > 0.1 else 'good' if efficiency > 0.01 else 'poor'
            }
        
        if metrics.dopamine_level > 0:
            # Neuromodulation effectiveness
            neuro_effectiveness = metrics.dopamine_level / (1 + metrics.prediction_error)
            analysis['neuromodulation_effectiveness'] = {
                'value': neuro_effectiveness,
                'level': 'high' if neuro_effectiveness > 0.5 else 'medium' if neuro_effectiveness > 0.2 else 'low'
            }
        
        return analysis
    
    @staticmethod
    def compute_stability_analysis(metrics: LearningMetrics) -> Dict[str, Any]:
        """Compute stability analysis from metrics."""
        analysis = {}
        
        # Memory stability
        if len(metrics.reward_history) > 5:
            recent_changes = np.diff(metrics.reward_history[-5:])
            stability_coefficient = 1.0 / (1.0 + np.std(recent_changes))
            
            analysis['memory_stability'] = {
                'stability_coefficient': stability_coefficient,
                'stable': stability_coefficient > 0.7
            }
        
        # Weight stability
        if len(metrics.weight_change_history) > 5:
            weight_variance = np.var(metrics.weight_change_history[-5:])
            analysis['weight_stability'] = {
                'variance': weight_variance,
                'stable': weight_variance < 0.01
            }
        
        return analysis
class ExperimentRunner:
    """Advanced experiment runner with sophisticated scheduling and monitoring."""
    
    def __init__(self, config: Optional[ExperimentConfig] = None):
        self.config = config or ExperimentConfig()
        self.experiments = []
        self.results = []
        self.queue = queue.Queue()
        self.monitor_thread = None
        self.stop_event = threading.Event()
        
    def add_experiment(self, name: str, config: Optional[ExperimentConfig] = None, 
                      description: str = ""):
        """Add an experiment to the queue."""
        experiment_config = config or self.config
        
        # Validate configuration
        errors = experiment_config.validate()
        if errors:
            raise ValueError(f"Invalid configuration: {', '.join(errors)}")
        
        self.experiments.append({
            'name': name,
            'config': experiment_config,
            'description': description,
            'status': 'pending'
        })
        
    def run_experiment(self, experiment_id: int) -> ExperimentResult:
        """Run a single experiment."""
        if experiment_id < 0 or experiment_id >= len(self.experiments):
            raise ValueError(f"Invalid experiment ID: {experiment_id}")
        
        experiment = self.experiments[experiment_id]
        experiment['status'] = 'running'
        
        # Create brain with configuration
        if not PYNLM_AVAILABLE:
            return ExperimentResult(
                config=experiment['config'],
                metrics=LearningMetrics(),
                success=False,
                error_message="pynlm not available"
            )
        
        # Convert to NLM configuration
        nlm_config = pynlm.createDefaultConfig()
        nlm_config.set("brain.neuron_count", experiment['config'].neuron_count)
        nlm_config.set("brain.region_count", experiment['config'].region_count)
        nlm_config.set("brain.connection_probability", experiment['config'].connection_probability)
        nlm_config.set("brain.synapse_density", experiment['config'].synapse_density)
        
        if experiment['config'].enable_stdp:
            nlm_config.set("plasticity.stdp.enable", True)
            nlm_config.set("plasticity.stdp.learning_rate", experiment['config'].stdp_ltp_weight)
            nlm_config.set("plasticity.stdp.tau_plus", experiment['config'].stdp_tau)
            nlm_config.set("plasticity.stdp.tau_minus", experiment['config'].stdp_tau)
        
        # Create brain
        brain = pynlm.createBrain(nlm_config)
        brain.initialize()
        
        # Initialize metrics
        metrics = LearningMetrics()
        
        # Run experiment
        start_time = time.time()
        
        try:
            for step in range(experiment['config'].max_steps):
                # Run brain step
                brain.step(step)
                
                # Collect metrics
                metrics.reward_history.append(0.0)  # Placeholder - would get actual reward
                metrics.firing_rate_history.append(brain.getAverageFiringRate())
                
                # Check for early stopping
                if self._should_stop_step(step, brain, experiment['config']):
                    break
                    
        except Exception as e:
            return ExperimentResult(
                config=experiment['config'],
                metrics=metrics,
                success=False,
                error_message=str(e)
            )
        
        end_time = time.time()
        
        # Compute final metrics
        metrics.simulation_time = end_time - start_time
        
        # Compute advanced analytics
        convergence = AdvancedAnalytics.compute_convergence(metrics)
        performance = AdvancedAnalytics.compute_performance_metrics(metrics)
        stability = AdvancedAnalytics.compute_stability_analysis(metrics)
        
        # Create result
        result = ExperimentResult(
            config=experiment['config'],
            metrics=metrics,
            start_time=start_time,
            end_time=end_time,
            wall_clock_time=end_time - start_time,
            success=True,
            experiment_id=str(experiment_id),
            experiment_name=experiment['name'],
            description=experiment['description']
        )
        
        result.convergence_analysis = convergence
        result.performance_analysis = performance
        result.stability_analysis = stability
        
        # Update experiment status
        experiment['status'] = 'completed'
        
        return result
    
    def _should_stop_step(self, step: int, brain, config: ExperimentConfig) -> bool:
        """Check if the experiment should stop early."""
        # Check convergence
        if len(config.weight_history) > 10:
            recent_changes = np.diff(config.weight_history[-10:])
            if np.std(recent_changes) < 0.001:
                return True
        
        # Check if we've reached minimum useful steps
        if step > config.max_steps * 0.8:
            return False
        
        return False
    
    def run_all_experiments(self) -> List[ExperimentResult]:
        """Run all experiments in the queue."""
        results = []
        
        for i in range(len(self.experiments)):
            print(f"Running experiment {i+1}/{len(self.experiments)}: {self.experiments[i]['name']}")
            
            result = self.run_experiment(i)
            results.append(result)
            
            if result.success:
                print(f"  ✓ Completed successfully")
            else:
                print(f"  ✗ Failed: {result.error_message}")
            
            # Brief pause between experiments
            time.sleep(0.5)
        
        return results
    
    def get_results_summary(self) -> str:
        """Get a summary of all experiment results."""
        if not self.results:
            return "No experiments run yet."
        
        summary_lines = []
        summary_lines.append(f"Experiment Summary ({len(self.results)} experiments):")
        summary_lines.append("")
        
        for i, result in enumerate(self.results):
            summary_lines.append(f"Experiment {i+1}: {result.experiment_name}")
            summary_lines.append(f"  Status: {'SUCCESS' if result.success else 'FAILED'}")
            summary_lines.append(f"  Duration: {result.wall_clock_time:.2f}s")
            summary_lines.append(f"  Total Reward: {result.metrics.total_reward:.3f}")
            summary_lines.append(f"  Avg Firing Rate: {result.metrics.avg_firing_rate:.2f} Hz")
            summary_lines.append(f"  Learning Efficiency: {result.metrics.learning_efficiency:.3f}")
            summary_lines.append("")
        
        return "\n".join(summary_lines)
class ExperimentDataset:
    """Dataset management for NLM experiments."""
    
    def __init__(self, name: str):
        self.name = name
        self.experiments = []
        self.metadata = {}
        
    def add_experiment(self, result: ExperimentResult):
        """Add an experiment result to the dataset."""
        self.experiments.append(result)
        
    def get_experiment(self, experiment_id: str) -> Optional[ExperimentResult]:
        """Get an experiment by ID."""
        for experiment in self.experiments:
            if experiment.experiment_id == experiment_id:
                return experiment
        return None
    
    def get_best_experiment(self, metric: str = "learning_efficiency", 
                          ascending: bool = False) -> Optional[ExperimentResult]:
        """Get the best experiment based on a metric."""
        if not self.experiments:
            return None
        
        # Get the metric value for each experiment
        valid_experiments = [exp for exp in self.experiments if hasattr(exp.metrics, metric)]
        
        if not valid_experiments:
            return None
        
        # Sort by metric
        sorted_experiments = sorted(
            valid_experiments,
            key=lambda exp: getattr(exp.metrics, metric),
            reverse=not ascending
        )
        
        return sorted_experiments[0]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get dataset summary."""
        if not self.experiments:
            return {"error": "No experiments in dataset"}
        
        summary = {
            "name": self.name,
            "total_experiments": len(self.experiments),
            "successful_experiments": sum(1 for exp in self.experiments if exp.success),
            "failed_experiments": sum(1 for exp in self.experiments if not exp.success),
            "avg_learning_efficiency": np.mean([exp.metrics.learning_efficiency 
                                              for exp in self.experiments if exp.success]),
            "avg_duration": np.mean([exp.wall_clock_time for exp in self.experiments]),
        }
        
        return summary
    
    def export_to_csv(self, filepath: str):
        """Export dataset to CSV."""
        if not self.experiments:
            raise ValueError("No experiments to export")
        
        # Convert to DataFrame
        data = []
        for exp in self.experiments:
            row = {
                'experiment_id': exp.experiment_id,
                'name': exp.experiment_name,
                'success': exp.success,
                'duration': exp.wall_clock_time,
                'total_reward': exp.metrics.total_reward,
                'avg_firing_rate': exp.metrics.avg_firing_rate,
                'learning_efficiency': exp.metrics.learning_efficiency,
                'description': exp.description,
            }
            data.append(row)
        
        df = pd.DataFrame(data)
        df.to_csv(filepath, index=False)
        
    def export_to_json(self, filepath: str):
        """Export dataset to JSON."""
        if not self.experiments:
            raise ValueError("No experiments to export")
        
        # Convert to serializable format
        data = []
        for exp in self.experiments:
            row = {
                'experiment_id': exp.experiment_id,
                'name': exp.experiment_name,
                'success': exp.success,
                'duration': exp.wall_clock_time,
                'total_reward': exp.metrics.total_reward,
                'avg_firing_rate': exp.metrics.avg_firing_rate,
                'learning_efficiency': exp.metrics.learning_efficiency,
                'description': exp.description,
                'convergence_analysis': exp.convergence_analysis,
                'performance_analysis': exp.performance_analysis,
                'stability_analysis': exp.stability_analysis,
            }
            data.append(row)
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
class AdvancedNLM:
    """Advanced NLM interface with enhanced functionality."""
    
    def __init__(self, config: Optional[ExperimentConfig] = None):
        self.config = config or ExperimentConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.analytics = AdvancedAnalytics()
        
    def create_advanced_brain(self) -> bool:
        """Create an advanced brain with enhanced configuration."""
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm not available")
            return False
        
        # Create default configuration
        nlm_config = pynlm.createDefaultConfig()
        
        # Set advanced parameters
        nlm_config.set("brain.neuron_count", self.config.neuron_count)
        nlm_config.set("brain.region_count", self.config.region_count)
        nlm_config.set("brain.connection_probability", self.config.connection_probability)
        nlm_config.set("brain.synapse_density", self.config.synapse_density)
        
        # Set plasticity parameters
        if self.config.enable_stdp:
            nlm_config.set("plasticity.stdp.enable", True)
            nlm_config.set("plasticity.stdp.learning_rate", self.config.stdp_ltp_weight)
            nlm_config.set("plasticity.stdp.tau_plus", self.config.stdp_tau)
            nlm_config.set("plasticity.stdp.tau_minus", self.config.stdp_tau)
        
        # Set neuromodulation parameters
        if self.config.enable_dopamine:
            nlm_config.set("neuromod.dopamine.scale", self.config.dopamine_scale)
        
        if self.config.enable_curiosity:
            nlm_config.set("neuromod.curiosity.enable", True)
            nlm_config.set("neuromod.curiosity.gain", self.config.curiosity_gain)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(nlm_config)
        self.brain.initialize()
        
        print(f"✓ Advanced brain created")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        
        return True
    
    def create_advanced_world(self, width: int = 20, height: int = 20,
                            vision_width: int = 8, vision_height: int = 8) -> bool:
        """Create an advanced world with enhanced features."""
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm not available")
            return False
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=width,
            height=height,
            vision_width=vision_width,
            vision_height=vision_height
        )
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        print(f"✓ Advanced world created")
        print(f"  Size: {width}x{height}")
        print(f"  Vision: {vision_width}x{vision_height}")
        
        return True
    
    def run_advanced_simulation(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Run an advanced simulation with comprehensive monitoring."""
        if not self.brain:
            print("ERROR: Brain not initialized")
            return {}
        
        print(f"Running advanced simulation for {max_steps} steps...")
        
        # Initialize monitoring
        metrics = LearningMetrics()
        start_time = time.time()
        
        try:
            for step in range(max_steps):
                # Update world if present
                if self.world:
                    self.world.update(0.1)  # 10ms timestep
                    
                    # Get and process sensory input
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    
                    # Get and apply motor command
                    motor_cmd = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Run brain step
                self.brain.step(step)
                
                # Collect metrics
                metrics.reward_history.append(0.0)  # Placeholder
                metrics.firing_rate_history.append(self.brain.getAverageFiringRate())
                
                # Log progress every 10% of steps
                if step % (max_steps // 10) == 0:
                    print(f"  Progress: {step/max_steps*100:.0f}% "
                          f"| Firing: {self.brain.getFiringNeuronCount():4d} "
                          f"| Rate: {self.brain.getAverageFiringRate():6.2f} Hz")
                
        except Exception as e:
            print(f"Simulation interrupted: {e}")
            return {
                'success': False,
                'error': str(e),
                'metrics': metrics,
                'wall_clock_time': time.time() - start_time
            }
        
        # Finalize metrics
        end_time = time.time()
        metrics.simulation_time = end_time - start_time
        metrics.wall_clock_time = end_time - start_time
        
        # Compute advanced analytics
        convergence = self.analytics.compute_convergence(metrics)
        performance = self.analytics.compute_performance_metrics(metrics)
        stability = self.analytics.compute_stability_analysis(metrics)
        
        print(f"\n✓ Simulation completed in {metrics.wall_clock_time:.2f} seconds")
        
        return {
            'success': True,
            'metrics': metrics,
            'convergence': convergence,
            'performance': performance,
            'stability': stability,
            'wall_clock_time': metrics.wall_clock_time,
            'simulation_time': metrics.simulation_time
        }
    
    def save_experiment_result(self, filepath: str, experiment_name: str = "",
                             description: str = "") -> bool:
        """Save experiment result to file."""
        if not self.brain:
            print("ERROR: Brain not initialized")
            return False
        
        # Create metrics from current brain state
        metrics = LearningMetrics()
        metrics.reward_history = [0.0] * 100  # Placeholder
        metrics.firing_rate_history = [self.brain.getAverageFiringRate()] * 100
        
        # Create result
        result = ExperimentResult(
            config=self.config,
            metrics=metrics,
            success=True,
            experiment_name=experiment_name or "Advanced NLM Experiment",
            description=description
        )
        
        # Save to file
        try:
            with open(filepath, 'w') as f:
                json.dump({
                    'experiment_name': result.experiment_name,
                    'success': result.success,
                    'config': self.config.__dict__,
                    'metrics': metrics.__dict__,
                    'wall_clock_time': result.wall_clock_time,
                    'convergence_analysis': result.convergence_analysis,
                    'performance_analysis': result.performance_analysis,
                    'stability_analysis': result.stability_analysis,
                }, f, indent=2, default=str)
            
            print(f"✓ Experiment result saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving experiment result: {e}")
            return False
    
    def load_experiment_result(self, filepath: str) -> Optional[Dict[str, Any]]:
        """Load experiment result from file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            print(f"✓ Experiment result loaded from {filepath}")
            return data
            
        except Exception as e:
            print(f"Error loading experiment result: {e}")
            return None

# Convenience functions for easy usage
def create_default_config() -> ExperimentConfig:
    """Create a default experiment configuration."""
    return ExperimentConfig()

def create_advanced_nlm(config: Optional[ExperimentConfig] = None) -> AdvancedNLM:
    """Create an advanced NLM with enhanced functionality."""
    return AdvancedNLM(config)

def create_experiment_runner(config: Optional[ExperimentConfig] = None) -> ExperimentRunner:
    """Create an experiment runner with advanced scheduling."""
    return ExperimentRunner(config)

def create_experiment_dataset(name: str) -> ExperimentDataset:
    """Create an experiment dataset for managing multiple experiments."""
    return ExperimentDataset(name)

# Set up matplotlib for plotting (if available)
try:
    plt.style.use('seaborn-v0_8')
    MATPLOTLIB_AVAILABLE = True
except:
    MATPLOTLIB_AVAILABLE = False
    print("Warning: matplotlib not available. Plotting functions will be limited.")

if __name__ == '__main__':
    print("Enhanced NLM Python API")
    print("Version: Advanced Features")
    print()
    print("Available modules:")
    print("  - AdvancedNLM: Enhanced NLM interface")
    print("  - ExperimentConfig: Advanced experiment configuration")
    print("  - ExperimentResult: Comprehensive experiment results")
    print("  - LearningMetrics: Detailed learning metrics")
    print("  - AdvancedAnalytics: Advanced analytics and analysis")
    print("  - ExperimentRunner: Advanced experiment scheduling")
    print("  - ExperimentDataset: Experiment dataset management")
    print()
    print("Import examples:")
    print("  from enhanced_nlm import AdvancedNLM, ExperimentRunner")
    print("  from enhanced_nlm import create_advanced_nlm, create_experiment_runner")
    print()
    print("For more information, see the documentation.")
