"""
NLM Python Interface
===================

A high-level, Pythonic interface for the NLM (Neural Learning Machine) framework.

This module provides an enhanced, user-friendly API for working with NLM,
abstracting away the low-level C++ details and providing intuitive Python interfaces
for common use cases.

Key Features:
- Pythonic API design with snake_case method names
- High-level convenience functions
- Type hints and comprehensive documentation
- Error handling and validation
- Automatic resource management
- Advanced functionality for research and development

Usage:
    import pynlm
    # or
    from pynlm import Brain, Config, SimpleWorld, etc.
    
    # Quick start example:
    brain = Brain()
    config = Config()
    world = SimpleWorld()
"""

from typing import Optional, List, Tuple, Dict, Any, Union
from enum import Enum
import warnings

# Import core NLM classes
from .pynlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Main classes
    Config, Brain, AgentBrain, SimpleWorld,
    
    # Sensory input types
    SensoryInput, Vision, Audio, InternalSignals,
    
    # Motor types
    Action,
    
    # World objects
    WorldObject, AgentBody, ActionResult, SensoryPercept,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Global constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Configuration source enum
    ConfigSource
)

__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Main classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    
    # Sensory input types
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    
    # Motor types
    'Action',
    
    # World objects
    'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Global constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Configuration source
    'ConfigSource',
    
    # High-level API
    'NLMEnvironment', 'Agent', 'Experiment',
]


class NLMEnvironment:
    """
    High-level interface for NLM simulation environments.
    
    This class provides a unified interface for setting up and managing
    NLM simulations with sensible defaults and advanced configuration options.
    
    Examples:
        # Create a simple environment
        env = NLMEnvironment(width=80, height=80, vision_size=16)
        
        # Configure learning parameters
        env.setup_for_learning(
            neuron_count=500,
            learning_rate=0.01,
            enable_curiosity=True,
            enable_development=True
        )
        
        # Run an episode
        agent = env.create_agent()
        state = env.reset()
        for step in range(100):
            action = agent.act(state)
            state, reward, done = env.step(action)
            if done:
                break
    """
    
    def __init__(self, width: int = 100, height: int = 100, 
                 vision_width: int = 32, vision_height: int = 32):
        """
        Initialize NLM environment.
        
        Args:
            width: World width in units
            height: World height in units  
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self.world = SimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        
        self.config = createDefaultConfig()
        self.brain = None
        self.agent = None
        
        # Default configuration
        self._default_config()
    
    def _default_config(self):
        """Set up default configuration."""
        self.config.set("simulation_timestep", 0.001)
        self.config.set("neuron_count", 300)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.12)
        self.config.set("max_energy", 50.0)
        self.config.set("energy_decay_rate", 0.02)
    
    def setup_for_learning(self, neuron_count: int = 500, 
                          learning_rate: float = 0.01,
                          enable_curiosity: bool = True,
                          enable_development: bool = True,
                          enable_reward_modulation: bool = True,
                          enable_structural_plasticity: bool = False,
                          enable_prediction: bool = True,
                          enable_cognition: bool = True):
        """
        Configure environment for learning scenarios.
        
        Args:
            neuron_count: Number of neurons in the brain
            learning_rate: Base learning rate for plasticity
            enable_curiosity: Enable curiosity-driven exploration
            enable_development: Enable developmental changes
            enable_reward_modulation: Enable reward-based learning
            enable_structural_plasticity: Enable structural changes
            enable_prediction: Enable prediction system
            enable_cognition: Enable cognitive functions
        """
        self.config.set("neuron_count", neuron_count)
        self.config.set("region_count", max(2, neuron_count // 150))  # Scale regions with neurons
        
        # Plasticity parameters
        self.config.set("stdp_ltp_weight", learning_rate)
        self.config.set("stdp_ltd_weight", learning_rate * 0.9)
        self.config.set("stdp_tau", 20.0)
        self.config.set("synaptogenesis_rate", 0.0001 if enable_structural_plasticity else 0.0)
        self.config.set("pruning_rate", 0.00001 if enable_structural_plasticity else 0.0)
        
        # Enable development
        self.config.set("enable_development", enable_development)
        
        # Initialize brain
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = Agent(self.brain, self.world)
    
    def setup_for_behavior(self, neuron_count: int = 200,
                          energy_efficiency: bool = True,
                          stable_behavior: bool = True):
        """
        Configure environment for stable behavior and efficiency.
        
        Args:
            neuron_count: Number of neurons
            energy_efficiency: Optimize for energy efficiency
            stable_behavior: Prioritize stable behavior over exploration
        """
        self.config.set("neuron_count", neuron_count)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.08)
        self.config.set("max_energy", 30.0 if energy_efficiency else 50.0)
        self.config.set("energy_decay_rate", 0.05 if energy_efficiency else 0.02)
        
        # Reduce plasticity for stability
        self.config.set("stdp_ltp_weight", 0.01)
        self.config.set("stdp_ltd_weight", 0.009)
        self.config.set("synaptogenesis_rate", 0.00001)
        
        self.brain = createBrain(self.config)
        self.brain.initialize()
        self.agent = Agent(self.brain, self.world)
    
    def create_agent(self, agent_type: str = "default") -> 'Agent':
        """
        Create an agent for the environment.
        
        Args:
            agent_type: Type of agent ('default', 'curious', 'conservative', etc.)
            
        Returns:
            Configured Agent instance
        """
        if self.brain is None:
            raise RuntimeError("Brain not initialized. Call setup_for_learning() or setup_for_behavior() first.")
        
        return Agent(self.brain, self.world, agent_type)
    
    def reset(self) -> Dict[str, Any]:
        """
        Reset the environment to initial state.
        
        Returns:
            Dictionary containing initial state information
        """
        if self.brain:
            self.brain.reset()
            self.brain.initialize()
        
        self.world.reset()
        
        # Set random starting position
        import random
        self.world.setAgentStart(random.uniform(10, self.world.getWidth() - 10),
                                 random.uniform(10, self.world.getHeight() - 10))
        
        state = self._get_state()
        return state
    
    def step(self, action: Union[ActionType, MotorCommand]) -> Tuple[Dict[str, Any], float, bool]:
        """
        Execute one simulation step.
        
        Args:
            action: Action to execute
            
        Returns:
            Tuple of (next_state, reward, done)
        """
        if not self.agent:
            raise RuntimeError("Agent not created. Call create_agent() first.")
        
        # Process action through agent
        self.agent.execute_action(action)
        
        # Update world
        self.world.update(self.config.getOr("simulation_timestep", 0.001))
        
        # Get new state
        next_state = self._get_state()
        
        # Calculate reward
        reward = self._calculate_reward(action)
        
        # Check if episode is done
        done = self._check_episode_end()
        
        return next_state, reward, done
    
    def _get_state(self) -> Dict[str, Any]:
        """Get current environment state."""
        if not self.brain:
            return {}
        
        state = {
            'perception': self.world.getSensoryPercept(),
            'agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'agent_velocity': (self.world.getAgentBody().velocityX, self.world.getAgentBody().velocityY),
            'agent_orientation': self.world.getAgentBody().orientation,
            'agent_health': self.world.getAgentBody().health,
            'agent_energy': self.world.getAgentBody().energy,
            'agent_age': self.world.getAgentBody().age,
        }
        
        # Add brain statistics
        if self.brain:
            state['brain_stats'] = {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
                'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0.0,
                'development_stage': self.agent.getDevelopmentalStage() if self.agent else DevelopmentalStage.Initial,
            }
        
        return state
    
    def _calculate_reward(self, action: Union[ActionType, MotorCommand]) -> float:
        """Calculate reward for executed action."""
        reward = 0.0
        
        # Base reward for action
        reward += 0.1
        
        # Additional reward based on action type
        if isinstance(action, ActionType):
            if action in [ActionType.Eat, ActionType.Drink]:
                reward += 1.0
            elif action == ActionType.Interact:
                reward += 0.5
        
        # Penalty for low energy
        if self.world.getAgentBody().energy < 20:
            reward -= 0.1
        
        # Bonus for good health
        if self.world.getAgentBody().health > 80:
            reward += 0.2
        
        return reward
    
    def _check_episode_end(self) -> bool:
        """Check if episode should end."""
        agent = self.world.getAgentBody()
        
        # End if agent is dead
        if agent.health <= 0:
            return True
        
        # End if agent is out of bounds
        if (agent.x < 0 or agent.x >= self.world.getWidth() or
            agent.y < 0 or agent.y >= self.world.getHeight()):
            return True
        
        # End if agent has been too long (default 1000 steps)
        if agent.age > 1000:
            return True
        
        return False
    
    def render(self) -> str:
        """
        Render current environment state as text.
        
        Returns:
            Text representation of environment
        """
        if not self.brain:
            return "Environment not initialized"
        
        lines = []
        lines.append("=" * 50)
        lines.append("NLM Environment State")
        lines.append("=" * 50)
        
        # Agent position
        agent = self.world.getAgentBody()
        lines.append(f"Agent: ({agent.x:.1f}, {agent.y:.1f}) "
                    f"Health: {agent.health:.1f} Energy: {agent.energy:.1f}")
        
        # World dimensions
        lines.append(f"World: {self.world.getWidth()} x {self.world.getHeight()}")
        
        # Brain statistics
        if self.brain:
            lines.append(f"Brain: {self.brain.getTotalNeuronCount()} neurons, "
                        f"{self.brain.getTotalSynapseCount()} synapses")
            lines.append(f"Activity: {self.brain.getTotalSpikeCount()} spikes, "
                        f"{self.brain.getFiringNeuronCount()} firing neurons")
        
        # Vision grid (simplified)
        perception = self.world.getSensoryPercept()
        vision = perception.getVision()
        if vision:
            lines.append("Vision (grayscale):")
            for y in range(min(10, perception.getVisionHeight())):
                row = ""
                for x in range(min(10, perception.getVisionWidth())):
                    idx = y * perception.getVisionWidth() + x
                    if idx < len(vision):
                        val = int(vision[idx] * 10)
                        row += "#" if val > 0 else "."
                    else:
                        row += "."
                lines.append(row)
        
        lines.append("=" * 50)
        
        return "\n".join(lines)


class Agent:
    """
    High-level interface for NLM agents.
    
    This class provides a simplified interface for controlling NLM agents,
    abstracting away the low-level brain interactions and providing
    intuitive methods for action selection and learning.
    
    Examples:
        # Create an agent
        agent = Agent(brain, world, "curious")
        
        # Act based on current state
        action = agent.act(state)
        
        # Update after action
        state, reward, done = env.step(action)
    """
    
    def __init__(self, brain: Brain, world: SimpleWorld, 
                 agent_type: str = "default"):
        """
        Initialize agent.
        
        Args:
            brain: NLM brain instance
            world: World instance
            agent_type: Type of agent ('default', 'curious', 'conservative')
        """
        self.brain = brain
        self.world = world
        self.agent_brain = createAgentBrain(brain)
        self.agent_brain.initialize(world)
        self.agent_type = agent_type
        
        # Configure agent based on type
        self._configure_agent_type()
    
    def _configure_agent_type(self):
        """Configure agent based on type."""
        if self.agent_type == "curious":
            self.agent_brain.enableCuriosity(True)
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableDevelopment(True)
        
        elif self.agent_type == "conservative":
            self.agent_brain.enableCuriosity(False)
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableDevelopment(True)
        
        elif self.agent_type == "exploratory":
            self.agent_brain.enableCuriosity(True)
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableDevelopment(True)
            self.agent_brain.enableStructuralPlasticity(True)
        
        else:  # default
            self.agent_brain.enableCuriosity(True)
            self.agent_brain.enableRewardModulation(True)
            self.agent_brain.enableDevelopment(True)
    
    def act(self, state: Optional[Dict[str, Any]] = None) -> Union[ActionType, MotorCommand]:
        """
        Select action based on current state.
        
        Args:
            state: Current state (if None, uses current world state)
            
        Returns:
            Selected action
        """
        # If no state provided, get current world state
        if state is None:
            state = self._get_state()
        
        # Process sensory input
        if state:
            self.agent_brain.processSensoryInput(state.get('perception', self.world.getSensoryPercept()))
        
        # Apply neuromodulation based on state
        if state:
            brain_stats = state.get('brain_stats', {})
            reward = brain_stats.get('reward', 0.0)
            predicted_reward = brain_stats.get('predicted_reward', 0.0)
            self.agent_brain.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        timestep = self.brain.getConfig().getOr("simulation_timestep", 0.001)
        self.agent_brain.updateDevelopment(timestep)
        
        # Select action based on current brain state
        action = self.agent_brain.decodeMotorCommand()
        
        # For curious agents, add some randomness
        if self.agent_type == "curious" and self.agent_brain.getCuriosityLevel() > 0.5:
            import random
            if random.random() < self.agent_brain.getCuriosityLevel() * 0.3:
                action = self._get_random_action()
        
        return action
    
    def execute_action(self, action: Union[ActionType, MotorCommand]) -> ActionResult:
        """
        Execute action in world.
        
        Args:
            action: Action to execute
            
        Returns:
            Action result
        """
        return self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def _get_state(self) -> Dict[str, Any]:
        """Get current agent state."""
        return {
            'perception': self.world.getSensoryPercept(),
            'neuromodulation': self.getNeuromodulationLevel(),
            'curiosity': self.getCuriosityLevel(),
            'novelty': self.getNoveltyLevel(),
            'prediction_error': self.getPredictionError(),
            'developmental_stage': self.getDevelopmentalStage(),
        }
    
    def _get_random_action(self) -> Union[ActionType, MotorCommand]:
        """Get random action for exploration."""
        import random
        
        actions = [
            MotorCommand.MoveForward,
            MotorCommand.MoveBackward,
            MotorCommand.TurnLeft,
            MotorCommand.TurnRight,
            MotorCommand.Interact,
            MotorCommand.LookLeft,
            MotorCommand.LookRight,
            MotorCommand.Wait,
        ]
        
        return random.choice(actions)
    
    # Agent state getters
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return self.agent_brain.getNeuromodulationLevel()
    
    def getCuriosityLevel(self) -> float:
        """Get current curiosity level."""
        return self.agent_brain.getCuriosityLevel()
    
    def getNoveltyLevel(self) -> float:
        """Get current novelty level."""
        return self.agent_brain.getNoveltyLevel()
    
    def getPredictionError(self) -> float:
        """Get current prediction error."""
        return self.agent_brain.getPredictionError()
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.agent_brain.getDevelopmentalStage()
    
    def getBrain(self) -> Brain:
        """Get underlying brain."""
        return self.agent_brain.getBrain()
    
    def getWorld(self) -> SimpleWorld:
        """Get associated world."""
        return self.world
    
    def reset(self):
        """Reset agent state."""
        self.agent_brain.reset()
    
    def enable_learning(self, enable: bool = True):
        """Enable or disable learning."""
        self.agent_brain.enableRewardModulation(enable)
        self.agent_brain.enableCuriosity(enable)
        self.agent_brain.enableDevelopment(enable)
    
    def set_curiosity(self, level: float):
        """Set curiosity level (0.0 to 1.0)."""
        # Note: This is a simplified example; actual implementation would depend
        # on the neuromodulation system architecture
        if level < 0.0 or level > 1.0:
            raise ValueError("Curiosity level must be between 0.0 and 1.0")


class Experiment:
    """
    High-level interface for running NLM experiments.
    
    This class provides tools for running systematic experiments with NLM agents,
    collecting data, and analyzing results.
    
    Examples:
        # Create an experiment
        experiment = Experiment("Curiosity vs Conservation")
        
        # Run multiple conditions
        for agent_type in ["curious", "conservative", "exploratory"]:
            metrics = experiment.run(agent_type, max_episodes=100)
            experiment.record_results(agent_type, metrics)
        
        # Analyze results
        analysis = experiment.analyze()
        experiment.plot_results()
    """
    
    def __init__(self, name: str):
        """
        Initialize experiment.
        
        Args:
            name: Name of the experiment
        """
        self.name = name
        self.results = {}
        self.environment = None
    
    def create_environment(self, env_type: str = "default", **kwargs) -> NLMEnvironment:
        """
        Create experiment environment.
        
        Args:
            env_type: Type of environment ('default', 'simple', 'complex')
            **kwargs: Environment configuration parameters
            
        Returns:
            Configured NLMEnvironment
        """
        if env_type == "default":
            self.environment = NLMEnvironment(**kwargs)
        elif env_type == "simple":
            self.environment = NLMEnvironment(width=50, height=50, vision_width=8, vision_height=8)
        elif env_type == "complex":
            self.environment = NLMEnvironment(width=100, height=100, vision_width=20, vision_height=20)
        
        return self.environment
    
    def run(self, agent_type: str, max_episodes: int = 100, 
            max_steps: int = 1000, headless: bool = True) -> Dict[str, Any]:
        """
        Run experiment with specified agent type.
        
        Args:
            agent_type: Type of agent ('curious', 'conservative', 'exploratory')
            max_episodes: Maximum number of episodes
            max_steps: Maximum steps per episode
            headless: Whether to suppress output
            
        Returns:
            Dictionary containing experiment metrics
        """
        if not self.environment:
            self.create_environment()
        
        # Setup environment for specified agent type
        self.environment.setup_for_learning(
            neuron_count=300,
            enable_curiosity=(agent_type in ["curious", "exploratory"]),
            enable_development=True,
            enable_reward_modulation=True
        )
        
        # Run episodes
        episode_results = []
        
        for episode in range(max_episodes):
            # Reset environment
            state = self.environment.reset()
            
            # Create agent
            agent = self.environment.create_agent(agent_type)
            
            # Run episode
            episode_reward = 0.0
            episode_steps = 0
            episode_success = False
            
            for step in range(max_steps):
                # Select action
                action = agent.act(state)
                
                # Execute action
                next_state, reward, done = self.environment.step(action)
                
                episode_reward += reward
                episode_steps += 1
                
                if done:
                    if agent.getWorld().getAgentBody().health > 0:
                        episode_success = True
                    break
                
                state = next_state
            
            # Store episode results
            episode_result = {
                'episode': episode,
                'agent_type': agent_type,
                'steps': episode_steps,
                'reward': episode_reward,
                'success': episode_success,
                'final_health': agent.getWorld().getAgentBody().health,
                'final_energy': agent.getWorld().getAgentBody().energy,
                'neuromodulation': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'development_stage': agent.getDevelopmentalStage().name,
            }
            
            episode_results.append(episode_result)
            
            if not headless and (episode + 1) % 10 == 0:
                print(f"Episode {episode + 1}/{max_episodes}: "
                      f"Reward: {episode_reward:.2f}, "
                      f"Steps: {episode_steps}, "
                      f"Success: {episode_success}")
        
        # Calculate metrics
        metrics = self._calculate_metrics(episode_results)
        
        # Store results
        self.results[agent_type] = {
            'episodes': episode_results,
            'metrics': metrics,
            'agent_type': agent_type,
        }
        
        if not headless:
            print(f"\nExperiment '{self.name}' completed for agent type '{agent_type}'")
            print(f"Metrics: {metrics}")
        
        return metrics
    
    def _calculate_metrics(self, episode_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate experiment metrics."""
        if not episode_results:
            return {}
        
        rewards = [ep['reward'] for ep in episode_results]
        steps = [ep['steps'] for ep in episode_results]
        successes = [ep['success'] for ep in episode_results]
        neuromodulation = [ep['neuromodulation'] for ep in episode_results]
        curiosity = [ep['curiosity'] for ep in episode_results]
        
        return {
            'total_episodes': len(episode_results),
            'mean_reward': sum(rewards) / len(rewards),
            'max_reward': max(rewards),
            'min_reward': min(rewards),
            'mean_steps': sum(steps) / len(steps),
            'success_rate': sum(successes) / len(successes),
            'mean_neuromodulation': sum(neuromodulation) / len(neuromodulation),
            'mean_curiosity': sum(curiosity) / len(curiosity),
        }
    
    def record_results(self, agent_type: str, metrics: Dict[str, Any]):
        """
        Record experiment results.
        
        Args:
            agent_type: Agent type identifier
            metrics: Metrics to record
        """
        if agent_type not in self.results:
            self.results[agent_type] = {}
        
        self.results[agent_type]['recorded_metrics'] = metrics
    
    def analyze(self) -> Dict[str, Any]:
        """
        Analyze experiment results.
        
        Returns:
            Dictionary containing analysis results
        """
        if not self.results:
            return {}
        
        analysis = {
            'experiment_name': self.name,
            'agent_types': list(self.results.keys()),
            'total_episodes': 0,
            'comparison': {},
        }
        
        for agent_type, data in self.results.items():
            if 'episodes' in data:
                analysis['total_episodes'] += len(data['episodes'])
        
        # Compare agent types if multiple
        if len(self.results) > 1:
            comparison = {}
            for agent_type1 in self.results:
                for agent_type2 in self.results:
                    if agent_type1 != agent_type2 and agent_type1 < agent_type2:
                        key = f"{agent_type1}_vs_{agent_type2}"
                        comparison[key] = self._compare_agents(agent_type1, agent_type2)
            
            analysis['comparison'] = comparison
        
        return analysis
    
    def _compare_agents(self, agent_type1: str, agent_type2: str) -> Dict[str, Any]:
        """Compare two agent types."""
        if agent_type1 not in self.results or agent_type2 not in self.results:
            return {}
        
        metrics1 = self.results[agent_type1].get('metrics', {})
        metrics2 = self.results[agent_type2].get('metrics', {})
        
        comparison = {}
        for key in metrics1:
            if key in metrics2:
                comparison[key] = {
                    'type1_value': metrics1[key],
                    'type2_value': metrics2[key],
                    'difference': metrics1[key] - metrics2[key],
                    'improvement': 'better' if metrics1[key] > metrics2[key] else 'worse'
                    if isinstance(metrics1[key], (int, float)) else 'equal'
                }
        
        return comparison
    
    def plot_results(self, output_dir: str = ".", show_plots: bool = False):
        """
        Plot experiment results.
        
        Args:
            output_dir: Directory to save plots
            show_plots: Whether to display plots
        """
        try:
            import matplotlib.pyplot as plt
            import matplotlib as mpl
            
            # Set up plotting style
            mpl.style.use('seaborn-v0_8')
            
            # Create figure
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle(f"Experiment Results: {self.name}", fontsize=16)
            
            if len(self.results) > 1:
                agent_types = list(self.results.keys())
                
                # Reward comparison
                ax = axes[0, 0]
                for agent_type in agent_types:
                    episodes = self.results[agent_type]['episodes']
                    rewards = [ep['reward'] for ep in episodes]
                    ax.plot(rewards, label=agent_type, alpha=0.7)
                
                ax.set_xlabel('Episode')
                ax.set_ylabel('Reward')
                ax.set_title('Episode Rewards')
                ax.legend()
                ax.grid(True, alpha=0.3)
                
                # Success rate
                ax = axes[0, 1]
                for agent_type in agent_types:
                    episodes = self.results[agent_type]['episodes']
                    success_rate = []
                    cumulative_success = 0
                    for i, ep in enumerate(episodes):
                        if ep['success']:
                            cumulative_success += 1
                        success_rate.append(cumulative_success / (i + 1))
                    
                    ax.plot(success_rate, label=agent_type, alpha=0.7)
                
                ax.set_xlabel('Episode')
                ax.set_ylabel('Cumulative Success Rate')
                ax.set_title('Success Rate Over Time')
                ax.legend()
                ax.grid(True, alpha=0.3)
                
                # Neuromodulation levels
                ax = axes[1, 0]
                for agent_type in agent_types:
                    episodes = self.results[agent_type]['episodes']
                    neuromodulation = [ep['neuromodulation'] for ep in episodes]
                    ax.plot(neuromodulation, label=agent_type, alpha=0.7)
                
                ax.set_xlabel('Episode')
                ax.set_ylabel('Neuromodulation Level')
                ax.set_title('Neuromodulation Levels')
                ax.legend()
                ax.grid(True, alpha=0.3)
                
                # Curiosity levels
                ax = axes[1, 1]
                for agent_type in agent_types:
                    episodes = self.results[agent_type]['episodes']
                    curiosity = [ep['curiosity'] for ep in episodes]
                    ax.plot(curiosity, label=agent_type, alpha=0.7)
                
                ax.set_xlabel('Episode')
                ax.set_ylabel('Curiosity Level')
                ax.set_title('Curiosity Levels')
                ax.legend()
                ax.grid(True, alpha=0.3)
            
            else:
                # Single agent type - show detailed episode metrics
                ax = axes[0, 0]
                episodes = self.results[agent_types[0]]['episodes']
                rewards = [ep['reward'] for ep in episodes]
                ax.plot(rewards)
                ax.set_xlabel('Episode')
                ax.set_ylabel('Reward')
                ax.set_title('Episode Rewards')
                ax.grid(True, alpha=0.3)
                
                # Success rate
                ax = axes[0, 1]
                success_rate = []
                cumulative_success = 0
                for i, ep in enumerate(episodes):
                    if ep['success']:
                        cumulative_success += 1
                    success_rate.append(cumulative_success / (i + 1))
                ax.plot(success_rate)
                ax.set_xlabel('Episode')
                ax.set_ylabel('Cumulative Success Rate')
                ax.set_title('Success Rate Over Time')
                ax.grid(True, alpha=0.3)
                
                # Other plots can be added as needed
                axes[1, 0].text(0.5, 0.5, 'Add additional plots here',
                               ha='center', va='center', transform=axes[1, 0].transAxes)
                axes[1, 1].text(0.5, 0.5, 'Add additional plots here',
                               ha='center', va='center', transform=axes[1, 1].transAxes)
            
            # Adjust layout and save
            plt.tight_layout()
            
            # Save figure
            import os
            os.makedirs(output_dir, exist_ok=True)
            filename = os.path.join(output_dir, f"{self.name.replace(' ', '_')}_results.png")
            plt.savefig(filename, dpi=300, bbox_inches='tight')
            
            if show_plots:
                plt.show()
            
            plt.close()
            
            print(f"Results plot saved to: {filename}")
            
        except ImportError:
            print("Matplotlib not available. Install matplotlib to generate plots.")
        except Exception as e:
            print(f"Error plotting results: {e}")
    
    def save_results(self, filepath: str):
        """
        Save experiment results to file.
        
        Args:
            filepath: Path to save results file
        """
        import json
        from datetime import datetime
        
        # Prepare data for JSON serialization
        serializable_results = {}
        for agent_type, data in self.results.items():
            serializable_results[agent_type] = {
                'episodes': data['episodes'],
                'metrics': data.get('metrics', {}),
                'agent_type': agent_type,
                'timestamp': datetime.now().isoformat(),
            }
        
        # Save to file
        with open(filepath, 'w') as f:
            json.dump(serializable_results, f, indent=2, default=str)
        
        print(f"Results saved to: {filepath}")
    
    def load_results(self, filepath: str):
        """
        Load experiment results from file.
        
        Args:
            filepath: Path to load results file
        """
        import json
        
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.results = data
        print(f"Results loaded from: {filepath}")


# Convenience functions for common use cases
def quick_simulation(width: int = 80, height: int = 80, 
                    vision_width: int = 16, vision_height: int = 16,
                    episodes: int = 5) -> Dict[str, Any]:
    """
    Run a quick simulation with sensible defaults.
    
    This is a convenience function for getting started with NLM.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        episodes: Number of episodes to run
        
    Returns:
        Dictionary containing simulation results
    """
    print("Running quick simulation...")
    
    # Create environment
    env = NLMEnvironment(width, height, vision_width, vision_height)
    env.setup_for_learning(neuron_count=200, enable_curiosity=True)
    
    all_results = []
    
    for episode in range(episodes):
        print(f"Episode {episode + 1}/{episodes}")
        
        # Create agent
        agent = env.create_agent("curious")
        
        # Reset environment
        state = env.reset()
        
        # Run episode
        for step in range(100):
            action = agent.act(state)
            next_state, reward, done = env.step(action)
            state = next_state
            
            if done:
                print(f"  Episode ended after {step + 1} steps")
                break
        
        # Store results
        result = {
            'episode': episode,
            'agent_type': 'curious',
            'final_health': agent.getWorld().getAgentBody().health,
            'final_energy': agent.getWorld().getAgentBody().energy,
            'neuromodulation': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
        }
        
        all_results.append(result)
    
    print(f"Quick simulation completed with {episodes} episodes")
    return {
        'results': all_results,
        'environment': env,
    }


def create_test_brain() -> Brain:
    """
    Create a test brain with optimized configuration for testing.
    
    Returns:
        Configured Brain instance
    """
    config = createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    
    brain = createBrain(config)
    brain.initialize()
    
    return brain


def create_test_world(width: int = 50, height: int = 50,
                     vision_width: int = 8, vision_height: int = 8) -> SimpleWorld:
    """
    Create a test world with sensible defaults.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        
    Returns:
        Configured SimpleWorld
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world


# Error handling and validation functions
def validate_configuration(config: Config) -> List[str]:
    """
    Validate configuration and return list of warnings.
    
    Args:
        config: Configuration to validate
        
    Returns:
        List of warning messages
    """
    warnings = []
    
    # Check neuron count
    neuron_count = config.getOr("neuron_count", 100)
    if neuron_count < 10:
        warnings.append("Neuron count is very low. Consider increasing for better learning.")
    elif neuron_count > 10000:
        warnings.append("Neuron count is very high. Performance may be affected.")
    
    # Check region count
    region_count = config.getOr("region_count", 2)
    if region_count < 1:
        warnings.append("Region count should be at least 1.")
    
    # Check connection probability
    conn_prob = config.getOr("connection_probability", 0.15)
    if conn_prob < 0.01:
        warnings.append("Connection probability is very low. Network may be too sparse.")
    elif conn_prob > 0.5:
        warnings.append("Connection probability is very high. Network may be too dense.")
    
    # Check timestep
    timestep = config.getOr("simulation_timestep", 0.001)
    if timestep <= 0:
        warnings.append("Simulation timestep must be positive.")
    
    return warnings


def safe_step(brain: Brain, world: SimpleWorld, 
              max_steps: int = 1000, timestep: float = 0.001) -> Dict[str, Any]:
    """
    Safely run simulation steps with error handling.
    
    Args:
        brain: Brain instance
        world: World instance
        max_steps: Maximum number of steps
        timestep: Simulation timestep
        
    Returns:
        Dictionary containing simulation results
    """
    results = {
        'steps_completed': 0,
        'total_spikes': 0,
        'active_neurons': [],
        'errors': [],
        'completed_successfully': True,
    }
    
    try:
        for step in range(max_steps):
            # Check for errors
            if brain.getTotalNeuronCount() == 0:
                results['errors'].append(f"Step {step}: No neurons in brain")
                break
            
            # Execute step
            brain.step(step, step * timestep)
            
            # Record statistics
            results['total_spikes'] += brain.getTotalSpikeCount()
            results['active_neurons'].append(brain.getActiveNeuronCount())
            
            results['steps_completed'] = step + 1
            
            # Check for catastrophic errors
            if brain.getActiveNeuronCount() > brain.getTotalNeuronCount() * 0.95:
                results['errors'].append(f"Step {step}: Excessive neuron activity")
            
    except Exception as e:
        results['errors'].append(f"Step {step}: {str(e)}")
        results['completed_successfully'] = False
    
    return results


# Performance monitoring functions
def monitor_performance(brain: Brain, duration_steps: int = 100,
                       sampling_interval: int = 10) -> Dict[str, Any]:
    """
    Monitor brain performance over time.
    
    Args:
        brain: Brain instance
        duration_steps: Number of steps to monitor
        sampling_interval: Steps between samples
        
    Returns:
        Dictionary containing performance metrics
    """
    performance = {
        'sampling_interval': sampling_interval,
        'samples': [],
        'spike_rate': [],
        'activity_level': [],
        'neurons_active': [],
        'neurons_firing': [],
    }
    
    for step in range(duration_steps):
        if step % sampling_interval == 0:
            # Record metrics
            sample = {
                'step': step,
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            }
            
            performance['samples'].append(sample)
            performance['spike_rate'].append(brain.getTotalSpikeCount() / (step + 1))
            performance['activity_level'].append(brain.getActiveNeuronCount() / max(brain.getTotalNeuronCount(), 1))
            performance['neurons_active'].append(brain.getActiveNeuronCount())
            performance['neurons_firing'].append(brain.getFiringNeuronCount())
        
        # Execute step
        brain.step(step, step * 0.001)
    
    return performance


# Export all important symbols
__version__ = "1.0.0"
__author__ = "NLM Project"
__description__ = "Python interface for Neural Learning Machine (NLM)"

# Set up default logging
import logging
logging.basicConfig(level=logging.WARNING)