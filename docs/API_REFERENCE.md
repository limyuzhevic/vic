# NLM Python API Reference

## Core API Documentation

This document provides detailed information about all classes, functions, and exceptions in the NLM Python API.

## Table of Contents

- [Core Classes](#core-classes)
  - [Config](#config)
  - [Brain](#brain)
  - [AgentBrain](#agentbrain)
  - [SimpleWorld](#simpleworld)
  - [WorldObject](#worldobject)
  - [Vision](#vision)
  - [Audio](#audio)
  - [InternalSignals](#internalsignals)
  - [SensoryInput](#sensoryinput)
  - [Action](#action)
  - [ActionResult](#actionresult)
  - [AgentBody](#agentbody)
  - [Environment](#environment)
  - [Experiment](#experiment)
  - [ExperimentRunner](#experimentrunner)
- [Expert APIs](#expert-apis)
  - [Performance APIs](#performance-apis)
  - [Advanced Simulation APIs](#advanced-simulation-apis)
  - [Data Analysis APIs](#data-analysis-apis)
  - [Custom Architecture APIs](#custom-architecture-apis)
  - [Configuration APIs](#configuration-apis)
  - [Experiment Control APIs](#experiment-control-apis)
  - [Integration APIs](#integration-apis)
  - [Visualization APIs](#visualization-apis)
  - [Utility APIs](#utility-apis)
- [Factory Functions](#factory-functions)
- [Exception Types](#exception-types)
- [Configuration Options](#configuration-options)
- [Type Definitions](#type-definitions)

## Core Classes

### Config

Configuration management for NLM simulation parameters.

```python
class Config:
    """Configuration management for NLM simulation parameters."""
    
    def __init__(self):
        """Initialize empty configuration."""
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value.
        
        Args:
            key: Configuration key (e.g., 'brain.simulated_neurons')
            value: Configuration value
        """
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
    
    def loadFromFile(self, filename: str) -> None:
        """Load configuration from JSON file.
        
        Args:
            filename: Path to configuration file
        """
    
    def saveToFile(self, filename: str) -> None:
        """Save configuration to JSON file.
        
        Args:
            filename: Path to save configuration file
        """
    
    def validate(self) -> None:
        """Validate configuration parameters.
        
        Raises:
            ConfigurationError: If configuration is invalid
        """
    
    def merge(self, other_config: 'Config') -> None:
        """Merge another configuration into this one.
        
        Args:
            other_config: Configuration to merge
        """
    
    def copy(self) -> 'Config':
        """Create a copy of this configuration.
        
        Returns:
            Copy of this configuration
        """
    
    def reset(self) -> None:
        """Reset configuration to default values."""
```

### Brain

Core neural network simulation engine.

```python
class Brain:
    """Core neural network simulation engine."""
    
    def __init__(self, config: Config):
        """Initialize brain with configuration.
        
        Args:
            config: Brain configuration
        """
    
    def initialize(self, world: 'SimpleWorld') -> None:
        """Initialize brain with world context.
        
        Args:
            world: Simulation world
        """
    
    def update(self, time_step: float) -> None:
        """Update brain state for time step.
        
        Args:
            time_step: Time step for simulation
        """
    
    def getStatistics(self) -> Dict[str, Any]:
        """Get brain statistics.
        
        Returns:
            Dictionary containing brain statistics
        """
    
    def getActiveNeuronCount(self) -> int:
        """Get number of currently active neurons.
        
        Returns:
            Number of active neurons
        """
    
    def getNeuronActivity(self, neuron_id: 'NeuronId') -> float:
        """Get activity level for specific neuron.
        
        Args:
            neuron_id: Neuron identifier
            
        Returns:
            Neuron activity level (0.0 to 1.0)
        """
    
    def save(self, filename: str) -> None:
        """Save brain state to file.
        
        Args:
            filename: Path to save brain state
        """
    
    def load(self, filename: str) -> None:
        """Load brain state from file.
        
        Args:
            filename: Path to load brain state
        """
```

### AgentBrain

Interface for agent behavior control and sensory-motor processing.

```python
class AgentBrain:
    """Interface for agent behavior control and sensory-motor processing."""
    
    def __init__(self, brain: 'Brain'):
        """Initialize agent brain with underlying neural network.
        
        Args:
            brain: Underlying neural network
        """
    
    def initialize(self, brain: 'Brain', world: 'SimpleWorld') -> None:
        """Initialize agent brain with brain and world.
        
        Args:
            brain: Underlying neural network
            world: Simulation world
        """
    
    def processSensoryInput(self, percept: 'SensoryInput') -> None:
        """Process sensory input.
        
        Args:
            percept: Sensory input to process
        """
    
    def decodeMotorCommand(self) -> 'Action':
        """Decode motor command from neural activity.
        
        Returns:
            Motor command action
        """
    
    def reset(self) -> None:
        """Reset agent brain state."""
```

### SimpleWorld

Simulation environment for agent interaction.

```python
class SimpleWorld:
    """Simulation environment for agent interaction."""
    
    def __init__(self):
        """Initialize empty world."""
    
    def configure(self, width: int, height: int, view_distance: int, cell_size: float) -> None:
        """Configure world dimensions and settings.
        
        Args:
            width: World width in grid cells
            height: World height in grid cells
            view_distance: Sensory view distance
            cell_size: Size of each grid cell
        """
    
    def setAgentStart(self, x: int, y: int) -> None:
        """Set agent starting position.
        
        Args:
            x: Starting X coordinate
            y: Starting Y coordinate
        """
    
    def getAgentBody(self) -> 'AgentBody':
        """Get agent body information.
        
        Returns:
            Agent body object
        """
    
    def getSensoryPercept(self) -> 'SensoryInput':
        """Get sensory percept from world.
        
        Returns:
            Current sensory percept
        """
    
    def applyMotorCommand(self, action: 'Action', step: int) -> None:
        """Apply motor command to world.
        
        Args:
            action: Motor command to apply
            step: Current simulation step
        """
    
    def update(self, time_step: float) -> None:
        """Update world state for time step.
        
        Args:
            time_step: Time step for simulation
        """
    
    def reset(self) -> None:
        """Reset world to initial state."""
```

### WorldObject

Base class for objects in the simulation world.

```python
class WorldObject:
    """Base class for objects in the simulation world."""
    
    def __init__(self):
        """Initialize world object."""
    
    @property
    def x(self) -> int:
        """Get X coordinate."""
        pass
    
    @property
    def y(self) -> int:
        """Get Y coordinate."""
        pass
    
    @property
    def type(self) -> 'WorldObjectType':
        """Get object type."""
        pass
    
    @property
    def energy(self) -> float:
        """Get object energy."""
        pass
```

### Vision

Visual sensory input processing.

```python
class Vision:
    """Visual sensory input processing."""
    
    def __init__(self, width: int, height: int, channels: int = 3):
        """Initialize vision processor.
        
        Args:
            width: Image width
            height: Image height
            channels: Number of color channels
        """
    
    def setData(self, image_data: Any) -> None:
        """Set visual data.
        
        Args:
            image_data: Visual data (numpy array or compatible)
        """
    
    def getData(self) -> Any:
        """Get visual data.
        
        Returns:
            Current visual data
        """
    
    def preprocess(self) -> Any:
        """Preprocess visual data.
        
        Returns:
            Preprocessed visual data
        """
```

### Audio

Audio sensory input processing.

```python
class Audio:
    """Audio sensory input processing."""
    
    def __init__(self, sample_rate: int, frame_size: int):
        """Initialize audio processor.
        
        Args:
            sample_rate: Audio sample rate
            frame_size: Size of audio frames
        """
    
    def setData(self, audio_samples: Any) -> None:
        """Set audio data.
        
        Args:
            audio_samples: Audio samples (numpy array or compatible)
        """
    
    def getData(self) -> Any:
        """Get audio data.
        
        Returns:
            Current audio data
        """
    
    def extract_features(self) -> Dict[str, Any]:
        """Extract audio features.
        
        Returns:
            Dictionary of audio features
        """
```

### InternalSignals

Internal physiological signals for the agent.

```python
class InternalSignals:
    """Internal physiological signals for the agent."""
    
    def __init__(self):
        """Initialize internal signals system."""
    
    def addSignal(self, value: float, signal_type: str = "default") -> None:
        """Add internal signal.
        
        Args:
            value: Signal value
            signal_type: Type of signal
        """
    
    def getSignal(self, signal_type: str) -> float:
        """Get signal value.
        
        Args:
            signal_type: Type of signal
            
        Returns:
            Signal value
        """
    
    def getAllSignals(self) -> Dict[str, float]:
        """Get all internal signals.
        
        Returns:
            Dictionary of all signal values
        """
```

### SensoryInput

Base class for all sensory inputs.

```python
class SensoryInput:
    """Base class for all sensory inputs."""
    
    def __init__(self, input_type: str):
        """Initialize sensory input.
        
        Args:
            input_type: Type of sensory input
        """
    
    def getData(self) -> Any:
        """Get sensory input data.
        
        Returns:
            Sensory input data
        """
    
    def getType(self) -> str:
        """Get sensory input type.
        
        Returns:
            Input type
        """
```

### Action

Motor commands and actions.

```python
class Action:
    """Motor commands and actions."""
    
    def __init__(self, action_type: 'ActionType', parameters: List[float] = None):
        """Initialize action.
        
        Args:
            action_type: Type of action
            parameters: Action parameters
        """
    
    def getType(self) -> 'ActionType':
        """Get action type.
        
        Returns:
            Action type
        """
    
    def getParameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            Action parameters
        """
    
    # Factory methods
    @staticmethod
    def createMoveAction(action_type: str, parameters: List[float]) -> 'Action':
        """Create move action.
        
        Args:
            action_type: Type of movement
            parameters: Movement parameters [dx, dy, dz]
            
        Returns:
            Move action
        """
    
    @staticmethod
    def createLookAction(action_type: str, parameters: List[float] = None) -> 'Action':
        """Create look action.
        
        Args:
            action_type: Type of look action
            parameters: Look parameters
            
        Returns:
            Look action
        """
    
    @staticmethod
    def createRestAction() -> 'Action':
        """Create rest action.
        
        Returns:
            Rest action
        """
    
    @staticmethod
    def createCustomAction(action_type: str, parameters: List[float]) -> 'Action':
        """Create custom action.
        
        Args:
            action_type: Type of custom action
            parameters: Action parameters
            
        Returns:
            Custom action
        """
```

### ActionResult

Result of applying an action to the world.

```python
class ActionResult:
    """Result of applying an action to the world."""
    
    def __init__(self):
        """Initialize action result."""
    
    def wasSuccessful(self) -> bool:
        """Check if action was successful.
        
        Returns:
            True if action succeeded
        """
    
    def getOutcome(self) -> str:
        """Get action outcome.
        
        Returns:
            Action outcome
        """
    
    def getReward(self) -> float:
        """Get reward from action.
        
        Returns:
            Reward value
        """
    
    def getError(self) -> Optional[str]:
        """Get error message.
        
        Returns:
            Error message if action failed
        """
```

### AgentBody

Physical representation of the agent in the world.

```python
class AgentBody:
    """Physical representation of the agent in the world."""
    
    def __init__(self):
        """Initialize agent body."""
    
    @property
    def x(self) -> int:
        """Get X coordinate."""
        pass
    
    @property
    def y(self) -> int:
        """Get Y coordinate."""
        pass
    
    @property
    def energy(self) -> float:
        """Get agent energy."""
        pass
    
    @property
    def health(self) -> float:
        """Get agent health."""
        pass
    
    @property
    def age(self) -> float:
        """Get agent age."""
        pass
```

### Environment

External environment containing the simulation world.

```python
class Environment:
    """External environment containing the simulation world."""
    
    def __init__(self):
        """Initialize environment."""
    
    def addObject(self, obj: 'WorldObject') -> None:
        """Add object to environment.
        
        Args:
            obj: Object to add
        """
    
    def removeObject(self, obj: 'WorldObject') -> None:
        """Remove object from environment.
        
        Args:
            obj: Object to remove
        """
    
    def getObjectAt(self, x: int, y: int) -> Optional['WorldObject']:
        """Get object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Object at position or None
        """
    
    def update(self, time_step: float) -> None:
        """Update environment state.
        
        Args:
            time_step: Time step for simulation
        """
```

### Experiment

Experiment framework for scientific studies.

```python
class Experiment:
    """Experiment framework for scientific studies."""
    
    def __init__(self, name: str, config: Config):
        """Initialize experiment.
        
        Args:
            name: Experiment name
            config: Experiment configuration
        """
    
    def run(self, world: 'SimpleWorld', brain: 'Brain') -> Dict[str, Any]:
        """Run experiment.
        
        Args:
            world: Simulation world
            brain: Brain instance
            
        Returns:
            Experiment results
        """
    
    def getResults(self) -> Dict[str, Any]:
        """Get experiment results.
        
        Returns:
            Experiment results
        """
    
    def saveResults(self, filename: str) -> None:
        """Save experiment results.
        
        Args:
            filename: Path to save results
        """
```

### ExperimentRunner

Manages multiple experiments.

```python
class ExperimentRunner:
    """Manages multiple experiments."""
    
    def __init__(self):
        """Initialize experiment runner."""
    
    def addExperiment(self, experiment: 'Experiment') -> None:
        """Add experiment to runner.
        
        Args:
            experiment: Experiment to add
        """
    
    def runExperiments(self) -> List[Dict[str, Any]]:
        """Run all experiments.
        
        Returns:
            List of experiment results
        """
    
    def getExperimentResults(self, experiment_name: str) -> Optional[Dict[str, Any]]:
        """Get results for specific experiment.
        
        Args:
            experiment_name: Name of experiment
            
        Returns:
            Experiment results or None
        """
```

## Expert APIs

### Performance APIs

#### MemoryPool

Memory pool management for performance optimization.

```python
class MemoryPool:
    """Memory pool management for performance optimization."""
    
    def __init__(self, initial_size: int = 1024, max_size: int = 65536):
        """Initialize memory pool.
        
        Args:
            initial_size: Initial pool size
            max_size: Maximum pool size
        """
    
    def allocate(self, size: int) -> Any:
        """Allocate memory from pool.
        
        Args:
            size: Size of memory to allocate
            
        Returns:
            Allocated memory block
        """
    
    def free(self, block: Any) -> None:
        """Free memory block.
        
        Args:
            block: Memory block to free
        """
    
    def getStats(self) -> Dict[str, Any]:
        """Get memory pool statistics.
        
        Returns:
            Dictionary of statistics
        """
```

#### SIMDProcessor

SIMD operations for parallel processing.

```python
class SIMDProcessor:
    """SIMD operations for parallel processing."""
    
    def __init__(self, vector_width: int = 256):
        """Initialize SIMD processor.
        
        Args:
            vector_width: SIMD vector width in bits
        """
    
    def processVector(self, data: List[float], operation: Callable) -> List[float]:
        """Process data with SIMD operations.
        
        Args:
            data: Input data
            operation: Operation to apply
            
        Returns:
            Processed data
        """
    
    def batchedOperations(self, batch_data: List[List[float]], ops: List[Callable]) -> List[List[float]]:
        """Perform multiple SIMD operations on batch data.
        
        Args:
            batch_data: Batch of input data
            ops: List of operations to apply
            
        Returns:
            Batch of processed data
        """
```

#### ParallelProcessor

Parallel processing interfaces.

```python
class ParallelProcessor:
    """Parallel processing interfaces."""
    
    def __init__(self, num_threads: int = 4):
        """Initialize parallel processor.
        
        Args:
            num_threads: Number of threads to use
        """
    
    def processChunk(self, data: List[Any], processor: Callable, chunk_size: int = 1024) -> List[Any]:
        """Process data in parallel chunks.
        
        Args:
            data: Input data
            processor: Function to apply to chunks
            chunk_size: Size of chunks
            
        Returns:
            Processed data
        """
    
    def distributeWork(self, work_func: Callable, data_list: List[Any]) -> List[Any]:
        """Distribute work across threads.
        
        Args:
            work_func: Work function to apply
            data_list: List of data items
            
        Returns:
            Processed results
        """
```

#### CustomAllocator

Custom memory allocators for performance optimization.

```python
class CustomAllocator:
    """Custom memory allocators for performance optimization."""
    
    def __init__(self, allocator_type: str = "buddy", alignment: int = 64):
        """Initialize custom allocator.
        
        Args:
            allocator_type: Type of allocator
            alignment: Memory alignment
        """
    
    def allocate(self, size: int, alignment: Optional[int] = None) -> Any:
        """Custom allocation.
        
        Args:
            size: Size of memory to allocate
            alignment: Memory alignment
            
        Returns:
            Allocated memory
        """
    
    def free(self, ptr: Any) -> None:
        """Custom free.
        
        Args:
            ptr: Memory pointer to free
        """
    
    def getStats(self) -> Dict[str, Any]:
        """Get allocator statistics.
        
        Returns:
            Dictionary of statistics
        """
```

#### ZeroCopyInterface

Zero-copy data interfaces for performance.

```python
class ZeroCopyInterface:
    """Zero-copy data interfaces for performance."""
    
    def __init__(self):
        """Initialize zero-copy interface."""
    
    def createInterface(self, data_ptr: Any, size: int, dtype: str = "float32") -> Any:
        """Create zero-copy interface.
        
        Args:
            data_ptr: Data pointer
            size: Data size
            dtype: Data type
            
        Returns:
            Interface identifier
        """
    
    def readData(self, interface_id: str, start: int = 0, end: int = -1) -> Any:
        """Read data from zero-copy interface.
        
        Args:
            interface_id: Interface identifier
            start: Start index
            end: End index (-1 for all)
            
        Returns:
            Data from interface
        """
```

#### DirectMemoryAccess

Direct memory access for performance-critical operations.

```python
class DirectMemoryAccess:
    """Direct memory access for performance-critical operations."""
    
    def __init__(self, device: str = "cpu"):
        """Initialize direct memory access.
        
        Args:
            device: Target device ("cpu", "gpu", etc.)
        """
    
    def read(self, address: int, size: int) -> Any:
        """Read from memory address.
        
        Args:
            address: Memory address
            size: Size to read
            
        Returns:
            Data from memory
        """
    
    def write(self, address: int, data: Any) -> None:
        """Write to memory address.
        
        Args:
            address: Memory address
            data: Data to write
        """
```

### Advanced Simulation APIs

#### CustomPlasticityRules

Custom synaptic plasticity rules for expert control.

```python
class CustomPlasticityRules:
    """Custom synaptic plasticity rules for expert control."""
    
    def __init__(self):
        """Initialize custom plasticity rules."""
    
    def addRule(self, rule_type: str, **params) -> None:
        """Add plasticity rule.
        
        Args:
            rule_type: Type of rule ("hebbian", "stdp", "structural")
            **params: Rule parameters
        """
    
    def updateWeights(self, brain: 'Brain', time_step: float) -> None:
        """Update synaptic weights.
        
        Args:
            brain: Brain instance
            time_step: Time step
        """
    
    def getRuleStats(self) -> Dict[str, Any]:
        """Get rule statistics.
        
        Returns:
            Dictionary of statistics
        """
```

#### AdvancedNeuromodulation

Advanced neuromodulator dynamics for expert control.

```python
class AdvancedNeuromodulation:
    """Advanced neuromodulator dynamics for expert control."""
    
    def __init__(self, config: Dict[str, Any]):
        """Initialize advanced neuromodulation.
        
        Args:
            config: Neuromodulation configuration
        """
    
    def applyModulation(self, action: 'Action', step: int) -> 'Action':
        """Apply neuromodulation to action.
        
        Args:
            action: Action to modulate
            step: Current step
            
        Returns:
            Modulated action
        """
    
    def getCurrentStrength(self) -> float:
        """Get current neuromodulation strength.
        
        Returns:
            Current strength
        """
```

#### CustomConnectivityPatterns

Custom neural connectivity patterns.

```python
class CustomConnectivityPatterns:
    """Custom neural connectivity patterns."""
    
    def __init__(self):
        """Initialize custom connectivity patterns."""
    
    def createPattern(self, pattern_type: str, **params) -> Dict['NeuronId', List['SynapseId']]:
        """Create connectivity pattern.
        
        Args:
            pattern_type: Type of pattern ("scale_free", "modular", "random")
            **params: Pattern parameters
            
        Returns:
            Dictionary mapping neurons to their synapses
        """
    
    def applyPattern(self, brain: 'Brain', pattern: Dict['NeuronId', List['SynapseId']]) -> None:
        """Apply connectivity pattern to brain.
        
        Args:
            brain: Brain instance
            pattern: Connectivity pattern
        """
```

#### NeuralPopulationManager

Neural population management and dynamics.

```python
class NeuralPopulationManager:
    """Neural population management and dynamics."""
    
    def __init__(self):
        """Initialize population manager."""
    
    def createPopulation(self, name: str, neuron_type: 'NeuronType', size: int) -> 'PopulationId':
        """Create neural population.
        
        Args:
            name: Population name
            neuron_type: Type of neurons
            size: Population size
            
        Returns:
            Population identifier
        """
    
    def getPopulationStats(self, population_id: 'PopulationId') -> Dict[str, Any]:
        """Get population statistics.
        
        Args:
            population_id: Population identifier
            
        Returns:
            Dictionary of statistics
        """
```

### Data Analysis APIs

#### NeuralSignalProcessor

Advanced neural signal processing.

```python
class NeuralSignalProcessor:
    """Advanced neural signal processing."""
    
    def __init__(self):
        """Initialize neural signal processor."""
    
    def filterSignal(self, signal: List[float], filter_type: str, **params) -> List[float]:
        """Filter neural signal.
        
        Args:
            signal: Input signal
            filter_type: Type of filter ("butterworth", "kalman", "wavelet")
            **params: Filter parameters
            
        Returns:
            Filtered signal
        """
    
    def extractFeatures(self, signal: List[float]) -> Dict[str, float]:
        """Extract features from neural signal.
        
        Args:
            signal: Input signal
            
        Returns:
            Dictionary of extracted features
        """
```

#### PatternRecognizer

Pattern recognition for neural activity.

```python
class PatternRecognizer:
    """Pattern recognition for neural activity."""
    
    def __init__(self, recognition_method: str = "template"):
        """Initialize pattern recognizer.
        
        Args:
            recognition_method: Recognition method
        """
    
    def recognizePattern(self, activity_pattern: List[float]) -> Dict[str, Any]:
        """Recognize pattern in neural activity.
        
        Args:
            activity_pattern: Neural activity pattern
            
        Returns:
            Pattern recognition results
        """
    
    def trainPattern(self, pattern: List[float], label: str) -> None:
        """Train pattern recognizer.
        
        Args:
            pattern: Pattern to train on
            label: Pattern label
        """
```

#### StatisticalAnalyzer

Statistical analysis of neural data.

```python
class StatisticalAnalyzer:
    """Statistical analysis of neural data."""
    
    def __init__(self):
        """Initialize statistical analyzer."""
    
    def analyzeDistribution(self, data: List[float]) -> Dict[str, Any]:
        """Analyze data distribution.
        
        Args:
            data: Input data
            
        Returns:
            Distribution analysis
        """
    
    def correlationAnalysis(self, data1: List[float], data2: List[float]) -> Dict[str, float]:
        """Perform correlation analysis.
        
        Args:
            data1: First dataset
            data2: Second dataset
            
        Returns:
            Correlation results
        """
```

#### MLIntegration

Machine learning pipeline integration.

```python
class MLIntegration:
    """Machine learning pipeline integration."""
    
    def __init__(self, ml_framework: str = "scikit-learn"):
        """Initialize ML integration.
        
        Args:
            ml_framework: Machine learning framework
        """
    
    def trainModel(self, X: List[List[float]], y: List[float], model_type: str) -> Any:
        """Train machine learning model.
        
        Args:
            X: Training features
            y: Training labels
            model_type: Type of model
            
        Returns:
            Trained model
        """
    
    def predict(self, model: Any, X: List[List[float]]) -> List[float]:
        """Make predictions.
        
        Args:
            model: Trained model
            X: Input features
            
        Returns:
            Predictions
        """
```

#### FeatureExtractor

Advanced feature extraction for neural data.

```python
class FeatureExtractor:
    """Advanced feature extraction for neural data."""
    
    def __init__(self):
        """Initialize feature extractor."""
    
    def extractTimeDomainFeatures(self, signal: List[float]) -> Dict[str, float]:
        """Extract time-domain features.
        
        Args:
            signal: Input signal
            
        Returns:
            Time-domain features
        """
    
    def extractFrequencyDomainFeatures(self, signal: List[float], sample_rate: float) -> Dict[str, float]:
        """Extract frequency-domain features.
        
        Args:
            signal: Input signal
            sample_rate: Sample rate
            
        Returns:
            Frequency-domain features
        """
```

### Custom Architecture APIs

#### CustomNeuronModel

Custom neuron models for expert users.

```python
class CustomNeuronModel:
    """Custom neuron models for expert users."""
    
    def __init__(self, model_type: str = "biophysical_lif"):
        """Initialize custom neuron model.
        
        Args:
            model_type: Type of neuron model
        """
    
    def createNeuron(self, params: Dict[str, Any]) -> 'NeuronId':
        """Create neuron with parameters.
        
        Args:
            params: Neuron parameters
            
        Returns:
            Neuron identifier
        """
    
    def updateNeuron(self, neuron_id: 'NeuronId', inputs: List[float], dt: float) -> float:
        """Update neuron state.
        
        Args:
            neuron_id: Neuron identifier
            inputs: Synaptic inputs
            dt: Time step
            
        Returns:
            Neuron output
        """
```

#### SynapticDynamics

Complex synaptic behavior models.

```python
class SynapticDynamics:
    """Complex synaptic behavior models."""
    
    def __init__(self, dynamics_type: str = "stdp"):
        """Initialize synaptic dynamics.
        
        Args:
            dynamics_type: Type of dynamics
        """
    
    def updateSynapse(self, synapse_id: 'SynapseId', pre_spike: bool, post_spike: bool) -> None:
        """Update synaptic state.
        
        Args:
            synapse_id: Synapse identifier
            pre_spike: Pre-synaptic spike
            post_spike: Post-synaptic spike
        """
    
    def getSynapticWeight(self, synapse_id: 'SynapseId') -> float:
        """Get synaptic weight.
        
        Args:
            synapse_id: Synapse identifier
            
        Returns:
            Synaptic weight
        """
```

#### RegionConfiguration

Anatomical and functional region configuration.

```python
class RegionConfiguration:
    """Anatomical and functional region configuration."""
    
    def __init__(self):
        """Initialize region configuration."""
    
    def createRegion(self, name: str, region_type: str, boundaries: Dict[str, float]) -> 'RegionId':
        """Create region.
        
        Args:
            name: Region name
            region_type: Type of region
            boundaries: Region boundaries
            
        Returns:
            Region identifier
        """
    
    def assignNeuronsToRegion(self, region_id: 'RegionId', neuron_ids: List['NeuronId']) -> None:
        """Assign neurons to region.
        
        Args:
            region_id: Region identifier
            neuron_ids: List of neuron identifiers
        """
```

#### NetworkTopology

Scale-free and modular network topologies.

```python
class NetworkTopology:
    """Scale-free and modular network topologies."""
    
    def __init__(self, topology_type: str = "scale_free"):
        """Initialize network topology.
        
        Args:
            topology_type: Type of topology
        """
    
    def generateTopology(self, num_neurons: int, **params) -> Dict['NeuronId', List['SynapseId']]:
        """Generate network topology.
        
        Args:
            num_neurons: Number of neurons
            **params: Topology parameters
            
        Returns:
            Dictionary mapping neurons to their synapses
        """
    
    def calculateClusteringCoefficient(self, topology: Dict['NeuronId', List['SynapseId']]) -> float:
        """Calculate clustering coefficient.
        
        Args:
            topology: Network topology
            
        Returns:
            Clustering coefficient
        """
```

### Configuration APIs

#### DynamicConfig

Dynamic configuration with runtime updates.

```python
class DynamicConfig:
    """Dynamic configuration with runtime updates."""
    
    def __init__(self, initial_config: Optional[Dict[str, Any]] = None):
        """Initialize dynamic configuration.
        
        Args:
            initial_config: Initial configuration
        """
    
    def loadFromJson(self, filename: str) -> None:
        """Load configuration from JSON file.
        
        Args:
            filename: Path to JSON file
        """
    
    def updateConfig(self, updates: Dict[str, Any]) -> None:
        """Update configuration dynamically.
        
        Args:
            updates: Configuration updates
        """
    
    def validate(self) -> None:
        """Validate configuration.
        
        Raises:
            ConfigurationError: If configuration is invalid
        """
```

#### ConfigInheritance

Hierarchical configuration management.

```python
class ConfigInheritance:
    """Hierarchical configuration management."""
    
    def __init__(self):
        """Initialize configuration inheritance."""
    
    def inheritFrom(self, parent_config: Dict[str, Any], child_config: Dict[str, Any]) -> Dict[str, Any]:
        """Inherit child configuration from parent.
        
        Args:
            parent_config: Parent configuration
            child_config: Child configuration
            
        Returns:
            Merged configuration
        """
    
    def resolveInheritance(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Resolve configuration inheritance.
        
        Args:
            config: Configuration with inheritance references
            
        Returns:
            Resolved configuration
        """
```

#### ConfigValidator

Schema-based configuration validation.

```python
class ConfigValidator:
    """Schema-based configuration validation."""
    
    def __init__(self, schema: Dict[str, Any]):
        """Initialize configuration validator.
        
        Args:
            schema: Configuration schema
        """
    
    def validate(self, config: Dict[str, Any]) -> List[str]:
        """Validate configuration against schema.
        
        Args:
            config: Configuration to validate
            
        Returns:
            List of validation errors (empty if valid)
        """
```

#### ConfigOptimizer

Performance-tuning configuration optimization.

```python
class ConfigOptimizer:
    """Performance-tuning configuration optimization."""
    
    def __init__(self):
        """Initialize configuration optimizer."""
    
    def optimizeForPerformance(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Optimize configuration for performance.
        
        Args:
            config: Input configuration
            
        Returns:
            Optimized configuration
        """
    
    def optimizeForAccuracy(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Optimize configuration for accuracy.
        
        Args:
            config: Input configuration
            
        Returns:
            Optimized configuration
        """
```

#### ConfigSnapshot

Version control for configurations.

```python
class ConfigSnapshot:
    """Version control for configurations."""
    
    def __init__(self, initial_config: Dict[str, Any]):
        """Initialize configuration snapshot.
        
        Args:
            initial_config: Initial configuration
        """
    
    def createSnapshot(self, name: str) -> str:
        """Create configuration snapshot.
        
        Args:
            name: Snapshot name
            
        Returns:
            Snapshot identifier
        """
    
    def restoreSnapshot(self, snapshot_id: str) -> Dict[str, Any]:
        """Restore configuration from snapshot.
        
        Args:
            snapshot_id: Snapshot identifier
            
        Returns:
            Restored configuration
        """
```

### Experiment Control APIs

#### CustomExperimentLoop

Custom experiment loops for expert control.

```python
class CustomExperimentLoop:
    """Custom experiment loops for expert control."""
    
    def __init__(self, duration: float = 100.0, time_step: float = 0.001,
                 loop_functions: Optional[List[Callable]] = None):
        """Initialize custom experiment loop.
        
        Args:
            duration: Experiment duration
            time_step: Simulation time step
            loop_functions: Functions to execute in loop
        """
    
    def addLoopFunction(self, function: Callable, name: str = "") -> None:
        """Add function to experiment loop.
        
        Args:
            function: Function to add
            name: Function name
        """
    
    def run(self, initial_state: Optional[Dict] = None) -> Dict:
        """Run experiment loop.
        
        Args:
            initial_state: Initial experiment state
            
        Returns:
            Experiment results
        """
```

#### ParallelExperimentRunner

Parallel experiment execution for expert control.

```python
class ParallelExperimentRunner:
    """Parallel experiment execution for expert control."""
    
    def __init__(self, max_workers: int = 4, execution_mode: str = "threads"):
        """Initialize parallel experiment runner.
        
        Args:
            max_workers: Maximum number of workers
            execution_mode: Execution mode ("threads" or "processes")
        """
    
    def addExperiment(self, experiment_id: str, experiment_func: Callable,
                      params: Optional[Dict] = None) -> None:
        """Add experiment to runner.
        
        Args:
            experiment_id: Experiment identifier
            experiment_func: Experiment function
            params: Experiment parameters
        """
    
    def runExperiments(self, experiment_ids: Optional[List[str]] = None) -> Dict:
        """Run experiments in parallel.
        
        Args:
            experiment_ids: List of experiment IDs to run
            
        Returns:
            Experiment results
        """
```

#### CheckpointManager

Experiment checkpointing for expert control.

```python
class CheckpointManager:
    """Experiment checkpointing for expert control."""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints", auto_checkpoint: bool = True,
                 checkpoint_interval: float = 60.0):
        """Initialize checkpoint manager.
        
        Args:
            checkpoint_dir: Directory for checkpoints
            auto_checkpoint: Enable automatic checkpointing
            checkpoint_interval: Checkpoint interval in seconds
        """
    
    def createCheckpoint(self, checkpoint_name: str, experiment_state: Dict,
                         additional_data: Optional[Dict] = None) -> str:
        """Create experiment checkpoint.
        
        Args:
            checkpoint_name: Checkpoint name
            experiment_state: Experiment state to checkpoint
            additional_data: Additional checkpoint data
            
        Returns:
            Checkpoint name
        """
    
    def restoreCheckpoint(self, checkpoint_name: str, target_state: Dict) -> Dict:
        """Restore experiment state from checkpoint.
        
        Args:
            checkpoint_name: Checkpoint name
            target_state: Target experiment state
            
        Returns:
            Restored experiment state
        """
```

#### ResultAnalyzer

Statistical analysis of experiment results for expert control.

```python
class ResultAnalyzer:
    """Statistical analysis of experiment results for expert control."""
    
    def __init__(self, analysis_methods: Optional[List[str]] = None):
        """Initialize result analyzer.
        
        Args:
            analysis_methods: Analysis methods to use
        """
    
    def analyzeResults(self, results: List[Dict], analysis_type: str = "statistical") -> Dict:
        """Analyze experiment results.
        
        Args:
            results: Experiment results
            analysis_type: Type of analysis
            
        Returns:
            Analysis results
        """
```

#### ExperimentMonitor

Monitor experiment execution for expert control.

```python
class ExperimentMonitor:
    """Monitor experiment execution for expert control."""
    
    def __init__(self, monitoring_interval: float = 1.0):
        """Initialize experiment monitor.
        
        Args:
            monitoring_interval: Monitoring interval in seconds
        """
    
    def startMonitoring(self, experiment_id: str, monitoring_func: Callable) -> None:
        """Start monitoring experiment.
        
        Args:
            experiment_id: Experiment identifier
            monitoring_func: Monitoring function
        """
    
    def stopMonitoring(self, experiment_id: str) -> Dict:
        """Stop monitoring experiment.
        
        Args:
            experiment_id: Experiment identifier
            
        Returns:
            Monitoring results
        """
```

### Integration APIs

#### CXXExtensionAPI

Direct C++ extension APIs.

```python
class CXXExtensionAPI:
    """Direct C++ extension APIs."""
    
    def __init__(self):
        """Initialize C++ extension API."""
    
    def callFunction(self, function_name: str, args: List[Any]) -> Any:
        """Call C++ function.
        
        Args:
            function_name: Name of C++ function
            args: Function arguments
            
        Returns:
            Function result
        """
```

#### CustomIOFormat

Multiple data format support.

```python
class CustomIOFormat:
    """Multiple data format support."""
    
    def __init__(self, format_type: str = "hdf5"):
        """Initialize custom I/O format.
        
        Args:
            format_type: Type of format
        """
    
    def readData(self, filename: str) -> Any:
        """Read data from file.
        
        Args:
            filename: Path to file
            
        Returns:
            Read data
        """
    
    def writeData(self, filename: str, data: Any) -> None:
        """Write data to file.
        
        Args:
            filename: Path to file
            data: Data to write
        """
```

#### SensorIntegration

External sensor integration.

```python
class SensorIntegration:
    """External sensor integration."""
    
    def __init__(self, sensor_type: str = "neurosensor"):
        """Initialize sensor integration.
        
        Args:
            sensor_type: Type of sensor
        """
    
    def connectSensor(self, sensor_config: Dict[str, Any]) -> None:
        """Connect external sensor.
        
        Args:
            sensor_config: Sensor configuration
        """
    
    def readSensorData(self) -> Any:
        """Read sensor data.
        
        Returns:
            Sensor data
        """
```

#### WebSocketAPI

Remote control interfaces.

```python
class WebSocketAPI:
    """Remote control interfaces."""
    
    def __init__(self, websocket_url: str):
        """Initialize WebSocket API.
        
        Args:
            websocket_url: WebSocket URL
        """
    
    def connect(self) -> None:
        """Connect to WebSocket server."""
    
    def disconnect(self) -> None:
        """Disconnect from WebSocket server."""
    
    def sendCommand(self, command: str) -> Any:
        """Send command to server.
        
        Args:
            command: Command to send
            
        Returns:
            Command result
        """
```

#### ExternalResourceManager

External resource management.

```python
class ExternalResourceManager:
    """External resource management."""
    
    def __init__(self):
        """Initialize external resource manager."""
    
    def allocateResource(self, resource_type: str, params: Dict[str, Any]) -> Any:
        """Allocate external resource.
        
        Args:
            resource_type: Type of resource
            params: Resource parameters
            
        Returns:
            Allocated resource
        """
    
    def freeResource(self, resource: Any) -> None:
        """Free external resource.
        
        Args:
            resource: Resource to free
        """
```

### Visualization APIs

#### AdvancedPlotting

Scientific visualization.

```python
class AdvancedPlotting:
    """Scientific visualization."""
    
    def __init__(self):
        """Initialize advanced plotting."""
    
    def createPlot(self, data: Dict[str, Any], plot_type: str) -> Any:
        """Create plot from data.
        
        Args:
            data: Plot data
            plot_type: Type of plot
            
        Returns:
            Plot object
        """
    
    def savePlot(self, plot: Any, filename: str) -> None:
        """Save plot to file.
        
        Args:
            plot: Plot object
            filename: Path to save plot
        """
```

#### RealTimeVisualizer

Interactive real-time visualization.

```python
class RealTimeVisualizer:
    """Interactive real-time visualization."""
    
    def __init__(self, pipeline: Any):
        """Initialize real-time visualizer.
        
        Args:
            pipeline: Visualization pipeline
        """
    
    def render(self, data: Any, options: Dict[str, Any]) -> Any:
        """Render visualization.
        
        Args:
            data: Visualization data
            options: Rendering options
            
        Returns:
            Rendered visualization
        """
```

#### VisualizationPipeline

Complex visualization workflows.

```python
class VisualizationPipeline:
    """Complex visualization workflows."""
    
    def __init__(self):
        """Initialize visualization pipeline."""
    
    def addStep(self, step_name: str, data: Any) -> None:
        """Add step to pipeline.
        
        Args:
            step_name: Step name
            data: Step data
        """
    
    def runPipeline(self) -> Dict[str, Any]:
        """Run visualization pipeline.
        
        Returns:
            Pipeline results
        """
```

#### DataExporter

Multiple export formats.

```python
class DataExporter:
    """Multiple export formats."""
    
    def __init__(self, export_format: str = "json"):
        """Initialize data exporter.
        
        Args:
            export_format: Export format
        """
    
    def exportData(self, data: Any, filename: str) -> None:
        """Export data to file.
        
        Args:
            data: Data to export
            filename: Path to output file
        """
```

### Utility APIs

#### MemoryProfiler

Memory usage analysis.

```python
class MemoryProfiler:
    """Memory usage analysis."""
    
    def __init__(self):
        """Initialize memory profiler."""
    
    def takeSnapshot(self, tag: str = "") -> Dict:
        """Take memory snapshot.
        
        Args:
            tag: Snapshot tag
            
        Returns:
            Memory snapshot
        """
    
    def analyzeGrowth(self, time_window: float = 60.0) -> Dict:
        """Analyze memory growth.
        
        Args:
            time_window: Analysis time window in seconds
            
        Returns:
            Growth analysis
        """
```

#### PerformanceProfiler

Comprehensive performance metrics.

```python
class PerformanceProfiler:
    """Comprehensive performance metrics."""
    
    def __init__(self):
        """Initialize performance profiler."""
    
    def startProfiling(self, operation: str) -> None:
        """Start profiling operation.
        
        Args:
            operation: Operation to profile
        """
    
    def stopProfiling(self, operation: str) -> float:
        """Stop profiling operation.
        
        Args:
            operation: Operation to stop profiling
            
        Returns:
            Execution time
        """
    
    def getStats(self, operation: str) -> Dict:
        """Get profiling statistics.
        
        Args:
            operation: Operation to get stats for
            
        Returns:
            Performance statistics
        """
```

#### BenchmarkRunner

Standardized performance testing.

```python
class BenchmarkRunner:
    """Standardized performance testing."""
    
    def __init__(self, warmup_iterations: int = 10, benchmark_iterations: int = 100):
        """Initialize benchmark runner.
        
        Args:
            warmup_iterations: Number of warmup iterations
            benchmark_iterations: Number of benchmark iterations
        """
    
    def runBenchmark(self, func: Callable, *args, **kwargs) -> Dict:
        """Run benchmark for function.
        
        Args:
            func: Function to benchmark
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Benchmark results
        """
```

#### ProfilingSession

Complete profiling workflows.

```python
class ProfilingSession:
    """Complete profiling workflows."""
    
    def __init__(self):
        """Initialize profiling session."""
    
    def startSession(self) -> None:
        """Start profiling session."""
    
    def stopSession(self) -> Dict:
        """Stop profiling session.
        
        Returns:
            Session results
        """
    
    def analyzeResults(self, session_data: Dict) -> Dict:
        """Analyze profiling results.
        
        Args:
            session_data: Profiling session data
            
        Returns:
            Analysis results
        """
```

## Factory Functions

### Core Factory Functions

```python
# Configuration factory functions
def create_default_config() -> Config:
    """Create default configuration.
    
    Returns:
        Default configuration
    """

def create_minimal_config() -> Config:
    """Create minimal configuration.
    
    Returns:
        Minimal configuration
    """

def create_high_performance_config() -> Config:
    """Create high performance configuration.
    
    Returns:
        High performance configuration
    """

def create_grid_world_config() -> Config:
    """Create grid world configuration.
    
    Returns:
        Grid world configuration
    """

def create_maze_world_config() -> Config:
    """Create maze world configuration.
    
    Returns:
        Maze world configuration
    """

# Brain factory functions
def create_brain(config: Config) -> 'Brain':
    """Create brain with configuration.
    
    Args:
        config: Brain configuration
        
    Returns:
        Brain instance
    """

def create_simple_world() -> 'SimpleWorld':
    """Create simple world.
    
    Returns:
        Simple world instance
    """

def create_agent_brain(brain: 'Brain') -> 'AgentBrain':
    """Create agent brain.
    
    Args:
        brain: Underlying brain
        
    Returns:
        Agent brain instance
    """

# Control factory functions
def create_simulation_controller() -> Any:
    """Create simulation controller.
    
    Returns:
        Simulation controller instance
    """

def create_performance_monitor() -> Any:
    """Create performance monitor.
    
    Returns:
        Performance monitor instance
    """

def create_batch_processor(name: str) -> Any:
    """Create batch processor.
    
    Args:
        name: Processor name
        
    Returns:
        Batch processor instance
    """

def create_experiment_manager() -> Any:
    """Create experiment manager.
    
    Returns:
        Experiment manager instance
    """
```

### Expert Factory Functions

```python
# Expert performance factory functions
def createMemoryPool(initial_size: int = 1024, max_size: int = 65536) -> expert.MemoryPool:
    """Create memory pool.
    
    Args:
        initial_size: Initial pool size
        max_size: Maximum pool size
        
    Returns:
        Memory pool instance
    """

def createSIMDProcessor(vector_width: int = 256) -> expert.SIMDProcessor:
    """Create SIMD processor.
    
    Args:
        vector_width: SIMD vector width
        
    Returns:
        SIMD processor instance
    """

def createParallelProcessor(num_threads: int = 4) -> expert.ParallelProcessor:
    """Create parallel processor.
    
    Args:
        num_threads: Number of threads
        
    Returns:
        Parallel processor instance
    """

# Expert simulation factory functions
def createCustomPlasticityRules() -> expert.CustomPlasticityRules:
    """Create custom plasticity rules.
    
    Returns:
        Custom plasticity rules instance
    """

def createAdvancedNeuromodulation(config: Dict[str, Any]) -> expert.AdvancedNeuromodulation:
    """Create advanced neuromodulation.
    
    Args:
        config: Neuromodulation configuration
        
    Returns:
        Advanced neuromodulation instance
    """

# Expert analysis factory functions
def createNeuralSignalProcessor() -> expert.NeuralSignalProcessor:
    """Create neural signal processor.
    
    Returns:
        Neural signal processor instance
    """

def createPatternRecognizer(recognition_method: str = "template") -> expert.PatternRecognizer:
    """Create pattern recognizer.
    
    Args:
        recognition_method: Recognition method
        
    Returns:
        Pattern recognizer instance
    """

def createStatisticalAnalyzer() -> expert.StatisticalAnalyzer:
    """Create statistical analyzer.
    
    Returns:
        Statistical analyzer instance
    """

# Expert architecture factory functions
def createCustomNeuronModel(model_type: str = "biophysical_lif") -> expert.CustomNeuronModel:
    """Create custom neuron model.
    
    Args:
        model_type: Type of neuron model
        
    Returns:
        Custom neuron model instance
    """

def createSynapticDynamics(dynamics_type: str = "stdp") -> expert.SynapticDynamics:
    """Create synaptic dynamics.
    
    Args:
        dynamics_type: Type of dynamics
        
    Returns:
        Synaptic dynamics instance
    """

# Expert experiment factory functions
def createCustomExperimentLoop(duration: float = 100.0, time_step: float = 0.001) -> expert.CustomExperimentLoop:
    """Create custom experiment loop.
    
    Args:
        duration: Experiment duration
        time_step: Time step
        
    Returns:
        Custom experiment loop instance
    """

def createParallelExperimentRunner(max_workers: int = 4) -> expert.ParallelExperimentRunner:
    """Create parallel experiment runner.
    
    Args:
        max_workers: Maximum number of workers
        
    Returns:
        Parallel experiment runner instance
    """
```

### Visualization Factory Functions

```python
# Visualization factory functions
def generate_neuron_activity_plot(brain: 'Brain') -> Any:
    """Generate neuron activity plot.
    
    Args:
        brain: Brain instance
        
    Returns:
        Neuron activity plot
    """

def generate_connection_density_plot(brain: 'Brain') -> Any:
    """Generate connection density plot.
    
    Args:
        brain: Brain instance
        
    Returns:
        Connection density plot
    """
```

### Serialization Factory Functions

```python
# Serialization factory functions
def vector_to_json(vector: List[float]) -> str:
    """Convert vector to JSON string.
    
    Args:
        vector: Input vector
        
    Returns:
        JSON string
    """

def json_to_vector(json_str: str) -> List[float]:
    """Convert JSON string to vector.
    
    Args:
        json_str: JSON string
        
    Returns:
        Vector
    """

def object_to_json(obj: Any) -> str:
    """Convert object to JSON string.
    
    Args:
        obj: Object to convert
        
    Returns:
        JSON string
    """

def json_to_object(json_str: str) -> Any:
    """Convert JSON string to object.
    
    Args:
        json_str: JSON string
        
    Returns:
        Object
    """
```

## Exception Types

### NLMError

Base exception for all NLM errors.

```python
class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: str = "general"):
        """Initialize NLM error.
        
        Args:
            message: Error message
            error_code: Error code
        """
```

### ConfigurationError

Configuration-related errors.

```python
class ConfigurationError(NLMError):
    """Configuration-related errors."""
    
    def __init__(self, message: str, config_key: str = ""):
        """Initialize configuration error.
        
        Args:
            message: Error message
            config_key: Configuration key that caused error
        """
```

### SimulationError

Simulation-related errors.

```python
class SimulationError(NLMError):
    """Simulation-related errors."""
    
    def __init__(self, message: str, step: int = 0):
        """Initialize simulation error.
        
        Args:
            message: Error message
            step: Simulation step
        """
```

### ValidationError

Validation-related errors.

```python
class ValidationError(NLMError):
    """Validation-related errors."""
    
    def __init__(self, message: str, validation_rule: str = ""):
        """Initialize validation error.
        
        Args:
            message: Error message
            validation_rule: Validation rule that failed
        """
```

## Configuration Options

### Core Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| brain.simulated_neurons | int | 1000 | Number of simulated neurons |
| brain.min_plasticity_rate | float | 0.001 | Minimum plasticity rate |
| brain.max_plasticity_rate | float | 0.1 | Maximum plasticity rate |
| brain.development_stage_length | int | 1000 | Length of development stages |
| brain.structure_formation | bool | true | Enable structure formation |
| world.width | int | 20 | World width in grid cells |
| world.height | int | 20 | World height in grid cells |
| world.view_distance | int | 64 | Sensory view distance |
| world.cell_size | float | 1.0 | Size of each grid cell |
| neuromodulation.dopamine_influence | float | 0.8 | Dopamine influence strength |
| neuromodulation.curiosity_strength | float | 0.6 | Curiosity drive strength |
| neuromodulation.novelty_threshold | float | 0.5 | Novelty detection threshold |

### Performance Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| performance.memory_pool_size | int | 8192 | Memory pool size |
| performance.enable_ssimd | bool | true | Enable SSE/SIMD instructions |
| performance.parallel_threads | int | 4 | Number of parallel threads |
| performance.memory_profiling | bool | false | Enable memory profiling |
| performance.benchmark_mode | bool | false | Enable benchmark mode |
| performance.zero_copy_enabled | bool | false | Enable zero-copy interfaces |

### Expert Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| expert.memory_pool_enabled | bool | true | Enable memory pool |
| expert.simd_processor_enabled | bool | true | Enable SIMD processor |
| expert.parallel_processor_enabled | bool | true | Enable parallel processor |
| expert.custom_allocator_enabled | bool | false | Enable custom allocator |
| expert.custom_plasticity_rules_enabled | bool | false | Enable custom plasticity rules |
| expert.advanced_neuromodulation_enabled | bool | false | Enable advanced neuromodulation |
| expert.custom_connectivity_patterns_enabled | bool | false | Enable custom connectivity patterns |
| expert.neural_population_manager_enabled | bool | false | Enable neural population manager |
| expert.neural_signal_processing_enabled | bool | false | Enable neural signal processing |
| expert.pattern_recognizer_enabled | bool | false | Enable pattern recognizer |
| expert.ml_integration_enabled | bool | false | Enable ML integration |
| expert.feature_extractor_enabled | bool | false | Enable feature extractor |
| expert.custom_neuron_models_enabled | bool | false | Enable custom neuron models |
| expert.synaptic_dynamics_enabled | bool | false | Enable synaptic dynamics |
| expert.region_configuration_enabled | bool | false | Enable region configuration |
| expert.network_topology_enabled | bool | false | Enable network topology |
| expert.custom_experiment_loop_enabled | bool | false | Enable custom experiment loop |
| expert.parallel_experiment_execution_enabled | bool | false | Enable parallel experiment execution |
| expert.checkpointing_enabled | bool | false | Enable checkpointing |
| expert.result_analysis_enabled | bool | false | Enable result analysis |
| expert.experiment_monitoring_enabled | bool | false | Enable experiment monitoring |
| expert.advanced_plotting_enabled | bool | false | Enable advanced plotting |
| expert.real_time_visualization_enabled | bool | false | Enable real-time visualization |
| expert.custom_visualization_pipeline_enabled | bool | false | Enable custom visualization pipeline |
| expert.data_export_enabled | bool | false | Enable data export |

## Type Definitions

### Core Types

```python
# Neuron and synapse identifiers
type NeuronId = int
type SynapseId = int
type RegionId = int
type PopulationId = int

# Neural component types
type NeuronType = str  # "excitatory", "inhibitory", "custom"
type SynapseType = str  # "excitatory", "inhibitory", "modulatory"

# Developmental stages
type DevelopmentalStage = str  # "embryonic", "infant", "juvenile", "adult"

# Neural firing states
type FiringState = str  # "resting", "firing", "adapting", "refractory"

# Action types
type ActionType = str  # "move", "look", "rest", "custom"

# Motor command
type MotorCommand = Dict[str, Any]

# World object types
type WorldObjectType = str  # "agent", "food", "obstacle", "treasure"
```

### Expert API Type Aliases

```python
# Performance types
type MemoryPoolId = str
type SIMDRegister = List[float]

# Simulation types
type PlasticityRuleId = str
type NeuromodulatorType = str

# Analysis types
type SignalFeature = Dict[str, float]
type PatternMatch = Dict[str, Any]

# Experiment types
type ExperimentId = str
type CheckpointId = str

# Visualization types
type PlotId = str
type VisualizationPipeline = List[Dict[str, Any]]

# Utility types
type MemorySnapshot = Dict[str, Any]
type PerformanceMetric = Dict[str, float]
```

## References

1. [NLM Project Documentation](https://github.com/Kilo-Org/kilocode/tree/main/docs)
2. [Expert API Summary](https://github.com/Kilo-Org/kilocode/tree/main/python/expert/EXPERT_API_SUMMARY.md)
3. [Phase 6 Final Audit](https://github.com/Kilo-Org/kilocode/tree/main/docs/PHASE6_FINAL_AUDIT.md)

## Changelog

### Version 2.0.0
- Complete API documentation
- Expert module integration
- Enhanced error handling
- Performance optimizations
- New factory functions
- Comprehensive examples

### Version 1.0.0
- Initial release
- Basic API
- Core functionality

## Acknowledgments

This documentation was created to support the enhanced NLM Python API, providing comprehensive coverage for users of all skill levels.

## License

MIT License. See LICENSE file for details.

## Author

NLM Team