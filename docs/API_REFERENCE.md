# NLM Python API Reference

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) Python bindings. This is the authoritative reference for using the enhanced Python interfaces with validation, convenience methods, and Pythonic features.

## Module Overview

```python
import pynlm
```

The `pynlm` module provides access to:

- **Brain**: Central neural simulation engine
- **Config**: Configuration management with validation
- **AgentBrain**: Interface for connecting brain to world
- **SimpleWorld**: Virtual world simulation
- **Action**: Motor commands and actions
- **SensoryInput**: Sensory data (Vision, Audio, InternalSignals)
- **SensoryPercept**: Combined sensory information
- **AgentBody**: Agent physical state and properties
- **ActionResult**: Results from actions in the world
- **WorldObject**: Objects in the world

## Core Classes

### Config
```python
class Config:
    def __init__(self, filepath=None):
        """Create a new configuration
        
        Args:
            filepath (str, optional): Path to JSON configuration file
            
        Examples:
            >>> config = Config()
            >>> config2 = Config("brain_config.json")
        """
        
    def loadFromFile(self, filepath):
        """Load configuration from JSON file with validation
        
        Args:
            filepath (str): Path to JSON configuration file
            
        Raises:
            FileNotFoundError: If file doesn't exist
            ValueError: If JSON is invalid
            
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
        """
        
    def saveToFile(self, filepath):
        """Save configuration to JSON file
        
        Args:
            filepath (str): Path to save JSON configuration file
            
        Examples:
            >>> config.saveToFile("my_config.json")
        """
        
    def __setitem__(self, key, value):
        """Set configuration value using dictionary-like syntax
        
        Args:
            key (str): Configuration key
            value: Configuration value (str, int, float, or bool)
            
        Raises:
            ValueError: If key is empty or type unsupported
            
        Examples:
            >>> config["neuron_count"] = 1000
            >>> config["learning_rate"] = 0.01
        """
        
    def __getitem__(self, key):
        """Get configuration value with default fallback
        
        Args:
            key (str): Configuration key
            
        Returns:
            Configuration value or None if not found
            
        Examples:
            >>> neuron_count = config.get("neuron_count", 1000)
            >>> learning_rate = config["learning_rate"]
        """
        
    def get(self, key, default=None):
        """Get configuration value with explicit default
        
        Args:
            key (str): Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
            
        Examples:
            >>> count = config.get("neuron_count", 1000)
        """
        
    def set(self, key, value):
        """Set configuration value with validation
        
        Args:
            key (str): Configuration key
            value: Configuration value (str, int, float, or bool)
            
        Raises:
            ValueError: If key is invalid or type unsupported
            
        Examples:
            >>> config.set("synapse_density", 0.1)
        """
        
    def has(self, key):
        """Check if configuration key exists
        
        Args:
            key (str): Configuration key to check
            
        Returns:
            bool: True if key exists, False otherwise
            
        Examples:
            >>> if config.has("neuron_count"):
            ...     print(f"Count: {config['neuron_count']}")
        """
        
    def getKeys(self):
        """Get all configuration keys
        
        Returns:
            list: List of all configuration keys
            
        Examples:
            >>> keys = config.getKeys()
            >>> print(f"Config keys: {keys}")
        """
        
    def clear(self):
        """Clear all configuration entries
        
        Examples:
            >>> config.clear()  # Start fresh
        """
        
    def summary(self):
        """Get a human-readable summary of configuration
        
        Returns:
            str: Configuration summary
            
        Examples:
            >>> print(config.summary())
        """
        
    def validate(self, required_keys=None):
        """Validate configuration
        
        Args:
            required_keys (list, optional): List of required key names
            
        Returns:
            bool: True if valid, False otherwise
            
        Raises:
            ValueError: If validation fails
            
        Examples:
            >>> config.validate()
            >>> config.validate(["neuron_count", "region_count"])
        """
        
    def getNumericStats(self):
        """Get statistics for numeric configuration values
        
        Returns:
            dict: Dictionary with keys 'mean', 'median', 'min', 'max'
            
        Examples:
            >>> stats = config.getNumericStats()
            >>> print(f"Mean: {stats['mean']}")
        """
        
    def getConfigSummary(self, includeValues=False):
        """Get detailed configuration summary
        
        Args:
            includeValues (bool): Whether to include actual values
            
        Returns:
            str: Detailed configuration summary
            
        Examples:
            >>> print(config.getConfigSummary())
            >>> print(config.getConfigSummary(includeValues=True))
        """
        
    def resetToDefaults(self):
        """Reset configuration to default values
        
        Examples:
            >>> config.resetToDefaults()  # Restore to factory defaults
        """
        
    def mergeConfig(self, other_config):
        """Merge another configuration into this one
        
        Args:
            other_config (Config): Configuration to merge
            
        Examples:
            >>> config.mergeConfig(default_config)
        """
        
    def __repr__(self):
        """String representation of Config
        
        Returns:
            str: String representation
        """

### Action
```python
class Action:
    def __init__(self, action_type=ActionType.Wait):
        """Create a new action
        
        Args:
            action_type (ActionType): Type of action to create
            
        Examples:
            >>> action = Action(ActionType.MoveForward)
            >>> action = Action()  # Default to ActionType.Wait
        """
        
    def __init__(self, action_type, parameters):
        """Create an action with parameters
        
        Args:
            action_type (ActionType): Type of action
            parameters (list): Action parameters
            
        Examples:
            >>> action = Action(ActionType.Interact, [0.5, 0.3])
        """
        
    def getType(self):
        """Get action type
        
        Returns:
            ActionType: The action type
            
        Examples:
            >>> action_type = action.getType()
        """
        
    def setType(self, action_type):
        """Set action type
        
        Args:
            action_type (ActionType): New action type
            
        Examples:
            >>> action.setType(ActionType.TurnLeft)
        """
        
    def getParameters(self):
        """Get action parameters
        
        Returns:
            list: List of action parameters
            
        Examples:
            >>> params = action.getParameters()
        """
        
    def setParameters(self, parameters):
        """Set action parameters
        
        Args:
            parameters (list): New action parameters
            
        Examples:
            >>> action.setParameters([0.2, 0.8])
        """
        
    def getName(self):
        """Get human-readable action name
        
        Returns:
            str: Action name
            
        Examples:
            >>> name = action.getName()
        """
        
    def clone(self):
        """Create a copy of this action
        
        Returns:
            Action: Copy of this action
            
        Examples:
            >>> action_copy = action.clone()
        """
        
    # Convenience factory methods
    @staticmethod
    def create_move_forward(distance=1.0):
        """Create a MoveForward action
        
        Args:
            distance (float): Movement distance
            
        Returns:
            Action: MoveForward action
            
        Examples:
            >>> move_action = Action.create_move_forward(2.0)
        """
        
    @staticmethod
    def create_turn_left(angle=0.5):
        """Create a TurnLeft action
        
        Args:
            angle (float): Turning angle in radians
            
        Returns:
            Action: TurnLeft action
            
        Examples:
            >>> turn_action = Action.create_turn_left(1.0)
        """
        
    @staticmethod
    def create_turn_right(angle=0.5):
        """Create a TurnRight action
        
        Args:
            angle (float): Turning angle in radians
            
        Returns:
            Action: TurnRight action
            
        Examples:
            >>> turn_action = Action.create_turn_right(0.5)
        """
        
    @staticmethod
    def create_look(direction):
        """Create a Look action
        
        Args:
            direction (list): [x, y] direction vector
            
        Returns:
            Action: Look action
            
        Examples:
            >>> look_action = Action.create_look([1.0, 0.0])
        """
        
    @staticmethod
    def create_interact():
        """Create an Interact action
        
        Returns:
            Action: Interact action
            
        Examples:
            >>> interact_action = Action.create_interact()
        """
        
    @staticmethod
    def create_eat():
        """Create an Eat action
        
        Returns:
            Action: Eat action
            
        Examples:
            >>> eat_action = Action.create_eat()
        """
        
    @staticmethod
    def create_wait():
        """Create a Wait action
        
        Returns:
            Action: Wait action
            
        Examples:
            >>> wait_action = Action.create_wait()
        """
        
    @staticmethod
    def create_rest():
        """Create a Rest action
        
        Returns:
            Action: Rest action
            
        Examples:
            >>> rest_action = Action.create_rest()
        """
        
    def __repr__(self):
        """String representation of Action
        
        Returns:
            str: String representation
        """

### ActionResult
```python
class ActionResult:
    def __init__(self, reward=0.0, success=True, message=""):
        """Create an action result
        
        Args:
            reward (float): Reward value (-1.0 to 1.0)
            success (bool): Whether action was successful
            message (str): Result message
            
        Examples:
            >>> result = ActionResult(reward=0.5, success=True, "Found food")
            >>> result = ActionResult(success=False, "Hit wall")
        """
        
    @property
    def reward(self):
        """Get reward value
        
        Returns:
            float: Reward value
            
        Examples:
            >>> reward = result.reward
        """
        
    @reward.setter
    def reward(self, value):
        """Set reward value
        
        Args:
            value (float): New reward value
            
        Raises:
            ValueError: If reward not in [-1.0, 1.0]
            
        Examples:
            >>> result.reward = 0.8
        """
        
    @property
    def success(self):
        """Get success status
        
        Returns:
            bool: Success status
            
        Examples:
            >>> is_success = result.success
        """
        
    @success.setter
    def success(self, value):
        """Set success status
        
        Args:
            value (bool): New success status
            
        Examples:
            >>> result.success = False
        """
        
    @property
    def message(self):
        """Get result message
        
        Returns:
            str: Result message
            
        Examples:
            >>> message = result.message
        """
        
    @message.setter
    def message(self, value):
        """Set result message
        
        Args:
            value (str): New message
            
        Examples:
            >>> result.message = "Action completed"
        """
        
    # Factory methods
    @staticmethod
    def success(reward=1.0, message="Action successful"):
        """Create a successful action result
        
        Args:
            reward (float): Reward value (default: 1.0)
            message (str): Result message
            
        Returns:
            ActionResult: Successful result
            
        Examples:
            >>> success_result = ActionResult.success(0.8, "Found resource")
        """
        
    @staticmethod
    def failure(reward=-1.0, message="Action failed"):
        """Create a failed action result
        
        Args:
            reward (float): Penalty value (default: -1.0)
            message (str): Result message
            
        Returns:
            ActionResult: Failed result
            
        Examples:
            >>> failure_result = ActionResult.failure(message="Hit obstacle")
        """
        
    @staticmethod
    def neutral(message="Action completed"):
        """Create a neutral action result
        
        Args:
            message (str): Result message
            
        Returns:
            ActionResult: Neutral result
            
        Examples:
            >>> neutral_result = ActionResult.neutral()
        """
        
    def __repr__(self):
        """String representation of ActionResult
        
        Returns:
            str: String representation
        """

### Brain
```python
class Brain:
    def __init__(self, config=None):
        """Create a new brain
        
        Args:
            config (Config, optional): Brain configuration
            
        Raises:
            ValueError: If config is invalid
            
        Examples:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain = Brain()  # Uses default config
        """
        
    def initialize(self):
        """Initialize the brain with configuration
        
        Raises:
            RuntimeError: If brain already initialized
            
        Examples:
            >>> brain.initialize()
        """
        
    def step(self, step_number):
        """Perform one simulation step
        
        Args:
            step_number (int): Current simulation step number
            
        Examples:
            >>> brain.step(0)
            >>> brain.step(100)
        """
        
    def step_with_time(self, step_number, timestamp):
        """Perform one simulation step with timestamp
        
        Args:
            step_number (int): Current simulation step number
            timestamp (float): Current simulation time
            
        Examples:
            >>> brain.step_with_time(step, 0.1)
        """
        
    def receiveSensoryInput(self, input_data):
        """Inject sensory input into the brain
        
        Args:
            input_data (SensoryInput): Sensory data to inject
            
        Examples:
            >>> brain.receiveSensoryInput(vision_data)
        """
        
    def produceAction(self):
        """Generate motor action based on brain activity
        
        Returns:
            Action: Generated motor action
            
        Returns None if no action selected
            
        Examples:
            >>> action = brain.produceAction()
            >>> if action:
            ...     print(f"Action: {action.getType()}")
        """
        
    def reset(self):
        """Reset brain to initial state
        
        Examples:
            >>> brain.reset()
        """
        
    def save(self, filepath):
        """Save brain state to file
        
        Args:
            filepath (str): Path to save brain state
            
        Returns:
            bool: True if saved successfully
            
        Examples:
            >>> brain.save("brain_state.bin")
        """
        
    def load(self, filepath):
        """Load brain state from file
        
        Args:
            filepath (str): Path to brain state file
            
        Returns:
            bool: True if loaded successfully
            
        Examples:
            >>> brain.load("brain_state.bin")
        """
        
    def addRegion(self, name=""):
        """Add a new neural region
        
        Args:
            name (str): Region name
            
        Returns:
            RegionId: ID of the new region
            
        Examples:
            >>> region_id = brain.addRegion("cortex")
        """
        
    def getRegion(self, region_id):
        """Get region by ID
        
        Args:
            region_id (RegionId): Region identifier
            
        Returns:
            NeuralRegion: Region object or None if not found
            
        Examples:
            >>> region = brain.getRegion(region_id)
        """
        
    def getTotalNeuronCount(self):
        """Get total neuron count
        
        Returns:
            int: Total number of neurons
            
        Examples:
            >>> count = brain.getTotalNeuronCount()
        """
        
    def getTotalSynapseCount(self):
        """Get total synapse count
        
        Returns:
            int: Total number of synapses
            
        Examples:
            >>> count = brain.getTotalSynapseCount()
        """
        
    def getFiringNeuronCount(self):
        """Get count of currently firing neurons
        
        Returns:
            int: Number of firing neurons
            
        Examples:
            >>> firing = brain.getFiringNeuronCount()
        """
        
    def getAverageFiringRate(self):
        """Get average firing rate
        
        Returns:
            float: Average firing rate (spikes/second)
            
        Examples:
            >>> rate = brain.getAverageFiringRate()
        """
        
    def getTotalSpikeCount(self):
        """Get total spike count since initialization
        
        Returns:
            int: Total spike count
            
        Examples:
            >>> spikes = brain.getTotalSpikeCount()
        """
        
    def getDevelopmentalStage(self):
        """Get current developmental stage
        
        Returns:
            DevelopmentalStage: Current developmental stage
            
        Examples:
            >>> stage = brain.getDevelopmentalStage()
        """
        
    def setDevelopmentalStage(self, stage):
        """Set developmental stage
        
        Args:
            stage (DevelopmentalStage): New developmental stage
            
        Examples:
            >>> brain.setDevelopmentalStage(DevelopmentalStage.Adult)
        """
        
    def getConfig(self):
        """Get configuration object
        
        Returns:
            Config: Configuration object
            
        Examples:
            >>> config = brain.getConfig()
        """
        
    def logStatus(self):
        """Print brain status and statistics
        
        Examples:
            >>> brain.logStatus()
        """
        
    # Advanced statistics and analysis
    def getBrainStatistics(self):
        """Get comprehensive brain statistics
        
        Returns:
            dict: Dictionary containing statistics
            
        Examples:
            >>> stats = brain.getBrainStatistics()
            >>> print(f"Neurons: {stats['total_neurons']}")
        """
        
    def getRegionStatistics(self):
        """Get statistics for all regions
        
        Returns:
            dict: Dictionary mapping region IDs to statistics
            
        Examples:
            >>> region_stats = brain.getRegionStatistics()
        """
        
    def getConnectivityStats(self):
        """Get connectivity statistics
        
        Returns:
            dict: Dictionary with connection statistics
            
        Examples:
            >>> conn_stats = brain.getConnectivityStats()
        """
        
    def analyzeActivityPattern(self, steps=10):
        """Analyze recent activity patterns
        
        Args:
            steps (int): Number of recent steps to analyze
            
        Returns:
            dict: Activity pattern analysis
            
        Examples:
            >>> pattern = brain.analyzeActivityPattern(100)
        """
        
    # Batch operations
    def runSteps(self, num_steps, timestep=0.001):
        """Run multiple simulation steps
        
        Args:
            num_steps (int): Number of steps to run
            timestep (float): Time per step in seconds
            
        Returns:
            list: List of step results
            
        Examples:
            >>> results = brain.runSteps(1000, timestep=0.1)
        """
        
    def injectSensoryData(self, sensory_data_list):
        """Inject multiple sensory inputs
        
        Args:
            sensory_data_list (list): List of SensoryInput objects
            
        Examples:
            >>> brain.injectSensoryData([vision1, vision2, audio1])
        """
        
    def simulateEpisode(self, num_steps=100, timestep=0.1):
        """Run a complete episode simulation
        
        Args:
            num_steps (int): Number of steps in episode
            timestep (float): Time per step in seconds
            
        Returns:
            dict: Episode results including actions and rewards
            
        Examples:
            >>> episode = brain.simulateEpisode(500, timestep=0.1)
        """
        
    def __repr__(self):
        """String representation of Brain
        
        Returns:
            str: String representation
        """

### SimpleWorld
```python
class SimpleWorld:
    def __init__(self):
        """Create a new simple world
        
        Examples:
            >>> world = SimpleWorld()
        """
        
    def configure(self, width, height, visionWidth, visionHeight):
        """Configure world dimensions
        
        Args:
            width (int): World width in cells
            height (int): World height in cells
            visionWidth (int): Agent vision width
            visionHeight (int): Agent vision height
            
        Examples:
            >>> world.configure(20, 20, 8, 8)
        """
        
    def reset(self):
        """Reset world to initial state
        
        Examples:
            >>> world.reset()
        """
        
    def setAgentStart(self, x, y):
        """Set agent starting position
        
        Args:
            x (float): X coordinate
            y (float): Y coordinate
            
        Examples:
            >>> world.setAgentStart(5.0, 5.0)
        """
        
    def update(self, timestep):
        """Update world state
        
        Args:
            timestep (float): Time step in seconds
            
        Examples:
            >>> world.update(0.1)
        """
        
    def applyMotorCommand(self, action, current_time):
        """Apply motor command to world
        
        Args:
            action (Action): Motor action to apply
            current_time (float): Current simulation time
            
        Returns:
            ActionResult: Result of the action
            
        Examples:
            >>> result = world.applyMotorCommand(action, 1.5)
        """
        
    def getSensoryPercept(self):
        """Get current sensory percept
        
        Returns:
            SensoryPercept: Current sensory information
            
        Examples:
            >>> percept = world.getSensoryPercept()
        """
        
    def getAgentBody(self):
        """Get agent body state
        
        Returns:
            AgentBody: Current agent state
            
        Examples:
            >>> body = world.getAgentBody()
        """
        
    def addObject(self, world_object):
        """Add object to world
        
        Args:
            world_object (WorldObject): Object to add
            
        Examples:
            >>> world.addObject(WorldObject(3.0, 4.0, WorldObjectType.Resource))
        """
        
    def removeObject(self, x, y):
        """Remove object from world
        
        Args:
            x (float): X coordinate
            y (float): Y coordinate
            
        Examples:
            >>> world.removeObject(3.0, 4.0)
        """
        
    def isValidPosition(self, x, y):
        """Check if position is valid
        
        Args:
            x (float): X coordinate
            y (float): Y coordinate
            
        Returns:
            bool: True if position is valid
            
        Examples:
            >>> if world.isValidPosition(3.0, 4.0):
            ...     print("Valid position")
        """
        
    # World management methods
    def getWidth(self):
        """Get world width
        
        Returns:
            int: World width
            
        Examples:
            >>> width = world.getWidth()
        """
        
    def getHeight(self):
        """Get world height
        
        Returns:
            int: World height
            
        Examples:
            >>> height = world.getHeight()
        """
        
    def getSimulationTime(self):
        """Get current simulation time
        
        Returns:
            float: Current simulation time
            
        Examples:
            >>> time = world.getSimulationTime()
        """
        
    def getMaxEnergy(self):
        """Get maximum energy capacity
        
        Returns:
            float: Maximum energy
            
        Examples:
            >>> energy = world.getMaxEnergy()
        """
        
    def setMaxEnergy(self, energy):
        """Set maximum energy capacity
        
        Args:
            energy (float): New maximum energy
            
        Examples:
            >>> world.setMaxEnergy(100.0)
        """
        
    def getEnergyDecayRate(self):
        """Get energy decay rate
        
        Returns:
            float: Energy decay rate
            
        Examples:
            >>> rate = world.getEnergyDecayRate()
        """
        
    def setEnergyDecayRate(self, rate):
        """Set energy decay rate
        
        Args:
            rate (float): New energy decay rate
            
        Examples:
            >>> world.setEnergyDecayRate(0.01)
        """
        
    def setRandomSeed(self, seed):
        """Set random seed for reproducible simulations
        
        Args:
            seed (int): Random seed
            
        Examples:
            >>> world.setRandomSeed(42)
        """
        
    def getRandomSeed(self):
        """Get current random seed
        
        Returns:
            int: Current random seed
            
        Examples:
            >>> seed = world.getRandomSeed()
        """
        
    # World statistics and analysis
    def getWorldStatistics(self):
        """Get comprehensive world statistics
        
        Returns:
            dict: World statistics
            
        Examples:
            >>> stats = world.getWorldStatistics()
        """
        
    def getObjectStatistics(self):
        """Get object statistics
        
        Returns:
            dict: Object statistics by type
            
        Examples:
            >>> obj_stats = world.getObjectStatistics()
        """
        
    def analyzeEnvironment(self):
        """Analyze environment properties
        
        Returns:
            dict: Environment analysis
            
        Examples:
            >>> analysis = world.analyzeEnvironment()
        """
        
    # Advanced world operations
    def runEpisode(self, agent_brain, num_steps=100, timestep=0.1):
        """Run complete episode with agent
        
        Args:
            agent_brain (AgentBrain): Agent brain interface
            num_steps (int): Number of steps
            timestep (float): Time per step
            
        Returns:
            dict: Episode results
            
        Examples:
            >>> episode = world.runEpisode(agent_brain, 500, timestep=0.1)
        """
        
    def simulateMultipleAgents(self, agent_configs, num_steps=100):
        """Simulate multiple agents
        
        Args:
            agent_configs (list): List of agent configurations
            num_steps (int): Number of steps
            
        Returns:
            list: List of simulation results
            
        Examples:
            >>> results = world.simulateMultipleAgents([config1, config2], 200)
        """
        
    def __repr__(self):
        """String representation of SimpleWorld
        
        Returns:
            str: String representation
        """

### AgentBrain
```python
class AgentBrain:
    def __init__(self, brain):
        """Create agent brain interface
        
        Args:
            brain (Brain): Underlying brain instance
            
        Raises:
            ValueError: If brain is invalid
            
        Examples:
            >>> brain = Brain(config)
            >>> agent_brain = AgentBrain(brain)
        """
        
    def initialize(self, world):
        """Initialize agent with world
        
        Args:
            world (SimpleWorld): World to connect to
            
        Raises:
            ValueError: If world is invalid
            
        Examples:
            >>> agent_brain.initialize(world)
        """
        
    def getSensoryInputSize(self):
        """Get expected sensory input size
        
        Returns:
            int: Expected sensory input size
            
        Examples:
            >>> size = agent_brain.getSensoryInputSize()
        """
        
    def getMotorOutputSize(self):
        """Get expected motor output size
        
        Returns:
            int: Expected motor output size
            
        Examples:
            >>> size = agent_brain.getMotorOutputSize()
        """
        
    def processSensoryInput(self, percept):
        """Process sensory percept and inject into brain
        
        Args:
            percept (SensoryPercept): Sensory information
            
        Raises:
            ValueError: If percept is invalid
            
        Examples:
            >>> agent_brain.processSensoryInput(percept)
        """
        
    def decodeMotorCommand(self):
        """Decode brain motor activity into motor command
        
        Returns:
            Action: Motor command action
            
        Returns None if no action selected
            
        Examples:
            >>> action = agent_brain.decodeMotorCommand()
        """
        
    def applyRewardModulation(self, reward, predicted_reward):
        """Apply reward-based neuromodulation
        
        Args:
            reward (float): Actual reward received
            predicted_reward (float): Predicted reward
            
        Examples:
            >>> agent_brain.applyRewardModulation(0.5, 0.3)
        """
        
    def updateDevelopment(self, timestep):
        """Update development system
        
        Args:
            timestep (float): Time step for development update
            
        Examples:
            >>> agent_brain.updateDevelopment(0.1)
        """
        
    def getDevelopmentalStage(self):
        """Get current developmental stage
        
        Returns:
            DevelopmentalStage: Current developmental stage
            
        Examples:
            >>> stage = agent_brain.getDevelopmentalStage()
        """
        
    def getNeuromodulationLevel(self):
        """Get current neuromodulation level
        
        Returns:
            float: Neuromodulation level
            
        Examples:
            >>> level = agent_brain.getNeuromodulationLevel()
        """
        
    def getCuriosityLevel(self):
        """Get curiosity level
        
        Returns:
            float: Curiosity level (0.0 to 1.0)
            
        Examples:
            >>> curiosity = agent_brain.getCuriosityLevel()
        """
        
    def getNoveltyLevel(self):
        """Get novelty level
        
        Returns:
            float: Novelty level (0.0 to 1.0)
            
        Examples:
            >>> novelty = agent_brain.getNoveltyLevel()
        """
        
    def getPredictionError(self):
        """Get prediction error
        
        Returns:
            float: Prediction error
            
        Examples:
            >>> error = agent_brain.getPredictionError()
        """
        
    def reset(self):
        """Reset agent for new episode
        
        Examples:
            >>> agent_brain.reset()
        """
        
    def getBrain(self):
        """Get the underlying brain
        
        Returns:
            Brain: Underlying brain instance
            
        Examples:
            >>> brain = agent_brain.getBrain()
        """
        
    # Subsystem enable/disable
    def enableRewardModulation(self, enable):
        """Enable or disable reward modulation
        
        Args:
            enable (bool): Whether to enable reward modulation
            
        Examples:
            >>> agent_brain.enableRewardModulation(True)
        """
        
    def enableStructuralPlasticity(self, enable):
        """Enable or disable structural plasticity
        
        Args:
            enable (bool): Whether to enable structural plasticity
            
        Examples:
            >>> agent_brain.enableStructuralPlasticity(True)
        """
        
    def enableDevelopment(self, enable):
        """Enable or disable development
        
        Args:
            enable (bool): Whether to enable development
            
        Examples:
            >>> agent_brain.enableDevelopment(True)
        """
        
    def enableCuriosity(self, enable):
        """Enable or disable curiosity-driven exploration
        
        Args:
            enable (bool): Whether to enable curiosity
            
        Examples:
            >>> agent_brain.enableCuriosity(True)
        """
        
    # Subsystem status checks
    def isRewardModulationEnabled(self):
        """Check if reward modulation is enabled
        
        Returns:
            bool: Whether reward modulation is enabled
            
        Examples:
            >>> if agent_brain.isRewardModulationEnabled():
            ...     print("Reward modulation is enabled")
        """
        
    def isStructuralPlasticityEnabled(self):
        """Check if structural plasticity is enabled
        
        Returns:
            bool: Whether structural plasticity is enabled
            
        Examples:
            >>> if agent_brain.isStructuralPlasticityEnabled():
            ...     print("Structural plasticity is enabled")
        """
        
    def isDevelopmentEnabled(self):
        """Check if development is enabled
        
        Returns:
            bool: Whether development is enabled
            
        Examples:
            >>> if agent_brain.isDevelopmentEnabled():
            ...     print("Development is enabled")
        """
        
    def isCuriosityEnabled(self):
        """Check if curiosity is enabled
        
        Returns:
            bool: Whether curiosity is enabled
            
        Examples:
            >>> if agent_brain.isCuriosityEnabled():
            ...     print("Curiosity is enabled")
        """
        
    # Agent-specific operations
    def runEpisode(self, world, num_steps=100, timestep=0.1):
        """Run complete agent episode
        
        Args:
            world (SimpleWorld): World to simulate in
            num_steps (int): Number of steps in episode
            timestep (float): Time per step
            
        Returns:
            dict: Episode results including actions, rewards, and stats
            
        Examples:
            >>> results = agent_brain.runEpisode(world, 500, timestep=0.1)
        """
        
    def trainAgent(self, world, episodes=100, steps_per_episode=100, timestep=0.1):
        """Train agent in world environment
        
        Args:
            world (SimpleWorld): Training world
            episodes (int): Number of training episodes
            steps_per_episode (int): Steps per episode
            timestep (float): Time per step
            
        Returns:
            dict: Training results and statistics
            
        Examples:
            >>> training_results = agent_brain.trainAgent(world, episodes=1000)
        """
        
    def evaluateAgent(self, world, episodes=100, steps_per_episode=100, timestep=0.1):
        """Evaluate agent performance
        
        Args:
            world (SimpleWorld): Evaluation world
            episodes (int): Number of evaluation episodes
            steps_per_episode (int): Steps per episode
            timestep (float): Time per step
            
        Returns:
            dict: Evaluation results and metrics
            
        Examples:
            >>> eval_results = agent_brain.evaluateAgent(world, episodes=100)
        """
        
    def __repr__(self):
        """String representation of AgentBrain
        
        Returns:
            str: String representation
        """

### Helper Functions and Utilities
```python
def validate_neuron_id(neuron_id):
    """Validate neuron ID and throw if invalid
    
    Args:
        neuron_id (NeuronId): Neuron ID to validate
        
    Raises:
        ValueError: If neuron ID is invalid
        
    Examples:
        >>> validate_neuron_id(neuron_id)
    """
    
def validate_action_type(action_type):
    """Validate action type and throw if invalid
    
    Args:
        action_type (ActionType): Action type to validate
        
    Raises:
        ValueError: If action type is invalid
        
    Examples:
        >>> validate_action_type(ActionType.MoveForward)
    """
    
def validate_float_range(value, min, max, paramName):
    """Validate that a float value is within specified range
    
    Args:
        value (float): Value to validate
        min (float): Minimum allowed value
        max (float): Maximum allowed value
        paramName (str): Parameter name for error message
        
    Raises:
        ValueError: If value is outside range
        
    Examples:
        >>> validate_float_range(0.5, 0.0, 1.0, "learning_rate")
    """

# Advanced mathematical utilities
def clamp(value, min_val, max_val):
    """Clamp value between min and max
    
    Args:
        value (float): Value to clamp
        min_val (float): Minimum value
        max_val (float): Maximum value
        
    Returns:
        float: Clamped value
        
    Examples:
        >>> clamped = clamp(0.8, 0.0, 1.0)  # Returns 0.8
        >>> clamped = clamp(1.5, 0.0, 1.0)  # Returns 1.0
    """

def lerp(start, end, t):
    """Linear interpolation between two values
    
    Args:
        start (float): Start value
        end (float): End value
        t (float): Interpolation factor (0.0 to 1.0)
        
    Returns:
        float: Interpolated value
        
    Examples:
        >>> value = lerp(0.0, 1.0, 0.5)  # Returns 0.5
        >>> value = lerp(10.0, 20.0, 0.3)  # Returns 13.0
    """

def distance(point1, point2):
    """Calculate Euclidean distance between two points
    
    Args:
        point1 (list): First point [x, y]
        point2 (list): Second point [x, y]
        
    Returns:
        float: Euclidean distance
        
    Examples:
        >>> dist = distance([0.0, 0.0], [3.0, 4.0])  # Returns 5.0
    """

def format_time(seconds):
    """Format time in human-readable format
    
    Args:
        seconds (float): Time in seconds
        
    Returns:
        str: Formatted time string
        
    Examples:
        >>> format_time(125.5)  # Returns "2:05.50"
    """

def createDefaultConfig():
    """Create a default configuration
    
    Returns:
        Config: Default configuration
        
    Examples:
        >>> config = createDefaultConfig()
    """

def createBrain(config=None):
    """Create a new brain with configuration
    
    Args:
        config (Config, optional): Configuration (uses defaults if None)
        
    Returns:
        Brain: New brain instance
        
    Examples:
        >>> brain = createBrain()
        >>> config = Config({"neuron_count": 1000})
        >>> brain = createBrain(config)
    """

def createSimpleWorld():
    """Create a new simple world
    
    Returns:
        SimpleWorld: New world instance
        
    Examples:
        >>> world = createSimpleWorld()
    """

def createAgentBrain(brain):
    """Create a new agent brain interface
    
    Args:
        brain (Brain): Underlying brain
        
    Returns:
        AgentBrain: Agent brain interface
        
    Examples:
        >>> brain = createBrain()
        >>> agent = createAgentBrain(brain)
    """

# Type validation helpers
validateNeuronId = validate_neuron_id
validateActionType = validate_action_type
validateFloatRange = validate_float_range

# Module-level constants
__all__ = [
    'Config', 'Action', 'ActionResult', 'Brain', 'SimpleWorld', 'AgentBrain',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'SensoryPercept', 'AgentBody', 'WorldObject',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    'validate_neuron_id', 'validate_action_type', 'validate_float_range',
    'clamp', 'lerp', 'distance', 'format_time',
    
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'
]

# Module metadata
__version__ = "2.1.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python bindings for NLM with validation and convenience methods"

# Example usage section (commented out to avoid execution during import)
"""
# Basic usage examples:

import pynlm

# Create brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Create configuration
config = pynlm.Config()
config["neuron_count"] = 1000
config["learning_rate"] = 0.01

# Create world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
"""
```

## Usage Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain()
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Example 2: Complete Agent Episode
```python
import pynlm

# Setup configuration
config = pynlm.createDefaultConfig()

# Create brain, world, and agent
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
results = agent.runEpisode(world, num_steps=500, timestep=0.1)

# Print results
print(f"Episode completed in {len(results['actions'])} steps")
print(f"Total reward: {results['total_reward']:.2f}")
print(f"Average curiosity: {results['avg_curiosity']:.3f}")
```

### Example 3: Advanced Configuration
```python
import pynlm

# Create configuration with custom settings
config = pynlm.Config()

# Set basic parameters
config["neuron_count"] = 2000
config["region_count"] = 5
config["connection_probability"] = 0.15

# Set advanced parameters
config["stdp_ltp_weight"] = 0.01
config["stdp_ltd_weight"] = 0.012
config["synaptogenesis_rate"] = 0.001
config["pruning_rate"] = 0.00001

# Enable features
config["plasticity.stdp.enable"] = True
config["plasticity.hebbian.enable"] = True
config["neuromod.dopamine.scale"] = 1.0
config["neuromod.curiosity.enable"] = True

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Create brain statistics
stats = brain.getBrainStatistics()
print(f"Configured with {stats['total_neurons']} neurons across {stats['region_count']} regions")
```

### Example 4: Batch Processing
```python
import pynlm

# Create multiple brains
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config["neuron_count"] = 500 + i * 100
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run steps for all brains
sim_results = []
for i, brain in enumerate(brains):
    results = brain.runSteps(100, timestep=0.1)
    sim_results.append(results)
    print(f"Brain {i}: {results['total_spikes']} spikes")

# Get comparison statistics
comparison = {
    'total_neurons': [b.getTotalNeuronCount() for b in brains],
    'total_spikes': [b.getTotalSpikeCount() for b in brains],
    'firing_rates': [b.getAverageFiringRate() for b in brains]
}

print("Batch processing complete!")
```

### Example 5: World Analysis
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

# Add various objects
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=-5.0))
world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Wall, value=0.0))

# Analyze world
analysis = world.analyzeEnvironment()
stats = world.getWorldStatistics()
obj_stats = world.getObjectStatistics()

print(f"World dimensions: {stats['width']}x{stats['height']}")
print(f"Total objects: {stats['total_objects']}")
print(f"Resource nodes: {obj_stats.get('Resource', 0)}")
print(f"Hazard nodes: {obj_stats.get('Hazard', 0)}")

# Simulate agent
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

episode = world.runEpisode(agent, num_steps=200)
print(f"Episode completed with {episode['actions']} actions")
```

### Example 6: Advanced Action Management
```python
import pynlm

# Create different types of actions
actions = [
    pynlm.Action.create_move_forward(2.0),
    pynlm.Action.create_turn_left(0.5),
    pynlm.Action.create_turn_right(0.5),
    pynlm.Action.create_interact(),
    pynlm.Action.create_eat(),
    pynlm.Action.create_wait()
]

# Modify actions
actions[0].setParameters([2.0, 0.1])  # Move forward with rotation
actions[1].setType(pynlm.ActionType.Look)  # Look instead of turn

# Process action results
results = []
for action in actions:
    result = pynlm.ActionResult(
        reward=1.0 if action.getType() != pynlm.ActionType.Wait else 0.0,
        success=True,
        message=f"Executed {action.getName()}"
    )
    results.append(result)

# Get action statistics
print(f"Executed {len(actions)} actions")
print(f"Successful actions: {sum(1 for r in results if r.success)}")
print(f"Total reward: {sum(r.reward for r in results):.2f}")
```

## Configuration Parameters

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.region_count` | int | 1 | Number of brain regions |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable spike-timing-dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity rate |
| `development.critical_period` | float | 0.8 | Critical period plasticity modifier |

### Memory Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_capacity` | int | 100 | Working memory capacity |
| `memory.episodic_max_episodes` | int | 1000 | Maximum episodic memory episodes |

## Advanced Features

### Batch Processing
The enhanced bindings provide batch processing utilities:

```python
# Run multiple simulation steps
results = brain.runSteps(1000, timestep=0.1)

# Process multiple sensory inputs
brain.injectSensoryData([vision1, vision2, audio1])

# Run multiple episodes
all_episodes = []
for i in range(10):
    episode = agent.runEpisode(world, num_steps=100)
    all_episodes.append(episode)
```

### Statistics Aggregation
Get comprehensive statistics:

```python
# Brain statistics
brain_stats = brain.getBrainStatistics()
region_stats = brain.getRegionStatistics()
connectivity_stats = brain.getConnectivityStats()

# World statistics
world_stats = world.getWorldStatistics()
object_stats = world.getObjectStatistics()

# Agent statistics
agent_stats = {
    'curiosity_levels': [],
    'novelty_levels': [],
    'prediction_errors': []
}
```

### Validation and Error Handling
All configuration parameters are validated with helpful error messages:

```python
# Configuration validation
config.validate()
config.validate(["neuron_count", "region_count"])

# Parameter validation
validate_float_range(0.5, 0.0, 1.0, "learning_rate")
validate_action_type(ActionType.MoveForward)
validate_neuron_id(neuron_id)
```

### Memory Management
Enhanced memory management utilities:

```python
# Reset and cleanup
brain.reset()
agent.reset()
world.reset()

# Save and load
brain.save("brain_state.bin")
brain.load("brain_state.bin")
```

## Error Handling

The enhanced bindings provide comprehensive error handling:

### Common Exceptions

- **ValueError**: Invalid parameter values
- **FileNotFoundError**: Missing files
- **TypeError**: Invalid type conversions
- **RuntimeError**: Runtime errors

### Error Messages

Error messages include detailed information to help with debugging:

```
ValueError: Configuration key cannot be empty
ValueError: Learning rate must be between 0.0 and 1.0
ValueError: Invalid action type for this operation
FileNotFoundError: Configuration file "config.json" not found
```

### Validation Functions

Use validation functions for custom validation:

```python
# Validate configuration keys
if not config.has("required_key"):
    raise ValueError("Missing required key: required_key")

# Validate parameter ranges
if not (0.0 <= config["learning_rate"] <= 1.0):
    raise ValueError("Learning rate out of valid range")

# Validate action types
validate_action_type(action_type)
```

## Performance Considerations

### Memory Usage
- Large simulations may require significant memory
- Consider using `brain.reset()` to free memory
- Monitor memory usage with system tools

### Processing Speed
- Single-threaded operation for real-time applications
- Batch operations are optimized for speed
- Consider parallel processing for multiple agents

### Configuration Optimization
```python
# Optimized configuration for different use cases

# For speed
fast_config = Config()
fast_config["neuron_count"] = 500
fast_config["region_count"] = 1

# For accuracy
accurate_config = Config()
accurate_config["neuron_count"] = 5000
accurate_config["region_count"] = 10

# For exploration
exploration_config = Config()
exploration_config["neuromod.curiosity.enable"] = True
exploration_config["neuromod.novelty.enable"] = True
```

## Migration Guide

### From Basic to Enhanced API

**Basic API (original):**
```python
import pynlm

brain = pynlm.Brain()
brain.initialize()
brain.step(0)
```

**Enhanced API:**
```python
import pynlm

# Enhanced API with validation and convenience methods
brain = pynlm.createBrain()
brain.initialize()
brain.step(0)

# Additional features available
stats = brain.getBrainStatistics()
```

### Backward Compatibility

All existing code continues to work without modifications:

```python
# This still works exactly as before
import pynlm

brain = pynlm.Brain()
brain.initialize()
for i in range(100):
    brain.step(i)
```

The enhanced bindings maintain full backward compatibility while adding new features.

## Installation and Setup

### System Requirements
- Python 3.8+
- CMake 3.16+
- C++20 compatible compiler

### Installation
```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Build with custom options
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Testing
```bash
# Run tests
pytest

# Run specific tests
pytest tests/test_brain.cpp
```

## Troubleshooting

### Common Issues

**Issue**: "ImportError: No module named 'pynlm'"
**Solution**: Ensure the module is properly installed:
```bash
pip install --force-reinstall .
```

**Issue**: "ValueError: Configuration key cannot be empty"
**Solution**: Check all configuration key assignments:
```python
config["valid_key"] = "value"  # OK
config[""] = "value"           # Raises error
```

**Issue**: "Segmentation fault during simulation"
**Solution**: Ensure proper initialization:
```python
brain.initialize()  # Required before stepping
```

### Getting Help

- **Documentation**: See `HOW_TO_USE.md` and `easy_usage.md`
- **Examples**: Look at the example scripts in the repository
- **Support**: Check GitHub issues for common problems

## License

MIT License. See LICENSE file for details.

---

*This documentation covers the enhanced Python bindings (version 2.1.0) with validation, convenience methods, and Pythonic interfaces. For the latest version and updates, visit the NLM project repository.*
