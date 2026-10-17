#!/usr/bin/env python3
"""
Advanced Statistical Analysis Tools for NLM Neural Data

This module provides comprehensive advanced statistical analysis tools for neural simulation data,
including time series analysis, wavelet analysis, network analysis, dimensionality reduction,
clustering, classification, and feature extraction.
"""

import numpy as np
import pandas as pd
import scipy.stats as stats
import scipy.signal as signal
import scipy.fft as fft
from scipy import linalg
from scipy.spatial.distance import pdist, squareform
from scipy.cluster import hierarchy
from sklearn.decomposition import PCA, FastICA, TSNE, UMAP
from sklearn.cluster import KMeans, DBSCAN, AgglomerativeClustering
from sklearn.preprocessing import StandardScaler, MinMaxScaler, Normalizer
from sklearn.model_selection import cross_val_score, GridSearchCV, train_test_split
from sklearn.svm import SVC, SVR
from sklearn.ensemble import RandomForestClassifier, RandomForestRegressor, IsolationForest
from sklearn.neighbors import KNeighborsClassifier
from sklearn.linear_model import LogisticRegression, LinearRegression
from sklearn.metrics import (accuracy_score, precision_score, recall_score, f1_score,
                           mean_squared_error, r2_score, silhouette_score,
                           calinski_harabasz_score, davies_bouldin_score)
from sklearn.neural_network import MLPClassifier, MLPRegressor
from typing import List, Dict, Tuple, Optional, Any, Union, Callable, Generator
from dataclasses import dataclass, field
from enum import Enum
import warnings
import time
from collections import defaultdict
import itertools
from abc import ABC, abstractmethod

warnings.filterwarnings('ignore')

class AnalysisMethod(Enum):
    """Advanced analysis methods"""
    TIME_SERIES = "time_series"
    WAVELET = "wavelet"
    NETWORK = "network"
    DIMENSIONALITY_REDUCTION = "dimensionality_reduction"
    CLUSTERING = "clustering"
    CLASSIFICATION = "classification"
    FEATURE_EXTRACTION = "feature_extraction"
    STATISTICAL_TESTING = "statistical_testing"
    POWER_ANALYSIS = "power_analysis"
    META_ANALYSIS = "meta_analysis"

@dataclass
class AnalysisResult:
    """Container for advanced analysis results"""
    method: AnalysisMethod
    data: Dict[str, Any]
    results: Dict[str, Any]
    parameters: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class TimeSeriesData:
    """Enhanced time series data container"""
    timestamps: np.ndarray
    values: np.ndarray
    metadata: Dict[str, Any] = field(default_factory=dict)
    labels: Optional[List[str]] = None
    sampling_rate: Optional[float] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}
        if self.labels is None:
            self.labels = [f"signal_{i}" for i in range(len(self.values))]

@dataclass
class WaveletData:
    """Wavelet analysis results"""
    frequencies: np.ndarray
    coefficients: np.ndarray
    scales: np.ndarray
    decomposition_level: int
    signal_info: Dict[str, Any] = field(default_factory=dict)

@dataclass
class NetworkData:
    """Network/graph data structure"""
    adjacency_matrix: np.ndarray
    node_attributes: Optional[Dict[str, np.ndarray]] = None
    edge_attributes: Optional[Dict[str, Any]] = None
    node_labels: Optional[List[str]] = None
    community_labels: Optional[List[int]] = None

class AdvancedStatisticalAnalyzer:
    """Comprehensive advanced statistical analysis for neural data"""
    
    @staticmethod
    def time_series_analysis(data: TimeSeriesData, 
                           methods: List[str] = None) -> Dict[str, AnalysisResult]:
        """Perform comprehensive time series analysis"""
        if methods is None:
            methods = ['stationarity', 'trend', 'seasonality', 'autocorrelation', 
                      'spectral', 'frequency_dominance', 'burst_detection']
        
        results = {}
        
        # Stationarity tests
        if 'stationarity' in methods:
            results['stationarity'] = AdvancedStatisticalAnalyzer._stationarity_test(data)
        
        # Trend analysis
        if 'trend' in methods:
            results['trend'] = AdvancedStatisticalAnalyzer._trend_analysis(data)
        
        # Seasonality detection
        if 'seasonality' in methods:
            results['seasonality'] = AdvancedStatisticalAnalyzer._seasonality_detection(data)
        
        # Autocorrelation analysis
        if 'autocorrelation' in methods:
            results['autocorrelation'] = AdvancedStatisticalAnalyzer._autocorrelation_analysis(data)
        
        # Spectral analysis
        if 'spectral' in methods:
            results['spectral'] = AdvancedStatisticalAnalyzer._spectral_analysis(data)
        
        # Frequency domain analysis
        if 'frequency_dominance' in methods:
            results['frequency_dominance'] = AdvancedStatisticalAnalyzer._frequency_dominance(data)
        
        # Burst detection
        if 'burst_detection' in methods:
            results['burst_detection'] = AdvancedStatisticalAnalyzer._burst_detection(data)
        
        return results
    
    @staticmethod
    def _stationarity_test(data: TimeSeriesData) -> AnalysisResult:
        """Test for stationarity using multiple methods"""
        values = data.values
        
        # Augmented Dickey-Fuller test
        from statsmodels.tsa.stattools import adfuller
        try:
            adf_result = adfuller(values)
            adf_statistic = adf_result[0]
            p_value = adf_result[1]
            critical_values = adf_result[4]
            is_stationary = p_value < 0.05
        except:
            adf_statistic = 0.0
            p_value = 1.0
            critical_values = {}
            is_stationary = False
        
        # KPSS test
        from statsmodels.tsa.stattools import kpss
        try:
            kpss_statistic, kpss_pvalue, kpss_critical = kpss(values, regression='c')
            is_stationary_kpss = kpss_pvalue > 0.05
        except:
            kpss_statistic = 0.0
            kpss_pvalue = 1.0
            is_stationary_kpss = False
        
        # Phillips-Perron test
        from statsmodels.tsa.stattools import ppfuller
        try:
            pp_statistic, pp_pvalue = ppfuller(values)
            is_stationary_pp = pp_pvalue < 0.05
        except:
            pp_statistic = 0.0
            pp_pvalue = 1.0
            is_stationary_pp = False
        
        # Overall stationarity assessment
        stationarity_votes = [is_stationary, is_stationary_kpss, is_stationary_pp]
        final_stationarity = len([v for v in stationarity_votes if v]) >= 2
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "adf_test": {
                    "statistic": adf_statistic,
                    "p_value": p_value,
                    "critical_values": critical_values,
                    "is_stationary": is_stationary
                },
                "kpss_test": {
                    "statistic": kpss_statistic,
                    "p_value": kpss_pvalue,
                    "is_stationary": is_stationary_kpss
                },
                "phillips_perron_test": {
                    "statistic": pp_statistic,
                    "p_value": pp_pvalue,
                    "is_stationary": is_stationary_pp
                },
                "overall_assessment": {
                    "final_stationarity": final_stationarity,
                    "stationarity_votes": stationarity_votes,
                    "recommendation": "Stationary" if final_stationarity else "Non-stationary"
                }
            }
        )
    
    @staticmethod
    def _trend_analysis(data: TimeSeriesData) -> AnalysisResult:
        """Analyze trends in time series data"""
        values = data.values
        timestamps = data.timestamps
        
        # Linear regression for trend
        if len(values) > 1:
            x = np.arange(len(values))
            slope, intercept, r_value, p_value, std_err = stats.linregress(x, values)
            
            # Detrend the data
            trend_values = slope * x + intercept
            detrended_values = values - trend_values
            
            # Calculate trend statistics
            trend_strength = abs(slope) / (np.std(values) + 1e-10)
            trend_significance = 1 - p_value
            
            # Cusum test for structural breaks
            try:
                from statsmodels.tsa.stattools import cusumolsresid
                cusum_result = cusumolsresid(values, trend='linear')
                cusum_statistic = cusum_result[0][-1] if len(cusum_result[0]) > 0 else 0
                cusum_pvalue = cusum_result[1][-1] if len(cusum_result[1]) > 0 else 1
            except:
                cusum_statistic = 0.0
                cusum_pvalue = 1.0
            
        else:
            slope = intercept = r_value = p_value = std_err = 0.0
            trend_values = values
            detrended_values = values
            trend_strength = 0.0
            trend_significance = 0.0
            cusum_statistic = 0.0
            cusum_pvalue = 1.0
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "linear_trend": {
                    "slope": slope,
                    "intercept": intercept,
                    "r_squared": r_value ** 2,
                    "p_value": p_value,
                    "std_error": std_err,
                    "trend_strength": trend_strength,
                    "trend_significance": trend_significance
                },
                "detrended_analysis": {
                    "detrended_values": detrended_values.tolist(),
                    "detrended_mean": np.mean(detrended_values),
                    "detrended_std": np.std(detrended_values),
                    "detrended_range": np.max(detrended_values) - np.min(detrended_values)
                },
                "structural_break": {
                    "cusum_statistic": cusum_statistic,
                    "cusum_pvalue": cusum_pvalue,
                    "has_break": cusum_pvalue < 0.05
                }
            }
        )
    
    @staticmethod
    def _seasonality_detection(data: TimeSeriesData) -> AnalysisResult:
        """Detect seasonality patterns in time series"""
        values = data.values
        
        # STL decomposition
        try:
            from statsmodels.tsa.seasonal import seasonal_decompose
            if len(values) > 12:
                period = 12 if len(values) < 50 else len(values) // 4
                decomposition = seasonal_decompose(values, model='additive', period=period)
                
                seasonal_component = decomposition.seasonal
                trend_component = decomposition.trend
                residual_component = decomposition.resid
                
                # Calculate seasonal strength
                seasonal_strength = np.std(seasonal_component) / np.std(values)
                trend_strength = np.std(trend_component) / np.std(values) if len(np.where(~np.isnan(trend_component))[0]) > 0 else 0
                residual_strength = np.std(residual_component[~np.isnan(residual_component)]) / np.std(values) if len(residual_component[~np.isnan(residual_component)]) > 0 else 0
                
                # Find dominant seasonal periods using FFT
                fft_values = fft.fft(seasonal_component[~np.isnan(seasonal_component)])
                freqs = fft.fftfreq(len(seasonal_component[~np.isnan(seasonal_component)]))
                power_spectrum = np.abs(fft_values) ** 2
                
                # Find peaks in power spectrum
                from scipy.signal import find_peaks
                peaks, properties = find_peaks(power_spectrum, height=np.max(power_spectrum) * 0.1, distance=5)
                peak_freqs = freqs[peaks]
                peak_powers = properties['peak_heights']
                
                # Convert frequencies to periods
                periods = []
                for freq in peak_freqs:
                    if freq > 0:
                        period = 1.0 / freq
                        periods.append(period)
                
                dominant_period = None
                if periods:
                    dominant_period = periods[np.argmax(peak_powers)]
                
            else:
                seasonal_strength = trend_strength = residual_strength = 0.0
                dominant_period = None
                decomposition = None
        except:
            seasonal_strength = trend_strength = residual_strength = 0.0
            dominant_period = None
            decomposition = None
        
        # Periodogram-based analysis
        try:
            from scipy.signal import periodogram
            freqs, psd = periodogram(values)
            
            # Find peaks in periodogram
            peaks, properties = find_peaks(psd, height=np.max(psd) * 0.1)
            peak_freqs = freqs[peaks]
            peak_powers = properties['peak_heights']
            
            # Convert to periods
            periods = [1.0 / f if f > 0 else 0 for f in peak_freqs]
            periods = [p for p in periods if p > 0]
            
            if periods:
                dominant_period_periodogram = periods[np.argmax(peak_powers)]
                seasonal_strength_periodogram = np.max(peak_powers) / np.sum(psd)
            else:
                dominant_period_periodogram = None
                seasonal_strength_periodogram = 0.0
        except:
            seasonal_strength_periodogram = 0.0
            dominant_period_periodogram = None
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "stl_decomposition": {
                    "seasonal_strength": seasonal_strength,
                    "trend_strength": trend_strength,
                    "residual_strength": residual_strength,
                    "has_seasonal_component": seasonal_strength > 0.3
                },
                "frequency_analysis": {
                    "dominant_period": dominant_period,
                    "dominant_period_periodogram": dominant_period_periodogram,
                    "seasonal_strength_periodogram": seasonal_strength_periodogram,
                    "candidate_periods": periods if 'periods' in locals() else []
                },
                "seasonality_assessment": {
                    "is_seasonal": (seasonal_strength > 0.3) or (seasonal_strength_periodogram > 0.3),
                    "seasonality_confidence": (seasonal_strength + seasonal_strength_periodogram) / 2,
                    "recommended_period": dominant_period or dominant_period_periodogram
                }
            }
        )
    
    @staticmethod
    def _autocorrelation_analysis(data: TimeSeriesData) -> AnalysisResult:
        """Analyze autocorrelation structure"""
        values = data.values
        
        # Calculate autocorrelation function
        n = len(values)
        if n > 1:
            # Normalized autocorrelation
            values_centered = values - np.mean(values)
            autocorr = np.correlate(values_centered, values_centered, mode='full')
            autocorr = autocorr[n - 1:] / (np.var(values) * n)
            
            # Truncate to reasonable lags
            max_lag = min(50, n - 1)
            autocorr = autocorr[:max_lag + 1]
            lags = np.arange(max_lag + 1)
            
            # Find significant lags
            from scipy import stats
            acf_se = np.sqrt((1 + 2 * np.sum(autocorr[1:] ** 2)) / n)
            significant_lags = np.where(np.abs(autocorr[1:]) > 1.96 * acf_se)[0] + 1
            
            # Calculate partial autocorrelation
            from statsmodels.tsa.stattools import pacf
            try:
                pacf_values = pacf(values, nlags=max_lag)
                significant_pacf_lags = np.where(np.abs(pacf_values[1:]) > 1.96 / np.sqrt(n))[0] + 1
            except:
                pacf_values = np.zeros(max_lag + 1)
                significant_pacf_lags = []
            
            # ARMA model identification
            try:
                from statsmodels.tsa.arima_model import ARIMA
                # Simplified ARMA identification
                best_aic = float('inf')
                best_order = (0, 0, 0)
                
                for p in range(3):
                    for d in range(2):
                        for q in range(3):
                            if p + q > 6:
                                continue
                            try:
                                model = ARIMA(values, order=(p, d, q))
                                model_fit = model.fit(disp=False)
                                if model_fit.aic < best_aic:
                                    best_aic = model_fit.aic
                                    best_order = (p, d, q)
                            except:
                                pass
            except:
                best_order = (0, 0, 0)
                best_aic = 0.0
            
        else:
            autocorr = np.array([1.0])
            lags = np.array([0])
            significant_lags = []
            pacf_values = np.array([1.0])
            significant_pacf_lags = []
            best_order = (0, 0, 0)
            best_aic = 0.0
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "autocorrelation_function": {
                    "lags": lags.tolist(),
                    "acf": autocorr.tolist(),
                    "significant_lags": significant_lags.tolist(),
                    "number_of_significant_lags": len(significant_lags),
                    "autocorrelation_decay": np.sum(np.abs(autocorr[1:]))
                },
                "partial_autocorrelation": {
                    "pacf": pacf_values.tolist(),
                    "significant_pacf_lags": significant_pacf_lags.tolist(),
                    "pacf_significant_lags": len(significant_pacf_lags)
                },
                "arma_identification": {
                    "best_order": best_order,
                    "best_aic": best_aic,
                    "model_type": "ARMA",
                    "model_quality": "Good" if best_aic < 100 else "Poor"
                }
            }
        )
    
    @staticmethod
    def _spectral_analysis(data: TimeSeriesData) -> AnalysisResult:
        """Perform spectral analysis"""
        values = data.values
        
        # Compute periodogram
        freqs, psd = signal.periodogram(values)
        
        # Find peaks in power spectrum
        peaks, properties = signal.find_peaks(psd, height=np.max(psd) * 0.1)
        peak_freqs = freqs[peaks]
        peak_powers = properties['peak_heights']
        peak_widths = properties['widths'] if 'widths' in properties else []
        
        # Calculate spectral entropy
        psd_normalized = psd / np.sum(psd)
        psd_normalized = psd_normalized[psd_normalized > 0]
        spectral_entropy = -np.sum(psd_normalized * np.log(psd_normalized))
        
        # Calculate dominant frequency and bandwidth
        if len(peak_freqs) > 0:
            dominant_freq = peak_freqs[np.argmax(peak_powers)]
            dominant_power = np.max(peak_powers)
            
            # Calculate bandwidth at half power
            half_power = dominant_power / 2
            above_half_power = np.where(psd >= half_power)[0]
            if len(above_half_power) > 0:
                bandwidth = freqs[above_half_power[-1]] - freqs[above_half_power[0]]
            else:
                bandwidth = 0.0
        else:
            dominant_freq = 0.0
            dominant_power = 0.0
            bandwidth = 0.0
        
        # Wavelet analysis
        try:
            import pywt
            scales = np.logspace(0, 5, 50)
            coefficients, frequencies_wt = pywt.cwt(values, scales, wavelet='morlet')
            
            # Calculate wavelet power spectrum
            wavelet_power = np.mean(np.abs(coefficients) ** 2, axis=1)
            
            # Find significant frequencies in wavelet domain
            wt_peaks, wt_properties = signal.find_peaks(wavelet_power, height=np.max(wavelet_power) * 0.1)
            wt_peak_freqs = frequencies_wt[wt_peaks]
            wt_peak_powers = wt_properties['peak_heights'] if 'peak_heights' in wt_properties else wavelet_power[wt_peaks]
            
            dominant_wavelet_freq = wt_peak_freqs[np.argmax(wt_peak_powers)] if len(wt_peak_freqs) > 0 else 0.0
            
        except:
            dominant_wavelet_freq = 0.0
            wavelet_power = np.array([])
            wt_peak_freqs = np.array([])
        
        return AnalysisResult(
            method=AnalysisMethod.WAVELET,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "periodogram": {
                    "frequencies": freqs.tolist(),
                    "power_spectrum": psd.tolist(),
                    "dominant_frequency": dominant_freq,
                    "dominant_power": dominant_power,
                    "bandwidth": bandwidth,
                    "number_of_peaks": len(peak_freqs),
                    "peak_frequencies": peak_freqs.tolist(),
                    "peak_powers": peak_powers.tolist()
                },
                "spectral_entropy": {
                    "spectral_entropy": spectral_entropy,
                    "spectrum_concentration": np.sum(psd_normalized ** 2),
                    "spectral_flatness": np.exp(np.mean(np.log(psd + 1e-10))) / (np.mean(psd) + 1e-10)
                },
                "wavelet_analysis": {
                    "dominant_wavelet_frequency": dominant_wavelet_freq,
                    "wavelet_power": wavelet_power.tolist() if len(wavelet_power) > 0 else [],
                    "wavelet_frequencies": frequencies_wt.tolist() if len(wavelet_power) > 0 else [],
                    "number_of_wavelet_peaks": len(wt_peak_freqs)
                }
            }
        )
    
    @staticmethod
    def _frequency_dominance(data: TimeSeriesData) -> AnalysisResult:
        """Analyze frequency dominance and modulation"""
        values = data.values
        
        # Compute FFT
        fft_values = fft.fft(values)
        freqs = fft.fftfreq(len(values))
        power_spectrum = np.abs(fft_values) ** 2
        
        # Focus on positive frequencies
        positive_freqs = freqs[freqs > 0]
        positive_power = power_spectrum[freqs > 0]
        
        # Calculate frequency statistics
        if len(positive_freqs) > 0:
            # Find dominant frequency
            dominant_idx = np.argmax(positive_power)
            dominant_freq = positive_freqs[dominant_idx]
            dominant_power = positive_power[dominant_idx]
            
            # Calculate spectral centroid (mean frequency)
            spectral_centroid = np.sum(positive_freqs * positive_power) / np.sum(positive_power)
            
            # Calculate spectral spread (standard deviation of frequencies)
            if np.sum(positive_power) > 0:
                freq_mean = spectral_centroid
                freq_variance = np.sum(positive_power * (positive_freqs - freq_mean) ** 2) / np.sum(positive_power)
                spectral_spread = np.sqrt(freq_variance)
            else:
                spectral_spread = 0.0
            
            # Calculate skewness and kurtosis of frequency distribution
            if len(positive_freqs) > 2:
                freq_skew = stats.skew(positive_freqs, weights=positive_power)
                freq_kurtosis = stats.kurtosis(positive_freqs, weights=positive_power)
            else:
                freq_skew = 0.0
                freq_kurtosis = 0.0
            
            # Identify frequency bands
            # Define bands based on typical neural frequencies
            delta_band = (0.5, 4.0)
            theta_band = (4.0, 8.0)
            alpha_band = (8.0, 13.0)
            beta_band = (13.0, 30.0)
            gamma_band = (30.0, 100.0)
            
            def calculate_band_power(band):
                band_mask = (positive_freqs >= band[0]) & (positive_freqs <= band[1])
                return np.sum(positive_power[band_mask]) if np.any(band_mask) else 0.0
            
            delta_power = calculate_band_power(delta_band)
            theta_power = calculate_band_power(theta_band)
            alpha_power = calculate_band_power(alpha_band)
            beta_power = calculate_band_power(beta_band)
            gamma_power = calculate_band_power(gamma_band)
            
            total_power = delta_power + theta_power + alpha_power + beta_power + gamma_power
            
            if total_power > 0:
                delta_ratio = delta_power / total_power
                theta_ratio = theta_power / total_power
                alpha_ratio = alpha_power / total_power
                beta_ratio = beta_power / total_power
                gamma_ratio = gamma_power / total_power
            else:
                delta_ratio = theta_ratio = alpha_ratio = beta_ratio = gamma_ratio = 0.0
            
            # Determine frequency modulation index
            if spectral_centroid > 0:
                freq_modulation_index = spectral_spread / spectral_centroid
            else:
                freq_modulation_index = 0.0
            
        else:
            dominant_freq = dominant_power = spectral_centroid = spectral_spread = 0.0
            freq_skew = freq_kurtosis = 0.0
            delta_ratio = theta_ratio = alpha_ratio = beta_ratio = gamma_ratio = 0.0
            freq_modulation_index = 0.0
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "frequency_dominance": {
                    "dominant_frequency": dominant_freq,
                    "dominant_power": dominant_power,
                    "spectral_centroid": spectral_centroid,
                    "spectral_spread": spectral_spread,
                    "frequency_skewness": freq_skew,
                    "frequency_kurtosis": freq_kurtosis,
                    "frequency_modulation_index": freq_modulation_index
                },
                "frequency_bands": {
                    "delta_power": delta_power,
                    "delta_ratio": delta_ratio,
                    "theta_power": theta_power,
                    "theta_ratio": theta_ratio,
                    "alpha_power": alpha_power,
                    "alpha_ratio": alpha_ratio,
                    "beta_power": beta_power,
                    "beta_ratio": beta_ratio,
                    "gamma_power": gamma_power,
                    "gamma_ratio": gamma_ratio,
                    "total_power": total_power
                },
                "frequency_assessment": {
                    "is_band_dominant": max(delta_ratio, theta_ratio, alpha_ratio, beta_ratio, gamma_ratio) > 0.5,
                    "dominant_band": max(zip(['delta', 'theta', 'alpha', 'beta', 'gamma'], 
                                            [delta_ratio, theta_ratio, alpha_ratio, beta_ratio, gamma_ratio]), 
                                         key=lambda x: x[1])[0] if total_power > 0 else 'none',
                    "frequency_range": {
                        "min_dominant_frequency": np.min(positive_freqs) if len(positive_freqs) > 0 else 0,
                        "max_dominant_frequency": np.max(positive_freqs) if len(positive_freqs) > 0 else 0
                    }
                }
            }
        )
    
    @staticmethod
    def _burst_detection(data: TimeSeriesData) -> AnalysisResult:
        """Detect bursts in neural activity"""
        values = data.values
        
        # Threshold-based burst detection
        threshold = np.mean(values) + 2 * np.std(values)
        candidate_bursts = values > threshold
        
        # Find burst intervals
        burst_intervals = []
        in_burst = False
        burst_start = 0
        
        for i, is_burst in enumerate(candidate_bursts):
            if is_burst and not in_burst:
                in_burst = True
                burst_start = i
            elif not is_burst and in_burst:
                in_burst = False
                burst_end = i - 1
                burst_intervals.append((burst_start, burst_end))
        
        if in_burst:
            burst_intervals.append((burst_start, len(values) - 1))
        
        # Calculate burst statistics
        burst_durations = []
        burst_energies = []
        burst_amplitudes = []
        
        for start, end in burst_intervals:
            duration = end - start + 1
            burst_durations.append(duration)
            
            burst_values = values[start:end + 1]
            burst_energies.append(np.sum(burst_values ** 2))
            burst_amplitudes.append(np.mean(burst_values))
        
        # Inter-burst interval analysis
        inter_burst_intervals = []
        if len(burst_intervals) > 1:
            for i in range(1, len(burst_intervals)):
                prev_end = burst_intervals[i - 1][1]
                curr_start = burst_intervals[i][0]
                inter_burst_intervals.append(curr_start - prev_end - 1)
        
        # Burst regularity (coefficient of variation of inter-burst intervals)
        if len(inter_burst_intervals) > 1:
            burst_regularity = np.std(inter_burst_intervals) / np.mean(inter_burst_intervals)
        else:
            burst_regularity = 0.0
        
        # Burst rate (bursts per second)
        if data.sampling_rate is not None:
            burst_rate = len(burst_intervals) / (len(values) / data.sampling_rate)
        else:
            burst_rate = len(burst_intervals)
        
        # Detect burst shape characteristics
        burst_shapes = []
        for start, end in burst_intervals[:10]:  # Analyze first 10 bursts
            burst_values = values[start:end + 1]
            if len(burst_values) > 3:
                # Calculate derivative to detect rising/falling phases
                derivative = np.diff(burst_values)
                rising_phase = np.where(derivative > 0)[0]
                falling_phase = np.where(derivative < 0)[0]
                
                if len(rising_phase) > 0 and len(falling_phase) > 0:
                    rise_time = rising_phase[-1] - rising_phase[0]
                    fall_time = falling_phase[-1] - falling_phase[0]
                    burst_shapes.append({
                        'rise_time': rise_time,
                        'fall_time': fall_time,
                        'rise_fall_ratio': rise_time / (fall_time + 1e-10),
                        'duration': end - start + 1,
                        'max_amplitude': np.max(burst_values),
                        'mean_amplitude': np.mean(burst_values)
                    })
        
        # Statistical tests for burst characteristics
        from scipy import stats
        burst_stats = {}
        if burst_durations:
            burst_stats['duration_stats'] = {
                'mean': np.mean(burst_durations),
                'std': np.std(burst_durations),
                'skew': stats.skew(burst_durations) if len(burst_durations) > 2 else 0,
                'kurtosis': stats.kurtosis(burst_durations) if len(burst_durations) > 3 else 0,
                'cv': np.std(burst_durations) / np.mean(burst_durations) if np.mean(burst_durations) > 0 else 0
            }
        
        if burst_energies:
            burst_stats['energy_stats'] = {
                'mean': np.mean(burst_energies),
                'std': np.std(burst_energies),
                'skew': stats.skew(burst_energies) if len(burst_energies) > 2 else 0,
                'kurtosis': stats.kurtosis(burst_energies) if len(burst_energies) > 3 else 0,
                'cv': np.std(burst_energies) / np.mean(burst_energies) if np.mean(burst_energies) > 0 else 0
            }
        
        return AnalysisResult(
            method=AnalysisMethod.TIME_SERIES,
            data={"timestamps": data.timestamps, "values": data.values},
            results={
                "burst_detection": {
                    "threshold": threshold,
                    "number_of_bursts": len(burst_intervals),
                    "burst_intervals": burst_intervals,
                    "burst_durations": burst_durations,
                    "burst_rate": burst_rate,
                    "burst_regularity": burst_regularity,
                    "burst_shapes": burst_shapes
                },
                "burst_statistics": burst_stats,
                "burst_assessment": {
                    "has_bursts": len(burst_intervals) > 0,
                    "burst_density": len(burst_intervals) / len(values) if len(values) > 0 else 0,
                    "burst_characteristics": {
                        "regular": burst_regularity < 0.5,
                        "dense": len(burst_intervals) / len(values) > 0.1,
                        "irregular": burst_regularity > 1.0,
                        "brief": np.mean(burst_durations) < 5 if burst_durations else False,
                        "prolonged": np.mean(burst_durations) > 20 if burst_durations else False
                    }
                }
            }
        )

class WaveletAnalyzer:
    """Advanced wavelet analysis for neural signals"""
    
    @staticmethod
    def continuous_wavelet_transform(data: TimeSeriesData, 
                                    wavelet: str = "morlet",
                                    scales: Optional[np.ndarray] = None,
                                    freqs: Optional[np.ndarray] = None) -> WaveletData:
        """Perform continuous wavelet transform"""
        try:
            import pywt
            
            values = data.values
            
            # Set default scales/frequencies if not provided
            if scales is None and freqs is None:
                # Use scales that cover the frequency range of interest
                max_scale = len(values) / 8
                min_scale = 2
                scales = np.logspace(np.log10(min_scale), np.log10(max_scale), 50)
                freqs = 1 / (scales * data.sampling_rate) if data.sampling_rate else 1 / scales
            
            # Perform wavelet transform
            coefficients = pywt.cwt(values, scales, wavelet=wavelet)
            
            # Calculate frequencies
            if freqs is None:
                freqs = pywt.scale2frequency(wavelet, scales)
            
            # Calculate signal information
            signal_info = {
                'signal_length': len(values),
                'sampling_rate': data.sampling_rate,
                'wavelet': wavelet,
                'scale_range': [scales[0], scales[-1]],
                'frequency_range': [freqs[0], freqs[-1]] if len(freqs) > 0 else [0, 0]
            }
            
            return WaveletData(
                frequencies=freqs,
                coefficients=coefficients,
                scales=scales,
                decomposition_level=int(np.log2(len(values))),
                signal_info=signal_info
            )
            
        except ImportError:
            raise ImportError("pywt package required for wavelet analysis. Install with: pip install pywt")
    
    @staticmethod
    def wavelet_packet_decomposition(data: TimeSeriesData,
                                    wavelet: str = "db4",
                                    level: int = 5) -> Dict[str, Any]:
        """Perform wavelet packet decomposition"""
        try:
            import pywt
            
            values = data.values
            
            # Perform wavelet packet decomposition
            wp = pywt.WaveletPacket(data=values, wavelet=wavelet, level=level)
            
            # Extract coefficients at different nodes
            coefficient_dict = {}
            for node in wp.getChildren():
                key = node.name
                coefficient_dict[key] = node.data
            
            # Calculate energy at each node
            energy_dict = {}
            for key, coeff in coefficient_dict.items():
                energy_dict[key] = np.sum(np.abs(coeff) ** 2) if len(coeff) > 0 else 0
            
            # Normalize energy
            total_energy = np.sum(list(energy_dict.values()))
            if total_energy > 0:
                normalized_energy = {k: v / total_energy for k, v in energy_dict.items()}
            else:
                normalized_energy = energy_dict
            
            return {
                'coefficients': coefficient_dict,
                'energy': energy_dict,
                'normalized_energy': normalized_energy,
                'wavelet': wavelet,
                'level': level,
                'signal_length': len(values)
            }
            
        except ImportError:
            raise ImportError("pywt package required for wavelet packet decomposition. Install with: pip install pywt")
    
    @staticmethod
    def wavelet_denoising(data: TimeSeriesData,
                         wavelet: str = "db4",
                         level: int = 5,
                         threshold: str = "universal") -> np.ndarray:
        """Apply wavelet-based denoising"""
        try:
            import pywt
            
            values = data.values
            
            # Perform wavelet decomposition
            wavelet_obj = pywt.Wavelet(wavelet)
            coefficients = pywt.wavedec(values, wavelet_obj, level=level)
            
            # Apply thresholding
            if threshold == "universal":
                # Universal threshold
                threshold_value = np.sqrt(2 * np.log(len(values)))
            elif threshold == "sure":
                # Stein's Unbiased Risk Estimate
                from pywt.thresholding import threshold
                # Simplified SURE threshold calculation
                threshold_value = np.sqrt(2 * np.log(len(values)))
            else:
                threshold_value = float(threshold)
            
            # Apply threshold to detail coefficients
            thresholded_coefficients = coefficients.copy()
            for i in range(1, len(coefficients)):
                # Apply soft thresholding
                thresholded_coefficients[i] = pywt.threshold(coefficients[i], threshold_value)
            
            # Reconstruct signal
            denoised_signal = pywt.waverec(thresholded_coefficients, wavelet_obj)
            
            # Ensure output has same length as input
            if len(denoised_signal) > len(values):
                denoised_signal = denoised_signal[:len(values)]
            elif len(denoised_signal) < len(values):
                # Pad if shorter
                padded_signal = np.zeros(len(values))
                padded_signal[:len(denoised_signal)] = denoised_signal
                denoised_signal = padded_signal
            
            return denoised_signal
            
        except ImportError:
            raise ImportError("pywt package required for wavelet denoising. Install with: pip install pywt")
class NetworkAnalyzer:
    """Advanced network analysis for neural connectivity"""
    
    @staticmethod
    def compute_connectivity_matrix(neural_data: List[np.ndarray],
                                   method: str = "correlation",
                                   window_size: Optional[int] = None) -> NetworkData:
        """Compute functional connectivity matrix"""
        if len(neural_data) == 0:
            return NetworkData(adjacency_matrix=np.array([]))
        
        # Convert to numpy array
        data_matrix = np.array(neural_data)  # Shape: (time, neurons)
        
        # Compute connectivity matrix
        if method == "correlation":
            # Pearson correlation between neurons
            connectivity = np.corrcoef(data_matrix.T)
        elif method == "partial_correlation":
            # Partial correlation using graphical lasso
            try:
                from sklearn.covariance import Graphical Lasso
                gl = Graphical Lasso(random_state=42).fit(data_matrix)
                connectivity = -gl.covariance_
                # Keep only positive correlations for simplicity
                connectivity = np.maximum(connectivity, 0)
            except:
                connectivity = np.corrcoef(data_matrix.T)
        elif method == "mutual_information":
            # Mutual information (simplified)
            from scipy.stats import pearsonr
            n_neurons = data_matrix.shape[1]
            connectivity = np.ones((n_neurons, n_neurons))
            
            for i in range(n_neurons):
                for j in range(i + 1, n_neurons):
                    corr, _ = pearsonr(data_matrix[:, i], data_matrix[:, j])
                    mi = -np.log(1 - corr ** 2) if abs(corr) < 1 else 10.0
                    connectivity[i, j] = connectivity[j, i] = mi
        elif method == "granger_causality":
            # Granger causality (simplified)
            n_neurons = data_matrix.shape[1]
            connectivity = np.zeros((n_neurons, n_neurons))
            
            try:
                from statsmodels.tsa.api import grangercausalitytests
                
                for i in range(n_neurons):
                    for j in range(n_neurons):
                        if i != j:
                            try:
                                # Limit test length to avoid long computation
                                test_data = data_matrix[:min(200, len(data_matrix))]
                                maxlag = min(5, len(test_data) // 10)
                                
                                if maxlag > 0:
                                    test_result = grangercausalitytests(test_data[:, [j, i]], 
                                                                       maxlag=maxlag, 
                                                                       verbose=False)
                                    # Extract p-value for best lag
                                    p_values = []
                                    for lag in range(1, maxlag + 1):
                                        if lag < len(test_result):
                                            p_val = test_result[lag - 1][0]['ssr_ftest'][1]
                                            p_values.append(p_val)
                                    
                                    if p_values:
                                        best_p = np.min(p_values)
                                        if best_p < 0.05:
                                            connectivity[i, j] = 1.0 / (best_p + 1e-6)
                            except:
                                pass
            except:
                connectivity = np.corrcoef(data_matrix.T)
        else:
            # Default to correlation
            connectivity = np.corrcoef(data_matrix.T)
        
        # Apply sliding window if specified
        if window_size is not None and window_size < len(data_matrix):
            n_windows = len(data_matrix) - window_size + 1
            window_connectivity = np.zeros((n_windows, n_neurons, n_neurons))
            
            for i in range(n_windows):
                window_data = data_matrix[i:i + window_size]
                window_connectivity[i] = np.corrcoef(window_data.T)
            
            # Average connectivity across windows
            connectivity = np.mean(window_connectivity, axis=0)
        
        return NetworkData(
            adjacency_matrix=connectivity,
            node_attributes={
                'mean_activity': np.mean(data_matrix, axis=0),
                'std_activity': np.std(data_matrix, axis=0),
                'firing_rate': data_matrix.mean(axis=0) if hasattr(data_matrix, 'mean') else np.mean(data_matrix, axis=0)
            }
        )
    
    @staticmethod
    def compute_network_metrics(adjacency_matrix: np.ndarray,
                               node_attributes: Optional[Dict[str, np.ndarray]] = None,
                               edge_attributes: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Compute comprehensive network metrics"""
        if adjacency_matrix.size == 0:
            return {}
        
        n_nodes = adjacency_matrix.shape[0]
        
        # Basic topology metrics
        metrics = {}
        
        # Degree metrics
        degrees = np.sum(adjacency_matrix > 0, axis=1)
        metrics['degree'] = {
            'mean': np.mean(degrees),
            'std': np.std(degrees),
            'min': np.min(degrees),
            'max': np.max(degrees),
            'median': np.median(degrees),
            'density': np.sum(adjacency_matrix > 0) / (n_nodes * (n_nodes - 1) / 2)
        }
        
        # Weighted degree metrics
        if np.any(adjacency_matrix):
            weighted_degrees = np.sum(adjacency_matrix, axis=1)
            metrics['weighted_degree'] = {
                'mean': np.mean(weighted_degrees),
                'std': np.std(weighted_degrees),
                'min': np.min(weighted_degrees),
                'max': np.max(weighted_degrees),
                'median': np.median(weighted_degrees)
            }
        
        # Clustering coefficients
        clustering_coeffs = []
        for i in range(n_nodes):
            neighbors = np.where(adjacency_matrix[i] > 0)[0]
            if len(neighbors) >= 2:
                # Compute local clustering coefficient
                subgraph = adjacency_matrix[np.ix_(neighbors, neighbors)]
                # Normalization for weighted case
                triads = len(neighbors) * (len(neighbors) - 1) / 2
                actual_triads = np.sum(subgraph) / 2
                clustering_coeff = actual_triads / triads if triads > 0 else 0
                clustering_coeffs.append(clustering_coeff)
        
        metrics['clustering_coefficient'] = {
            'mean': np.mean(clustering_coeffs) if clustering_coeffs else 0,
            'std': np.std(clustering_coeffs) if clustering_coeffs else 0,
            'min': np.min(clustering_coeffs) if clustering_coeffs else 0,
            'max': np.max(clustering_coeffs) if clustering_coeffs else 0
        }
        
        # Betweenness centrality
        try:
            from networkx import from_numpy_array, betweenness_centrality
            G = from_numpy_array(adjacency_matrix)
            bc = betweenness_centrality(G)
            metrics['betweenness_centrality'] = {
                'mean': np.mean(list(bc.values())),
                'std': np.std(list(bc.values())),
                'max': np.max(list(bc.values())),
                'min': np.min(list(bc.values()))
            }
        except:
            # Fallback calculation
            metrics['betweenness_centrality'] = {
                'mean': 0.0, 'std': 0.0, 'max': 0.0, 'min': 0.0
            }
        
        # Eigenvector centrality
        try:
            from networkx import from_numpy_array, eigenvector_centrality
            G = from_numpy_array(adjacency_matrix)
            ec = eigenvector_centrality(G, max_iter=1000)
            metrics['eigenvector_centrality'] = {
                'mean': np.mean(list(ec.values())),
                'std': np.std(list(ec.values())),
                'max': np.max(list(ec.values())),
                'min': np.min(list(ec.values()))
            }
        except:
            metrics['eigenvector_centrality'] = {
                'mean': 0.0, 'std': 0.0, 'max': 0.0, 'min': 0.0
            }
        
        # Network efficiency
        try:
            from networkx import from_numpy_array, efficiency
            G = from_numpy_array(adjacency_matrix)
            efficiency_values = efficiency(G)
            metrics['network_efficiency'] = {
                'global_efficiency': efficiency_values if isinstance(efficiency_values, float) else 0,
                'local_efficiency': np.mean(list(efficiency_values.values())) if isinstance(efficiency_values, dict) else 0
            }
        except:
            metrics['network_efficiency'] = {
                'global_efficiency': 0.0,
                'local_efficiency': 0.0
            }
        
        # Community detection (Louvain)
        try:
            import community as community_louvain
            from networkx import from_numpy_array
            G = from_numpy_array(adjacency_matrix)
            partition = community_louvain.best_partition(G)
            metrics['community_detection'] = {
                'n_communities': len(set(partition.values())),
                'community_sizes': {
                    comm: sum(1 for p in partition.values() if p == comm)
                    for comm in set(partition.values())
                },
                'modularity': community_louvain.modularity(partition, G)
            }
        except:
            metrics['community_detection'] = {
                'n_communities': 1,
                'community_sizes': {0: n_nodes},
                'modularity': 0.0
            }
        
        # Neural-specific metrics
        if node_attributes:
            # Hub identification
            hub_threshold = np.percentile(list(degrees), 75)
            hubs = np.where(degrees > hub_threshold)[0]
            metrics['neural_hubs'] = {
                'hub_nodes': hubs.tolist(),
                'n_hubs': len(hubs),
                'hub_ratio': len(hubs) / n_nodes,
                'hub_degree_mean': np.mean(degrees[hubs]) if len(hubs) > 0 else 0
            }
            
            # Neuromodulatory potential
            if 'firing_rate' in node_attributes:
                firing_rates = node_attributes['firing_rate']
                # Identify high-frequency neurons
                high_freq_threshold = np.percentile(firing_rates, 90)
                high_freq_nodes = np.where(firing_rates > high_freq_threshold)[0]
                metrics['high_frequency_nodes'] = {
                    'high_freq_nodes': high_freq_nodes.tolist(),
                    'n_high_freq': len(high_freq_nodes),
                    'high_freq_ratio': len(high_freq_nodes) / n_nodes
                }
        
        return metrics
    
    @staticmethod
    def identify_communication_channels(adjacency_matrix: np.ndarray,
                                      threshold: float = 0.5) -> Dict[str, Any]:
        """Identify significant communication channels"""
        if adjacency_matrix.size == 0:
            return {}
        
        n_nodes = adjacency_matrix.shape[0]
        
        # Identify significant connections
        significant_connections = []
        for i in range(n_nodes):
            for j in range(i + 1, n_nodes):
                weight = adjacency_matrix[i, j]
                if weight > threshold:
                    significant_connections.append((i, j, weight))
        
        # Sort by strength
        significant_connections.sort(key=lambda x: x[2], reverse=True)
        
        # Cluster connections into channels
        if significant_connections:
            # Simple clustering based on proximity
            channels = []
            processed = set()
            
            for i, j, weight in significant_connections:
                if (i, j) not in processed and (j, i) not in processed:
                    # Create a new channel
                    channel_nodes = [i, j]
                    channel_strength = weight
                    
                    # Find connected nodes to expand the channel
                    for k, l, k_weight in significant_connections:
                        if ((i in (k, l) or j in (k, l)) and 
                            (k, l) not in processed and (l, k) not in processed and
                            len(set(channel_nodes + [k, l])) <= 6):  # Limit channel size
                            channel_nodes.extend([k, l] if k not in channel_nodes else [])
                            channel_strength += k_weight
                    
                    channels.append({
                        'nodes': list(set(channel_nodes)),
                        'strength': channel_strength,
                        'connections': [(i, j, weight) for (ii, jj, ww) in significant_connections 
                                       if (ii, jj) in [(i, j), (j, i)] or 
                                       (ii, jj) in [(i, j), (j, i)]]
                    })
                    
                    # Mark processed
                    processed.update([(i, j), (j, i)])
            
            # Sort channels by strength
            channels.sort(key=lambda x: x['strength'], reverse=True)
            
        else:
            channels = []
        
        return {
            'significant_connections': significant_connections,
            'number_of_significant_connections': len(significant_connections),
            'channels': channels,
            'number_of_channels': len(channels),
            'communication_density': len(significant_connections) / (n_nodes * (n_nodes - 1) / 2)
        }
class DimensionalityReducer:
    """Advanced dimensionality reduction for neural data"""
    
    @staticmethod
    def apply_pca(data: np.ndarray, 
                 n_components: int = 2,
                 svd_solver: str = 'auto',
                 random_state: int = 42) -> Dict[str, Any]:
        """Apply Principal Component Analysis"""
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        pca = PCA(n_components=n_components, random_state=random_state, svd_solver=svd_solver)
        transformed_data = pca.fit_transform(data_scaled)
        
        # Calculate explained variance
        explained_variance_ratio = pca.explained_variance_ratio_
        cumulative_explained_variance = np.cumsum(explained_variance_ratio)
        
        return {
            'transformed_data': transformed_data,
            'explained_variance_ratio': explained_variance_ratio,
            'cumulative_explained_variance': cumulative_explained_variance,
            'n_components': n_components,
            'singular_values': pca.singular_values_,
            'components': pca.components_,
            'mean': pca.mean_,
            'n_features': pca.n_features_in_
        }
    
    @staticmethod
    def apply_tsne(data: np.ndarray,
                  n_components: int = 2,
                  perplexity: float = 30.0,
                  random_state: int = 42,
                  n_iter: int = 1000) -> Dict[str, Any]:
        """Apply t-Distributed Stochastic Neighbor Embedding"""
        from sklearn.manifold import TSNE
        
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        tsne = TSNE(n_components=n_components, perplexity=perplexity, 
                   random_state=random_state, n_iter=n_iter)
        transformed_data = tsne.fit_transform(data_scaled)
        
        return {
            'transformed_data': transformed_data,
            'perplexity': perplexity,
            'n_iter': n_iter,
            'kl_divergence': tsne.kl_divergence_ if hasattr(tsne, 'kl_divergence_') else None
        }
    
    @staticmethod
    def apply_umap(data: np.ndarray,
                  n_components: int = 2,
                  n_neighbors: int = 15,
                  min_dist: float = 0.1,
                  random_state: int = 42) -> Dict[str, Any]:
        """Apply Uniform Manifold Approximation and Projection"""
        try:
            from umap import UMAP
            
            scaler = StandardScaler()
            data_scaled = scaler.fit_transform(data)
            
            reducer = UMAP(n_components=n_components, n_neighbors=n_neighbors,
                          min_dist=min_dist, random_state=random_state)
            transformed_data = reducer.fit_transform(data_scaled)
            
            return {
                'transformed_data': transformed_data,
                'n_neighbors': n_neighbors,
                'min_dist': min_dist,
                'embedding_similarity': reducer.embedding_similarity_ if hasattr(reducer, 'embedding_similarity_') else None
            }
        except ImportError:
            raise ImportError("UMAP not installed. Install with: pip install umap-learn")
    
    @staticmethod
    def apply_ica(data: np.ndarray,
                 n_components: int = 2,
                 algorithm: str = 'factorial',
                 random_state: int = 42) -> Dict[str, Any]:
        """Apply Independent Component Analysis"""
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        ica = FastICA(n_components=n_components, algorithm=algorithm, 
                     random_state=random_state, max_iter=1000)
        transformed_data = ica.fit_transform(data_scaled)
        
        return {
            'transformed_data': transformed_data,
            'mixing_matrix': ica.mixing_,
            'unmixing_matrix': ica.components_,
            'mean': ica.mean_,
            'n_components': n_components,
            'algorithm': algorithm
        }
    
    @staticmethod
    def apply_autoencoder(data: np.ndarray,
                        n_components: int = 2,
                        hidden_layers: List[int] = None,
                        epochs: int = 100,
                        learning_rate: float = 0.001,
                        random_state: int = 42) -> Dict[str, Any]:
        """Apply neural autoencoder for dimensionality reduction"""
        try:
            from tensorflow.keras.models import Sequential
            from tensorflow.keras.layers import Dense, Input
            from tensorflow.keras.callbacks import EarlyStopping
            
            if hidden_layers is None:
                hidden_layers = [64, 32, 64]
            
            scaler = StandardScaler()
            data_scaled = scaler.fit_transform(data)
            
            # Build autoencoder
            model = Sequential()
            model.add(Input(shape=(data.shape[1],)))
            
            # Encoder
            for layer_size in hidden_layers:
                model.add(Dense(layer_size, activation='relu'))
            
            # Bottleneck
            model.add(Dense(n_components, activation='linear'))
            
            # Decoder
            for layer_size in reversed(hidden_layers):
                model.add(Dense(layer_size, activation='relu'))
            
            model.add(Dense(data.shape[1], activation='linear'))
            
            model.compile(optimizer='adam', loss='mse')
            
            # Train autoencoder
            early_stopping = EarlyStopping(patience=10, restore_best_weights=True)
            history = model.fit(data_scaled, data_scaled,
                              epochs=epochs, batch_size=32,
                              shuffle=True, verbose=0,
                              callbacks=[early_stopping])
            
            # Get encoded representation
            encoder_model = Sequential(model.layers[:-len(hidden_layers)-1])
            encoded_data = encoder_model.predict(data_scaled)
            
            return {
                'transformed_data': encoded_data,
                'encoded_representation': encoded_data,
                'autoencoder_model': model,
                'encoder_model': encoder_model,
                'decoder_model': Sequential(model.layers[-len(hidden_layers)-1:]),
                'training_history': history.history if 'history' in locals() else None,
                'n_components': n_components,
                'hidden_layers': hidden_layers,
                'epochs': epochs,
                'learning_rate': learning_rate
            }
            
        except ImportError:
            raise ImportError("TensorFlow not installed. Install with: pip install tensorflow")
    
    @staticmethod
    def apply_custom_pipelines(data: np.ndarray,
                              methods: List[str],
                              random_state: int = 42) -> Dict[str, Any]:
        """Apply multiple dimensionality reduction methods"""
        results = {}
        
        for method in methods:
            if method == 'pca':
                results['pca'] = DimensionalityReducer.apply_pca(data, random_state=random_state)
            elif method == 'tsne':
                results['tsne'] = DimensionalityReducer.apply_tsne(data, random_state=random_state)
            elif method == 'umap':
                try:
                    results['umap'] = DimensionalityReducer.apply_umap(data, random_state=random_state)
                except ImportError:
                    results['umap'] = {'error': 'UMAP not available'}
            elif method == 'ica':
                results['ica'] = DimensionalityReducer.apply_ica(data, random_state=random_state)
            elif method == 'autoencoder':
                try:
                    results['autoencoder'] = DimensionalityReducer.apply_autoencoder(data, random_state=random_state)
                except ImportError:
                    results['autoencoder'] = {'error': 'TensorFlow not available'}
        
        return results
class ClusteringAnalyzer:
    """Advanced clustering analysis for neural data"""
    
    @staticmethod
    def apply_kmeans(data: np.ndarray,
                    n_clusters: int = 3,
                    n_init: int = 10,
                    random_state: int = 42) -> Dict[str, Any]:
        """Apply K-means clustering"""
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        kmeans = KMeans(n_clusters=n_clusters, n_init=n_init, 
                       random_state=random_state, n_jobs=-1)
        cluster_labels = kmeans.fit_predict(data_scaled)
        
        # Calculate clustering metrics
        from sklearn.metrics import silhouette_score, calinski_harabasz_score, davies_bouldin_score
        
        silhouette = silhouette_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        calinski = calinski_harabasz_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        davies = davies_bouldin_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        
        # Cluster centers in original space
        centers_scaled = kmeans.cluster_centers_
        centers_original = scaler.inverse_transform(centers_scaled)
        
        return {
            'cluster_labels': cluster_labels,
            'cluster_centers': centers_original,
            'cluster_centers_scaled': centers_scaled,
            'inertia': kmeans.inertia_,
            'n_clusters': n_clusters,
            'silhouette_score': silhouette,
            'calinski_harabasz_score': calinski,
            'davies_bouldin_score': davies,
            'n_iterations': kmeans.n_iter_
        }
    
    @staticmethod
    def apply_dbscan(data: np.ndarray,
                    eps: float = 0.5,
                    min_samples: int = 5,
                    metric: str = 'euclidean') -> Dict[str, Any]:
        """Apply DBSCAN clustering"""
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        dbscan = DBSCAN(eps=eps, min_samples=min_samples, metric=metric)
        cluster_labels = dbscan.fit_predict(data_scaled)
        
        # Calculate metrics for each cluster
        unique_labels = np.unique(cluster_labels)
        n_clusters = len(unique_labels[unique_labels != -1])  # -1 is noise
        n_noise = np.sum(cluster_labels == -1)
        
        cluster_metrics = {}
        for label in unique_labels:
            if label == -1:
                continue
            
            cluster_points = data_scaled[cluster_labels == label]
            if len(cluster_points) > 1:
                cluster_metrics[f'cluster_{label}'] = {
                    'size': len(cluster_points),
                    'mean': np.mean(cluster_points, axis=0),
                    'std': np.std(cluster_points, axis=0),
                    'min': np.min(cluster_points, axis=0),
                    'max': np.max(cluster_points, axis=0)
                }
        
        return {
            'cluster_labels': cluster_labels,
            'n_clusters': n_clusters,
            'n_noise_points': n_noise,
            'eps': eps,
            'min_samples': min_samples,
            'cluster_metrics': cluster_metrics
        }
    
    @staticmethod
    def apply_hierarchical(data: np.ndarray,
                          n_clusters: int = 3,
                          method: str = 'ward',
                          metric: str = 'euclidean') -> Dict[str, Any]:
        """Apply hierarchical clustering"""
        scaler = StandardScaler()
        data_scaled = scaler.fit_transform(data)
        
        # Perform hierarchical clustering
        linkage_matrix = hierarchy.linkage(data_scaled, method=method, metric=metric)
        
        # Cut tree to get clusters
        cluster_labels = hierarchy.fcluster(linkage_matrix, n_clusters, criterion='maxclust')
        
        # Calculate metrics
        from sklearn.metrics import silhouette_score, calinski_harabasz_score, davies_bouldin_score
        
        silhouette = silhouette_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        calinski = calinski_harabasz_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        davies = davies_bouldin_score(data_scaled, cluster_labels) if n_clusters > 1 else 0
        
        # Visualize dendrogram (simplified)
        try:
            import matplotlib.pyplot as plt
            plt.figure(figsize=(10, 6))
            hierarchy.dendrogram(linkage_matrix, truncate_mode='level', p=5)
            plt.title('Hierarchical Clustering Dendrogram')
            plt.tight_layout()
            dendrogram_path = 'hierarchical_dendrogram.png'
            plt.savefig(dendrogram_path)
            plt.close()
            dendrogram_file = dendrogram_path
        except:
            dendrogram_file = None
        
        return {
            'cluster_labels': cluster_labels,
            'linkage_matrix': linkage_matrix,
            'n_clusters': n_clusters,
            'silhouette_score': silhouette,
            'calinski_harabasz_score': calinski,
            'davies_bouldin_score': davies,
            'dendrogram_file': dendrogram_file
        }
    
    @staticmethod
    def apply_gaussian_mixture(data: np.ndarray,
                              n_components: int = 3,
                              covariance_type: str = 'full',
                              random_state: int = 42) -> Dict[str, Any]:
        """Apply Gaussian Mixture Model clustering"""
        try:
            from sklearn.mixture import GaussianMixture
            
            scaler = StandardScaler()
            data_scaled = scaler.fit_transform(data)
            
            gmm = GaussianMixture(n_components=n_components, 
                                covariance_type=covariance_type,
                                random_state=random_state)
            cluster_labels = gmm.fit_predict(data_scaled)
            
            # Calculate metrics
            from sklearn.metrics import silhouette_score
            
            silhouette = silhouette_score(data_scaled, cluster_labels) if n_components > 1 else 0
            
            # Get cluster parameters
            cluster_params = {
                'means': gmm.means_,
                'covariances': gmm.covariances_,
                'weights': gmm.weights_,
                'precisions': gmm.precisions_
            }
            
            # Calculate responsibilities for each point
            responsibilities = gmm.predict_proba(data_scaled)
            
            return {
                'cluster_labels': cluster_labels,
                'cluster_parameters': cluster_params,
                'cluster_responsibilities': responsibilities,
                'n_components': n_components,
                'covariance_type': covariance_type,
                'silhouette_score': silhouette,
                'bic': gmm.bic(data_scaled),
                'aic': gmm.aic(data_scaled)
            }
            
        except ImportError:
            raise ImportError("scikit-learn not installed. Install with: pip install scikit-learn")
class ClassificationAnalyzer:
    """Advanced classification tools for neural data"""
    
    @staticmethod
    def train_svm_classifier(features: np.ndarray,
                            labels: np.ndarray,
                            test_size: float = 0.2,
                            kernel: str = 'rbf',
                            random_state: int = 42) -> Dict[str, Any]:
        """Train Support Vector Machine classifier"""
        X_train, X_test, y_train, y_test = train_test_split(
            features, labels, test_size=test_size, random_state=random_state
        )
        
        # Scale features
        scaler = StandardScaler()
        X_train_scaled = scaler.fit_transform(X_train)
        X_test_scaled = scaler.transform(X_test)
        
        # Train SVM
        svm = SVC(kernel=kernel, random_state=random_state)
        svm.fit(X_train_scaled, y_train)
        
        # Predict and evaluate
        y_pred = svm.predict(X_test_scaled)
        
        # Calculate metrics
        accuracy = accuracy_score(y_test, y_pred)
        precision = precision_score(y_test, y_pred, average='weighted', zero_division=0)
        recall = recall_score(y_test, y_pred, average='weighted', zero_division=0)
        f1 = f1_score(y_test, y_pred, average='weighted', zero_division=0)
        
        # Cross-validation
        cv_scores = cross_val_score(svm, X_train_scaled, y_train, cv=5)
        cv_mean = np.mean(cv_scores)
        cv_std = np.std(cv_scores)
        
        return {
            'model': svm,
            'scaler': scaler,
            'accuracy': accuracy,
            'precision': precision,
            'recall': recall,
            'f1_score': f1,
            'cross_validation_mean': cv_mean,
            'cross_validation_std': cv_std,
            'predictions': y_pred,
            'true_labels': y_test,
            'feature_importance': None  # SVM doesn't provide feature importance directly
        }
    
    @staticmethod
    def train_random_forest(features: np.ndarray,
                          labels: np.ndarray,
                          test_size: float = 0.2,
                          n_estimators: int = 100,
                          random_state: int = 42) -> Dict[str, Any]:
        """Train Random Forest classifier"""
        X_train, X_test, y_train, y_test = train_test_split(
            features, labels, test_size=test_size, random_state=random_state
        )
        
        # Scale features
        scaler = StandardScaler()
        X_train_scaled = scaler.fit_transform(X_train)
        X_test_scaled = scaler.transform(X_test)
        
        # Train Random Forest
        rf = RandomForestClassifier(n_estimators=n_estimators, 
                                  random_state=random_state, n_jobs=-1)
        rf.fit(X_train_scaled, y_train)
        
        # Predict and evaluate
        y_pred = rf.predict(X_test_scaled)
        
        # Calculate metrics
        accuracy = accuracy_score(y_test, y_pred)
        precision = precision_score(y_test, y_pred, average='weighted', zero_division=0)
        recall = recall_score(y_test, y_pred, average='weighted', zero_division=0)
        f1 = f1_score(y_test, y_pred, average='weighted', zero_division=0)
        
        # Cross-validation
        cv_scores = cross_val_score(rf, X_train_scaled, y_train, cv=5)
        cv_mean = np.mean(cv_scores)
        cv_std = np.std(cv_scores)
        
        return {
            'model': rf,
            'scaler': scaler,
            'accuracy': accuracy,
            'precision': precision,
            'recall': recall,
            'f1_score': f1,
            'cross_validation_mean': cv_mean,
            'cross_validation_std': cv_std,
            'predictions': y_pred,
            'true_labels': y_test,
            'feature_importance': rf.feature_importances_
        }
    
    @staticmethod
    def train_neural_network(features: np.ndarray,
                           labels: np.ndarray,
                           test_size: float = 0.2,
                           hidden_layers: List[int] = (64, 32),
                           epochs: int = 100,
                           random_state: int = 42) -> Dict[str, Any]:
        """Train neural network classifier"""
        try:
            from tensorflow.keras.models import Sequential
            from tensorflow.keras.layers import Dense, Dropout, Input
            from tensorflow.keras.utils import to_categorical
            from tensorflow.keras.callbacks import EarlyStopping
            
            X_train, X_test, y_train, y_test = train_test_split(
                features, labels, test_size=test_size, random_state=random_state
            )
            
            # Scale features
            scaler = StandardScaler()
            X_train_scaled = scaler.fit_transform(X_train)
            X_test_scaled = scaler.transform(X_test)
            
            # Convert labels to categorical if needed
            n_classes = len(np.unique(labels))
            if n_classes > 2:
                y_train_cat = to_categorical(y_train)
                y_test_cat = to_categorical(y_test)
                loss_function = 'categorical_crossentropy'
                y_train_target = y_train_cat
                y_test_target = y_test_cat
            else:
                y_train_target = y_train
                y_test_target = y_test
                loss_function = 'binary_crossentropy'
            
            # Build neural network
            model = Sequential()
            model.add(Input(shape=(X_train_scaled.shape[1],)))
            
            for layer_size in hidden_layers:
                model.add(Dense(layer_size, activation='relu'))
                model.add(Dropout(0.2))
            
            # Output layer
            if n_classes > 2:
                model.add(Dense(n_classes, activation='softmax'))
            else:
                model.add(Dense(1, activation='sigmoid'))
            
            model.compile(optimizer='adam', loss=loss_function, metrics=['accuracy'])
            
            # Train model
            early_stopping = EarlyStopping(patience=10, restore_best_weights=True)
            history = model.fit(X_train_scaled, y_train_target,
                              epochs=epochs, batch_size=32,
                              shuffle=True, verbose=0,
                              validation_split=0.1,
                              callbacks=[early_stopping])
            
            # Evaluate model
            y_pred_prob = model.predict(X_test_scaled)
            if n_classes > 2:
                y_pred = np.argmax(y_pred_prob, axis=1)
            else:
                y_pred = (y_pred_prob > 0.5).astype(int).flatten()
            
            # Calculate metrics
            accuracy = accuracy_score(y_test, y_pred)
            precision = precision_score(y_test, y_pred, average='weighted', zero_division=0)
            recall = recall_score(y_test, y_pred, average='weighted', zero_division=0)
            f1 = f1_score(y_test, y_pred, average='weighted', zero_division=0)
            
            return {
                'model': model,
                'scaler': scaler,
                'accuracy': accuracy,
                'precision': precision,
                'recall': recall,
                'f1_score': f1,
                'predictions': y_pred,
                'true_labels': y_test,
                'prediction_probabilities': y_pred_prob,
                'training_history': history.history,
                'n_classes': n_classes
            }
            
        except ImportError:
            raise ImportError("TensorFlow not installed. Install with: pip install tensorflow")
class FeatureExtractor:
    """Advanced feature extraction for neural data"""
    
    @staticmethod
    def extract_statistical_features(time_series: np.ndarray) -> np.ndarray:
        """Extract statistical features from time series"""
        if len(time_series) == 0:
            return np.array([])
        
        features = []
        
        # Basic statistics
        features.extend([
            np.mean(time_series),
            np.std(time_series),
            np.min(time_series),
            np.max(time_series),
            np.median(time_series),
            np.percentile(time_series, 25),
            np.percentile(time_series, 75),
            stats.skew(time_series),
            stats.kurtosis(time_series)
        ])
        
        # Temporal features
        diff = np.diff(time_series)
        features.extend([
            np.mean(diff),
            np.std(diff),
            np.max(diff),
            np.min(diff),
            np.median(diff),
            stats.skew(diff) if len(diff) > 2 else 0,
            stats.kurtosis(diff) if len(diff) > 3 else 0
        ])
        
        # Zero-crossing features
        zero_crossings = np.where(np.diff(np.sign(time_series)))[0]
        features.extend([
            len(zero_crossings),
            len(zero_crossings) / len(time_series),
            np.std(zero_crossings) if len(zero_crossings) > 1 else 0,
            np.mean(zero_crossings) if len(zero_crossings) > 0 else 0
        ])
        
        # Autocorrelation features
        if len(time_series) > 10:
            autocorr = np.correlate(time_series, time_series, mode='full')[len(time_series)-1:]
            autocorr = autocorr / np.sum(time_series**2)
            # Use first 10 lags
            autocorr = autocorr[:min(10, len(autocorr))]
            features.extend(autocorr.tolist())
        else:
            features.extend([0] * 10)
        
        # Frequency domain features
        try:
            fft_values = np.fft.fft(time_series)
            power_spectrum = np.abs(fft_values) ** 2
            
            # Spectral features
            dominant_freq_idx = np.argmax(power_spectrum[1:len(power_spectrum)//2]) + 1
            dominant_freq = dominant_freq_idx / len(time_series)
            total_power = np.sum(power_spectrum)
            dominant_power = power_spectrum[dominant_freq_idx]
            spectral_entropy = -np.sum((power_spectrum / total_power) * np.log(power_spectrum / total_power + 1e-10))
            
            features.extend([
                dominant_freq,
                dominant_power / total_power,
                spectral_entropy,
                np.sum(power_spectrum[:len(power_spectrum)//2]) / total_power
            ])
            
        except Exception:
            features.extend([0] * 4)
        
        return np.array(features)
    
    @staticmethod
    def extract_spectral_features(time_series: np.ndarray,
                                 sampling_rate: float = 1.0) -> np.ndarray:
        """Extract spectral features from time series"""
        if len(time_series) == 0:
            return np.array([])
        
        # Compute FFT
        n = len(time_series)
        fft_values = np.fft.fft(time_series)
        freqs = np.fft.fftfreq(n, 1/sampling_rate)
        power_spectrum = np.abs(fft_values) ** 2
        
        # Focus on positive frequencies
        positive_freqs = freqs[freqs > 0]
        positive_power = power_spectrum[freqs > 0]
        
        if len(positive_freqs) == 0:
            return np.array([0] * 10)
        
        features = []
        
        # Basic spectral features
        features.extend([
            np.max(positive_power),
            np.argmax(positive_power) / len(positive_freqs) if len(positive_freqs) > 0 else 0,
            np.sum(positive_power) / np.sum(power_spectrum),
            np.sqrt(np.mean(positive_power ** 2))
        ])
        
        # Spectral entropy
        psd_normalized = positive_power / np.sum(positive_power)
        psd_normalized = psd_normalized[psd_normalized > 0]
        spectral_entropy = -np.sum(psd_normalized * np.log(psd_normalized)) if len(psd_normalized) > 0 else 0
        features.append(spectral_entropy)
        
        # Bandwidth (frequency range containing 50% of power)
        half_power = np.sum(positive_power) / 2
        cumulative_power = np.cumsum(positive_power)
        bandwidth_idx = np.searchsorted(cumulative_power, half_power)
        bandwidth = positive_freqs[bandwidth_idx] if bandwidth_idx < len(positive_freqs) else 0
        features.append(bandwidth)
        
        # Spectral slope
        if len(positive_freqs) > 2:
            log_freq = np.log(np.abs(positive_freqs) + 1e-10)
            log_power = np.log(np.abs(positive_power) + 1e-10)
            
            if len(np.unique(log_freq)) > 1 and np.std(log_freq) > 0:
                slope = np.polyfit(log_freq, log_power, 1)[0]
                features.append(slope)
            else:
                features.append(0)
        else:
            features.append(0)
        
        # Harmonic features
        peak_idx = np.argmax(positive_power)
        harmonic_power = 0
        for harmonic in range(2, 5):
            if peak_idx + harmonic < len(positive_power):
                harmonic_power += positive_power[peak_idx + harmonic]
        features.append(harmonic_power / (np.sum(positive_power) + 1e-10))
        
        # Skewness and kurtosis of power spectrum
        features.extend([
            stats.skew(positive_power) if len(positive_power) > 2 else 0,
            stats.kurtosis(positive_power) if len(positive_power) > 3 else 0
        ])
        
        return np.array(features)
    
    @staticmethod
    def extract_temporal_features(time_series: np.ndarray) -> np.ndarray:
        """Extract temporal features from time series"""
        if len(time_series) == 0:
            return np.array([])
        
        features = []
        
        # Statistical features
        features.extend([
            np.mean(time_series),
            np.std(time_series),
            np.min(time_series),
            np.max(time_series),
            np.median(time_series),
            stats.skew(time_series) if len(time_series) > 2 else 0,
            stats.kurtosis(time_series) if len(time_series) > 3 else 0
        ])
        
        # Change detection
        diff = np.diff(time_series)
        mean_change = np.mean(diff)
        std_change = np.std(diff)
        
        # Rate of change
        rate_of_change = np.sum(diff != 0) / len(diff) if len(diff) > 0 else 0
        
        features.extend([
            mean_change,
            std_change,
            rate_of_change,
            np.max(diff),
            np.min(diff),
            np.max(np.abs(diff))  # Maximum absolute change
        ])
        
        # Local statistics (sliding window)
        window_size = min(10, len(time_series) // 2)
        if window_size >= 3:
            local_means = []
            local_stds = []
            for i in range(len(time_series) - window_size + 1):
                window = time_series[i:i + window_size]
                local_means.append(np.mean(window))
                local_stds.append(np.std(window))
            
            features.extend([
                np.mean(local_means),
                np.std(local_means),
                np.mean(local_stds),
                np.std(local_stds),
                np.max(local_means) - np.min(local_means),
                np.max(local_stds) - np.min(local_stds)
            ])
        else:
            features.extend([0] * 7)
        
        return np.array(features)
    
    @staticmethod
    def extract_multiscale_features(time_series: np.ndarray,
                                   scales: List[int] = None) -> np.ndarray:
        """Extract multiscale features using wavelet-like decomposition"""
        if scales is None:
            scales = [2, 4, 8, 16, 32]
        
        features = []
        
        for scale in scales:
            # Downsample the signal
            downsampled = time_series[::scale]
            if len(downsampled) < 2:
                continue
            
            # Extract features from downsampled signal
            mean = np.mean(downsampled)
            std = np.std(downsampled)
            range_val = np.max(downsampled) - np.min(downsampled)
            
            # Local variation
            if len(downsampled) > 1:
                variation = np.std(np.diff(downsampled)) / (np.mean(np.abs(downsampled)) + 1e-10)
            else:
                variation = 0
            
            # Detrend (simple linear detrending)
            x = np.arange(len(downsampled))
            slope, intercept, _, _, _ = stats.linregress(x, downsampled)
            detrended = downsampled - (slope * x + intercept)
            detrended_variance = np.var(detrended)
            
            features.extend([mean, std, range_val, variation, detrended_variance])
        
        return np.array(features)

class StatisticalTester:
    """Statistical testing utilities for neural data"""
    
    @staticmethod
    def independent_t_test(group1: np.ndarray, group2: np.ndarray) -> Dict[str, Any]:
        """Perform independent t-test between two groups"""
        if len(group1) < 2 or len(group2) < 2:
            return {
                't_statistic': 0.0,
                'p_value': 1.0,
                'mean_difference': 0.0,
                'standard_error': 0.0,
                'degrees_of_freedom': 0,
                'effect_size': 0.0,
                'confidence_interval': (0.0, 0.0),
                'significant': False
            }
        
        # Perform t-test
        t_stat, p_value = stats.ttest_ind(group1, group2, equal_var=False)
        
        # Calculate additional statistics
        mean1, mean2 = np.mean(group1), np.mean(group2)
        std1, std2 = np.std(group1, ddof=1), np.std(group2, ddof=1)
        n1, n2 = len(group1), len(group2)
        
        # Pooled standard error
        se = np.sqrt(std1**2 / n1 + std2**2 / n2)
        mean_diff = mean1 - mean2
        
        # Degrees of freedom (Welch-Satterthwaite)
        df = ((std1**2 / n1 + std2**2 / n2)**2) / (
            (std1**2 / n1)**2 / (n1 - 1) + (std2**2 / n2)**2 / (n2 - 1)
        )
        
        # Effect size (Cohen's d)
        pooled_std = np.sqrt(((n1 - 1) * std1**2 + (n2 - 1) * std2**2) / (n1 + n2 - 2))
        effect_size = mean_diff / pooled_std if pooled_std > 0 else 0
        
        # Confidence interval
        t_critical = stats.t.ppf(0.975, df)
        margin = t_critical * se
        ci_lower = mean_diff - margin
        ci_upper = mean_diff + margin
        
        return {
            't_statistic': t_stat,
            'p_value': p_value,
            'mean_difference': mean_diff,
            'standard_error': se,
            'degrees_of_freedom': df,
            'effect_size': effect_size,
            'confidence_interval': (ci_lower, ci_upper),
            'significant': p_value < 0.05,
            'mean_group1': mean1,
            'mean_group2': mean2,
            'std_group1': std1,
            'std_group2': std2
        }
    
    @staticmethod
    def wilcoxon_signed_rank_test(paired1: np.ndarray, paired2: np.ndarray) -> Dict[str, Any]:
        """Perform Wilcoxon signed-rank test for paired samples"""
        if len(paired1) != len(paired2) or len(paired1) < 2:
            return {
                'w_statistic': 0.0,
                'p_value': 1.0,
                'mean_difference': 0.0,
                'median_difference': 0.0,
                'significant': False
            }
        
        # Calculate differences
        differences = paired1 - paired2
        non_zero_diffs = differences[differences != 0]
        
        if len(non_zero_diffs) < 2:
            return {
                'w_statistic': 0.0,
                'p_value': 1.0,
                'mean_difference': np.mean(differences),
                'median_difference': np.median(differences),
                'significant': False
            }
        
        # Perform Wilcoxon test
        w_stat, p_value = stats.wilcoxon(non_zero_diffs)
        
        return {
            'w_statistic': w_stat,
            'p_value': p_value,
            'mean_difference': np.mean(differences),
            'median_difference': np.median(differences),
            'std_difference': np.std(differences),
            'significant': p_value < 0.05,
            'n_nonzero_pairs': len(non_zero_diffs)
        }
    
    @staticmethod
    def chi_square_test(observed_counts: List[List[int]]) -> Dict[str, Any]:
        """Perform chi-square test of independence"""
        try:
            chi2_stat, p_value, dof, expected = stats.chi2_contingency(observed_counts)
            
            # Calculate effect size (Cramer's V)
            n = np.sum(observed_counts)
            min_dim = min(observed_counts.shape) - 1
            cramers_v = np.sqrt(chi2_stat / (n * min_dim)) if n * min_dim > 0 else 0
            
            return {
                'chi2_statistic': chi2_stat,
                'p_value': p_value,
                'degrees_of_freedom': dof,
                'expected_frequencies': expected,
                'cramers_v': cramers_v,
                'significant': p_value < 0.05,
                'effect_size': cramers_v
            }
        except Exception as e:
            return {
                'chi2_statistic': 0.0,
                'p_value': 1.0,
                'degrees_of_freedom': 0,
                'expected_frequencies': np.array([]),
                'cramers_v': 0.0,
                'significant': False,
                'effect_size': 0.0,
                'error': str(e)
            }
    
    @staticmethod
    def anova_test(groups: List[np.ndarray]) -> Dict[str, Any]:
        """Perform one-way ANOVA test"""
        if len(groups) < 2:
            return {
                'f_statistic': 0.0,
                'p_value': 1.0,
                'degrees_of_freedom': (0, 0),
                'group_means': [],
                'group_stds': [],
                'significant': False
            }
        
        try:
            # Perform ANOVA
            f_stat, p_value = stats.f_oneway(*groups)
            
            # Calculate additional statistics
            group_means = [np.mean(g) for g in groups]
            group_stds = [np.std(g, ddof=1) for g in groups]
            group_sizes = [len(g) for g in groups]
            
            # Total degrees of freedom
            dof_between = len(groups) - 1
            dof_within = sum(group_sizes) - len(groups)
            
            # Calculate eta-squared (effect size)
            grand_mean = np.mean(np.concatenate(groups))
            ss_between = sum([len(g) * (mean - grand_mean) ** 2 
                             for g, mean in zip(groups, group_means)])
            ss_within = sum([(len(g) - 1) * (std ** 2) 
                           for g, std in zip(groups, group_stds)])
            ss_total = ss_between + ss_within
            
            eta_squared = ss_between / ss_total if ss_total > 0 else 0
            
            return {
                'f_statistic': f_stat,
                'p_value': p_value,
                'degrees_of_freedom': (dof_between, dof_within),
                'group_means': group_means,
                'group_stds': group_stds,
                'group_sizes': group_sizes,
                'eta_squared': eta_squared,
                'significant': p_value < 0.05
            }
            
        except Exception as e:
            return {
                'f_statistic': 0.0,
                'p_value': 1.0,
                'degrees_of_freedom': (0, 0),
                'group_means': [],
                'group_stds': [],
                'group_sizes': [],
                'eta_squared': 0.0,
                'significant': False,
                'error': str(e)
            }
class AdvancedNLMTools:
    """Main advanced analysis tools for NLM neural data"""
    
    def __init__(self):
        self.statistical_analyzer = AdvancedStatisticalAnalyzer()
        self.wavelet_analyzer = WaveletAnalyzer()
        self.network_analyzer = NetworkAnalyzer()
        self.dimensionality_reducer = DimensionalityReducer()
        self.clustering_analyzer = ClusteringAnalyzer()
        self.classification_analyzer = ClassificationAnalyzer()
        self.feature_extractor = FeatureExtractor()
        self.statistical_tester = StatisticalTester()
    
    def analyze_time_series(self, data: TimeSeriesData, methods: List[str] = None) -> Dict[str, AnalysisResult]:
        """Perform comprehensive time series analysis"""
        return self.statistical_analyzer.time_series_analysis(data, methods)
    
    def analyze_wavelet(self, data: TimeSeriesData, wavelet: str = "morlet") -> WaveletData:
        """Perform wavelet analysis"""
        return self.wavelet_analyzer.continuous_wavelet_transform(data, wavelet)
    
    def analyze_network(self, neural_data: List[np.ndarray], method: str = "correlation") -> NetworkData:
        """Analyze neural connectivity"""
        return self.network_analyzer.compute_connectivity_matrix(neural_data, method)
    
    def reduce_dimensions(self, data: np.ndarray, methods: List[str] = None) -> Dict[str, Any]:
        """Apply dimensionality reduction"""
        if methods is None:
            methods = ['pca', 'tsne']
        return self.dimensionality_reducer.apply_custom_pipelines(data, methods)
    
    def cluster_data(self, data: np.ndarray, method: str = 'kmeans', **kwargs) -> Dict[str, Any]:
        """Apply clustering"""
        if method == 'kmeans':
            return self.clustering_analyzer.apply_kmeans(data, **kwargs)
        elif method == 'dbscan':
            return self.clustering_analyzer.apply_dbscan(data, **kwargs)
        elif method == 'hierarchical':
            return self.clustering_analyzer.apply_hierarchical(data, **kwargs)
        elif method == 'gmm':
            return self.clustering_analyzer.apply_gaussian_mixture(data, **kwargs)
        else:
            raise ValueError(f"Unknown clustering method: {method}")
    
    def classify_data(self, features: np.ndarray, labels: np.ndarray, 
                     classifier: str = 'svm', **kwargs) -> Dict[str, Any]:
        """Train classification model"""
        if classifier == 'svm':
            return self.classification_analyzer.train_svm_classifier(features, labels, **kwargs)
        elif classifier == 'random_forest':
            return self.classification_analyzer.train_random_forest(features, labels, **kwargs)
        elif classifier == 'neural_network':
            return self.classification_analyzer.train_neural_network(features, labels, **kwargs)
        else:
            raise ValueError(f"Unknown classifier: {classifier}")
    
    def extract_features(self, time_series: np.ndarray, feature_types: List[str] = None) -> np.ndarray:
        """Extract features from time series"""
        if feature_types is None:
            feature_types = ['statistical', 'spectral', 'temporal']
        
        features = []
        
        if 'statistical' in feature_types:
            features.append(self.feature_extractor.extract_statistical_features(time_series))
        
        if 'spectral' in feature_types:
            features.append(self.feature_extractor.extract_spectral_features(time_series))
        
        if 'temporal' in feature_types:
            features.append(self.feature_extractor.extract_temporal_features(time_series))
        
        if 'multiscale' in feature_types:
            features.append(self.feature_extractor.extract_multiscale_features(time_series))
        
        if features:
            return np.hstack(features)
        else:
            return np.array([])
    
    def statistical_test(self, test_type: str, *args, **kwargs) -> Dict[str, Any]:
        """Perform statistical test"""
        if test_type == 't_test':
            return self.statistical_tester.independent_t_test(*args, **kwargs)
        elif test_type == 'wilcoxon':
            return self.statistical_tester.wilcoxon_signed_rank_test(*args, **kwargs)
        elif test_type == 'chi_square':
            return self.statistical_tester.chi_square_test(*args, **kwargs)
        elif test_type == 'anova':
            return self.statistical_tester.anova_test(*args, **kwargs)
        else:
            raise ValueError(f"Unknown statistical test: {test_type}")

# Convenience functions

def create_advanced_analyzer() -> AdvancedNLMTools:
    """Convenience function to create advanced analyzer"""
    return AdvancedNLMTools()

def analyze_neural_time_series(time_series: np.ndarray, 
                              timestamps: Optional[np.ndarray] = None,
                              sampling_rate: Optional[float] = None) -> Dict[str, AnalysisResult]:
    """Convenience function for neural time series analysis"""
    if timestamps is None:
        timestamps = np.arange(len(time_series))
    
    data = TimeSeriesData(
        timestamps=timestamps,
        values=time_series,
        sampling_rate=sampling_rate
    )
    
    analyzer = AdvancedNLMTools()
    return analyzer.analyze_time_series(data)

def extract_neural_features(time_series: np.ndarray, 
                           feature_types: List[str] = None) -> np.ndarray:
    """Convenience function for feature extraction"""
    analyzer = AdvancedNLMTools()
    return analyzer.extract_features(time_series, feature_types)

# Export all classes and functions
__all__ = [
    'AnalysisMethod', 'AnalysisResult', 'TimeSeriesData', 'WaveletData', 'NetworkData',
    'AdvancedStatisticalAnalyzer', 'WaveletAnalyzer', 'NetworkAnalyzer',
    'DimensionalityReducer', 'ClusteringAnalyzer', 'ClassificationAnalyzer',
    'FeatureExtractor', 'StatisticalTester', 'AdvancedNLMTools',
    'create_advanced_analyzer', 'analyze_neural_time_series',
    'extract_neural_features'
]