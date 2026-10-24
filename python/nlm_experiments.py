#!/usr/bin/env python3
"""
NLM Experiment Management System

A comprehensive system for managing and running experiments with the NLM neural simulation framework.
Provides advanced experiment control, result analysis, and data management capabilities.
"""

import json
import pickle
import numpy as np
import pandas as pd
from datetime import datetime
from typing import List, Dict, Any, Optional, Callable, Union
from enum import Enum
import threading
import time
from collections import defaultdict, deque
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

# Add the python directory to the path
import sys
sys.path.insert(0, str(Path(__file__).parent))

from nlm_py import (
    NLMConfig, NLMBrain, NLMExperiment, NLMAnalyzer,
    NLMVisualizer, PerformanceMonitor
)
class ExperimentType(Enum):
    """Types of experiments supported."""
    CONNECTIVITY = "connectivity"
    PLASTICITY = "plasticity"
    STDP_VERIFICATION = "stdp_verification"
    BEHAVIORAL = "behavioral"
    DEVELOPMENTAL = "developmental"
    NOVELTY = "novelty"
    REWARD = "reward"
    PREDICTION = "prediction"
    FULL_INTEGRATION = "full_integration"
class ExperimentStatus(Enum):
    """Status of an experiment."""
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    PAUSED = "paused"
class NLMExperimentSpec:
    """Specification for an experiment."""
    
    def __init__(self, name: str, exp_type: ExperimentType, config: Optional[NLMConfig] = None):
        self.name = name
        self.type = exp_type
        self.config = config or NLMConfig()
        self.parameters = {}
        self.objectives = {}
        self.measurement_metrics = []
        self.target_values = {}
        self.created_at = datetime.now()
        self.description = ""
        self.tags = []
        
    def add_parameter(self, name: str, value: Any, description: str = ""):
        """Add a parameter to the experiment."""
        self.parameters[name] = {
            'value': value,
            'description': description,
            'type': type(value).__name__
        }
        
    def set_objective(self, metric: str, target: float, tolerance: float = 0.1):
        """Set a success objective for the experiment."""
        self.objectives[metric] = {
            'target': target,
            'tolerance': tolerance,
            'actual': None,
            'achieved': False
        }
        
    def add_measurement_metric(self, metric: str, function: Callable):
        """Add a metric to be measured during experiment."""
        self.measurement_metrics.append({
            'name': metric,
            'function': function,
            'unit': 'dimensionless',
            'higher_better': True
        })
class NLMExperimentResult:
    """Results of an experiment run."""
    
    def __init__(self, experiment_spec: NLMExperimentSpec):
        self.experiment_spec = experiment_spec
        self.status = ExperimentStatus.PENDING
        self.start_time: Optional[datetime] = None
        self.end_time: Optional[datetime] = None
        self.duration: Optional[float] = None
        
        # Data collection
        self.time_series_data = []
        self.final_brain_state: Optional[Any] = None
        self.metrics_history = []
        
        # Results
        self.computed_metrics = {}
        self.analysis_results = {}
        self.passed_objectives = {}
        self.failed_objectives = {}
        
        # Files
        self.result_files = []
        
    def add_time_series_point(self, step: int, timestamp: float, data: Dict[str, Any]):
        """Add a time series data point."""
        self.time_series_data.append({
            'step': step,
            'timestamp': timestamp,
            'data': data
        })
        
    def add_metric_measurement(self, step: int, metric_name: str, value: Any):
        """Add a metric measurement."""
        self.metrics_history.append({
            'step': step,
            'metric': metric_name,
            'value': value,
            'timestamp': time.time()
        })
        
    def complete(self, final_brain_state: Any = None):
        """Mark experiment as completed."""
        self.status = ExperimentStatus.COMPLETED
        self.end_time = datetime.now()
        self.final_brain_state = final_brain_state
        self.compute_final_metrics()
        
    def fail(self, error_message: str):
        """Mark experiment as failed."""
        self.status = ExperimentStatus.FAILED
        self.end_time = datetime.now()
        self.error_message = error_message
        
    def compute_final_metrics(self):
        """Compute final metrics based on objectives and measurements."""
        for metric_name, objective in self.experiment_spec.objectives.items():
            # Get final value for metric
            final_values = [m['value'] for m in self.metrics_history 
                          if m['metric'] == metric_name]
            
            if final_values:
                final_value = final_values[-1]
                objective['actual'] = final_value
                objective['achieved'] = (
                    abs(final_value - objective['target']) <= objective['tolerance']
                )
                
                if objective['achieved']:
                    self.passed_objectives[metric_name] = final_value
                else:
                    self.failed_objectives[metric_name] = {
                        'actual': final_value,
                        'target': objective['target'],
                        'tolerance': objective['tolerance']
                    }
                
                self.computed_metrics[metric_name] = final_value
                
    def save_results(self, filepath: str, format: str = 'json'):
        """Save experiment results to file."""
        if format == 'json':
            data = self.to_dict()
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        elif format == 'pickle':
            with open(filepath, 'wb') as f:
                pickle.dump(self, f)
                
        self.result_files.append(filepath)
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert experiment results to dictionary."""
        return {
            'experiment_spec': {
                'name': self.experiment_spec.name,
                'type': self.experiment_spec.type.value,
                'parameters': self.experiment_spec.parameters,
                'objectives': self.experiment_spec.objectives,
                'measurement_metrics': self.experiment_spec.measurement_metrics,
                'description': self.experiment_spec.description,
                'tags': self.experiment_spec.tags,
                'created_at': self.experiment_spec.created_at.isoformat()
            },
            'status': self.status.value,
            'start_time': self.start_time.isoformat() if self.start_time else None,
            'end_time': self.end_time.isoformat() if self.end_time else None,
            'duration': self.duration,
            'metrics_history': self.metrics_history,
            'time_series_data': self.time_series_data,
            'computed_metrics': self.computed_metrics,
            'passed_objectives': self.passed_objectives,
            'failed_objectives': self.failed_objectives,
            'analysis_results': self.analysis_results,
            'result_files': self.result_files
        }
class NLMExperimentRunner:
    """Runner for managing and executing experiments."""
    
    def __init__(self, experiment_dir: str = "experiments"):
        self.experiment_dir = Path(experiment_dir)
        self.experiment_dir.mkdir(exist_ok=True)
        self.running_experiments = {}
        self.completed_experiments = []
        self.failed_experiments = []
        self.experiment_history = []
        
    def create_experiment(self, spec: NLMExperimentSpec) -> str:
        """Create and register an experiment."""
        exp_id = f"{spec.type.value}_{spec.name}_{int(time.time())}"
        
        # Save experiment specification
        spec_file = self.experiment_dir / f"{exp_id}_spec.json"
        spec_dict = {
            'name': spec.name,
            'type': spec.type.value,
            'config': spec.config.to_dict(),
            'parameters': spec.parameters,
            'objectives': spec.objectives,
            'description': spec.description,
            'tags': spec.tags,
            'created_at': spec.created_at.isoformat()
        }
        
        with open(spec_file, 'w') as f:
            json.dump(spec_dict, f, indent=2)
            
        return exp_id
        
    def run_experiment(self, exp_id: str) -> NLMExperimentResult:
        """Run an experiment by ID."""
        spec_file = self.experiment_dir / f"{exp_id}_spec.json"
        
        if not spec_file.exists():
            raise FileNotFoundError(f"Experiment specification not found: {exp_id}")
            
        # Load experiment specification
        with open(spec_file, 'r') as f:
            spec_dict = json.load(f)
            
        # Create experiment specification
        config = NLMConfig(spec_dict.get('config', {}))
        spec = NLMExperimentSpec(
            name=spec_dict['name'],
            exp_type=ExperimentType(spec_dict['type']),
            config=config
        )
        
        spec.parameters = spec_dict.get('parameters', {})
        spec.objectives = spec_dict.get('objectives', {})
        spec.description = spec_dict.get('description', '')
        spec.tags = spec_dict.get('tags', [])
        spec.created_at = datetime.fromisoformat(spec_dict['created_at'])
        
        # Create brain and run experiment
        brain = NLMBrain(config)
        result = NLMExperimentResult(spec)
        result.status = ExperimentStatus.RUNNING
        result.start_time = datetime.now()
        
        self.running_experiments[exp_id] = result
        
        try:
            # Run experiment based on type
            if spec.type == ExperimentType.CONNECTIVITY:
                self._run_connectivity_experiment(brain, result)
            elif spec.type == ExperimentType.PLASTICITY:
                self._run_plasticity_experiment(brain, result)
            elif spec.type == ExperimentType.STDP_VERIFICATION:
                self._run_stdp_experiment(brain, result)
            else:
                self._run_generic_experiment(brain, result)
                
            result.complete(brain.save_state())
            self.completed_experiments.append(exp_id)
            
        except Exception as e:
            result.fail(str(e))
            self.failed_experiments.append(exp_id)
            raise
            
        finally:
            del self.running_experiments[exp_id]
            self.experiment_history.append(result)
            
            # Save results
            result_file = self.experiment_dir / f"{exp_id}_result.json"
            result.save_results(str(result_file))
            
        return result
        
    def _run_connectivity_experiment(self, brain: NLMBrain, result: NLMExperimentResult):
        """Run connectivity experiment."""
        steps = result.experiment_spec.parameters.get('steps', 100)
        
        brain.initialize()
        
        for step in range(steps):
            if brain._brain:
                brain._brain.step(step, step * 0.001)
            stats = brain.get_stats()
            result.add_time_series_point(step, step * 0.001, stats)
            
            # Record key metrics
            for metric in result.experiment_spec.measurement_metrics:
                if metric['name'] in stats:
                    result.add_metric_measurement(step, metric['name'], stats[metric['name']])
                    
    def _run_plasticity_experiment(self, brain: NLMBrain, result: NLMExperimentResult):
        """Run plasticity experiment."""
        steps = result.experiment_spec.parameters.get('steps', 1000)
        
        brain.reset()
        brain.initialize()
        
        # Enable plasticity
        if brain._brain:
            for region in brain._brain.getRegions():
                for synapse in region.getSynapses():
                    synapse.enablePlasticity(True, True, False)
                    
        initial_stats = brain.get_stats()
        
        for step in range(steps):
            if brain._brain:
                brain._brain.injectCurrentToNeurons(
                    from nlm_py import NeuronType.Sensory, 30.0f)
                brain._brain.step(step, step * 0.001)
                
            stats = brain.get_stats()
            result.add_time_series_point(step, step * 0.001, stats)
            
            # Record key metrics
            for metric in result.experiment_spec.measurement_metrics:
                if metric['name'] in stats:
                    result.add_metric_measurement(step, metric['name'], stats[metric['name']])
                    
    def _run_stdp_experiment(self, brain: NLMBrain, result: NLMExperimentResult):
        """Run STDP verification experiment."""
        trials = result.experiment_spec.parameters.get('trials', 50)
        
        brain.reset()
        brain.initialize()
        
        # Create correlated activity
        for trial in range(trials):
            neurons = brain.get_neurons()
            if len(neurons) >= 2:
                pre_neuron = neurons[0]
                post_neuron = neurons[1]
                
                if brain._brain:
                    pre_neuron._neuron.injectCurrent(60.0f)
                    brain._brain.step(trial * 2, trial * 2 * 0.001)
                    
                    post_neuron._neuron.injectCurrent(60.0f)
                    brain._brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
                    
            stats = brain.get_stats()
            result.add_time_series_point(trial * 2, trial * 2 * 0.001, stats)
            
    def _run_generic_experiment(self, brain: NLMBrain, result: NLMExperimentResult):
        """Run generic experiment."""
        steps = result.experiment_spec.parameters.get('steps', 500)
        timestep = result.experiment_spec.parameters.get('timestep', 0.001)
        
        brain.initialize()
        
        for step in range(steps):
            if brain._brain:
                brain._brain.step(step, step * timestep)
            stats = brain.get_stats()
            result.add_time_series_point(step, step * timestep, stats)
            
    def get_experiment_status(self, exp_id: str) -> Dict[str, Any]:
        """Get status of an experiment."""
        spec_file = self.experiment_dir / f"{exp_id}_spec.json"
        result_file = self.experiment_dir / f"{exp_id}_result.json"
        
        status = {
            'experiment_id': exp_id,
            'spec_file': str(spec_file.exists()),
            'has_results': str(result_file.exists())
        }
        
        if exp_id in self.running_experiments:
            status['status'] = 'running'
        elif exp_id in self.completed_experiments:
            status['status'] = 'completed'
        elif exp_id in self.failed_experiments:
            status['status'] = 'failed'
        else:
            status['status'] = 'unknown'
            
        return status
class NLMExperimentAnalyzer:
    """Advanced analysis of experiment results."""
    
    def analyze_spike_patterns(self, experiment_result: NLMExperimentResult) -> Dict[str, Any]:
        """Analyze spike patterns in experiment results."""
        spike_data = []
        
        for time_point in experiment_result.time_series_data:
            stats = time_point['data']
            if 'total_spike_count' in stats:
                spike_data.append(stats['total_spike_count'])
                
        if not spike_data:
            return {'error': 'No spike data available'}
            
        analyzer = NLMAnalyzer()
        spike_stats = analyzer.calculate_spike_statistics(spike_data)
        
        return {
            'total_spikes': sum(spike_data),
            'spike_statistics': spike_stats,
            'spike_trend': self._calculate_trend(spike_data),
            'peak_spike_activity': max(spike_data),
            'min_spike_activity': min(spike_data),
            'coefficient_of_variation': spike_stats['std'] / spike_stats['mean'] if spike_stats['mean'] > 0 else 0
        }
        
    def _calculate_trend(self, data: List[float]) -> Dict[str, float]:
        """Calculate trend in time series data."""
        if len(data) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'r_squared': 0.0}
            
        x = list(range(len(data)))
        y = data
        
        n = len(x)
        sum_x = sum(x)
        sum_y = sum(y)
        sum_xx = sum(xi * xi for xi in x)
        sum_yy = sum(yi * yi for yi in y)
        sum_xy = sum(xi * yi for xi, yi in zip(x, y))
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x)
        intercept = (sum_y - slope * sum_x) / n
        
        ss_res = sum((yi - (slope * xi + intercept))**2 for xi, yi in zip(x, y))
        ss_tot = sum((yi - sum_y / n)**2 for yi in y)
        
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_squared
        }
        
    def detect_patterns(self, experiment_result: NLMExperimentResult, 
                       threshold: float = 0.1) -> Dict[str, Any]:
        """Detect patterns in experiment data."""
        analyzer = NLMAnalyzer()
        pattern_results = []
        
        # Analyze time series data
        for time_point in experiment_result.time_series_data:
            stats = time_point['data']
            
            # Look for significant changes
            for metric_name, metric_value in stats.items():
                if isinstance(metric_value, (int, float)):
                    pattern = analyzer.detect_patterns(
                        list(stats.values()), threshold
                    )
                    pattern_results.extend(pattern)
                    
        return {
            'patterns': pattern_results,
            'total_patterns': len(pattern_results),
            'significant_changes': len([p for p in pattern_results if p['change'] != 0])
        }
class NLMExperimentVisualizer:
    """Visualization tools for experiment results."""
    
    @staticmethod
    def create_time_series_plots(experiment_result: NLMExperimentResult, 
                                output_dir: str = "plots"):
        """Create time series plots for experiment results."""
        output_path = Path(output_dir)
        output_path.mkdir(exist_ok=True)
        
        if not experiment_result.time_series_data:
            return
            
        # Prepare data
        steps = [point['step'] for point in experiment_result.time_series_data]
        timestamps = [point['timestamp'] for point in experiment_result.time_series_data]
        
        # Plot key metrics
        metrics_to_plot = ['total_spike_count', 'active_neurons', 'firing_neurons', 
                          'average_firing_rate', 'excitation_inhibition_ratio']
        
        for metric in metrics_to_plot:
            values = []
            for point in experiment_result.time_series_data:
                stats = point['data']
                if metric in stats:
                    values.append(stats[metric])
                else:
                    values.append(None)
                    
            if values and any(v is not None for v in values):
                NLMExperimentVisualizer._plot_metric(
                    steps, values, metric, output_path
                )
                
    @staticmethod
    def _plot_metric(steps: List[int], values: List[float], metric_name: str, output_path: Path):
        """Plot a single metric."""
        plt.figure(figsize=(12, 6))
        
        # Filter out None values
        valid_data = [(s, v) for s, v in zip(steps, values) if v is not None]
        if not valid_data:
            return
            
        plot_steps, plot_values = zip(*valid_data)
        
        plt.plot(plot_steps, plot_values, linewidth=2)
        plt.title(f'{metric_name.replace("_", " ").title()} Over Time')
        plt.xlabel('Step')
        plt.ylabel(metric_name.replace('_', ' ').title())
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        
        # Save plot
        filename = f"{metric_name}_over_time.png"
        plt.savefig(output_path / filename, dpi=150)
        plt.close()
        
    @staticmethod
    def create_summary_dashboard(experiment_result: NLMExperimentResult,
                                 output_path: str = "dashboard.html"):
        """Create an HTML dashboard with experiment summary."""
        
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Experiment Dashboard - {experiment_result.experiment_spec.name}</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .metrics {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }}
                .metric-card {{ background-color: #e8f4f8; padding: 15px; border-radius: 5px; text-align: center; }}
                .metric-value {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
                .metric-label {{ color: #7f8c8d; font-size: 14px; }}
                .results {{ margin: 20px 0; }}
                table {{ width: 100%; border-collapse: collapse; }}
                th, td {{ padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }}
                th {{ background-color: #f8f9fa; }}
                .success {{ color: #27ae60; }}
                .failed {{ color: #e74c3c; }}
                .pending {{ color: #f39c12; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Experiment Dashboard</h1>
                <h2>{experiment_result.experiment_spec.name}</h2>
                <p>Type: {experiment_result.experiment_spec.type.value}</p>
                <p>Duration: {experiment_result.duration:.2f} seconds</p>
                <p>Status: <span class="{experiment_result.status.value}">{experiment_result.status.value.upper()}</span></p>
            </div>
            
            <div class="metrics">
                <div class="metric-card">
                    <div class="metric-value">{experiment_result.experiment_spec.type.value.upper()}</div>
                    <div class="metric-label">Experiment Type</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{len(experiment_result.time_series_data)}</div>
                    <div class="metric-label">Time Points</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{len(experiment_result.passed_objectives)}</div>
                    <div class="metric-label">Passed Objectives</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{len(experiment_result.failed_objectives)}</div>
                    <div class="metric-label">Failed Objectives</div>
                </div>
            </div>
            
            <div class="results">
                <h3>Experiment Results</h3>
                <table>
                    <tr>
                        <th>Metric</th>
                        <th>Value</th>
                        <th>Target</th>
                        <th>Status</th>
                    </tr>
                    {self._create_html_table_rows(experiment_result)}
                </table>
            </div>
            
            <div class="results">
                <h3>Analysis Results</h3>
                <pre>{json.dumps(experiment_result.analysis_results, indent=2)}</pre>
            </div>
        </body>
        </html>
        """
        
        with open(output_path, 'w') as f:
            f.write(html_content)
            
    @staticmethod
    def _create_html_table_rows(experiment_result: NLMExperimentResult) -> str:
        """Create HTML table rows for experiment results."""
        rows = []
        
        # Add computed metrics
        for metric_name, value in experiment_result.computed_metrics.items():
            objective = experiment_result.experiment_spec.objectives.get(metric_name, {})
            target = objective.get('target', 'N/A')
            tolerance = objective.get('tolerance', 'N/A')
            
            status = 'Unknown'
            if metric_name in experiment_result.passed_objectives:
                status = '<span class="success">✓ PASSED</span>'
            elif metric_name in experiment_result.failed_objectives:
                status = '<span class="failed">✗ FAILED</span>'
                
            rows.append(f"""
                <tr>
                    <td>{metric_name}</td>
                    <td>{value:.4f if isinstance(value, float) else value}</td>
                    <td>{target} (±{tolerance})</td>
                    <td>{status}</td>
                </tr>
            """)
            
        return '\n'.join(rows) if rows else '<tr><td colspan="4">No results available</td></tr>'
class NLMDataManager:
    """Manage experiment data storage and retrieval."""
    
    def __init__(self, data_dir: str = "experiment_data"):
        self.data_dir = Path(data_dir)
        self.data_dir.mkdir(exist_ok=True)
        
    def save_experiment_data(self, experiment_result: NLMExperimentResult):
        """Save experiment data in multiple formats."""
        # Create subdirectory for this experiment
        exp_dir = self.data_dir / experiment_result.experiment_spec.name.replace(' ', '_')
        exp_dir.mkdir(exist_ok=True)
        
        # Save time series data as CSV
        if experiment_result.time_series_data:
            df_data = []
            for point in experiment_result.time_series_data:
                row = {'step': point['step'], 'timestamp': point['timestamp']}
                row.update(point['data'])
                df_data.append(row)
                
            df = pd.DataFrame(df_data)
            csv_path = exp_dir / f"{experiment_result.experiment_spec.name}_time_series.csv"
            df.to_csv(csv_path, index=False)
            
        # Save metrics history as CSV
        if experiment_result.metrics_history:
            metrics_df = pd.DataFrame(experiment_result.metrics_history)
            csv_path = exp_dir / f"{experiment_result.experiment_spec.name}_metrics.csv"
            metrics_df.to_csv(csv_path, index=False)
            
        # Save experiment specification
        spec_path = exp_dir / f"{experiment_result.experiment_spec.name}_spec.json"
        with open(spec_path, 'w') as f:
            json.dump(experiment_result.experiment_spec.__dict__, f, indent=2, default=str)
            
        # Save experiment result
        result_path = exp_dir / f"{experiment_result.experiment_spec.name}_result.json"
        experiment_result.save_results(str(result_path))
        
    def load_experiment_data(self, experiment_name: str) -> NLMExperimentResult:
        """Load experiment data from files."""
        exp_dir = self.data_dir / experiment_name.replace(' ', '_')
        
        if not exp_dir.exists():
            raise FileNotFoundError(f"Experiment data not found: {experiment_name}")
            
        # Load specification
        spec_path = exp_dir / f"{experiment_name}_spec.json"
        with open(spec_path, 'r') as f:
            spec_dict = json.load(f)
            
        # Load time series data
        time_series_path = exp_dir / f"{experiment_name}_time_series.csv"
        if time_series_path.exists():
            time_series_df = pd.read_csv(time_series_path)
            time_series_data = []
            for _, row in time_series_df.iterrows():
                time_series_data.append({
                    'step': int(row['step']),
                    'timestamp': float(row['timestamp']),
                    'data': row.drop(['step', 'timestamp']).to_dict()
                })
        else:
            time_series_data = []
            
        # Load metrics data
        metrics_path = exp_dir / f"{experiment_name}_metrics.csv"
        if metrics_path.exists():
            metrics_df = pd.read_csv(metrics_path)
            metrics_history = []
            for _, row in metrics_df.iterrows():
                metrics_history.append({
                    'step': int(row['step']),
                    'metric': row['metric'],
                    'value': float(row['value']) if row['metric'].isdigit() or '.' in row['value'] else row['value'],
                    'timestamp': float(row['timestamp'])
                })
        else:
            metrics_history = []
            
        # Create experiment result
        from nlm_py import NLMExperimentSpec, ExperimentType
        spec = NLMExperimentSpec(
            name=spec_dict['name'],
            exp_type=ExperimentType(spec_dict['type']),
            config=NLMConfig(spec_dict.get('config', {}))
        )
        
        result = NLMExperimentResult(spec)
        result.time_series_data = time_series_data
        result.metrics_history = metrics_history
        result.status = ExperimentStatus.COMPLETED
        result.final_brain_state = None  # Would need to implement state loading
        
        return result
def create_sample_experiment() -> NLMExperimentSpec:
    """Create a sample experiment for demonstration."""
    spec = NLMExperimentSpec(
        name="Sample Connectivity Test",
        exp_type=ExperimentType.CONNECTIVITY
    )
    
    spec.add_parameter('steps', 100, 'Number of simulation steps')
    spec.add_parameter('timestep', 0.001, 'Simulation timestep')
    
    spec.set_objective('total_spike_count', 50, 20)
    spec.set_objective('average_firing_rate', 5.0, 2.0)
    
    return spec
def main():
    """Example usage of the experiment management system."""
    # Create experiment runner
    runner = NLMExperimentRunner()
    
    # Create a sample experiment
    experiment_spec = create_sample_experiment()
    
    # Run the experiment
    exp_id = runner.create_experiment(experiment_spec)
    print(f"Created experiment with ID: {exp_id}")
    
    # Run the experiment
    result = runner.run_experiment(exp_id)
    print(f"Experiment completed with status: {result.status.value}")
    
    # Create analysis
    analyzer = NLMExperimentAnalyzer()
    spike_analysis = analyzer.analyze_spike_patterns(result)
    print(f"Spike analysis: {spike_analysis}")
    
    # Create visualizations
    output_path = f"experiments/{exp_id}_dashboard.html"
    NLMExperimentVisualizer.create_summary_dashboard(result, output_path)
    print(f"Dashboard created: {output_path}")
    
    # Save data
    data_manager = NLMDataManager()
    data_manager.save_experiment_data(result)
    print("Experiment data saved")
if __name__ == "__main__":
    main()