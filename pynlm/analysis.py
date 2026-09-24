# Analysis and visualization tools for NLM

"""
Analysis and visualization tools for NLM (Neural Learning Machine).

This module provides tools for analyzing simulation results, generating visualizations,
and extracting insights from neural network behavior.
"""

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import json
import time
from typing import List, Dict, Optional, Any, Tuple
from enum import Enum
import warnings

warnings.filterwarnings('ignore')

from ._cpp import Brain, Neuron, Synapse, Region, Population
from ._cpp import NeuronType, SynapseType, DevelopmentalStage
from ._cpp import SensoryPercept, Action, ActionResult
from ._cpp import SimpleWorld, WorldObject, WorldObjectType

__all__ = [
    'SimulationAnalyzer', 'ResultsProcessor', 'Visualizer', 'PlotGenerator',
    'StatisticsCalculator', 'PatternDetector', 'PerformanceProfiler',
    'ActivityAnalyzer', 'ConnectivityAnalyzer', 'LearningAnalyzer',
    'createAnalyzer', 'createProcessor', 'createVisualizer',
    'createPlotter', 'createStatistics', 'createPatternDetector',
    'createProfiler', 'createActivityAnalyzer', 'createConnectivityAnalyzer',
    'createLearningAnalyzer',
]


class SimulationAnalyzer:
    """
    Analyzer for simulation data and results.

    This class provides comprehensive analysis of simulation data including
    statistical analysis, pattern detection, and performance evaluation.
    """

    def __init__(self, results: List[Dict]):
        """
        Initialize analyzer with simulation results.

        Args:
            results: List of simulation result dictionaries.
        """
        self.results = results
        self.analysis_results = {}
        self.processed_data = {}

    def analyze(self, analysis_type: str = 'comprehensive') -> Dict:
        """
        Analyze simulation results.

        Args:
            analysis_type: Type of analysis ('comprehensive', 'basic', 'detailed').

        Returns:
            Dictionary containing analysis results.
        """
        if not self.results:
            return {'error': 'No results to analyze'}

        self.analysis_results = {
            'total_steps': len(self.results),
            'analysis_type': analysis_type,
            'timestamp': time.time(),
        }

        if analysis_type == 'comprehensive':
            self._comprehensive_analysis()
        elif analysis_type == 'basic':
            self._basic_analysis()
        elif analysis_type == 'detailed':
            self._detailed_analysis()

        return self.analysis_results

    def _comprehensive_analysis(self):
        """Perform comprehensive analysis."""
        # Basic statistics
        self._calculate_basic_statistics()

        # Pattern detection
        self._detect_patterns()

        # Temporal analysis
        self._analyze_temporal_dynamics()

        # Cross-correlation analysis
        self._analyze_cross_correlations()

        # Anomaly detection
        self._detect_anomalies()

    def _basic_analysis(self):
        """Perform basic analysis."""
        self._calculate_basic_statistics()

    def _detailed_analysis(self):
        """Perform detailed analysis."""
        self._comprehensive_analysis()
        self._perform_machine_learning_analysis()
        self._analyze_neural_dynamics()

    def _calculate_basic_statistics(self):
        """Calculate basic statistics."""
        stats = {}

        # Numerical statistics
        for key in ['firing_rate', 'total_spikes', 'firing_neurons',
                    'active_neurons', 'average_firing_rate',
                    'excitatory_inhibitory_ratio']:
            values = [r.get(key) for r in self.results if key in r and r[key] is not None]
            if values:
                stats[f'{key}_mean'] = np.mean(values)
                stats[f'{key}_std'] = np.std(values)
                stats[f'{key}_min'] = np.min(values)
                stats[f'{key}_max'] = np.max(values)

        # Derived statistics
        if 'firing_rate' in self.results[0]:
            rates = [r['firing_rate'] for r in self.results]
            if len(rates) > 1:
                stats['firing_rate_trend'] = np.polyfit(range(len(rates)), rates, 1)[0]

        self.analysis_results['basic_statistics'] = stats

    def _detect_patterns(self):
        """Detect patterns in the data."""
        patterns = {}

        if 'firing_rate' in self.results[0]:
            # Detect periodic patterns
            rates = [r['firing_rate'] for r in self.results]
            autocorr = np.correlate(rates - np.mean(rates), rates - np.mean(rates), mode='full')
            max_lag = len(rates) // 2
            autocorr = autocorr[max_lag:]

            # Find significant peaks in autocorrelation
            threshold = np.mean(autocorr) + 2 * np.std(autocorr)
            peaks = np.where(autocorr > threshold)[0]
            if len(peaks) > 0:
                patterns['periodic_patterns'] = {
                    'lags': peaks.tolist(),
                    'strengths': autocorr[peaks].tolist(),
                    'dominant_period': peaks[np.argmax(autocorr[peaks])]
                }

        # Detect state transitions
        if 'firing_rate' in self.results[0]:
            state_transitions = []
            current_state = np.mean(self.results[0]['firing_rate']) > 1.0

            for step, result in enumerate(self.results[1:], 1):
                new_state = np.mean(result['firing_rate']) > 1.0
                if new_state != current_state:
                    state_transitions.append({
                        'step': step,
                        'from_state': 'high' if current_state else 'low',
                        'to_state': 'high' if new_state else 'low'
                    })
                    current_state = new_state

            patterns['state_transitions'] = state_transitions

        self.analysis_results['patterns'] = patterns

    def _analyze_temporal_dynamics(self):
        """Analyze temporal dynamics."""
        if not self.results:
            return

        dynamics = {}

        # Calculate temporal derivatives
        if 'firing_rate' in self.results[0]:
            rates = [r['firing_rate'] for r in self.results]
            derivatives = np.diff(rates)

            dynamics['rate_of_change'] = {
                'mean': np.mean(derivatives),
                'std': np.std(derivatives),
                'max_increase': np.max(derivatives),
                'max_decrease': np.min(derivatives),
                'transitions': np.sum(np.abs(derivatives) > 0.1)
            }

        # Calculate volatility
        if 'firing_rate' in self.results[0]:
            rates = [r['firing_rate'] for r in self.results]
            volatility = np.std(rates) / np.mean(rates) if np.mean(rates) > 0 else 0.0
            dynamics['volatility'] = volatility

        self.analysis_results['temporal_dynamics'] = dynamics

    def _analyze_cross_correlations(self):
        """Analyze cross-correlations between variables."""
        if not self.results:
            return

        correlations = {}
        numeric_keys = [key for key in self.results[0].keys() if key not in ['step']]

        for i, key1 in enumerate(numeric_keys):
            for key2 in numeric_keys[i + 1:]:
                values1 = [r.get(key1) for r in self.results if r.get(key1) is not None]
                values2 = [r.get(key2) for r in self.results if r.get(key2) is not None]

                if len(values1) == len(values2) and len(values1) > 1:
                    correlation = np.corrcoef(values1, values2)[0, 1]
                    if not np.isnan(correlation):
                        correlations[f'{key1}_{key2}'] = correlation

        self.analysis_results['cross_correlations'] = correlations

    def _detect_anomalies(self):
        """Detect anomalies in the data."""
        anomalies = {}

        if 'firing_rate' in self.results[0]:
            rates = [r['firing_rate'] for r in self.results]
            mean_rate = np.mean(rates)
            std_rate = np.std(rates)

            # Detect outliers using z-score
            z_scores = [(rate - mean_rate) / std_rate for rate in rates if std_rate > 0]
            outliers = [i for i, z in enumerate(z_scores) if abs(z) > 3.0]

            anomalies['outliers'] = {
                'indices': outliers,
                'values': [rates[i] for i in outliers],
                'z_scores': z_scores,
                'count': len(outliers)
            }

        self.analysis_results['anomalies'] = anomalies

    def _perform_machine_learning_analysis(self):
        """Perform machine learning analysis."""
        ml_analysis = {}

        # Prepare data for clustering
        if 'firing_rate' in self.results[0]:
            rates = np.array([r['firing_rate'] for r in self.results]).reshape(-1, 1)

            # Simple clustering using k-means
            try:
                from sklearn.cluster import KMeans

                kmeans = KMeans(n_clusters=3, random_state=42)
                cluster_labels = kmeans.fit_predict(rates)

                ml_analysis['clustering'] = {
                    'n_clusters': 3,
                    'cluster_labels': cluster_labels.tolist(),
                    'cluster_centers': kmeans.cluster_centers_.flatten().tolist()
                }
            except ImportError:
                ml_analysis['clustering'] = {'error': 'scikit-learn not available'}

        self.analysis_results['machine_learning'] = ml_analysis

    def _analyze_neural_dynamics(self):
        """Analyze neural dynamics."""
        neural_dynamics = {}

        # Analyze firing patterns
        if 'firing_rate' in self.results[0]:
            rates = [r['firing_rate'] for r in self.results]

            # Burst detection
            threshold = np.mean(rates) + np.std(rates)
            in_burst = False
            bursts = []

            for step, rate in enumerate(rates):
                if rate > threshold and not in_burst:
                    in_burst = True
                    burst_start = step
                elif rate <= threshold and in_burst:
                    in_burst = False
                    bursts.append((burst_start, step - 1))

            neural_dynamics['bursts'] = {
                'count': len(bursts),
                'durations': [end - start + 1 for start, end in bursts],
                'start_steps': [start for start, _ in bursts],
                'end_steps': [end for _, end in bursts]
            }

        self.analysis_results['neural_dynamics'] = neural_dynamics


class ResultsProcessor:
    """
    Processor for simulation results.

    This class provides tools for processing and transforming simulation data
    for analysis and visualization.
    """

    def __init__(self, results: List[Dict]):
        """
        Initialize results processor.

        Args:
            results: List of simulation result dictionaries.
        """
        self.results = results
        self.processed_results = []

    def normalize(self, method: str = 'zscore'):
        """
        Normalize results data.

        Args:
            method: Normalization method ('zscore', 'minmax', 'robust').

        Returns:
            Processed results.
        """
        if not self.results:
            return []

        processed = []
        numeric_keys = self._get_numeric_keys()

        for result in self.results:
            processed_result = result.copy()

            for key in numeric_keys:
                values = [r.get(key) for r in self.results if r.get(key) is not None]
                if not values:
                    continue

                if method == 'zscore':
                    mean_val = np.mean(values)
                    std_val = np.std(values)
                    if std_val > 0:
                        processed_result[key] = (result[key] - mean_val) / std_val
                    else:
                        processed_result[key] = 0.0

                elif method == 'minmax':
                    min_val = np.min(values)
                    max_val = np.max(values)
                    if max_val > min_val:
                        processed_result[key] = (result[key] - min_val) / (max_val - min_val)
                    else:
                        processed_result[key] = 0.5

                elif method == 'robust':
                    median = np.median(values)
                    mad = np.median(np.abs(values - median))
                    if mad > 0:
                        processed_result[key] = (result[key] - median) / (1.4826 * mad)
                    else:
                        processed_result[key] = 0.0

            processed.append(processed_result)

        self.processed_results = processed
        return processed

    def filter(self, condition: Dict):
        """
        Filter results based on condition.

        Args:
            condition: Dictionary of conditions to filter by.

        Returns:
            Filtered results.
        """
        filtered = []

        for result in self.results:
            matches = True
            for key, value in condition.items():
                if key not in result or result[key] != value:
                    matches = False
                    break

            if matches:
                filtered.append(result)

        return filtered

    def aggregate(self, group_by: str, aggregation_functions: Dict):
        """
        Aggregate results by group.

        Args:
            group_by: Key to group by.
            aggregation_functions: Dictionary of aggregation functions.

        Returns:
            Aggregated results.
        """
        if not self.results:
            return []

        # Group results
        groups = {}
        for result in self.results:
            group_key = result.get(group_by)
            if group_key not in groups:
                groups[group_key] = []
            groups[group_key].append(result)

        # Apply aggregations
        aggregated = []
        for group_key, group_results in groups.items():
            aggregated_result = {group_by: group_key}

            for key, func in aggregation_functions.items():
                values = [r.get(key) for r in group_results if r.get(key) is not None]
                if values:
                    if func == 'mean':
                        aggregated_result[f'{key}_mean'] = np.mean(values)
                    elif func == 'std':
                        aggregated_result[f'{key}_std'] = np.std(values)
                    elif func == 'sum':
                        aggregated_result[f'{key}_sum'] = np.sum(values)
                    elif func == 'min':
                        aggregated_result[f'{key}_min'] = np.min(values)
                    elif func == 'max':
                        aggregated_result[f'{key}_max'] = np.max(values)
                    elif func == 'median':
                        aggregated_result[f'{key}_median'] = np.median(values)

            aggregated.append(aggregated_result)

        return aggregated

    def _get_numeric_keys(self) -> List[str]:
        """Get numeric keys from results."""
        if not self.results:
            return []

        numeric_keys = set()
        for result in self.results:
            for key, value in result.items():
                if isinstance(value, (int, float)) and not key.startswith('step'):
                    numeric_keys.add(key)

        return list(numeric_keys)


class Visualizer:
    """
    Base class for visualization.

    This class provides basic visualization functionality.
    """

    def __init__(self, results: List[Dict]):
        """
        Initialize visualizer.

        Args:
            results: List of simulation result dictionaries.
        """
        self.results = results

    def plot(self, x: str, y: str, output_file: str = None, **kwargs):
        """
        Create a plot.

        Args:
            x: X-axis variable.
            y: Y-axis variable.
            output_file: Output file path (optional).
            **kwargs: Additional plot parameters.

        Returns:
            Matplotlib figure.
        """
        try:
            import matplotlib.pyplot as plt

            fig, ax = plt.subplots(figsize=kwargs.get('figsize', (10, 6)))

            # Extract data
            x_values = [r.get(x) for r in self.results if r.get(x) is not None]
            y_values = [r.get(y) for r in self.results if r.get(y) is not None]

            # Create plot
            ax.plot(x_values, y_values, **kwargs)
            ax.set_xlabel(x)
            ax.set_ylabel(y)
            ax.set_title(f"{y} vs {x}")
            ax.grid(True, alpha=0.3)

            if output_file:
                plt.savefig(output_file, dpi=kwargs.get('dpi', 150), bbox_inches='tight')
                print(f"Plot saved to {output_file}")

            return fig

        except ImportError:
            print("matplotlib not available - cannot create plots")
            return None

    def create_histogram(self, variable: str, output_file: str = None, **kwargs):
        """
        Create a histogram.

        Args:
            variable: Variable to histogram.
            output_file: Output file path (optional).
            **kwargs: Additional histogram parameters.

        Returns:
            Matplotlib figure.
        """
        try:
            import matplotlib.pyplot as plt

            fig, ax = plt.subplots(figsize=kwargs.get('figsize', (10, 6)))

            # Extract data
            values = [r.get(variable) for r in self.results if r.get(variable) is not None]

            # Create histogram
            ax.hist(values, bins=kwargs.get('bins', 50), alpha=0.7, **kwargs)
            ax.set_xlabel(variable)
            ax.set_ylabel('Frequency')
            ax.set_title(f'Distribution of {variable}')
            ax.grid(True, alpha=0.3)

            if output_file:
                plt.savefig(output_file, dpi=kwargs.get('dpi', 150), bbox_inches='tight')
                print(f"Histogram saved to {output_file}")

            return fig

        except ImportError:
            print("matplotlib not available - cannot create plots")
            return None

    def create_scatter(self, x: str, y: str, output_file: str = None, **kwargs):
        """
        Create a scatter plot.

        Args:
            x: X-axis variable.
            y: Y-axis variable.
            output_file: Output file path (optional).
            **kwargs: Additional plot parameters.

        Returns:
            Matplotlib figure.
        """
        try:
            import matplotlib.pyplot as plt

            fig, ax = plt.subplots(figsize=kwargs.get('figsize', (10, 6)))

            # Extract data
            x_values = [r.get(x) for r in self.results if r.get(x) is not None]
            y_values = [r.get(y) for r in self.results if r.get(y) is not None]

            # Create scatter plot
            ax.scatter(x_values, y_values, alpha=0.7, **kwargs)
            ax.set_xlabel(x)
            ax.set_ylabel(y)
            ax.set_title(f"{y} vs {x}")
            ax.grid(True, alpha=0.3)

            if output_file:
                plt.savefig(output_file, dpi=kwargs.get('dpi', 150), bbox_inches='tight')
                print(f"Scatter plot saved to {output_file}")

            return fig

        except ImportError:
            print("matplotlib not available - cannot create plots")
            return None


class PlotGenerator:
    """
    Generate plots for simulation results.

    This class provides methods for generating comprehensive plots
    and visualizations of simulation data.
    """

    def __init__(self, analyzer: SimulationAnalyzer):
        """
        Initialize plot generator.

        Args:
            analyzer: Simulation analyzer instance.
        """
        self.analyzer = analyzer

    def generate_all_plots(self, output_dir: str = 'plots'):
        """
        Generate all available plots.

        Args:
            output_dir: Output directory.

        Returns:
            Dictionary of plot file paths.
        """
        import os
        os.makedirs(output_dir, exist_ok=True)

        plots = {}

        # Generate time series plots
        if 'firing_rate' in self.analyzer.results[0]:
            plots['firing_rate_time_series'] = self._plot_firing_rate_time_series(output_dir)

        # Generate distribution plots
        if 'firing_rate' in self.analyzer.results[0]:
            plots['firing_rate_distribution'] = self._plot_firing_rate_distribution(output_dir)

        # Generate correlation plots
        if 'cross_correlations' in self.analyzer.analysis_results:
            plots['correlation_matrix'] = self._plot_correlation_matrix(output_dir)

        # Generate anomaly plots
        if 'anomalies' in self.analyzer.analysis_results:
            plots['anomaly_detection'] = self._plot_anomalies(output_dir)

        return plots

    def _plot_firing_rate_time_series(self, output_dir: str):
        """Plot firing rate time series."""
        try:
            import matplotlib.pyplot as plt

            fig, axes = plt.subplots(3, 1, figsize=(12, 10))

            # Subplot 1: Firing rate
            steps = [r.get('step') for r in self.analyzer.results]
            rates = [r.get('firing_rate') for r in self.analyzer.results]

            axes[0].plot(steps, rates, 'b-', linewidth=1)
            axes[0].set_xlabel('Step')
            axes[0].set_ylabel('Firing Rate')
            axes[0].set_title('Firing Rate Over Time')
            axes[0].grid(True, alpha=0.3)

            # Subplot 2: Smoothed firing rate
            window_size = min(50, len(rates) // 10)
            if window_size > 1:
                smoothed = np.convolve(rates, np.ones(window_size)/window_size, mode='same')
                axes[1].plot(steps, smoothed, 'r-', linewidth=2)
                axes[1].set_xlabel('Step')
                axes[1].set_ylabel('Smoothed Firing Rate')
                axes[1].set_title(f'Firing Rate (Smoothed, window={window_size})')
                axes[1].grid(True, alpha=0.3)

            # Subplot 3: Rate of change
            if len(rates) > 1:
                rate_of_change = np.diff(rates)
                axes[2].plot(steps[1:], rate_of_change, 'g-', linewidth=1)
                axes[2].set_xlabel('Step')
                axes[2].set_ylabel('Rate of Change')
                axes[2].set_title('Firing Rate Change')
                axes[2].grid(True, alpha=0.3)

            plt.tight_layout()

            output_file = f"{output_dir}/firing_rate_time_series.png"
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()

            return output_file

        except ImportError:
            print("matplotlib not available - cannot create plots")
            return None

    def _plot_firing_rate_distribution(self, output_dir: str):
        """Plot firing rate distribution."""
        try:
            import matplotlib.pyplot as plt

            fig, axes = plt.subplots(2, 2, figsize=(12, 10))

            rates = np.array([r.get('firing_rate') for r in self.analyzer.results])

            # Histogram
            axes[0, 0].hist(rates, bins=50, alpha=0.7, color='skyblue', edgecolor='black')
            axes[0, 0].set_xlabel('Firing Rate')
            axes[0, 0].set_ylabel('Frequency')
            axes[0, 0].set_title('Firing Rate Distribution')
            axes[0, 0].grid(True, alpha=0.3)

            # Box plot
            axes[0, 1].boxplot(rates, patch_artist=True,
                              boxprops=dict(facecolor='lightblue'))
            axes[0, 1].set_ylabel('Firing Rate')
            axes[0, 1].set_title('Firing Rate Box Plot')
            axes[0, 1].grid(True, alpha=0.3)

            # Q-Q plot
            stats = scipy.stats.provisional_qqplot(rates, line='s')
            axes[1, 0].plot(stats[0], stats[1], 'bo', alpha=0.7)
            axes[1, 0].plot(stats[0], stats[2], 'r--', label='Reference line')
            axes[1, 0].set_xlabel('Theoretical Quantiles')
            axes[1, 0].set_ylabel('Sample Quantiles')
            axes[1, 0].set_title('Q-Q Plot')
            axes[1, 0].legend()
            axes[1, 0].grid(True, alpha=0.3)

            # Cumulative distribution
            sorted_rates = np.sort(rates)
            cdf = np.arange(1, len(sorted_rates) + 1) / len(sorted_rates)
            axes[1, 1].plot(sorted_rates, cdf, 'b-', linewidth=2)
            axes[1, 1].set_xlabel('Firing Rate')
            axes[1, 1].set_ylabel('Cumulative Probability')
            axes[1, 1].set_title('Cumulative Distribution Function')
            axes[1, 1].grid(True, alpha=0.3)

            plt.tight_layout()

            output_file = f"{output_dir}/firing_rate_distribution.png"
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()

            return output_file

        except ImportError:
            print("matplotlib or scipy not available - cannot create plots")
            return None

    def _plot_correlation_matrix(self, output_dir: str):
        """Plot correlation matrix."""
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns

            correlations = self.analyzer.analysis_results.get('cross_correlations', {})

            if not correlations:
                return None

            # Create correlation matrix
            keys = list(correlations.keys())
            values = [correlations[key] for key in keys]

            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            axes = axes.flatten()

            for i in range(min(4, len(values))):
                axes[i].bar(i, values[i], color='skyblue')
                axes[i].set_ylim(0, 1)
                axes[i].set_title(keys[i])
                axes[i].set_ylabel('Correlation')

            plt.suptitle('Correlation Matrix')
            plt.tight_layout()

            output_file = f"{output_dir}/correlation_matrix.png"
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()

            return output_file

        except ImportError:
            print("matplotlib or seaborn not available - cannot create plots")
            return None

    def _plot_anomalies(self, output_dir: str):
        """Plot anomalies."""
        try:
            import matplotlib.pyplot as plt

            anomalies = self.analyzer.analysis_results.get('anomalies', {})

            if 'outliers' not in anomalies:
                return None

            outliers = anomalies['outliers']
            rates = np.array([r.get('firing_rate') for r in self.analyzer.results])

            fig, axes = plt.subplots(2, 1, figsize=(10, 8))

            # Plot normal data
            axes[0].plot(range(len(rates)), rates, 'b-', alpha=0.7, label='Normal data')
            axes[0].set_xlabel('Step')
            axes[0].set_ylabel('Firing Rate')
            axes[0].set_title('Anomaly Detection - Normal Data')
            axes[0].legend()
            axes[0].grid(True, alpha=0.3)

            # Plot outliers highlighted
            axes[1].plot(range(len(rates)), rates, 'b-', alpha=0.3, label='Normal data')
            axes[1].scatter(outliers['indices'], outliers['values'],
                           color='red', s=50, alpha=0.8, label='Outliers')
            axes[1].set_xlabel('Step')
            axes[1].set_ylabel('Firing Rate')
            axes[1].set_title('Anomaly Detection - Outliers Highlighted')
            axes[1].legend()
            axes[1].grid(True, alpha=0.3)

            plt.tight_layout()

            output_file = f"{output_dir}/anomaly_detection.png"
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()

            return output_file

        except ImportError:
            print("matplotlib not available - cannot create plots")
            return None


class StatisticsCalculator:
    """
    Calculate comprehensive statistics from simulation data.
    """

    def __init__(self, results: List[Dict]):
        """
        Initialize statistics calculator.

        Args:
            results: List of simulation result dictionaries.
        """
        self.results = results

    def calculate_all_statistics(self) -> Dict:
        """Calculate all statistics."""
        statistics = {}

        if not self.results:
            return statistics

        numeric_keys = self._get_numeric_keys()

        for key in numeric_keys:
            values = [r.get(key) for r in self.results if r.get(key) is not None]

            if len(values) == 0:
                continue

            stats = self._calculate_statistics(values, key)
            statistics[key] = stats

        # Calculate temporal statistics
        statistics['temporal'] = self._calculate_temporal_statistics()

        # Calculate trend analysis
        statistics['trend'] = self._calculate_trend_analysis()

        return statistics

    def _calculate_statistics(self, values: List[float], key: str) -> Dict:
        """Calculate statistics for a single variable."""
        stats = {}

        try:
            arr = np.array(values)

            stats['count'] = len(arr)
            stats['mean'] = float(np.mean(arr))
            stats['std'] = float(np.std(arr))
            stats['min'] = float(np.min(arr))
            stats['max'] = float(np.max(arr))
            stats['median'] = float(np.median(arr))
            stats['range'] = float(np.max(arr) - np.min(arr))
            stats['cv'] = float(np.std(arr) / np.mean(arr) if np.mean(arr) != 0 else 0.0)

            # Percentiles
            if len(arr) >= 100:
                percentiles = np.percentile(arr, [25, 50, 75, 90, 95, 99])
                stats['percentiles'] = {
                    '25': float(percentiles[0]),
                    '50': float(percentiles[1]),
                    '75': float(percentiles[2]),
                    '90': float(percentiles[3]),
                    '95': float(percentiles[4]),
                    '99': float(percentiles[5])
                }

            # Skewness and kurtosis
            if len(arr) >= 3:
                from scipy import stats
                stats['skewness'] = float(stats.skew(arr))
                stats['kurtosis'] = float(stats.kurtosis(arr))

        except Exception as e:
            stats['error'] = str(e)

        return stats

    def _calculate_temporal_statistics(self) -> Dict:
        """Calculate temporal statistics."""
        if not self.results or 'step' not in self.results[0]:
            return {}

        temporal = {}
        steps = [r['step'] for r in self.results]

        for key in ['firing_rate', 'total_spikes', 'firing_neurons']:
            if key not in self.results[0]:
                continue

            values = [r.get(key) for r in self.results if r.get(key) is not None]

            if len(values) >= 2:
                # Calculate temporal derivatives
                temporal[f'{key}_derivative'] = {
                    'mean': float(np.mean(np.diff(values))),
                    'std': float(np.std(np.diff(values))),
                    'max_change': float(np.max(np.abs(np.diff(values))))
                }

                # Calculate autocorrelation
                try:
                    from scipy import signal
                    autocorr = signal.correlate(values - np.mean(values),
                                               values - np.mean(values), mode='full')
                    autocorr = autocorr[len(autocorr)//2:]
                    lag_times = np.arange(0, len(autocorr))

                    # Find first zero-crossing
                    zero_crossings = np.where(np.diff(np.sign(autocorr)))[0]
                    if len(zero_crossings) > 0:
                        temporal[f'{key}_autocorr_time'] = {
                            'zero_crossing_lag': int(zero_crossings[0]),
                            'correlation_at_zero': float(autocorr[zero_crossings[0]]),
                            'half_width': self._calculate_half_width(autocorr)
                        }
                except ImportError:
                    pass

        return temporal

    def _calculate_trend_analysis(self) -> Dict:
        """Calculate trend analysis."""
        trend = {}

        if 'step' not in self.results[0]:
            return trend

        for key in ['firing_rate', 'total_spikes']:
            if key not in self.results[0]:
                continue

            values = [r.get(key) for r in self.results if r.get(key) is not None]

            if len(values) >= 2:
                # Linear regression
                x = np.array(range(len(values)))
                y = np.array(values)

                slope, intercept = np.polyfit(x, y, 1)

                trend[f'{key}_linear'] = {
                    'slope': float(slope),
                    'intercept': float(intercept),
                    'r_squared': float(np.corrcoef(x, y)[0, 1] ** 2)
                }

                # Detrended series
                trend_values = y - (slope * x + intercept)
                trend[f'{key}_detrended_stats'] = {
                    'mean': float(np.mean(trend_values)),
                    'std': float(np.std(trend_values)),
                    'autocorrelation': float(np.corrcoef(trend_values[:-1], trend_values[1:])[0, 1])
                }

        return trend

    def _calculate_half_width(self, autocorr: np.ndarray) -> int:
        """Calculate half-width of autocorrelation."""
        if len(autocorr) < 2:
            return 0

        # Find where autocorrelation drops to half of its maximum value
        max_corr = autocorr[0]
        half_max = max_corr / 2.0

        for i, corr in enumerate(autocorr):
            if corr <= half_max:
                return i

        return len(autocorr) - 1

    def _get_numeric_keys(self) -> List[str]:
        """Get numeric keys from results."""
        if not self.results:
            return []

        numeric_keys = set()
        for result in self.results:
            for key, value in result.items():
                if isinstance(value, (int, float)) and not key.startswith('step'):
                    numeric_keys.add(key)

        return list(numeric_keys)


class PatternDetector:
    """
    Detect patterns in simulation data.
    """

    def __init__(self, results: List[Dict]):
        """
        Initialize pattern detector.

        Args:
            results: List of simulation result dictionaries.
        """
        self.results = results

    def detect_all_patterns(self) -> Dict:
        """Detect all patterns."""
        patterns = {}

        patterns.update(self._detect_periodic_patterns())
        patterns.update(self._detect_state_transitions())
        patterns.update(self._detect_bursts())
        patterns.update(self._detect_regressions())

        return patterns

    def _detect_periodic_patterns(self) -> Dict:
        """Detect periodic patterns."""
        periodic = {}

        if 'firing_rate' not in self.results[0]:
            return periodic

        rates = np.array([r['firing_rate'] for r in self.results])

        # Calculate power spectrum
        try:
            from scipy import signal
            n = len(rates)
            freqs = np.fft.fftfreq(n)
            fft_vals = np.fft.fft(rates - np.mean(rates))
            power_spectrum = np.abs(fft_vals) ** 2

            # Find peaks in power spectrum
            peak_indices, _ = signal.find_peaks(power_spectrum[:n//2])

            if len(peak_indices) > 0:
                periodic_patterns = []
                for idx in peak_indices[:5]:  # Top 5 peaks
                    freq = freqs[idx]
                    if freq > 0:  # Only positive frequencies
                        period = 1.0 / freq if freq > 0 else float('inf')
                        power = power_spectrum[idx]
                        periodic_patterns.append({
                            'frequency': float(freq),
                            'period': float(period),
                            'power': float(power)
                        })

                periodic['power_spectrum_peaks'] = periodic_patterns

        except ImportError:
            pass

        return periodic

    def _detect_state_transitions(self) -> Dict:
        """Detect state transitions."""
        transitions = {}

        if 'firing_rate' not in self.results[0]:
            return transitions

        rates = [r['firing_rate'] for r in self.results]
        threshold = np.mean(rates) + np.std(rates)

        state_changes = []
        current_state = 'low' if rates[0] < threshold else 'high'

        for step, rate in enumerate(rates[1:], 1):
            new_state = 'low' if rate < threshold else 'high'
            if new_state != current_state:
                state_changes.append({
                    'step': step,
                    'from_state': current_state,
                    'to_state': new_state,
                    'duration': 1
                })
                current_state = new_state
            elif state_changes and state_changes[-1]['to_state'] == current_state:
                # Extend current state
                state_changes[-1]['duration'] += 1

        transitions['state_transitions'] = state_changes

        return transitions

    def _detect_bursts(self) -> Dict:
        """Detect bursts in the data."""
        bursts = {}

        if 'firing_rate' not in self.results[0]:
            return bursts

        rates = np.array([r['firing_rate'] for r in self.results])

        # Find burst threshold
        threshold = np.mean(rates) + np.std(rates)

        in_burst = False
        burst_starts = []
        burst_end_steps = []

        for step, rate in enumerate(rates):
            if rate > threshold and not in_burst:
                in_burst = True
                burst_starts.append(step)
            elif rate <= threshold and in_burst:
                in_burst = False
                burst_end_steps.append(step - 1)

        # Close any open bursts
        if in_burst:
            burst_end_steps.append(len(rates) - 1)

        # Calculate burst characteristics
        burst_durations = []
        burst_amplitudes = []

        for start, end in zip(burst_starts, burst_end_steps):
            duration = end - start + 1
            burst_durations.append(duration)

            if duration > 0:
                burst_amplitudes.append(np.mean(rates[start:end + 1]))

        bursts['detected_bursts'] = {
            'count': len(burst_starts),
            'durations': burst_durations,
            'amplitudes': burst_amplitudes,
            'mean_duration': np.mean(burst_durations) if burst_durations else 0,
            'mean_amplitude': np.mean(burst_amplitudes) if burst_amplitudes else 0
        }

        return bursts

    def _detect_regressions(self) -> Dict:
        """Detect regression patterns."""
        regressions = {}

        if 'firing_rate' not in self.results[0]:
            return regressions

        rates = np.array([r['firing_rate'] for r in self.results])

        # Find decreasing trends
        decreasing_segments = []
        current_segment = None

        for step, rate in enumerate(rates):
            if current_segment is None:
                current_segment = {'start': step, 'end': step, 'rates': [rate]}
            else:
                if rate < current_segment['rates'][-1]:  # Continuing decrease
                    current_segment['end'] = step
                    current_segment['rates'].append(rate)
                else:  # Segment ended
                    if len(current_segment['rates']) >= 3:  # Minimum length
                        decreasing_segments.append(current_segment)
                    current_segment = {'start': step, 'end': step, 'rates': [rate]}

        # Check final segment
        if current_segment and len(current_segment['rates']) >= 3:
            decreasing_segments.append(current_segment)

        regressions['decreasing_segments'] = decreasing_segments

        return regressions


# Convenience functions

def createAnalyzer(results: List[Dict]) -> SimulationAnalyzer:
    """Create a simulation analyzer."""
    return SimulationAnalyzer(results)


def createProcessor(results: List[Dict]) -> ResultsProcessor:
    """Create a results processor."""
    return ResultsProcessor(results)


def createVisualizer(results: List[Dict]) -> Visualizer:
    """Create a visualizer."""
    return Visualizer(results)


def createPlotter(analyzer: SimulationAnalyzer) -> PlotGenerator:
    """Create a plot generator."""
    return PlotGenerator(analyzer)


def createStatistics(results: List[Dict]) -> StatisticsCalculator:
    """Create a statistics calculator."""
    return StatisticsCalculator(results)


def createPatternDetector(results: List[Dict]) -> PatternDetector:
    """Create a pattern detector."""
    return PatternDetector(results)


def createProfiler(results: List[Dict]) -> 'PerformanceProfiler':
    """Create a performance profiler."""
    return PerformanceProfiler(results)


def createActivityAnalyzer(results: List[Dict]) -> 'ActivityAnalyzer':
    """Create an activity analyzer."""
    return ActivityAnalyzer(results)


def createConnectivityAnalyzer(results: List[Dict]) -> 'ConnectivityAnalyzer':
    """Create a connectivity analyzer."""
    return ConnectivityAnalyzer(results)


def createLearningAnalyzer(results: List[Dict]) -> 'LearningAnalyzer':
    """Create a learning analyzer."""
    return LearningAnalyzer(results)


# Export commonly used classes and functions
__all__.extend([
    'SimulationAnalyzer', 'ResultsProcessor', 'Visualizer', 'PlotGenerator',
    'StatisticsCalculator', 'PatternDetector', 'PerformanceProfiler',
    'ActivityAnalyzer', 'ConnectivityAnalyzer', 'LearningAnalyzer',
    'createAnalyzer', 'createProcessor', 'createVisualizer',
    'createPlotter', 'createStatistics', 'createPatternDetector',
    'createProfiler', 'createActivityAnalyzer', 'createConnectivityAnalyzer',
    'createLearningAnalyzer'
])


# Performance profiler
class PerformanceProfiler:
    """Profile performance metrics."""

    def __init__(self, results: List[Dict]):
        self.results = results

    def profile(self) -> Dict:
        """Profile performance."""
        profile = {}

        if not self.results:
            return profile

        # Time-related metrics
        if 'step' in self.results[0]:
            steps = [r['step'] for r in self.results]
            profile['temporal'] = {
                'steps_per_unit_time': len(steps) / (steps[-1] - steps[0]) if len(steps) > 1 else 0,
                'time_efficiency': len(steps) / (max(steps) - min(steps) + 1)
            }

        # Data-related metrics
        profile['data'] = {
            'total_results': len(self.results),
            'data_points': sum(len(r) for r in self.results),
            'memory_efficiency': len(self.results) / (max(len(r) for r in self.results) if self.results else 1)
        }

        return profile


# Activity analyzer
class ActivityAnalyzer:
    """Analyze activity patterns."""

    def __init__(self, results: List[Dict]):
        self.results = results

    def analyze_activity(self) -> Dict:
        """Analyze activity patterns."""
        activity = {}

        if 'firing_rate' not in self.results[0]:
            return activity

        rates = np.array([r['firing_rate'] for r in self.results])

        activity['firing_patterns'] = {
            'mean_firing_rate': np.mean(rates),
            'firing_rate_std': np.std(rates),
            'firing_rate_cv': np.std(rates) / np.mean(rates) if np.mean(rates) > 0 else 0,
            'cv_class': self._classify_cv(np.std(rates) / np.mean(rates) if np.mean(rates) > 0 else 0)
        }

        return activity

    def _classify_cv(self, cv: float) -> str:
        """Classify coefficient of variation."""
        if cv < 0.1:
            return 'low'
        elif cv < 0.3:
            return 'moderate'
        else:
            return 'high'


# Connectivity analyzer
class ConnectivityAnalyzer:
    """Analyze connectivity patterns."""

    def __init__(self, results: List[Dict]):
        self.results = results

    def analyze_connectivity(self) -> Dict:
        """Analyze connectivity patterns."""
        connectivity = {}

        if 'excitatory_inhibitory_ratio' not in self.results[0]:
            return connectivity

        ratios = [r['excitatory_inhibitory_ratio'] for r in self.results
                 if r.get('excitatory_inhibitory_ratio') is not None]

        if ratios:
            connectivity['excitation_inhibition'] = {
                'mean_ratio': np.mean(ratios),
                'ratio_std': np.std(ratios),
                'balanced_range': np.sum((np.array(ratios) > 0.5) & (np.array(ratios) < 2.0)) / len(ratios)
            }

        return connectivity


# Learning analyzer
class LearningAnalyzer:
    """Analyze learning patterns."""

    def __init__(self, results: List[Dict]):
        self.results = results

    def analyze_learning(self) -> Dict:
        """Analyze learning patterns."""
        learning = {}

        if 'firing_rate' not in self.results[0]:
            return learning

        rates = [r['firing_rate'] for r in self.results]

        # Calculate learning metrics
        if len(rates) >= 10:
            first_half = rates[:len(rates)//2]
            second_half = rates[len(rates)//2:]

            learning['learning_trajectory'] = {
                'first_half_mean': np.mean(first_half),
                'second_half_mean': np.mean(second_half),
                'improvement': np.mean(second_half) - np.mean(first_half),
                'improvement_rate': (np.mean(second_half) - np.mean(first_half)) / len(first_half)
            }

        return learning


# Main entry point
def main():
    """Main entry point for analysis tools."""
    import argparse
    import sys

    parser = argparse.ArgumentParser(
        prog="nlm-analysis",
        description="NLM (Neural Learning Machine) Analysis Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-analysis analyze --file results.json --format json
  nlm-analysis plot --input results.json --output plots/
  nlm-analysis stats --file results.json --type comprehensive
        """
    )

    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Analyze command
    analyze_parser = subparsers.add_parser(
        "analyze",
        help="Analyze simulation results"
    )
    analyze_parser.add_argument(
        "--file", "-f",
        required=True,
        help="Results file to analyze"
    )
    analyze_parser.add_argument(
        "--format", "-f",
        choices=["json", "csv"],
        default="json",
        help="Input file format"
    )
    analyze_parser.add_argument(
        "--type", "-t",
        choices=["basic", "detailed", "comprehensive"],
        default="comprehensive",
        help="Type of analysis"
    )

    # Plot command
    plot_parser = subparsers.add_parser(
        "plot",
        help="Generate plots from results"
    )
    plot_parser.add_argument(
        "--input", "-i",
        required=True,
        help="Input results file"
    )
    plot_parser.add_argument(
        "--output", "-o",
        required=True,
        help="Output directory for plots"
    )
    plot_parser.add_argument(
        "--format", "-f",
        choices=["png", "pdf", "svg"],
        default="png",
        help="Output plot format"
    )

    # Stats command
    stats_parser = subparsers.add_parser(
        "stats",
        help="Generate statistics from results"
    )
    stats_parser.add_argument(
        "--file", "-f",
        required=True,
        help="Results file to analyze"
    )
    stats_parser.add_argument(
        "--type", "-t",
        choices=["basic", "detailed", "comprehensive"],
        default="comprehensive",
        help="Type of statistics"
    )

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        return 1

    try:
        if args.command == "analyze":
            analyze_results(args)
        elif args.command == "plot":
            plot_results(args)
        elif args.command == "stats":
            generate_stats(args)
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1

    return 0


def analyze_results(args):
    """Analyze results file."""
    print(f"Analyzing results from {args.file}")

    try:
        with open(args.file, 'r') as f:
            if args.format == "json":
                results = json.load(f)
            else:  # CSV
                df = pd.read_csv(f)
                results = df.to_dict('records')

        if not results:
            print("No results to analyze")
            return

        analyzer = SimulationAnalyzer(results)
        analysis = analyzer.analyze(args.type)

        # Save analysis
        output_file = args.file.replace(f".{args.format}", f"_analysis.json")
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)

        print(f"Analysis saved to {output_file}")
        print(f"Analysis type: {args.type}")
        print(f"Total steps analyzed: {analysis.get('total_steps', 0)}")

    except FileNotFoundError:
        print(f"File not found: {args.file}")
    except Exception as e:
        print(f"Error analyzing file: {e}")


def plot_results(args):
    """Generate plots from results."""
    print(f"Generating plots from {args.input}")

    try:
        with open(args.input, 'r') as f:
            results = json.load(f)

        analyzer = SimulationAnalyzer(results)
        plotter = PlotGenerator(analyzer)

        plots = plotter.generate_all_plots(args.output)

        print(f"Plots generated in {args.output}")
        for plot_type, plot_file in plots.items():
            print(f"  {plot_type}: {plot_file}")

    except FileNotFoundError:
        print(f"File not found: {args.input}")
    except Exception as e:
        print(f"Error generating plots: {e}")


def generate_stats(args):
    """Generate statistics from results."""
    print(f"Generating statistics from {args.file}")

    try:
        with open(args.file, 'r') as f:
            results = json.load(f)

        calculator = StatisticsCalculator(results)
        statistics = calculator.calculate_all_statistics()

        # Print statistics
        print("\nStatistics Summary:")
        for key, value in statistics.items():
            print(f"\n{key}:")
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    if isinstance(subvalue, dict):
                        print(f"  {subkey}:")
                        for k, v in subvalue.items():
                            print(f"    {k}: {v}")
                    else:
                        print(f"  {subkey}: {subvalue}")
            else:
                print(f"  {value}")

        # Save statistics
        output_file = args.file.replace(f".json", f"_stats.json")
        with open(output_file, 'w') as f:
            json.dump(statistics, f, indent=2)

        print(f"\nStatistics saved to {output_file}")

    except FileNotFoundError:
        print(f"File not found: {args.file}")
    except Exception as e:
        print(f"Error generating statistics: {e}")


if __name__ == "__main__":
    sys.exit(main())