# Advanced NLM (Neural Learning Machine) Documentation

## Overview

This document provides comprehensive documentation for the advanced features and improvements made to the NLM (Neural Learning Machine) project. The improvements focus on enhancing the Python bindings, advanced AgentBrain features, debugging and monitoring tools, development system enhancements, and advanced simulation presets.

## Table of Contents

1. [Introduction](#introduction)
2. [Advanced Python Bindings](#advanced-python-bindings)
3. [Enhanced AgentBrain Features](#enhanced-agentbrain-features)
4. [Debugging and Monitoring Tools](#debugging-and-monitoring-tools)
5. [Enhanced Development System](#enhanced-development-system)
6. [Advanced Simulation Presets](#advanced-simulation-presets)
7. [Examples and Tutorials](#examples-and-tutorials)
8. [API Reference](#api-reference)
9. [Best Practices](#best-practices)
10. [Troubleshooting](#troubleshooting)

## Introduction

The NLM project has been enhanced with advanced features to provide power users with comprehensive control over neural network simulations. These improvements address the user's request for "more commands" and "advanced stuff for more advanced users" while maintaining backward compatibility with existing functionality.

## Advanced Python Bindings

The Python bindings have been extended with advanced commands that provide comprehensive access to NLM's capabilities.

### Core Advanced Commands

#### Direct Neuron Manipulation
```python
import pynlm

# Inject current into a specific neuron by ID
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Find a neuron ID (this would be obtained from the brain)
neuron_id = pynlm.INVALID_NEURON_ID  # Would be set by brain initialization

# Inject current into specific neuron
pynlm.injectCurrentToNeuron(brain, neuron_id, 50.0)
```

#### Synapse Engineering
```python
# Modify synapse weight directly
synapse_id = pynlm.INVALID_SYNAPSE_ID  # Would be obtained from the brain
new_weight = 0.75
pynlm.modifySynapseWeight(brain, synapse_id, new_weight)
```

#### Network Analysis
```python
# Get comprehensive brain network statistics
stats = pynlm.getBrainNetworkStats(brain)
print(f"Firing rate ratio: {stats[0]:.3f}")
print(f"Active neuron ratio: {stats[1]:.3f}")
print(f"Average firing rate: {stats[2]:.3f}")
print(f"Total spikes: {stats[3]}")
print(f"Total synapses: {stats[4]}")
print(f"E/I ratio: {stats[5]:.3f}")
```

#### Advanced Simulation Presets
```python
import pynlm

config = pynlm.createDefaultConfig()

# Apply learning preset (high learning with exploration)
pynlm.setSimulationPreset(config, "high_learning")

# Apply performance preset (fast execution with basic learning)
pynlm.setSimulationPreset(config, "fast_performance")

# Apply balanced preset
pynlm.setSimulationPreset(config, "balanced")
```

#### Batch Processing
```python
# Run batch simulation across multiple episodes
rewards, episode_lengths = pynlm.runBatchSimulation(
    brain, world, agent,
    num_episodes=10,
    steps_per_episode=1000
)

print(f"Average reward: {sum(rewards)/len(rewards):.3f}")
print(f"Average episode length: {sum(episode_lengths)/len(episode_lengths):.1f}")
```

#### Advanced Agent Configuration
```python
# Configure agent with advanced learning settings
pynlm.configureAdvancedAgent(
    agent,
    enableAdvancedLearning=True,
    learningRate=1.5,  # Higher learning rate
    adaptiveExploration=True
)
```

#### Development Stage Control
```python
# Set specific development stage
pynlm.setDevelopmentStage(brain, "maturation")
pynlm.setDevelopmentStage(brain, "critical")
pynlm.setDevelopmentStage(brain, "adult")
```

#### Experimental Utilities
```python
# Run ablation test
ablation_results = pynlm.runAblationTest(brain, ["plasticity", "development", "neuromodulation"])
print(f"Plasticity disabled: {ablation_results['plasticity']}")
print(f"Development disabled: {ablation_results['development']}")
print(f"Neuromodulation disabled: {ablation_results['neuromodulation']}")
```

#### Debug and Monitoring
```python
# Enable debug logging
pynlm.enableDebugLogging(brain)

# Get system resources
resources = pynlm.getSystemResources()
print(f"System: {resources[0]}")
print(f"Bindings: {resources[1]}")
print(f"Architecture: {resources[2]}")
```

#### Data Export/Import
```python
# Export brain state to file
pynlm.exportBrainToFile(brain, "my_brain_state.bin")

# Import brain state from file
if pynlm.importBrainFromFile(brain, "saved_brain.bin"):
    print("Brain state imported successfully")
else:
    print("Failed to import brain state")
```

#### Progress Tracking
```python
# Create progress tracker for long-running simulations
progress_tracker = pynlm.createProgressTracker(10000)

# Use in simulation loop
for step in range(10000):
    # ... simulation code ...
    progress_tracker.increment()
    
    # Periodically display progress
    if step % 1000 == 0:
        print(f"Progress: {progress_tracker.getProgress() * 100:.1f}%")
```

### Advanced Configuration Methods

#### Category-based Configuration
```python
# Set advanced configuration with category prefix
config.setAdvanced("plasticity.stdp", "learning_rate", "0.01")
config.setAdvanced("neuromod.curiosity", "enable", "true")

# Get advanced configuration value
learning_rate = config.getAdvanced("plasticity.stdp", "learning_rate")
print(f"STDP learning rate: {learning_rate}")
```

#### Simulation Profiles
```python
# Set simulation profile
config.setSimulationProfile("research")  # 5000 neurons, 0.05 density
# or
config.setSimulationProfile("production")  # 1000 neurons, 0.1 density
```

## Enhanced AgentBrain Features

The AgentBrain class has been enhanced with advanced features for power users.

### Learning Strategy Control
```python
# Configure learning strategy
agent.setLearningStrategy(agent_brain.LearningStrategy.Aggressive)
# Other strategies: Conservative, Balanced, ConservativeBalanced

# Configure action selection strategy  
agent.setActionSelectionStrategy(agent_brain.ActionSelectionStrategy.Boltzmann)
# Other strategies: Deterministic, Random, Ucb, EpsGreedy
```

### Advanced Neuromodulation Control
```python
# Configure neuromodulation effects
agent.setDopamineEffect(1.5)
agent.setCuriosityEffect(2.0)
agent.setNoveltyEffect(0.8)
agent.setPredictionErrorEffect(1.2)
```

### Development System Enhancement
```python
# Configure development speed
agent.setDevelopmentSpeed(2.0)  # 2x normal development

# Set custom developmental stages
agent.addCustomDevelopmentalStage(120.0, agent_brain.DevelopmentalStage.CriticalPeriod)
agent.addCustomDevelopmentalStage(240.0, agent_brain.DevelopmentalStage.Maturation)
```

### Memory System Integration
```python
# Enable memory access
agent.enableWorkingMemoryAccess(True)
agent.enableEpisodicMemoryAccess(True)
agent.enableSemanticMemoryAccess(True)
```

### Performance Monitoring
```python
# Get performance statistics
stats = agent.getPerformanceStats()
print(f"Steps executed: {stats.stepsExecuted}")
print(f"Average firing rate: {stats.avgFiringRate:.3f}")
print(f"Total reward: {stats.totalReward:.3f}")
print(f"Memory usage: {stats.memoryUsage} bytes")
```

### Advanced Debugging and Monitoring
```python
# Configure debug level
agent.setDebugLevel(agent_brain.DebugLevel.Detailed)
# Levels: Basic, Intermediate, Advanced, Detailed, Diagnostic

# Enable profiling
agent.enableProfiling(True)

# Record profile data
agent.recordProfileData("step_time", 0.1)
agent.recordProfileData("memory_usage", 512)
```

### Advanced Sensory Processing
```python
# Enable sensory filtering
agent.enableSensoryFiltering(True)
agent.setVisionFilteringStrength(0.7)
agent.setTouchFilteringStrength(0.5)
```

### Custom Action Mappings
```python
# Add custom action mappings
agent.addCustomActionMapping(agent_brain.ActionType.MoveForward, "walk_forward")
agent.addCustomActionMapping(agent_brain.ActionType.TurnLeft, "rotate_left_90")

# Get action mapping name
action_name = agent.getActionMappingName(agent_brain.ActionType.MoveForward)
print(f"Action: {action_name}")
```

### Reward Shaping
```python
# Set custom reward function
def custom_reward(percept, body):
    # Custom reward calculation based on perception and body state
    vision = percept.getVision()
    if len(vision) > 0 and max(vision) > 0.5:  # Good vision
        return 1.0
    elif len(vision) > 0 and max(vision) < 0.1:  # Poor vision
        return -0.5
    return 0.0

agent.setRewardFunction(custom_reward)
```

### Episode Management
```python
# Configure episode limits
agent.setEpisodeLimit(1000)
agent.setStepLimit(10000)
agent.setTimeLimit(300.0)  # 5 minutes

# Episode tracking
print(f"Current episode: {agent.getCurrentEpisode()}")
print(f"Steps in current episode: {agent.getStepsInCurrentEpisode()}")
print(f"Episode time: {agent.getCurrentEpisodeTime():.3f}s")
```

### External Event Handling
```python
# Set external event callback
def handle_external_event(event):
    print(f"External event received: {event}")
    # Handle external events

agent.setExternalEventCallback(handle_external_event)

# Trigger external event
agent.triggerExternalEvent("user_action", {"action": "move", "target": "goal"})
```

## Debugging and Monitoring Tools

### DebugLogger

The DebugLogger provides comprehensive logging capabilities:

#### Basic Logging
```python
from nlm.performance import DebugLogger

# Set log level
DebugLogger.setLogLevel(DebugLogger.LogLevel.Debug)

# Enable file output
DebugLogger.enableFileOutput("nlm_debug.log")

# Log messages
DebugLogger.logDebug("Starting simulation")
DebugLogger.logInfo("Simulation initialized")
DebugLogger.logWarning("Low memory usage")
DebugLogger.logError("Failed to load configuration")
DebugLogger.logCritical("Simulation crashed")
```

#### Log History and Search
```python
# Get recent logs
recent_logs = DebugLogger.getRecentLogs(10)
print(recent_logs)

# Search logs by pattern
search_results = DebugLogger.searchLogs("error", DebugLogger.LogLevel.Error)
for entry in search_results:
    print(entry.toString())

# Clear log history
DebugLogger.clearHistory()
```

#### Performance Logging
```python
# Enable performance logging
DebugLogger.enablePerformanceLogging(True)

# Log simulation state
DebugLogger.setSimulationState(1000, 50.0)  # step 1000, time 50.0s

# Log performance metrics
DebugLogger.logInfo("Performance metrics collected", "PERFORMANCE")
```

### PerformanceMonitor

The PerformanceMonitor provides real-time performance tracking:

#### Basic Monitoring
```python
from nlm.performance import PerformanceMonitor

# Start monitoring
PerformanceMonitor.startMonitoring()

# Record performance metrics
PerformanceMonitor.recordMetrics(
    neurons=500,
    totalNeurons=1000,
    firingNeurons=150,
    totalSpikes=5000,
    avgFiringRate=10.0,
    simTime=500.0,
    steps=500,
    cpuUsage=45.5,
    memoryUsage=256.0
)
```

#### Progress Tracking
```python
# Start progress tracking
PerformanceMonitor.startProgress(10000, "Running neural network simulation")

# Update progress in simulation loop
for step in range(10000):
    # Run simulation
    step += 1
    
    # Update progress
    PerformanceMonitor.incrementProgress(1)
    
    # Display progress
    progress = PerformanceMonitor.getProgress()
    print(f"Progress: {progress.getProgressPercent():.1f}%")
    if progress.getProgressPercent() % 10 == 0:
        print(f"Estimated time remaining: {progress.getEstimatedTimeRemaining():.1f}s")
```

#### Benchmarking
```python
# Create benchmark
with PerformanceMonitor.createBenchmark("learning") as benchmark:
    # Run benchmark code
    for i in range(100):
        # ... benchmark code ...
        pass
    
    print(f"Benchmark duration: {benchmark.getElapsedTime():.3f}s")

# Get benchmark results
results = PerformanceMonitor.getBenchmarkResults()
for result in results:
    print(f"{result.name}: {result.duration:.3f}s (avg: {result.averageTimePerIteration * 1000:.1f}ms)")
```

#### Performance Reports
```python
# Generate performance report
report = PerformanceMonitor.generatePerformanceReport()
print(report)

# Generate benchmark report
benchmark_report = PerformanceMonitor.generateBenchmarkReport()
print(benchmark_report)
```

## Enhanced Development System

The EnhancedDevelopmentSystem provides advanced development control:

### Basic Development Control
```python
from nlm.development import EnhancedDevelopmentSystem

development = EnhancedDevelopmentSystem()

# Set current stage
development.setCurrentStage(EnhancedDevelopmentalStage.Maturation)

# Update development
development.update(brain, rng, dt)
```

### Custom Developmental Stages
```python
# Define custom developmental stage
stage = CustomDevelopmentalStage(
    EnhancedDevelopmentalStage.CustomStage,
    60.0,  # Start age
    120.0, # End age
    0.8f,  # Plasticity modifier
    0.9f,  # Learning rate
    0.3f   # Exploration rate
)

development.addCustomStage(stage)
```

### Epigenetic Factors
```python
# Add epigenetic factor
stress = EpigeneticFactor("stress", 1.0f, 0.01f, 0.002f)
curiosity = EpigeneticFactor("curiosity", 0.8f, 0.004f, 0.001f)

development.addEpigeneticFactor(stress)
development.addEpigeneticFactor(curiosity)

# Update epigenetic factors
development.updateEpigeneticFactors(environmentalStability, socialComplexity)

# Get epigenetic factor value
stress_value = development.getEpigeneticFactor("stress")
print(f"Stress level: {stress_value:.3f}")
```

### Developmental Events
```python
# Define developmental event
event = DevelopmentalEvent(
    180.0,  # Trigger age
    DevelopmentalTrigger.ExperienceBased,
    EnhancedDevelopmentalStage.Adult,
    "Maturation complete"
)

development.addDevelopmentalEvent(event)
```

### External Influences
```python
# Define external influence
influence = ExternalInfluence(
    "environment",  # Source
    DevelopmentalInfluence.Positive,  # Type
    0.5f,  # Strength
    {"plasticity": 0.8f, "learning": 1.2f},  # Factors
    60.0  # Duration
)

development.addExternalInfluence(influence)
```

### Performance Profiling
```python
# Get development profile
profile = development.getProfile()
print(f"Current age: {profile.currentAge}s")
print(f"Current stage: {profile.currentStage}")
print(f"Development score: {profile.getDevelopmentScore():.3f}")

# Get development report
report = development.getDevelopmentReport()
print(report)
```

## Advanced Simulation Presets

The AdvancedSimulationPresets provide predefined simulation configurations:

### Preset Usage
```python
from nlm.experiments import SimulationPresetManager, AdvancedSimulationController

# Apply preset
config = pynlm.createDefaultConfig()
SimulationPresetManager.applyPreset(config, SimulationPreset.LearningConverge)

# Or use advanced simulation controller
AdvancedSimulationController.setSimulationMode(SimulationMode.HighPrecision)
```

### Batch Simulation
```python
# Configure batch simulation
batch_config = BatchSimulationConfig()
batch_config.batchName = "learning_comparison"
batch_config.numSimulations = 5
batch_config.presets = [SimulationPreset.LearningConverge, SimulationPreset.ExplorationCuriosity]

# Start batch simulation
AdvancedSimulationController.startBatchSimulation(batch_config)
```

### Performance Presets
```python
# Configure performance preset
perf_preset = PerformancePreset(
    "fast_learning",
    "Optimized for fast learning with moderate memory usage",
    0.8,  # CPU priority
    0.5,  # Memory priority
    0.9   # Speed priority
)
```

### Simulation Templates
```python
# Configure simulation template
template = SimulationTemplate(
    "research_simulation",
    "Template for research simulations",
    SimulationPreset.LearningConverge
)
template.parameters["brain.neuron_count"] = "5000"
template.enabledFeatures = ["detailed_logging", "checkpointing"]

# Apply template
config = SimulationTemplateManager.applyTemplate("research_simulation")
```

## Examples and Tutorials

### Example 1: Advanced Agent Training
```python
import pynlm
from nlm.performance import DebugLogger, PerformanceMonitor

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure advanced agent
agent.setLearningStrategy(pynlm.LearningStrategy.Aggressive)
agent.setActionSelectionStrategy(pynlm.ActionSelectionStrategy.Boltzmann)
agent.enableCuriosity(True)

# Enable monitoring
DebugLogger.enableFileOutput("advanced_agent.log")
PerformanceMonitor.startMonitoring()

# Run simulation with progress tracking
for episode in range(100):
    world.reset()
    agent.reset()
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if world.getAgentBody().energy > 50 else -0.1
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Update progress
        PerformanceMonitor.incrementProgress(1)
    
    # Print episode statistics
    print(f"Episode {episode}: Steps {step+1}, Firing neurons: {brain.getFiringNeuronCount()}")

print("Simulation completed successfully!")
```

### Example 2: Network Analysis and Debugging
```python
import pynlm
from nlm.performance import DebugLogger

# Enable detailed logging
DebugLogger.setLogLevel(DebugLogger.LogLevel.Detailed)
DebugLogger.enableFileOutput("network_analysis.log")

# Create brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with network analysis
for step in range(5000):
    brain.step(step)
    
    # Log neuron statistics periodically
    if step % 500 == 0:
        firing_rate = brain.getAverageFiringRate()
        DebugLogger.logInfo(f"Step {step}: Firing rate = {firing_rate:.3f}", "NETWORK_ANALYSIS")
        
        # Get network statistics
        network_stats = pynlm.getBrainNetworkStats(brain)
        DebugLogger.logInfo(f"Network stats: FR={network_stats[0]:.3f}, Active={network_stats[1]:.3f}, Spikes={network_stats[3]}", 
                           "NETWORK_ANALYSIS")

# Export network analysis
DebugLogger.exportLogs("network_analysis_report.txt")
```

### Example 3: Performance Optimization
```python
import pynlm
from nlm.performance import PerformanceMonitor

# Configure performance monitoring
PerformanceMonitor.setPerformancePreset(PerformancePreset("balanced_performance", 
                                                           "Balanced performance optimization",
                                                           0.7,  # CPU priority
                                                           0.7,  # Memory priority
                                                           0.7)) # Speed priority

# Start performance monitoring
PerformanceMonitor.startMonitoring()

# Run performance-critical simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure for high performance
pynlm.setSimulationPreset(config, "fast_performance")

# Run simulation
for step in range(10000):
    brain.step(step)
    PerformanceMonitor.incrementProgress(1)

# Generate performance reports
perf_report = PerformanceMonitor.generatePerformanceReport()
benchmark_report = PerformanceMonitor.generateBenchmarkReport()

print("=== PERFORMANCE REPORT ===")
print(perf_report)
print("=== BENCHMARK REPORT ===")
print(benchmark_report)
```

### Example 4: Development System Exploration
```python
import pynlm
from nlm.development import EnhancedDevelopmentSystem

# Create enhanced development system
development = EnhancedDevelopmentSystem()

# Configure custom developmental stages
custom_stage1 = CustomDevelopmentalStage(
    EnhancedDevelopmentalStage.SensoryIntegration,
    0.0, 60.0, 1.0f, 1.0f, 0.1f
)

custom_stage2 = CustomDevelopmentalStage(
    EnhancedDevelopmentalStage.MotorCoordination,
    60.0, 180.0, 0.7f, 0.8f, 0.3f
)

development.addCustomStage(custom_stage1)
development.addCustomStage(custom_stage2)

# Configure epigenetic factors
stress_factor = EpigeneticFactor("stress", 1.0f, 0.01f, 0.002f)
curiosity_factor = EpigeneticFactor("curiosity", 0.8f, 0.004f, 0.001f)

development.addEpigeneticFactor(stress_factor)
development.addEpigeneticFactor(curiosity_factor)

# Create brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with enhanced development
for step in range(10000):
    development.update(brain, rng, dt)  # Update development
    brain.step(step)  # Run brain simulation
    
    # Log developmental state
    stage = development.getCurrentStage()
    print(f"Step {step}: Stage = {stage}, Age = {development.getProfile().currentAge:.1f}s")

# Get development analysis
analysis = development.getDevelopmentAnalysis()
print(analysis)
```

### Example 5: Advanced Simulation Presets
```python
import pynlm
from nlm.experiments import SimulationPresetManager, AdvancedSimulationController

# Create advanced configuration
advanced_config = AdvancedSimulationConfig()
advanced_config.mode = SimulationMode.HighPrecision
advanced_config.preset = SimulationPreset.LearningConverge
advanced_config.timeStep = 0.0005
advanced_config.maxSteps = 20000
advanced_config.enableProfiling = True
advanced_config.enableLogging = True

# Apply preset
config = pynlm.createDefaultConfig()
SimulationPresetManager.applyAdvancedPreset(config, SimulationPreset.LearningConverge, advanced_config)

# Create and start simulation
brain = pynlm.createBrain(config)
brain.initialize()

AdvancedSimulationController.startSimulation(brain, None, None)
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()`: Create default configuration
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file
- `set(key, value, source)`: Set configuration value
- `get(key)`: Get configuration value
- `has(key)`: Check if key exists

#### Brain
- `createBrain(config)`: Create brain with configuration
- `initialize()`: Initialize brain
- `step(step_number)`: Run simulation step
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

#### AgentBrain
- `createAgentBrain(brain)`: Create agent brain interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward modulation

#### SimpleWorld
- `createSimpleWorld()`: Create simple world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `update(timestep)`: Update world
- `getSensoryPercept()`: Get sensory percept

### Advanced Classes

#### DebugLogger
- `setLogLevel(level)`: Set log level
- `enableFileOutput(filepath, append)`: Enable file output
- `log(level, message, context)`: Log message
- `getRecentLogs(limit)`: Get recent logs
- `searchLogs(pattern, minLevel)`: Search logs

#### PerformanceMonitor
- `startMonitoring()`: Start monitoring
- `stopMonitoring()`: Stop monitoring
- `recordMetrics(...)`: Record performance metrics
- `getPerformanceStats()`: Get performance statistics
- `startProgress(totalSteps, description)`: Start progress tracking
- `createBenchmark(name)`: Create benchmark

#### EnhancedDevelopmentSystem
- `addCustomStage(stage)`: Add custom developmental stage
- `setCurrentStage(stage)`: Set current stage
- `update(brain, rng, dt)`: Update development
- `addEpigeneticFactor(factor)`: Add epigenetic factor
- `getDevelopmentReport()`: Get development report

#### SimulationPresetManager
- `applyPreset(config, preset)`: Apply preset
- `searchPresets(keyword)`: Search presets
- `getAllPresets()`: Get all presets
- `exportPreset(preset, filepath)`: Export preset

#### AdvancedSimulationController
- `setSimulationMode(mode)`: Set simulation mode
- `setAdvancedConfig(config)`: Set advanced configuration
- `startSimulation(brain, world, agent)`: Start simulation
- `getSimulationProgress()`: Get simulation progress
- `getCurrentStatus()`: Get current status

### Enums and Constants

#### LogLevel
- `Debug`: Debug messages
- `Info`: Information messages
- `Warning`: Warning messages
- `Error`: Error messages
- `Critical`: Critical messages

#### SimulationMode
- `Standard`: Standard simulation
- `HighPrecision`: High precision mode
- `RealTime`: Real-time mode
- `BatchProcessing`: Batch processing mode
- `Parallel`: Parallel processing mode
- `Streaming`: Streaming mode
- `EventDriven`: Event-driven mode
- `Adaptive`: Adaptive mode
- `Learning`: Learning-focused mode
- `Development`: Development-focused mode
- `Exploration`: Exploration-focused mode

#### SimulationPreset
- `LearningConverge`: Fast convergence with strong learning
- `LearningExplore`: Balanced learning and exploration
- `LearningStable`: Stable, conservative learning
- `ExplorationCuriosity`: High curiosity and exploration
- `ExplorationNovelty`: Novelty-driven exploration
- `ExplorationRandom`: Random exploration
- `DevelopmentRapid`: Rapid development
- `DevelopmentBalanced`: Balanced development
- `DevelopmentMature`: Mature, stable development
- `PerformanceFast`: Fast execution
- `PerformanceQuality`: High quality, thorough simulation
- `PerformanceBalanced`: Balanced performance

## Best Practices

### 1. Configuration Management

```python
# Use configuration presets for consistency
config = pynlm.createDefaultConfig()
pynlm.applyLearningPreset(config)  # or applyExplorationPreset, etc.

# Validate configuration before use
if SimulationPresetManager.validatePreset(SimulationPreset.LearningConverge):
    # Use configuration
    pass
```

### 2. Error Handling

```python
# Use try-catch blocks for external operations
try:
    brain.step(step)
except Exception as e:
    print(f"Error in simulation step {step}: {e}")
    # Handle error appropriately
```

### 3. Performance Monitoring

```python
# Start monitoring early
PerformanceMonitor.startMonitoring()

# Record performance metrics periodically
for step in range(total_steps):
    # Run simulation
    brain.step(step)
    
    # Record metrics
    PerformanceMonitor.recordMetrics(...)
    
    # Update progress
    PerformanceMonitor.incrementProgress(1)
```

### 4. Logging Strategy

```python
# Configure logging based on use case
if "production":
    DebugLogger.setLogLevel(DebugLogger.LogLevel.Info)
    DebugLogger.enableFileOutput("production.log")
elif "development":
    DebugLogger.setLogLevel(DebugLogger.LogLevel.Debug)
    DebugLogger.enableFileOutput("development.log")
elif "testing":
    DebugLogger.setLogLevel(DebugLogger.LogLevel.Warning)
```

### 5. Resource Management

```python
# Monitor resource usage
if PerformanceMonitor.getCurrentMemoryUsage() > 512:  # 512MB threshold
    print("Warning: High memory usage")

if PerformanceMonitor.getCurrentCPUUsage() > 80:  # 80% threshold
    print("Warning: High CPU usage")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Simulation Crashes
**Symptoms**: Simulation crashes with segmentation fault or runtime error
**Solutions**:
1. Check configuration for invalid parameters
2. Enable detailed logging: `DebugLogger.setLogLevel(DebugLogger.LogLevel.Detailed)`
3. Validate configuration before use: `SimulationPresetManager.validatePreset()`
4. Check memory limits: `PerformanceMonitor.getCurrentMemoryUsage()`

#### Issue: Slow Performance
**Symptoms**: Simulation runs slowly or stalls
**Solutions**:
1. Use performance preset: `pynlm.setSimulationPreset(config, "fast_performance")`
2. Enable performance monitoring: `PerformanceMonitor.startMonitoring()`
3. Check for performance bottlenecks: `PerformanceMonitor.generatePerformanceReport()`
4. Optimize configuration: `SimulationPresetManager.getPresetIssues(preset)`

#### Issue: Memory Issues
**Symptoms**: High memory usage or out-of-memory errors
**Solutions**:
1. Use memory-efficient preset: `pynlm.setSimulationPreset(config, "memory_efficient")`
2. Monitor memory usage: `PerformanceMonitor.getCurrentMemoryUsage()`
3. Configure memory limits: `advanced_config.memoryLimit = 512`
4. Enable checkpointing to reduce memory footprint

#### Issue: Development System Errors
**Symptoms**: Development stage transitions fail or unexpected behavior
**Solutions**:
1. Validate developmental stages: `development.getDevelopmentAnalysis()`
2. Check epigenetic factors: `development.getCurrentEpigeneticState()`
3. Enable detailed logging for development: `DebugLogger.setLogLevel(DebugLogger.LogLevel.Detailed)`
4. Validate external influences: `development.getProfile().environmentalStability`

#### Issue: Python Binding Errors
**Symptoms**: Python bindings fail to load or function incorrectly
**Solutions**:
1. Check installation: `import pynlm; print(pynlm.__version__)`
2. Validate Python environment: `sys.version`
3. Check for missing dependencies: `pip list | grep pynlm`
4. Restart Python interpreter if necessary

### Error Code Reference

#### Config Errors
- `-1`: Invalid configuration file
- `-2`: Missing required parameters
- `-3`: Invalid parameter values

#### Brain Errors
- `-10`: Brain not initialized
- `-11`: Invalid step number
- `-12`: Memory allocation failed

#### Simulation Errors
- `-20`: Simulation already running
- `-21`: Invalid simulation parameters
- `-22`: Simulation timeout

#### Performance Errors
- `-30`: Performance monitoring not started
- `-31`: Invalid performance metrics
- `-32`: Performance report generation failed

#### Development Errors
- `-40`: Invalid developmental stage
- `-41`: Developmental transition failed
- `-42`: Epigenetic factor error

### Support and Resources

#### Documentation
- Main documentation: `docs/ARCHITECTURE.md`
- Python API reference: `python/bindings.cpp`
- Configuration documentation: `configs/default.cfg`
- Performance documentation: `docs/PERFORMANCE.md`

#### Examples
- Basic examples: `examples/basic_usage.py`
- Advanced examples: `examples/advanced_usage.py`
- Performance examples: `examples/performance.py`
- Development examples: `examples/development.py`

#### Community Resources
- GitHub repository: `https://github.com/nlm-project/nlm`
- Documentation website: `https://nlm.readthedocs.io`
- Issue tracker: `https://github.com/nlm-project/nlm/issues`
- Discussion forum: `https://github.com/nlm-project/nlm/discussions`

### Getting Help

If you encounter issues or need assistance:

1. **Check the documentation**: Review the relevant sections of this documentation
2. **Search for solutions**: Look for similar issues in the issue tracker
3. **Report bugs**: File a bug report with detailed information
4. **Request features**: Suggest new features or enhancements
5. **Contribute**: Submit pull requests for bug fixes or improvements

## Conclusion

The NLM project has been significantly enhanced with advanced features that provide comprehensive control over neural network simulations. These improvements include:

1. **Advanced Python Bindings**: Extended with commands for direct neuron manipulation, synapse engineering, network analysis, and batch processing.

2. **Enhanced AgentBrain Features**: Added learning strategy control, advanced neuromodulation, development system enhancement, memory integration, and performance monitoring.

3. **Debugging and Monitoring Tools**: Provided DebugLogger for comprehensive logging and PerformanceMonitor for real-time performance tracking.

4. **Enhanced Development System**: Added custom developmental stages, epigenetic factors, developmental events, and external influences.

5. **Advanced Simulation Presets**: Created preset configurations for different use cases and simulation modes.

These improvements maintain backward compatibility while providing power users with the advanced tools they need for complex neural network simulations. The documentation and examples provided will help users get started with the advanced features and make the most of the NLM platform's capabilities.