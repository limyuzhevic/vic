# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the experimental artificial brain simulation. This documentation covers all available classes, functions, and advanced features.

## Quick Start Examples

### Basic Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Simulation

```python
import pynlm

# Setup complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent performance
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Simulation complete!")
print(f"Final stats: {brain.getTotalSpikeCount()} spikes, {agent.getCuriosityLevel():.3f} curiosity")
```

### Advanced Configuration

```python
import pynlm

# Create advanced configuration
params = [
    ("neuron_count", 2000),
    ("connection_probability", 0.2),
    ("plasticity.stdp.enable", True),
    ("plasticity.hebbian.enable", True),
    ("plasticity.structural.enable", True),
    ("neuromod.dopamine.scale", 1.5f),
    ("neuromod.curiosity.enable", True),
    ("neuromod.novelty.enable", True),
    ("development.enabled", True)
]

config = pynlm.createAdvancedConfig(params)
brain = pynlm.createAdvancedBrain(config)
brain.initialize()
```

### Brain State Management

```python
import pynlm

# Create and save brain state
brain = pynlm.createDefaultBrain(1000)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Save brain state
brain.save("my_brain_state.nlm")

# Load brain state later
new_brain = pynlm.createDefaultBrain(1000)
new_brain.load("my_brain_state.nlm")

print(f"Loaded brain with {new_brain.getTotalNeuronCount()} neurons")
```

### Simulation Control and Monitoring

```python
import pynlm

# Production brain with specific configuration
brain = pynlm.createProductionBrain(
    neuronCount=500,
    connectionProb=0.15,
    enablePlasticity=True
)
brain.initialize()

# Run simulation with progress monitoring
result = pynlm.runBrainSimulation(
    brain=brain,
    config=std::make_shared(pynlm.getConfig()),
    numSteps=2000,
    recordProgress=True,
    useAllFeatures=True
)

print(f"Simulation completed: {result['total_spikes']} spikes in {result['final_step']} steps")
print(f"Average firing rate: {result['avg_firing_rate']:.2f} Hz")
```

## Core Classes

### Brain

The central neural simulation brain class with complete Phase 6 integration.

**Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(currentStep)` - Perform simulation step
- `step(currentStep, currentTime)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `produceAction()` - Produce motor action from neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Total neurons across regions
- `getTotalSynapseCount()` - Total synapses across regions
- `getAverageFiringRate()` - Average firing rate
- `getExcitationInhibitionRatio()` - E/I balance
- `getDevelopmentalStage()` - Current developmental stage

**Properties:**
- `total_spikes` - Total spikes generated
- `firing_neurons` - Currently firing neurons
- `avg_firing_rate` - Average firing rate in Hz
- `e_i_ratio` - Excitation/inhibition ratio

### AgentBrain

Connects NLM brain to world, handling sensory transduction and motor decoding.

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getThoughtTrace()` - Get thought trace

**Enable/Disable Methods:**
- `enableRewardModulation(True/False)`
- `enableStructuralPlasticity(True/False)`
- `enableDevelopment(True/False)`
- `enableCuriosity(True/False)`

### SimpleWorld

Simple 2D world providing sensory experience without semantic labels.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state

### Config

Configuration class for NLM system parameters.

**Methods:**
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `get(key)` - Get value
- `getOr(key, defaultValue)` - Get with default
- `set(key, value)` - Set value
- `getKeys()` - Get all keys

### SensoryInput Classes

#### Vision
Vision sensory input with configurable dimensions.

**Properties:**
- `width`, `height`, `channels` - Vision dimensions
- `data` - Raw vision data

#### Audio
Audio sensory input with configurable sample rate.

**Properties:**
- `sampleRate` - Audio sample rate
- `numSamples` - Number of audio samples
- `data` - Raw audio data

#### InternalSignals
Internal signals (neural feedback) with signal management.

**Methods:**
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals
- `addMultipleSignals(values)` - Add multiple signals
- `getSignalCount()` - Get signal count
- `getSum()` - Get sum of all signals

### Action
Action representation for motor output with enhanced functionality.

**Properties:**
- `type` - Action type
- `parameters` - Action parameters

**Methods:**
- `getName()` - Get action name
- `clone()` - Clone action
- `addParameter(param)` - Add parameter
- `removeParameter(index)` - Remove parameter
- `toDict()` - Convert to dictionary
- `fromDict(dict)` - Load from dictionary

## Advanced Features

### Configuration Helpers

#### createAdvancedConfig(params)

Create configuration with multiple parameters.

**Parameters:**
- `params` - List of (key, value) pairs

**Returns:** Configured Config object

#### getConfigStats(config)

Get configuration statistics and analysis.

**Parameters:**
- `config` - Configuration to analyze

**Returns:** Dictionary with configuration statistics

### Simulation Helpers

#### runBrainSimulation(brain, config, numSteps, recordProgress, useAllFeatures)

Run a complete brain simulation with monitoring.

**Parameters:**
- `brain` - Brain to simulate
- `config` - Simulation configuration
- `numSteps` - Number of simulation steps
- `recordProgress` - Whether to record progress
- `useAllFeatures` - Whether to enable all features

**Returns:** Dictionary with simulation results

### Factory Functions

#### createProductionBrain(neuronCount, connectionProb, enablePlasticity)

Create production-ready brain with specific configuration.

**Parameters:**
- `neuronCount` - Number of neurons
- `connectionProb` - Connection probability
- `enablePlasticity` - Whether to enable plasticity

#### createResearchBrain(neuronCount, connectionProb, enableAllFeatures)

Create research brain with all features enabled.

**Parameters:**
- `neuronCount` - Number of neurons
- `connectionProb` - Connection probability
- `enableAllFeatures` - Whether to enable all features

### Brain Analysis

#### getBrainSummary(brain)

Get comprehensive brain status analysis.

**Parameters:**
- `brain` - Brain to analyze

**Returns:** String with brain summary

## Data Types and Enumerations

### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

### SynapseType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Electrical`
- `GapJunction`

### ActionType
- `MoveForward`
- `MoveBackward`
- `TurnLeft`
- `TurnRight`
- `Look`
- `Interact`
- `Eat`
- `Wait`
- `Rest`

### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

### FiringState
- `Resting`
- `Active`
- `Refractory`
- `Inhibited`

## Error Handling

The Python API provides comprehensive error handling:

1. **RuntimeError** - For runtime errors during simulation
2. **ValueError** - For invalid parameter values
3. **KeyError** - For missing configuration keys
4. **FileNotFoundError** - For missing files

Use try-catch blocks for robust error handling:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.ValueError as e:
    print(f"Invalid value: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

1. **Reuse Brain Objects:** Create brain objects once and reuse them for multiple simulations
2. **Batch Operations:** Use batch methods where available
3. **Memory Management:** Explicitly release memory when no longer needed
4. **Progress Monitoring:** Use progress recording for long simulations
5. **Configuration Caching:** Cache frequently used configurations

## Example: Learning Agent

```python
import pynlm
import time

class LearningAgent:
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.agent.initialize(self.world)
        self.enableAllLearning(True)
        
        self.step_count = 0
        self.reward_history = []
        
    def enableAllLearning(self, enable):
        self.agent.enableRewardModulation(enable)
        self.agent.enableStructuralPlasticity(enable)
        self.agent.enableDevelopment(enable)
        self.agent.enableCuriosity(enable)
        
    def runEpisode(self, maxSteps=1000):
        start_time = time.time()
        
        for step in range(maxSteps):
            # World update
            self.world.update(0.1)
            
            # Sense
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain process
            self.brain.step(step)
            
            # Act
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn
            reward = result.reward if result.success else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            self.step_count += 1
            self.reward_history.append(reward)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Action {action}, Reward {reward:.3f}")
                self.printStatus()
                
        end_time = time.time()
        print(f"Episode completed in {end_time - start_time:.2f} seconds")
        return self.getEpisodeStats()
        
    def printStatus(self):
        print(f"  Spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {self.agent.getPredictionError():.3f}")
        
    def getEpisodeStats(self):
        return {
            'steps': self.step_count,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_reward': sum(self.reward_history) / len(self.reward_history) if self.reward_history else 0,
            'max_reward': max(self.reward_history) if self.reward_history else 0,
            'min_reward': min(self.reward_history) if self.reward_history else 0,
            'curiosity_level': self.agent.getCuriosityLevel(),
            'final_firing_rate': self.brain.getAverageFiringRate()
        }

# Example usage
if __name__ == "__main__":
    agent = LearningAgent()
    stats = agent.runEpisode(1000)
    
    print("\nEpisode Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
```

## Installation and Setup

### Prerequisites

- Python 3.8+
- pip
- Compiled NLM library with Python bindings enabled

### Installation

```bash
# Install from compiled package
pip install pynlm

# Or install from source
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### Building from Source

```bash
# Clone NLM repository
# Navigate to project root
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
pip install pybind11 scikit-build-core
pip install .
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure NLM is installed with Python bindings enabled
   - Check that pynlm.so/.pyd is in Python path

2. **RuntimeError: Failed to initialize brain**
   - Check that configuration is valid
   - Ensure all required parameters are set
   - Verify brain is not already initialized

3. **ValueError: Invalid parameter value**
   - Check parameter types and ranges
   - Use default configuration for debugging
   - Validate parameters before use

4. **Memory issues**
   - Monitor memory usage during long simulations
   - Use explicit cleanup when no longer needed
   - Consider using smaller configurations for testing

### Getting Help

- Check the NLM documentation for detailed API references
- Report issues with detailed error messages
- Include configuration and simulation details when debugging
- Use the examples as starting points for your own implementations

## License

This documentation is provided under the MIT License. The NLM project itself is experimental and research-focused. Please see the main README.md for complete licensing information.