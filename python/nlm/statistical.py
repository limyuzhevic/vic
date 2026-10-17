#!/usr/bin/env python3
"""
Statistical Analysis Utilities for NLM Neural Data

This module provides advanced statistical analysis functions for neural simulation data,
including descriptive statistics, probability distributions, hypothesis testing,
and machine learning utilities for neural data analysis.
"""

import numpy as np
import scipy.stats as stats
import pandas as pd
import math
from typing import List, Dict, Tuple, Optional, Union, Any, Generator
from dataclasses import dataclass
from enum import Enum
import warnings

warnings.filterwarnings('ignore')

class StatisticalMethod(Enum):
    """Statistical analysis methods"""
    DESCRIPTIVE = "descriptive"
    DISTRIBUTIONS = "distributions"
    HYPOTHESIS = "hypothesis"
    REGRESSION = "regression"
    CLUSTERING = "clustering"
    CLASSIFICATION = "classification"

@dataclass
class StatisticalResult:
    """Container for statistical analysis results"""
    method: StatisticalMethod
    data: Dict[str, Any]
    statistics: Dict[str, float]
    p_value: Optional[float] = None
    confidence_interval: Optional[Tuple[float, float]] = None
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class TimeSeriesData:
    """Container for time series neural data"""
    timestamps: np.ndarray
    values: np.ndarray
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

class StatisticalAnalyzer:
    """Advanced statistical analysis for neural data"""
    
    @staticmethod
    def compute_descriptive_statistics(data: Union[List[float], np.ndarray], 
                                     name: str = "data") -> StatisticalResult:
        """Compute comprehensive descriptive statistics"""
        if isinstance(data, list):
            data = np.array(data)
        
        if len(data) == 0:
            return StatisticalResult(
                method=StatisticalMethod.DESCRIPTIVE,
                data={"name": name, "values": []},
                statistics={}
            )
        
        # Basic statistics
        stats_dict = {
            "count": len(data),
            "min": np.min(data),
            "max": np.max(data),
            "mean": np.mean(data),
            "median": np.median(data),
            "std": np.std(data),
            "variance": np.var(data),
            "range": np.max(data) - np.min(data),
            "q25": np.percentile(data, 25),
            "q50": np.percentile(data, 50),
            "q75": np.percentile(data, 75),
            "skewness": stats.skew(data),
            "kurtosis": stats.kurtosis(data),
        }
        
        return StatisticalResult(
            method=StatisticalMethod.DESCRIPTIVE,
            data={"name": name, "values": data.tolist()},
            statistics=stats_dict
        )
    
    @staticmethod
    def compute_neural_activity_statistics(firing_rates: List[float],
                                          time_window: Optional[float] = None) -> StatisticalResult:
        """Compute statistics for neural firing rate data"""
        if not firing_rates:
            return StatisticalResult(
                method=StatisticalMethod.DESCRIPTIVE,
                data={"firing_rates": []},
                statistics={}
            )
        
        stats_dict = {
            "firing_rate_mean": np.mean(firing_rates),
            "firing_rate_std": np.std(firing_rates),
            "firing_rate_median": np.median(firing_rates),
            "firing_rate_q1": np.percentile(firing_rates, 25),
            "firing_rate_q3": np.percentile(firing_rates, 75),
            "coefficient_of_variation": np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) != 0 else float('inf'),
            "firing_rate_cv": np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) != 0 else float('inf'),
            "sync_index": StatisticalAnalyzer._compute_sync_index(firing_rates),
            "mean_inter_spike_interval": np.mean(np.diff(np.where(np.diff(firing_rates) != 0)[0])) if len(np.where(np.diff(firing_rates) != 0)[0]) > 1 else float('inf'),
        }
        
        return StatisticalResult(
            method=StatisticalMethod.DESCRIPTIVE,
            data={"firing_rates": firing_rates},
            statistics=stats_dict
        )
    
    @staticmethod
    def _compute_sync_index(firing_rates: np.ndarray) -> float:
        """Compute synchronization index between neurons"""
        if len(firing_rates) < 2:
            return 0.0
        
        # Compute cross-correlation
        normalized_rates = firing_rates - np.mean(firing_rates)
        if np.std(normalized_rates) == 0:
            return 0.0
        
        return np.corrcoef(normalized_rates[:-1], normalized_rates[1:])[0, 1]
    
    @staticmethod
    def analyze_distribution(data: Union[List[float], np.ndarray],
                           distribution_type: str = "normal") -> StatisticalResult:
        """Analyze if data follows a specific distribution"""
        if isinstance(data, list):
            data = np.array(data)
        
        if distribution_type == "normal":
            # Shapiro-Wilk test for normality
            if len(data) < 3 or len(data) > 5000:
                shapiro_stat = shapiro_wilk_test(data)
                return StatisticalResult(
                    method=StatisticalMethod.DISTRIBUTIONS,
                    data={"values": data.tolist(), "type": "normal"},
                    statistics={"shapiro_stat": shapiro_stat, "is_normal": shapiro_stat > 0.05},
                    p_value=1.0 - shapiro_stat
                )
            else:
                stat, p_value = stats.shapiro(data)
                return StatisticalResult(
                    method=StatisticalMethod.DISTRIBUTIONS,
                    data={"values": data.tolist(), "type": "normal"},
                    statistics={"shapiro_stat": stat, "p_value": p_value, "is_normal": p_value > 0.05},
                    p_value=p_value
                )
        
        elif distribution_type == "poisson":
            # Poisson distribution test
            rate = np.mean(data)
            if rate == 0:
                return StatisticalResult(
                    method=StatisticalMethod.DISTRIBUTIONS,
                    data={"values": data.tolist(), "type": "poisson"},
                    statistics={"rate": rate, "chi_square": 0.0, "is_poisson": True},
                    p_value=1.0
                )
            
            # Expected frequencies for chi-square test
            expected = []
            max_count = int(np.max(data))
            for k in range(max_count + 1):
                expected.append(rate * math.exp(-rate) * (k ** 0) if k == 0 else rate * math.exp(-rate) * (k ** 0))
            
            # Simplified chi-square test
            observed = np.bincount(data.astype(int))
            chi_square = np.sum((observed - expected[:len(observed)]) ** 2 / np.array(expected[:len(observed)]) + 1e-10)
            
            return StatisticalResult(
                method=StatisticalMethod.DISTRIBUTIONS,
                data={"values": data.tolist(), "type": "poisson", "rate": rate},
                statistics={"rate": rate, "chi_square": chi_square, "is_poisson": chi_square < 3.84},
                p_value=1.0 - min(chi_square / 3.84, 1.0)
            )
        
        return StatisticalResult(
            method=StatisticalMethod.DISTRIBUTIONS,
            data={"values": data.tolist(), "type": distribution_type},
            statistics={},
            p_value=None
        )
    
    @staticmethod
    def compute_monte_carlo(data: Union[List[float], np.ndarray],
                           n_samples: int = 10000,
                           confidence_level: float = 0.95) -> Dict[str, Any]:
        """Compute Monte Carlo confidence intervals"""
        if isinstance(data, list):
            data = np.array(data)
        
        if len(data) == 0:
            return {"error": "No data provided"}
        
        # Bootstrap sampling
        bootstrap_means = []
        for _ in range(n_samples):
            sample = np.random.choice(data, size=len(data), replace=True)
            bootstrap_means.append(np.mean(sample))
        
        # Calculate confidence intervals
        alpha = (1 - confidence_level) / 2
        lower = np.percentile(bootstrap_means, alpha * 100)
        upper = np.percentile(bootstrap_means, (1 - alpha) * 100)
        
        return {
            "original_mean": np.mean(data),
            "bootstrap_mean": np.mean(bootstrap_means),
            "confidence_interval": (lower, upper),
            "confidence_level": confidence_level,
            "n_bootstrap_samples": n_samples,
            "standard_error": np.std(bootstrap_means),
            "bias": np.mean(bootstrap_means) - np.mean(data)
        }
    
    @staticmethod
    def correlation_analysis(data1: Union[List[float], np.ndarray],
                           data2: Union[List[float], np.ndarray],
                           method: str = "pearson") -> StatisticalResult:
        """Compute correlation between two datasets"""
        if isinstance(data1, list):
            data1 = np.array(data1)
        if isinstance(data2, list):
            data2 = np.array(data2)
        
        if len(data1) != len(data2) or len(data1) == 0:
            return StatisticalResult(
                method=StatisticalMethod.DESCRIPTIVE,
                data={"data1": [], "data2": []},
                statistics={}
            )
        
        if method == "pearson":
            corr, p_value = stats.pearsonr(data1, data2)
        elif method == "spearman":
            corr, p_value = stats.spearmanr(data1, data2)
        elif method == "kendall":
            corr, p_value = stats.kendalltau(data1, data2)
        else:
            corr, p_value = stats.pearsonr(data1, data2)
        
        return StatisticalResult(
            method=StatisticalMethod.DESCRIPTIVE,
            data={"data1": data1.tolist(), "data2": data2.tolist()},
            statistics={
                "correlation": corr,
                "p_value": p_value,
                "sample_size": len(data1),
                "correlation_type": method
            },
            p_value=p_value
        )

class NeuralDataProcessor:
    """Utilities for processing neural simulation data"""
    
    @staticmethod
    def extract_neuron_features(brain_state: Any) -> Dict[str, np.ndarray]:
        """Extract features from brain state"""
        if not _CORE_AVAILABLE:
            return {}
        
        features = {}
        
        # Extract firing rates
        if hasattr(brain_state, 'neurons'):
            firing_rates = []
            for neuron in brain_state.neurons:
                firing_rates.append(neuron.get_firing_rate())
            
            if firing_rates:
                features['firing_rates'] = np.array(firing_rates)
                
                # Compute additional features
                features['firing_rate_stats'] = {
                    'mean': np.mean(firing_rates),
                    'std': np.std(firing_rates),
                    'cv': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) != 0 else 0,
                }
        
        return features
    
    @staticmethod
    def time_series_to_features(time_series: TimeSeriesData) -> Dict[str, np.ndarray]:
        """Convert time series to feature matrix"""
        if len(time_series.values) == 0:
            return {}
        
        features = {}
        values = time_series.values
        
        # Basic time series features
        features['mean'] = np.mean(values)
        features['std'] = np.std(values)
        features['min'] = np.min(values)
        features['max'] = np.max(values)
        features['range'] = np.max(values) - np.min(values)
        features['cv'] = np.std(values) / np.mean(values) if np.mean(values) != 0 else 0
        
        # Statistical moments
        features['skewness'] = stats.skew(values)
        features['kurtosis'] = stats.kurtosis(values)
        
        # Time domain features
        diff = np.diff(values)
        features['mean_diff'] = np.mean(diff)
        features['std_diff'] = np.std(diff)
        
        # Frequency domain features (using FFT)
        fft_values = np.fft.fft(values)
        features['dominant_frequency'] = np.argmax(np.abs(fft_values[1:len(fft_values)//2])) / len(values)
        features['power_spectrum_mean'] = np.mean(np.abs(fft_values) ** 2)
        
        return features

# Convenience functions

def compute_neural_statistics(firing_rates: List[float]) -> Dict[str, float]:
    """Convenience function to compute neural firing rate statistics"""
    return StatisticalAnalyzer.compute_neural_activity_statistics(firing_rates).statistics

def analyze_neural_data(data: List[float], method: str = "normal") -> Dict[str, Any]:
    """Convenience function to analyze neural data distribution"""
    return StatisticalAnalyzer.analyze_distribution(data, method).__dict__

def bootstrap_confidence_interval(data: List[float], 
                                  n_samples: int = 10000,
                                  confidence_level: float = 0.95) -> Dict[str, Any]:
    """Convenience function for bootstrap confidence intervals"""
    return StatisticalAnalyzer.compute_monte_carlo(data, n_samples, confidence_level)
