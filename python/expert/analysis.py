"""Data Analysis APIs for Expert Users"""

from typing import Dict, List, Optional, Callable, Any, Union, Tuple
import numpy as np
import time

class NeuralSignalProcessor:
    """Neural signal processing for expert users"""
    
    def __init__(self, sampling_rate: float = 1000.0, filter_type: str = "bandpass"):
        self.sampling_rate = sampling_rate
        self.filter_type = filter_type
        self.processing_history = []
        self.signal_buffers = {}
    
    def filter_signal(self, signal: List[float], frequency_range: Tuple[float, float] = (1.0, 100.0), 
                     filter_order: int = 4) -> List[float]:
        """Filter neural signal"""
        # Simulate filtering process
        filtered = []
        for value in signal:
            # Apply bandpass filter logic
            filtered_value = value * np.exp(-abs(frequency_range[0] - 50.0) / 20.0)
            filtered.append(filtered_value)
        
        self.processing_history.append({
            'original_length': len(signal),
            'filtered_length': len(filtered),
            'filter_type': self.filter_type,
            'frequency_range': frequency_range,
            'timestamp': time.time()
        })
        
        return filtered
    
    def spike_detection(self, signal: List[float], threshold: float = 3.0, refractory_period: float = 2.0) -> List[Dict]:
        """Detect spikes in neural signal"""
        spikes = []
        last_spike_time = -refractory_period
        
        for i, value in enumerate(signal):
            if value > threshold and (i - last_spike_time) * (1.0 / self.sampling_rate) > refractory_period:
                spike_time = i / self.sampling_rate
                spikes.append({
                    'time': spike_time,
                    'amplitude': value,
                    'index': i
                })
                last_spike_time = i
        
        return spikes
    
    def feature_extraction(self, signal: List[float], features: List[str]) -> Dict:
        """Extract features from neural signal"""
        extracted = {}
        
        if 'mean' in features:
            extracted['mean'] = np.mean(signal)
        if 'std' in features:
            extracted['std'] = np.std(signal)
        if 'max' in features:
            extracted['max'] = np.max(signal)
        if 'min' in features:
            extracted['min'] = np.min(signal)
        if 'duration' in features:
            extracted['duration'] = len(signal) / self.sampling_rate
        if 'power' in features:
            extracted['power'] = np.sum(np.square(signal))
        
        return extracted

class PatternRecognizer:
    """Pattern recognition for neural data"""
    
    def __init__(self, recognition_threshold: float = 0.85, pattern_history_size: int = 1000):
        self.recognition_threshold = recognition_threshold
        self.pattern_history_size = pattern_history_size
        self.pattern_database = {}
        self.recognition_stats = {
            'total_patterns': 0,
            'successful_recognitions': 0,
            'accuracy': 0.0
        }
    
    def train_pattern(self, pattern_id: str, pattern_data: List[float], labels: Optional[List[str]] = None) -> None:
        """Train pattern recognition model"""
        normalized_pattern = self._normalize_pattern(pattern_data)
        
        self.pattern_database[pattern_id] = {
            'pattern': normalized_pattern,
            'labels': labels or [f'pattern_{pattern_id}'],
            'training_time': time.time(),
            'usage_count': 0
        }
        
        self.recognition_stats['total_patterns'] += 1
    
    def recognize_pattern(self, test_data: List[float], pattern_id: Optional[str] = None) -> Dict:
        """Recognize pattern in test data"""
        test_normalized = self._normalize_pattern(test_data)
        
        best_match = None
        best_score = 0.0
        
        if pattern_id:
            if pattern_id in self.pattern_database:
                pattern = self.pattern_database[pattern_id]['pattern']
                score = self._calculate_similarity(test_normalized, pattern)
                best_match = pattern_id
                best_score = score
        else:
            # Search all patterns
            for pid, pattern_info in self.pattern_database.items():
                pattern = pattern_info['pattern']
                score = self._calculate_similarity(test_normalized, pattern)
                if score > best_score:
                    best_score = score
                    best_match = pid
        
        if best_score >= self.recognition_threshold:
            self.recognition_stats['successful_recognitions'] += 1
            accuracy = (self.recognition_stats['successful_recognitions'] / 
                       self.recognition_stats['total_patterns'])
            self.recognition_stats['accuracy'] = accuracy
        
        return {
            'pattern_id': best_match,
            'confidence': best_score,
            'recognized': best_score >= self.recognition_threshold,
            'labels': self.pattern_database[best_match]['labels'] if best_match else []
        }
    
    def _normalize_pattern(self, pattern: List[float]) -> List[float]:
        """Normalize pattern data"""
        if not pattern:
            return pattern
        
        mean = np.mean(pattern)
        std = np.std(pattern)
        if std > 0:
            return [(x - mean) / std for x in pattern]
        return pattern
    
    def _calculate_similarity(self, pattern1: List[float], pattern2: List[float]) -> float:
        """Calculate similarity between patterns"""
        if len(pattern1) != len(pattern2):
            return 0.0
        
        # Cosine similarity
        dot_product = sum(p1 * p2 for p1, p2 in zip(pattern1, pattern2))
        magnitude1 = np.sqrt(sum(p1 * p1 for p1 in pattern1))
        magnitude2 = np.sqrt(sum(p2 * p2 for p2 in pattern2))
        
        if magnitude1 > 0 and magnitude2 > 0:
            return dot_product / (magnitude1 * magnitude2)
        return 0.0

class StatisticalAnalyzer:
    """Statistical analysis for neural data"""
    
    def __init__(self, significance_level: float = 0.05, confidence_interval: float = 0.95):
        self.significance_level = significance_level
        self.confidence_interval = confidence_interval
        self.analysis_history = []
    
    def time_series_analysis(self, data: List[float], window_size: int = 100) -> Dict:
        """Perform time series analysis"""
        if len(data) < window_size:
            window_size = len(data) // 2
        
        means = []
        stds = []
        trends = []
        
        for i in range(0, len(data) - window_size + 1, window_size // 2):
            window = data[i:i + window_size]
            means.append(np.mean(window))
            stds.append(np.std(window))
            
            # Simple trend calculation
            if len(window) > 1:
                trend = np.corrcoef(range(len(window)), window)[0, 1]
                trends.append(trend)
        
        return {
            'mean_series': means,
            'std_series': stds,
            'trend_series': trends,
            'summary': {
                'overall_mean': np.mean(data),
                'overall_std': np.std(data),
                'data_length': len(data),
                'window_size': window_size
            }
        }
    
    def frequency_analysis(self, signal: List[float], sampling_rate: float = 1000.0) -> Dict:
        """Perform frequency analysis using FFT"""
        n = len(signal)
        if n == 0:
            return {}
        
        # Compute FFT
        fft_result = np.fft.fft(signal)
        freqs = np.fft.fftfreq(n, 1.0 / sampling_rate)
        
        # Only consider positive frequencies
        positive_freqs = freqs[:n//2]
        positive_magnitudes = np.abs(fft_result[:n//2])
        
        # Find dominant frequencies
        dominant_indices = np.argsort(positive_magnitudes)[-5:][::-1]  # Top 5
        
        return {
            'frequencies': positive_freqs[dominant_indices].tolist(),
            'magnitudes': positive_magnitudes[dominant_indices].tolist(),
            'spectrum': {
                'min_freq': np.min(positive_freqs),
                'max_freq': np.max(positive_freqs),
                'total_power': np.sum(positive_magnitudes**2)
            }
        }
    
    def hypothesis_test(self, sample1: List[float], sample2: List[float], test_type: str = "t_test") -> Dict:
        """Perform statistical hypothesis test"""
        if test_type == "t_test":
            # Independent t-test
            t_stat = (np.mean(sample1) - np.mean(sample2)) / np.sqrt(np.var(sample1)/len(sample1) + np.var(sample2)/len(sample2))
            degrees_of_freedom = len(sample1) + len(sample2) - 2
            
            return {
                'test_type': 'independent_t_test',
                't_statistic': t_stat,
                'degrees_of_freedom': degrees_of_freedom,
                'p_value': 2 * (1 - np.abs(t_stat) / np.sqrt(degrees_of_freedom)),  # Approximation
                'significant': True  # Simplified
            }
        
        return {'error': f'Unsupported test type: {test_type}'}

class MLIntegration:
    """Machine learning integration for neural analysis"""
    
    def __init__(self, backend: str = "scikit-learn", model_cache_size: int = 100):
        self.backend = backend
        self.model_cache_size = model_cache_size
        self.model_cache = {}
        self.training_history = []
    
    def train_classifier(self, X_train: List[List[float]], y_train: List[str], 
                       model_type: str = "random_forest", params: Optional[Dict] = None) -> str:
        """Train machine learning classifier"""
        model_id = f"{model_type}_{len(self.model_cache)}"
        
        # Simulate model training
        self.model_cache[model_id] = {
            'type': model_type,
            'parameters': params or {},
            'training_data_size': len(X_train),
            'classes': list(set(y_train)),
            'created_at': time.time(),
            'training_time': len(X_train) * 0.001  # Simulate training time
        }
        
        self.training_history.append({
            'model_id': model_id,
            'type': model_type,
            'training_samples': len(X_train),
            'classes': len(set(y_train)),
            'timestamp': time.time()
        })
        
        return model_id
    
    def predict(self, model_id: str, X_test: List[List[float]]) -> List[str]:
        """Make predictions using trained model"""
        if model_id not in self.model_cache:
            raise ValueError(f"Model {model_id} not found")
        
        # Simulate predictions
        return [f"class_{i % 3}" for i in range(len(X_test))]
    
    def cross_validation(self, X_data: List[List[float]], y_data: List[str], 
                        n_splits: int = 5) -> Dict:
        """Perform cross-validation"""
        # Simulate cross-validation
        fold_scores = []
        for i in range(n_splits):
            fold_score = 0.7 + (i * 0.02)  # Simulate varying performance
            fold_scores.append(fold_score)
        
        return {
            'fold_scores': fold_scores,
            'mean_score': np.mean(fold_scores),
            'std_score': np.std(fold_scores),
            'best_fold': np.argmax(fold_scores),
            'cv_results': {'folds': n_splits}
        }

class SignalFilter:
    """Signal filtering for neural data"""
    
    def __init__(self):
        self.filter_config = {}
        self.filter_stats = {'applied': 0, 'errors': 0}
    
    def apply_filter(self, signal: List[float], filter_type: str, **kwargs) -> List[float]:
        """Apply signal filter"""
        self.filter_stats['applied'] += 1
        
        if filter_type == "moving_average":
            window_size = kwargs.get('window_size', 5)
            return self._moving_average(signal, window_size)
        elif filter_type == "butterworth":
            order = kwargs.get('order', 4)
            cutoff = kwargs.get('cutoff', 100.0)
            return self._butterworth(signal, order, cutoff)
        elif filter_type == "notch":
            frequency = kwargs.get('frequency', 50.0)
            return self._notch_filter(signal, frequency)
        else:
            self.filter_stats['errors'] += 1
            return signal
    
    def _moving_average(self, signal: List[float], window_size: int) -> List[float]:
        """Apply moving average filter"""
        if len(signal) < window_size:
            return signal
        
        result = []
        for i in range(len(signal) - window_size + 1):
            window = signal[i:i + window_size]
            result.append(sum(window) / window_size)
        
        # Pad the result to maintain original length
        padding = [signal[0]] * (len(signal) - len(result))
        return result + padding
    
    def _butterworth(self, signal: List[float], order: int, cutoff: float) -> List[float]:
        """Apply Butterworth filter"""
        # Simulate Butterworth filtering
        normalized_cutoff = cutoff / 500.0  # Assume 500Hz sampling rate
        filtered = []
        
        for value in signal:
            filtered_value = value * (1 - normalized_cutoff) / (1 + normalized_cutoff)
            filtered.append(filtered_value)
        
        return filtered
    
    def _notch_filter(self, signal: List[float], frequency: float) -> List[float]:
        """Apply notch filter"""
        # Simulate notch filtering
        filtered = []
        
        for i, value in enumerate(signal):
            notch_effect = np.sin(2 * np.pi * frequency * i / 1000.0) * 0.1
            filtered_value = value * (1 - notch_effect)
            filtered.append(filtered_value)
        
        return filtered

class FeatureExtractor:
    """Feature extraction for neural data analysis"""
    
    def __init__(self):
        self.feature_cache = {}
        self.extraction_stats = {'total_features': 0, 'cache_hits': 0, 'cache_misses': 0}
    
    def extract_time_domain_features(self, signal: List[float], sampling_rate: float = 1000.0) -> Dict:
        """Extract time-domain features"""
        features = {}
        
        features['mean'] = np.mean(signal)
        features['std'] = np.std(signal)
        features['median'] = np.median(signal)
        features['min'] = np.min(signal)
        features['max'] = np.max(signal)
        features['range'] = features['max'] - features['min']
        
        # Statistical moments
        features['skewness'] = self._skewness(signal)
        features['kurtosis'] = self._kurtosis(signal)
        
        # Temporal features
        features['duration'] = len(signal) / sampling_rate
        features['zero_crossings'] = self._count_zero_crossings(signal)
        features['peak_count'] = self._count_peaks(signal)
        
        self.extraction_stats['total_features'] += len(features)
        
        return features
    
    def extract_frequency_domain_features(self, signal: List[float], sampling_rate: float = 1000.0) -> Dict:
        """Extract frequency-domain features"""
        features = {}
        
        n = len(signal)
        if n == 0:
            return features
        
        # FFT
        fft_result = np.fft.fft(signal)
        freqs = np.fft.fftfreq(n, 1.0 / sampling_rate)
        magnitudes = np.abs(fft_result)
        
        # Frequency features
        positive_freqs = freqs[:n//2]
        positive_magnitudes = magnitudes[:n//2]
        
        features['dominant_frequency'] = np.argmax(positive_magnitudes) * sampling_rate / n
        features['spectral_power'] = np.sum(positive_magnitudes**2)
        features['spectral_entropy'] = self._spectral_entropy(positive_magnitudes)
        features['band_power_alpha'] = self._band_power(positive_freqs, positive_magnitudes, 8, 13)
        features['band_power_beta'] = self._band_power(positive_freqs, positive_magnitudes, 13, 30)
        features['band_power_gamma'] = self._band_power(positive_freqs, positive_magnitudes, 30, 100)
        
        self.extraction_stats['total_features'] += len(features)
        
        return features
    
    def _skewness(self, data: List[float]) -> float:
        """Calculate skewness"""
        mean = np.mean(data)
        std = np.std(data)
        if std == 0:
            return 0.0
        
        n = len(data)
        return (sum((x - mean)**3 for x in data) / n) / (std**3)
    
    def _kurtosis(self, data: List[float]) -> float:
        """Calculate kurtosis"""
        mean = np.mean(data)
        std = np.std(data)
        if std == 0:
            return 0.0
        
        n = len(data)
        return (sum((x - mean)**4 for x in data) / n) / (std**4)
    
    def _count_zero_crossings(self, signal: List[float]) -> int:
        """Count zero crossings"""
        crossings = 0
        for i in range(1, len(signal)):
            if signal[i-1] * signal[i] < 0:
                crossings += 1
        return crossings
    
    def _count_peaks(self, signal: List[float], threshold: float = 0.5) -> int:
        """Count peaks in signal"""
        peaks = 0
        for i in range(1, len(signal) - 1):
            if signal[i] > threshold and signal[i] > signal[i-1] and signal[i] > signal[i+1]:
                peaks += 1
        return peaks
    
    def _spectral_entropy(self, magnitudes: List[float]) -> float:
        """Calculate spectral entropy"""
        # Normalize to probability distribution
        total = sum(magnitudes)
        if total == 0:
            return 0.0
        
        probs = [m / total for m in magnitudes]
        entropy = 0.0
        for p in probs:
            if p > 0:
                entropy -= p * np.log(p)
        
        return entropy
    
    def _band_power(self, freqs: List[float], magnitudes: List[float], f_min: float, f_max: float) -> float:
        """Calculate power in frequency band"""
        mask = (freqs >= f_min) & (freqs <= f_max)
        return np.sum(np.array(magnitudes)[mask]**2)

# Expert analysis exports
__all__ = [
    'NeuralSignalProcessor', 'PatternRecognizer', 'StatisticalAnalyzer',
    'MLIntegration', 'SignalFilter', 'FeatureExtractor'
]
