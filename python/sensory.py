#!/usr/bin/env python3
"""
Sensory module for NLM (Neural Learning Machine).

This module provides sensory processing classes for vision, audio, and internal
signals that allow the NLM agent to perceive its environment.
"""

from typing import Dict, Any, Optional

class SensoryInput:
    """
    Base class for sensory input.

    This abstract base class defines the interface for all types of
    sensory input that agents can receive from their environment.
    """

    def __init__(self, source_id: str = ""):
        """
        Initialize sensory input.

        Args:
            source_id: ID of the sensory source (e.g., camera, microphone)
        """
        self.source_id = source_id
        self.timestamp = 0.0
        self.quality = 1.0  # 0.0 to 1.0, indicates signal quality

    def get_timestamp(self) -> float:
        """Get the timestamp of this sensory input."""
        return self.timestamp

    def get_quality(self) -> float:
        """Get the quality of this sensory input."""
        return self.quality

    def set_quality(self, quality: float) -> None:
        """Set the quality of this sensory input."""
        self.quality = max(0.0, min(1.0, quality))

    def process(self) -> Dict[str, Any]:
        """
        Process the sensory input.

        Returns:
            Processed sensory data as dictionary
        """
        return self._process_internal()

    def _process_internal(self) -> Dict[str, Any]:
        """Internal processing method for subclasses."""
        return {}

    def __str__(self) -> str:
        """String representation."""
        return f"{self.__class__.__name__}(source='{self.source_id}')"
class Vision(SensoryInput):
    """
    Vision sensory input representing visual information.

    This class captures visual information from cameras or other
    vision sensors, including object detection, scene analysis, etc.
    """

    def __init__(self, source_id: str = "camera", width: int = 640, height: int = 480):
        """
        Initialize vision sensor.

        Args:
            source_id: ID of the vision sensor
            width: Image width in pixels
            height: Image height in pixels
        """
        super().__init__(source_id)
        self.width = width
        self.height = height
        self.resolution = (width, height)
        self.objects_detected: Dict[str, Dict[str, Any]] = {}
        self.scene_description: Optional[str] = None

    def add_object(self, obj_id: str, properties: Dict[str, Any]) -> None:
        """
        Add a detected object to vision data.

        Args:
            obj_id: Unique identifier for the object
            properties: Dictionary of object properties (position, size, etc.)
        """
        self.objects_detected[obj_id] = properties.copy()

    def remove_object(self, obj_id: str) -> bool:
        """
        Remove an object from vision data.

        Args:
            obj_id: ID of object to remove

        Returns:
            True if object was removed, False otherwise
        """
        if obj_id in self.objects_detected:
            del self.objects_detected[obj_id]
            return True
        return False

    def get_objects_by_type(self, object_type: str) -> Dict[str, Dict[str, Any]]:
        """
        Get all detected objects of a specific type.

        Args:
            object_type: Type of objects to filter by

        Returns:
            Dictionary of objects of the specified type
        """
        return {obj_id: props for obj_id, props in self.objects_detected.items()
                if props.get('type') == object_type}

    def get_closest_object(self) -> Optional[Dict[str, Any]]:
        """Get the closest detected object."""
        if not self.objects_detected:
            return None

        closest_id = min(self.objects_detected.keys(),
                        key=lambda obj_id: self.objects_detected[obj_id].get('distance', float('inf')))
        return self.objects_detected[closest_id]

    def set_scene_description(self, description: str) -> None:
        """
        Set a textual description of the scene.

        Args:
            description: Scene description
        """
        self.scene_description = description

    def _process_internal(self) -> Dict[str, Any]:
        """Process vision data."""
        result = {
            'width': self.width,
            'height': self.height,
            'object_count': len(self.objects_detected),
            'scene_description': self.scene_description,
        }

        # Add summary statistics
        if self.objects_detected:
            types = set()
            for props in self.objects_detected.values():
                types.add(props.get('type', 'unknown'))
            result['detected_types'] = list(types)

        return result

    def __str__(self) -> str:
        """String representation."""
        return f"Vision(source='{self.source_id}', resolution={self.resolution}, objects={len(self.objects_detected)})"
class Audio(SensoryInput):
    """
    Audio sensory input representing sound information.

    This class captures audio information from microphones or other
    audio sensors, including sound sources, volume levels, and localization.
    """

    def __init__(self, source_id: str = "microphone", sample_rate: int = 44100):
        """
        Initialize audio sensor.

        Args:
            source_id: ID of the audio sensor
            sample_rate: Audio sample rate in Hz
        """
        super().__init__(source_id)
        self.sample_rate = sample_rate
        self.sound_sources: Dict[str, Dict[str, Any]] = {}
        self.noise_level = 0.0
        self.distance_to_speaker = 0.0

    def add_sound_source(self, source_id: str, properties: Dict[str, Any]) -> None:
        """
        Add a sound source to audio data.

        Args:
            source_id: Unique identifier for the sound source
            properties: Dictionary of sound source properties
        """
        self.sound_sources[source_id] = properties.copy()

    def remove_sound_source(self, source_id: str) -> bool:
        """
        Remove a sound source from audio data.

        Args:
            source_id: ID of sound source to remove

        Returns:
            True if sound source was removed, False otherwise
        """
        if source_id in self.sound_sources:
            del self.sound_sources[source_id]
            return True
        return False

    def get_loudest_source(self) -> Optional[Dict[str, Any]]:
        """Get the loudest sound source."""
        if not self.sound_sources:
            return None

        loudest_id = max(self.sound_sources.keys(),
                        key=lambda src_id: self.sound_sources[src_id].get('volume', 0.0))
        return self.sound_sources[loudest_id]

    def set_noise_level(self, noise: float) -> None:
        """
        Set background noise level.

        Args:
            noise: Noise level (0.0 to 1.0)
        """
        self.noise_level = max(0.0, min(1.0, noise))

    def _process_internal(self) -> Dict[str, Any]:
        """Process audio data."""
        result = {
            'sample_rate': self.sample_rate,
            'source_count': len(self.sound_sources),
            'noise_level': self.noise_level,
            'distance_to_speaker': self.distance_to_speaker,
        }

        # Add source information
        if self.sound_sources:
            sources = []
            for src_id, props in self.sound_sources.items():
                sources.append({
                    'id': src_id,
                    'volume': props.get('volume', 0.0),
                    'type': props.get('type', 'unknown'),
                    'location': props.get('location', (0, 0, 0)),
                })
            result['sources'] = sources

        return result

    def __str__(self) -> str:
        """String representation."""
        return f"Audio(source='{self.source_id}', sample_rate={self.sample_rate}, sources={len(self.sound_sources)})"
class InternalSignals(SensoryInput):
    """
    Internal signals sensory input representing agent's internal state.

    This class captures the agent's internal state including neural activity,
    neuromodulation levels, and other internal measurements.
    """

    def __init__(self, source_id: str = "internal_sensor"):
        """
        Initialize internal signals sensor.

        Args:
            source_id: ID of the internal sensor
        """
        super().__init__(source_id)
        self.neural_activity: Dict[str, float] = {}
        self.mood = 0.0  # -1.0 to 1.0
        self.energy = 0.0  # 0.0 to 1.0
        self.focus = 0.0  # 0.0 to 1.0
        self.confidence = 0.0  # 0.0 to 1.0

    def set_neural_activity(self, neuron_type: str, activity: float) -> None:
        """
        Set neural activity level for a neuron type.

        Args:
            neuron_type: Type of neuron (e.g., "excitatory", "inhibitory")
            activity: Neural activity level (0.0 to 1.0)
        """
        self.neural_activity[neuron_type] = max(0.0, min(1.0, activity))

    def get_neural_activity(self, neuron_type: str) -> float:
        """
        Get neural activity level for a neuron type.

        Args:
            neuron_type: Type of neuron

        Returns:
            Neural activity level or 0.0 if not found
        """
        return self.neural_activity.get(neuron_type, 0.0)

    def set_mood(self, mood: float) -> None:
        """
        Set agent mood.

        Args:
            mood: Mood level (-1.0 to 1.0, negative = unhappy, positive = happy)
        """
        self.mood = max(-1.0, min(1.0, mood))

    def set_energy(self, energy: float) -> None:
        """
        Set agent energy level.

        Args:
            energy: Energy level (0.0 to 1.0)
        """
        self.energy = max(0.0, min(1.0, energy))

    def set_focus(self, focus: float) -> None:
        """
        Set agent focus level.

        Args:
            focus: Focus level (0.0 to 1.0)
        """
        self.focus = max(0.0, min(1.0, focus))

    def set_confidence(self, confidence: float) -> None:
        """
        Set agent confidence level.

        Args:
            confidence: Confidence level (0.0 to 1.0)
        """
        self.confidence = max(0.0, min(1.0, confidence))

    def _process_internal(self) -> Dict[str, Any]:
        """Process internal signals data."""
        result = {
            'neural_activity': self.neural_activity.copy(),
            'mood': self.mood,
            'energy': self.energy,
            'focus': self.focus,
            'confidence': self.confidence,
        }

        # Add overall state
        overall_stimulation = sum(self.neural_activity.values()) / max(len(self.neural_activity), 1)
        result['overall_stimulation'] = overall_stimulation

        return result

    def __str__(self) -> str:
        """String representation."""
        return (f"InternalSignals(source='{self.source_id}', "
                f"mood={self.mood:.2f}, energy={self.energy:.2f}, "
                f"focus={self.focus:.2f})")