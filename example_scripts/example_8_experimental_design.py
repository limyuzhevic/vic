#!/usr/bin/env python3
"""
Example 8: Experimental Design

This example demonstrates advanced experimental design techniques for
NLM simulations. It focuses on creating comprehensive experimental
frameworks, statistical analysis, and reproducible research practices.

Key features demonstrated:
- Experimental design and planning
- Statistical power analysis
- Reproducible research workflows
- Parameter space exploration
- Experimental validation and verification
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import json
import os
import shutil
from datetime import datetime
from typing import Dict, List, Optional, Callable
import itertools

class ExperimentalDesign:
    """
    Advanced experimental design system for NLM simulations.
    
    This class provides tools for:
    - Experimental planning and design
    - Parameter space exploration
    - Statistical power analysis
    - Reproducible research workflows
    - Experimental validation and verification
    """
    
    def __init__(self, project_name):
        self.project_name = project_name
        self.project_root = f"experimental_data/{project_name}"
        self.experiments = []
        self.design_matrix = None
        self.results_database = {}
        self.design_parameters = {}
        
        # Create project directory
        os.makedirs(self.project_root, exist_ok=True)
        os.makedirs(f"{self.project_root}/configs", exist_ok=True)
        os.makedirs(f"{self.project_root}/results", exist_ok=True)
        os.makedirs(f"{self.project_root}/logs", exist_ok=True)
        os.makedirs(f"{self.project_root}/visualizations", exist_ok=True)
        
        self._setup_logging()
    
    def _setup_logging(self):
        """Setup logging for experimental design."""
        import logging
        
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(f"{self.project_root}/logs/experimental_design.log"),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger(__name__)
    
    def create_experiment_matrix(self, param_ranges, n_combinations=None):
        """
        Create an experimental design matrix.
        
        Args:
            param_ranges: Dictionary of parameter ranges
            n_combinations: Number of combinations to generate (None for full factorial)
            
        Returns:
            dict: Experimental design matrix
        """
        self.logger.info(f"Creating experimental design matrix for {self.project_name}")
        
        # Convert ranges to lists for easy iteration
        param_lists = {}
        for param, ranges in param_ranges.items():
            if isinstance(ranges, tuple) and len(ranges) == 2:
                # Linear spacing for continuous parameters
                param_lists[param] = np.linspace(ranges[0], ranges[1], 5)
            elif isinstance(ranges, list):
                param_lists[param] = ranges
            else:
                param_lists[param] = [ranges]
        
        # Generate all combinations
        param_names = list(param_lists.keys())
        all_combinations = list(itertools.product(*[param_lists[param] for param in param_names]))
        
        # Sample if too many combinations
        if n_combinations is not None and len(all_combinations) > n_combinations:
            # Use random sampling
            np.random.seed(42)
            sampled_indices = np.random.choice(len(all_combinations), n_combinations, replace=False)
            all_combinations = [all_combinations[i] for i in sampled_indices]
            self.logger.info(f"Sampled {len(all_combinations)} combinations from {len(param_lists)} parameters")
        else:
            self.logger.info(f"Generated full factorial design with {len(all_combinations)} combinations")
        
        # Convert combinations to list of dictionaries
        design_matrix = []
        for combination in all_combinations:
            params = {param: value for param, value in zip(param_names, combination)}
            design_matrix.append(params)
        
        self.design_matrix = design_matrix
        self.logger.info(f"Design matrix created with {len(design_matrix)} experiments")
        
        return design_matrix
    
    def save_design_matrix(self, filename=None):
        """Save experimental design matrix to file."""
        if filename is None:
            filename = f"{self.project_root}/configs/design_matrix.json"
        
        with open(filename, 'w') as f:
            json.dump(self.design_matrix, f, indent=2, default=str)
        
        self.logger.info(f"Design matrix saved to {filename}")
    
    def load_design_matrix(self, filename=None):
        """Load experimental design matrix from file."""
        if filename is None:
            filename = f"{self.project_root}/configs/design_matrix.json"
        
        try:
            with open(filename, 'r') as f:
                self.design_matrix = json.load(f)
            
            self.logger.info(f"Design matrix loaded from {filename}")
            return True
        except Exception as e:
            self.logger.error(f"Error loading design matrix: {e}")
            return False
    
    def create_experiment(self, name, description, config_template, variations=None):
        """
        Create a single experiment with variations.
        
        Args:
            name: Experiment name
            description: Experiment description
            config_template: Base configuration template
            variations: Dictionary of parameters to vary
            
        Returns:
            dict: Experiment configuration
        """
        experiment = {
            'name': name,
            'description': description,
            'config_template': config_template,
            'variations': variations or {},
            'timestamp': datetime.now().isoformat(),
            'status': 'pending'
        }
        
        self.experiments.append(experiment)
        self.logger.info(f"Created experiment: {name}")
        
        return experiment
    
    def run_experimental_batch(self, experiment_configs, max_workers=None):
        """
        Run a batch of experiments.
        
        Args:
            experiment_configs: List of experiment configurations
            max_workers: Maximum number of parallel workers
            
        Returns:
            dict: Batch results
        """
        self.logger.info(f"Running experimental batch with {len(experiment_configs)} experiments")
        
        batch_results = {
            'experiment_configs': experiment_configs,
            'results': {},
            'summary': {},
            'timestamp': datetime.now().isoformat()
        }
        
        # Run each experiment
        for i, config in enumerate(experiment_configs):
            experiment_name = config['name']
            self.logger.info(f"Running experiment {i+1}/{len(experiment_configs)}: {experiment_name}")
            
            try:
                # Create configuration
                nlm_config = self._create_nlm_config(config['config_template'], config.get('variations', {}))
                
                # Create components
                brain = pynlm.createBrain(nlm_config)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(width=15, height=15, visionWidth=6, visionWidth=6)
                world.reset()
                world.setAgentStart(7.5, 7.5)
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                # Configure experiment
                experiment_config = ExperimentConfig(
                    name=config['name'],
                    description=config['description'],
                    max_steps=config.get('max_steps', 200),
                    timestep=config.get('timestep', 0.1),
                    save_checkpoints=config.get('save_checkpoints', False),
                    verbose=False
                )
                
                # Create experiment runner
                runner = NLMExperimentRunner(brain, world, agent)
                
                # Run experiment
                result = runner.run_experiment(experiment_config)
                
                # Store results
                batch_results['results'][experiment_name] = {
                    'status': 'success' if result.success else 'failed',
                    'result': result,
                    'runner': runner,
                    'config': config,
                    'timestamp': datetime.now().isoformat()
                }
                
                self.logger.info(f"Experiment {experiment_name} completed: {'✓' if result.success else '✗'}")
                
                # Save experiment data
                self._save_experiment_data(experiment_name, batch_results['results'][experiment_name])
                
            except Exception as e:
                error_msg = f"Experiment {experiment_name} failed: {e}"
                self.logger.error(error_msg)
                
                batch_results['results'][experiment_name] = {
                    'status': 'error',
                    'error': str(e),
                    'timestamp': datetime.now().isoformat()
                }
        
        # Calculate batch summary
        batch_summary = self._calculate_batch_summary(batch_results['results'])
        batch_results['summary'] = batch_summary
        
        # Save batch results
        self._save_batch_results(batch_results)
        
        return batch_results
    
    def _create_nlm_config(self, template, variations):
        """Create NLM configuration from template with variations."""
        config = pynlm.createDefaultConfig()
        
        # Apply template parameters
        for key, value in template.items():
            config.set(key, value)
        
        # Apply variations
        for key, value in variations.items():
            config.set(key, value)
        
        return config
    
    def _save_experiment_data(self, experiment_name, experiment_data):
        """Save experiment data to disk."""
        experiment_dir = f"{self.project_root}/results/{experiment_name}"
        os.makedirs(experiment_dir, exist_ok=True)
        
        # Save experiment configuration
        with open(f"{experiment_dir}/config.json", 'w') as f:
            json.dump(experiment_data['config'], f, indent=2)
        
        # Save experiment results
        if 'result' in experiment_data:
            result = experiment_data['result']
            with open(f"{experiment_dir}/result.json", 'w') as f:
                # Convert numpy types to Python types
                result_dict = {
                    'name': result.name,
                    'duration': result.duration,
                    'steps_completed': result.steps_completed,
                    'final_firing_rate': result.final_firing_rate,
                    'total_spikes': result.total_spikes,
                    'success': result.success,
                    'error': result.error,
                    'learning_metrics': result.learning_metrics,
                    'checkpoint_files': result.checkpoint_files
                }
                json.dump(result_dict, f, indent=2, default=str)
        
        # Save runner data if available
        if 'runner' in experiment_data:
            runner = experiment_data['runner']
            if hasattr(runner, 'monitoring_data'):
                with open(f"{experiment_dir}/monitoring_data.json", 'w') as f:
                    json.dump(runner.monitoring_data, f, indent=2, default=str)
    
    def _calculate_batch_summary(self, results):
        """Calculate summary statistics for batch results."""
        summary = {
            'total_experiments': len(results),
            'successful_experiments': sum(1 for r in results.values() if r['status'] == 'success'),
            'failed_experiments': sum(1 for r in results.values() if r['status'] == 'failed'),
            'error_experiments': sum(1 for r in results.values() if r['status'] == 'error'),
            'success_rate': 0
        }
        
        if summary['total_experiments'] > 0:
            summary['success_rate'] = summary['successful_experiments'] / summary['total_experiments']
        
        # Calculate performance metrics from successful experiments
        if summary['successful_experiments'] > 0:
            successful_results = [r['result'] for r in results.values() 
                                if r['status'] == 'success' and 'result' in r]
            
            if successful_results:
                durations = [r.duration for r in successful_results]
                firing_rates = [r.final_firing_rate for r in successful_results]
                spike_counts = [r.total_spikes for r in successful_results]
                
                summary['performance'] = {
                    'mean_duration': np.mean(durations),
                    'std_duration': np.std(durations),
                    'mean_firing_rate': np.mean(firing_rates),
                    'std_firing_rate': np.std(firing_rates),
                    'mean_spike_count': np.mean(spike_counts),
                    'std_spike_count': np.std(spike_counts)
                }
        
        return summary
    
    def _save_batch_results(self, batch_results):
        """Save batch results to file."""
        with open(f"{self.project_root}/results/batch_results.json", 'w') as f:
            json.dump(batch_results, f, indent=2, default=str)
        
        self.logger.info(f"Batch results saved to {self.project_root}/results/batch_results.json")
    
    def power_analysis(self, effect_size, alpha=0.05, power=0.8, std_dev=None):
        """
        Perform statistical power analysis.
        
        Args:
            effect_size: Expected effect size (Cohen's d)
            alpha: Significance level
            power: Desired statistical power
            std_dev: Standard deviation (if None, estimated from effect_size)
            
        Returns:
            dict: Power analysis results
        """
        self.logger.info(f"Performing power analysis: effect_size={effect_size}, alpha={alpha}, power={power}")
        
        if std_dev is None:
            std_dev = effect_size  # Cohen's d assumption
        
        # Calculate required sample size
        from scipy import stats
        
        # Using Cohen's d power calculation
        df = 2  # degrees of freedom for two-sample t-test
        
        # Critical t-value
        t_critical = stats.t.ppf(1 - alpha/2, df)
        
        # Non-centrality parameter
        ncp = effect_size * np.sqrt(df/2)
        
        # Power calculation (simplified)
        power_calc = 1 - stats.nct.cdf(t_critical, df, ncp)
        
        # Required sample size for desired power
        if power_calc < power:
            # Estimate required sample size
            required_n = int(np.ceil(2 * ((t_critical + stats.t.ppf(power, df)) / effect_size) ** 2))
        else:
            required_n = 2 * df
        
        power_analysis = {
            'effect_size': effect_size,
            'alpha': alpha,
            'power': power,
            'std_dev': std_dev,
            'calculated_power': power_calc,
            'required_sample_size': required_n,
            'recommendation': 'Increase sample size' if power_calc < power else 'Sample size adequate'
        }
        
        self.logger.info(f"Power analysis: calculated power = {power_calc:.3f}, required sample size = {required_n}")
        
        return power_analysis
    
    def reproducible_research_setup(self):
        """
        Setup reproducible research infrastructure.
        
        Returns:
            dict: Reproducible research setup information
        """
        self.logger.info("Setting up reproducible research infrastructure")
        
        # Create necessary directories
        directories = [
            'configs',
            'results',
            'logs',
            'visualizations',
            'scripts',
            'notebooks',
            'data'
        ]
        
        for directory in directories:
            os.makedirs(f"{self.project_root}/{directory}", exist_ok=True)
        
        # Create README
        readme_content = f"""# {self.project_name}

## Project Overview

This project implements advanced NLM (Neural Learning Machine) experimental research.

## Experimental Design

This project uses a systematic experimental design approach to investigate neural network behavior and learning patterns.

## Configuration

All experimental configurations are stored in the `configs/` directory.

## Results

Experimental results are stored in the `results/` directory, organized by experiment.

## Analysis

Data analysis and visualization code is available in the `scripts/` directory.

## Reproduction

To reproduce experiments:

1. Ensure all dependencies are installed
2. Navigate to the project directory
3. Run the experimental design script
4. Results will be saved to the `results/` directory

## License

MIT License
"""
        
        with open(f"{self.project_root}/README.md", 'w') as f:
            f.write(readme_content)
        
        # Create requirements file
        requirements = [
            'pynlm',
            'numpy',
            'pandas',
            'matplotlib',
            'scikit-learn',
            'scipy',
            'statsmodels',
            'joblib',
            'tqdm'
        ]
        
        with open(f"{self.project_root}/requirements.txt", 'w') as f:
            f.write('\n'.join(requirements))
        
        self.logger.info("Reproducible research infrastructure setup complete")
        
        return {
            'project_root': self.project_root,
            'directories_created': directories,
            'readme_created': True,
            'requirements_created': True
        }
    
    def generate_experiment_report(self, experiment_name):
        """
        Generate a comprehensive report for a specific experiment.
        
        Args:
            experiment_name: Name of the experiment
            
        Returns:
            dict: Report data
        """
        if experiment_name not in self.results_database:
            self.logger.warning(f"Experiment {experiment_name} not found in results database")
            return None
        
        experiment_data = self.results_database[experiment_name]
        
        # Create report
        report = {
            'experiment_name': experiment_name,
            'timestamp': datetime.now().isoformat(),
            'configuration': experiment_data.get('config', {}),
            'results': experiment_data.get('result', {}),
            'analysis': self._analyze_experiment_results(experiment_data),
            'recommendations': self._generate_recommendations(experiment_data)
        }
        
        # Save report
        report_filename = f"{self.project_root}/results/{experiment_name}/report.json"
        os.makedirs(os.path.dirname(report_filename), exist_ok=True)
        
        with open(report_filename, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        self.logger.info(f"Report generated for experiment {experiment_name}")
        
        return report
    
    def _analyze_experiment_results(self, experiment_data):
        """Analyze experiment results."""
        if 'result' not in experiment_data:
            return {'error': 'No results available for analysis'}
        
        result = experiment_data['result']
        
        analysis = {
            'success': result.success,
            'performance': self._calculate_performance_metrics(result),
            'learning_dynamics': self._analyze_learning_dynamics(result),
            'statistical_significance': self._check_statistical_significance(result)
        }
        
        return analysis
    
    def _calculate_performance_metrics(self, result):
        """Calculate performance metrics."""
        if not result.success:
            return {'error': 'Experiment failed'}
        
        metrics = {
            'duration_efficiency': result.duration / result.steps_completed if result.steps_completed > 0 else 0,
            'firing_rate': result.final_firing_rate,
            'spike_activity': result.total_spikes,
            'learning_rate': self._estimate_learning_rate(result),
            'stability': self._calculate_stability(result)
        }
        
        return metrics
    
    def _analyze_learning_dynamics(self, result):
        """Analyze learning dynamics."""
        if not result.success or not result.learning_metrics:
            return {'error': 'No learning metrics available'}
        
        learning_metrics = result.learning_metrics
        
        analysis = {
            'trend': learning_metrics.get('firing_rate_trend', 0),
            'stability': learning_metrics.get('stability', 0),
            'exploration': learning_metrics.get('exploration_index', 0),
            'coherence': learning_metrics.get('coherence', 0),
            'peak_activity': learning_metrics.get('peak_activity', 0),
            'average_activity': learning_metrics.get('average_activity', 0)
        }
        
        return analysis
    
    def _check_statistical_significance(self, result):
        """Check statistical significance of results."""
        if not result.success:
            return {'error': 'Experiment failed'}
        
        # This is a simplified statistical significance test
        # In practice, you'd use more sophisticated statistical tests
        significance = {
            'learning_significant': abs(result.learning_metrics.get('firing_rate_trend', 0)) > 0.1,
            'stability_significant': result.learning_metrics.get('stability', 0) > 0.5,
            'exploration_significant': result.learning_metrics.get('exploration_index', 0) > 0.3
        }
        
        return significance
    
    def _estimate_learning_rate(self, result):
        """Estimate learning rate from experiment results."""
        if not result.learning_metrics:
            return 0.0
        
        trend = result.learning_metrics.get('firing_rate_trend', 0)
        return max(0, trend)
    
    def _calculate_stability(self, result):
        """Calculate stability from experiment results."""
        if not result.learning_metrics:
            return 0.0
        
        return result.learning_metrics.get('stability', 0)
    
    def _generate_recommendations(self, experiment_data):
        """Generate recommendations based on experiment results."""
        recommendations = []
        
        if 'result' not in experiment_data or not experiment_data['result'].success:
            recommendations.append('Experiment failed. Review experimental conditions.')
        else:
            result = experiment_data['result']
            
            if result.final_firing_rate < 1.0:
                recommendations.append('Low firing rates detected. Consider increasing neuron count or adjusting parameters.')
            
            if result.learning_metrics:
                trend = result.learning_metrics.get('firing_rate_trend', 0)
                if abs(trend) < 0.05:
                    recommendations.append('Little learning detected. Increase plasticity parameters or extend experiment duration.')
                elif trend > 0.1:
                    recommendations.append('Positive learning trend observed. Consider similar configurations for future experiments.')
                else:
                    recommendations.append('Negative trend observed. Investigate potential issues with experimental setup.')
            
            if result.learning_metrics and result.learning_metrics.get('stability', 0) < 0.3:
                recommendations.append('Low stability detected. Consider reducing exploration or increasing regularization.')
        
        return recommendations

# Global experimental design manager
experimental_design_manager = None

def get_experimental_design_manager(project_name):
    """Get or create experimental design manager."""
    global experimental_design_manager
    if experimental_design_manager is None:
        experimental_design_manager = ExperimentalDesign(project_name)
    return experimental_design_manager

def create_reproducible_research_project(project_name):
    """Create a reproducible research project."""
    experimental_design_manager = get_experimental_design_manager(project_name)
    return experimental_design_manager.reproducible_research_setup()

def run_experiment_batch(project_name, experiment_configs):
    """Run a batch of experiments in a reproducible research project."""
    experimental_design_manager = get_experimental_design_manager(project_name)
    return experimental_design_manager.run_experimental_batch(experiment_configs)

def generate_research_report(project_name, experiment_name):
    """Generate a research report for an experiment."""
    experimental_design_manager = get_experimental_design_manager(project_name)
    return experimental_design_manager.generate_experiment_report(experiment_name)

def example_experimental_design():
    """Demonstrate experimental design workflow."""
    print("=== Example 8: Experimental Design ===\n")
    
    # Create experimental design project
    print("1. Creating experimental design project...")
    project_name = "NLM_Experimental_Research"
    experimental_design = ExperimentalDesign(project_name)
    
    # Setup reproducible research infrastructure
    print("\n2. Setting up reproducible research infrastructure...")
    setup_info = experimental_design.reproducible_research_setup()
    print(f"   Project root: {setup_info['project_root']}")
    print(f"   Directories created: {len(setup_info['directories_created'])}")
    
    # Create experimental design matrix
    print("\n3. Creating experimental design matrix...")
    param_ranges = {
        'neuron_count': (500, 5000),
        'timestep': (0.001, 0.005),
        'plasticity': True,
        'curiosity': True,
        'development': False
    }
    
    design_matrix = experimental_design.create_experiment_matrix(param_ranges, n_combinations=20)
    print(f"   Generated {len(design_matrix)} experimental configurations")
    
    # Save design matrix
    experimental_design.save_design_matrix()
    
    # Create sample experiment configurations
    print("\n4. Creating experiment configurations...")
    experiment_configs = []
    
    for i, params in enumerate(design_matrix[:5]):  # Test first 5 configurations
        config = {
            'name': f'Experiment_{i+1}',
            'description': f'Test experiment with {params["neuron_count"]} neurons, timestep={params["timestep"]}',
            'config_template': {
                'brain.neuron_count': params['neuron_count'],
                'simulation_timestep': params['timestep'],
                'plasticity.stdp.enable': params['plasticity'],
                'neuromod.curiosity.enable': params['curiosity'],
                'development.enable': params.get('development', False)
            },
            'max_steps': 100,
            'timestep': params['timestep'],
            'save_checkpoints': True
        }
        
        experiment_configs.append(config)
    
    # Run experimental batch
    print("\n5. Running experimental batch...")
    batch_results = experimental_design.run_experimental_batch(experiment_configs)
    
    # Calculate power analysis
    print("\n6. Performing power analysis...")
    power_analysis = experimental_design.power_analysis(
        effect_size=0.5,
        alpha=0.05,
        power=0.8
    )
    print(f"   Effect size: {power_analysis['effect_size']}")
    print(f"   Required sample size: {power_analysis['required_sample_size']}")
    
    # Generate experiment reports
    print("\n7. Generating experiment reports...")
    for experiment_name in batch_results['results'].keys():
        if batch_results['results'][experiment_name]['status'] == 'success':
            report = experimental_design.generate_experiment_report(experiment_name)
            if report:
                print(f"   ✓ Report generated for {experiment_name}")
    
    # Display batch summary
    print(f"\n=== Experimental Design Summary ===")
    summary = batch_results['summary']
    print(f"Total experiments: {summary['total_experiments']}")
    print(f"Successful: {summary['successful_experiments']}")
    print(f"Success rate: {summary['success_rate']*100:.1f}%")
    
    if 'performance' in summary:
        print(f"\nPerformance metrics:")
        perf = summary['performance']
        print(f"  Mean duration: {perf['mean_duration']:.2f}s")
        print(f"  Mean firing rate: {perf['mean_firing_rate']:.3f} Hz")
        print(f"  Mean spike count: {perf['mean_spike_count']}")
    
    return experimental_design, batch_results

def example_power_analysis():
    """Demonstrate power analysis example."""
    print("\n=== Power Analysis Example ===\n")
    
    # Create experimental design
    experimental_design = ExperimentalDesign("Power_Analysis_Example")
    
    # Perform different power analyses
    effect_sizes = [0.2, 0.5, 0.8]
    powers = []
    
    print("Power analysis for different effect sizes:")
    for effect_size in effect_sizes:
        power_analysis = experimental_design.power_analysis(
            effect_size=effect_size,
            alpha=0.05,
            power=0.8
        )
        powers.append(power_analysis['calculated_power'])
        print(f"  Effect size {effect_size}: Power = {power_analysis['calculated_power']:.3f}, "
              f"Sample size = {power_analysis['required_sample_size']}")
    
    # Create visualization of power curves
    print("\nCreating power curve visualization...")
    try:
        import matplotlib.pyplot as plt
        from scipy import stats
        
        sample_sizes = np.arange(10, 201, 10)
        effect_sizes_for_plot = np.array(effect_sizes)
        
        plt.figure(figsize=(10, 6))
        
        for i, effect_size in enumerate(effect_sizes_for_plot):
            powers_for_size = []
            for n in sample_sizes:
                ncp = effect_size * np.sqrt(2 * (n / 2))  # Non-centrality parameter
                t_critical = stats.t.ppf(0.975, 2 * (n / 2 - 1))
                power = 1 - stats.nct.cdf(t_critical, 2 * (n / 2 - 1), ncp)
                powers_for_size.append(power)
            
            plt.plot(sample_sizes, powers_for_size, 
                    label=f'Effect size = {effect_size}', linewidth=2)
        
        plt.axhline(y=0.8, color='red', linestyle='--', label='Desired power (0.8)')
        plt.xlabel('Sample Size')
        plt.ylabel('Statistical Power')
        plt.title('Power Analysis for Different Effect Sizes')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.savefig("power_analysis_curves.png", dpi=150, bbox_inches='tight')
        plt.close()
        
        print("✓ Power curves saved to 'power_analysis_curves.png'")
        
    except ImportError:
        print("Note: Matplotlib not available for power curve visualization")
    
    return powers

def example_reproducible_research():
    """Demonstrate reproducible research setup."""
    print("\n=== Reproducible Research Example ===\n")
    
    # Create multiple research projects
    projects = ['neural_network_research', 'plasticity_studies', 'social_learning_research']
    
    for project_name in projects:
        print(f"\nSetting up reproducible research project: {project_name}")
        
        experimental_design = ExperimentalDesign(project_name)
        setup_info = experimental_design.reproducible_research_setup()
        
        # Create a sample experiment
        print(f"  Creating sample experiment...")
        experimental_design.create_experiment(
            name='Sample_Experiment',
            description='Sample experiment for demonstration',
            config_template={
                'brain.neuron_count': 1000,
                'simulation_timestep': 0.001
            }
        )
        
        # Generate report
        print(f"  Generating research report...")
        report = experimental_design.generate_experiment_report('Sample_Experiment')
        
        if report:
            print(f"  ✓ Research report generated with {len(report.get('recommendations', []))} recommendations")
    
    return projects

if __name__ == "__main__":
    print("NLM Advanced Features - Example 8: Experimental Design")
    print("=" * 65)
    
    try:
        # Run experimental design example
        experimental_design, batch_results = example_experimental_design()
        
        # Run power analysis example
        powers = example_power_analysis()
        
        # Run reproducible research example
        projects = example_reproducible_research()
        
        print("\n" + "=" * 65)
        print("Example 8 completed successfully!")
        print("Key insights:")
        print("  - Systematic experimental design improves research quality")
        print("  - Power analysis ensures adequate sample sizes")
        print("  - Reproducible research infrastructure enables collaboration")
        print("  - Batch experiments accelerate research progress")
        print("  - Experimental design matrices optimize parameter exploration")
        print("  - Statistical analysis validates experimental findings")
        
    except Exception as e:
        print(f"\nError in example 8: {e}")
        import traceback
        traceback.print_exc()
