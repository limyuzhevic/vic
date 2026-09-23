#!/usr/bin/env python3
"""
AgentBrain module for NLM (Neural Learning Machine).

This module provides the AgentBrain class and related functionality for
connecting NLM brain to world simulation, enabling agent behavior.
"""

from typing import List, Dict, Any, Optional, Union
from dataclasses import dataclass

@dataclass
class SensoryPercept:
    """
    Sensory percept data structure.

    This class represents the sensory information that an agent receives
    from the world, including vision, audio, and internal signals.
    """

    vision: Optional[Dict[str, Any]] = None
    audio: Optional[Dict[str, Any]] = None
    internal_signals: Optional[Dict[str, Any]] = None
    timestamp: float = 0.0

    def get_combined_signal(self) -> Dict[str, Any]:
        """Get combined sensory signal."""
        combined = {}
        if self.vision:
            combined.update(self.vision)
        if self.audio:
            combined.update(self.audio)
        if self.internal_signals:
            combined.update(self.internal_signals)
        combined['timestamp'] = self.timestamp
        return combined

    def __str__(self) -> str:
        """String representation."""
        signals = []
        if self.vision:
            signals.append("vision")
        if self.audio:
            signals.append("audio")
        if self.internal_signals:
            signals.append("internal")
        return f"SensoryPercept({', '.join(signals)})"
class Action:
    """Action representation for motor output."""

    def __init__(self, action_type: str = "move", params: Optional[Dict[str, Any]] = None):
        """
        Initialize an action.

        Args:
            action_type: Type of action (e.g., "move", "speak", "wait")
            params: Parameters for the action
        """
        self.action_type = action_type
        self.params = params or {}

    def get_parameter(self, key: str, default: Any = None) -> Any:
        """Get an action parameter."""
        return self.params.get(key, default)

    def __str__(self) -> str:
        """String representation."""
        return f"Action({self.action_type}, {self.params})"

    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Action(action_type='{self.action_type}', params={self.params})"
class MotorCommand:
    """Motor command representation."""

    def __init__(self, motor_neurons: Optional[List[int]] = None):
        """
        Initialize a motor command.

        Args:
            motor_neurons: List of motor neuron IDs
        """
        self.motor_neurons = motor_neurons or []

    def get_neuron_count(self) -> int:
        """Get number of motor neurons in command."""
        return len(self.motor_neurons)

    def add_neuron(self, neuron_id: int) -> None:
        """Add a motor neuron to the command."""
        if neuron_id not in self.motor_neurons:
            self.motor_neurons.append(neuron_id)

    def __str__(self) -> str:
        """String representation."""
        return f"MotorCommand({len(self.motor_neurons)} neurons)"

    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"MotorCommand(motor_neurons={self.motor_neurons})"
class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world.

    This class provides the interface between the neural brain and the
    external world, handling sensory processing, motor decoding, and
    learning modulation.
    """

    def __init__(self, brain: Any):
        """
        Initialize an agent brain with a neural brain.

        Args:
            brain: Neural brain instance to control
        """
        self.brain = brain
        self.sensory_input_size = 16
        self.motor_output_size = 8
        self.world = None

        # Neuromodulation settings
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False

        # Current levels
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0

    def initialize(self, world: Any) -> bool:
        """
        Initialize agent with world.

        Args:
            world: World instance to interact with

        Returns:
            True if initialization successful, False otherwise
        """
        try:
            self.world = world
            print(f"AgentBrain initialized with world")
            print(f"  - Sensory input size: {self.sensory_input_size}")
            print(f"  - Motor output size: {self.motor_output_size}")
            return True
        except Exception as e:
            print(f"AgentBrain initialization failed: {e}")
            return False

    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        return self.sensory_input_size

    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        return self.motor_output_size

    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """
        Process sensory percept and inject into brain.

        Args:
            percept: Sensory percept from world
        """
        print(f"Agent processing sensory input: {percept}")

        # Simple sensory processing
        combined_signal = percept.get_combined_signal()
        if combined_signal:
            # Simulate sensory processing
            intensity = sum(combined_signal.values()) / len(combined_signal)
            self._inject_sensory_signal(intensity)

    def _inject_sensory_signal(self, intensity: float) -> None:
        """Inject processed sensory signal into brain."""
        # Simplified signal injection
        print(f"  -> Injected sensory signal with intensity {intensity:.3f}")

    def decode_motor_command(self) -> MotorCommand:
        """
        Decode brain motor activity into motor command.

        Returns:
            Motor command based on brain activity
        """
        print("Decoding motor command from brain...")

        # Simple motor decoding
        brain_active = self.brain.get_active_neuron_count()
        brain_total = self.brain.get_total_neuron_count()

        if brain_total > 0:
            activity_ratio = brain_active / brain_total
            neuron_count = int(activity_ratio * self.motor_output_size)
            neuron_count = max(1, min(neuron_count, self.motor_output_size))
        else:
            neuron_count = 0

        command = MotorCommand()
        for i in range(neuron_count):
            command.add_neuron(i)

        print(f"  -> Generated motor command with {neuron_count} neurons")
        return command

    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.

        Args:
            reward: Actual reward received
            predicted_reward: Expected reward
        """
        self._prediction_error = abs(reward - predicted_reward)
        self._neuromodulation_level = min(1.0, self._prediction_error)

        print(f"Applied reward modulation:")
        print(f"  - Reward: {reward}")
        print(f"  - Predicted: {predicted_reward}")
        print(f"  - Prediction error: {self._prediction_error:.3f}")
        print(f"  - Neuromodulation level: {self._neuromodulation_level:.3f}")

    def update_development(self, timestep: float) -> None:
        """
        Update development system.

        Args:
            timestep: Current simulation timestep
        """
        if self._development_enabled:
            development_rate = self._calculate_development_rate(timestep)
            self._neuromodulation_level = min(1.0,
                                             self._neuromodulation_level + development_rate)
            print(f"Updated development (rate: {development_rate:.4f})")

    def _calculate_development_rate(self, timestep: float) -> float:
        """Calculate developmental change rate."""
        base_rate = 0.001
        if self._curiosity_enabled:
            base_rate *= (1.0 + self._curiosity_level)
        return base_rate

    def get_developmental_stage(self) -> int:
        """Get current developmental stage."""
        return self.brain.get_developmental_stage()

    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self._neuromodulation_level

    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level

    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._novelty_level

    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._prediction_error

    def reset(self) -> None:
        """Reset agent for new episode."""
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        print("Agent reset for new episode")

    def get_brain(self) -> Any:
        """Get the underlying brain."""
        return self.brain

    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable
        print(f"Reward modulation {'enabled' if enable else 'disabled'}")

    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable
        print(f"Structural plasticity {'enabled' if enable else 'disabled'}")

    def enable_development(self, enable: bool) -> None:
        """Enable or disable development."""
        self._development_enabled = enable
        print(f"Development {'enabled' if enable else 'disabled'}")

    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity."""
        self._curiosity_enabled = enable
        print(f"Curiosity {'enabled' if enable else 'disabled'}")

    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled

    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled

    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled

    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled

    def __str__(self) -> str:
        """String representation."""
        return f"AgentBrain(neuromodulation={self._neuromodulation_level:.2f})"

    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"AgentBrain(neuromodulation_enabled={self._reward_modulation_enabled}, "
                f"structural_plasticity_enabled={self._structural_plasticity_enabled}, "
                f"development_enabled={self._development_enabled}, "
                f"curiosity_enabled={self._curiosity_enabled})")
def create_agent(brain: Any) -> AgentBrain:
    """
    Create a new agent brain interface.

    Args:
        brain: Neural brain instance

    Returns:
        New AgentBrain instance
    """
    return AgentBrain(brain)
def demo_agent_behavior() -> None:
    """
    Run a demonstration of basic agent behavior.

    This function shows how to create and use an agent brain,
    demonstrating basic agent functionality and world interaction.
    """
    print("Running agent behavior demo...")

    # Create brain and agent
    from nlm.python.brain import Brain

    brain = Brain()
    agent = create_agent(brain)

    # Initialize (no world for this demo)
    print("Agent created successfully!")
    print(f"  - Agent features:")
    print(f"    - Reward modulation: {agent.is_reward_modulation_enabled()}")
    print(f"    - Structural plasticity: {agent.is_structural_plasticity_enabled()}")
    print(f"    - Development: {agent.is_development_enabled()}")
    print(f"    - Curiosity: {agent.is_curiosity_enabled()}")

    print("Demo completed successfully!")