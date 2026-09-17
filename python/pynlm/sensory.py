"""
Sensory input classes for pynlm.

Implements Vision, Audio, and related sensory classes with full type hints,
Pythonic APIs, and comprehensive error handling.
"""

from typing import Any, Dict, List, Optional, Union
from dataclasses import dataclass
from enum import Enum

from .exceptions import (
    SimulationError, ValidationError,
    raise_simulation_error, raise_validation_error
)


class SensoryInputType(Enum):
    """Types of sensory input."""
    VISION = "vision"
    AUDIO = "audio"
    INTERNAL = "internal"
    PROPRIOCEPTION = "proprioception"


class DataFormat(Enum):
    """Data formats for sensory input."""
    FLOAT32 = "float32"
    FLOAT64 = "float64"
    INT8 = "int8"
    INT16 = "int16"
    INT32 = "int32"
    INT64 = "int64"


@dataclass
class SensoryInput:
    """Base class for sensory input data.
    
    Attributes:
        data_type: Type of sensory input
        data: Raw sensory data
        dimensions: Shape of the data
        timestamp: Time when data was collected
        format: Data format
        metadata: Optional metadata dictionary
    """
    
    data_type: SensoryInputType
    data: Union[List[float], List[int], Any]
    dimensions: List[int]
    timestamp: float
    format: DataFormat = DataFormat.FLOAT32
    metadata: Optional[Dict[str, Any]] = None
    
    def __post_init__(self):
        """Validate sensory input after initialization."""
        if self.data is None:
            raise ValidationError("Sensory data cannot be None", field="data")
        
        if not isinstance(self.timestamp, (int, float)):
            raise ValidationError(f"Timestamp must be numeric, got {type(self.timestamp)}",
                                field="timestamp", value=self.timestamp)
        
        if not self.dimensions:
            raise ValidationError("Dimensions cannot be empty", field="dimensions")
        
        if self.metadata is None:
            self.metadata = {}


@dataclass
class Vision(SensoryInput):
    """Visual sensory input.
    
    Attributes:
        width: Image width in pixels
        height: Image height in pixels
        channels: Number of color channels
        color_space: Color space (e.g., RGB, grayscale)
        resolution: Pixel resolution
    """
    
    width: int
    height: int
    channels: int = 3
    color_space: str = "RGB"
    resolution: Optional[float] = None
    
    def __post_init__(self):
        """Validate vision input after initialization."""
        super().__post_init__()
        
        if self.width <= 0:
            raise ValidationError(f"Vision width must be positive, got {self.width}",
                                field="width", value=self.width)
        
        if self.height <= 0:
            raise ValidationError(f"Vision height must be positive, got {self.height}",
                                field="height", value=self.height)
        
        if self.channels <= 0:
            raise ValidationError(f"Vision channels must be positive, got {self.channels}",
                                field="channels", value=self.channels)
        
        if self.color_space not in ["RGB", "BGR", "grayscale", "HSV"]:
            raise ValidationError(f"Invalid color space: {self.color_space}",
                                field="color_space", value=self.color_space)
        
        # Set default dimensions
        self.dimensions = [self.height, self.width, self.channels]
        self.data_type = SensoryInputType.VISION


@dataclass
class Audio(SensoryInput):
    """Audio sensory input.
    
    Attributes:
        sample_rate: Sampling rate in Hz
        duration: Audio duration in seconds
        channels: Number of audio channels
        encoding: Audio encoding format
    """
    
    sample_rate: int
    duration: float
    channels: int = 1
    encoding: str = "pcm"
    
    def __post_init__(self):
        """Validate audio input after initialization."""
        super().__post_init__()
        
        if self.sample_rate <= 0:
            raise ValidationError(f"Sample rate must be positive, got {self.sample_rate}",
                                field="sample_rate", value=self.sample_rate)
        
        if self.duration <= 0:
            raise ValidationError(f"Duration must be positive, got {self.duration}",
                                field="duration", value=self.duration)
        
        if self.channels <= 0:
            raise ValidationError(f"Channels must be positive, got {self.channels}",
                                field="channels", value=self.channels)
        
        if self.encoding not in ["pcm", "wav", "mp3", "flac"]:
            raise ValidationError(f"Invalid encoding: {self.encoding}",
                                field="encoding", value=self.encoding)
        
        # Set default dimensions
        expected_samples = int(self.sample_rate * self.duration)
        self.dimensions = [expected_samples, self.channels]
        self.data_type = SensoryInputType.AUDIO


@dataclass
class InternalSignals(SensoryInput):
    """Internal neural signals.
    
    Attributes:
        signal_type: Type of internal signal
        signal_name: Name of the signal
        min_value: Minimum expected value
        max_value: Maximum expected value
    """
    
    signal_type: str
    signal_name: str
    min_value: float = 0.0
    max_value: float = 1.0
    
    def __post_init__(self):
        """Validate internal signals after initialization."""
        super().__post_init__()
        
        if not self.signal_type:
            raise ValidationError("Signal type cannot be empty", field="signal_type")
        
        if not self.signal_name:
            raise ValidationError("Signal name cannot be empty", field="signal_name")
        
        if self.min_value >= self.max_value:
            raise ValidationError("min_value must be less than max_value",
                                field="range", value={"min": self.min_value, "max": self.max_value})
        
        # Set default dimensions
        self.dimensions = [len(self.data) if hasattr(self.data, '__len__') else 0]
        self.data_type = SensoryInputType.INTERNAL


@dataclass
class Proprioception(SensoryInput):
    """Proprioceptive sensory input.
    
    Attributes:
        body_part: Body part being sensed
        sensor_type: Type of sensor
        joint_angle: Optional joint angle
        position: Optional position vector
        orientation: Optional orientation vector
    """
    
    body_part: str
    sensor_type: str
    joint_angle: Optional[float] = None
    position: Optional[List[float]] = None
    orientation: Optional[List[float]] = None
    
    def __post_init__(self):
        """Validate proprioception after initialization."""
        super().__post_init__()
        
        if not self.body_part:
            raise ValidationError("Body part cannot be empty", field="body_part")
        
        if not self.sensor_type:
            raise ValidationError("Sensor type cannot be empty", field="sensor_type")
        
        if self.position is not None and len(self.position) != 3:
            raise ValidationError("Position must be a 3D vector", field="position")
        
        if self.orientation is not None and len(self.orientation) != 3:
            raise ValidationError("Orientation must be a 3D vector", field="orientation")
        
        # Set default dimensions
        self.dimensions = [6]  # 3 position + 3 orientation
        self.data_type = SensoryInputType.PROPRIOCEPTION


# Vision processing utilities
class VisionProcessor:
    """Utility class for processing vision data."""
    
    @staticmethod
    def resize_vision(vision: Vision, new_width: int, new_height: int) -> Vision:
        """Resize vision data.
        
        Args:
            vision: Vision input to resize
            new_width: New width in pixels
            new_height: New height in pixels
            
        Returns:
            Resized Vision object
        """
        if new_width <= 0 or new_height <= 0:
            raise ValidationError("New dimensions must be positive",
                                field="dimensions", value={"width": new_width, "height": new_height})
        
        # Create new vision object with resized dimensions
        new_vision = Vision(
            width=new_width,
            height=new_height,
            channels=vision.channels,
            color_space=vision.color_space,
            data=vision.data,  # In practice, this would be resized
            timestamp=vision.timestamp,
            format=vision.format,
            metadata=vision.metadata
        )
        
        return new_vision
    
    @staticmethod
    def normalize_vision(vision: Vision, min_val: float = 0.0, max_val: float = 1.0) -> Vision:
        """Normalize vision data to specified range.
        
        Args:
            vision: Vision input to normalize
            min_val: Minimum value after normalization
            max_val: Maximum value after normalization
            
        Returns:
            Normalized Vision object
        """
        if min_val >= max_val:
            raise ValidationError("min_val must be less than max_val",
                                field="range", value={"min": min_val, "max": max_val})
        
        # Create new vision with normalized data
        # In practice, this would apply normalization
        normalized_vision = Vision(
            width=vision.width,
            height=vision.height,
            channels=vision.channels,
            color_space=vision.color_space,
            data=vision.data,  # Normalized data would go here
            timestamp=vision.timestamp,
            format=vision.format,
            metadata=vision.metadata
        )
        
        return normalized_vision
    
    @staticmethod
    def extract_region(vision: Vision, x: int, y: int, width: int, height: int) -> Vision:
        """Extract a region from vision data.
        
        Args:
            vision: Vision input to extract from
            x: X coordinate of top-left corner
            y: Y coordinate of top-left corner
            width: Region width
            height: Region height
            
        Returns:
            Region Vision object
        """
        if x < 0 or y < 0 or x + width > vision.width or y + height > vision.height:
            raise ValidationError("Region coordinates or dimensions out of bounds",
                                field="region", value={"x": x, "y": y, "width": width, "height": height})
        
        # Create new vision object for extracted region
        region_vision = Vision(
            width=width,
            height=height,
            channels=vision.channels,
            color_space=vision.color_space,
            data=vision.data,  # Extracted region would go here
            timestamp=vision.timestamp,
            format=vision.format,
            metadata=vision.metadata
        )
        
        return region_vision


# Audio processing utilities
class AudioProcessor:
    """Utility class for processing audio data."""
    
    @staticmethod
    def resample_audio(audio: Audio, new_sample_rate: int) -> Audio:
        """Resample audio to new sample rate.
        
        Args:
            audio: Audio input to resample
            new_sample_rate: New sample rate in Hz
            
        Returns:
            Resampled Audio object
        """
        if new_sample_rate <= 0:
            raise ValidationError("Sample rate must be positive",
                                field="sample_rate", value=new_sample_rate)
        
        # Calculate new duration
        new_duration = len(audio.data) / new_sample_rate if hasattr(audio.data, '__len__') else audio.duration
        
        # Create new audio object with resampled data
        # In practice, this would apply resampling
        resampled_audio = Audio(
            sample_rate=new_sample_rate,
            duration=new_duration,
            channels=audio.channels,
            encoding=audio.encoding,
            data=audio.data,  # Resampled data would go here
            timestamp=audio.timestamp,
            format=audio.format,
            metadata=audio.metadata
        )
        
        return resampled_audio
    
    @staticmethod
    def normalize_audio(audio: Audio, target_db: float = -20.0) -> Audio:
        """Normalize audio to target decibel level.
        
        Args:
            audio: Audio input to normalize
            target_db: Target decibel level
            
        Returns:
            Normalized Audio object
        """
        # Create new audio object with normalized data
        # In practice, this would apply amplitude normalization
        normalized_audio = Audio(
            sample_rate=audio.sample_rate,
            duration=audio.duration,
            channels=audio.channels,
            encoding=audio.encoding,
            data=audio.data,  # Normalized data would go here
            timestamp=audio.timestamp,
            format=audio.format,
            metadata=audio.metadata
        )
        
        return normalized_audio


# Sensory input factory functions
def createVision(width: int, height: int, channels: int = 3, 
                color_space: str = "RGB", data: Optional[List[float]] = None,
                timestamp: float = 0.0) -> Vision:
    """Create a Vision sensory input.
    
    Args:
        width: Image width in pixels
        height: Image height in pixels
        channels: Number of color channels
        color_space: Color space
        data: Raw image data
        timestamp: Timestamp
        
    Returns:
        Vision object
    """
    return Vision(
        width=width,
        height=height,
        channels=channels,
        color_space=color_space,
        data=data or [0.0] * (width * height * channels),
        timestamp=timestamp
    )


def createAudio(sample_rate: int, duration: float, channels: int = 1,
                encoding: str = "pcm", data: Optional[List[float]] = None,
                timestamp: float = 0.0) -> Audio:
    """Create an Audio sensory input.
    
    Args:
        sample_rate: Sampling rate in Hz
        duration: Audio duration in seconds
        channels: Number of audio channels
        encoding: Audio encoding format
        data: Raw audio data
        timestamp: Timestamp
        
    Returns:
        Audio object
    """
    return Audio(
        sample_rate=sample_rate,
        duration=duration,
        channels=channels,
        encoding=encoding,
        data=data or [0.0] * int(sample_rate * duration * channels),
        timestamp=timestamp
    )


def createInternalSignals(data: List[float], signal_type: str, signal_name: str,
                         timestamp: float = 0.0) -> InternalSignals:
    """Create internal signals sensory input.
    
    Args:
        data: Signal data values
        signal_type: Type of signal
        signal_name: Name of the signal
        timestamp: Timestamp
        
    Returns:
        InternalSignals object
    """
    return InternalSignals(
        data=data,
        signal_type=signal_type,
        signal_name=signal_name,
        timestamp=timestamp
    )


def createProprioception(body_part: str, sensor_type: str, data: List[float],
                         timestamp: float = 0.0) -> Proprioception:
    """Create proprioception sensory input.
    
    Args:
        body_part: Body part being sensed
        sensor_type: Type of sensor
        data: Proprioceptive data (position, orientation, etc.)
        timestamp: Timestamp
        
    Returns:
        Proprioception object
    """
    return Proprioception(
        body_part=body_part,
        sensor_type=sensor_type,
        data=data,
        timestamp=timestamp
    )
