# Agent and sensory-motor integration for NLM

"""
Agent and sensory-motor integration for NLM (Neural Learning Machine).

This module provides classes for the agent system that connects the brain
to the world, handling sensory transduction and motor decoding.
"""

from ._cpp import (
    AgentBody, AgentBrain, SensoryPercept,
    MotorCommand, ActionResult, Vision, Audio,
    InternalSignals, ActionType
)

__all__ = [
    'AgentBody', 'AgentBrain', 'SensoryPercept',
    'MotorCommand', 'ActionResult',
    'createAgentBrain', 'createAgentSystem',
    'AgentManager', 'SensoryMotorProcessor',
    'NeuralController', 'MotorOutput',
    'AgentState', 'SensoryProcessor',
]


class AgentManager:
    """
    Manager for agent simulation and behavior control.

    This class provides high-level control over agent behavior,
    including goal setting, action selection, and performance monitoring.
    """

    def __init__(self, agent_brain: AgentBrain):
        """
        Initialize agent manager.

        Args:
            agent_brain: The agent brain to manage.
        """
        self.agent_brain = agent_brain
        self.state = AgentState()
        self.goal = None
        self.performance_metrics = {}
        self.action_history: List[Dict] = []

    def setGoal(self, goal: Dict):
        """
        Set agent goal.

        Args:
            goal: Dictionary containing goal parameters.
        """
        self.goal = goal
        self.state.goal = goal

    def executeAction(self, action: MotorCommand, world_time: float) -> ActionResult:
        """
        Execute action in the world.

        Args:
            action: Action to execute.
            world_time: Current world time.

        Returns:
            Action result.
        """
        # In a real implementation, this would communicate with the world
        # For now, return a placeholder result
        result = ActionResult()
        result.success = True
        result.reward = 0.0
        result.message = f"Executed action: {action}"
        return result

    def updateState(self, new_state: Dict):
        """
        Update agent state.

        Args:
            new_state: New state dictionary.
        """
        self.state.update(new_state)

    def getPerformance(self) -> Dict:
        """
        Get agent performance metrics.

        Returns:
            Dictionary containing performance metrics.
        """
        return self.performance_metrics.copy()


class SensoryMotorProcessor:
    """
    Processor for sensory input and motor output.

    This class handles the complete sensory-motor loop, including
    sensory transduction, neural processing, and motor decoding.
    """

    def __init__(self, agent_brain: AgentBrain):
        """
        Initialize sensory-motor processor.

        Args:
            agent_brain: The agent brain to process for.
        """
        self.agent_brain = agent_brain
        self.sensory_buffer: List[SensoryPercept] = []
        self.motor_buffer: List[MotorCommand] = []
        self.processors = []
        self.filters = []

    def addSensoryProcessor(self, processor):
        """Add sensory processor."""
        self.processors.append(processor)

    def addMotorFilter(self, filter_):
        """Add motor filter."""
        self.filters.append(filter_)

    def processSensoryInput(self, percept: SensoryPercept):
        """
        Process sensory input through the pipeline.

        Args:
            percept: Sensory percept to process.
        """
        # Add to buffer
        self.sensory_buffer.append(percept)

        # Apply processors
        for processor in self.processors:
            percept = processor(pert)

        # Send to brain
        self.agent_brain.processSensoryInput(pert)

    def decodeMotorCommand(self) -> MotorCommand:
        """
        Decode motor command from brain activity.

        Returns:
            Decoded motor command.
        """
        # Get motor command from brain
        command = self.agent_brain.decodeMotorCommand()

        # Apply filters
        for filter_ in self.filters:
            command = filter_(command)

        # Add to buffer
        self.motor_buffer.append(command)

        return command

    def getSensoryHistory(self, steps: int = None) -> List[SensoryPercept]:
        """
        Get recent sensory history.

        Args:
            steps: Number of steps to retrieve (optional).

        Returns:
            List of recent sensory percepts.
        """
        if steps is None:
            return self.sensory_buffer.copy()
        return self.sensory_buffer[-steps:]

    def getMotorHistory(self, steps: int = None) -> List[MotorCommand]:
        """
        Get recent motor command history.

        Args:
            steps: Number of steps to retrieve (optional).

        Returns:
            List of recent motor commands.
        """
        if steps is None:
            return self.motor_buffer.copy()
        return self.motor_buffer[-steps:]

    def clearBuffers(self):
        """Clear sensory and motor buffers."""
        self.sensory_buffer.clear()
        self.motor_buffer.clear()


class NeuralController:
    """
    Neural controller for agent behavior.

    This class provides advanced neural control capabilities including
    reward modulation, development control, and adaptive learning.
    """

    def __init__(self, agent_brain: AgentBrain):
        """
        Initialize neural controller.

        Args:
            agent_brain: The agent brain to control.
        """
        self.agent_brain = agent_brain
        self.reward_history: List[float] = []
        self.prediction_history: List[float] = []
        self.development_enabled = False
        self.plasticity_enabled = False

    def applyReward(self, reward: float, predicted_reward: float = 0.0):
        """
        Apply reward-based neuromodulation.

        Args:
            reward: Actual reward received.
            predicted_reward: Predicted reward (optional).
        """
        self.agent_brain.applyRewardModulation(reward, predicted_reward)
        self.reward_history.append(reward)
        self.prediction_history.append(predicted_reward)

    def updateDevelopment(self, timestep: float):
        """
        Update development system.

        Args:
            timestep: Time step for development update.
        """
        self.agent_brain.updateDevelopment(timestep)

    def enableFeatures(self, reward: bool = False, development: bool = False,
                      curiosity: bool = False, plasticity: bool = False):
        """
        Enable agent features.

        Args:
            reward: Enable reward modulation.
            development: Enable development.
            curiosity: Enable curiosity.
            plasticity: Enable structural plasticity.
        """
        if reward:
            self.agent_brain.enableRewardModulation(True)
        if development:
            self.agent_brain.enableDevelopment(True)
        if curiosity:
            self.agent_brain.enableCuriosity(True)
        if plasticity:
            self.agent_brain.enableStructuralPlasticity(True)

    def getAdaptiveParameters(self) -> Dict:
        """
        Get adaptive parameters based on experience.

        Returns:
            Dictionary of adaptive parameters.
        """
        if not self.reward_history:
            return {}

        # Calculate statistics
        avg_reward = sum(self.reward_history) / len(self.reward_history)
        max_reward = max(self.reward_history)
        min_reward = min(self.reward_history)

        # Determine if learning is occurring
        if len(self.reward_history) > 1:
            reward_variance = sum((r - avg_reward) ** 2 for r in self.reward_history) / len(self.reward_history)
        else:
            reward_variance = 0.0

        return {
            'average_reward': avg_reward,
            'max_reward': max_reward,
            'min_reward': min_reward,
            'reward_variance': reward_variance,
            'curiosity_enabled': self.agent_brain.isCuriosityEnabled(),
            'development_enabled': self.agent_brain.isDevelopmentEnabled(),
            'plasticity_enabled': self.agent_brain.isStructuralPlasticityEnabled(),
            'reward_enabled': self.agent_brain.isRewardModulationEnabled(),
        }


class MotorOutput:
    """
    Motor output controller and action executor.

    This class manages motor output generation, including action selection,
    execution, and monitoring.
    """

    def __init__(self):
        """Initialize motor output controller."""
        self.action_history: List[Tuple[float, MotorCommand, ActionResult]] = []
        self.motor_neurons: Dict[MotorCommand, float] = {}
        self.action_weights = {}

    def executeAction(self, command: MotorCommand, current_time: float,
                     feedback_function = None) -> ActionResult:
        """
        Execute motor action with optional feedback.

        Args:
            command: Motor command to execute.
            current_time: Current time.
            feedback_function: Optional feedback function.

        Returns:
            Action result.
        """
        # Execute action
        result = ActionResult()
        result.success = True
        result.message = f"Action {command} executed"

        # Get feedback if provided
        if feedback_function:
            result = feedback_function(command, current_time)

        # Record action
        self.action_history.append((current_time, command, result))

        # Update motor neuron activity
        if command in self.motor_neurons:
            self.motor_neurons[command] += 1.0
        else:
            self.motor_neurons[command] = 1.0

        return result

    def getActionHistory(self, steps: int = None) -> List[Tuple[float, MotorCommand, ActionResult]]:
        """
        Get action history.

        Args:
            steps: Number of steps to retrieve (optional).

        Returns:
            List of action history entries.
        """
        if steps is None:
            return self.action_history.copy()
        return self.action_history[-steps:]

    def getMostFrequentAction(self) -> Optional[MotorCommand]:
        """
        Get most frequently executed action.

        Returns:
            Most frequent action or None if no history.
        """
        if not self.action_history:
            return None

        action_counts = {}
        for _, action, _ in self.action_history:
            action_counts[action] = action_counts.get(action, 0) + 1

        return max(action_counts.items(), key=lambda x: x[1])[0]

    def getActionStatistics(self) -> Dict:
        """
        Get action execution statistics.

        Returns:
            Dictionary of action statistics.
        """
        if not self.action_history:
            return {}

        total_actions = len(self.action_history)
        successful_actions = sum(1 for _, _, result in self.action_history 
                               if result.success)

        return {
            'total_actions': total_actions,
            'successful_actions': successful_actions,
            'success_rate': successful_actions / total_actions if total_actions > 0 else 0.0,
            'most_frequent_action': self.getMostFrequentAction(),
            'motor_neuron_activity': self.motor_neurons.copy(),
        }


class AgentState:
    """
    Agent state management.

    This class manages agent state including position, velocity, and
    other relevant variables.
    """

    def __init__(self):
        """Initialize agent state."""
        self.position = {'x': 0.0, 'y': 0.0}
        self.orientation = 0.0
        self.velocity = {'x': 0.0, 'y': 0.0}
        self.angular_velocity = 0.0
        self.energy = 1.0
        self.health = 1.0
        self.age = 0
        self.is_moving = False
        self.is_turning = False
        self.goal = None
        self.last_action_time = 0.0

    def update(self, new_state: Dict):
        """
        Update state with new state dictionary.

        Args:
            new_state: New state dictionary.
        """
        for key, value in new_state.items():
            if hasattr(self, key):
                setattr(self, key, value)

    def getStateDict(self) -> Dict:
        """
        Get state as dictionary.

        Returns:
            State dictionary.
        """
        return {
            'position': self.position,
            'orientation': self.orientation,
            'velocity': self.velocity,
            'angular_velocity': self.angular_velocity,
            'energy': self.energy,
            'health': self.health,
            'age': self.age,
            'is_moving': self.is_moving,
            'is_turning': self.is_turning,
            'last_action_time': self.last_action_time,
            'goal': self.goal,
        }


# Convenience functions

def createAgentBrain(brain) -> AgentBrain:
    """
    Create an agent brain.

    Args:
        brain: The brain to create agent for.

    Returns:
        AgentBrain instance.
    """
    return AgentBrain(brain)


def createAgentSystem():
    """
    Create a complete agent system.

    Returns:
        Tuple of (AgentBrain, World, AgentManager).
    """
    from ._cpp import SimpleWorld, createDefaultConfig, Brain

    config = createDefaultConfig()
    brain = Brain(config)
    world = SimpleWorld()
    agent_brain = AgentBrain(brain)

    manager = AgentManager(agent_brain)

    return agent_brain, world, manager


def createSensoryMotorProcessor(agent_brain: AgentBrain) -> SensoryMotorProcessor:
    """
    Create sensory-motor processor.

    Args:
        agent_brain: The agent brain to process for.

    Returns:
        SensoryMotorProcessor instance.
    """
    return SensoryMotorProcessor(agent_brain)


def createNeuralController(agent_brain: AgentBrain) -> NeuralController:
    """
    Create neural controller.

    Args:
        agent_brain: The agent brain to control.

    Returns:
        NeuralController instance.
    """
    return NeuralController(agent_brain)


def createMotorOutput() -> MotorOutput:
    """
    Create motor output controller.

    Returns:
        MotorOutput instance.
    """
    return MotorOutput()


# Export commonly used classes and functions
__all__.extend([
    'AgentManager', 'SensoryMotorProcessor', 'NeuralController', 'MotorOutput',
    'AgentState', 'createAgentBrain', 'createAgentSystem',
    'createSensoryMotorProcessor', 'createNeuralController', 'createMotorOutput'
])