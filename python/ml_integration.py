#!/usr/bin/env python3
"""
NLM Machine Learning Pipeline Integration

This module provides machine learning pipeline integration for the NLM framework,
including data preprocessing, model training, evaluation, and deployment capabilities.
"""

import sys
import os
import json
import numpy as np
from typing import Dict, Any, List, Optional, Callable, Tuple, Union
from enum import Enum
import pandas as pd
from sklearn.model_selection import train_test_split, cross_val_score, GridSearchCV
from sklearn.preprocessing import StandardScaler, MinMaxScaler, Normalizer
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score, accuracy_score
from sklearn.ensemble import RandomForestRegressor, RandomForestClassifier
from sklearn.linear_model import LinearRegression, Ridge, LogisticRegression
from sklearn.neural_network import MLPRegressor, MLPClassifier
from sklearn.svm import SVR, SVC
from sklearn.gaussian_process import GaussianProcessRegressor
import warnings
warnings.filterwarnings('ignore')

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Try to import NLM modules
try:
    from brain.Brain import Brain
    from core.Config.Config import Config
    from visualization.VisualizationInterface import VisualizationInterface
    _NLM_AVAILABLE = True
    print("✓ NLM framework available")
except ImportError as e:
    print(f"⚠ NLM framework not available: {e}")
    _NLM_AVAILABLE = False

# Machine learning model type enumeration
class ModelType(Enum):
    REGRESSION = "regression"
    CLASSIFICATION = "classification"
    NEURAL_NETWORK = "neural_network"
    TREE_BASED = "tree_based"
    SVM = "svm"
    GAUSSIAN_PROCESS = "gaussian_process"

# Training strategy enumeration
class TrainingStrategy(Enum):
    SUPERVISED = "supervised"
    UN superVISED = "unsupervised"
    REINFORCEMENT = "reinforcement"
    SEMI_SUPERVISED = "semi_supervised"
    TRANSFER = "transfer"

# Data processing pipeline stages
class PipelineStage(Enum):
    RAW_DATA = "raw_data"
    PREPROCESSING = "preprocessing"
    FEATURE_EXTRACTION = "feature_extraction"
    DIMENSIONALITY_REDUCTION = "dimensionality_reduction"
    MODELING = "modeling"
    VALIDATION = "validation"
    OPTIMIZATION = "optimization"
    DEPLOYMENT = "deployment"

# Model evaluation metrics
class EvaluationMetric(Enum):
    MSE = "mse"
    RMSE = "rmse"
    MAE = "mae"
    R2 = "r2"
    ACCURACY = "accuracy"
    PRECISION = "precision"
    RECALL = "recall"
    F1 = "f1"
    ROC_AUC = "roc_auc"
    CUSTOM = "custom"

class MLData:
    """Machine learning data container"""
    
    def __init__(self, X: Optional[np.ndarray] = None, y: Optional[np.ndarray] = None,
                 feature_names: Optional[List[str]] = None, 
                 target_name: Optional[str] = None, 
                 metadata: Optional[Dict[str, Any]] = None):
        self.X = X
        self.y = y
        self.feature_names = feature_names or []
        self.target_name = target_name
        self.metadata = metadata or {}
        self.preprocessing_steps = []
        self.feature_importance = None
        self.scaler = None
        self.prepared = False
    
    def is_prepared(self) -> bool:
        """Check if data is prepared"""
        return self.prepared and self.X is not None and self.y is not None
    
    def shape(self) -> Tuple[int, int]:
        """Get data shape"""
        if self.X is not None:
            return (self.X.shape[0], self.X.shape[1])
        return (0, 0)
    
    def save(self, filepath: str):
        """Save data to file"""
        data = {
            'X': self.X.tolist() if self.X is not None else None,
            'y': self.y.tolist() if self.y is not None else None,
            'feature_names': self.feature_names,
            'target_name': self.target_name,
            'metadata': self.metadata,
            'preprocessing_steps': self.preprocessing_steps,
            'feature_importance': self.feature_importance.tolist() if self.feature_importance is not None else None,
        }
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    @classmethod
    def load(cls, filepath: str) -> 'MLData':
        """Load data from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        ml_data = cls(
            X=np.array(data['X']) if data['X'] is not None else None,
            y=np.array(data['y']) if data['y'] is not None else None,
            feature_names=data['feature_names'],
            target_name=data['target_name'],
            metadata=data['metadata']
        )
        ml_data.preprocessing_steps = data['preprocessing_steps']
        ml_data.feature_importance = np.array(data['feature_importance']) if data['feature_importance'] is not None else None
        ml_data.prepared = True
        
        return ml_data

class FeatureExtractor:
    """Feature extraction and engineering class"""
    
    def __init__(self):
        self.features_created = []
        self.transformers = {}
        self.feature_names = []
    
    def extract_time_series_features(self, X: np.ndarray, 
                                     window_sizes: List[int] = None,
                                     overlap: float = 0.5) -> np.ndarray:
        """Extract time series features from input data"""
        if window_sizes is None:
            window_sizes = [3, 5, 7]
        
        n_samples, n_features = X.shape
        extracted_features = []
        
        for window in window_sizes:
            stride = int(window * (1 - overlap))
            for i in range(0, n_samples - window + 1, stride):
                window_data = X[i:i + window]
                
                # Statistical features
                mean = np.mean(window_data, axis=0)
                std = np.std(window_data, axis=0)
                min_val = np.min(window_data, axis=0)
                max_val = np.max(window_data, axis=0)
                range_val = max_val - min_val
                median = np.median(window_data, axis=0)
                skewness = self._calculate_skewness(window_data, axis=0)
                kurtosis = self._calculate_kurtosis(window_data, axis=0)
                
                # Temporal features
                first_diff = np.diff(window_data, axis=0)
                second_diff = np.diff(first_diff, axis=0)
                mean_first_diff = np.mean(first_diff, axis=0)
                mean_second_diff = np.mean(second_diff, axis=0)
                
                # Frequency domain features (simplified)
                fft_features = self._calculate_fft_features(window_data, n_features)
                
                window_features = np.concatenate([
                    mean, std, min_val, max_val, range_val, median,
                    skewness, kurtosis, mean_first_diff, mean_second_diff,
                    fft_features.flatten()
                ])
                
                extracted_features.append(window_features)
        
        if extracted_features:
            return np.array(extracted_features)
        else:
            return np.array([])
    
    def _calculate_skewness(self, data: np.ndarray, axis: int) -> np.ndarray:
        """Calculate skewness of data"""
        mean = np.mean(data, axis=axis)
        std = np.std(data, axis=axis)
        if std == 0:
            return np.zeros(data.shape[1])
        
        skewness = np.mean(((data - mean) / std) ** 3, axis=axis)
        return skewness
    
    def _calculate_kurtosis(self, data: np.ndarray, axis: int) -> np.ndarray:
        """Calculate kurtosis of data"""
        mean = np.mean(data, axis=axis)
        std = np.std(data, axis=axis)
        if std == 0:
            return np.zeros(data.shape[1])
        
        kurtosis = np.mean(((data - mean) / std) ** 4, axis=axis) - 3
        return kurtosis
    
    def _calculate_fft_features(self, data: np.ndarray, n_features: int) -> np.ndarray:
        """Calculate FFT features"""
        fft_features = []
        for i in range(n_features):
            fft_result = np.fft.fft(data[:, i])
            magnitude_spectrum = np.abs(fft_result)
            phase_spectrum = np.angle(fft_result)
            
            # Extract key frequency components
            fft_features.extend(magnitude_spectrum[:10])
            fft_features.extend(phase_spectrum[:10])
        
        return np.array(fft_features)
    
    def extract_neural_features(self, brain: Any, features: List[str] = None) -> Dict[str, Any]:
        """Extract features from NLM brain state"""
        if not _NLM_AVAILABLE:
            raise ImportError("NLM framework not available for feature extraction")
        
        if features is None:
            features = ['neural_activity', 'spiking_rate', 'connectivity', 'plasticity']
        
        neural_features = {}
        
        if 'neural_activity' in features:
            neural_features['avg_activity'] = self._get_avg_neural_activity(brain)
            neural_features['activity_variance'] = self._get_activity_variance(brain)
            neural_features['active_neuron_ratio'] = self._get_active_neuron_ratio(brain)
        
        if 'spiking_rate' in features:
            neural_features['spiking_rate'] = self._get_spiking_rate(brain)
            neural_features['spike_intervals'] = self._get_spike_intervals(brain)
        
        if 'connectivity' in features:
            neural_features['connection_density'] = self._get_connection_density(brain)
            neural_features['avg_connection_strength'] = self._get_avg_connection_strength(brain)
        
        if 'plasticity' in features:
            neural_features['plasticity_level'] = self._get_plasticity_level(brain)
            neural_features['adaptation_rate'] = self._get_adaptation_rate(brain)
        
        return neural_features
    
    def create_feature_set(self, X: np.ndarray, y: np.ndarray, 
                          feature_types: List[str] = None) -> np.ndarray:
        """Create comprehensive feature set from raw data"""
        if feature_types is None:
            feature_types = ['statistical', 'temporal', 'frequency', 'neural']
        
        feature_sets = []
        
        if 'statistical' in feature_types:
            feature_sets.append(self._extract_statistical_features(X))
        
        if 'temporal' in feature_types:
            feature_sets.append(self._extract_temporal_features(X))
        
        if 'frequency' in feature_types:
            feature_sets.append(self._extract_frequency_features(X))
        
        if 'neural' in feature_types and _NLM_AVAILABLE:
            # Would need brain instance for neural features
            pass
        
        if feature_sets:
            return np.hstack(feature_sets)
        else:
            return X
    
    def _extract_statistical_features(self, X: np.ndarray) -> np.ndarray:
        """Extract statistical features"""
        features = []
        
        # Basic statistics
        features.append(np.mean(X, axis=0))
        features.append(np.std(X, axis=0))
        features.append(np.min(X, axis=0))
        features.append(np.max(X, axis=0))
        features.append(np.median(X, axis=0))
        
        # Higher moments
        features.append(self._calculate_skewness(X, axis=0))
        features.append(self._calculate_kurtosis(X, axis=0))
        
        return np.hstack(features)
    
    def _extract_temporal_features(self, X: np.ndarray) -> np.ndarray:
        """Extract temporal features"""
        n_samples, n_features = X.shape
        
        # First differences
        first_diff = np.diff(X, axis=0)
        features = [np.mean(first_diff, axis=0), np.std(first_diff, axis=0)]
        
        # Second differences
        second_diff = np.diff(first_diff, axis=0)
        features.extend([np.mean(second_diff, axis=0), np.std(second_diff, axis=0)])
        
        # Lag features
        lag_features = []
        for lag in [1, 2, 3]:
            if n_samples > lag:
                lag_features.append(X[lag:] - X[:-lag])
        
        if lag_features:
            lag_matrix = np.vstack(lag_features)
            features.extend([np.mean(lag_matrix, axis=0), np.std(lag_matrix, axis=0)])
        
        return np.hstack(features)
    
    def _extract_frequency_features(self, X: np.ndarray) -> np.ndarray:
        """Extract frequency domain features"""
        n_samples, n_features = X.shape
        
        fft_features = []
        for i in range(n_features):
            fft_result = np.fft.fft(X[:, i])
            magnitude_spectrum = np.abs(fft_result)
            phase_spectrum = np.angle(fft_result)
            
            # Extract key frequency components
            fft_features.extend(magnitude_spectrum[:10])
            fft_features.extend(phase_spectrum[:10])
        
        return np.array(fft_features)
    
    def _get_avg_neural_activity(self, brain: Any) -> float:
        """Get average neural activity from brain"""
        if hasattr(brain, 'getActiveNeuronCount'):
            active = brain.getActiveNeuronCount()
        else:
            active = 0
        
        if hasattr(brain, 'getTotalNeuronCount'):
            total = brain.getTotalNeuronCount()
            return active / total if total > 0 else 0.0
        return 0.0
    
    def _get_activity_variance(self, brain: Any) -> float:
        """Get activity variance from brain"""
        # Simplified implementation
        return 0.25
    
    def _get_active_neuron_ratio(self, brain: Any) -> float:
        """Get ratio of active neurons"""
        return self._get_avg_neural_activity(brain)
    
    def _get_spiking_rate(self, brain: Any) -> float:
        """Get overall spiking rate"""
        if hasattr(brain, 'getTotalSpikeCount'):
            spikes = brain.getTotalSpikeCount()
            if hasattr(brain, 'getTotalSimulationTime'):
                time = brain.getTotalSimulationTime()
                return spikes / time if time > 0 else 0.0
        return 0.0
    
    def _get_spike_intervals(self, brain: Any) -> List[float]:
        """Get spike intervals"""
        # Simplified implementation
        return [0.1, 0.2, 0.15, 0.25]
    
    def _get_connection_density(self, brain: Any) -> float:
        """Get connection density"""
        if hasattr(brain, 'getTotalSynapseCount'):
            synapses = brain.getTotalSynapseCount()
            if hasattr(brain, 'getTotalNeuronCount'):
                neurons = brain.getTotalNeuronCount()
                return synapses / (neurons * neurons) if neurons > 0 else 0.0
        return 0.0
    
    def _get_avg_connection_strength(self, brain: Any) -> float:
        """Get average connection strength"""
        # Simplified implementation
        return 0.5
    
    def _get_plasticity_level(self, brain: Any) -> float:
        """Get plasticity level"""
        # Simplified implementation
        return 0.3
    
    def _get_adaptation_rate(self, brain: Any) -> float:
        """Get adaptation rate"""
        # Simplified implementation
        return 0.1

class DataPreprocessor:
    """Data preprocessing and normalization class"""
    
    def __init__(self):
        self.preprocessing_steps = []
        self.scalers = {}
    
    def normalize(self, X: np.ndarray, method: str = 'standard') -> np.ndarray:
        """Normalize input data"""
        if method == 'standard':
            scaler = StandardScaler()
            X_normalized = scaler.fit_transform(X)
            self.scalers['standard'] = scaler
        elif method == 'minmax':
            scaler = MinMaxScaler()
            X_normalized = scaler.fit_transform(X)
            self.scalers['minmax'] = scaler
        elif method == 'normalize':
            scaler = Normalizer()
            X_normalized = scaler.fit_transform(X)
            self.scalers['normalize'] = scaler
        else:
            raise ValueError(f"Unknown normalization method: {method}")
        
        self.preprocessing_steps.append(f'normalized_{method}')
        return X_normalized
    
    def handle_missing_values(self, X: np.ndarray, strategy: str = 'mean') -> np.ndarray:
        """Handle missing values in data"""
        if strategy == 'mean':
            col_means = np.nanmean(X, axis=0)
            nan_cols = np.isnan(X)
            X_filled = X.copy()
            for i in range(X.shape[1]):
                X_filled[nan_cols[:, i], i] = col_means[i]
        elif strategy == 'median':
            col_medians = np.nanmedian(X, axis=0)
            nan_cols = np.isnan(X)
            X_filled = X.copy()
            for i in range(X.shape[1]):
                X_filled[nan_cols[:, i], i] = col_medians[i]
        elif strategy == 'forward_fill':
            X_filled = np.nan_to_num(X, nan='forward')
        else:
            raise ValueError(f"Unknown missing value strategy: {strategy}")
        
        self.preprocessing_steps.append(f'missing_values_{strategy}')
        return X_filled
    
    def scale_features(self, X: np.ndarray, feature_indices: List[int] = None) -> np.ndarray:
        """Scale specific features"""
        if feature_indices is None:
            feature_indices = list(range(X.shape[1]))
        
        X_scaled = X.copy()
        for i in feature_indices:
            if i < X.shape[1]:
                col = X[:, i]
                if np.std(col) > 0:
                    X_scaled[:, i] = (col - np.mean(col)) / np.std(col)
        
        self.preprocessing_steps.append(f'scaled_features_{feature_indices}')
        return X_scaled
    
    def create_train_test_split(self, X: np.ndarray, y: np.ndarray, 
                                test_size: float = 0.2, random_state: int = 42,
                                stratify: bool = False) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
        """Create train-test split"""
        if stratify and y is not None:
            # Create stratification labels for classification
            if len(np.unique(y)) < 10:  # Discrete classes
                stratify_labels = y
            else:
                # For continuous targets, create bins
                y_bins = np.digitize(y, np.percentile(y, [10, 25, 50, 75, 90]))
                stratify_labels = y_bins
        else:
            stratify_labels = None
        
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=test_size, random_state=random_state, stratify=stratify_labels
        )
        
        self.preprocessing_steps.append(f'train_test_split_{test_size}')
        return X_train, X_test, y_train, y_test
    
    def apply_feature_selection(self, X: np.ndarray, feature_selector: Any,
                               fit: bool = True) -> np.ndarray:
        """Apply feature selection"""
        if fit:
            X_selected = feature_selector.fit_transform(X)
        else:
            X_selected = feature_selector.transform(X)
        
        self.preprocessing_steps.append(f'feature_selection_{feature_selector.__class__.__name__}')
        return X_selected

class ModelTrainer:
    """Machine learning model training class"""
    
    def __init__(self):
        self.models = {}
        self.best_model = None
        self.best_score = 0.0
    
    def train_model(self, X_train: np.ndarray, y_train: np.ndarray,
                   model_type: str = 'random_forest', 
                   hyperparameters: Dict[str, Any] = None,
                   cv: int = 5) -> Any:
        """Train a machine learning model"""
        if hyperparameters is None:
            hyperparameters = self._get_default_hyperparameters(model_type)
        
        if model_type == 'random_forest':
            model = RandomForestRegressor(**hyperparameters)
        elif model_type == 'linear_regression':
            model = LinearRegression(**hyperparameters)
        elif model_type == 'ridge':
            model = Ridge(**hyperparameters)
        elif model_type == 'neural_network':
            model = MLPRegressor(**hyperparameters)
        elif model_type == 'svm':
            model = SVR(**hyperparameters)
        elif model_type == 'gaussian_process':
            model = GaussianProcessRegressor(**hyperparameters)
        else:
            raise ValueError(f"Unknown model type: {model_type}")
        
        # Train model with cross-validation
        model.fit(X_train, y_train)
        
        # Evaluate model
        cv_scores = cross_val_score(model, X_train, y_train, cv=cv, scoring='r2')
        avg_score = np.mean(cv_scores)
        
        # Store model
        self.models[model_type] = {
            'model': model,
            'cv_scores': cv_scores,
            'avg_score': avg_score,
            'hyperparameters': hyperparameters
        }
        
        # Update best model
        if avg_score > self.best_score:
            self.best_score = avg_score
            self.best_model = {
                'model': model,
                'type': model_type,
                'score': avg_score,
                'hyperparameters': hyperparameters
            }
        
        return model
    
    def hyperparameter_optimization(self, X_train: np.ndarray, y_train: np.ndarray,
                                    model_type: str, param_grid: Dict[str, Any],
                                    cv: int = 5) -> Any:
        """Perform hyperparameter optimization using GridSearchCV"""
        if model_type == 'random_forest':
            model = RandomForestRegressor()
        elif model_type == 'linear_regression':
            model = LinearRegression()
        elif model_type == 'ridge':
            model = Ridge()
        elif model_type == 'neural_network':
            model = MLPRegressor()
        elif model_type == 'svm':
            model = SVR()
        elif model_type == 'gaussian_process':
            model = GaussianProcessRegressor()
        else:
            raise ValueError(f"Unknown model type: {model_type}")
        
        # Perform grid search
        grid_search = GridSearchCV(
            estimator=model,
            param_grid=param_grid,
            cv=cv,
            scoring='r2',
            n_jobs=-1
        )
        
        grid_search.fit(X_train, y_train)
        
        # Store optimized model
        self.models[model_type] = {
            'model': grid_search.best_estimator_,
            'cv_scores': grid_search.cv_results_['mean_test_score'],
            'best_params': grid_search.best_params_,
            'avg_score': np.mean(grid_search.cv_results_['mean_test_score'])
        }
        
        # Update best model if better
        if grid_search.best_score > self.best_score:
            self.best_score = grid_search.best_score
            self.best_model = {
                'model': grid_search.best_estimator_,
                'type': model_type,
                'score': grid_search.best_score,
                'best_params': grid_search.best_params_
            }
        
        return grid_search.best_estimator_
    
    def train_multiple_models(self, X_train: np.ndarray, y_train: np.ndarray,
                             model_types: List[str] = None,
                             hyperparameters: Dict[str, Any] = None,
                             cv: int = 5) -> Dict[str, Any]:
        """Train multiple models and return results"""
        if model_types is None:
            model_types = ['random_forest', 'linear_regression', 'ridge']
        
        results = {}
        for model_type in model_types:
            print(f"Training {model_type}...")
            model = self.train_model(X_train, y_train, model_type, hyperparameters, cv)
            results[model_type] = self.models[model_type]
        
        return results
    
    def _get_default_hyperparameters(self, model_type: str) -> Dict[str, Any]:
        """Get default hyperparameters for model type"""
        defaults = {
            'random_forest': {'n_estimators': 100, 'max_depth': None, 'random_state': 42},
            'linear_regression': {},
            'ridge': {'alpha': 1.0},
            'neural_network': {'hidden_layer_sizes': (100,), 'max_iter': 500, 'random_state': 42},
            'svm': {'C': 1.0, 'kernel': 'rbf', 'gamma': 'scale'},
            'gaussian_process': {}
        }
        return defaults.get(model_type, {})
    
    def evaluate_model(self, model: Any, X_test: np.ndarray, y_test: np.ndarray) -> Dict[str, float]:
        """Evaluate model performance"""
        y_pred = model.predict(X_test)
        
        # Calculate metrics
        metrics = {
            'mse': mean_squared_error(y_test, y_pred),
            'rmse': np.sqrt(mean_squared_error(y_test, y_pred)),
            'mae': mean_absolute_error(y_test, y_pred),
            'r2': r2_score(y_test, y_pred),
        }
        
        return metrics
    
    def predict(self, model: Any, X: np.ndarray) -> np.ndarray:
        """Make predictions using trained model"""
        return model.predict(X)

class ModelEvaluator:
    """Model evaluation and validation class"""
    
    def __init__(self):
        self.evaluation_results = {}
    
    def evaluate_models(self, models: Dict[str, Any], X_test: np.ndarray, 
                       y_test: np.ndarray) -> Dict[str, Dict[str, float]]:
        """Evaluate multiple models"""
        results = {}
        for model_name, model_info in models.items():
            model = model_info['model']
            metrics = self.evaluate_model(model, X_test, y_test)
            results[model_name] = metrics
            self.evaluation_results[model_name] = {
                'metrics': metrics,
                'cross_val_scores': model_info.get('cv_scores', []),
                'average_score': model_info.get('avg_score', 0.0)
            }
        
        return results
    
    def evaluate_model(self, model: Any, X_test: np.ndarray, y_test: np.ndarray) -> Dict[str, float]:
        """Evaluate single model"""
        y_pred = model.predict(X_test)
        
        # Calculate regression metrics
        metrics = {
            'mse': mean_squared_error(y_test, y_pred),
            'rmse': np.sqrt(mean_squared_error(y_test, y_pred)),
            'mae': mean_absolute_error(y_test, y_pred),
            'r2': r2_score(y_test, y_pred),
        }
        
        return metrics
    
    def cross_validate(self, model: Any, X: np.ndarray, y: np.ndarray, 
                       cv: int = 5, scoring: str = 'r2') -> Dict[str, Any]:
        """Cross-validate model"""
        cv_scores = cross_val_score(model, X, y, cv=cv, scoring=scoring)
        
        results = {
            'scores': cv_scores,
            'mean_score': np.mean(cv_scores),
            'std_score': np.std(cv_scores),
            'n_splits': cv
        }
        
        return results
    
    def compare_models(self, model_results: Dict[str, Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple models and return ranking"""
        comparison = {}
        
        for model_name, result in model_results.items():
            comparison[model_name] = {
                'r2_score': result.get('metrics', {}).get('r2', 0.0),
                'cv_mean': result.get('average_score', 0.0),
                'cv_std': np.std(result.get('cross_val_scores', [])),
                'rank': 0  # Will be determined later
            }
        
        # Rank models by R2 score
        ranked_models = sorted(comparison.items(), 
                              key=lambda x: x[1]['r2_score'], reverse=True)
        
        for i, (model_name, _) in enumerate(ranked_models):
            comparison[model_name]['rank'] = i + 1
        
        return comparison
    
    def generate_report(self, model_results: Dict[str, Dict[str, Any]]) -> str:
        """Generate evaluation report"""
        report = "Model Evaluation Report\n"
        report += "=" * 50 + "\n\n"
        
        comparison = self.compare_models(model_results)
        
        report += f"{'Model':<30} {'R2 Score':<15} {'CV Mean':<12} {'Rank':<8}\n"
        report += "-" * 65 + "\n"
        
        for model_name, metrics in comparison.items():
            report += f"{model_name:<30} {metrics['r2_score']:<15.4f} {metrics['cv_mean']:<12.4f} {metrics['rank']:<8}\n"
        
        report += "\n\nBest Model: " + max(comparison.keys(), key=lambda x: comparison[x]['r2_score'])
        
        return report

class MLPipeline:
    """Complete machine learning pipeline"""
    
    def __init__(self):
        self.feature_extractor = FeatureExtractor()
        self.data_preprocessor = DataPreprocessor()
        self.model_trainer = ModelTrainer()
        self.model_evaluator = ModelEvaluator()
        
        self.stage = PipelineStage.RAW_DATA
        self.is_fitted = False
    
    def fit(self, X: np.ndarray, y: np.ndarray, **kwargs) -> 'MLPipeline':
        """Fit the complete machine learning pipeline"""
        self.stage = PipelineStage.PREPROCESSING
        
        # Step 1: Feature extraction
        X_features = self.feature_extractor.create_feature_set(X, y)
        
        # Step 2: Data preprocessing
        X_normalized = self.data_preprocessor.normalize(X_features, method='standard')
        
        # Step 3: Train-test split
        X_train, X_test, y_train, y_test = self.data_preprocessor.create_train_test_split(
            X_normalized, y, **kwargs
        )
        
        # Step 4: Model training
        self.stage = PipelineStage.MODELING
        model = self.model_trainer.train_model(X_train, y_train, **kwargs)
        
        # Step 5: Model evaluation
        self.stage = PipelineStage.VALIDATION
        metrics = self.model_evaluator.evaluate_model(model, X_test, y_test)
        cv_results = self.model_evaluator.cross_validate(model, X_train, y_train)
        
        # Store results
        self.model = model
        self.X_test = X_test
        self.y_test = y_test
        self.metrics = metrics
        self.cv_results = cv_results
        self.is_fitted = True
        
        return self
    
    def predict(self, X: np.ndarray) -> np.ndarray:
        """Make predictions using fitted model"""
        if not self.is_fitted:
            raise RuntimeError("Pipeline not fitted. Call fit() first.")
        
        # Preprocess input data (same steps as during training)
        X_features = self.feature_extractor.create_feature_set(X, np.zeros(X.shape[0]))
        X_normalized = self.data_preprocessor.normalize(X_features, method='standard')
        
        # Make predictions
        return self.model.predict(X_normalized)
    
    def evaluate(self, X_test: np.ndarray, y_test: np.ndarray) -> Dict[str, float]:
        """Evaluate model on test data"""
        if not self.is_fitted:
            raise RuntimeError("Pipeline not fitted. Call fit() first.")
        
        return self.model_evaluator.evaluate_model(self.model, X_test, y_test)
    
    def get_feature_importance(self) -> Optional[np.ndarray]:
        """Get feature importance from trained model"""
        if hasattr(self.model, 'feature_importances_'):
            return self.model.feature_importances_
        elif hasattr(self.model, 'coef_'):
            return np.abs(self.model.coef_)
        return None
    
    def save(self, filepath: str):
        """Save pipeline to file"""
        pipeline_data = {
            'stage': self.stage.value,
            'is_fitted': self.is_fitted,
            'feature_extractor': self.feature_extractor.__dict__,
            'data_preprocessor': self.data_preprocessor.__dict__,
            'model_trainer': self.model_trainer.models,
            'model_evaluator': self.model_evaluator.evaluation_results,
            'metrics': self.metrics,
            'cv_results': self.cv_results,
        }
        
        with open(filepath, 'w') as f:
            json.dump(pipeline_data, f, indent=2)
    
    @classmethod
    def load(cls, filepath: str) -> 'MLPipeline':
        """Load pipeline from file"""
        with open(filepath, 'r') as f:
            pipeline_data = json.load(f)
        
        pipeline = cls()
        pipeline.stage = PipelineStage(pipeline_data['stage'])
        pipeline.is_fitted = pipeline_data['is_fitted']
        
        # Note: This is a simplified loading - in practice, you'd need to 
        # properly serialize the objects
        return pipeline

class NeuromodulationML:
    """Machine learning integration with neuromodulation systems"""
    
    def __init__(self):
        self.ml_pipeline = MLPipeline()
        self.neuromodulation_data = []
        self.brain_state_data = []
    
    def integrate_with_brain(self, brain: Any, features: List[str] = None):
        """Integrate machine learning with NLM brain"""
        if features is None:
            features = ['neural_activity', 'spiking_rate', 'connectivity', 'plasticity']
        
        # Extract features from brain state
        feature_extractor = FeatureExtractor()
        neural_features = feature_extractor.extract_neural_features(brain, features)
        
        # Store data
        self.brain_state_data.append({
            'brain_state': brain,
            'neural_features': neural_features,
            'timestamp': np.time.time()
        })
        
        # Return features for ML training
        return neural_features
    
    def train_neuromodulation_model(self, X: np.ndarray, y: np.ndarray, **kwargs):
        """Train a neuromodulation model"""
        # Create and fit ML pipeline
        pipeline = MLPipeline()
        pipeline.fit(X, y, **kwargs)
        
        self.ml_pipeline = pipeline
        return pipeline
    
    def predict_neuromodulation(self, brain: Any) -> float:
        """Predict neuromodulation effect for brain state"""
        # Extract features from brain
        feature_extractor = FeatureExtractor()
        neural_features = feature_extractor.extract_neural_features(brain)
        
        # Convert to feature vector
        X = np.array(list(neural_features.values())).reshape(1, -1)
        
        # Make prediction
        if hasattr(self.ml_pipeline, 'predict'):
            prediction = self.ml_pipeline.predict(X)[0]
            return prediction
        
        return 0.0
    
    def analyze_neuromodulation_patterns(self) -> Dict[str, Any]:
        """Analyze neuromodulation patterns"""
        if not self.brain_state_data:
            return {}
        
        # Analyze patterns in stored data
        analysis = {
            'data_points': len(self.brain_state_data),
            'feature_stats': {},
            'patterns': [],
            'trends': []
        }
        
        # Calculate feature statistics
        for feature_name in self.brain_state_data[0]['neural_features'].keys():
            values = [data['neural_features'][feature_name] 
                     for data in self.brain_state_data]
            analysis['feature_stats'][feature_name] = {
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values)
            }
        
        return analysis

# Convenience functions

def create_ml_pipeline() -> MLPipeline:
    """Create a new ML pipeline"""
    return MLPipeline()

def train_model(X: np.ndarray, y: np.ndarray, **kwargs) -> MLPipeline:
    """Train a model using ML pipeline"""
    pipeline = MLPipeline()
    return pipeline.fit(X, y, **kwargs)

def evaluate_trained_model(pipeline: MLPipeline, X_test: np.ndarray, 
                           y_test: np.ndarray) -> Dict[str, float]:
    """Evaluate a trained model"""
    return pipeline.evaluate(X_test, y_test)

# Example usage function
if __name__ == "__main__":
    print("NLM Machine Learning Pipeline Integration")
    print("=====================================")
    print()
    print("This module provides comprehensive machine learning")
    print("pipeline integration for the NLM framework.")
    print()
    print("Key Components:")
    print("1. MLData - Data container with preprocessing support")
    print("2. FeatureExtractor - Extract features from NLM brain")
    print("3. DataPreprocessor - Data cleaning and normalization")
    print("4. ModelTrainer - Train various ML models")
    print("5. ModelEvaluator - Model evaluation and validation")
    print("6. MLPipeline - Complete end-to-end pipeline")
    print("7. NeuromodulationML - Integration with neuromodulation")
    print()
    print("Usage Examples:")
    print()
    print("# Create pipeline")
    print("from nlm_ml import create_ml_pipeline")
    print("pipeline = create_ml_pipeline()")
    print()
    print("# Train model")
    print("from sklearn.datasets import make_regression")
    print("X, y = make_regression(n_samples=100, n_features=4, random_state=42)")
    print("pipeline.fit(X, y, model_type='random_forest', test_size=0.2)")
    print()
    print("# Make predictions")
    print("predictions = pipeline.predict(X_test)")
    print()
    print("# Evaluate model")
    print("metrics = pipeline.evaluate(X_test, y_test)")
    print("print(f\"R2 Score: {metrics['r2']:.4f}\")")
    print()
    print("# Use with NLM brain")
    print("from nlm_ml import NeuromodulationML")
    print("nml_ml = NeuromodulationML()")
    print("features = nml_ml.integrate_with_brain(brain)")
    print("X = np.array(list(features.values())).reshape(-1, 1)")
    print("y = brain.get_target_values()  # Custom target extraction")
    print("model = nml_ml.train_neuromodulation_model(X, y)")
    print()
    print("Features:")
    print("- Feature extraction from NLM brain states")
    print("- Multiple model types (Regression, Classification, Neural Networks)")
    print("- Hyperparameter optimization")
    print("- Model evaluation and validation")
    print("- Neuromodulation integration")
    print("- Complete end-to-end pipeline")
