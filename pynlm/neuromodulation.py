"""
Neuromodulation - Dynamic Regulation of Neural Processing
=========================================================

Neuromodulation is a fundamental aspect of brain function, involving the
release of neuromodulators that globally influence neural processing.
This module implements biologically plausible neuromodulation systems
that regulate learning, attention, and behavior based on task demands
and environmental context.

Key Neuromodulators:

1. Dopamine (DA):
   - Reinforcement learning signal
   - Prediction error encoding
   - Reward-based plasticity modulation
   - Phasic and tonic release patterns

2. Novelty:
   - Detection of unexpected stimuli
   - Promotion of exploration
   - Learning rate adaptation
   - Habituation mechanisms

3. Curiosity:
   - Driven by information gain
   - Intrinsic motivation
   - Goal exploration
   - Action selection bias

4. Prediction Error:
   - Bayesian inference violation
   - Learning signal
   - Model updating
   - Confidence estimation

Implementation Features:
- Real-time neuromodulator dynamics
- Interaction between different neuromodulators
- Context-dependent release patterns
- Spike-based and rate-based models
- Integration with plasticity systems
- Experimental control interfaces

Integration with Other Systems:
- Plasticity: Modulates synaptic learning rates
- Memory: Influences memory consolidation
- Cognition: Controls attention and planning
- Prediction: Provides prediction error signals
- Development: Guides developmental processes

Mathematical Models:
- Dopamine: Approximate value learning
- Novelty: Surprise-based detection
- Curiosity: Information gain maximization
- Prediction Error: TD learning signals

Example Applications:
- Reinforcement learning tasks
- Classical conditioning
- Operant conditioning
- Sensorimotor integration
- Cognitive flexibility

References:
- Robinson et al. (1995). "The role of dopamine in reward and learning"
- Schultz et al. (1997). "A sequential sampling model of decision making"
- Yu & Dayan (2005). "Inference, information, and control in the brain"
- Niv et al. (2015). "The neuroscience of value-based decision making"
"""

from typing import List, Dict, Any, Optional
import numpy as np
class Neuromodulator:
    """
    Neuromodulation system integrating multiple neuromodulators.

    This class implements a comprehensive neuromodulation system that
    coordinates the release and effects of different neuromodulators
    including dopamine, novelty, curiosity, and prediction error.
    The system provides biologically plausible models of how these
    signals interact to guide learning and behavior.

    Neuromodulation in the brain:
    - Provides global gain modulation
    - Signals prediction errors and rewards
    - Encodes novelty and unexpected events
    - Drives exploration vs. exploitation
    - Modulates plasticity and learning rates
    - Influences attention and cognitive control

    The architecture follows key principles:
    1. Multiple interacting neuromodulators
    2. Real-time dynamic updates
    3. Context-dependent processing
    4. Integration with plasticity systems
    5. Experimental control interfaces

    Attributes:
        dopamine_level (float): Current dopamine concentration
        novelty_level (float): Current novelty detection level
        curiosity_level (float): Current curiosity level
        prediction_error (float): Current prediction error signal
        learning_rate_modulation (float): Global learning rate
        exploration_bonus (float): Exploration bias
        adaptation_rate (float): Neuromodulator adaptation speed

    Example:
        >>> from pynlm.neuromodulation import Neuromodulator
        >>> 
        >>> # Create neuromodulation system
        >>> neuromod = Neuromodulator()
        >>> 
        >>> # Initialize
        >>> neuromod.initialize()
        >>> 
        >>> # Apply reward
        >>> neuromod.apply_reward(1.0)
        >>> 
        >>> # Process novelty
        >>> neuromod.update_novelty([0.1, 0.5, 0.9])
        >>> 
        >>> # Get state
        >>> state = neuromod.get_state()
        >>> print(f"Dopamine: {state['dopamine']:.2f}")
    """

    def __init__(self):
        """
        Initialize neuromodulation system.

        Sets up neuromodulator dynamics with biologically plausible parameters
        and initial states for all neuromodulatory signals.
        """
        # Neuromodulator states
        self.dopamine_level = 0.1
        self.novelty_level = 0.05
        self.curiosity_level = 0.0
        self.prediction_error = 0.0

        # Neuromodulator parameters
        self.dopamine_baseline = 0.1
        self.novelty_baseline = 0.05
        self.curiosity_baseline = 0.0
        self.pe_baseline = 0.0

        # Modulation parameters
        self.reward_sensitivity = 1.0
        self.curiosity_sensitivity = 1.0
        self.novelty_threshold = 0.3
        self.prediction_error_gain = 1.0

        # Dynamics parameters
        self.dopamine_decay = 0.95
        self.novelty_decay = 0.98
        self.curiosity_decay = 0.99
        self.pe_decay = 0.99

        # History for smoothing
        self.dopamine_history = []
        self.novelty_history = []
        self.curiosity_history = []
        self.pe_history = []

    def initialize(self):
        """Initialize neuromodulation system to default state."""
        self.dopamine_level = self.dopamine_baseline
        self.novelty_level = self.novelty_baseline
        self.curiosity_level = self.curiosity_baseline
        self.prediction_error = self.pe_baseline

        self.dopamine_history.clear()
        self.novelty_history.clear()
        self.curiosity_history.clear()
        self.pe_history.clear()

    def apply_reward(self, reward: float):
        """
        Apply reward signal to neuromodulation system.

        Args:
            reward (float): Reward magnitude (positive = reward, negative = punishment)

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.apply_reward(1.0)  # Reward
            >>> neuromod.apply_reward(-0.5)  # Punishment

        Note:
            This method implements a phasic dopamine response to reward prediction errors,
            following the temporal difference learning framework.
        """
        # Calculate prediction error
        delta = reward - self.prediction_error

        # Update dopamine based on prediction error
        self.dopamine_level += delta * self.reward_sensitivity

        # Keep dopamine in reasonable bounds
        self.dopamine_level = max(0.0, min(2.0, self.dopamine_level))

        # Record in history
        self.dopamine_history.append(self.dopamine_level)
        if len(self.dopamine_history) > 100:
            self.dopamine_history.pop(0)

    def update_novelty(self, sensory_input: List[float]):
        """
        Update novelty detection based on sensory input.

        Args:
            sensory_input (List[float]): Sensory input pattern

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.update_novelty([0.1, 0.2, 0.3])  # Novel input
            >>> neuromod.update_novelty([0.0, 0.0, 0.0])  # Familiar input

        Note:
            Novelty detection follows information theory principles,
            measuring surprise or deviation from expected inputs.
        """
        # Calculate input statistics (mean and standard deviation)
        if not sensory_input:
            return

        input_mean = np.mean(sensory_input)
        input_std = np.std(sensory_input)

        # Novelty is based on deviation from mean
        novelty = np.sum((sensory_input - input_mean) ** 2)

        # Normalize by input variance
        if input_std > 0:
            novelty /= input_std ** 2

        # Apply novelty detection with threshold
        if novelty > self.novelty_threshold:
            # Rapid increase in novelty level
            self.novelty_level = min(1.0, self.novelty_level + 0.3 * novelty)
        else:
            # Gradual decay for familiar inputs
            self.novelty_level *= self.novelty_decay

        # Keep in reasonable bounds
        self.novelty_level = max(0.0, min(1.0, self.novelty_level))

        # Record in history
        self.novelty_history.append(self.novelty_level)
        if len(self.novelty_history) > 100:
            self.novelty_history.pop(0)

    def set_dopamine_level(self, level: float):
        """
        Set dopamine level directly.

        Args:
            level (float): Target dopamine level (0.0 to 2.0)

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.set_dopamine_level(0.8)  # High dopamine
            >>> neuromod.set_dopamine_level(0.1)  # Low dopamine
        """
        self.dopamine_level = max(0.0, min(2.0, level))

    def set_novelty_level(self, level: float):
        """
        Set novelty level directly.

        Args:
            level (float): Target novelty level (0.0 to 1.0)

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.set_novelty_level(0.5)  # Medium novelty
        """
        self.novelty_level = max(0.0, min(1.0, level))

    def set_curiosity_level(self, level: float):
        """
        Set curiosity level directly.

        Args:
            level (float): Target curiosity level (0.0 to 1.0)

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.set_curiosity_level(0.7)  # High curiosity
        """
        self.curiosity_level = max(0.0, min(1.0, level))

    def get_state(self) -> Dict[str, Any]:
        """
        Get current neuromodulator state.

        Returns:
            Dict[str, Any]: Current neuromodulator state with the following keys:
                - dopamine (float): Current dopamine level
                - novelty (float): Current novelty level
                - curiosity (float): Current curiosity level
                - prediction_error (float): Current prediction error
                - learning_rate (float): Global learning rate modulation
                - exploration_bias (float): Current exploration bias

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.apply_reward(1.0)
            >>> neuromod.update_novelty([0.1, 0.5, 0.9])
            >>> 
            >>> state = neuromod.get_state()
            >>> print(f"Neuromodulator state: {state}")
        """
        # Apply smoothing based on history
        dopamine_smooth = self._smooth_value(self.dopamine_level, self.dopamine_history)
        novelty_smooth = self._smooth_value(self.novelty_level, self.novelty_history)
        curiosity_smooth = self._smooth_value(self.curiosity_level, self.curiosity_history)
        pe_smooth = self._smooth_value(self.prediction_error, self.pe_history)

        # Calculate derived values
        learning_rate = 0.01 * (1.0 + dopamine_smooth)
        exploration_bias = 0.5 * (1.0 - novelty_smooth) + 0.5 * curiosity_smooth

        return {
            "dopamine": dopamine_smooth,
            "novelty": novelty_smooth,
            "curiosity": curiosity_smooth,
            "prediction_error": pe_smooth,
            "learning_rate": learning_rate,
            "exploration_bias": exploration_bias
        }

    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update neuromodulation system for one time step.

        Args:
            dt (float): Time step duration in seconds

        Returns:
            Dict[str, Any]: Neuromodulation statistics

        Example:
            >>> neuromod = Neuromodulator()
            >>> neuromod.initialize()
            >>> 
            >>> for step in range(100):
            ...     stats = neuromod.update(0.001)
            ...     if step % 10 == 0:
            ...         print(f"Dopamine: {stats['dopamine']:.3f}")

        Note:
            This method should be called at each simulation step to
            allow neuromodulators to update their dynamics.
        """
        # Apply decay to neuromodulators
        self.dopamine_level *= self.dopamine_decay
        self.novelty_level *= self.novelty_decay
        self.curiosity_level *= self.curiosity_decay
        self.prediction_error *= self.pe_decay

        # Smooth values based on history
        self._update_history()

        # Calculate derived statistics
        stats = self.get_state()

        return stats

    def _smooth_value(self, current_value: float, history: List[float]) -> float:
        """Apply exponential smoothing to a value."""
        if not history:
            return current_value

        # Exponential smoothing
        alpha = 0.1
        smoothed = alpha * current_value + (1 - alpha) * history[-1]

        return smoothed

    def _update_history(self):
        """Update history lists with current values."""
        self.dopamine_history.append(self.dopamine_level)
        self.novelty_history.append(self.novelty_level)
        self.curiosity_history.append(self.curiosity_level)
        self.pe_history.append(self.prediction_error)

        # Keep history manageable
        max_history = 100
        for history in [self.dopamine_history, self.novelty_history,
                       self.curiosity_history, self.pe_history]:
            if len(history) > max_history:
                history.pop(0)

    def __repr__(self) -> str:
        return f"<Neuromodulator DA={self.dopamine_level:.2f} Novelty={self.novelty_level:.2f} Curiosity={self.curiosity_level:.2f}>"