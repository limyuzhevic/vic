# Learning and plasticity management for NLM

"""
Learning and plasticity management for NLM (Neural Learning Machine).

This module provides classes for managing learning processes, plasticity rules,
and adaptive behavior in the NLM system.
"""

import numpy as np
import time
from typing import List, Dict, Optional, Any, Tuple
from enum import Enum

from ._cpp import (
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType
)

__all__ = [
    'LearningSystem', 'PlasticityManager', 'STDPLearner', 'HebbianLearner',
    'MetaplasticitySystem', 'CuriosityEngine', 'NoveltyDetector',
    'RewardSystem', 'DevelopmentManager', 'AdaptiveLearning',
    'createLearningSystem', 'createPlasticityManager',
    'createSTDPLearner', 'createHebbianLearner',
    'createMetaplasticitySystem', 'createCuriosityEngine',
    'createNoveltyDetector', 'createRewardSystem',
    'createDevelopmentManager', 'createAdaptiveLearning',
]


class LearningSystem:
    """
    Central learning system for NLM.

    This class manages all learning processes including synaptic plasticity,
    neuromodulation, and adaptive behavior control.
    """

    def __init__(self, brain):
        """
        Initialize learning system.

        Args:
            brain: The brain to learn for.
        """
        self.brain = brain
        self.plasticity_manager = PlasticityManager(brain)
        self.reward_system = RewardSystem(self)
        self.curiosity_engine = CuriosityEngine(self)
        self.novelty_detector = NoveltyDetector(self)
        self.metaplasticity_system = MetaplasticitySystem(self)
        self.development_manager = DevelopmentManager(self)
        self.stdp_learner = STDPLearner(self)
        self.hebbian_learner = HebbianLearner(self)

        self.learning_enabled = True
        self.adaptation_rate = 0.01
        self.exploration_rate = 0.1

    def enableLearning(self, enabled=True):
        """Enable or disable learning."""
        self.learning_enabled = enabled

    def update(self, timestep: float):
        """
        Update learning system.

        Args:
            timestep: Time step for update.
        """
        if not self.learning_enabled:
            return

        # Update plasticity
        self.plasticity_manager.update(timestep)

        # Update curiosity
        self.curiosity_engine.update(timestep)

        # Update novelty detection
        self.novelty_detector.update(timestep)

        # Update metaplasticity
        self.metaplasticity_system.update(timestep)

        # Update development
        self.development_manager.update(timestep)

    def getLearningStats(self) -> Dict:
        """
        Get learning statistics.

        Returns:
            Dictionary containing learning statistics.
        """
        stats = {
            'plasticity_enabled': self.plasticity_manager.enabled,
            'reward_enabled': self.reward_system.enabled,
            'curiosity_enabled': self.curiosity_engine.enabled,
            'novelty_enabled': self.novelty_detector.enabled,
            'metaplasticity_enabled': self.metaplasticity_system.enabled,
            'development_enabled': self.development_manager.enabled,
            'total_synapses_learned': self.plasticity_manager.getTotalSynapsesLearned(),
            'learning_rate': self.getAverageLearningRate(),
            'exploration_rate': self.exploration_rate,
        }

        # Add component statistics
        stats.update(self.plasticity_manager.getStats())
        stats.update(self.reward_system.getStats())
        stats.update(self.curiosity_engine.getStats())
        stats.update(self.novelty_detector.getStats())

        return stats

    def getAverageLearningRate(self) -> float:
        """Get average learning rate."""
        # Implementation would track learning over time
        return self.adaptation_rate


class PlasticityManager:
    """
    Manager for synaptic plasticity rules.

    This class handles all plasticity rules including STDP, Hebbian learning,
    and metaplasticity.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize plasticity manager.

        Args:
            learning_system: The learning system to manage.
        """
        self.learning_system = learning_system
        self.stdp_learner = STDPLearner(learning_system)
        self.hebbian_learner = HebbianLearner(learning_system)
        self.metaplasticity_system = MetaplasticitySystem(learning_system)

        self.enabled = True
        self.global_learning_rate = 0.01
        self.stdp_enabled = True
        self.hebbian_enabled = True
        self.metaplasticity_enabled = False

    def update(self, timestep: float):
        """Update plasticity rules."""
        if not self.enabled:
            return

        # Update STDP
        if self.stdp_enabled:
            self.stdp_learner.update(timestep)

        # Update Hebbian learning
        if self.hebbian_enabled:
            self.hebbian_learner.update(timestep)

        # Update metaplasticity
        if self.metaplasticity_enabled:
            self.metaplasticity_system.update(timestep)

    def getStats(self) -> Dict:
        """Get plasticity statistics."""
        stats = {
            'enabled': self.enabled,
            'global_learning_rate': self.global_learning_rate,
            'stdp_enabled': self.stdp_enabled,
            'hebbian_enabled': self.hebbian_enabled,
            'metaplasticity_enabled': self.metaplasticity_enabled,
            'total_synapses_learned': self.getTotalSynapsesLearned(),
        }

        # Add component stats
        stats.update(self.stdp_learner.getStats())
        stats.update(self.hebbian_learner.getStats())
        stats.update(self.metaplasticity_system.getStats())

        return stats

    def getTotalSynapsesLearned(self) -> int:
        """Get total number of synapses learned."""
        # Implementation would track this
        return 0


class STDPLearner:
    """
    Spike-Timing-Dependent Plasticity (STDP) learner.

    This class implements STDP learning rules based on the timing
    between pre-synaptic and post-synaptic spikes.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize STDP learner.

        Args:
            learning_system: The learning system to learn for.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.learning_rate = 0.01
        self.hebbian_weight = 0.02
        self.ltd_weight = 0.015
        self.time_constant = 20.0
        self.stdp_trace_enabled = True

    def update(self, timestep: float):
        """Update STDP learning."""
        # Implementation would update synaptic weights based on spike timing
        pass

    def getStats(self) -> Dict:
        """Get STDP statistics."""
        return {
            'enabled': self.enabled,
            'learning_rate': self.learning_rate,
            'hebbian_weight': self.hebbian_weight,
            'ltd_weight': self.ltd_weight,
            'time_constant': self.time_constant,
            'stdp_trace_enabled': self.stdp_trace_enabled,
        }


class HebbianLearner:
    """
    Hebbian learning rule implementation.

    This class implements Hebbian learning rules where simultaneous
    activation of synapses strengthens their connection.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize Hebbian learner.

        Args:
            learning_system: The learning system to learn for.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.learning_rate = 0.02
        self.association_window = 50.0
        self.coactivation_threshold = 0.5

    def update(self, timestep: float):
        """Update Hebbian learning."""
        # Implementation would update synaptic weights based on coactivation
        pass

    def getStats(self) -> Dict:
        """Get Hebbian statistics."""
        return {
            'enabled': self.enabled,
            'learning_rate': self.learning_rate,
            'association_window': self.association_window,
            'coactivation_threshold': self.coactivation_threshold,
        }


class MetaplasticitySystem:
    """
    Metaplasticity system for adaptive learning rates.

    This class implements metaplasticity where the learning rate
    itself adapts based on learning history and experience.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize metaplasticity system.

        Args:
            learning_system: The learning system to adapt.
        """
        self.learning_system = learning_system
        self.enabled = False
        self.base_learning_rate = 0.01
        self.adaptation_rate = 0.001
        self.memory_length = 100
        self.target_error = 0.1

        self.error_history: List[float] = []
        self.current_learning_rate = self.base_learning_rate

    def update(self, timestep: float):
        """Update metaplasticity."""
        if not self.enabled:
            return

        # Implementation would adapt learning rate based on error
        self.error_history.append(self.getCurrentError())

        if len(self.error_history) > self.memory_length:
            self.error_history.pop(0)

        # Adjust learning rate based on error
        avg_error = sum(self.error_history) / len(self.error_history)
        self.current_learning_rate = self.base_learning_rate * \
            (1.0 - avg_error / self.target_error)

    def getCurrentError(self) -> float:
        """Get current prediction error."""
        # Implementation would calculate prediction error
        return 0.0

    def getStats(self) -> Dict:
        """Get metaplasticity statistics."""
        return {
            'enabled': self.enabled,
            'base_learning_rate': self.base_learning_rate,
            'adaptation_rate': self.adaptation_rate,
            'target_error': self.target_error,
            'memory_length': self.memory_length,
            'current_learning_rate': self.current_learning_rate,
            'error_history_length': len(self.error_history),
        }


class CuriosityEngine:
    """
    Curiosity engine for exploration and intrinsic motivation.

    This class implements curiosity-driven exploration where agents
    explore novel situations for intrinsic reward.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize curiosity engine.

        Args:
            learning_system: The learning system to drive.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.curiosity_strength = 0.5
        self.exploration_rate = 0.1
        self.noise_level = 0.1
        self.intrinsic_reward_weight = 0.3
        self.novelty_threshold = 0.1

    def update(self, timestep: float):
        """Update curiosity engine."""
        # Implementation would generate curiosity-driven exploration
        pass

    def getStats(self) -> Dict:
        """Get curiosity statistics."""
        return {
            'enabled': self.enabled,
            'curiosity_strength': self.curiosity_strength,
            'exploration_rate': self.exploration_rate,
            'noise_level': self.noise_level,
            'intrinsic_reward_weight': self.intrinsic_reward_weight,
            'novelty_threshold': self.novelty_threshold,
        }


class NoveltyDetector:
    """
    Novelty detector for identifying new experiences.

    This class detects novel events and patterns in the environment
    to drive curiosity and exploration.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize novelty detector.

        Args:
            learning_system: The learning system to monitor.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.noise_threshold = 0.05
        self.recency_weight = 0.7
        self.importance_weight = 0.3

    def update(self, timestep: float):
        """Update novelty detector."""
        # Implementation would detect novelty in experiences
        pass

    def getStats(self) -> Dict:
        """Get novelty statistics."""
        return {
            'enabled': self.enabled,
            'noise_threshold': self.noise_threshold,
            'recency_weight': self.recency_weight,
            'importance_weight': self.importance_weight,
        }


class RewardSystem:
    """
    Reward system for extrinsic motivation.

    This class handles reward prediction, reward delivery, and
    reward-based learning.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize reward system.

        Args:
            learning_system: The learning system to reward.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.reward_discount_factor = 0.99
        self.reward_scale = 1.0
        self.prediction_error_enabled = True
        self.bonus_reward_enabled = False
        self.penalty_reward_enabled = False

    def update(self, timestep: float):
        """Update reward system."""
        # Implementation would update reward predictions and deliver rewards
        pass

    def getStats(self) -> Dict:
        """Get reward statistics."""
        return {
            'enabled': self.enabled,
            'reward_discount_factor': self.reward_discount_factor,
            'reward_scale': self.reward_scale,
            'prediction_error_enabled': self.prediction_error_enabled,
            'bonus_reward_enabled': self.bonus_reward_enabled,
            'penalty_reward_enabled': self.penalty_reward_enabled,
        }


class DevelopmentManager:
    """
    Development system for agent maturation.

    This class manages developmental stages, maturation processes,
    and age-related changes in learning capabilities.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize development manager.

        Args:
            learning_system: The learning system to develop.
        """
        self.learning_system = learning_system
        self.enabled = True
        self.current_stage = DevelopmentalStage.Initial
        self.stage_duration = 100.0
        self.maturation_rate = 0.01
        self.critical_period_enabled = True
        self.pruning_enabled = True

    def update(self, timestep: float):
        """Update development system."""
        # Implementation would update developmental stage and processes
        pass

    def getStats(self) -> Dict:
        """Get development statistics."""
        return {
            'enabled': self.enabled,
            'current_stage': self.current_stage,
            'stage_duration': self.stage_duration,
            'maturation_rate': self.maturation_rate,
            'critical_period_enabled': self.critical_period_enabled,
            'pruning_enabled': self.pruning_enabled,
        }


class AdaptiveLearning:
    """
    Adaptive learning system for optimizing learning parameters.

    This class provides advanced learning optimization including
    hyperparameter tuning and learning strategy adaptation.
    """

    def __init__(self, learning_system: LearningSystem):
        """
        Initialize adaptive learning system.

        Args:
            learning_system: The learning system to optimize.
        """
        self.learning_system = learning_system
        self.enabled = False
        self.optimization_algorithm = 'adam'
        self.learning_rate = 0.01
        self.momentum = 0.9
        self.batch_size = 32
        self.early_stopping_patience = 10

    def update(self, timestep: float):
        """Update adaptive learning."""
        # Implementation would optimize learning parameters
        pass

    def getStats(self) -> Dict:
        """Get adaptive learning statistics."""
        return {
            'enabled': self.enabled,
            'optimization_algorithm': self.optimization_algorithm,
            'learning_rate': self.learning_rate,
            'momentum': self.momentum,
            'batch_size': self.batch_size,
            'early_stopping_patience': self.early_stopping_patience,
        }


# Convenience functions

def createLearningSystem(brain) -> LearningSystem:
    """
    Create a learning system.

    Args:
        brain: The brain to create learning system for.

    Returns:
        LearningSystem instance.
    """
    return LearningSystem(brain)


def createPlasticityManager(learning_system: LearningSystem) -> PlasticityManager:
    """
    Create a plasticity manager.

    Args:
        learning_system: The learning system to manage.

    Returns:
        PlasticityManager instance.
    """
    return PlasticityManager(learning_system)


def createSTDPLearner(learning_system: LearningSystem) -> STDPLearner:
    """
    Create an STDP learner.

    Args:
        learning_system: The learning system to learn for.

    Returns:
        STDPLearner instance.
    """
    return STDPLearner(learning_system)


def createHebbianLearner(learning_system: LearningSystem) -> HebbianLearner:
    """
    Create a Hebbian learner.

    Args:
        learning_system: The learning system to learn for.

    Returns:
        HebbianLearner instance.
    """
    return HebbianLearner(learning_system)


def createMetaplasticitySystem(learning_system: LearningSystem) -> MetaplasticitySystem:
    """
    Create a metaplasticity system.

    Args:
        learning_system: The learning system to adapt.

    Returns:
        MetaplasticitySystem instance.
    """
    return MetaplasticitySystem(learning_system)


def createCuriosityEngine(learning_system: LearningSystem) -> CuriosityEngine:
    """
    Create a curiosity engine.

    Args:
        learning_system: The learning system to drive.

    Returns:
        CuriosityEngine instance.
    """
    return CuriosityEngine(learning_system)


def createNoveltyDetector(learning_system: LearningSystem) -> NoveltyDetector:
    """
    Create a novelty detector.

    Args:
        learning_system: The learning system to monitor.

    Returns:
        NoveltyDetector instance.
    """
    return NoveltyDetector(learning_system)


def createRewardSystem(learning_system: LearningSystem) -> RewardSystem:
    """
    Create a reward system.

    Args:
        learning_system: The learning system to reward.

    Returns:
        RewardSystem instance.
    """
    return RewardSystem(learning_system)


def createDevelopmentManager(learning_system: LearningSystem) -> DevelopmentManager:
    """
    Create a development manager.

    Args:
        learning_system: The learning system to develop.

    Returns:
        DevelopmentManager instance.
    """
    return DevelopmentManager(learning_system)


def createAdaptiveLearning(learning_system: LearningSystem) -> AdaptiveLearning:
    """
    Create an adaptive learning system.

    Args:
        learning_system: The learning system to optimize.

    Returns:
        AdaptiveLearning instance.
    """
    return AdaptiveLearning(learning_system)


# Export commonly used classes and functions
__all__.extend([
    'LearningSystem', 'PlasticityManager', 'STDPLearner', 'HebbianLearner',
    'MetaplasticitySystem', 'CuriosityEngine', 'NoveltyDetector',
    'RewardSystem', 'DevelopmentManager', 'AdaptiveLearning',
    'createLearningSystem', 'createPlasticityManager',
    'createSTDPLearner', 'createHebbianLearner',
    'createMetaplasticitySystem', 'createCuriosityEngine',
    'createNoveltyDetector', 'createRewardSystem',
    'createDevelopmentManager', 'createAdaptiveLearning'
])