"""
Experiment helper utilities for NLM simulations.

Provides comprehensive experiment setup and execution utilities for running
complete NLM agent simulations with proper configuration and subsystem management.
"""

from typing import Dict, Any, Optional, List, Callable
import time
import statistics

from .nlm_exception import ValidationError


class ExperimentConfig:
    """
    Configuration class for experiment setup.
    
    Provides structured configuration for experiment parameters including
    brain settings, world settings, and subsystem configurations.
    """
    
    def __init__(self, **kwargs):
        """
        Initialize experiment configuration.
        
        Args:
            **kwargs: Configuration parameters
        """
        self.params = kwargs
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        return self.params.get(key, default)
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value."""
        self.params[key] = value
    
    def get_all(self) -> Dict[str, Any]:
        """Get all configuration parameters."""
        return self.params.copy()
    
    def merge(self, other: 'ExperimentConfig') -> 'ExperimentConfig':
        """
        Merge another configuration.
        
        Args:
            other: Other configuration to merge
            
        Returns:
            Merged configuration
        """
        merged = ExperimentConfig(**self.params)
        merged.params.update(other.params)
        return merged


class ExperimentResults:
    """
    Container for experiment results.
    
    Provides comprehensive storage and analysis of experiment results.
    """
    
    def __init__(self, config: ExperimentConfig):
        """
        Initialize experiment results.
        
        Args:
            config: Experiment configuration
        """
        self.config = config
        self.brain: Optional['Brain'] = None
        self.agent: Optional['AgentBrain'] = None
        self.world: Optional['SimpleWorld'] = None
        self.metrics: List[Dict[str, Any]] = []
        self.start_time: Optional[float] = None
        self.end_time: Optional[float] = None
        self.success: bool = False
        self.errors: List[str] = []
    
    def add_metrics(self, metrics: Dict[str, Any]) -> None:
        """Add metrics for the current step."""
        self.metrics.append(metrics.copy())
    
    def get_summary(self) -> Dict[str, Any]:
        """
        Get experiment summary statistics.
        
        Returns:
            Dictionary containing summary statistics
        """
        if not self.metrics:
            return {
                'steps_completed': 0,
                'success': self.success,
                'errors': self.errors,
            }
        
        # Basic statistics
        summary = {
            'steps_completed': len(self.metrics),
            'success': self.success,
            'errors': self.errors,
        
            # Time statistics
            'duration': self.end_time - self.start_time if self.end_time and self.start_time else 0,
            'steps_per_second': len(self.metrics) / (self.end_time - self.start_time) 
                               if self.end_time and self.start_time and self.end_time > self.start_time else 0,
        }
        
        # Brain statistics
        if self.brain:
            summary.update({
                'final_neuron_count': self.brain.getTotalNeuronCount(),
                'final_synapse_count': self.brain.getTotalSynapseCount(),
                'final_firing_neuron_count': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'developmental_stage': self.brain.getDevelopmentalStage(),
            })
        
        # World statistics
        if self.world:
            body = self.world.getAgentBody()
            summary.update({
                'final_agent_x': body.x,
                'final_agent_y': body.y,
                'final_agent_energy': body.energy,
                'final_agent_health': body.health,
                'final_agent_age': body.age,
                'final_world_time': self.world.getSimulationTime(),
            })
        
        # Agent statistics
        if self.agent:
            summary.update({
                'final_curiosity': self.agent.getCuriosityLevel(),
                'final_novelty': self.agent.getNoveltyLevel(),
                'final_prediction_error': self.agent.getPredictionError(),
                'final_neuromodulation': self.agent.getNeuromodulationLevel(),
                'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled(),
                'development_enabled': self.agent.isDevelopmentEnabled(),
                'curiosity_enabled': self.agent.isCuriosityEnabled(),
            })
        
        # Performance statistics
        if self.metrics:
            # Calculate basic statistics
            neuron_counts = [m.get('neuron_count', 0) for m in self.metrics]
            firing_rates = [m.get('average_firing_rate', 0) for m in self.metrics]
            energies = [m.get('agent_energy', 0) for m in self.metrics]
            
            if neuron_counts:
                summary['initial_neuron_count'] = neuron_counts[0]
                summary['final_neuron_count'] = neuron_counts[-1]
                summary['neuron_count_change'] = neuron_counts[-1] - neuron_counts[0]
                summary['neuron_count_stdev'] = statistics.stdev(neuron_counts) if len(neuron_counts) > 1 else 0
            
            if firing_rates:
                summary['initial_firing_rate'] = firing_rates[0]
                summary['final_firing_rate'] = firing_rates[-1]
                summary['firing_rate_change'] = firing_rates[-1] - firing_rates[0]
                summary['firing_rate_stdev'] = statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0
            
            if energies:
                summary['initial_energy'] = energies[0]
                summary['final_energy'] = energies[-1]
                summary['energy_change'] = energies[-1] - energies[0]
                summary['energy_stdev'] = statistics.stdev(energies) if len(energies) > 1 else 0
        
        return summary
    
    def get_trend_analysis(self) -> Dict[str, Any]:
        """
        Get trend analysis of experiment results.
        
        Returns:
            Dictionary containing trend analysis
        """
        if len(self.metrics) < 3:
            return {
                'status': 'insufficient_data',
                'message': 'Need at least 3 data points for trend analysis'
            }
        
        trends = {}
        
        for key in self.metrics[0].keys():
            values = [m[key] for m in self.metrics if key in m]
            
            if len(values) >= 3:
                # Calculate slope (simple linear regression)
                n = len(values)
                x_values = list(range(n))
                
                x_mean = statistics.mean(x_values)
                y_mean = statistics.mean(values)
                
                numerator = sum((x - x_mean) * (y - y_mean) for x, y in zip(x_values, values))
                denominator = sum((x - x_mean) ** 2 for x in x_values)
                
                if denominator != 0:
                    slope = numerator / denominator
                    
                    # Determine trend category
                    if abs(slope) < 0.01:
                        trend = 'stable'
                        trend_strength = 'strong'
                    elif abs(slope) < 0.05:
                        trend = 'slightly_stable'
                        trend_strength = 'moderate'
                    elif abs(slope) < 0.1:
                        trend = 'moderate'
                        trend_strength = 'weak'
                    else:
                        trend = 'strong'
                        trend_strength = 'very_weak' if slope > 0 else 'very_weak_reverse'
                    
                    trends[key] = {
                        'slope': slope,
                        'trend': trend,
                        'strength': trend_strength,
                        'initial': values[0],
                        'final': values[-1],
                        'change': values[-1] - values[0],
                    }
        
        return {
            'status': 'analysis_complete',
            'trends': trends,
            'num_trends': len(trends),
        }
    
    def get_performance_profile(self) -> Dict[str, Any]:
        """
        Get performance profile of the experiment.
        
        Returns:
            Dictionary containing performance profile
        """
        summary = self.get_summary()
        
        profile = {
            'efficiency': {},
            'stability': {},
            'effectiveness': {},
        }
        
        # Efficiency metrics
        if 'steps_per_second' in summary:
            profile['efficiency']['throughput'] = summary['steps_per_second']
            
            if summary['steps_per_second'] > 10:
                profile['efficiency']['quality'] = 'excellent'
            elif summary['steps_per_second'] > 5:
                profile['efficiency']['quality'] = 'good'
            elif summary['steps_per_second'] > 1:
                profile['efficiency']['quality'] = 'fair'
            else:
                profile['efficiency']['quality'] = 'poor'
        
        # Stability metrics
        if 'neuron_count_stdev' in summary:
            neuron_stdev = summary['neuron_count_stdev']
            if neuron_stdev < 10:
                profile['stability']['brain_structure'] = 'very_stable'
            elif neuron_stdev < 50:
                profile['stability']['brain_structure'] = 'stable'
            elif neuron_stdev < 100:
                profile['stability']['brain_structure'] = 'moderately_stable'
            else:
                profile['stability']['brain_structure'] = 'unstable'
        
        if 'firing_rate_stdev' in summary:
            firing_stdev = summary['firing_rate_stdev']
            if firing_stdev < 0.5:
                profile['stability']['firing_activity'] = 'very_stable'
            elif firing_stdev < 2.0:
                profile['stability']['firing_activity'] = 'stable'
            elif firing_stdev < 5.0:
                profile['stability']['firing_activity'] = 'moderately_stable'
            else:
                profile['stability']['firing_activity'] = 'unstable'
        
        # Effectiveness metrics
        if 'final_energy' in summary and 'final_health' in summary:
            energy = summary['final_energy']
            health = summary['final_health']
            
            energy_ratio = energy / 100.0  # Assuming max energy is 100
            health_ratio = health / 1.0    # Assuming max health is 1.0
            
            effectiveness = (energy_ratio + health_ratio) / 2
            
            if effectiveness > 0.8:
                profile['effectiveness']['overall'] = 'excellent'
            elif effectiveness > 0.6:
                profile['effectiveness']['overall'] = 'good'
            elif effectiveness > 0.4:
                profile['effectiveness']['overall'] = 'fair'
            else:
                profile['effectiveness']['overall'] = 'poor'
        
        return profile
class ExperimentHelper:
    """
    Helper class for running NLM experiments.
    
    Provides comprehensive utilities for setting up and running NLM experiments
    with proper configuration, error handling, and result collection.
    """
    
    @staticmethod
    def run_experiment(
        config: 'Config',
        world: 'SimpleWorld',
        num_steps: int,
        enable_learning: bool = True,
        enable_development: bool = True,
        enable_curiosity: bool = True,
        enable_structural_plasticity: bool = True,
        log_interval: int = 100,
        verbose: bool = False
    ) -> ExperimentResults:
        """
        Run a complete NLM agent experiment.
        
        Args:
            config: Brain configuration
            world: World instance
            num_steps: Number of simulation steps
            enable_learning: Enable reward modulation learning
            enable_development: Enable developmental progression
            enable_curiosity: Enable curiosity-driven exploration
            enable_structural_plasticity: Enable structural plasticity
            log_interval: Interval for logging progress
            verbose: Enable verbose output
            
        Returns:
            Experiment results object
            
        Raises:
            ValidationError: If experiment setup fails
        """
        # Validate inputs
        if not config:
            raise ValidationError("Configuration cannot be None")
        if not world:
            raise ValidationError("World cannot be None")
        if num_steps <= 0:
            raise ValidationError(f"Number of steps must be positive, got {num_steps}")
        
        # Create experiment configuration
        exp_config = ExperimentConfig(
            num_steps=num_steps,
            enable_learning=enable_learning,
            enable_development=enable_development,
            enable_curiosity=enable_curiosity,
            enable_structural_plasticity=enable_structural_plasticity,
            log_interval=log_interval,
        )
        
        # Create experiment results object
        results = ExperimentResults(exp_config)
        
        try:
            # Create brain and agent
            brain = createBrain(config)
            agent = createAgentBrain(brain)
            
            results.brain = brain
            results.agent = agent
            results.world = world
            results.start_time = time.time()
            
            if verbose:
                print(f"Starting experiment with {num_steps} steps")
                print(f"Learning enabled: {enable_learning}")
                print(f"Development enabled: {enable_development}")
                print(f"Curiosity enabled: {enable_curiosity}")
                print(f"Structural plasticity enabled: {enable_structural_plasticity}")
            
            # Initialize everything
            brain.initialize()
            agent.initialize(world)
            
            # Enable subsystems based on arguments
            if enable_learning:
                agent.enableRewardModulation(True)
                if verbose:
                    print("Reward modulation enabled")
            
            if enable_structural_plasticity:
                agent.enableStructuralPlasticity(True)
                if verbose:
                    print("Structural plasticity enabled")
            
            if enable_development:
                agent.enableDevelopment(True)
                if verbose:
                    print("Development enabled")
            
            if enable_curiosity:
                agent.enableCuriosity(True)
                if verbose:
                    print("Curiosity enabled")
            
            # Run simulation
            for step in range(num_steps):
                try:
                    # Update world
                    world.update(0.1)
                    
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Process sensory input
                    agent.processSensoryInput(percept)
                    
                    # Run brain step
                    brain.step(step)
                    
                    # Get action and apply it
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Apply reward if learning is enabled
                    if enable_learning:
                        # Simple reward based on energy
                        reward = world.getAgentBody().energy / 100.0
                        agent.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    if enable_development:
                        agent.updateDevelopment(0.1)
                    
                    # Log progress
                    if step % log_interval == 0:
                        metrics = SimulationMetrics.collect(brain, world, agent)
                        results.add_metrics(metrics)
                        
                        if verbose:
                            print(f"Step {step}:")
                            print(f"  Firing neurons: {metrics['firing_neuron_count']}")
                            print(f"  Curiosity: {metrics.get('agent_curiosity_level', 0):.3f}")
                            print(f"  Novelty: {metrics.get('agent_novelty_level', 0):.3f}")
                            print(f"  Dev Stage: {metrics['developmental_stage']}")
                            print(f"  Agent Energy: {metrics.get('agent_energy', 0):.1f}")
                    
                except Exception as e:
                    error_msg = f"Error at step {step}: {e}"
                    results.errors.append(error_msg)
                    
                    if verbose:
                        print(f"ERROR: {error_msg}")
                    
                    # Continue with experiment (but record error)
                    continue
            
            # Record final metrics
            final_metrics = SimulationMetrics.collect(brain, world, agent)
            results.add_metrics(final_metrics)
            
            results.end_time = time.time()
            results.success = True
            
            if verbose:
                print(f"\nExperiment completed successfully!")
                print(f"Total steps: {num_steps}")
                print(f"Errors encountered: {len(results.errors)}")
                print(f"Duration: {results.end_time - results.start_time:.2f} seconds")
                print(f"Steps per second: {num_steps / (results.end_time - results.start_time):.2f}")
        
        except Exception as e:
            error_msg = f"Experiment failed: {e}"
            results.errors.append(error_msg)
            results.success = False
            
            if verbose:
                print(f"ERROR: {error_msg}")
                import traceback
                traceback.print_exc()
        
        return results
    
    @staticmethod
    def run_multiple_experiments(
        config: 'Config',
        world: 'SimpleWorld',
        num_steps_list: List[int],
        experiment_configs: List[Dict[str, Any]],
        verbose: bool = False
    ) -> List[ExperimentResults]:
        """
        Run multiple experiments with different configurations.
        
        Args:
            config: Base brain configuration
            world: World instance
            num_steps_list: List of step counts for each experiment
            experiment_configs: List of configuration dictionaries for each experiment
            verbose: Enable verbose output
            
        Returns:
            List of experiment results
        """
        if len(num_steps_list) != len(experiment_configs):
            raise ValidationError("Number of step counts must match number of configurations")
        
        results = []
        
        for i, (num_steps, exp_config) in enumerate(zip(num_steps_list, experiment_configs)):
            if verbose:
                print(f"\nRunning experiment {i + 1}/{len(num_steps_list)}")
                print(f"Configuration: {exp_config}")
            
            # Merge configurations
            merged_config = ExperimentConfig(**exp_config)
            
            # Run experiment
            exp_result = ExperimentHelper.run_experiment(
                config=config,
                world=world,
                num_steps=num_steps,
                **merged_config.params
            )
            
            results.append(exp_result)
            
            if verbose:
                print(f"Experiment {i + 1} completed with {exp_result.success}")
                if exp_result.errors:
                    print(f"Errors: {len(exp_result.errors)}")
        
        return results
    
    @staticmethod
    def compare_experiments(
        experiment_results: List[ExperimentResults],
        metric_keys: Optional[List[str]] = None
    ) -> Dict[str, Any]:
        """
        Compare multiple experiment results.
        
        Args:
            experiment_results: List of experiment results to compare
            metric_keys: List of metric keys to compare (defaults to all available)
            
        Returns:
            Dictionary containing comparison results
        """
        if not experiment_results:
            return {
                'status': 'error',
                'message': 'No experiment results provided'
            }
        
        # Collect all metrics
        all_metrics = {}
        for exp_result in experiment_results:
            exp_key = f"Experiment_{len(all_metrics)}"
            all_metrics[exp_key] = {
                'config': exp_result.config.get_all(),
                'summary': exp_result.get_summary(),
                'success': exp_result.success,
                'errors': exp_result.errors,
            }
            
            if exp_result.metrics:
                # Collect metrics from each step
                for i, metrics in enumerate(exp_result.metrics):
                    all_metrics[exp_key][f'step_{i}'] = metrics
        
        # Determine metric keys to compare
        if metric_keys is None:
            # Use keys from the first experiment's summary
            if experiment_results and experiment_results[0].metrics:
                metric_keys = list(experiment_results[0].metrics[0].keys())
            else:
                metric_keys = []
        
        comparison = {
            'experiments': {},
            'metrics': metric_keys,
            'summary': {
                'total_experiments': len(experiment_results),
                'successful_experiments': sum(1 for exp in experiment_results if exp.success),
                'failed_experiments': sum(1 for exp in experiment_results if not exp.success),
            },
        }
        
        # Compare each experiment
        for i, exp_result in enumerate(experiment_results):
            comparison['experiments'][f'exp_{i}'] = {
                'success': exp_result.success,
                'errors': exp_result.errors,
                'final_metrics': exp_result.metrics[-1] if exp_result.metrics else None,
                'summary': exp_result.get_summary(),
                'performance_profile': exp_result.get_performance_profile(),
            }
        
        return comparison
    
    @staticmethod
    def analyze_experiment_reproduction(
        baseline_results: ExperimentResults,
        test_results: ExperimentResults,
        tolerance: float = 0.1
    ) -> Dict[str, Any]:
        """
        Analyze whether test results reproduce baseline results.
        
        Args:
            baseline_results: Baseline experiment results
            test_results: Test experiment results
            tolerance: Tolerance for considering metrics similar
            
        Returns:
            Dictionary containing reproduction analysis
        ""
        analysis = {
            'reproduced': True,
            'similarity_score': 0.0,
            'differences': [],
            'summary': '',
        }
        
        if not baseline_results.metrics or not test_results.metrics:
            analysis['reproduced'] = False
            analysis['summary'] = 'One or both experiments have no metrics'
            return analysis
        
        # Compare final metrics
        baseline_final = baseline_results.metrics[-1]
        test_final = test_results.metrics[-1]
        
        # Compare key metrics
        key_metrics = [
            'average_firing_rate',
            'total_spike_count',
            'firing_neuron_count',
            'developmental_stage',
            'agent_energy',
            'agent_health',
            'agent_curiosity_level',
            'agent_novelty_level',
        ]
        
        similarities = 0
        total_comparisons = 0
        
        for metric in key_metrics:
            if metric in baseline_final and metric in test_final:
                baseline_val = baseline_final[metric]
                test_val = test_final[metric]
                
                total_comparisons += 1
                
                if baseline_val == test_val:
                    similarities += 1
                elif isinstance(baseline_val, (int, float)) and isinstance(test_val, (int, float)):
                    # Calculate relative difference
                    if baseline_val == 0:
                        diff = abs(test_val)
                    else:
                        diff = abs((test_val - baseline_val) / baseline_val)
                    
                    if diff <= tolerance:
                        similarities += 1
                    else:
                        analysis['differences'].append({
                            'metric': metric,
                            'baseline': baseline_val,
                            'test': test_val,
                            'difference': diff,
                            'threshold': tolerance,
                        })
        
        # Calculate similarity score
        if total_comparisons > 0:
            analysis['similarity_score'] = similarities / total_comparisons
        
        # Determine if reproduction was successful
        analysis['reproduced'] = analysis['similarity_score'] >= (1.0 - tolerance)
        
        # Generate summary
        if analysis['reproduced']:
            analysis['summary'] = f"Experiment successfully reproduced ({analysis['similarity_score']*100:.1f}% similarity)"
        else:
            analysis['summary'] = f"Experiment reproduction failed ({analysis['similarity_score']*100:.1f}% similarity)"
            if analysis['differences']:
                analysis['summary'] += f" with {len(analysis['differences'])} significant differences"
        
        return analysis
    
    @staticmethod
    def export_experiment_results(
        experiment_results: ExperimentResults,
        filename: str,
        format: str = 'csv'
    ) -> bool:
        """
        Export experiment results to a file.
        
        Args:
            experiment_results: Experiment results to export
            filename: Output filename
            format: Export format ('csv', 'json', 'tsv')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Create data for export
            export_data = []
            
            for i, metrics in enumerate(experiment_results.metrics):
                export_data.append({
                    'step': i,
                    **metrics,
                })
            
            # Add experiment metadata
            export_metadata = {
                'experiment_config': experiment_results.config.get_all(),
                'summary': experiment_results.get_summary(),
                'success': experiment_results.success,
                'errors': experiment_results.errors,
            }
            
            if format == 'csv':
                import csv
                with open(filename, 'w', newline='') as csvfile:
                    if export_data:
                        writer = csv.DictWriter(csvfile, fieldnames=export_data[0].keys())
                        writer.writeheader()
                        writer.writerows(export_data)
                    else:
                        # Write header with metadata
                        writer = csv.writer(csvfile)
                        writer.writerow(['step', 'config', 'summary', 'success', 'errors'])
                        writer.writerow([0, str(export_metadata['experiment_config']), 
                                       str(export_metadata['summary']), 
                                       export_metadata['success'], 
                                       '; '.join(export_metadata['errors']) if export_metadata['errors'] else ''])
            
            elif format == 'json':
                import json
                with open(filename, 'w') as jsonfile:
                    json.dump({
                        'metadata': export_metadata,
                        'data': export_data,
                    }, jsonfile, indent=2, default=str)
            
            elif format == 'tsv':
                import csv
                with open(filename, 'w', newline='') as tsvfile:
                    if export_data:
                        writer = csv.DictWriter(tsvfile, fieldnames=export_data[0].keys(), delimiter='\t')
                        writer.writeheader()
                        writer.writerows(export_data)
                    else:
                        writer = csv.writer(tsvfile, delimiter='\t')
                        writer.writerow(['step', 'config', 'summary', 'success', 'errors'])
                        writer.writerow([0, str(export_metadata['experiment_config']), 
                                       str(export_metadata['summary']), 
                                       export_metadata['success'], 
                                       '; '.join(export_metadata['errors']) if export_metadata['errors'] else ''])
            
            else:
                raise ValidationError(f"Unsupported format: {format}")
            
            return True
        
        except Exception as e:
            raise ValidationError(f"Failed to export experiment results: {e}")
    
    @staticmethod
    def import_experiment_results(
        filename: str,
        format: str = 'csv'
    ) -> ExperimentResults:
        """
        Import experiment results from a file.
        
        Args:
            filename: Input filename
            format: Import format ('csv', 'json', 'tsv')
            
        Returns:
            Experiment results object
        """
        try:
            # Import data
            if format == 'csv':
                import csv
                with open(filename, 'r') as csvfile:
                    reader = csv.DictReader(csvfile)
                    data = list(reader)
            
            elif format == 'json':
                import json
                with open(filename, 'r') as jsonfile:
                    imported = json.load(jsonfile)
                    data = imported.get('data', [])
                    metadata = imported.get('metadata', {})
            
            elif format == 'tsv':
                import csv
                with open(filename, 'r') as tsvfile:
                    reader = csv.DictReader(tsvfile, delimiter='\t')
                    data = list(reader)
            
            else:
                raise ValidationError(f"Unsupported format: {format}")
            
            # Create experiment results
            config = ExperimentConfig()
            exp_result = ExperimentResults(config)
            
            # Parse data
            for i, row in enumerate(data):
                metrics = {}
                for key, value in row.items():
                    if key.startswith('step_'):
                        continue
                    
                    try:
                        # Try to convert to appropriate type
                        if value.lower() in ['true', 'false']:
                            metrics[key] = value.lower() == 'true'
                        elif value.replace('.', '', 1).replace('-', '', 1).isdigit():
                            if '.' in value:
                                metrics[key] = float(value)
                            else:
                                metrics[key] = int(value)
                        else:
                            metrics[key] = value
                    except:
                        metrics[key] = value
                
                exp_result.add_metrics(metrics)
            
            return exp_result
        
        except Exception as e:
            raise ValidationError(f"Failed to import experiment results: {e}")


# Import helper classes
from .simulation_metrics import SimulationMetrics
from .performance_monitor import PerformanceMonitor
from .registration_system import BrainContext

# Create module-level exports
__all__ = [
    'ExperimentConfig',
    'ExperimentResults',
    'ExperimentHelper',
    'SimulationMetrics',
    'PerformanceMonitor',
    'BrainContext',
]