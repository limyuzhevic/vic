#!/usr/bin/env python3

"""
NLM Sensory - Neural Network Sensory Systems

This module provides sensory system classes for agents to perceive their environment:
- Vision: Visual input processing
- Audio: Auditory input processing
- InternalSignals: Internal state monitoring
"""

from typing import Dict, List, Optional, Any, Union
import numpy as np
import time

class NLMError(Exception):
    """Base exception for NLM operations."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = time.time()
    
    def __str__(self):
        return f"[{self.error_code}] {self.message}"

class Vision:
    """Visual sensory system."""
    
    def __init__(self, resolution: int = 64, field_of_view: float = 90.0,
                 brightness: float = 1.0, contrast: float = 1.0):
        self.resolution = resolution
        self.field_of_view = field_of_view
        self.brightness = brightness
        self.contrast = contrast
        self.noise_level = 0.0
        self.color_mode = "grayscale"
        self.frame_rate = 30
    
    def get_observation(self) -> Dict:
        """Get visual observation."""
        # Simulate visual input
        width = height = self.resolution
        
        # Generate random visual data (simulating vision)
        observation = {
            "width": width,
            "height": height,
            "field_of_view": self.field_of_view,
            "brightness": self.brightness,
            "contrast": self.contrast,
            "noise_level": self.noise_level,
            "color_mode": self.color_mode,
            "frame_rate": self.frame_rate,
            "data": np.random.randn(height, width).tolist(),
            "timestamp": time.time()
        }
        
        # Apply brightness and contrast adjustments
        if self.brightness != 1.0:
            observation["data"] = [
                [pixel * self.brightness for pixel in row] 
                for row in observation["data"]
            ]
        
        if self.contrast != 1.0:
            mean_val = np.mean(observation["data"])
            observation["data"] = [
                [(pixel - mean_val) * self.contrast + mean_val for pixel in row]
                for row in observation["data"]
            ]
        
        return observation

class Audio:
    """Auditory sensory system."""
    
    def __init__(self, sample_rate: int = 44100, frequency_range: tuple = (20, 20000),
                 volume: float = 1.0, noise_level: float = 0.1):
        self.sample_rate = sample_rate
        self.frequency_range = frequency_range
        self.volume = volume
        self.noise_level = noise_level
        self.silence_threshold = 0.01
        self.sound_types = []
        self.direction_filtering = True
    
    def get_observation(self) -> Dict:
        """Get audio observation."""
        # Generate random audio data (simulating sound)
        duration = 0.1  # seconds
        num_samples = int(duration * self.sample_rate)
        
        # Create complex audio signal with random components
        time_axis = np.linspace(0, duration, num_samples)
        audio_data = np.random.randn(num_samples)
        
        # Add some frequency components
        for freq in np.random.uniform(self.frequency_range[0], 
                                       self.frequency_range[1], 5):
            audio_data += np.sin(2 * np.pi * freq * time_axis) * 0.3
        
        # Apply volume and noise
        audio_data *= self.volume
        audio_data += np.random.randn(num_samples) * self.noise_level
        
        observation = {
            "sample_rate": self.sample_rate,
            "frequency_range": self.frequency_range,
            "volume": self.volume,
            "noise_level": self.noise_level,
            "silence_threshold": self.silence_threshold,
            "sound_types": self.sound_types,
            "direction_filtering": self.direction_filtering,
            "data": audio_data.tolist(),
            "duration": duration,
            "timestamp": time.time()
        }
        
        return observation

class InternalSignals:
    """Internal neural signals monitoring."""
    
    def __init__(self, update_rate: float = 10.0):
        self.update_rate = update_rate
        self.last_update = 0
        self.metrics = {
            "heart_rate": 0.0,
            "respiration_rate": 0.0,
            "body_temperature": 36.5,
            "energy_level": 1.0,
            "stress_level": 0.0,
            "focus_level": 0.0,
            "arousal_level": 0.0,
            "attention_level": 0.0
        }
        self.baseline_metrics = {}
    
    def get_observation(self) -> Dict:
        """Get internal signals observation."""
        current_time = time.time()
        
        # Update metrics if time has passed
        if current_time - self.last_update >= 1.0 / self.update_rate:
            self._update_metrics()
            self.last_update = current_time
        
        observation = {
            "metrics": self.metrics,
            "baseline_metrics": self.baseline_metrics,
            "update_rate": self.update_rate,
            "last_update": self.last_update,
            "timestamp": current_time
        }
        
        return observation
    
    def set_baseline(self) -> None:
        """Set current metrics as baseline."""
        self.baseline_metrics = self.metrics.copy()
    
    def _update_metrics(self) -> None:
        """Update internal metrics with small random variations."""
        # Simulate biological variations
        self.metrics["heart_rate"] += np.random.randn() * 0.5
        self.metrics["respiration_rate"] += np.random.randn() * 0.2
        self.metrics["body_temperature"] += np.random.randn() * 0.1
        self.metrics["energy_level"] += np.random.randn() * 0.05
        self.metrics["stress_level"] += np.random.randn() * 0.05
        self.metrics["focus_level"] += np.random.randn() * 0.05
        self.metrics["arousal_level"] += np.random.randn() * 0.05
        self.metrics["attention_level"] += np.random.randn() * 0.05
        
        # Clamp to reasonable ranges
        self.metrics["heart_rate"] = max(40, min(180, self.metrics["heart_rate"]))
        self.metrics["respiration_rate"] = max(4, min(40, self.metrics["respiration_rate"]))
        self.metrics["body_temperature"] = max(34, min(42, self.metrics["body_temperature"]))
        self.metrics["energy_level"] = max(0, min(1, self.metrics["energy_level"]))
        self.metrics["stress_level"] = max(0, min(1, self.metrics["stress_level"]))
        self.metrics["focus_level"] = max(0, min(1, self.metrics["focus_level"]))
        self.metrics["arousal_level"] = max(0, min(1, self.metrics["arousal_level"]))
        self.metrics["attention_level"] = max(0, min(1, self.metrics["attention_level"]))

class NLMContextManager:
    """Context manager for NLM resources."""
    
    def __init__(self, auto_save: bool = True, auto_profiling: bool = False):
        self.auto_save = auto_save
        self.auto_profiling = auto_profiling
        self.session_id = id(self)
        self.start_time = None
        self.brain = None
        self.experiment = None
        self.monitor = None
    
    def __enter__(self):
        self.start_time = time.time()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration = time.time() - self.start_time if self.start_time else 0
        
        if exc_type:
            print(f"NLM Session {self.session_id} ended with error: {exc_val}")
        else:
            print(f"NLM Session {self.session_id} completed successfully in {duration:.2f}s")
        
        if self.auto_save and self.brain:
            self.save_session()
    
    def create_brain(self, config: 'Config') -> 'Brain':
        """Create a brain for this session."""
        from nlm.core.brain import Brain
        self.brain = Brain(config)
        self.brain.initialize()
        return self.brain
    
    def create_monitor(self) -> 'RealTimeMonitor':
        """Create a monitor for this session."""
        from nlm.monitoring.real_time import RealTimeMonitor
        self.monitor = RealTimeMonitor()
        self.monitor.start()
        return self.monitor
    
    def save_session(self, prefix: str = "session") -> tuple:
        """Save current session state."""
        timestamp = time.time()
        brain_file = f"{prefix}_{self.session_id}_brain.pkl"
        
        if self.brain:
            from nlm.core.brain import save_brain
            save_brain(self.brain, brain_file)
        
        return brain_file
    
    def load_session(self, brain_file: str) -> 'Brain':
        """Load session state."""
        from nlm.core.brain import load_brain
        if brain_file:
            self.brain = load_brain(brain_file)
        
        return self.brain

# Import types
import time
from nlm.core.brain import Brain, save_brain, load_brain
from nlm.core.config import Config

__all__ = [
    "Vision",
    "Audio",
    "InternalSignals",
    "NLMError",
    "NLMContextManager",
    "__version__"
]