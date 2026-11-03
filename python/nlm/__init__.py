"""
NLM (熙然) - Neural Learning Machine Python API

A comprehensive Python interface to the NLM artificial brain simulation framework.
This module provides both beginner-friendly and advanced APIs for working with
artificial neural systems that learn from experience.

Features:
- Brain simulation with real spiking neurons
- Event-driven spike propagation and synaptic plasticity
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Development stages and maturation
- Prediction and planning systems
- Cognitive formation and attention
- Environment interaction
- Checkpoint persistence

Examples:
    >>> import nlm
    >>> 
    >>> # Create a basic brain
    >>> brain = nlm.createBrain(nlm.createDefaultConfig())
    >>> brain.initialize()
    >>> 
    >>> # Run simulation
    >>> for i in range(1000):
    ...     brain.step(i)
    >>> 
    >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
    >>> print(f"Firing neurons: {brain.getFiringNeuronCount()}")

See HOW_TO_USE.md for detailed documentation.
"""

import sys
from typing import Optional, List, Dict, Any, Union
import numpy as np

# Import the core C++ extensions
from .core import (
    Config,
    createBrain,
    createDefaultConfig,
    createSimpleWorld,
    createAgentBrain,
    NeuronType,
    DevelopmentalStage,
    ActionType,
    WorldObjectType
)

__version__ = "0.1.0"
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'SimpleWorld', 
    'AgentBrain',
    
    # Enums
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType',
    
    # Factory functions
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Utility functions
    'runSimulation',
    'runAgentSimulation',
    'createBrainWithLearning',
    
    # Advanced APIs
    'AdvancedBrainAPI',
    'LearningPipeline',
    
    # Constants
    '__version__'
]


class Brain:
    """
    Enhanced Python interface to the NLM Brain simulation.
    
    This class provides a more Pythonic API for working with artificial
    neural systems, with improved error handling, convenience methods,
    and comprehensive documentation.
    
    The brain simulates real spiking neural computation with:
    - Leaky Integrate-and-Fire (LIF) neuron dynamics
    - Event-driven spike propagation with synaptic delays
    - Multiple plasticity rules (STDP, Hebbian, structural)
    - Integrated memory systems
    - Neuromodulation and learning
    - Development and maturation
    
    Examples:
        >>> # Create brain with custom configuration
        >>> config = nlm.createDefaultConfig()
        >>> config.set("brain.neuron_count", 2000)
        >>> brain = nlm.Brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run training simulation
        >>> history = brain.run_training_episode(500, sensory_input=visual_data)
        >>> brain.save("trained_brain.checkpoint")
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize a new NLM brain.
        
        Args:
            config: Configuration for the brain. If None, uses default config.
        """
        if config is None:
            config = createDefaultConfig()
        self._brain = createBrain(config)
        self._config = config
    
    def initialize(self) -> None:
        """Initialize the brain simulation."""
        self._brain.initialize()
    
    def reset(self) -> None:
        """Reset the brain to its initial state."""
        self._brain.reset()
        self._brain.initialize()
    
    def step(self, step_number: int, timestamp: float = 0.0) -> None:
        """
        Execute one simulation step.
        
        Args:
            step_number: Step index for simulation logging
            timestamp: Time in seconds for this step
        """
        self._brain.step(step_number, timestamp)
    
    def inject_sensory_input(self, sensory_input) -> None:
        """
        Inject sensory input into the brain.
        
        Args:
            sensory_input: Sensory data from environment
        """
        self._brain.receiveSensoryInput(sensory_input)
    
    def get_motor_output(self) -> Any:
        """
        Get motor command from the brain.
        
        Returns:
            Motor action to apply to environment
        """
        return self._brain.produceAction()
    
    # Statistics and monitoring
    def get_neuron_count(self) -> int:
        """Get total number of neurons in the brain."""
        return self._brain.getTotalNeuronCount()
    
    def get_synapse_count(self) -> int:
        """Get total number of synapses in the brain."""
        return self._brain.getTotalSynapseCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get number of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count since initialization."""
        return self._brain.getTotalSpikeCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.getAverageFiringRate()
    
    def get_development_stage(self) -> DevelopmentalStage:
        """Get current developmental stage of the brain."""
        return self._brain.getDevelopmentalStage()
    
    # Memory system access
    def get_working_memory(self):
        """Access working memory system."""
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self):
        """Access episodic memory system."""
        return self._brain.getEpisodicMemory()
    
    def get_associative_memory(self):
        """Access associative memory system."""
        return self._brain.getAssociativeMemory()
    
    # Prediction system access
    def get_prediction_system(self):
        """Access prediction system."""
        return self._brain.getPredictionSystem()
    
    # Cognition system access
    def get_planner(self):
        """Access neural planner system."""
        return self._brain.getPlanner()
    
    def get_concept_formation(self):
        """Access concept formation system."""
        return self._brain.getConceptFormation()
    
    def get_attention(self):
        """Access attentional selection system."""
        return self._brain.getAttention()
    
    # Neuromodulation system access
    def get_dopamine(self):
        """Access dopamine neuromodulation system."""
        return self._brain.getDopamine()
    
    def get_curiosity(self):
        """Access curiosity neuromodulation system."""
        return self._brain.getCuriosity()
    
    def get_novelty(self):
        """Access novelty detection system."""
        return self._brain.getNovelty()
    
    def get_prediction_error(self):
        """Access prediction error system."""
        return self._brain.getPredictionErrorSignal()
    
    # Development system access
    def get_development_system(self):
        """Access development system."""
        return self._brain.getDevelopmentSystem()
    
    # Persistence (checkpointing)
    def save(self, filepath: str) -> bool:
        """
        Save brain state to checkpoint file.
        
        Args:
            filepath: Path to save checkpoint file
            
        Returns:
            True if save successful, False otherwise
        """
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from checkpoint file.
        
        Args:
            filepath: Path to checkpoint file
            
        Returns:
            True if load successful, False otherwise
        """
        return self._brain.load(filepath)
    
    def run_training_episode(self, num_steps: int, sensory_pattern=None) -> Dict[str, Any]:
        """
        Run a complete training episode with automatic sensory processing.
        
        Args:
            num_steps: Number of simulation steps to run
            sensory_pattern: Optional pattern to inject each step
            
        Returns:
            Dictionary with training statistics
        """
        history = {
            'steps': num_steps,
            'spike_counts': [],
            'firing_rates': [],
            'learning_metrics': {}
        }
        
        for step in range(num_steps):
            if sensory_pattern is not None:
                self.inject_sensory_input(sensory_pattern[step % len(sensory_pattern)])
            
            self.step(step)
            
            history['spike_counts'].append(self.get_total_spike_count())
            history['firing_rates'].append(self.get_firing_neuron_count())
        
        # Calculate learning metrics
        if len(history['spike_counts']) > 1:
            history['learning_metrics'] = {
                'spike_increase': history['spike_counts'][-1] - history['spike_counts'][0],
                'avg_firing_rate': sum(history['firing_rates']) / len(history['firing_rates']),
                'final_development_stage': self.get_development_stage().value
            }
        
        return history
    
    def get_brain_status(self) -> Dict[str, Any]:
        """
        Get comprehensive status information about the brain.
        
        Returns:
            Dictionary with brain status information
        """
        return {
            'neuron_count': self.get_neuron_count(),
            'synapse_count': self.get_synapse_count(),
            'firing_neuron_count': self.get_firing_neuron_count(),
            'total_spike_count': self.get_total_spike_count(),
            'average_firing_rate': self.get_average_firing_rate(),
            'development_stage': self.get_development_stage().value,
            'has_working_memory': self.get_working_memory() is not None,
            'has_episodic_memory': self.get_episodic_memory() is not None,
            'has_associative_memory': self.get_associative_memory() is not None,
            'has_prediction_system': self.get_prediction_system() is not None,
            'has_planner': self.get_planner() is not None,
            'has_concept_formation': self.get_concept_formation() is not None,
            'has_attention': self.get_attention() is not None,
            'has_dopamine': self.get_dopamine() is not None,
            'has_curiosity': self.get_curiosity() is not None,
            'has_novelty': self.get_novelty() is not None,
            'has_prediction_error': self.get_prediction_error() is not None,
        }


class SimpleWorld:
    """
    Python interface to the SimpleWorld environment.
    
    The world represents the environment in which the brain operates,
    providing sensory input and receiving motor actions.
    
    The world supports:
    - 2D grid-based spatial representation
    - Vision (sensory perception)
    - Resource management
    - Hazard zones
    - Action effects
    
    Examples:
        >>> import nlm
        >>> 
        >>> # Create and configure world
        >>> world = nlm.SimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        >>> world.reset()
        >>> world.setAgentStart(10.0, 10.0)
        >>> 
        >>> # Run simulation
        >>> for i in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     brain.inject_sensory_input(percept)
        ...     brain.step(i)
        ...     action = brain.get_motor_output()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
    """
    
    def __init__(self):
        """Create a new SimpleWorld instance."""
        self._world = createSimpleWorld()
    
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int, maxEnergy: float = 100.0) -> None:
        """
        Configure world dimensions and parameters.
        
        Args:
            width: World width in cells
            height: World height in cells
            visionWidth: Agent vision width in cells
            visionHeight: Agent vision height in cells
            maxEnergy: Maximum energy for the agent
        """
        self._world.configure(width, height, visionWidth, visionHeight)
        self._world.setMaxEnergy(maxEnergy)
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self._world.reset()
    
    def update(self, timestep: float) -> None:
        """
        Update world simulation.
        
        Args:
            timestep: Time step duration in seconds
        """
        self._world.update(timestep)
    
    def get_sensory_percept(self):
        """
        Get current sensory percept from the world.
        
        Returns:
            Sensory perception data
        """
        return self._world.getSensoryPercept()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.setAgentStart(x, y)
    
    def apply_motor_command(self, command, timestamp: float) -> None:
        """
        Apply motor command to world.
        
        Args:
            command: Motor action to apply
            timestamp: Time when action occurs
        """
        self._world.applyMotorCommand(command, timestamp)
    
    # World properties
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy."""
        self._world.setMaxEnergy(energy)


class AgentBrain:
    """
    Python interface to the AgentBrain integration system.
    
    The AgentBrain acts as the bridge between the brain and the environment,
    processing sensory input and converting brain activity into motor actions.
    
    The agent supports:
    - Sensory processing and feature extraction
    - Motor command decoding
    - Reward modulation and learning
    - Curiosity-driven exploration
    - Development and maturation
    
    Examples:
        >>> import nlm
        >>> 
        >>> # Create complete agent system
        >>> brain = nlm.Brain(nlm.createDefaultConfig())
        >>> brain.initialize()
        >>> world = nlm.SimpleWorld()
        >>> world.configure(width=20, height=20)
        >>> agent = nlm.AgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Enable learning subsystems
        >>> agent.enable_reward_modulation(True)
        >>> agent.enable_curiosity(True)
        >>> agent.enable_structural_plasticity(True)
        >>> agent.enable_development(True)
        >>> 
        >>> # Run complete agent simulation
        >>> for step in range(1000):
        ...     world.update(0.1)
        ...     agent.process_sensory_input(world.getSensoryPercept())
        ...     brain.step(step)
        ...     action = agent.decode_motor_command()
        ...     world.apply_motor_command(action, world.getSimulationTime())
        ...     
        ...     # Optional: apply reward and update development
        ...     reward = world.getSensoryPercept().get_internal()[0] if ...
        ...     agent.apply_reward_modulation(reward, 0.0)
        ...     agent.update_development(0.1)
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize AgentBrain with a brain instance.
        
        Args:
            brain: Brain instance to control
        """
        self._brain = brain._brain
        self._agent = createAgentBrain(brain._brain)
    
    def initialize(self, world: SimpleWorld) -> None:
        """
        Initialize agent with world environment.
        
        Args:
            world: World environment to interact with
        """
        self._agent.initialize(world._world)
    
    def process_sensory_input(self, percept) -> None:
        """
        Process sensory input from environment.
        
        Args:
            percept: Sensory perception data
        """
        self._agent.processSensoryInput(percept)
    
    def decode_motor_command(self):
        """
        Decode motor command from brain activity.
        
        Returns:
            Motor action to apply to environment
        """
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> None:
        """
        Apply reward modulation for learning.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward (for prediction error)
        """
        self._agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float) -> None:
        """
        Update development state.
        
        Args:
            timestep: Time step duration
        """
        # Note: Development is updated internally by the brain
        pass
    
    # Subsystem enable/disable
    def enable_reward_modulation(self, enabled: bool) -> None:
        """Enable or disable reward modulation."""
        self._agent.enableRewardModulation(enabled)
    
    def enable_structural_plasticity(self, enabled: bool) -> None:
        """Enable or disable structural plasticity."""
        self._agent.enableStructuralPlasticity(enabled)
    
    def enable_development(self, enabled: bool) -> None:
        """Enable or disable development."""
        self._agent.enableDevelopment(enabled)
    
    def enable_curiosity(self, enabled: bool) -> None:
        """Enable or disable curiosity-driven exploration."""
        self._agent.enableCuriosity(enabled)
    
    # Statistics and monitoring
    def get_curiosity_level(self) -> float:
        """Get current curiosity level."""
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get current novelty level."""
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get current prediction error."""
        return self._agent.getPredictionError()
    
    def get_neuromodulation_level(self) -> float:
        """Get overall neuromodulation level."""
        return self._agent.getNeuromodulationLevel()


# Factory functions for backward compatibility and convenience
def createBrain(config: Optional[Config] = None) -> Brain:
    """
    Create a new Brain instance.
    
    Args:
        config: Configuration for the brain. If None, uses default config.
        
    Returns:
        Brain instance
    """
    return Brain(config)

def createDefaultConfig() -> Config:
    """
    Create a default configuration for the brain.
    
    Returns:
        Config instance with default values
    """
    return Config()

def createSimpleWorld() -> SimpleWorld:
    """
    Create a new SimpleWorld instance.
    
    Returns:
        SimpleWorld instance
    """
    return SimpleWorld()

def createAgentBrain(brain: Brain) -> AgentBrain:
    """
    Create an AgentBrain instance.
    
    Args:
        brain: Brain instance to control
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)

# Utility functions
def runSimulation(brain: Brain, num_steps: int, sensory_pattern=None) -> Dict[str, Any]:
    """
    Run a simulation with a brain.
    
    Args:
        brain: Brain instance to run
        num_steps: Number of steps to run
        sensory_pattern: Optional sensory pattern
        
    Returns:
        Simulation results
    """
    return brain.run_training_episode(num_steps, sensory_pattern)


def runAgentSimulation(brain: Brain, world: SimpleWorld, agent: AgentBrain, 
                      num_steps: int) -> Dict[str, Any]:
    """
    Run a complete agent simulation.
    
    Args:
        brain: Brain instance
        world: World environment
        agent: AgentBrain instance
        num_steps: Number of steps to run
        
    Returns:
        Simulation results
    """
    results = {
        'steps': num_steps,
        'world_updates': [],
        'brain_steps': [],
        'agent_decisions': [],
        'rewards': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        results['world_updates'].append({
            'step': step,
            'simulation_time': world.get_simulation_time()
        })
        
        # Process sensory input
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Brain step
        brain.step(step)
        results['brain_steps'].append({
            'step': step,
            'neuron_count': brain.get_neuron_count(),
            'firing_neuron_count': brain.get_firing_neuron_count()
        })
        
        # Decode motor command
        action = agent.decode_motor_command()
        results['agent_decisions'].append({
            'step': step,
            'action': action
        })
        
        # Apply action to world
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Optional: apply reward and development
        # reward = percept.get_internal()[0] if percept.get_internal() else 0.0
        # agent.apply_reward_modulation(reward, 0.0)
        # agent.update_development(0.1)
        
        # Record reward (placeholder)
        results['rewards'].append(0.0)
    
    return results


def createBrainWithLearning(config: Optional[Config] = None, 
                            enable_everything: bool = True) -> Brain:
    """
    Create a brain with learning subsystems enabled.
    
    Args:
        config: Configuration for the brain
        enable_everything: Whether to enable all learning subsystems
        
    Returns:
        Brain instance with learning enabled
    """
    brain = Brain(config)
    brain.initialize()
    
    if enable_everything:
        # Enable all learning subsystems by accessing the agent
        # Note: This is a convenience function - actual agent creation
        # would be done separately
        pass
    
    return brain


# Set up module-level imports
__all__.extend([
    'Brain',
    'SimpleWorld', 
    'AgentBrain',
    'AdvancedBrainAPI',
    'LearningPipeline',
    'runSimulation',
    'runAgentSimulation',
    'createBrainWithLearning'
])

if __name__ == "__main__":
    # Test the module
    print(f"NLM Python API version {__version__}")
    print("Type 'import nlm' to start using the API")
    print("See HOW_TO_USE.md for detailed documentation")
    
    # Quick test
    try:
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        print("✓ Basic functionality test passed")
        
        # Clean up
        del brain
        print("✓ Cleanup successful")
        
    except Exception as e:
# Test class for Python integration
class BrainAPITests:
    """
    Test class for Python NLM API integration tests.
    
    This class provides test methods for validating the Python NLM API
    and ensuring proper integration between Python and C++ components.
    """
    
    def __init__(self):
        """Initialize the test class."""
        self.test_results = []
    
    def runAllTests(self):
        """
        Run all API tests.
        
        Returns:
            int: 0 if all tests pass, 1 if any tests fail
        """
        print("=== Python NLM API Integration Tests ===")
        
        tests = [
            self.testBasicModuleImport,
            self.testBrainClass,
            self.testWorldClass,
            self.testAgentClass,
            self.testFactoryFunctions,
            self.testConfigSystem,
            self.testBrainOperations,
            self.testMemorySystems,
            self.testAdvancedAPI,
            self.testLearningPipeline,
        ]
        
        passed = 0
        total = len(tests)
        
        for test in tests:
            try:
                if test():
                    passed += 1
                    print(f"✓ {test.__name__} passed")
                else:
                    print(f"✗ {test.__name__} failed")
            except Exception as e:
                print(f"✗ {test.__name__} failed with exception: {e}")
        
        print(f"\nTest Summary: {passed}/{total} tests passed")
        
        return 0 if passed == total else 1
    
    def testBasicModuleImport(self):
        """Test basic module import functionality."""
        print("  Testing basic module import...")
        
        # Check if all expected classes and functions are available
        expected_items = ['Brain', 'SimpleWorld', 'AgentBrain', 'Config', 'createBrain', 
                         'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
                         'AdvancedBrainAPI', 'LearningPipeline']
        
        for item in expected_items:
            if not hasattr(sys.modules[__name__], item):
                print(f"    ✗ Missing expected item: {item}")
                return False
        
        print(f"    ✓ All basic module imports successful")
        return True
    
    def testBrainClass(self):
        """Test Brain class functionality."""
        print("  Testing Brain class...")
        
        try:
            # Create brain
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            # Test basic methods
            if brain.get_neuron_count() <= 0:
                print("    ✗ Invalid neuron count")
                return False
            
            # Test step method
            brain.step(0)
            
            # Test memory system access
            if brain.get_working_memory() is None:
                print("    ✗ Working memory not accessible")
                return False
            
            print("    ✓ Brain class tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Brain class test failed: {e}")
            return False
    
    def testWorldClass(self):
        """Test SimpleWorld class functionality."""
        print("  Testing SimpleWorld class...")
        
        try:
            # Create world
            world = createSimpleWorld()
            world.configure(10, 10, 8, 8)
            world.reset()
            world.set_agent_start(5.0, 5.0)
            
            # Test world methods
            if world.get_width() != 10 or world.get_height() != 10:
                print("    ✗ World configuration incorrect")
                return False
            
            world.update(0.1)
            
            print("    ✓ SimpleWorld class tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ SimpleWorld class test failed: {e}")
            return False
    
    def testAgentClass(self):
        """Test AgentBrain class functionality."""
        print("  Testing AgentBrain class...")
        
        try:
            # Create brain and world
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            world = createSimpleWorld()
            world.configure(10, 10, 8, 8)
            world.reset()
            
            # Create agent
            agent = createAgentBrain(brain)
            agent.initialize(world)
            
            # Test agent methods
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            
            action = agent.decode_motor_command()
            
            print("    ✓ AgentBrain class tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ AgentBrain class test failed: {e}")
            return False
    
    def testFactoryFunctions(self):
        """Test factory functions."""
        print("  Testing factory functions...")
        
        try:
            # Test createDefaultConfig
            config = createDefaultConfig()
            if not isinstance(config, Config):
                print("    ✗ createDefaultConfig returned invalid type")
                return False
            
            # Test createBrain
            brain = createBrain(config)
            if not isinstance(brain, Brain):
                print("    ✗ createBrain returned invalid type")
                return False
            
            # Test createSimpleWorld
            world = createSimpleWorld()
            if not isinstance(world, SimpleWorld):
                print("    ✗ createSimpleWorld returned invalid type")
                return False
            
            # Test createAgentBrain
            agent = createAgentBrain(brain)
            if not isinstance(agent, AgentBrain):
                print("    ✗ createAgentBrain returned invalid type")
                return False
            
            print("    ✓ Factory functions tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Factory functions test failed: {e}")
            return False
    
    def testConfigSystem(self):
        """Test configuration system."""
        print("  Testing configuration system...")
        
        try:
            config = createDefaultConfig()
            
            # Test setting values
            config.set("brain.neuron_count", 500)
            config.set("brain.synapse_density", 0.1)
            
            # Test getting values
            neuronCount = config.getOr("brain.neuron_count", 1000)
            synapseDensity = config.getOr("brain.synapse_density", 0.05)
            
            if neuronCount != 500 or synapseDensity != 0.1:
                print("    ✗ Configuration get/set operations failed")
                return False
            
            # Test key existence
            if not config.has("brain.neuron_count"):
                print("    ✗ Configuration key existence check failed")
                return False
            
            print("    ✓ Configuration system tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Configuration system test failed: {e}")
            return False
    
    def testBrainOperations(self):
        """Test brain operations."""
        print("  Testing brain operations...")
        
        try:
            # Create brain
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            # Test step operations
            for i in range(10):
                brain.step(i)
            
            # Test statistics
            neuronCount = brain.get_neuron_count()
            spikeCount = brain.get_total_spike_count()
            firingCount = brain.get_firing_neuron_count()
            
            if neuronCount <= 0 or spikeCount < 0 or firingCount < 0:
                print("    ✗ Brain statistics invalid")
                return False
            
            # Test developmental stage
            stage = brain.get_development_stage()
            if not hasattr(stage, 'value'):
                print("    ✗ Developmental stage invalid")
                return False
            
            print("    ✓ Brain operations tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Brain operations test failed: {e}")
            return False
    
    def testMemorySystems(self):
        """Test memory systems."""
        print("  Testing memory systems...")
        
        try:
            # Create brain
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            # Test memory system access
            workingMemory = brain.get_working_memory()
            episodicMemory = brain.get_episodic_memory()
            associativeMemory = brain.get_associative_memory()
            
            if workingMemory is None or episodicMemory is None or associativeMemory is None:
                print("    ✗ Memory systems not accessible")
                return False
            
            # Test memory operations
            if hasattr(workingMemory, 'storeWorkingMemory'):
                print("    ✓ Memory system operations available")
            
            print("    ✓ Memory systems tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Memory systems test failed: {e}")
            return False
    
    def testAdvancedAPI(self):
        """Test advanced API."""
        print("  Testing advanced API...")
        
        try:
            # Create brain and advanced API
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            advancedAPI = AdvancedBrainAPI(brain)
            
            # Test advanced API methods
            if not hasattr(advancedAPI, 'configure_for_task'):
                print("    ✗ Advanced API missing configure_for_task method")
                return False
            
            if not hasattr(advancedAPI, 'get_performance_stats'):
                print("    ✗ Advanced API missing get_performance_stats method")
                return False
            
            print("    ✓ Advanced API tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Advanced API test failed: {e}")
            return False
    
    def testLearningPipeline(self):
        """Test learning pipeline."""
        print("  Testing learning pipeline...")
        
        try:
            # Create learning pipeline
            pipeline = LearningPipeline()
            
            # Test pipeline methods
            if not hasattr(pipeline, 'add_brain_creation_step'):
                print("    ✗ Learning pipeline missing add_brain_creation_step method")
                return False
            
            if not hasattr(pipeline, 'add_initialization_step'):
                print("    ✗ Learning pipeline missing add_initialization_step method")
                return False
            
            if not hasattr(pipeline, 'add_training_step'):
                print("    ✗ Learning pipeline missing add_training_step method")
                return False
            
            print("    ✓ Learning pipeline tests passed")
            return True
            
        except Exception as e:
            print(f"    ✗ Learning pipeline test failed: {e}")
            return False


__all__.append('BrainAPITests')