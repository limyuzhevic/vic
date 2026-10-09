#!/usr/bin/env python3
"""
Experiment configuration utilities for NLM (Neural Learning Machine)

This module provides utilities for configuring and managing experiments,
including experiment templates, parameter optimization, and experiment tracking.
"""

import json
import os
import random
import numpy as np
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, field
from enum import Enum
import time
from pathlib import Path
from copy import deepcopy
import math
from dataclasses import asdict

# Mock NLM bindings for demonstration
class MockBrain:
    """Mock brain class for experiment configuration"""
    def __init__(self):
        self.total_neurons = 1000
        self.synapse_count = 5000
        self.region_count = 1
    
    def getTotalNeuronCount(self):
        return self.total_neurons
    
    def getTotalSynapseCount(self):
        return self.synapse_count
    
    def getRegionCount(self):
        return self.region_count
    
    def getAverageFiringRate(self):
        return np.random.random() * 0.8
    
    def getExcitationInhibitionRatio(self):
        return np.random.random() * 2.0 + 0.5
    
    def getCurrentStep(self):
        return 100
    
    def getCurrentTime(self):
        return 0.1
    
    def getWorkingMemory(self):
        return 50
    
    def step(self, step, timestep):
        pass
    
    def getTotalSpikeCount(self):
        return 1000
    
    def getFiringNeuronCount(self):
        return 50

# Mock config class
class MockConfig:
    """Mock config class for experiment configuration"""
    def __init__(self):
        self.data = {}
    
    def copy(self):
        return MockConfig()
    
    def set(self, key, value, source=None):
        self.data[key] = value
    
    def get(self, key, default=None):
        return self.data.get(key, default)

# Mock NLM module
class MockNLM:
    """Mock NLM module"""
    ConfigSource = type('ConfigSource', (), {'Default': 'default'})
    
    @staticmethod
    def createDefaultConfig():
        return MockConfig()
    
    @staticmethod
    def createBrain(config):
        return MockBrain()

# Replace pynlm with mock
import sys
sys.modules['pynlm'] = MockNLM

# Now import the real modules
import pynlm

class ExperimentType(Enum):
    """Types of experiments"""
    BASIC = "basic"
    STDP = "stdp"
    PLASTICITY = "plasticity"
    INTEGRATION = "integration"
    DEVELOPMENT = "development"
    COGNITION = "cognition"
    NEUROMODULATION = "neuromodulation"
    MEMORY = "memory"
    VISUALIZATION = "visualization"
    BENCHMARK = "benchmark"
    TRANSFER = "transfer"
    META_LEARNING = "meta_learning"

class ExperimentStatus(Enum):
    """Status of experiments"""
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    PAUSED = "paused"
    CANCELLED = "cancelled"

@dataclass
class ExperimentParameters:
    """Base parameters for experiments"""
    name: str = "experiment"
    description: str = ""
    experiment_type: ExperimentType = ExperimentType.BASIC
    status: ExperimentStatus = ExperimentStatus.PENDING
    
    # Simulation parameters
    neuron_count: int = 1000
    region_count: int = 1
    simulation_timestep: float = 0.001
    max_steps: int = 10000
    random_seed: int = 42
    
    # Learning parameters
    learning_rate: float = 0.01
    learning_rate_decay: float = 0.0001
    weight_decay: float = 0.0005
    momentum: float = 0.9
    epsilon: float = 1e-8
    batch_size: int = 32
    
    # Network architecture
    connection_probability: float = 0.1
    initial_weight_mean: float = 0.5
    initial_weight_std: float = 0.1
    
    # Plasticity parameters
    stdp_ltp_weight: float = 0.02
    stdp_ltd_weight: float = 0.015
    stdp_tau: float = 20.0
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation
    dopamine_scale: float = 1.0
    curiosity_enabled: bool = True
    novelty_enabled: bool = True
    prediction_error_enabled: bool = True
    
    # Development
    enable_development: bool = True
    development_start_step: int = 1000
    development_interval: int = 1000
    
    # Memory
    working_memory_capacity: int = 100
    episodic_memory_max_episodes: int = 1000
    associative_memory_capacity: int = 500
    
    # Results tracking
    track_firing_rate: bool = True
    track_weight_changes: bool = True
    track_memory_usage: bool = True
    track_plasticity_events: bool = True
    
    # Output
    save_results: bool = True
    results_dir: str = "./results"
    checkpoint_interval: int = 1000
    generate_plots: bool = True
    plot_format: str = "png"
    
    # Experiment-specific parameters
    experiment_specific: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict:
        """Convert parameters to dictionary"""
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'ExperimentParameters':
        """Create parameters from dictionary"""
        # Convert enum values back to enum objects
        if 'experiment_type' in data and isinstance(data['experiment_type'], str):
            data['experiment_type'] = ExperimentType(data['experiment_type'])
        
        if 'status' in data and isinstance(data['status'], str):
            data['status'] = ExperimentStatus(data['status'])
        
        return cls(**data)

@dataclass
class ExperimentResult:
    """Results from an experiment"""
    experiment_name: str
    experiment_type: ExperimentType
    timestamp: float
    
    # Basic metrics
    final_firing_rate: float = 0.0
    final_weight_change: float = 0.0
    total_spikes: int = 0
    average_firing_rate: float = 0.0
    
    # Plasticity metrics
    stdp_events: int = 0
    hebbian_events: int = 0
    structural_plasticity_events: int = 0
    synaptogenesis_count: int = 0
    pruning_count: int = 0
    
    # Memory metrics
    working_memory_traces: int = 0
    episodic_memory_episodes: int = 0
    associative_memory_associations: int = 0
    
    # Neuromodulation metrics
    dopamine_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    
    # Development metrics
    developmental_stage: int = 0
    plasticity_changes: float = 0.0
    
    # Performance metrics
    simulation_time: float = 0.0
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    
    # Results tracking
    firing_rate_history: List[float] = field(default_factory=list)
    weight_change_history: List[float] = field(default_factory=list)
    spike_count_history: List[int] = field(default_factory=list)
    memory_usage_history: List[float] = field(default_factory=list)
    
    # Metadata
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict:
        """Convert results to dictionary"""
        return asdict(self)

class ExperimentTemplate:
    """Template for common experiments"""
    
    @staticmethod
    def create_basic_learning_template(name: str = "basic_learning") -> ExperimentParameters:
        """Create template for basic learning experiment"""
        return ExperimentParameters(
            name=name,
            description="Basic learning experiment with STDP and Hebbian plasticity",
            experiment_type=ExperimentType.BASIC,
            neuron_count=1000,
            max_steps=5000,
            connection_probability=0.1,
            stdp_ltp_weight=0.02,
            stdp_ltd_weight=0.015,
            stdp_tau=20.0,
            synaptogenesis_rate=0.0001,
            pruning_rate=0.00001,
            enable_development=False,
            curiosity_enabled=True,
            novelty_enabled=True,
            track_firing_rate=True,
            track_weight_changes=True,
            track_memory_usage=True,
            track_plasticity_events=True
        )
    
    @staticmethod
    def create_stdp_verification_template(name: str = "stdp_verification") -> ExperimentParameters:
        """Create template for STDP verification experiment"""
        return ExperimentParameters(
            name=name,
            description="Experiment to verify STDP learning rules",
            experiment_type=ExperimentType.STDP,
            neuron_count=500,
            max_steps=2000,
            connection_probability=0.15,
            stdp_ltp_weight=0.025,
            stdp_ltd_weight=0.018,
            stdp_tau=15.0,
            enable_development=False,
            curiosity_enabled=False,
            novelty_enabled=False,
            track_firing_rate=True,
            track_weight_changes=True,
            track_plasticity_events=True,
            experiment_specific={
                'pre_post_timing': 'pre_before_post',
                'weight_change_threshold': 0.001
            }
        )
    
    @staticmethod
    def create_plasticity_learning_template(name: str = "plasticity_learning") -> ExperimentParameters:
        """Create template for plasticity learning experiment"""
        return ExperimentParameters(
            name=name,
            description="Experiment exploring plasticity mechanisms",
            experiment_type=ExperimentType.PLASTICITY,
            neuron_count=2000,
            max_steps=10000,
            connection_probability=0.05,
            stdp_ltp_weight=0.015,
            stdp_ltd_weight=0.012,
            stdp_tau=25.0,
            synaptogenesis_rate=0.0005,
            pruning_rate=0.00005,
            enable_development=True,
            development_start_step=2000,
            development_interval=2000,
            curiosity_enabled=True,
            novelty_enabled=True,
            track_firing_rate=True,
            track_weight_changes=True,
            track_memory_usage=True,
            track_plasticity_events=True
        )
    
    @staticmethod
    def create_integration_test_template(name: str = "integration_test") -> ExperimentParameters:
        """Create template for integration test"""
        return ExperimentParameters(
            name=name,
            description="Integration test for all brain systems",
            experiment_type=ExperimentType.INTEGRATION,
            neuron_count=1500,
            max_steps=8000,
            connection_probability=0.08,
            stdp_ltp_weight=0.018,
            stdp_ltd_weight=0.016,
            stdp_tau=18.0,
            synaptogenesis_rate=0.0002,
            pruning_rate=0.00002,
            enable_development=True,
            curiosity_enabled=True,
            novelty_enabled=True,
            prediction_error_enabled=True,
            track_firing_rate=True,
            track_weight_changes=True,
            track_memory_usage=True,
            track_plasticity_events=True,
            experiment_specific={
                'integration_test': True,
                'test_memory': True,
                'test_neuromodulation': True,
                'test_development': True,
                'test_prediction': True
            }
        )
    
    @staticmethod
    def create_development_template(name: str = "development") -> ExperimentParameters:
        """Create template for development experiment"""
        return ExperimentParameters(
            name=name,
            description="Experiment on brain development and plasticity",
            experiment_type=ExperimentType.DEVELOPMENT,
            neuron_count=1200,
            max_steps=15000,
            connection_probability=0.06,
            stdp_ltp_weight=0.02,
            stdp_ltd_weight=0.015,
            stdp_tau=20.0,
            synaptogenesis_rate=0.001,
            pruning_rate=0.0001,
            enable_development=True,
            development_start_step=1000,
            development_interval=500,
            curiosity_enabled=True,
            novelty_enabled=True,
            track_firing_rate=True,
            track_weight_changes=True,
            track_memory_usage=True,
            track_plasticity_events=True,
            experiment_specific={
                'developmental_stages': ['initial', 'critical_period', 'maturation', 'adult'],
                'plasticity_rates': [1.0, 0.8, 0.5, 0.2]
            }
        )
    
    @staticmethod
    def create_benchmark_template(name: str = "benchmark") -> ExperimentParameters:
        """Create template for benchmark experiment"""
        return ExperimentParameters(
            name=name,
            description="Benchmark performance of NLM implementation",
            experiment_type=ExperimentType.BENCHMARK,
            neuron_count=2000,
            max_steps=20000,
            connection_probability=0.12,
            stdp_ltp_weight=0.025,
            stdp_ltd_weight=0.02,
            stdp_tau=15.0,
            synaptogenesis_rate=0.0008,
            pruning_rate=0.00008,
            enable_development=True,
            curiosity_enabled=True,
            novelty_enabled=True,
            track_firing_rate=True,
            track_weight_changes=True,
            track_memory_usage=True,
            track_plasticity_events=True,
            experiment_specific={
                'benchmark_mode': True,
                'performance_metrics': ['firing_rate', 'memory_usage', 'cpu_usage', 'convergence_time']
            }
        )

class ParameterOptimizer:
    """Optimize experiment parameters using various methods"""
    
    def __init__(self):
        self.best_parameters = None
        self.optimization_history = []
        
    def random_search(self, param_space: Dict, num_trials: int = 50) -> Dict:
        """Optimize parameters using random search"""
        print(f"Running random search with {num_trials} trials...")
        
        best_score = float('inf')
        best_params = None
        
        for trial in range(num_trials):
            # Generate random parameters
            params = {}
            for param_name, param_range in param_space.items():
                if isinstance(param_range, tuple):
                    # Continuous parameter
                    min_val, max_val = param_range
                    params[param_name] = random.uniform(min_val, max_val)
                elif isinstance(param_range, list):
                    # Discrete parameter
                    params[param_name] = random.choice(param_range)
                else:
                    # Fixed parameter
                    params[param_name] = param_range
            
            # Evaluate parameters (simulated)
            score = self._evaluate_parameters(params)
            
            self.optimization_history.append({
                'trial': trial,
                'parameters': params,
                'score': score
            })
            
            if score < best_score:
                best_score = score
                best_params = params
                
                print(f"  Trial {trial + 1}: New best score = {score:.6f}")
        
        self.best_parameters = best_params
        return best_params
    
    def bayesian_optimization(self, param_space: Dict, num_trials: int = 30) -> Dict:
        """Optimize parameters using Bayesian optimization"""
        print(f"Running Bayesian optimization with {num_trials} trials...")
        
        # Initialize with random parameters
        best_params = {}
        for param_name, param_range in param_space.items():
            if isinstance(param_range, tuple):
                min_val, max_val = param_range
                best_params[param_name] = random.uniform(min_val, max_val)
            elif isinstance(param_range, list):
                best_params[param_name] = random.choice(param_range)
            else:
                best_params[param_name] = param_range
        
        best_score = self._evaluate_parameters(best_params)
        
        for trial in range(num_trials):
            # Generate new parameter suggestion
            new_params = self._suggest_parameters(param_space, self.optimization_history)
            
            # Evaluate new parameters
            score = self._evaluate_parameters(new_params)
            
            self.optimization_history.append({
                'trial': trial,
                'parameters': new_params,
                'score': score
            })
            
            if score < best_score:
                best_score = score
                best_params = new_params
                
                print(f"  Trial {trial + 1}: New best score = {score:.6f}")
        
        self.best_parameters = best_params
        return best_params
    
    def grid_search(self, param_space: Dict, num_combinations: int = 100) -> Dict:
        """Optimize parameters using grid search"""
        print(f"Running grid search with {num_combinations} combinations...")
        
        # Generate parameter combinations
        combinations = self._generate_parameter_combinations(param_space, num_combinations)
        
        best_score = float('inf')
        best_params = None
        
        for i, params in enumerate(combinations):
            # Evaluate parameters
            score = self._evaluate_parameters(params)
            
            self.optimization_history.append({
                'trial': i,
                'parameters': params,
                'score': score
            })
            
            if score < best_score:
                best_score = score
                best_params = params
                
                print(f"  Combination {i + 1}: New best score = {score:.6f}")
        
        self.best_parameters = best_params
        return best_params
    
    def _evaluate_parameters(self, params: Dict) -> float:
        """Evaluate parameters and return score (lower is better)"""
        # Simulated evaluation based on parameter values
        # In a real implementation, this would run an actual experiment
        
        score = 0.0
        
        # Weight parameters based on importance
        if 'neuron_count' in params:
            # Optimal neuron count around 1000
            neuron_penalty = abs(params['neuron_count'] - 1000) / 1000.0
            score += neuron_penalty * 10.0
        
        if 'connection_probability' in params:
            # Optimal connection probability around 0.1
            connection_penalty = abs(params['connection_probability'] - 0.1) / 0.1
            score += connection_penalty * 5.0
        
        if 'learning_rate' in params:
            # Optimal learning rate around 0.01
            lr_penalty = abs(params['learning_rate'] - 0.01) / 0.01
            score += lr_penalty * 8.0
        
        if 'max_steps' in params:
            # More steps should improve results but with diminishing returns
            steps_penalty = max(0.0, 1.0 - (params['max_steps'] / 20000.0))
            score += steps_penalty * 2.0
        
        return score
    
    def _suggest_parameters(self, param_space: Dict, history: List[Dict]) -> Dict:
        """Suggest new parameters based on Bayesian optimization"""
        # Simple implementation - in practice, use proper GP-based optimization
        if not history:
            return {param: random.uniform(0, 1) for param in param_space.keys()}
        
        # Calculate mean and variance of each parameter
        param_names = list(param_space.keys())
        param_means = {name: [] for name in param_names}
        param_vars = {name: [] for name in param_names}
        
        for entry in history:
            for name in param_names:
                if name in entry['parameters']:
                    param_means[name].append(entry['parameters'][name])
        
        # Generate new parameters with some noise
        new_params = {}
        for name in param_names:
            if param_means[name]:
                mean_val = np.mean(param_means[name])
                std_val = np.std(param_means[name]) if len(param_means[name]) > 1 else 0.1
                
                # Add noise
                new_params[name] = np.clip(
                    np.random.normal(mean_val, std_val),
                    param_space[name][0] if isinstance(param_space[name], tuple) else param_space[name],
                    param_space[name][1] if isinstance(param_space[name], tuple) else param_space[name]
                )
            else:
                # Fallback to random
                if isinstance(param_space[name], tuple):
                    new_params[name] = random.uniform(param_space[name][0], param_space[name][1])
                elif isinstance(param_space[name], list):
                    new_params[name] = random.choice(param_space[name])
                else:
                    new_params[name] = param_space[name]
        
        return new_params
    
    def _generate_parameter_combinations(self, param_space: Dict, max_combinations: int) -> List[Dict]:
        """Generate parameter combinations for grid search"""
        combinations = []
        
        # For simplicity, just generate random combinations
        for _ in range(max_combinations):
            params = {}
            for param_name, param_range in param_space.items():
                if isinstance(param_range, tuple):
                    params[param_name] = random.uniform(param_range[0], param_range[1])
                elif isinstance(param_range, list):
                    params[param_name] = random.choice(param_range)
                else:
                    params[param_name] = param_range
            
            combinations.append(params)
        
        return combinations

class ExperimentTracker:
    """Track and manage experiments"""
    
    def __init__(self, experiments_dir: str = "./experiments"):
        self.experiments_dir = experiments_dir
        self.experiment_registry = {}
        self.results_history = {}
        
        # Create experiments directory
        Path(experiments_dir).mkdir(parents=True, exist_ok=True)
        
        # Load existing experiments
        self._load_experiments()
    
    def register_experiment(self, name: str, parameters: ExperimentParameters) -> bool:
        """Register a new experiment"""
        try:
            if name in self.experiment_registry:
                print(f"Experiment '{name}' already registered")
                return False
            
            self.experiment_registry[name] = {
                'parameters': parameters,
                'timestamp': time.time(),
                'status': ExperimentStatus.PENDING.value
            }
            
            # Save experiment registry
            self._save_experiments()
            
            print(f"Experiment '{name}' registered successfully")
            return True
            
        except Exception as e:
            print(f"Error registering experiment: {e}")
            return False
    
    def load_experiment(self, name: str) -> Optional[ExperimentParameters]:
        """Load an experiment by name"""
        if name in self.experiment_registry:
            return self.experiment_registry[name]['parameters']
        
        return None
    
    def run_experiment(self, name: str, brain, config) -> ExperimentResult:
        """Run an experiment and return results"""
        try:
            if name not in self.experiment_registry:
                print(f"Experiment '{name}' not found")
                return None
            
            # Get experiment parameters
            experiment_info = self.experiment_registry[name]
            parameters = experiment_info['parameters']
            
            # Update status
            experiment_info['status'] = ExperimentStatus.RUNNING.value
            self._save_experiments()
            
            print(f"Running experiment '{name}'...")
            
            # Run experiment based on type
            result = self._run_experiment_by_type(name, parameters, brain, config)
            
            # Update status
            experiment_info['status'] = ExperimentStatus.COMPLETED.value
            experiment_info['result'] = result.__dict__
            self._save_experiments()
            
            # Store results
            if name not in self.results_history:
                self.results_history[name] = []
            
            self.results_history[name].append(result)
            
            # Save results
            self._save_experiment_results(name, result)
            
            print(f"Experiment '{name}' completed successfully")
            return result
            
        except Exception as e:
            print(f"Error running experiment '{name}': {e}")
            
            # Update status to failed
            if name in self.experiment_registry:
                self.experiment_registry[name]['status'] = ExperimentStatus.FAILED.value
                self._save_experiments()
            
            return None
    
    def _run_experiment_by_type(self, name: str, parameters: ExperimentParameters, brain, config) -> ExperimentResult:
        """Run experiment based on its type"""
        # Create configuration for the experiment
        experiment_config = config.copy() if hasattr(config, 'copy') else config
        
        # Set experiment-specific parameters
        experiment_config.set("neuron_count", parameters.neuron_count, pynlm.ConfigSource.Default)
        experiment_config.set("region_count", parameters.region_count, pynlm.ConfigSource.Default)
        experiment_config.set("connection_probability", parameters.connection_probability, pynlm.ConfigSource.Default)
        experiment_config.set("stdp_ltp_weight", parameters.stdp_ltp_weight, pynlm.ConfigSource.Default)
        experiment_config.set("stdp_ltd_weight", parameters.stdp_ltd_weight, pynlm.ConfigSource.Default)
        experiment_config.set("stdp_tau", parameters.stdp_tau, pynlm.ConfigSource.Default)
        experiment_config.set("synaptogenesis_rate", parameters.synaptogenesis_rate, pynlm.ConfigSource.Default)
        experiment_config.set("pruning_rate", parameters.pruning_rate, pynlm.ConfigSource.Default)
        
        # Run simulation
        start_time = time.time()
        
        # Collect metrics during simulation
        firing_rate_history = []
        weight_change_history = []
        spike_count_history = []
        memory_usage_history = []
        
        for step in range(parameters.max_steps):
            # Run simulation step
            brain.step(step, step * parameters.simulation_timestep)
            
            # Collect metrics periodically
            if step % 100 == 0:
                firing_rate_history.append(brain.getAverageFiringRate())
                spike_count_history.append(brain.getTotalSpikeCount())
                memory_usage_history.append(brain.getWorkingMemory() if hasattr(brain, 'getWorkingMemory') else 0)
                
                # Collect weight changes if needed
                if parameters.track_weight_changes:
                    # Simple weight change calculation (would be more sophisticated in real implementation)
                    weight_change_history.append(abs(brain.getExcitationInhibitionRatio() - 1.0))
        
        # Calculate final metrics
        final_firing_rate = brain.getAverageFiringRate()
        final_weight_change = abs(brain.getExcitationInhibitionRatio() - 1.0)
        total_spikes = brain.getTotalSpikeCount()
        average_firing_rate = np.mean(firing_rate_history) if firing_rate_history else 0.0
        
        # Calculate plasticity metrics
        stdp_events = 0  # Would need to track these during simulation
        hebbian_events = 0  # Would need to track these during simulation
        structural_plasticity_events = 0  # Would need to track these during simulation
        
        # Calculate development metrics
        developmental_stage = brain.getDevelopmentalStage() if hasattr(brain, 'getDevelopmentalStage') else 0
        
        # Calculate neuromodulation metrics
        dopamine_level = 0.0  # Would need to access dopamine system
        curiosity_level = 0.0  # Would need to access curiosity system
        novelty_level = 0.0  # Would need to access novelty detection system
        prediction_error = 0.0  # Would need to access prediction error system
        
        # Calculate memory metrics
        working_memory_traces = 0  # Would need to access working memory
        episodic_memory_episodes = 0  # Would need to access episodic memory
        associative_memory_associations = 0  # Would need to access associative memory
        
        # Create result
        result = ExperimentResult(
            experiment_name=name,
            experiment_type=parameters.experiment_type,
            timestamp=time.time(),
            final_firing_rate=final_firing_rate,
            final_weight_change=final_weight_change,
            total_spikes=total_spikes,
            average_firing_rate=average_firing_rate,
            stdp_events=stdp_events,
            hebbian_events=hebbian_events,
            structural_plasticity_events=structural_plasticity_events,
            synaptogenesis_count=0,  # Would need to track
            pruning_count=0,  # Would need to track
            working_memory_traces=working_memory_traces,
            episodic_memory_episodes=episodic_memory_episodes,
            associative_memory_associations=associative_memory_associations,
            dopamine_level=dopamine_level,
            curiosity_level=curiosity_level,
            novelty_level=novelty_level,
            prediction_error=prediction_error,
            developmental_stage=developmental_stage,
            plasticity_changes=final_weight_change,
            simulation_time=time.time() - start_time,
            cpu_usage=0.0,  # Would need to monitor CPU
            memory_usage=0.0,  # Would need to monitor memory
            firing_rate_history=firing_rate_history,
            weight_change_history=weight_change_history,
            spike_count_history=spike_count_history,
            memory_usage_history=memory_usage_history,
            metadata={
                'parameters_used': parameters.to_dict(),
                'neuron_count': parameters.neuron_count,
                'region_count': parameters.region_count,
                'connection_probability': parameters.connection_probability
            }
        )
        
        return result
    
    def _save_experiments(self) -> None:
        """Save experiment registry"""
        registry_file = os.path.join(self.experiments_dir, "experiment_registry.json")
        
        with open(registry_file, 'w') as f:
            json.dump(self.experiment_registry, f, indent=2, default=str)
    
    def _load_experiments(self) -> None:
        """Load experiment registry"""
        registry_file = os.path.join(self.experiments_dir, "experiment_registry.json")
        
        if os.path.exists(registry_file):
            with open(registry_file, 'r') as f:
                self.experiment_registry = json.load(f)
    
    def _save_experiment_results(self, name: str, result: ExperimentResult) -> None:
        """Save experiment results"""
        results_file = os.path.join(self.experiments_dir, f"{name}_results.json")
        
        with open(results_file, 'w') as f:
            json.dump(result.to_dict(), f, indent=2, default=str)
        
        # Add to results history
        if name not in self.results_history:
            self.results_history[name] = []
        
        self.results_history[name].append(result)
    
    def get_experiment_results(self, name: str) -> Optional[List[ExperimentResult]]:
        """Get experiment results"""
        if name in self.results_history:
            return self.results_history[name]
        
        return None
    
    def list_experiments(self) -> List[str]:
        """List all registered experiments"""
        return list(self.experiment_registry.keys())

class ExperimentConfigManager:
    """Manage experiment configurations and settings"""
    
    def __init__(self, config_dir: str = "./configs/experiments"):
        self.config_dir = config_dir
        self.config_templates = {}
        
        # Create config directory
        Path(config_dir).mkdir(parents=True, exist_ok=True)
        
        # Load default templates
        self._load_config_templates()
    
    def _load_config_templates(self) -> None:
        """Load default configuration templates"""
        templates = {
            'basic_learning': ExperimentTemplate.create_basic_learning_template(),
            'stdp_verification': ExperimentTemplate.create_stdp_verification_template(),
            'plasticity_learning': ExperimentTemplate.create_plasticity_learning_template(),
            'integration_test': ExperimentTemplate.create_integration_test_template(),
            'development': ExperimentTemplate.create_development_template(),
            'benchmark': ExperimentTemplate.create_benchmark_template()
        }
        
        for name, template in templates.items():
            self.config_templates[name] = template
    
    def get_template(self, name: str) -> Optional[ExperimentParameters]:
        """Get experiment template by name"""
        return self.config_templates.get(name)
    
    def save_template(self, name: str, parameters: ExperimentParameters) -> bool:
        """Save experiment template"""
        try:
            self.config_templates[name] = parameters
            
            # Save to file
            template_file = os.path.join(self.config_dir, f"{name}_template.json")
            with open(template_file, 'w') as f:
                # Convert to dict for JSON serialization
                template_dict = parameters.to_dict()
                
                # Convert enum values to strings
                if 'experiment_type' in template_dict:
                    template_dict['experiment_type'] = template_dict['experiment_type'].value
                
                if 'status' in template_dict:
                    template_dict['status'] = template_dict['status'].value
                
                json.dump(template_dict, f, indent=2)
            
            print(f"Template '{name}' saved successfully")
            return True
            
        except Exception as e:
            print(f"Error saving template '{name}': {e}")
            return False
    
    def load_template(self, name: str) -> bool:
        """Load experiment template by name"""
        try:
            template_file = os.path.join(self.config_dir, f"{name}_template.json")
            
            if not os.path.exists(template_file):
                print(f"Template file '{template_file}' not found")
                return False
            
            with open(template_file, 'r') as f:
                template_dict = json.load(f)
            
            # Convert string values back to enum objects
            if 'experiment_type' in template_dict:
                template_dict['experiment_type'] = ExperimentType(template_dict['experiment_type'])
            
            if 'status' in template_dict:
                template_dict['status'] = ExperimentStatus(template_dict['status'])
            
            # Create template object
            template = ExperimentParameters.from_dict(template_dict)
            
            self.config_templates[name] = template
            
            print(f"Template '{name}' loaded successfully")
            return True
            
        except Exception as e:
            print(f"Error loading template '{name}': {e}")
            return False
    
    def list_templates(self) -> List[str]:
        """List all available templates"""
        return list(self.config_templates.keys())
    
    def export_template(self, name: str, filepath: str) -> bool:
        """Export template to file"""
        try:
            if name not in self.config_templates:
                print(f"Template '{name}' not found")
                return False
            
            template = self.config_templates[name]
            
            # Save to file
            with open(filepath, 'w') as f:
                template_dict = template.to_dict()
                json.dump(template_dict, f, indent=2)
            
            print(f"Template '{name}' exported to '{filepath}'")
            return True
            
        except Exception as e:
            print(f"Error exporting template '{name}': {e}")
            return False
    
    def import_template(self, filepath: str) -> bool:
        """Import template from file"""
        try:
            # Load template from file
            with open(filepath, 'r') as f:
                template_dict = json.load(f)
            
            # Create template object
            template = ExperimentParameters.from_dict(template_dict)
            
            # Get template name from filename (without extension)
            template_name = os.path.splitext(os.path.basename(filepath))[0]
            
            # Save template
            self.config_templates[template_name] = template
            
            print(f"Template '{template_name}' imported successfully")
            return True
            
        except Exception as e:
            print(f"Error importing template from '{filepath}': {e}")
            return False

# Main function for experiment configuration management
def experiment_config_manager() -> ExperimentConfigManager:
    """Create experiment configuration manager instance"""
    return ExperimentConfigManager()

# Example usage
if __name__ == "__main__":
    print("Initializing Experiment Configuration Manager...")
    
    # Create config manager
    config_manager = experiment_config_manager()
    
    # List available templates
    print(f"\nAvailable templates:")
    for template_name in config_manager.list_templates():
        print(f"  - {template_name}")
    
    # Get a template
    basic_template = config_manager.get_template("basic_learning")
    if basic_template:
        print(f"\nBasic learning template:")
        print(f"  Name: {basic_template.name}")
        print(f"  Description: {basic_template.description}")
        print(f"  Neuron count: {basic_template.neuron_count}")
        print(f"  Max steps: {basic_template.max_steps}")
        print(f"  Connection probability: {basic_template.connection_probability}")
    
    # Save a custom template
    custom_template = ExperimentParameters(
        name="custom_experiment",
        description="Custom experiment for testing",
        neuron_count=2000,
        max_steps=15000,
        connection_probability=0.08
    )
    
    config_manager.save_template("custom_experiment", custom_template)
    
    print(f"\nExperiment configuration management complete.")