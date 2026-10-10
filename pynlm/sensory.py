"""
Sensory processing and advanced sensing capabilities.

This module contains sensory processing components and advanced sensing
features for expert users.
"""

from .core import *

class SensoryProcessor:
    """
    Advanced sensory processor for expert users.
    
    Provides advanced sensory processing, feature extraction, and
    multi-modal sensing capabilities.
    """
    
    def __init__(self):
        self._sensors = []
        self._feature_extractors = []
        self._noise_models = {}
        self._calibration_data = {}
        self._last_reading = None
        
    def add_sensor(self, sensor):
        """Add a sensor to the processor."""
        self._sensors.append(sensor)
        
    def add_feature_extractor(self, name, extractor_func):
        """Add a feature extractor function."""
        self._feature_extractors.append((name, extractor_func))
        
    def calibrate_sensor(self, sensor_name, calibration_data):
        """
        Calibrate a sensor with calibration data.
        
        Args:
            sensor_name: Name of the sensor
            calibration_data: Calibration data dictionary
        """
        self._calibration_data[sensor_name] = calibration_data
        print(f"[INFO] Calibrated sensor '{sensor_name}' with data: {calibration_data}")
        
    def process_sensory_input(self, raw_input):
        """
        Process raw sensory input with advanced feature extraction.
        
        Args:
            raw_input: Raw sensory input data
            
        Returns:
            dict: Processed sensory features
        """
        processed = {
            'timestamp': time.time() if 'time' in globals() else 0.0,
            'sensor_type': type(raw_input).__name__,
            'raw_data': raw_input,
            'features': {},
            'quality': 1.0
        }
        
        # Apply feature extractors
        for name, extractor in self._feature_extractors:
            try:
                features = extractor(raw_input)
                processed['features'][name] = features
            except Exception as e:
                print(f"[WARNING] Feature extractor '{name}' failed: {e}")
        
        # Apply noise models
        for sensor_name, noise_model in self._noise_models.items():
            try:
                processed['features'][f"{sensor_name}_noise"] = noise_model.apply(raw_input)
            except Exception as e:
                print(f"[WARNING] Noise model '{sensor_name}' failed: {e}")
        
        self._last_reading = processed
        return processed
        
    def get_feature_vector(self, sensory_input):
        """
        Convert sensory input to feature vector for machine learning.
        
        Args:
            sensory_input: Raw sensory input
            
        Returns:
            list: Feature vector
        """
        processed = self.process_sensory_input(sensory_input)
        
        # Convert features to flat vector
        feature_vector = []
        for name, features in processed['features'].items():
            if isinstance(features, (list, tuple)):
                feature_vector.extend(features)
            elif isinstance(features, (int, float)):
                feature_vector.append(features)
            elif isinstance(features, dict):
                feature_vector.extend(list(features.values()))
        
        return feature_vector
        
    def detect_anomalies(self, sensory_input, threshold=2.0):
        """
        Detect anomalies in sensory input.
        
        Args:
            sensory_input: Sensory input to analyze
            threshold: Anomaly detection threshold
            
        Returns:
            list: Detected anomalies
        """
        anomalies = []
        
        # Store in history for comparison
        if not hasattr(self, '_history'):
            self._history = []
        
        processed = self.process_sensory_input(sensory_input)
        self._history.append(processed)
        
        # Limit history size
        if len(self._history) > 1000:
            self._history.pop(0)
        
        if len(self._history) >= 10:
            # Compare with recent history
            recent = self._history[-10:]
            current_features = processed['features']
            
            for i, past in enumerate(recent[:-1]):
                for feature_name, current_val in current_features.items():
                    if feature_name in past['features']:
                        past_val = past['features'][feature_name]
                        
                        if isinstance(current_val, (int, float)) and isinstance(past_val, (int, float)):
                            deviation = abs(current_val - past_val)
                            if deviation > threshold:
                                anomalies.append({
                                    'feature': feature_name,
                                    'current': current_val,
                                    'previous': past_val,
                                    'deviation': deviation,
                                    'timestamp': processed['timestamp']
                                })
        
        return anomalies
        
    def get_sensory_statistics(self):
        """
        Get statistics about sensory processing.
        
        Returns:
            dict: Sensory processing statistics
        """
        if not hasattr(self, '_history') or not self._history:
            return {}
        
        feature_stats = {}
        
        for feature_name in self._get_all_feature_names():
            values = []
            for processed in self._history:
                if feature_name in processed['features']:
                    val = processed['features'][feature_name]
                    if isinstance(val, (int, float)):
                        values.append(val)
            
            if values:
                feature_stats[feature_name] = {
                    'mean': sum(values) / len(values),
                    'min': min(values),
                    'max': max(values),
                    'std': (sum((v - sum(values)/len(values))**2 for v in values) / len(values))**0.5,
                    'count': len(values)
                }
        
        return {
            'total_readings': len(self._history),
            'feature_count': len(self._feature_extractors),
            'noise_models': len(self._noise_models),
            'feature_statistics': feature_stats,
            'last_reading_time': self._last_reading['timestamp'] if self._last_reading else None
        }
        
    def _get_all_feature_names(self):
        """Get all feature names from all extractors."""
        feature_names = []
        for name, _ in self._feature_extractors:
            feature_names.append(name)
        for noise_name in self._noise_models.keys():
            feature_names.append(f"{noise_name}_noise")
        return feature_names
        
    def add_noise_model(self, name, noise_func):
        """
        Add a noise model for sensor simulation.
        
        Args:
            name: Noise model name
            noise_func: Function that adds noise to sensor readings
        """
        self._noise_models[name] = noise_func
        
    def simulate_sensor_failure(self, sensor_name, failure_type="noise"):
        """
        Simulate sensor failure for testing robustness.
        
        Args:
            sensor_name: Sensor name
            failure_type: Type of failure (noise, bias, dropout)
        """
        if failure_type == "noise":
            self.add_noise_model(sensor_name, lambda x: x * 1.5 + 0.5)
        elif failure_type == "bias":
            self.add_noise_model(sensor_name, lambda x: x + 2.0)
        elif failure_type == "dropout":
            self.add_noise_model(sensor_name, lambda x: 0.0)
        
        print(f"[INFO] Simulated {failure_type} failure for sensor '{sensor_name}'")
        
    def get_last_reading(self):
        """Get the last processed sensory reading."""
        return self._last_reading
        
    def clear_history(self):
        """Clear sensory processing history."""
        self._history.clear()
        print("[INFO] Sensory processing history cleared")
        
    def __str__(self):
        """String representation of sensory processor."""
        return f"SensoryProcessor(sensors={len(self._sensors)}, features={len(self._feature_extractors)})"
        
    def __repr__(self):
        """Detailed representation of sensory processor."""
        return f"SensoryProcessor(sensors={self._sensors}, feature_extractors={len(self._feature_extractors)}, " \
               f"noise_models={len(self._noise_models)})"
# Export advanced sensory classes and functions
from .sensory import SensoryProcessor