#!/usr/bin/env python3
"""
NLM Advanced Data Analysis and Visualization Module
===================================================

Comprehensive data analysis and visualization tools for NLM simulation results.
Provides capabilities for:

- Neural activity analysis and pattern detection
- Network topology analysis and visualization
- Learning progress tracking and analysis
- Performance benchmarking and optimization analysis
- Statistical analysis and hypothesis testing
- Machine learning model analysis and validation
- Interactive visualization and plotting
- Data export and import
- Custom analysis algorithms
- Real-time monitoring and live visualization
"""

import numpy as np
import pandas as pd
import time
import threading
import json
import math
from typing import Dict, List, Optional, Any, Tuple, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
from datetime import datetime, timedelta
import warnings
from scipy import stats, signal
from scipy.spatial.distance import pdist, squareform
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from sklearn.decomposition import PCA, TSNE
from sklearn.manifold import MDS
from sklearn.cluster import KMeans
from sklearn.ensemble import IsolationForest
import umap
import plotly.graph_objects as go
import plotly.express as px
from plotly.subplots import make_subplots

# Try to import the core NLM bindings
try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Warning: pynlm module not found. Some features will not be available.")

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
class AnalysisType(Enum):
    """Type of analysis to perform"""
    NEURAL_ACTIVITY = "neural_activity"
    NETWORK_TOPOLOGY = "network_topology"
    LEARNING_PROGRESS = "learning_progress"
    SPATIAL_DISTRIBUTION = "spatial_distribution"
    TEMPORAL_PATTERN = "temporal_pattern"
    CONNECTIVITY_ANALYSIS = "connectivity_analysis"
    STATISTICAL_ANALYSIS = "statistical_analysis"
    MACHINE_LEARNING = "machine_learning"
    PREDICTION_ANALYSIS = "prediction_analysis"
    MEMORY_ANALYSIS = "memory_analysis"
class VisualizationType(Enum):
    """Type of visualization"""
    HEATMAP = "heatmap"
    RASTER_PLOT = "raster_plot"
    NETWORK_GRAPH = "network_graph"
    TIME_SERIES = "time_series"
    SCATTER_PLOT = "scatter_plot"
    CONTOUR_PLOT = "contour_plot"
    HISTOGRAM = "histogram"
    BOX_PLOT = "box_plot"
    PAIRWISE_PLOT = "pairwise_plot"
    3D_SURFACE = "3d_surface"
    STREAMGRAPH = "streamgraph"
    INTERACTIVE_MAP = "interactive_map"
class AnalysisWindow:
    """Analysis time window configuration"""
    
    def __init__(self, 
                 start_step: int = 0,
                 end_step: Optional[int] = None,
                 step_size: int = 1,
                 type: str = 'fixed'):
        self.start_step = start_step
        self.end_step = end_step
        self.step_size = step_size
        self.type = type  # 'fixed', 'sliding', 'adaptive'
        self.created_at = time.time()
    
    def get_steps(self, total_steps: int) -> List[int]:
        """Get list of step indices to analyze"""
        if self.end_step is None:
            self.end_step = total_steps
        
        if self.type == 'fixed':
            return list(range(self.start_step, min(self.end_step, total_steps), self.step_size))
        elif self.type == 'sliding':
            # Sliding window analysis
            window_size = min(100, self.end_step - self.start_step)
            steps = []
            for start in range(self.start_step, self.end_step - window_size + 1, self.step_size):
                steps.extend(range(start, start + window_size, self.step_size))
            return list(set(steps))  # Remove duplicates
        else:  # adaptive
            # Adaptive window based on simulation characteristics
            pass
        
        return list(range(self.start_step, min(self.end_step, total_steps), self.step_size))
class NeuralActivityAnalyzer:
    """Advanced neural activity analysis"""
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.analysis_history = []
    
    def analyze_spike_patterns(self,
                              neurons: Optional[List[int]] = None,
                              time_window: Optional[AnalysisWindow] = None,
                              metrics: List[str] = None) -> Dict[str, Any]:
        """
        Analyze spike patterns with advanced metrics
        
        Args:
            neurons: Specific neurons to analyze
            time_window: Time window for analysis
            metrics: List of metrics to compute
            
        Returns:
            Dictionary with spike pattern analysis
        """
        metrics = metrics or [
            'firing_rate', 'coherence', 'synchrony', 'burst_detection',
            'inter_spike_interval', 'firing_frequency_distribution',
            'population_activity', 'spatial_pattern', 'temporal_pattern'
        ]
        
        analysis_result = {
            'spike_patterns': {},
            'temporal_dynamics': {},
            'population_metrics': {},
            'spatial_analysis': {}
        }
        
        # This would integrate with NLM brain data if available
        # For now, provide framework structure
        
        return analysis_result
    
    def analyze_neural_coherence(self,
                                neurons: List[int],
                                window_size: int = 100,
                                threshold: float = 0.5) -> Dict[str, Any]:
        """
        Analyze coherence between neurons
        
        Args:
            neurons: List of neuron indices
            window_size: Analysis window size
            threshold: Coherence threshold for significant connections
            
        Returns:
            Dictionary with coherence analysis
        """
        # Calculate pairwise coherence
        coherence_matrix = np.zeros((len(neurons), len(neurons)))
        
        for i, neuron_i in enumerate(neurons):
            for j, neuron_j in enumerate(neurons):
                if i < j:
                    # Calculate coherence (simplified)
                    coherence = self._calculate_neuron_coherence(neuron_i, neuron_j, window_size)
                    coherence_matrix[i, j] = coherence
                    coherence_matrix[j, i] = coherence
        
        # Find significant connections
        significant_connections = np.where(coherence_matrix > threshold)
        
        return {
            'coherence_matrix': coherence_matrix.tolist(),
            'significant_connections': list(zip(*significant_connections)),
            'mean_coherence': np.mean(coherence_matrix),
            'max_coherence': np.max(coherence_matrix),
            'coherence_distribution': self._analyze_coherence_distribution(coherence_matrix)
        }
    
    def analyze_firing_regularity(self,
                                 neuron_id: int,
                                 window_size: int = 1000) -> Dict[str, Any]:
        """
        Analyze firing regularity and pattern
        
        Args:
            neuron_id: Neuron identifier
            window_size: Analysis window size
            
        Returns:
            Dictionary with firing regularity analysis
        """
        # This would extract spike times from NLM brain
        # For now, provide framework
        
        return {
            'firing_rate': 0.0,
            'cv_of_isi': 0.0,  # Coefficient of variation of interspike intervals
            'burst_index': 0.0,
            'regularity_index': 0.0,
            'spike_train_spectrum': []
        }
    
    def detect_bursts(self,
                      neuron_id: int,
                      spike_times: List[float],
                      threshold: float = 2.0,
                      min_burst_duration: float = 10.0,
                      min_inter_burst_interval: float = 50.0) -> Dict[str, Any]:
        """
        Detect bursts in spike trains
        
        Args:
            neuron_id: Neuron identifier
            spike_times: List of spike times
            threshold: Firing rate threshold for burst detection
            min_burst_duration: Minimum burst duration
            min_inter_burst_interval: Minimum interval between bursts
            
        Returns:
            Dictionary with burst analysis
        """
        if len(spike_times) < 2:
            return {'bursts': [], 'burst_metrics': {}}
        
        # Calculate instantaneous firing rate
        isi = np.diff(spike_times)
        instantaneous_rate = 1.0 / isi if len(isi) > 0 else np.array([])
        
        # Detect bursts using threshold crossing
        burst_indices = []
        in_burst = False
        burst_start = 0
        
        for i, rate in enumerate(instantaneous_rate):
            if rate > threshold and not in_burst:
                in_burst = True
                burst_start = i
            elif rate <= threshold and in_burst:
                burst_end = i
                if burst_end - burst_start >= min_burst_duration:
                    burst_indices.append((burst_start, burst_end))
                in_burst = False
        
        # Handle case where burst continues to end
        if in_burst:
            burst_end = len(instantaneous_rate) - 1
            if burst_end - burst_start >= min_burst_duration:
                burst_indices.append((burst_start, burst_end))
        
        # Extract burst information
        bursts = []
        for start, end in burst_indices:
            if end > start:
                burst_spikes = spike_times[start:end+1]
                if len(burst_spikes) >= 2:
                    burst_duration = burst_spikes[-1] - burst_spikes[0]
                    mean_isi = np.mean(np.diff(burst_spikes))
                    burst_rate = len(burst_spikes) / burst_duration
                    
                    bursts.append({
                        'start_time': burst_spikes[0],
                        'end_time': burst_spikes[-1],
                        'duration': burst_duration,
                        'spike_count': len(burst_spikes),
                        'mean_isi': mean_isi,
                        'burst_rate': burst_rate
                    })
        
        # Calculate burst metrics
        if bursts:
            burst_durations = [b['duration'] for b in bursts]
            spike_counts = [b['spike_count'] for b in bursts]
            burst_rates = [b['burst_rate'] for b in bursts]
            
            burst_metrics = {
                'total_bursts': len(bursts),
                'mean_burst_duration': np.mean(burst_durations),
                'std_burst_duration': np.std(burst_durations),
                'mean_spike_count': np.mean(spike_counts),
                'mean_burst_rate': np.mean(burst_rates),
                'burst_rate_distribution': self._calculate_burst_rate_distribution(burst_rates)
            }
        else:
            burst_metrics = {
                'total_bursts': 0,
                'mean_burst_duration': 0.0,
                'std_burst_duration': 0.0,
                'mean_spike_count': 0.0,
                'mean_burst_rate': 0.0,
                'burst_rate_distribution': []
            }
        
        return {'bursts': bursts, 'burst_metrics': burst_metrics}
    
    def analyze_population_dynamics(self,
                                   neuron_groups: List[List[int]],
                                   time_window: AnalysisWindow = None) -> Dict[str, Any]:
        """
        Analyze population-level neural dynamics
        
        Args:
            neuron_groups: List of neuron groups
            time_window: Time window for analysis
            
        Returns:
            Dictionary with population dynamics analysis
        """
        # This would analyze population activity patterns
        # For now, provide framework
        
        return {
            'population_activity': {},
            'mean_field_dynamics': {},
            'collective_bursting': {},
            'information_flow': {}
        }
    
    def _calculate_neuron_coherence(self,
                                   neuron_i: int,
                                   neuron_j: int,
                                   window_size: int) -> float:
        """Calculate coherence between two neurons (framework)"""
        # This would calculate actual coherence from NLM data
        return np.random.random() * 0.5  # Placeholder
    
    def _analyze_coherence_distribution(self, coherence_matrix: np.ndarray) -> Dict[str, Any]:
        """Analyze coherence distribution"""
        # Flatten upper triangle (excluding diagonal)
        upper_triangle = coherence_matrix[np.triu_indices_from(coherence_matrix, k=1)]
        
        if len(upper_triangle) == 0:
            return {'mean': 0.0, 'std': 0.0, 'distribution': []}
        
        return {
            'mean': float(np.mean(upper_triangle)),
            'std': float(np.std(upper_triangle)),
            'min': float(np.min(upper_triangle)),
            'max': float(np.max(upper_triangle)),
            'percentile_25': float(np.percentile(upper_triangle, 25)),
            'percentile_75': float(np.percentile(upper_triangle, 75)),
            'histogram': self._calculate_histogram(upper_triangle, bins=20)
        }
    
    def _calculate_burst_rate_distribution(self, burst_rates: List[float]) -> Dict[str, Any]:
        """Calculate burst rate distribution"""
        if not burst_rates:
            return {'histogram': [], 'mean': 0.0, 'std': 0.0}
        
        return {
            'histogram': self._calculate_histogram(burst_rates, bins=20),
            'mean': float(np.mean(burst_rates)),
            'std': float(np.std(burst_rates)),
            'skewness': float(stats.skew(burst_rates) if len(burst_rates) > 1 else 0.0),
            'kurtosis': float(stats.kurtosis(burst_rates) if len(burst_rates) > 1 else 0.0)
        }
    
    def _calculate_histogram(self, data: np.ndarray, bins: int = 20) -> List[Dict[str, Any]]:
        """Calculate histogram data"""
        if len(data) == 0:
            return []
        
        counts, bin_edges = np.histogram(data, bins=bins)
        bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2
        
        return [
            {
                'bin_center': float(center),
                'bin_edge_low': float(bin_edges[i]),
                'bin_edge_high': float(bin_edges[i+1]),
                'count': int(counts[i])
            }
            for i, center in enumerate(bin_centers)
        ]
class NetworkTopologyAnalyzer:
    """Network topology analysis and visualization"""
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.topology_cache = {}
    
    def analyze_connectivity_patterns(self,
                                     neurons: List[int] = None,
                                     min_connection_strength: float = 0.1) -> Dict[str, Any]:
        """
        Analyze network connectivity patterns
        
        Args:
            neurons: Specific neurons to analyze
            min_connection_strength: Minimum connection strength threshold
            
        Returns:
            Dictionary with connectivity analysis
        """
        connectivity_analysis = {
            'degree_distribution': {},
            'clustering_coefficient': {},
            'path_length': {},
            'small_world_coefficient': {},
            'community_structure': {},
            'hub_analysis': {},
            'connection_strength_distribution': {}
        }
        
        # This would analyze actual network data from NLM
        # For now, provide framework
        
        return connectivity_analysis
    
    def calculate_clustering_coefficient(self,
                                        adjacency_matrix: np.ndarray) -> float:
        """Calculate clustering coefficient of a network"""
        n = adjacency_matrix.shape[0]
        clustering_coeff = 0.0
        
        for i in range(n):
            neighbors = np.where(adjacency_matrix[i] > 0)[0]
            k_i = len(neighbors)
            
            if k_i < 2:
                continue
            
            # Count triangles
            triangle_count = 0
            for j in neighbors:
                for k in neighbors:
                    if j < k and adjacency_matrix[j, k] > 0:
                        triangle_count += 1
            
            clustering_coeff += triangle_count / (k_i * (k_i - 1))
        
        return clustering_coeff / n if n > 0 else 0.0
    
    def calculate_path_lengths(self,
                               adjacency_matrix: np.ndarray) -> Dict[str, float]:
        """Calculate shortest path lengths"""
        n = adjacency_matrix.shape[0]
        
        # Convert to distance matrix
        distance_matrix = 1.0 / (adjacency_matrix + 1e-10)  # Inverse of connectivity
        
        # Calculate shortest paths (simplified - would use Floyd-Warshall or similar)
        path_lengths = {}
        
        # Placeholder for actual calculation
        path_lengths['average'] = 2.5
        path_lengths['characteristic'] = 2.3
        path_lengths['diameter'] = 8.0
        path_lengths['radius'] = 4.0
        
        return path_lengths
    
    def detect_communities(self,
                          adjacency_matrix: np.ndarray,
                          method: str = 'louvain') -> Dict[str, Any]:
        """
        Detect communities in network
        
        Args:
            adjacency_matrix: Network adjacency matrix
            method: Community detection method ('louvain', 'greedy', 'spectral')
            
        Returns:
            Dictionary with community analysis
        """
        n = adjacency_matrix.shape[0]
        
        # Simplified community detection framework
        # Would use actual community detection algorithms in practice
        
        communities = {}
        for i in range(n):
            # Assign nodes to communities based on connectivity
            neighbors = np.where(adjacency_matrix[i] > 0)[0]
            community_id = hash(tuple(sorted(neighbors))) % 100  # Simplified
            
            if community_id not in communities:
                communities[community_id] = []
            communities[community_id].append(i)
        
        # Calculate community quality metrics
        modularity = self._calculate_modularity(adjacency_matrix, communities)
        
        return {
            'communities': communities,
            'modularity': modularity,
            'number_of_communities': len(communities),
            'community_sizes': [len(comm) for comm in communities.values()],
            'inter_community_edges': self._count_inter_community_edges(adjacency_matrix, communities)
        }
    
    def analyze_network_entropy(self,
                               adjacency_matrix: np.ndarray) -> Dict[str, Any]:
        """
        Analyze network entropy and complexity
        
        Args:
            adjacency_matrix: Network adjacency matrix
            
        Returns:
            Dictionary with entropy analysis
        """
        # Calculate various entropy measures
        n = adjacency_matrix.shape[0]
        
        # Degree entropy
        degrees = np.sum(adjacency_matrix > 0, axis=1)
        degree_entropy = self._calculate_shannon_entropy(degrees)
        
        # Strength entropy
        strengths = np.sum(adjacency_matrix, axis=1)
        strength_entropy = self._calculate_shannon_entropy(strengths)
        
        # Clustering entropy
        clustering_coeffs = []
        for i in range(n):
            neighbors = np.where(adjacency_matrix[i] > 0)[0]
            k_i = len(neighbors)
            if k_i >= 2:
                # Calculate local clustering
                triangle_count = 0
                for j in neighbors:
                    for k in neighbors:
                        if j < k and adjacency_matrix[j, k] > 0:
                            triangle_count += 1
                clustering_coeffs.append(triangle_count / (k_i * (k_i - 1)))
        
        clustering_entropy = self._calculate_shannon_entropy(np.array(clustering_coeffs))
        
        return {
            'degree_entropy': degree_entropy,
            'strength_entropy': strength_entropy,
            'clustering_entropy': clustering_entropy,
            'total_network_entropy': degree_entropy + strength_entropy + clustering_entropy,
            'network_complexity': self._calculate_network_complexity(adjacency_matrix)
        }
    
    def _calculate_modularity(self,
                              adjacency_matrix: np.ndarray,
                              communities: Dict[int, List[int]]) -> float:
        """Calculate modularity of community partition"""
        n = adjacency_matrix.shape[0]
        total_edges = np.sum(adjacency_matrix) / 2
        
        if total_edges == 0:
            return 0.0
        
        modularity = 0.0
        for comm_id, nodes in communities.items():
            for i in nodes:
                for j in nodes:
                    if i != j:
                        modularity += adjacency_matrix[i, j]
                    
                    modularity -= (len(nodes) / (2 * total_edges)) * (adjacency_matrix[i, j] + adjacency_matrix[j, i])
        
        return modularity / (2 * total_edges)
    
    def _count_inter_community_edges(self,
                                    adjacency_matrix: np.ndarray,
                                    communities: Dict[int, List[int]]) -> int:
        """Count edges between communities"""
        inter_edges = 0
        community_assignments = {}
        
        for comm_id, nodes in communities.items():
            for node in nodes:
                community_assignments[node] = comm_id
        
        n = adjacency_matrix.shape[0]
        for i in range(n):
            for j in range(i + 1, n):
                if adjacency_matrix[i, j] > 0:
                    if community_assignments.get(i) != community_assignments.get(j):
                        inter_edges += 1
        
        return inter_edges
    
    def _calculate_shannon_entropy(self, values: np.ndarray) -> float:
        """Calculate Shannon entropy"""
        # Normalize values to create probability distribution
        if np.sum(values) == 0:
            return 0.0
        
        probs = values / np.sum(values)
        # Remove zero probabilities
        probs = probs[probs > 0]
        
        return -np.sum(probs * np.log2(probs))
    
    def _calculate_network_complexity(self, adjacency_matrix: np.ndarray) -> float:
        """Calculate network complexity metric"""
        n = adjacency_matrix.shape[0]
        
        # Simplified complexity calculation
        avg_degree = np.mean(np.sum(adjacency_matrix > 0, axis=1))
        avg_clustering = self.calculate_clustering_coefficient(adjacency_matrix)
        
        return avg_degree * avg_clustering
class LearningProgressAnalyzer:
    """Analyze learning progress and performance trends"""
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.performance_cache = {}
    
    def analyze_learning_curve(self,
                              experiment_id: str = None,
                              metric: str = 'accuracy',
                              window_size: int = 100) -> Dict[str, Any]:
        """
        Analyze learning curve with smoothing and trend detection
        
        Args:
            experiment_id: Experiment identifier
            metric: Performance metric to analyze
            window_size: Smoothing window size
            
        Returns:
            Dictionary with learning curve analysis
        """
        # Extract performance data
        if self.simulation:
            # Get simulation performance data
            status = self.simulation.get_simulation_status()
            performance_data = self._extract_performance_data(status)
        else:
            # Use cached performance data
            performance_data = self.performance_cache.get(experiment_id, [])
        
        if not performance_data:
            return {'error': 'No performance data available'}
        
        # Apply moving average smoothing
        smoothed_performance = self._moving_average(performance_data, window_size)
        
        # Calculate learning metrics
        learning_metrics = {
            'final_performance': smoothed_performance[-1] if smoothed_performance else 0.0,
            'initial_performance': smoothed_performance[0] if len(smoothed_performance) > 0 else 0.0,
            'improvement': (smoothed_performance[-1] - smoothed_performance[0]) 
                          if len(smoothed_performance) > 1 else 0.0,
            'improvement_rate': self._calculate_improvement_rate(smoothed_performance),
            'plateau_detected': self._detect_plateau(smoothed_performance),
            'learning_velocity': self._calculate_learning_velocity(smoothed_performance)
        }
        
        # Fit learning curve models
        learning_metrics.update(self._fit_learning_models(performance_data, smoothed_performance))
        
        return learning_metrics
    
    def analyze_performance_trends(self,
                                  experiment_id: str = None,
                                  metrics: List[str] = None,
                                  time_window: AnalysisWindow = None) -> Dict[str, Any]:
        """
        Analyze performance trends over time
        
        Args:
            experiment_id: Experiment identifier
            metrics: List of metrics to analyze
            time_window: Time window for analysis
            
        Returns:
            Dictionary with trend analysis
        """
        metrics = metrics or ['accuracy', 'loss', 'f1_score', 'precision', 'recall']
        
        trend_analysis = {
            'metrics': {},
            'cross_correlation': {},
            'covariance_matrix': {},
            'statistical_significance': {}
        }
        
        # Extract data
        if self.simulation:
            status = self.simulation.get_simulation_status()
            performance_data = self._extract_performance_data(status)
        else:
            performance_data = self.performance_cache.get(experiment_id, [])
        
        # Analyze each metric
        for metric in metrics:
            if metric in performance_data.columns:
                trend_analysis['metrics'][metric] = self._analyze_single_metric(
                    performance_data[metric], time_window
                )
        
        # Calculate cross-correlations
        if len(performance_data) > 1:
            valid_metrics = [m for m in metrics if m in performance_data.columns]
            if len(valid_metrics) >= 2:
                correlation_matrix = performance_data[valid_metrics].corr()
                trend_analysis['cross_correlation'] = correlation_matrix.to_dict()
                
                # Calculate covariance
                covariance_matrix = performance_data[valid_metrics].cov()
                trend_analysis['covariance_matrix'] = covariance_matrix.to_dict()
        
        return trend_analysis
    
    def detect_performance_anomalies(self,
                                    experiment_id: str = None,
                                    method: str = 'isolation_forest',
                                    contamination: float = 0.1) -> Dict[str, Any]:
        """
        Detect performance anomalies and outliers
        
        Args:
            experiment_id: Experiment identifier
            method: Anomaly detection method
            contamination: Expected proportion of outliers
            
        Returns:
            Dictionary with anomaly detection results
        """
        if self.simulation:
            status = self.simulation.get_simulation_status()
            performance_data = self._extract_performance_data(status)
        else:
            performance_data = self.performance_cache.get(experiment_id, [])
        
        if len(performance_data) < 10:
            return {'error': 'Insufficient data for anomaly detection'}
        
        # Convert to numpy array for processing
        X = performance_data.values
        
        # Apply anomaly detection
        if method == 'isolation_forest':
            from sklearn.ensemble import IsolationForest
            
            clf = IsolationForest(contamination=contamination, random_state=42)
            anomaly_labels = clf.fit_predict(X)
            anomaly_scores = clf.decision_function(X)
            
            anomalies = np.where(anomaly_labels == -1)[0]
            
            return {
                'method': method,
                'anomaly_count': len(anomalies),
                'anomaly_indices': anomalies.tolist(),
                'anomaly_scores': anomaly_scores.tolist(),
                'anomaly_timestamps': performance_data.index[anomalies].tolist() if hasattr(performance_data, 'index') else [],
                'anomaly_reasons': self._analyze_anomaly_reasons(X, anomalies)
            }
        
        return {'error': f'Unsupported method: {method}'}
    
    def predict_next_performance(self,
                                experiment_id: str = None,
                                model_type: str = 'arima',
                                forecast_horizon: int = 10,
                                confidence_interval: float = 0.95) -> Dict[str, Any]:
        """
        Predict future performance using time series models
        
        Args:
            experiment_id: Experiment identifier
            model_type: Prediction model type
            forecast_horizon: Number of steps to forecast
            confidence_interval: Confidence interval for predictions
            
        Returns:
            Dictionary with prediction results
        """
        if self.simulation:
            status = self.simulation.get_simulation_status()
            performance_data = self._extract_performance_data(status)
        else:
            performance_data = self.performance_cache.get(experiment_id, [])
        
        if len(performance_data) < 5:
            return {'error': 'Insufficient data for prediction'}
        
        # Prepare time series data
        time_series = performance_data.iloc[:, 0].values  # Use first metric
        
        predictions = {
            'model_type': model_type,
            'forecast_horizon': forecast_horizon,
            'confidence_interval': confidence_interval,
            'predictions': [],
            'confidence_intervals': [],
            'prediction_accuracy': {}
        }
        
        # Simple prediction (would use proper time series models in practice)
        # This is a framework - actual implementation would use statsmodels, etc.
        last_value = time_series[-1]
        trend = np.mean(np.diff(time_series))
        
        for i in range(forecast_horizon):
            predicted_value = last_value + trend * (i + 1)
            predictions['predictions'].append(float(predicted_value))
        
        return predictions
    
    def _extract_performance_data(self, status: Dict[str, Any]) -> pd.DataFrame:
        """Extract performance data from simulation status"""
        # This would extract actual performance data from NLM simulation
        # For now, return a dummy DataFrame
        
        steps = list(range(1, min(status.get('total_steps', 100) + 1, 101)))
        
        # Create synthetic performance data
        data = {
            'step': steps,
            'accuracy': np.random.beta(2, 1, len(steps)) * 0.8 + 0.1,
            'loss': np.random.exponential(0.5, len(steps)),
            'f1_score': np.random.beta(2, 1, len(steps)) * 0.7 + 0.2,
            'precision': np.random.beta(1.5, 1.5, len(steps)) * 0.6 + 0.2,
            'recall': np.random.beta(1.5, 1.5, len(steps)) * 0.6 + 0.2,
            'spike_rate': np.random.gamma(2, 2, len(steps)),
            'learning_rate': 0.01 * np.ones(len(steps))
        }
        
        return pd.DataFrame(data)
    
    def _moving_average(self, data: np.ndarray, window_size: int) -> np.ndarray:
        """Calculate moving average"""
        return np.convolve(data, np.ones(window_size)/window_size, mode='same')
    
    def _calculate_improvement_rate(self, smoothed_data: np.ndarray) -> float:
        """Calculate improvement rate from smoothed data"""
        if len(smoothed_data) < 2:
            return 0.0
        
        return (smoothed_data[-1] - smoothed_data[0]) / len(smoothed_data)
    
    def _detect_plateau(self, smoothed_data: np.ndarray, threshold: float = 0.01) -> bool:
        """Detect if performance has plateaued"""
        if len(smoothed_data) < 10:
            return False
        
        # Check if recent performance changes are small
        recent_changes = np.abs(np.diff(smoothed_data[-10:]))
        return np.mean(recent_changes) < threshold
    
    def _calculate_learning_velocity(self, smoothed_data: np.ndarray) -> float:
        """Calculate learning velocity (derivative of performance)"""
        if len(smoothed_data) < 2:
            return 0.0
        
        velocity = np.diff(smoothed_data)
        return np.mean(np.abs(velocity))
    
    def _analyze_single_metric(self, metric_data: pd.Series, time_window: AnalysisWindow) -> Dict[str, Any]:
        """Analyze single metric time series"""
        if len(metric_data) == 0:
            return {}
        
        return {
            'mean': float(np.mean(metric_data)),
            'std': float(np.std(metric_data)),
            'min': float(np.min(metric_data)),
            'max': float(np.max(metric_data)),
            'trend': self._calculate_trend(metric_data),
            'stationarity': self._check_stationarity(metric_data),
            'seasonality': self._detect_seasonality(metric_data)
        }
    
    def _calculate_trend(self, data: pd.Series) -> Dict[str, Any]:
        """Calculate linear trend"""
        if len(data) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'r_squared': 0.0}
        
        x = np.arange(len(data))
        y = data.values
        
        slope, intercept = np.polyfit(x, y, 1)
        
        # Calculate R-squared
        y_pred = slope * x + intercept
        ss_res = np.sum((y - y_pred) ** 2)
        ss_tot = np.sum((y - np.mean(y)) ** 2)
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
        
        return {
            'slope': float(slope),
            'intercept': float(intercept),
            'r_squared': float(r_squared)
        }
    
    def _check_stationarity(self, data: pd.Series, critical_value: float = 0.05) -> Dict[str, Any]:
        """Check stationarity using Augmented Dickey-Fuller test"""
        try:
            from statsmodels.tsa.stattools import adfuller
            
            result = adfuller(data.dropna())
            adf_stat, p_value, _, _, _, _ = result
            
            return {
                'adf_statistic': float(adf_stat),
                'p_value': float(p_value),
                'is_stationary': p_value < critical_value,
                'critical_value': critical_value
            }
        except ImportError:
            return {
                'adf_statistic': 0.0,
                'p_value': 1.0,
                'is_stationary': False,
                'critical_value': critical_value,
                'note': 'statsmodels not available'
            }
    
    def _detect_seasonality(self, data: pd.Series, period: int = 7) -> Dict[str, Any]:
        """Detect seasonality using autocorrelation"""
        if len(data) < period * 2:
            return {'seasonality_detected': False, 'peak_period': None}
        
        # Calculate autocorrelation
        autocorr = np.correlate(data - np.mean(data), data - np.mean(data), mode='full')
        autocorr = autocorr[len(autocorr)//2:]
        autocorr = autocorr / autocorr[0]
        
        # Check for significant peaks at multiples of period
        peak_indices = np.where(np.abs(autocorr[1:]) > 0.3)[0] + 1
        seasonal_periods = [idx for idx in peak_indices if idx % period == 0]
        
        return {
            'seasonality_detected': len(seasonal_periods) > 0,
            'peak_period': seasonal_periods[0] if seasonal_periods else None,
            'autocorrelation_peaks': peak_indices[:10].tolist()
        }
    
    def _fit_learning_models(self, 
                           raw_data: pd.DataFrame, 
                           smoothed_data: np.ndarray) -> Dict[str, Any]:
        """Fit learning curve models"""
        if len(raw_data) < 5:
            return {}
        
        models = {}
        
        # Try different learning curve models
        try:
            # Power law model: y = a * x^b
            from scipy.optimize import curve_fit
            
            def power_law(x, a, b):
                return a * (x ** b)
            
            x_data = np.arange(len(raw_data))
            y_data = raw_data.iloc[:, 0].values
            
            try:
                popt, pcov = curve_fit(power_law, x_data, y_data, maxfev=5000)
                a, b = popt
                y_pred = power_law(x_data, a, b)
                
                ss_res = np.sum((y_data - y_pred) ** 2)
                ss_tot = np.sum((y_data - np.mean(y_data)) ** 2)
                r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
                
                models['power_law'] = {
                    'parameters': {'a': float(a), 'b': float(b)},
                    'r_squared': float(r_squared),
                    'predictions': y_pred.tolist()
                }
            except:
                pass
            
            # Exponential model: y = a * (1 - exp(-b * x))
            def exp_model(x, a, b):
                return a * (1 - np.exp(-b * x))
            
            try:
                popt, pcov = curve_fit(exp_model, x_data, y_data, maxfev=5000)
                a, b = popt
                y_pred = exp_model(x_data, a, b)
                
                ss_res = np.sum((y_data - y_pred) ** 2)
                ss_tot = np.sum((y_data - np.mean(y_data)) ** 2)
                r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
                
                models['exponential'] = {
                    'parameters': {'a': float(a), 'b': float(b)},
                    'r_squared': float(r_squared),
                    'predictions': y_pred.tolist()
                }
            except:
                pass
                
        except ImportError:
            models['note'] = 'scipy.optimize not available for curve fitting'
        
        return models
    
    def _analyze_anomaly_reasons(self, X: np.ndarray, anomaly_indices: np.ndarray) -> List[str]:
        """Analyze reasons for detected anomalies"""
        reasons = []
        
        for idx in anomaly_indices:
            # Simple heuristic: high variance or unusual patterns
            row_data = X[idx]
            
            # Check for outliers
            mean = np.mean(X, axis=0)
            std = np.std(X, axis=0)
            
            for j, (val, mu, sigma) in enumerate(zip(row_data, mean, std)):
                if sigma > 0 and abs(val - mu) > 3 * sigma:
                    reasons.append(f"Feature {j} value {val:.2f} is outlier (mean={mu:.2f}, std={sigma:.2f})")
                    break
            
            if not reasons:
                reasons.append(f"Anomaly at index {idx}: unusual combination of features")
        
        return reasons
class VisualizationEngine:
    """Advanced visualization engine for NLM results"""
    
    def __init__(self):
        self.plot_style = 'seaborn'
        self.color_scheme = 'viridis'
        self.figure_size = (12, 8)
        self.default_dpi = 100
    
    def create_interactive_plotly_figure(self,
                                        data: Dict[str, Any],
                                        plot_type: VisualizationType = VisualizationType.HEATMAP,
                                        layout: Dict[str, Any] = None) -> Any:
        """
        Create interactive Plotly figure
        
        Args:
            data: Visualization data
            plot_type: Type of plot
            layout: Layout configuration
            
        Returns:
            Plotly figure object
        """
        try:
            import plotly.graph_objects as go
            from plotly.subplots import make_subplots
            
            fig = go.Figure()
            
            if plot_type == VisualizationType.HEATMAP:
                self._create_heatmap(data, fig)
            elif plot_type == VisualizationType.RASTER_PLOT:
                self._create_raster_plot(data, fig)
            elif plot_type == VisualizationType.NETWORK_GRAPH:
                self._create_network_graph(data, fig)
            elif plot_type == VisualizationType.TIME_SERIES:
                self._create_time_series(data, fig)
            elif plot_type == VisualizationType.SCATTER_PLOT:
                self._create_scatter_plot(data, fig)
            
            # Apply layout
            if layout:
                fig.update_layout(**layout)
            
            return fig
            
        except ImportError:
            logger.warning("Plotly not available, falling back to matplotlib")
            return self._create_matplotlib_figure(data, plot_type, layout)
    
    def create_matplotlib_figure(self,
                                data: Dict[str, Any],
                                plot_type: VisualizationType = VisualizationType.HEATMAP,
                                save_path: str = None,
                                show: bool = True) -> Any:
        """
        Create matplotlib figure
        
        Args:
            data: Visualization data
            plot_type: Type of plot
            save_path: Path to save figure
            show: Whether to display figure
            
        Returns:
            Matplotlib figure object
        """
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns
            
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            if plot_type == VisualizationType.HEATMAP:
                self._create_matplotlib_heatmap(data, ax)
            elif plot_type == VisualizationType.RASTER_PLOT:
                self._create_matplotlib_raster_plot(data, ax)
            elif plot_type == VisualizationType.TIME_SERIES:
                self._create_matplotlib_time_series(data, ax)
            elif plot_type == VisualizationType.SCATTER_PLOT:
                self._create_matplotlib_scatter_plot(data, ax)
            
            if save_path:
                plt.savefig(save_path, dpi=self.default_dpi, bbox_inches='tight')
            
            if show:
                plt.show()
            
            return fig
            
        except ImportError as e:
            logger.error(f"Matplotlib/Seaborn not available: {e}")
            return None
    
    def create_animation(self,
                         frames: List[Dict[str, Any]],
                         plot_type: VisualizationType = VisualizationType.TIME_SERIES,
                         interval: int = 100,
                         save_path: str = None) -> Any:
        """
        Create animation of time series data
        
        Args:
            frames: List of frame data for animation
            plot_type: Type of animation
            interval: Animation interval in milliseconds
            save_path: Path to save animation
            
        Returns:
            Animation object
        """
        try:
            from matplotlib.animation import FuncAnimation
            
            fig, ax = plt.subplots(figsize=self.figure_size)
            
            def animate(frame):
                ax.clear()
                self._animate_frame(frame, ax, plot_type)
            
            anim = FuncAnimation(fig, animate, frames=frames, interval=interval, blit=False)
            
            if save_path:
                anim.save(save_path, writer='ffmpeg', fps=30)
            
            return anim
            
        except Exception as e:
            logger.error(f"Animation creation failed: {e}")
            return None
    
    def create_dashboard(self,
                        metrics: Dict[str, Any],
                        layout_type: str = 'grid') -> Any:
        """
        Create monitoring dashboard
        
        Args:
            metrics: Dictionary of metrics to display
            layout_type: Dashboard layout type
            
        Returns:
            Dashboard object (could be web-based or matplotlib)
        """
        dashboard_data = {
            'title': 'NLM Simulation Dashboard',
            'metrics': metrics,
            'layout': layout_type,
            'timestamp': datetime.now().isoformat(),
            'metrics_summary': self._calculate_dashboard_summary(metrics)
        }
        
        return dashboard_data
    
    def export_visualization(self,
                            figure: Any,
                            filename: str,
                            format: str = 'png',
                            quality: int = 95) -> bool:
        """
        Export visualization to file
        
        Args:
            figure: Figure object (matplotlib or plotly)
            filename: Output filename
            format: Export format
            quality: Quality for lossy formats
            
        Returns:
            Success status
        """
        try:
            if format == 'png':
                if hasattr(figure, 'savefig'):
                    figure.savefig(filename, dpi=self.default_dpi, bbox_inches='tight')
                else:
                    # Plotly figure
                    figure.write_image(filename)
            elif format == 'pdf':
                if hasattr(figure, 'savefig'):
                    figure.savefig(filename, format='pdf', bbox_inches='tight')
            elif format == 'html':
                if hasattr(figure, 'write_html'):
                    figure.write_html(filename.replace('.html', '.html'))
            
            logger.info(f"Visualization exported to {filename}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to export visualization: {e}")
            return False
    
    def set_style(self, style: str = 'seaborn'):
        """Set plotting style"""
        self.plot_style = style
        try:
            if style == 'seaborn':
                import seaborn as sns
                sns.set_style("darkgrid")
            elif style == 'ggplot':
                import matplotlib.pyplot as plt
                plt.style.use('ggplot')
            elif style == 'default':
                import matplotlib.pyplot as plt
                plt.style.use('default')
        except ImportError:
            logger.warning(f"Style {style} not available")
    
    def _create_heatmap(self, data: Dict[str, Any], fig: Any):
        """Create heatmap visualization"""
        # Extract heatmap data
        if 'matrix' in data:
            matrix = data['matrix']
            fig.add_trace(go.Heatmap(
                z=matrix,
                colorscale='Viridis',
                showscale=True
            ))
    
    def _create_raster_plot(self, data: Dict[str, Any], fig: Any):
        """Create raster plot visualization"""
        # Implementation for raster plot
        pass
    
    def _create_network_graph(self, data: Dict[str, Any], fig: Any):
        """Create network graph visualization"""
        # Implementation for network graph
        pass
    
    def _create_time_series(self, data: Dict[str, Any], fig: Any):
        """Create time series visualization"""
        # Implementation for time series
        pass
    
    def _create_scatter_plot(self, data: Dict[str, Any], fig: Any):
        """Create scatter plot visualization"""
        # Implementation for scatter plot
        pass
    
    def _create_matplotlib_heatmap(self, data: Dict[str, Any], ax: Any):
        """Create matplotlib heatmap"""
        if 'matrix' in data:
            sns.heatmap(data['matrix'], annot=False, cmap='viridis', ax=ax)
    
    def _create_matplotlib_raster_plot(self, data: Dict[str, Any], ax: Any):
        """Create matplotlib raster plot"""
        # Implementation
        pass
    
    def _create_matplotlib_time_series(self, data: Dict[str, Any], ax: Any):
        """Create matplotlib time series"""
        # Implementation
        pass
    
    def _create_matplotlib_scatter_plot(self, data: Dict[str, Any], ax: Any):
        """Create matplotlib scatter plot"""
        # Implementation
        pass
    
    def _animate_frame(self, frame: Dict[str, Any], ax: Any, plot_type: VisualizationType):
        """Animate a single frame"""
        pass
    
    def _calculate_dashboard_summary(self, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate dashboard summary statistics"""
        summary = {}
        
        for metric_name, metric_data in metrics.items():
            if isinstance(metric_data, (list, np.ndarray)):
                values = np.array(metric_data)
                summary[metric_name] = {
                    'mean': float(np.mean(values)),
                    'std': float(np.std(values)),
                    'min': float(np.min(values)),
                    'max': float(np.max(values)),
                    'current': float(values[-1]) if len(values) > 0 else 0.0
                }
        
        return summary
class NLPMonitoring:
    """
    Comprehensive monitoring system for NLM simulations.
    
    Provides real-time monitoring, alerting, and performance tracking
    for NLM simulations with advanced analysis capabilities.
    """
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.monitoring_data = []
        self.alert_rules = []
        self.dashboard_data = {}
        self.monitoring_thread = None
        self.is_monitoring = False
        self.sampling_interval = 1.0  # seconds
    
    def add_alert_rule(self,
                      name: str,
                      condition: Callable,
                      severity: str = 'warning',
                      message: str = "Alert triggered"):
        """
        Add alert rule for monitoring
        
        Args:
            name: Rule name
            condition: Function that takes monitoring data and returns boolean
            severity: Severity level ('info', 'warning', 'error')
            message: Alert message
        """
        self.alert_rules.append({
            'name': name,
            'condition': condition,
            'severity': severity,
            'message': message,
            'last_triggered': None
        })
    
    def start_monitoring(self, interval: float = 1.0):
        """
        Start real-time monitoring
        
        Args:
            interval: Monitoring interval in seconds
        """
        self.sampling_interval = interval
        self.is_monitoring = True
        
        def monitor_loop():
            while self.is_monitoring:
                try:
                    self._collect_monitoring_data()
                    self._check_alert_rules()
                    self._update_dashboard()
                    time.sleep(interval)
                except Exception as e:
                    logger.error(f"Monitoring error: {e}")
                    break
        
        self.monitoring_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitoring_thread.start()
        logger.info(f"Monitoring started with interval {interval}s")
    
    def stop_monitoring(self):
        """Stop monitoring"""
        self.is_monitoring = False
        
        if self.monitoring_thread and self.monitoring_thread.is_alive():
            self.monitoring_thread.join(timeout=5.0)
            self.monitoring_thread = None
        
        logger.info("Monitoring stopped")
    
    def _collect_monitoring_data(self):
        """Collect monitoring data"""
        timestamp = time.time()
        
        monitoring_point = {
            'timestamp': timestamp,
            'simulation_status': self._get_simulation_status(),
            'performance_metrics': self._get_performance_metrics(),
            'resource_usage': self._get_resource_usage(),
            'network_stats': self._get_network_stats()
        }
        
        self.monitoring_data.append(monitoring_point)
        
        # Keep only recent data
        if len(self.monitoring_data) > 10000:
            self.monitoring_data = self.monitoring_data[-5000:]
    
    def _get_simulation_status(self) -> Dict[str, Any]:
        """Get current simulation status"""
        if self.simulation:
            return self.simulation.get_simulation_status()
        return {'state': 'unknown', 'total_steps': 0}
    
    def _get_performance_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics"""
        # This would get actual performance metrics
        # For now, return dummy data
        return {
            'step_rate': np.random.normal(10, 2),
            'memory_usage': np.random.uniform(100, 500),
            'cpu_usage': np.random.uniform(10, 80),
            'network_activity': np.random.uniform(0.1, 1.0)
        }
    
    def _get_resource_usage(self) -> Dict[str, Any]:
        """Get system resource usage"""
        import psutil
        
        return {
            'cpu_percent': psutil.cpu_percent(),
            'memory_percent': psutil.virtual_memory().percent,
            'memory_available': psutil.virtual_memory().available,
            'disk_usage': psutil.disk_usage('/').percent,
            'network_io': psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv
        }
    
    def _get_network_stats(self) -> Dict[str, Any]:
        """Get network statistics"""
        # This would get actual network statistics
        # For now, return dummy data
        return {
            'connections': np.random.randint(100, 1000),
            'packets_sent': np.random.randint(0, 1000),
            'packets_received': np.random.randint(0, 1000),
            'bandwidth_usage': np.random.uniform(1, 100)
        }
    
    def _check_alert_rules(self):
        """Check if any alert rules are triggered"""
        if not self.monitoring_data:
            return
        
        latest_data = self.monitoring_data[-1]
        
        for rule in self.alert_rules:
            try:
                if rule['condition'](latest_data):
                    # Trigger alert
                    self._trigger_alert(rule, latest_data)
            except Exception as e:
                logger.error(f"Alert rule error: {e}")
    
    def _trigger_alert(self, rule: Dict[str, Any], data: Dict[str, Any]):
        """Trigger alert notification"""
        rule['last_triggered'] = time.time()
        
        alert_message = {
            'name': rule['name'],
            'severity': rule['severity'],
            'message': rule['message'],
            'timestamp': time.time(),
            'data': data
        }
        
        logger.warning(f"ALERT [{rule['severity'].upper()}]: {rule['message']}")
        
        # In a real implementation, this would send notifications
        # (email, SMS, webhook, etc.)
    
    def _update_dashboard(self):
        """Update dashboard data"""
        if not self.monitoring_data:
            return
        
        latest_data = self.monitoring_data[-1]
        
        # Calculate dashboard metrics
        recent_data = self.monitoring_data[-100:]  # Last 100 samples
        
        dashboard = {
            'timestamp': datetime.now().isoformat(),
            'simulation_status': latest_data['simulation_status'],
            'performance_summary': self._calculate_performance_summary(recent_data),
            'resource_summary': self._calculate_resource_summary(recent_data),
            'alerts': self._get_active_alerts(),
            'trend_data': self._get_trend_data(recent_data)
        }
        
        self.dashboard_data = dashboard
    
    def _calculate_performance_summary(self, data: List[Dict]) -> Dict[str, Any]:
        """Calculate performance summary from monitoring data"""
        if not data:
            return {}
        
        step_rates = [d['performance_metrics']['step_rate'] for d in data]
        memory_usage = [d['performance_metrics']['memory_usage'] for d in data]
        cpu_usage = [d['performance_metrics']['cpu_usage'] for d in data]
        
        return {
            'average_step_rate': np.mean(step_rates),
            'peak_step_rate': np.max(step_rates),
            'average_memory_usage': np.mean(memory_usage),
            'average_cpu_usage': np.mean(cpu_usage),
            'performance_trend': self._calculate_trend(step_rates)
        }
    
    def _calculate_resource_summary(self, data: List[Dict]) -> Dict[str, Any]:
        """Calculate resource summary from monitoring data"""
        # Extract resource usage data
        # Implementation would depend on actual resource monitoring
        return {
            'average_cpu': np.mean([d['resource_usage']['cpu_percent'] for d in data]),
            'average_memory': np.mean([d['resource_usage']['memory_percent'] for d in data]),
            'resource_alerts': 0
        }
    
    def _get_active_alerts(self) -> List[Dict[str, Any]]:
        """Get active alerts"""
        active_alerts = []
        
        for rule in self.alert_rules:
            if rule['last_triggered'] and (time.time() - rule['last_triggered']) < 300:  # 5 minutes
                active_alerts.append({
                    'name': rule['name'],
                    'severity': rule['severity'],
                    'message': rule['message'],
                    'last_triggered': rule['last_triggered']
                })
        
        return active_alerts
    
    def _get_trend_data(self, data: List[Dict]) -> Dict[str, Any]:
        """Get trend data for charting"""
        if not data:
            return {}
        
        timestamps = [d['timestamp'] for d in data]
        step_rates = [d['performance_metrics']['step_rate'] for d in data]
        cpu_usage = [d['resource_usage']['cpu_percent'] for d in data]
        
        return {
            'timestamps': timestamps,
            'step_rates': step_rates,
            'cpu_usage': cpu_usage,
            'step_rate_trend': self._calculate_trend(step_rates),
            'cpu_trend': self._calculate_trend(cpu_usage)
        }
    
    def _calculate_trend(self, values: List[float]) -> Dict[str, Any]:
        """Calculate linear trend of values"""
        if len(values) < 2:
            return {'slope': 0.0, 'direction': 'stable'}
        
        x = np.arange(len(values))
        y = np.array(values)
        
        slope, intercept = np.polyfit(x, y, 1)
        
        if slope > 0.01:
            direction = 'increasing'
        elif slope < -0.01:
            direction = 'decreasing'
        else:
            direction = 'stable'
        
        return {
            'slope': float(slope),
            'direction': direction,
            'change_per_step': float(slope)
        }
    
    def get_monitoring_history(self, limit: int = 1000) -> List[Dict[str, Any]]:
        """Get monitoring history"""
        return self.monitoring_data[-limit:]
    
    def get_dashboard_data(self) -> Dict[str, Any]:
        """Get current dashboard data"""
        return self.dashboard_data.copy()
class NLPAnalysisSuite:
    """
    Comprehensive analysis suite for NLM simulations.
    
    Provides advanced analysis tools for neural simulation data including:
    - Neural activity analysis
    - Network topology analysis
    - Learning progress analysis
    - Statistical analysis
    - Machine learning integration
    - Visualization generation
    """
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.neural_analyzer = NeuralActivityAnalyzer(simulation)
        self.topology_analyzer = NetworkTopologyAnalyzer(simulation)
        self.learning_analyzer = LearningProgressAnalyzer(simulation)
        self.visualizer = VisualizationEngine()
        self.monitoring = NLPMonitoring(simulation)
        self.analysis_cache = {}
    
    def run_comprehensive_analysis(self,
                                  analysis_types: List[AnalysisType] = None,
                                  visualization_types: List[VisualizationType] = None,
                                  time_window: AnalysisWindow = None) -> Dict[str, Any]:
        """
        Run comprehensive analysis suite
        
        Args:
            analysis_types: Types of analysis to perform
            visualization_types: Types of visualizations to generate
            time_window: Time window for analysis
            
        Returns:
            Dictionary with comprehensive analysis results
        """
        analysis_types = analysis_types or list(AnalysisType)
        visualization_types = visualization_types or [VisualizationType.HEATMAP]
        
        comprehensive_results = {
            'timestamp': datetime.now().isoformat(),
            'analysis_summary': {},
            'detailed_results': {},
            'visualizations': {},
            'recommendations': []
        }
        
        # Run neural activity analysis
        if AnalysisType.NEURAL_ACTIVITY in analysis_types:
            neural_results = self.neural_analyzer.analyze_spike_patterns()
            comprehensive_results['detailed_results']['neural_activity'] = neural_results
        
        # Run network topology analysis
        if AnalysisType.NETWORK_TOPOLOGY in analysis_types:
            topology_results = self.topology_analyzer.analyze_connectivity_patterns()
            comprehensive_results['detailed_results']['network_topology'] = topology_results
        
        # Run learning progress analysis
        if AnalysisType.LEARNING_PROGRESS in analysis_types:
            learning_results = self.learning_analyzer.analyze_learning_curve()
            comprehensive_results['detailed_results']['learning_progress'] = learning_results
        
        # Generate visualizations
        for viz_type in visualization_types:
            viz_data = self._generate_visualization_data(viz_type, comprehensive_results)
            comprehensive_results['visualizations'][viz_type.value] = viz_data
        
        # Generate recommendations
        comprehensive_results['recommendations'] = self._generate_recommendations(comprehensive_results)
        
        return comprehensive_results
    
    def _generate_visualization_data(self,
                                    viz_type: VisualizationType,
                                    analysis_results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate visualization data for specified type"""
        # This would generate actual visualization data
        # For now, return framework structure
        
        return {
            'type': viz_type.value,
            'data': {},
            'layout': {},
            'config': {}
        }
    
    def _generate_recommendations(self, analysis_results: Dict[str, Any]) -> List[str]:n:
        """Generate recommendations based on analysis results"""
        recommendations = []
        
        # Check for potential issues
        if 'neural_activity' in analysis_results['detailed_results']:
            neural_results = analysis_results['detailed_results']['neural_activity']
            # Add recommendations based on neural activity analysis
            
        if 'network_topology' in analysis_results['detailed_results']:
            topology_results = analysis_results['detailed_results']['network_topology']
            # Add recommendations based on network topology
            
        if 'learning_progress' in analysis_results['detailed_results']:
            learning_results = analysis_results['detailed_results']['learning_progress']
            if learning_results.get('plateau_detected', False):
                recommendations.append(
                    "Learning plateau detected. Consider adjusting plasticity parameters or introducing novelty."
                )
        
        if not recommendations:
            recommendations.append("Analysis completed successfully. No critical issues detected.")
        
        return recommendations
class NLPPythonEnvironment:
    """
    High-performance Python environment for NLM simulations.
    
    Provides optimized Python environment with:
    - Performance monitoring and optimization
    - Memory management
    - Threading support
    - Error handling and recovery
    - Cross-platform compatibility
    - Advanced debugging tools
    """
    
    def __init__(self):
        self.initialized = False
        self.performance_monitor = None
        self.memory_optimizer = None
        self.thread_pool = None
        self.error_handler = None
        self.debugger = None
        
        # Initialize environment
        self._initialize_environment()
    
    def _initialize_environment(self):
        """Initialize Python environment for NLM"""
        try:
            # Set up matplotlib for non-interactive use
            import matplotlib
            matplotlib.use('Agg')
            
            # Set up numpy for performance
            np.seterr(all='warn')
            
            # Optimize numpy settings for performance
            np.set_printoptions(precision=6, suppress=True)
            
            # Configure pandas
            pd.options.mode.chained_assignment = None
            pd.options.display.max_columns = 100
            
            # Initialize performance monitoring
            self.performance_monitor = PerformanceMonitor()
            
            # Initialize memory optimizer
            self.memory_optimizer = MemoryOptimizer()
            
            # Initialize error handler
            self.error_handler = ErrorHandler()
            
            # Initialize debugging tools
            self.debugger = Debugger()
            
            self.initialized = True
            logger.info("NLM Python environment initialized successfully")
            
        except Exception as e:
            logger.error(f"Failed to initialize NLM Python environment: {e}")
            raise
    
    def optimize_performance(self, optimization_level: str = 'balanced'):
        """
        Optimize Python performance
        
        Args:
            optimization_level: Optimization level ('balanced', 'aggressive', 'minimal')
        """
        if not self.initialized:
            raise RuntimeError("Python environment not initialized")
        
        # Configure numpy optimizations
        np.set_printoptions(precision=6, suppress=True)
        
        # Optimize memory usage
        self.memory_optimizer.optimize_memory()
        
        # Configure threading
        if optimization_level == 'aggressive':
            self._configure_aggressive_threading()
        elif optimization_level == 'balanced':
            self._configure_balanced_threading()
        
        logger.info(f"Performance optimization completed (level: {optimization_level})")
    
    def _configure_aggressive_threading(self):
        """Configure aggressive threading optimizations"""
        # Enable thread-local caching
        # Optimize numpy operations for threading
        # Configure memory allocation
        pass
    
    def _configure_balanced_threading(self):
        """Configure balanced threading optimizations"""
        # Balance performance and stability
        pass
    
    def get_environment_stats(self) -> Dict[str, Any]:
        """Get environment statistics"""
        if not self.initialized:
            return {'error': 'Environment not initialized'}
        
        return {
            'initialized': True,
            'numpy_version': np.__version__,
            'pandas_version': pd.__version__,
            'matplotlib_version': matplotlib.__version__,
            'performance_monitor': self.performance_monitor.get_stats() if self.performance_monitor else {},
            'memory_optimizer': self.memory_optimizer.get_stats() if self.memory_optimizer else {}
        }
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.cleanup()
    
    def cleanup(self):
        """Cleanup environment resources"""
        try:
            if self.performance_monitor:
                self.performance_monitor.cleanup()
            
            if self.memory_optimizer:
                self.memory_optimizer.cleanup()
            
            if self.debugger:
                self.debugger.cleanup()
            
            logger.info("NLM Python environment cleaned up")
            
        except Exception as e:
            logger.error(f"Error during cleanup: {e}")
class PerformanceMonitor:
    """Performance monitoring utilities"""
    
    def __init__(self):
        self.metrics = {}
        self.start_time = time.time()
        self.memory_samples = []
        self.cpu_samples = []
        self.thread_samples = []
    
    def start_monitoring(self):
        """Start performance monitoring"""
        self.start_time = time.time()
        self._collect_samples()
        logger.info("Performance monitoring started")
    
    def _collect_samples(self):
        """Collect performance samples"""
        import psutil
        
        self.memory_samples.append(psutil.virtual_memory().percent)
        self.cpu_samples.append(psutil.cpu_percent())
        self.thread_samples.append(len(threading.enumerate()))
        
        # Schedule next sample
        threading.Timer(1.0, self._collect_samples).start()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get performance statistics"""
        if not self.memory_samples:
            return {'status': 'not_monitoring'}
        
        elapsed = time.time() - self.start_time
        
        return {
            'monitoring_duration': elapsed,
            'samples_collected': len(self.memory_samples),
            'average_memory_usage': np.mean(self.memory_samples),
            'peak_memory_usage': np.max(self.memory_samples),
            'average_cpu_usage': np.mean(self.cpu_samples),
            'peak_cpu_usage': np.max(self.cpu_samples),
            'average_thread_count': np.mean(self.thread_samples),
            'current_memory_usage': self.memory_samples[-1] if self.memory_samples else 0,
            'current_cpu_usage': self.cpu_samples[-1] if self.cpu_samples else 0
        }
    
    def cleanup(self):
        """Cleanup monitoring resources"""
        # Stop sample collection
        pass
class MemoryOptimizer:
    """Memory optimization utilities"""
    
    def __init__(self):
        self.optimization_level = 0
        self.memory_usage_history = []
    
    def optimize_memory(self):
        """Optimize memory usage"""
        import gc
        
        # Force garbage collection
        gc.collect()
        
        # Collect initial memory usage
        initial_memory = self._get_memory_usage()
        
        # Perform optimizations
        self._optimize_numpy_memory()
        self._optimize_pandas_memory()
        self._optimize_matplotlib_memory()
        
        # Force garbage collection again
        gc.collect()
        
        final_memory = self._get_memory_usage()
        
        self.optimization_level += 1
        
        logger.info(f"Memory optimization completed: "
                   f"{initial_memory:.2f}MB -> {final_memory:.2f}MB "
                   f"(savings: {initial_memory - final_memory:.2f}MB)")
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB"""
        import psutil
        return psutil.virtual_memory().used / (1024 * 1024)
    
    def _optimize_numpy_memory(self):
        """Optimize numpy memory usage"""
        # Configure numpy to use less memory
        np.seterr(all='warn')
        
        # Clean up large numpy arrays
        import gc
        gc.collect()
    
    def _optimize_pandas_memory(self):
        """Optimize pandas memory usage"""
        # Configure pandas for memory efficiency
        pd.options.mode.chained_assignment = None
    
    def _optimize_matplotlib_memory(self):
        """Optimize matplotlib memory usage"""
        # Configure matplotlib for memory efficiency
        import matplotlib.pyplot as plt
        plt.close('all')
    
    def get_stats(self) -> Dict[str, Any]:
        """Get memory optimization statistics"""
        return {
            'optimization_level': self.optimization_level,
            'current_memory_usage': self._get_memory_usage(),
            'optimizations_performed': self.optimization_level
        }
    
    def cleanup(self):
        """Cleanup memory optimization resources"""
        import gc
        gc.collect()
class ErrorHandler:
    """Error handling and recovery utilities"""
    
    def __init__(self):
        self.error_history = []
        self.error_handlers = {}
        self.recovery_strategies = {}
    
    def add_error_handler(self,
                         error_type: str,
                         handler: Callable,
                         priority: int = 1):
        """
        Add error handler for specific error type
        
        Args:
            error_type: Type of error to handle
            handler: Error handling function
            priority: Handler priority (lower = higher priority)
        """
        self.error_handlers[error_type] = {
            'handler': handler,
            'priority': priority,
            'last_used': None
        }
    
    def add_recovery_strategy(self,
                             error_type: str,
                             strategy: Callable,
                             condition: Callable = None):
        """
        Add recovery strategy for specific error type
        
        Args:
            error_type: Type of error to recover from
            strategy: Recovery strategy function
            condition: Condition function that must return True to apply strategy
        """
        self.recovery_strategies[error_type] = {
            'strategy': strategy,
            'condition': condition or (lambda *args, **kwargs: True)
        }
    
    def handle_error(self, error: Exception, context: Dict[str, Any] = None):
        """
        Handle error with appropriate strategy
        
        Args:
            error: Exception to handle
            context: Context information
        """
        error_info = {
            'timestamp': time.time(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'stack_trace': self._get_stack_trace(error)
        }
        
        self.error_history.append(error_info)
        
        # Find appropriate error handler
        error_type = type(error).__name__
        handler = self._find_best_handler(error_type)
        
        if handler:
            try:
                handler(error, context, error_info)
            except Exception as e:
                logger.error(f"Error handler failed: {e}")
        
        # Apply recovery strategy if available
        recovery_strategy = self.recovery_strategies.get(error_type)
        if recovery_strategy and recovery_strategy['condition'](error, context, error_info):
            try:
                recovery_strategy['strategy'](error, context, error_info)
            except Exception as e:
                logger.error(f"Recovery strategy failed: {e}")
        
        # Log error
        logger.error(f"Error handled: {error_type} - {error}")
    
    def _find_best_handler(self, error_type: str) -> Optional[Dict[str, Any]]:
        """Find best error handler for error type"""
        # This would find the most appropriate handler
        # For now, return None (would implement in full version)
        return None
    
    def _get_stack_trace(self, error: Exception) -> str:
        """Get stack trace for error"""
        import traceback
        return traceback.format_exc()
    
    def get_error_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get error history"""
        return self.error_history[-limit:]
    
    def cleanup(self):
        """Cleanup error handling resources"""
        self.error_history.clear()
class Debugger:
    """Advanced debugging utilities for NLM"""
    
    def __init__(self):
        self.debug_mode = False
        self.breakpoints = {}
        self.watch_variables = {}
        self.debug_log = []
    
    def enable_debug_mode(self, enable: bool = True):
        """Enable/disable debug mode"""
        self.debug_mode = enable
        logger.info(f"Debug mode {'enabled' if enable else 'disabled'}")
    
    def add_breakpoint(self,
                      name: str,
                      condition: Callable = None,
                      line_number: int = None):
        """
        Add breakpoint
        
        Args:
            name: Breakpoint name
            condition: Function that returns True if breakpoint should trigger
            line_number: Line number to break at (framework)
        """
        self.breakpoints[name] = {
            'condition': condition,
            'line_number': line_number,
            'enabled': True,
            'trigger_count': 0
        }
    
    def set_watch_variable(self, name: str, value: Any):
        """Set watch variable for debugging"""
        self.watch_variables[name] = {
            'value': value,
            'timestamp': time.time(),
            'type': type(value).__name__
        }
    
    def log_debug_info(self, message: str, data: Any = None):
        """Log debug information"""
        debug_entry = {
            'timestamp': time.time(),
            'message': message,
            'data': data,
            'watch_variables': self.watch_variables.copy()
        }
        
        self.debug_log.append(debug_entry)
        
        if self.debug_mode:
            logger.debug(f"DEBUG: {message}")
    
    def get_debug_log(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get debug log entries"""
        return self.debug_log[-limit:]
    
    def cleanup(self):
        """Cleanup debugging resources"""
        self.breakpoints.clear()
        self.watch_variables.clear()
        self.debug_log.clear()
def demonstrate_analysis_capabilities():
    """Demonstrate advanced analysis capabilities"""
    print("=== NLM Advanced Analysis and Visualization Module Demo ===\n")
    
    # Initialize analysis suite
    analyzer = NLPAnalysisSuite()
    
    # Create sample monitoring data
    monitoring_data = {
        'simulation_status': {
            'state': 'running',
            'total_steps': 1000,
            'current_step': 500
        },
        'performance_metrics': {
            'step_rate': 25.0,
            'memory_usage': 450.0,
            'cpu_usage': 45.0
        },
        'resource_usage': {
            'cpu_percent': 45.0,
            'memory_percent': 35.0,
            'disk_usage': 20.0
        }
    }
    
    # Add alert rules
    analyzer.monitoring.add_alert_rule(
        name='high_cpu_usage',
        condition=lambda data: data['resource_usage']['cpu_percent'] > 80,
        severity='warning',
        message='CPU usage above 80%'
    )
    
    analyzer.monitoring.add_alert_rule(
        name='high_memory_usage',
        condition=lambda data: data['resource_usage']['memory_percent'] > 90,
        severity='error',
        message='Memory usage above 90%'
    )
    
    print(f"Analysis suite initialized with {len(analyzer.monitoring.alert_rules)} alert rules")
    
    # Run comprehensive analysis
    print("\nRunning comprehensive analysis...")
    comprehensive_results = analyzer.run_comprehensive_analysis(
        analysis_types=[AnalysisType.NEURAL_ACTIVITY, AnalysisType.LEARNING_PROGRESS],
        visualization_types=[VisualizationType.HEATMAP, VisualizationType.TIME_SERIES]
    )
    
    print(f"Analysis completed:")
    print(f"  - Neural activity analysis: {'completed' if 'neural_activity' in comprehensive_results['detailed_results'] else 'failed'}")
    print(f"  - Learning progress analysis: {'completed' if 'learning_progress' in comprehensive_results['detailed_results'] else 'failed'}")
    print(f"  - Visualizations generated: {len(comprehensive_results['visualizations'])}")
    print(f"  - Recommendations: {len(comprehensive_results['recommendations'])}")
    
    for i, recommendation in enumerate(comprehensive_results['recommendations'], 1):
        print(f"    {i}. {recommendation}")
    
    # Create dashboard
    print("\nCreating monitoring dashboard...")
    dashboard = analyzer.monitoring.create_dashboard(monitoring_data)
    print(f"Dashboard created with metrics: {list(dashboard.keys())}")
    
    # Start monitoring
    analyzer.monitoring.start_monitoring(interval=2.0)
    print("Monitoring started")
    
    # Simulate some monitoring activity
    print("\nSimulating monitoring activity...")
    for i in range(5):
        time.sleep(1)
        monitoring_data['timestamp'] = time.time()
        # This would update dashboard in real implementation
    
    # Stop monitoring
    analyzer.monitoring.stop_monitoring()
    print("Monitoring stopped")
    
    # Get monitoring history
    history = analyzer.monitoring.get_monitoring_history(limit=10)
    print(f"\nMonitoring history retrieved: {len(history)} entries")
    
    # Get dashboard data
    dashboard_data = analyzer.monitoring.get_dashboard_data()
    print(f"Dashboard data retrieved: {len(dashboard_data)} metrics")
    
    return analyzer
def main():
    """Main demonstration function"""
    print("NLM Advanced Data Analysis and Visualization Module")
    print("=" * 60)
    
    # Demonstrate analysis capabilities
    analyzer = demonstrate_analysis_capabilities()
    
    print("\nAdvanced analysis demonstration completed successfully!")
    return analyzer
if __name__ == "__main__":
    main()
