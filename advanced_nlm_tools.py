#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Utilities Package

Advanced Python utilities and tools for working with the NLM neural simulation framework.
This package provides enhanced functionality for experiment management, analysis,
configuration, and simulation control that extends the basic pynlm bindings.
"""

import json
import time
import numpy as np
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import threading
from concurrent.futures import ThreadPoolExecutor
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class SimulationState(Enum):
    """States for simulation control."""
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    STEP_MODE = "step_mode"

@dataclass
class ExperimentConfig:
    """Configuration for NLM experiments."""
    name: str
    description: str = ""
    max_steps: int = 1000
    timestep: float = 0.1
    random_seed: int = 42
    verbose: bool = True
    save_checkpoints: bool = False
    checkpoint_interval: int = 100
    callbacks: List[Callable] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class ExperimentResults:
    """Results from an NLM experiment."""
    name: str
    duration: float
    steps_completed: int
    final_firing_rate: float
    total_spikes: int
    learning_metrics: Dict[str, Any]
    checkpoint_files: List[str] = field(default_factory=list)
    error: Optional[str] = None
    success: bool = True

class NLMExperimentRunner:
    """
    Enhanced experiment runner for NLM simulations with advanced features.
    
    Provides:
    - Simulation control (pause, resume, step modes)
    - Callback system for monitoring
    - Checkpoint management
    - Performance metrics collection
    - Progress tracking
    """
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.state = SimulationState.STOPPED
        self.current_step = 0
        self.step_count = 0
        self.start_time = None
        self.pause_time = None
        self.callbacks = []
        self.metrics_history = []
        self.checkpoints = []
        self.lock = threading.Lock()
        self._executor = ThreadPoolExecutor(max_workers=1)
        
        if not (brain and world and agent):
            raise ValueError("Brain, world, and agent are required for experiment runner")
    
    def add_callback(self, callback: Callable, event_type: str = "step"):
        """Add a callback function for simulation events."""
        self.callbacks.append((callback, event_type))
    
    def run_experiment(self, config: ExperimentConfig) -> ExperimentResults:
        """Run an NLM experiment with advanced control features."""
        with self.lock:
            if self.state != SimulationState.STOPPED:
                raise RuntimeError("Experiment already running. Stop current experiment first.")
            
            self.state = SimulationState.RUNNING
            self.start_time = time.time()
            self.current_step = 0
            self.step_count = 0
            self.metrics_history.clear()
            self.checkpoints.clear()
        
        try:
            logger.info(f"Starting experiment: {config.name}")
            logger.info(f"Description: {config.description}")
            logger.info(f"Max steps: {config.max_steps}, Timestep: {config.timestep}")
            
            for step in range(config.max_steps):
                if self.state == SimulationState.STOPPED:
                    break
                elif self.state == SimulationState.PAUSED:
                    time.sleep(0.01)
                    continue
                elif self.state == SimulationState.STEP_MODE:
                    if not self.step():
                        break
                else:
                    self.step()
                
                self.step_count += 1
                
                # Collect metrics
                metrics = self._collect_metrics(step)
                self.metrics_history.append(metrics)
                
                # Save checkpoint
                if config.save_checkpoints and self.step_count % config.checkpoint_interval == 0:
                    checkpoint = self.save_checkpoint(f"{config.name}_step_{self.step_count}")
                    self.checkpoints.append(checkpoint)
                
                # Execute callbacks
                for callback, event_type in self.callbacks:
                    try:
                        callback(self, metrics, event_type)
                    except Exception as e:
                        logger.warning(f"Callback {callback} failed: {e}")
                
                # Log progress
                if config.verbose and self.step_count % 100 == 0:
                    logger.info(f"Step {self.step_count}/{config.max_steps}: "
                              f"Firing rate: {metrics.get('firing_rate', 0):.3f}, "
                              f"Total spikes: {metrics.get('total_spikes', 0)}")
            
            duration = time.time() - self.start_time
            final_metrics = self.metrics_history[-1] if self.metrics_history else {}
            
            result = ExperimentResults(
                name=config.name,
                duration=duration,
                steps_completed=self.step_count,
                final_firing_rate=final_metrics.get('firing_rate', 0.0),
                total_spikes=final_metrics.get('total_spikes', 0),
                learning_metrics=self._analyze_learning_metrics(),
                checkpoint_files=self.checkpoints
            )
            
            logger.info(f"Experiment {config.name} completed in {duration:.2f}s")
            logger.info(f"Final metrics: {result.learning_metrics}")
            
            return result
            
        except Exception as e:
            logger.error(f"Experiment {config.name} failed: {e}")
            return ExperimentResults(
                name=config.name,
                duration=time.time() - self.start_time if self.start_time else 0,
                steps_completed=self.step_count,
                final_firing_rate=0.0,
                total_spikes=0,
                learning_metrics={},
                error=str(e),
                success=False
            )
        finally:
            with self.lock:
                self.state = SimulationState.STOPPED
                self._executor.shutdown(wait=True)
    
    def step(self) -> bool:
        """Execute one simulation step. Returns False if simulation should stop."""
        if self.state != SimulationState.RUNNING:
            return False
        
        try:
            # Update world
            if self.world:
                self.world.update(0.1)
            
            # Get sensory input
            if self.world:
                percept = self.world.getSensoryPercept()
                if self.agent:
                    self.agent.processSensoryInput(percept)
            
            # Brain step
            if self.brain:
                self.brain.step(self.current_step)
            
            # Get action
            if self.agent:
                action = self.agent.decodeMotorCommand()
                if self.world:
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            self.current_step += 1
            return True
            
        except Exception as e:
            logger.error(f"Step failed: {e}")
            return False
    
    def _collect_metrics(self, step: int) -> Dict[str, Any]:
        """Collect metrics for current simulation state."""
        metrics = {
            'step': step,
            'simulation_time': self.world.getSimulationTime() if self.world else 0,
            'brain_firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
            'firing_neuron_count': self.brain.getFiringNeuronCount() if self.brain else 0,
            'total_spikes': self.brain.getTotalSpikeCount() if self.brain else 0,
            'total_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
            'total_synapses': self.brain.getTotalSynapseCount() if self.brain else 0,
            'development_stage': self.brain.getDevelopmentalStage() if self.brain else 0,
        }
        
        if self.agent:
            metrics.update({
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            })
        
        if self.world:
            body = self.world.getAgentBody()
            if body:
                metrics.update({
                    'agent_x': body.x,
                    'agent_y': body.y,
                    'agent_energy': body.energy,
                    'agent_age': body.age,
                })
        
        return metrics
    
    def _analyze_learning_metrics(self) -> Dict[str, Any]:
        """Analyze learning patterns from collected metrics."""
        if not self.metrics_history:
            return {}
        
        firing_rates = [m.get('brain_firing_rate', 0) for m in self.metrics_history]
        spike_counts = [m.get('total_spikes', 0) for m in self.metrics_history]
        
        analysis = {
            'firing_rate_trend': self._calculate_trend(firing_rates),
            'spike_activity_trend': self._calculate_trend(spike_counts),
            'stability': self._calculate_stability(firing_rates),
            'exploration_index': self._calculate_exploration_index(),
            'peak_activity': max(firing_rates) if firing_rates else 0,
            'average_activity': np.mean(firing_rates) if firing_rates else 0,
            'coherence': self._calculate_coherence(),
        }
        
        return analysis
    
    def _calculate_trend(self, values: List[float]) -> float:
        """Calculate linear trend of values over time."""
        if len(values) < 2:
            return 0.0
        
        indices = np.arange(len(values))
        if np.std(indices) == 0:
            return 0.0
        
        correlation = np.corrcoef(indices, values)[0, 1]
        return correlation
    
    def _calculate_stability(self, values: List[float]) -> float:
        """Calculate stability (inverse of variance)."""
        if len(values) < 2:
            return 1.0
        
        return 1.0 / (1.0 + np.var(values))
    
    def _calculate_exploration_index(self) -> float:
        """Calculate exploration-exploitation balance."""
        if not self.metrics_history or not self.agent:
            return 0.5
        
        curiosity_levels = [m.get('curiosity_level', 0) for m in self.metrics_history]
        return np.mean(curiosity_levels)
    
    def _calculate_coherence(self) -> float:
        """Calculate coherence between different metrics."""
        if not self.metrics_history:
            return 0.0
        
        metrics = self.metrics_history[0]
        if len(metrics) < 2:
            return 0.0
        
        # Simple coherence calculation based on correlation
        values = list(metrics.values())
        if len(values) < 2:
            return 0.0
        
        correlation = np.corrcoef(values)[0, 1]
        return abs(correlation)
    
    def pause(self):
        """Pause the simulation."""
        with self.lock:
            self.state = SimulationState.PAUSED
    
    def resume(self):
        """Resume the simulation."""
        with self.lock:
            self.state = SimulationState.RUNNING
    
    def step_mode(self):
        """Set simulation to step mode."""
        with self.lock:
            self.state = SimulationState.STEP_MODE
    
    def save_checkpoint(self, name: str) -> str:
        """Save current simulation state as checkpoint."""
        import pynlm
        
        checkpoint_file = f"{name}.bin"
        if self.brain:
            self.brain.save(checkpoint_file)
        
        checkpoint_data = {
            'name': name,
            'step': self.step_count,
            'simulation_time': self.world.getSimulationTime() if self.world else 0,
            'timestamp': time.time(),
            'file': checkpoint_file
        }
        
        self.checkpoints.append(checkpoint_data)
        return checkpoint_file
    
    def load_checkpoint(self, checkpoint_file: str):
        """Load simulation state from checkpoint."""
        if not self.brain:
            raise RuntimeError("Brain is required for checkpoint loading")
        
        if not self.brain.load(checkpoint_file):
            raise RuntimeError(f"Failed to load checkpoint from {checkpoint_file}")
        
        # Find checkpoint metadata
        for checkpoint in self.checkpoints:
            if checkpoint['file'] == checkpoint_file:
                self.step_count = checkpoint['step']
                break

class NLMConfigAnalyzer:
    """
    Advanced configuration analysis and validation for NLM simulations.
    
    Provides:
    - Configuration validation
    - Parameter optimization suggestions
    - Configuration comparison
    - Performance prediction
    - Network analysis
    """
    
    def __init__(self):
        self.analyzed_configs = []
    
    def validate_config(self, config) -> Dict[str, Any]:
        """Validate configuration and provide optimization suggestions."""
        validation_result = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': [],
            'score': 100.0
        }
        
        # Check for required parameters
        required_params = ['brain.neuron_count', 'simulation_timestep']
        for param in required_params:
            if not hasattr(config, 'has') or not config.has(param):
                validation_result['warnings'].append(f"Missing parameter: {param}")
                validation_result['score'] -= 10.0
        
        # Check parameter ranges
        if hasattr(config, 'has') and config.has('brain.neuron_count'):
            neuron_count = config.getOr<int64_t>('brain.neuron_count', 0)
            if neuron_count < 10 or neuron_count > 10000:
                validation_result['warnings'].append(
                    f"Neuron count {neuron_count} may be outside optimal range (100-5000)"
                )
                validation_result['score'] -= 15.0
        
        if hasattr(config, 'has') and config.has('simulation_timestep'):
            timestep = config.getOr<double>('simulation_timestep', 0.0)
            if timestep <= 0 or timestep > 0.1:
                validation_result['errors'].append(
                    f"Invalid timestep {timestep}. Must be positive and <= 0.1s"
                )
                validation_result['valid'] = False
                validation_result['score'] -= 50.0
        
        # Suggest optimizations
        if hasattr(config, 'has') and config.has('plasticity.stdp.enable'):
            if not config.getOr<bool>('plasticity.stdp.enable', True):
                validation_result['suggestions'].append(
                    "Consider enabling STDP for better learning performance"
                )
        
        if hasattr(config, 'has') and config.has('neuromod.dopamine.scale'):
            dopamine_scale = config.getOr<float>('neuromod.dopamine.scale', 1.0)
            if abs(dopamine_scale - 1.0) > 0.5:
                validation_result['suggestions'].append(
                    f"Dopamine scale {dopamine_scale} is quite extreme. Consider values near 1.0"
                )
        
        self.analyzed_configs.append({
            'config': config,
            'validation_result': validation_result,
            'timestamp': time.time()
        })
        
        return validation_result
    
    def compare_configs(self, configs: List) -> Dict[str, Any]:n
        """Compare multiple configurations and provide recommendations."""
        comparison = {
            'configs': [],
            'recommendations': [],
            'performance_estimates': {}
        }
        
        for config in configs:
            # Simple performance estimation based on configuration
            score = 100.0
            
            # Parameter-based scoring
            if hasattr(config, 'has') and config.has('brain.neuron_count'):
                neurons = config.getOr<int64_t>('brain.neuron_count', 0)
                if 1000 <= neurons <= 5000:
                    score += 20.0
                elif 100 <= neurons <= 10000:
                    score += 10.0
            
            if hasattr(config, 'has') and config.has('plasticity.stdp.enable'):
                if config.getOr<bool>('plasticity.stdp.enable', True):
                    score += 30.0
            
            comparison['performance_estimates'][str(config)] = {
                'estimated_performance': score,
                'expected_firing_rate': min(max(neurons / 100, 1.0), 50.0) if neurons else 10.0
            }
            
            comparison['configs'].append({
                'config': config,
                'performance_score': score,
                'parameters': self._extract_key_parameters(config)
            })
        
        # Generate recommendations
        best_config = max(comparison['performance_estimates'].items(), 
                         key=lambda x: x[1]['estimated_performance'])
        
        if best_config[0] != str(configs[-1]):  # Not the last config
            comparison['recommendations'].append(
                f"Consider using configuration with highest performance score"
            )
        
        return comparison
    
    def _extract_key_parameters(self, config) -> Dict[str, Any]:
        """Extract key parameters from configuration."""
        params = {}
        
        # Get a few key parameters
        key_paths = [
            'brain.neuron_count',
            'brain.synapse_density',
            'plasticity.stdp.enable',
            'neuromod.dopamine.scale',
            'neuromod.curiosity.enable'
        ]
        
        for path in key_paths:
            try:
                if hasattr(config, 'has') and config.has(path):
                    # This is a simplified parameter extraction
                    # In a real implementation, you'd need to handle nested config objects
                    params[path] = f"value_at_{path}"
            except:
                pass
        
        return params

class NLMAnalysisTools:
    """
    Advanced analysis tools for NLM simulation data.
    
    Provides:
    - Neural activity analysis
    - Learning pattern analysis
    - Statistical analysis
    - Visualization helpers
    - Performance profiling
    """
    
    @staticmethod
    def analyze_firing_patterns(spike_times: List[float], window_size: float = 1.0) -> Dict[str, Any]:
        """Analyze neural firing patterns from spike times."""
        if not spike_times:
            return {'error': 'No spike times provided'}
        
        spike_times = sorted(spike_times)
        
        # Calculate basic statistics
        firing_rate = len(spike_times) / (max(spike_times) - min(spike_times) if len(spike_times) > 1 else 1.0)
        
        # Inter-spike intervals
        isi = np.diff(spike_times)
        mean_isi = np.mean(isi) if len(isi) > 0 else 0
        std_isi = np.std(isi) if len(isi) > 0 else 0
        
        # Burst detection (simple)
        burst_threshold = mean_isi * 0.5
        burst_count = sum(isi < burst_threshold) if len(isi) > 0 else 0
        
        # Power spectrum
        if len(spike_times) >= 10:
            from scipy import signal
            fs = 1000  # Sampling frequency
            t = np.array(spike_times) * fs
            f, Pxx = signal.welch(t, fs, nperseg=min(len(t), 256))
            peak_freq = f[np.argmax(Pxx)] if len(Pxx) > 0 else 0
        else:
            peak_freq = 0
        
        return {
            'firing_rate': firing_rate,
            'mean_isi': mean_isi,
            'isi_std': std_isi,
            'burst_count': burst_count,
            'burst_ratio': burst_count / len(isi) if len(isi) > 0 else 0,
            'peak_frequency': peak_freq,
            'regularity': 1.0 / (1.0 + std_isi) if std_isi > 0 else 1.0
        }
    
    @staticmethod
    def calculate_learning_curve(steps: List[int], values: List[float]) -> Dict[str, Any]:
        """Calculate learning curve and metrics."""
        if len(steps) != len(values):
            return {'error': 'Steps and values must have same length'}
        
        # Smooth the curve
        window_size = min(10, len(values))
        if window_size > 1:
            kernel = np.ones(window_size) / window_size
            smoothed = np.convolve(values, kernel, mode='same')
        else:
            smoothed = values
        
        # Calculate learning metrics
        initial = values[0] if len(values) > 0 else 0
        final = values[-1] if len(values) > 0 else 0
        improvement = final - initial
        
        # Calculate learning rate (slope of smoothed curve)
        if len(smoothed) >= 2:
            x = np.arange(len(smoothed))
            slope = np.polyfit(x, smoothed, 1)[0]
        else:
            slope = 0
        
        # Calculate plateaus
        plateaus = NLMAnalysisTools._find_plateaus(smoothed)
        
        return {
            'steps': steps,
            'values': values,
            'smoothed': smoothed.tolist(),
            'initial': initial,
            'final': final,
            'improvement': improvement,
            'improvement_percentage': (improvement / initial * 100) if initial > 0 else 0,
            'learning_rate': slope,
            'plateaus': plateaus,
            'convergence_step': NLMAnalysisTools._find_convergence(smoothed)
        }
    
    @staticmethod
    def _find_plateaus(smoothed_values: np.ndarray, threshold: float = 0.01) -> List[Dict]:
        """Find plateaus in smoothed values."""
        plateaus = []
        in_plateau = False
        plateau_start = 0
        
        for i in range(1, len(smoothed_values)):
            if not in_plateau and abs(smoothed_values[i] - smoothed_values[i-1]) < threshold:
                in_plateau = True
                plateau_start = i - 1
            elif in_plateau and abs(smoothed_values[i] - smoothed_values[i-1]) >= threshold:
                if i - plateau_start > 5:  # Minimum plateau length
                    plateaus.append({
                        'start': plateau_start,
                        'end': i - 1,
                        'value': np.mean(smoothed_values[plateau_start:i]),
                        'length': i - plateau_start
                    })
                in_plateau = False
        
        if in_plateau and len(smoothed_values) - plateau_start > 5:
            plateaus.append({
                'start': plateau_start,
                'end': len(smoothed_values) - 1,
                'value': np.mean(smoothed_values[plateau_start:]),
                'length': len(smoothed_values) - plateau_start
            })
        
        return plateaus
    
    @staticmethod
    def _find_convergence(values: np.ndarray, threshold: float = 0.05) -> int:
        """Find convergence step."""
        if len(values) < 10:
            return len(values) - 1
        
        for i in range(10, len(values)):
            recent_mean = np.mean(values[i-10:i])
            overall_mean = np.mean(values)
            
            if abs(recent_mean - overall_mean) / overall_mean < threshold:
                return i
        
        return len(values) - 1
    
    @staticmethod
    def batch_analyze_experiments(experiment_results: List[Dict]) -> Dict[str, Any]:
        """Analyze multiple experiments and provide comparative insights."""
        if not experiment_results:
            return {'error': 'No experiment results provided'}
        
        analysis = {
            'experiment_count': len(experiment_results),
            'successful_experiments': sum(1 for exp in experiment_results if exp.get('success', False)),
            'failed_experiments': sum(1 for exp in experiment_results if not exp.get('success', False)),
            'average_duration': np.mean([exp.get('duration', 0) for exp in experiment_results]),
            'duration_std': np.std([exp.get('duration', 0) for exp in experiment_results]),
            'parameter_space_coverage': NLMAnalysisTools._analyze_parameter_space(experiment_results),
            'performance_rankings': NLMAnalysisTools._rank_performance(experiment_results),
            'recommendations': []
        }
        
        # Generate recommendations based on analysis
        if analysis['failed_experiments'] > analysis['experiment_count'] * 0.2:
            analysis['recommendations'].append(
                "High failure rate detected. Consider tuning hyperparameters or reducing complexity."
            )
        
        if analysis['duration_std'] > analysis['average_duration'] * 0.5:
            analysis['recommendations'].append(
                "High variance in experiment duration. Consider more consistent configuration."
            )
        
        return analysis
    
    @staticmethod
    def _analyze_parameter_space(experiment_results: List[Dict]) -> Dict[str, Any]:
        """Analyze parameter space coverage."""
        param_coverage = {}
        
        for exp in experiment_results:
            if 'config' in exp:
                config = exp['config']
                # Extract simple parameters for analysis
                for key in ['neuron_count', 'timestep', 'learning_rate']:
                    if key in config:
                        if key not in param_coverage:
                            param_coverage[key] = {'min': config[key], 'max': config[key], 'values': []}
                        else:
                            param_coverage[key]['min'] = min(param_coverage[key]['min'], config[key])
                            param_coverage[key]['max'] = max(param_coverage[key]['max'], config[key])
                        param_coverage[key]['values'].append(config[key])
        
        return param_coverage
    
    @staticmethod
    def _rank_performance(experiment_results: List[Dict]) -> List[Dict]:
        """Rank experiments by performance."""
        ranked = []
        
        for exp in experiment_results:
            if exp.get('success', False):
                # Calculate performance score based on available metrics
                score = 0.0
                metrics = exp.get('learning_metrics', {})
                
                if 'firing_rate_trend' in metrics:
                    score += abs(metrics['firing_rate_trend']) * 50
                
                if 'stability' in metrics:
                    score += metrics['stability'] * 30
                
                if 'exploration_index' in metrics:
                    score += (1.0 - abs(metrics['exploration_index'] - 0.5)) * 20
                
                ranked.append({
                    'experiment_name': exp.get('name', 'unnamed'),
                    'performance_score': score,
                    'metrics': metrics
                })
        
        ranked.sort(key=lambda x: x['performance_score'], reverse=True)
        return ranked

# Global instances
experiment_runner = None
config_analyzer = NLMConfigAnalyzer()
analysis_tools = NLMAnalysisTools()

def get_experiment_runner(brain=None, world=None, agent=None):
    """Get or create experiment runner instance."""
    global experiment_runner
    if experiment_runner is None and (brain or world or agent):
        experiment_runner = NLMExperimentRunner(brain, world, agent)
    return experiment_runner

def get_config_analyzer():
    """Get configuration analyzer instance."""
    return config_analyzer

def get_analysis_tools():
    """Get analysis tools instance."""
    return analysis_tools

# Example usage functions
def create_advanced_brain_config(neuron_count=1000, enable_learning=True, 
                                enable_development=True, enable_curiosity=True) -> Any:
    """Create an advanced brain configuration."""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    
    # Set basic parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Set plasticity
    config.set("plasticity.stdp.enable", enable_learning)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", enable_learning)
    config.set("plasticity.structural.enable", True)
    
    # Set neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", enable_curiosity)
    config.set("neuromod.novelty.enable", True)
    
    # Set development
    config.set("development.enable", enable_development)
    config.set("development.initial_stage", "CriticalPeriod")
    
    # Set prediction
    config.set("prediction.enable", True)
    config.set("prediction.horizon", 10)
    
    return config

def run_advanced_simulation(brain_config=None, world_config=None, 
                          agent_config=None, num_steps=1000) -> Dict[str, Any]:
    """Run an advanced simulation with the given configurations."""
    import pynlm
    
    # Create components
    brain = pynlm.createBrain(brain_config or create_advanced_brain_config())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    if world_config:
        world.configure(**world_config)
    else:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with experiment runner
    runner = NLMExperimentRunner(brain, world, agent)
    
    config = ExperimentConfig(
        name="Advanced Simulation",
        max_steps=num_steps,
        timestep=0.1,
        save_checkpoints=True,
        verbose=True
    )
    
    result = runner.run_experiment(config)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'experiment_result': result,
        'runner': runner
    }