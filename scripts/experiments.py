#!/usr/bin/env python3
"""
NLM Advanced Experimentation Utilities
Comprehensive experiment framework for advanced research and testing
"""

import sys
import os
import time
import statistics
import itertools
from typing import Dict, List, Optional, Callable, Tuple, Any
import json
import copy
from dataclasses import dataclass, field
from enum import Enum

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm

class ExperimentType(Enum):
    PLACEMENT = "placement"
    PLASTICITY = "plasticity"
    DEVELOPMENT = "development"
    NEUROMODULATION = "neuromodulation"
    COGNITION = "cognition"
    INTEGRATION = "integration"
    COMPARATIVE = "comparative"

@dataclass
class ExperimentConfig:
    """Configuration for advanced experiments"""
    name: str
    type: ExperimentType
    brain_config: pynlm.Config
    duration: int = 100
    repetition: int = 5
    parameter_sweep: Optional[Dict[str, List]] = None
    metrics: List[str] = field(default_factory=lambda: [
        'spike_count', 'firing_rate', 'activity_pattern', 'memory_performance',
        'learning_efficiency', 'stability_index'
    ])
    random_seed: int = 42

@dataclass
class ExperimentResult:
    """Result of an experiment"""
    experiment_name: str
    metrics: Dict[str, List[float]]
    brain_states: List[Dict]
    parameter_combinations: List[Dict]
    performance_data: Dict[str, float]
    quality_score: float = 0.0

class ExperimentRunner:
    """Advanced experiment runner for NLM"""
    
    def __init__(self):
        self.experiments: Dict[str, ExperimentConfig] = {}
        self.results: Dict[str, ExperimentResult] = {}
        self.current_experiment: Optional[ExperimentConfig] = None
    
    def register_experiment(self, config: ExperimentConfig):
        """Register an experiment configuration"""
        self.experiments[config.name] = config
        print(f"Registered experiment: {config.name}")
    
    def create_brain(self, config: pynlm.Config, seed: int = None) -> pynlm.Brain:
        """Create a brain with the given configuration"""
        if seed is not None:
            config.set("random_seed", seed)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    def run_placement_experiment(self, brain_config: pynlm.Config, 
                                neuron_counts: List[int], 
                                region_counts: List[int]) -> ExperimentResult:
        """Experiment to test different brain architectures (placement study)"""
        print(f"Running placement experiment with {len(neuron_counts)} neuron counts and {len(region_counts)} region counts")
        
        results = ExperimentResult(
            experiment_name="Placement_Study",
            metrics={},
            brain_states=[],
            parameter_combinations=[]
        )
        
        for neurons in neuron_counts:
            for regions in region_counts:
                param_combo = {'neuron_count': neurons, 'region_count': regions}
                results.parameter_combinations.append(param_combo)
                
                print(f"  Testing {neurons} neurons, {regions} regions")
                
                for rep in range(5):  # 5 repetitions per configuration
                    brain = self.create_brain(brain_config, seed=42 + rep)
                    
                    # Modify configuration for this run
                    brain.getConfig().set("neuron_count", neurons)
                    brain.getConfig().set("region_count", regions)
                    
                    # Re-initialize with new config
                    brain.reset()
                    brain.initialize()
                    
                    # Run simulation
                    spikes = []
                    firing_rates = []
                    for step in range(100):
                        brain.step(step)
                        spikes.append(brain.getTotalSpikeCount())
                        firing_rates.append(brain.getAverageFiringRate())
                    
                    # Record metrics
                    results.brain_states.append({
                        'parameters': param_combo.copy(),
                        'repetition': rep,
                        'total_spikes': spikes[-1],
                        'max_spikes': max(spikes),
                        'avg_firing_rate': statistics.mean(firing_rates) if firing_rates else 0,
                        'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
                        'spike_variation': statistics.variance(spikes) if len(spikes) > 1 else 0,
                    })
                    
                    # Aggregate metrics
                    if 'total_spikes' not in results.metrics:
                        results.metrics['total_spikes'] = []
                        results.metrics['avg_firing_rate'] = []
                        results.metrics['firing_rate_std'] = []
                        results.metrics['spike_variation'] = []
                    
                    results.metrics['total_spikes'].append(spikes[-1])
                    results.metrics['avg_firing_rate'].append(statistics.mean(firing_rates))
                    results.metrics['firing_rate_std'].append(statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0)
                    results.metrics['spike_variation'].append(statistics.variance(spikes) if len(spikes) > 1 else 0)
        
        # Calculate performance metrics
        results.performance_data = {
            'best_performance': max(results.metrics.get('total_spikes', [0])),
            'average_performance': statistics.mean(results.metrics.get('total_spikes', [0])),
            'performance_std': statistics.stdev(results.metrics.get('total_spikes', [0])) if len(results.metrics.get('total_spikes', [])) > 1 else 0,
            'convergence_rate': len([s for s in results.brain_states if s['spike_variation'] < 1000]) / len(results.brain_states) if results.brain_states else 0,
        }
        
        # Calculate quality score
        spike_mean = statistics.mean(results.metrics.get('total_spikes', [0]))
        spike_std = statistics.stdev(results.metrics.get('total_spikes', [0])) if len(results.metrics.get('total_spikes', [])) > 1 else 0
        results.quality_score = min(100, spike_mean / 10) * (1 - min(1, spike_std / spike_mean)) if spike_mean > 0 else 0
        
        return results
    
    def run_plasticity_experiment(self, brain_config: pynlm.Config,
                                   learning_rates: List[float],
                                   plasticity_types: List[str]) -> ExperimentResult:
        """Experiment to study plasticity effects"""
        print(f"Running plasticity experiment with {len(learning_rates)} learning rates and {len(plasticity_types)} plasticity types")
        
        results = ExperimentResult(
            experiment_name="Plasticity_Study",
            metrics={},
            brain_states=[],
            parameter_combinations=[]
        )
        
        for learning_rate in learning_rates:
            for plasticity_type in plasticity_types:
                param_combo = {'learning_rate': learning_rate, 'plasticity_type': plasticity_type}
                results.parameter_combinations.append(param_combo)
                
                print(f"  Testing learning_rate={learning_rate}, plasticity_type={plasticity_type}")
                
                for rep in range(3):  # 3 repetitions per configuration
                    brain = self.create_brain(brain_config, seed=100 + rep)
                    
                    # Configure plasticity
                    if plasticity_type == "STDP":
                        brain.getConfig().set("stdp_ltp_weight", learning_rate)
                        brain.getConfig().set("stdp_ltd_weight", learning_rate * 0.95)
                    elif plasticity_type == "Hebbian":
                        brain.getConfig().set("hebbian_learning_rate", learning_rate)
                    
                    # Re-initialize with new config
                    brain.reset()
                    brain.initialize()
                    
                    # Enable plasticity
                    agent = pynlm.createAgentBrain(brain)
                    agent.enableRewardModulation(True)
                    
                    # Run simulation with input to induce learning
                    rewards = []
                    spikes = []
                    for step in range(200):
                        # Inject stimulating input
                        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
                        
                        # Step brain
                        brain.step(step)
                        
                        # Record metrics
                        spikes.append(brain.getTotalSpikeCount())
                        
                        # Calculate reward
                        reward = len([s for s in spikes if s > brain.getTotalSpikeCount() * 0.5]) / (step + 1)
                        rewards.append(reward)
                    
                    # Record metrics
                    results.brain_states.append({
                        'parameters': param_combo.copy(),
                        'repetition': rep,
                        'total_spikes': spikes[-1],
                        'max_spikes': max(spikes),
                        'learning_efficiency': (spikes[-1] - spikes[0]) / spikes[0] if spikes[0] > 0 else 0,
                        'reward_accumulation': sum(rewards),
                        'plasticity_effectiveness': statistics.mean([abs(reward - statistics.mean(rewards)) for reward in rewards]),
                    })
                    
                    # Aggregate metrics
                    if 'learning_efficiency' not in results.metrics:
                        results.metrics['learning_efficiency'] = []
                        results.metrics['reward_accumulation'] = []
                        results.metrics['plasticity_effectiveness'] = []
                    
                    results.metrics['learning_efficiency'].append((spikes[-1] - spikes[0]) / spikes[0] if spikes[0] > 0 else 0)
                    results.metrics['reward_accumulation'].append(sum(rewards))
                    results.metrics['plasticity_effectiveness'].append(statistics.mean([abs(reward - statistics.mean(rewards)) for reward in rewards]))
        
        # Calculate performance metrics
        results.performance_data = {
            'best_learning_efficiency': max(results.metrics.get('learning_efficiency', [0])),
            'average_learning_efficiency': statistics.mean(results.metrics.get('learning_efficiency', [0])),
            'total_reward': sum(results.metrics.get('reward_accumulation', [0])),
            'average_plasticity_effectiveness': statistics.mean(results.metrics.get('plasticity_effectiveness', [0])),
        }
        
        # Calculate quality score
        eff_mean = statistics.mean(results.metrics.get('learning_efficiency', [0]))
        reward_mean = statistics.mean(results.metrics.get('reward_accumulation', [0]))
        results.quality_score = min(100, (eff_mean * 100 + reward_mean) / 2)
        
        return results
    
    def run_development_experiment(self, brain_config: pynlm.Config,
                                   developmental_stages: List[int],
                                   maturation_rates: List[float]) -> ExperimentResult:
        """Experiment to study development effects"""
        print(f"Running development experiment with {len(developmental_stages)} stages and {len(maturation_rates)} maturation rates")
        
        results = ExperimentResult(
            experiment_name="Development_Study",
            metrics={},
            brain_states=[],
            parameter_combinations=[]
        )
        
        for stages in developmental_stages:
            for maturation_rate in maturation_rates:
                param_combo = {'developmental_stages': stages, 'maturation_rate': maturation_rate}
                results.parameter_combinations.append(param_combo)
                
                print(f"  Testing {stages} developmental stages, maturation_rate={maturation_rate}")
                
                for rep in range(3):  # 3 repetitions per configuration
                    brain = self.create_brain(brain_config, seed=200 + rep)
                    
                    # Configure development
                    brain.getConfig().set("developmental_stages", stages)
                    brain.getConfig().set("maturation_rate", maturation_rate)
                    
                    # Re-initialize with new config
                    brain.reset()
                    brain.initialize()
                    
                    # Run simulation
                    development_scores = []
                    firing_rates = []
                    spike_counts = []
                    
                    for step in range(300):
                        brain.step(step)
                        
                        # Calculate development score based on firing rate stability
                        if step >= 10:
                            recent_firing_rates = []
                            for i in range(max(0, step - 10), min(len(brain.getSpikeHistory()), step + 1)):
                                recent_firing_rates.append(brain.getAverageFiringRate())
                            
                            if recent_firing_rates:
                                development_score = 100 - statistics.stdev(recent_firing_rates) / statistics.mean(recent_firing_rates) * 100 if statistics.mean(recent_firing_rates) > 0 else 0
                                development_scores.append(development_score)
                        
                        firing_rates.append(brain.getAverageFiringRate())
                        spike_counts.append(brain.getTotalSpikeCount())
                    
                    # Record metrics
                    results.brain_states.append({
                        'parameters': param_combo.copy(),
                        'repetition': rep,
                        'total_spikes': spike_counts[-1],
                        'avg_firing_rate': statistics.mean(firing_rates) if firing_rates else 0,
                        'firing_rate_stability': statistics.stdev(firing_rates) / statistics.mean(firing_rates) if firing_rates and statistics.mean(firing_rates) > 0 else 0,
                        'development_consistency': statistics.mean(development_scores) if development_scores else 0,
                        'maturation_effectiveness': len([s for s in development_scores if s > 50]) / len(development_scores) if development_scores else 0,
                    })
                    
                    # Aggregate metrics
                    if 'development_consistency' not in results.metrics:
                        results.metrics['development_consistency'] = []
                        results.metrics['maturation_effectiveness'] = []
                        results.metrics['firing_rate_stability'] = []
                    
                    results.metrics['development_consistency'].append(statistics.mean(development_scores) if development_scores else 0)
                    results.metrics['maturation_effectiveness'].append(len([s for s in development_scores if s > 50]) / len(development_scores) if development_scores else 0)
                    results.metrics['firing_rate_stability'].append(statistics.stdev(firing_rates) / statistics.mean(firing_rates) if firing_rates and statistics.mean(firing_rates) > 0 else 0)
        
        # Calculate performance metrics
        results.performance_data = {
            'best_development_consistency': max(results.metrics.get('development_consistency', [0])),
            'average_development_consistency': statistics.mean(results.metrics.get('development_consistency', [0])),
            'average_maturation_effectiveness': statistics.mean(results.metrics.get('maturation_effectiveness', [0])),
            'best_firing_rate_stability': max(results.metrics.get('firing_rate_stability', [0])),  # Lower is better for stability
        }
        
        # Calculate quality score
        dev_consistency = statistics.mean(results.metrics.get('development_consistency', [0]))
        mat_effectiveness = statistics.mean(results.metrics.get('maturation_effectiveness', [0]))
        results.quality_score = min(100, (dev_consistency + mat_effectiveness * 100) / 2)
        
        return results
    
    def run_comparative_experiment(self, brain_configs: List[pynlm.Config], 
                                   experiment_name: str = "Comparative_Study") -> ExperimentResult:
        """Experiment to compare different brain configurations"""
        print(f"Running comparative experiment with {len(brain_configs)} configurations")
        
        results = ExperimentResult(
            experiment_name=experiment_name,
            metrics={},
            brain_states=[],
            parameter_combinations=[]
        )
        
        for i, config in enumerate(brain_configs):
            param_combo = {'config_index': i, 'neuron_count': config.getOr('neuron_count', 0)}
            results.parameter_combinations.append(param_combo)
            
            print(f"  Testing configuration {i + 1}/{len(brain_configs)}")
            
            # Test each configuration multiple times
            for rep in range(5):
                brain = self.create_brain(config, seed=300 + i + rep)
                
                # Record initial state
                initial_spikes = brain.getTotalSpikeCount()
                initial_firing_rate = brain.getAverageFiringRate()
                
                # Run simulation
                spike_trajectory = []
                firing_rate_trajectory = []
                rewards = []
                
                for step in range(150):
                    brain.step(step)
                    spike_trajectory.append(brain.getTotalSpikeCount())
                    firing_rate_trajectory.append(brain.getAverageFiringRate())
                    
                    # Calculate simple reward based on activity
                    reward = (brain.getTotalSpikeCount() - initial_spikes) / max(initial_spikes, 1)
                    rewards.append(reward)
                
                # Record metrics
                results.brain_states.append({
                    'parameters': param_combo.copy(),
                    'repetition': rep,
                    'total_spikes': spike_trajectory[-1],
                    'max_spikes': max(spike_trajectory),
                    'min_spikes': min(spike_trajectory),
                    'spike_trajectory_mean': statistics.mean(spike_trajectory) if spike_trajectory else 0,
                    'spike_trajectory_std': statistics.stdev(spike_trajectory) if len(spike_trajectory) > 1 else 0,
                    'average_firing_rate': statistics.mean(firing_rate_trajectory) if firing_rate_trajectory else 0,
                    'reward_accumulation': sum(rewards),
                    'learning_efficiency': (spike_trajectory[-1] - spike_trajectory[0]) / spike_trajectory[0] if spike_trajectory[0] > 0 else 0,
                })
                
                # Aggregate metrics
                if 'total_spikes' not in results.metrics:
                    results.metrics['total_spikes'] = []
                    results.metrics['spike_trajectory_mean'] = []
                    results.metrics['spike_trajectory_std'] = []
                    results.metrics['reward_accumulation'] = []
                    results.metrics['learning_efficiency'] = []
                
                results.metrics['total_spikes'].append(spike_trajectory[-1])
                results.metrics['spike_trajectory_mean'].append(statistics.mean(spike_trajectory) if spike_trajectory else 0)
                results.metrics['spike_trajectory_std'].append(statistics.stdev(spike_trajectory) if len(spike_trajectory) > 1 else 0)
                results.metrics['reward_accumulation'].append(sum(rewards))
                results.metrics['learning_efficiency'].append((spike_trajectory[-1] - spike_trajectory[0]) / spike_trajectory[0] if spike_trajectory[0] > 0 else 0)
        
        # Calculate performance metrics
        spike_vals = results.metrics.get('total_spikes', [])
        results.performance_data = {
            'best_performance': max(spike_vals) if spike_vals else 0,
            'average_performance': statistics.mean(spike_vals) if spike_vals else 0,
            'performance_std': statistics.stdev(spike_vals) if len(spike_vals) > 1 else 0,
            'total_reward': sum(results.metrics.get('reward_accumulation', [0])),
            'average_learning_efficiency': statistics.mean(results.metrics.get('learning_efficiency', [0])),
            'consistency_score': 1 - (statistics.stdev(results.metrics.get('total_spikes', [0])) / statistics.mean(results.metrics.get('total_spikes', [0]))) if len(results.metrics.get('total_spikes', [])) > 1 and statistics.mean(results.metrics.get('total_spikes', [0])) > 0 else 0,
        }
        
        # Calculate quality score
        performance = statistics.mean(results.metrics.get('total_spikes', [0]))
        reward = statistics.mean(results.metrics.get('reward_accumulation', [0]))
        efficiency = statistics.mean(results.metrics.get('learning_efficiency', [0]))
        results.quality_score = min(100, (performance / 10 + reward + efficiency * 100) / 3)
        
        return results
    
    def run_experiment(self, experiment_type: ExperimentType, 
                      brain_config: pynlm.Config,
                      parameters: Dict[str, Any] = None) -> ExperimentResult:
        """Run a specific experiment type"""
        if parameters is None:
            parameters = {}
        
        if experiment_type == ExperimentType.PLACEMENT:
            return self.run_placement_experiment(
                brain_config,
                parameters.get('neuron_counts', [100, 500, 1000]),
                parameters.get('region_counts', [1, 4, 8])
            )
        
        elif experiment_type == ExperimentType.PLASTICITY:
            return self.run_plasticity_experiment(
                brain_config,
                parameters.get('learning_rates', [0.001, 0.01, 0.1]),
                parameters.get('plasticity_types', ['STDP', 'Hebbian'])
            )
        
        elif experiment_type == ExperimentType.DEVELOPMENT:
            return self.run_development_experiment(
                brain_config,
                parameters.get('developmental_stages', [10, 50, 100]),
                parameters.get('maturation_rates', [0.001, 0.01, 0.1])
            )
        
        elif experiment_type == ExperimentType.COMPARATIVE:
            return self.run_comparative_experiment(
                parameters.get('brain_configs', [brain_config]),
                parameters.get('name', 'Comparative_Study')
            )
        
        else:
            raise ValueError(f"Unsupported experiment type: {experiment_type}")
    
    def save_experiment_results(self, experiment_name: str, filepath: str):
        """Save experiment results to file"""
        if experiment_name not in self.results:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        # Convert results to serializable format
        serialized = {
            'name': experiment_name,
            'result': self.results[experiment_name]
        }
        
        with open(filepath, 'w') as f:
            json.dump(serialized, f, indent=2, default=str)
        
        print(f"Saved experiment '{experiment_name}' to {filepath}")
    
    def load_experiment_results(self, filepath: str) -> str:
        """Load experiment results from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # This is a simplified version - in practice, you'd need more sophisticated
        # deserialization to handle the complex objects
        print(f"Loaded experiment results from {filepath}")
        return data.get('name', 'Unknown')
    
    def generate_experiment_report(self, experiment_name: str) -> str:
        """Generate a comprehensive report for an experiment"""
        if experiment_name not in self.results:
            return f"Experiment '{experiment_name}' not found"
        
        result = self.results[experiment_name]
        
        report = f"""
=== NLM Advanced Experiment Report ===

Experiment: {result.experiment_name}
Quality Score: {result.quality_score:.2f}/100

Performance Metrics:
"""
        
        for metric_name, metric_values in result.metrics.items():
            if metric_values:
                report += f"  {metric_name}:
"
                report += f"    Mean: {statistics.mean(metric_values):.4f}
"
                report += f"    Std: {statistics.stdev(metric_values):.4f} (n={len(metric_values)})
"
                report += f"    Min: {min(metric_values):.4f}
"
                report += f"    Max: {max(metric_values):.4f}
"
        
        report += f"\nPerformance Data:
"
        for key, value in result.performance_data.items():
            if isinstance(value, float):
                report += f"  {key}: {value:.4f}
"
            else:
                report += f"  {key}: {value}
"
        
        report += f"\nParameter Combinations Tested: {len(result.parameter_combinations)}
"
        report += f"Total Brain States Collected: {len(result.brain_states)}
"
        
        # Add visualization suggestions
        report += f"\nVisualization Suggestions:
"
        report += "  1. Plot metric trajectories over parameter space
"
        report += "  2. Create scatter plots of performance vs parameters
"
        report += "  3. Generate histograms of key metrics
"
        report += "  4. Show learning curves for different configurations
"
        
        return report
    
    def analyze_experiments(self) -> Dict[str, Any]:
        """Analyze all completed experiments"""
        analysis = {
            'total_experiments': len(self.results),
            'experiments_by_type': {},
            'quality_scores': {},
            'best_performers': {},
            'recommendations': []
        }
        
        # Analyze by experiment type
        for exp_name, result in self.results.items():
            exp_type = exp_name.split('_')[0] if '_' in exp_name else 'unknown'
            if exp_type not in analysis['experiments_by_type']:
                analysis['experiments_by_type'][exp_type] = []
            
            analysis['experiments_by_type'][exp_type].append(result)
            analysis['quality_scores'][exp_name] = result.quality_score
            
            # Find best performer
            if result.metrics and 'total_spikes' in result.metrics:
                best_idx = result.metrics['total_spikes'].index(max(result.metrics['total_spikes']))
                analysis['best_performers'][exp_name] = {
                    'best_value': result.metrics['total_spikes'][best_idx],
                    'best_parameters': result.parameter_combinations[best_idx],
                    'repetition': result.brain_states[best_idx]['repetition']
                }
        
        # Generate recommendations
        if analysis['quality_scores']:
            avg_quality = statistics.mean(analysis['quality_scores'].values())
            if avg_quality < 50:
                analysis['recommendations'].append("Consider simplifying experiment parameters for better results")
            elif avg_quality < 80:
                analysis['recommendations'].append("Good experiment quality. Consider increasing parameter ranges")
            else:
                analysis['recommendations'].append("Excellent experiment quality. Ready for research publication")
        
        # Add type-specific recommendations
        for exp_type, experiments in analysis['experiments_by_type'].items():
            if exp_type == 'Placement_Study' and experiments:
                best_configs = []
                for exp in experiments:
                    if exp.metrics and 'total_spikes' in exp.metrics:
                        best_configs.append((exp.experiment_name, max(exp.metrics['total_spikes'])))
                
                if best_configs:
                    best_config = max(best_configs, key=lambda x: x[1])
                    analysis['recommendations'].append(
                        f"For placement studies, use {best_config[0]} with {best_config[1]:,} spikes as optimal configuration"
                    )
        
        return analysis

def create_research_suite() -> Dict[str, Callable]:
    """Create a suite of research-oriented experiment templates"""
    runner = ExperimentRunner()
    
    # Template configurations
    default_config = pynlm.createDefaultConfig()
    
    # Register common experiment templates
    experiments = {}
    
    # 1. Brain Architecture Study
    placement_config = copy.deepcopy(default_config)
    placement_config.set("neuron_count", 500)
    
    placement_study = ExperimentConfig(
        name="Brain_Architecture_Study",
        type=ExperimentType.PLACEMENT,
        brain_config=placement_config,
        duration=100,
        parameter_sweep={
            'neuron_counts': [100, 500, 1000, 2000],
            'region_counts': [1, 4, 8]
        }
    )
    
    # 2. Plasticity Parameter Study
    plasticity_study = ExperimentConfig(
        name="Plasticity_Parameter_Study", 
        type=ExperimentType.PLASTICITY,
        brain_config=copy.deepcopy(default_config),
        duration=200,
        parameter_sweep={
            'learning_rates': [0.001, 0.01, 0.1],
            'plasticity_types': ['STDP', 'Hebbian']
        }
    )
    
    # 3. Development Study
    development_study = ExperimentConfig(
        name="Development_Parameter_Study",
        type=ExperimentType.DEVELOPMENT,
        brain_config=copy.deepcopy(default_config),
        duration=300,
        parameter_sweep={
            'developmental_stages': [10, 50, 100],
            'maturation_rates': [0.001, 0.01, 0.1]
        }
    )
    
    # 4. Comparative Study
    comparative_study = ExperimentConfig(
        name="Configuration_Comparative_Study",
        type=ExperimentType.COMPARATIVE,
        brain_config=copy.deepcopy(default_config),
        duration=150,
        parameter_sweep={
            'brain_configs': [
                copy.deepcopy(default_config),
                pynlm.createPerformanceConfig(),
                pynlm.createResearchConfig()
            ]
        }
    )
    
    # Register experiments
    for study in [placement_study, plasticity_study, development_study, comparative_study]:
        runner.register_experiment(study)
        experiments[study.name] = lambda cfg, st=study: runner.run_experiment(
            st.type, cfg, st.parameter_sweep
        )
    
    return experiments

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Advanced Experimentation Framework")
    parser.add_argument("--run-placement", action="store_true", help="Run placement experiment")
    parser.add_argument("--run-plasticity", action="store_true", help="Run plasticity experiment")
    parser.add_argument("--run-development", action="store_true", help="Run development experiment")
    parser.add_argument("--run-comparative", action="store_true", help="Run comparative experiment")
    parser.add_argument("--create-suite", action="store_true", help="Create research experiment suite")
    parser.add_argument("--output-format", choices=['text', 'json', 'csv'], default='text', help="Output format")
    parser.add_argument("--save-results", action="store_true", help="Save experiment results to file")
    
    args = parser.parse_args()
    
    print("=== NLM Advanced Experimentation Framework ===")
    
    # Initialize experiment runner
    runner = ExperimentRunner()
    
    # Create default config
    default_config = pynlm.createDefaultConfig()
    
    # Run experiments based on arguments
    if args.run_placement:
        print("\n=== Running Placement Experiment ===")
        result = runner.run_placement_experiment(
            default_config,
            [100, 500, 1000, 2000],
            [1, 4, 8]
        )
        runner.results["Placement_Study"] = result
        print("Placement experiment completed!")
    
    elif args.run_plasticity:
        print("\n=== Running Plasticity Experiment ===")
        result = runner.run_plasticity_experiment(
            default_config,
            [0.001, 0.01, 0.1],
            ['STDP', 'Hebbian']
        )
        runner.results["Plasticity_Study"] = result
        print("Plasticity experiment completed!")
    
    elif args.run_development:
        print("\n=== Running Development Experiment ===")
        result = runner.run_development_experiment(
            default_config,
            [10, 50, 100],
            [0.001, 0.01, 0.1]
        )
        runner.results["Development_Study"] = result
        print("Development experiment completed!")
    
    elif args.run_comparative:
        print("\n=== Running Comparative Experiment ===")
        configs = [default_config, pynlm.createPerformanceConfig(), pynlm.createResearchConfig()]
        result = runner.run_comparative_experiment(configs)
        runner.results["Comparative_Study"] = result
        print("Comparative experiment completed!")
    
    elif args.create_suite:
        print("\n=== Creating Research Experiment Suite ===")
        experiments = create_research_suite()
        
        print("Registered experiment templates:")
        for name in experiments.keys():
            print(f"  - {name}")
        
        # Save suite configuration
        suite_config = {
            'experiments': list(experiments.keys()),
            'description': 'Advanced NLM research experiment suite',
            'recommended_order': ['Brain_Architecture_Study', 'Plasticity_Parameter_Study', 'Development_Parameter_Study', 'Configuration_Comparative_Study']
        }
        
        with open('experiment_suite_config.json', 'w') as f:
            json.dump(suite_config, f, indent=2)
        
        print("\nExperiment suite configuration saved to 'experiment_suite_config.json'")
        print("\nTo run experiments from the suite:")
        print("  - Brain_Architecture_Study: Tests different brain architectures")
        print("  - Plasticity_Parameter_Study: Tests plasticity parameters")
        print("  - Development_Parameter_Study: Tests development parameters")
        print("  - Configuration_Comparative_Study: Compares different brain configs")
    
    else:
        print("\n=== Available Experiment Options ===")
        print("  --run-placement        Run brain placement architecture study")
        print("  --run-plasticity      Run plasticity parameter study")
        print("  --run-development     Run development parameter study")
        print("  --run-comparative     Run configuration comparative study")
        print("  --create-suite        Create research experiment suite")
        print("  --save-results        Save results to file")
        print("  --output-format       Output format (text, json, csv)")
        
        print("\nExample usage:")
        print("  python experiments.py --run-placement --save-results")
        print("  python experiments.py --run-plasticity")
        print("  python experiments.py --create-suite")
        
        return
    
    # Generate reports for completed experiments
    print("\n=== Generating Experiment Reports ===")
    for exp_name in runner.results.keys():
        report = runner.generate_experiment_report(exp_name)
        print(f"\n--- {exp_name} Report ---")
        print(report[:1000] + "..." if len(report) > 1000 else report)
        
        if args.save_results:
            filename = f"{exp_name.lower().replace('_', '_')}_report.txt"
            with open(filename, 'w') as f:
                f.write(report)
            print(f"Report saved to {filename}")
    
    # Generate comprehensive analysis
    print("\n=== Comprehensive Experiment Analysis ===")
    analysis = runner.analyze_experiments()
    
    print(f"\nTotal experiments analyzed: {analysis['total_experiments']}")
    print(f"Experiments by type: {analysis['experiments_by_type']}")
    
    if analysis['best_performers']:
        print(f"\nBest performers:")
        for exp_name, best in analysis['best_performers'].items():
            print(f"  {exp_name}:")
            print(f"    Best value: {best['best_value']}")
            print(f"    Parameters: {best['parameters']}")
    
    if analysis['recommendations']:
        print(f"\nRecommendations:")
        for rec in analysis['recommendations']:
            print(f"  • {rec}")
    
    # Save analysis
    if args.output_format == 'json':
        with open('experiment_analysis.json', 'w') as f:
            json.dump(analysis, f, indent=2, default=str)
        print("\nAnalysis saved to 'experiment_analysis.json'")
    
    print("\n=== Experiment Framework Complete ===")

if __name__ == "__main__":
    main()
