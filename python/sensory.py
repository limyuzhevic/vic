"""
Sensory module - Sensory processing functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import math
import time

from .pynlm import (
    Vision as VisionCore, Audio as AudioCore, InternalSignals as InternalSignalsCore,
    SensoryInput as SensoryInputCore, Sensor as SensorCore, Perception as PerceptionCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    MembranePotential, SynapticWeight, FiringRate, FiringState, NeuronType,
    RegionId, PopulationId, ActionType, ObservationData
)

__all__ = [
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Sensor', 'Perception',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
]

class Vision:
    """Vision sensory system for processing visual information"""
    
    def __init__(self, resolution: float = 0.1, range: float = 100.0):
        """Initialize vision system
        
        Args:
            resolution: Vision resolution
            range: Vision range
        """
        self.resolution = resolution
        self.range = range
        self.features = []
        self.timestamp = time.time()
    
    def process(self, input_data: ObservationData) -> Dict[str, float]:
        """Process visual input
        
        Args:
            input_data: Visual input data
            
        Returns:
            Processed features
        """
        self.features = []
        self.timestamp = time.time()
        
        # Process input data
        if input_data.size() > 0:
            data = input_data.getData()
            
            # Extract visual features
            features = {}
            
            # Basic statistics
            features['mean'] = sum(data) / len(data)
            features['std'] = (sum((x - features['mean']) ** 2 for x in data) / len(data)) ** 0.5
            features['min'] = min(data)
            features['max'] = max(data)
            features['range'] = features['max'] - features['min']
            
            # Texture features
            features['contrast'] = self._calculate_contrast(data)
            features['roughness'] = self._calculate_roughness(data)
            features['smoothness'] = self._calculate_smoothness(data)
            
            # Edge features
            features['edge_density'] = self._calculate_edge_density(data)
            features['edge_strength'] = self._calculate_edge_strength(data)
            
            # Motion features
            features['motion_energy'] = self._calculate_motion_energy(data)
            
            # Shape features
            features['compactness'] = self._calculate_compactness(data)
            features['convexity'] = self._calculate_convexity(data)
            
            # Color features
            features['brightness'] = self._calculate_brightness(data)
            features['saturation'] = self._calculate_saturation(data)
            
            # Add to features list
            self.features.extend(features.values())
        
        return self.features.copy()
    
    def _calculate_contrast(self, data: List[float]) -> float:
        """Calculate visual contrast
        
        Args:
            data: Input data
            
        Returns:
            Contrast value
        """
        if len(data) < 2:
            return 0.0
        
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        return math.sqrt(variance)
    
    def _calculate_roughness(self, data: List[float]) -> float:
        """Calculate visual roughness
        
        Args:
            data: Input data
            
        Returns:
            Roughness value
        """
        # Simple roughness calculation based on high-frequency components
        if len(data) < 3:
            return 0.0
        
        # Calculate differences between adjacent pixels
        differences = [abs(data[i] - data[i-1]) for i in range(1, len(data))]
        
        # Roughness is average of differences
        return sum(differences) / len(differences)
    
    def _calculate_smoothness(self, data: List[float]) -> float:
        """Calculate visual smoothness
        
        Args:
            data: Input data
            
        Returns:
            Smoothness value
        """
        return 1.0 - self._calculate_roughness(data)
    
    def _calculate_edge_density(self, data: List[float]) -> float:
        """Calculate edge density
        
        Args:
            data: Input data
            
        Returns:
            Edge density value
        """
        if len(data) < 2:
            return 0.0
        
        # Simple edge detection
        edges = [1 for i in range(1, len(data)) if abs(data[i] - data[i-1]) > 0.5]
        return len(edges) / (len(data) - 1)
    
    def _calculate_edge_strength(self, data: List[float]) -> float:
        """Calculate edge strength
        
        Args:
            data: Input data
            
        Returns:
            Edge strength value
        """
        if len(data) < 2:
            return 0.0
        
        # Calculate total edge strength
        edge_strength = sum(abs(data[i] - data[i-1]) for i in range(1, len(data)))
        
        return edge_strength / (len(data) - 1)
    
    def _calculate_motion_energy(self, data: List[float]) -> float:
        """Calculate motion energy
        
        Args:
            data: Input data
            
        Returns:
            Motion energy value
        """
        if len(data) < 2:
            return 0.0
        
        # Calculate temporal differences
        motion_energy = sum((data[i] - data[i-1]) ** 2 for i in range(1, len(data)))
        
        return motion_energy / (len(data) - 1)
    
    def _calculate_compactness(self, data: List[float]) -> float:
        """Calculate visual compactness
        
        Args:
            data: Input data
            
        Returns:
            Compactness value
        """
        # Compactness based on variance
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        
        return 1.0 / (1.0 + variance)
    
    def _calculate_convexity(self, data: List[float]) -> float:
        """Calculate visual convexity
        
        Args:
            data: Input data
            
        Returns:
            Convexity value
        """
        # Simple convexity calculation
        return 0.5  # Placeholder
    
    def _calculate_brightness(self, data: List[float]) -> float:
        """Calculate visual brightness
        
        Args:
            data: Input data
            
        Returns:
            Brightness value
        """
        return sum(data) / len(data)
    
    def _calculate_saturation(self, data: List[float]) -> float:
        """Calculate visual saturation
        
        Args:
            data: Input data
            
        Returns:
            Saturation value
        """
        # Saturation based on contrast
        contrast = self._calculate_contrast(data)
        return min(contrast, 1.0)
    
    def get_features(self) -> List[float]:
        """Get processed features
        
        Returns:
            List of features
        """
        return self.features.copy()
    
    def get_feature(self, index: int) -> float:
        """Get specific feature
        
        Args:
            index: Feature index
            
        Returns:
            Feature value
        """
        if index < len(self.features):
            return self.features[index]
        return 0.0

class Audio:
    """Audio sensory system for processing sound information"""
    
    def __init__(self, sampling_rate: float = 44100.0, range: float = 50.0):
        """Initialize audio system
        
        Args:
            sampling_rate: Audio sampling rate
            range: Audio range
        """
        self.sampling_rate = sampling_rate
        self.range = range
        self.features = []
        self.timestamp = time.time()
    
    def process(self, input_data: ObservationData) -> Dict[str, float]:
        """Process audio input
        
        Args:
            input_data: Audio input data
            
        Returns:
            Processed features
        """
        self.features = []
        self.timestamp = time.time()
        
        if input_data.size() > 0:
            data = input_data.getData()
            
            # Extract audio features
            features = {}
            
            # Basic statistics
            features['mean'] = sum(data) / len(data)
            features['std'] = (sum((x - features['mean']) ** 2 for x in data) / len(data)) ** 0.5
            features['min'] = min(data)
            features['max'] = max(data)
            features['range'] = features['max'] - features['min']
            
            # Frequency features
            features['dominant_frequency'] = self._calculate_dominant_frequency(data)
            features['spectral_centroid'] = self._calculate_spectral_centroid(data)
            features['spectral_spread'] = self._calculate_spectral_spread(data)
            
            # Temporal features
            features['tempo'] = self._calculate_tempo(data)
            features['beat_strength'] = self._calculate_beat_strength(data)
            
            # Harmonic features
            features['harmonic_content'] = self._calculate_harmonic_content(data)
            features[' inharmonicity'] = self._calculate_inharmonicity(data)
            
            # Noise features
            features['noise_floor'] = self._calculate_noise_floor(data)
            features['noise_ratio'] = self._calculate_noise_ratio(data)
            
            # Add to features list
            self.features.extend(features.values())
        
        return self.features.copy()
    
    def _calculate_dominant_frequency(self, data: List[float]) -> float:
        """Calculate dominant frequency
        
        Args:
            data: Input data
            
        Returns:
            Dominant frequency
        """
        # Simple FFT approximation
        if len(data) < 2:
            return 0.0
        
        # Calculate zero-crossing rate
        zero_crossings = sum(1 for i in range(1, len(data)) if data[i-1] * data[i] < 0)
        
        if zero_crossings < 2:
            return 0.0
        
        return (zero_crossings / 2) / (len(data) / self.sampling_rate)
    
    def _calculate_spectral_centroid(self, data: List[float]) -> float:
        """Calculate spectral centroid
        
        Args:
            data: Input data
            
        Returns:
            Spectral centroid
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_spectral_spread(self, data: List[float]) -> float:
        """Calculate spectral spread
        
        Args:
            data: Input data
            
        Returns:
            Spectral spread
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_tempo(self, data: List[float]) -> float:
        """Calculate tempo
        
        Args:
            data: Input data
            
        Returns:
            Tempo
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_beat_strength(self, data: List[float]) -> float:
        """Calculate beat strength
        
        Args:
            data: Input data
            
        Returns:
            Beat strength
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_harmonic_content(self, data: List[float]) -> float:
        """Calculate harmonic content
        
        Args:
            data: Input data
            
        Returns:
            Harmonic content
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_inharmonicity(self, data: List[float]) -> float:
        """Calculate inharmonicity
        
        Args:
            data: Input data
            
        Returns:
            Inharmonicity
        """
        # Placeholder implementation
        return 0.0
    
    def _calculate_noise_floor(self, data: List[float]) -> float:
        """Calculate noise floor
        
        Args:
            data: Input data
            
        Returns:
            Noise floor
        """
        # Calculate minimum absolute value
        if not data:
            return 0.0
        
        abs_values = [abs(x) for x in data]
        return min(abs_values)
    
    def _calculate_noise_ratio(self, data: List[float]) -> float:
        """Calculate noise ratio
        
        Args:
            data: Input data
            
        Returns:
            Noise ratio
        """
        if len(data) < 2:
            return 0.0
        
        # Calculate signal-to-noise ratio approximation
        mean = sum(data) / len(data)
        noise = sum((x - mean) ** 2 for x in data) / len(data)
        
        if mean == 0:
            return 0.0
        
        return noise / (mean ** 2)
    
    def get_features(self) -> List[float]:
        """Get processed features
        
        Returns:
            List of features
        """
        return self.features.copy()
    
    def get_feature(self, index: int) -> float:
        """Get specific feature
        
        Args:
            index: Feature index
            
        Returns:
            Feature value
        """
        if index < len(self.features):
            return self.features[index]
        return 0.0

class InternalSignals:
    """Internal signals sensory system for processing internal state information"""
    
    def __init__(self, range: float = 20.0):
        """Initialize internal signals system
        
        Args:
            range: Internal signals range
        """
        self.range = range
        self.features = []
        self.timestamp = time.time()
    
    def process(self, input_data: ObservationData) -> Dict[str, float]:
        """Process internal signals input
        
        Args:
            input_data: Internal signals input data
            
        Returns:
            Processed features
        """
        self.features = []
        self.timestamp = time.time()
        
        if input_data.size() > 0:
            data = input_data.getData()
            
            # Extract internal signals features
            features = {}
            
            # Basic statistics
            features['mean'] = sum(data) / len(data)
            features['std'] = (sum((x - features['mean']) ** 2 for x in data) / len(data)) ** 0.5
            features['min'] = min(data)
            features['max'] = max(data)
            features['range'] = features['max'] - features['min']
            
            # Homeostatic features
            features['homeostatic_load'] = self._calculate_homeostatic_load(data)
            features['metabolic_state'] = self._calculate_metabolic_state(data)
            
            # Arousal features
            features['arousal_level'] = self._calculate_arousal_level(data)
            features['attention_level'] = self._calculate_attention_level(data)
            
            # Stress features
            features['stress_level'] = self._calculate_stress_level(data)
            features['fatigue_level'] = self._calculate_fatigue_level(data)
            
            # Add to features list
            self.features.extend(features.values())
        
        return self.features.copy()
    
    def _calculate_homeostatic_load(self, data: List[float]) -> float:
        """Calculate homeostatic load
        
        Args:
            data: Input data
            
        Returns:
            Homeostatic load
        """
        # Normalize to range [0, 1]
        if not data:
            return 0.0
        
        # Calculate normalized sum
        normalized_sum = sum(min(x, 1.0) for x in data) / len(data)
        
        return normalized_sum
    
    def _calculate_metabolic_state(self, data: List[float]) -> float:
        """Calculate metabolic state
        
        Args:
            data: Input data
            
        Returns:
            Metabolic state
        """
        # Calculate mean energy level
        if not data:
            return 0.0
        
        return sum(data) / len(data)
    
    def _calculate_arousal_level(self, data: List[float]) -> float:
        """Calculate arousal level
        
        Args:
            data: Input data
            
        Returns:
            Arousal level
        """
        # Calculate arousal based on variance
        if len(data) < 2:
            return 0.0
        
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        
        # Normalize arousal to [0, 1]
        return min(variance, 1.0)
    
    def _calculate_attention_level(self, data: List[float]) -> float:
        """Calculate attention level
        
        Args:
            data: Input data
            
        Returns:
            Attention level
        """
        # Calculate attention based on peaks
        if len(data) < 2:
            return 0.0
        
        # Count local maxima
        peaks = 0
        for i in range(1, len(data) - 1):
            if data[i-1] < data[i] and data[i] > data[i+1]:
                peaks += 1
        
        return min(peaks / 5.0, 1.0)  # Cap at 1.0
    
    def _calculate_stress_level(self, data: List[float]) -> float:
        """Calculate stress level
        
        Args:
            data: Input data
            
        Returns:
            Stress level
        """
        # Calculate stress based on negative values
        negative_values = [min(x, 0.0) for x in data]
        
        if not data:
            return 0.0
        
        stress = sum(abs(x) for x in negative_values) / sum(abs(x) for x in data)
        return stress
    
    def _calculate_fatigue_level(self, data: List[float]) -> float:
        """Calculate fatigue level
        
        Args:
            data: Input data
            
        Returns:
            Fatigue level
        """
        # Calculate fatigue based on sustained low values
        if len(data) < 3:
            return 0.0
        
        # Look for sustained low values
        sustained_low = 0
        for i in range(len(data) - 2):
            if data[i] < 0.3 and data[i+1] < 0.3 and data[i+2] < 0.3:
                sustained_low += 1
        
        return min(sustained_low / len(data), 1.0)
    
    def get_features(self) -> List[float]:
        """Get processed features
        
        Returns:
            List of features
        """
        return self.features.copy()
    
    def get_feature(self, index: int) -> float:
        """Get specific feature
        
        Args:
            index: Feature index
            
        Returns:
            Feature value
        """
        if index < len(self.features):
            return self.features[index]
        return 0.0

class SensoryInput:
    """Sensory input combining multiple sensory modalities"""
    
    def __init__(self):
        """Initialize sensory input"""
        self.vision = Vision()
        self.audio = Audio()
        self.internal_signals = InternalSignals()
        self.timestamp = time.time()
    
    def update(self, vision_data: Optional[ObservationData] = None,
               audio_data: Optional[ObservationData] = None,
               internal_data: Optional[ObservationData] = None):
        """Update sensory input
        
        Args:
            vision_data: Vision input data
            audio_data: Audio input data
            internal_data: Internal signals data
        """
        self.timestamp = time.time()
        
        if vision_data is not None:
            self.vision.process(vision_data)
        
        if audio_data is not None:
            self.audio.process(audio_data)
        
        if internal_data is not None:
            self.internal_signals.process(internal_data)
    
    def get_vision_features(self) -> List[float]:
        """Get vision features
        
        Returns:
            Vision features
        """
        return self.vision.get_features()
    
    def get_audio_features(self) -> List[float]:
        """Get audio features
        
        Returns:
            Audio features
        """
        return self.audio.get_features()
    
    def get_internal_features(self) -> List[float]:
        """Get internal signals features
        
        Returns:
            Internal signals features
        """
        return self.internal_signals.get_features()
    
    def get_all_features(self) -> Dict[str, List[float]]:
        """Get all sensory features
        
        Returns:
            Dictionary of all features
        """
        return {
            'vision': self.get_vision_features(),
            'audio': self.get_audio_features(),
            'internal_signals': self.get_internal_features()
        }
    
    def get_feature_vector(self) -> List[float]:
        """Get combined feature vector
        
        Returns:
            Combined feature vector
        """
        all_features = []
        all_features.extend(self.get_vision_features())
        all_features.extend(self.get_audio_features())
        all_features.extend(self.get_internal_features())
        
        return all_features

# Convenience functions
def create_vision(resolution: float = 0.1, range: float = 100.0) -> Vision:
    """Create vision system
    
    Args:
        resolution: Vision resolution
        range: Vision range
        
    Returns:
        Vision instance
    """
    return Vision(resolution, range)

def create_audio(sampling_rate: float = 44100.0, range: float = 50.0) -> Audio:
    """Create audio system
    
    Args:
        sampling_rate: Audio sampling rate
        range: Audio range
        
    Returns:
        Audio instance
    """
    return Audio(sampling_rate, range)

def create_internal_signals(range: float = 20.0) -> InternalSignals:
    """Create internal signals system
    
    Args:
        range: Internal signals range
        
    Returns:
        InternalSignals instance
    """
    return InternalSignals(range)

def create_sensory_input() -> SensoryInput:
    """Create sensory input system
    
    Returns:
        SensoryInput instance
    """
    return SensoryInput()

class SensoryUtils:
    """Utility functions for sensory operations"""
    
    @staticmethod
    def create_vision(resolution: float = 0.1, range: float = 100.0) -> Vision:
        """Create vision system
        
        Args:
            resolution: Vision resolution
            range: Vision range
            
        Returns:
            Vision instance
        """
        return Vision(resolution, range)
    
    @staticmethod
    def create_audio(sampling_rate: float = 44100.0, range: float = 50.0) -> Audio:
        """Create audio system
        
        Args:
            sampling_rate: Audio sampling rate
            range: Audio range
            
        Returns:
            Audio instance
        """
        return Audio(sampling_rate, range)
    
    @staticmethod
    def create_internal_signals(range: float = 20.0) -> InternalSignals:
        """Create internal signals system
        
        Args:
            range: Internal signals range
            
        Returns:
            InternalSignals instance
        """
        return InternalSignals(range)
    
    @staticmethod
    def create_sensory_input() -> SensoryInput:
        """Create sensory input system
        
        Returns:
            SensoryInput instance
        """
        return SensoryInput()
    
    @staticmethod
    def combine_sensory_features(vision_features: List[float], 
                                 audio_features: List[float],
                                 internal_features: List[float]) -> List[float]:
        """Combine sensory features
        
        Args:
            vision_features: Vision features
            audio_features: Audio features
            internal_features: Internal signals features
            
        Returns:
            Combined feature vector
        """
        combined = []
        combined.extend(vision_features)
        combined.extend(audio_features)
        combined.extend(internal_features)
        return combined
    
    @staticmethod
    def extract_feature(combined_features: List[float], feature_type: str, index: int) -> float:
        """Extract specific feature from combined features
        
        Args:
            combined_features: Combined feature vector
            feature_type: Feature type ('vision', 'audio', 'internal')
            index: Feature index within its category
            
        Returns:
            Feature value
        """
        if feature_type == 'vision':
            if index < len(combined_features):
                return combined_features[index]
        elif feature_type == 'audio':
            if index < len(combined_features):
                return combined_features[len(vision_features) + index]
        elif feature_type == 'internal':
            if index < len(combined_features):
                return combined_features[len(vision_features) + len(audio_features) + index]
        
        return 0.0
