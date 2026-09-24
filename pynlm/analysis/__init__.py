"""
pynlm.analysis - Analysis and statistics for NLM project

This module provides analysis and statistics including:
- Neural pattern analysis
- Performance metrics
- Statistical analysis
- Comparative analysis
"""

import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple, Union
from scipy import stats
from scipy.signal import find_peaks
import logging
from collections import defaultdict
from sklearn.decomposition import PCA
from sklearn.manifold import TSNE
from sklearn.cluster import KMeans
import warnings

logger = logging.getLogger(__name__)


class NeuralPatternAnalyzer:
    """Analyze neural activity patterns."""
    
    def __init__(self, activity_data: np.ndarray):
        """
        Initialize neural pattern analyzer.
        
        Parameters
        ----------
        activity_data : np.ndarray
            Neural activity data (time x neurons)
        """
        self.activity_data = activity_data
        self.time_steps = activity_data.shape[0]
        self.num_neurons = activity_data.shape[1]
    
    def analyze_spike_patterns(self, spike_data: np.ndarray, 
                             min_height: float = 0.5, distance: int = 10) -> Dict[str, Any]:
        """
        Analyze spike patterns.
        
        Parameters
        ----------
        spike_data : np.ndarray
            Spike data (neurons x time)
        min_height : float, optional
            Minimum height for peak detection, by default 0.5
        distance : int, optional
            Minimum distance between peaks, by default 10
            
        Returns
        -------
        Dict[str, Any]
            Analysis results
        """
        peaks = {}
        for neuron_idx in range(spike_data.shape[0]):
            neuron_spikes = spike_data[neuron_idx]
            peak_indices, _ = find_peaks(neuron_spikes, height=min_height, distance=distance)
            peaks[neuron_idx] = {
                'peak_indices': peak_indices.tolist(),
                'peak_heights': neuron_spikes[peak_indices].tolist(),
                'num_peaks': len(peak_indices)
            }
        
        return {
            'peaks': peaks,
            'total_peaks': sum(len(p['peak_indices']) for p in peaks.values()),
            'neurons_with_peaks': sum(1 for p in peaks.values() if p['num_peaks'] > 0)
        }
    
    def detect_patterns(self, method: str = "pca", n_components: Optional[int] = None) -> Dict[str, Any]:
        """
        Detect patterns in neural activity data.
        
        Parameters
        ----------
        method : str, optional
            Pattern detection method ("pca", "tsne", "kmeans"), by default "pca"
        n_components : int, optional
            Number of components for dimensionality reduction, by default None
            
        Returns
        -------
        Dict[str, Any]
            Pattern detection results
        """
        if method == "pca":
            n_components = n_components or min(50, self.time_steps - 1)
            pca = PCA(n_components=n_components)
            transformed_data = pca.fit_transform(self.activity_data.T)
            explained_variance = pca.explained_variance_ratio_
            
            return {
                'method': method,
                'transformed_data': transformed_data.tolist(),
                'explained_variance': explained_variance.tolist(),
                'components': n_components,
                'total_variance_explained': sum(explained_variance)
            }
        elif method == "tsne":
            n_components = n_components or 2
            tsne = TSNE(n_components=n_components, random_state=42)
            transformed_data = tsne.fit_transform(self.activity_data.T)
            
            return {
                'method': method,
                'transformed_data': transformed_data.tolist(),
                'components': n_components
            }
        elif method == "kmeans":
            n_clusters = n_components or 10
            kmeans = KMeans(n_clusters=n_clusters, random_state=42)
            cluster_labels = kmeans.fit_predict(self.activity_data.T)
            
            return {
                'method': method,
                'cluster_labels': cluster_labels.tolist(),
                'n_clusters': n_clusters,
                'cluster_centers': kmeans.cluster_centers_.tolist()
            }
        else:
            raise ValueError(f"Unsupported method: {method}")


class PerformanceMetrics:
    """Calculate performance metrics."""
    
    def __init__(self, metrics: Dict[str, Any]):
        """
        Initialize performance metrics calculator.
        
        Parameters
        ----------
        metrics : Dict[str, Any]
            Initial metrics
        """
        self.metrics = metrics
    
    def calculate_regression_metrics(self, y_true: np.ndarray, y_pred: np.ndarray) -> Dict[str, float]:
        """
        Calculate regression metrics.
        
        Parameters
        ----------
        y_true : np.ndarray
            True values
        y_pred : np.ndarray
            Predicted values
            
        Returns
        -------
        Dict[str, float]
            Regression metrics
        """
        from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
        
        mse = mean_squared_error(y_true, y_pred)
        mae = mean_absolute_error(y_true, y_pred)
        r2 = r2_score(y_true, y_pred)
        
        return {
            'mse': float(mse),
            'mae': float(mae),
            'r2': float(r2),
            'rmse': float(np.sqrt(mse))
        }


class StatisticalAnalyzer:
    """Statistical analysis utilities."""
    
    def __init__(self, data: Dict[str, Any]):
        """
        Initialize statistical analyzer.
        
        Parameters
        ----------
        data : Dict[str, Any]
            Data for analysis
        """
        self.data = data
    
    def calculate_descriptive_statistics(self, data: np.ndarray) -> Dict[str, float]:
        """
        Calculate descriptive statistics.
        
        Parameters
        ----------
        data : np.ndarray
            Data array
            
        Returns
        -------
        Dict[str, float]
            Descriptive statistics
        """
        return {
            'mean': float(np.mean(data)),
            'median': float(np.median(data)),
            'std': float(np.std(data)),
            'min': float(np.min(data)),
            'max': float(np.max(data)),
            'range': float(np.max(data) - np.min(data)),
            'q25': float(np.percentile(data, 25)),
            'q50': float(np.percentile(data, 50)),
            'q75': float(np.percentile(data, 75)),
            'skew': float(stats.skew(data)),
            'kurtosis': float(stats.kurtosis(data))
        }


class ComparativeAnalyzer:
    """Comparative analysis utilities."""
    
    def __init__(self, data1: np.ndarray, data2: np.ndarray):
        """
        Initialize comparative analyzer.
        
        Parameters
        ----------
        data1 : np.ndarray
            First dataset
        data2 : np.ndarray
            Second dataset
        """
        self.data1 = data1
        self.data2 = data2
    
    def calculate_correlation(self, method: str = "pearson") -> float:
        """
        Calculate correlation between datasets.
        
        Parameters
        ----------
        method : str, optional
            Correlation method ("pearson", "spearman", "kendall"), by default "pearson"
            
        Returns
        -------
        float
            Correlation coefficient
        """
        if method == "pearson":
            return float(np.corrcoef(self.data1.flatten(), self.data2.flatten())[0, 1])
        elif method == "spearman":
            return float(stats.spearmanr(self.data1.flatten(), self.data2.flatten())[0])
        elif method == "kendall":
            return float(stats.kendalltau(self.data1.flatten(), self.data2.flatten())[0])
        else:
            raise ValueError(f"Unsupported method: {method}")


class PatternDetector:
    """Pattern detection utilities."""
    
    def __init__(self, data: np.ndarray):
        """
        Initialize pattern detector.
        
        Parameters
        ----------
        data : np.ndarray
            Data for pattern detection
        """
        self.data = data
    
    def detect_trends(self) -> Dict[str, Any]:
        """
        Detect trends in data.
        
        Returns
        -------
        Dict[str, Any]
            Trend detection results
        """
        # Simple trend detection using rolling mean
        rolling_mean = pd.Series(self.data.flatten()).rolling(window=10, center=True).mean()
        
        # Identify increasing/decreasing trends
        increasing = []
        decreasing = []
        
        for i in range(1, len(rolling_mean)):
            if rolling_mean.iloc[i] > rolling_mean.iloc[i-1]:
                increasing.append(i)
            elif rolling_mean.iloc[i] < rolling_mean.iloc[i-1]:
                decreasing.append(i)
        
        return {
            'increasing_trends': increasing,
            'decreasing_trends': decreasing,
            'trend_strength': abs(len(increasing) - len(decreasing)) / len(rolling_mean),
            'rolling_mean': rolling_mean.tolist()
        }


class CorrelationAnalyzer:
    """Correlation analysis utilities."""
    
    def __init__(self, data: np.ndarray):
        """
        Initialize correlation analyzer.
        
        Parameters
        ----------
        data : np.ndarray
            Data for correlation analysis
        """
        self.data = data
    
    def calculate_matrix_correlation(self, method: str = "pearson") -> Dict[str, Any]:
        """
        Calculate correlation matrix.
        
        Parameters
        ----------
        method : str, optional
            Correlation method ("pearson", "spearman", "kendall"), by default "pearson"
            
        Returns
        -------
        Dict[str, Any]
            Correlation matrix results
        """
        if method == "pearson":
            corr_matrix = np.corrcoef(self.data)
        elif method == "spearman":
            corr_matrix = pd.DataFrame(self.data).corr(method='spearman').values
        elif method == "kendall":
            corr_matrix = pd.DataFrame(self.data).corr(method='kendall').values
        else:
            raise ValueError(f"Unsupported method: {method}")
        
        return {
            'method': method,
            'correlation_matrix': corr_matrix.tolist(),
            'correlation_coefficient_mean': float(np.mean(np.abs(corr_matrix))),
            'correlation_coefficient_std': float(np.std(np.abs(corr_matrix)))
        }


# Example usage function
def example_usage():
    """Example usage of the analysis module."""
    # Generate example data
    np.random.seed(42)
    time_steps = 1000
    neurons = 50
    activity_data = np.random.randn(time_steps, neurons)
    
    # Add some patterns
    for i in range(10):
        t = np.linspace(0, 4*np.pi, time_steps)
        activity_data[:, i] += 2 * np.sin(t + i)
    
    # Create analyzer
    analyzer = NeuralPatternAnalyzer(activity_data)
    
    # Analyze patterns
    patterns = analyzer.detect_patterns("pca")
    
    # Create performance metrics calculator
    metrics = PerformanceMetrics({"example_metric": 0.85})
    
    # Calculate some metrics
    y_true = np.random.randn(100)
    y_pred = y_true + np.random.randn(100) * 0.1
    reg_metrics = metrics.calculate_regression_metrics(y_true, y_pred)
    
    return {
        "activity_data_shape": activity_data.shape,
        "patterns_found": len(patterns.get('transformed_data', [])),
        "regression_metrics": reg_metrics
    }


if __name__ == "__main__":
    print("Testing pynlm.analysis module...")
    
    # Run example usage
    results = example_usage()
    print("Example usage completed successfully!")
    print(f"Activity data shape: {results['activity_data_shape']}")
    print(f"Patterns found: {results['patterns_found']}")
    print(f"Regression metrics: {results['regression_metrics']}")