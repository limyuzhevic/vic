# pynlm - Python Bindings for NLM (Neural Learning Machine)

This is the high-level Python API for the NLM C++ neural simulation framework.

## Overview

pynlm provides a comprehensive Python interface to the NLM neural simulation system.
It offers both high-level convenience classes and advanced features for power users.

## Key Features

- **Easy-to-use API**: High-level classes for Brain, Experiment, Config, etc.
- **Advanced features**: Neuromodulation control, memory systems, development, prediction
- **Visualization tools**: Plotting, animation, and interactive visualizations
- **Analysis tools**: Statistics and data analysis
- **Debug tools**: Monitoring and debugging utilities
- **Extensibility**: Easy to add new features and commands
- **Examples**: Comprehensive examples for beginners and advanced users

## Quick Start

```python
import pynlm

# Create a brain
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

# Run simulation
brain.run(1000)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Installation

The package uses scikit-build-core for building and requires pybind11.

```bash
pip install .
```

Or build from source:

```bash
mkdir build
cd build
cmake ..
make -j4
pip install .
```

## Module Structure

- `pynlm.Brain`: Main brain simulation class
- `pynlm.Experiment`: Experiment management and control
- `pynlm.Config`: Configuration management
- `pynlm.VisualizationTools`: Visualization and plotting
- `pynlm.AnalysisTools`: Data analysis and statistics
- `pynlm.DebugTools`: Debug and monitoring tools
- `pynlm.AdvancedFeatures`: Advanced features for power users
- `pynlm.MemorySystem`: Memory system access
- `pynlm.NeuromodulationController`: Neuromodulation control
- `pynlm.DevelopmentSystem`: Development system control
- `pynlm.PredictionSystem`: Prediction system control
- `pynlm.CognitionSystem`: Cognition system control

## Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create a simple brain
brain = pynlm.Brain.createDefault(neuron_count=500)
brain.initialize()

# Run a short simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation completed!")
```

### Complete Agent Simulation

```python
import pynlm

# Setup
config = pynlm.Config.createDefault()
brain = pynlm.Brain.create(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Experiment Management

```python
import pynlm

# Create experiment components
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(pynlm.createSimpleWorld())

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent_brain.initialize(world)

# Create and run experiment
experiment = pynlm.createExperiment(brain, agent_brain, world)
experiment.configure(duration=1000, timestep=0.1)
experiment.start()

# Run for some steps
for i in range(100):
    experiment.step()

# Stop experiment
experiment.stop()

# Print results
print(experiment.getExperimentSummary())
```

### Visualization and Analysis

```python
import pynlm

# Create and run a simulation
brain = pynlm.Brain.createDefault(2000)
brain.initialize()
brain.run(500)

# Get brain metrics
metrics = pynlm.VisualizationTools.getMetrics(brain)
print("Brain metrics:", metrics)

# Plot results
pynlm.VisualizationTools.plotNeurons(brain, "neurons.png")
pynlm.VisualizationTools.plotActivityOverTime(brain, "activity.png")

# Generate analysis report
report = pynlm.AnalysisTools.generateReport(brain)
print(report)

# Create animation
pynlm.VisualizationTools.createAnimation(brain, "brain_activity.gif")
```

### Advanced Features

```python
import pynlm

# Enable advanced mode
pynlm.AdvancedFeatures.enableAdvancedMode()
pynlm.AdvancedFeatures.enableOptimization()

# Add custom callbacks
def step_callback(step):
    print(f"Step {step} completed")

pynlm.AdvancedFeatures.addStepCallback(step_callback)

# Create brain with high-performance config
brain = pynlm.Brain.createDefault(5000)
brain.initialize()

# Add step callback for monitoring
pynlm.AdvancedFeatures.addStepCallback(lambda step: print(f"Progress: {step}/5000"))

# Run simulation
brain.run(5000)

# Remove callback
pynlm.AdvancedFeatures.removeStepCallback(step_callback)
```

### Debug and Monitoring

```python
import pynlm

# Enable debug mode
pynlm.DebugTools.enableDebugMode()
pynlm.DebugTools.enableProfiling()
pynlm.DebugTools.enableMemoryTracking()

# Create and run simulation
brain = pynlm.Brain.createDefault(1000)
brain.initialize()
brain.run(200)

# Get debug information
print("Debug messages:", pynlm.DebugTools.getDebugMessages())
print("Memory usage:", pynlm.DebugTools.getMemoryUsage())
print("System info:", pynlm.DebugTools.getSystemInfo())

# Disable debugging
pynlm.DebugTools.disableDebugMode()
pynlm.DebugTools.disableProfiling()
pynlm.DebugTools.disableMemoryTracking()
```

### Memory System Access

```python
import pynlm

# Create brain and enable memory
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

# Access memory system
memory_system = brain.getMemorySystem()

# Working memory operations
memory_system.updateWorkingMemory([0.1, 0.2, 0.3, 0.4, 0.5])
working_memory = memory_system.getWorkingMemory()
print("Working memory:", working_memory)

# Episodic memory
memory_system.recordExperience("Learned to move forward", reward=1.0, prediction_error=0.1)

# Associative memory
memory_system.learnAssociation([0.1, 0.2, 0.3], [0.7, 0.8, 0.9])

# Memory analysis
trace = memory_system.getMemoryTrace()
print("Memory trace length:", len(trace))
```

### Neuromodulation Control

```python
import pynlm

# Create agent and enable neuromodulation
brain = pynlm.Brain.createDefault(500)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Access neuromodulation controller
neuromodulation = agent.getNeuromodulationController()

# Set neuromodulator levels
neuromodulation.setDopamineLevel(0.8)
neuromodulation.setCuriosityLevel(0.6)
neuromodulation.setNoveltyLevel(0.4)

# Apply reward modulation
neuromodulation.setReward(1.0)

# Get neuromodulation levels
print(f"Dopamine: {neuromodulation.getCurrentDopamineLevel()}")
print(f"Curiosity: {neuromodulation.getCurrentCuriosityLevel()}")
print(f"Novelty: {neuromodulation.getCurrentNoveltyLevel()}")
print(f"Reward: {neuromodulation.getCurrentReward()}")
```

### Development System Control

```python
import pynlm

# Create brain and enable development
brain = pynlm.Brain.createDefault(800)
brain.initialize()

# Access development system
development = brain.getDevelopmentSystem()

# Control development stages
print("Current developmental stage:", development.getCurrentStage())
development.enableStructuralPlasticity(True)

# Set development rates
print(f"Plasticity rate: {development.getPlasticityRate()}")
print(f"Maturation rate: {development.getMaturationRate()}")
print(f"Development progress: {development.getDevelopmentProgress()}")

# Get developmental trajectory
trajectory = development.getDevelopmentTrajectory()
print(f"Development trajectory length: {len(trajectory)}")
```

### Prediction System

```python
import pynlm

# Create brain and enable prediction
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

# Access prediction system
prediction = brain.getPredictionSystem()

# Control prediction
prediction.enablePrediction(True)

# Set prediction parameters
prediction.setPredictionHorizon(5.0)
prediction.setPredictionConfidence(0.9)

# Get prediction statistics
print(f"Prediction accuracy: {prediction.getPredictionAccuracy()}")
print(f"Prediction error: {prediction.getPredictionError()}")
print(f"Prediction confidence: {prediction.getPredictionConfidence()}")

# Get prediction traces
trace = prediction.getPredictionTrace()
error_history = prediction.getPredictionErrorHistory()
```

### Cognition System

```python
import pynlm

# Create brain and enable cognition
brain = pynlm.Brain.createDefault(1200)
brain.initialize()

# Access cognition system
cognition = brain.getCognitionSystem()

# Enable cognitive functions
cognition.enablePlanning(True)
cognition.enableConceptFormation(True)
cognition.enableAttention(True)

# Get cognition statistics
print(f"Planning success rate: {cognition.getPlanningSuccessRate()}")
print(f"Concept stability: {cognition.getConceptStability()}")
print(f"Attention level: {cognition.getAttentionLevel()}")

# Get cognitive traces
planning_trace = cognition.getPlanningTrace()
concept_trace = cognition.getConceptTrace()
```

## Configuration Management

### Creating Configurations

```python
import pynlm

# Create default configuration
config = pynlm.Config.createDefault()

# Create high-performance configuration
config = pynlm.Config.createHighPerformance()

# Create minimal configuration
config = pynlm.Config.createMinimal()

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")
```

### Configuration Properties

```python
import pynlm

config = pynlm.Config.createDefault()

# Get configuration values
print(config.get("brain.neuronCount"))
print(config.getInt("brain.neuronCount"))
print(config.getFloat("brain.connectivity"))
print(config.getBool("brain.plasticity.enabled"))

# Set configuration values
config.set("brain.neuronCount", "2000")
config.set("brain.connectivity", "0.5")
config.set("brain.plasticity.enabled", "true")

# Check if configuration exists
print(config.has("brain.neuronCount"))
print(config.has("nonexistent.key"))

# Get all configuration keys
keys = config.getAllKeys()
print("Configuration keys:", keys)

# Convert to dictionary
config_dict = config.toDict()
print("Configuration as dict:", config_dict)
```

## Experiment Management

### Creating and Running Experiments

```python
import pynlm

# Create experiment components
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Create experiment
experiment = pynlm.createExperiment(brain, agent, world)
experiment.setGoal("Learn to navigate the world efficiently")
experiment.setEvaluationMetric("reward")

# Run experiment
experiment.configure(duration=500, timestep=0.1)
experiment.start()

# Run for a while
for i in range(100):
    experiment.step()

# Stop experiment
experiment.stop()

# Print results
print(experiment.getExperimentSummary())

# Get experiment metrics
print(f"Performance: {experiment.getPerformance()}")
print(f"Reward: {experiment.getReward()}")
print(f"Accuracy: {experiment.getAccuracy()}")
```

### Experiment State Management

```python
import pynlm

# Create and run experiment
experiment = pynlm.createExperiment(brain, agent, world)
experiment.configure(duration=1000, timestep=0.1)

# Save experiment state
experiment.saveState("experiment_state.json")

# Run some steps
for i in range(100):
    experiment.step()

# Load experiment state
experiment.loadState("experiment_state.json")

# Reset experiment
experiment.reset()
```

## Visualization and Plotting

### Basic Visualization

```python
import pynlm

# Create and run simulation
brain = pynlm.Brain.createDefault(1500)
brain.initialize()
brain.run(300)

# Plot neuron activity
pynlm.VisualizationTools.plotNeurons(brain, "neurons.png")

# Plot connections
pynlm.VisualizationTools.plotConnections(brain, "connections.png")

# Plot activity over time
pynlm.VisualizationTools.plotActivityOverTime(brain, "activity.png")

# Plot memory contents
pynlm.VisualizationTools.plotMemory(brain, "memory.png")

# Plot neuromodulator levels
pynlm.VisualizationTools.plotNeuromodulation(brain, "neuromodulation.png")

# Plot developmental trajectory
pynlm.VisualizationTools.plotDevelopment(brain, "development.png")
```

### Interactive Visualizations

```python
import pynlm

# Create brain
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

# Create interactive HTML plot
pynlm.VisualizationTools.createInteractivePlot(brain, "interactive_brain.html")

# Create animation
pynlm.VisualizationTools.createAnimation(brain, "brain_animation.gif")
```

### Experiment Visualization

```python
import pynlm

# Create and run experiment
experiment = pynlm.createExperiment(brain, agent, world)
experiment.configure(duration=500, timestep=0.1)

# Plot experiment results
pynlm.VisualizationTools.plotExperimentResults(experiment, "experiment_results.png")
```

## Analysis Tools

### Statistical Analysis

```python
import pynlm

# Create and run simulation
brain = pynlm.Brain.createDefault(1200)
brain.initialize()
brain.run(400)

# Compute spiking statistics
spiking_stats = pynlm.AnalysisTools.computeSpikingStatistics(brain)
print("Spiking statistics:", spiking_stats)

# Compute network topology
network_stats = pynlm.AnalysisTools.computeNetworkTopology(brain)
print("Network topology:", network_stats)

# Compute memory statistics
memory_stats = pynlm.AnalysisTools.computeMemoryStatistics(brain)
print("Memory statistics:", memory_stats)

# Compute neuromodulation statistics
neuromodulation_stats = pynlm.AnalysisTools.computeNeuromodulationStatistics(brain)
print("Neuromodulation statistics:", neuromodulation_stats)

# Compute development statistics
development_stats = pynlm.AnalysisTools.computeDevelopmentStatistics(brain)
print("Development statistics:", development_stats)

# Compute prediction statistics
prediction_stats = pynlm.AnalysisTools.computePredictionStatistics(brain)
print("Prediction statistics:", prediction_stats)

# Compute cognition statistics
cognition_stats = pynlm.AnalysisTools.computeCognitionStatistics(brain)
print("Cognition statistics:", cognition_stats)
```

### Report Generation

```python
import pynlm

# Create and run simulation
brain = pynlm.Brain.createDefault(1000)
brain.initialize()
brain.run(200)

# Generate analysis report
report = pynlm.AnalysisTools.generateReport(brain)
print(report)
```

## Debug and Monitoring

### Enabling Debug Features

```python
import pynlm

# Enable all debug features
pynlm.DebugTools.enableDebugMode()
pynlm.DebugTools.enableProfiling()
pynlm.DebugTools.enableMemoryTracking()

# Set log level
pynlm.DebugTools.setLogLevel(2)  # 0=ERROR, 1=WARNING, 2=INFO, 3=DEBUG

# Log debug messages
pynlm.DebugTools.log("Starting simulation")
pynlm.DebugTools.log("Brain initialized")
pynlm.DebugTools.log("Simulation running")

# Create and run simulation
brain = pynlm.Brain.createDefault(800)
brain.initialize()
brain.run(100)

# Get debug information
print("Debug messages:", pynlm.DebugTools.getDebugMessages())
print("Memory usage:", pynlm.DebugTools.getMemoryUsage())
print("System info:", pynlm.DebugTools.getSystemInfo())

# Disable all debug features
pynlm.DebugTools.disableDebugMode()
pynlm.DebugTools.disableProfiling()
pynlm.DebugTools.disableMemoryTracking()
pynlm.DebugTools.clearDebugMessages()
```

## Advanced Features for Power Users

### Custom Callbacks and Hooks

```python
import pynlm

# Define step callback
def step_callback(step):
    print(f"Step {step} completed")

# Define event callback
def event_callback(event_name):
    print(f"Event: {event_name}")

# Add callbacks
pynlm.AdvancedFeatures.addStepCallback(step_callback)
pynlm.AdvancedFeatures.addEventCallback(event_callback)

# Create brain and run simulation
brain = pynlm.Brain.createDefault(500)
brain.initialize()
brain.run(10)  # Will trigger step callbacks

# Remove callbacks
pynlm.AdvancedFeatures.removeStepCallback(step_callback)
pynlm.AdvancedFeatures.removeEventCallback(event_callback)
```

### Performance Optimization

```python
import pynlm

# Enable optimization
pynlm.AdvancedFeatures.enableOptimization()

# Create high-performance brain
brain = pynlm.Brain.createDefault(5000)
brain.initialize()

# Run simulation
brain.run(1000)

# Check if optimization is enabled
print("Optimization enabled:", pynlm.AdvancedFeatures.isOptimizationEnabled())

# Disable optimization
pynlm.AdvancedFeatures.disableOptimization()
```

### Scripting and Automation

```python
import pynlm

# Enable advanced mode
pynlm.AdvancedFeatures.enableAdvancedMode()

# Schedule tasks
def task1():
    print("Task 1 executed")

def task2():
    print("Task 2 executed")

pynlm.AdvancedFeatures.scheduleTask(task1, 1.0)  # Execute after 1 second
pynlm.AdvancedFeatures.scheduleTask(task2, 2.0)  # Execute after 2 seconds

# Create brain and run simulation
brain = pynlm.Brain.createDefault(300)
brain.initialize()

# Execute scripts
def example_script():
    brain.run(50)
    brain.save("example_brain_state.json")

pynlm.AdvancedFeatures.executeScript(example_script)

# Cancel tasks
pynlm.AdvancedFeatures.cancelTask(task1)
```

## Memory System Examples

### Working Memory

```python
import pynlm

brain = pynlm.Brain.createDefault(600)
brain.initialize()

memory_system = brain.getMemorySystem()

# Working memory operations
memory_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
memory_system.updateWorkingMemory(memory_data)

retrieved_memory = memory_system.getWorkingMemory()
print(f"Working memory size: {len(retrieved_memory)}")
print(f"Working memory sum: {sum(retrieved_memory)}")
```

### Episodic Memory

```python
import pynlm

brain = pynlm.Brain.createDefault(400)
brain.initialize()

memory_system = brain.getMemorySystem()

# Episodic memory operations
memory_system.recordExperience("First experience: moved forward", reward=1.0, prediction_error=0.1)
memory_system.recordExperience("Second experience: turned left", reward=0.5, prediction_error=0.2)
memory_system.recordExperience("Third experience: explored new area", reward=0.8, prediction_error=0.3)

# Search memory
query_memory = [0.1, 0.2, 0.3]
search_result = memory_system.searchMemory(query_memory)
print(f"Memory search result: {search_result}")
```

### Associative Memory

```python
import pynlm

brain = pynlm.Brain.createDefault(700)
brain.initialize()

memory_system = brain.getMemorySystem()

# Associative memory learning
input_patterns = [
    [0.1, 0.2, 0.3],
    [0.4, 0.5, 0.6],
    [0.7, 0.8, 0.9]
]

output_patterns = [
    [0.7, 0.8, 0.9],
    [0.4, 0.5, 0.6],
    [0.1, 0.2, 0.3]
]

for input_pattern, output_pattern in zip(input_patterns, output_patterns):
    memory_system.learnAssociation(input_pattern, output_pattern)

# Memory consolidation
memory_system.consolidateMemory()
```

## Neuromodulation Examples

### Reward-Based Learning

```python
import pynlm

brain = pynlm.Brain.createDefault(500)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)

neuromodulation = agent.getNeuromodulationController()

# Simulate reward-based learning
for step in range(50):
    # Simulate reward
    reward = 1.0 if step % 10 == 0 else 0.5
    neuromodulation.setReward(reward)
    
    # Apply neuromodulation
    neuromodulation.setDopamineLevel(reward)
    neuromodulation.setCuriosityLevel(0.5)
    neuromodulation.setNoveltyLevel(0.3)
    
    print(f"Step {step}: Reward={reward}, Dopamine={neuromodulation.getCurrentDopamineLevel()}")
```

### Prediction Error Learning

```python
import pynlm

brain = pynlm.Brain.createDefault(400)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)

neuromodulation = agent.getNeuromodulationController()

# Simulate prediction error learning
for step in range(50):
    # Simulate prediction error
    prediction_error = 0.1 if step % 5 == 0 else 0.01
    neuromodulation.setPredictionError(prediction_error)
    neuromodulation.setNoveltyLevel(prediction_error)
    
    print(f"Step {step}: Prediction Error={prediction_error}, Novelty={neuromodulation.getCurrentNoveltyLevel()}")
```

## Development System Examples

### Developmental Stages

```python
import pynlm

brain = pynlm.Brain.createDefault(800)
brain.initialize()

# Access development system
development = brain.getDevelopmentSystem()

# Control developmental stages
print(f"Initial developmental stage: {development.getCurrentStage()}")

development.enableStructuralPlasticity(True)

# Set development rates
print(f"Plasticity rate: {development.getPlasticityRate()}")
print(f"Maturation rate: {development.getMaturationRate()}")
print(f"Development progress: {development.getDevelopmentProgress()}")

# Get developmental trajectory
trajectory = development.getDevelopmentTrajectory()
print(f"Development trajectory: {trajectory}")
```

### Critical Periods

```python
import pynlm

brain = pynlm.Brain.createDefault(600)
brain.initialize()

# Access development system
development = brain.getDevelopmentSystem()

# Control critical periods
print(f"Is in critical period: {development.isInCriticalPeriod()}")

# Enter critical period
# development.enterCriticalPeriod()

# Exit critical period
# development.exitCriticalPeriod()
```

## Prediction System Examples

### Predictive Learning

```python
import pynlm

brain = pynlm.Brain.createDefault(700)
brain.initialize()

# Access prediction system
prediction = brain.getPredictionSystem()

# Enable prediction
prediction.enablePrediction(True)

# Set prediction parameters
prediction.setPredictionHorizon(5.0)
prediction.setPredictionConfidence(0.9)

# Get prediction statistics
print(f"Prediction accuracy: {prediction.getPredictionAccuracy()}")
print(f"Prediction error: {prediction.getPredictionError()}")
print(f"Prediction confidence: {prediction.getPredictionConfidence()}")

# Record prediction errors
for i in range(10):
    prediction_error = 0.1 * (i + 1) / 10
    prediction.recordPredictionError(prediction_error)

# Get error history
error_history = prediction.getPredictionErrorHistory()
print(f"Prediction error history: {error_history}")
```

## Cognition System Examples

### Cognitive Functions

```python
import pynlm

brain = pynlm.Brain.createDefault(900)
brain.initialize()

# Access cognition system
cognition = brain.getCognitionSystem()

# Enable cognitive functions
cognition.enablePlanning(True)
cognition.enableConceptFormation(True)
cognition.enableAttention(True)

# Get cognition statistics
print(f"Planning success rate: {cognition.getPlanningSuccessRate()}")
print(f"Concept stability: {cognition.getConceptStability()}")
print(f"Attention level: {cognition.getAttentionLevel()}")

# Get cognitive traces
planning_trace = cognition.getPlanningTrace()
concept_trace = cognition.getConceptTrace()

print(f"Planning trace: {planning_trace}")
print(f"Concept trace: {concept_trace}")
```

## Configuration Examples

### Custom Brain Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.Config()

# Set configuration values
config.set("brain.neuronCount", "3000")
config.set("brain.connectivity", "0.05")
config.set("brain.region.count", "5")
config.set("brain.simulation.dt", "0.05")
config.set("brain.simulation.maxSteps", "10000")
config.set("brain.plasticity.enabled", "true")
config.set("brain.plasticity.stdp.enabled", "true")
config.set("brain.plasticity.hebbian.enabled", "true")
config.set("brain.development.enabled", "true")
config.set("brain.development.stages", "10")
config.set("brain.memory.working.enabled", "true")
config.set("brain.memory.working.capacity", "2000")
config.set("brain.memory.episodic.enabled", "true")
config.set("brain.memory.associative.enabled", "true")
config.set("brain.neuromodulation.enabled", "true")
config.set("brain.neuromodulation.dopamine.enabled", "true")
config.set("brain.neuromodulation.curiosity.enabled", "true")
config.set("brain.neuromodulation.novelty.enabled", "true")
config.set("brain.prediction.enabled", "true")
config.set("brain.prediction.horizon", "10")
config.set("brain.cognition.enabled", "true")
config.set("brain.cognition.planning.enabled", "true")
config.set("brain.cognition.concept.enabled", "true")

# Create brain with custom configuration
brain = pynlm.Brain.create(config)
brain.initialize()
```

### Loading and Saving Configuration

```python
import pynlm

# Create and configure brain
brain = pynlm.Brain.createDefault(2000)
brain.initialize()

# Get brain configuration
config = brain.getConfig()

# Save configuration to file
config.saveToFile("brain_config.json")

# Load configuration from file
new_config = pynlm.Config()
new_config.loadFromFile("brain_config.json")

# Create brain with loaded configuration
new_brain = pynlm.Brain.create(new_config)
new_brain.initialize()
```

## Advanced Configuration Examples

### Configuration Templates

```python
import pynlm

# Create configuration templates
templates = {
    "small": pynlm.Config.createMinimal(),
    "medium": pynlm.Config.createDefault(),
    "large": pynlm.Config.createHighPerformance()
}

# Use different configurations
for size, config in templates.items():
    print(f"{size} brain configuration:")
    print(f"  Neurons: {config.getInt('brain.neuronCount')}")
    print(f"  Connectivity: {config.getFloat('brain.connectivity')}")
    print(f"  Memory capacity: {config.getInt('brain.memory.working.capacity')}")
    print()
```

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.Config.createDefault()

# Validate configuration
def validate_config(config):
    errors = []
    
    # Check if neuron count is reasonable
    neuron_count = config.getInt("brain.neuronCount", 0)
    if neuron_count < 100 or neuron_count > 100000:
        errors.append("Neuron count must be between 100 and 100000")
    
    # Check if connectivity is valid
    connectivity = config.getFloat("brain.connectivity", 0.0)
    if connectivity < 0.0 or connectivity > 1.0:
        errors.append("Connectivity must be between 0.0 and 1.0")
    
    return errors

# Validate configuration
validation_errors = validate_config(config)
if validation_errors:
    print("Configuration validation errors:")
    for error in validation_errors:
        print(f"  - {error}")
else:
    print("Configuration is valid")
```

## Experiment Examples

### Experiment Templates

```python
import pynlm

# Create experiment templates
templates = {
    "short": pynlm.createExperiment(brain, agent, world),
    "medium": pynlm.createExperiment(brain, agent, world),
    "long": pynlm.createExperiment(brain, agent, world)
}

# Configure experiments
templates["short"].configure(duration=100, timestep=0.1)
templates["medium"].configure(duration=1000, timestep=0.1)
templates["long"].configure(duration=5000, timestep=0.05)

# Set experiment goals
templates["short"].setGoal("Quick test of brain functionality")
templates["medium"].setGoal("Learn basic navigation")
templates["long"].setGoal("Advanced learning and adaptation")

# Set evaluation metrics
templates["short"].setEvaluationMetric("reward")
templates["medium"].setEvaluationMetric("accuracy")
templates["long"].setEvaluationMetric("performance")
```

### Experiment Comparison

```python
import pynlm

# Create multiple experiments with different configurations
brain1 = pynlm.Brain.createDefault(1000)
brain2 = pynlm.Brain.createHighPerformance(2000)

agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

world1 = pynlm.createSimpleWorld()
world2 = pynlm.createSimpleWorld()

experiment1 = pynlm.createExperiment(brain1, agent1, world1)
experiment2 = pynlm.createExperiment(brain2, agent2, world2)

# Configure experiments
experiment1.configure(duration=500, timestep=0.1)
experiment2.configure(duration=500, timestep=0.1)

# Run experiments
experiment1.start()
experiment2.start()

for i in range(50):
    experiment1.step()
    experiment2.step()

experiment1.stop()
experiment2.stop()

# Compare results
print("Experiment 1 results:")
print(experiment1.getExperimentSummary())
print()
print("Experiment 2 results:")
print(experiment2.getExperimentSummary())
```

## Multi-Agent Examples

### Parallel Agents

```python
import pynlm

# Create multiple brains
brains = [pynlm.Brain.createDefault(500) for _ in range(3)]
for brain in brains:
    brain.initialize()

# Create agents
agents = [pynlm.createAgentBrain(brain) for brain in brains]

# Create worlds
worlds = [pynlm.createSimpleWorld() for _ in range(3)]
for i, world in enumerate(worlds):
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()

# Initialize agents
for agent, world in zip(agents, worlds):
    agent.initialize(world)

# Run parallel agents
for step in range(100):
    for agent, world in zip(agents, worlds):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print("Parallel agents simulation complete!")
```

## Checkpoint and Recovery Examples

### Checkpoint Management

```python
import pynlm

# Create and run brain
brain = pynlm.Brain.createDefault(1000)
brain.initialize()

# Create checkpoints
checkpoints = []
for step in range(0, 1000, 100):
    brain.run(step)
    brain.save(f"checkpoint_{step}.json")
    checkpoints.append(f"checkpoint_{step}.json")

# Restore from checkpoint
brain.reset()
brain.load("checkpoint_500.json")

# Clean up checkpoints
import os
for checkpoint in checkpoints:
    if os.path.exists(checkpoint):
        os.remove(checkpoint)
```

### Experiment Recovery

```python
import pynlm

# Create and run experiment
brain = pynlm.Brain.createDefault(800)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

experiment = pynlm.createExperiment(brain, agent, world)
experiment.configure(duration=500, timestep=0.1)

# Save experiment state
experiment.saveState("experiment_state.json")

# Run some steps
for i in range(50):
    experiment.step()

# Restore experiment state
experiment.loadState("experiment_state.json")

# Continue running
for i in range(50):
    experiment.step()
```

## Migration Examples

### From Old API to New API

```python
# Old API (original bindings)
import pynlm as old_api

brain_old = old_api.createBrain(old_api.createDefaultConfig())
brain_old.initialize()
brain_old.run(100)

# New API (high-level bindings)
import pynlm as new_api

brain_new = new_api.Brain.createDefault(1000)
brain_new.initialize()
brain_new.run(100)

# Compare results
print(f"Old API neurons: {brain_old.getTotalNeuronCount()}")
print(f"New API neurons: {brain_new.getTotalNeuronCount()}")
print(f"Old API spikes: {brain_old.getTotalSpikeCount()}")
print(f"New API spikes: {brain_new.getTotalSpikeCount()}")
```

## Error Handling Examples

### Graceful Error Handling

```python
import pynlm

# Create brain
brain = pynlm.Brain.createDefault(1000)

# Initialize brain
try:
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Error initializing brain: {e}")

# Run simulation
try:
    brain.run(100)
    print("Simulation completed successfully")
except Exception as e:
    print(f"Error running simulation: {e}")

# Load brain state
try:
    brain.load("nonexistent_brain.json")
    print("Brain state loaded successfully")
except Exception as e:
    print(f"Error loading brain state: {e}")
```

### Configuration Error Handling

```python
import pynlm

# Create configuration
config = pynlm.Config.createDefault()

# Set configuration values
try:
    config.set("brain.neuronCount", "1000")
    print("Neuron count set successfully")
except Exception as e:
    print(f"Error setting neuron count: {e}")

# Get configuration values
try:
    neuron_count = config.getInt("brain.neuronCount", -1)
    print(f"Neuron count: {neuron_count}")
except Exception as e:
    print(f"Error getting neuron count: {e}")

# Load configuration from file
try:
    config.loadFromFile("nonexistent_config.json")
    print("Configuration loaded successfully")
except Exception as e:
    print(f"Error loading configuration: {e}")
```

## Performance Optimization Examples

### Optimized Brain Creation

```python
import pynlm

# Enable optimization
pynlm.AdvancedFeatures.enableOptimization()

# Create high-performance brain
brain = pynlm.Brain.createHighPerformance(5000)
brain.initialize()

# Run simulation
brain.run(1000)

# Check optimization status
print("Optimization enabled:", pynlm.AdvancedFeatures.isOptimizationEnabled())

# Disable optimization
pynlm.AdvancedFeatures.disableOptimization()
```

### Efficient Memory Usage

```python
import pynlm

# Enable memory tracking
pynlm.DebugTools.enableMemoryTracking()

# Create and run brain
brain = pynlm.Brain.createDefault(2000)
brain.initialize()
brain.run(200)

# Get memory usage
memory_usage = pynlm.DebugTools.getMemoryUsage()
print(f"Memory usage: {memory_usage} bytes")

# Disable memory tracking
pynlm.DebugTools.disableMemoryTracking()
```

## Examples Summary

This file contains comprehensive examples for:

1. **Basic Usage**: Simple brain simulations and configurations
2. **Complete Applications**: Full agent simulations with worlds
3. **Visualization**: Plotting and interactive visualizations
4. **Analysis**: Statistical analysis and report generation
5. **Debugging**: Debug and monitoring tools
6. **Advanced Features**: Custom callbacks, optimization, and scripting
7. **Memory Systems**: Working memory, episodic memory, and associative memory
8. **Neuromodulation**: Reward-based learning and prediction error
9. **Development**: Developmental stages and plasticity
10. **Prediction**: Predictive learning and confidence
11. **Cognition**: Planning, concept formation, and attention
12. **Experiment Management**: Experiment setup, execution, and analysis
13. **Multi-Agent**: Parallel agent simulations
14. **Checkpointing**: State saving and recovery
15. **Error Handling**: Graceful error handling and validation
16. **Performance**: Optimization and efficient memory usage
17. **Migration**: Migration from old to new API
18. **Configuration**: Configuration management and validation

Each example demonstrates a specific feature or concept, providing a practical introduction to the pynlm library.

## Getting Help

For more detailed information or help with specific examples:

1. Check the official documentation at: https://nlm.readthedocs.io
2. Visit the NLM project website: https://github.com/nlm-project/nlm
3. Join the NLM community for discussions and support
4. Report issues and feature requests at the GitHub repository

## License

This software is part of the NLM (Neural Learning Machine) project.
For licensing information, see the LICENSE file.

## Copyright

Copyright (c) 2026 NLM Authors

---

This comprehensive documentation provides an extensive overview of the pynlm library's capabilities, usage patterns, and examples for both beginners and advanced users.
