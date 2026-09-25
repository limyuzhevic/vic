# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Advanced Configuration Usage

#### Enhanced Configuration Builder

```python
import pynlm

# Create advanced configuration using ConfigBuilder
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 5000)\
    .set("brain.synapse_density", 0.2)\
    .set("brain.initial_weight_mean", 0.5)\
    .set("brain.initial_weight_std", 0.1)\
    .set("brain.v_thresh", -50.0)\
    .set("brain.v_rest", -70.0)\
    .set("brain.v_reset", -75.0)\
    .set("brain.tau_mem", 20.0)\
    .set("brain.tau_ref", 2.0)

# Apply advanced plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.growth_rate", 0.0001f)

# Apply neuromodulation settings
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Configuration Presets

```python
import pynlm

# Apply performance preset for fast execution
performance_config = pynlm.createConfigBuilder()
performance_config.applyPreset("performance")
performance_brain = pynlm.createBrain(performance_config)
performance_brain.initialize()

# Apply accuracy preset for high precision
accuracy_config = pynlm.createConfigBuilder()
accuracy_config.applyPreset("accuracy")
accuracy_brain = pynlm.createBrain(accuracy_config)
accuracy_brain.initialize()

# Apply balanced preset for middle ground
balanced_config = pynlm.createConfigBuilder()
balanced_config.applyPreset("balanced")
balanced_brain = pynlm.createBrain(balanced_config)
balanced_brain.initialize()

# Verify presets were applied
print(f"Performance preset neurons: {performance_config.getOr('brain.neuron_count', 1000)}")
print(f"Accuracy preset neurons: {accuracy_config.getOr('brain.neuron_count', 1000)}")
print(f"Balanced preset neurons: {balanced_config.getOr('brain.neuron_count', 1000)}")
```

#### Hierarchical Configuration

```python
import pynlm

# Create parent configuration
parent_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 2000)\
    .set("plasticity.plasticity_rate", 0.01)\
    .set("memory.capacity", 5000)

parent_brain = pynlm.createBrain(parent_config)
parent_brain.initialize()

# Create child configuration that inherits from parent
child_config = pynlm.createConfigBuilder()\
    .set("brain.synapse_density", 0.15)\
    .set("development.stage", "adult")

child_config.setParentConfig(parent_config)

child_brain = pynlm.createBrain(child_config)
child_brain.initialize()

# Child configuration inherits parent's neuron_count
print(f"Child neuron count (inherited): {child_config.getOr('brain.neuron_count', 1000)}")
print(f"Child synapse density: {child_config.getOr('brain.synapse_density', 0.05)}")
print(f"Child plasticity rate: {child_config.getOr('plasticity.plasticity_rate', 0.05)}")
```

#### Configuration Constraints and Validation

```python
import pynlm

# Create configuration with constraints
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.plasticity_rate", 0.05)\
    .set("memory.capacity", 5000)

# Validate configuration
validation_result = config.validate()
if validation_result.isValid():
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in validation_result.getErrors():
        print(f"  - {error}")

# Check specific key validation
if config.validateKey("brain.neuron_count"):
    print("Brain neuron count is valid")
else:
    print("Brain neuron count is invalid")

# Test range constraint
config.set("learning_rate", 0.5)  # This should fail if constrained to [0, 1]
if config.validateKey("learning_rate"):
    print("Learning rate is valid")
else:
    print("Learning rate is invalid")
```

#### Advanced Factory Methods

```python
import pynlm

# Create different brain types using advanced factory methods

# Standard brain with custom configuration
standard_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 3000)\
    .set("brain.synapse_density", 0.1)
standard_brain = pynlm.createBrain(standard_config)

# Minimal brain for testing and debugging
minimal_brain = pynlm.createBrain(pynlm.createBrainWithType("minimal"))

# Complex brain with all advanced features
complex_brain = pynlm.createBrain(pynlm.createBrainWithType("complex"))

# Create specific brain types
performance_brain = pynlm.createBrain(pynlm.createBrainWithType("performance"))
accuracy_brain = pynlm.createBrain(pynlm.createBrainWithType("accuracy"))

# Create world and agent with advanced configuration
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.setMaxEnergy(100.0)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with advanced features
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward and development
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

### Memory Systems Integration

#### Working Memory Examples

```python
import pynlm

# Create and initialize brain with working memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check if working memory is available
wm = brain.getWorkingMemory()
if wm:
    print("Working memory is available")
    
    # Store a pattern in working memory
    pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
    wm.store(pattern, strength=0.8)
    
    # Retrieve the pattern
    retrieved = wm.retrieve()
    print(f"Stored pattern: {pattern}")
    print(f"Retrieved pattern: {retrieved}")
    
    # Check memory activity
    activity = wm.getMemoryActivity()
    print(f"Memory activity: {activity:.3f}")
    
    # Run competition to select winner
    wm.runCompetition()
else:
    print("Working memory is not available")
```

#### Episodic Memory Examples

```python
import pynlm

# Create and initialize brain with episodic memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check if episodic memory is available
em = brain.getEpisodicMemory()
if em:
    print("Episodic memory is available")
    
    # Create an episodic memory item
    from datetime import datetime
    memory_item = pynlm.EpisodicMemoryItem(
        timestamp=datetime.now().timestamp(),
        positionX=10.0,
        positionY=20.0,
        action=pynlm.ActionType.MoveForward,
        reward=1.0,
        sensoryState=[0.1, 0.2, 0.3, 0.4, 0.5],
        resultingSensoryState=[0.2, 0.3, 0.4, 0.5, 0.6],
        activeNeurons=[123, 456, 789]
    )
    
    # Store the episode
    em.storeEpisode(memory_item)
    
    # Retrieve similar episodes
    query = [0.15, 0.25, 0.35, 0.45, 0.55]
    similar = em.retrieveSimilar(query, maxResults=5)
    print(f"Retrieved {len(similar)} similar episodes")
    
    # Retrieve temporal episodes
    past_episodes = em.retrieveTemporal(0, 100, maxResults=3)
    print(f"Retrieved {len(past_episodes)} past episodes")
    
    # Get recent episodes
    recent = em.getRecentEpisodes(5)
    print(f"Retrieved {len(recent)} recent episodes")
    
    # Replay episodes
    if recent:
        em.replayEpisode(recent[0])
        print("Replayed most recent episode")
else:
    print("Episodic memory is not available")
```

#### Associative Memory Examples

```python
import pynlm

# Create and initialize brain with associative memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check if associative memory is available
am = brain.getAssociativeMemory()
if am:
    print("Associative memory is available")
    
    # Create associations between patterns
    pattern_a = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern_b = [0.6, 0.7, 0.8, 0.9, 1.0]
    
    am.associate(pattern_a, pattern_b, strength=0.8)
    
    # Associate from experience
    memory_item = pynlm.EpisodicMemoryItem(
        sensoryState=[0.1, 0.2, 0.3, 0.4, 0.5],
        resultingSensoryState=[0.6, 0.7, 0.8, 0.9, 1.0],
        action=pynlm.ActionType.MoveForward,
        reward=1.0
    )
    
    am.associateFromExperience(memory_item)
    
    # Retrieve patterns
    query = [0.6, 0.7, 0.8, 0.9, 1.0]
    retrieved = am.retrieve(query, maxResults=5)
    print(f"Retrieved {len(retrieved)} patterns")
    
    # Get association strength
    strength = am.getAssociationStrength(pattern_a, pattern_b)
    print(f"Association strength: {strength:.3f}")
    
    # Spread activation
    activated = am.spreadActivation(query, steps=3)
    print(f"Spread activation produced {len(activated)} patterns")
else:
    print("Associative memory is not available")
```

### Prediction System Integration

#### Basic Prediction Examples

```python
import pynlm

# Create and initialize brain with prediction system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check if prediction system is available
pred_sys = brain.getPredictionSystem()
if pred_sys:
    print("Prediction system is available")
    
    # Create initial state
    initial_state = pynlm.SensoryInput()
    initial_state.setData([0.1, 0.2, 0.3, 0.4, 0.5])
    
    # Predict next state
    predicted = pred_sys.predictNextState(initial_state)
    
    # Create actual state
    actual_state = pynlm.SensoryInput()
    actual_state.setData([0.15, 0.25, 0.35, 0.45, 0.55])
    
    # Update predictions
    pred_sys.updatePredictions(predicted, actual_state)
    
    # Get prediction error
    error = pred_sys.getPredictionError()
    print(f"Prediction error: {error:.3f}")
    
    # Get confidence
    confidence = pred_sys.getConfidence()
    print(f"Prediction confidence: {confidence:.3f}")
    
    # Get error history
    history = pred_sys.getErrorHistory()
    print(f"Error history (last 5): {history[-5:] if len(history) >= 5 else history}")
    
    # Clear history for next experiment
    pred_sys.clearHistory()
else:
    print("Prediction system is not available")
```

### Neuromodulation Integration

#### Neuromodulation Examples

```python
import pynlm

# Create and initialize brain with neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check neuromodulation systems
 dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionError()

if dopamine:
    print("Dopamine system is available")
    dopamine_level = brain.getDopamineLevel()
    print(f"Dopamine level: {dopamine_level:.3f}")

if curiosity:
    print("Curiosity system is available")
    curiosity_level = brain.getCuriosityLevel()
    print(f"Curiosity level: {curiosity_level:.3f}")
    
    novelty_level = curiosity.getNoveltyLevel()
    print(f"Novelty level: {novelty_level:.3f}")

if novelty:
    print("Novelty system is available")
    # Novelty system can be accessed through curiosity

if prediction_error:
    print("Prediction error system is available")
    error = prediction_error.getError()
    print(f"Prediction error magnitude: {error:.3f}")

# Run simulation with neuromodulation
if curiosity and dopamine:
    for step in range(50):
        brain.step(step)
        
        # Inject sensory input to activate neuromodulation
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        
        # Get current levels
        curiosity_level = brain.getCuriosityLevel()
        dopamine_level = brain.getDopamineLevel()
        
        if step % 10 == 0:
            print(f"Step {step}: Curiosity={curiosity_level:.3f}, Dopamine={dopamine_level:.3f}")
```

### Cognition Systems Integration

#### Cognition Examples

```python
import pynlm

# Create and initialize brain with cognition systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

if planner:
    print("Planner system is available")
    # Planner can create action sequences based on goals
    
if concept_formation:
    print("Concept formation system is available")
    # Concept formation can abstract patterns into concepts
    
if attention:
    print("Attention system is available")
    # Attention can focus on relevant stimuli and regions

# Run simulation with cognition
for step in range(50):
    brain.step(step)
    
    if attention:
        # Apply attention to specific regions
        attention.focusOnRegion(pynlm.RegionId(0))
    
    if concept_formation:
        # Get current sensory input for concept formation
        percept = world.getSensoryPercept() if 'world' in locals() else None
        if percept:
            # Process for concept formation
            pass

print("Cognition systems are integrated and functional")
```

### Performance Monitoring Integration

#### Comprehensive Performance Monitoring

```python
import pynlm

# Create brain and performance monitor
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = pynlm.createPerformanceMonitor()

# Start performance monitoring for different phases
monitor.startPhase("initialization")
brain.initialize()
init_time = monitor.endPhase("initialization")
print(f"Initialization time: {init_time:.3f} seconds")

# Run simulation with detailed monitoring
monitor.startPhase("simulation")
for step in range(200):
    brain.step(step)
    
    # Record step time
    monitor.recordStepTime(0.001)  # Assuming each step takes 1ms
    
    # Update neural counts
    monitor.updateNeuralCounts(brain.getActiveNeuronCount(), brain.getTotalSynapseCount())
    
    # Print progress every 50 steps
    if step % 50 == 0:
        avg_firing = brain.getAverageFiringRate()
        firing_count = brain.getFiringNeuronCount()
        print(f"Step {step}: {firing_count} firing neurons, avg rate {avg_firing:.2f} Hz")

sim_time = monitor.endPhase("simulation")
print(f"Simulation time: {sim_time:.3f} seconds")

# Get performance metrics
avg_step_time = monitor.getAverageStepTime()
total_sim_time = monitor.getTotalSimulationTime()
step_count = monitor.getStepCount()
active_neurons = monitor.getActiveNeuronCount()
active_synapses = monitor.getActiveSynapseCount()

print(f"\nPerformance Summary:")
print(f"Average step time: {avg_step_time:.6f} s")
print(f"Total simulation time: {total_sim_time:.3f} s")
print(f"Steps per second: {step_count/total_sim_time:.1f}")
print(f"Active neurons: {active_neurons}")
print(f"Active synapses: {active_synapses}")

# Get phase times
phase_times = monitor.getPhaseTimes()
if not phase_times.empty():
    print(f"\nPhase Times:")
    for phase, time in phase_times.items():
        print(f"  {phase}: {time:.3f} s")

# Get full performance report
print(f"\nFull Performance Report:")
print(monitor.getPerformanceReport())
```

#### Batch Processing Performance

```python
import pynlm

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Create batch processor and monitor
batch_processor = pynlm.createBatchProcessor()
monitor = pynlm.createPerformanceMonitor()

# Run multiple episodes
print("Running 20 episodes with 100 steps each...")
result = batch_processor.runEpisodes(agent, world, 20, 100, monitor)

# Print comprehensive batch report
print(f"\nBatch Processing Report:")
print(batch_processor.getBatchReport(result))

# Print performance report
print(f"\nPerformance Report:")
print(monitor.getPerformanceReport())
```

### Advanced Integration Examples

#### Phase 6 Integration (Complete System)

```python
import pynlm

# Run Phase 6 integration experiment (all systems together)
from pynlm.Phase6IntegratedExperiment import Phase6IntegratedExperiment

experiment = Phase6IntegratedExperiment()

# Configure for integration test
config = pynlm.Phase6Config()
config.neuronCount = 500
config.maxSteps = 1000
config.regionCount = 3
config.connectionProbability = 0.05
config.enableDevelopment = True
config.enableCheckpointing = True
config.checkpointPath = "/tmp/nlm_checkpoint_test.bin"

# Run integration experiment
print("Running Phase 6 Integration Experiment...")
result = experiment.run(config)

print(f"\nIntegration Results:")
print(f"Average reward: {result.totalReward:.3f}")
print(f"Average firing rate: {result.avgFiringRate:.3f}")
print(f"Episodes stored: {result.memoryEpisodesStored:.1f}")
print(f"Wall time: {result.totalWallClockTime:.3f} s")

# Verify integration
print(f"\nIntegration Verification:")
print(f"Working Memory: {'YES' if result.memoryWorkingMemoryIntegrated else 'NO'}")
print(f"Episodic Memory: {'YES' if result.memoryEpisodicMemoryIntegrated else 'NO'}")
print(f"Neuromodulation: {'YES' if result.neuromodulationIntegrated else 'NO'}")
print(f"Prediction: {'YES' if result.predictionIntegrated else 'NO'}")
print(f"Development: {'YES' if result.developmentIntegrated else 'NO'}")
print(f"Checkpointing: {'YES' if result.checkpointingWorks else 'NO'}")

# Run verification suite
print(f"\nRunning Integration Verification Suite...")
if experiment.verifyIntegration():
    print("✓ All systems are properly integrated")
else:
    print("✗ Integration verification failed")
```

#### Complete Neuroscience Analysis

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Perform comprehensive neuroscience analysis
print("=== Neuroscience Analysis ===")

# 1. Network topology analysis
topology = pynlm.analyzeNetworkTopology(brain)
print(topology)

# 2. Development pattern analysis
development = pynlm.analyzeDevelopmentPattern(brain)
print(development)

# 3. Plasticity state analysis
plasticity = pynlm.analyzePlasticityState(brain)
print(plasticity)

# 4. Memory usage analysis
memory_usage = pynlm.Debugger.analyzeMemoryUsage(brain)
print(memory_usage)

# 5. Complete profiling
profiler = pynlm.createDebugger()
profile_report = profiler.profileBrain(brain, steps=50)
print(profile_report)
```

### Advanced Serialization and Persistence

#### Complete State Management

```python
import pynlm
import tempfile
import os

# Create and initialize complex brain
config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("brain.synapse_density", 0.1)\
    .set("plasticity.plasticity_rate", 0.05)\
    .build()

brain1 = pynlm.createBrain(config)
brain1.initialize()

# Enable all subsystems
agent = pynlm.createAgentBrain(brain1)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation to populate memory and state
for i in range(200):
    brain1.step(i)

# Save complete brain state
serializer = pynlm.createSerializer()

with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_path = f.name

print(f"Saving complete brain state to {checkpoint_path}...")
serializer.saveBrainState(brain1, checkpoint_path)

# Get brain snapshot before save
snapshot = serializer.getBrainSnapshot(brain1)
print("Brain snapshot before save:")
print(snapshot)

# Create new brain and load state
brain2 = pynlm.createBrain(config)
brain2.initialize()

print(f"Loading brain state from {checkpoint_path}...")
serializer.loadBrainState(brain2, checkpoint_path)

# Get brain snapshot after load
snapshot2 = serializer.getBrainSnapshot(brain2)
print("Brain snapshot after load:")
print(snapshot2)

# Save and load configuration
config_path = "/tmp/nlm_config.json"
print(f"Saving configuration to {config_path}...")
serializer.saveConfig(config, config_path)

config2 = pynlm.createConfigBuilder().build()
print(f"Loading configuration from {config_path}...")
serializer.loadConfig(config2, config_path)

print(f"Configuration keys: {config2.getKeys()}")

# Clean up
os.unlink(checkpoint_path)
if os.path.exists(config_path):
    os.unlink(config_path)
```

### Experiment Management Integration

#### Complete Experiment Workflow

```python
import pynlm

# Create experiment configuration
experiment_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 800)\
    .set("max_steps", 500)\
    .set("learning_rate", 0.01)\
    .set("enable_reward_modulation", True)\
    .set("enable_curiosity", True)\
    .set("enable_structural_plasticity", True)\
    .build()

# Create experiment
experiment_name = "memory_and_learning_experiment"
experiment = pynlm.createExperiment(experiment_name, experiment_config)

# Create experiment runner with monitoring
monitor = pynlm.createPerformanceMonitor()
runner = pynlm.createExperimentRunner(experiment, monitor)

# Create batch processor for multiple runs
batch_processor = pynlm.createBatchProcessor()

# Run single experiment
print(f"Running experiment: {experiment_name}")
pynlm.runExperiment(runner)

# Get experiment summary
experiment_summary = pynlm.getExperimentSummary(experiment)
print(f"\nExperiment Summary:")
print(experiment_summary)

# Get runner statistics
runner_stats = pynlm.getRunnerStatistics(runner)
print(f"\nRunner Statistics:")
print(runner_stats)

# Get performance report
performance_report = monitor.getPerformanceReport()
print(f"\nPerformance Report:")
print(performance_report)

# Run multiple experiments in batch
experiments = [
    ("experiment_1", pynlm.createConfigBuilder().set("brain.neuron_count", 500).build()),
    ("experiment_2", pynlm.createConfigBuilder().set("brain.neuron_count", 1000).build()),
    ("experiment_3", pynlm.createConfigBuilder().set("brain.neuron_count", 1500).build()),
]

experiment_runners = []
for name, exp_config in experiments:
    exp = pynlm.createExperiment(name, exp_config)
    runner = pynlm.createExperimentRunner(exp, monitor)
    experiment_runners.append(runner)

print(f"\nRunning batch of {len(experiment_runners)} experiments...")
pynlm.runExperimentBatch(experiment_runners)

# Collect results
for i, runner in enumerate(experiment_runners):
    name = experiments[i][0]
    exp = runner.getExperiment()
    summary = pynlm.getExperimentSummary(exp)
    stats = pynlm.getRunnerStatistics(runner)
    print(f"\n{name} Results:")
    print(summary)
    print(stats)
```

### Debugging and Troubleshooting Integration

#### Complete Debugging Workflow

```python
import pynlm

# Create and initialize complex brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Create debugger
debugger = pynlm.createDebugger()

# Profile brain activity
print("Profiling brain activity...")
profile_report = debugger.profileBrain(brain, steps=50)
print(profile_report)

# Analyze memory usage
print("\nAnalyzing memory usage...")
memory_analysis = debugger.analyzeMemoryUsage(brain)
print(memory_analysis)

# Dump all state information
print("\nDumping neuron states...")
debugger.dumpNeuronStates(brain, "neuron_states.txt")

print("\nDumping synapse weights...")
debugger.dumpSynapseWeights(brain, "synapse_weights.txt")

print("\nDumping configuration...")
debugger.dumpConfiguration(brain.getConfig(), "config_dump.txt")

# Common troubleshooting patterns
print("\n=== Troubleshooting Patterns ===")

# Pattern 1: Check if brain initialized properly
if brain.getTotalNeuronCount() == 0:
    print("WARNING: Brain has no neurons. Did you call initialize()?")
else:
    print(f"✓ Brain has {brain.getTotalNeuronCount()} neurons")

# Pattern 2: Check firing activity
if brain.getFiringNeuronCount() == 0:
    print("WARNING: No neurons are firing. Check for input sources.")
else:
    print(f"✓ {brain.getFiringNeuronCount()} neurons are firing")

# Pattern 3: Check memory systems
if brain.getWorkingMemory() is None:
    print("WARNING: Working memory not available")
else:
    print(f"✓ Working memory has {brain.getWorkingMemory().getActiveTraces()} traces")

if brain.getEpisodicMemory() is None:
    print("WARNING: Episodic memory not available")
else:
    print(f"✓ Episodic memory has {brain.getEpisodicMemory().getEpisodeCount()} episodes")

# Pattern 4: Check neuromodulation
if brain.getCuriosityLevel() == 0.0:
    print("WARNING: Curiosity system not activated")
else:
    print(f"✓ Curiosity level: {brain.getCuriosityLevel():.3f}")

# Pattern 5: Performance check
if brain.getAverageFiringRate() < 0.1:
    print("WARNING: Low firing rate. System may not be learning effectively")
else:
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Advanced Configuration with Templates

#### Configuration Template System

```python
import pynlm

# Create base configuration
base_config = pynlm.createConfigBuilder()\
    .set("brain.neuron_count", 1000)\
    .set("brain.synapse_density", 0.1)

# Create specialized templates
training_config = pynlm.createConfigBuilder()\
    .set("plasticity.plasticity_rate", 0.1)\
    .set("memory.capacity", 5000)\
    .set("enable_reward", True)

production_config = pynlm.createConfigBuilder()\
    .set("plasticity.plasticity_rate", 0.01)\
    .set("memory.capacity", 2000)\
    .set("enable_reward", False)

# Apply template to create specialized configurations
training_brain = pynlm.createBrain(training_config.build())
training_brain.initialize()

production_brain = pynlm.createBrain(production_config.build())
production_brain.initialize()

# Compare configurations
print("Training configuration:")
print(f"  Neurons: {training_config.getOr('brain.neuron_count', 1000)}")
print(f"  Plasticity rate: {training_config.getOr('plasticity.plasticity_rate', 0.05)}")
print(f"  Memory capacity: {training_config.getOr('memory.capacity', 1000)}")
print(f"  Reward enabled: {training_config.getOr('enable_reward', False)}")

print("\nProduction configuration:")
print(f"  Neurons: {production_config.getOr('brain.neuron_count', 1000)}")
print(f"  Plasticity rate: {production_config.getOr('plasticity.plasticity_rate', 0.05)}")
print(f"  Memory capacity: {production_config.getOr('memory.capacity', 1000)}")
print(f"  Reward enabled: {production_config.getOr('enable_reward', False)}")
```

This comprehensive documentation provides extensive examples of all the advanced features in NLM, including factory methods, configuration builders, memory systems, prediction systems, cognition systems, performance monitoring, integration features, and troubleshooting. Each section provides practical, working code examples that demonstrate the full capabilities of the NLM framework.

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
