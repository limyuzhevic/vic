"""
Sensory input module for NLM Python bindings.
"""

import time
import numpy as np
from typing import Dict, List, Any, Optional, Union

try:
    from pynlm.bindings import SensoryInput, Vision, Audio, InternalSignals, createSimpleWorld
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Sensory module limited.")

# Import exceptions
from .exceptions import NLMInitializationError


class SensoryState:
    """Represents current sensory state."""
    
    def __init__(self, vision_data=None, audio_data=None, internal_data=None):
        self.vision = vision_data or []
        self.audio = audio_data or []
        self.internal = internal_data or []
        self.timestamp = time.time()
        self.dimension = len(vision_data) if vision_data else 0
    
    def get_vision(self) -> List[float]:
        """Get vision sensory data."""
        return self.vision
    
    def set_vision(self, data: List[float]):
        """Set vision sensory data."""
        self.vision = data
        self.dimension = len(data) if data else 0
    
    def get_audio(self) -> List[float]:
        """Get audio sensory data."""
        return self.audio
    
    def set_audio(self, data: List[float]):
        """Set audio sensory data."""
        self.audio = data
    
    def get_internal(self) -> List[float]:
        """Get internal signals."""
        return self.internal
    
    def set_internal(self, data: List[float]):
        """Set internal signals."""
        self.internal = data
    
    def get_all_signals(self) -> Dict[str, List[float]]:
        """Get all sensory signals."""
        return {
            'vision': self.vision,
            'audio': self.audio,
            'internal': self.internal,
        }
    
    def get_timestamp(self) -> float:
        """Get timestamp."""
        return self.timestamp
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp."""
        self.timestamp = timestamp
    
    def get_dimension(self) -> int:
        """Get sensory dimension."""
        return self.dimension
    
    def add_noise(self, noise_level: float = 0.1):
        """Add random noise to sensory data."""
        if self.vision:
            self.vision = [v + np.random.normal(0, noise_level) for v in self.vision]
        if self.audio:
            self.audio = [a + np.random.normal(0, noise_level) for a in self.audio]
        if self.internal:
            self.internal = [i + np.random.normal(0, noise_level) for i in self.internal]
    
    def normalize(self, method: str = 'minmax'):
        """
        Normalize sensory data.
        
        Args:
            method: Normalization method ('minmax', 'zscore', 'softmax')
        """
        if method == 'minmax':
            for data_list in [self.vision, self.audio, self.internal]:
                if data_list:
                    min_val = min(data_list)
                    max_val = max(data_list)
                    if max_val > min_val:
                        data_list[:] = [(v - min_val) / (max_val - min_val) for v in data_list]
        
        elif method == 'zscore':
            for data_list in [self.vision, self.audio, self.internal]:
                if data_list:
                    mean_val = np.mean(data_list)
                    std_val = np.std(data_list)
                    if std_val > 0:
                        data_list[:] = [(v - mean_val) / std_val for v in data_list]
        
        elif method == 'softmax':
            for data_list in [self.vision, self.audio, self.internal]:
                if data_list:
                    exp_data = np.exp(data_list)
                    data_list[:] = exp_data / np.sum(exp_data)
    
    def __repr__(self) -> str:
        return f"SensoryState(vision={len(self.vision)}, audio={len(self.audio)}, internal={len(self.internal)})"


class VisionProcessor:
    """Vision sensory processor."""
    
    def __init__(self, width: int, height: int, channels: int = 3):
        """
        Initialize vision processor.
        
        Args:
            width: Vision width
            height: Vision height
            channels: Number of color channels
        """
        self.width = width
        self.height = height
        self.channels = channels
        self.sensitivity = 1.0
        self.noise_level = 0.0
        self.preferred_range = [0.0, 1.0]
    
    def process(self, raw_data: List[float]) -> SensoryState:
        """
        Process raw vision data.
        
        Args:
            raw_data: Raw sensor data
        
        Returns:
            Processed sensory state
        """
        state = SensoryState()
        
        # Resize if needed
        if len(raw_data) != self.width * self.height * self.channels:
            # Simplified resizing
            target_size = self.width * self.height * self.channels
            if len(raw_data) > 0:
                step = len(raw_data) / target_size
                processed = []
                for i in range(target_size):
                    idx = int(i * step)
                    processed.append(raw_data[min(idx, len(raw_data) - 1)])
                raw_data = processed
        
        # Apply sensitivity and noise
        state.vision = [v * self.sensitivity for v in raw_data]
        if self.noise_level > 0:
            state.add_noise(self.noise_level)
        
        # Normalize to preferred range
        if raw_data:
            min_val = min(raw_data)
            max_val = max(raw_data)
            if max_val > min_val:
                state.vision = [
                    self.preferred_range[0] + 
                    (v - min_val) * (self.preferred_range[1] - self.preferred_range[0]) / (max_val - min_val)
                    for v in state.vision
                ]
        
        return state


class AudioProcessor:
    """Audio sensory processor."""
    
    def __init__(self, sample_rate: int = 44100, num_samples: int = 1024):
        """
        Initialize audio processor.
        
        Args:
            sample_rate: Audio sample rate
            num_samples: Number of audio samples
        """
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.frequency_sensitivity = 1.0
        self.noise_level = 0.0
        self.bands = 20  # Number of frequency bands
    
    def process(self, raw_data: List[float]) -> SensoryState:
        """
        Process raw audio data.
        
        Args:
            raw_data: Raw sensor data
        
        Returns:
            Processed sensory state
        """
        state = SensoryState()
        
        # Resize if needed
        if len(raw_data) != self.num_samples:
            if len(raw_data) > 0:
                step = len(raw_data) / self.num_samples
                processed = []
                for i in range(self.num_samples):
                    idx = int(i * step)
                    processed.append(raw_data[min(idx, len(raw_data) - 1)])
                raw_data = processed
        
        # Apply frequency sensitivity and noise
        state.audio = [a * self.frequency_sensitivity for a in raw_data]
        if self.noise_level > 0:
            state.add_noise(self.noise_level)
        
        # Normalize
        state.normalize('zscore')
        
        return state


class InternalSignalsProcessor:
    """Internal signals processor."""
    
    def __init__(self):
        """Initialize internal signals processor."""
        self.signal_history = []
        self.smoothing_factor = 0.9
        self.baseline_values = {}
        self.anomaly_threshold = 2.0
    
    def process(self, raw_data: List[float]) -> SensoryState:
        """
        Process raw internal signals.
        
        Args:
            raw_data: Raw sensor data
        
        Returns:
            Processed sensory state
        """
        state = SensoryState()
        
        # Apply smoothing
        if not self.signal_history:
            state.internal = raw_data.copy()
        else:
            smoothed = []
            for i, val in enumerate(raw_data):
                if i < len(self.signal_history):
                    smoothed_val = (self.smoothing_factor * val + 
                                  (1 - self.smoothing_factor) * self.signal_history[i])
                else:
                    smoothed_val = val
                smoothed.append(smoothed_val)
            state.internal = smoothed
        
        # Update history
        self.signal_history = state.internal.copy()
        
        # Detect anomalies
        self._detect_anomalies(state.internal)
        
        # Normalize
        state.normalize('zscore')
        
        return state
    
    def _detect_anomalies(self, signals: List[float]):
        """Detect anomalies in signals."""
        if not signals:
            return
        
        mean_val = np.mean(signals)
        std_val = np.std(signals)
        
        if std_val > 0:
            z_scores = [(s - mean_val) / std_val for s in signals]
            anomalies = [abs(z) > self.anomaly_threshold for z in z_scores]
            
            for i, is_anomaly in enumerate(anomalies):
                if is_anomaly:
                    signal_name = f'signal_{i}'
                    if signal_name not in self.baseline_values:
                        self.baseline_values[signal_name] = signals[i]


class SensorySystem:
    """Integrated sensory processing system."""
    
    def __init__(self, world=None):
        """
        Initialize sensory system.
        
        Args:
            world: World object (if None, will be created)
        """
        self.world = world
        self.vision_processor = VisionProcessor(8, 8)
        self.audio_processor = AudioProcessor()
        self.internal_processor = InternalSignalsProcessor()
        
        # Sensory configuration
        self.enable_noise = False
        self.noise_level = 0.1
        self.enable_normalization = True
        self.normalization_method = 'zscore'
    
    def process_sensory_input(self, sensory_data: Dict[str, Any] = None) -> SensoryState:
        """
        Process sensory input.
        
        Args:
            sensory_data: Raw sensory data
        
        Returns:
            Processed sensory state
        """
        state = SensoryState()
        
        if not sensory_data:
            return state
        
        # Process vision
        if 'vision' in sensory_data:
            state.vision = sensory_data['vision']
            state.set_vision(self.vision_processor.process(sensory_data['vision']).vision)
        
        # Process audio
        if 'audio' in sensory_data:
            state.audio = sensory_data['audio']
            state.set_audio(self.audio_processor.process(sensory_data['audio']).audio)
        
        # Process internal signals
        if 'internal' in sensory_data:
            state.internal = sensory_data['internal']
            state.set_internal(
                self.internal_processor.process(sensory_data['internal']).internal
            )
        
        # Apply system-wide processing
        if self.enable_noise:
            state.add_noise(self.noise_level)
        
        if self.enable_normalization:
            state.normalize(self.normalization_method)
        
        return state
    
    def configure_vision(self, **kwargs):
        """Configure vision processor."""
        for key, value in kwargs.items():
            if hasattr(self.vision_processor, key):
                setattr(self.vision_processor, key, value)
    
    def configure_audio(self, **kwargs):
        """Configure audio processor."""
        for key, value in kwargs.items():
            if hasattr(self.audio_processor, key):
                setattr(self.audio_processor, key, value)
    
    def configure_internal(self, **kwargs):
        """Configure internal processor."""
        for key, value in kwargs.items():
            if hasattr(self.internal_processor, key):
                setattr(self.internal_processor, key, value)
    
    def get_sensory_state(self) -> SensoryState:
        """Get current sensory state."""
        if self.world:
            percept = self.world.getSensoryPercept()
            if percept:
                return self.process_sensory_input({
                    'vision': percept.getVision(),
                    'audio': percept.getAudio(),
                    'internal': percept.getInternal(),
                })
        return SensoryState()
    
    def set_sensory_config(self, **kwargs):
        """Set sensory system configuration."""
        for key, value in kwargs.items():
            if key == 'enable_noise':
                self.enable_noise = value
            elif key == 'noise_level':
                self.noise_level = value
            elif key == 'enable_normalization':
                self.enable_normalization = value
            elif key == 'normalization_method':
                self.normalization_method = value
    
    def __repr__(self) -> str:
        return f"SensorySystem(vision={self.vision_processor.width}x{self.vision_processor.height}, audio_samples={self.audio_processor.num_samples})"


def create_sensory_system(world=None, **kwargs):
    """Create sensory system with configuration."""
    system = SensorySystem(world)
    
    # Apply configuration
    if 'vision_width' in kwargs:
        system.configure_vision(width=kwargs['vision_width'])
    if 'vision_height' in kwargs:
        system.configure_vision(height=kwargs['vision_height'])
    if 'sample_rate' in kwargs:
        system.configure_audio(sample_rate=kwargs['sample_rate'])
    if 'num_samples' in kwargs:
        system.configure_audio(num_samples=kwargs['num_samples'])
    
    # Set system configuration
    system.set_sensory_config(**kwargs)
    
    return system


def create_sensory_percept(state: SensoryState):
    """Create sensory percept from processed state."""
    from pynlm.bindings import createSimpleWorld
    
    world = createSimpleWorld()
    
    # Create percept with processed data
    percept = world.getSensoryPercept()
    
    # Set the processed data
    if state.get_vision():
        percept.setVision(state.get_vision())
    if state.get_audio():
        percept.setAudio(state.get_audio())
    if state.get_internal():
        percept.setInternal(state.get_internal())
    
    percept.setTimestamp(state.get_timestamp())
    
    return percept
