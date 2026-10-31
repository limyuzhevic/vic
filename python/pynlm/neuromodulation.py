"""
Neuromodulation module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Union
from enum import Enum

try:
    from pynlm.bindings import AgentBrain, createAgentBrain
    from pynlm.bindings import Reward, PredictionError, Novelty, Curiosity
    from pynlm.bindings import createBrain, createDefaultConfig
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Neuromodulation module limited.")

# Import exceptions and utilities
from .exceptions import NLMInitializationError, NLMConfigurationError
from .core import calculate_statistics


class NeuromodulationLevel(Enum):
    """Neuromodulation level states."""
    MINIMAL = 0.0
    LOW = 0.25
    MODERATE = 0.5
    HIGH = 0.75
    MAXIMUM = 1.0


class NeuromodulationState:
    """Represents current neuromodulation state."""
    
    def __init__(self):
        """Initialize neuromodulation state."""
        self.dopamine = 0.0
        self.curiosity = 0.0
        self.novelty = 0.0
        self.prediction_error = 0.0
        self.reward_prediction = 0.0
        self.baseline_dopamine = 0.0
        self.total_rewards = 0.0
        self.timestamp = time.time()
        
        # Learning rates
        self.dopamine_sensitivity = 1.0
        self.curiosity_scaling = 1.0
        self.novelty_threshold = 0.5
        self.prediction_error_weight = 1.0
    
    def update_dopamine(self, reward: float, prediction: float = 0.0):
        """
        Update dopamine level based on reward and prediction error.
        
        Args:
            reward: Actual reward received
            prediction: Predicted reward
        """
        error = reward - prediction
        self.prediction_error = error
        self.reward_prediction = prediction
        
        # Dopamine response to prediction error
        dopamine_change = self.dopamine_sensitivity * error
        self.dopamine += dopamine_change * 0.1  # Smooth update
        
        # Keep within bounds
        self.dopamine = max(0.0, min(1.0, self.dopamine))
        self.prediction_error = max(0.0, min(1.0, abs(error)))
        
        self.timestamp = time.time()
    
    def update_curiosity(self, novelty: float, exploration_bonus: float = 0.0):
        """
        Update curiosity level based on novelty.
        
        Args:
            novelty: Novelty detection score
            exploration_bonus: Additional exploration drive
        """
        # Curiosity increases with novelty
        curiosity_change = self.curiosity_scaling * novelty * exploration_bonus
        self.curiosity += curiosity_change * 0.05  # Smooth update
        
        # Decay over time
        self.curiosity *= 0.95
        
        # Keep within bounds
        self.curiosity = max(0.0, min(1.0, self.curiosity))
        self.novelty = max(0.0, min(1.0, novelty))
        
        self.timestamp = time.time()
    
    def get_modulation_factor(self, modulator_type: str) -> float:
        """
        Get modulation factor for specific neuromodulator.
        
        Args:
            modulator_type: Type of neuromodulator ('dopamine', 'curiosity', 'novelty')
        
        Returns:
            Modulation factor (0.0 to 1.0)
        """
        if modulator_type == 'dopamine':
            return self.dopamine
        elif modulator_type == 'curiosity':
            return self.curiosity
        elif modulator_type == 'novelty':
            return self.novelty
        else:
            return 0.0
    
    def reset(self):
        """Reset neuromodulation state."""
        self.dopamine = 0.0
        self.curiosity = 0.0
        self.novelty = 0.0
        self.prediction_error = 0.0
        self.reward_prediction = 0.0
        self.total_rewards = 0.0
        self.timestamp = time.time()
    
    def get_summary(self) -> Dict[str, Any]:
        """Get neuromodulation summary."""
        return {
            'dopamine': self.dopamine,
            'curiosity': self.curiosity,
            'novelty': self.novelty,
            'prediction_error': self.prediction_error,
            'reward_prediction': self.reward_prediction,
            'total_rewards': self.total_rewards,
            'timestamp': self.timestamp,
        }
    
    def __repr__(self) -> str:
        return f"NeuromodulationState(dopamine={self.dopamine:.3f}, curiosity={self.curiosity:.3f}, novelty={self.novelty:.3f})"


class RewardProcessor:
    """Process and learn from rewards."""
    
    def __init__(self, initial_value: float = 0.5):
        """
        Initialize reward processor.
        
        Args:
            initial_value: Initial value estimate
        """
        self.value_estimate = initial_value
        self.temporal_difference = 0.0
        self.learning_rate = 0.1
        self.discount_factor = 0.99
        self.reward_history = []
        self.value_history = []
        self.optimistic_value = initial_value * 1.5
        self.pessimistic_value = initial_value * 0.5
    
    def update(self, reward: float, value_estimate: float = None):
        """
        Update value estimate with new reward.
        
        Args:
            reward: Received reward
            value_estimate: Alternative value estimate (optional)
        """
        if value_estimate is None:
            value_estimate = self.value_estimate
        
        # Temporal difference learning
        td_error = reward + self.discount_factor * value_estimate - self.value_estimate
        self.temporal_difference = td_error
        
        # Update value estimate
        self.value_estimate += self.learning_rate * td_error
        
        # Record history
        self.reward_history.append(reward)
        self.value_history.append(self.value_estimate)
        
        # Keep history manageable
        if len(self.reward_history) > 1000:
            self.reward_history = self.reward_history[-500:]
            self.value_history = self.value_history[-500:]
    
    def get_expected_reward(self) -> float:
        """Get expected reward based on current estimate."""
        return self.value_estimate
    
    def get_reward_confidence(self) -> float:
        """Get confidence in reward estimate."""
        if len(self.reward_history) < 2:
            return 0.5
        
        variance = np.var(self.reward_history)
        return max(0.0, min(1.0, 1.0 - variance))
    
    def get_reward_range(self) -> Tuple[float, float]:
        """Get min/max reward range."""
        if not self.reward_history:
            return 0.0, 1.0
        
        return min(self.reward_history), max(self.reward_history)
    
    def reset(self):
        """Reset reward processor."""
        self.value_estimate = 0.5
        self.temporal_difference = 0.0
        self.reward_history.clear()
        self.value_history.clear()
    
    def __repr__(self) -> str:
        return f"RewardProcessor(value={self.value_estimate:.3f}, td={self.temporal_difference:.3f})"


class NoveltyDetector:
    """Detect novel stimuli and experiences."""
    
    def __init__(self, memory_capacity: int = 100):
        """
        Initialize novelty detector.
        
        Args:
            memory_capacity: Number of experiences to remember
        """
        self.memory = []
        self.memory_capacity = memory_capacity
        self.similarity_threshold = 0.7
        self.anomaly_threshold = 2.0
        self.current_novelty = 0.0
        
        # Feature extraction
        self.feature_weights = {
            'frequency': 0.3,
            'intensity': 0.3,
            'pattern': 0.4,
        }
    
    def encode_experience(self, data: List[float]) -> List[float]:
        """
        Encode experience as feature vector.
        
        Args:
            data: Raw data to encode
        
        Returns:
            Feature vector
        """
        # Simple feature extraction
        features = []
        
        # Frequency feature
        if len(data) > 1:
            diffs = [abs(data[i] - data[i-1]) for i in range(1, len(data))]
            freq_feature = np.mean(diffs) if diffs else 0.0
            features.append(freq_feature)
        
        # Intensity feature
        intensity_feature = np.mean([abs(x) for x in data]) if data else 0.0
        features.append(intensity_feature)
        
        # Pattern feature (simple autocorrelation)
        if len(data) > 3:
            autocorr = np.corrcoef(data[:-1], data[1:])[0, 1] if np.std(data[:-1]) > 0 and np.std(data[1:]) > 0 else 0.0
            pattern_feature = abs(autocorr)
            features.append(pattern_feature)
        
        return features
    
    def detect_novelty(self, data: List[float]) -> float:
        """
        Detect novelty of new experience.
        
        Args:
            data: New experience data
        
        Returns:
            Novelty score (0.0 to 1.0)
        """
        if not self.memory:
            self.current_novelty = 1.0
            self._add_to_memory(data)
            return self.current_novelty
        
        # Encode new experience
        new_features = self.encode_experience(data)
        
        # Compare with memory
        similarities = []
        for old_data in self.memory:
            old_features = self.encode_experience(old_data)
            similarity = self._calculate_similarity(new_features, old_features)
            similarities.append(similarity)
        
        # Calculate novelty (1 - average similarity)
        avg_similarity = np.mean(similarities) if similarities else 0.0
        self.current_novelty = max(0.0, min(1.0, 1.0 - avg_similarity))
        
        # Add to memory if novel enough
        if self.current_novelty > self.similarity_threshold:
            self._add_to_memory(data)
        
        return self.current_novelty
    
    def _calculate_similarity(self, features1: List[float], features2: List[float]) -> float:
        """Calculate similarity between feature vectors."""
        if len(features1) != len(features2):
            return 0.0
        
        # Cosine similarity
        dot_product = sum(f1 * f2 for f1, f2 in zip(features1, features2))
        norm1 = np.linalg.norm(features1)
        norm2 = np.linalg.norm(features2)
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
        
        return dot_product / (norm1 * norm2)
    
    def _add_to_memory(self, data: List[float]):
        """Add experience to memory."""
        if len(self.memory) >= self.memory_capacity:
            self.memory.pop(0)
        self.memory.append(data.copy())
    
    def get_novelty_summary(self) -> Dict[str, Any]:
        """Get novelty detector summary."""
        return {
            'current_novelty': self.current_novelty,
            'memory_size': len(self.memory),
            'similarity_threshold': self.similarity_threshold,
            'anomaly_threshold': self.anomaly_threshold,
        }
    
    def reset(self):
        """Reset novelty detector."""
        self.memory.clear()
        self.current_novelty = 0.0
    
    def __repr__(self) -> str:
        return f"NoveltyDetector(memory={len(self.memory)}, current_novelty={self.current_novelty:.3f})"


class CuriositySystem:
    """Curiosity-driven exploration system."""
    
    def __init__(self, baseline_curiosity: float = 0.1):
        """
        Initialize curiosity system.
        
        Args:
            baseline_curiosity: Baseline curiosity level
        """
        self.baseline_curiosity = baseline_curiosity
        self.current_curiosity = baseline_curiosity
        self.exploration_drive = 0.0
        self.interest_level = 0.0
        self.satisfaction_level = 0.0
        self.exploration_history = []
        self.learning_progress = 0.0
        
        # Curiosity dynamics
        self.learning_rate = 0.05
        self.decay_rate = 0.02
        self.satisfaction_threshold = 0.7
        self.threshold_increase_rate = 0.01
    
    def update(self, novelty: float, reward: float, time_step: float = 0.1):
        """
        Update curiosity system.
        
        Args:
            novelty: Novelty level
            reward: Reward received
            time_step: Time step duration
        """
        # Curiosity increases with novelty
        curiosity_boost = novelty * self.interest_level
        self.current_curiosity += curiosity_boost * self.learning_rate
        
        # Curiosity decreases with satisfaction
        satisfaction_decay = (1.0 - self.satisfaction_level) * self.decay_rate
        self.current_curiosity -= satisfaction_decay * time_step
        
        # Learning progress affects curiosity
        self.learning_progress = min(1.0, self.learning_progress + reward * 0.1)
        if self.learning_progress > self.satisfaction_threshold:
            self.exploration_drive = 0.0
            self.satisfaction_level = 1.0
        else:
            self.satisfaction_level = self.learning_progress / self.satisfaction_threshold
            self.exploration_drive = 1.0 - self.satisfaction_level
        
        # Update interest level
        self.interest_level = min(1.0, self.interest_level + curiosity_boost * 0.5)
        
        # Keep within bounds
        self.current_curiosity = max(0.0, min(1.0, self.current_curiosity))
        
        # Record exploration
        self.exploration_history.append({
            'novelty': novelty,
            'reward': reward,
            'curiosity': self.current_curiosity,
            'timestamp': time.time(),
        })
    
    def get_exploration_drive(self) -> float:
        """Get exploration drive level."""
        return self.exploration_drive
    
    def get_curiosity_level(self) -> float:
        """Get current curiosity level."""
        return self.current_curiosity
    
    def should_explore(self, threshold: float = None) -> bool:
        """
        Check if agent should explore.
        
        Args:
            threshold: Exploration threshold (optional)
        """
        if threshold is None:
            threshold = self.satisfaction_threshold
        
        return (self.exploration_drive > 0.5 and 
                self.current_curiosity > self.baseline_curiosity * 1.5)
    
    def increase_threshold(self):
        """Increase satisfaction threshold (reduce exploration)."""
        self.satisfaction_threshold = min(1.0, self.satisfaction_threshold + self.threshold_increase_rate)
    
    def reset(self):
        """Reset curiosity system."""
        self.current_curiosity = self.baseline_curiosity
        self.exploration_drive = 0.0
        self.interest_level = 0.0
        self.satisfaction_level = 0.0
        self.learning_progress = 0.0
        self.exploration_history.clear()
    
    def get_curiosity_summary(self) -> Dict[str, Any]:
        """Get curiosity system summary."""
        return {
            'current_curiosity': self.current_curiosity,
            'baseline_curiosity': self.baseline_curiosity,
            'exploration_drive': self.exploration_drive,
            'interest_level': self.interest_level,
            'satisfaction_level': self.satisfaction_level,
            'learning_progress': self.learning_progress,
            'should_explore': self.should_explore(),
        }
    
    def __repr__(self) -> str:
        return f"CuriositySystem(curiosity={self.current_curiosity:.3f}, explore={self.exploration_drive:.3f})"


class EnhancedAgentBrain:
    """Enhanced agent brain with neuromodulation."""
    
    def __init__(self, brain=None):
        """
        Initialize enhanced agent brain.
        
        Args:
            brain: Original brain object
        """
        self.brain = brain
        self.neuromodulation = NeuromodulationState()
        self.reward_processor = RewardProcessor()
        self.novelty_detector = NoveltyDetector()
        self.curiosity_system = CuriositySystem()
        
        # Subsystem enable flags
        self.reward_modulation_enabled = True
        self.structural_plasticity_enabled = True
        self.development_enabled = True
        self.curiosity_enabled = True
        
        # Performance tracking
        self.performance_metrics = {
            'steps_completed': 0,
            'total_rewards': 0.0,
            'novelty_detected': 0,
            'exploration_count': 0,
        }
    
    def process_sensory_input(self, percept):
        """Process sensory input with neuromodulation."""
        # Process with original brain
        if self.brain:
            self.brain.receiveSensoryInput(percept)
        
        # Analyze for novelty
        vision_data = percept.getVision() if percept else []
        if vision_data:
            novelty = self.novelty_detector.detect_novelty(vision_data)
            self.neuromodulation.update_curiosity(novelty, self.curiosity_system.get_exploration_drive())
        
        return percept
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if not self.reward_modulation_enabled:
            return
        
        # Update neuromodulation
        self.neuromodulation.update_dopamine(reward, predicted_reward)
        
        # Update reward processor
        self.reward_processor.update(reward, predicted_reward)
        
        # Record performance
        self.performance_metrics['total_rewards'] += reward
        if reward > 0:
            self.performance_metrics['novelty_detected'] += 1
    
    def get_neuromodulation_level(self) -> float:
        """Get overall neuromodulation level."""
        return (self.neuromodulation.dopamine + 
                self.neuromodulation.curiosity + 
                self.neuromodulation.novelty) / 3.0
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self.curiosity_system.get_curiosity_level()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self.neuromodulation.novelty
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self.neuromodulation.prediction_error
    
    def should_explore(self) -> bool:
        """Check if agent should explore."""
        return (self.curiosity_enabled and 
                self.curiosity_system.should_explore())
    
    def enable_reward_modulation(self, enabled: bool):
        """Enable/disable reward modulation."""
        self.reward_modulation_enabled = enabled
    
    def enable_structural_plasticity(self, enabled: bool):
        """Enable/disable structural plasticity."""
        self.structural_plasticity_enabled = enabled
    
    def enable_development(self, enabled: bool):
        """Enable/disable development."""
        self.development_enabled = enabled
    
    def enable_curiosity(self, enabled: bool):
        """Enable/disable curiosity."""
        self.curiosity_enabled = enabled
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        return {
            'neuromodulation': self.neuromodulation.get_summary(),
            'reward_processing': {
                'value_estimate': self.reward_processor.get_expected_reward(),
                'confidence': self.reward_processor.get_reward_confidence(),
                'temporal_difference': self.reward_processor.temporal_difference,
            },
            'novelty_detection': self.novelty_detector.get_novelty_summary(),
            'curiosity': self.curiosity_system.get_curiosity_summary(),
            'performance': self.performance_metrics,
        }
    
    def reset(self):
        """Reset all subsystems."""
        self.neuromodulation.reset()
        self.reward_processor.reset()
        self.novelty_detector.reset()
        self.curiosity_system.reset()
        
        # Reset performance metrics
        self.performance_metrics = {
            'steps_completed': 0,
            'total_rewards': 0.0,
            'novelty_detected': 0,
            'exploration_count': 0,
        }
    
    def __repr__(self) -> str:
        return f"EnhancedAgentBrain(neuromodulation={self.get_neuromodulation_level():.3f}, curiosity={self.get_curiosity_level():.3f})"


def create_enhanced_agent_brain(brain=None) -> EnhancedAgentBrain:
    """Create enhanced agent brain with neuromodulation."""
    return EnhancedAgentBrain(brain)


def run_neuromodulated_simulation(brain, world, agent, steps: int = 100):
    """
    Run simulation with neuromodulation.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of steps
    """
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input (with novelty detection)
        if hasattr(agent, 'process_sensory_input'):
            agent.process_sensory_input(percept)
        else:
            brain.receiveSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        if hasattr(agent, 'apply_reward_modulation'):
            agent.apply_reward_modulation(reward, 0.0)
        
        # Check if agent should explore
        if hasattr(agent, 'should_explore') and agent.should_explore():
            # Apply curiosity bonus
            curiosity = agent.get_curiosity_level()
            reward += curiosity * 0.1


def analyze_neuromodulation_data(agent_session: EnhancedAgentBrain) -> Dict[str, Any]:
    """Analyze neuromodulation data from agent session."""
    summary = agent_session.get_performance_summary()
    
    analysis = {
        'dopamine_dynamics': {},
        'curiosity_trajectory': {},
        'reward_learning': {},
        'novelty_adaptation': {},
    }
    
    # Analyze dopamine dynamics
    neuromod = summary['neuromodulation']
    analysis['dopamine_dynamics'] = {
        'final_dopamine': neuromod['dopamine'],
        'prediction_error': neuromod['prediction_error'],
        'reward_prediction': neuromod['reward_prediction'],
        'total_rewards': neuromod['total_rewards'],
    }
    
    # Analyze curiosity trajectory
    curiosity = summary['curiosity']
    analysis['curiosity_trajectory'] = {
        'final_curiosity': curiosity['current_curiosity'],
        'baseline_curiosity': curiosity['baseline_curiosity'],
        'exploration_drive': curiosity['exploration_drive'],
        'should_explore': curiosity['should_explore'],
    }
    
    # Analyze reward learning
    reward = summary['reward_processing']
    analysis['reward_learning'] = {
        'value_estimate': reward['value_estimate'],
        'confidence': reward['confidence'],
        'temporal_difference': reward['temporal_difference'],
    }
    
    # Analyze novelty adaptation
    novelty = summary['novelty_detection']
    analysis['novelty_adaptation'] = {
        'current_novelty': novelty['current_novelty'],
        'memory_size': novelty['memory_size'],
        'similarity_threshold': novelty['similarity_threshold'],
    }
    
    return analysis
