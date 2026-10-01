# NLM Enhanced Python Bindings

## Overview

This directory contains enhanced Python bindings for the NLM (Neural Learning Machine) neural simulation framework. The enhanced bindings provide comprehensive access to all neural systems and features of the NLM framework, with extensive documentation, examples, and utilities for research and development.

## Key Features

### 🚀 Enhanced Core Functionality

- **Complete Neural System Access**: All neural systems (memory, prediction, cognition, neuromodulation, development, plasticity) exposed
- **High-Level Convenience Functions**: Factory functions for common tasks
- **Expert APIs**: Advanced functionality for research
- **NumPy Integration**: Seamless integration with scientific Python ecosystem

### 📚 Comprehensive Documentation

- **Complete Docstrings**: Documentation for all bindings
- **Usage Examples**: Practical examples for all features
- **API Reference**: Complete API documentation
- **Research Workflows**: Templates for common research tasks

### 🔬 Research-Ready Features

- **Reproducible Research**: Fixed seed experiments
- **Experiment Framework**: High-level experiment management
- **Visualization Tools**: Advanced plotting and video export
- **Data Analysis**: Statistical analysis and pattern recognition
- **Performance Benchmarking**: Performance measurement tools

## Installation

### Prerequisites

```bash
# Python 3.8+
python3 --version

# Required dependencies
pip install pybind11>=2.11
pip install numpy>=1.20
pip install pytest>=7.0
```

### Install Enhanced Bindings

#### Option 1: Using Setup Script (Recommended)

```bash
# Install enhanced bindings
python3 setup_enhanced.py
```

#### Option 2: Manual Installation

```bash
# Clone repository
cd /path/to/nlm

# Build enhanced bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=python3
make -j4

# Install Python package
pip install -e .

# Install additional dependencies
pip install matplotlib opencv-python
```

#### Option 3: Using pip (Development)

```bash
# Install in development mode
cd /path/to/nlm
pip install -e .
```

## Quick Start

### 1. Basic Usage

```python
import pynlm as nlm

# Create default configuration
config = nlm.create_default_config()
print(f"Configuration: {config}")

# Create neural brain
brain = nlm.create_brain(config)
brain.initialize()
print(f"Brain initialized with {brain.getRegionCount()} regions")

# Run simulation
for i in range(10):
    brain.step(i)
    print(f"Step {i}: {brain.getActiveNeuronCount()} active neurons")

# Produce action
action = brain.produce_action()
print(f"Action: {action.getName()}")
```

### 2. Complete Agent-Environment System

```python
import pynlm as nlm

# Create complete system
config, world, brain, agent = nlm.create_complete_agent()
print(f"World: {world}")
print(f"Agent brain: {agent}")

# Run episode
stats = nlm.run_learning_episode(agent, world, max_steps=100)
print(f"Episode completed: {stats['total_reward']:.2f} reward in {stats['steps']} steps")
```

### 3. Advanced Memory Systems

```python
import pynlm as nlm

# Create brain
config, world, brain, agent = nlm.create_complete_agent()

# Create working memory
working_memory = nlm.NeuralWorkingMemory()
working_memory.initialize(brain)

# Store and retrieve patterns
vision_data = world.getSensoryPercept().getVision()
working_memory.store(vision_data)
pattern = working_memory.retrieve()

print(f"Working memory: {working_memory.getMemoryActivity():.3f} activity")
```

### 4. Neuromodulation

```python
import pynlm as nlm

# Create brain with neuromodulation
config, world, brain, agent = nlm.create_complete_agent()
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Get neuromodulation components
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

print(f"Neuromodulation levels:")
print(f"  - Dopamine: {dopamine.getLevel():.3f}")
print(f"  - Curiosity: {curiosity.getLevel():.3f}")
print(f"  - Novelty: {novelty.getLevel():.3f}")

# Update neuromodulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

### 5. NumPy Integration

```python
import pynlm as nlm
import numpy as np

# Create brain and world
config, world, brain, agent = nlm.create_complete_agent()

# Process vision data
percept = world.getSensoryPercept()
vision_array = nlm.to_numpy(percept.getVision())

# NumPy operations
mean_val = np.mean(vision_array)
std_val = np.std(vision_array)
max_val = np.max(vision_array)

print(f"Vision statistics: mean={mean_val:.3f}, std={std_val:.3f}, max={max_val:.3f}")
```

## API Reference

### Core Classes

#### Config
```python
class Config:
    def __init__() -> Config
    def loadFromFile(filepath: str) -> bool
    def loadFromArgs(argc: int, argv: List[str]) -> bool
    def saveToFile(filepath: str) -> bool
    def has(key: str) -> bool
    def getKeys() -> List[str]
    def clear() -> None
    def summary() -> str
    def get(key: str, default_value: Any = None) -> Any
    def set(key: str, value: Any, source: str = "Runtime") -> None
```

#### Brain
```python
class Brain:
    def __init__(config: Config) -> Brain
    def initialize() -> bool
    def step(currentStep: SimulationStep) -> None
    def step(currentStep: SimulationStep, currentTime: Timestamp) -> None
    def receiveSensoryInput(input: SensoryInput) -> None
    def produceAction() -> Action
    def reset() -> None
    def save(filepath: str) -> bool
    def load(filepath: str) -> bool
    # ... many more methods
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(brain: Brain) -> AgentBrain
    def initialize(world: SimpleWorld) -> None
    def processSensoryInput(percept: SensoryPercept) -> None
    def decodeMotorCommand() -> MotorCommand
    def applyRewardModulation(reward: float, predicted_reward: float) -> None
    def updateDevelopment(timestep: float) -> None
    # ... many more methods
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__() -> SimpleWorld
    def configure(width: float, height: float, visionWidth: int, visionHeight: int) -> None
    def update(timestep: float) -> None
    def applyMotorCommand(cmd: MotorCommand, currentTime: float) -> ActionResult
    def getSensoryPercept() -> SensoryPercept
    def getAgentBody() -> AgentBody
    # ... many more methods
```

### Memory Systems

#### NeuralWorkingMemory
```python
class NeuralWorkingMemory:
    def __init__() -> NeuralWorkingMemory
    def initialize(brain: Brain) -> None
    def store(pattern: List[float], strength: float = 1.0) -> None
    def retrieve() -> List[float]
    def contains(neuron: NeuronId) -> bool
    def getNeuronActivation(neuron: NeuronId) -> float
    def update(dt: float) -> None
    def clear() -> None
    # ... many more methods
```

#### NeuralEpisodicMemory
```python
class NeuralEpisodicMemory:
    def __init__() -> NeuralEpisodicMemory
    def initialize(brain: Brain) -> None
    def encode(event: List[float], timestamp: float) -> None
    def retrieve(time_window: float) -> List[float]
    def replay() -> None
    def consolidate() -> None
    # ... many more methods
```

### Prediction and Cognition

#### PredictionSystem
```python
class PredictionSystem:
    def __init__() -> PredictionSystem
    def initialize(brain: Brain) -> None
    def update(input: List[float], dt: float) -> None
    def getPrediction() -> float
    def getPredictionError() -> float
    def getConfidence() -> float
    def reset() -> None
```

#### NeuralPlanner
```python
class NeuralPlanner:
    def __init__() -> NeuralPlanner
    def planAction(state: List[float]) -> Action
    def evaluateAction(action: Action, state: List[float]) -> float
    def getActionSequence() -> List[Action]
    def clearPlan() -> None
```

#### ConceptFormation
```python
class ConceptFormation:
    def __init__() -> ConceptFormation
    def learnConcept(pattern: List[float], name: str) -> None
    def getConcept(name: str) -> List[float]
    def getAllConcepts() -> Dict[str, List[float]]
    def applyConcept(name: str, pattern: List[float]) -> List[float]
```

### Neuromodulation

#### Dopamine
```python
class Dopamine:
    def __init__() -> Dopamine
    def initialize(brain: Brain) -> None
    def getLevel() -> float
    def update(reward: float, prediction_error: float, dt: float) -> None
    def getRewardSignal() -> float
    def getPredictionErrorSignal() -> float
    def reset() -> None
```

#### Curiosity
```python
class Curiosity:
    def __init__() -> Curiosity
    def initialize(brain: Brain) -> None
    def getLevel() -> float
    def update(novelty: float, prediction_error: float, dt: float) -> None
    def getExplorationDrive() -> float
    def setNoveltyWeight(weight: float) -> None
    def setPredictionErrorWeight(weight: float) -> None
    def reset() -> None
```

### Development and Plasticity

#### DevelopmentSystem
```python
class DevelopmentSystem:
    def __init__() -> DevelopmentSystem
    def initialize(brain: Brain) -> None
    def update(dt: float) -> None
    def getStage() -> DevelopmentalStage
    def setStage(stage: DevelopmentalStage) -> None
    def getAge() -> float
    def getPlasticityRate() -> float
    def getMaturationLevel() -> float
    def reset() -> None
```

#### STDP
```python
class STDP:
    def __init__() -> STDP
    def update(preSpike: bool, postSpike: bool, dt: float) -> None
    def getLearningRate() -> float
    def setLearningRate(rate: float) -> None
    def getLTPwindow() -> float
    def getLTDwindow() -> float
    def getMaxWeight() -> float
    def getMinWeight() -> float
```

### Advanced Utility Classes

#### NLMExperiment
```python
class NLMExperiment:
    def __init__(name: str, config_dict: Dict = None) -> None
    def run(max_steps: int) -> Dict
    def getResults() -> Dict
    def addMetric(name: str, value: float) -> None
    def saveResults(filepath: str) -> None
```

#### NLMVisualizer
```python
class NLMVisualizer:
    @staticmethod
    def plot_vision(percept: SensoryPercept, title: str = "Vision Input") -> None
    
    @staticmethod
    def plot_activity_over_time(brain: Brain, steps: int) -> None
    
    @staticmethod
    def save_to_video(world: SimpleWorld, filepath: str, fps: int = 10, duration: int = 5) -> None
```

### Convenience Functions

#### Factory Functions
```python
# Create default configuration
config = nlm.create_default_config()

# Create brain with configuration
brain = nlm.create_brain(config)

# Create simple world
world = nlm.create_simple_world()

# Create agent brain
agent = nlm.create_agent_brain(brain)

# Create demo environment
config, world, brain, agent = nlm.create_demo_environment()
```

#### High-Level Functions
```python
# Run episode
stats = nlm.run_learning_episode(agent, world, max_steps=100)

# Benchmark performance
benchmark_time = nlm.benchmark_performance(brain, 100)

# Validate configuration
errors = nlm.validate_configuration(config)

# Convert to/from NumPy
numpy_array = nlm.to_numpy(python_list)
python_list = nlm.from_numpy(numpy_array)
```

#### Expert Utilities
```python
# Create complete agent with configuration
config, world, brain, agent = nlm.create_complete_agent({
    "brain/neurons": 1000,
    "brain/regions": 10,
    "brain/development.enabled": True,
})

# Create reproducible simulation
config, world, brain, agent = nlm.create_reproducible_simulation(seed=42)

# Analyze neural activity
analysis = nlm.analyze_neural_activity(brain, steps=100)
```

## Research Workflows

### 1. Experiment Framework

```python
import pynlm as nlm

# Create experiment
experiment = nlm.NLMExperiment(
    name="curiosity_driven_learning",
    config_dict={
        "brain/neuromodulation.curiosity": True,
        "brain/neuromodulation.novelty": True,
        "brain/development.enabled": True,
    }
)

# Run experiment
metrics = experiment.run(max_steps=1000)

# Save results
experiment.save_results("experiment_results.json")
```

### 2. Comparative Experiments

```python
import pynlm as nlm
import numpy as np

def run_parameter_sweep(param_name, param_values, base_config):
    """Run parameter sweep experiment."""
    results = []
    
    for value in param_values:
        # Create experiment with modified parameter
        config = base_config.copy()
        config[f"brain/{param_name}"] = value
        
        experiment = nlm.NLMExperiment(
            name=f"{param_name}_{value}",
            config_dict=config
        )
        
        metrics = experiment.run(max_steps=500)
        metrics['parameter_value'] = value
        results.append(metrics)
    
    return results

# Example usage
base_config = {"brain/neurons": 500, "brain/regions": 5}
param_values = [0.1, 0.5, 1.0, 2.0]

results = run_parameter_sweep("novelty.weight", param_values, base_config)

# Analyze results
rewards = [r['total_reward'] for r in results]
print(f"Average reward: {np.mean(rewards):.2f} ± {np.std(rewards):.2f}")
```

### 3. Real-time Monitoring

```python
import pynlm as nlm
import matplotlib.pyplot as plt
import time

# Create simulation
config, world, brain, agent = nlm.create_complete_agent()

# Setup monitoring
fig, axes = plt.subplots(2, 2, figsize=(12, 10))
axes = axes.flatten()

# Monitoring loop
for step in range(100):
    # Update simulation
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    world.update(0.1)
    
    # Collect metrics
    stats = {
        'time': world.getSimulationTime(),
        'reward': agent.getNeuromodulationLevel(),
        'activity': brain.getAverageFiringRate(),
        'neurons': brain.getActiveNeuronCount(),
    }
    
    # Update plots (simplified)
    axes[0].clear()
    axes[0].plot(stats['time'], stats['reward'], 'r-')
    axes[0].set_title('Reward over Time')
    
    axes[1].clear()
    axes[1].plot(stats['time'], stats['activity'], 'g-')
    axes[1].set_title('Neural Activity over Time')
    
    # Save frame
    plt.savefig(f'frame_{step:04d}.png')
    
    time.sleep(0.01)

plt.close()
print("Real-time monitoring completed")
```

### 4. Model Analysis

```python
import pynlm as nlm
import numpy as np
import matplotlib.pyplot as plt

# Create and run simulation
config, world, brain, agent = nlm.create_complete_agent()

# Collect neural data over time
neural_data = []
action_data = []
reward_data = []

for step in range(500):
    # Record data
    neural_data.append({
        'step': step,
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'excitatory_inhibitory': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
    })
    
    # Run simulation
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.update(0.1)
    
    action_data.append({
        'step': step,
        'action': action.getType(),
        'parameters': action.getParameters(),
    })
    
    reward_data.append({
        'step': step,
        'reward': agent.getNeuromodulationLevel(),
    })

# Analyze neural patterns
neural_array = np.array([[d['active_neurons'], d['firing_rate']] 
                        for d in neural_data])

# Plot neural dynamics
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

ax1.scatter(neural_array[:, 0], neural_array[:, 1], alpha=0.5, s=1)
ax1.set_xlabel('Active Neurons')
ax1.set_ylabel('Firing Rate')
ax1.set_title('Neural Activity Dynamics')

# Plot action sequence
action_sequence = [d['action'] for d in action_data]
unique_actions, counts = np.unique(action_sequence, return_counts=True)

ax2.bar(range(len(unique_actions)), counts)
ax2.set_xlabel('Action Type')
ax2.set_ylabel('Frequency')
ax2.set_title('Action Sequence')
ax2.set_xticks(range(len(unique_actions)))
ax2.set_xticklabels([str(a) for a in unique_actions])

plt.tight_layout()
plt.savefig('neural_dynamics.png')
print("Neural dynamics analysis completed")
```

## Performance Considerations

### Memory Usage
- **Recommended**: 8GB RAM for basic usage
- **For Advanced**: 16GB+ RAM for complex experiments
- **Memory-Heavy**: Working memory and episodic memory can consume significant memory

### CPU Requirements
- **Basic Simulation**: Single core sufficient
- **Advanced Research**: Multi-core recommended
- **Real-time**: Optimize for target framerate

### Optimization Tips
1. **Use NumPy**: Leverage vectorized operations
2. **Batch Processing**: Run multiple episodes in batch
3. **Memory Management**: Monitor and optimize memory usage
4. **Configuration**: Optimize configuration parameters
5. **Hardware**: Use dedicated GPU for large simulations

## Troubleshooting

### Common Issues

#### Import Errors
```bash
# If import fails
try:
    import pynlm as nlm
    print("Success")
except ImportError as e:
    print(f"Import error: {e}")
    # Check if dependencies are installed
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])
```

#### Build Errors
```bash
# If build fails
# Check for required system packages
# On Ubuntu:
apt-get update
apt-get install build-essential cmake python3-dev

# On macOS:
brew install cmake

# On Windows:
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```

#### Runtime Errors
```python
# Handle runtime errors gracefully
try:
    # Your code here
    pass
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except nlm.ValueError as e:
    print(f"Value error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

#### NumPy Integration Issues
```bash
# If NumPy is not available
pip install numpy
```

#### Visualization Issues
```bash
# If matplotlib/OpenCV not available
pip install matplotlib
pip install opencv-python
```

### Debugging Tips
1. **Enable verbose logging**: Set appropriate configuration options
2. **Use print statements**: Add debug output for troubleshooting
3. **Check dependencies**: Ensure all required packages are installed
4. **Monitor memory**: Watch for memory leaks
5. **Profile performance**: Use profiling tools for optimization

## Future Enhancements

### Planned Features
1. **Web Integration**: Jupyter notebook support
2. **Cloud Computing**: Distributed computing support
3. **Machine Learning**: Deep learning integration
4. **Real-time**: Live simulation visualization
5. **Mobile**: Mobile device support

### Extension Points
1. **Plugin Architecture**: Extensible plugin system
2. **Custom Binding**: User-defined bindings
3. **Model Export**: Model conversion utilities
4. **Cross-language**: C#, Julia, R bindings

## License

This enhanced Python bindings are licensed under the MIT License. The original NLM project is also under the MIT License.

For more information about the NLM project, visit: https://github.com/nlm-project/nlm

## Version Information

- **Core Version**: NLM 0.1.0
- **Python Bindings**: Enhanced v2.0.0
- **Build System**: CMake 3.16+
- **Dependencies**: pybind11 2.11+, NumPy 1.20+

## Acknowledgments

The enhanced Python bindings were developed with contributions from:
- NLM Research Team
- Python Bindings Contributors
- Scientific Computing Community
- NumPy and pybind11 Developers

Special thanks to all researchers who provided feedback and suggestions for enhancement.