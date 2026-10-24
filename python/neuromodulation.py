"""
Neuromodulation module - Neuromodulation systems for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import time
import math

from .pynlm import (
    Neuromodulator as NeuromodulatorCore, Dopamine as DopamineCore, 
    Reward as RewardCore, Novelty as NoveltyCore, Curiosity as CuriosityCore,
    Brain as BrainCore, Synapse as SynapseCore, Neuron as NeuronCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    MembranePotential, SynapticWeight, FiringRate, FiringState, NeuronType,
    RegionId, PopulationId, ActionType, ObservationData
)

__all__ = [
    'Neuromodulator', 'Dopamine', 'Reward', 'Novelty', 'Curiosity',
    'Brain', 'Synapse', 'Neuron',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
]

class Neuromodulator:
    """Abstract base class for neuromodulatory signals"""
    
    def __init__(self):
        """Initialize neuromodulator"""
        self.level = 0.0
        self.baseline = 0.0
        self.timestamp = time.time()
    
    def get_name(self) -> str:
        """Get modulator name
        
        Returns:
            Modulator name
        """
        return self.__class__.__name__
    
    def get_level(self) -> float:
        """Get current level
        
        Returns:
            Current level
        """
        return self.level
    
    def set_level(self, level: float):
        """Set current level
        
        Args:
            level: Current level
        """
        self.level = max(0.0, min(1.0, level))
        self.timestamp = time.time()
    
    def get_plasticity_factor(self) -> float:
        """Get plasticity factor
        
        Returns:
            Plasticity factor
        """
        # Base implementation
        return 1.0 + self.level
    
    def update(self, dt: TimestepDuration):
        """Update neuromodulator state
        
        Args:
            dt: Time step
        """
        # Decay towards baseline
        self.level = max(0.0, self.level - 0.1 * dt)
        self.timestamp = time.time()

class Dopamine(Neuromodulator):
    """Dopamine: Reward and reinforcement learning signal"""
    
    def __init__(self):
        """Initialize dopamine
        """
        super().__init__()
        self.baseline = 0.0
        self.peak = 1.0
        self.decay_rate = 0.1
        self.release_rate = 1.0
        self.reward_history = []
        self.error_history = []
        self.timestamp = time.time()
    
    def signal_reward(self, reward: float):
        """Signal reward
        
        Args:
            reward: Reward value
        """
        self.level = min(self.peak, self.level + reward * self.release_rate)
        self.reward_history.append((time.time(), reward))
        self.timestamp = time.time()
    
    def signal_reward_prediction_error(self, error: float):
        """Signal reward prediction error
        
        Args:
            error: Prediction error
        """
        self.level = max(0.0, self.level + error * self.release_rate)
        self.error_history.append((time.time(), error))
        self.timestamp = time.time()
    
    def get_plasticity_factor(self) -> float:
        """Get dopamine-modulated plasticity factor
        
        Returns:
            Plasticity factor
        """
        # Dopamine increases plasticity for positive prediction errors
        # and decreases plasticity for negative prediction errors
        return 0.5 + 0.5 * self.level
    
    def get_reward_history(self) -> List[Tuple[float, float]]:
        """Get reward history
        
        Returns:
            Reward history
        """
        return self.reward_history.copy()
    
    def get_error_history(self) -> List[Tuple[float, float]]:
        """Get error history
        
        Returns:
            Error history
        """
        return self.error_history.copy()
    
    def get_recent_rewards(self, seconds: float = 60.0) -> List[Tuple[float, float]]:
        """Get recent rewards
        
        Args:
            seconds: Time window in seconds
            
        Returns:
            Recent rewards
        """
        current_time = time.time()
        return [(t, r) for t, r in self.reward_history if current_time - t <= seconds]
    
    def get_average_reward(self, seconds: float = 60.0) -> float:
        """Get average reward over time window
        
        Args:
            seconds: Time window in seconds
            
        Returns:
            Average reward
        """
        recent_rewards = self.get_recent_rewards(seconds)
        if not recent_rewards:
            return 0.0
        
        return sum(r for _, r in recent_rewards) / len(recent_rewards)

class Acetylcholine(Neuromodulator):
    """Acetylcholine: Attention and memory consolidation"""
    
    def __init__(self):
        """Initialize acetylcholine
        """
        super().__init__()
        self.baseline = 0.0
        self.decay_rate = 0.05
        self.release_rate = 0.5
        self.attention_history = []
    
    def get_plasticity_factor(self) -> float:
        """Get acetylcholine-modulated plasticity factor
        
        Returns:
            Plasticity factor
        """
        # Acetylcholine enhances plasticity
        return 1.0 + self.level * 0.5
    
    def signal_attention(self, attention: float):
        """Signal attention
        
        Args:
            attention: Attention level
        """
        self.level = min(1.0, self.level + attention * self.release_rate)
        self.attention_history.append((time.time(), attention))
        self.timestamp = time.time()

class Norepinephrine(Neuromodulator):
    """Norepinephrine: Arousal and vigilance"""
    
    def __init__(self):
        """Initialize norepinephrine
        """
        super().__init__()
        self.baseline = 0.0
        self.decay_rate = 0.08
        self.release_rate = 0.3
        self.arousal_history = []
    
    def get_plasticity_factor(self) -> float:
        """Get norepinephrine-modulated plasticity factor
        
        Returns:
            Plasticity factor
        """
        # Norepinephrine has complex effects on plasticity
        return 1.0 + self.level * 0.3
    
    def signal_arousal(self, arousal: float):
        """Signal arousal
        
        Args:
            arousal: Arousal level
        """
        self.level = min(1.0, self.level + arousal * self.release_rate)
        self.arousal_history.append((time.time(), arousal))
        self.timestamp = time.time()

class Serotonin(Neuromodulator):
    """Serotonin: Mood, impulsivity, and social behavior"""
    
    def __init__(self):
        """Initialize serotonin
        """
        super().__init__()
        self.baseline = 0.0
        self.decay_rate = 0.03
        self.release_rate = 0.2
        self.mood_history = []
    
    def get_plasticity_factor(self) -> float:
        """Get serotonin-modulated plasticity factor
        
        Returns:
            Plasticity factor
        """
        # Serotonin modulates plasticity based on mood
        if self.level > 0.5:
            return 1.0 - self.level * 0.5  # High serotonin reduces plasticity
        else:
            return 1.0 + self.level * 0.3  # Low serotonin increases plasticity

class Reward:
    """Reward signal for reinforcement learning"""
    
    def __init__(self, discount_factor: float = 0.99):
        """Initialize reward
        
        Args:
            discount_factor: Discount factor for future rewards
        """
        self.value = 0.0
        self.accumulated = 0.0
        self.history = []
        self.discount_factor = discount_factor
        self.timestamp = time.time()
    
    def get_value(self) -> float:
        """Get current reward value
        
        Returns:
            Current reward value
        """
        return self.value
    
    def set_value(self, value: float):
        """Set current reward value
        
        Args:
            value: Current reward value
        """
        self.value = value
        self.timestamp = time.time()
    
    def add(self, delta: float):
        """Add to reward
        
        Args:
            delta: Reward change
        """
        self.value += delta
        self.accumulated += delta
        self.history.append((time.time(), delta))
        self.timestamp = time.time()
    
    def reset(self):
        """Reset accumulated reward"""
        self.value = 0.0
        self.accumulated = 0.0
        self.history.clear()
        self.timestamp = time.time()
    
    def compute(self, observation: Optional[ObservationData] = None) -> float:
        """Compute reward from observation
        
        Args:
            observation: Optional observation
            
        Returns:
            Computed reward
        """
        # Simple reward computation based on observation
        if observation is not None:
            data = observation.getData()
            if data:
                # Sum positive features
                reward = sum(max(x, 0.0) for x in data) * 0.1
                self.add(reward)
                return reward
        
        return 0.0
    
    def get_history(self) -> List[Tuple[float, float]]:
        """Get reward history
        
        Returns:
            Reward history
        """
        return self.history.copy()
    
    def get_accumulated(self) -> float:
        """Get accumulated reward
        
        Returns:
            Accumulated reward
        """
        return self.accumulated
    
    def clear_history(self):
        """Clear reward history"""
        self.history.clear()
        self.timestamp = time.time()
    
    def get_timestamp(self) -> float:
        """Get last update timestamp
        
        Returns:
            Timestamp
        """
        return self.timestamp

# Convenience functions
def create_dopamine() -> Dopamine:
    """Create dopamine neuromodulator
    
    Returns:
        Dopamine instance
    """
    return Dopamine()

def create_acetylcholine() -> Acetylcholine:
    """Create acetylcholine neuromodulator
    
    Returns:
        Acetylcholine instance
    """
    return Acetylcholine()

def create_norepinephrine() -> Norepinephrine:
    """Create norepinephrine neuromodulator
    
    Returns:
        Norepinephrine instance
    """
    return Norepinephrine()

def create_serotonin() -> Serotonin:
    """Create serotonin neuromodulator
    
    Returns:
        Serotonin instance
    """
    return Serotonin()

def create_reward(discount_factor: float = 0.99) -> Reward:
    """Create reward system
    
    Args:
        discount_factor: Discount factor
        
    Returns:
        Reward instance
    """
    return Reward(discount_factor)

class NeuromodulationUtils:
    """Utility functions for neuromodulation operations"""
    
    @staticmethod
    def create_dopamine() -> Dopamine:
        """Create dopamine neuromodulator
        
        Returns:
            Dopamine instance
        """
        return Dopamine()
    
    @staticmethod
    def create_acetylcholine() -> Acetylcholine:
        """Create acetylcholine neuromodulator
        
        Returns:
            Acetylcholine instance
        """
        return Acetylcholine()
    
    @staticmethod
    def create_norepinephrine() -> Norepinephrine:
        """Create norepinephrine neuromodulator
        
        Returns:
            Norepinephrine instance
        """
        return Norepinephrine()
    
    @staticmethod
    def create_serotonin() -> Serotonin:
        """Create serotonin neuromodulator
        
        Returns:
            Serotonin instance
        """
        return Serotonin()
    
    @staticmethod
    def create_reward(discount_factor: float = 0.99) -> Reward:
        """Create reward system
        
        Args:
            discount_factor: Discount factor
            
        Returns:
            Reward instance
        """
        return Reward(discount_factor)
    
    @staticmethod
    def calculate_neuromodulation_stats(neuromodulators: List[Neuromodulator]) -> Dict[str, Any]:
        """Calculate neuromodulation statistics
        
        Args:
            neuromodulators: List of neuromodulators
            
        Returns:
            Statistics dictionary
        """
        stats = {
            'total_neuromodulators': len(neuromodulators),
            'average_level': sum(nm.get_level() for nm in neuromodulators) / max(len(neuromodulators), 1),
            'modulator_names': [nm.get_name() for nm in neuromodulators],
            'plasticity_factors': [nm.get_plasticity_factor() for nm in neuromodulators]
        }
        
        return stats
    
    @staticmethod
    def activate_all_neuromodulators(neuromodulators: List[Neuromodulator]):
        """Activate all neuromodulators
        
        Args:
            neuromodulators: List of neuromodulators
        """
        for nm in neuromodulators:
            nm.set_level(1.0)
    
    @staticmethod
    def deactivate_all_neuromodulators(neuromodulators: List[Neuromodulator]):
        """Deactivate all neuromodulators
        
        Args:
            neuromodulators: List of neuromodulators
        """
        for nm in neuromodulators:
            nm.set_level(0.0)
