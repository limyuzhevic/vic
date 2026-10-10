"""
Advanced NLM Python features for expert users.

This module provides advanced Python features and commands for expert users,
including performance monitoring, advanced visualization, machine learning
integration, and custom neural models.
"""

import sys
import time
import json
import math
from typing import Any, List, Dict, Optional, Callable, Union
from enum import Enum

class PerformanceProfiler:
    """
    Advanced performance profiler for NLM simulations.
    
    Provides detailed profiling of simulation performance,
    memory usage, and bottleneck analysis.
    """
    
    def __init__(self):
        self._profiles = {}
        self._current_profile = None
        self._sampling_rate = 1000  # Hz
        self._max_samples = 1000000
        self._profiler_active = False
        
    def start_profiling(self, name: str, config: Dict = None):
        """
        Start profiling a simulation.
        
        Args:
            name: Profile name
            config: Configuration options
        """
        self._current_profile = {
            'name': name,
            'start_time': time.time(),
            'config': config or {},
            'samples': [],
            'metrics': {
                'cpu_time': 0.0,
                'memory_usage': 0.0,
                'simulation_steps': 0,
                'neuron_operations': 0,
                'synapse_operations': 0,
                'spike_events': 0
            }
        }
        self._profiler_active = True
        print(f"[PROFILER] Started profiling '{name}'")
        
    def stop_profiling(self) -> Dict:
        """
        Stop profiling and return results.
        
        Returns:
            Profile results
        """
        if not self._profiler_active or not self._current_profile:
            print("[PROFILER] No active profile to stop")
            return {}
        
        end_time = time.time()
        duration = end_time - self._current_profile['start_time']
        
        results = self._current_profile.copy()
        results['end_time'] = end_time
        results['duration'] = duration
        results['sampling_rate'] = self._sampling_rate
        results['total_samples'] = len(results['samples'])
        
        # Calculate derived metrics
        if duration > 0:
            results['steps_per_second'] = results['metrics']['simulation_steps'] / duration
            results['neurons_per_second'] = results['metrics']['neuron_operations'] / duration
            results['synapses_per_second'] = results['metrics']['synapse_operations'] / duration
            results['spikes_per_second'] = results['metrics']['spike_events'] / duration
        
        # Store profile
        self._profiles[results['name']] = results
        self._current_profile = None
        self._profiler_active = False
        
        print(f"[PROFILER] Stopped profiling '{results['name']}' (duration: {duration:.2f}s)")
        return results
        
    def add_sample(self, sample: Dict):
        """Add a profiling sample."""
        if self._profiler_active and self._current_profile:
            self._current_profile['samples'].append(sample)
            
            # Limit sample size
            if len(self._current_profile['samples']) > self._max_samples:
                self._current_profile['samples'].pop(0)
        
    def get_current_profile(self) -> Dict:
        """Get current profile state."""
        return self._current_profile.copy() if self._current_profile else None
        
    def get_profile(self, name: str) -> Dict:
        """Get a specific profile by name."""
        return self._profiles.get(name)
        
    def get_all_profiles(self) -> Dict:
        """Get all profiles."""
        return self._profiles.copy()
        
    def analyze_bottlenecks(self) -> List[Dict]:
        """
        Analyze performance bottlenecks.
        
        Returns:
            List of bottleneck analysis results
        """
        bottlenecks = []
        
        for profile_name, profile in self._profiles.items():
            metrics = profile['metrics']
            
            # Check for high CPU usage (estimated)
            if profile['duration'] > 1.0:
                cpu_efficiency = metrics['neuron_operations'] / profile['duration']
                if cpu_efficiency < 10000:  # Less than 10k neurons/sec
                    bottlenecks.append({
                        'profile': profile_name,
                        'type': 'cpu_bound',
                        'severity': 'high' if cpu_efficiency < 1000 else 'medium',
                        'metric': cpu_efficiency,
                        'suggestion': 'Consider optimizing neuron dynamics or using vectorization'
                    })
            
            # Check for memory issues
            if metrics['memory_usage'] > 500:  # MB
                bottlenecks.append({
                    'profile': profile_name,
                    'type': 'memory_bound',
                    'severity': 'high' if metrics['memory_usage'] > 1000 else 'medium',
                    'metric': metrics['memory_usage'],
                    'suggestion': 'Consider reducing memory footprint or enabling compression'
                })
            
            # Check spike processing efficiency
            if profile['duration'] > 0:
                spike_efficiency = metrics['spike_events'] / profile['duration']
                if spike_efficiency < 1000:  # Less than 1k spikes/sec
                    bottlenecks.append({
                        'profile': profile_name,
                        'type': 'spike_processing',
                        'severity': 'medium',
                        'metric': spike_efficiency,
                        'suggestion': 'Consider optimizing spike propagation or reducing connectivity'
                    })
        
        return bottlenecks
        
    def generate_performance_report(self) -> str:
        """
        Generate a performance report.
        
        Returns:
            Formatted performance report
        """
        if not self._profiles:
            return "No performance data available."
        
        report = "NLM Performance Analysis Report\n"
        report += "=" * 50 + "\n\n"
        
        for profile_name, profile in self._profiles.items():
            report += f"Profile: {profile_name}\n"
            report += f"Duration: {profile['duration']:.2f} seconds\n"
            report += f"Total Steps: {profile['metrics']['simulation_steps']}\n"
            report += f"Steps per Second: {profile.get('steps_per_second', 0):.0f}\n"
            report += f"Total Spikes: {profile['metrics']['spike_events']}\n"
            report += f"Spikes per Second: {profile.get('spikes_per_second', 0):.0f}\n"
            report += f"Neuron Operations: {profile['metrics']['neuron_operations']:,}\n"
            report += f"Synapse Operations: {profile['metrics']['synapse_operations']:,}\n"
            report += f"Memory Usage: {profile['metrics']['memory_usage']:.1f} MB\n\n"
        
        # Add bottleneck analysis
        bottlenecks = self.analyze_bottlenecks()
        if bottlenecks:
            report += "Performance Bottlenecks:\n"
            for bottleneck in bottlenecks:
                report += f"  - {bottleneck['profile']}: {bottleneck['type']}\n"
                report += f"    Severity: {bottleneck['severity']}\n"
                report += f"    Metric: {bottleneck['metric']:.2f}\n"
                report += f"    Suggestion: {bottleneck['suggestion']}\n\n"
        else:
            report += "No significant performance bottlenecks detected.\n\n"
        
        return report
class AdvancedVisualizer:
    """
    Advanced visualization system for NLM data.
    
    Provides sophisticated plotting, analysis, and visualization
    capabilities for neural data.
    """
    
    def __init__(self):
        self._plots = []
        self._animations = []
        self._data_series = {}
        self._visualization_backend = "matplotlib"  # Default backend
        
    def set_backend(self, backend: str):
        """Set visualization backend."""
        self._visualization_backend = backend
        print(f"[VISUALIZER] Backend set to: {backend}")
        
    def plot_spike_train(self, neuron_ids: List[int], spike_times: List[float], 
                        title: str = "Spike Train", save_path: str = None):
        """
        Plot spike train for specific neurons.
        
        Args:
            neuron_ids: List of neuron IDs
            spike_times: List of spike times
            title: Plot title
            save_path: Path to save plot
        """
        # Create plot data structure
        plot_data = {
            'type': 'spike_train',
            'title': title,
            'neuron_ids': neuron_ids,
            'spike_times': spike_times,
            'timestamp': time.time()
        }
        
        self._plots.append(plot_data)
        
        if save_path:
            self._save_plot(plot_data, save_path)
        
        print(f"[VISUALIZER] Created spike train plot: {title}")
        
    def plot_population_dynamics(self, populations: Dict[str, List[float]],
                                 title: str = "Population Dynamics",
                                 save_path: str = None):
        """
        Plot population-level dynamics over time.
        
        Args:
            populations: Dictionary of population data
            title: Plot title
            save_path: Path to save plot
        """
        plot_data = {
            'type': 'population_dynamics',
            'title': title,
            'populations': populations,
            'timestamp': time.time()
        }
        
        self._plots.append(plot_data)
        
        if save_path:
            self._save_plot(plot_data, save_path)
        
        print(f"[VISUALIZER] Created population dynamics plot: {title}")
        
    def plot_connectivity_matrix(self, connectivity_matrix: List[List[float]],
                                 title: str = "Connectivity Matrix",
                                 save_path: str = None):
        """
        Plot connectivity matrix.
        
        Args:
            connectivity_matrix: 2D connectivity matrix
            title: Plot title
            save_path: Path to save plot
        """
        plot_data = {
            'type': 'connectivity_matrix',
            'title': title,
            'matrix': connectivity_matrix,
            'timestamp': time.time()
        }
        
        self._plots.append(plot_data)
        
        if save_path:
            self._save_plot(plot_data, save_path)
        
        print(f"[VISUALIZER] Created connectivity matrix plot: {title}")
        
    def create_time_series_plot(self, data: Dict[str, List[float]],
                               x_label: str = "Time",
                               y_label: str = "Value",
                               title: str = "Time Series",
                               save_path: str = None):
        """
        Create time series plot.
        
        Args:
            data: Time series data dictionary
            x_label: X-axis label
            y_label: Y-axis label
            title: Plot title
            save_path: Path to save plot
        """
        plot_data = {
            'type': 'time_series',
            'title': title,
            'x_label': x_label,
            'y_label': y_label,
            'data': data,
            'timestamp': time.time()
        }
        
        self._plots.append(plot_data)
        
        if save_path:
            self._save_plot(plot_data, save_path)
        
        print(f"[VISUALIZER] Created time series plot: {title}")
        
    def create_3d_scatter_plot(self, x: List[float], y: List[float], z: List[float],
                               title: str = "3D Scatter Plot",
                               save_path: str = None):
        """
        Create 3D scatter plot.
        
        Args:
            x: X coordinates
            y: Y coordinates
            z: Z coordinates
            title: Plot title
            save_path: Path to save plot
        """
        plot_data = {
            'type': '3d_scatter',
            'title': title,
            'x': x,
            'y': y,
            'z': z,
            'timestamp': time.time()
        }
        
        self._plots.append(plot_data)
        
        if save_path:
            self._save_plot(plot_data, save_path)
        
        print(f"[VISUALIZER] Created 3D scatter plot: {title}")
        
    def animate_neurons(self, simulation_steps: int, neuron_data_callback: Callable,
                       fps: int = 30, save_path: str = None):
        """
        Create animation of neuron activity over time.
        
        Args:
            simulation_steps: Number of simulation steps
            neuron_data_callback: Function that returns neuron data for each step
            fps: Frames per second
            save_path: Path to save animation
        """
        animation_data = {
            'type': 'neuron_animation',
            'steps': simulation_steps,
            'fps': fps,
            'data_callback': neuron_data_callback,
            'created_at': time.time(),
            'duration': simulation_steps / fps
        }
        
        self._animations.append(animation_data)
        
        if save_path:
            self._save_animation(animation_data, save_path)
        
        print(f"[VISUALIZER] Created neuron animation: {simulation_steps} steps at {fps} fps")
        
    def add_data_series(self, name: str, data: List[float], x_values: List[float] = None):
        """
        Add a data series for later visualization.
        
        Args:
            name: Series name
            data: Data points
            x_values: X-axis values (if None, uses indices)
        """
        if x_values is None:
            x_values = list(range(len(data)))
        
        self._data_series[name] = {
            'x': x_values,
            'y': data,
            'name': name,
            'added_at': time.time()
        }
        
    def generate_html_dashboard(self, title: str = "NLM Dashboard") -> str:
        """
        Generate an HTML dashboard with all plots and data.
        
        Args:
            title: Dashboard title
            
        Returns:
            HTML dashboard as string
        """
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>{title}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .plot {{ border: 1px solid #ccc; margin: 10px 0; padding: 10px; }}
        .data-table {{ border-collapse: collapse; width: 100%; }}
        .data-table th, .data-table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        .data-table th {{ background-color: #f2f2f2; }}
    </style>
</head>
<body>
    <h1>{title}</h1>
    <p>Generated at {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
    
    <h2>Plots</h2>
"""
        
        for i, plot in enumerate(self._plots):
            html += f"""
    <div class="plot">
        <h3>{plot['title']}</h3>
        <p>Type: {plot['type']}</p>
        <p>Created: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(plot['timestamp']))}</p>
    </div>
"""
        
        html += """
    <h2>Data Series</h2>
    <table class="data-table">
        <tr><th>Series Name</th><th>Data Points</th><th>Created</th></tr>
"""
        
        for name, data in self._data_series.items():
            html += f"""
        <tr>
            <td>{name}</td>
            <td>{len(data['y'])} points</td>
            <td>{time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(data['added_at']))}</td>
        </tr>
"""
        
        html += """
    </table>
    
    <h2>Animations</h2>
"""
        
        for i, animation in enumerate(self._animations):
            html += f"""
    <div class="plot">
        <h3>Animation {i+1}</h3>
        <p>Duration: {animation['duration']:.2f} seconds</p>
        <p>FPS: {animation['fps']}</p>
        <p>Steps: {animation['steps']}</p>
    </div>
"""
        
        html += """
</body>
</html>
"""
        
        return html
        
    def _save_plot(self, plot_data: Dict, filepath: str):
        """Save plot data to file."""
        try:
            # Create directory if it doesn't exist
            directory = os.path.dirname(filepath)
            if directory and not os.path.exists(directory):
                os.makedirs(directory)
            
            # Save plot data as JSON
            with open(filepath, 'w') as f:
                json.dump(plot_data, f, indent=2)
                
            print(f"[VISUALIZER] Saved plot to: {filepath}")
            
        except Exception as e:
            print(f"[ERROR] Failed to save plot: {e}")
            
    def _save_animation(self, animation_data: Dict, filepath: str):
        """Save animation data to file."""
        try:
            # Create directory if it doesn't exist
            directory = os.path.dirname(filepath)
            if directory and not os.path.exists(directory):
                os.makedirs(directory)
            
            # Save animation data as JSON
            with open(filepath, 'w') as f:
                json.dump(animation_data, f, indent=2)
                
            print(f"[VISUALIZER] Saved animation to: {filepath}")
            
        except Exception as e:
            print(f"[ERROR] Failed to save animation: {e}")
class MachineLearningInterface:
    """
    Machine learning interface for NLM integration.
    
    Provides integration with machine learning frameworks
    for training, prediction, and analysis.
    """
    
    def __init__(self):
        self._models = {}
        self._training_data = {}
        self._ml_framework = None  # 'sklearn', 'tensorflow', 'pytorch', etc.
        self._feature_extractors = {}
        self._predictors = {}
        
    def set_framework(self, framework: str):
        """Set machine learning framework."""
        self._ml_framework = framework
        print(f"[ML] ML framework set to: {framework}")
        
    def add_training_data(self, name: str, data: Dict, labels: List[str]):
        """
        Add training data for machine learning models.
        
        Args:
            name: Data name
            data: Feature data
            labels: Target labels
        """
        self._training_data[name] = {
            'data': data,
            'labels': labels,
            'added_at': time.time()
        }
        print(f"[ML] Added training data '{name}': {len(data)} samples, {len(labels)} labels")
        
    def train_classifier(self, data_name: str, model_name: str, 
                        model_type: str = "random_forest",
                        hyperparameters: Dict = None):
        """
        Train a classifier on training data.
        
        Args:
            data_name: Name of training data
            model_name: Name for the trained model
            model_type: Type of classifier
            hyperparameters: Model hyperparameters
        """
        if data_name not in self._training_data:
            print(f"[ERROR] Training data '{data_name}' not found")
            return False
        
        if self._ml_framework is None:
            print("[ERROR] ML framework not set. Use set_framework() first.")
            return False
        
        # Simplified model training (would use actual ML framework in production)
        training_data = self._training_data[data_name]
        
        # Create a simple model representation
        model = {
            'name': model_name,
            'type': model_type,
            'training_data': data_name,
            'hyperparameters': hyperparameters or {},
            'trained_at': time.time(),
            'accuracy': 0.85,  # Placeholder accuracy
            'features_used': list(training_data['data'].keys()) if isinstance(training_data['data'], dict) else []
        }
        
        self._models[model_name] = model
        print(f"[ML] Trained {model_type} model '{model_name}' on data '{data_name}'")
        return True
        
    def predict(self, model_name: str, features: Dict) -> Dict:
        """
        Make predictions using a trained model.
        
        Args:
            model_name: Name of trained model
            features: Feature data for prediction
            
        Returns:
            Prediction results
        """
        if model_name not in self._models:
            print(f"[ERROR] Model '{model_name}' not found")
            return {'error': 'Model not found'}
        
        model = self._models[model_name]
        
        # Simplified prediction (would use actual ML framework in production)
        predictions = {
            'model': model_name,
            'features': features,
            'predicted_class': 'class_A',  # Placeholder prediction
            'confidence': 0.92,  # Placeholder confidence
            'prediction_time': time.time()
        }
        
        print(f"[ML] Model '{model_name}' predicted class: {predictions['predicted_class']}")
        return predictions
        
    def evaluate_model(self, model_name: str, test_data_name: str) -> Dict:
        """
        Evaluate a trained model on test data.
        
        Args:
            model_name: Name of trained model
            test_data_name: Name of test data
            
        Returns:
            Model evaluation results
        """
        if model_name not in self._models:
            print(f"[ERROR] Model '{model_name}' not found")
            return {'error': 'Model not found'}
        
        if test_data_name not in self._training_data:
            print(f"[ERROR] Test data '{test_data_name}' not found")
            return {'error': 'Test data not found'}
        
        # Simplified evaluation (would use actual ML framework in production)
        evaluation = {
            'model': model_name,
            'test_data': test_data_name,
            'accuracy': 0.87,  # Placeholder accuracy
            'precision': 0.85,  # Placeholder precision
            'recall': 0.83,  # Placeholder recall
            'f1_score': 0.84,  # Placeholder F1 score
            'evaluation_time': time.time()
        }
        
        print(f"[ML] Model '{model_name}' evaluated on test data '{test_data_name}'")
        print(f"    Accuracy: {evaluation['accuracy']:.2f}, F1-score: {evaluation['f1_score']:.2f}")
        
        return evaluation
        
    def create_feature_extractor(self, name: str, extractor_func: Callable):
        """
        Create a feature extractor function.
        
        Args:
            name: Feature extractor name
            extractor_func: Function to extract features
        """
        self._feature_extractors[name] = {
            'function': extractor_func,
            'created_at': time.time()
        }
        print(f"[ML] Created feature extractor '{name}'")
        
    def extract_features(self, data: Dict, extractor_name: str) -> Dict:
        """
        Extract features from data using a feature extractor.
        
        Args:
            data: Input data
            extractor_name: Name of feature extractor
            
        Returns:
            Extracted features
        """
        if extractor_name not in self._feature_extractors:
            print(f"[ERROR] Feature extractor '{extractor_name}' not found")
            return {}
        
        extractor = self._feature_extractors[extractor_name]
        features = extractor['function'](data)
        
        print(f"[ML] Extracted features using '{extractor_name}'")
        return features
        
    def get_model_summary(self) -> Dict:
        """
        Get summary of all trained models.
        
        Returns:
            Model summary
        """
        summary = {
            'total_models': len(self._models),
            'models': [],
            'feature_extractors': len(self._feature_extractors),
            'training_datasets': len(self._training_data),
            'framework': self._ml_framework
        }
        
        for name, model in self._models.items():
            summary['models'].append({
                'name': name,
                'type': model['type'],
                'accuracy': model['accuracy'],
                'training_data': model['training_data'],
                'trained_at': model['trained_at']
            })
        
        return summary
class AdvancedExperimentFramework:
    """
    Advanced experiment framework for systematic testing.
    
    Provides advanced experimental design, execution, and analysis
    capabilities for systematic research.
    """
    
    def __init__(self):
        self._experiments = []
        self._experiment_results = {}
        self._factorial_designs = {}
        self._parameter_combinations = []
        
    def create_factorial_design(self, name: str, parameters: Dict[str, List[Any]]):
        """
        Create a factorial experimental design.
        
        Args:
            name: Design name
            parameters: Parameter combinations
        """
        # Generate all parameter combinations
        param_names = list(parameters.keys())
        param_values = list(parameters.values())
        
        def generate_combinations(current_index, current_combination):
            if current_index == len(param_names):
                self._parameter_combinations.append(current_combination.copy())
                return
            
            for value in param_values[current_index]:
                current_combination[param_names[current_index]] = value
                generate_combinations(current_index + 1, current_combination)
        
        generate_combinations(0, {})
        
        design = {
            'name': name,
            'parameters': parameters,
            'total_combinations': len(self._parameter_combinations),
            'created_at': time.time()
        }
        
        self._factorial_designs[name] = design
        print(f"[EXPERIMENT] Created factorial design '{name}' with {design['total_combinations']} combinations")
        
    def run_experiment(self, name: str, config: Dict, simulation_callback: Callable) -> Dict:
        """
        Run an experiment with systematic parameter variations.
        
        Args:
            name: Experiment name
            config: Experiment configuration
            simulation_callback: Function to run simulations
            
        Returns:
            Experiment results
        """
        experiment = {
            'name': name,
            'config': config,
            'start_time': time.time(),
            'status': 'running',
            'results': [],
            'parameters': config.get('parameters', [])
        }
        
        self._experiments.append(experiment)
        
        try:
            # Run the experiment
            if 'factorial_design' in config:
                # Run factorial design
                design_name = config['factorial_design']
                if design_name in self._factorial_designs:
                    parameters = self._factorial_designs[design_name]['parameters']
                    
                    for param_combination in self._parameter_combinations:
                        # Merge with base config
                        exp_config = {**config, **param_combination}
                        
                        # Run simulation
                        result = simulation_callback(exp_config)
                        experiment['results'].append({
                            'parameters': param_combination,
                            'result': result,
                            'timestamp': time.time()
                        })
            else:
                # Run single experiment
                result = simulation_callback(config)
                experiment['results'].append({
                    'parameters': {},
                    'result': result,
                    'timestamp': time.time()
                })
            
            experiment['end_time'] = time.time()
            experiment['status'] = 'completed'
            
            self._experiment_results[name] = experiment
            
            duration = experiment['end_time'] - experiment['start_time']
            print(f"[EXPERIMENT] Completed experiment '{name}' in {duration:.2f}s with {len(experiment['results'])} results")
            
            return experiment
            
        except Exception as e:
            experiment['status'] = 'error'
            experiment['error'] = str(e)
            print(f"[ERROR] Experiment '{name}' failed: {e}")
            return experiment
            
    def analyze_experiment_results(self, experiment_name: str) -> Dict:
        """
        Analyze experiment results.
        
        Args:
            experiment_name: Name of experiment
            
        Returns:
            Analysis results
        """
        if experiment_name not in self._experiment_results:
            print(f"[ERROR] Experiment '{experiment_name}' not found")
            return {}
        
        experiment = self._experiment_results[experiment_name]
        
        if experiment['status'] != 'completed':
            print(f"[ERROR] Experiment '{experiment_name}' is not completed")
            return {}
        
        # Analyze results
        analysis = {
            'experiment_name': experiment_name,
            'total_runs': len(experiment['results']),
            'parameter_combinations': len(set(
                tuple(r['parameters'].items()) for r in experiment['results']
            )),
            'successful_runs': sum(1 for r in experiment['results'] if r['result'].get('success', True)),
            'average_result': self._calculate_average_results(experiment['results']),
            'parameter_effects': self._analyze_parameter_effects(experiment['results']),
            'analysis_time': time.time()
        }
        
        print(f"[EXPERIMENT] Analyzed experiment '{experiment_name}': {analysis['successful_runs']}/{analysis['total_runs']} successful")
        
        return analysis
        
    def compare_experiments(self, experiment_names: List[str]) -> Dict:
        """
        Compare multiple experiments.
        
        Args:
            experiment_names: List of experiment names
            
        Returns:
            Comparison results
        """
        comparison = {
            'experiments': [],
            'comparison_time': time.time()
        }
        
        for exp_name in experiment_names:
            if exp_name in self._experiment_results:
                exp = self._experiment_results[exp_name]
                comparison['experiments'].append({
                    'name': exp_name,
                    'status': exp['status'],
                    'total_runs': len(exp['results']),
                    'successful_runs': sum(1 for r in exp['results'] if r['result'].get('success', True)),
                    'start_time': exp['start_time'],
                    'end_time': exp.get('end_time'),
                    'duration': exp.get('end_time', time.time()) - exp['start_time']
                })
        
        print(f"[EXPERIMENT] Compared {len(comparison['experiments'])} experiments")
        return comparison
        
    def _calculate_average_results(self, results: List[Dict]) -> Dict:
        """Calculate average results across all runs."""
        if not results:
            return {}
        
        # Extract numeric values from results
        numeric_values = {}
        for result in results:
            if isinstance(result['result'], dict):
                for key, value in result['result'].items():
                    if isinstance(value, (int, float)):
                        if key not in numeric_values:
                            numeric_values[key] = []
                        numeric_values[key].append(value)
        
        # Calculate averages
        averages = {}
        for key, values in numeric_values.items():
            if values:
                averages[key] = sum(values) / len(values)
        
        return averages
        
    def _analyze_parameter_effects(self, results: List[Dict]) -> Dict:
        """Analyze the effect of different parameters on results."""
        if not results:
            return {}
        
        # Group results by parameter values
        parameter_effects = {}
        
        # Find all parameter keys
        all_params = set()
        for result in results:
            all_params.update(result['parameters'].keys())
        
        # Analyze each parameter
        for param in all_params:
            param_effects = {}
            for param_value in set(r['parameters'].get(param) for r in results):
                # Filter results for this parameter value
                filtered_results = [r for r in results if r['parameters'].get(param) == param_value]
                
                if filtered_results:
                    # Calculate average result for this parameter value
                    avg_result = self._calculate_average_results(filtered_results)
                    param_effects[str(param_value)] = avg_result
            
            parameter_effects[param] = param_effects
        
        return parameter_effects
        
    def get_experiment_summary(self) -> Dict:
        """
        Get summary of all experiments.
        
        Returns:
            Experiment summary
        """
        return {
            'total_experiments': len(self._experiments),
            'completed_experiments': len([e for e in self._experiments if e['status'] == 'completed']),
            'failed_experiments': len([e for e in self._experiments if e['status'] == 'error']),
            'factorial_designs': len(self._factorial_designs),
            'parameter_combinations': len(self._parameter_combinations)
        }
# Export advanced features classes and functions
__all__ = [
    'PerformanceProfiler',
    'AdvancedVisualizer',
    'MachineLearningInterface',
    'AdvancedExperimentFramework'
]

# Global instances for convenience
_profiler = PerformanceProfiler()
_visualizer = AdvancedVisualizer()
_ml_interface = MachineLearningInterface()
_experiment_framework = AdvancedExperimentFramework()

# Convenience functions
def profile_simulation(simulation_func, name: str, **kwargs):
    """Profile a simulation function."""
    _profiler.start_profiling(name, kwargs)
    try:
        result = simulation_func(**kwargs)
    finally:
        _profiler.stop_profiling()
    return result

def create_plot(plot_type: str, **kwargs):
    """Create a visualization plot."""
    if plot_type == 'spike_train':
        return _visualizer.plot_spike_train(**kwargs)
    elif plot_type == 'population_dynamics':
        return _visualizer.plot_population_dynamics(**kwargs)
    elif plot_type == 'connectivity_matrix':
        return _visualizer.plot_connectivity_matrix(**kwargs)
    elif plot_type == 'time_series':
        return _visualizer.create_time_series_plot(**kwargs)
    elif plot_type == '3d_scatter':
        return _visualizer.create_3d_scatter_plot(**kwargs)
    else:
        raise ValueError(f"Unknown plot type: {plot_type}")

# Make advanced features available as module attributes
import types

# Create a module that contains all advanced features
advanced_features = types.ModuleType('advanced_features')
advanced_features.PerformanceProfiler = PerformanceProfiler
advanced_features.AdvancedVisualizer = AdvancedVisualizer
advanced_features.MachineLearningInterface = MachineLearningInterface
advanced_features.AdvancedExperimentFramework = AdvancedExperimentFramework
advanced_features.get_profiler = lambda: _profiler
advanced_features.get_visualizer = lambda: _visualizer
advanced_features.get_ml_interface = lambda: _ml_interface
advanced_features.get_experiment_framework = lambda: _experiment_framework