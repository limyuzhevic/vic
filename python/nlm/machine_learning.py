#!/usr/bin/env python3
"""
Machine Learning Utilities for Neural Data

This module provides machine learning utilities specifically designed for neural
simulation data, including classification, clustering, regression, and feature
extraction for neural networks and brain data analysis.
"""

import numpy as np
import pandas as pd
from typing import List, Dict, Tuple, Optional, Any, Union
from dataclasses import dataclass, field
from enum import Enum
import warnings
import math
from sklearn.preprocessing import StandardScaler, MinMaxScaler, Normalizer
from sklearn.decomposition import PCA, IncrementalPCA, FastICA
from sklearn.cluster import KMeans, DBSCAN, AgglomerativeClustering
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC, SVR
from sklearn.linear_model import LinearRegression, LogisticRegression
from sklearn.ensemble import RandomForestClassifier, IsolationForest
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score
from sklearn.metrics import mean_squared_error, r2_score

warnings.filterwarnings('ignore')

class MLModelType(Enum):
    """Machine learning model types"""
    CLASSIFICATION = "classification"
    REGRESSION = "regression"
    CLUSTERING = "clustering"
    ANOMALY_DETECTION = "anomaly_detection"
    DIMENSIONALITY_REDUCTION = "dimensionality_reduction"

@dataclass
class MLModelResult:
    """Container for machine learning model results"""
    model_type: MLModelType
    model_name: str
    performance_metrics: Dict[str, float]
    training_time: float
    prediction_time: float
    parameters: Dict[str, Any] = field(default_factory=dict)
    feature_importance: Optional[np.ndarray] = None
    cross_validation_scores: Optional[Dict[str, float]] = None

@dataclass
class NeuralFeatureSet:
    """Container for neural features extracted from simulation data"""
    features: np.ndarray
    labels: Optional[np.ndarray] = None
    feature_names: Optional[List[str]] = None
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        if self.feature_names is None:
            self.feature_names = [f"feature_{i}" for i in range(self.features.shape[1])]

class NeuralDataPreprocessor:
    """Preprocessing utilities for neural data"""
    
    @staticmethod
    def standardize_features(features: np.ndarray) -> np.ndarray:
        """Standardize features to zero mean and unit variance"""
        scaler = StandardScaler()
        return scaler.fit_transform(features)
    
    @staticmethod
    def normalize_features(features: np.ndarray, 
                         method: str = "minmax") -> np.ndarray:
        """Normalize features to [0, 1] range"""
        if method == "minmax":
            scaler = MinMaxScaler()
        elif method == "standard":
            scaler = StandardScaler()
        elif method == "normalize":
            scaler = Normalizer()
        else:
            scaler = MinMaxScaler()
        
        return scaler.fit_transform(features)
    
    @staticmethod
    def extract_time_domain_features(time_series: List[float]) -> np.ndarray:
        """Extract time domain features from neural time series"""
        if not time_series:
            return np.array([])
        
        ts = np.array(time_series)
        features = np.zeros(15)
        
        # Basic statistics
        features[0] = np.mean(ts)
        features[1] = np.std(ts)
        features[2] = np.min(ts)
        features[3] = np.max(ts)
        features[4] = features[3] - features[2]  # Range
        features[5] = np.median(ts)
        features[6] = stats.skew(ts)
        features[7] = stats.kurtosis(ts)
        
        # Temporal features
        diff = np.diff(ts)
        features[8] = np.mean(diff)
        features[9] = np.std(diff)
        features[10] = np.max(diff)
        features[11] = np.min(diff)
        
        # Zero-crossing features
        zero_crossings = np.where(np.diff(np.sign(ts)))[0]
        features[12] = len(zero_crossings) / len(ts) if len(ts) > 0 else 0
        
        # Autocorrelation features
        if len(ts) > 10:
            acf = np.correlate(ts, ts, mode='full')[len(ts)-1:] / np.sum(ts**2)
            features[13] = np.max(np.abs(acf[1:10]))
        else:
            features[13] = 0
        
        # Frequency domain features
        fft_vals = np.fft.fft(ts)
        power_spectrum = np.abs(fft_vals) ** 2
        features[14] = np.sum(power_spectrum[1:len(ts)//2]) / np.sum(power_spectrum)
        
        return features
    
    @staticmethod
    def extract_frequency_domain_features(time_series: List[float]) -> np.ndarray:
        """Extract frequency domain features from neural time series"""
        if not time_series:
            return np.array([])
        
        ts = np.array(time_series)
        n = len(ts)
        
        # Compute FFT
        fft_vals = np.fft.fft(ts)
        freqs = np.fft.fftfreq(n)
        power_spectrum = np.abs(fft_vals) ** 2
        
        features = np.zeros(10)
        
        # Basic spectral features
        features[0] = np.max(power_spectrum[1:n//2])  # Peak frequency power
        features[1] = np.argmax(power_spectrum[1:n//2]) / n  # Dominant frequency
        features[2] = np.sum(power_spectrum[1:n//2]) / np.sum(power_spectrum)  # Power in frequencies
        
        # Spectral entropy
        psd_norm = power_spectrum[1:n//2] / np.sum(power_spectrum[1:n//2])
        psd_norm = psd_norm[psd_norm > 0]  # Remove zeros
        if len(psd_norm) > 0:
            features[3] = -np.sum(psd_norm * np.log(psd_norm))
        else:
            features[3] = 0
        
        # Bandwidth
        half_power = np.sum(power_spectrum[1:n//2]) / 2
        cumulative = np.cumsum(power_spectrum[1:n//2])
        bandwidth_idx = np.searchsorted(cumulative, half_power)
        features[4] = freqs[bandwidth_idx] if bandwidth_idx < len(freqs) else 0
        
        # Spectral slope
        if len(power_spectrum[1:n//2]) > 2:
            freq_axis = freqs[1:n//2]
            log_freq = np.log(np.abs(freq_axis) + 1e-10)
            log_power = np.log(np.abs(power_spectrum[1:n//2]) + 1e-10)
            
            if len(np.unique(log_freq)) > 1 and np.std(log_freq) > 0:
                features[5] = np.polyfit(log_freq, log_power, 1)[0]
            else:
                features[5] = 0
        else:
            features[5] = 0
        
        # Higher order features
        features[6] = np.sqrt(np.mean(power_spectrum[1:n//2] ** 2))
        features[7] = np.mean(np.abs(np.diff(power_spectrum[1:n//2])))
        
        # Harmonic features
        peak_idx = np.argmax(power_spectrum[1:n//2])
        harmonic_power = 0
        for harmonic in range(2, 5):
            if peak_idx + harmonic < len(power_spectrum[1:n//2]):
                harmonic_power += power_spectrum[1:n//2][peak_idx + harmonic]
        features[8] = harmonic_power / (np.sum(power_spectrum[1:n//2]) + 1e-10)
        
        return features
class NeuralClassifier:
    """Neural data classification utilities"""
    
    @staticmethod
    def classify_neurons_by_firing_pattern(firing_rates: List[float],
                                          threshold: float = 0.5) -> Dict[str, Any]:
        """Classify neurons based on firing rate patterns"""
        if not firing_rates:
            return {"error": "No firing rates provided"}
        
        rates = np.array(firing_rates)
        
        # Define firing rate categories
        categories = {
            "silent": rates < threshold * 0.1,
            "quiescent": (threshold * 0.1 <= rates) & (rates < threshold * 0.5),
            "moderate": (threshold * 0.5 <= rates) & (rates < threshold * 2),
            "active": (threshold * 2 <= rates) & (rates < threshold * 5),
            "high": (threshold * 5 <= rates) & (rates < threshold * 10),
            "bursting": rates >= threshold * 10
        }
        
        classification_results = {}
        for category, mask in categories.items():
            if np.any(mask):
                classification_results[category] = {
                    "count": np.sum(mask),
                    "percentage": np.sum(mask) / len(rates) * 100,
                    "mean_rate": np.mean(rates[mask]),
                    "std_rate": np.std(rates[mask]) if np.sum(mask) > 1 else 0,
                    "min_rate": np.min(rates[mask]),
                    "max_rate": np.max(rates[mask])
                }
        
        return classification_results
    
    @staticmethod
    def train_neural_classifier(features: NeuralFeatureSet,
                               model_type: str = "random_forest",
                               test_size: float = 0.2,
                               random_state: int = 42) -> MLModelResult:
        """Train a neural classifier"""
        if features.features.shape[0] == 0:
            return MLModelResult(
                model_type=MLModelType.CLASSIFICATION,
                model_name="error",
                performance_metrics={"error": "No features provided"},
                training_time=0,
                prediction_time=0
            )
        
        X = features.features
        y = features.labels if features.labels is not None else None
        
        if y is None:
            return MLModelResult(
                model_type=MLModelType.CLASSIFICATION,
                model_name="error",
                performance_metrics={"error": "No labels provided"},
                training_time=0,
                prediction_time=0
            )
        
        # Split data
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=test_size, random_state=random_state
        )
        
        # Initialize model
        if model_type == "random_forest":
            model = RandomForestClassifier(random_state=random_state)
        elif model_type == "svm":
            model = SVC(random_state=random_state)
        elif model_type == "logistic_regression":
            model = LogisticRegression(random_state=random_state)
        elif model_type == "knn":
            model = KNeighborsClassifier()
        else:
            model = RandomForestClassifier(random_state=random_state)
        
        # Train model
        start_time = time.time()
        model.fit(X_train, y_train)
        training_time = time.time() - start_time
        
        # Predict
        start_time = time.time()
        y_pred = model.predict(X_test)
        prediction_time = time.time() - start_time
        
        # Compute metrics
        accuracy = accuracy_score(y_test, y_pred)
        precision = precision_score(y_test, y_pred, average='weighted', zero_division=0)
        recall = recall_score(y_test, y_pred, average='weighted', zero_division=0)
        f1 = f1_score(y_test, y_pred, average='weighted', zero_division=0)
        
        # Cross-validation
        cv_scores = cross_val_score(model, X_train, y_train, cv=5)
        cv_mean = np.mean(cv_scores)
        cv_std = np.std(cv_scores)
        
        return MLModelResult(
            model_type=MLModelType.CLASSIFICATION,
            model_name=model_type,
            performance_metrics={
                "accuracy": accuracy,
                "precision": precision,
                "recall": recall,
                "f1_score": f1,
                "cv_mean": cv_mean,
                "cv_std": cv_std
            },
            training_time=training_time,
            prediction_time=prediction_time,
            parameters=model.get_params(),
            feature_importance=getattr(model, 'feature_importances_', None)
        )
    
    @staticmethod
    def train_neural_regressor(features: NeuralFeatureSet,
                              model_type: str = "random_forest",
                              test_size: float = 0.2,
                              random_state: int = 42) -> MLModelResult:
        """Train a neural regressor"""
        if features.features.shape[0] == 0:
            return MLModelResult(
                model_type=MLModelType.REGRESSION,
                model_name="error",
                performance_metrics={"error": "No features provided"},
                training_time=0,
                prediction_time=0
            )
        
        X = features.features
        y = features.labels if features.labels is not None else None
        
        if y is None:
            return MLModelResult(
                model_type=MLModelType.REGRESSION,
                model_name="error",
                performance_metrics={"error": "No labels provided"},
                training_time=0,
                prediction_time=0
            )
        
        # Split data
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=test_size, random_state=random_state
        )
        
        # Initialize model
        if model_type == "random_forest":
            from sklearn.ensemble import RandomForestRegressor
            model = RandomForestRegressor(random_state=random_state)
        elif model_type == "svm":
            model = SVR()
        elif model_type == "linear_regression":
            model = LinearRegression()
        else:
            from sklearn.ensemble import RandomForestRegressor
            model = RandomForestRegressor(random_state=random_state)
        
        # Train model
        start_time = time.time()
        model.fit(X_train, y_train)
        training_time = time.time() - start_time
        
        # Predict
        start_time = time.time()
        y_pred = model.predict(X_test)
        prediction_time = time.time() - start_time
        
        # Compute metrics
        mse = mean_squared_error(y_test, y_pred)
        r2 = r2_score(y_test, y_pred)
        
        # Cross-validation
        cv_scores = cross_val_score(model, X_train, y_train, cv=5, scoring='neg_mean_squared_error')
        cv_mean = -np.mean(cv_scores)
        cv_std = np.std(cv_scores)
        
        return MLModelResult(
            model_type=MLModelType.REGRESSION,
            model_name=model_type,
            performance_metrics={
                "mse": mse,
                "r2_score": r2,
                "rmse": np.sqrt(mse),
                "cv_mean": cv_mean,
                "cv_std": cv_std
            },
            training_time=training_time,
            prediction_time=prediction_time,
            parameters=model.get_params(),
            feature_importance=getattr(model, 'feature_importances_', None)
        )
    
    @staticmethod
    def cluster_neural_data(features: NeuralFeatureSet,
                           model_type: str = "kmeans",
                           n_clusters: int = 3,
                           random_state: int = 42) -> MLModelResult:
        """Cluster neural data"""
        if features.features.shape[0] == 0:
            return MLModelResult(
                model_type=MLModelType.CLUSTERING,
                model_name="error",
                performance_metrics={"error": "No features provided"},
                training_time=0,
                prediction_time=0
            )
        
        X = features.features
        
        # Initialize model
        if model_type == "kmeans":
            model = KMeans(n_clusters=n_clusters, random_state=random_state, n_init=10)
        elif model_type == "dbscan":
            model = DBSCAN(eps=0.5, min_samples=5)
        elif model_type == "hierarchical":
            model = AgglomerativeClustering(n_clusters=n_clusters)
        else:
            model = KMeans(n_clusters=n_clusters, random_state=random_state, n_init=10)
        
        # Train model
        start_time = time.time()
        labels = model.fit_predict(X)
        training_time = time.time() - start_time
        
        # Predict (just use the same labels for clustering)
        start_time = time.time()
        predicted_labels = model.predict(X)
        prediction_time = time.time() - start_time
        
        # Compute metrics (silhouette score for clustering)
        from sklearn.metrics import silhouette_score
        if len(np.unique(labels)) > 1 and len(labels) > 1:
            silhouette = silhouette_score(X, labels)
        else:
            silhouette = 0
        
        return MLModelResult(
            model_type=MLModelType.CLUSTERING,
            model_name=model_type,
            performance_metrics={
                "silhouette_score": silhouette,
                "n_clusters": len(np.unique(labels)),
                "cluster_sizes": {i: np.sum(labels == i) for i in np.unique(labels)}
            },
            training_time=training_time,
            prediction_time=prediction_time,
            parameters=model.get_params(),
            cross_validation_scores={"silhouette": silhouette}
        )
    
    @staticmethod
    def detect_anomalies(features: NeuralFeatureSet,
                        model_type: str = "isolation_forest",
                        contamination: float = 0.1) -> MLModelResult:
        """Detect anomalies in neural data"""
        if features.features.shape[0] == 0:
            return MLModelResult(
                model_type=MLModelType.ANOMALY_DETECTION,
                model_name="error",
                performance_metrics={"error": "No features provided"},
                training_time=0,
                prediction_time=0
            )
        
        X = features.features
        
        # Initialize model
        if model_type == "isolation_forest":
            model = IsolationForest(contamination=contamination, random_state=42)
        else:
            model = IsolationForest(contamination=contamination, random_state=42)
        
        # Train model
        start_time = time.time()
        model.fit(X)
        training_time = time.time() - start_time
        
        # Predict
        start_time = time.time()
        anomaly_scores = model.decision_function(X)
        predictions = model.predict(X)
        prediction_time = time.time() - start_time
        
        # Compute metrics (simplified)
        n_anomalies = np.sum(predictions == -1)
        anomaly_ratio = n_anomalies / len(predictions)
        
        return MLModelResult(
            model_type=MLModelType.ANOMALY_DETECTION,
            model_name=model_type,
            performance_metrics={
                "n_anomalies": n_anomalies,
                "anomaly_ratio": anomaly_ratio,
                "contamination": contamination
            },
            training_time=training_time,
            prediction_time=prediction_time,
            parameters=model.get_params()
        )
    
    @staticmethod
    def reduce_dimensions(features: NeuralFeatureSet,
                         method: str = "pca",
                         n_components: int = 2) -> MLModelResult:
        """Reduce dimensionality of neural features"""
        if features.features.shape[0] == 0:
            return MLModelResult(
                model_type=MLModelType.DIMENSIONALITY_REDUCTION,
                model_name="error",
                performance_metrics={"error": "No features provided"},
                training_time=0,
                prediction_time=0
            )
        
        X = features.features
        
        # Initialize model
        if method == "pca":
            model = PCA(n_components=n_components)
        elif method == "ica":
            model = FastICA(n_components=n_components)
        elif method == "tsne":
            from sklearn.manifold import TSNE
            model = TSNE(n_components=n_components, random_state=42)
        else:
            model = PCA(n_components=n_components)
        
        # Train model
        start_time = time.time()
        X_reduced = model.fit_transform(X)
        training_time = time.time() - start_time
        
        # Predict (transform)
        start_time = time.time()
        X_transformed = model.transform(X)
        prediction_time = time.time() - start_time
        
        # Compute metrics
        explained_variance = np.sum(model.explained_variance_ratio_) if hasattr(model, 'explained_variance_ratio_') else 0
        
        return MLModelResult(
            model_type=MLModelType.DIMENSIONALITY_REDUCTION,
            model_name=method,
            performance_metrics={
                "explained_variance": explained_variance,
                "n_components": n_components,
                "original_dimensions": features.features.shape[1]
            },
            training_time=training_time,
            prediction_time=prediction_time,
            parameters=model.get_params(),
            feature_importance=getattr(model, 'components_', None)
        )
# Convenience functions

def classify_neurons(firing_rates: List[float], threshold: float = 0.5) -> Dict[str, Any]:
    """Convenience function to classify neurons by firing rate"""
    return NeuralClassifier.classify_neurons_by_firing_pattern(firing_rates, threshold)

def preprocess_neural_data(time_series: List[float]) -> np.ndarray:
    """Convenience function to preprocess neural time series data"">
    processor = NeuralDataPreprocessor()
    return processor.extract_time_domain_features(time_series)
