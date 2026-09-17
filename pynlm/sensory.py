"""
Sensory processing classes for NLM neural simulation.
Provides classes for vision, audio, internal signals, and sensory input handling.
"""

from typing import Dict, List, Optional, Any, Union
import time
class SensoryInput:
    """Base class for sensory input.
    
    This class provides the base interface for all sensory input types,
    including vision, audio, internal signals, and proprioception.
    """
    
    def __init__(self, sensor_type: str = "generic"):
        """Initialize sensory input.
        
        Args:
            sensor_type: Type of sensor generating this input
        """
        self._sensor_type = sensor_type
        self._timestamp = time.time()
        self._data = []
        self._dimensions = 0
        self._quality_score = 1.0
    
    def getType(self) -> str:
        """Get the type of sensory input.
        
        Returns:
            Sensor type
        """
        return self._sensor_type
    
    def getData(self) -> List[float]:
        """Get the raw data as a list.
        
        Returns:
            Raw sensory data
        """
        return self._data
    
    def getDimensions(self) -> int:
        """Get the dimensionality of sensory data.
        
        Returns:
            Number of dimensions
        """
        return self._dimensions
    
    def getTimestamp(self) -> float:
        """Get the timestamp when data was captured.
        
        Returns:
            Timestamp in seconds
        """
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set the timestamp.
        
        Args:
            timestamp: Timestamp in seconds
        """
        self._timestamp = timestamp
    
    def setData(self, data: List[float]) -> None:
        """Set the sensory data.
        
        Args:
            data: Raw sensory data
        """
        self._data = data
        self._dimensions = len(data) if data else 0
    
    def getQualityScore(self) -> float:
        """Get the quality score of sensory data.
        
        Returns:
            Quality score between 0 and 1
        """
        return self._quality_score
    
    def setQualityScore(self, score: float) -> None:
        """Set the quality score.
        
        Args:
            score: Quality score between 0 and 1
        """
        self._quality_score = max(0.0, min(1.0, score))
    
    def encode(self) -> Dict[str, Any]:
        """Encode sensory data for neural processing.
        
        Returns:
            Encoded sensory representation
        """
        return {
            "type": self._sensor_type,
            "timestamp": self._timestamp,
            "data": self._data,
            "dimensions": self._dimensions,
            "quality": self._quality_score
        }
    
    def decode(self, encoded: Dict[str, Any]) -> None:
        """Decode neural output to sensory representation.
        
        Args:
            encoded: Encoded neural data
        """
        self._sensor_type = encoded.get("type", self._sensor_type)
        self._timestamp = encoded.get("timestamp", self._timestamp)
        self._data = encoded.get("data", self._data)
        self._dimensions = encoded.get("dimensions", self._dimensions)
        self._quality_score = encoded.get("quality", self._quality_score)
    
    def __repr__(self) -> str:
        return f"<SensoryInput: {self._sensor_type}, dim={self._dimensions}, quality={self._quality_score:.2f}>"

    def __str__(self) -> str:
        return self.__repr__()
class Vision(SensoryInput):
    """Vision sensory input for visual processing.
    
    This class provides vision input with support for different resolutions,
    color channels, and feature extraction.
    """
    
    def __init__(self, width: int = 8, height: int = 8, channels: int = 3):
        """Initialize vision sensory input.
        
        Args:
            width: Vision width in pixels
            height: Vision height in pixels
            channels: Number of color channels (1=gray, 3=RGB)
        """
        super().__init__("vision")
        self._width = width
        self._height = height
        self._channels = channels
        self._pixel_data = []
        self._feature_vectors = []
        self._edge_features = []
        self._color_features = []
        self._texture_features = []
    
    def getWidth(self) -> int:
        """Get vision width.
        
        Returns:
            Vision width in pixels
        """
        return self._width
    
    def getHeight(self) -> int:
        """Get vision height.
        
        Returns:
            Vision height in pixels
        """
        return self._height
    
    def getChannels(self) -> int:
        """Get number of color channels.
        
        Returns:
            Number of channels
        """
        return self._channels
    
    def getPixelData(self) -> List[List[List[float]]]:
        """Get 2D array of pixel data.
        
        Returns:
            3D array of pixel data [y][x][channel]
        """
        return self._pixel_data
    
    def setPixelData(self, pixel_data: List[List[List[float]]]) -> None:
        """Set pixel data.
        
        Args:
            pixel_data: 3D array of pixel data [y][x][channel]
        """
        self._pixel_data = pixel_data
        self._extract_features()
    
    def getFeatureVector(self) -> List[float]:
        """Get feature vector for neural processing.
        
        Returns:
            Feature vector
        """
        if not self._feature_vectors:
            self._extract_features()
        return self._feature_vectors
    
    def getEdgeFeatures(self) -> List[float]:
        """Get edge detection features.
        
        Returns:
            Edge feature vector
        """
        if not self._edge_features:
            self._extract_features()
        return self._edge_features
    
    def getColorFeatures(self) -> List[float]:
        """Get color features.
        
        Returns:
            Color feature vector
        """
        if not self._color_features:
            self._extract_features()
        return self._color_features
    
    def getTextureFeatures(self) -> List[float]:
        """Get texture features.
        
        Returns:
            Texture feature vector
        """
        if not self._texture_features:
            self._extract_features()
        return self._texture_features
    
    def _extract_features(self) -> None:
        """Extract features from pixel data."""
        if not self._pixel_data:
            return
        
        # Simple feature extraction
        feature_vector = []
        edge_features = []
        color_features = []
        texture_features = []
        
        total_pixels = self._width * self._height
        
        for y in range(self._height):
            for x in range(self._width):
                if self._channels == 1:
                    pixel = self._pixel_data[y][x][0] if x < len(self._pixel_data[y]) else 0.0
                else:
                    pixel = sum(self._pixel_data[y][x][:self._channels]) / self._channels if x < len(self._pixel_data[y]) else 0.0
                
                # Add pixel intensity to feature vector
                feature_vector.append(pixel)
                
                # Simple edge detection
                if x > 0 and y > 0:
                    prev_pixel = self._pixel_data[y-1][x-1][0] if self._channels == 1 else sum(self._pixel_data[y-1][x-1][:self._channels]) / self._channels
                    edge_strength = abs(pixel - prev_pixel)
                    edge_features.append(edge_strength)
                
                # Color features (simplified)
                if self._channels >= 3:
                    r = self._pixel_data[y][x][0] if x < len(self._pixel_data[y]) else 0.0
                    g = self._pixel_data[y][x][1] if x < len(self._pixel_data[y]) and self._channels > 1 else 0.0
                    b = self._pixel_data[y][x][2] if x < len(self._pixel_data[y]) and self._channels > 2 else 0.0
                    color_features.extend([r, g, b])
        
        self._feature_vectors = feature_vector
        self._edge_features = edge_features
        self._color_features = color_features
        self._texture_features = []  # Placeholder for texture analysis
    
    def encode(self) -> Dict[str, Any]:
        """Encode vision data for neural processing.
        
        Returns:
            Encoded vision representation
        """
        data = super().encode()
        data.update({
            "width": self._width,
            "height": self._height,
            "channels": self._channels,
            "pixel_data": self._pixel_data,
            "feature_vector": self._feature_vectors,
            "edge_features": self._edge_features,
            "color_features": self._color_features,
            "texture_features": self._texture_features
        })
        return data
    
    def decode(self, encoded: Dict[str, Any]) -> None:
        """Decode neural output to vision representation.
        
        Args:
            encoded: Encoded vision data
        """
        super().decode(encoded)
        self._width = encoded.get("width", self._width)
        self._height = encoded.get("height", self._height)
        self._channels = encoded.get("channels", self._channels)
        self._pixel_data = encoded.get("pixel_data", self._pixel_data)
        self._feature_vectors = encoded.get("feature_vector", self._feature_vectors)
        self._edge_features = encoded.get("edge_features", self._edge_features)
        self._color_features = encoded.get("color_features", self._color_features)
        self._texture_features = encoded.get("texture_features", self._texture_features)
    
    def __repr__(self) -> str:
        return f"<Vision: {self._width}x{self._height}, {self._channels} channels, features={len(self._feature_vectors)}>"

    def __str__(self) -> str:
        return self.__repr__()
class Audio(SensoryInput):
    """Audio sensory input for sound processing.
    
    This class provides audio input with support for different sample rates,
    number of samples, and feature extraction for neural processing.
    """
    
    def __init__(self, sample_rate: int = 44100, num_samples: int = 1024):
        """Initialize audio sensory input.
        
        Args:
            sample_rate: Sampling rate in Hz
            num_samples: Number of audio samples
        """
        super().__init__("audio")
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._audio_samples = []
        self._frequency_features = []
        self._mfcc_coefficients = []
        self._spectrogram = []
        self._sound_envelope = []
    
    def getSampleRate(self) -> int:
        """Get audio sample rate.
        
        Returns:
            Sample rate in Hz
        """
        return self._sample_rate
    
    def setSampleRate(self, sample_rate: int) -> None:
        """Set audio sample rate.
        
        Args:
            sample_rate: Sample rate in Hz
        """
        self._sample_rate = sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of audio samples.
        
        Returns:
            Number of samples
        """
        return self._num_samples
    
    def setNumSamples(self, num_samples: int) -> None:
        """Set number of audio samples.
        
        Args:
            num_samples: Number of samples
        """
        self._num_samples = num_samples
    
    def getAudioSamples(self) -> List[float]:
        """Get audio samples.
        
        Returns:
            List of audio samples
        """
        return self._audio_samples
    
    def setAudioSamples(self, audio_samples: List[float]) -> None:
        """Set audio samples.
        
        Args:
            audio_samples: List of audio samples
        """
        self._audio_samples = audio_samples
        if len(audio_samples) > self._num_samples:
            self._audio_samples = audio_samples[:self._num_samples]
        elif len(audio_samples) < self._num_samples:
            self._audio_samples.extend([0.0] * (self._num_samples - len(self._audio_samples)))
        
        self._extract_features()
    
    def getFrequencyFeatures(self) -> List[float]:
        """Get frequency domain features.
        
        Returns:
            Frequency feature vector
        """
        if not self._frequency_features:
            self._extract_features()
        return self._frequency_features
    
    def getMFCCCoefficients(self) -> List[List[float]]:
        """Get Mel-frequency cepstral coefficients.
        
        Returns:
            MFCC coefficients matrix
        """
        if not self._mfcc_coefficients:
            self._extract_features()
        return self._mfcc_coefficients
    
    def getSpectrogram(self) -> List[List[float]]:
        """Get spectrogram representation.
        
        Returns:
            Spectrogram matrix
        """
        if not self._spectrogram:
            self._extract_features()
        return self._spectrogram
    
    def getSoundEnvelope(self) -> List[float]:
        """Get sound envelope (amplitude over time).
        
        Returns:
            Sound envelope
        """
        if not self._sound_envelope:
            self._extract_features()
        return self._sound_envelope
    
    def _extract_features(self) -> None:
        """Extract features from audio samples."""
        if not self._audio_samples:
            return
        
        # Simple frequency analysis
        self._frequency_features = []
        
        # Calculate envelope (amplitude over time)
        self._sound_envelope = []
        for sample in self._audio_samples:
            self._sound_envelope.append(abs(sample))
        
        # Simple MFCC-like coefficients (placeholder for actual MFCC computation)
        self._mfcc_coefficients = []
        for i in range(13):  # 13 MFCC coefficients
            self._mfcc_coefficients.append([0.0] * len(self._audio_samples))
        
        # Simple spectrogram (placeholder for actual spectrogram computation)
        self._spectrogram = []
        for i in range(10):  # 10 frequency bins
            self._spectrogram.append([0.0] * (self._num_samples // 10))
    
    def encode(self) -> Dict[str, Any]:
        """Encode audio data for neural processing.
        
        Returns:
            Encoded audio representation
        """
        data = super().encode()
        data.update({
            "sample_rate": self._sample_rate,
            "num_samples": self._num_samples,
            "audio_samples": self._audio_samples,
            "frequency_features": self._frequency_features,
            "mfcc_coefficients": self._mfcc_coefficients,
            "spectrogram": self._spectrogram,
            "sound_envelope": self._sound_envelope
        })
        return data
    
    def decode(self, encoded: Dict[str, Any]) -> None:
        """Decode neural output to audio representation.
        
        Args:
            encoded: Encoded audio data
        """
        super().decode(encoded)
        self._sample_rate = encoded.get("sample_rate", self._sample_rate)
        self._num_samples = encoded.get("num_samples", self._num_samples)
        self._audio_samples = encoded.get("audio_samples", self._audio_samples)
        self._frequency_features = encoded.get("frequency_features", self._frequency_features)
        self._mfcc_coefficients = encoded.get("mfcc_coefficients", self._mfcc_coefficients)
        self._spectrogram = encoded.get("spectrogram", self._spectrogram)
        self._sound_envelope = encoded.get("sound_envelope", self._sound_envelope)
    
    def __repr__(self) -> str:
        return f"<Audio: {self._sample_rate} Hz, {self._num_samples} samples, features={len(self._frequency_features)}>"

    def __str__(self) -> str:
        return self.__repr__()
class InternalSignals(SensoryInput):
    """Internal signals sensory input for neural state monitoring.
    
    This class provides access to internal neural state variables,
    including firing rates, membrane potentials, and synaptic activity.
    """
    
    def __init__(self):
        """Initialize internal signals sensory input."""
        super().__init__("internal")
        self._signals = {}
        self._history = []
        self._signal_types = []
    
    def addSignal(self, value: float) -> None:
        """Add an internal signal value.
        
        Args:
            value: Signal value to add
        """
        signal_id = len(self._signals)
        self._signals[signal_id] = value
        self._signal_types.append("float")
        self._history.append(dict(self._signals))
        
        # Update encoded data
        self._data.append(value)
        self._dimensions = len(self._data)
    
    def clearSignals(self) -> None:
        """Clear all internal signals."""
        self._signals.clear()
        self._signal_types.clear()
        self._history.clear()
        self._data.clear()
        self._dimensions = 0
    
    def getSignal(self, signal_id: int) -> float:
        """Get signal value by ID.
        
        Args:
            signal_id: Signal identifier
            
        Returns:
            Signal value
        """
        return self._signals.get(signal_id, 0.0)
    
    def getAllSignals(self) -> Dict[int, float]:
        """Get all signal values.
        
        Returns:
            Dictionary of signal ID to value
        """
        return self._signals.copy()
    
    def getSignalHistory(self, signal_id: int, steps_back: int = 10) -> List[float]:
        """Get signal history.
        
        Args:
            signal_id: Signal identifier
            steps_back: Number of past steps to retrieve
            
        Returns:
            Signal history
        """
        if steps_back <= 0:
            return []
        
        history = []
        for i in range(min(steps_back, len(self._history))):
            history.append(self._history[-(i + 1)].get(signal_id, 0.0))
        
        return history
    
    def getSignalAverage(self, signal_id: int, steps: int = 100) -> float:
        """Get signal average over time.
        
        Args:
            signal_id: Signal identifier
            steps: Number of steps for average
            
        Returns:
            Average signal value
        """
        history = self.getSignalHistory(signal_id, steps)
        if not history:
            return 0.0
        
        return sum(history) / len(history)
    
    def encode(self) -> Dict[str, Any]:
        """Encode internal signals for neural processing.
        
        Returns:
            Encoded internal signals representation
        """
        data = super().encode()
        data.update({
            "signals": self._signals,
            "signal_types": self._signal_types,
            "history": self._history
        })
        return data
    
    def decode(self, encoded: Dict[str, Any]) -> None:
        """Decode neural output to internal signals representation.
        
        Args:
            encoded: Encoded internal signals data
        """
        super().decode(encoded)
        self._signals = encoded.get("signals", self._signals)
        self._signal_types = encoded.get("signal_types", self._signal_types)
        self._history = encoded.get("history", self._history)
    
    def __repr__(self) -> str:
        return f"<InternalSignals: {len(self._signals)} signals, history={len(self._history)}>"

    def __str__(self) -> str:
        return self.__repr__()
class Proprioception(SensoryInput):
    """Proprioceptive sensory input for body position and movement.
    
    This class provides proprioceptive input including position,
    orientation, velocity, and acceleration data.
    """
    
    def __init__(self):
        """Initialize proprioceptive sensory input."""
        super().__init__("proprioception")
        self._position_x = 0.0
        self._position_y = 0.0
        self._position_z = 0.0
        self._orientation = 0.0
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._velocity_z = 0.0
        self._angular_velocity = 0.0
        self._acceleration = 0.0
    
    def getPositionX(self) -> float:
        """Get X position.
        
        Returns:
            X position
        """
        return self._position_x
    
    def setPositionX(self, x: float) -> None:
        """Set X position.
        
        Args:
            x: X position
        """
        self._position_x = x
    
    def getPositionY(self) -> float:
        """Get Y position.
        
        Returns:
            Y position
        """
        return self._position_y
    
    def setPositionY(self, y: float) -> None:
        """Set Y position.
        
        Args:
            y: Y position
        """
        self._position_y = y
    
    def getPositionZ(self) -> float:
        """Get Z position.
        
        Returns:
            Z position
        """
        return self._position_z
    
    def setPositionZ(self, z: float) -> None:
        """Set Z position.
        
        Args:
            z: Z position
        """
        self._position_z = z
    
    def getOrientation(self) -> float:
        """Get orientation (in degrees).
        
        Returns:
            Orientation in degrees
        """
        return self._orientation
    
    def setOrientation(self, orientation: float) -> None:
        """Set orientation.
        
        Args:
            orientation: Orientation in degrees
        """
        self._orientation = orientation % 360.0
    
    def getVelocityX(self) -> float:
        """Get X velocity.
        
        Returns:
            X velocity
        """
        return self._velocity_x
    
    def setVelocityX(self, vx: float) -> None:
        """Set X velocity.
        
        Args:
            vx: X velocity
        """
        self._velocity_x = vx
    
    def getVelocityY(self) -> float:
        """Get Y velocity.
        
        Returns:
            Y velocity
        """
        return self._velocity_y
    
    def setVelocityY(self, vy: float) -> None:
        """Set Y velocity.
        
        Args:
            vy: Y velocity
        """
        self._velocity_y = vy
    
    def getVelocityZ(self) -> float:
        """Get Z velocity.
        
        Returns:
            Z velocity
        """
        return self._velocity_z
    
    def setVelocityZ(self, vz: float) -> None:
        """Set Z velocity.
        
        Args:
            vz: Z velocity
        """
        self._velocity_z = vz
    
    def getAngularVelocity(self) -> float:
        """Get angular velocity.
        
        Returns:
            Angular velocity in degrees per second
        """
        return self._angular_velocity
    
    def setAngularVelocity(self, angular_velocity: float) -> None:
        """Set angular velocity.
        
        Args:
            angular_velocity: Angular velocity in degrees per second
        """
        self._angular_velocity = angular_velocity
    
    def getAcceleration(self) -> float:
        """Get acceleration.
        
        Returns:
            Acceleration magnitude
        """
        return self._acceleration
    
    def setAcceleration(self, acceleration: float) -> None:
        """Set acceleration.
        
        Args:
            acceleration: Acceleration magnitude
        """
        self._acceleration = acceleration
    
    def encode(self) -> Dict[str, Any]:
        """Encode proprioceptive data for neural processing.
        
        Returns:
            Encoded proprioceptive representation
        """
        data = super().encode()
        data.update({
            "position_x": self._position_x,
            "position_y": self._position_y,
            "position_z": self._position_z,
            "orientation": self._orientation,
            "velocity_x": self._velocity_x,
            "velocity_y": self._velocity_y,
            "velocity_z": self._velocity_z,
            "angular_velocity": self._angular_velocity,
            "acceleration": self._acceleration
        })
        return data
    
    def decode(self, encoded: Dict[str, Any]) -> None:
        """Decode neural output to proprioceptive representation.
        
        Args:
            encoded: Encoded proprioceptive data
        """
        super().decode(encoded)
        self._position_x = encoded.get("position_x", self._position_x)
        self._position_y = encoded.get("position_y", self._position_y)
        self._position_z = encoded.get("position_z", self._position_z)
        self._orientation = encoded.get("orientation", self._orientation)
        self._velocity_x = encoded.get("velocity_x", self._velocity_x)
        self._velocity_y = encoded.get("velocity_y", self._velocity_y)
        self._velocity_z = encoded.get("velocity_z", self._velocity_z)
        self._angular_velocity = encoded.get("angular_velocity", self._angular_velocity)
        self._acceleration = encoded.get("acceleration", self._acceleration)
    
    def __repr__(self) -> str:
        return f"<Proprioception: pos=({self._position_x:.2f}, {self._position_y:.2f}, {self._position_z:.2f}), vel=({self._velocity_x:.2f}, {self._velocity_y:.2f}, {self._velocity_z:.2f})>"

    def __str__(self) -> str:
        return self.__repr__()
# Global factory functions
def createVision(width: int = 8, height: int = 8, channels: int = 3) -> Vision:
    """Create vision sensory input.
    
    Args:
        width: Vision width in pixels
        height: Vision height in pixels
        channels: Number of color channels
        
    Returns:
        Vision instance
    """
    return Vision(width, height, channels)
def createAudio(sample_rate: int = 44100, num_samples: int = 1024) -> Audio:
    """Create audio sensory input.
    
    Args:
        sample_rate: Audio sample rate
        num_samples: Number of audio samples
        
    Returns:
        Audio instance
    """
    return Audio(sample_rate, num_samples)
def createInternalSignals() -> InternalSignals:
    """Create internal signals sensory input.
    
    Returns:
        InternalSignals instance
    """
    return InternalSignals()
def createProprioception() -> Proprioception:
    """Create proprioceptive sensory input.
    
    Returns:
        Proprioception instance
    """
    return Proprioception()

__all__ = [
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Proprioception",
    "createVision",
    "createAudio",
    "createInternalSignals",
    "createProprioception"
]
